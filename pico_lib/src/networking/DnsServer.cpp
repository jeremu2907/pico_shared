#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>

#include "networking/DnsServer.hpp"

#define PORT_DNS_SERVER 53
#define DUMP_DATA 0
#define MAX_DNS_MSG_SIZE 300

using namespace Networking;

int DnsServerCallback::dnsSocketNewDgram(struct udp_pcb **udp, void *cb_data, udp_recv_fn cb_udp_recv) {
    *udp = udp_new();
    if (*udp == NULL) {
        return -ENOMEM;
    }
    udp_recv(*udp, cb_udp_recv, (void *)cb_data);
    return ERR_OK;
}

void DnsServerCallback::dnsSocketFree(struct udp_pcb **udp) {
    if (*udp != NULL) {
        udp_remove(*udp);
        *udp = NULL;
    }
}

int DnsServerCallback::dnsSocketBind(struct udp_pcb **udp, uint32_t ip, uint16_t port) {
    ip_addr_t addr;
    IP4_ADDR(&addr, ip >> 24 & 0xff, ip >> 16 & 0xff, ip >> 8 & 0xff, ip & 0xff);
    err_t err = udp_bind(*udp, &addr, port);
    if (err != ERR_OK) {
        //printf("dns failed to bind to port %u: %d", port, err);
        assert(false);
    }
    return err;
}

#if DUMP_DATA
void DnsServerCallback::dump_bytes(const uint8_t *bptr, uint32_t len) {
    unsigned int i = 0;

    for (i = 0; i < len;) {
        if ((i & 0x0f) == 0) {
            //printf("\n");
        } else if ((i & 0x07) == 0) {
            //printf(" ");
        }
        //printf("%02x ", bptr[i++]);
    }
    //printf("\n");
}
#endif

int DnsServerCallback::dnsSocketSendTo(struct udp_pcb **udp, const void *buf, size_t len, const ip_addr_t *dest, uint16_t port) {
    if (len > 0xffff) {
        len = 0xffff;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (p == NULL) {
        //printf("DNS: Failed to send message out of memory\n");
        return -ENOMEM;
    }

    memcpy(p->payload, buf, len);
    err_t err = udp_sendto(*udp, p, dest, port);

    pbuf_free(p);

    if (err != ERR_OK) {
        //printf("DNS: Failed to send message %d\n", err);
        return err;
    }

#if DUMP_DATA
    dump_bytes(buf, len);
#endif
    return len;
}

void DnsServerCallback::dnsServerProcess(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *src_addr, u16_t src_port) {
    DnsServer *d = (DnsServer *)arg;
    //printf("dnsServerProcess %u\n", p->tot_len);

    uint8_t dns_msg[MAX_DNS_MSG_SIZE];
    DnsHeader *dns_hdr = (DnsHeader*)dns_msg;

    size_t msg_len = pbuf_copy_partial(p, dns_msg, sizeof(dns_msg), 0);
    if (msg_len < sizeof(DnsHeader)) {
        pbuf_free(p);
    }

#if DUMP_DATA
    dump_bytes(dns_msg, msg_len);
#endif

    uint16_t flags = lwip_ntohs(dns_hdr->flags);
    uint16_t question_count = lwip_ntohs(dns_hdr->question_count);

    //printf("len %d\n", msg_len);
    //printf("dns flags 0x%x\n", flags);
    //printf("dns question count 0x%x\n", question_count);

    // flags from rfc1035
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    // |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

    // Check QR indicates a query
    if (((flags >> 15) & 0x1) != 0) {
        //printf("Ignoring non-query\n");
        pbuf_free(p);
    }

    // Check for standard query
    if (((flags >> 11) & 0xf) != 0) {
        //printf("Ignoring non-standard query\n");
        pbuf_free(p);
    }

    // Check question count
    if (question_count < 1) {
        //printf("Invalid question count\n");
        pbuf_free(p);
    }

    // //print the question
    //printf("question: ");
    const uint8_t *question_ptr_start = dns_msg + sizeof(DnsHeader);
    const uint8_t *question_ptr_end = dns_msg + msg_len;
    const uint8_t *question_ptr = question_ptr_start;
    while(question_ptr < question_ptr_end) {
        if (*question_ptr == 0) {
            question_ptr++;
            break;
        } else {
            if (question_ptr > question_ptr_start) {
                //printf(".");
            }
            int label_len = *question_ptr++;
            if (label_len > 63) {
                //printf("Invalid label\n");
                pbuf_free(p);
            }
            //printf("%.*s", label_len, question_ptr);
            question_ptr += label_len;
        }
    }
    //printf("\n");

    // Check question length
    if (question_ptr - question_ptr_start > 255) {
        //printf("Invalid question length\n");
        pbuf_free(p);
    }

    // Skip QNAME and QTYPE
    question_ptr += 4;

    // Generate answer
    uint8_t *answer_ptr = dns_msg + (question_ptr - dns_msg);
    *answer_ptr++ = 0xc0; // pointer
    *answer_ptr++ = question_ptr_start - dns_msg; // pointer to question
    
    *answer_ptr++ = 0;
    *answer_ptr++ = 1; // host address

    *answer_ptr++ = 0;
    *answer_ptr++ = 1; // Internet class

    *answer_ptr++ = 0;
    *answer_ptr++ = 0;
    *answer_ptr++ = 0;
    *answer_ptr++ = 60; // ttl 60s

    *answer_ptr++ = 0;
    *answer_ptr++ = 4; // length
    memcpy(answer_ptr, &d->ip.addr, 4); // use our address
    answer_ptr += 4;

    dns_hdr->flags = lwip_htons(
                0x1 << 15 | // QR = response
                0x1 << 10 | // AA = authoritative
                0x1 << 7);   // RA = authenticated
    dns_hdr->question_count = lwip_htons(1);
    dns_hdr->answer_record_count = lwip_htons(1);
    dns_hdr->authority_record_count = 0;
    dns_hdr->additional_record_count = 0;

    // Send the reply
    //printf("Sending %d byte reply to %s:%d\n", answer_ptr - dns_msg, ipaddr_ntoa(src_addr), src_port);
    dnsSocketSendTo(&d->udp, &dns_msg, answer_ptr - dns_msg, src_addr, src_port);
}

void DnsServer::dnsServerInit(DnsServer *d, ip_addr_t *ip) {
    if (DnsServerCallback::dnsSocketNewDgram(&d->udp, d, DnsServerCallback::dnsServerProcess) != ERR_OK) {
        //printf("dns server failed to start\n");
        return;
    }
    if (DnsServerCallback::dnsSocketBind(&d->udp, 0, PORT_DNS_SERVER) != ERR_OK) {
        //printf("dns server failed to bind\n");
        return;
    }
    ip_addr_copy(d->ip, *ip);
    //printf("dns server listening on port %d\n", PORT_DNS_SERVER);
}

void DnsServer::dnsServerDeinit(DnsServer *d) {
    DnsServerCallback::dnsSocketFree(&d->udp);
}
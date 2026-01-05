#pragma once

#include "lwipopts.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"

namespace Networking
{

    struct DnsServer
    {
        struct udp_pcb *udp;
        ip_addr_t ip;

        static void dns_server_init(DnsServer *d, ip_addr_t *ip);
        static void dns_server_deinit(DnsServer *d);
    };

    struct DnsHeader
    {
        uint16_t id;
        uint16_t flags;
        uint16_t question_count;
        uint16_t answer_record_count;
        uint16_t authority_record_count;
        uint16_t additional_record_count;
    };

    class DnsServerCallback
    {
    public:
        static void dns_socket_free(struct udp_pcb **udp);
        static int dns_socket_bind(struct udp_pcb **udp, uint32_t ip, uint16_t port);
        static int dns_socket_new_dgram(struct udp_pcb **udp, void *cb_data, udp_recv_fn cb_udp_recv);
        static void dns_server_process(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *src_addr, u16_t src_port);

    private:
        static int dns_socket_sendto(struct udp_pcb **udp, const void *buf, size_t len, const ip_addr_t *dest, uint16_t port);
#if DUMP_DATA
        static void dump_bytes(const uint8_t *bptr, uint32_t len)
        {
            unsigned int i = 0;

            for (i = 0; i < len;)
            {
                if ((i & 0x0f) == 0)
                {
                    // printf("\n");
                }
                else if ((i & 0x07) == 0)
                {
                    // printf(" ");
                }
                // printf("%02x ", bptr[i++]);
            }
            // printf("\n");
        }
#endif
    };
}
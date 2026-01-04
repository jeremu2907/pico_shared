#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cyw43_config.h"
#include "lwip/udp.h"

#include "wifi/DhcpServer.hpp"

#include "wifi/AccessPoint.hpp"

using namespace Wifi;

#define DHCPDISCOVER (1)
#define DHCPOFFER (2)
#define DHCPREQUEST (3)
#define DHCPDECLINE (4)
#define DHCPACK (5)
#define DHCPNACK (6)
#define DHCPRELEASE (7)
#define DHCPINFORM (8)

#define DHCP_OPT_PAD (0)
#define DHCP_OPT_SUBNET_MASK (1)
#define DHCP_OPT_ROUTER (3)
#define DHCP_OPT_DNS (6)
#define DHCP_OPT_HOST_NAME (12)
#define DHCP_OPT_REQUESTED_IP (50)
#define DHCP_OPT_IP_LEASE_TIME (51)
#define DHCP_OPT_MSG_TYPE (53)
#define DHCP_OPT_SERVER_ID (54)
#define DHCP_OPT_PARAM_REQUEST_LIST (55)
#define DHCP_OPT_MAX_MSG_SIZE (57)
#define DHCP_OPT_VENDOR_CLASS_ID (60)
#define DHCP_OPT_CLIENT_ID (61)
#define DHCP_OPT_END (255)

#define PORT_DHCP_SERVER (67)
#define PORT_DHCP_CLIENT (68)

#define DEFAULT_LEASE_TIME_S (24 * 60 * 60) // in seconds

#define MAC_LEN (6)
#define MAKE_IP4(a, b, c, d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))

void DhcpServer::dhcp_server_init(DhcpServer *d, ip_addr_t *ip, ip_addr_t *nm)
{
    ip_addr_copy(d->ip, *ip);
    ip_addr_copy(d->nm, *nm);
    memset(d->lease, 0, sizeof(d->lease));
    if (DhcpServerCallback::dhcp_socket_new_dgram(&d->udp,
                                                  d,
                                                  DhcpServerCallback::dhcp_server_process) != 0)
    {
        return;
    }
    DhcpServerCallback::dhcp_socket_bind(&d->udp, PORT_DHCP_SERVER);
}

void DhcpServer::dhcp_server_deinit(DhcpServer *d)
{
    DhcpServerCallback::dhcp_socket_free(&d->udp);
}

int DhcpServerCallback::dhcp_socket_new_dgram(struct udp_pcb **udp, void *cb_data, udp_recv_fn cb_udp_recv)
{
    // family is AF_INET
    // type is SOCK_DGRAM

    *udp = udp_new();
    if (*udp == NULL)
    {
        return -ENOMEM;
    }

    // Register callback
    udp_recv(*udp, cb_udp_recv, (void *)cb_data);

    return 0; // success
}

void DhcpServerCallback::dhcp_socket_free(struct udp_pcb **udp)
{
    if (*udp != NULL)
    {
        udp_remove(*udp);
        *udp = NULL;
    }
}

int DhcpServerCallback::dhcp_socket_bind(struct udp_pcb **udp, uint16_t port)
{
    // TODO convert lwIP errors to errno
    return udp_bind(*udp, IP_ANY_TYPE, port);
}

int DhcpServerCallback::dhcp_socket_sendto(struct udp_pcb **udp, struct netif *nif, const void *buf, size_t len, uint32_t ip, uint16_t port)
{
    if (len > 0xffff)
    {
        len = 0xffff;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (p == NULL)
    {
        return -ENOMEM;
    }

    memcpy(p->payload, buf, len);

    ip_addr_t dest;
    IP4_ADDR(ip_2_ip4(&dest), ip >> 24 & 0xff, ip >> 16 & 0xff, ip >> 8 & 0xff, ip & 0xff);
    err_t err;
    if (nif != NULL)
    {
        err = udp_sendto_if(*udp, p, &dest, port, nif);
    }
    else
    {
        err = udp_sendto(*udp, p, &dest, port);
    }

    pbuf_free(p);

    if (err != ERR_OK)
    {
        return err;
    }

    return len;
}

uint8_t *DhcpServerCallback::opt_find(uint8_t *opt, uint8_t cmd)
{
    for (int i = 0; i < 308 && opt[i] != DHCP_OPT_END;)
    {
        if (opt[i] == cmd)
        {
            return &opt[i];
        }
        i += 2 + opt[i + 1];
    }
    return NULL;
}

void DhcpServerCallback::opt_write_n(uint8_t **opt, uint8_t cmd, size_t n, const void *data)
{
    uint8_t *o = *opt;
    *o++ = cmd;
    *o++ = n;
    memcpy(o, data, n);
    *opt = o + n;
}

void DhcpServerCallback::opt_write_u8(uint8_t **opt, uint8_t cmd, uint8_t val)
{
    uint8_t *o = *opt;
    *o++ = cmd;
    *o++ = 1;
    *o++ = val;
    *opt = o;
}

void DhcpServerCallback::opt_write_u32(uint8_t **opt, uint8_t cmd, uint32_t val)
{
    uint8_t *o = *opt;
    *o++ = cmd;
    *o++ = 4;
    *o++ = val >> 24;
    *o++ = val >> 16;
    *o++ = val >> 8;
    *o++ = val;
    *opt = o;
}

void DhcpServerCallback::dhcp_server_process(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *src_addr, u16_t src_port)
{
    DhcpServer *d = (DhcpServer *)arg;
    (void)upcb;
    (void)src_addr;
    (void)src_port;

    // This is around 548 bytes
    dhcp_msg_t dhcp_msg;

#define DHCP_MIN_SIZE (240 + 3)
    if (p->tot_len < DHCP_MIN_SIZE)
    {
        pbuf_free(p);
        return;
    }

    size_t len = pbuf_copy_partial(p, &dhcp_msg, sizeof(dhcp_msg), 0);
    if (len < DHCP_MIN_SIZE)
    {
        pbuf_free(p);
        return;
    }

    dhcp_msg.op = DHCPOFFER;
    memcpy(&dhcp_msg.yiaddr, &ip4_addr_get_u32(ip_2_ip4(&d->ip)), 4);

    uint8_t *opt = (uint8_t *)&dhcp_msg.options;
    opt += 4; // assume magic cookie: 99, 130, 83, 99

    uint8_t *msgtype = opt_find(opt, DHCP_OPT_MSG_TYPE);
    if (msgtype == NULL)
    {
        // A DHCP package without MSG_TYPE?
        pbuf_free(p);
        return;
    }

    switch (msgtype[2])
    {
    case DHCPDISCOVER:
    {
        int yi = DHCPS_MAX_IP;
        for (int i = 0; i < DHCPS_MAX_IP; ++i)
        {
            if (memcmp(d->lease[i].mac, dhcp_msg.chaddr, MAC_LEN) == 0)
            {
                // MAC match, use this IP address
                yi = i;
                break;
            }
            if (yi == DHCPS_MAX_IP)
            {
                // Look for a free IP address
                if (memcmp(d->lease[i].mac, "\x00\x00\x00\x00\x00\x00", MAC_LEN) == 0)
                {
                    // IP available
                    yi = i;
                }
                uint32_t expiry = d->lease[i].expiry << 16 | 0xffff;
                if ((int32_t)(expiry - cyw43_hal_ticks_ms()) < 0)
                {
                    // IP expired, reuse it
                    memset(d->lease[i].mac, 0, MAC_LEN);
                    yi = i;
                }
            }
        }
        if (yi == DHCPS_MAX_IP)
        {
            // No more IP addresses left
            pbuf_free(p);
            return;
        }
        dhcp_msg.yiaddr[3] = DHCPS_BASE_IP + yi;
        opt_write_u8(&opt, DHCP_OPT_MSG_TYPE, DHCPOFFER);
        break;
    }

    case DHCPREQUEST:
    {
        uint8_t *o = opt_find(opt, DHCP_OPT_REQUESTED_IP);
        if (o == NULL)
        {
            // Should be NACK
            pbuf_free(p);
            return;
        }
        if (memcmp(o + 2, &ip4_addr_get_u32(ip_2_ip4(&d->ip)), 3) != 0)
        {
            // Should be NACK
            pbuf_free(p);
            return;
        }
        uint8_t yi = o[5] - DHCPS_BASE_IP;
        if (yi >= DHCPS_MAX_IP)
        {
            // Should be NACK
            pbuf_free(p);
            return;
        }
        if (memcmp(d->lease[yi].mac, dhcp_msg.chaddr, MAC_LEN) == 0)
        {
            // MAC match, ok to use this IP address
        }
        else if (memcmp(d->lease[yi].mac, "\x00\x00\x00\x00\x00\x00", MAC_LEN) == 0)
        {
            // IP unused, ok to use this IP address
            memcpy(d->lease[yi].mac, dhcp_msg.chaddr, MAC_LEN);
        }
        else
        {
            // IP already in use
            // Should be NACK
            pbuf_free(p);
            return;
        }
        d->lease[yi].expiry = (cyw43_hal_ticks_ms() + DEFAULT_LEASE_TIME_S * 1000) >> 16;
        dhcp_msg.yiaddr[3] = DHCPS_BASE_IP + yi;
        opt_write_u8(&opt, DHCP_OPT_MSG_TYPE, DHCPACK);
        printf("DHCPS: client connected: MAC=%02x:%02x:%02x:%02x:%02x:%02x IP=%u.%u.%u.%u\n",
               dhcp_msg.chaddr[0], dhcp_msg.chaddr[1], dhcp_msg.chaddr[2], dhcp_msg.chaddr[3], dhcp_msg.chaddr[4], dhcp_msg.chaddr[5],
               dhcp_msg.yiaddr[0], dhcp_msg.yiaddr[1], dhcp_msg.yiaddr[2], dhcp_msg.yiaddr[3]);

        AccessPoint::onDhcpClientConnect();

        break;
    }

    default:
    {
        pbuf_free(p);
        return;
    }
    }

    opt_write_n(&opt, DHCP_OPT_SERVER_ID, 4, &ip4_addr_get_u32(ip_2_ip4(&d->ip)));
    opt_write_n(&opt, DHCP_OPT_SUBNET_MASK, 4, &ip4_addr_get_u32(ip_2_ip4(&d->nm)));
    opt_write_n(&opt, DHCP_OPT_ROUTER, 4, &ip4_addr_get_u32(ip_2_ip4(&d->ip))); // aka gateway; can have multiple addresses
    opt_write_n(&opt, DHCP_OPT_DNS, 4, &ip4_addr_get_u32(ip_2_ip4(&d->ip)));    // this server is the dns
    opt_write_u32(&opt, DHCP_OPT_IP_LEASE_TIME, DEFAULT_LEASE_TIME_S);
    *opt++ = DHCP_OPT_END;
    struct netif *nif = ip_current_input_netif();
    dhcp_socket_sendto(&d->udp, nif, &dhcp_msg, opt - (uint8_t *)&dhcp_msg, 0xffffffff, PORT_DHCP_CLIENT);
}

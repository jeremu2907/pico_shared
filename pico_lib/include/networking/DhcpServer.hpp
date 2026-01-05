#pragma once

#include "lwipopts.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"

#define DHCPS_BASE_IP (16)
#define DHCPS_MAX_IP (8)

namespace Networking
{
    struct DhcpServerLease
    {
        uint8_t mac[6];
        uint16_t expiry;
    };

    struct DhcpServer
    {
        ip_addr_t ip;
        ip_addr_t nm;
        DhcpServerLease lease[DHCPS_MAX_IP];
        struct udp_pcb *udp;

        static void dhcp_server_init(DhcpServer *d, ip_addr_t *ip, ip_addr_t *nm);
        static void dhcp_server_deinit(DhcpServer *d);
    };

    class DhcpServerCallback
    {
    public:
        static void dhcp_socket_free(struct udp_pcb **udp);
        static int dhcp_socket_bind(struct udp_pcb **udp, uint16_t port);
        static int dhcp_socket_new_dgram(struct udp_pcb **udp, void *cb_data, udp_recv_fn cb_udp_recv);

        static void dhcp_server_process(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *src_addr, u16_t src_port);

    private:
        struct dhcp_msg_t
        {
            uint8_t op;    // message opcode
            uint8_t htype; // hardware address type
            uint8_t hlen;  // hardware address length
            uint8_t hops;
            uint32_t xid;  // transaction id, chosen by client
            uint16_t secs; // client seconds elapsed
            uint16_t flags;
            uint8_t ciaddr[4];    // client IP address
            uint8_t yiaddr[4];    // your IP address
            uint8_t siaddr[4];    // next server IP address
            uint8_t giaddr[4];    // relay agent IP address
            uint8_t chaddr[16];   // client hardware address
            uint8_t sname[64];    // server host name
            uint8_t file[128];    // boot file name
            uint8_t options[312]; // optional parameters, variable, starts with magic
        };

        static int dhcp_socket_sendto(struct udp_pcb **udp, struct netif *nif, const void *buf, size_t len, uint32_t ip, uint16_t port);

        static uint8_t *opt_find(uint8_t *opt, uint8_t cmd);
        static void opt_write_u8(uint8_t **opt, uint8_t cmd, uint8_t val);
        static void opt_write_u32(uint8_t **opt, uint8_t cmd, uint32_t val);
        static void opt_write_n(uint8_t **opt, uint8_t cmd, size_t n, const void *data);
    };
}

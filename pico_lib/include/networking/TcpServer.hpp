#pragma once

#include <string.h>
#include <cstring>

#include "lwipopts.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

namespace Networking
{
    struct TcpServer
    {
        struct tcp_pcb *server_pcb;
        bool complete;
        ip_addr_t gw;
    };

    struct TcpConnectState
    {
        struct tcp_pcb *pcb;
        int sent_len;
        char headers[128];
        char result[256];
        int header_len;
        int result_len;
        ip_addr_t *gw;
    };

    class TcpServerCallback
    {
    public:
        static void tcp_server_err(void *arg, err_t err);
        static void tcp_server_close(TcpServer *state);
        static bool tcp_server_open(void *arg, const char *ap_name);
        static err_t tcp_server_poll(void *arg, struct tcp_pcb *pcb);
        static err_t tcp_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len);
        static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err);
        static err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
        static int test_server_content(const char *request, const char *params, char *result, size_t max_result_len);
        static err_t tcp_close_client_connection(TcpConnectState *con_state, struct tcp_pcb *client_pcb, err_t close_err);
    };
}
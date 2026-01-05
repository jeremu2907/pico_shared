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
        static void tcpServerErr(void *arg, err_t err);
        static void tcpServerClose(TcpServer *state);
        static bool tcpServerOpen(void *arg, const char *ap_name);
        static err_t tcpServerPoll(void *arg, struct tcp_pcb *pcb);
        static err_t tcpServerSent(void *arg, struct tcp_pcb *pcb, u16_t len);
        static err_t tcpServerAccept(void *arg, struct tcp_pcb *client_pcb, err_t err);
        static err_t tcpServerRecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
        static int testServerContent(const char *request, const char *params, char *result, size_t max_result_len);
        static err_t tcpCloseClientConnection(TcpConnectState *con_state, struct tcp_pcb *client_pcb, err_t close_err);
    };
}
#include "networking/TcpServer.hpp"

#include "wifi/AccessPoint.hpp"

using namespace Networking;

#define TCP_PORT 80
#define POLL_TIME_S 1
#define HTTP_GET "GET"
#define HTTP_RESPONSE_HEADERS "HTTP/1.1 %d OK\nContent-Length: %d\nContent-Type: text/html; charset=utf-8\nConnection: close\n\n"
#define LED_TEST_BODY "<html><body style=\"font-size: xx-large\"><p>Led is %s</p><p><a href=\"?led=%d\" style=\"display:inline-block; padding:8px 16px; background:#4CAF50; color:white; border-radius:4px; margin:auto\">Turn led %s</a></body></html>"
#define LED_PARAM "led=%d"
#define LED_TEST "/ledtest"
#define LED_GPIO 0
#define HTTP_RESPONSE_REDIRECT "HTTP/1.1 302 Redirect\nLocation: http://%s" LED_TEST "\n\n"

void TcpServerCallback::tcpServerErr(void *arg, err_t err)
{
    TcpConnectState *con_state = (TcpConnectState *)arg;
    if (err != ERR_ABRT)
    {
        // printf("tcp_client_err_fn %d\n", err);
        tcpCloseClientConnection(con_state, con_state->pcb, err);
    }
}

void TcpServerCallback::tcpServerClose(TcpServer *state)
{
    if (state->server_pcb)
    {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
}

bool TcpServerCallback::tcpServerOpen(void *arg, const char *ap_name)
{
    TcpServer *state = (TcpServer *)arg;
    // printf("starting server on port %d\n", TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb)
    {
        // printf("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, IP_ANY_TYPE, TCP_PORT);
    if (err)
    {
        // printf("failed to bind to port %d\n", TCP_PORT);
        return false;
    }

    state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!state->server_pcb)
    {
        // printf("failed to listen\n");
        if (pcb)
        {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(state->server_pcb, state);
    tcp_accept(state->server_pcb, tcpServerAccept);

    // printf("Try connecting to '%s' (press 'd' to disable access point)\n", ap_name);
    return true;
}

err_t TcpServerCallback::tcpServerPoll(void *arg, tcp_pcb *pcb)
{
    // TcpConnectState *con_state = (TcpConnectState *)arg;
    // printf("tcp_server_poll_fn\n");
    // return tcpCloseClientConnection(con_state, pcb, ERR_OK); // Just disconnect clent?
    return ERR_OK;
}

err_t TcpServerCallback::tcpServerSent(void *arg, tcp_pcb *pcb, u16_t len)
{
    TcpConnectState *con_state = (TcpConnectState *)arg;
    // printf("tcpServerSent %u\n", len);
    con_state->sent_len += len;
    if (con_state->sent_len >= con_state->header_len + con_state->result_len)
    {
        // printf("all done\n");
        return tcpCloseClientConnection(con_state, pcb, ERR_OK);
    }
    return ERR_OK;
}

err_t TcpServerCallback::tcpServerAccept(void *arg, tcp_pcb *client_pcb, err_t err)
{
    TcpServer *state = (TcpServer *)arg;
    if (err != ERR_OK || client_pcb == NULL)
    {
        // printf("failure in accept\n");
        return ERR_VAL;
    }
    // printf("client connected\n");

    // Create the state for the connection
    TcpConnectState *con_state = (TcpConnectState *)calloc(1, sizeof(TcpConnectState));
    if (!con_state)
    {
        // printf("failed to allocate connect state\n");
        return ERR_MEM;
    }
    con_state->pcb = client_pcb; // for checking
    con_state->gw = &state->gw;

    // setup connection to client
    tcp_arg(client_pcb, con_state);
    tcp_sent(client_pcb, tcpServerSent);
    tcp_recv(client_pcb, tcpServerRecv);
    tcp_poll(client_pcb, tcpServerPoll, POLL_TIME_S * 2);
    tcp_err(client_pcb, tcpServerErr);

    Wifi::AccessPoint::onTcpAccept();

    return ERR_OK;
}

err_t TcpServerCallback::tcpServerRecv(void *arg, tcp_pcb *pcb, pbuf *p, err_t err)
{
    TcpConnectState *con_state = (TcpConnectState *)arg;
    if (!p)
    {
        // printf("connection closed\n");
        return tcpCloseClientConnection(con_state, pcb, ERR_OK);
    }
    assert(con_state && con_state->pcb == pcb);
    if (p->tot_len > 0)
    {
        // printf("tcpServerRecv %d err %d\n", p->tot_len, err);
#if 0
        for (struct pbuf *q = p; q != NULL; q = q->next) {
           //printf("in: %.*s\n", q->len, q->payload);
        }
#endif
        // Copy the request into the buffer
        pbuf_copy_partial(p, con_state->headers, p->tot_len > sizeof(con_state->headers) - 1 ? sizeof(con_state->headers) - 1 : p->tot_len, 0);

        // Handle GET request
        if (strncmp(HTTP_GET, con_state->headers, sizeof(HTTP_GET) - 1) == 0)
        {
            char *request = con_state->headers + sizeof(HTTP_GET); // + space
            char *params = strchr(request, '?');
            if (params)
            {
                if (*params)
                {
                    char *space = strchr(request, ' ');
                    *params++ = 0;
                    if (space)
                    {
                        *space = 0;
                    }
                }
                else
                {
                    params = NULL;
                }
            }

            // Generate content
            con_state->result_len = testServerContent(request, params, con_state->result, sizeof(con_state->result));
            // printf("Request: %s?%s\n", request, params);
            // printf("Result: %d\n", con_state->result_len);

            // Check we had enough buffer space
            if (con_state->result_len > sizeof(con_state->result) - 1)
            {
                printf("Too much result data %d\n", con_state->result_len);
                return tcpCloseClientConnection(con_state, pcb, ERR_CLSD);
            }

            // Generate web page
            if (con_state->result_len > 0)
            {
                con_state->header_len = snprintf(con_state->headers, sizeof(con_state->headers), HTTP_RESPONSE_HEADERS,
                                                 200, con_state->result_len);
                if (con_state->header_len > sizeof(con_state->headers) - 1)
                {
                    // printf("Too much header data %d\n", con_state->header_len);
                    return tcpCloseClientConnection(con_state, pcb, ERR_CLSD);
                }
            }
            else
            {
                // Send redirect
                con_state->header_len = snprintf(con_state->headers, sizeof(con_state->headers), HTTP_RESPONSE_REDIRECT,
                                                 ipaddr_ntoa(con_state->gw));
                // printf("Sending redirect %s", con_state->headers);
            }

            // Send the headers to the client
            con_state->sent_len = 0;
            err_t err = tcp_write(pcb, con_state->headers, con_state->header_len, 0);
            if (err != ERR_OK)
            {
                // printf("failed to write header data %d\n", err);
                return tcpCloseClientConnection(con_state, pcb, err);
            }

            // Send the body to the client
            if (con_state->result_len)
            {
                err = tcp_write(pcb, con_state->result, con_state->result_len, 0);
                if (err != ERR_OK)
                {
                    // printf("failed to write result data %d\n", err);
                    return tcpCloseClientConnection(con_state, pcb, err);
                }
            }
        }
        tcp_recved(pcb, p->tot_len);
    }
    pbuf_free(p);
    return ERR_OK;
}

int TcpServerCallback::testServerContent(const char *request, const char *params, char *result, size_t max_result_len)
{
    int len = 0;
    if (strncmp(request, LED_TEST, sizeof(LED_TEST) - 1) == 0)
    {
        // Get the state of the led
        bool value;
        cyw43_gpio_get(&cyw43_state, LED_GPIO, &value);
        int led_state = value;

        // See if the user changed it
        if (params)
        {
            int led_param = sscanf(params, LED_PARAM, &led_state);
            if (led_param == 1)
            {
                if (led_state)
                {
                    // Turn led on
                    cyw43_gpio_set(&cyw43_state, LED_GPIO, true);
                }
                else
                {
                    // Turn led off
                    cyw43_gpio_set(&cyw43_state, LED_GPIO, false);
                }
            }
        }
        // Generate result
        if (led_state)
        {
            len = snprintf(result, max_result_len, LED_TEST_BODY, "ON", 0, "OFF");
        }
        else
        {
            len = snprintf(result, max_result_len, LED_TEST_BODY, "OFF", 1, "ON");
        }
    }
    return len;
}

err_t TcpServerCallback::tcpCloseClientConnection(TcpConnectState *con_state, tcp_pcb *client_pcb, err_t close_err)
{
    printf("################### Closing connection...\n");
    if (client_pcb)
    {
        assert(con_state && con_state->pcb == client_pcb);
        tcp_arg(client_pcb, NULL);
        tcp_poll(client_pcb, NULL, 0);
        tcp_sent(client_pcb, NULL);
        tcp_recv(client_pcb, NULL);
        tcp_err(client_pcb, NULL);
        err_t err = tcp_close(client_pcb);
        if (err != ERR_OK)
        {
            ////printf("close failed %d, calling abort\n", err);
            tcp_abort(client_pcb);
            close_err = ERR_ABRT;
        }
        if (con_state)
        {
            free(con_state);
        }
    }
    return close_err;
}

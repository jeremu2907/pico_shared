extern "C"
{
#include "wifi/dhcpserver.h"
#include "wifi/dnsserver.h"
#include "wifi/tcpserver.h"
}

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

int main()
{
    stdio_init_all();

    TCP_SERVER_T *state = (TCP_SERVER_T*)calloc(1, sizeof(TCP_SERVER_T));
    if (!state)
    {
        printf("failed to allocate state\n");
        return 1;
    }

    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        return 1;
    }

    // Get notified if the user presses a key
    // stdio_set_chars_available_callback(key_pressed_func, state);

    const char *AP_NAME = "picow_test";
    const char *AP_PASSWORD = "password";

    cyw43_arch_enable_ap_mode(AP_NAME, AP_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);

#if LWIP_IPV6
#define IP(x) ((x).u_addr.ip4)
#else
#define IP(x) (x)
#endif

    ip4_addr_t mask;
    IP(state->gw).addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    IP(mask).addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

#undef IP

    // Start the dhcp server
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &state->gw, &mask);

    // Start the dns server
    dns_server_t dns_server;
    dns_server_init(&dns_server, &state->gw);

    if (!tcp_server_open(state, AP_NAME))
    {
        printf("failed to open server\n");
        return 1;
    }

    state->complete = false;
    while (!state->complete)
    {
        sleep_ms(1);
    }
    tcp_server_close(state);
    dns_server_deinit(&dns_server);
    dhcp_server_deinit(&dhcp_server);
    cyw43_arch_deinit();
    printf("Test complete\n");
    return 0;
}
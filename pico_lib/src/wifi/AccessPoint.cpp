#include "wifi/AccessPoint.hpp"
#include "wifi/TcpServer.hpp"
#include "wifi/DhcpServer.hpp"
#include "Macros.hpp"

extern "C"
{
// #include "wifi/tcpserver.h"
#include "wifi/dnsserver.h"
// #include "wifi/dhcpserver.h"
}

using namespace Wifi;

AccessPoint::AccessPoint(std::string ssid, std::string password)
{
    state = (TcpServer *)calloc(1, sizeof(TcpServer));
    if (!state)
    {
        ERR_START
        printf("failed to allocate state\n");
        ERR_END
    }

    cyw43_arch_enable_ap_mode(ssid.c_str(), password.c_str(), CYW43_AUTH_WPA2_AES_PSK);

#if LWIP_IPV6
#define IP(x) ((x).u_addr.ip4)
#else
#define IP(x) (x)
#endif

    ip4_addr_t mask;
    IP(state->gw).addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    IP(mask).addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

#undef IP

    dhcp_server = (DhcpServer *)calloc(1, sizeof(DhcpServer));
    dns_server = (dns_server_t *)calloc(1, sizeof(dns_server_t));

    DhcpServer::dhcp_server_init(dhcp_server, &state->gw, &mask);
    dns_server_init(dns_server, &state->gw);

    if (!TcpServerCallback::tcp_server_open(state, ssid.c_str()))
    {
        ERR_START
        printf("failed to open server\n");
        ERR_END
    }

    state->complete = false;
}

AccessPoint::~AccessPoint()
{
    TcpServerCallback::tcp_server_close(state);
    DhcpServer::dhcp_server_deinit(dhcp_server);
    dns_server_deinit(dns_server);

    free(state);
    free(dns_server);
    free(dhcp_server);
}

void AccessPoint::runLoop()
{
    if (!state->complete)
    {
        sleep_ms(1);
    }
}

void AccessPoint::onTcpAccept()
{
    m_sCallbackOnTcpAccept();
}

void AccessPoint::onDhcpClientConnect()
{
    m_sCallbackOnDhcpConnect();
}

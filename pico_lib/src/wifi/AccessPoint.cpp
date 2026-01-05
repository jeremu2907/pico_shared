#include "wifi/AccessPoint.hpp"
#include "networking/TcpServer.hpp"
#include "networking/DnsServer.hpp"
#include "networking/DhcpServer.hpp"
#include "Macros.hpp"

using namespace Wifi;

AccessPoint::AccessPoint(std::string ssid, std::string password)
{
    state = (Networking::TcpServer *)calloc(1, sizeof(Networking::TcpServer));
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

    dns_server = (Networking::DnsServer *)calloc(1, sizeof(Networking::DnsServer));
    dhcp_server = (Networking::DhcpServer *)calloc(1, sizeof(Networking::DhcpServer));

    Networking::DnsServer::dns_server_init(dns_server, &state->gw);
    Networking::DhcpServer::dhcp_server_init(dhcp_server, &state->gw, &mask);

    if (!Networking::TcpServerCallback::tcp_server_open(state, ssid.c_str()))
    {
        ERR_START
        printf("failed to open server\n");
        ERR_END
    }

    state->complete = false;
}

AccessPoint::~AccessPoint()
{
    Networking::TcpServerCallback::tcp_server_close(state);
    Networking::DhcpServer::dhcp_server_deinit(dhcp_server);
    Networking::DnsServer::dns_server_deinit(dns_server);

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

void AccessPoint::setDhcpLeaseTimeS(uint seconds)
{
    m_sDhcpLeaseTimeS = seconds;
}

uint AccessPoint::getDhcpLeaseTimeS()
{
    return m_sDhcpLeaseTimeS;
}
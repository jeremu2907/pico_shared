#pragma once

#include <string>
#include <functional>

#include <pico/stdlib.h>

namespace Networking
{
    struct TcpServer;
    struct DnsServer;
    struct DhcpServer;

    class TcpServerCallback;
    class DnsServerCallback;
    class DhcpServerCallback;
}

namespace Wifi
{
    class AccessPoint
    {
        friend class Networking::TcpServerCallback;
        friend class Networking::DhcpServerCallback;

    public:
        explicit AccessPoint(std::string ssid, std::string password);
        ~AccessPoint();

        void runLoop();
        
        void setDhcpLeaseTimeS(uint seconds);

        template <typename Func>
        static void installCallbackOnTcpAccept(Func f)
        {
            m_sCallbackOnTcpAccept = f;
        }

        template <typename Func>
        static void installCallbackOnDhcpConnect(Func f)
        {
            m_sCallbackOnDhcpConnect = f;
        }

    private:
        static uint getDhcpLeaseTimeS();

        static void onTcpAccept();
        static void onDhcpClientConnect();

        std::string ssid;
        std::string password;

        Networking::TcpServer *state = nullptr;
        Networking::DnsServer *dns_server = nullptr;
        Networking::DhcpServer *dhcp_server = nullptr;

        inline static uint m_sDhcpLeaseTimeS = 24 * 60 * 60;
        inline static std::function<void()> m_sCallbackOnTcpAccept = []() {};
        inline static std::function<void()> m_sCallbackOnDhcpConnect = []() {};
    };
}
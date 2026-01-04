#pragma once

#include <string>
#include <functional>

typedef struct dns_server_t_ dns_server_t;

namespace Wifi
{
    struct TcpServer;
    struct DhcpServer;

    class TcpServerCallback;
    class DhcpServerCallback;

    class AccessPoint
    {
        friend class TcpServerCallback;
        friend class DhcpServerCallback;

    public:
        explicit AccessPoint(std::string ssid, std::string password);
        ~AccessPoint();

        void runLoop();

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
        static void onTcpAccept();

        static void onDhcpClientConnect();

        std::string ssid;
        std::string password;

        TcpServer *state = nullptr;
        DhcpServer *dhcp_server = nullptr;
        dns_server_t *dns_server = nullptr;

        inline static std::function<void()> m_sCallbackOnTcpAccept = []() {};
        inline static std::function<void()> m_sCallbackOnDhcpConnect = []() {};
    };
}
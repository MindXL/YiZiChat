#pragma once

#include <YiZi/YiZi.h>

namespace YiZi::Server
{
    class Application
    {
    public:
        Application(ip_t ip, port_t port);
        ~Application();

        void Run() const;

    private:
        const ip_t m_Ip;
        const port_t m_Port;
        const socket_t m_Listenfd;
    };
}

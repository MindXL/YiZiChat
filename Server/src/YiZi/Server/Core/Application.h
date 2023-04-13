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
    };
}

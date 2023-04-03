#include "Application.h"

#include <thread>
#include <iostream>
#include <memory>

#include "ConnectionHandler.h"
#include "SSocket.h"

namespace YiZi::Server
{
    Application::Application(const ip_t ip, const port_t port)
    {
        SListenSocket::Get()->Initialize();
        SListenSocket::Get()->Listen(ip, port);
    }

    Application::~Application()
    {
        SListenSocket::Get()->Close();
    }

    void Application::Run() const
    {
        std::cout << "YiZiChat server-end is running..." << std::endl;

        while (true)
        {
            auto client = std::make_shared<SAcceptSocket>();
            client->Accept();

            std::thread(ConnectionHandler::Handle, client).detach();
            client = nullptr;

            std::this_thread::yield();
        }
    }
}

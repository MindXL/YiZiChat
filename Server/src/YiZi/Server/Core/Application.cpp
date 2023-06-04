#include "Application.h"

#include <thread>
#include <iostream>

#include "ConnectionHandler.h"
#include "SSocket.h"
#include "Environment.h"
#include "MySQLConnector.h"

namespace YiZi::Server
{
    Application::Application(const ip_t ip, const port_t port)
    {
        try
        {
            Environment::Get()->CheckOuterEnvironment();
            MySQLConnector::Get()->Connect();
            Environment::Get()->CheckInnerEnvironment();
        }
        catch (const std::runtime_error&)
        {
            throw;
        }

        SListenSocket::Get()->Initialize();
        SListenSocket::Get()->Listen(ip, port);
    }

    Application::~Application()
    {
        SListenSocket::Get()->Close();
        MySQLConnector::Get()->Disconnect();
    }

    void Application::Run() const
    {
        std::cout << "YiZiChat server-end is running..." << std::endl;

        while (true)
        {
            auto* const client = new SAcceptSocket{};
            try
            {
                client->Accept();
            }
            catch (const std::runtime_error& error)
            {
                std::cerr << error.what() << std::endl;
                continue;
            }

            std::thread(ConnectionHandler::Handle, client).detach();

            std::this_thread::yield();
        }
    }
}

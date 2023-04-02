#include "Application.h"

#include <arpa/inet.h>
#include <thread>
#include <iostream>

#include "ConnectionHandler.h"

namespace YiZi::Server
{
    Application::Application(const ip_t ip, const port_t port)
        : m_Ip{ip}, m_Port{port},
          m_Listenfd{socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)}
    {
        sockaddr_in server_addr{};
        server_addr.sin_family = PF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip);
        server_addr.sin_port = htons(port);

        constexpr int option = 1;
        if (setsockopt(m_Listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof option) < 0
            || bind(m_Listenfd, (sockaddr*)&server_addr, sizeof server_addr) < 0
            || listen(m_Listenfd, 100))
        {
            throw std::runtime_error{"Fatal: Socket initialization failed."};
        }
    }

    Application::~Application()
    {
        shutdown(m_Listenfd, SHUT_RDWR);
    }

    void Application::Run() const
    {
        std::cout << "YiZiChat server-end is running..." << std::endl;

        socket_t connfd = 0;
        sockaddr_in client_addr{};

        while (true)
        {
            socklen_t cli_len = sizeof client_addr;

            connfd = accept(m_Listenfd, (sockaddr*)&client_addr, &cli_len);

            std::thread(ConnectionHandler::Handle, connfd).detach();

            std::this_thread::yield();
        }
    }
}

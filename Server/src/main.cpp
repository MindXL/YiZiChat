#include <arpa/inet.h>
#include <iostream>
#include <thread>

#include <YiZi/YiZi.h>

#include "ConnectionHandler.h"

int main()
{
    std::cout << "YiZiChat server-end is running..." << std::endl;

    constexpr YiZi::ip_t ip = "127.0.0.1";
    constexpr YiZi::port_t port = 5000;

    const YiZi::socket_t listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in server_addr{};
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    constexpr int option = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof option) < 0)
        return EXIT_FAILURE;
    if (bind(listenfd, (sockaddr*)&server_addr, sizeof server_addr) < 0)
        return EXIT_FAILURE;
    if (listen(listenfd, 100))
        return EXIT_FAILURE;

    YiZi::socket_t connfd = 0;
    sockaddr_in client_addr{};

    while (true)
    {
        socklen_t cli_len = sizeof client_addr;

        connfd = accept(listenfd, (sockaddr*)&client_addr, &cli_len);

        std::thread(YiZi::ConnectionHandler::Handle, connfd).detach();

        std::this_thread::yield();
    }
}

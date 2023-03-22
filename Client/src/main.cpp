#include "pch.h"

int main()
{
    std::cout << "YiZiChat client-end is running..." << std::endl;

    std::this_thread::sleep_for(5000ms);

    constexpr YiZi::ip_t ip = "127.0.0.1";
    constexpr YiZi::port_t port = 5000;

    YiZi::Socket::Get()->Connect(ip, port);

    //std::thread(YiZi::ConnectionHandler::Handle).detach();

    YiZi::ConnectionHandler::HandleTestRequest();
    YiZi::ConnectionHandler::Handle();

    while (true);
}

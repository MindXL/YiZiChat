#include <iostream>

#include <YiZi/Server/Core/Application.h>

int main()
{
    constexpr YiZi::ip_t ip = "127.0.0.1";
    constexpr YiZi::port_t port = 5000;

    try
    {
        auto* const app = new YiZi::Server::Application{ip, port};
        app->Run();
    }
    catch (std::runtime_error& error)
    {
        std::cerr << error.what() << std::endl;
    }
}

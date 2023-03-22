#pragma once

#include <cstdint>

#include <YiZi/YiZi.h>

namespace YiZi
{
    class ConnectionHandler
    {
    public:
        static void Handle(socket_t connfd);

    private:
        static void Dispatch(socket_t connfd, uint8_t* reqBuffer, uint8_t* resBuffer);

        static void HandleTestRequest(socket_t connfd, uint8_t* reqBuffer, uint8_t* resBuffer);
    };
}

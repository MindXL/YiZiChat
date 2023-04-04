#pragma once

#include <cstdint>
#include <memory>

#include "SSocket.h"

namespace YiZi
{
    class ConnectionHandler
    {
    public:
        static void Handle(const std::shared_ptr<Server::SAcceptSocket>& client);

    private:
        static void Dispatch(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer);

        static void HandleLoginRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer);
        static void HandleChatMessageRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer);
        static void HandleTestRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer);
    };
}

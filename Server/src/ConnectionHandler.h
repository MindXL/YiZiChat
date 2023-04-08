#pragma once

#include <cstdint>
#include <memory>

#include "SSocket.h"

namespace YiZi::Server
{
    class ConnectionHandler
    {
    public:
        static void Handle(const std::shared_ptr<Server::SAcceptSocket>& client);

    private:
        static bool Dispatch(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer);

        static bool HandleLoginRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer);
        static bool HandleLogoutRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer);
        static bool HandleChatMessageRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer);

        static const std::string s_DatabaseUserJoinTimeExpr; // Used in HandleLoginRequest()
    };
}

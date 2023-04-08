#pragma once

#include <cstdint>
#include <memory>

#include "SSocket.h"

namespace YiZi::Server
{
    class ConnectionHandler
    {
    public:
        ~ConnectionHandler();

        static void Handle(SAcceptSocket* const client) { ConnectionHandler{client}.Run(); }

    private:
        explicit ConnectionHandler(SAcceptSocket* client);

        void Run() const;
        [[nodiscard]] bool Dispatch() const;

        [[nodiscard]] bool HandleLoginRequest() const;
        [[nodiscard]] bool HandleLogoutRequest() const;
        [[nodiscard]] bool HandleChatMessageRequest() const;

    private:
        SAcceptSocket* const m_Client;
        uint8_t* const m_ReqBuffer;
        uint8_t* const m_ResBuffer;

        static const std::string s_DatabaseUserJoinTimeExpr; // Used in HandleLoginRequest()
    };
}

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

        void Run();
        [[nodiscard]] bool Dispatch();

        [[nodiscard]] bool HandleLoginRequest();
        [[nodiscard]] bool HandleLogoutRequest() const;
        [[nodiscard]] bool HandleChatMessageRequest() const;

        [[nodiscard]] bool ValidateUserLogin();

    private:
        SAcceptSocket* const m_Client;
        uint32_t m_UserId;
        uint8_t* const m_ReqBuffer;
        uint8_t* const m_ResBuffer;

        static const std::string s_DatabaseUserJoinTimeExpr; // Used in HandleLoginRequest()
    };
}

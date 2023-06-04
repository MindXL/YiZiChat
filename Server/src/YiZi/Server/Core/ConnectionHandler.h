#pragma once

#include <cstdint>
#include <string>
#include <atomic>

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

        [[nodiscard]] bool HandleTestConnectionRequest() const;
        [[nodiscard]] bool HandleLoginRequest();
        [[nodiscard]] bool HandleLogoutRequest() const;
        [[nodiscard]] bool HandleChannelListRequest() const;
        [[nodiscard]] bool HandleChannelConnectionRequest();
        [[nodiscard]] bool HandleChatMessageRequest() const;
        [[nodiscard]] bool HandleChangeUserPasswordRequest() const;
        [[nodiscard]] bool HandleChangeUserNicknameRequest() const;
        [[nodiscard]] bool HandleValidateAdminRequest();
        [[nodiscard]] bool HandleRegisterUserRequest();

        [[nodiscard]] bool ValidateUserLogin();
        [[nodiscard]] bool ValidateChannel();
        [[nodiscard]] bool ValidateRegisterUser();

    private:
        SAcceptSocket* const m_Client;
        uint32_t m_UserId;
        bool m_IsAdmin = false;
        uint32_t m_ChannelId;
        uint8_t* const m_ReqBuffer;
        uint8_t* const m_ResBuffer;

        std::atomic<bool> m_AdminToken;

        static const std::string s_SQLCountUserId;
        static const std::string s_SQLCountChannelId;
    };
}

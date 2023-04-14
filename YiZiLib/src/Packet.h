#pragma once

#include <algorithm>

#include "Database.h"

namespace YiZi::Packet
{
    enum class PacketType
    {
        LoginRequest, LoginResponse,
        LogoutRequest,
        ChatMessageRequest, ChatMessageResponse,
    };

    struct PacketHeader
    {
        uint8_t type;
    };

    static constexpr int PACKET_HEADER_LENGTH = sizeof(PacketHeader);

    struct LoginRequest
    {
        uint8_t phone[Database::User::ItemLength::PHONE_LENGTH + 1];
        uint8_t password[Database::User::ItemLength::PASSWORD_MAX_LENGTH + 1];
    };

    static constexpr int LOGIN_REQUEST_LENGTH = sizeof(LoginRequest);

    enum class LoginFailReason : uint8_t
    {
        UserNotExist,
        UserPasswordIncorrect,
        UserAlreadyLoggedIn
    };

    struct LoginResponse
    {
        uint8_t isValid; // Whether the user exists in database;
        uint8_t reason; // Only makes sense if "isValid == false".

        uint32_t id;
        uint8_t nickname[(Database::User::ItemLength::NICKNAME_MAX_LENGTH + 1) * sizeof(char16_t)];
        uint32_t join_time;
        uint8_t isAdmin;
    };

    static constexpr int LOGIN_RESPONSE_LENGTH = sizeof(LoginResponse);

    struct LogoutRequest {};

    static constexpr int LOGOUT_REQUEST_LENGTH = 0;

    struct ChatMessageRequest
    {
        uint32_t id;
        uint8_t content[(Database::Transcript::ItemLength::CONTENT_MAX_LENGTH + 1) * sizeof(char16_t)];
    };

    static constexpr int CHAT_MESSAGE_REQUEST_LENGTH = sizeof(ChatMessageRequest);

    struct ChatMessageResponse
    {
        uint8_t nickname[(Database::User::ItemLength::NICKNAME_MAX_LENGTH + 1) * sizeof(char16_t)];
        int64_t timestamp; // milliseconds
        uint8_t content[(Database::Transcript::ItemLength::CONTENT_MAX_LENGTH + 1) * sizeof(char16_t)];
    };

    static constexpr int CHAT_MESSAGE_RESPONSE_LENGTH = sizeof(ChatMessageResponse);

    // macro "max" is defined in minwindef.h
#ifdef YZ_CLIENT
#ifdef max
#undef max
#endif
#endif
    static constexpr int REQUEST_MAX_LENGTH()
    {
        return PACKET_HEADER_LENGTH + std::max({
                   LOGIN_REQUEST_LENGTH,
                   LOGOUT_REQUEST_LENGTH,
                   CHAT_MESSAGE_REQUEST_LENGTH,
                   0
               });
    }

    static constexpr int RESPONSE_MAX_LENGTH()
    {
        return PACKET_HEADER_LENGTH + std::max({
                   LOGIN_RESPONSE_LENGTH,
                   CHAT_MESSAGE_RESPONSE_LENGTH,
                   0
               });
    }
#ifdef YZ_CLIENT
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
}

#pragma once

#include <algorithm>

#include "Database.h"

namespace YiZi::Packet
{
    enum class PacketType
    {
        TestConnectionRequest, TestConnectionResponse,

        LoginRequest, LoginResponse,
        LogoutRequest,

        ChannelListRequest, ChannelListResponse,
        ChannelDetailResponse,
        ChannelConnectionRequest, ChannelConnectionResponse,

        ChatMessageRequest, ChatMessageResponse,

        ChangeUserPasswordRequest,
        ChangeUserPasswordResponse,
        ChangeUserNicknameRequest,
        ChangeUserNicknameResponse,

        ValidateAdminRequest, ValidateAdminResponse,
        RegisterUserRequest, RegisterUserResponse
    };

    struct PacketHeader
    {
        uint8_t type;
    };

    static constexpr int PACKET_HEADER_LENGTH = sizeof(PacketHeader);

    struct TestConnectionRequest {};

    static constexpr int TEST_CONNECTION_REQUEST_LENGTH = 0;

    struct TestConnectionResponse {};

    static constexpr int TEST_CONNECTION_RESPONSE_LENGTH = 0;

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
        uint64_t join_time;
        uint8_t isAdmin;
    };

    static constexpr int LOGIN_RESPONSE_LENGTH = sizeof(LoginResponse);

    struct LogoutRequest {};

    static constexpr int LOGOUT_REQUEST_LENGTH = 0;

    struct ChannelListRequest {};

    static constexpr int CHANNEL_LIST_REQUEST_LENGTH = 0;

    struct ChannelListResponse
    {
        uint32_t count;
    };

    static constexpr int CHANNEL_LIST_RESPONSE_LENGTH = sizeof(ChannelListResponse);

    struct ChannelDetailResponse
    {
        uint32_t id;
        uint8_t name[(Database::Channel::ItemLength::NAME_MAX_LENGTH + 1) * sizeof(char16_t)];
        uint64_t join_time;
        uint8_t description[(Database::Channel::ItemLength::DESCRIPTION_MAX_LENGTH + 1) * sizeof(char16_t)];
    };

    static constexpr int CHANNEL_DETAIL_RESPONSE_LENGTH = sizeof(ChannelDetailResponse);

    struct ChannelConnectionRequest
    {
        uint32_t cid; // channel id
    };

    static constexpr int CHANNEL_CONNECTION_REQUEST_LENGTH = sizeof(ChannelConnectionRequest);

    enum class ChannelConnectionFailReason : uint8_t
    {
        ChannelNotExist
    };

    struct ChannelConnectionResponse
    {
        uint8_t isValid;
        uint8_t reason; // Only makes sense if "isValid == false".
    };

    static constexpr int CHANNEL_CONNECTION_RESPONSE_LENGTH = sizeof(ChannelConnectionResponse);

    struct ChatMessageRequest
    {
        uint8_t content[(Database::Transcript::ItemLength::CONTENT_MAX_LENGTH + 1) * sizeof(char16_t)];
    };

    static constexpr int CHAT_MESSAGE_REQUEST_LENGTH = sizeof(ChatMessageRequest);

    struct ChatMessageResponse
    {
        uint8_t nickname[(Database::User::ItemLength::NICKNAME_MAX_LENGTH + 1) * sizeof(char16_t)];
        uint64_t timestamp; // milliseconds
        uint8_t content[(Database::Transcript::ItemLength::CONTENT_MAX_LENGTH + 1) * sizeof(char16_t)];
    };

    static constexpr int CHAT_MESSAGE_RESPONSE_LENGTH = sizeof(ChatMessageResponse);

    struct ChangeUserPasswordRequest
    {
        uint8_t password[Database::User::ItemLength::PASSWORD_MAX_LENGTH + 1];
    };

    static constexpr int CHANGE_USER_PASSWORD_REQUEST_LENGTH = sizeof(ChangeUserPasswordRequest);

    enum class ChangeUserPasswordFailReason : uint8_t
    {
        PasswordIsIdentical
    };

    struct ChangeUserPasswordResponse
    {
        uint8_t success;
        uint8_t reason; // Only makes sense if "success == false".
    };

    static constexpr int CHANGE_USER_PASSWORD_RESPONSE_LENGTH = sizeof(ChangeUserPasswordResponse);

    struct ChangeUserNicknameRequest
    {
        uint8_t nickname[(Database::User::ItemLength::NICKNAME_MAX_LENGTH + 1) * sizeof(char16_t)];
    };

    static constexpr int CHANGE_USER_NICKNAME_REQUEST_LENGTH = sizeof(ChangeUserNicknameRequest);

    enum class ChangeUserNicknameFailReason : uint8_t
    {
        NicknameAlreadyExists
    };

    struct ChangeUserNicknameResponse
    {
        uint8_t success;
        uint8_t reason;
    };

    static constexpr int CHANGE_USER_NICKNAME_RESPONSE_LENGTH = sizeof(ChangeUserNicknameResponse);

    struct ValidateAdminRequest
    {
        uint8_t password[Database::User::ItemLength::PASSWORD_MAX_LENGTH + 1];
    };

    static constexpr int VALIDATE_ADMIN_REQUEST_LENGTH = sizeof(ValidateAdminRequest);

    enum class ValidateAdminFailReason : uint8_t
    {
        UserPasswordIncorrect,
    };

    struct ValidateAdminResponse
    {
        uint8_t success;
        uint8_t reason;
    };

    static constexpr int VALIDATE_ADMIN_RESPONSE_LENGTH = sizeof(ValidateAdminResponse);

    struct RegisterUserRequest
    {
        uint8_t phone[Database::User::ItemLength::PHONE_LENGTH + 1];
    };

    static constexpr int REGISTER_USER_REQUEST_LENGTH = sizeof(RegisterUserRequest);

    enum class RegisterUserFailReason : uint8_t
    {
        AdminTokenExpired,
        UserAlreadyExists,
    };

    struct RegisterUserResponse
    {
        uint8_t success;
        uint8_t reason;
        uint8_t password[Database::User::ItemLength::PASSWORD_MAX_LENGTH + 1];
    };

    static constexpr int REGISTER_USER_RESPONSE_LENGTH = sizeof(RegisterUserResponse);

    // Macro "max" is defined in "minwindef.h" on client-end.
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
                   CHANNEL_LIST_REQUEST_LENGTH,
                   CHANNEL_CONNECTION_REQUEST_LENGTH,
                   CHAT_MESSAGE_REQUEST_LENGTH,
                   CHANGE_USER_PASSWORD_REQUEST_LENGTH,
                   CHANGE_USER_NICKNAME_REQUEST_LENGTH,
                   VALIDATE_ADMIN_REQUEST_LENGTH,
                   REGISTER_USER_REQUEST_LENGTH,
                   0
               });
    }

    static constexpr int RESPONSE_MAX_LENGTH()
    {
        return PACKET_HEADER_LENGTH + std::max({
                   LOGIN_RESPONSE_LENGTH,
                   CHANNEL_LIST_RESPONSE_LENGTH,
                   CHANNEL_DETAIL_RESPONSE_LENGTH,
                   CHANNEL_CONNECTION_RESPONSE_LENGTH,
                   CHAT_MESSAGE_RESPONSE_LENGTH,
                   CHANGE_USER_PASSWORD_RESPONSE_LENGTH,
                   CHANGE_USER_NICKNAME_RESPONSE_LENGTH,
                   VALIDATE_ADMIN_RESPONSE_LENGTH,
                   REGISTER_USER_RESPONSE_LENGTH,
                   0
               });
    }
#ifdef YZ_CLIENT
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
}

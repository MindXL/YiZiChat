#include "ConnectionHandler.h"

#include <arpa/inet.h>
#include <string_view>
#include <thread>

#include "BufferManager.h"
#include "MySQLConnector.h"
#include "LoginMap.h"
#include "Channel.h"

namespace YiZi::Server
{
    const std::string ConnectionHandler::s_SQLCountUserId{std::move(std::string{"COUNT("}.append(Database::User::Item::id).append(")"))};

    const std::string ConnectionHandler::s_SQLCountChannelId{std::move(std::string{"COUNT("}.append(Database::Channel::Item::id).append(")"))};

    ConnectionHandler::ConnectionHandler(SAcceptSocket* const client)
        : m_Client{client}, m_UserId{0}, m_ChannelId{0}, m_ReqBuffer{BufferManager::Get()->Fetch().first},
          m_ResBuffer{m_ReqBuffer + Packet::REQUEST_MAX_LENGTH()} { }

    ConnectionHandler::~ConnectionHandler()
    {
        m_Client->Close();
        BufferManager::Get()->Return(m_ReqBuffer);
        delete m_Client;
    }

    void ConnectionHandler::Run()
    {
        while (true)
        {
            if (!m_Client->Receive(m_ReqBuffer, Packet::REQUEST_MAX_LENGTH()))
                break;

            // TODO: Use multi-thread to wrap Dispatch().
            if (!Dispatch())
                break;
        }
        LoginMap::Get()->erase(m_UserId);
    }

    bool ConnectionHandler::Dispatch()
    {
        switch (const auto request_header = reinterpret_cast<Packet::PacketHeader*>(m_ReqBuffer);
            Packet::PacketType{request_header->type})
        {
        case Packet::PacketType::TestConnectionRequest: return HandleTestConnectionRequest();
        case Packet::PacketType::LoginRequest: return HandleLoginRequest();
        case Packet::PacketType::LogoutRequest: return HandleLogoutRequest();
        case Packet::PacketType::ChannelListRequest: return HandleChannelListRequest();
        case Packet::PacketType::ChannelConnectionRequest: return HandleChannelConnectionRequest();
        case Packet::PacketType::ChatMessageRequest: return HandleChatMessageRequest();
        case Packet::PacketType::ChangeUserPasswordRequest: return HandleChangeUserPasswordRequest();
        case Packet::PacketType::ChangeUserNicknameRequest: return HandleChangeUserNicknameRequest();
        // TODO: Debug break if m_IsAdmin is not true.
        case Packet::PacketType::ValidateAdminRequest: return HandleValidateAdminRequest();
        case Packet::PacketType::RegisterUserRequest: return HandleRegisterUserRequest();
        default: return false;
        }
    }

    bool ConnectionHandler::HandleTestConnectionRequest() const
    {
        auto* const response_header = reinterpret_cast<Packet::PacketHeader*>(m_ResBuffer);
        response_header->type = (uint8_t)Packet::PacketType::TestConnectionResponse;

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::TEST_CONNECTION_RESPONSE_LENGTH;
        m_Client->Send(m_ResBuffer, response_len);

        // Close socket.
        return false;
    }

    bool ConnectionHandler::HandleLoginRequest()
    {
        const bool isValid = ValidateUserLogin();

        auto* const response_header = reinterpret_cast<Packet::PacketHeader*>(m_ResBuffer);
        response_header->type = (uint8_t)Packet::PacketType::LoginResponse;

        auto* const response_data = reinterpret_cast<Packet::LoginResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);
        response_data->isValid = static_cast<uint8_t>(isValid);

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::LOGIN_RESPONSE_LENGTH;
        if (const bool success = m_Client->Send(m_ResBuffer, response_len);
            !success)
            return false;
        return isValid;
    }

    bool ConnectionHandler::HandleLogoutRequest() const
    {
        return false;
    }

    bool ConnectionHandler::HandleChannelListRequest() const
    {
        const auto* const channelMap = ChannelMap::Get();

        const uint32_t count = channelMap->size(); // TODO: Limit the number of channels.

        auto* const response_header = reinterpret_cast<Packet::PacketHeader*>(m_ResBuffer);

        {
            /* Inform the client of the number of channels. */
            response_header->type = (uint8_t)Packet::PacketType::ChannelListResponse;

            auto* const response_data = reinterpret_cast<Packet::ChannelListResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);
            response_data->count = count;

            constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::CHANNEL_LIST_RESPONSE_LENGTH;
            if (const bool success = m_Client->Send(m_ResBuffer, response_len);
                !success)
                return false;
        }
        if (count == 0)
            return true;

        {
            /* Send the details of each channel. */
            response_header->type = (uint8_t)Packet::PacketType::ChannelDetailResponse;
            auto* const response_data = reinterpret_cast<Packet::ChannelDetailResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

            for (const auto& [cid, channel] : *channelMap)
            {
                response_data->id = cid;

                const auto& name = channel.GetName();
                memcpy(response_data->name, name.c_str(), name.length() * sizeof(char16_t));
                *((char16_t*)response_data->name + name.length()) = u'\0';

                response_data->join_time = channel.GetJoinTime();

                const auto& description = channel.GetDescription();
                memcpy(response_data->description, description.c_str(), description.length() * sizeof(char16_t));
                *((char16_t*)response_data->description + description.length()) = u'\0';

                constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::CHANNEL_DETAIL_RESPONSE_LENGTH;
                if (const bool success = m_Client->Send(m_ResBuffer, response_len);
                    !success)
                    return false;
            }
        }
        return true;
    }

    bool ConnectionHandler::HandleChannelConnectionRequest()
    {
        const bool isValid = ValidateChannel();

        auto* const response_header = reinterpret_cast<Packet::PacketHeader*>(m_ResBuffer);
        response_header->type = (uint8_t)Packet::PacketType::ChannelConnectionResponse;

        auto* const response_data = reinterpret_cast<Packet::ChannelConnectionResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);
        response_data->isValid = static_cast<uint8_t>(isValid);

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::CHANNEL_CONNECTION_RESPONSE_LENGTH;
        if (const bool success = m_Client->Send(m_ResBuffer, response_len);
            !success)
            return false;
        return true;
    }

    bool ConnectionHandler::HandleChatMessageRequest() const
    {
        const auto* const s_LoginMap = LoginMap::Get();
        const auto client_it = s_LoginMap->find(m_UserId);
        if (client_it == s_LoginMap->cend())
            return false;

        const auto* const request_data = (Packet::ChatMessageRequest*)(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);

        const uint64_t timestamp = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count());
        const std::u16string content{(const char16_t*)request_data->content}; // mysqlx can't handle string_view like.

        std::thread tSend{
            [&]
            {
                auto* const response_header = (Packet::PacketHeader*)m_ResBuffer;
                response_header->type = (uint8_t)Packet::PacketType::ChatMessageResponse;

                auto* const response_data = (Packet::ChatMessageResponse*)(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

                const std::u16string& nickname = client_it->second.nickname;
                memcpy(response_data->nickname, nickname.data(), nickname.length() * sizeof(char16_t));
                *((char16_t*)response_data->nickname + nickname.length()) = u'\0';

                response_data->timestamp = timestamp;

                // TODO: Come up with a way to avoid copying content.
                memcpy(response_data->content, request_data->content, content.length() * sizeof(char16_t));
                *((char16_t*)response_data->content + content.length()) = u'\0';

                constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::CHAT_MESSAGE_RESPONSE_LENGTH;

                const auto* const loginMap = LoginMap::Get();
                for (const auto& userId : ChannelConnectionMap::Get()->at(m_ChannelId))
                {
                    if (userId == m_UserId)
                        continue;
                    loginMap->at(userId).client->Send(m_ResBuffer, response_len);
                }
            }
        };
        tSend.detach();

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tTranscript = db->getTable(Database::Transcript::name);
        tTranscript.insert(
            Database::Transcript::Item::cid,
            Database::Transcript::Item::uid,
            Database::Transcript::Item::time,
            Database::Transcript::Item::content
        ).values(m_ChannelId, m_UserId, timestamp, content).execute();

        if (tSend.joinable())
            tSend.join();
        return true;
    }

    bool ConnectionHandler::HandleChangeUserPasswordRequest() const
    {
        const auto* const request_data = reinterpret_cast<Packet::ChangeUserPasswordRequest*>(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);
        const std::string_view password{(const char*)request_data->password};

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tUser = db->getTable(Database::User::name);
        mysqlx::RowResult result = tUser.select(
                                            Database::User::Item::password
                                        )
                                        .where("id=:id")
                                        .bind("id", m_UserId)
                                        .execute();

#ifdef YZ_DEBUG
        if (result.count() != 1)
        {
            // This means that thread-local user id was cached incorrectly.
            // TODO: If admin can delete user, then server will close the live connection with the specific user. By that time, change the behavior here.
            __asm("int3");
        }
#endif

        auto* const response_header = (Packet::PacketHeader*)m_ResBuffer;
        response_header->type = (uint8_t)Packet::PacketType::ChangeUserPasswordResponse;

        auto* const response_data = reinterpret_cast<Packet::ChangeUserPasswordResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

        const auto row = result.fetchOne();
        if (password != row[0].get<std::string>())
        {
            response_data->success = true;

            // Ignore that database might fail for now.
            tUser.update()
                 .set(Database::User::Item::password, password.data())
                 .where("id=:id")
                 .bind("id", m_UserId)
                 .execute();
        }
        else
        {
            response_data->success = false;
            response_data->reason = (uint8_t)Packet::ChangeUserPasswordFailReason::PasswordIsIdentical;
        }

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::CHANGE_USER_PASSWORD_RESPONSE_LENGTH;
        const bool success = m_Client->Send(m_ResBuffer, response_len);
        return success;
    }

    bool ConnectionHandler::HandleChangeUserNicknameRequest() const
    {
        const auto* const request_data = reinterpret_cast<Packet::ChangeUserNicknameRequest*>(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);
        const std::u16string_view nickname{(const char16_t*)request_data->nickname};

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tUser = db->getTable(Database::User::name);

        mysqlx::RowResult result = tUser.select(
                                            s_SQLCountChannelId
                                        )
                                        .where("nickname=:nickname")
                                        .bind("nickname", nickname.data())
                                        .execute();

        auto* const response_header = (Packet::PacketHeader*)m_ResBuffer;
        response_header->type = (uint8_t)Packet::PacketType::ChangeUserNicknameResponse;

        auto* const response_data = reinterpret_cast<Packet::ChangeUserNicknameResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

        if (result.fetchOne()[0].get<uint32_t>() == 0)
        {
            // Ignore that database might fail for now.
            tUser.update()
                 .set(Database::User::Item::nickname, nickname.data())
                 .where("id=:id")
                 .bind("id", m_UserId)
                 .execute();

            response_data->success = true;
        }
        else
        {
            response_data->success = false;
            response_data->reason = (uint8_t)Packet::ChangeUserNicknameFailReason::NicknameAlreadyExists;
        }

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::CHANGE_USER_NICKNAME_RESPONSE_LENGTH;
        const bool success = m_Client->Send(m_ResBuffer, response_len);
        return success;
    }

    bool ConnectionHandler::HandleValidateAdminRequest()
    {
        const auto* const request_data = reinterpret_cast<Packet::ValidateAdminRequest*>(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);
        const std::string_view password{(const char*)request_data->password};

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tUser = db->getTable(Database::User::name);
        mysqlx::RowResult result = tUser.select(
                                            s_SQLCountUserId
                                        )
                                        .where("id=:id AND password=:password AND is_admin=1")
                                        .bind("id", m_UserId).bind("password", password.data())
                                        .execute();

        auto* const response_header = (Packet::PacketHeader*)m_ResBuffer;
        response_header->type = (uint8_t)Packet::PacketType::ValidateAdminResponse;

        auto* const response_data = reinterpret_cast<Packet::ValidateAdminResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

        if (result.fetchOne()[0].get<uint32_t>() == 1)
        {
            response_data->success = true;
            m_AdminToken = true;
        }
        else
        {
            response_data->success = false;
            response_data->reason = (uint8_t)Packet::ValidateAdminFailReason::UserPasswordIncorrect;
        }

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::VALIDATE_ADMIN_RESPONSE_LENGTH;
        const bool success = m_Client->Send(m_ResBuffer, response_len);
        return success;
    }

    bool ConnectionHandler::HandleRegisterUserRequest()
    {
        const bool isValid = ValidateRegisterUser();

        auto* const response_data = reinterpret_cast<Packet::RegisterUserResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);
        response_data->success = isValid;

        if (isValid)
        {
            // TODO: Generate a default password and nickname.
            const std::string password{"123456789"};

            const auto* const request_data = reinterpret_cast<Packet::RegisterUserRequest*>(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);
            const std::string_view phone{(const char*)request_data->phone, Database::User::ItemLength::PHONE_LENGTH};

            const uint64_t timestamp = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count());

            auto* db = MySQLConnector::Get()->GetSchema();
            mysqlx::Table tUser = db->getTable(Database::User::name);
            tUser.insert(
                     Database::User::Item::phone,
                     Database::User::Item::password,
                     Database::User::Item::nickname,
                     Database::User::Item::join_time,
                     Database::User::Item::is_admin
                     //).values(phone.data(), password, "CONVERT(unix_timestamp(now()) * 1000, CHAR)", timestamp, 0)
                 ).values(phone.data(), password, std::to_string(timestamp).data(), timestamp, 0)
                 .execute();

            memcpy(response_data->password, password.data(), password.length());
        }

        auto* const response_header = (Packet::PacketHeader*)m_ResBuffer;
        response_header->type = (uint8_t)Packet::PacketType::RegisterUserResponse;

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::REGISTER_USER_RESPONSE_LENGTH;
        const bool success = m_Client->Send(m_ResBuffer, response_len);
        return success;
    }

    bool ConnectionHandler::ValidateUserLogin()
    {
        const auto* const request_data = reinterpret_cast<Packet::LoginRequest*>(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);
        const std::string_view phone{(const char*)request_data->phone, Database::User::ItemLength::PHONE_LENGTH};
        const std::string_view password{(const char*)request_data->password};

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tUser = db->getTable(Database::User::name);
        mysqlx::RowResult result = tUser.select(
                                            Database::User::Item::id,
                                            Database::User::Item::password,
                                            Database::User::Item::nickname,
                                            Database::User::Item::join_time,
                                            Database::User::Item::is_admin
                                        )
                                        .where("phone=:phone")
                                        .bind("phone", phone.data())
                                        .execute();
#ifdef YZ_DEBUG
        if (result.count() != 0 && result.count() != 1)
        {
            // This means "There are multiple users who have the same phone number in database".
            // If this is triggered, then there is something wrong in sql statements.
            // This if-statement won't exist in release configuration.
            __asm("int3");
        }
#endif

        auto* const response_data = reinterpret_cast<Packet::LoginResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

        if (result.count() == 0)
        {
            // User doesn't exist in database.
            response_data->reason = static_cast<decltype(response_data->reason)>(Packet::LoginFailReason::UserNotExist);
            return false;
        }

        const auto& row = result.fetchOne();
        if (password != row[1].get<std::string>())
        {
            // Password in request is incorrect.
            response_data->reason = static_cast<decltype(response_data->reason)>(Packet::LoginFailReason::UserPasswordIncorrect);
            return false;
        }

        const auto& loginMap = LoginMap::Get();
        m_UserId = row[0].get<decltype(m_UserId)>();
        if (loginMap->find(m_UserId) != loginMap->cend())
        {
            // User has already logged in.
            response_data->reason = static_cast<decltype(response_data->reason)>(Packet::LoginFailReason::UserAlreadyLoggedIn);
            return false;
        }

        /* From now on, user's login is finally valid. */

        response_data->id = m_UserId;

        const std::u16string_view nickname((const char16_t*)row[2].getRawBytes().first,
                                           row[2].getRawBytes().second / sizeof(char16_t));
        memcpy(response_data->nickname, nickname.data(), nickname.length() * sizeof(char16_t));
        *((char16_t*)response_data->nickname + nickname.length()) = u'\0';

        response_data->join_time = row[3].get<decltype(response_data->join_time)>();

        const bool is_admin = row[4].get<bool>();
        m_IsAdmin = is_admin;
        response_data->isAdmin = static_cast<decltype(response_data->isAdmin)>(is_admin);

        loginMap->emplace(m_UserId,
                          ClientInfo
                          {
                              .id = response_data->id,
                              .phone = std::string{phone},
                              .nickname = std::u16string{nickname},
                              .join_time = response_data->join_time,
                              .is_admin = is_admin,
                              .client = m_Client
                          });
        return true;
    }

    bool ConnectionHandler::ValidateChannel()
    {
        const auto* const request_data = (Packet::ChannelConnectionRequest*)(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);
        const auto& cid = request_data->cid;

        auto* const connectionMap = ChannelConnectionMap::Get();
        if (auto connections = connectionMap->find(cid);
            connections != connectionMap->cend())
        {
            // Establish connection.
            connections->second.emplace(m_UserId);
            m_ChannelId = cid;
            return true;
        }

        // This means that channel id doesn't exist in memory. Check it up for once.
        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tChannel = db->getTable(Database::Channel::name);
        mysqlx::RowResult result = tChannel.select(
            Database::Channel::Item::id
        ).where("id=:id").bind("id", cid).execute();

        auto* const response_data = reinterpret_cast<Packet::ChannelConnectionResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

        switch (result.count())
        {
        case 0:
            {
                response_data->reason = (uint8_t)Packet::ChannelConnectionFailReason::ChannelNotExist;
                return false;
            }
#ifdef YZ_DEBUG
        case 1:
            {
                // This means it actually exists.
                connectionMap->emplace(cid, std::unordered_set<uint32_t>{cid});
                m_ChannelId = cid;
                return true;
            }
        default:
            {
                // This means that multiple channel with same id exists in mysql.
                // There might be something wrong in sql statements.
                __asm("int3");
                return false;
            }
#endif
        }
    }

    bool ConnectionHandler::ValidateRegisterUser()
    {
        auto* const response_data = reinterpret_cast<Packet::RegisterUserResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);
        if (!m_AdminToken)
        {
            response_data->reason = (uint8_t)Packet::RegisterUserFailReason::AdminTokenExpired;
            return false;
        }
        m_AdminToken = false;

        const auto* const request_data = reinterpret_cast<Packet::RegisterUserRequest*>(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);
        const std::string_view phone{(const char*)request_data->phone, Database::User::ItemLength::PHONE_LENGTH};

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tUser = db->getTable(Database::User::name);

        if (mysqlx::RowResult result = tUser.select(
                                                s_SQLCountUserId
                                            )
                                            .where("phone==:phone")
                                            .bind("phone", phone.data())
                                            .execute();
            result.fetchOne()[0].get<uint32_t>() != 0)
        {
            response_data->reason = (uint8_t)Packet::RegisterUserFailReason::UserAlreadyExists;
            return false;
        }

        return true;
    }
}

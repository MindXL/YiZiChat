#include "ConnectionHandler.h"

#include <arpa/inet.h>
#include <string_view>
#include <thread>

#include "BufferManager.h"
#include "MySQLConnector.h"
#include "LoginMap.h"

namespace YiZi::Server
{
    ConnectionHandler::ConnectionHandler(SAcceptSocket* const client)
        : m_Client{client}, m_UserId{0}, m_ReqBuffer{BufferManager::Get()->Fetch().first}, m_ResBuffer{m_ReqBuffer + Packet::REQUEST_MAX_LENGTH()} { }

    ConnectionHandler::~ConnectionHandler()
    {
        BufferManager::Get()->Return(m_ReqBuffer);
        m_Client->Close();
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
    }

    bool ConnectionHandler::Dispatch()
    {
        switch (const auto request_header = reinterpret_cast<Packet::PacketHeader*>(m_ReqBuffer);
            Packet::PacketType{request_header->type})
        {
        case Packet::PacketType::LoginRequest: return HandleLoginRequest();
        case Packet::PacketType::LogoutRequest: return HandleLogoutRequest();
        case Packet::PacketType::ChannelListRequest: return HandleChannelListRequest();
        case Packet::PacketType::ChatMessageRequest: return HandleChatMessageRequest();
        default: return false;
        }
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
        LoginMap::Get()->erase(m_UserId);
        return false;
    }

    bool ConnectionHandler::HandleChannelListRequest() const
    {
        // TODO: Maybe move database request to a thread.
        // TODO: Cache channel data on server.
        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tChannel = db->getTable(Database::Channel::name);
        mysqlx::RowResult result = tChannel.select(
            Database::Channel::Item::id,
            Database::Channel::Item::name,
            Database::Channel::Item::join_time,
            Database::Channel::Item::description
        ).execute();

        const uint32_t count = (uint32_t)result.count(); // TODO: Limit the number of channels.

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

            for (const auto& row : result)
            {
                response_data->id = row[0].get<decltype(response_data->id)>();

                const auto& [name, nameByteCount] = row[1].getRawBytes();
                memcpy(response_data->name, name, nameByteCount);
                *((char16_t*)response_data->name + nameByteCount / sizeof(char16_t)) = u'\0';

                response_data->join_time = row[2].get<decltype(response_data->join_time)>();

                const auto& [description, descriptionByteCount] = row[3].getRawBytes();
                memcpy(response_data->description, description, descriptionByteCount);
                *((char16_t*)response_data->description + descriptionByteCount / sizeof(char16_t)) = u'\0';

                constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::CHANNEL_DETAIL_RESPONSE_LENGTH;
                if (const bool success = m_Client->Send(m_ResBuffer, response_len);
                    !success)
                    return false;
            }
        }
        return true;
    }

    bool ConnectionHandler::HandleChatMessageRequest() const
    {
        const auto* const s_LoginMap = LoginMap::Get();
        const auto client_it = s_LoginMap->find(m_UserId);
        if (client_it == s_LoginMap->cend())
            return false;

        const auto* const request_data = (Packet::ChatMessageRequest*)(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);

        const uint32_t& uid = request_data->id;
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
                for (const auto& [userId, userInfo] : *LoginMap::Get())
                {
                    if (userId == m_UserId)
                        continue;
                    userInfo.client->Send(m_ResBuffer, response_len);
                }
            }
        };
        tSend.detach();

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tTranscript = db->getTable(Database::Transcript::name);
        tTranscript.insert(
            Database::Transcript::Item::uid,
            Database::Transcript::Item::time,
            Database::Transcript::Item::content
        ).values(uid, timestamp, content).execute();

        if (tSend.joinable())
            tSend.join();
        return true;
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
}

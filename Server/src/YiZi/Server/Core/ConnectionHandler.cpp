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
    ConnectionHandler::ConnectionHandler(SAcceptSocket* const client)
        : m_Client{client}, m_UserId{0}, m_ChannelId{0}, m_ReqBuffer{BufferManager::Get()->Fetch().first},
          m_ResBuffer{m_ReqBuffer + Packet::REQUEST_MAX_LENGTH()} { }

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
        case Packet::PacketType::ChannelConnectionRequest: return HandleChannelConnectionRequest();
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
        const auto* const channelMap = ChannelMap::Get();

        const uint32_t count = (uint32_t)channelMap->size(); // TODO: Limit the number of channels.

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
        case 1:
            {
                // This means it actually exists.
                connectionMap->emplace(cid, std::unordered_set<uint32_t>{cid});
                m_ChannelId = cid;
                return true;
            }
#ifdef YZ_DEBUG
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
}

#include "ConnectionHandler.h"

#include <arpa/inet.h>
#include <string_view>

#include "BufferManager.h"
#include "MySQLConnector.h"
#include "LoginMap.h"

namespace YiZi::Server
{
    const std::string ConnectionHandler::s_DatabaseUserJoinTimeExpr = std::move(
        std::string{"unix_timestamp("}.append(Database::User::Item::join_time).append(")"));

    ConnectionHandler::ConnectionHandler(SAcceptSocket* const client)
        : m_Client{client}, m_ReqBuffer{BufferManager::Get()->Fetch().first}, m_ResBuffer{m_ReqBuffer + Packet::REQUEST_MAX_LENGTH()} { }

    ConnectionHandler::~ConnectionHandler()
    {
        BufferManager::Get()->Return(m_ReqBuffer);
        m_Client->Close();
        delete m_Client;
    }

    void ConnectionHandler::Run() const
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

    bool ConnectionHandler::Dispatch() const
    {
        switch (const auto request_header = reinterpret_cast<Packet::PacketHeader*>(m_ReqBuffer);
            Packet::PacketType{request_header->type})
        {
        case Packet::PacketType::LoginRequest: return HandleLoginRequest();
        case Packet::PacketType::LogoutRequest: return HandleLogoutRequest();
        case Packet::PacketType::ChatMessageRequest: return HandleChatMessageRequest();
        default: return false;
        }
    }

    bool ConnectionHandler::HandleLoginRequest() const
    {
        const auto* const request_data = reinterpret_cast<Packet::LoginRequest*>(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);
        const std::string_view phone{(const char*)request_data->phone, Database::User::ItemLength::PHONE_LENGTH};
        std::string_view password{(const char*)request_data->password, Database::User::ItemLength::PASSWORD_MAX_LENGTH};
        password.remove_suffix(password.size() - password.find_first_of('\0'));

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tUser = db->getTable(Database::User::name);
        mysqlx::RowResult result = tUser.select(
                                            Database::User::Item::id,
                                            Database::User::Item::nickname,
                                            s_DatabaseUserJoinTimeExpr,
                                            Database::User::Item::is_admin
                                        )
                                        .where("phone=:phone AND password=:password")
                                        .bind("phone", phone.data()).bind("password", password.data())
                                        .execute();

        auto* const response_header = reinterpret_cast<Packet::PacketHeader*>(m_ResBuffer);
        response_header->type = (uint8_t)Packet::PacketType::LoginResponse;

        auto* const response_data = reinterpret_cast<Packet::LoginResponse*>(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

        const bool isValid = result.count() == 1;
        response_data->isValid = static_cast<uint8_t>(isValid);
        if (isValid)
        {
            mysqlx::Row row = result.fetchOne();
            response_data->id = row[0].get<decltype(response_data->id)>();

            const std::u16string_view nickname((const char16_t*)row[1].getRawBytes().first, row[1].getRawBytes().second / sizeof(char16_t));
            memcpy(response_data->nickname, nickname.data(), nickname.length() * sizeof(char16_t));
            *((char16_t*)response_data->nickname + nickname.length()) = u'\0';

            response_data->join_time = row[2].get<decltype(response_data->join_time)>();

            const bool is_admin = row[3].get<bool>();
            response_data->isAdmin = static_cast<decltype(response_data->isAdmin)>(is_admin);

            LoginMap::Get()->emplace(m_Client,
                                     ClientInfo
                                     {
                                         .id = response_data->id,
                                         .phone = std::string{phone},
                                         .nickname = std::u16string{nickname},
                                         .join_time = response_data->join_time,
                                         .is_admin = is_admin
                                     });
        }

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::LOGIN_RESPONSE_LENGTH;
        if (const bool success = m_Client->Send(m_ResBuffer, response_len);
            !success)
            return false;
        return isValid;
    }

    bool ConnectionHandler::HandleLogoutRequest() const
    {
        LoginMap::Get()->erase(m_Client);
        return false;
    }

    bool ConnectionHandler::HandleChatMessageRequest() const
    {
        const auto* const s_LoginMap = LoginMap::Get();
        const auto client_it = s_LoginMap->find(m_Client);
        if (client_it == s_LoginMap->cend())
            return false;

        const auto* const request_data = (Packet::ChatMessageRequest*)(m_ReqBuffer + Packet::PACKET_HEADER_LENGTH);

        // TODO: Store the transcript in database.

        auto* const response_header = (Packet::PacketHeader*)m_ResBuffer;
        response_header->type = (uint8_t)Packet::PacketType::ChatMessageResponse;

        auto* const response_data = (Packet::ChatMessageResponse*)(m_ResBuffer + Packet::PACKET_HEADER_LENGTH);

        const std::u16string& nickname = client_it->second.nickname;
        memcpy(response_data->nickname, nickname.data(), nickname.length() * sizeof(char16_t));
        *((char16_t*)response_data->nickname + nickname.length()) = u'\0';

        response_data->timestamp = static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );

        // TODO: Come up with a way to avoid copying content.
        const std::u16string_view content{(const char16_t*)request_data->content};
        memcpy(response_data->content, request_data->content, content.length() * sizeof(char16_t));
        *((char16_t*)response_data->content + content.length()) = u'\0';

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::CHAT_MESSAGE_RESPONSE_LENGTH;
        for (const auto& [client, _] : *LoginMap::Get())
        {
            if (client == m_Client)
                continue;
            client->Send(m_ResBuffer, response_len);
        }

        return true;
    }
}

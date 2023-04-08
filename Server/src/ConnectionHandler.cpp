#include "ConnectionHandler.h"

#include <arpa/inet.h>
#include <iostream>
#include <string_view>

#include "BufferManager.h"
#include "MySQLConnector.h"
#include "LoginMap.h"

namespace YiZi::Server
{
    void ConnectionHandler::Handle(const std::shared_ptr<Server::SAcceptSocket>& client)
    {
        auto [reqBuffer, resBuffer] = BufferManager::Get()->Fetch();

        while (true)
        {
            if (!client->Receive(reqBuffer, Packet::REQUEST_MAX_LENGTH()))
                break;

            // TODO: Use multi-thread to wrap Dispatch().
            if (!Dispatch(client, reqBuffer, resBuffer))
                break;
        }

        client->Close();
        BufferManager::Get()->Return(reqBuffer);
    }

    bool ConnectionHandler::Dispatch(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* const reqBuffer, uint8_t* const resBuffer)
    {
        switch (const auto request_header = reinterpret_cast<Packet::PacketHeader*>(reqBuffer);
            Packet::PacketType{request_header->type})
        {
        case Packet::PacketType::LoginRequest: return HandleLoginRequest(client, reqBuffer, resBuffer);
        case Packet::PacketType::LogoutRequest: return HandleLogoutRequest(client, reqBuffer, resBuffer);
        case Packet::PacketType::ChatMessageRequest: return HandleChatMessageRequest(client, reqBuffer, resBuffer);
        default: return false;
        }
    }

    const std::string ConnectionHandler::s_DatabaseUserJoinTimeExpr = std::move(
        std::string{"unix_timestamp("}.append(Database::User::Item::join_time).append(")"));

    bool ConnectionHandler::HandleLoginRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer)
    {
        const auto* const request_data = reinterpret_cast<Packet::LoginRequest*>(reqBuffer + Packet::PACKET_HEADER_LENGTH);
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

        auto* const response_header = reinterpret_cast<Packet::PacketHeader*>(resBuffer);
        response_header->type = (uint8_t)Packet::PacketType::LoginResponse;

        auto* const response_data = reinterpret_cast<Packet::LoginResponse*>(resBuffer + Packet::PACKET_HEADER_LENGTH);

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

            LoginMap::Get()->emplace(client,
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
        if (const bool success = client->Send(resBuffer, response_len);
            !success)
            return false;
        return isValid;
    }

    bool ConnectionHandler::HandleLogoutRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer)
    {
        LoginMap::Get()->erase(client);
        return false;
    }

    bool ConnectionHandler::HandleChatMessageRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer)
    {
        const auto* const request_data = (Packet::ChatMessageRequest*)(reqBuffer + Packet::PACKET_HEADER_LENGTH);

        // TODO: If user is not logged in.

        /* Test code */
        std::u16string_view content{(const char16_t*)request_data->content, Database::Transcript::ItemLength::CONTENT_MAX_LENGTH};
        content.remove_suffix(content.length() - content.find_first_of(u'\0'));
        std::cout << "Got a message from client: ";
        //std::u<< content << std::endl;
        /* Test code */

        // TODO: Send message to all other users.
        return true;
    }
}

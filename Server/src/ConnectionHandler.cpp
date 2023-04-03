#include "ConnectionHandler.h"

#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <string_view>

#include "BufferManager.h"

namespace YiZi
{
    void ConnectionHandler::Handle(const std::shared_ptr<Server::SAcceptSocket>& client)
    {
        auto [reqBuffer, resBuffer] = BufferManager::Get()->Fetch();

        while (true)
        {
            if (!client->Receive(reqBuffer, Packet::REQUEST_MAX_LENGTH()))
                break;

            Dispatch(client, reqBuffer, resBuffer);
        }

        BufferManager::Get()->Return(reqBuffer);
    }

    void ConnectionHandler::Dispatch(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* const reqBuffer, uint8_t* const resBuffer)
    {
        switch (const auto request_header = reinterpret_cast<Packet::PacketHeader*>(reqBuffer);
            Packet::PacketType{request_header->type})
        {
        case Packet::PacketType::TestRequest: HandleTestRequest(client, reqBuffer, resBuffer);
            break;
        case Packet::PacketType::LoginRequest: HandleLoginRequest(client, reqBuffer, resBuffer);
            break;
        default: ;
        }
    }

    void ConnectionHandler::HandleLoginRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* reqBuffer, uint8_t* resBuffer)
    {
        const auto* const request_data = reinterpret_cast<Packet::LoginRequest*>(reqBuffer + Packet::PACKET_HEADER_LENGTH);
        const std::string_view phone{(const char*)request_data->phone, Database::User::ItemLength::PHONE_LENGTH};
        std::string_view password{(const char*)request_data->password, Database::User::ItemLength::PASSWORD_MAX_LENGTH};
        password.remove_suffix(password.size() - password.find_first_of('\0'));

        auto* const response_header = reinterpret_cast<Packet::PacketHeader*>(resBuffer);
        response_header->type = (uint8_t)Packet::PacketType::LoginResponse;

        auto* const response_data = reinterpret_cast<Packet::LoginResponse*>(resBuffer + Packet::PACKET_HEADER_LENGTH);

        // TODO: Require database support.
        if (phone == "13312345678" && password == "password")
        {
            response_data->isValid = true;

            response_data->id = 0;
            constexpr const char16_t* nickname = u"12as";
            memcpy(response_data->nickname, nickname, sizeof(nickname) * sizeof(char16_t));
            // TODO: Make sure a zero character follows (if nickname doesn't take up the whole space).

            response_data->join_time = 0;

            response_data->isAdmin = true;
        }
        else
        {
            response_data->isValid = false;
        }

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::LOGIN_RESPONSE_LENGTH;
        bool success = client->Send(resBuffer, response_len);
    }

    void ConnectionHandler::HandleTestRequest(const std::shared_ptr<Server::SAcceptSocket>& client, uint8_t* const reqBuffer,
                                              uint8_t* const resBuffer)
    {
        const auto* const request_data = reinterpret_cast<Packet::TestRequest*>(reqBuffer + Packet::PACKET_HEADER_LENGTH);

        const std::string_view content{(const char*)request_data->message, request_data->length};

        std::cout << "Got a test message: " << content << std::endl;

        auto* const response_header = reinterpret_cast<Packet::PacketHeader*>(resBuffer);
        response_header->type = (uint8_t)Packet::PacketType::TestResponse;

        auto* const response_data = reinterpret_cast<Packet::TestResponse*>(resBuffer + Packet::PACKET_HEADER_LENGTH);

        const std::string message{"Welcome to YiZiChat."};
        memcpy(response_data->message, message.data(), message.length());
        response_data->length = (uint32_t)message.length();

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::TEST_RESPONSE_LENGTH;
        bool success = client->Send(resBuffer, response_len);
    }
}

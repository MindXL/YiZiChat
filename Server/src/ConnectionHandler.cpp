#include "ConnectionHandler.h"

#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <string_view>

#include "BufferManager.h"

namespace YiZi
{
    void ConnectionHandler::Handle(socket_t connfd)
    {
        auto [reqBuffer, resBuffer] = BufferManager::Get()->Fetch();

        while (true)
        {
            if (recv(connfd, reqBuffer, Packet::REQUEST_MAX_LENGTH(), 0) <= 0)
                break;

            Dispatch(connfd, reqBuffer, resBuffer);
        }

        BufferManager::Get()->Return(reqBuffer);
    }

    void ConnectionHandler::Dispatch(socket_t connfd, uint8_t* const reqBuffer, uint8_t* const resBuffer)
    {
        switch (const auto request_header = reinterpret_cast<Packet::PacketHeader*>(reqBuffer);
            Packet::PacketType{request_header->type})
        {
        case Packet::PacketType::TestRequest: HandleTestRequest(connfd, reqBuffer, resBuffer);
            break;
        default: ;
        }
    }

    void ConnectionHandler::HandleTestRequest(socket_t connfd, uint8_t* const reqBuffer, uint8_t* const resBuffer)
    {
        const auto request_data = reinterpret_cast<Packet::TestRequest*>(reqBuffer + Packet::PACKET_HEADER_LENGTH);

        const std::string_view content{(char*)request_data->message, request_data->length};

        std::cout << "Got a test message: " << content << std::endl;

        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::TEST_RESPONSE_LENGTH;
        const auto response = resBuffer;

        const auto response_header = reinterpret_cast<Packet::PacketHeader*>(response);
        response_header->type = (uint8_t)Packet::PacketType::TestResponse;
        response_header->length = response_len;

        const auto response_data = reinterpret_cast<Packet::TestResponse*>(response + Packet::PACKET_HEADER_LENGTH);

        const std::string message{"Welcome to YiZiChat."};
        memcpy(response_data->message, message.data(), message.length());
        response_data->length = message.length();

        bool success = send(connfd, response, response_len, 0);
    }
}

#include "pch.h"
#include "ConnectionHandler.h"

namespace YiZi
{
    void ConnectionHandler::Handle()
    {
        while (true)
        {
            if (recv(Socket::Get()->GetConnfd(), (char*)Buffer::Get()->GetResBuffer(), Packet::RESPONSE_MAX_LENGTH(), 0) <= 0)
                break;

            Dispatch();
        }
    }

    void ConnectionHandler::Dispatch()
    {
        switch (const auto response_header = reinterpret_cast<Packet::PacketHeader*>(Buffer::Get()->GetResBuffer());
            Packet::PacketType{response_header->type})
        {
        case Packet::PacketType::TestResponse: HandleTestResponse();
            break;
        default: ;
        }
    }

    void ConnectionHandler::HandleTestRequest()
    {
        constexpr int request_len = Packet::PACKET_HEADER_LENGTH + Packet::TEST_REQUEST_LENGTH;
        const auto request = Buffer::Get()->GetReqBuffer();

        const auto request_header = reinterpret_cast<Packet::PacketHeader*>(request);
        request_header->type = uint8_t(Packet::PacketType::TestRequest);
        request_header->length = request_len;

        const auto request_data = reinterpret_cast<Packet::TestRequest*>(request + Packet::PACKET_HEADER_LENGTH);

        const std::string message{"Hello world."};
        memcpy_s(request_data->message, 30, message.data(), message.length());

        request_data->length = (uint32_t)message.length();

        std::cout << "Sending message: [" << message << "] to server..." << std::endl;

        if (!send(Socket::Get()->GetConnfd(), (char*)request, request_len, 0))
        {
            std::cout << "Send message failed." << std::endl;
            //return -1;
        }
    }

    void ConnectionHandler::HandleTestResponse()
    {
        constexpr int response_len = Packet::PACKET_HEADER_LENGTH + Packet::TEST_RESPONSE_LENGTH;
        const auto response = Buffer::Get()->GetResBuffer();

        const auto response_data = reinterpret_cast<Packet::TestResponse*>(response + Packet::PACKET_HEADER_LENGTH);
        std::string_view message{(char*)response_data->message, response_data->length};

        std::cout << "Got reply from server: [" << message << "]." << std::endl;
    }
}

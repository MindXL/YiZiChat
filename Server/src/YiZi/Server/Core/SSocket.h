#pragma once

#include <YiZi/YiZi.h>
#include <arpa/inet.h>

namespace YiZi::Server
{
    // Server-End YiZi::Socket implementation (Used to listen).
    class SListenSocket final : Socket
    {
    public:
        ~SListenSocket() override = default;

        void Initialize() override;
        void Close() override;

        bool Connect(ip_t ip, port_t port) override;
        bool Listen(ip_t ip, port_t port) override;
        bool Accept() override;

        bool Send(const void* buffer, packet_length_t byteCount) override;
        packet_length_t Receive(void* buffer, packet_length_t byteCount) override;

        static Socket* Get() { return s_SListenSocket; }

    private:
        sockaddr_in m_ServerAddress{};

        static Socket* s_SListenSocket;

    private:
        SListenSocket() = default;
    };

    // Server-End YiZi::Socket implementation (Used to accept).
    class SAcceptSocket final : Socket
    {
    public:
        ~SAcceptSocket() override = default;

        void Initialize() override;
        void Close() override;

        bool Connect(ip_t ip, port_t port) override;
        bool Listen(ip_t ip, port_t port) override;
        bool Accept() override;

        bool Send(const void* buffer, packet_length_t byteCount) override;
        packet_length_t Receive(void* buffer, packet_length_t byteCount) override;

    private:
        sockaddr_in m_ClientAddress{};
    };
}

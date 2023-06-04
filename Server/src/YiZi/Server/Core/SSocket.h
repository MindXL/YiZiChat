#pragma once

#include <YiZi/YiZi.h>

#include <arpa/inet.h>
#include <atomic>

namespace YiZi::Server
{
    // Server-End YiZi::Socket implementation (Used to listen).
    class SListenSocket final : public Socket
    {
    public:
        ~SListenSocket() override = default;

        void Initialize() override;
        void Close() override;

        bool Connect(ip_t ip, port_t port) override;
        bool Listen(ip_t ip, port_t port) override;
        bool Accept() override;

        int Send(const void* buffer, packet_length_t byteCount) override;
        packet_length_t Receive(void* buffer, packet_length_t byteCount, int flags = 0) override;

        [[nodiscard]] bool IsClosed() const override { return s_IsClosed; }

        [[nodiscard]] static Socket* Get() { return s_SListenSocket; }

    private:
        SListenSocket() = default;

        void SetClosed() override { s_IsClosed = true; }
        void UnsetClosed() override { s_IsClosed = false; }

    private:
        sockaddr_in m_ServerAddress{};

        static std::atomic<bool> s_IsClosed;

        static Socket* s_SListenSocket;
    };

    // Server-End YiZi::Socket implementation (Used to accept).
    class SAcceptSocket final : public Socket
    {
    public:
        ~SAcceptSocket() override = default;

        void Initialize() override { UnsetClosed(); }
        void Close() override;

        bool Connect(ip_t ip, port_t port) override;
        bool Listen(ip_t ip, port_t port) override;
        bool Accept() override;

        int Send(const void* buffer, packet_length_t byteCount) override;
        packet_length_t Receive(void* buffer, packet_length_t byteCount, int flags = 0) override;
        [[nodiscard]] bool IsClosed() const override { return m_IsClosed; }

    private:
        void SetClosed() override { m_IsClosed = true; }
        void UnsetClosed() override { m_IsClosed = false; }

    private:
        sockaddr_in m_ClientAddress{};

        bool m_IsClosed = true;
    };
}

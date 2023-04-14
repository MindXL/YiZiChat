#pragma once

namespace YiZi::Client
{
    // Client-End YiZi::Socket implementation.
    class CSocket final : public Socket
    {
    public:
        ~CSocket() override = default;

        void Initialize() override { m_Socket.Create(); }
        void Close() override { m_Socket.Close(); }

        bool Connect(ip_t ip, port_t port) override;
        bool Listen(ip_t ip, port_t port) override { throw std::logic_error{"Listen() is not supported in Client-End Socket"}; }
        bool Accept() override { throw std::logic_error{"Accept() is not supported in Client-End Socket"}; }

        int Send(const void* buffer, const packet_length_t byteCount) override { return m_Socket.Send(buffer, byteCount); }
        packet_length_t Receive(void* buffer, const packet_length_t byteCount) override { return m_Socket.Receive(buffer, byteCount); }

        static Socket* Get() { return s_CSocket; }

    private:
        static Socket* s_CSocket;

    private:
        CSocket() = default;
    };
}

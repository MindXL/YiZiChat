#pragma once

namespace YiZi
{
    class Socket
    {
    public:
        virtual ~Socket() = default;

        virtual void Initialize() = 0;
        virtual void Close() = 0;

        virtual bool Connect(ip_t ip, port_t port) = 0;
        virtual bool Listen(ip_t ip, port_t port) = 0;
        virtual bool Accept() = 0;

        virtual int Send(const void* buffer, packet_length_t byteCount) = 0;
        virtual packet_length_t Receive(void* buffer, packet_length_t byteCount) = 0;

        ip_t GetIP() const { return m_IP; }
        port_t GetPort() const { return m_Port; }
        const socket_t& GetSocket() const { return m_Socket; }

    protected:
        ip_t m_IP{};
        port_t m_Port{};
        socket_t m_Socket{};
    };
}

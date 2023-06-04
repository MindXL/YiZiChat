#pragma once

namespace YiZi
{
    class Socket
    {
    public:
        virtual ~Socket() = default;

        Socket() = default;
#if defined(YZ_CLIENT)
        explicit Socket(const Socket* const other)
            : m_IP{other->m_IP}, m_Port{other->m_Port} { }
#elif defined(YZ_SERVER)
        explicit Socket(const Socket* const other)
            : m_IP{other->m_IP}, m_Port{other->m_Port}, m_Socket{other->m_Socket} {}
#else
        //#error "macro missing"
#endif

        virtual void Initialize() = 0;
        virtual void Close() = 0;

        virtual bool Connect(ip_t ip, port_t port) = 0;
        virtual bool Listen(ip_t ip, port_t port) = 0;
        virtual bool Accept() = 0;

        virtual int Send(const void* buffer, packet_length_t byteCount) = 0;
        virtual packet_length_t Receive(void* buffer, packet_length_t byteCount, int flags = 0) = 0;

        [[nodiscard]] ip_t GetIP() const { return m_IP; }
        [[nodiscard]] port_t GetPort() const { return m_Port; }
        [[nodiscard]] const socket_t& GetSocket() const { return m_Socket; }

        [[nodiscard]] virtual bool IsClosed() const = 0;

    private:
        // Make sure to set close flag if sent zero-length packet.
        virtual void SetClosed() = 0;
        // Make sure to unset close flag if initialized.
        virtual void UnsetClosed() = 0;

    protected:
        ip_t m_IP{};
        port_t m_Port{};
        socket_t m_Socket{};
    };
}

#include "SSocket.h"

#include <stdexcept>
#include <limits>

namespace YiZi::Server
{
    std::atomic<bool> SListenSocket::s_IsClosed{true};

    Socket* SListenSocket::s_SListenSocket{new SListenSocket{}};

    void SListenSocket::Initialize()
    {
        m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        m_ServerAddress.sin_family = PF_INET;

        constexpr int option = 1;
        if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof option) < 0)
            throw std::runtime_error{"Fatal: SListenSocket initialization (setsockopt) failed."};

        UnsetClosed();
    }

    void SListenSocket::Close()
    {
        shutdown(m_Socket, SHUT_RDWR);

        SetClosed();
    }

    bool SListenSocket::Connect(ip_t ip, port_t port)
    {
        throw std::logic_error{"Connect() is not supported in Server-End Socket (SListenSocket)."};
    }

    bool SListenSocket::Listen(const ip_t ip, const port_t port)
    {
        m_ServerAddress.sin_addr.s_addr = inet_addr(ip);
        m_ServerAddress.sin_port = htons(port);

        if (bind(m_Socket, (const sockaddr*)&m_ServerAddress, sizeof m_ServerAddress) < 0)
        {
            SetClosed();
            throw std::runtime_error{"Fatal: SListenSocket bind failed."};
        }
        if (listen(m_Socket, 100) == -1)
        {
            SetClosed();
            throw std::runtime_error{"Fatal: SListenSocket Listen failed."};
        }
        return true;
    }

    bool SListenSocket::Accept()
    {
        throw std::logic_error{"Accept() is not supported in Server-End Socket (SListenSocket)."};
    }

    int SListenSocket::Send(const void* const buffer, packet_length_t byteCount)
    {
        throw std::logic_error{"Send() is not supported in Server-End Socket (SListenSocket)."};
    }

    packet_length_t SListenSocket::Receive(void* const buffer, packet_length_t byteCount)
    {
        throw std::logic_error{"Receive() is not supported in Server-End Socket (SListenSocket)."};
    }

    void SAcceptSocket::Close()
    {
        shutdown(m_Socket, SHUT_RDWR);

        SetClosed();
    }

    bool SAcceptSocket::Connect(ip_t ip, port_t port)
    {
        throw std::logic_error{"Connect() is not supported in Server-End Socket (SAcceptSocket)."};
    }

    bool SAcceptSocket::Listen(const ip_t ip, const port_t port)
    {
        throw std::logic_error{"Listen() is not supported in Server-End Socket (SAcceptSocket)."};
    }

    bool SAcceptSocket::Accept()
    {
        socklen_t cli_addr_len;
        const socket_t listenfd = SListenSocket::Get()->GetSocket();
        const socket_t socket = accept(listenfd, (sockaddr*)&m_ClientAddress, &cli_addr_len);

        if (socket == -1)
        {
            SetClosed();
            throw std::runtime_error{"SAcceptSocket: Socket accept failed."};
        }

        m_Socket = socket;
        return true;
    }

    int SAcceptSocket::Send(const void* const buffer, const packet_length_t byteCount)
    {
        const auto count = send(m_Socket, buffer, byteCount, 0);
#ifdef YZ_DEBUG
        // This means somewhere in code sent a packet longer than 2147483647.
        if (count > static_cast<decltype(count)>(std::numeric_limits<int>::max()))
        {
            SetClosed();
            __asm("int3");
        }
        return -1;
#else
        if (count <= 0)
            SetClosed();
        return static_cast<int>(count);
#endif
    }

    packet_length_t SAcceptSocket::Receive(void* const buffer, const packet_length_t byteCount)
    {
        const auto count = recv(m_Socket, buffer, byteCount, 0);
        if (count <= 0)
            SetClosed();
        return count;
    }
}

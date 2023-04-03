#include "SSocket.h"

namespace YiZi::Server
{
    void SListenSocket::Initialize()
    {
        m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        m_ServerAddress.sin_family = PF_INET;

        constexpr int option = 1;
        if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof option) < 0)
            throw std::runtime_error{"Fatal: SListenSocket initialization (setsockopt) failed."};
    }

    void SListenSocket::Close()
    {
        shutdown(m_Socket, SHUT_RDWR);
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
            throw std::runtime_error{"Fatal: SListenSocket bind failed."};
        if (listen(m_Socket, 100) == -1)
            throw std::runtime_error{"Fatal: SListenSocket Listen failed."};
        return true;
    }

    bool SListenSocket::Accept()
    {
        throw std::logic_error{"Accept() is not supported in Server-End Socket (SListenSocket)."};
    }

    bool SListenSocket::Send(const void* const buffer, packet_length_t byteCount)
    {
        throw std::logic_error{"Send() is not supported in Server-End Socket (SListenSocket)."};
    }

    packet_length_t SListenSocket::Receive(void* const buffer, packet_length_t byteCount)
    {
        throw std::logic_error{"Receive() is not supported in Server-End Socket (SListenSocket)."};
    }

    Socket* SListenSocket::s_SListenSocket{new SListenSocket{}};

    void SAcceptSocket::Initialize() {}

    void SAcceptSocket::Close()
    {
        shutdown(m_Socket, SHUT_RDWR);
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
        const socket_t socket = accept(SListenSocket::Get()->GetSocket(), (sockaddr*)&m_ClientAddress, &cli_addr_len);

        if (socket == -1)
            throw std::runtime_error{"SAcceptSocket: Socket accept failed."};

        m_Socket = socket;
        return true;
    }

    bool SAcceptSocket::Send(const void* const buffer, const packet_length_t byteCount)
    {
        return send(m_Socket, buffer, byteCount, 0) != -1;
    }

    packet_length_t SAcceptSocket::Receive(void* const buffer, const packet_length_t byteCount)
    {
        return recv(m_Socket, buffer, byteCount, 0) != -1;
    }
}

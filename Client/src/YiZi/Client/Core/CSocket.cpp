#include "pch.h"
#include "CSocket.h"

namespace YiZi::Client
{
    std::atomic<bool> CSocket::s_IsClosed{true};

    Socket* const CSocket::s_CSocket{new CSocket{}};

    bool CSocket::Connect(const ip_t ip, const port_t port)
    {
        m_IP = ip;
        m_Port = port;
        return m_Socket.Connect(ip, port);
    }

    int CSocket::Send(const void* buffer, const packet_length_t byteCount)
    {
        const int count = m_Socket.Send(buffer, byteCount);
        if (count <= 0)
            SetClosed();
        return count;
    }

    packet_length_t CSocket::Receive(void* buffer, const packet_length_t byteCount)
    {
        const int count = m_Socket.Receive(buffer, byteCount);
        if (count <= 0)
            SetClosed();
        return count;
    }
}

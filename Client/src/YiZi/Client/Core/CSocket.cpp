#include "pch.h"
#include "CSocket.h"

namespace YiZi::Client
{
    Socket* CSocket::s_CSocket{new CSocket{}};

    bool CSocket::Connect(const ip_t ip, const port_t port)
    {
        m_IP = ip;
        m_Port = port;
        return m_Socket.Connect(ip, port);
    }
}

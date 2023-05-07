#include "pch.h"
#include "CSocket.h"

namespace YiZi::Client
{
    std::atomic<bool> CSocket::s_IsClosed{true};

    std::atomic<bool> CSocket::s_ThreadClosed{true};

    const std::thread::id CSocket::s_MainThreadId{std::this_thread::get_id()};

    Socket* const CSocket::s_CSocket{new CSocket{}};

    CSocket::CSocket(const Socket* const other)
        : Socket(other)
    {
#ifdef YZ_DEBUG
        // Only when using MFC static library. (Lib is used to load debug symbols).
        AfxSocketInit();
#endif
        m_Socket.Attach(other->GetSocket());
    }

    CSocket::~CSocket()
    {
        if (m_IsAttached)
            m_Socket.Detach();
    }

    void CSocket::Initialize()
    {
#ifdef YZ_DEBUG
        // Only when using MFC static library. (Lib is used to load debug symbols).
        AfxSocketInit();
#endif
        m_Socket.Create();
        UnsetClosed();
    }

    void CSocket::Close()
    {
        m_Socket.CancelBlockingCall();
        m_Socket.ShutDown();
        if (m_IsAttached)
        {
            m_Socket.Detach();
            m_IsAttached = false;
        }
        m_Socket.Close();
        SetClosed();
    }

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
            Close();
        return count;
    }

    packet_length_t CSocket::Receive(void* buffer, const packet_length_t byteCount)
    {
        const int count = m_Socket.Receive(buffer, byteCount);
        if (count <= 0)
            Close();
        return count;
    }

    void CSocket::SetClosed()
    {
        if (std::this_thread::get_id() == s_MainThreadId)
        {
            s_ThreadClosed = true;
            s_IsClosed = true;
        }
        else
        {
            s_ThreadClosed = true;
        }
    }

    void CSocket::UnsetClosed()
    {
        if (std::this_thread::get_id() == s_MainThreadId)
        {
            s_ThreadClosed = false;
            s_IsClosed = false;
        }
        else
        {
            s_ThreadClosed = false;
        }
    }
}

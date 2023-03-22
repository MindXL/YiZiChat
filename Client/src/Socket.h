#pragma once

namespace YiZi
{
    class Socket
    {
    public:
        Socket();

        [[nodiscard]] static Socket* Get();

        void Connect(const char* ip, int port);

        socket_t GetConnfd() const { return m_Connfd; }

    private:
        ip_t m_Ip;
        port_t m_Port;

        socket_t m_Connfd;

        static Socket* s_Socket;
    };
}

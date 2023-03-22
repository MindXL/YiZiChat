#include "pch.h"
#include "Socket.h"

namespace YiZi
{
    Socket* Socket::s_Socket = new Socket();

    Socket::Socket()
        : m_Ip{nullptr}, m_Port{0}
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cout << "WSAStartup() failed." << std::endl;
            //return -1;
        }
        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
        {
            WSACleanup();
            //return -1;
        }

        m_Connfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_Connfd == INVALID_SOCKET)
        {
            std::cout << "Connection failed." << std::endl;
            //return -1;
        }
    }

    Socket* Socket::Get()
    {
        return s_Socket;
    }

    void Socket::Connect(const char* ip, const int port)
    {
        m_Ip = ip;
        m_Port = port;

        sockaddr_in server_addr{};
        server_addr.sin_family = PF_INET;
        server_addr.sin_addr.s_addr = inet_addr(m_Ip);
        server_addr.sin_port = htons(m_Port);

        //if (setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof option) < 0)
        //    return EXIT_FAILURE;
        //if (bind(connfd, (sockaddr*)&server_addr, sizeof server_addr) < 0)
        //    return EXIT_FAILURE;
        //if (connect(m_Connfd, (sockaddr*)&server_addr, sizeof server_addr))
        //    return EXIT_FAILURE;
        connect(m_Connfd, (sockaddr*)&server_addr, sizeof server_addr);
    }
}

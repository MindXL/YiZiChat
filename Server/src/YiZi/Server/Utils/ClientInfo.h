#pragma once

#include <string>

#include <YiZi/Server/Core/SSocket.h>

namespace YiZi::Server
{
    struct ClientInfo
    {
        const uint32_t id;
        const std::string phone;
        const std::u16string nickname;
        const uint32_t join_time;
        const bool is_admin;

        SAcceptSocket* const client;
    };
}

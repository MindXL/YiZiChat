#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <memory>

#include "SSocket.h"

namespace YiZi::Server
{
    struct UserInfo
    {
        const std::string phone;
        const std::u16string nickname;
        const uint32_t join_time;
        const bool is_admin;

        const std::shared_ptr<SAcceptSocket>& client;
    };

    class LoginMap final : public std::unordered_map<uint32_t, UserInfo>
    {
    public:
        static LoginMap* Get() { return s_LoginMap; }

    private:
        static LoginMap* s_LoginMap;
    };
}

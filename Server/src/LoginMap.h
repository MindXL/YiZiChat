#pragma once

#include <unordered_map>

#include "../utils/ClientInfo.h"

namespace YiZi::Server
{
    class LoginMap final : public std::unordered_map<uint32_t, ClientInfo>
    {
    public:
        static LoginMap* Get() { return s_LoginMap; }

    private:
        static LoginMap* s_LoginMap;
    };
}

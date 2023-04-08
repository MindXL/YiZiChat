#pragma once

#include <memory>
#include <unordered_map>

#include "SSocket.h"
#include "../utils/ClientInfo.h"

namespace YiZi::Server
{
    class LoginMap final : public std::unordered_map<std::shared_ptr<SAcceptSocket>, ClientInfo>
    {
    public:
        static LoginMap* Get() { return s_LoginMap; }

    private:
        static LoginMap* s_LoginMap;
    };
}

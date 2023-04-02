#pragma once

#include <cstdint>

namespace YiZi
{
    typedef const char* ip_t;

#if defined(YZ_CLIENT)
    typedef unsigned int port_t;
    typedef unsigned long long socket_t;
#elif defined(YZ_SERVER)
    typedef uint16_t port_t;
    typedef int socket_t;
#else
    // TODO: "#error" will not compile on ubuntu (wsl).
    //#error "macro missing"
#endif
}

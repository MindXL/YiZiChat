#pragma once

#include <cstdint>
#include <stddef.h>

namespace YiZi
{
#if defined(YZ_CLIENT)
    typedef LPCTSTR ip_t;
    typedef unsigned int port_t;
    typedef CSocket socket_t;
    typedef int packet_length_t;
#elif defined(YZ_SERVER)
    typedef const char* ip_t;
    typedef uint16_t port_t;
    typedef int socket_t;
    typedef size_t packet_length_t;
#else
    // TODO: "#error" will not compile on ubuntu (wsl).
    //#error "macro missing"
#endif
}

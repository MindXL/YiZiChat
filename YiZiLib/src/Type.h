#pragma once

namespace YiZi
{
    typedef const char* ip_t;
    typedef unsigned int port_t;
#if defined(YZ_CLIENT)
    typedef unsigned long long socket_t;
#elif defined(YZ_SERVER)
    typedef int socket_t;
#else
    //#error macro missing
#endif
}

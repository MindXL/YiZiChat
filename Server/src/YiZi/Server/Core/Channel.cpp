#include "Channel.h"

namespace YiZi::Server
{
    ChannelMap* ChannelMap::s_ChannelMap = new ChannelMap{};

    ChannelConnectionMap* ChannelConnectionMap::s_ChannelConnectionMap = new ChannelConnectionMap{};
}

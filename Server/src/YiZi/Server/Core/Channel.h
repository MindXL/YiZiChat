#pragma once

#include <YiZi/YiZi.h>
#include <string>

namespace YiZi::Server
{
    using channel_string_t = std::u16string;

    using Channel = ChannelBase<channel_string_t>;

    class ChannelMap : public ChannelMapBase<channel_string_t>
    {
    public:
        [[nodiscard]] static ChannelMap* Get() { return s_ChannelMap; }

    private:
        static ChannelMap* s_ChannelMap;

    private:
        ChannelMap() = default;
    };

    class ChannelConnectionMap : public ChannelConnectionMapBase
    {
    public:
        [[nodiscard]] static ChannelConnectionMap* Get() { return s_ChannelConnectionMap; }

    private:
        static ChannelConnectionMap* s_ChannelConnectionMap;

    private:
        ChannelConnectionMap() = default;
    };
}

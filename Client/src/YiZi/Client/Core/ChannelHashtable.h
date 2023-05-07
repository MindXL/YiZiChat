#pragma once
namespace YiZi::Client
{
    class ChannelHashtable final : public std::map<uint32_t, Channel>
    {
    public:
        static ChannelHashtable* Get() { return s_ChannelHashtable; }

    private:
        ChannelHashtable() = default;
        static ChannelHashtable* s_ChannelHashtable;
    };
}

#pragma once

#include <unordered_map>
#include <map>
#include <unordered_set>

namespace YiZi
{
    template<typename string_t>
    class ChannelBase
    {
    public:
        ChannelBase(const uint32_t id, string_t name, const uint64_t join_time, string_t description)
            : m_Id{id}, m_Name{std::move(name)}, m_JoinTime{join_time}, m_Description{std::move(description)} {}

        [[nodiscard]] uint32_t GetId() const { return m_Id; }
        [[nodiscard]] const string_t& GetName() const { return m_Name; }
        [[nodiscard]] uint64_t GetJoinTime() const { return m_JoinTime; }
        [[nodiscard]] const string_t& GetDescription() const { return m_Description; }

    private:
        const uint32_t m_Id;
        const string_t m_Name;
        const uint64_t m_JoinTime;
        const string_t m_Description;
    };

    // <channel id, channel info>
    template<typename string_t>
    using ChannelMapBase = std::map<uint32_t, ChannelBase<string_t>>;

    // <channel id, logged-in user's ids>
    using ChannelConnectionMapBase = std::unordered_map<uint32_t, std::unordered_set<uint32_t>>;
}

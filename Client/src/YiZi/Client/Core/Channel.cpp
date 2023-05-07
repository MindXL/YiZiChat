#include "pch.h"
#include "Channel.h"

namespace YiZi::Client
{
    Channel* Channel::s_CurrentChannel = nullptr;

    Channel::Channel(const uint32_t id, CString name, const uint64_t join_time, CString description)
        : m_Id{id}, m_Name{std::move(name)}, m_JoinTime{join_time}, m_Description{std::move(description)} {}

    void Channel::NewCurrentChannel(Channel&& channel)
    {
#ifdef YZ_DEBUG
        // Control this strictly in debug mode.
        if (s_CurrentChannel != nullptr)
            __debugbreak();
#endif

        s_CurrentChannel = new Channel{std::move(channel)};
    }

    void Channel::DeleteCurrentChannel()
    {
#ifdef YZ_DEBUG
        // Control this strictly in debug mode.
        if (s_CurrentChannel == nullptr)
            __debugbreak();
#endif

        delete s_CurrentChannel;
        s_CurrentChannel = nullptr;
    }
}

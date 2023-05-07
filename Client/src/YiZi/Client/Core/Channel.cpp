#include "pch.h"
#include "Channel.h"

namespace YiZi::Client
{
    Channel* Channel::s_CurrentChannel = nullptr;

    Channel::Channel(const uint32_t id, CString name, const uint64_t join_time, CString description)
        : m_Id{id}, m_Name{std::move(name)}, m_JoinTime{join_time}, m_Description{std::move(description)} {}
}

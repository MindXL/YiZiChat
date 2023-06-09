#include "BufferManager.h"

#include <YiZi/YiZi.h>

namespace YiZi::Server
{
    BufferManager* BufferManager::s_BufferManager = new BufferManager();

    std::pair<uint8_t* const, uint8_t* const> BufferManager::Fetch()
    {
        uint8_t* buffer = nullptr;

        if (m_SpareBuffers.empty())
        {
            buffer = new uint8_t[Packet::REQUEST_MAX_LENGTH() + Packet::RESPONSE_MAX_LENGTH()];
        }
        else
        {
            buffer = m_SpareBuffers.front();
            m_SpareBuffers.pop_front();
        }

        return {buffer, buffer + Packet::REQUEST_MAX_LENGTH()};
    }

    void BufferManager::Free(const int count)
    {
        for (int _ = 0; _ < count; _++)
        {
            const uint8_t* const buffer = m_SpareBuffers.front();
            m_SpareBuffers.pop_front();
            delete[] buffer;
        }
    }
}

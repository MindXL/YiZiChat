#pragma once

#include <cstdint>
#include <list>

namespace YiZi::Server
{
    class BufferManager
    {
    public:
        [[nodiscard]] std::pair<uint8_t* const, uint8_t* const> Fetch();
        void Return(uint8_t* reqBuffer);

        void Free(int count);

        static BufferManager* Get() { return s_BufferManager; }

    private:
        std::list<uint8_t*> m_SpareBuffers;

        static BufferManager* s_BufferManager;
    };
}

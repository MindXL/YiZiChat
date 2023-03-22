#include "pch.h"
#include "Buffer.h"

namespace YiZi
{
    Buffer* Buffer::s_Buffer = new Buffer();

    Buffer::Buffer()
        : m_ReqBuffer(new uint8_t[Packet::REQUEST_MAX_LENGTH()]), m_ResBuffer(new uint8_t[Packet::RESPONSE_MAX_LENGTH()]) {}

    Buffer::~Buffer()
    {
        delete[] m_ResBuffer;
        delete[] m_ReqBuffer;
    }

    Buffer* Buffer::Get()
    {
        return s_Buffer;
    }

    std::pair<uint8_t* const, uint8_t* const> Buffer::GetBuffer()
    {
        return {m_ReqBuffer, m_ResBuffer};
    }

    uint8_t* Buffer::GetReqBuffer() const
    {
        return m_ReqBuffer;
    }

    uint8_t* Buffer::GetResBuffer() const
    {
        return m_ResBuffer;
    }
}

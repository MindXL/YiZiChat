#include "pch.h"
#include "Buffer.h"

namespace YiZi::Client
{
    Buffer* Buffer::s_Buffer = new Buffer();

    Buffer::Buffer()
        : m_ReqBuffer{new uint8_t[Packet::REQUEST_MAX_LENGTH()]}, m_ResBuffer{new uint8_t[Packet::RESPONSE_MAX_LENGTH()]} {}

    Buffer::~Buffer()
    {
        delete[] m_ResBuffer;
        delete[] m_ReqBuffer;
    }
}

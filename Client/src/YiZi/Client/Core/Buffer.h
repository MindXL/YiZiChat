#pragma once

namespace YiZi::Client
{
    class Buffer
    {
    public:
        Buffer();

        ~Buffer();

        [[nodiscard]] static Buffer* Get() { return s_Buffer; }

        [[nodiscard]] std::pair<uint8_t* const, uint8_t* const> GetBuffer() { return {m_ReqBuffer, m_ResBuffer}; }
        [[nodiscard]] uint8_t* GetReqBuffer() const { return m_ReqBuffer; }
        [[nodiscard]] uint8_t* GetResBuffer() const { return m_ResBuffer; }

    private:
        uint8_t* m_ReqBuffer;
        uint8_t* m_ResBuffer;

        static Buffer* s_Buffer;
    };
}

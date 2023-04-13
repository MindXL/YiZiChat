#pragma once

namespace YiZi::Client
{
    class Buffer
    {
    public:
        Buffer();
        ~Buffer();

        [[nodiscard]] static Buffer* Get();

        [[nodiscard]] std::pair<uint8_t* const, uint8_t* const> GetBuffer();
        [[nodiscard]] uint8_t* GetReqBuffer() const;
        [[nodiscard]] uint8_t* GetResBuffer() const;

    private:
        uint8_t* m_ReqBuffer;
        uint8_t* m_ResBuffer;

        static Buffer* s_Buffer;
    };
}

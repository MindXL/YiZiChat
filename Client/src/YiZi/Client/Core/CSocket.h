#pragma once

namespace YiZi::Client
{
    // Client-End YiZi::Socket implementation.
    class CSocket final : public Socket
    {
    public:
        explicit CSocket(const Socket* other);

        void Initialize() override;
        void Close() override;

        bool Connect(ip_t ip, port_t port) override;
        bool Listen(ip_t ip, port_t port) override { throw std::logic_error{"Listen() is not supported in Client-End Socket"}; }
        bool Accept() override { throw std::logic_error{"Accept() is not supported in Client-End Socket"}; }

        int Send(const void* buffer, packet_length_t byteCount) override;
        packet_length_t Receive(void* buffer, packet_length_t byteCount) override;

        [[nodiscard]] static Socket* Get() { return s_CSocket; }

        [[nodiscard]] bool IsClosed() const override { return s_IsClosed && s_ThreadClosed; }

    private:
        CSocket() = default;

        void SetClosed() override;
        void UnsetClosed() override;

    private:
        static std::atomic<bool> s_IsClosed;
        static std::atomic<bool> s_ThreadClosed;
        static const std::thread::id s_MainThreadId;

        static Socket* const s_CSocket;
    };
}

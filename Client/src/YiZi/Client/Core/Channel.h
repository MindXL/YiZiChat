#pragma once

namespace YiZi::Client
{
    class Channel
    {
    public:
        Channel(uint32_t id, CString name, uint64_t join_time, CString description);

        [[nodiscard]] uint32_t GetId() const { return m_Id; }
        [[nodiscard]] const CString& GetName() const { return m_Name; }
        [[nodiscard]] uint64_t GetJoinTime() const { return m_JoinTime; }
        [[nodiscard]] const CString& GetDescription() const { return m_Description; }

        static void NewCurrentChannel(Channel&& channel);
        [[nodiscard]] static const Channel* GetCurrentChannel() { return s_CurrentChannel; }
        static void DeleteCurrentChannel();

    private:
        const uint32_t m_Id;
        const CString m_Name;
        const uint64_t m_JoinTime;
        const CString m_Description;

        static Channel* s_CurrentChannel;
    };
}

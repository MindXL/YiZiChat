#pragma once

namespace YiZi::Client
{
    class User
    {
    public:
        User(uint32_t id, CStringA phone, CString nickname, uint32_t join_time, bool is_admin);

        [[nodiscard]] uint32_t GetId() const { return m_Id; }
        [[nodiscard]] const CStringA& GetPhone() const { return m_Phone; }
        [[nodiscard]] const CString& GetNickname() const { return m_Nickname; }
        [[nodiscard]] uint32_t GetJoinTime() const { return m_JoinTime; }
        [[nodiscard]] bool GetIsAdmin() const { return m_IsAdmin; }

        static bool IsValid() { return s_User != nullptr; }
        static User* New(uint32_t id, CStringA phone, CString nickname, uint32_t join_time, bool is_admin);
        [[nodiscard]] static User* Get() { return s_User; }
        static void Delete()
        {
            if (IsValid())
                delete s_User;
        }

    private:
        const uint32_t m_Id;
        const CStringA m_Phone;
        const CString m_Nickname;
        const uint32_t m_JoinTime;
        const bool m_IsAdmin;

        static User* s_User;
    };
}

#include "pch.h"
#include "User.h"

namespace YiZi::Client
{
    User* User::s_User = nullptr;

    User::User(const uint32_t id, CStringA phone, CString nickname, const uint64_t join_time, const bool is_admin)
        : m_Id{id}, m_Phone{std::move(phone)}, m_Nickname{std::move(nickname)}, m_JoinTime{join_time}, m_IsAdmin{is_admin} {}

    User* User::New(const uint32_t id, CStringA phone, CString nickname, const uint64_t join_time, const bool is_admin)
    {
        Delete();
        s_User = new User{id, std::move(phone), std::move(nickname), join_time, is_admin};
        return s_User;
    }

    void User::Delete()
    {
        if (IsValid())
        {
            delete s_User;
            s_User = nullptr;
        }
    }
}

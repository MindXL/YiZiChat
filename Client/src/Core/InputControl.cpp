#include "pch.h"

#include "InputControl.h"

namespace YiZi::Client
{
    bool InputControl::User_Phone(const CStringA& phone)
    {
        const int length = phone.GetLength();
        if (length == 0)
        {
            AfxMessageBox(_T("请填入手机号"));
            return false;
        }
        if (length != Database::User::ItemLength::PHONE_LENGTH)
        {
            AfxMessageBox(_T("手机号的长度必须为11位。"));
            return false;
        }
        if (!std::regex_match(phone.GetString(), std::regex(R"(1[3456789]\d{9})")))
        {
            AfxMessageBox(_T("无效手机号。"));
            return false;
        }
        return true;
    }

    bool InputControl::User_Password(const CStringA& password)
    {
        const int length = password.GetLength();
        if (length == 0)
        {
            AfxMessageBox(_T("请填入密码"));
            return false;
        }
        if (length < Database::User::ItemLength::PASSWORD_MIN_LENGTH || length > Database::User::ItemLength::PASSWORD_MAX_LENGTH)
        {
            CString hint;
            hint.AppendFormat(
                _T("密码长度应处于%d位至%d位之间！当前长度为%d位。"),
                Database::User::ItemLength::PASSWORD_MIN_LENGTH,
                Database::User::ItemLength::PASSWORD_MAX_LENGTH,
                length
            );
            AfxMessageBox(hint);
            return false;
        }
        if (!std::regex_match(password.GetString(), std::regex(R"([-\w]{8,20})")))
        {
            AfxMessageBox(_T("密码中存在非法字符。"));
            return false;
        }
        return true;
    }

    bool InputControl::User_Nickname(const CString& nickname)
    {
        const int length = nickname.GetLength();
        if (length == 0)
        {
            AfxMessageBox(_T("请填入昵称"));
            return false;
        }
        if (length < Database::User::ItemLength::NICKNAME_MIN_LENGTH || length > Database::User::ItemLength::NICKNAME_MAX_LENGTH)
        {
            CString hint;
            hint.AppendFormat(
                _T("昵称长度应处于%d位至%d位之间！当前长度为%d位。"),
                Database::User::ItemLength::NICKNAME_MIN_LENGTH,
                Database::User::ItemLength::NICKNAME_MAX_LENGTH,
                length
            );
            AfxMessageBox(hint);
            return false;
        }
        return true;
    }
}

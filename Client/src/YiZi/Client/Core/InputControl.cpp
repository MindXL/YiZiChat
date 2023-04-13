#include "pch.h"

#include "InputControl.h"

namespace YiZi::Client
{
    bool InputControl::User_Phone(const CStringA& phone)
    {
        const int length = phone.GetLength();
        if (length == 0)
        {
            AfxMessageBox(_T("�������ֻ���"));
            return false;
        }
        if (length != Database::User::ItemLength::PHONE_LENGTH)
        {
            AfxMessageBox(_T("�ֻ��ŵĳ��ȱ���Ϊ11λ��"));
            return false;
        }
        if (!std::regex_match(phone.GetString(), std::regex(R"(1[3456789]\d{9})")))
        {
            AfxMessageBox(_T("��Ч�ֻ��š�"));
            return false;
        }
        return true;
    }

    bool InputControl::User_Password(const CStringA& password)
    {
        const int length = password.GetLength();
        if (length == 0)
        {
            AfxMessageBox(_T("����������"));
            return false;
        }
        if (length < Database::User::ItemLength::PASSWORD_MIN_LENGTH || length > Database::User::ItemLength::PASSWORD_MAX_LENGTH)
        {
            CString hint;
            hint.AppendFormat(
                _T("���볤��Ӧ����%dλ��%dλ֮�䣡��ǰ����Ϊ%dλ��"),
                Database::User::ItemLength::PASSWORD_MIN_LENGTH,
                Database::User::ItemLength::PASSWORD_MAX_LENGTH,
                length
            );
            AfxMessageBox(hint);
            return false;
        }
        if (!std::regex_match(password.GetString(), std::regex(R"([-\w]{8,20})")))
        {
            AfxMessageBox(_T("�����д��ڷǷ��ַ���"));
            return false;
        }
        return true;
    }

    bool InputControl::User_Nickname(const CString& nickname)
    {
        const int length = nickname.GetLength();
        if (length == 0)
        {
            AfxMessageBox(_T("�������ǳ�"));
            return false;
        }
        if (length < Database::User::ItemLength::NICKNAME_MIN_LENGTH || length > Database::User::ItemLength::NICKNAME_MAX_LENGTH)
        {
            CString hint;
            hint.AppendFormat(
                _T("�ǳƳ���Ӧ����%dλ��%dλ֮�䣡��ǰ����Ϊ%dλ��"),
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

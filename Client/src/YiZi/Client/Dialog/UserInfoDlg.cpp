// UserInfoDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "UserInfoDlg.h"

// CUserInfoDlg 对话框

IMPLEMENT_DYNAMIC(CUserInfoDlg, CDialogEx)

CUserInfoDlg::CUserInfoDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_USER_INFO_DIALOG, pParent)
      , m_csPhone(_T(""))
      , m_csJoinTime(_T("")) {}

void CUserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PHONE, m_csPhone);
    DDV_MaxChars(pDX, m_csPhone, YiZi::Database::User::ItemLength::PHONE_LENGTH);
    DDX_Control(pDX, IDC_EDIT_PASSWORD, m_cePassword);
    DDX_Control(pDX, IDC_EDIT_NICKNAME, m_ceNickname);
    DDX_Text(pDX, IDC_EDIT_JOIN_TIME, m_csJoinTime);
    DDV_MaxChars(pDX, m_csJoinTime, 19);
}

BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_CHANGE_PASSWORD, &CUserInfoDlg::OnBnClickedButtonChangePassword)
    ON_BN_CLICKED(IDC_BUTTON_CHANGE_NICKNAME, &CUserInfoDlg::OnBnClickedButtonChangeNickname)
END_MESSAGE_MAP()

bool CUserInfoDlg::HandleChangeUserPasswordRequest()
{
    // TODO: Apply for a new buffer.
    uint8_t* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();
    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHANGE_USER_PASSWORD_REQUEST_LENGTH;

    auto* const request_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(reqBuffer);
    request_header->type = (uint8_t)YiZi::Packet::PacketType::ChangeUserPasswordRequest;

    auto* const request_data = reinterpret_cast<YiZi::Packet::ChangeUserPasswordRequest*>(reqBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);
    memcpy_s(request_data->password, YiZi::Database::User::ItemLength::PASSWORD_MAX_LENGTH,
             m_csaNewPassword.GetString(), m_csaNewPassword.GetLength());
    request_data->password[m_csaNewPassword.GetLength()] = '\0';

    const bool success = YiZi::Client::CSocket::Get()->Send(reqBuffer, request_len);
    return success;
}

bool CUserInfoDlg::HandleChangeUserPasswordResponse()
{
    uint8_t* const resBuffer = YiZi::Client::Buffer::Get()->GetResBuffer();
    constexpr int response_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHANGE_USER_PASSWORD_RESPONSE_LENGTH;

    const auto* const response_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(resBuffer);
    auto* const g_socket = YiZi::Client::CSocket::Get();
    while (true)
    {
        g_socket->Receive(resBuffer, response_len,MSG_PEEK);
        if (response_header->type != (uint8_t)YiZi::Packet::PacketType::ChangeUserPasswordResponse)
        {
            std::this_thread::yield();
            continue;
        }
        g_socket->Receive(resBuffer, response_len);
        break;
    }

    const auto* const response_data = reinterpret_cast<YiZi::Packet::ChangeUserPasswordResponse*>(resBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    if (response_data->success)
    {
        AfxMessageBox(_T("修改成功。"));
        return true;
    }

    /* If change failed */
    switch (YiZi::Packet::ChangeUserPasswordFailReason{response_data->reason})
    {
    case YiZi::Packet::ChangeUserPasswordFailReason::PasswordIsIdentical:
        {
            AfxMessageBox(_T("修改失败，密码不能与原密码相同。"));
            break;
        }
    default: AfxMessageBox(_T("修改失败，请重试。"));
    }
    return false;
}

bool CUserInfoDlg::HandleChangeUserNicknameRequest()
{
    uint8_t* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();
    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHANGE_USER_NICKNAME_REQUEST_LENGTH;

    auto* const request_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(reqBuffer);
    request_header->type = (uint8_t)YiZi::Packet::PacketType::ChangeUserNicknameRequest;

    auto* const request_data = reinterpret_cast<YiZi::Packet::ChangeUserNicknameRequest*>(reqBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);
    memcpy_s(request_data->nickname, YiZi::Database::User::ItemLength::NICKNAME_MAX_LENGTH * sizeof(wchar_t),
             m_csNewNickname.GetString(), m_csNewNickname.GetLength() * sizeof(wchar_t));
    *((wchar_t*)request_data->nickname + m_csNewNickname.GetLength()) = _T('\0');

    const bool success = YiZi::Client::CSocket::Get()->Send(reqBuffer, request_len);
    return success;
}

bool CUserInfoDlg::HandleChangeUserNicknameResponse()
{
    uint8_t* const resBuffer = YiZi::Client::Buffer::Get()->GetResBuffer();
    constexpr int response_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHANGE_USER_NICKNAME_RESPONSE_LENGTH;

    const auto* const response_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(resBuffer);
    auto* const g_socket = YiZi::Client::CSocket::Get();
    while (true)
    {
        g_socket->Receive(resBuffer, response_len, MSG_PEEK);
        if (response_header->type != (uint8_t)YiZi::Packet::PacketType::ChangeUserNicknameResponse)
        {
            std::this_thread::yield();
            continue;
        }
        g_socket->Receive(resBuffer, response_len);
        break;
    }

    const auto* const response_data = reinterpret_cast<YiZi::Packet::ChangeUserNicknameResponse*>(resBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    if (response_data->success)
    {
        AfxMessageBox(_T("修改成功。"));
        return true;
    }

    /* If change failed */
    switch (YiZi::Packet::ChangeUserNicknameFailReason{response_data->reason})
    {
    case YiZi::Packet::ChangeUserNicknameFailReason::NicknameAlreadyExists:
        {
            AfxMessageBox(_T("昵称已存在。"));
            break;
        }
    default: AfxMessageBox(_T("修改失败，请重试。"));
    }
    return false;
}

// CUserInfoDlg 消息处理程序

BOOL CUserInfoDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    const auto* const user = YiZi::Client::User::Get();
    if (!user->IsValid())
        throw std::logic_error{"[CUserInfoDlg]: User instance is not initialized. There could be a logic error in coding."};

    m_csPhone = YiZi::Client::User::Get()->GetPhone();
    m_cePassword.SetWindowTextW(s_csPasswordPlaceholder);
    m_ceNickname.SetWindowTextW(user->GetNickname());
    m_csJoinTime = CTime{static_cast<__time64_t>(YiZi::Client::User::Get()->GetJoinTime() / 1000)}.Format(_T("%F %T"));
    UpdateData(false);

    return TRUE; // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

const CString CUserInfoDlg::s_csPasswordPlaceholder{_T("\001\001\001\001\001\001\001\001\001\001")};

void CUserInfoDlg::OnBnClickedButtonChangePassword()
{
    /* If user click for the first time. */
    if (!m_bIsChangingPassword)
    {
        m_bIsChangingPassword = true;

        m_cePassword.SetWindowTextW(_T(""));
        m_cePassword.EnableWindow(true);
        m_cePassword.SetReadOnly(false);
        m_cePassword.SetFocus();

        return;
    }

    m_cePassword.SetReadOnly(true);
    m_cePassword.EnableWindow(false);

    /* If user click for the second time. */
    m_bIsChangingPassword = false;

    auto recover = [&]
    {
        m_cePassword.SetWindowTextW(s_csPasswordPlaceholder);
        m_csaNewPassword.Empty();
    };

    // If user didn't change the text.
    if (!m_cePassword.GetModify())
    {
        recover();
        return;
    }

    if (AfxMessageBox(_T("是否确认修改密码"), MB_YESNO) == IDNO)
    {
        recover();
        return;
    }

    CString csPassword;
    m_cePassword.GetWindowText(csPassword);
    m_csaNewPassword = csPassword;

    if (!YiZi::Client::InputControl::User_Password(m_csaNewPassword))
    {
        recover();
        return;
    }

    if (!HandleChangeUserPasswordRequest())
    {
        AfxMessageBox(_T("修改失败，请重试。"));
        recover();
        return;
    }

    if (!HandleChangeUserPasswordResponse())
        recover();
}

void CUserInfoDlg::OnBnClickedButtonChangeNickname()
{
    /* If user click for the first time. */
    if (!m_bIsChangingNickname)
    {
        m_bIsChangingNickname = true;

        m_ceNickname.EnableWindow(true);
        m_ceNickname.SetReadOnly(false);
        m_ceNickname.SetFocus();

        return;
    }

    /* If user click for the second time. */
    m_ceNickname.SetReadOnly(true);
    m_ceNickname.EnableWindow(false);

    m_bIsChangingNickname = false;

    const CString oldNickname = YiZi::Client::User::Get()->GetNickname();
    auto recover = [&]
    {
        m_ceNickname.SetWindowTextW(oldNickname);
        m_csNewNickname.Empty();
    };

    // If user didn't change the text.
    if (!m_ceNickname.GetModify())
    {
        recover();
        return;
    }

    if (AfxMessageBox(_T("是否确认修改昵称"), MB_YESNO) == IDNO)
    {
        recover();
        return;
    }

    m_ceNickname.GetWindowText(m_csNewNickname);

    if (m_csNewNickname == oldNickname)
    {
        AfxMessageBox(_T("新昵称不能与原昵称相同。"));
        recover();
        return;
    }

    if (!YiZi::Client::InputControl::User_Nickname(m_csNewNickname))
    {
        recover();
        return;
    }

    if (!HandleChangeUserNicknameRequest())
    {
        AfxMessageBox(_T("修改失败，请重试。"));
        recover();
        return;
    }

    if (!HandleChangeUserNicknameResponse())
    {
        recover();
        return;
    }

    YiZi::Client::User::Get()->SetNickname(m_csNewNickname);
}

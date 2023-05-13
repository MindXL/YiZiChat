// LoginDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "LoginDlg.h"

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_LOGIN_DIALOG, pParent),
      m_csPhone(_T("")),
      m_csPassword(_T("")) {}

CLoginDlg::~CLoginDlg() {}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_txtPhone, m_csPhone);
    DDV_MaxChars(pDX, m_csPhone, YiZi::Database::User::ItemLength::PHONE_LENGTH);
    DDX_Text(pDX, IDC_txtPassword, m_csPassword);
    DDV_MaxChars(pDX, m_csPassword, YiZi::Database::User::ItemLength::PASSWORD_MAX_LENGTH);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CLoginDlg 消息处理程序

void CLoginDlg::OnBnClickedOk()
{
    auto* const g_Socket = YiZi::Client::CSocket::Get();
#ifdef YZ_DEBUG
    // Socket must be closed before CLoginDlg pops up or before login process starts.
    if (!g_Socket->IsClosed())
        __debugbreak();
#endif

    if (!HandleLoginRequest() || !HandleLoginResponse())
    {
        g_Socket->Close();
        return;
    }

    CString message{ _T("登录成功。\n欢迎你：") };
    message.Append(YiZi::Client::User::Get()->GetNickname());
    AfxMessageBox(message);

    CDialogEx::OnOK();
}

bool CLoginDlg::HandleLoginRequest()
{
    UpdateData(true);
    const CStringA phone{m_csPhone};
    if (!YiZi::Client::InputControl::User_Phone(phone))
    {
        GetDlgItem(IDC_txtPhone)->GetFocus();
        return false;
    }

    const CStringA password{m_csPassword};
    if (!YiZi::Client::InputControl::User_Password(password))
    {
        GetDlgItem(IDC_txtPassword)->GetFocus();
        return false;
    }

    uint8_t* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();
    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::LOGIN_REQUEST_LENGTH;

    auto* const request_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(reqBuffer);
    request_header->type = (uint8_t)YiZi::Packet::PacketType::LoginRequest;

    auto* const request_data = reinterpret_cast<YiZi::Packet::LoginRequest*>(reqBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);
    memcpy_s(request_data->phone, YiZi::Database::User::ItemLength::PHONE_LENGTH,
             phone.GetString(), phone.GetLength());
    request_data->phone[YiZi::Database::User::ItemLength::PHONE_LENGTH] = '\0';
    memcpy_s(request_data->password, YiZi::Database::User::ItemLength::PASSWORD_MAX_LENGTH,
             password.GetString(), password.GetLength());
    request_data->password[password.GetLength()] = '\0';

    if (auto* const socket = YiZi::Client::CSocket::Get();
        !socket->Connect(_T("127.0.0.1"), 5000) || !socket->Send(reqBuffer, request_len))
    {
        AfxMessageBox(_T("连接服务器失败。请检查网络。"));
        return false;
    }

    return true;
}

bool CLoginDlg::HandleLoginResponse()
{
    uint8_t* const resBuffer = YiZi::Client::Buffer::Get()->GetResBuffer();
    constexpr int response_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::LOGIN_RESPONSE_LENGTH;

    YiZi::Client::CSocket::Get()->Receive(resBuffer, response_len);

    const auto* const response_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(resBuffer);
    if (response_header->type != (uint8_t)YiZi::Packet::PacketType::LoginResponse)
    {
        AfxMessageBox(_T("服务器错误。请重试。"));
        return false;
    }

    const auto* const response_data = reinterpret_cast<YiZi::Packet::LoginResponse*>(resBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);
    if (!static_cast<bool>(response_data->isValid))
    {
        switch (static_cast<YiZi::Packet::LoginFailReason>(response_data->reason))
        {
        case YiZi::Packet::LoginFailReason::UserNotExist:
            {
                AfxMessageBox(_T("账号不存在。\n请检查手机号，或联系管理员以注册。"));
                break;
            }
        case YiZi::Packet::LoginFailReason::UserPasswordIncorrect:
            {
                AfxMessageBox(_T("密码错误。请检查密码。"));
                break;
            }
        case YiZi::Packet::LoginFailReason::UserAlreadyLoggedIn:
            {
                AfxMessageBox(_T("该用户已登录。"));
                break;
            }
#ifdef YZ_DEBUG
        default:
            {
                // Wrong type was passed in response since all cases of "YiZi::Packet::LoginFailReason" are handled above.
                // There is something wrong in server-end code.
                __debugbreak();
            }
#endif
        }
        m_csPassword.Empty();
        UpdateData(false);
        return false;
    }

    YiZi::Client::User::New(response_data->id,
                            std::move(CStringA{m_csPhone}),
                            std::move(CString{(const wchar_t*)response_data->nickname, YiZi::Database::User::ItemLength::NICKNAME_MAX_LENGTH}),
                            response_data->join_time,
                            static_cast<bool>(response_data->isAdmin));

    return true;
}

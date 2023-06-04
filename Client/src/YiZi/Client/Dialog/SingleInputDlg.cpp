// SingleInputDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "SingleInputDlg.h"

// CSingleInputDlg 对话框

IMPLEMENT_DYNAMIC(CSingleInputDlg, CDialogEx)

CSingleInputDlg::CSingleInputDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_SINGLE_INPUT_DIALOG, pParent) {}

void CSingleInputDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SINGLE_INPUT, m_ceSingleInput);
}

BEGIN_MESSAGE_MAP(CSingleInputDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CSingleInputDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CSingleInputDlg::SetUsage(const Usage usage)
{
#ifdef YZ_DEBUG
    // Cannot set usage as None.
    if (usage == Usage::None)
        __debugbreak();
#endif

    m_uUsage = usage;
}

bool CSingleInputDlg::HandleValidateAdminRequest() const
{
    uint8_t* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();
    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::VALIDATE_ADMIN_REQUEST_LENGTH;

    auto* const request_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(reqBuffer);
    request_header->type = (uint8_t)YiZi::Packet::PacketType::ValidateAdminRequest;

    auto* const request_data = reinterpret_cast<YiZi::Packet::ValidateAdminRequest*>(reqBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    CString csPassword;
    m_ceSingleInput.GetWindowTextW(csPassword);
    const CStringA password{csPassword};

    memcpy_s(request_data->password, YiZi::Database::User::ItemLength::PASSWORD_MAX_LENGTH,
             password.GetString(), password.GetLength());
    request_data->password[password.GetLength()] = '\0';

    const bool success = YiZi::Client::CSocket::Get()->Send(reqBuffer, request_len);
    if (!success)
        AfxMessageBox(_T("链接失败。请重新启动软件。"));
    return success;
}

bool CSingleInputDlg::HandleValidateAdminResponse() const
{
    uint8_t* const resBuffer = YiZi::Client::Buffer::Get()->GetResBuffer();
    constexpr int response_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::VALIDATE_ADMIN_RESPONSE_LENGTH;

    const auto* const response_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(resBuffer);
    auto* const g_socket = YiZi::Client::CSocket::Get();
    while (true)
    {
        g_socket->Receive(resBuffer, response_len, MSG_PEEK);
        if (response_header->type != (uint8_t)YiZi::Packet::PacketType::ValidateAdminResponse)
        {
            std::this_thread::yield();
            continue;
        }
        g_socket->Receive(resBuffer, response_len);
        break;
    }

    const auto* const response_data = reinterpret_cast<YiZi::Packet::ValidateAdminResponse*>(resBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    if (response_data->success)
    {
        AfxMessageBox(_T("验证成功。"));
        return true;
    }

    /* If validation failed */
    switch (YiZi::Packet::ValidateAdminFailReason{response_data->reason})
    {
    case YiZi::Packet::ValidateAdminFailReason::UserPasswordIncorrect:
        {
            AfxMessageBox(_T("密码错误。"));
            break;
        }
    default: AfxMessageBox(_T("出现未知错误，请重试。"));
    }
    return false;
}

bool CSingleInputDlg::HandleRegisterUserRequest() const
{
    uint8_t* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();
    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::REGISTER_USER_REQUEST_LENGTH;

    auto* const request_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(reqBuffer);
    request_header->type = (uint8_t)YiZi::Packet::PacketType::RegisterUserRequest;

    auto* const request_data = reinterpret_cast<YiZi::Packet::RegisterUserRequest*>(reqBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    CString csPhone;
    m_ceSingleInput.GetWindowTextW(csPhone);
    const CStringA phone{csPhone};

    memcpy_s(request_data->phone, YiZi::Database::User::ItemLength::PASSWORD_MAX_LENGTH,
             phone.GetString(), phone.GetLength());
    request_data->phone[phone.GetLength()] = '\0';

    const bool success = YiZi::Client::CSocket::Get()->Send(reqBuffer, request_len);
    if (!success)
        AfxMessageBox(_T("链接失败。请重新启动软件。"));
    return success;
}

bool CSingleInputDlg::HandleRegisterUserResponse()
{
    uint8_t* const resBuffer = YiZi::Client::Buffer::Get()->GetResBuffer();
    constexpr int response_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::REGISTER_USER_RESPONSE_LENGTH;

    const auto* const response_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(resBuffer);
    auto* const g_socket = YiZi::Client::CSocket::Get();
    while (true)
    {
        g_socket->Receive(resBuffer, response_len, MSG_PEEK);
        if (response_header->type != (uint8_t)YiZi::Packet::PacketType::RegisterUserResponse)
        {
            std::this_thread::yield();
            continue;
        }
        g_socket->Receive(resBuffer, response_len);
        break;
    }

    const auto* const response_data = reinterpret_cast<YiZi::Packet::RegisterUserResponse*>(resBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    if (response_data->success)
    {
        AfxMessageBox(_T("注册成功。"));
        m_Additional.defaultPassword = response_data->password;
        return true;
    }

    /* If validation failed */
    switch (YiZi::Packet::RegisterUserFailReason{response_data->reason})
    {
    case YiZi::Packet::RegisterUserFailReason::AdminTokenExpired:
        {
            AfxMessageBox(_T("需要重新验证管理员权限。"));
            CDialogEx::OnCancel();
        }
    case YiZi::Packet::RegisterUserFailReason::UserAlreadyExists:
        {
            AfxMessageBox(_T("该账号已存在。"));
            CDialogEx::OnCancel();
        }
    default: AfxMessageBox(_T("出现未知错误，请重试。"));
    }
    return false;
}

// CSingleInputDlg 消息处理程序

BOOL CSingleInputDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    switch (m_uUsage)
    {
#ifdef YZ_DEBUG
    // "usage" must be set before "DoModal()"
    case Usage::None: __debugbreak();
#endif
    case Usage::ValidateAdmin:
        {
            SetWindowTextW(_T("请输入密码 验证管理员身份"));
            m_ceSingleInput.SetPasswordChar('*');
            break;
        }
    case Usage::RegisterUser:
        {
            SetWindowTextW(_T("请输入手机号 为其注册账号"));
            break;
        }
    default: ;
    }

    return TRUE; // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CSingleInputDlg::OnBnClickedOk()
{
    switch (m_uUsage)
    {
#ifdef YZ_DEBUG
    case Usage::None: __debugbreak();
#endif
    case Usage::ValidateAdmin:
        {
            CString csPassword;
            m_ceSingleInput.GetWindowTextW(csPassword);
            if (const CStringA password{csPassword};
                !YiZi::Client::InputControl::User_Password(password))
            {
                GetDlgItem(IDC_EDIT_PASSWORD)->GetFocus();
                m_ceSingleInput.SetWindowTextW(_T(""));
                return;
            }

            if (!HandleValidateAdminRequest())
            {
                m_ceSingleInput.SetWindowTextW(_T(""));
                return;
            }

            if (!HandleValidateAdminResponse())
            {
                m_ceSingleInput.SetWindowTextW(_T(""));
                return;
            }

            EndDialog(YiZi::Client::DialogBoxCommandID::CID_VALIDATE_ADMIN_SUCCESS);
            break;
        }
    case Usage::RegisterUser:
        {
            CString csPhone;
            m_ceSingleInput.GetWindowTextW(csPhone);
            if (const CStringA phone{csPhone};
                !YiZi::Client::InputControl::User_Phone(phone))
            {
                GetDlgItem(IDC_EDIT_PASSWORD)->GetFocus();
                return;
            }

            if (!HandleRegisterUserRequest())
            {
                m_ceSingleInput.SetWindowTextW(_T(""));
                return;
            }

            if (!HandleRegisterUserResponse())
            {
                m_ceSingleInput.SetWindowTextW(_T(""));
                return;
            }

            SetWindowTextW(_T("该账户的默认密码"));
            m_ceSingleInput.SetReadOnly(true);
            m_ceSingleInput.SetWindowTextW(m_Additional.defaultPassword);

            GetDlgItem(IDOK)->EnableWindow(0);

            break;
        }
    default: ;
    }
}

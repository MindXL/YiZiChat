// ValidateAdminDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "ValidateAdminDlg.h"

// CValidateAdminDlg 对话框

IMPLEMENT_DYNAMIC(CValidateAdminDlg, CDialogEx)

CValidateAdminDlg::CValidateAdminDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_VALIDATE_ADMIN_DIALOG, pParent)
      , m_csPassword(_T("")) {}

void CValidateAdminDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_csPassword);
    DDV_MaxChars(pDX, m_csPassword, 20);
}

BEGIN_MESSAGE_MAP(CValidateAdminDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CValidateAdminDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CValidateAdminDlg 消息处理程序

bool CValidateAdminDlg::HandleValidateAdminRequest() const
{
    uint8_t* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();
    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::VALIDATE_ADMIN_REQUEST_LENGTH;

    auto* const request_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(reqBuffer);
    request_header->type = (uint8_t)YiZi::Packet::PacketType::ValidateAdminRequest;

    auto* const request_data = reinterpret_cast<YiZi::Packet::ValidateAdminRequest*>(reqBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);
    const CStringA password{m_csPassword};
    memcpy_s(request_data->password, YiZi::Database::User::ItemLength::PASSWORD_MAX_LENGTH,
             password.GetString(), password.GetLength());
    request_data->password[password.GetLength()] = '\0';

    const bool success = YiZi::Client::CSocket::Get()->Send(reqBuffer, request_len);
    if (!success)
        AfxMessageBox(_T("链接失败。请重新启动软件。"));
    return success;
}

bool CValidateAdminDlg::HandleValidateAdminResponse()
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

void CValidateAdminDlg::OnBnClickedOk()
{
    UpdateData(true);
    if (const CStringA password{m_csPassword};
        !YiZi::Client::InputControl::User_Password(password))
    {
        GetDlgItem(IDC_EDIT_PASSWORD)->GetFocus();
        return;
    }

    if (!HandleValidateAdminRequest())
        return;

    if (!HandleValidateAdminResponse())
        return;

    EndDialog(YiZi::Client::DialogBoxCommandID::CID_VALIDATE_ADMIN_SUCCESS);

    //CDialogEx::OnOK();
}

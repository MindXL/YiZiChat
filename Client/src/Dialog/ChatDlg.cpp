// ChatDlg.cpp: 实现文件
//

#include "pch.h"
#include "../../Client.h"
#include "afxdialogex.h"
#include "ChatDlg.h"

#include "../Dialog/UserInfoDlg.h"

// CChatDlg 对话框

IMPLEMENT_DYNAMIC(CChatDlg, CDialogEx)

CChatDlg::CChatDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHAT_DIALOG, pParent)
      , m_csTranscript(_T(""))
      , m_csMessage(_T(""))
{
    auto* const request_header = (YiZi::Packet::PacketHeader*)m_pChatRequestBuffer;
    request_header->type = (uint8_t)YiZi::Packet::PacketType::ChatMessageRequest;
}

CChatDlg::~CChatDlg() {}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_TRANSCRIPT, m_csTranscript);
    DDX_Text(pDX, IDC_EDIT_MESSAGE, m_csMessage);
    DDV_MaxChars(pDX, m_csMessage, YiZi::Database::Transcript::ItemLength::CONTENT_MAX_LENGTH);
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatDlg::OnBnClickedButtonSend)
    ON_BN_CLICKED(IDC_BUTTON_EMPTY_TRANSCRIPT, &CChatDlg::OnBnClickedButtonEmptyTranscript)
    ON_COMMAND(ID_USER_INFO, &CChatDlg::OnUserInfo)
END_MESSAGE_MAP()

bool CChatDlg::HandleChatMessageRequest() const
{
    auto* const request_data = (YiZi::Packet::ChatMessageRequest*)(m_pChatRequestBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    const int message_len = m_csMessage.GetLength();
    memcpy_s(request_data->content, YiZi::Database::Transcript::ItemLength::CONTENT_MAX_LENGTH,
             m_csMessage.GetString(), message_len * sizeof(wchar_t));
    *((char16_t*)request_data->content + message_len) = u'\0';

    YiZi::Client::CSocket::Get()->Send(m_pChatRequestBuffer, s_iChatRequestBufferLen);

    return true;
}

bool CChatDlg::HandleChatMessageResponse()
{
    // TODO: Handle receiving messages from other clients.
    return true;
}

// CChatDlg 消息处理程序

void CChatDlg::OnBnClickedButtonSend()
{
    UpdateData(true);
    // Pop a tip window when user continues to click the button up to 3 times.
    if (m_csMessage.GetLength() <= 0)
        return;

    if (const bool success = HandleChatMessageRequest();
        !success)
    {
        AfxMessageBox(_T("连接错误。请重新尝试发送。"));
        return;
    }

    m_csTranscript.Append(m_csMessage);
    m_csTranscript.Append(_T("\r\n"));
    m_csMessage.Empty();
    UpdateData(false);
}

void CChatDlg::OnBnClickedButtonEmptyTranscript()
{
    m_csTranscript.Empty();
    UpdateData(false);
}


void CChatDlg::OnUserInfo()
{
    CUserInfoDlg{}.DoModal();
}

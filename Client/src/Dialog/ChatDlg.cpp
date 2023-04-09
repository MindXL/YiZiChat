// ChatDlg.cpp: 实现文件
//

#include "pch.h"
#include "../../Client.h"
#include "afxdialogex.h"
#include "ChatDlg.h"

#include "../Dialog/UserInfoDlg.h"
#include "../Dialog/AboutDlg.h"

#include "../Core/Buffer.h"
#include "../Core/User.h"

// CChatDlg 对话框

IMPLEMENT_DYNAMIC(CChatDlg, CDialogEx)

CChatDlg::CChatDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHAT_DIALOG, pParent)
      , m_csTranscript(_T(""))
      , m_csMessage(_T("")) {}

CChatDlg::~CChatDlg()
{
    delete m_pChatRequestBuffer;
    delete m_pChatResponseBuffer;
}

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
    ON_COMMAND(ID_ABOUT, &CChatDlg::OnAbout)
    ON_COMMAND(ID_LOGOUT, &CChatDlg::OnLogout)
    ON_MESSAGE(WM_RECVDATA, &CChatDlg::OnRecvData)
END_MESSAGE_MAP()

BOOL CChatDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    auto* const request_header = (YiZi::Packet::PacketHeader*)m_pChatRequestBuffer;
    request_header->type = (uint8_t)YiZi::Packet::PacketType::ChatMessageRequest;

    m_tListenChatMessageThread = std::thread{ListenChatMessage, GetSafeHwnd()};
    m_tListenChatMessageThread.detach();

    return TRUE; // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

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

void CChatDlg::HandleChatMessageResponse()
{
    const auto* const response_data = (YiZi::Packet::ChatMessageResponse*)(m_pChatResponseBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    // TODO: Improve output.
    m_csTranscript.Append((const wchar_t*)response_data->content);
    m_csTranscript.Append(_T("\r\n"));
    UpdateData(false);
}

void CChatDlg::HandleLogoutRequest()
{
    auto* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();

    auto* const request_header = (YiZi::Packet::PacketHeader*)reqBuffer;
    request_header->type = (uint8_t)YiZi::Packet::PacketType::LogoutRequest;

    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::LOGOUT_REQUEST_LENGTH;
    YiZi::Client::CSocket::Get()->Send(reqBuffer, request_len);
}

void CChatDlg::ListenChatMessage(const HWND hWnd)
{
    CSocket socket;
    socket.Attach(YiZi::Client::CSocket::Get()->GetSocket());

    const auto* const response_header = (YiZi::Packet::PacketHeader*)m_pChatResponseBuffer;
    while (true)
    {
        if (socket.Receive(m_pChatResponseBuffer, s_iChatResponseBufferLen) == -1)
            break;

        if (response_header->type != (uint8_t)YiZi::Packet::PacketType::ChatMessageResponse)
        {
            std::this_thread::yield();
            continue;
        }

        ::SendMessage(hWnd, WM_RECVDATA, 0, 0);
        std::this_thread::yield();
    }
}

// CChatDlg 消息处理程序

void CChatDlg::OnBnClickedButtonSend()
{
    UpdateData(true);
    // TODO: Pop a tip window when user continues to click the button up to 3 times.
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

void CChatDlg::OnAbout()
{
    CAboutDlg{}.DoModal();
}

void CChatDlg::OnLogout()
{
    if (AfxMessageBox(_T("是否退出应用？"), MB_YESNO) == IDNO)
        return;

    //WSACancelBlockingCall();

    HandleLogoutRequest();

    YiZi::Client::User::Delete();
}

LRESULT CChatDlg::OnRecvData(WPARAM wParam, LPARAM lParam)
{
    HandleChatMessageResponse();
    return 0;
}

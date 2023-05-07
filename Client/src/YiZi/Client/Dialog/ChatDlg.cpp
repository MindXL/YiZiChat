// ChatDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "ChatDlg.h"

#include <YiZi/Client/Dialog/UserInfoDlg.h>
#include <YiZi/Client/Dialog/AboutDlg.h>

// CChatDlg 对话框

IMPLEMENT_DYNAMIC(CChatDlg, CDialogEx)

CChatDlg::CChatDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHAT_DIALOG, pParent)
      , m_csTranscript(_T(""))
      , m_csMessage(_T(""))
      , m_csLocalUserNickname{YiZi::Client::User::Get()->GetNickname()} {}

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

    request_data->id = YiZi::Client::User::Get()->GetId();

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

    WriteTranscript((const wchar_t*)response_data->content,
                    CTime{static_cast<__time64_t>(response_data->timestamp / 1000)},
                    (const wchar_t*)response_data->nickname);
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
    auto* const socket = new YiZi::Client::CSocket{YiZi::Client::CSocket::Get()};

    const auto* const response_header = (YiZi::Packet::PacketHeader*)m_pChatResponseBuffer;
    while (true)
    {
        if (const auto count = socket->Receive(m_pChatResponseBuffer, s_iChatResponseBufferLen);
            count <= 0)
            break;

        if (response_header->type != (uint8_t)YiZi::Packet::PacketType::ChatMessageResponse)
        {
            std::this_thread::yield();
            continue;
        }

        ::PostMessage(hWnd, WM_RECVDATA, 0, 0);
        std::this_thread::yield();
    }

    delete socket;
}

void CChatDlg::WriteTranscript(const CString& message)
{
    WriteTranscript(message, CTime::GetCurrentTime(), m_csLocalUserNickname);
}

void CChatDlg::WriteTranscript(const CString& message, const CTime& time, const CString& nickname)
{
    std::lock_guard lock{m_mCSTranscript};
    m_csTranscript.AppendFormat(_T("%s <%s>: %s\r\n"), time.Format(_T("%T")), nickname, message);
    UpdateData(false);
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

    WriteTranscript(m_csMessage);
    m_csMessage.Empty();
    UpdateData(false);
}

void CChatDlg::OnBnClickedButtonEmptyTranscript()
{
    std::lock_guard lock{m_mCSTranscript};
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

    HandleLogoutRequest(); // Don't call socket to close after this, because sever-end closed the connection.

    YiZi::Client::User::Delete();
    YiZi::Client::Channel::DeleteCurrentChannel();

    EndDialog(YiZi::Client::DialogBoxCommandID::CID_LOGOUT);
}

LRESULT CChatDlg::OnRecvData(WPARAM wParam, LPARAM lParam)
{
    HandleChatMessageResponse();
    return 0;
}

// Will be called after EndDialog() ends.
BOOL CChatDlg::DestroyWindow()
{
    if (m_tListenChatMessageThread.joinable())
        m_tListenChatMessageThread.join();
    if (auto* const g_Socket = YiZi::Client::CSocket::Get();
        !g_Socket->IsClosed())
    {
        g_Socket->Close();
    }

    delete m_pChatRequestBuffer;

    return CDialogEx::DestroyWindow();
}

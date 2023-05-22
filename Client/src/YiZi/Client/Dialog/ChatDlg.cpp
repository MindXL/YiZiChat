// ChatDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "ChatDlg.h"

#include <YiZi/Client/Dialog/UserInfoDlg.h>
#include <YiZi/Client/Dialog/AboutDlg.h>
#include <YiZi/Client/Dialog/CheckFontDlg.h>

// CChatDlg 对话框

IMPLEMENT_DYNAMIC(CChatDlg, CDialogEx)

CChatDlg::CChatDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHAT_DIALOG, pParent)
      , m_csMessage(_T(""))
      , m_csLocalUserNickname{YiZi::Client::User::Get()->GetNickname()} {}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_MESSAGE, m_csMessage);
    DDV_MaxChars(pDX, m_csMessage, YiZi::Database::Transcript::ItemLength::CONTENT_MAX_LENGTH);
    DDX_Control(pDX, IDC_RICHEDIT2_TRANSCRIPT, m_recTranscript);
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatDlg::OnBnClickedButtonSend)
    ON_BN_CLICKED(IDC_BUTTON_EMPTY_TRANSCRIPT, &CChatDlg::OnBnClickedButtonEmptyTranscript)
    ON_COMMAND(ID_USER_INFO, &CChatDlg::OnUserInfo)
    ON_COMMAND(ID_ABOUT, &CChatDlg::OnAbout)
    ON_COMMAND(ID_LOGOUT, &CChatDlg::OnLogout)
    ON_MESSAGE(WM_RECVDATA, &CChatDlg::OnRecvData)
    ON_COMMAND(ID_FONT, &CChatDlg::OnFont)
END_MESSAGE_MAP()

BOOL CChatDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_recTranscript.SetDefaultCharFormat(*YiZi::Client::DefaultCF::Get());

    m_recTranscript.SetBackgroundColor(false, RGB(240, 240, 240));

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
    memcpy_s(request_data->content, YiZi::Database::Transcript::ItemLength::CONTENT_MAX_LENGTH * sizeof(wchar_t),
             m_csMessage.GetString(), message_len * sizeof(wchar_t));
    *((char16_t*)request_data->content + message_len) = u'\0';

    YiZi::Client::CSocket::Get()->Send(m_pChatRequestBuffer, s_iChatRequestBufferLen);

    return true;
}

void CChatDlg::HandleChatMessageResponse()
{
    const auto* const response_data = (YiZi::Packet::ChatMessageResponse*)(m_pChatResponseBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);

    const auto content{(const wchar_t*)response_data->content};
    const CTime sendTime{static_cast<__time64_t>(response_data->timestamp / 1000)};
    const auto nickname{(const wchar_t*)response_data->nickname};

    // TODO: Store and write can be run in parallel.
    StoreTranscript(content, sendTime, nickname);
    WriteTranscript(content, sendTime, nickname, true);
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
        if (const auto count = socket->Receive(m_pChatResponseBuffer, s_iChatResponseBufferLen, MSG_PEEK);
            count <= 0)
            break;

        if (response_header->type != (uint8_t)YiZi::Packet::PacketType::ChatMessageResponse)
        {
            std::this_thread::yield();
            continue;
        }

        socket->Receive(m_pChatResponseBuffer, s_iChatResponseBufferLen);
        ::PostMessage(hWnd, WM_RECVDATA, 0, 0);
        std::this_thread::yield();
    }

    delete socket;
}

void CChatDlg::StoreTranscript(const CString& message, const CTime& time, const CString& nickname)
{
    std::lock_guard lock{m_mLists};

    m_lTranscriptItem.emplace_back(message, time, nickname);
}

void CChatDlg::WriteTranscript(const CString& message, const CTime& time, const CString& nickname, const bool toLock)
{
    std::unique_lock lock{m_mTranscript, std::defer_lock};
    if (toLock)
        lock.lock();

    // TODO: Limit the size of transcript recorded.

    m_recTranscript.SetSel(LONG_MAX, LONG_MAX);

    if (!m_bIsFirstLine)
        m_recTranscript.ReplaceSel(_T("\r\n\r\n"));
    else
        m_bIsFirstLine = false;

    m_recTranscript.SetSelectionCharFormat(*YiZi::Client::TranscriptNicknameCF::Get());
    m_recTranscript.ReplaceSel(nickname);

    m_recTranscript.ReplaceSel(_T("    "));

    m_recTranscript.SetSelectionCharFormat(*YiZi::Client::TranscriptTimeCF::Get());
    m_recTranscript.ReplaceSel(time.Format(_T("%T")));

    m_recTranscript.ReplaceSel(_T("\r\n"));

    m_recTranscript.SetSelectionCharFormat(*YiZi::Client::TranscriptContentCF::Get());
    m_recTranscript.ReplaceSel(message);

    m_recTranscript.SetSelectionCharFormat(*YiZi::Client::DefaultCF::Get());

    if (toLock)
        lock.unlock();

    SendDlgItemMessage(IDC_RICHEDIT2_TRANSCRIPT, WM_VSCROLL, SB_BOTTOM, 0);
}

void CChatDlg::RewriteAllTranscript()
{
    std::lock_guard lock{m_mTranscript};
    ClearTranscript(false);
    for (const auto& [content,time,nickname] : m_lTranscriptItem)
        WriteTranscript(content, time, nickname, false);
}

void CChatDlg::ClearTranscript(const bool toLock)
{
    std::unique_lock lock{m_mTranscript, std::defer_lock};
    if (toLock)
        lock.lock();

    m_recTranscript.SetSel(0, -1);
    m_recTranscript.SetReadOnly(false);
    m_recTranscript.Clear();
    m_recTranscript.SetReadOnly(true);

    m_bIsFirstLine = true;

    if (toLock)
        lock.unlock();
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

    // TODO: Store message in a map. Use CTime to be map's key.
    // When multiple messages are stored, make sure their sequences of appearance in the windows matches their own times.
    const CTime current{CTime::GetCurrentTime()};

    // TODO: Store and write can be run in parallel.
    StoreTranscript(m_csMessage, current, m_csLocalUserNickname);
    WriteTranscript(m_csMessage, current, m_csLocalUserNickname, true);
    m_csMessage.Empty();
    UpdateData(false);
}

void CChatDlg::OnBnClickedButtonEmptyTranscript()
{
    ClearTranscript(true);
}

void CChatDlg::OnUserInfo()
{
    CUserInfoDlg{}.DoModal();
    // TODO: Update local user's nickname in transcripts.
    // TODO: Receive a packet of others changing their nickname, and then update in transcripts.
}

void CChatDlg::OnAbout()
{
    CAboutDlg{}.DoModal();
}

void CChatDlg::OnFont()
{
    // TODO: Check if chat message can still be received while dlg is open.
    CCheckFontDlg{}.DoModal();

    RewriteAllTranscript();

    SendDlgItemMessage(IDC_RICHEDIT2_TRANSCRIPT, WM_VSCROLL, SB_BOTTOM, 0);
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

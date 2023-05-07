// SelectChannelDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "SelectChannelDlg.h"

// CSelectChannelDlg 对话框

IMPLEMENT_DYNAMIC(CSelectChannelDlg, CDialogEx)

CSelectChannelDlg::CSelectChannelDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_SELECT_CHANNEL_DIALOG, pParent) {}

CSelectChannelDlg::~CSelectChannelDlg() {}

void CSelectChannelDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_lbChannel, m_lbChannel);
}

BEGIN_MESSAGE_MAP(CSelectChannelDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOIN, &CSelectChannelDlg::OnBnClickedButtonJoin)
    ON_MESSAGE(WM_RECV_CHANNEL, &CSelectChannelDlg::OnRecvChannel)
END_MESSAGE_MAP()

void CSelectChannelDlg::FetchChannelList(const HWND hwnd)
{
    YiZi::Client::ChannelHashtable::Get()->clear();
    auto* const socket = new YiZi::Client::CSocket{YiZi::Client::CSocket::Get()};

    // TODO: Auto retry if fetching fails.
    if (!HandleChannelListRequest(socket))
        return;
    HandleChannelListResponse(socket, hwnd);

    delete socket;
}

bool CSelectChannelDlg::HandleChannelListRequest(YiZi::Client::CSocket* socket)
{
#ifdef YZ_DEBUG
    // Ensure socket is open.
    if (socket->IsClosed())
        __debugbreak();
#endif

    uint8_t* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();
    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHANNEL_LIST_REQUEST_LENGTH;

    auto* const request_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(reqBuffer);
    request_header->type = (uint8_t)YiZi::Packet::PacketType::ChannelListRequest;

    if (!socket->Send(reqBuffer, request_len))
    {
        AfxMessageBox(_T("频道信息请求失败。"));
        return false;
    }
    return true;
}

void CSelectChannelDlg::HandleChannelListResponse(YiZi::Client::CSocket* socket, const HWND hwnd)
{
    uint8_t* const resBuffer = YiZi::Client::Buffer::Get()->GetResBuffer();
    constexpr int response_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHANNEL_LIST_RESPONSE_LENGTH;

    socket->Receive(resBuffer, response_len);

    const auto* const response_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(resBuffer);
    if (response_header->type != (uint8_t)YiZi::Packet::PacketType::ChannelListResponse)
    {
        // TODO: Handle.
        return;
    }

    const auto* const response_data = reinterpret_cast<YiZi::Packet::ChannelListResponse*>(resBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);
    const uint32_t count = response_data->count;

    if (count == 0)
    {
        AfxMessageBox(_T("当前暂无频道运行在服务器上。"));
        return;
    }

    // TODO: Make this async.
    // TODO: Handle failure.
    for (uint32_t _ = 0; _ < count; ++_)
        HandleChannelDetailResponse(socket, hwnd);
}

void CSelectChannelDlg::HandleChannelDetailResponse(YiZi::Client::CSocket* socket, HWND hwnd)
{
    uint8_t* const resBuffer = YiZi::Client::Buffer::Get()->GetResBuffer();
    constexpr int response_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHANNEL_DETAIL_RESPONSE_LENGTH;

    socket->Receive(resBuffer, response_len);

    const auto* const response_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(resBuffer);
    if (response_header->type != (uint8_t)YiZi::Packet::PacketType::ChannelDetailResponse)
    {
        // TODO: Handle.
        return;
    }

    const auto* const response_data = reinterpret_cast<YiZi::Packet::ChannelDetailResponse*>(resBuffer + YiZi::Packet::PACKET_HEADER_LENGTH);
    const auto* const pChannel = &(YiZi::Client::ChannelHashtable::Get()->emplace(response_data->id,
                                                                                  YiZi::Client::Channel
                                                                                  {
                                                                                      response_data->id,
                                                                                      std::move(CString{(const wchar_t*)response_data->name}),
                                                                                      response_data->join_time,
                                                                                      std::move(CString{(const wchar_t*)response_data->description})
                                                                                  }).first->second);
    ::PostMessage(hwnd, WM_RECV_CHANNEL, (WPARAM)pChannel, 0);
}

// CSelectChannelDlg 消息处理程序

BOOL CSelectChannelDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

#ifdef YZ_DEBUG
    // Ensure socket is open.
    if (YiZi::Client::CSocket::Get()->IsClosed())
        __debugbreak();
#endif

    m_tFetchChannelListThread = std::thread{FetchChannelList, GetSafeHwnd()};
    m_tFetchChannelListThread.detach();

    return TRUE; // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

BOOL CSelectChannelDlg::DestroyWindow()
{
    if (m_tFetchChannelListThread.joinable())
        m_tFetchChannelListThread.join();

    return CDialogEx::DestroyWindow();
}

void CSelectChannelDlg::OnBnClickedButtonJoin() {}

afx_msg LRESULT CSelectChannelDlg::OnRecvChannel(const WPARAM wParam, LPARAM lParam)
{
    const auto* const pChannel = reinterpret_cast<YiZi::Client::Channel*>(wParam);

    const int number = m_lbChannel.AddString(pChannel->GetName());
    m_umChannelMap.emplace(number, pChannel->GetId());

    return 0;
}

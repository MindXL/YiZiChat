// TestConnectionDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "TestConnectionDlg.h"

// CTestConnectionDlg 对话框

IMPLEMENT_DYNAMIC(CTestConnectionDlg, CDialogEx)

CTestConnectionDlg::CTestConnectionDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_TEST_CONNECTION_DIALOG, pParent)
      , m_csAddress(_T(""))
      , m_csPort(_T("")) {}

CTestConnectionDlg::~CTestConnectionDlg() {}

void CTestConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_ADDRESS, m_csAddress);
    DDX_Text(pDX, IDC_EDIT_PORT, m_csPort);
}

BEGIN_MESSAGE_MAP(CTestConnectionDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CTestConnectionDlg::OnBnClickedOk)
END_MESSAGE_MAP()

bool CTestConnectionDlg::HandleTestConnectionRequest()
{
    uint8_t* const reqBuffer = YiZi::Client::Buffer::Get()->GetReqBuffer();
    constexpr int request_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::TEST_CONNECTION_REQUEST_LENGTH;

    auto* const request_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(reqBuffer);
    request_header->type = (uint8_t)YiZi::Packet::PacketType::TestConnectionRequest;

    auto* const socket = YiZi::Client::CSocket::Get();
    return socket->Send(reqBuffer, request_len);
}

bool CTestConnectionDlg::HandleTestConnectionResponse()
{
    uint8_t* const resBuffer = YiZi::Client::Buffer::Get()->GetResBuffer();
    constexpr int response_len = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::TEST_CONNECTION_RESPONSE_LENGTH;

    YiZi::Client::CSocket::Get()->Receive(resBuffer, response_len);

    const auto* const response_header = reinterpret_cast<YiZi::Packet::PacketHeader*>(resBuffer);
    return response_header->type == (uint8_t)YiZi::Packet::PacketType::TestConnectionResponse;
}

BOOL CTestConnectionDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    const auto* const environment = YiZi::Client::Environment::Get();
    m_csAddress = environment->GetServerIp();
    if (const YiZi::port_t port = environment->GetServerPort();
        port != YiZi::port_t{})
        m_csPort.AppendFormat(_T("%d"), port);
    UpdateData(false);

    return TRUE; // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

// CTestConnectionDlg 消息处理程序

void CTestConnectionDlg::OnBnClickedOk()
{
    UpdateData(true);
    if (!YiZi::Client::InputControl::Server_Address(m_csAddress))
    {
        GetDlgItem(IDC_EDIT_ADDRESS)->GetFocus();
        return;
    }
    if (!YiZi::Client::InputControl::Server_Port(m_csPort))
    {
        GetDlgItem(IDC_EDIT_PORT)->GetFocus();
        return;
    }

    YiZi::port_t port{};
    std::stringstream ss;
    ss << CStringA{m_csPort}.GetString();
    ss >> port;

    auto* const socket = YiZi::Client::CSocket::Get();

    socket->Initialize();
    const bool success = socket->Connect(m_csAddress, port);

    if (!success || !HandleTestConnectionRequest() || !HandleTestConnectionResponse())
    {
        if (!socket->IsClosed())
            socket->Close();
        AfxMessageBox(_T("连接失败。"));
        return;
    }

    // Ensure here though server-end will close socket.
    if (!socket->IsClosed())
        socket->Close();

    YiZi::Client::Environment::Get()->CheckServerIpConfig(std::move(m_csAddress), port);

    CDialogEx::OnOK();
}

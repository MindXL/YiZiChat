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
      , m_uiPort(0) {}

CTestConnectionDlg::~CTestConnectionDlg() {}

void CTestConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_ADDRESS, m_csAddress);
    DDX_Text(pDX, IDC_EDIT_PORT, m_uiPort);
}

BEGIN_MESSAGE_MAP(CTestConnectionDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_CONFIRM, &CTestConnectionDlg::OnBnClickedButtonConfirm)
END_MESSAGE_MAP()

// CTestConnectionDlg 消息处理程序

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
    return response_header->type != (uint8_t)YiZi::Packet::PacketType::TestConnectionResponse;
}

void CTestConnectionDlg::OnBnClickedButtonConfirm()
{
    UpdateData(true);

    auto* const socket = YiZi::Client::CSocket::Get();

    const bool success = socket->Connect(m_csAddress, m_uiPort);

    if (!success || !HandleTestConnectionRequest() || !HandleTestConnectionResponse())
    {
        AfxMessageBox(_T("连接失败。"));
        return;
    }

    // Ensure here though server-end will close socket.
    if (!socket->IsClosed())
        socket->Close();

    CDialogEx::OnOK();
}

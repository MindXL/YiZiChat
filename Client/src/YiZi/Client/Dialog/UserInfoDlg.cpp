// UserInfoDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "UserInfoDlg.h"

// CUserInfoDlg 对话框

IMPLEMENT_DYNAMIC(CUserInfoDlg, CDialogEx)

CUserInfoDlg::CUserInfoDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_USER_INFO_DIALOG, pParent)
      , m_csPhone(_T(""))
      , m_csJoinTime(_T("")) {}

CUserInfoDlg::~CUserInfoDlg() {}

void CUserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PHONE, m_csPhone);
    DDV_MaxChars(pDX, m_csPhone, YiZi::Database::User::ItemLength::PHONE_LENGTH);
    DDX_Control(pDX, IDC_EDIT_PASSWORD, m_cePassword);
    DDX_Control(pDX, IDC_EDIT_NICKNAME, m_ceNickname);
    DDX_Text(pDX, IDC_EDIT_JOIN_TIME, m_csJoinTime);
    DDV_MaxChars(pDX, m_csJoinTime, 19);
}

BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialogEx)
END_MESSAGE_MAP()

// CUserInfoDlg 消息处理程序

BOOL CUserInfoDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    const auto* const user = YiZi::Client::User::Get();
    if (!user->IsValid())
        throw std::logic_error{"[CUserInfoDlg]: User instance is not initialized. There could be a logic error in coding."};

    m_csPhone = YiZi::Client::User::Get()->GetPhone();
    m_cePassword.SetWindowTextW(s_csPasswordPlaceholder);
    m_ceNickname.SetWindowTextW(user->GetNickname());
    m_csJoinTime = CTime{static_cast<__time64_t>(YiZi::Client::User::Get()->GetJoinTime() / 1000)}.Format(_T("%F %T"));
    UpdateData(false);

    return TRUE; // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

const CString CUserInfoDlg::s_csPasswordPlaceholder{_T("\001\001\001\001\001\001\001\001\001\001")};

// RegisterUserDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "RegisterUserDlg.h"

// CRegisterUserDlg 对话框

IMPLEMENT_DYNAMIC(CRegisterUserDlg, CDialogEx)

CRegisterUserDlg::CRegisterUserDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_REGISTER_USER_DIALOG, pParent) {}

void CRegisterUserDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRegisterUserDlg, CDialogEx)
END_MESSAGE_MAP()

// CRegisterUserDlg 消息处理程序

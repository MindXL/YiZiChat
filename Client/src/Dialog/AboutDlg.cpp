// AboutDlg.cpp: 实现文件
//

#include "pch.h"
#include "../../Client.h"
#include "afxdialogex.h"
#include "AboutDlg.h"

// CAboutDlg 对话框

IMPLEMENT_DYNAMIC(CAboutDlg, CDialogEx)

CAboutDlg::CAboutDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_ABOUT_DIALOG, pParent) {}

CAboutDlg::~CAboutDlg() {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CAboutDlg 消息处理程序

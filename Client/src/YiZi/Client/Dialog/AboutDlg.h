#pragma once
#include "afxdialogex.h"

// CAboutDlg 对话框

class CAboutDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CAboutDlg)

public:
    explicit CAboutDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CAboutDlg() override = default;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUT_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
};

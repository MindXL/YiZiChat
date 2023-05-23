#pragma once
#include "afxdialogex.h"

// CRegisterUserDlg 对话框

class CRegisterUserDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CRegisterUserDlg)

public:
    explicit CRegisterUserDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CRegisterUserDlg() override = default;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTER_USER_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
};

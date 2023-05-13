#pragma once
#include "afxdialogex.h"

// CLoginDlg 对话框

class CLoginDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CLoginDlg)

public:
    explicit CLoginDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CLoginDlg() override = default;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    bool HandleLoginRequest();
    bool HandleLoginResponse();
    afx_msg void OnBnClickedOk();

private:
    CString m_csPhone;
    CString m_csPassword;
};

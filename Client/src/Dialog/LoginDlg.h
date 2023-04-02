#pragma once
#include "afxdialogex.h"

// CLoginDlg 对话框

class CLoginDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CLoginDlg)

public:
    explicit CLoginDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CLoginDlg() override;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    CSocket* m_Socket{}; // Need to be abstracted and removed from here.
    bool HandleLoginRequest();
    bool HandleLoginResponse();

    CString m_csPhone;
    CString m_csPassword;

    afx_msg void OnBnClickedOk();
};

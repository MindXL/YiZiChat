#pragma once
#include "afxdialogex.h"

// CValidateAdminDlg 对话框

class CValidateAdminDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CValidateAdminDlg)

public:
    explicit CValidateAdminDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CValidateAdminDlg() override = default;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VALIDATE_ADMIN_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    [[nodiscard]] bool HandleValidateAdminRequest() const;
    [[nodiscard]] bool HandleValidateAdminResponse();

    afx_msg void OnBnClickedOk();

private:
    CString m_csPassword;
};

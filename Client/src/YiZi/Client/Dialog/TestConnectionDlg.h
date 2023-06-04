#pragma once
#include "afxdialogex.h"

#include <YiZi/YiZi.h>

// CTestConnectionDlg 对话框

class CTestConnectionDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CTestConnectionDlg)

public:
    explicit CTestConnectionDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CTestConnectionDlg() override;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_CONNECTION_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    [[nodiscard]] static bool HandleTestConnectionRequest();
    [[nodiscard]] static bool HandleTestConnectionResponse();

    BOOL OnInitDialog() override;
    afx_msg void OnBnClickedOk();

private:
    CString m_csAddress;
    CString m_csPort;
};

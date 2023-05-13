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
    [[nodiscard]] bool HandleTestConnectionRequest();
    [[nodiscard]] bool HandleTestConnectionResponse();

    afx_msg void OnBnClickedButtonConfirm();

private:
    CString m_csAddress;
    YiZi::port_t m_uiPort;
};

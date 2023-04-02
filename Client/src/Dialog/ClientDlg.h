// ClientDlg.h: 头文件
//

#pragma once

// CClientDlg 对话框
class CClientDlg final : public CDialogEx
{
    // 构造
public:
    explicit CClientDlg(CWnd* pParent = nullptr); // 标准构造函数

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    // 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    BOOL OnInitDialog() override;
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg [[nodiscard]] HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
};

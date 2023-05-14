#pragma once
#include "afxdialogex.h"

// CCheckFontDlg 对话框

class CCheckFontDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CCheckFontDlg)

public:
    explicit CCheckFontDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CCheckFontDlg() override = default;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHECK_FONT_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    BOOL OnInitDialog() override;
    BOOL DestroyWindow() override;

private:
    CComboBox m_cbFontSize;
};

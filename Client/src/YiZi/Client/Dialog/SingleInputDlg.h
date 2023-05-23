#pragma once
#include "afxdialogex.h"

// CSingleInputDlg 对话框

class CSingleInputDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CSingleInputDlg)

public:
    explicit CSingleInputDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CSingleInputDlg() override = default;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SINGLE_INPUT_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

public:
    enum class Usage
    {
        None,
        ValidateAdmin,
        RegisterUser
    };

    void SetUsage(Usage usage);

private:
    [[nodiscard]] bool HandleValidateAdminRequest() const;
    [[nodiscard]] bool HandleValidateAdminResponse() const;

    BOOL OnInitDialog() override;
    afx_msg void OnBnClickedOk();

private:
    Usage m_uUsage = Usage::None;
    CEdit m_ceSingleInput;
};

#pragma once
#include "afxdialogex.h"

// CUserInfoDlg 对话框

class CUserInfoDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CUserInfoDlg)

public:
    explicit CUserInfoDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CUserInfoDlg() override = default;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_INFO_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    BOOL OnInitDialog() override;

private:
    CString m_csPhone;
    CEdit m_cePassword;
    CEdit m_ceNickname;
    CString m_csJoinTime;

    static const CString s_csPasswordPlaceholder;
};

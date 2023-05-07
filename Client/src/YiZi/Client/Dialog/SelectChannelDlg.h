#pragma once
#include "afxdialogex.h"

#define WM_RECV_CHANNEL (WM_USER + 1)

// CSelectChannelDlg 对话框

class CSelectChannelDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CSelectChannelDlg)

public:
    explicit CSelectChannelDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CSelectChannelDlg() override;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_CHANNEL_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    static void FetchChannelList(HWND hwnd);
    static [[nodiscard]] bool HandleChannelListRequest(YiZi::Client::CSocket* socket);
    static void HandleChannelListResponse(YiZi::Client::CSocket* socket, HWND hwnd);
    static void HandleChannelDetailResponse(YiZi::Client::CSocket* socket, HWND hwnd);

    BOOL OnInitDialog() override;
    BOOL DestroyWindow() override;
    afx_msg void OnBnClickedButtonJoin();
    afx_msg LRESULT OnRecvChannel(WPARAM wParam, LPARAM lParam);

private:
    // std::unordered_map<m_lbChannel中的序号，所对应的频道id>
    std::unordered_map<int, uint32_t> m_umChannelMap;

    std::thread m_tFetchChannelListThread{};

    CListBox m_lbChannel;
};

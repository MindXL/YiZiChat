#pragma once
#include "afxdialogex.h"

#include <YiZi/Client/Core/Channel.h>

#define WM_RECV_CHANNEL (WM_USER + 1)
#define WM_CHANNEL_CONNECTION_SUCCESS (WM_RECV_CHANNEL + 1)
#define WM_CHANNEL_CONNECTION_FAILURE (WM_CHANNEL_CONNECTION_SUCCESS + 1)

// CSelectChannelDlg 对话框

class CSelectChannelDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CSelectChannelDlg)

public:
    explicit CSelectChannelDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CSelectChannelDlg() override = default;

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

    static void ConnectChannel(HWND hwnd);
    static bool HandleChannelConnectionRequest(YiZi::Client::CSocket* socket);
    static void HandleChannelConnectionResponse(YiZi::Client::CSocket* socket, HWND hwnd);

    void UpdateChannelDescription(int index);

    BOOL OnInitDialog() override;
    BOOL DestroyWindow() override;
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedRefresh();
    afx_msg LRESULT OnRecvChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChannelConnectionSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChannelConnectionFailure(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLbnSelchangelbchannel();

private:
    // std::unordered_map<m_lbChannel中的序号，所对应的频道id>
    std::unordered_map<int, YiZi::Client::Channel> m_umChannelMap;

    std::thread m_tFetchChannelListThread{};
    std::thread m_tConnectChannelThread{};

    CListBox m_lbChannel;

public:
    CString m_csChannelDescription;
};

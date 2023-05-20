#pragma once
#include "afxdialogex.h"

#define WM_RECVDATA (WM_USER + 1)

// CChatDlg 对话框

class CChatDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CChatDlg)

public:
    explicit CChatDlg(CWnd* pParent = nullptr); // 标准构造函数

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    BOOL OnInitDialog() override;

    [[nodiscard]] bool HandleChatMessageRequest() const;
    void HandleChatMessageResponse();
    static void HandleLogoutRequest();

    static void ListenChatMessage(HWND hWnd);

    void StoreTranscript(const CString& message, const CTime& time, const CString& nickname);
    // "toLock" is used to control whether lock inside the function or lock outside.
    void WriteTranscript(const CString& message, const CTime& time, const CString& nickname, bool toLock);

    void RewriteAllTranscript();
    void ClearTranscript(bool toLock);

    afx_msg void OnBnClickedButtonSend();
    afx_msg void OnBnClickedButtonEmptyTranscript();
    afx_msg void OnUserInfo();
    afx_msg void OnAbout();
    afx_msg void OnFont();
    afx_msg void OnLogout();
    afx_msg LRESULT OnRecvData(WPARAM wParam, LPARAM lParam);
    BOOL DestroyWindow() override;

private:
    static constexpr int s_iChatRequestBufferLen = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHAT_MESSAGE_REQUEST_LENGTH;
    static constexpr int s_iChatResponseBufferLen = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHAT_MESSAGE_RESPONSE_LENGTH;
    // 聊天界面单独设定一个Request buffer和Response buffer.
    uint8_t* m_pChatRequestBuffer = new uint8_t[s_iChatRequestBufferLen]{};
    static inline uint8_t m_pChatResponseBuffer[s_iChatResponseBufferLen]{};

    std::thread m_tListenChatMessageThread{};

    CRichEditCtrl m_recTranscript;
    CString m_csMessage;

    // TODO: Stop storing so many strings.
    std::mutex m_mLists;
    std::list<std::tuple<CString, CTime, CString>> m_lTranscriptItem;

    std::mutex m_mTranscript;
    const CString& m_csLocalUserNickname;
    bool m_bIsFirstLine = true;

    CHARFORMAT2 m_cfTranscriptContent{};
};

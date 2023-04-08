#pragma once
#include "afxdialogex.h"
#include "../../../YiZiLib/src/Packet.h"

// CChatDlg 对话框

class CChatDlg final : public CDialogEx
{
    DECLARE_DYNAMIC(CChatDlg)

public:
    explicit CChatDlg(CWnd* pParent = nullptr); // 标准构造函数
    ~CChatDlg() override;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT_DIALOG };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

private:
    bool HandleChatMessageRequest() const;
    bool HandleChatMessageResponse();
    static void HandleLogoutRequest();

    static constexpr int s_iChatRequestBufferLen = YiZi::Packet::PACKET_HEADER_LENGTH + YiZi::Packet::CHAT_MESSAGE_REQUEST_LENGTH;
    // 聊天界面单独设定一个Request buffer.
    uint8_t* m_pChatRequestBuffer = new uint8_t[s_iChatRequestBufferLen]{};

    CString m_csTranscript;
    CString m_csMessage;
    afx_msg void OnBnClickedButtonSend();
    afx_msg void OnBnClickedButtonEmptyTranscript();
    afx_msg void OnUserInfo();
    afx_msg void OnAbout();
    afx_msg void OnLogout();
};

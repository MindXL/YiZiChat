// CheckFontDlg.cpp: 实现文件
//

#include "pch.h"
#include <YiZi/Client/Client.h>
#include "afxdialogex.h"
#include "CheckFontDlg.h"

// CCheckFontDlg 对话框

IMPLEMENT_DYNAMIC(CCheckFontDlg, CDialogEx)

CCheckFontDlg::CCheckFontDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CHECK_FONT_DIALOG, pParent) {}

void CCheckFontDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MFCFONTCOMBO_FONT_FACE, m_fcbFontFace);
    DDX_Control(pDX, IDC_COMBO_FONT_SIZE, m_cbFontSize);
}

BEGIN_MESSAGE_MAP(CCheckFontDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CCheckFontDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    const auto* const g_Env = YiZi::Client::Environment::Get();

    // TODO: Read from file.
    m_fcbFontFace.SelectFont(g_Env->GetCurrentFontFace());

    int defaultIndex = 0;
    for (const auto& [fontSize, fontSizeStr] : *YiZi::Client::FontSizeMap::Get())
    {
        m_cbFontSize.AddString(fontSizeStr);

        const int index = m_cbFontSize.GetCount() - 1;
        m_cbFontSize.SetItemDataPtr(index, (void*)&fontSize);
        if (fontSize == g_Env->GetCurrentFontSize())
            defaultIndex = index;
    }
    m_cbFontSize.SetCurSel(defaultIndex);

    return TRUE; // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

BOOL CCheckFontDlg::DestroyWindow()
{
    auto* const nicknameCF = YiZi::Client::TranscriptNicknameCF::Get();
    auto* const timeCF = YiZi::Client::TranscriptTimeCF::Get();
    auto* const contentCF = YiZi::Client::TranscriptContentCF::Get();
    auto* const messageCFont = YiZi::Client::ChatDlgMessageCFont::Get();

    auto* const g_Env = YiZi::Client::Environment::Get();

    // If fontInfo equals nullptr, that means there's a bug in MFC.
    if (const auto* const fontInfo = m_fcbFontFace.GetSelFont();
        fontInfo != nullptr)
    {
        nicknameCF->SetFontFace(fontInfo->m_strName);
        timeCF->SetFontFace(fontInfo->m_strName);
        contentCF->SetFontFace(fontInfo->m_strName);
        messageCFont->SetFontFace(fontInfo->m_strName);

        g_Env->SetCurrentFontFace(fontInfo->m_strName);
    }

    const LONG fontSize = *static_cast<LONG*>(m_cbFontSize.GetItemDataPtr(m_cbFontSize.GetCurSel()));
    const LONG fontHeight = fontSize * 10;
    nicknameCF->SetFontHeight(fontHeight);
    timeCF->SetFontHeight(fontHeight * 0.8);
    contentCF->SetFontHeight(fontHeight * 1.5);
    messageCFont->SetFontHeight(fontSize);
    g_Env->SetCurrentFontSize(fontSize);

    return CDialogEx::DestroyWindow();
}

// CCheckFontDlg 消息处理程序

﻿// CheckFontDlg.cpp: 实现文件
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

    // TODO: Read from file.
    m_fcbFontFace.SelectFont(YiZi::Client::DefaultCF::Get()->szFaceName);

    constexpr LONG defaultFontSize = 16;
    int defaultIndex = 0;

    for (const auto& [fontSize, fontSizeStr] : *YiZi::Client::FontSizeMap::Get())
    {
        m_cbFontSize.AddString(fontSizeStr);

        const int index = m_cbFontSize.GetCount() - 1;
        m_cbFontSize.SetItemDataPtr(index, (void*)&fontSize);
        if (fontSize == defaultFontSize)
            defaultIndex = index;
    }

    m_cbFontSize.SetCurSel(defaultIndex);

    return TRUE; // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

BOOL CCheckFontDlg::DestroyWindow()
{
    auto* const defaultCF = YiZi::Client::TranscriptDefaultCF::Get();
    auto* const contentCF = YiZi::Client::TranscriptContentCF::Get();

    // If fontInfo equals nullptr, that means there's a bug in MFC.
    if (const auto* const fontInfo = m_fcbFontFace.GetSelFont();
        fontInfo != nullptr)
    {
        defaultCF->SetFontFace(fontInfo->m_strName);
        contentCF->SetFontFace(fontInfo->m_strName);
    }

    const LONG fontSize = *static_cast<LONG*>(m_cbFontSize.GetItemDataPtr(m_cbFontSize.GetCurSel()));
    const LONG fontHeight = fontSize * 10;
    defaultCF->SetFontHeight(fontHeight);
    contentCF->SetFontHeight(fontHeight * 1.5);

    return CDialogEx::DestroyWindow();
}

// CCheckFontDlg 消息处理程序

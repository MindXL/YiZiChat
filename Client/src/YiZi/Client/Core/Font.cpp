#include "pch.h"
#include "Font.h"

namespace YiZi::Client
{
    const FontSizeMap* const FontSizeMap::s_FontSizeMap = new FontSizeMap{
        {8, _T("8")},
        {9, _T("9")},
        {10, _T("10")},
        {11, _T("11")},
        {12, _T("12")},
        {14, _T("14")},
        {16, _T("16")},
        {18, _T("18")},
        {20, _T("20")},
        {22, _T("22")},
        {24, _T("24")},
        {26, _T("26")},
        {28, _T("28")},
        {36, _T("36")},
        {48, _T("48")},
        {72, _T("72")}
    };

    DefaultCF* DefaultCF::s_DefaultCF = new DefaultCF{};

    TranscriptNicknameCF* TranscriptNicknameCF::s_TranscriptNicknameCF = new TranscriptNicknameCF{};

    TranscriptTimeCF* TranscriptTimeCF::s_TranscriptTimeCF = new TranscriptTimeCF{};

    TranscriptContentCF* TranscriptContentCF::s_TranscriptContentCF = new TranscriptContentCF{};

    DefaultCFont* const DefaultCFont::s_DefaultCFont = new DefaultCFont{};

    ChatDlgMessageCFont* const ChatDlgMessageCFont::s_ChatDlgMessageCFont = new ChatDlgMessageCFont{};

    // Load font faces.
    // Save this.
    /*
    FontFaceVector::FontFaceVector()
    {
        const HDC hDC = GetDC(nullptr);

        LOGFONT lf{};
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfFaceName[0] = _T('\0');

        auto proc = [](CONST LOGFONTW* lpelfe, CONST TEXTMETRICW* lpntme, DWORD FontType, const LPARAM lParam)-> int
        {
            if (const auto& [hashtable, vector] = *reinterpret_cast<std::pair<std::set<CString>&, FontFaceVector*>*>(lParam);
                lpelfe->lfFaceName[0] != _T('@') && !hashtable.contains(lpelfe->lfFaceName))
            {
                vector->emplace_back(lpelfe->lfFaceName);
                hashtable.emplace(lpelfe->lfFaceName);
            }

            return 1;
        };

        std::set<CString> hashtable;
        std::pair<decltype(hashtable)&, FontFaceVector*> lParam{hashtable, this};

        EnumFontFamiliesEx(hDC, &lf, proc, (LPARAM)&lParam, 0);

        ReleaseDC(nullptr, hDC);
    }
    */

    DefaultCF::DefaultCF()
        : CHARFORMAT2()
    {
        cbSize = sizeof(DefaultCF);
        dwMask = 0xfeffffff;
        dwEffects = 0x44000000;
        yHeight = s_DefaultFontSize * 10;
        wcscpy_s(szFaceName, s_DefaultFontFace);
        wWeight = 400;
        lcid = 0x00000804;
        bUnderlineType = CFU_UNDERLINE;
    }

    TranscriptNicknameCF::TranscriptNicknameCF()
    {
        dwMask = 0xfeffffff | CFM_SIZE & (~CFM_UNDERLINETYPE);
        bUnderlineType = CFU_UNDERLINENONE;
    }

    TranscriptTimeCF::TranscriptTimeCF()
    {
        dwMask = 0xfeffffff | CFM_SIZE & (~CFM_UNDERLINETYPE);
        yHeight = static_cast<decltype(yHeight)>(160 * 0.8);
        bUnderlineType = CFU_UNDERLINENONE;
    }

    TranscriptContentCF::TranscriptContentCF()
    {
        dwMask = 0xfeffffff | CFM_SIZE & (~CFM_UNDERLINETYPE);
        yHeight = static_cast<decltype(yHeight)>(160 * 1.5);
        bUnderlineType = CFU_UNDERLINENONE;
    }

    void DefaultCFont::SetFontFace(const wchar_t* fontFace)
    {
        LOGFONT lf{};
        GetLogFont(&lf);

        DeleteObject();

        wcscpy_s(lf.lfFaceName, fontFace);
        CreateFontIndirect(&lf);
    }

    void DefaultCFont::SetFontHeight(const decltype(std::declval<LOGFONT>().lfHeight) fontSize)
    {
        LOGFONT lf{};
        GetLogFont(&lf);

        DeleteObject();

        const HDC hDC = GetDC(nullptr);
        lf.lfHeight = -MulDiv(fontSize * 0.8, GetDeviceCaps(hDC, LOGPIXELSY), 72);
        ReleaseDC(nullptr, hDC);

        CreateFontIndirect(&lf);
    }

    LOGFONT DefaultCFont::GetDefaultLOGFONT()
    {
        LOGFONT lf{};
        lf.lfHeight = -16;
        lf.lfWeight = 400;
        lf.lfCharSet = GB2312_CHARSET;
        wcscpy_s(lf.lfFaceName, s_DefaultFontFace);
        return lf;
    }

    DefaultCFont::DefaultCFont()
    {
        const LOGFONT lf{GetDefaultLOGFONT()};
        CreateFontIndirect(&lf);
    }

    ChatDlgMessageCFont::ChatDlgMessageCFont()
    {
        DeleteObject();

        LOGFONT lf{GetDefaultLOGFONT()};

        const HDC hDC = GetDC(nullptr);
        lf.lfHeight = -MulDiv(s_DefaultFontSize * 0.8, GetDeviceCaps(hDC, LOGPIXELSY), 72);
        ReleaseDC(nullptr, hDC);

        CreateFontIndirect(&lf);
    }
}

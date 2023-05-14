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

    TranscriptDefaultCF* TranscriptDefaultCF::s_TranscriptDefaultCF = new TranscriptDefaultCF{};

    TranscriptContentCF* TranscriptContentCF::s_TranscriptContentCF = new TranscriptContentCF{};

    DefaultCF::DefaultCF()
        : CHARFORMAT2()
    {
        cbSize = sizeof(DefaultCF);
        dwMask = 0xfeffffff;
        dwEffects = 0x44000000;
        yHeight = 160;
        wcscpy_s(szFaceName, _T("MS Shell Dlg 2"));
        wWeight = 400;
        lcid = 0x00000804;
        bUnderlineType = CFU_UNDERLINE;
    }

    TranscriptDefaultCF::TranscriptDefaultCF()
    {
        dwMask = 0xfeffffff | CFM_SIZE & (~CFM_UNDERLINETYPE);
        bUnderlineType = CFU_UNDERLINENONE;
    }

    TranscriptContentCF::TranscriptContentCF()
    {
        dwMask = 0xfeffffff | CFM_SIZE & (~CFM_UNDERLINETYPE);
        yHeight = static_cast<decltype(yHeight)>(160 * 1.5);
        bUnderlineType = CFU_UNDERLINENONE;
    }
}

#pragma once

#include <Richedit.h>

namespace YiZi::Client
{
    constexpr const wchar_t* const s_DefaultFontFace = _T("ÐÂËÎÌå");

    class FontSizeMap final : public std::map<long, const wchar_t*>
    {
    public:
        static const FontSizeMap* Get() { return s_FontSizeMap; }

    private:
        static const FontSizeMap* const s_FontSizeMap;

    private:
        using std::map<long, const wchar_t*>::map;
    };

    class DefaultCF : public CHARFORMAT2
    {
    public:
        static DefaultCF* Get() { return s_DefaultCF; }

        void SetFontFace(const wchar_t* fontFace) { wcscpy_s(szFaceName, fontFace); }
        void SetFontHeight(const decltype(yHeight) fontHeight) { yHeight = fontHeight; }

        explicit operator CHARFORMAT2() const { return *this; }

    private:
        static DefaultCF* s_DefaultCF;

    protected:
        DefaultCF();
    };

    class TranscriptNicknameCF final : public DefaultCF
    {
    public:
        static TranscriptNicknameCF* Get() { return s_TranscriptNicknameCF; }

    private:
        static TranscriptNicknameCF* s_TranscriptNicknameCF;

    private:
        TranscriptNicknameCF();
    };

    class TranscriptTimeCF final : public DefaultCF
    {
    public:
        static TranscriptTimeCF* Get() { return s_TranscriptTimeCF; }

    private:
        static TranscriptTimeCF* s_TranscriptTimeCF;

    private:
        TranscriptTimeCF();
    };

    class TranscriptContentCF final : public DefaultCF
    {
    public:
        static TranscriptContentCF* Get() { return s_TranscriptContentCF; }

    private:
        static TranscriptContentCF* s_TranscriptContentCF;

    private:
        TranscriptContentCF();
    };
}

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

    class TranscriptDefaultCF final : public DefaultCF
    {
    public:
        static TranscriptDefaultCF* Get() { return s_TranscriptDefaultCF; }

    private:
        static TranscriptDefaultCF* s_TranscriptDefaultCF;

    private:
        TranscriptDefaultCF();
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

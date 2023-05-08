#pragma once

namespace YiZi::Client
{
    class Channel : public ChannelBase<CString>
    {
    public:
        Channel(uint32_t id, CString name, uint64_t join_time, CString description);

        static void NewCurrentChannel(const Channel& channel);
        [[nodiscard]] static const Channel* GetCurrentChannel() { return s_CurrentChannel; }
        static void DeleteCurrentChannel();

    private:
        static Channel* s_CurrentChannel;
    };
}

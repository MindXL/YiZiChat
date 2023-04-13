#pragma once

namespace YiZi::Client
{
    class InputControl
    {
    public:
        static bool User_Phone(const CStringA& phone);
        static bool User_Password(const CStringA& password);
        static bool User_Nickname(const CString& nickname);
    };
}

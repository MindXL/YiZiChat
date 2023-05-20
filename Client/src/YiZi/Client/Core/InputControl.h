#pragma once

namespace YiZi::Client
{
    class InputControl
    {
    public:
        static bool Server_Address(const CString& ip);
        static bool Server_Port(const CString& port);

        static bool User_Phone(const CStringA& phone);
        static bool User_Password(const CStringA& password);
        static bool User_Nickname(const CString& nickname);

        InputControl() = delete;
    };
}

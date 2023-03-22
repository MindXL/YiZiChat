#pragma once

namespace YiZi
{
    class ConnectionHandler
    {
    public:
        static void Handle();

        static void HandleTestRequest();

    private:
        static void Dispatch();

        static void HandleTestResponse();
    };
}

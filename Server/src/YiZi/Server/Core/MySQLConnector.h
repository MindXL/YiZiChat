#pragma once

#include <mysqlx/xdevapi.h>

namespace YiZi::Server
{
    class MySQLConnector
    {
    public:
        MySQLConnector();
        ~MySQLConnector() { Disconnect(); }

        void Connect();
        void Disconnect();

        [[nodiscard]] bool IsConnected() const { return m_IsConnected; }

        [[nodiscard]] mysqlx::Schema* GetSchema() const { return m_Schema; }

        [[nodiscard]] static MySQLConnector* Get() { return s_MySQLConnector; }

    private:
        mysqlx::Session* m_Session;
        mysqlx::Schema* m_Schema;

        bool m_IsConnected;

        static MySQLConnector* s_MySQLConnector;
    };
}

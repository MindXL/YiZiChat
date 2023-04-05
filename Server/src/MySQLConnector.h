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

        [[nodiscard]] mysqlx::Schema* GetSchema() const { return m_Schema; }

        static MySQLConnector* Get() { return s_MySQLConnector; }

    private:
        mysqlx::Session* m_Session;
        mysqlx::Schema* m_Schema;

        static MySQLConnector* s_MySQLConnector;
    };
}

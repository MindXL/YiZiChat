#include "MySQLConnector.h"

#include <iostream>

#include "Environment.h"

namespace YiZi::Server
{
    MySQLConnector* MySQLConnector::s_MySQLConnector = new MySQLConnector{};

    MySQLConnector::MySQLConnector()
        : m_Session{nullptr}, m_Schema{nullptr} {}

    void MySQLConnector::Connect()
    {
        if (m_Session != nullptr || m_Schema != nullptr)
            throw std::logic_error{"[MySQLConnector]: Connect() was called when there exists a connection."};

        const auto* const env = Environment::Get();
        if (!env->IsDatabaseConfigReady())
            throw std::runtime_error{"[MySQLConnector]: Configuration for database is not ready yet."};

        std::cout << "Connecting to database... ";
        m_Session = new mysqlx::Session{
            mysqlx::SessionOption::HOST, "127.0.0.2",
            mysqlx::SessionOption::PORT, env->GetDBPort(),
            mysqlx::SessionOption::USER, env->GetDBUsername(),
            mysqlx::SessionOption::PWD, env->GetDBPassword(),
            mysqlx::SessionOption::DB, Environment::GetDBName()
        };
        std::cout << "Success." << std::endl;

        m_Schema = new mysqlx::Schema{m_Session->getSchema(Environment::GetDBName(), true)};
    }

    void MySQLConnector::Disconnect()
    {
        if (m_Schema != nullptr)
        {
            delete m_Schema;
            m_Schema = nullptr;
        }
        if (m_Session != nullptr)
        {
            m_Session->close();
            delete m_Session;
            m_Session = nullptr;
        }
    }
}

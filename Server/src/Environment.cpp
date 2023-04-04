#include "Environment.h"

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <pwd.h>
//#include <mysqlx/xdevapi.h>

namespace YiZi::Server
{
    Environment* Environment::s_Environment = new Environment{};

    void Environment::CheckEnvironment()
    {
        CheckDirectories();
        CheckDatabaseConfigFile();
    }

    void Environment::CheckDirectories()
    {
        const passwd* const pwd = getpwuid(getuid());
        m_RootDirectoryPath = pwd->pw_dir;
        std::cout << "The root directory of YiZiChat will be: " << m_RootDirectoryPath << std::endl;

        m_MainDirectoryPath = m_RootDirectoryPath / s_MainDirectoryName;
        m_SubDirectoryPath = m_MainDirectoryPath / s_SubDirectoryName;
        std::cout << "All files related will be stored in: " << m_SubDirectoryPath << std::endl;

        std::cout << "Checking for directories..." << std::endl;
        if (!std::filesystem::exists(m_SubDirectoryPath))
        {
            std::cout << "Directory doesn't exist. Creating... ";
            std::filesystem::create_directories(m_SubDirectoryPath);
            std::cout << "Done." << std::endl;
        }
        else
        {
            std::cout << "Directory already exists." << std::endl;
        }
    }

    void Environment::CheckDatabaseConfigFile()
    {
        m_DatabaseConfigFilePath = m_SubDirectoryPath / s_DatabaseConfigFileName;

        if (!std::filesystem::exists(m_DatabaseConfigFilePath))
        {
            std::cout << "Database information is not recorded. The information of database configuration will be stored in: "
                << m_DatabaseConfigFilePath
                << "Creating... ";

            std::ofstream oConfigFile{m_DatabaseConfigFilePath};
            std::cout << "Done." << std::endl;

            std::cout << "Please enter the host for database connection: ";
            std::cin >> m_DBHost;
            std::cout << "Please enter the port number: ";
            std::cin >> m_DBPort;
            std::cout << "Please enter username: ";
            std::cin >> m_DBUsername;
            std::cout << "Please enter password: ";
            std::cin >> m_DBPassword;

            oConfigFile << m_DBHost << '\n'
                << m_DBPort << '\n'
                << m_DBUsername << '\n'
                << m_DBPassword;
            oConfigFile.close();
        }
        else
        {
            std::cout << "Reading database configuration from file: " << m_DatabaseConfigFilePath << std::endl;

            std::ifstream iConfigFile{m_DatabaseConfigFilePath};
            // TODO: Prepare for corrupted config file.
            iConfigFile >> m_DBHost >> m_DBPort >> m_DBUsername >> m_DBPassword;
            iConfigFile.close();

            std::cout << "DBHost: " << m_DBHost << '\n'
                << "DBPort: " << m_DBPort << '\n'
                << "DBUsername: " << m_DBUsername << std::endl;
        }

        std::cout << "Connecting to database... ";

        // TODO: Test for database connection.
        std::cout << "Success." << std::endl;
    }
}

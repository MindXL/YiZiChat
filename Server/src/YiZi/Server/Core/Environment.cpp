#include "Environment.h"

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <pwd.h>

#include "MySQLConnector.h"
#include "Channel.h"

namespace YiZi::Server
{
    Environment* Environment::s_Environment = new Environment{};

    void Environment::CheckOuterEnvironment()
    {
        CheckDirectories();
        if (!IsDirectoryReady())
            throw std::runtime_error{"[Outer Environment]: Creating directories failed."};

        CheckDatabaseConfigFile();
        if (!IsDatabaseConfigReady())
            throw std::runtime_error{"[Outer Environment]: Configuring for database connection failed."};
    }

    void Environment::CheckInnerEnvironment()
    {
        LoadChannelMap();
        if (!IsChannelMapReady())
            throw std::runtime_error{"[Inner Environment]: Initializing ChannelMap failed."};
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

        m_IsDirectoryReady = true;
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
            system("stty -echo");
            std::cin >> m_DBPassword;
            system("stty echo");
            std::cout << std::endl;

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

        m_IsDatabaseConfigReady = true;
    }

    void Environment::LoadChannelMap()
    {
#ifdef YZ_DEBUG
        // Control this strictly.
        if (!MySQLConnector::Get()->IsConnected())
            __asm("int3");
#endif

        auto* const db = MySQLConnector::Get()->GetSchema();
        mysqlx::Table tChannel = db->getTable(Database::Channel::name);
        mysqlx::RowResult result = tChannel.select(
            Database::Channel::Item::id,
            Database::Channel::Item::name,
            Database::Channel::Item::join_time,
            Database::Channel::Item::description
        ).execute();

        auto* const channelMap = ChannelMap::Get();
        auto* const channelConnectionMap = ChannelConnectionMap::Get();
        for (const auto& row : result)
        {
            const auto cid = row[0].get<decltype(std::declval<Channel>().GetId())>();
            channelMap->emplace(cid,
                                Channel
                                {
                                    cid,
                                    std::u16string{(const char16_t*)row[1].getRawBytes().first},
                                    row[2].get<decltype(std::declval<Channel>().GetJoinTime())>(),
                                    std::u16string{(const char16_t*)row[3].getRawBytes().first}
                                });
            channelConnectionMap->emplace(cid, std::unordered_set<uint32_t>{});
        }

        m_IsChannelMapReady = true;
    }
}

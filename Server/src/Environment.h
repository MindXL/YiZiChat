#pragma once

#include <YiZi/YiZi.h>

#include <filesystem>

namespace YiZi::Server
{
    class Environment
    {
    public:
        static Environment* Get() { return s_Environment; }

        void CheckEnvironment();

        [[nodiscard]] std::filesystem::path GetSubDirPath() const { return m_SubDirectoryPath; }
        [[nodiscard]] std::filesystem::path GetDBConfigFilePath() const { return m_DatabaseConfigFilePath; }

        [[nodiscard]] const std::string& GetDBHost() const { return m_DBHost; }
        [[nodiscard]] port_t GetDBPort() const { return m_DBPort; }
        [[nodiscard]] const std::string& GetDBUsername() const { return m_DBUsername; }
        [[nodiscard]] const std::string& GetDBPassword() const { return m_DBPassword; }
        [[nodiscard]] static const char* GetDBName() { return s_DBName; }

    private:
        void CheckDirectories();
        void CheckDatabaseConfigFile();

    private:
        std::filesystem::path m_RootDirectoryPath;
        static constexpr const char* s_MainDirectoryName = "YiZiChat";
        std::filesystem::path m_MainDirectoryPath;
        static constexpr const char* s_SubDirectoryName = "Server";
        std::filesystem::path m_SubDirectoryPath;

        static constexpr const char* s_DatabaseConfigFileName = "YiZiChat-Server-Database.config";
        std::filesystem::path m_DatabaseConfigFilePath;
        std::string m_DBHost;
        port_t m_DBPort{};
        std::string m_DBUsername;
        std::string m_DBPassword;
        static constexpr const char* s_DBName = "YiZiChat";

        static Environment* s_Environment;
    };
}

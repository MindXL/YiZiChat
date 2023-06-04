#pragma once

#include <YiZi/YiZi.h>

#include <filesystem>

namespace YiZi::Server
{
    class Environment
    {
    public:
        static Environment* Get() { return s_Environment; }

        void CheckOuterEnvironment();

        [[nodiscard]] std::filesystem::path GetSubDirPath() const { return m_SubDirectoryPath; }
        [[nodiscard]] std::filesystem::path GetDBConfigFilePath() const { return m_DatabaseConfigFilePath; }
        [[nodiscard]] bool IsDirectoryReady() const { return m_IsDirectoryReady; }

        [[nodiscard]] const std::string& GetDBHost() const { return m_DBHost; }
        [[nodiscard]] port_t GetDBPort() const { return m_DBPort; }
        [[nodiscard]] const std::string& GetDBUsername() const { return m_DBUsername; }
        [[nodiscard]] const std::string& GetDBPassword() const { return m_DBPassword; }
        [[nodiscard]] static const char* GetDBName() { return s_DBName; }
        [[nodiscard]] bool IsDatabaseConfigReady() const { return m_IsDatabaseConfigReady; }

        void CheckInnerEnvironment();

        [[nodiscard]] bool IsChannelMapReady() const { return m_IsChannelMapReady; }

    private:
        void CheckDirectories();
        void CheckDatabaseConfigFile();

        void LoadChannelMap();

    private:
        std::filesystem::path m_RootDirectoryPath;
        static constexpr const char* s_MainDirectoryName = "YiZiChat";
        std::filesystem::path m_MainDirectoryPath;
        static constexpr const char* s_SubDirectoryName = "Server";
        std::filesystem::path m_SubDirectoryPath;
        bool m_IsDirectoryReady = false;

        static constexpr const char* s_DatabaseConfigFileName = "YiZiChat-Server-Database.config";
        std::filesystem::path m_DatabaseConfigFilePath;
        std::string m_DBHost;
        port_t m_DBPort{};
        std::string m_DBUsername;
        std::string m_DBPassword;
        static constexpr const char* s_DBName = "YiZiChat";
        bool m_IsDatabaseConfigReady = false;

        bool m_IsChannelMapReady = false;

        static Environment* s_Environment;
    };
}

#pragma once

#include <filesystem>

namespace YiZi::Client
{
    class Environment
    {
    public:
        static Environment* Get() { return s_Environment; }

        void CheckOuterEnvironment();

        [[nodiscard]] std::filesystem::path GetSubDirPath() const { return m_SubDirectoryPath; }
        [[nodiscard]] std::filesystem::path GetServerIpConfigFilePath() const { return m_ServerIpConfigFilePath; }
        [[nodiscard]] bool IsDirectoryReady() const { return m_IsDirectoryReady; }

        [[nodiscard]] const CString& GetServerIp() const { return m_ServerIp; }
        [[nodiscard]] port_t GetServerPort() const { return m_ServerPort; }
        [[nodiscard]] bool IsDatabaseConfigFileReady() const { return m_IsServerIpConfigFileReady; }

        // Will be called in CTestConnectionDlg.
        void CheckServerIpConfig(CString serverIp, port_t serverPort);

        void CheckInnerEnvironment();

        void SetCurrentFontFace(const CString& currentFontFace) { m_CurrentFontFace = currentFontFace; }
        [[nodiscard]] const CString& GetCurrentFontFace() const { return m_CurrentFontFace; }
        void SetCurrentFontSize(const LONG currentFontSize) { m_CurrentFontSize = currentFontSize; }
        [[nodiscard]] LONG GetCurrentFontSize() const { return m_CurrentFontSize; }

    private:
        Environment();

        void CheckDirectories();
        void CheckServerIpConfigFile(); // Just check the file. Does not check configuration.

        void LoadFontInfo();

    private:
        std::filesystem::path m_RootDirectoryPath;
        static constexpr const char* s_MainDirectoryName = "YiZiChat";
        std::filesystem::path m_MainDirectoryPath;
        static constexpr const char* s_SubDirectoryName = "Client";
        std::filesystem::path m_SubDirectoryPath;
        bool m_IsDirectoryReady = false;

        static constexpr const char* s_ServerIpConfigFileName = "YiZiChat-Client-ServerIp.config";
        std::filesystem::path m_ServerIpConfigFilePath;
        CString m_ServerIp{};
        port_t m_ServerPort{};
        bool m_IsServerIpConfigFileReady = false;

        CString m_CurrentFontFace{};
        LONG m_CurrentFontSize{};

        static Environment* s_Environment;
    };
}

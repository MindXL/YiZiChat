#include "pch.h"
#include "Environment.h"

namespace YiZi::Client
{
    Environment* Environment::s_Environment = new Environment{};

    void Environment::CheckOuterEnvironment()
    {
        CheckDirectories();
        if (!IsDirectoryReady())
            throw std::runtime_error{"[Outer Environment]: Creating directories failed."};

        CheckServerIpConfigFile();
        if (!IsDatabaseConfigFileReady())
            throw std::runtime_error{"[Outer Environment]: Check for server ip config file failed."};
    }

    void Environment::CheckServerIpConfig(CString serverIp, const port_t serverPort)
    {
        m_ServerIp = std::move(serverIp);
        const CStringA serverIp_s{m_ServerIp};
        m_ServerPort = serverPort;

        std::ofstream oConfigFile{m_ServerIpConfigFilePath};
        oConfigFile << serverIp_s << '\n'
            << m_ServerPort;
        oConfigFile.close();
    }

    void Environment::CheckDirectories()
    {
        char* buffer = nullptr;
        size_t bufferSize{};
        _dupenv_s(&buffer, &bufferSize, "APPDATA");
        m_RootDirectoryPath = buffer;

        m_MainDirectoryPath = m_RootDirectoryPath / s_MainDirectoryName;
        m_SubDirectoryPath = m_MainDirectoryPath / s_SubDirectoryName;

        if (!std::filesystem::exists(m_SubDirectoryPath))
            std::filesystem::create_directories(m_SubDirectoryPath);

        m_IsDirectoryReady = true;
    }

    void Environment::CheckServerIpConfigFile()
    {
        m_ServerIpConfigFilePath = m_SubDirectoryPath / s_ServerIpConfigFileName;

        if (!std::filesystem::exists(m_ServerIpConfigFilePath))
        {
            // Just check file here.
            // Configuration will be written in CTestConnectionDlg.
            std::ofstream{m_ServerIpConfigFilePath}.close();
        }
        else
        {
            std::ifstream iConfigFile{m_ServerIpConfigFilePath};
            // TODO: Prepare for corrupted config file.
            std::string serverIpBuffer;
            iConfigFile >> serverIpBuffer >> m_ServerPort;
            m_ServerIp = serverIpBuffer.c_str();
            iConfigFile.close();
        }

        m_IsServerIpConfigFileReady = true;
    }
}

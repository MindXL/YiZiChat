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

    void Environment::CheckInnerEnvironment()
    {
        LoadFontInfo();
    }

    Environment::Environment()
    {
        char* buffer = nullptr;
        size_t bufferSize{};
        _dupenv_s(&buffer, &bufferSize, "APPDATA");

        m_RootDirectoryPath = buffer;
        delete[] buffer;

        m_MainDirectoryPath = m_RootDirectoryPath / s_MainDirectoryName;
        m_SubDirectoryPath = m_MainDirectoryPath / s_SubDirectoryName;

        m_ServerIpConfigFilePath = m_SubDirectoryPath / s_ServerIpConfigFileName;
    }

    void Environment::CheckDirectories()
    {
        if (!std::filesystem::exists(m_SubDirectoryPath))
            std::filesystem::create_directories(m_SubDirectoryPath);

        m_IsDirectoryReady = true;
    }

    void Environment::CheckServerIpConfigFile()
    {
        if (!std::filesystem::exists(m_ServerIpConfigFilePath))
        {
            // Just check file here.
            // Configuration will be written in CTestConnectionDlg.
            std::ofstream{m_ServerIpConfigFilePath}.close();
        }
        else
        {
            std::ifstream iConfigFile{m_ServerIpConfigFilePath};

            std::string serverIpBuffer;

            bool success = true;
            for (int i = 0; i < 2; ++i)
            {
                switch (i)
                {
                case 0: iConfigFile >> serverIpBuffer;
                    break;
                case 1: iConfigFile >> m_ServerPort;
                    break;
#ifdef YZ_DEBUG
                // This means that maybe new item was added but not handled.
                default: __debugbreak();
#endif
                }
                if (iConfigFile.fail())
                {
                    success = false;
                    break;
                }
            }
            iConfigFile.close();
            if (success)
                m_ServerIp = serverIpBuffer.c_str();
            else
                std::ofstream{m_ServerIpConfigFilePath}.close();
        }

        m_IsServerIpConfigFileReady = true;
    }

    void Environment::LoadFontInfo()
    {
        m_CurrentFontFace = s_DefaultFontFace;
        m_CurrentFontSize = s_DefaultFontSize;
    }
}

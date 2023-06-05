﻿// Client.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "../../../framework.h"
#include "Client.h"

#include <YiZi/Client/Dialog/TestConnectionDlg.h>
#include <YiZi/Client/Dialog/LoginDlg.h>
#include <YiZi/Client/Dialog/SelectChannelDlg.h>
#include <YiZi/Client/Dialog/ChatDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CClientApp 构造

CClientApp::CClientApp()
{
    // 支持重新启动管理器
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CClientApp 对象

CClientApp theApp;

// CClientApp 初始化

BOOL CClientApp::InitInstance()
{
    // 如果一个运行在 Windows XP 上的应用程序清单指定要
    // 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
    //则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // 将它设置为包括所有要在应用程序中使用的
    // 公共控件类。
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    if (!AfxSocketInit())
    {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    // 创建 shell 管理器，以防对话框包含
    // 任何 shell 树视图控件或 shell 列表视图控件。
    CShellManager* pShellManager = new CShellManager;

    // 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

    // 标准初始化
    // 如果未使用这些功能并希望减小
    // 最终可执行文件的大小，则应移除下列
    // 不需要的特定初始化例程
    // 更改用于存储设置的注册表项
    // TODO: 应适当修改该字符串，
    // 例如修改为公司或组织名
    SetRegistryKey(_T(R"(由“心灵”（"Mind"）开发的易字聊天（局域网聊天系统）)"));

    // 调用 AfxInitRichEdit2() 以初始化 richedit2 库。"
    AfxInitRichEdit2();

    // Socket will be initialized inside Main().
    // Socket may be initialized or closed multiple times inside Main().
    Main();

    // Make sure that socket is closed.
    if (auto* const g_Socket = YiZi::Client::CSocket::Get();
        !g_Socket->IsClosed())
        g_Socket->Close();

    // 删除上面创建的 shell 管理器。
    if (pShellManager != nullptr)
    {
        delete pShellManager;
    }

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
    ControlBarCleanUp();
#endif

    // 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
    //  而不是启动应用程序的消息泵。
    return FALSE;
}

void CClientApp::Main()
{
    auto* const g_Env = YiZi::Client::Environment::Get();
    g_Env->CheckOuterEnvironment();
    g_Env->CheckInnerEnvironment();

    CDialogEx* dlg = nullptr;
    INT_PTR nResponse = 0;

    // Only break to close socket.
    while (true)
    {
        dlg = new CTestConnectionDlg();
        nResponse = dlg->DoModal();
        delete dlg;
        if (nResponse == IDCANCEL)
            break;
        if (nResponse == YiZi::Client::DialogBoxCommandID::CID_FAIL)
        {
            TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
            TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
            break;
        }

        dlg = new CLoginDlg();
        //m_pMainWnd = dlg;
        nResponse = dlg->DoModal();
        delete dlg;
        if (nResponse == IDCANCEL)
            break;
        if (nResponse == YiZi::Client::DialogBoxCommandID::CID_FAIL)
        {
            TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
            TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
            break;
        }

        dlg = new CSelectChannelDlg();
        nResponse = dlg->DoModal();
        delete dlg;
        if (nResponse == IDCANCEL)
            break;
        if (nResponse == YiZi::Client::DialogBoxCommandID::CID_FAIL)
        {
            TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
            TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
            break;
        }
        // TODO: Handle if user closes CSelectChannelDlg.

        dlg = new CChatDlg{};
        //m_pMainWnd = dlg;
        nResponse = dlg->DoModal();
        delete dlg;
        if (nResponse == IDCANCEL)
            break;
        if (nResponse == YiZi::Client::DialogBoxCommandID::CID_FAIL)
        {
            TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
            TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
            break;
        }
        if (nResponse != YiZi::Client::DialogBoxCommandID::CID_LOGOUT)
            break;

        // Make sure that socket is closed.
        if (auto* const g_Socket = YiZi::Client::CSocket::Get();
            !g_Socket->IsClosed())
        {
            g_Socket->Close();
        }
    }
}

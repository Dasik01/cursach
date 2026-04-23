// BruteForce.cpp: определяет поведение классов для приложения.
//

#include "pch.h"
#include "framework.h"
#include "BruteForce.h"
#include "BruteForceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBruteForceApp

BEGIN_MESSAGE_MAP(CBruteForceApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CBruteForceApp::CBruteForceApp()
{
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CBruteForceApp theApp;

BOOL CBruteForceApp::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    CShellManager* pShellManager = new CShellManager;

    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    CBruteForceDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();

    if (pShellManager != nullptr)
    {
        delete pShellManager;
    }

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
    ControlBarCleanUp();
#endif

    return FALSE;
}
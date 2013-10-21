// JxUpdater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "JxUpdater.h"
#include "JxUpdaterDlg.h"
#include "KProcess.h"
#include "UpdateDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// KJxUpdaterApp

BEGIN_MESSAGE_MAP(KJxUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// KJxUpdaterApp construction

KJxUpdaterApp::KJxUpdaterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only KJxUpdaterApp object

KJxUpdaterApp theApp;


// KJxUpdaterApp initialization

BOOL KJxUpdaterApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	int nRetCode = false;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	nRetCode = CanRun();
	if (!nRetCode)
	{
		return FALSE;
	}

	KJxUpdaterDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int KJxUpdaterApp::CanRun()
{
	int nRetCode = FALSE;
	int nResult  = FALSE;

	TCHAR *pSlash = NULL;
	TCHAR szAppPath[MAX_PATH];
	CString strProcess;
	CString StrProMain;
	KProcess Proc;

	nRetCode = GetModuleFileName(NULL, szAppPath, sizeof(szAppPath) / sizeof(TCHAR));
	KG_PROCESS_ERROR(nRetCode);
	pSlash = _tcsrchr(szAppPath, _T('\\'));
	KG_PROCESS_ERROR(pSlash);
	*(pSlash + 1) = _T('\0');

	strProcess = szAppPath;
	strProcess += APP_EXE_NAME;

	StrProMain = szAppPath;
	StrProMain += MAIN_EXE_NAME;

	while (TRUE)
	{
		nRetCode = Proc.FindProcess(strProcess);
		if (!nRetCode)
			break;

		nRetCode = Proc.FindProcess(StrProMain);
		if (!nRetCode)
			break;

		//TCHAR MsgBuffer[MAX_PATH];
		//LoadString(theApp.m_hInstance, 136, MsgBuffer, MAX_PATH);
		//CString strWarn = CString(MsgBuffer) + _T("\n") + strProcess;
		nRetCode = MessageBox(NULL, MSG_CANT_RUN, APP_EXE_NAME, MB_ICONEXCLAMATION | MB_ICONWARNING | MB_RETRYCANCEL);
		KG_PROCESS_ERROR(nRetCode && nRetCode != IDCANCEL);
	}

	nResult = TRUE;
Exit0:
	return nResult;
}
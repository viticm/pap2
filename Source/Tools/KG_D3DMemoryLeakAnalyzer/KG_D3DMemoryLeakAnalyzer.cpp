// KG_D3DMemoryLeakAnalyzer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "KG_D3DMemoryLeakAnalyzer.h"
#include "KG_D3DMemoryLeakAnalyzerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKG_D3DMemoryLeakAnalyzerApp

BEGIN_MESSAGE_MAP(CKG_D3DMemoryLeakAnalyzerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKG_D3DMemoryLeakAnalyzerApp construction

CKG_D3DMemoryLeakAnalyzerApp::CKG_D3DMemoryLeakAnalyzerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKG_D3DMemoryLeakAnalyzerApp object

CKG_D3DMemoryLeakAnalyzerApp theApp;


// CKG_D3DMemoryLeakAnalyzerApp initialization

BOOL CKG_D3DMemoryLeakAnalyzerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
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

	CKG_D3DMemoryLeakAnalyzerDlg dlg;
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

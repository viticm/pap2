// EffectCompiler.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EffectCompiler.h"
#include "EffectCompilerDlg.h"

#include "../CommandLineParser/CmdLineParser.h"
#include "CmdLineFileEnumer.h"
#include "Core.h"
#include "DiskFileController.h"
#include "ConsoleMode.h"
#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEffectCompilerApp

BEGIN_MESSAGE_MAP(CEffectCompilerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEffectCompilerApp construction

CEffectCompilerApp::CEffectCompilerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CEffectCompilerApp object

CEffectCompilerApp theApp;

void ParseCmdLine(LPCTSTR strCmdLine, AppCmdParams* pParams)
{
	CCmdLineParser	parser;
	parser.Parse(strCmdLine);

	_ASSERTE(NULL != pParams);

	CString csMode;
	if (parser.HasKey(_T("Mode")) && parser.GetVal(_T("Mode"), csMode))
	{
		if(0 == _tcsicmp(csMode, _T("vc")))
		{
			pParams->nRunMode = AppCmdParams::em_run_mode_vc;
		}
		else if (0 == _tcsicmp(csMode, _T("console")))
		{
			pParams->nRunMode = AppCmdParams::em_run_mode_console;
		}
		else	//=="dialog"
		{
			pParams->nRunMode = AppCmdParams::em_run_mode_dialog;
		}
	}
	else
	{
		pParams->nRunMode = AppCmdParams::em_run_mode_dialog;	//默认就是Dialog

		if (AttachConsole(ATTACH_PARENT_PROCESS))	//如果是从控制台启动的，那么使用控制台模式
		{
			pParams->nRunMode = AppCmdParams::em_run_mode_console;
			FreeConsole();
		}
	}
	
	pParams->bHasCmdFilePathsToBeCompiled = parser.HasKey(_T("F"));
	pParams->bForceCompile = parser.HasKey(_T("C"));
	pParams->bShowOnlyError = parser.HasKey(_T("S"));
	bool bHasOutputDir = parser.HasKey(_T("O"));
	if (! bHasOutputDir)
	{
		pParams->strOutputDir = _T("");
	}
	else 
	{
		bool bRet = parser.GetVal(_T("O"), pParams->strOutputDir);
		if(! bRet)
		{
			pParams->strOutputDir = _T("");
		}
	}

	if (! parser.GetVal(_T("CPY"), pParams->strCopyToDir))
	{
		pParams->strCopyToDir = _T("");
	}

	if(parser.HasKey(_T("?")))
	{
		pParams->bShowHelp = TRUE;
	}
	else
	{
		pParams->bShowHelp = FALSE;
	}
}

// CEffectCompilerApp initialization

BOOL CEffectCompilerApp::InitInstance()
{
	LPCTSTR strCmdLine = GetCommandLine();

	ParseCmdLine(strCmdLine, &g_AppCmdParams);

	if (AppCmdParams::em_run_mode_console == g_AppCmdParams.nRunMode)	//在vc的命令模式下面，不要重定向，vc会重定向
	{
		SetAppIntoConsoleMode();
	}

	//系统自己加的
	if(AppCmdParams::em_run_mode_dialog == g_AppCmdParams.nRunMode)
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

		/*CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		if (!ProcessShellCommand(cmdInfo))
		{
		return FALSE;
		}*/

	}
	BOOL bExceptionCatched = FALSE;
	__try
	{
		AppMain();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		bExceptionCatched = TRUE;		
	}

	if (bExceptionCatched)
	{
		LPCTSTR strTmp = _T("application exception caught, app abort");

		if(AppCmdParams::em_run_mode_dialog == g_AppCmdParams.nRunMode)
		{
			::MessageBox(NULL, strTmp, _T("Notice"), MB_OK);
		}
		std::tcout << strTmp  << _T("\r\n");
	}


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CEffectCompilerApp::AppMain()
{
	if (g_AppCmdParams.bShowHelp)
	{
		std::tcout << _T("Compile fx files into fxo files\n");
		std::tcout << _T("EffectCompiler [/Mode:Dialog|Console|VC] [/C] [/S] [/O:\"output path\"][/CPY:CopytoDir][/F:[paths]]\n");
		std::tcout << _T("/Mode: Run at \"dialog\" mode, or \"console\" mode, \"vc\" mode is use for vc compiler\n");
		std::tcout << _T("/C	Force compile, ignore file write time\n");
		std::tcout << _T("/S	Show only error Info\n");
		std::tcout << _T("/O:	Follows the output path for fxo files, \nfxo files will be output according to wheather this param is specified. Be awared that the path should be envelopped with \"\".\n");
		std::tcout << _T("/F:	Follows the fx files, \nIf it is not specified, files which's in the same dir as the application will be compiled. Be awared that the paths should be envelopped with \"\", and sperated with blanks\n");
		std::tcout << _T("/CPY:	Follows the copy-to dir, \nIf it is specified, every file compiled successfully will be copied to the  directory\n");
		std::tcout << _T("Sample : EffectCompiler /C /O:\"D:\\Debug\" /F: \"D:\\a.fx\" \"D:\\b.fx\"\n");
		std::tcout << _T("\n...Press Enter to continue\n");
		std::tcout << std::flush;
		return;
	}
	switch(g_AppCmdParams.nRunMode)
	{
	case AppCmdParams::em_run_mode_console:
	case AppCmdParams::em_run_mode_vc:
		{
			if (g_AppCmdParams.bHasCmdFilePathsToBeCompiled)
			{
				TCHAR strDir[MAX_PATH * 2] = _T("");
				::GetCurrentDirectory(_countof(strDir), strDir);
				ICFileEnumer* pFileEnumer = ::CreateCmdLineFileEnumer(::GetCommandLine(), strDir);
				ICCompileFileController* pFileController = ::CreateDiskFileController();
				if (NULL != pFileEnumer && NULL != pFileController)
				{
					Core::CompileShaders(*pFileEnumer, Core::GetDefaultOutput(), *pFileController
						, g_AppCmdParams.strOutputDir, g_AppCmdParams.strCopyToDir, g_AppCmdParams.bForceCompile, g_AppCmdParams.bShowOnlyError);
				}

				SAFE_DELETE(pFileEnumer);
			}
		}
		break;
	default:
		{
			CEffectCompilerDlg dlg;
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
		}
	    break;
	}
}
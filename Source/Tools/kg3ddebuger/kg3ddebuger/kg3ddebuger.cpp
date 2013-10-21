// kg3ddebuger.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "kg3ddebuger.h"
#include "kg3ddebugerDlg.h"
#include "Psapi.h"
#include "SelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "Psapi.lib")

#pragma warning(disable : 4996)

#define RemoveCodeSize 1024


// Ckg3ddebugerApp

BEGIN_MESSAGE_MAP(Ckg3ddebugerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Ckg3ddebugerApp construction

Ckg3ddebugerApp::Ckg3ddebugerApp()
    : m_hHookProcess(NULL),
      m_pRemoveCode(NULL),
      m_pRemoveData(NULL),
      m_strHookName(TEXT(""))
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

Ckg3ddebugerApp::~Ckg3ddebugerApp()
{
}

void Ckg3ddebugerApp::Release()
{
    if (m_pRemoveCode)
        VirtualFreeEx(m_hHookProcess, m_pRemoveCode, RemoveCodeSize, MEM_RELEASE);

    if (m_pRemoveData)
        VirtualFreeEx(m_hHookProcess, m_pRemoveData, sizeof(RemoveParam), MEM_RELEASE);

    if (m_hHookProcess)
        CloseHandle(m_hHookProcess);
}

// The one and only Ckg3ddebugerApp object

Ckg3ddebugerApp theApp;

// Ckg3ddebugerApp initialization

#pragma runtime_checks( "", off )
#pragma optimize( "", off )

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    bool bNeedFreeLib = false;

    RemoveParam* param = (RemoveParam*)lpParameter;

    typedef HRESULT (*tfn_Get3DEngineInterface)(void**);

    HMODULE hModeue = param->LoadLibraryA(param->LibName);

    if (!hModeue)
        return 0;

    bNeedFreeLib = true;

    tfn_Get3DEngineInterface Get3DEngineInterface = (tfn_Get3DEngineInterface)param->GetProcAddress(hModeue, param->PrcName);

    if (!Get3DEngineInterface)
        return 0;

    IKG3DEngineManager* pManager = NULL;
    Get3DEngineInterface((void**)&pManager);

    if (!pManager)
        return 0;

    if (param->CallType == Call_Get_Render)
        param->Param2 = (DWORD)pManager->GetRenderOption((RENDER_OPTION)param->Param1);
    else if (param->CallType == Call_Set_Render)
        pManager->SetRenderOption((RENDER_OPTION)param->Param1, (BOOL)param->Param2);
    else if (param->CallType == Call_Get_FrameMove)
        param->Param2 = (DWORD)pManager->GetFrameMoveOption((FRAMEMOVE_OPTION)param->Param1);
    else if (param->CallType == Call_Set_FrameMove)
        pManager->SetFrameMoveOption((FRAMEMOVE_OPTION)param->Param1, (BOOL)param->Param2);
    else if (param->CallType == Call_Get_Render_Param)
        param->Param2 = pManager->GetRenderParam((RENDER_PARAM)param->Param1);
    else if (param->CallType == Call_Set_Render_Param)
        pManager->SetRenderParam((RENDER_PARAM)param->Param1, param->Param2);
    else
        ;

    param->FreeLibrary(hModeue);

    return 1;
}

#pragma optimize( "", on  )
#pragma runtime_checks( "", restore  )



BOOL Ckg3ddebugerApp::GetRenderOption(RENDER_OPTION opt)
{
    if (!m_hHookProcess)
        return FALSE;

    m_RemoveParam.CallType = Call_Get_Render;
    m_RemoveParam.Param1 = opt;
    m_RemoveParam.Param2 = FALSE;

    if (WriteProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&m_RemoveParam, sizeof(RemoveParam), NULL))
    {
        WaitForSingleObject(
            CreateRemoteThread(m_hHookProcess, NULL, 0, (PTHREAD_START_ROUTINE)m_pRemoveCode, (LPVOID)m_pRemoveData, 0, NULL),
            INFINITE
            );
        RemoveParam Result;
        ReadProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&Result, sizeof(RemoveParam), NULL);

        return (BOOL)Result.Param2;
    }
    
    return FALSE;
}

HRESULT Ckg3ddebugerApp::SetRenderOption(RENDER_OPTION opt, BOOL bEnable)
{
    if (!m_hHookProcess)
        return E_FAIL;

    m_RemoveParam.CallType = Call_Set_Render;
    m_RemoveParam.Param1 = opt;
    m_RemoveParam.Param2 = (DWORD)bEnable;

    if (WriteProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&m_RemoveParam, sizeof(RemoveParam), NULL))
    {
        WaitForSingleObject(
            CreateRemoteThread(m_hHookProcess, NULL, 0, (PTHREAD_START_ROUTINE)m_pRemoveCode, (LPVOID)m_pRemoveData, 0, NULL),
            INFINITE
            );
        return S_OK;
    }

    return E_FAIL;
}


BOOL Ckg3ddebugerApp::GetFrameMoveOption(FRAMEMOVE_OPTION opt)
{
    if (!m_hHookProcess)
        return FALSE;

    m_RemoveParam.CallType = Call_Get_FrameMove;
    m_RemoveParam.Param1 = opt;
    m_RemoveParam.Param2 = FALSE;

    if (WriteProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&m_RemoveParam, sizeof(RemoveParam), NULL))
    {
        WaitForSingleObject(
            CreateRemoteThread(m_hHookProcess, NULL, 0, (PTHREAD_START_ROUTINE)m_pRemoveCode, (LPVOID)m_pRemoveData, 0, NULL),
            INFINITE
            );
        RemoveParam Result;
        ReadProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&Result, sizeof(RemoveParam), NULL);

        return (BOOL)Result.Param2;
    }

    return FALSE;
}

HRESULT Ckg3ddebugerApp::SetFrameMoveOption(FRAMEMOVE_OPTION opt, BOOL bEnable)
{
    if (!m_hHookProcess)
        return E_FAIL;

    m_RemoveParam.CallType = Call_Set_FrameMove;
    m_RemoveParam.Param1 = opt;
    m_RemoveParam.Param2 = (DWORD)bEnable;

    if (WriteProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&m_RemoveParam, sizeof(RemoveParam), NULL))
    {
        WaitForSingleObject(
            CreateRemoteThread(m_hHookProcess, NULL, 0, (PTHREAD_START_ROUTINE)m_pRemoveCode, (LPVOID)m_pRemoveData, 0, NULL),
            INFINITE
            );
        return S_OK;
    }

    return E_FAIL;
}

DWORD Ckg3ddebugerApp::GetRenderParam(RENDER_PARAM opt)
{
    if (!m_hHookProcess)
        return FALSE;

    m_RemoveParam.CallType = Call_Get_Render_Param;
    m_RemoveParam.Param1 = opt;
    m_RemoveParam.Param2 = 0;

    if (WriteProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&m_RemoveParam, sizeof(RemoveParam), NULL))
    {
        WaitForSingleObject(
            CreateRemoteThread(m_hHookProcess, NULL, 0, (PTHREAD_START_ROUTINE)m_pRemoveCode, (LPVOID)m_pRemoveData, 0, NULL),
            INFINITE
            );
        RemoveParam Result;
        ReadProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&Result, sizeof(RemoveParam), NULL);

        return (DWORD)Result.Param2;
    }

    return FALSE;
}

HRESULT Ckg3ddebugerApp::SetRenderParam(RENDER_PARAM opt, DWORD Value)
{
    if (!m_hHookProcess)
        return E_FAIL;

    m_RemoveParam.CallType = Call_Set_Render_Param;
    m_RemoveParam.Param1 = opt;
    m_RemoveParam.Param2 = (DWORD)Value;

    if (WriteProcessMemory(m_hHookProcess, m_pRemoveData, (PVOID)&m_RemoveParam, sizeof(RemoveParam), NULL))
    {
        WaitForSingleObject(
            CreateRemoteThread(m_hHookProcess, NULL, 0, (PTHREAD_START_ROUTINE)m_pRemoveCode, (LPVOID)m_pRemoveData, 0, NULL),
            INFINITE
            );
        return S_OK;
    }

    return E_FAIL;
}


BOOL Ckg3ddebugerApp::InitInstance()
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
	SetRegistryKey(_T("KG3DEngine Debug Tool"));

    TOKEN_PRIVILEGES tkp;
    HANDLE hToken;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        MessageBox(NULL, TEXT("OpenProcessToken Error :("), TEXT("Error"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    ZeroMemory(&m_RemoveParam, sizeof(m_RemoveParam));

    HMODULE hModule = LoadLibraryA("Kernel32.dll");

    m_RemoveParam.LoadLibraryA = (HMODULE (WINAPI *)(LPCSTR))GetProcAddress(hModule, "LoadLibraryA");
    m_RemoveParam.FreeLibrary = (BOOL (WINAPI *)(HMODULE))GetProcAddress(hModule, "FreeLibrary");
    m_RemoveParam.GetProcAddress = (FARPROC (WINAPI *)(HMODULE, LPCSTR))GetProcAddress(hModule, "GetProcAddress");
    
    strcpy(m_RemoveParam.PrcName, "Get3DEngineInterface");
    FreeLibrary(hModule);

    DWORD aProcessIds[1024];
    DWORD NumProcess = 0;
    DWORD cbSize = 0;
    vector<tstring> AttNames;
    vector<HANDLE> Handles;

    if (EnumProcesses(aProcessIds, sizeof(aProcessIds), &cbSize))
    {
        NumProcess = cbSize / sizeof(DWORD);

        for (DWORD i = 0; i < NumProcess; ++i)
        {
            HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, aProcessIds[i]);

            if (hProcess == NULL)
                continue;

            TCHAR ProcName[128];
            GetProcessImageFileName(hProcess, ProcName, sizeof(ProcName));

            for (size_t s = 0; s < _tcslen(ProcName); ++s)
                ProcName[s] = _totlower(ProcName[s]);

            if (_tcsstr(ProcName, TEXT("sceneeditord.exe"))  ||
                _tcsstr(ProcName, TEXT("sceneeditor.exe"))   || 
                _tcsstr(ProcName, TEXT("jx3clientd.exe"))    ||
                _tcsstr(ProcName, TEXT("jx3client.exe"))     ||
                _tcsstr(ProcName, TEXT("jx3benchmarkd.exe")) ||
                _tcsstr(ProcName, TEXT("jx3benchmark.exe")) )
            {
                Handles.push_back(hProcess);
                AttNames.push_back(ProcName);

                /*
                m_hHookProcess = hProcess;

                TCHAR* pName = _tcsrchr(ProcName, TEXT('\\'));
                if (pName)
                    m_strHookName = ++pName;
                    */

                continue;
            }

            CloseHandle( hProcess );
        }
    }

    CSelDlg selDlg(AttNames);
    selDlg.DoModal();

    int sel =  selDlg.GetSelProc();
    if (sel < 0)
    {
        MessageBox(NULL, TEXT("\tNo Process Be Attach :(\t\t"), TEXT("Warnning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    m_hHookProcess = Handles[sel];
    m_strHookName = AttNames[sel].c_str();

    for (size_t i = 0; i < Handles.size(); ++i)
    {
        if (i != sel)
            CloseHandle(Handles[i]);
    }

    if (m_hHookProcess)
    {
        HMODULE aModules[1024];
        DWORD cbSize = 0;
        DWORD NumModules = 0;

        EnumProcessModules(m_hHookProcess, aModules, sizeof(aModules), &cbSize);

        NumModules = cbSize / sizeof(DWORD);

        for (DWORD i = 0; i < NumModules; ++i)
        {
            TCHAR ModuName[128];

            if (!i)
            {
                GetModuleFileNameEx(m_hHookProcess, aModules[i], ModuName, sizeof(ModuName));
                m_strHookName = ModuName;
            }

            GetModuleBaseName(m_hHookProcess, aModules[i], ModuName, sizeof(ModuName));

            for (size_t s = 0; s < _tcslen(ModuName); ++s)
                ModuName[s] = _totlower(ModuName[s]);

            if (_tcsstr(ModuName, TEXT("kg3dengined.dll")))
            {
                strcpy(m_RemoveParam.LibName, "kg3dengined.dll");
                break;
            }

            if (_tcsstr(ModuName, TEXT("kg3dengine.dll")))
            {
                strcpy(m_RemoveParam.LibName, "kg3dengine.dll");
                break;
            }
        }

        if (!*m_RemoveParam.LibName)
        {
            CloseHandle(m_hHookProcess);
            m_hHookProcess = NULL;
        }
    }

    if (!m_hHookProcess)
    {
        MessageBox(NULL, TEXT("\tNo Process Be Attach :(\t\t"), TEXT("Warnning"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    m_pRemoveData = VirtualAllocEx(m_hHookProcess, NULL, sizeof(RemoveParam), MEM_COMMIT, PAGE_READWRITE);   
    m_pRemoveCode = VirtualAllocEx(m_hHookProcess, NULL, RemoveCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    if (!m_pRemoveCode || !m_pRemoveData)
    {
        Release();
        MessageBox(NULL, TEXT("\tVirtualAllocEx Error :(\t\t"), TEXT("Error"), MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    BYTE* ProcAddr = (BYTE*)&ThreadProc;

#ifdef _DEBUG 
    void* StartAddr = ProcAddr + *(WORD*)(ProcAddr + 1) + 5;
#else
    void* StartAddr = ProcAddr;
#endif


    DWORD NumWrite = 0;

    if (!WriteProcessMemory(m_hHookProcess, m_pRemoveCode, StartAddr, RemoveCodeSize, &NumWrite))
    {
        MessageBox(NULL, TEXT("\tWriteProcessMemory Error :(\t\t"), TEXT("Error"), MB_OK | MB_ICONWARNING);
        Release();
        return FALSE;
    }

	Ckg3ddebugerDlg dlg;
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

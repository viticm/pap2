///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KSystemCheck.cpp
// Creator  : zhongguangze
// Date     : 
// Comment  : 
//
///////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KSystemCheck.h"
#include <io.h>

/////////////////////////////////////////////////////////////////////////////////
int _CheckKPPAvailable();
int _InstallKPP(const TCHAR cszCurrentDir[]);

int _CheckDirectXAvailable();
int _InstallDirectX(const TCHAR cszCurrentDir[]);

int _CreateAndWaitProcess(const TCHAR cszCommandLine[], DWORD dwWaitMilliseconds);

////////////////////////////////////////////////////////////////////////////////
int KGU_CheckGameEnvironment()
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szCurrentDir[MAX_PATH];
    tstring strCurrentDir;

    // 3rdParty directory is in current directory, not exe file path, because of multi-version.
    nRetCode = GetCurrentDirectory(_countof(szCurrentDir), szCurrentDir);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szCurrentDir));

    strCurrentDir = szCurrentDir;
    nRetCode = (int)strCurrentDir.length();
    ASSERT(nRetCode > 0);
    if (strCurrentDir[nRetCode - 1] != _T('\\') && strCurrentDir[nRetCode - 1] != _T('/'))
        strCurrentDir += _T('\\');

    nRetCode = _CheckKPPAvailable();
    if (!nRetCode)
    {
        nRetCode = _InstallKPP(strCurrentDir.c_str());
        // KGLOG_PROCESS_ERROR(nRetCode && "_InstallKPP()");   // KPP is not the necessary.
    }

    nRetCode = _CheckDirectXAvailable();
    if (!nRetCode)
    {
        nRetCode = _InstallDirectX(strCurrentDir.c_str());
        if (nRetCode)
        {
            MessageBox(
                NULL,
                _T("安装或更新了剑网3所依赖的组件，可能需要重新启动机器！\n")
                _T("如无法运行剑网3客户端，请重新启动机器再尝试！\n\n")
                _T("点击“确定”继续"),
                _T("剑网3更新程序"),
                MB_OK | MB_ICONINFORMATION
            );
        }
        else
        {
            MessageBox(
                NULL,
                _T("安装DirectX运行包失败！\n")
                _T("您的系统未安装剑网3依赖的组件，自动安装时出错，请到官方网站下载并手动安装！"),
                _T("剑网3更新程序"),
                MB_OK | MB_ICONERROR
            );
        }
    }
    
    nResult = true;
Exit0:
    return nResult;
}

int _CheckKPPAvailable()
{
    int nResult  = false;
    int nRetCode = false;
    HKEY hKey = NULL;
    DWORD dwValue  = 0;
    DWORD dwValueSize = 0;

    nRetCode = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Kingsoft\\KPP2"), 0, KEY_QUERY_VALUE, &hKey
    );
    KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

    dwValueSize = sizeof(dwValue);
    nRetCode = RegQueryValueEx(hKey, _T("version"), NULL, NULL, (LPBYTE)&dwValue, &dwValueSize);
    KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

    KG_PROCESS_ERROR(dwValue > 0);    // > 0 indicate installed.

    nResult = true;
Exit0:
    if (hKey)
    {
        RegCloseKey(hKey);
        hKey = NULL;
    }
    return nResult;
}

int _InstallKPP(const TCHAR cszCurrentDir[])
{
	int nResult  = false;
	int nRetCode = false;
    tstring strInstallFileName;
    tstring strCommandLine;
    const DWORD INSTALL_WAIT_TIMEOUT = 30 * 1000;

    ASSERT(cszCurrentDir);
    ASSERT(cszCurrentDir[0]);

    strInstallFileName = tstring(cszCurrentDir) + _T("3rdParty\\kppjw3.exe");
    strCommandLine = _T("\"") + strInstallFileName + _T("\"");

    nRetCode = _CreateAndWaitProcess(strCommandLine.c_str(), INSTALL_WAIT_TIMEOUT);
    KGLOG_PROCESS_ERROR(nRetCode && "_CreateAndWaitProcess()");

	nResult = true;
Exit0:
	return nResult;
}

int _CheckDirectXAvailable()
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szSystemDirectory[MAX_PATH];
    tstring strDLLFileName;

    nRetCode = GetSystemDirectory(szSystemDirectory, _countof(szSystemDirectory));
    KG_PROCESS_ERROR(nRetCode > 0);
    KG_PROCESS_ERROR(nRetCode < _countof(szSystemDirectory));
    strDLLFileName = tstring(szSystemDirectory) + _T("\\D3DX9_42.dll");
    
    nRetCode = _taccess(strDLLFileName.c_str(), 0);
    KG_PROCESS_ERROR(nRetCode == 0);     // D3D had been installed if DLL file exists.

    nResult = true;
Exit0:
    return nResult;
}

int _InstallDirectX(const TCHAR cszCurrentDir[])
{
	int nResult  = false;
	int nRetCode = false;
    TCHAR szSystemDirectory[MAX_PATH];
    tstring strNecessaryFileName;
    tstring strInstallFileName;
    tstring strCommandLine;
    LONGLONG llAvailableDiskSize = 0;
    const LONGLONG NEED_DISK_SIZE = 50 * 1024 * 1024;
    const DWORD INSTALL_WAIT_TIMEOUT = INFINITE;

    ASSERT(cszCurrentDir);
    ASSERT(cszCurrentDir[0]);
    
    nRetCode = GetSystemDirectory(szSystemDirectory, _countof(szSystemDirectory));
    KG_PROCESS_ERROR(nRetCode > 0);
    KG_PROCESS_ERROR(nRetCode < _countof(szSystemDirectory));

    nRetCode = GetDiskFreeSpaceEx(szSystemDirectory, (PULARGE_INTEGER)&llAvailableDiskSize, NULL, NULL);
    KGLOG_PROCESS_ERROR(nRetCode && "GetDiskFreeSpaceEx()");

    if (llAvailableDiskSize < NEED_DISK_SIZE)
    {
        MessageBox(
            NULL, _T("系统盘磁盘空间不足, 安装DirectX运行包失败！"), _T("剑网3更新程序"), NULL
        );
        KGLOG_PROCESS_ERROR(false && "System disk space not enough.");
    }

    // Check the DirectX version is bigger than Aug2009.
    strNecessaryFileName = tstring(cszCurrentDir) + _T("3rdParty\\dx9_42\\Aug2009_d3dx9_42_x86.cab");
    nRetCode = _taccess(strNecessaryFileName.c_str(), 0);
    KGLOG_PROCESS_ERROR(nRetCode == 0 && "DirectX version too lower.");

    strInstallFileName = tstring(cszCurrentDir) + _T("3rdParty\\dx9_42\\DXSETUP.exe");
    strCommandLine = _T("\"") + strInstallFileName + _T("\" /Silent");

    nRetCode = _CreateAndWaitProcess(strCommandLine.c_str(), INSTALL_WAIT_TIMEOUT);
    KGLOG_PROCESS_ERROR(nRetCode && "_CreateAndWaitProcess()");

	nResult = true;
Exit0:
	return nResult;
}

int _CreateAndWaitProcess(const TCHAR cszCommandLine[], DWORD dwWaitMilliseconds)
{
    int nResult  = false;
    int nRetCode = false;
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInfo = { 0 };

    ASSERT(cszCommandLine);
    ASSERT(cszCommandLine[0]);

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);

    nRetCode = CreateProcess(
        NULL, (LPSTR)cszCommandLine, NULL, NULL, false, 0, NULL, NULL, &StartupInfo, &ProcessInfo
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = WaitForSingleObject(ProcessInfo.hProcess, dwWaitMilliseconds);
    KGLOG_PROCESS_ERROR(nRetCode == WAIT_OBJECT_0);
    
    nResult = true;
Exit0:
    if (ProcessInfo.hProcess)
    {
        CloseHandle(ProcessInfo.hProcess);
        ProcessInfo.hProcess = NULL;
    }
    if (ProcessInfo.hThread)
    {
        CloseHandle(ProcessInfo.hThread);
        ProcessInfo.hThread = NULL;
    }
    return nResult;
}
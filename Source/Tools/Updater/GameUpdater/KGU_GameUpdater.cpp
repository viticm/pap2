////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KGU_GameUpdater.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-7-7 17:48:30
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>
#include <direct.h>
#include "KGU_GameUpdater.h"
#include "KGU_SelfUpdater.h"
#include "unzip.h"
#include "KGU_Public.h"
#include "KGameUpdater.h"
#include "KFlashWindow.h"

////////////////////////////////////////////////////////////////////////////////
const TCHAR AUTO_UPDATE_WITH_WAIT_FLAG[]            = _T("-w");
const TCHAR AUTO_UPDATE_WITHOUT_WAIT_FLAG[]         = _T("-a");

////////////////////////////////////////////////////////////////////////////////
KGU_GameUpdater::KGU_GameUpdater()
{
    memset(&m_Parameter, 0, sizeof(m_Parameter));
}

KGU_GameUpdater::~KGU_GameUpdater()
{
}

int KGU_GameUpdater::Init(const TCHAR cszCommandLine[], void *pvContext)
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szApplicationName[MAX_PATH];

    KGLOG_PROCESS_ERROR(cszCommandLine);
    // KGLOG_PROCESS_ERROR(cszCommandLine[0]); // Command line could be empty string.

    KG_USE_ARGUMENT(pvContext);

    nRetCode = GetModuleFileName(
        NULL, szApplicationName, sizeof(szApplicationName) / sizeof(szApplicationName[0])
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szApplicationName) / sizeof(szApplicationName[0]));

    m_strApplicationName = szApplicationName;

    m_strApplicationPath = szApplicationName;
    nRetCode = (int)m_strApplicationPath.find_last_of(_T('\\'));
    KGLOG_PROCESS_ERROR(nRetCode != tstring::npos);
    m_strApplicationPath.erase(nRetCode + 1);

    m_strTempFilePath = m_strApplicationPath + _T("update\\");

    nRetCode = CreateDirectory(m_strTempFilePath.c_str(), NULL);
    if (nRetCode == false)  // Create Directory if not exist.
    {
        nRetCode = GetLastError();
        KGLOG_PROCESS_ERROR(nRetCode == ERROR_ALREADY_EXISTS);
    }

    nRetCode = _AnalyzeCommandLine(cszCommandLine);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    nResult = true;
Exit0:
    return nResult;
}

int KGU_GameUpdater::UnInit(void *pvContext)
{
    KG_USE_ARGUMENT(pvContext);

    if (m_Parameter.hSuccessSelfUpdateEvent)
    {
        CloseHandle(m_Parameter.hSuccessSelfUpdateEvent);
        m_Parameter.hSuccessSelfUpdateEvent = NULL;
    }
    if (m_Parameter.hFailedSelfUpdateEvent)
    {
        CloseHandle(m_Parameter.hFailedSelfUpdateEvent);
        m_Parameter.hFailedSelfUpdateEvent = NULL;
    }
    return true;
}

int KGU_GameUpdater::Run()
{
    int nResult  = false;
    int nRetCode = false;

    switch (m_Parameter.eType)
    {
    case TYPE_SELF_UPDATE:
        nRetCode = _SelfUpdate();
        KGLOG_PROCESS_ERROR(nRetCode && "_SelfUpdate()");
        break;
    case TYPE_AUTO_UPDATE_WITH_WAIT:
    case TYPE_AUTO_UPDATE_WITHOUT_WAIT:
    case TYPE_MANUAL_UPDATE:
        nRetCode = _PatchUpdate();
        KGLOG_PROCESS_ERROR(nRetCode && "_PatchUpdate()");
        break;
    default:
        ASSERT(false);
        KGLOG_PROCESS_ERROR(false);
        break;
    }
    
    nResult = true;
Exit0:
    return nResult;
}

int KGU_GameUpdater::_AnalyzeCommandLine(const TCHAR cszCommandLine[])
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uBufferSize = 0;

    ASSERT(cszCommandLine);
    KGLogPrintf(KGLOG_INFO, "Command line : %s", cszCommandLine);

    memset(&m_Parameter, 0, sizeof(m_Parameter));

    if (cszCommandLine[0] == _T('\0'))
    {
        m_Parameter.eType = TYPE_SELF_UPDATE;
        KG_PROCESS_SUCCESS(true);
    }

    nRetCode = _tcsnicmp(
        cszCommandLine, AUTO_UPDATE_WITH_WAIT_FLAG, _tcslen(AUTO_UPDATE_WITH_WAIT_FLAG)
    );
    if (nRetCode == 0)
    {   
        m_Parameter.eType = TYPE_AUTO_UPDATE_WITH_WAIT;

        nRetCode = _stscanf(
            cszCommandLine + _tcslen(AUTO_UPDATE_WITH_WAIT_FLAG), _T("%p %p"),
            &(m_Parameter.hSuccessSelfUpdateEvent), &(m_Parameter.hFailedSelfUpdateEvent)
        );
        KGLOG_PROCESS_ERROR(nRetCode == 2);
        KGLOG_PROCESS_ERROR(m_Parameter.hSuccessSelfUpdateEvent);
        KGLOG_PROCESS_ERROR(m_Parameter.hFailedSelfUpdateEvent);

        KG_PROCESS_SUCCESS(true);
    }

    nRetCode = _tcsnicmp(
        cszCommandLine, AUTO_UPDATE_WITHOUT_WAIT_FLAG, _tcslen(AUTO_UPDATE_WITHOUT_WAIT_FLAG)
    );
    if (nRetCode == 0)
    {
        m_Parameter.eType = TYPE_AUTO_UPDATE_WITHOUT_WAIT;
        KG_PROCESS_SUCCESS(true);
    }

    m_Parameter.eType = TYPE_MANUAL_UPDATE;
    uBufferSize = sizeof(m_Parameter.szManualPackageName) / sizeof(m_Parameter.szManualPackageName);
    _tcsncpy(m_Parameter.szManualPackageName, cszCommandLine, uBufferSize);
    KGLOG_PROCESS_ERROR(m_Parameter.szManualPackageName[uBufferSize - 1] == _T('\0'));

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_Parameter.hSuccessSelfUpdateEvent)
        {
            CloseHandle(m_Parameter.hSuccessSelfUpdateEvent);
            m_Parameter.hSuccessSelfUpdateEvent = NULL;
        }
        if (m_Parameter.hFailedSelfUpdateEvent)
        {
            CloseHandle(m_Parameter.hFailedSelfUpdateEvent);
            m_Parameter.hFailedSelfUpdateEvent = NULL;
        }
    }
    return nResult;
}

int KGU_GameUpdater::_SelfUpdate()
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uRunningProcessCount = 0;
    KGU_SelfUpdater SelfUpdater;
    int nSelfUpdaterInitFlag = false;
    int nNeedSelfUpdateFlag  = false;
    SECURITY_ATTRIBUTES SecurityAttributes;
    HANDLE hSucessEvent = NULL;
    HANDLE hFailedEvent = NULL;
    HANDLE hAutoUpdateWithWaitProcess    = NULL;
    HANDLE hAutoUpdateWithoutWaitProcess = NULL;
    
    // -------------------- Make sure only one gameupdater.exe process -------------
    while (true)
    {
        nRetCode = KGU_GetRunningProcessCount(m_strApplicationName.c_str(), &uRunningProcessCount);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (uRunningProcessCount <= 1)
            break;

        nRetCode = MessageBox(
            NULL, _T("剑网3更新程序正在运行，关闭后再尝试更新"), _T("剑网3更新程序"),
            MB_ICONEXCLAMATION | MB_ICONWARNING | MB_RETRYCANCEL
        );
        KGLOG_PROCESS_ERROR(nRetCode == IDRETRY);
    }
    // -------------------------------------------------------------------------

    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.lpSecurityDescriptor = NULL;
    SecurityAttributes.bInheritHandle = true;   // The self update event could be inherited by auto update process.

    hSucessEvent = CreateEvent(&SecurityAttributes, false, false, NULL);
    KGLOG_PROCESS_ERROR(hSucessEvent);

    hFailedEvent = CreateEvent(&SecurityAttributes, false, false, NULL);
    KGLOG_PROCESS_ERROR(hFailedEvent);

    hAutoUpdateWithWaitProcess = _CreateAutoUpdateWithWaitProcess(hSucessEvent, hFailedEvent);
    // KGLOG_PROCESS_ERROR(hAutoUpdateWithWaitProcess);

    nRetCode = SelfUpdater.Init(
        m_strApplicationName.c_str(), m_strApplicationPath.c_str(), m_strTempFilePath.c_str(), NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode && "SelfUpdate.Init()");
    nSelfUpdaterInitFlag = true;

    nRetCode = SelfUpdater.CheckNeedUpdate(&nNeedSelfUpdateFlag);
    KGLOG_PROCESS_ERROR(nRetCode && "SelfUpdater.CheckNeedUpdate()");

    KG_PROCESS_SUCCESS(nNeedSelfUpdateFlag == false);   // Not need self-update.

    nRetCode = SelfUpdater.DownloadFiles();
    KGLOG_PROCESS_ERROR(nRetCode && "SelfUpdater.DownloadFiles()");

    if (hAutoUpdateWithWaitProcess) // To avoiding the affect of The auto update process.
    {
        nRetCode = TerminateProcess(hAutoUpdateWithWaitProcess, 0);
        // KGLOG_PROCESS_ERROR(nRetCode);  // The process may be end.
    }

    nRetCode = SelfUpdater.UpdateFiles();
    KGLOG_PROCESS_ERROR(nRetCode && "SelfUpdater.UpdateFiles()");

    hAutoUpdateWithoutWaitProcess = _CreateAutoUpdateWithoutWaitProcess();
    KGLOG_PROCESS_ERROR(hAutoUpdateWithoutWaitProcess);
    
Exit1:
    nResult = true;
Exit0:
    // Self update finish, set corresponding event to resume auto update.
    if (nResult && hSucessEvent)
    {
        SetEvent(hSucessEvent);
    }
    if (!nResult && hFailedEvent)
    {
        SetEvent(hFailedEvent);
    }
    if (nSelfUpdaterInitFlag)
    {
        SelfUpdater.UnInit(NULL);
        nSelfUpdaterInitFlag = true;
    }
    if (hAutoUpdateWithoutWaitProcess)
    {
        CloseHandle(hAutoUpdateWithoutWaitProcess);
        hAutoUpdateWithoutWaitProcess = NULL;
    }
    if (hAutoUpdateWithWaitProcess)
    {
        CloseHandle(hAutoUpdateWithWaitProcess);
        hAutoUpdateWithWaitProcess = NULL;
    }
    if (hFailedEvent)
    {
        CloseHandle(hFailedEvent);
        hFailedEvent = NULL;
    }
    if (hSucessEvent)
    {
        CloseHandle(hSucessEvent);
        hSucessEvent = NULL;
    }
    return nResult;
}

int KGU_GameUpdater::_PatchUpdate()
{
    int nResult  = false;
    int nRetCode = false;
    int nGameUpdaterInitFlag = false;
    KGameUpdater GameUpdater;
    tstring strSwfFileName;

    // Check the Flash had been installed and make sure flash window could be created.
    nRetCode = _MakeSureFlashInstalled();
    KGLOG_PROCESS_ERROR(nRetCode && "_MakeSureFlashInstalled()");

    strSwfFileName = m_strApplicationPath + _T("update.swf");
    nRetCode = _ExtractSwfFile(strSwfFileName.c_str());
    KGLOG_PROCESS_ERROR(nRetCode && "_ExtractSwfFile()");

    nRetCode = GameUpdater.Init(strSwfFileName.c_str());
    KGLOG_PROCESS_ERROR(nRetCode && "GameUpdater.Init()");
    nGameUpdaterInitFlag = true;

    // Remove swf file if the flash interface window is created.
    nRetCode = _RemoveSwfFile(strSwfFileName.c_str());
    // KGLOG_PROCESS_ERROR(nRetCode && "_RemoveSwfFile()");

    if ((m_Parameter.eType == TYPE_AUTO_UPDATE_WITH_WAIT) ||
        (m_Parameter.eType == TYPE_AUTO_UPDATE_WITHOUT_WAIT)
    )
    {
        nRetCode = GameUpdater.AutoUpdate(
            m_Parameter.hSuccessSelfUpdateEvent, m_Parameter.hFailedSelfUpdateEvent
        );
        KGLOG_PROCESS_ERROR(nRetCode && "GameUpdater.AutoUpdate()");
    }
    else
    {
        ASSERT(m_Parameter.eType == TYPE_MANUAL_UPDATE);
        nRetCode = GameUpdater.ManualUpdate(m_Parameter.szManualPackageName);
        KGLOG_PROCESS_ERROR(nRetCode && "GameUpdater.ManualUpdate()");
    }

    nResult = true;
Exit0:
    if (nGameUpdaterInitFlag)
    {
        GameUpdater.UnInit();
        nGameUpdaterInitFlag = false;
    }
    if (!strSwfFileName.empty())
        _RemoveSwfFile(strSwfFileName.c_str());
    return nResult;
}

HANDLE KGU_GameUpdater::_CreateAutoUpdateWithWaitProcess(HANDLE hSuccessEvent, HANDLE hFailedEvent)
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szComandLine[MAX_PATH + 256];
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInfo = { 0 };

    ASSERT(hSuccessEvent);
    ASSERT(hFailedEvent);

    nRetCode = _sntprintf(
        szComandLine, sizeof(szComandLine) / sizeof(szComandLine[0]), "\"%s\" %s %p %p",
        m_strApplicationName.c_str(), AUTO_UPDATE_WITH_WAIT_FLAG, hSuccessEvent, hFailedEvent
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szComandLine) / sizeof(szComandLine[0]));

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);

    // Sleep 1 second and make sure new process with a new log file.
    Sleep(1000);

    nRetCode = CreateProcess(
        NULL, szComandLine, NULL, NULL, true,   // Inherit self update event.
        0, NULL, NULL, &StartupInfo, &ProcessInfo
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (ProcessInfo.hProcess)
        {
            CloseHandle(ProcessInfo.hProcess);
            ProcessInfo.hProcess = NULL;
        }
    }
    if (ProcessInfo.hThread)    // No use.
    {
        CloseHandle(ProcessInfo.hThread);
        ProcessInfo.hThread = NULL;
    }
    return ProcessInfo.hProcess;
}

HANDLE KGU_GameUpdater::_CreateAutoUpdateWithoutWaitProcess()
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szComandLine[MAX_PATH + 256];
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInfo = { 0 };

    nRetCode = _sntprintf(
        szComandLine, sizeof(szComandLine) / sizeof(szComandLine[0]), "\"%s\" %s",
        m_strApplicationName.c_str(), AUTO_UPDATE_WITHOUT_WAIT_FLAG
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < sizeof(szComandLine) / sizeof(szComandLine[0]));

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);

    nRetCode = CreateProcess(
        NULL, szComandLine, NULL, NULL, false,  // Not inherit self update event.
        0, NULL, NULL, &StartupInfo, &ProcessInfo
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (ProcessInfo.hProcess)
        {
            CloseHandle(ProcessInfo.hProcess);
            ProcessInfo.hProcess = NULL;
        }
    }
    if (ProcessInfo.hThread)    // No use.
    {
        CloseHandle(ProcessInfo.hThread);
        ProcessInfo.hThread = NULL;
    }
    return ProcessInfo.hProcess;
}

int KGU_GameUpdater::_MakeSureFlashInstalled()
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szCurrentDir[MAX_PATH];
    tstring strCurrentDir;
    tstring strInstallFileName;
    tstring strCommandLine;
    unsigned uRunningCount = 0;
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInfo = { 0 };

    nRetCode = KG_CheckFlashAvailable();
    KG_PROCESS_SUCCESS(nRetCode);

    nRetCode = GetCurrentDirectory(_countof(szCurrentDir), szCurrentDir);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szCurrentDir));

    strCurrentDir = szCurrentDir;
    nRetCode = (int)strCurrentDir.length();
    ASSERT(nRetCode > 0);
    if (strCurrentDir[nRetCode - 1] != _T('\\') && strCurrentDir[nRetCode - 1] != _T('/'))
        strCurrentDir += _T('\\');

    strInstallFileName = strCurrentDir + _T("3rdParty\\install_flash_player_10_active_x.exe");
    strCommandLine = _T("\"") + strInstallFileName + _T("\" /s");

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);
    
    nRetCode = CreateProcess(
        NULL, (LPTSTR)strCommandLine.c_str(), NULL, NULL, false, 
        NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
    KGLOG_PROCESS_ERROR(nRetCode == WAIT_OBJECT_0);
    
Exit1:
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

int KGU_GameUpdater::_ExtractSwfFile(const TCHAR cszSwfFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    HZIP hZip = NULL;
    ZRESULT zResult = ZR_FLATE;
    int nIndex = -1;
    ZIPENTRY ZipEntry;

    ASSERT(cszSwfFileName);
    ASSERT(cszSwfFileName[0]);

#ifdef _DEBUG
    // If swf file exist when debug mode, ignore extracting, which is convenient for art colleague adjust swf file.
    nRetCode = _taccess(cszSwfFileName, 0);
    KG_PROCESS_SUCCESS(nRetCode == 0);
#endif // _DEBUG

    // Remove the existed file and use the file compressed in gameupdater.exe.
    nRetCode = SetFileAttributes(cszSwfFileName, FILE_ATTRIBUTE_NORMAL);
    // KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = _tremove(cszSwfFileName);
    // KGLOG_PROCESS_ERROR(nRetCode == 0);

    hZip = OpenZip(m_strApplicationName.c_str(), "Nubrefi3h78dfJKdf1DFJsw");
    KGLOG_PROCESS_ERROR(hZip);

    zResult = FindZipItem(hZip, _T("update.swf"), true, &nIndex, &ZipEntry);
    KGLOG_PROCESS_ERROR(zResult == ZR_OK);
    KGLOG_PROCESS_ERROR(nIndex >= 0);

    zResult = UnzipItem(hZip, nIndex, cszSwfFileName);
    KGLOG_PROCESS_ERROR(zResult == ZR_OK);

Exit1:
    nResult = true;
Exit0:
    if (hZip)
    {
        CloseZip(hZip);
        hZip = NULL;
    }
    return nResult;
}

int KGU_GameUpdater::_RemoveSwfFile(const TCHAR cszSwfFileName[])
{
    int nResult  = false;
    int nRetCode = false;

    ASSERT(cszSwfFileName);
    ASSERT(cszSwfFileName[0]);

#ifdef _DEBUG
    // Do not remove swf file when debug mode.
    KG_PROCESS_SUCCESS(true);
#endif  // _DEBUG

    nRetCode = SetFileAttributes(cszSwfFileName, FILE_ATTRIBUTE_NORMAL);
    // KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = _tremove(cszSwfFileName);
    KGLOG_PROCESS_ERROR(nRetCode == 0);
    
Exit1:
    nResult = true;
Exit0:
    return nResult;   
}
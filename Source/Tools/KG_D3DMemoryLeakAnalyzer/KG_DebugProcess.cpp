////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_DebugProcess.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-7-27 17:41:17
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG_DebugProcess.h"
#include <shlwapi.h>
#include <DbgHelp.h>
#include <Psapi.h>

using namespace std;

int g_nShowDebugInfo = false;

////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK SymCallbackProc64(
    HANDLE  hProcess,
    ULONG   ActionCode,
    ULONG64 CallbackData,
    ULONG64 UserContext
)
{
    KG_USE_ARGUMENT(hProcess);

    KG_DebugProcess *pDbg = (KG_DebugProcess *)UserContext;
    ASSERT(pDbg);

    switch (ActionCode)
    {
    case CBA_DEBUG_INFO:
        if (g_nShowDebugInfo)
        {
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] Debug Info: %s\n", (char *)CallbackData);
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo);
        }
        break;
    case CBA_DEFERRED_SYMBOL_LOAD_CANCEL:
        if (g_nShowDebugInfo)
        {
            //The CallbackData parameter should be ignored
            //pDbg->OutputDebugString("[CBA] CBA_DEFERRED_SYMBOL_LOAD_CANCEL");
            return false; // return false to not cancel the symbol load
        }
        break;
    case CBA_DEFERRED_SYMBOL_LOAD_COMPLETE:
        if (g_nShowDebugInfo)
        {
            IMAGEHLP_DEFERRED_SYMBOL_LOAD64 *pInfo = (IMAGEHLP_DEFERRED_SYMBOL_LOAD64 *)CallbackData;
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] Symbol load has complete: %s\n", pInfo->FileName);
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo); 
        }
        break;
    case CBA_DEFERRED_SYMBOL_LOAD_FAILURE:
        if (g_nShowDebugInfo)
        {
            IMAGEHLP_DEFERRED_SYMBOL_LOAD64 *pInfo = (IMAGEHLP_DEFERRED_SYMBOL_LOAD64 *)CallbackData;
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] Symbol load has failed: %s\n", pInfo->FileName);
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo);
        }
        break;
    case CBA_DEFERRED_SYMBOL_LOAD_PARTIAL:
        if (g_nShowDebugInfo)
        {
            IMAGEHLP_DEFERRED_SYMBOL_LOAD64 *pInfo = (IMAGEHLP_DEFERRED_SYMBOL_LOAD64 *)CallbackData;
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] Symbol load has partially completed: %s\n", pInfo->FileName);
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo);
        }
       break;
    case CBA_DEFERRED_SYMBOL_LOAD_START:
        if (g_nShowDebugInfo)
        {
            IMAGEHLP_DEFERRED_SYMBOL_LOAD64 *pInfo = (IMAGEHLP_DEFERRED_SYMBOL_LOAD64 *)CallbackData;
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] Symbol load start: %s\n", pInfo->FileName);
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo);
        }
       break;
    case CBA_DUPLICATE_SYMBOL:
        if (g_nShowDebugInfo)
        {
            IMAGEHLP_DUPLICATE_SYMBOL64 *pInfo = (IMAGEHLP_DUPLICATE_SYMBOL64 *)CallbackData;
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] Duplicate symbols were found: %s\n", pInfo->Symbol->Name);
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo);
        }
       break;
    case CBA_EVENT:
        if (g_nShowDebugInfo)
        {
            IMAGEHLP_CBA_EVENT *pInfo = (IMAGEHLP_CBA_EVENT *)CallbackData;
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] Event: %s", pInfo->desc);
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo);
        }
        break;
    case CBA_READ_MEMORY:
        if (g_nShowDebugInfo)
        {
            IMAGEHLP_CBA_READ_MEMORY *pInfo = (IMAGEHLP_CBA_READ_MEMORY *)CallbackData;
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] The loaded image has been read: (addr:%p, size:%u)\n", pInfo->addr, pInfo->bytes);
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo);
        }
       break;
    case CBA_SET_OPTIONS:
        if (g_nShowDebugInfo)
        {
            char szInfo[512];
            _snprintf(szInfo, countof(szInfo), "[CBA] Set Option to 0x%8X\n", SymGetOptions());
            szInfo[countof(szInfo) - 1] = '\0'; 
            pDbg->OutputDebugString(szInfo);
        }
        break;
    case CBA_SYMBOLS_UNLOADED:
        if (g_nShowDebugInfo)
        {
            //The CallbackData parameter should be ignored
            pDbg->OutputDebugString("[CBA] CBA_SYMBOLS_UNLOADED\n");
        }
        break;

    default:
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
int _GetFileNameFromHandle(HANDLE hFile, string *psRetFileName) 
{
    int nResult = false;
    int nRetCode = false;
    TCHAR szDriverList[MAX_PATH];
    TCHAR szFilename[MAX_PATH];
    TCHAR szDeviceName[MAX_PATH];
    HANDLE hFileMap = NULL;
    LARGE_INTEGER ullFileSize;
    void *pvMem = NULL;
    TCHAR *pchDriver = NULL;
    size_t uDeviceNameLen = 0;

    KG_ASSERT_EXIT(hFile);
    KG_ASSERT_EXIT(psRetFileName);

    nRetCode = ::GetFileSizeEx(hFile, &ullFileSize); 
    KG_ASSERT_EXIT(nRetCode);
    KG_ASSERT_EXIT(ullFileSize.QuadPart > 0 && "Cannot map a file with a length of zero.");

    hFileMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 1, NULL);
    KG_ASSERT_EXIT(hFileMap);

    pvMem = ::MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);
    KG_ASSERT_EXIT(pvMem);

    nRetCode = ::GetMappedFileName(GetCurrentProcess(), pvMem, szFilename, MAX_PATH);
    KG_ASSERT_EXIT(nRetCode);

    // ------------ Translate path with device name to drive letters.--------
    nRetCode = ::GetLogicalDriveStrings(MAX_PATH-1, szDriverList);
    KG_ASSERT_EXIT(nRetCode > 0);

    pchDriver = szDriverList;
    while (*pchDriver)
    {
        TCHAR szDrive[3] = TEXT(" :");
        *szDrive = *pchDriver;
        while (*pchDriver++); // Go to the next NULL character.

        // example: convert from "c:" to "\Device\HarddiskVolume1"
        nRetCode = ::QueryDosDevice(szDrive, szDeviceName, MAX_PATH);
        if (nRetCode == 0)
            continue;
        uDeviceNameLen = _tcslen(szDeviceName); // uDeviceNameLen < nRetCode

        nRetCode = _tcsnicmp(szFilename, szDeviceName, uDeviceNameLen);
        if (nRetCode != 0)
            continue;

        szFilename[0] = *szDrive;
        szFilename[1] = TCHAR(':');
        memmove(
            szFilename + sizeof(szFilename[0]) * 2, 
            szFilename + uDeviceNameLen,
            sizeof(szFilename[0]) * (MAX_PATH - uDeviceNameLen)
        );
       
        break; // found it
    } 

    *psRetFileName = szFilename;
    nResult = true;
Exit0:
    if (pvMem)
    {
        ::UnmapViewOfFile(pvMem);
        pvMem = NULL;
    }
    if (hFileMap)
    {
        ::CloseHandle(hFileMap);
        hFileMap = NULL;
    }
    return nResult;
}


////////////////////////////////////////////////////////////////////////////////
KG_DebugProcess::KG_DebugProcess(void):
    m_hProcess(NULL),
    m_pfCallBack(NULL),
    m_pvCallBackUserData(NULL),
    m_pLastBreakAddr(NULL),
    m_nMemUseStateChangeFlag(false),
    m_nEnableMemUseAnalyzeFlag(true),
    m_nExitFlag(false)
{
    ::InitializeCriticalSection(&m_MemUseLock);
}

KG_DebugProcess::~KG_DebugProcess(void)
{
    ::DeleteCriticalSection(&m_MemUseLock);
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::Create(
    const char cszAppPathName[],
    const char cszWorkPath[],
    const char *pcszSymbolSearchPath
)
{
    int nResult  = false;
    int nRetCode = false;
    PROCESS_INFORMATION ProcessInfo = { 0 };
    STARTUPINFO StartupInfo;

    KG_ASSERT_EXIT(cszAppPathName);
    KG_ASSERT_EXIT(cszWorkPath);
    KG_ASSERT_EXIT(pcszSymbolSearchPath);
    KG_ASSERT_EXIT(!m_hProcess);

    memset(&StartupInfo, 0, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);
    nRetCode = ::CreateProcess(
        cszAppPathName,  // LPCTSTR lpApplicationName
        NULL,             // LPTSTR lpCommandLine,
        NULL,             // lpProcessAttributes,
        NULL,             // lpThreadAttributes,
        FALSE,            // bInheritHandles,
        DEBUG_ONLY_THIS_PROCESS, // dwCreationFlags,
        NULL,                    // lpEnvironment,
        cszWorkPath,             // lpCurrentDirectory,
        &StartupInfo,            // lpStartupInfo,
        &ProcessInfo             // lpProcessInformation
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::SymInitialize(
        ProcessInfo.hProcess, 
        (char *)pcszSymbolSearchPath,
        FALSE
    );
    KG_ASSERT_EXIT(nRetCode);
    
    if (g_nShowDebugInfo)
    {
        nRetCode = ::SymRegisterCallback64(
            ProcessInfo.hProcess, &SymCallbackProc64, (ULONG64)this
        );
        KG_ASSERT_EXIT(nRetCode);

        nRetCode = (DWORD)::SymGetOptions();
        nRetCode |= SYMOPT_DEBUG;
        ::SymSetOptions(nRetCode);
    }

    m_hProcess = ProcessInfo.hProcess;
    m_nExitFlag = false;
    nResult = true;
Exit0:
    if (ProcessInfo.hThread)
    {
        ::CloseHandle(ProcessInfo.hThread);
        ProcessInfo.hThread = NULL;
    } 
    if (!nResult)
    {
        if (ProcessInfo.hProcess)
        {
            ::TerminateProcess(ProcessInfo.hProcess, 0);
            ::CloseHandle(ProcessInfo.hProcess);
            ProcessInfo.hProcess = NULL;
        }
    }
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::WaitForExit(
    PDEBUG_PROCESS_CALLBACK pfCallBack, void *pvUserData
)
{
    int nResult  = false;
    int nRetCode = false;
    DEBUG_EVENT DebugEvent;                 // debugging event information 
    char szSymbolSearchPath[4096];

    KG_ASSERT_EXIT(m_hProcess);
    KG_ASSERT_EXIT(pfCallBack);
    
    m_pfCallBack         = pfCallBack;
    m_pvCallBackUserData = pvUserData;

    ::SymGetSearchPath(m_hProcess, szSymbolSearchPath, countof(szSymbolSearchPath));
    OutputDebugString("Set symbol Search path to:\n");
    OutputDebugString(szSymbolSearchPath);
    OutputDebugString("\n-----------------\n\n");

    while (!m_nExitFlag) 
    { 
        nRetCode = ::WaitForDebugEvent(&DebugEvent, 500); 
        if (nRetCode)
        {
            nRetCode = _OnDebugEvent(&DebugEvent);
            KG_ASSERT_EXIT(nRetCode);
        }
        else
        {
            nRetCode = ::GetLastError();
            KG_ASSERT_EXIT(nRetCode == ERROR_SEM_TIMEOUT);
        }

        if (m_nMemUseStateChangeFlag)
        {
            ::EnterCriticalSection(&m_MemUseLock);
            nResult = m_MemUse.Enable(m_nEnableMemUseAnalyzeFlag);
            ::LeaveCriticalSection(&m_MemUseLock);
            m_nMemUseStateChangeFlag = false;

            if (m_pfCallBack)
            {
                m_pfCallBack(DBG_MEM_USE_STATE_CHANGE, NULL, m_pvCallBackUserData);
            }
        }
    }

    nResult = true;
Exit0:
    if (m_hProcess)
    {
        ::SymCleanup(m_hProcess);

        ::CloseHandle(m_hProcess);
        m_hProcess = NULL;
    }
    m_BreakMap.clear();
    m_pLastBreakAddr = NULL;

    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::Stop()
{
    if (m_hProcess)
    {
        ::TerminateProcess(m_hProcess, 0); // will get the message at ::WaitForDebugEvent
    }
    return true;
}
int  KG_DebugProcess::EnableMemUseAnalyze(int nEnable)
{
    m_nMemUseStateChangeFlag = true;
    m_nEnableMemUseAnalyzeFlag = nEnable;

    return true;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::GetLeakReport(std::string *psRetLeakReport)
{
    return m_MemLeak.GetLeakReport(psRetLeakReport);
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::GetMemRegion(std::string *psRetMemState)
{
    int nResult = false;
    ::EnterCriticalSection(&m_MemUseLock);
    nResult = m_MemUse.GetMemRegion(psRetMemState);
    ::LeaveCriticalSection(&m_MemUseLock);
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::GetMemAllocFunction(std::string *psRetMemAllocFunction)
{
    int nResult = false;
    ::EnterCriticalSection(&m_MemUseLock);
    nResult = m_MemUse.GetMemAllocFunction(psRetMemAllocFunction);
    ::LeaveCriticalSection(&m_MemUseLock);
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::GetFunctionSymbol(
    unsigned uAddress,  
    std::string *psRetModule,  
    std::string *psRetSymbol, 
    std::string *psRetFileLine
)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwDisplacement;
    DWORD64 dw64Displacement;
    unsigned char byBuffer[sizeof(SYMBOL_INFO) + sizeof(char) * (MAX_SYM_NAME - 1)];
    SYMBOL_INFO *pSymbol = (SYMBOL_INFO *)byBuffer;
    IMAGEHLP_MODULE64 ModuleInfo;
    IMAGEHLP_LINE64 Line;
    char szLineNum[32];
  
    //KG_ASSERT_EXIT(psRetModule);
    //KG_ASSERT_EXIT(psRetSymbol);
    //KG_ASSERT_EXIT(psRetFileLine);
    if (psRetModule)
        psRetModule->clear();
    if (psRetSymbol)
        psRetSymbol->clear();
    if (psRetFileLine)
        psRetFileLine->clear();

    if (psRetModule)
    {
        ModuleInfo.SizeOfStruct = sizeof(ModuleInfo);
        nRetCode = ::SymGetModuleInfo64(m_hProcess, uAddress, &ModuleInfo);
        KG_PROCESS_ERROR(nRetCode);
        *psRetModule = ModuleInfo.ModuleName;
    }

    if (psRetSymbol)
    {
        pSymbol->SizeOfStruct = sizeof(*pSymbol);
        pSymbol->MaxNameLen = MAX_SYM_NAME;
        nRetCode = ::SymFromAddr(m_hProcess, (DWORD64)uAddress, &dw64Displacement, pSymbol);
        KG_PROCESS_SUCCESS(!nRetCode); // some dll do not has pdb file
        *psRetSymbol = pSymbol->Name;
    }

    if (psRetFileLine)
    {
        Line.SizeOfStruct = sizeof(Line);
        nRetCode = ::SymGetLineFromAddr64(m_hProcess, (DWORD64)uAddress, &dwDisplacement, &Line);
        KG_PROCESS_SUCCESS(!nRetCode); // some dll do not has pdb file

        *psRetFileLine = Line.FileName;
        *psRetFileLine += ", ";
        itoa(Line.LineNumber, szLineNum, 10);
        *psRetFileLine += szLineNum;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::GetSymbolAddress(const char cszSymbol[], void **ppvRetAddress)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned char byBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    SYMBOL_INFO *pSymbol = (SYMBOL_INFO *)byBuffer;

    KG_ASSERT_EXIT(cszSymbol);
    KG_ASSERT_EXIT(ppvRetAddress);

    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;

    nRetCode = ::SymFromName(m_hProcess, (char *)cszSymbol, pSymbol);
    KG_ASSERT_EXIT(nRetCode);

    *ppvRetAddress = (void *)pSymbol->Address; 

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::OutputDebugString(const char cszString[])
{
    int nResult = false;
   
    KG_ASSERT_EXIT(cszString);
    KG_PROCESS_ERROR(m_pfCallBack);
    
    m_pfCallBack(DBG_DEBUG_STRING, (void *)cszString, m_pvCallBackUserData);
  
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::SetBreakpoint(void *pvAddress)
{
    int nResult  = false;
    int nRetCode = false;
    BYTE byOrgCode = 0xcc;  // int 3

    if (m_BreakMap.find(pvAddress) == m_BreakMap.end())
    {
        nRetCode = _SetBreakpoint(pvAddress, &byOrgCode);
        KG_ASSERT_EXIT(nRetCode);

        m_BreakMap.insert(make_pair(pvAddress, byOrgCode));
    }

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::RemoveBreakpoint(void *pvAddress)
{
    int nResult  = false;
    int nRetCode = false;
    BREAKPOINT_MAP::iterator itBreak;

    itBreak = m_BreakMap.find(pvAddress);
    KG_ASSERT_EXIT(itBreak != m_BreakMap.end());

    if (m_pLastBreakAddr != pvAddress) // maybe be call when on current break, already removed in onbreak process
    {
        nRetCode = _RemoveBreakpoint(pvAddress, itBreak->second);
        KG_ASSERT_EXIT(nRetCode);
    }
    else
    {
        m_pLastBreakAddr = NULL;
    }

    m_BreakMap.erase(itBreak);

    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::RunUntilReturn()
{
    return false;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_SetBreakpoint(void *pbyAddress, unsigned char *pbyRetOrgCode)
{
    int nResult = false;
    int nRetCode = false;
    DWORD dwReadWriteBytes = 0;
    DWORD dwOldProtect = 0;
    unsigned char byBreakCode = 0xcc; // int 3
    
    ASSERT(pbyAddress);
    ASSERT(pbyRetOrgCode);

    nRetCode = ::ReadProcessMemory(m_hProcess, pbyAddress, pbyRetOrgCode, sizeof(*pbyRetOrgCode), &dwReadWriteBytes);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::VirtualProtectEx(m_hProcess, pbyAddress, sizeof(byBreakCode), PAGE_READWRITE, &dwOldProtect);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::WriteProcessMemory(m_hProcess, pbyAddress, &byBreakCode, sizeof(byBreakCode), &dwReadWriteBytes);
    KG_ASSERT_EXIT(nRetCode);
    //if (m_pfCallBack)
    //{
    //    char szMessage[128];
    //    sprintf(szMessage, "WriteMemory:%p,new=0x%x, old=0x%x\n", pbyAddress, byBreakCode, *pbyRetOrgCode);
    //    m_pfCallBack(DBG_DEBUG_STRING, (void *)szMessage, m_pvCallBackUserData);
    //}

    nRetCode = ::VirtualProtectEx(m_hProcess, pbyAddress, sizeof(byBreakCode), dwOldProtect, &dwOldProtect);
    KG_ASSERT_EXIT(nRetCode);
    
    nRetCode = ::FlushInstructionCache(m_hProcess, pbyAddress, sizeof(byBreakCode));
    KG_ASSERT_EXIT(nRetCode);
    
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_RemoveBreakpoint(void *pbyAddress, unsigned char byOrgCode)
{
    int nResult = false;
    int nRetCode = false;
    DWORD dwReadWriteBytes = 0;
    DWORD dwOldProtect = 0;
    unsigned char byCurByte = 0;
    unsigned char byBreakCode = 0xcc;

    ASSERT(pbyAddress);

    nRetCode = ::ReadProcessMemory(m_hProcess, pbyAddress, &byCurByte, sizeof(byCurByte), &dwReadWriteBytes);
    KG_ASSERT_EXIT(nRetCode);
    KG_ASSERT_EXIT(byCurByte == byBreakCode);

    nRetCode = ::VirtualProtectEx(m_hProcess, pbyAddress, sizeof(byOrgCode), PAGE_READWRITE, &dwOldProtect);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::WriteProcessMemory(m_hProcess, pbyAddress, &byOrgCode, sizeof(byOrgCode), &dwReadWriteBytes);
    KG_ASSERT_EXIT(nRetCode);
    //if (m_pfCallBack)
    //{
    //    char szMessage[128];
    //    sprintf(szMessage, "WriteMemory:%p,new=0x%x, old=0x%x\n", pbyAddress, byOrgCode, byCurByte);
    //    m_pfCallBack(DBG_DEBUG_STRING, (void *)szMessage, m_pvCallBackUserData);
    //}
    nRetCode = ::VirtualProtectEx(m_hProcess, pbyAddress, sizeof(byOrgCode), dwOldProtect, &dwOldProtect);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::FlushInstructionCache(m_hProcess, pbyAddress, sizeof(byOrgCode));
    KG_ASSERT_EXIT(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_GetRemoteString(
    void    *pvRemoteAddress,
    unsigned uStringSize,
    int      nUnicodeFlag,
    std::string *psRetRemoteString
)
{
    int nResult  = false;
    int nRetCode = false;
    char    *pStringA = NULL;
    wchar_t *pStringW = NULL;
    
    KG_ASSERT_EXIT(pvRemoteAddress);
    KG_ASSERT_EXIT(psRetRemoteString);

    pStringA = new char[uStringSize];
    KG_ASSERT_EXIT(pStringA);

    if (nUnicodeFlag)
    {
        pStringW = new wchar_t[uStringSize];
        KG_ASSERT_EXIT(pStringW);

        nRetCode = ::ReadProcessMemory(
            m_hProcess,
            pvRemoteAddress,
            pStringW,
            uStringSize * sizeof(*pStringW),
            NULL
        );
        KG_ASSERT_EXIT(nRetCode);

        nRetCode = ::WideCharToMultiByte(
            CP_ACP, // ANSI code page
            0,
            pStringW,
            -1,
            pStringA,
            uStringSize * sizeof(*pStringA),
            NULL,
            NULL
        );
        KG_ASSERT_EXIT(nRetCode);
    }
    else
    {
        nRetCode = ::ReadProcessMemory(
            m_hProcess,
            pvRemoteAddress,
            pStringA,
            uStringSize * sizeof(*pStringA),
            NULL
        );
        KG_ASSERT_EXIT(nRetCode);
    }

    *psRetRemoteString = pStringA;
    nResult = true;
Exit0:
    KG_DELETE_ARRAY(pStringA);
    KG_DELETE_ARRAY(pStringW);

    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_OnDebugEvent(DEBUG_EVENT *pDebugEvent)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwContinueStatus = DBG_CONTINUE;  // exception continuation 
    
    ASSERT(pDebugEvent);
    //
    //if (m_pfCallBack)
    //{
    //    m_pfCallBack(DBG_DEBUG_STRING, "DebugEvent Begin ----------\n", m_pvCallBackUserData);
    //}

    switch (pDebugEvent->dwDebugEventCode) 
    {
    case EXCEPTION_DEBUG_EVENT: 
        _OnException(pDebugEvent->dwThreadId, &(pDebugEvent->u.Exception), &dwContinueStatus);
        break;

    case CREATE_PROCESS_DEBUG_EVENT:
        if (m_pfCallBack)
        {
            m_pfCallBack(DBG_PROCESS_CREATE, NULL, m_pvCallBackUserData);
        }
        ::EnterCriticalSection(&m_MemUseLock);
        m_MemUse.SetDebugger(this, m_hProcess);
        ::LeaveCriticalSection(&m_MemUseLock);;
        _OnModuleLoad(
             pDebugEvent->u.CreateProcessInfo.hFile,
             pDebugEvent->u.CreateProcessInfo.lpBaseOfImage
         );
        m_ThreadMap[pDebugEvent->dwThreadId] = pDebugEvent->u.CreateProcessInfo.hThread;
        break;

    case EXIT_PROCESS_DEBUG_EVENT: 
        if (m_pfCallBack)
        {
            m_pfCallBack(DBG_PROCESS_EXIT, NULL, m_pvCallBackUserData);
        }
        m_ThreadMap.erase(pDebugEvent->dwThreadId);
        m_nExitFlag = true;
        break;

    case LOAD_DLL_DEBUG_EVENT:
        _OnModuleLoad(
            pDebugEvent->u.LoadDll.hFile,
            pDebugEvent->u.LoadDll.lpBaseOfDll
        );
        break;

    case UNLOAD_DLL_DEBUG_EVENT: 
        // if unload, we can get the symbol when output call stack when or after DLL be unload
        //::SymUnloadModule64(m_hProcess, (DWORD64)pDebugEvent->u.UnloadDll.lpBaseOfDll);
        break;

    case OUTPUT_DEBUG_STRING_EVENT: 
        nRetCode = _OnOutputDebugString(pDebugEvent->dwThreadId, &pDebugEvent->u.DebugString);
		ASSERT(nRetCode);
        break;

    case CREATE_THREAD_DEBUG_EVENT:
        m_ThreadMap[pDebugEvent->dwThreadId] = pDebugEvent->u.CreateThread.hThread;
        break;

    case EXIT_THREAD_DEBUG_EVENT:
        m_ThreadMap.erase(pDebugEvent->dwThreadId);
        break;

    case RIP_EVENT:
        ASSERT(false);
        break;

    default:
        ASSERT(false);
        break;
    } 

    nRetCode = ::ContinueDebugEvent(
        pDebugEvent->dwProcessId, pDebugEvent->dwThreadId, dwContinueStatus
    );
    ASSERT(nRetCode);

    nResult = true;
//Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_OnException(
    DWORD dwThreadID, EXCEPTION_DEBUG_INFO *pExceptionInfo, DWORD *pdwRetContinueStatus
)
{
    int nResult  = false;
    int nRetCode = false;

    KG_ASSERT_EXIT(pExceptionInfo);
    KG_ASSERT_EXIT(pdwRetContinueStatus);
   
    *pdwRetContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
    switch(pExceptionInfo->ExceptionRecord.ExceptionCode)
    { 
    case EXCEPTION_ACCESS_VIOLATION: 
        _OnAccessViolation(pExceptionInfo);
        break;

    case EXCEPTION_BREAKPOINT: 
        _OnBreakpoint(dwThreadID, pExceptionInfo->ExceptionRecord.ExceptionAddress);
        *pdwRetContinueStatus = DBG_CONTINUE;
        break;

    case EXCEPTION_SINGLE_STEP:
        _OnSingleStep(dwThreadID);
        *pdwRetContinueStatus = DBG_CONTINUE;
        break;

    default:
        if (m_pfCallBack)
        {
            char szMsgBuf[256];
            nRetCode = _snprintf(
                szMsgBuf, sizeof(szMsgBuf),
                "%sexception at 0x%8p, Code 0x%8X\n",
                (pExceptionInfo->dwFirstChance ? "First-chance " : ""),
                pExceptionInfo->ExceptionRecord.ExceptionAddress,
                pExceptionInfo->ExceptionRecord.ExceptionCode
            );
            szMsgBuf[sizeof(szMsgBuf) - 1] = '\0';

            m_pfCallBack(DBG_DEBUG_STRING, (void *)szMsgBuf, m_pvCallBackUserData);
        }
        break;
    } 
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_OnModuleLoad(HANDLE hFile, void *lpBaseOfImage)
{
    int nResult  = false;
    int nRetCode = false;
    string sImageName;
    DWORD64 dw64BaseAddress = 0;
    IMAGEHLP_MODULE64 ModuleInfo;

    KG_ASSERT_EXIT(hFile);
    //KG_ASSERT_EXIT(lpImageName); maybe null
    KG_ASSERT_EXIT(lpBaseOfImage);

    nRetCode = _GetFileNameFromHandle(hFile, &sImageName);
    KG_ASSERT_EXIT(nRetCode); // full name with path

    // ------------- Load symbol file --------------------
    dw64BaseAddress = ::SymLoadModule64(
        m_hProcess,                     //HANDLE hProcess,
        hFile,                          //HANDLE hFile,
        (char *)sImageName.c_str(),     //PCSTR ImageName,
        NULL,                           //PCSTR ModuleName, //  a shortcut name for the module
        (DWORD64)lpBaseOfImage,         //DWORD64 BaseOfDll,
        0                               //DWORD SizeOfDll
    );
    KG_PROCESS_ERROR(dw64BaseAddress > 0 && "no pdb file found");

    ModuleInfo.SizeOfStruct = sizeof(ModuleInfo);
    nRetCode = ::SymGetModuleInfo64(m_hProcess, dw64BaseAddress, &ModuleInfo);
    KG_ASSERT_EXIT(nRetCode);

    ::EnterCriticalSection(&m_MemUseLock);
    m_MemUse.OnModuleLoad(ModuleInfo);
    ::LeaveCriticalSection(&m_MemUseLock);;

    nResult = true;
Exit0:
    if (hFile)
    {
        ::CloseHandle(hFile);
        hFile = NULL;
    }

    if (m_pfCallBack && (!sImageName.empty()))
    {
        string sMessage = "Loaded \"" + sImageName + "\"\n";
        m_pfCallBack(DBG_DEBUG_STRING, (void *)sMessage.c_str(), m_pvCallBackUserData);
    }

    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_OnOutputDebugString(
    DWORD dwThreadID, OUTPUT_DEBUG_STRING_INFO *pStringInfo
)
{
    int nResult  = false;
    int nRetCode = false;
    std::string sDebugString;

    KG_ASSERT_EXIT(pStringInfo);

    nRetCode = _GetRemoteString(
        pStringInfo->lpDebugStringData,
        pStringInfo->nDebugStringLength,
        pStringInfo->fUnicode, 
        &sDebugString
    );
    KG_ASSERT_EXIT(nRetCode);

    if (m_pfCallBack)
    {
        m_pfCallBack(DBG_DEBUG_STRING, (void *)sDebugString.c_str(), m_pvCallBackUserData);
    }

    nRetCode = m_MemLeak.OnDebugOutput(sDebugString.c_str(), this);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = m_D3DDebugOutputStack.OnDebugOutput(
        sDebugString.c_str(), this, dwThreadID
    );

    nResult = true;
Exit0:
    return nResult;
}////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::OutputStackFrame(DWORD dwThreadID)
{
    int nResult  = false;
    int nRetCode = false;
    THREAD_MAP::iterator itThread;
    HANDLE hThread = NULL;
    STACKFRAME64 StackFrame;
    CONTEXT Context;
    std::string sModule;
    std::string sSymbol;
    char szMessage[1024];

    itThread = m_ThreadMap.find(dwThreadID);
    KG_ASSERT_EXIT(itThread != m_ThreadMap.end());
    hThread = itThread->second;
    ASSERT(hThread);

    // get the context for the current thread
    Context.ContextFlags = CONTEXT_CONTROL;
    nRetCode = ::GetThreadContext(hThread, &Context);
    KG_ASSERT_EXIT(nRetCode);

    ZeroMemory(&StackFrame, sizeof(StackFrame));
    StackFrame.AddrPC.Offset    = Context.Eip;
    StackFrame.AddrPC.Mode      = AddrModeFlat;
    StackFrame.AddrFrame.Offset = Context.Ebp;
    StackFrame.AddrFrame.Mode   = AddrModeFlat;
    StackFrame.AddrStack.Offset = Context.Esp;
    StackFrame.AddrStack.Mode   = AddrModeFlat;

    while (true)
    {
        nRetCode = ::StackWalk64(
            IMAGE_FILE_MACHINE_I386,
            m_hProcess,
            hThread,
            &StackFrame,
            &Context,
            NULL,
            ::SymFunctionTableAccess64,
            ::SymGetModuleBase64,
            NULL
        );
        if (!nRetCode)
            break;

        nRetCode = GetFunctionSymbol((unsigned)StackFrame.AddrPC.Offset, &sModule, &sSymbol, NULL);
        //KG_PROCESS_ERROR(nRetCode);

        _snprintf(
            szMessage, countof(szMessage), 
            "[%.8X]%s!%s\n", (unsigned)StackFrame.AddrPC.Offset, sModule.c_str(), sSymbol.c_str()
        );
        szMessage[countof(szMessage) - 1] = '\0';

        nRetCode = OutputDebugString(szMessage);
        KG_ASSERT_EXIT(nRetCode);
    }
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_OnBreakpoint(DWORD dwThreadID, void *pvAddress)
{
    int nResult  = false;
    int nRetCode = false;
    BREAKPOINT_MAP::iterator itAddress;
    BYTE byOrgCode = 0;
    THREAD_MAP::iterator itThread;
    HANDLE hThread = NULL;
    CONTEXT Context;
    const unsigned SINGLE_STEP_BIT = 0x100;

    itAddress = m_BreakMap.find(pvAddress);
    KG_PROCESS_SUCCESS(itAddress == m_BreakMap.end());
    byOrgCode = itAddress->second;

    itThread = m_ThreadMap.find(dwThreadID);
    KG_ASSERT_EXIT(itThread != m_ThreadMap.end());
    hThread = itThread->second;
    ASSERT(hThread);

    // get the context for the current thread
    Context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
    nRetCode = ::GetThreadContext(hThread, &Context);
    KG_ASSERT_EXIT(nRetCode);

    // restore code at the breakpoint address  
    --Context.Eip;    // rewind the thread EIP by 1 byte (int 3)

    nRetCode = _RemoveBreakpoint(pvAddress, byOrgCode);        // remove the breakpoint
    KG_ASSERT_EXIT(nRetCode);

    Context.EFlags |= SINGLE_STEP_BIT;// Set the single step mode
    m_pLastBreakAddr = pvAddress;

    Context.ContextFlags = CONTEXT_CONTROL;
    nRetCode = ::SetThreadContext(hThread, &Context);
    KG_ASSERT_EXIT(nRetCode);

    // suspend another thread, make sure single step execute the current thread's code
    for (itThread = m_ThreadMap.begin(); itThread != m_ThreadMap.end(); ++itThread)
    {
        if ((itThread->second) != hThread)
        {
            ::SuspendThread(itThread->second);
        }
    }

    ::EnterCriticalSection(&m_MemUseLock);
    m_MemUse.OnBreak(pvAddress, hThread, Context);
    ::LeaveCriticalSection(&m_MemUseLock);;

Exit1:
    nResult = true;
Exit0:
    if (m_pfCallBack)
    {
        ALLOC_STAT Stat;
        ::EnterCriticalSection(&m_MemUseLock);
        m_MemUse.GetAllocStat(&Stat);
        ::LeaveCriticalSection(&m_MemUseLock);;
        m_pfCallBack(DBG_BREAK, &Stat, m_pvCallBackUserData);
    }
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_OnSingleStep(DWORD dwThreadID)
{
    int nResult  = false;
    int nRetCode = false;
    BYTE byOrgCode = 0;
    THREAD_MAP::iterator itThread;

    // reset the breakpoint
    if (m_pLastBreakAddr)
    {
        nRetCode = _SetBreakpoint(m_pLastBreakAddr, &byOrgCode);
        KG_ASSERT_EXIT(nRetCode);
    }

    for (itThread = m_ThreadMap.begin(); itThread != m_ThreadMap.end(); ++itThread)
    {
        if ((itThread->first) != dwThreadID)
        {
            ::ResumeThread(itThread->second);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_DebugProcess::_OnAccessViolation(EXCEPTION_DEBUG_INFO *pExceptionInfo)
{
    int nResult  = false;
    int nRetCode = false;
    char szMsgBuf[256];
    string sMessage;
    string sModule;
    string sSymbol;
    string sFileLine;

    KG_ASSERT_EXIT(pExceptionInfo);

    nRetCode = GetFunctionSymbol(
        (unsigned)(ULONG64)pExceptionInfo->ExceptionRecord.ExceptionAddress, &sModule, &sSymbol, &sFileLine
    );
    //KG_ASSERT_EXIT(nRetCode);
    nRetCode = _snprintf(
        szMsgBuf, sizeof(szMsgBuf),
        "%sexception at 0x%.8p (%s.%s) : 0x%.8X : Access violation %s location 0x%.8x\n",
        (pExceptionInfo->dwFirstChance ? "First-chance " : ""),
        pExceptionInfo->ExceptionRecord.ExceptionAddress,
        sModule.c_str(),
        sSymbol.c_str(),
        pExceptionInfo->ExceptionRecord.ExceptionCode,
        ((pExceptionInfo->ExceptionRecord.ExceptionInformation[0] == 1) ? "writing" : "reading"),
        (unsigned)pExceptionInfo->ExceptionRecord.ExceptionInformation[1]
    );
    szMsgBuf[sizeof(szMsgBuf) - 1] = '\0';

    nResult = true;
Exit0:
    if (m_pfCallBack)
    {
        m_pfCallBack(DBG_DEBUG_STRING, (void *)szMsgBuf, m_pvCallBackUserData);
    }
    return nResult;
}



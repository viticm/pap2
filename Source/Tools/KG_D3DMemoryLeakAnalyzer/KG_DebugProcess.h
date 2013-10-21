////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_DebugProcess.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-7-27 17:41:07
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_DEBUGPROCESS_H_
#define _INCLUDE_KG_DEBUGPROCESS_H_

#include "KG_D3DMemoryLeak.h"
#include "KG_MemoryUse.h"
#include "KG_D3DDebugOutputStack.h"

enum DEBUG_PROCESS_EVENT
{
    DBG_PROCESS_CREATE, // no data
    DBG_PROCESS_EXIT,   // no data
    DBG_DEBUG_STRING,    // pvData is char *
    DBG_MEM_USE_STATE_CHANGE,
    DBG_BREAK
};

typedef void (CALLBACK *PDEBUG_PROCESS_CALLBACK)(
    DEBUG_PROCESS_EVENT eEvent, void *pvData, void *pvUserData
);

////////////////////////////////////////////////////////////////////////////////
class KG_DebugProcess
{
public:
    KG_DebugProcess(void);
    ~KG_DebugProcess(void);

    int Create(
        const char cszAppPathName[],     // need dbghelp.dll(ver >= 6.5(vs2005 path), xp default is 5.1)
        const char cszWorkPath[],
        const char *pcszSymbolSearchPath // can be null, if not null, need symsrv.dll
    );
    int WaitForExit(PDEBUG_PROCESS_CALLBACK pfCallBack, void *pvUserData);
    int Stop();
    int EnableMemUseAnalyze(int nEnable);
    int IsRunning() { return m_hProcess != NULL; };

    int GetLeakReport(std::string *psRetLeakReport);
    int GetMemRegion(std::string *psRetMemState);
    int GetMemAllocFunction(std::string *psRetMemAllocFunction);

    // --------------------------  --------------------------
    int GetFunctionSymbol(
        unsigned uAddress,  
        std::string *psRetModule,   // can be null if not need
        std::string *psRetSymbol,   // can be null if not need
        std::string *psRetFileLine  // can be null if not need
    );
    int GetSymbolAddress(const char cszSymbol[], void **ppvRetAddress);

    int OutputDebugString(const char cszString[]);
    int OutputStackFrame(DWORD dwThreadID);
    int SetBreakpoint(void *pvAddress);
    int RemoveBreakpoint(void *pvAddress);
    
    int RunUntilReturn();

private:
    int _SetBreakpoint(void *pbyAddress, unsigned char *pbyRetOrgByte);
    int _RemoveBreakpoint(void *pbyAddress, unsigned char byOrgCode);
    int _GetRemoteString(
        void    *pvRemoteAddress,
        unsigned uStringSize, // in char count, include '\0'
        int      nUnicodeFlag,
        std::string *psRetRemoteString
    );
    int _OnDebugEvent(DEBUG_EVENT *pDebugEvent);
    int _OnException(
        DWORD dwThreadID, EXCEPTION_DEBUG_INFO *pExceptionInfo, DWORD *pdwRetContinueStatus
    );
    int _OnAccessViolation(EXCEPTION_DEBUG_INFO *pExceptionInfo);
    int _OnLoadDLL(LOAD_DLL_DEBUG_INFO *pDllInfo);
    int _OnModuleLoad(HANDLE hFile, void *lpBaseOfImage);
    int _OnOutputDebugString(DWORD dwThreadID, OUTPUT_DEBUG_STRING_INFO *pStringInfo);
    int _OnBreakpoint(DWORD dwThreadID, void *pvAddress);
    int _OnSingleStep(DWORD dwThreadID);
private:
    HANDLE m_hProcess;
    int    m_nExitFlag;

    PDEBUG_PROCESS_CALLBACK m_pfCallBack;
    void *m_pvCallBackUserData;

    typedef std::map<void *, BYTE> BREAKPOINT_MAP;
    BREAKPOINT_MAP m_BreakMap;
    void *m_pLastBreakAddr;

    typedef std::map<DWORD, HANDLE> THREAD_MAP;
    THREAD_MAP m_ThreadMap;

    KG_D3DMemoryLeak m_MemLeak;

    KG_MemoryUse     m_MemUse;
    CRITICAL_SECTION m_MemUseLock;
    int m_nMemUseStateChangeFlag;
    int m_nEnableMemUseAnalyzeFlag;

    KG_D3DDebugOutputStack m_D3DDebugOutputStack;
};


#endif //_INCLUDE_KG_DEBUGPROCESS_H_

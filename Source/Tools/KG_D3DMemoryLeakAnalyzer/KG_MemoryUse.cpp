#include "StdAfx.h"
#include "KG_MemoryUse.h"
#include "KG_DebugProcess.h"

using namespace std;
////////////////////////////////////////////////////////////////////////////////
#define ROUND_UP_TO_POWER2( x, n ) (((DWORD)(x) + ((n)-1)) & ~((DWORD)(n)-1))
#define ROUND_DOWN_TO_POWER2( x, n ) ((DWORD)(x) & ~((DWORD)(n)-1))

////////////////////////////////////////////////////////////////////////////////
//#define SHOW_DEBUG_INFO

#ifdef SHOW_DEBUG_INFO
    //#define SHOW_STACK_TRACE
    static char gs_DebugInfo[1024];
    static unsigned gs_OPID = 0;
#endif
////////////////////////////////////////////////////////////////////////////////
const unsigned HEAP_HEAD_SIZE   = 24;
const unsigned HEAP_GRANULARITY = 8;
const unsigned PAGE_SIZE = 4096;
const char * SYSTEM_MODULE_ARRAY[] =
{
    "ntdll",
    "kernel32",
    "d3d9",
    "d3dx9_40",
    "MSVCR80D",
    "msvcrt",
    "nvd3dum"
};

const char *FILETER_FUNCTION_ARRAY[]=
{
    "operator new",
    "operator new[]"
};
////////////////////////////////////////////////////////////////////////////////
bool _SortFunction(
    const KG_MemoryUse::_ALLOC_FUNCTION  *pcLeft, 
    const KG_MemoryUse::_ALLOC_FUNCTION *pcRight
)
{
    return (pcLeft->uTotalSize > pcRight->uTotalSize);
}
////////////////////////////////////////////////////////////////////////////////
KG_MemoryUse::KG_MemoryUse(void):
    m_pDebugger(NULL),
    m_hProcess(NULL),
    m_nEnableHookMemAllocFlag(true)
{
    ZeroMemory(&m_Stat, sizeof(m_Stat));
}

KG_MemoryUse::~KG_MemoryUse(void)
{
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::SetDebugger(KG_DebugProcess *pDebugger, HANDLE hProcess)
{
    m_pDebugger = pDebugger; 
    m_hProcess  = hProcess;

    m_BreakMap.clear();
    m_ThreadMap.clear();
    m_VMMap.clear();
    m_AllocMap.clear();
    m_ModuleMap.clear();
    m_FuncMap.clear();
    m_CallAddrMap.clear();
    ZeroMemory(&m_Stat, sizeof(m_Stat)); 
   
    return true;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::Enable(int nEnable)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_SUCCESS(m_nEnableHookMemAllocFlag == nEnable);

    if (nEnable)
    {
        nRetCode = _HookAllocAPI();
        KG_ASSERT_EXIT(nRetCode);
    }
    else
    {
        BREAK_MAP::iterator itBreak;
        THREAD_MAP::iterator itThread;
        for (itBreak = m_BreakMap.begin(); itBreak != m_BreakMap.end(); ++itBreak)
        {
            nRetCode = m_pDebugger->RemoveBreakpoint(itBreak->first);
            KG_ASSERT_EXIT(nRetCode);
        }
        m_BreakMap.clear();

        for (itThread = m_ThreadMap.begin(); itThread != m_ThreadMap.end(); ++itThread)
        {
            itThread->second.AllocHeap.nRunningFlag = false;
            itThread->second.FreeHeap.nRunningFlag  = false;
        }
        //m_ThreadMap.clear();

        m_VMMap.clear();
        m_AllocMap.clear();
        //m_ModuleMap.clear();
        m_FuncMap.clear();
        m_CallAddrMap.clear();
        ZeroMemory(&m_Stat, sizeof(m_Stat)); 
    }

    m_nEnableHookMemAllocFlag = nEnable;
Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::OnModuleLoad(const IMAGEHLP_MODULE64 &crModule)
{
    int nResult = false;
    int nRetCode = false;

    ASSERT(m_pDebugger);

    m_ModuleMap.insert(make_pair((void *)crModule.BaseOfImage, crModule));

    if (m_nEnableHookMemAllocFlag)
    {
        nRetCode = stricmp(crModule.ModuleName, "ntdll");
        KG_PROCESS_SUCCESS(nRetCode != 0);

        nRetCode = _HookAllocAPI();
        KG_ASSERT_EXIT(nRetCode);
    }
    
Exit1:
    nResult = true;
Exit0:
#ifdef SHOW_DEBUG_INFO
    sprintf(
        gs_DebugInfo, 
        "Memory,0x%p,%u,Image,File=%s\n", 
        (void *)crModule.BaseOfImage, crModule.ImageSize/1024, crModule.ImageName
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::OnBreak(void *pvBreakAddress, HANDLE hThread, const CONTEXT &crContext)
{
    int nResult = false;
    int nRetCode = false;
    THREAD_MAP::iterator itThread;
    BREAK_MAP::iterator itBreak;
    _THREAD *pThread = NULL;

    KG_ASSERT_EXIT(pvBreakAddress);
    KG_ASSERT_EXIT(m_pDebugger);

    itThread = m_ThreadMap.find(hThread);
    if (itThread == m_ThreadMap.end())
    {
        THREAD_MAP::_Pairib InsertResult;
        _THREAD Thread = { 0 };
        Thread.hThread = hThread;
        InsertResult = m_ThreadMap.insert(make_pair(hThread, Thread));
        KG_ASSERT_EXIT(InsertResult.second);
        pThread = &(InsertResult.first->second); 
    }
    else
    {
        pThread = &(itThread->second);
    }

    itBreak = m_BreakMap.find(pvBreakAddress);
    KG_PROCESS_SUCCESS(itBreak == m_BreakMap.end());

    pThread->pcContext = &crContext;
    ASSERT(itBreak->second);
    nRetCode = (this->*(itBreak->second))(pThread);
    KG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::GetMemRegion(std::string *psRetMemState)
{
    int nResult  = false;
    int nRetCode = false;
    VIRTUAL_MEM_MAP::iterator itVMMap;
    char szMsgBuf[1024];

    KG_ASSERT_EXIT(psRetMemState);

    psRetMemState->reserve(1024 * 10);
    *psRetMemState += "Address,Type,Size,CommitSize\r\n";
    for (itVMMap = m_VMMap.begin(); itVMMap != m_VMMap.end(); ++itVMMap)
    {
        _VM_REGION &CurRegion = itVMMap->second;
        nRetCode = _snprintf(
            szMsgBuf, _countof(szMsgBuf),
            "%p,%s,%u,%u\r\n",
            CurRegion.pvBase,
            CurRegion.nHeapMemFlag ? "Heap (Private)" : "Private",
            CurRegion.uSize / 1024,
            CurRegion.uCommitSize / 1024
        );
        szMsgBuf[_countof(szMsgBuf) - 1] = '\0';
        *psRetMemState += szMsgBuf;
    }
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::GetMemAllocFunction(std::string *psRetMemAllocFunction)
{
    int nResult  = false;
    int nRetCode = false;
    char szMsgBuf[1024];
    FUNCTION_MAP::iterator itFuncMap;
    std::vector<_ALLOC_FUNCTION *> FuncVec;
    std::vector<_ALLOC_FUNCTION *>::iterator itFuncVec;

    KG_ASSERT_EXIT(psRetMemAllocFunction);

    psRetMemAllocFunction->reserve(1024 * 10);

    FuncVec.reserve(m_FuncMap.size());
    for (itFuncMap = m_FuncMap.begin(); itFuncMap != m_FuncMap.end(); ++itFuncMap)
    {
        FuncVec.push_back(&(itFuncMap->second));
    }
    sort(FuncVec.begin(), FuncVec.end(), _SortFunction);

   *psRetMemAllocFunction = "Percent\tTotalSize\tHeapSize\tHeapHeadSize\tVMSize\tName\tFileLine\r\n";
   KG_PROCESS_SUCCESS(m_Stat.uTotalAllocSize == 0);
   for (itFuncVec = FuncVec.begin(); itFuncVec != FuncVec.end(); ++itFuncVec)
   {
       _ALLOC_FUNCTION *pFunction = *itFuncVec;
       if (((pFunction->uTotalSize / (float)m_Stat.uTotalAllocSize)) < 0.001)
       {
           break;
       }
        nRetCode = _snprintf(
            szMsgBuf, _countof(szMsgBuf),
            "%4.1f\t%6.3f\t%6.3f\t%6.3f\t%6.3f\t%s!%s\t%s\r\n",
            (pFunction->uTotalSize / (float)m_Stat.uTotalAllocSize) * 100,
            pFunction->uTotalSize / 1024.0f / 1024.0f,
            pFunction->uHeapAllocSize  / 1024.0f / 1024.0f,
            pFunction->uHeapHeadSize  / 1024.0f / 1024.0f,
            pFunction->uVMSize    / 1024.0f / 1024.0f,
            pFunction->sModule.c_str(),
            pFunction->sName.c_str(),
            pFunction->sFileLine.c_str()
        );
        szMsgBuf[_countof(szMsgBuf) - 1] = '\0';
        *psRetMemAllocFunction += szMsgBuf;
   }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_HookAllocAPI()
{
    int nResult  = false;
    int nRetCode = false;

    //nRetCode = _SetFunctionBreak(
    //    "ntdll!ZwMapViewOfSection", &KG_MemoryUse::_OnBreakAt_ZwMapViewOfSection
    //);
    //KG_ASSERT_EXIT(nRetCode);

    nRetCode = _SetFunctionBreak(
        "ntdll!ZwAllocateVirtualMemory", &KG_MemoryUse::_OnBreakAt_ZwAllocateVirtualMemory
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = _SetFunctionBreak(
        "ntdll!ZwFreeVirtualMemory", &KG_MemoryUse::_OnBreakAt_ZwFreeVirtualMemory
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = _SetFunctionBreak(
        "ntdll!RtlAllocateHeap", &KG_MemoryUse::_OnBreakAt_RtlAllocateHeap
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = _SetFunctionBreak(
        "ntdll!RtlFreeHeap", &KG_MemoryUse::_OnBreakAt_RtlFreeHeap
    );
    KG_ASSERT_EXIT(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_SetFunctionBreak(
    const char cszFunctionSymbol[], BREAK_PROC_FUNCTION pfProcFunction
)
{
    int nResult  = false;
    int nRetCode = false;
    BREAK_MAP::_Pairib InsertResult;
    void *pvFunctionAddr = NULL;

    ASSERT(cszFunctionSymbol);
    ASSERT(pfProcFunction);

    nRetCode = m_pDebugger->GetSymbolAddress(cszFunctionSymbol, &pvFunctionAddr);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = m_pDebugger->SetBreakpoint(pvFunctionAddr);
    KG_ASSERT_EXIT(nRetCode);

    InsertResult = m_BreakMap.insert(make_pair(pvFunctionAddr, pfProcFunction));
    KG_ASSERT_EXIT(InsertResult.second);

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_SetFunctionReturnBreak(
    void *pvReturnAddress, BREAK_PROC_FUNCTION pfProcFunction
)
{
    int nResult  = false;
    int nRetCode = false;
    BREAK_MAP::iterator itBreak;
    BREAK_MAP::_Pairib InsertResult;
#ifdef SHOW_DEBUG_INFO
    int nExistFlag = false;
#endif
    ASSERT(pvReturnAddress);
    ASSERT(pfProcFunction);

    itBreak = m_BreakMap.find(pvReturnAddress);
    if (itBreak != m_BreakMap.end())
    {
#ifdef SHOW_DEBUG_INFO
        nExistFlag = true;
#endif
       ASSERT(itBreak->second == pfProcFunction);
        KG_PROCESS_SUCCESS(true);
    }

    nRetCode = m_pDebugger->SetBreakpoint(pvReturnAddress);
    KG_ASSERT_EXIT(nRetCode);

    InsertResult = m_BreakMap.insert(make_pair(pvReturnAddress, pfProcFunction));
    KG_ASSERT_EXIT(InsertResult.second);

Exit1:
    nResult = true;
Exit0:
#ifdef SHOW_DEBUG_INFO
    sprintf(
        gs_DebugInfo,
        "_SetFunctionReturnBreak(0x%p), Exist=%d\n", pvReturnAddress, nExistFlag
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
static int gs_nForceRefreshCacheFlag = true;
BOOL CALLBACK ReadProcessMemoryProc64(
  HANDLE hProcess,
  DWORD64 lpBaseAddress,
  PVOID lpBuffer,
  DWORD nSize,
  LPDWORD lpNumberOfBytesRead
)
{
    int nResult  = false;
    int nRetCode = false;
    static unsigned char s_byCache[PAGE_SIZE * 2];
    static DWORD s_dwCacheSize = 0;
    static DWORD64 s_dw64CacheAddress  = 0;
    static DWORD64 s_dw64CacheAddressEnd = 0;
   
    *lpNumberOfBytesRead = 0;

    if (
        (gs_nForceRefreshCacheFlag) ||
        (s_dw64CacheAddress > lpBaseAddress) ||
        (s_dw64CacheAddressEnd < lpBaseAddress) ||
        (s_dw64CacheAddressEnd < (lpBaseAddress + nSize))
    )
    {
        DWORD dwReadSize = 0;
        DWORD   dwNewCacheSize   = PAGE_SIZE;
        DWORD64 dw64NewCacheAddr = ROUND_DOWN_TO_POWER2((lpBaseAddress + nSize), PAGE_SIZE);
        if (dw64NewCacheAddr > lpBaseAddress)
        {
            dwNewCacheSize += PAGE_SIZE;//  need read two page
        }
        
        // not in cache, reload cache memory
        nRetCode = ::ReadProcessMemory(hProcess, (void *)dw64NewCacheAddr, s_byCache, dwNewCacheSize, &dwReadSize);
        //KG_ASSERT_EXIT(nRetCode);
        if (!nRetCode)  // caller some times will read a invalided address (stack error ?)
            goto Exit0;

        s_dw64CacheAddress    = dw64NewCacheAddr;
        s_dwCacheSize         = dwReadSize;
        s_dw64CacheAddressEnd = s_dw64CacheAddress + s_dwCacheSize;
        gs_nForceRefreshCacheFlag = false;
    }
    ASSERT(s_dwCacheSize >= ((lpBaseAddress - s_dw64CacheAddress) + nSize));
    memcpy(lpBuffer, s_byCache + (lpBaseAddress - s_dw64CacheAddress), nSize);
    *lpNumberOfBytesRead = nSize;
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_GetAllocFunction(_THREAD *pThread, _ALLOC_FUNCTION **ppRetAllocFunc)
{
    int nResult  = false;
    int nRetCode = false;
    STACKFRAME64 StackFrame;
    CONTEXT Context;
    _ALLOC_FUNCTION AllocFunction;
    _CALL_ADDR_INFO *pAddrInfo = NULL;
    FUNCTION_MAP::iterator itFuncMap;
    unsigned uStackLevel = 0;
#ifdef SHOW_STACK_TRACE
    std::string sModule;
    std::string sSymbol;
#endif
    
    ASSERT(pThread);
    ASSERT(ppRetAllocFunc);

    ZeroMemory(&StackFrame, sizeof(StackFrame));
    Context = *(pThread->pcContext);
    StackFrame.AddrPC.Offset    = Context.Eip;
    StackFrame.AddrPC.Mode      = AddrModeFlat;
    StackFrame.AddrFrame.Offset = Context.Ebp;
    StackFrame.AddrFrame.Mode   = AddrModeFlat;
    StackFrame.AddrStack.Offset = Context.Esp;
    StackFrame.AddrStack.Mode   = AddrModeFlat;

    gs_nForceRefreshCacheFlag = true;
    while (uStackLevel < 64)
    {
        ++uStackLevel;

        nRetCode = ::StackWalk64(
            IMAGE_FILE_MACHINE_I386,
            m_hProcess,
            pThread->hThread,
            &StackFrame,
            &Context,
            NULL,//ReadProcessMemoryProc64(more quick ,but some time make error, don't know why :( ,            
            ::SymFunctionTableAccess64,
            ::SymGetModuleBase64,
            NULL
        );
        if (!nRetCode)
            break;

#ifdef SHOW_STACK_TRACE
        m_pDebugger->GetFunctionSymbol((unsigned)StackFrame.AddrPC.Offset, &sModule, &sSymbol, NULL);
        sprintf(gs_DebugInfo, "[%.8X]%s!%s\n", (unsigned)StackFrame.AddrPC.Offset, sModule.c_str(), sSymbol.c_str());
        m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif
        nRetCode = _GetCallAddrInfo((void *)StackFrame.AddrPC.Offset, &pAddrInfo);
        KG_ASSERT_EXIT(nRetCode);

        if (pAddrInfo->nIsUserFunction)
            break;
    }
    KG_ASSERT_EXIT(pAddrInfo);
#ifdef SHOW_STACK_TRACE
    m_pDebugger->OutputDebugString("---------------\n");
#endif

    // get first user function or last function in stack as alloc function

    *ppRetAllocFunc = pAddrInfo->pAllcFunction;
    KG_PROCESS_SUCCESS(*ppRetAllocFunc); // has cached

    nRetCode = m_pDebugger->GetFunctionSymbol(
        (unsigned)StackFrame.AddrPC.Offset, 
        &AllocFunction.sModule, 
        &AllocFunction.sName, 
        &AllocFunction.sFileLine
    );
    KG_ASSERT_EXIT(nRetCode);

    itFuncMap = m_FuncMap.find(AllocFunction.sName);
    if (itFuncMap == m_FuncMap.end())
    {
        FUNCTION_MAP::_Pairib InsertResult;
        AllocFunction.uHeapAllocSize = 0;
        AllocFunction.uHeapHeadSize = 0;
        AllocFunction.uVMSize   = 0;
        AllocFunction.uTotalSize = 0;
        AllocFunction.uAllocHeapCount = 0;
        AllocFunction.uFreeHeapCount  = 0;
        AllocFunction.uAllocVMCount   = 0;
        AllocFunction.uFreeVMCount    = 0;
        InsertResult = m_FuncMap.insert(make_pair(AllocFunction.sName, AllocFunction));
        KG_ASSERT_EXIT(InsertResult.second);
        *ppRetAllocFunc = &(InsertResult.first->second);
    }
    else
    {
        *ppRetAllocFunc = &(itFuncMap->second);
    }

    pAddrInfo->pAllcFunction = *ppRetAllocFunc; // save to cache
Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_GetCallAddrInfo(void *pvAddress, _CALL_ADDR_INFO **ppRetAddrInfo)
{
    int nResult  = false;
    int nRetCode = false;
    CALL_ADDR_MAP::iterator itCallAddr;
    _CALL_ADDR_INFO AddrInfo;
    void *pvBaseAddr = NULL;
    MODULE_MAP::iterator itModule;
    std::string sFunctionname;
    size_t i = 0;

    ASSERT(pvAddress);
    ASSERT(ppRetAddrInfo);

    // -------------------------- search cache first --------------------------
    itCallAddr = m_CallAddrMap.find(pvAddress);
    if (itCallAddr != m_CallAddrMap.end())
    {
        *ppRetAddrInfo = &(itCallAddr->second);
        KG_PROCESS_SUCCESS(true);
    }

    AddrInfo.nIsUserFunction = false;
    // -------------------------- new address --------------------------
    pvBaseAddr = (void *)::SymGetModuleBase64(m_hProcess, (DWORD64)pvAddress);
    itModule = m_ModuleMap.find(pvBaseAddr);
    KG_ASSERT_EXIT(itModule != m_ModuleMap.end());

    // -------------------------- filter system module --------------------------
    for (i = 0; i < _countof(SYSTEM_MODULE_ARRAY); ++i)
    {
        nRetCode = stricmp(itModule->second.ModuleName, SYSTEM_MODULE_ARRAY[i]);
        if (nRetCode == 0)
            break;
    }
    KG_PROCESS_SUCCESS(i < _countof(SYSTEM_MODULE_ARRAY)); // is system module

    // -------------------------- filter some function --------------------------
    nRetCode = m_pDebugger->GetFunctionSymbol(
        (unsigned)(DWORD64)pvAddress, NULL,  &sFunctionname,  NULL
    );
    KG_ASSERT_EXIT(nRetCode);

    for (i = 0; i < _countof(FILETER_FUNCTION_ARRAY); ++i)
    {
        nRetCode = stricmp(sFunctionname.c_str(), FILETER_FUNCTION_ARRAY[i]);
        if (nRetCode == 0)
            break;
    }
    if (i == _countof(FILETER_FUNCTION_ARRAY))  
        AddrInfo.nIsUserFunction = true;

Exit1:
    if (pvBaseAddr)
    {
        CALL_ADDR_MAP::_Pairib InsertResult;
        AddrInfo.pAllcFunction = NULL;
        InsertResult = m_CallAddrMap.insert(make_pair(pvAddress, AddrInfo));
        ASSERT(InsertResult.second);
        *ppRetAddrInfo = &(InsertResult.first->second);
    }
    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////////////
//int KG_MemoryUse::_OnBreakAt_ZwMapViewOfSection(_THREAD *pThread)
//{
//    int nResult  = false;
//    int nRetCode = false;
//    DWORD dwReadSize = 0;
//#ifdef SHOW_DEBUG_INFO
//    void *pvBaseAddr = NULL;
//#endif
//
//    ASSERT(pThread);
//
//    nRetCode = ::ReadProcessMemory(
//        m_hProcess, 
//        (void *)(int64_t)pThread->pcContext->Esp, 
//        &pThread->MapView.Stack, 
//        sizeof(pThread->MapView.Stack), 
//        &dwReadSize
//    );
//    KG_ASSERT_EXIT(nRetCode);
//
//    nRetCode = _SetFunctionReturnBreak(
//        pThread->MapView.Stack.pvReturnAddress, 
//        &KG_MemoryUse::_OnBreakAt_ZwMapViewOfSectionReturn
//    );
//    KG_ASSERT_EXIT(nRetCode);
//
//#ifdef SHOW_DEBUG_INFO
//#endif
//
//    nResult = true;
//Exit0:
//    return nResult;
//}
//////////////////////////////////////////////////////////////////////////////////
//int KG_MemoryUse::_OnBreakAt_ZwMapViewOfSectionReturn(_THREAD *pThread)
//{
//    int nResult  = false;
//    int nRetCode = false;
//    DWORD dwReadSize = 0;
//#ifdef SHOW_DEBUG_INFO
//    void *pvBaseAddr = NULL;
//#endif
//    _ALLOC_FUNCTION *pFunc = NULL;
//
//    ASSERT(pThread);
//
//    pThread->MapView.dwRetCode = pThread->pcContext->Eax;
//
//    nRetCode = ::ReadProcessMemory(
//        m_hProcess, 
//        pThread->MapView.Stack.ppBaseAddress, 
//        &(pThread->MapView.pBaseAddressOut), 
//        sizeof(pThread->MapView.pBaseAddressOut), 
//        &dwReadSize
//    );
//    KG_ASSERT_EXIT(nRetCode);
//
//    if (pThread->MapView.Stack.pSectionOffset)
//    {
//        nRetCode = ::ReadProcessMemory(
//            m_hProcess, 
//            pThread->MapView.Stack.pSectionOffset, 
//            &(pThread->MapView.ullSectionOffsetOut), 
//            sizeof(pThread->MapView.ullSectionOffsetOut), 
//            &dwReadSize
//        );
//        KG_ASSERT_EXIT(nRetCode);
//    }
//
//    if (pThread->MapView.Stack.pViewSize)
//    {
//        nRetCode = ::ReadProcessMemory(
//            m_hProcess, 
//            pThread->MapView.Stack.pViewSize, 
//            &(pThread->MapView.uViewSizeOut), 
//            sizeof(pThread->MapView.uViewSizeOut), 
//            &dwReadSize
//        );
//        KG_ASSERT_EXIT(nRetCode);
//    }
//    nRetCode = _GetAllocFunction(pThread, &pFunc);
//
//#ifdef SHOW_DEBUG_INFO
//    sprintf(
//        gs_DebugInfo,
//        "Memory,0x%p,%u,ZwMapViewOfSection,OP=%u RetCode=0x%x Type=%x\n", 
//        (pThread->MapView.pBaseAddressOut),
//        (pThread->MapView.uViewSizeOut / 1024), 
//        ++gs_OPID,
//        pThread->MapView.dwRetCode,
//        pThread->MapView.Stack.AllocationType
//    );
//    m_pDebugger->OutputDebugString(gs_DebugInfo);
//#endif
//
//    nResult = true;
//Exit0:
//    return nResult;
//}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnBreakAt_ZwAllocateVirtualMemory(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwReadSize = 0;
#ifdef SHOW_DEBUG_INFO
    void *pvBaseAddr = NULL;
#endif

    ASSERT(pThread);

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        (void *)(int64_t)pThread->pcContext->Esp, 
        &pThread->AllocVM.Stack, 
        sizeof(pThread->AllocVM.Stack), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = _SetFunctionReturnBreak(
        pThread->AllocVM.Stack.pvReturnAddress, 
        &KG_MemoryUse::_OnBreakAt_ZwAllocateVirtualMemoryReturn
    );
    KG_ASSERT_EXIT(nRetCode);

#ifdef SHOW_DEBUG_INFO
    nRetCode = ::ReadProcessMemory(
        m_hProcess, pThread->AllocVM.Stack.ppBaseAddress, &pvBaseAddr, sizeof(pvBaseAddr), &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        (void *)(pThread->AllocVM.Stack.pRegionSize), 
        &(pThread->AllocVM.dwRegionSizeIn), 
        sizeof(pThread->AllocVM.dwRegionSizeIn), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);
    
    sprintf(
        gs_DebugInfo,
        "ZwAllocateVirtualMemory(0x%p,0x%p(0x%p),0x%p,0x%p(%u),0x%x,0x%x)\n", 
        pThread->AllocVM.Stack.ProcessHandle, 
        pThread->AllocVM.Stack.ppBaseAddress, 
        pvBaseAddr,
        pThread->AllocVM.Stack.pZeroBits, 
        pThread->AllocVM.Stack.pRegionSize, 
        pThread->AllocVM.dwRegionSizeIn,
        pThread->AllocVM.Stack.AllocationType, 
        pThread->AllocVM.Stack.Protect    
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnBreakAt_ZwAllocateVirtualMemoryReturn(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwReadSize = 0;
  
    ASSERT(pThread);

    pThread->AllocVM.dwRetCode = pThread->pcContext->Eax;
    ++m_Stat.uVMAllocCount;

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        pThread->AllocVM.Stack.ppBaseAddress, 
        &(pThread->AllocVM.pBaseAddressOut), 
        sizeof(pThread->AllocVM.pBaseAddressOut), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        (void *)(pThread->AllocVM.Stack.pRegionSize), 
        &(pThread->AllocVM.dwRegionSizeOut), 
        sizeof(pThread->AllocVM.dwRegionSizeOut), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    // -------------------------- log it --------------------------
    if (pThread->AllocVM.dwRetCode == 0)
    {
        nRetCode = _OnZwAllocateVirtualMemorySuccess(pThread);
        KG_ASSERT_EXIT(nRetCode);
    }

#ifdef SHOW_DEBUG_INFO
    sprintf(
        gs_DebugInfo,
        "Memory,0x%p,%u/%u,ZwAllocateVirtualMemory,OP=%u RetCode=0x%.8x Type=%x\n", 
        (pThread->AllocVM.pBaseAddressOut),
        (pThread->AllocVM.dwRegionSizeIn  / 1024), 
        (pThread->AllocVM.dwRegionSizeOut / 1024), 
        ++gs_OPID,
        pThread->AllocVM.dwRetCode,
        pThread->AllocVM.Stack.AllocationType
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnBreakAt_ZwFreeVirtualMemory(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwReadSize = 0;
#ifdef SHOW_DEBUG_INFO
    void *pvBaseAddr = NULL;
#endif
    ASSERT(pThread);

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        (void *)(int64_t)pThread->pcContext->Esp, 
        &pThread->FreeVM.Stack, 
        sizeof(pThread->FreeVM.Stack), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = _SetFunctionReturnBreak(
        pThread->FreeVM.Stack.pvReturnAddress, 
        &KG_MemoryUse::_OnBreakAt_ZwFreeVirtualMemoryReturn
    );
    KG_ASSERT_EXIT(nRetCode);

#ifdef SHOW_DEBUG_INFO
    nRetCode = ::ReadProcessMemory(
        m_hProcess, pThread->FreeVM.Stack.ppBaseAddress, &pvBaseAddr, sizeof(pvBaseAddr), &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        (void *)(pThread->FreeVM.Stack.pRegionSize), 
        &(pThread->FreeVM.dwRegionSizeIn), 
        sizeof(pThread->FreeVM.dwRegionSizeIn), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    sprintf(
        gs_DebugInfo,
        "ZwFreeVirtualMemory(0x%p,0x%p(0x%p),0x%p(%u),0x%x)\n", 
        pThread->FreeVM.Stack.ProcessHandle, 
        pThread->FreeVM.Stack.ppBaseAddress, 
        pvBaseAddr,
        pThread->FreeVM.Stack.pRegionSize, 
        pThread->FreeVM.dwRegionSizeIn,
        pThread->FreeVM.Stack.FreeType 
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnBreakAt_ZwFreeVirtualMemoryReturn(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwReadSize = 0;

    ASSERT(pThread);

    pThread->FreeVM.dwRetCode = pThread->pcContext->Eax;
    ++m_Stat.uVMFreeCount;

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        pThread->FreeVM.Stack.ppBaseAddress, 
        &(pThread->FreeVM.pvAddressOut), 
        sizeof(pThread->FreeVM.pvAddressOut),
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        (void *)(pThread->FreeVM.Stack.pRegionSize), 
        &(pThread->FreeVM.dwRegionSizeOut), 
        sizeof(pThread->FreeVM.dwRegionSizeOut), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    if (pThread->FreeVM.dwRetCode == 0)
    {
        nRetCode = _OnZwFreeVirtualMemorySuccess(pThread);
        KG_ASSERT_EXIT(nRetCode);
    }
    
#ifdef SHOW_DEBUG_INFO
    sprintf(
        gs_DebugInfo,
        "Memory,0x%p,%u/%u,ZwFreeVirtualMemory,OP=%u RetCode=0x%.8x FreeType=%x\n", 
        pThread->FreeVM.pvAddressOut,
        (pThread->FreeVM.dwRegionSizeIn  / 1024), 
        (pThread->FreeVM.dwRegionSizeOut / 1024), 
        ++gs_OPID,
        pThread->FreeVM.dwRetCode,
        pThread->FreeVM.Stack.FreeType
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnBreakAt_RtlAllocateHeap(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwReadSize = 0;

    ASSERT(pThread);

    KG_PROCESS_SUCCESS(pThread->AllocHeap.nRunningFlag); // recursive call only read first time
    pThread->AllocHeap.nRunningFlag = true;

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        (void *)(int64_t)pThread->pcContext->Esp, 
        &pThread->AllocHeap.Stack, 
        sizeof(pThread->AllocHeap.Stack), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = _SetFunctionReturnBreak(
        pThread->AllocHeap.Stack.pvReturnAddress, 
        &KG_MemoryUse::_OnBreakAt_RtlAllocateHeapReturn
    );
    KG_ASSERT_EXIT(nRetCode);

#ifdef SHOW_DEBUG_INFO
    sprintf(
        gs_DebugInfo,
        "RtlAllocateHeap(0x%p,0x%x,0x%x)\n", 
        pThread->AllocHeap.Stack.hHeap, 
        pThread->AllocHeap.Stack.dwFlags, 
        pThread->AllocHeap.Stack.dwBytes
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnBreakAt_RtlAllocateHeapReturn(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;

    ASSERT(pThread);

    pThread->AllocHeap.nRunningFlag = false;
    pThread->AllocHeap.dwRetCode = pThread->pcContext->Eax;
    ++m_Stat.uHeapAllocCount;

    if (pThread->AllocHeap.dwRetCode)
    {
        nRetCode = _OnRtlAllocateHeapSuccess(pThread);
        KG_ASSERT_EXIT(nRetCode);
    }

#ifdef SHOW_DEBUG_INFO
    sprintf(
        gs_DebugInfo,
        "Memory,0x%.8X,%u,RtlAllocateHeap,OP=%u\n", 
        pThread->AllocHeap.dwRetCode,
        pThread->AllocHeap.Stack.dwBytes,
        ++gs_OPID
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnBreakAt_RtlFreeHeap(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwReadSize = 0;

    ASSERT(pThread);

    KG_PROCESS_SUCCESS(pThread->FreeHeap.nRunningFlag); // recursive call only read first time
    pThread->FreeHeap.nRunningFlag = true;

    nRetCode = ::ReadProcessMemory(
        m_hProcess, 
        (void *)(int64_t)pThread->pcContext->Esp, 
        &pThread->FreeHeap.Stack, 
        sizeof(pThread->FreeHeap.Stack), 
        &dwReadSize
    );
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = _SetFunctionReturnBreak(
        pThread->FreeHeap.Stack.pvReturnAddress,
        &KG_MemoryUse::_OnBreakAt_RtlFreeHeapReturn
    );
    KG_ASSERT_EXIT(nRetCode);

#ifdef SHOW_DEBUG_INFO
    sprintf(
        gs_DebugInfo,
        "RtlFreeHeap(0x%p,0x%x,0x%p)\n", 
        pThread->FreeHeap.Stack.hHeap, 
        pThread->FreeHeap.Stack.dwFlags, 
        pThread->FreeHeap.Stack.lpMem
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnBreakAt_RtlFreeHeapReturn(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;

    ASSERT(pThread);

    pThread->FreeHeap.nRunningFlag = false;
    pThread->FreeHeap.dwRetCode = pThread->pcContext->Eax;
    ++m_Stat.uHeapFreeCount;

    if (pThread->FreeHeap.dwRetCode)
    {
        nRetCode = _OnRtlFreeHeapSuccess(pThread);
        KG_PROCESS_ERROR(nRetCode);
    }

#ifdef SHOW_DEBUG_INFO
    sprintf(
        gs_DebugInfo,
        "Memory,0x%p,0,RtlFreeHeap,OP=%u RetCode=%u\n", 
        pThread->FreeHeap.Stack.lpMem,
        ++gs_OPID,
        pThread->FreeHeap.dwRetCode
    );
    m_pDebugger->OutputDebugString(gs_DebugInfo);
#endif

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnZwAllocateVirtualMemorySuccess(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uRealCommitSize = 0;
    _ALLOC_FUNCTION *pFunc = NULL;
    _ALLOC_BLOCK Block;
    ALLOC_MAP::_Pairib InsertResult;

    ASSERT(pThread);

    nRetCode = _LogAllocVMRegion(pThread, &uRealCommitSize);
    KG_ASSERT_EXIT(nRetCode);

    KG_PROCESS_SUCCESS(pThread->AllocHeap.nRunningFlag);

    KG_PROCESS_SUCCESS(uRealCommitSize == 0);

    nRetCode = _GetAllocFunction(pThread, &pFunc);
    KG_ASSERT_EXIT(nRetCode);

    Block.uAllocSize = uRealCommitSize;
    Block.uHeadSize  = 0;
    Block.pAllocFunc = pFunc;

    ++pFunc->uAllocVMCount;
    pFunc->uVMSize    += Block.uAllocSize;
    pFunc->uTotalSize += Block.uAllocSize;
    m_Stat.uTotalAllocSize += Block.uAllocSize;

    InsertResult = m_AllocMap.insert(make_pair(pThread->AllocVM.pBaseAddressOut, Block));
    KG_ASSERT_EXIT(InsertResult.second);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnZwFreeVirtualMemorySuccess(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    ALLOC_MAP::iterator itAlloc;
    _ALLOC_BLOCK *pBlock = NULL;
    void *pvBegin = NULL;
    void *pvEnd   = NULL;
    ASSERT(pThread);

    KG_PROCESS_SUCCESS(pThread->FreeVM.pvAddressOut == (void *)0x00010000); // this address alloc by system, we can't capture the alloc call by can get the free notify :(
    
    nRetCode = _LogFreeVMRegion(pThread);
    KG_ASSERT_EXIT(nRetCode);

    KG_PROCESS_SUCCESS(pThread->FreeHeap.nRunningFlag);

    pvBegin = pThread->FreeVM.pvAddressOut;
    pvEnd   = (char *)pvBegin + pThread->FreeVM.dwRegionSizeOut;
    // release all alloc information in the range of [pvBegin, pvEnd)
    itAlloc = m_AllocMap.lower_bound(pvBegin);
    while (itAlloc != m_AllocMap.end())
    {
        if (itAlloc->first >= pvEnd)
            break;

        pBlock = &(itAlloc->second);

        ++pBlock->pAllocFunc->uFreeVMCount;
        pBlock->pAllocFunc->uVMSize    -= pBlock->uAllocSize;
        pBlock->pAllocFunc->uTotalSize -= pBlock->uAllocSize;
        m_Stat.uTotalAllocSize         -= pBlock->uAllocSize;
        m_AllocMap.erase(itAlloc++);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnRtlAllocateHeapSuccess(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    _ALLOC_FUNCTION *pFunc = NULL;
    ALLOC_MAP::_Pairib InsertResult;
    _ALLOC_BLOCK Block;
    unsigned uTotalSize = 0;

    ASSERT(pThread);

    nRetCode = _GetAllocFunction(pThread, &pFunc);
    KG_ASSERT_EXIT(nRetCode);

    Block.uAllocSize = (unsigned)pThread->AllocHeap.Stack.dwBytes;
    uTotalSize = ((Block.uAllocSize ? Block.uAllocSize : 1) + HEAP_HEAD_SIZE);
    uTotalSize = ROUND_UP_TO_POWER2(uTotalSize, HEAP_GRANULARITY);
    Block.uHeadSize  = uTotalSize - Block.uAllocSize;
    Block.pAllocFunc = pFunc;

    ++pFunc->uAllocHeapCount;
    pFunc->uHeapAllocSize  += Block.uAllocSize;
    pFunc->uHeapHeadSize   += Block.uHeadSize;
    pFunc->uTotalSize      += uTotalSize;
    m_Stat.uTotalAllocSize += uTotalSize;

    InsertResult = m_AllocMap.insert(make_pair((void *)(DWORD64)pThread->AllocHeap.dwRetCode, Block));
    KG_ASSERT_EXIT(InsertResult.second);
    
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_OnRtlFreeHeapSuccess(_THREAD *pThread)
{
    int nResult  = false;
    ALLOC_MAP::iterator itMem;
    _ALLOC_BLOCK *pBlock = NULL;

    ASSERT(pThread);
    
    KG_PROCESS_SUCCESS(!(pThread->FreeHeap.Stack.lpMem)); // kernel32.CreateFile maybe free a NULL point :(

    itMem = m_AllocMap.find(pThread->FreeHeap.Stack.lpMem);
    KG_PROCESS_ERROR(itMem != m_AllocMap.end());
    
    pBlock = &(itMem->second);
    ++pBlock->pAllocFunc->uFreeHeapCount;
    pBlock->pAllocFunc->uHeapAllocSize -= pBlock->uAllocSize;
    pBlock->pAllocFunc->uHeapHeadSize  -= pBlock->uHeadSize;
    pBlock->pAllocFunc->uTotalSize     -= (pBlock->uAllocSize + pBlock->uHeadSize);
    m_Stat.uTotalAllocSize             -= (pBlock->uAllocSize + pBlock->uHeadSize);

    m_AllocMap.erase(itMem);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_LogAllocVMRegion(_THREAD *pThread, unsigned *puRetCommitSize)
{
    int nResult  = false;
    int nRetCode = false;
    VIRTUAL_MEM_MAP::iterator itVMMap;
    _VM_REGION *pContainRegion = NULL;
    _VM_REGION Region;

    ASSERT(pThread);
    ASSERT(puRetCommitSize);

    Region.pvBase       = pThread->AllocVM.pBaseAddressOut;
    Region.uSize        = pThread->AllocVM.dwRegionSizeOut;
    Region.pvEnd        = (char *)Region.pvBase + Region.uSize;
    Region.nHeapMemFlag = pThread->AllocHeap.nRunningFlag;
    Region.uCommitSize  = 0;
    if (pThread->AllocVM.Stack.AllocationType & MEM_COMMIT)
        Region.uCommitSize = Region.uSize;

    // ---------------- try merge region in same heap  ----------------
    itVMMap = m_VMMap.lower_bound(Region.pvBase);
    if (itVMMap != m_VMMap.begin())
        --itVMMap;

    if (pThread->AllocHeap.nRunningFlag && itVMMap != m_VMMap.end())
    {
        _VM_REGION &CurRegion = itVMMap->second;
        if (
            (CurRegion.pvBase == pThread->AllocHeap.Stack.hHeap) &&
            (CurRegion.pvEnd  == Region.pvBase)
        )
        {
            CurRegion.pvEnd = Region.pvEnd;
            CurRegion.uSize += Region.uSize;
            CurRegion.uCommitSize += Region.uCommitSize;
            CurRegion.nHeapMemFlag = true;
            CurRegion.vecPages.resize(
                CurRegion.vecPages.size() + Region.uSize / PAGE_SIZE, 
                Region.uCommitSize ? true : false
            );
            *puRetCommitSize = Region.uCommitSize;
            KG_PROCESS_SUCCESS(true);
        }
    }

    //
    nRetCode = _GetContainVMRegion(Region.pvBase, &pContainRegion);
    KG_ASSERT_EXIT(nRetCode);

    // ---------------- add new region  ----------------
    if (!pContainRegion)
    {
        VIRTUAL_MEM_MAP::_Pairib InsertResut;
        InsertResut = m_VMMap.insert(make_pair(Region.pvBase, Region));
        KG_ASSERT_EXIT(InsertResut.second);
        pContainRegion = &(InsertResut.first->second);
        pContainRegion->vecPages.resize(
            pContainRegion->uSize / PAGE_SIZE, 
            pContainRegion->uCommitSize ? true : false
        );
        *puRetCommitSize = pContainRegion->uCommitSize;
        KG_PROCESS_SUCCESS(true);
    }

    // ---------------- commit on exist region  ----------------    
    KG_ASSERT_EXIT(pThread->AllocVM.Stack.AllocationType & MEM_COMMIT);
    
    pContainRegion->nHeapMemFlag |= Region.nHeapMemFlag;
    
    *puRetCommitSize = pContainRegion->uCommitSize;
    nRetCode = _RecomputeCommitSize(pContainRegion, true, Region.pvBase, Region.uSize);
    KG_ASSERT_EXIT(nRetCode);

    ASSERT(pContainRegion->uCommitSize >= *puRetCommitSize);
    *puRetCommitSize = pContainRegion->uCommitSize - *puRetCommitSize;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_LogFreeVMRegion(_THREAD *pThread)
{
    int nResult  = false;
    int nRetCode = false;
    _VM_REGION *pContainRegion = NULL;
    void *pvFreeBase = NULL;
    unsigned uSize = 0;

    ASSERT(pThread);

    pvFreeBase = pThread->FreeVM.pvAddressOut;
    uSize  = pThread->FreeVM.dwRegionSizeOut;

    if (pThread->FreeVM.Stack.FreeType & MEM_RELEASE)
    {
        m_VMMap.erase(pvFreeBase);
        KG_PROCESS_SUCCESS(true);
    }

    ASSERT(pThread->FreeVM.Stack.FreeType == MEM_DECOMMIT);

    nRetCode = _GetContainVMRegion(pvFreeBase, &pContainRegion);
    KG_ASSERT_EXIT(nRetCode);
    KG_PROCESS_SUCCESS(!pContainRegion);

    nRetCode = _RecomputeCommitSize(pContainRegion, false, pvFreeBase, uSize);
    KG_ASSERT_EXIT(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_GetContainVMRegion(void *pvAddress, _VM_REGION **ppRetRegion)
{
    int nResult  = false;
    VIRTUAL_MEM_MAP::iterator itVMMap;
    _VM_REGION *pRegion = NULL;

    ASSERT(pvAddress);
    ASSERT(ppRetRegion);

    itVMMap = m_VMMap.lower_bound(pvAddress);
    if (itVMMap != m_VMMap.begin())
        --itVMMap;
    for (; itVMMap != m_VMMap.end(); ++itVMMap)
    {
        _VM_REGION &CurRegion = itVMMap->second;
        if (
            (CurRegion.pvBase <= pvAddress) &&
            (CurRegion.pvEnd  >  pvAddress)
        )
        {
            pRegion = &CurRegion;
            break;
        }
        if (CurRegion.pvBase > pvAddress)
            break;
    }
    
    *ppRetRegion = pRegion;
    nResult = true;
//Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_MemoryUse::_RecomputeCommitSize(
    _VM_REGION *pRegion, int nCommitFlag, void *pvAddress, unsigned uSize
)
{
    int nResult  = false;
    void *pvAddressEnd = NULL;
    unsigned uPageIndex = 0;
    char *pchCurPage = 0;
    char *pchEndPage = 0;
    unsigned uChangeSize = 0;
   
    ASSERT(pRegion);
    ASSERT(pvAddress);
    
    ASSERT(pvAddress >= pRegion->pvBase);
   
    pvAddressEnd = (char *)pvAddress + uSize;
    if (pvAddressEnd > pRegion->pvEnd)
    {
        uSize -= (unsigned)((char *)pvAddressEnd - (char *)pRegion->pvEnd);
        ASSERT(int(uSize) >= 0);
        pvAddressEnd = pRegion->pvEnd;
    }
  
    uPageIndex = (unsigned)((char *)pvAddress - (char *)pRegion->pvBase) / PAGE_SIZE;
    pchCurPage = &(pRegion->vecPages[uPageIndex]);
    pchEndPage = pchCurPage + uSize / PAGE_SIZE;

    while (pchCurPage < pchEndPage)
    {
        if (*pchCurPage != (char)nCommitFlag)
        {
            *pchCurPage = (char)nCommitFlag;
            uChangeSize += PAGE_SIZE;
        }
        ++pchCurPage;
    }

    if (nCommitFlag)
        pRegion->uCommitSize += uChangeSize;
    else
        pRegion->uCommitSize -= uChangeSize;

    nResult = true;
//Exit0:
    return nResult;
}

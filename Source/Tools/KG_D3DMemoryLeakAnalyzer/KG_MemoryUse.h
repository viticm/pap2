////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_MemoryUse.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/7/10 10:50:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_MEMORYUSE_H_
#define _INCLUDE_KG_MEMORYUSE_H_

#include <DbgHelp.h>

struct ALLOC_STAT
{
    unsigned uVMAllocCount;
    unsigned uVMFreeCount;
    unsigned uHeapAllocCount;
    unsigned uHeapFreeCount;
    unsigned uTotalAllocSize;
};

////////////////////////////////////////////////////////////////////////////////
class KG_DebugProcess;
class KG_MemoryUse
{
public:
    KG_MemoryUse(void);
    ~KG_MemoryUse(void);
    int SetDebugger(KG_DebugProcess *pDebugger, HANDLE hProcess);
    int Enable(int nEnable); // default value is true

    int OnModuleLoad(const IMAGEHLP_MODULE64 &crModule);
    int OnBreak(void *pvBreakAddress, HANDLE hThread, const CONTEXT &crContext);
    
    int GetMemRegion(std::string *psRetMemState);
    int GetMemAllocFunction(std::string *psRetMemAllocFunction);
    int GetAllocStat(ALLOC_STAT *pRetStat)
    {
        ASSERT(pRetStat);
        *pRetStat = m_Stat;
        return true;
    };
private:
    struct _THREAD;
    struct _CALL_ADDR_INFO;
    struct _VM_REGION;
    typedef int (KG_MemoryUse::*BREAK_PROC_FUNCTION)(_THREAD *pThread); 
    typedef std::map<void *, BREAK_PROC_FUNCTION> BREAK_MAP;
    struct _ALLOC_FUNCTION;

    int _HookAllocAPI();
    int _SetFunctionBreak(
        const char cszFunctionSymbol[], BREAK_PROC_FUNCTION pfProcFunction
    );
    int _SetFunctionReturnBreak(
        void *pvReturnAddress, BREAK_PROC_FUNCTION pfProcFunction
    );
    int _GetAllocFunction(_THREAD *pThread, _ALLOC_FUNCTION **ppRetAllocFunc);
    int _GetCallAddrInfo(void *pvAddress, _CALL_ADDR_INFO **ppRetAddrInfo);

    //int _OnBreakAt_ZwMapViewOfSection(_THREAD *pThread);
    //int _OnBreakAt_ZwMapViewOfSectionReturn(_THREAD *pThread);
    int _OnBreakAt_ZwAllocateVirtualMemory(_THREAD *pThread);
    int _OnBreakAt_ZwAllocateVirtualMemoryReturn(_THREAD *pThread);
    int _OnBreakAt_ZwFreeVirtualMemory(_THREAD *pThread);
    int _OnBreakAt_ZwFreeVirtualMemoryReturn(_THREAD *pThread);

    int _OnBreakAt_RtlAllocateHeap(_THREAD *pThread);
    int _OnBreakAt_RtlAllocateHeapReturn(_THREAD *pThread);
    int _OnBreakAt_RtlFreeHeap(_THREAD *pThread);
    int _OnBreakAt_RtlFreeHeapReturn(_THREAD *pThread);

    int _OnZwAllocateVirtualMemorySuccess(_THREAD *pThread);
    int _OnZwFreeVirtualMemorySuccess(_THREAD *pThread);
    int _OnRtlAllocateHeapSuccess(_THREAD *pThread);
    int _OnRtlFreeHeapSuccess(_THREAD *pThread);

    int _LogAllocVMRegion(_THREAD *pThread, unsigned *puRetCommitSize);
    int _LogFreeVMRegion(_THREAD *pThread);
    // get the region contain the pvAddress, return null if not found
    int _GetContainVMRegion(void *pvAddress, _VM_REGION **ppRetRegion);
    int _RecomputeCommitSize(
        _VM_REGION *pRegion, int nCommitFlag, void *pvAddress, unsigned uSize
    );
private:
    // ZwMapViewOfFunction can log type: mapped, shareable
    //struct  _ZW_MAP_VIEW_OF_SECTION
    //{
    //    struct _STACK
    //    {
    //        LPVOID  pvReturnAddress;  // ESP
    //        IN HANDLE  SectionHandle;
    //        IN HANDLE  ProcessHandle;
    //        IN OUT PVOID  *ppBaseAddress;
    //        IN ULONG_PTR  ZeroBits;
    //        IN SIZE_T  CommitSize;
    //        IN OUT PLARGE_INTEGER  pSectionOffset  OPTIONAL;
    //        IN OUT PSIZE_T  pViewSize;
    //        //IN SECTION_INHERIT  InheritDisposition;
    //        IN DWORD InheritDisposition;
    //        IN ULONG  AllocationType;
    //        IN ULONG  Win32Protect;
    //    } Stack;
    //    DWORD dwRetCode;
    //    void *pBaseAddressOut;
    //    LARGE_INTEGER ullSectionOffsetOut;
    //    SIZE_T uViewSizeOut;
    //};
    struct _ZW_ALLOCATE_VIRTUAL_MEMORY
    {
        // in param
        DWORD dwRegionSizeIn;
        struct _STACK
        {
            LPVOID pvReturnAddress;  // ESP
            HANDLE  ProcessHandle;  
            PVOID  *ppBaseAddress; 
            LONG_PTR  pZeroBits;
            PSIZE_T  pRegionSize;  
            ULONG  AllocationType;   
            ULONG  Protect;
        } Stack;
        // out param
        DWORD dwRetCode;
        void *pBaseAddressOut;
        DWORD dwRegionSizeOut;
    };
    struct _ZW_FREE_VIRTUAL_MEMORY
    {
        // in param
        DWORD dwRegionSizeIn;
        struct _STACK
        {
            LPVOID  pvReturnAddress;  // ESP
            HANDLE  ProcessHandle;
            PVOID  *ppBaseAddress;
            PSIZE_T pRegionSize;
            ULONG   FreeType;
        } Stack;
        // out param
        DWORD dwRetCode;
        DWORD dwRegionSizeOut;
        void *pvAddressOut;
    };
    struct _RTL_ALLOCATE_HEAP
    {
        int nRunningFlag; // to detect recursive call
        struct _STACK
        {
            LPVOID  pvReturnAddress;  // ESP
            HANDLE  hHeap;
            DWORD   dwFlags;
            SIZE_T  dwBytes;
        } Stack;
        // out param
        DWORD dwRetCode;
    };

    struct _RTL_FREE_HEAP
    {
        int nRunningFlag;
        struct _STACK
        {
            LPVOID  pvReturnAddress;  // ESP
            HANDLE  hHeap;
            DWORD   dwFlags;
            LPVOID  lpMem;
        } Stack;
        // out param
        DWORD dwRetCode;
    };
    ////////////////////////////////////////////////////////////////////////////
    struct _THREAD
    {
        HANDLE hThread;
        const CONTEXT *pcContext;
        _ZW_ALLOCATE_VIRTUAL_MEMORY AllocVM;
        _ZW_FREE_VIRTUAL_MEMORY     FreeVM;
        _RTL_ALLOCATE_HEAP          AllocHeap;
        _RTL_FREE_HEAP              FreeHeap;
        //_ZW_MAP_VIEW_OF_SECTION     MapView;
    };
    typedef std::map<HANDLE, _THREAD> THREAD_MAP;

    typedef std::map<void *, IMAGEHLP_MODULE64> MODULE_MAP;

    struct _ALLOC_FUNCTION
    {
        std::string sModule;
        std::string sName;
        std::string sFileLine;
        unsigned uVMSize;
        unsigned uHeapAllocSize;
        unsigned uHeapHeadSize;// block head or tail used by the memory manager
        unsigned uTotalSize; 
        unsigned uAllocHeapCount;
        unsigned uFreeHeapCount;
        unsigned uAllocVMCount;
        unsigned uFreeVMCount;
    };
    friend bool _SortFunction(
        const KG_MemoryUse::_ALLOC_FUNCTION  *pcLeft, 
        const KG_MemoryUse::_ALLOC_FUNCTION *pcRight
    );
    typedef std::map<std::string, _ALLOC_FUNCTION> FUNCTION_MAP;
  
    struct _ALLOC_BLOCK
    {
        unsigned uAllocSize;
        unsigned uHeadSize; // block head or tail used by the memory manager
        _ALLOC_FUNCTION *pAllocFunc;
    };
    typedef std::map<void *, _ALLOC_BLOCK> ALLOC_MAP;

    struct _VM_REGION 
    {
        void    *pvBase;
        unsigned uSize;
        void    *pvEnd; // = pvBase + uSize
        unsigned uCommitSize;
        int      nHeapMemFlag;
        std::vector<char> vecPages;
        ALLOC_MAP AllocMap;
    };
    typedef std::map<void *, _VM_REGION> VIRTUAL_MEM_MAP;

    struct _CALL_ADDR_INFO  // on function can contain multi allocate call
    {
        int nIsUserFunction; 
        _ALLOC_FUNCTION *pAllcFunction;
    };
    typedef std::map<void *, _CALL_ADDR_INFO> CALL_ADDR_MAP;
private:
    KG_DebugProcess *m_pDebugger;
    HANDLE m_hProcess;
    int    m_nEnableHookMemAllocFlag;
    THREAD_MAP      m_ThreadMap;
    BREAK_MAP       m_BreakMap;
    VIRTUAL_MEM_MAP m_VMMap;
    ALLOC_MAP       m_AllocMap;
    MODULE_MAP      m_ModuleMap;
    FUNCTION_MAP    m_FuncMap;
    CALL_ADDR_MAP   m_CallAddrMap;
    ALLOC_STAT m_Stat;    
};


#endif //_INCLUDE_KG_MEMORYUSE_H_

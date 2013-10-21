////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_D3DMemoryLeak.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/7/8 15:32:21
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_D3DMEMORYLEAK_H_
#define _INCLUDE_KG_D3DMEMORYLEAK_H_

////////////////////////////////////////////////////////////////////////////////
class KG_DebugProcess;
class KG_D3DMemoryLeak
{
public:
    KG_D3DMemoryLeak(void);
    ~KG_D3DMemoryLeak(void);

    int OnDebugOutput(const char cszString[], KG_DebugProcess *pDebugProcess);
    int GetLeakReport(std::string *psRetLeakReport);

private:
    typedef std::vector<unsigned> MEMORY_ID_LIST;
    struct FUNCTION_INFO 
    {
        unsigned        uAddress;
        std::string     sModule;
        std::string     sSymbol;
        MEMORY_ID_LIST  LeakMemoryIDList;
    };
    typedef std::map<unsigned, FUNCTION_INFO> FUNCTION_MAP; // key:address, value:leaked info

    static const unsigned D3D9_DEBUG_DUMP_STACK_COUNT = 16;
    struct MEMORY_INFO 
    {
        unsigned uAddress;
        unsigned uAllocID;
        unsigned uSize;
        unsigned uStackCount;
        FUNCTION_INFO *Stack[D3D9_DEBUG_DUMP_STACK_COUNT];
    };
    typedef std::vector<MEMORY_INFO> MEMORY_LIST;

    struct _LEAK_STACK;
private:
    MEMORY_INFO  m_CurMemInfo;
    MEMORY_LIST  m_MemList;
    FUNCTION_MAP m_FunMap;
    KG_DebugProcess *m_pDebugProcess;
};


#endif //_INCLUDE_KG_D3DMEMORYLEAK_H_

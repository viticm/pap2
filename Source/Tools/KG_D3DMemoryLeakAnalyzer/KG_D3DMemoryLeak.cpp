////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_D3DMemoryLeak.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/7/8 15:32:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG_D3DMemoryLeak.h"
#include "KG_DebugProcess.h"
#include <algorithm>
using namespace std;
////////////////////////////////////////////////////////////////////////////////
struct KG_D3DMemoryLeak::_LEAK_STACK 
{
    unsigned       uStackCount;
    FUNCTION_INFO *Stack[D3D9_DEBUG_DUMP_STACK_COUNT];

    int operator<(const _LEAK_STACK &crRight)
    {
        int WeightL = 0;
        int WeightR = 0;
        char *pchFindCreateL = NULL;
        char *pchFindCreateR = NULL;

        ASSERT(Stack[0]);
        if (Stack[0]->LeakMemoryIDList.size() > 0)
        {
            pchFindCreateL = strstr((char *)(Stack[0]->sSymbol.c_str()), "Create");
            WeightL = Stack[0]->LeakMemoryIDList[0];
        }
        ASSERT(crRight.Stack[0]);
        if (crRight.Stack[0]->LeakMemoryIDList.size() > 0)
        {
            pchFindCreateR = strstr((char *)(crRight.Stack[0]->sSymbol.c_str()), "Create");
            WeightR = crRight.Stack[0]->LeakMemoryIDList[0];
        }
        if (pchFindCreateL && !pchFindCreateR)
            WeightR = 0; 
        if (!pchFindCreateL && pchFindCreateR)
            WeightL = 0; 

        return WeightL > WeightR;
    }
};

////////////////////////////////////////////////////////////////////////////////
KG_D3DMemoryLeak::KG_D3DMemoryLeak(void)
{
}

KG_D3DMemoryLeak::~KG_D3DMemoryLeak(void)
{
}
int KG_D3DMemoryLeak::OnDebugOutput(
    const char cszString[], KG_DebugProcess *pDebugProcess
)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uMemoryAddress = 0;
    unsigned uAllocID = 0;
    unsigned uSize = 0;
    unsigned uStackLevel = 0;
    unsigned uFuncAddress = 0;
    FUNCTION_MAP::iterator it;
    FUNCTION_INFO *pFunInfo = NULL;

    KG_ASSERT_EXIT(cszString);
    KG_ASSERT_EXIT(pDebugProcess);

    nRetCode = strncmp(cszString, "Direct3D9: ", sizeof("Direct3D9: ") - 1);
    KG_PROCESS_SUCCESS(nRetCode != 0);

    nRetCode = sscanf(
        cszString, "Direct3D9: (WARN) :Memory Address: %x lAllocID=%u dwSize=%x,",
        &uMemoryAddress, &uAllocID, &uSize
    );
    if (nRetCode == 3)
    {
        memset(&m_CurMemInfo, 0, sizeof(m_CurMemInfo));
        m_CurMemInfo.uAddress = uMemoryAddress;
        m_CurMemInfo.uAllocID = uAllocID;
        m_CurMemInfo.uSize    = uSize;
        m_CurMemInfo.uStackCount = 0;

        KG_PROCESS_SUCCESS(true);
    }

    nRetCode = sscanf(
        cszString, "Direct3D9: (ERROR) :    [%u] : Address %x",
        &uStackLevel, &uFuncAddress
    );
    KG_PROCESS_SUCCESS(nRetCode != 2);

    pFunInfo = NULL;
    if (uFuncAddress > 0)
    {
        m_CurMemInfo.uStackCount = uStackLevel + 1;

        it = m_FunMap.find(uFuncAddress);
        if (it == m_FunMap.end())
        {
            string sSymbol;
            string sFileLine;
            FUNCTION_MAP::_Pairib InsertResult;
            InsertResult = m_FunMap.insert(make_pair(uFuncAddress, FUNCTION_INFO()));
            ASSERT(InsertResult.second);
            it = InsertResult.first;
            it->second.uAddress = uFuncAddress;
            nRetCode = pDebugProcess->GetFunctionSymbol(
                uFuncAddress, &(it->second.sModule), &sSymbol, &sFileLine
            );
            //KG_ASSERT_EXIT(nRetCode);
             it->second.sSymbol = sSymbol + sFileLine;
        }
        pFunInfo = &(it->second);
        pFunInfo->LeakMemoryIDList.push_back(m_CurMemInfo.uAllocID);
    }

    m_CurMemInfo.Stack[uStackLevel] = pFunInfo;

    if (uStackLevel == (D3D9_DEBUG_DUMP_STACK_COUNT - 1))
    {
        m_MemList.push_back(m_CurMemInfo);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_D3DMemoryLeak::GetLeakReport(std::string *psRetLeakReport)
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uStackLevel = 0;
    MEMORY_LIST::iterator itMem;
    FUNCTION_INFO *pFuncInfo = NULL;
    _LEAK_STACK FilteredStack;
    vector<_LEAK_STACK> aFilteredStackList;
    vector<_LEAK_STACK>::iterator itStack;
    char szNumber[32];

    KG_ASSERT_EXIT(psRetLeakReport);

    for (itMem = m_MemList.begin(); itMem != m_MemList.end(); ++itMem)
    {
        // found the first function in d3d9d.dll be call from application
        // We do not care about the stack in system DLL
        /*
            if stack is:
                model1.func1
                d3d9d.func2
                d3d9d.func3
                app.func4 (may be a call back)
                d3d9d.func5
                app.func6
                ntdll.func7
            we think app.func4 is the first appcation function call d3d9d.dll
        */

        int nFoundD3D9FunFlag = false;
        for (uStackLevel = 0; uStackLevel < itMem->uStackCount; ++uStackLevel)
        {
            pFuncInfo = itMem->Stack[uStackLevel];
            ASSERT(pFuncInfo);
            if (
                (stricmp(pFuncInfo->sModule.c_str(), "d3d9") == 0)  ||
                (stricmp(pFuncInfo->sModule.c_str(), "d3d9d") == 0)
            )
            {
                nFoundD3D9FunFlag = true;
            }
            else if (nFoundD3D9FunFlag)
            {
                break;
            }
        }
        if (
            (!nFoundD3D9FunFlag) ||
            (uStackLevel == 0) ||
            (uStackLevel >= itMem->uStackCount)
        )
            continue;

        // get a filtered stack
        memset(&FilteredStack, 0, sizeof(FilteredStack));
        FilteredStack.uStackCount = itMem->uStackCount - (uStackLevel - 1);
        memcpy(
            &FilteredStack.Stack[0], 
            &(itMem->Stack[uStackLevel - 1]),
            sizeof(FilteredStack.Stack[0]) * FilteredStack.uStackCount
        );

        // if is a new stack, add to list
        for (itStack = aFilteredStackList.begin(); itStack != aFilteredStackList.end(); ++itStack)
        {
            unsigned uMinStackCount = min(FilteredStack.uStackCount, itStack->uStackCount);
            
            ASSERT(uMinStackCount > 0);
            // 第一个栈一定是D3D内部的，在一个D3D的API内部，会在不同的位置调用内部API
            // 使得在FuncMap表中是两项，忽略这个差异，当成一个处理。
            nRetCode = stricmp(
                FilteredStack.Stack[0]->sModule.c_str(),
                itStack->Stack[0]->sModule.c_str()
            );
            if (nRetCode != 0)
                continue;

            nRetCode = stricmp(
                FilteredStack.Stack[0]->sSymbol.c_str(),
                itStack->Stack[0]->sSymbol.c_str()
            );
            if (nRetCode != 0)
                continue;
            
            nRetCode = memcmp(
                &(FilteredStack.Stack[1]),
                &(itStack->Stack[1]),
                sizeof(FilteredStack.Stack[1]) * (uMinStackCount - 1)
            );
            if (nRetCode == 0)
            {
                if (itStack->uStackCount < FilteredStack.uStackCount)
                {
                    memcpy(&(*itStack), &(FilteredStack), sizeof(FilteredStack));
                }
                break;
            }
        }
        if (itStack == aFilteredStackList.end())
        {
            aFilteredStackList.push_back(FilteredStack);
        }
    }
    // new resource maybe link to old resource, but old resource usually not link to new resource,
    // so the the larger id is more likely the one cause concatenated leak
    sort(aFilteredStackList.begin(), aFilteredStackList.end()); 
    
    // ------------------------------ make report ------------------------------
    psRetLeakReport->reserve(1024 * 8);
    for (itStack = aFilteredStackList.begin(); itStack != aFilteredStackList.end(); ++itStack)
    {
        for (uStackLevel = 0; uStackLevel < itStack->uStackCount; ++uStackLevel)
        {
            pFuncInfo = itStack->Stack[uStackLevel];
            ASSERT(pFuncInfo);

            *psRetLeakReport += pFuncInfo->sModule + ".";

            if (pFuncInfo->sSymbol.empty())
            {
                _snprintf(
                    szNumber,
                    sizeof(szNumber) / sizeof(szNumber[0]) - 1, 
                    "%.8X\r\n", 
                    pFuncInfo->uAddress
                );
                szNumber[sizeof(szNumber) / sizeof(szNumber[0]) - 1] = '\0';

                *psRetLeakReport += szNumber;
            }
            else
            {
                *psRetLeakReport += pFuncInfo->sSymbol + "\r\n";
            }
        }
        *psRetLeakReport += "----------------------------------------------------\r\n";
    }
    if (psRetLeakReport->empty())
    {
        *psRetLeakReport = "No Direct3D memory leak ^_^\r\n";
    }

    nResult = true;
Exit0:
    return nResult;
}
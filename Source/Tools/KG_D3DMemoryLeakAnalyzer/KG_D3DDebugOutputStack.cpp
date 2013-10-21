////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_D3DDebugOutputStack.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/9/14 18:43:45
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG_D3DDebugOutputStack.h"
#include "KG_DebugProcess.h"

////////////////////////////////////////////////////////////////////////////////
KG_D3DDebugOutputStack::KG_D3DDebugOutputStack(void)
{
}

KG_D3DDebugOutputStack::~KG_D3DDebugOutputStack(void)
{
}
int KG_D3DDebugOutputStack::OnDebugOutput(
    const char cszString[], KG_DebugProcess *pDebugProcess, DWORD dwThreadID
)
{
    int nResult  = false;
    int nRetCode = false;

    KG_ASSERT_EXIT(cszString);
    KG_ASSERT_EXIT(pDebugProcess);

    nRetCode = strncmp(cszString, "Direct3D9: (WARN) :", sizeof("Direct3D9: (WARN) :") - 1);
    KG_PROCESS_SUCCESS(nRetCode != 0);

    nRetCode = pDebugProcess->OutputDebugString("-- Call Stack for the upper warning --\n");
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = pDebugProcess->OutputStackFrame(dwThreadID);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = pDebugProcess->OutputDebugString("--\n");
    KG_ASSERT_EXIT(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

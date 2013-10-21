////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_D3DDebugOutputStack.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2009/9/14 18:43:37
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_D3DDEBUGOUTPUTSTACK_H_
#define _INCLUDE_KG_D3DDEBUGOUTPUTSTACK_H_

////////////////////////////////////////////////////////////////////////////////
class KG_DebugProcess;
class KG_D3DDebugOutputStack
{
public:
    KG_D3DDebugOutputStack(void);
    ~KG_D3DDebugOutputStack(void);

    int OnDebugOutput(const char cszString[], KG_DebugProcess *pDebugProcess, DWORD dwThreadID);
};


#endif //_INCLUDE_KG_D3DDEBUGOUTPUTSTACK_H_

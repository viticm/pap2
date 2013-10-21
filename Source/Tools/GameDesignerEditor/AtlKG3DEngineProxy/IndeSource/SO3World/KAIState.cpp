////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAIState.cpp 
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : AI×´Ì¬½áµã
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KAIState.h"
#include "KAIAction.h"
#include "KAIManager.h"

int KAIState::GetEventHandler(int nEvent)
{
    int   nAction = 0;

    for (int i = 0; i < (int)m_EventHandleVector.size(); i++)
    {
        if (m_EventHandleVector[i].nEvent == nEvent)
        {
            nAction = m_EventHandleVector[i].nAction;
            break;
        }
    }

    return nAction;
}

int KAIState::LuaHandleEvent(Lua_State* L)
{
    int                 nRetCode    = 0;
    int                 nEvent      = 0;
    int                 nAction     = 0;
    KAI_EVENT_HANDLE    Handle;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nEvent  = (int)Lua_ValueToNumber(L, 1);
    nAction = (int)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nAction > KAI_ACTION_BRANCH_NONE);

    for (int i = 0; i < (int)m_EventHandleVector.size(); i++)
    {
        if (m_EventHandleVector[i].nEvent == nEvent)
        {
            KGLogPrintf(
                KGLOG_ERR, "[AI] Duplicated event handler(%d, %d) <--> (%d, %d)\n", 
                nEvent, nAction, m_EventHandleVector[i].nEvent, m_EventHandleVector[i].nAction
            );
            goto Exit0;
        }
    }

    Handle.nEvent  = nEvent;
    Handle.nAction = nAction;

    m_EventHandleVector.push_back(Handle);

Exit0:
    return 0;
}

DEFINE_LUA_CLASS_BEGIN(KAIState)
    REGISTER_LUA_FUNC(KAIState, HandleEvent)
DEFINE_LUA_CLASS_END(KAIState)


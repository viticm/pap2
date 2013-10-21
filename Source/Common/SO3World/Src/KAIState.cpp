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

KAIActionHandle KAIState::GetEventHandler(int nEvent)
{
    KAIActionHandle Result;

    Result.nAIActionID  = KAI_ACTION_ID_NONE;
    Result.pAIAction    = NULL;

    for (int i = 0; i < (int)m_EventHandleVector.size(); i++)
    {
        if (m_EventHandleVector[i].nEvent == nEvent)
        {
            if (!m_EventHandleVector[i].ActionKey.pAIAction)
            {
                KAIAction* pAction = NULL;
                pAction = m_pAILogic->GetAction(m_EventHandleVector[i].ActionKey.nAIActionID);
                KGLOG_PROCESS_ERROR(pAction);

                m_EventHandleVector[i].ActionKey.pAIAction = pAction;
            }

            Result = m_EventHandleVector[i].ActionKey;
            break;
        }
    }

Exit0:
    return Result;
}

void KAIState::DebugStateInfo()
{
    for (int i = 0; i < (int)m_EventHandleVector.size(); i++)
    {
        int nEvent = m_EventHandleVector[i].nEvent;
        int nAction = m_EventHandleVector[i].ActionKey.nAIActionID;

        KGLogPrintf(KGLOG_DEBUG, "[AI] CurrentState Have AIEvent<%d>, Next Action is %d\n", nEvent, nAction);
    }

    return;
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

    KGLOG_PROCESS_ERROR(nAction > KAI_ACTION_ID_NONE);

    for (int i = 0; i < (int)m_EventHandleVector.size(); i++)
    {
        if (m_EventHandleVector[i].nEvent == nEvent)
        {
            KGLogPrintf(
                KGLOG_ERR, "[AI] Duplicated event handler(%d, %d) <--> (%d, %d)\n", 
                nEvent, nAction, m_EventHandleVector[i].nEvent, m_EventHandleVector[i].ActionKey.nAIActionID
            );
            goto Exit0;
        }
    }

    Handle.nEvent = nEvent;

    Handle.ActionKey.nAIActionID = nAction;
    Handle.ActionKey.pAIAction = NULL;

    m_EventHandleVector.push_back(Handle);

Exit0:
    return 0;
}

DEFINE_LUA_CLASS_BEGIN(KAIState)
    REGISTER_LUA_FUNC(KAIState, HandleEvent)
DEFINE_LUA_CLASS_END(KAIState)


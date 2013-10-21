////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAIVM.cpp
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : AI 状态机外壳
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KAIState.h"
#include "KAIVM.h"
#include "KAIManager.h"
#include "KSO3World.h"

KAIVM::KAIVM()
{
    m_pOwner                = NULL;
    m_nAIType               = 0;
    m_pState                = NULL;
    m_pAILogic              = NULL;
    m_nPrimaryTimerFrame    = 0;
    m_nSecondaryTimerFrame  = 0;
    m_nTertiaryTimerFrame   = 0;
    m_nCurrentEvent         = aevInvalid;
    m_nDebugCount           = 0;
    m_nPendingEvent         = aevInvalid;
    m_nPendingEventParam    = 0;
}

BOOL KAIVM::Setup(KCharacter* pCharacter, int nAIType)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KAILogic*   pAILogic    = NULL;
    int         nInitState  = 0;
    
    assert(pCharacter);

    // 允许动态的切换AI类型(便于调试),但是不允许在一个Action流程中切换
    KG_PROCESS_ERROR(m_nCurrentEvent == 0);

    m_pOwner = pCharacter;

    if (nAIType == 0)
    {
        // 清除原有AI类型
        m_pAILogic  = NULL;
        goto Exit1;
    }

    pAILogic = g_pSO3World->m_AIManager.GetAILogic(nAIType);
    if (!pAILogic)
    {
        KGLogPrintf(KGLOG_ERR, "[AI] Setup AIVM failed, AIType %d is invalid.", nAIType);
        goto Exit0;
    }

    m_nDebugCount   = 0;
    m_nAIType   = nAIType;
    m_pAILogic  = pAILogic;
    nInitState  = m_pAILogic->GetInitState();

    bRetCode = SetState(nInitState);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_nPrimaryTimerFrame    = 0;
    m_nSecondaryTimerFrame  = 0;
    m_nTertiaryTimerFrame   = 0;
    m_nCurrentEvent         = 0;

    FireEvent(aevOnPrimaryTimer, m_pOwner->m_dwID, g_pSO3World->m_nGameLoop);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

int KAIVM::GetAIType()
{
    return m_nAIType;
}


#define MAX_ACTION_CALL  64

void KAIVM::FireEvent(int nEvent, DWORD dwEventSrc, int nEventParam)
{
    int     nEventBlock = m_nCurrentEvent;
    int     nAction     = KAI_ACTION_BRANCH_ERROR;
    int     nCallCount  = 0;
    DWORD   dwScriptID  = ERROR_ID;

    assert(nEvent > 0);

    KG_PROCESS_ERROR(m_pAILogic);

    dwScriptID = m_pAILogic->GetScriptID();

    assert(m_pOwner);
    assert(m_pState);

    // 触发的事件预先判断在当前状态是否有托管
    nAction = m_pState->GetEventHandler(nEvent);
    KG_PROCESS_SUCCESS(nAction == KAI_ACTION_BRANCH_NONE);

    if (m_nDebugCount > 0)
        KGLogPrintf(KGLOG_DEBUG, "[AI] <%lu> On event: %d, src: %lu, param: %d\n", m_pOwner->m_dwID, nEvent, dwEventSrc, nEventParam);

    if (nEventBlock != aevInvalid)
    {
        // 重复事件替代
        if (m_nPendingEvent == aevInvalid || m_nPendingEvent == nEvent)
        {
            m_nPendingEvent = nEvent;
            m_dwPendingEventSrc = dwEventSrc;
            m_nPendingEventParam = nEventParam;
        }
        else
        {
            KGLogPrintf(
                KGLOG_ERR, "[AI] Event %d blocked by %d, PendingEvent %d, ai type %d, npc name is %s.\n", 
                nEvent, nEventBlock, m_nPendingEvent, m_nAIType, m_pOwner->m_szName
            );
        }
        goto Exit0;
    }

    m_nCurrentEvent = nEvent;

    m_pOwner->m_AIData.dwAIEventSrc   = dwEventSrc;
    m_pOwner->m_AIData.nAIEventParam  = nEventParam;

    while (nAction != KAI_ACTION_BRANCH_NONE)
    {
        int nNextAction = KAI_ACTION_BRANCH_ERROR;

        if (nCallCount > MAX_ACTION_CALL)
        {
            // 死循环检测
            KGLogPrintf(
                KGLOG_ERR, "[AI] Action call up to limit(%d), ai type = %d, event = %d, action = %d\n", 
                MAX_ACTION_CALL, m_nAIType, nEvent, nAction
            );
            break;
        }

        if (m_nDebugCount > 0)
        {
            if (g_pSO3World->m_ScriptCenter.IsScriptExist(dwScriptID) && g_pSO3World->m_ScriptCenter.IsFuncExist(dwScriptID, "DebugOnCallAction"))
            {
                int nLuaTopIndex = 0;

                g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

                g_pSO3World->m_ScriptCenter.PushValueToStack(nAction);
                g_pSO3World->m_ScriptCenter.CallFunction(dwScriptID, "DebugOnCallAction", 0);

                g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
            }
            else
            {
                KGLogPrintf(KGLOG_DEBUG, "[AI] <%lu> Call action %d\n", m_pOwner->m_dwID, nAction);
            }
        }

        nNextAction = m_pAILogic->CallAction(m_pOwner, nAction);
        if (nNextAction == KAI_ACTION_BRANCH_ERROR)
        {
            KGLogPrintf(
                KGLOG_ERR, "[AI] Action call return error, AI type = %d, event = %d, action = %d, character id = %u, name:%s, pos(%d, %d, %d)\n", 
                m_nAIType, nEvent, nAction, m_pOwner->m_dwID, m_pOwner->m_szName, m_pOwner->m_nX, m_pOwner->m_nY, m_pOwner->m_nZ
            );

            m_pAILogic = NULL;  // 出错的时候就把AI清掉，防止Log不断刷屏。
            break;
        }

        if (m_nDebugCount > 0)
            m_nDebugCount--;

        nAction = nNextAction;
        nCallCount++;
    }

    m_pOwner->m_AIData.dwAIEventSrc = ERROR_ID;

Exit1:
Exit0:
    m_nCurrentEvent = nEventBlock;
    return;
}

#undef MAX_ACTION_CALL

void KAIVM::Active()
{
    if (m_nPendingEvent != aevInvalid)
    {
        int nEvent = m_nPendingEvent;
        m_nPendingEvent = aevInvalid;

        FireEvent(nEvent, m_dwPendingEventSrc, m_nPendingEventParam);
    }

    if (m_nPrimaryTimerFrame != 0 && g_pSO3World->m_nGameLoop >= m_nPrimaryTimerFrame)
    {
        m_nPrimaryTimerFrame = 0;
        FireEvent(aevOnPrimaryTimer, m_pOwner->m_dwID, g_pSO3World->m_nGameLoop);
    }

    if (m_nSecondaryTimerFrame != 0 && g_pSO3World->m_nGameLoop >= m_nSecondaryTimerFrame)
    {
        m_nSecondaryTimerFrame = 0;
        FireEvent(aevOnSecondaryTimer, m_pOwner->m_dwID, g_pSO3World->m_nGameLoop);
    }

    if (m_nTertiaryTimerFrame != 0 && g_pSO3World->m_nGameLoop >= m_nTertiaryTimerFrame)
    {
        m_nTertiaryTimerFrame = 0;
        FireEvent(aevOnTertiaryTimer, m_pOwner->m_dwID, g_pSO3World->m_nGameLoop);
    }
}

BOOL KAIVM::SetState(int nState)
{
    BOOL        bResult = false;
    KAIState*   pState  = NULL;

    assert(m_pAILogic);

    pState = m_pAILogic->GetState(nState);
    KGLOG_PROCESS_ERROR(pState);

    m_pState = pState;

    bResult = true;
Exit0:
    return bResult;
}

void KAIVM::SetPrimaryTimer(int nFrame)
{
    KGLOG_PROCESS_ERROR(nFrame > 0);

    m_nPrimaryTimerFrame = g_pSO3World->m_nGameLoop + nFrame;
Exit0:
    return;
}

void KAIVM::SetSecondaryTimer(int nFrame)
{
    KGLOG_PROCESS_ERROR(nFrame > 0);

    m_nSecondaryTimerFrame = g_pSO3World->m_nGameLoop + nFrame;
Exit0:
    return;
}

void KAIVM::SetTertiaryTimer(int nFrame)
{
    KGLOG_PROCESS_ERROR(nFrame > 0);

    m_nTertiaryTimerFrame = g_pSO3World->m_nGameLoop + nFrame;
Exit0:
    return;
}

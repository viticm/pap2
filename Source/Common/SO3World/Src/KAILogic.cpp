////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAILogic.cpp
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : AI logic, 一个完整的AI逻辑设定
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KCharacter.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KSO3World.h"
#include "KAIState.h"
#include "KAIVM.h"
#include "KAIManager.h"
#include "KAILogic.h"
#include "Engine/KG_Time.h"


KAILogic::KAILogic()
{
    m_nAIType       = 0;
    m_dwScriptID    = 0;
    m_nInitState    = 0;
}

BOOL KAILogic::Setup(int nType, DWORD dwScriptID)
{
    BOOL      bResult       = false;
    int       nLuaTopIndex  = 0;
    BOOL      bRetCode      = false;

    // 为了支持reload,这个函数可能被重新调用,所以先clear();
    m_ActionTable.clear();
    m_StateTable.clear();
    m_UserActionTable.clear();

    m_nAIType       = nType;
    m_dwScriptID    = dwScriptID;
    m_nInitState    = 0;

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(this);

    bRetCode = g_pSO3World->m_ScriptCenter.CallFunction(dwScriptID, "Setup", 0);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
    return bResult;
}

KAIAction* KAILogic::GetAction(int nActionID)
{
    KAI_ACTION_TABLE::iterator it = m_ActionTable.find(nActionID);

    if (it != m_ActionTable.end())
    {
        return &it->second;
    }

    return NULL;
}

KAIState* KAILogic::GetState(int nState)
{
    KAI_STATE_TABLE::iterator it = m_StateTable.find(nState);

    if (it != m_StateTable.end())
    {
        return &it->second;
    }

    return NULL;
}

int KAILogic::GetInitState()
{
    return m_nInitState;
}

DWORD KAILogic::GetScriptID()
{
    return m_dwScriptID;
}

KAIActionHandle KAILogic::CallAction(KCharacter* pCharacter, KAIActionHandle ActionKey)
{
    KAIActionHandle Result;
    KAIActionHandle NextAction;
    KAIAction*      pAction     = ActionKey.pAIAction;
    int             nActionKey  = 0;

    if (!pAction)
        pAction = GetAction(ActionKey.nAIActionID);

    if (pAction == NULL)
    {
        KGLogPrintf(KGLOG_ERR, "[AI] Invalid action call(%d) in AI(%d)", ActionKey.nAIActionID, m_nAIType);
        goto Exit0;
    }

    nActionKey = pAction->m_nKey;

    if (nActionKey < KAI_USER_ACTION)
    {
        int             nBranchIndex    = 0;
        unsigned long   ulPrevTickCount = 0;
        unsigned long   ulPostTickCount = 0;
        KAI_ACTION_FUNC PAction         = NULL;

        if (g_pSO3World->m_AIManager.m_bLogAIRuntimeStat)
            ulPrevTickCount = KG_GetTickCount();

        PAction = g_pSO3World->m_AIManager.GetActionFunction(nActionKey);
        KGLOG_PROCESS_ERROR(PAction);

        nBranchIndex = (*PAction)(pCharacter, pAction);

        if (nBranchIndex > 0 && nBranchIndex <= KAI_ACTION_BRANCH_NUM)
        {
            int         nNextActionID   = pAction->m_nBranch[nBranchIndex - 1];
            KAIAction*  pNextAction     = pAction->m_pBranch[nBranchIndex - 1];

            if (!pNextAction)
            {
                pNextAction = GetAction(nNextActionID);
                pAction->m_pBranch[nBranchIndex - 1] = pNextAction;
            }

            NextAction.nAIActionID  = nNextActionID;
            NextAction.pAIAction    = pNextAction;
        }

        if (nBranchIndex == -1)
        {
            NextAction.nAIActionID = KAI_ACTION_ID_NONE;
            NextAction.pAIAction = NULL;
        }

        if (g_pSO3World->m_AIManager.m_bLogAIRuntimeStat)
        {
            ulPostTickCount = KG_GetTickCount();

            g_pSO3World->m_AIManager.m_ActionRunTimeStatisticsArray[nActionKey].ullTime += ulPostTickCount - ulPrevTickCount;
            g_pSO3World->m_AIManager.m_ActionRunTimeStatisticsArray[nActionKey].uCount++;
        }
    }
    else
    {
        int nLuaTopIndex = 0;
        int nBranchIndex = 0;

        KUSER_ACTION_TABLE::iterator it = m_UserActionTable.find(nActionKey);

        KGLOG_PROCESS_ERROR(it != m_UserActionTable.end());

        g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

        if (IS_PLAYER(pCharacter->m_dwID))
        {
            g_pSO3World->m_ScriptCenter.PushValueToStack((KPlayer*)pCharacter);
        }
        else
        {
            g_pSO3World->m_ScriptCenter.PushValueToStack((KNpc*)pCharacter);
        }

        g_pSO3World->m_ScriptCenter.PushValueToStack(pAction);

        g_pSO3World->m_ScriptCenter.CallFunction(m_dwScriptID, it->second.c_str(), 1);

        g_pSO3World->m_ScriptCenter.GetValuesFromStack("d", &nBranchIndex);

        g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

        if (nBranchIndex >= 1 && nBranchIndex <= KAI_ACTION_BRANCH_NUM)
        {
            int         nNextActionID   = pAction->m_nBranch[nBranchIndex - 1];
            KAIAction*  pNextAction     = pAction->m_pBranch[nBranchIndex - 1];

            if (!pNextAction)
            {
                pNextAction = GetAction(nNextActionID);
                pAction->m_pBranch[nBranchIndex - 1] = pNextAction;
            }

            NextAction.nAIActionID  = nNextActionID;
            NextAction.pAIAction    = pNextAction;
        }
    }

    Result = NextAction;
Exit0:
    return Result;
}

int KAILogic::LuaNewAction(Lua_State* L)
{
    int         nResult     = 0;
    int         nRetCode    = 0;
    int         nActionID   = KAI_ACTION_ID_NONE;
    int         nActionKey  = eakInvalid;
    KAIAction*  pActionData = NULL;
    std::pair<KAI_ACTION_TABLE::iterator, bool> RetPair;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nActionID = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nActionID > KAI_ACTION_ID_NONE);

    nActionKey = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nActionKey > eakInvalid);

    if (nActionKey < KAI_USER_ACTION)
    {
        KAI_ACTION_FUNC PAction  = NULL;

        PAction = g_pSO3World->m_AIManager.GetActionFunction(nActionKey);
        if (PAction == NULL)
        {
            KGLogPrintf(KGLOG_ERR, "[AI] Unregistered sys action(ID: %d, Key: %d) in ai %d\n", nActionID, nActionKey, m_nAIType);
            goto Exit0;
        }
    }
    else
    {
        KUSER_ACTION_TABLE::iterator it = m_UserActionTable.find(nActionKey);

        if (it == m_UserActionTable.end())
        {
            KGLogPrintf(KGLOG_ERR, "[AI] Unregistered lua actionID: %d, Key: %d) in ai %d\n", nActionID, nActionKey, m_nAIType);
            goto Exit0;
        }
    }

    RetPair = m_ActionTable.insert(std::make_pair(nActionID, KAIAction()));
    if (!RetPair.second)
    {
        KGLogPrintf(KGLOG_ERR, "[AI] Duplicated AI action(%d) in ai %d\n", nActionID, m_nAIType);
        goto Exit0;
    }

    pActionData = &(RetPair.first->second);

    pActionData->m_nKey = nActionKey;

    nResult = pActionData->LuaGetObj(L);
Exit0:
    return nResult;
}

int KAILogic::LuaNewState(Lua_State* L)
{
    int                     nResult     = 0;
    int                     nRetCode    = 0;
    int                     nState      = 0;
    KAIState*               pState      = NULL;
    std::pair<KAI_STATE_TABLE::iterator, bool> RetPair;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nState = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nState > 0);

    RetPair = m_StateTable.insert(std::make_pair(nState, KAIState(this)));
    if (!RetPair.second)
    {
        KGLogPrintf(KGLOG_ERR, "[AI] Duplicated AI state(%d) in ai type %d\n", nState, m_nAIType);
        goto Exit0;
    }

    pState = &(RetPair.first->second);

    nResult = pState->LuaGetObj(L);
Exit0:
    return nResult;
}

int KAILogic::LuaSetInitState(Lua_State* L)
{
    int         nRetCode    = 0;
    int         nInitState  = 0;
    KAIState*   pState      = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nInitState = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nInitState > 0);

    pState = GetState(nInitState);
    KGLOG_PROCESS_ERROR(pState);

    m_nInitState = nInitState;
Exit0:
    return 0;
}

int KAILogic::LuaRegisterUserAction(Lua_State* L)
{
    int         nRetCode    = 0;
    int         nKey        = 0;
    const char* pszFunction = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nKey = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nKey >= KAI_USER_ACTION);

    pszFunction = (const char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszFunction);

    m_UserActionTable[nKey] = pszFunction;
Exit0:
    return 0;
}

DEFINE_LUA_CLASS_BEGIN(KAILogic)

    REGISTER_LUA_INTEGER_READONLY(KAILogic, AIType)

    REGISTER_LUA_FUNC(KAILogic, NewState)
    REGISTER_LUA_FUNC(KAILogic, NewAction)
    REGISTER_LUA_FUNC(KAILogic, SetInitState)
    REGISTER_LUA_FUNC(KAILogic, RegisterUserAction)
DEFINE_LUA_CLASS_END(KAILogic)

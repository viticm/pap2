////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAIAction.cpp
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : 定义所有的Action
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KAIAction.h"
#include "KCharacter.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KAISearchTactics.h"
#include "KAIManager.h"
#include "KMath.h"
#include "KPlayerServer.h"
#include "KAISkillSelector.h"
#include "KNpcOrderList.h"


#define MIN_TIMER_INTERVAL  (GAME_FPS / 4)

#define REGISTER_AI_ACTION_FUNC(__VALUE__)                          \
    do                                                              \
    {                                                               \
        m_ActionFunctionTable[eak##__VALUE__] = AI##__VALUE__;      \
    } while (0)


KAIAction::KAIAction()
{
    memset(m_nParam, 0, sizeof(m_nParam));
    
    for (int i = 0; i < KAI_ACTION_BRANCH_NUM; i++)
    {
        m_nBranch[i] = KAI_ACTION_ID_ERROR;
        m_pBranch[i] = NULL;
    }
}

int KAIAction::LuaSetParam(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nParamCount = 0;
    int     nValue      = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount <= KAI_ACTION_PARAM_NUM);

    for (int i = 0; i < nParamCount; i++)
    {
        nValue = (int)Lua_ValueToNumber(L, i + 1);
        m_nParam[i] = nValue;
    }

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KAIAction::LuaGetParam(Lua_State* L)
{
    for (int i = 0; i < KAI_ACTION_PARAM_NUM; i++)
    {
        Lua_PushNumber(L, m_nParam[i]);
    }

    return KAI_ACTION_PARAM_NUM;
}

int KAIAction::LuaSetBranch(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nParamCount = 0;
    int     nBranch     = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount <= KAI_ACTION_BRANCH_NUM);

    for (int i = 0; i < nParamCount; i++)
    {
        nBranch = (int)Lua_ValueToNumber(L, i + 1);
        KGLOG_PROCESS_ERROR(nBranch > KAI_ACTION_ID_NONE);
        m_nBranch[i] = nBranch;
    }

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

DEFINE_LUA_CLASS_BEGIN(KAIAction)
REGISTER_LUA_FUNC(KAIAction, SetParam)
REGISTER_LUA_FUNC(KAIAction, GetParam)
REGISTER_LUA_FUNC(KAIAction, SetBranch)
DEFINE_LUA_CLASS_END(KAIAction)

///////////////////////////////////////////////////////////////////////////////////////////////

// Function:    切换AI状态
// Param:       [1]StateID
// Branch:      N/A
// Remark:      这个Action之后不能跟任何的Action（没有分支）。
//              切换State后，等待下一个事件发生的时候，由相应的EventHandle决定AI虚拟机的行为。
int AISetState(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nState = pActionData->m_nParam[0];

    pCharacter->m_AIVM.SetState(nState);

    return -1;
}

// Function:    设置主AI定时器
// Param:       [1]Frame
// Branch:      [1]Success
// Remark:      当设置的时间到之后，将触发OnPrimaryTimer事件。
//              当OnPrimaryTimer事件触发后，计数器自动清零，除非再次调用SetPrimaryTimer，否则不会再次触发OnPrimaryTimer事件。
int AISetPrimaryTimer(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nBranchSuccess  = 1;
    int nFrame          = pActionData->m_nParam[0];

    if (nFrame < MIN_TIMER_INTERVAL)
        nFrame = MIN_TIMER_INTERVAL;

    pCharacter->m_AIVM.SetPrimaryTimer(nFrame);

    return nBranchSuccess;
}

// Function:    设置次AI定时器
// Param:       [1]Frame
// Branch:      [1]Success
// Remark:      当设置的时间到之后，将触发OnSecondaryTimer事件。
//              当OnSecondaryTimer事件触发后，计数器自动清零，除非再次调用SetSecondaryTimer，否则不会再次触发OnSecondaryTimer事件。
int AISetSecondaryTimer(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nBranchSuccess  = 1;
    int nFrame          = pActionData->m_nParam[0];

    if (nFrame < MIN_TIMER_INTERVAL)
        nFrame = MIN_TIMER_INTERVAL;

    pCharacter->m_AIVM.SetSecondaryTimer(nFrame);

    return nBranchSuccess;
}

// Function:    设置第三AI定时器
// Param:       [1]Frame
// Branch:      [1]Success
// Remark:      当设置的时间到之后，将触发OnTertiaryTimer事件。
//              当OnTertiaryTimer事件触发后，计数器自动清零，除非再次调用OnTertiaryTimer，否则不会再次触发OnTertiaryTimer事件。
int AISetTertiaryTimer(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nBranchSuccess  = 1;
    int nFrame          = pActionData->m_nParam[0];

    if (nFrame < MIN_TIMER_INTERVAL)
        nFrame = MIN_TIMER_INTERVAL;

    pCharacter->m_AIVM.SetTertiaryTimer(nFrame);

    return nBranchSuccess;
}

// Function:    随机二叉分支
// Param:       [1]Rate1 [2]Rate2
// Branch:      [1]Branch1 [2]Branch2
// Remark:      随机产生一个整数n，0 <= n < Rate1 + Rate2。
//              如果n属于[0, Rate1), 则走Branch1分支；
//              否则走Branch2分支。
int AIRandomBiBranch(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nResult     = 0;
    int nBranch1    = 1;
    int nBranch2    = 2;
    int nRate1      = pActionData->m_nParam[0];
    int nRate2      = pActionData->m_nParam[1];
    int nTotalRate  = nRate1 + nRate2;
    int nRandom     = 0;
   
    KGLOG_PROCESS_ERROR(nRate1 >= 0);
    KGLOG_PROCESS_ERROR(nRate2 >= 0);
    KGLOG_PROCESS_ERROR(nTotalRate > 0);

    nRandom = g_Random(nTotalRate);

    if (nRandom < nRate1)
    {
        nResult = nBranch1;
        goto Exit0;
    }

    nResult = nBranch2;
Exit0:
    return nResult;
}

// Function:    随机三叉分支
// Param:       [1]Rate1 [2]Rate2 [3]Rate3
// Branch:      [1]Branch1 [2]Branch2 [3]Branch3
// Remark:      随机产生一个整数n，0 <= n < Rate1 + Rate2 + Rate3。
//              如果n属于[0, Rate1), 则走Branch1分支；
//              如果n属于[Rate1, Rate1 + Rate2)，则走Branch2分支；
//              否则走Branch3分支。
    int AIRandomTriBranch(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nResult     = 0;
    int nBranch1    = 1;
    int nBranch2    = 2;
    int nBranch3    = 3;
    int nRate1      = pActionData->m_nParam[0];
    int nRate2      = pActionData->m_nParam[1];
    int nRate3      = pActionData->m_nParam[2];
    int nTotalRate  = nRate1 + nRate2 + nRate3;
    int nRandom     = 0;

    KGLOG_PROCESS_ERROR(nRate1 >= 0);
    KGLOG_PROCESS_ERROR(nRate2 >= 0);
    KGLOG_PROCESS_ERROR(nRate3 >= 0);
    KGLOG_PROCESS_ERROR(nTotalRate > 0);

    nRandom = g_Random(nTotalRate);

    if (nRandom < nRate1)
    {
        nResult = nBranch1;
        goto Exit0;
    }

    if (nRandom < nRate1 + nRate2)
    {
        nResult = nBranch2;
        goto Exit0;
    }

    nResult = nBranch3;
Exit0:
    return nResult;
}

// Function:    控制角色站立
// Param:       N/A
// Branch:      [1]Success
// Remark:      
int AIStand(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nBranchSuccess  = 1;

    pCharacter->Stand(true);

    return nBranchSuccess;
}

#ifdef _SERVER
// Function:    设置NPC技能动作定时器
// Param:       NONE
// Branch:      [1]Success,[2]Pass
// Remark:      技能动作定时器,用副定时器机制,设置时间为SkillAniFrame[SkillSelectIndex]
int AINpcSetSecTimerBySkillAni(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult             = 0; 
    int     nBranchSuccess      = 1;
    int     nBranchPass         = 2;
    int     nSkillSelectIndex   = -1;
    int     nFrame              = MIN_TIMER_INTERVAL;
    KNpc*   pNpc                = NULL;

    KGLOG_PROCESS_ERROR(IS_NPC(pCharacter->m_dwID));

    pNpc = (KNpc*)pCharacter;
    assert(pNpc->m_pTemplate);

    nSkillSelectIndex = pNpc->m_nSkillSelectIndex;

    KG_PROCESS_ERROR_RET_CODE(nSkillSelectIndex >= 0 && nSkillSelectIndex < MAX_NPC_AI_SKILL, nBranchPass);

    nFrame = pNpc->m_pTemplate->nSkillAniFrame[pNpc->m_nSkillSelectIndex];

    KG_PROCESS_ERROR_RET_CODE(nFrame > 0, nBranchPass);

    if (nFrame < MIN_TIMER_INTERVAL)
        nFrame = MIN_TIMER_INTERVAL;

    pCharacter->m_AIVM.SetSecondaryTimer(nFrame);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    设置NPC技能停止定时器
// Param:       NONE
// Branch:      [1]Success,[2]Pass
// Remark:      技能停止定时器,用副定时器机制,设置时间为SkillRestFrame[SkillSelectIndex]
int AINpcSetSecTimerBySkillRest(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult             = 0; 
    int     nBranchSuccess      = 1;
    int     nBranchPass         = 2;
    int     nSkillSelectIndex   = -1;
    int     nFrame              = MIN_TIMER_INTERVAL;
    KNpc*   pNpc                = NULL;

    KGLOG_PROCESS_ERROR(IS_NPC(pCharacter->m_dwID));

    pNpc = (KNpc*)pCharacter;
    assert(pNpc->m_pTemplate);
    
    nSkillSelectIndex = pNpc->m_nSkillSelectIndex;

    KG_PROCESS_ERROR_RET_CODE(nSkillSelectIndex >= 0 && nSkillSelectIndex < MAX_NPC_AI_SKILL, nBranchPass);

    nFrame = pNpc->m_pTemplate->nSkillRestFrame[pNpc->m_nSkillSelectIndex];

    KG_PROCESS_ERROR_RET_CODE(nFrame > 0, nBranchPass);

    if (nFrame < MIN_TIMER_INTERVAL)
        nFrame = MIN_TIMER_INTERVAL;

    pCharacter->m_AIVM.SetSecondaryTimer(nFrame);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    根据势力关系寻找四周的敌人
// Param:       [1]Range
// Branch:      [1]Found，[2]NotFound
// Remark:      以指定的范围为半径，寻找四周的敌人。最大不能超过9个Region的范围。
//              当发现有敌人时，将敌人选中为当前选择目标，并执行Found分支，否则进入NotFound分支。
int AISearchEnemy(KCharacter* pCharacter, KAIAction* pActionData)
{    
    int                     nResult             = 0;
    int                     nRetCode            = false;
    int                     nBranchFound        = 1;
    int                     nBranchNotFound     = 2;
    int                     nRange              = pActionData->m_nParam[0];
    BOOL                    bSearchByLevel      = pActionData->m_nParam[1];
    BOOL                    bSearchByTargetLife = pActionData->m_nParam[2];
    TARGET_TYPE             eTargetType         = ttInvalid;
    KSearchForAnyCharacter  Tactic;

    Tactic.m_pSelf                      = pCharacter;
    Tactic.m_nDistance2                 = nRange * nRange;
    Tactic.m_nLifeConversionLevel       = g_pSO3World->m_Settings.m_ConstList.nAISearchByLifeConversionLevel;
    Tactic.m_nAngle                     = DIRECTION_COUNT;
    Tactic.m_nRelation                  = (int)sortEnemy;
    Tactic.m_bAdjustRangeByTargetLife   = bSearchByTargetLife;
    Tactic.m_bAdjustRangeByLevelDiff    = bSearchByLevel;
    Tactic.m_bAdjustByAttribute         = true;
    Tactic.m_bAdjustByVisible           = true;

    AISearchCharacter(Tactic);
    KG_PROCESS_ERROR_RET_CODE(Tactic.m_pResult, nBranchNotFound);

    if (IS_PLAYER(Tactic.m_pResult->m_dwID))
    {
        eTargetType = ttPlayer;
    }
    else
    {
        eTargetType = ttNpc;
    }

    nRetCode = pCharacter->SelectTarget(eTargetType, Tactic.m_pResult->m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = nBranchFound;
Exit0:
    return nResult;
}

// Function:    把当前选择的目标添加进自己的仇恨列表中
// Param:       N/A
// Branch:      [1]Success
// Remark:      
int AIAddTargetToThreatList(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0; 
    int         nRetCode        = false;
    int         nBranchSuccess  = 1;
    KCharacter* pTarget         = NULL;

    nRetCode = pCharacter->m_SelectTarget.GetTarget(&pTarget);
    KGLOG_PROCESS_ERROR(nRetCode);
    assert(pTarget);

    nRetCode = pCharacter->m_SimpThreatList.ModifyThreat(thtSpecialThreat, pTarget, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    if (IS_NPC(pCharacter->m_dwID))
    {
        KNpc* pSelf = (KNpc*)pCharacter;
        pSelf->m_SimpThreatList.UpdateKeepThreatField();
    }

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    选中第一仇恨
// Param:       N/A
// Branch:      [1]Success [2]Failed
// Remark:      将仇恨表中的第一仇恨选择为目标。如果成功则走Success分支，如果第一仇恨不存在则走Failed分支。
int AISelectFirstThreat(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult             = 0;
    BOOL                    bRetCode            = false;
    int                     nBranchSuccess      = 1;
    int                     nBranchFailed       = 2;
    KNpc*                   pSelf               = NULL;
    KCharacter*             pOldTarget          = NULL;
    const KSimpThreatNode*  cpFirstThreatNode   = NULL;
    const KSimpThreatNode*  cpOldThreatNode     = NULL;
    const KSimpThreatNode*  cpSelectThreatNode  = NULL;
    TARGET_TYPE             eTargetType         = ttInvalid;
    THREAT_TYPE             eThreatType         = thtMainThreat;

    assert(eThreatType > thtInvalid && eThreatType < thtTotal);

    KG_PROCESS_ERROR(IS_NPC(pCharacter->m_dwID));
    pSelf = (KNpc*)pCharacter;

    cpFirstThreatNode = pSelf->m_SimpThreatList.GetFirstThreat(eThreatType);
    KG_PROCESS_ERROR_RET_CODE(cpFirstThreatNode, nBranchFailed);
    assert(cpFirstThreatNode->pCharacter);

    pSelf->m_SelectTarget.GetTarget(&pOldTarget);
    if (pOldTarget)
        cpOldThreatNode = pSelf->m_SimpThreatList.FindThreat(pOldTarget);

    cpSelectThreatNode = cpFirstThreatNode;

    if (cpOldThreatNode && cpOldThreatNode != cpFirstThreatNode) // OT judgment. 
    {
        int     nOldThreat      = cpOldThreatNode->nThreat[eThreatType];
        int     nFirstThreat    = cpFirstThreatNode->nThreat[eThreatType];
        BOOL    bOldTopFlag     = cpOldThreatNode->bTopFlag;
        BOOL    bFirstTopFlag   = cpFirstThreatNode->bTopFlag;

        if (bOldTopFlag == bFirstTopFlag && nFirstThreat * 100 <= nOldThreat * pSelf->m_nOverThreatPercent)
            cpSelectThreatNode = cpOldThreatNode;

        if (bOldTopFlag && !bFirstTopFlag)
            cpSelectThreatNode = cpOldThreatNode;
    }

    // 特殊目标时不改变SelectTarget
    if (pCharacter->m_AIData.bSpecialSelectTarget && cpOldThreatNode)
        cpSelectThreatNode = cpOldThreatNode;

    if (IS_PLAYER(cpSelectThreatNode->pCharacter->m_dwID))
        eTargetType = ttPlayer;
    else
        eTargetType = ttNpc;

    bRetCode = pCharacter->SelectTarget(eTargetType, cpSelectThreatNode->pCharacter->m_dwID);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

BOOL SetAIChoiceTarget(int nTargetType, KCharacter* pSelf, KCharacter* pTarget)
{
    BOOL bRetCode = false;

    assert(nTargetType > aittInvalid && nTargetType < aittTotal);

    switch  (nTargetType)
    {
    case aittSelectTarget:
        bRetCode = pSelf->m_SelectTarget.SetTarget(pTarget);
        KG_PROCESS_ERROR(bRetCode);
        break;
    case aittSkillTarget:
        bRetCode = pSelf->m_SkillTarget.SetTarget(pTarget);
        KG_PROCESS_ERROR(bRetCode);
        break;
    default:
        KGLOG_PROCESS_ERROR(nTargetType >= 0);
        bRetCode = pSelf->m_AIData.ChoiceTarget[nTargetType].SetTarget(pTarget);
        KG_PROCESS_ERROR(bRetCode);
        break;
    }

    bRetCode = true;
Exit0:
    return bRetCode;
}

BOOL GetAIChoiceTarget(int nTargetType, KCharacter* pSelf, KCharacter** ppTarget)
{
    BOOL bRetCode = false;

    assert(nTargetType > aittInvalid && nTargetType < aittTotal);

    switch  (nTargetType)
    {
    case aittSelectTarget:
        bRetCode = pSelf->m_SelectTarget.GetTarget(ppTarget);
        KG_PROCESS_ERROR(bRetCode);
        break;
    case aittSkillTarget:
        bRetCode = pSelf->m_SkillTarget.GetTarget(ppTarget);
        KG_PROCESS_ERROR(bRetCode);
        break;
    default:
        KGLOG_PROCESS_ERROR(nTargetType >= 0);
        bRetCode = pSelf->m_AIData.ChoiceTarget[nTargetType].GetTarget(ppTarget);
        KG_PROCESS_ERROR(bRetCode);
        break;
    }

    bRetCode = true;
Exit0:
    return bRetCode;
}

// Function:    选择第N仇恨
// Param:       [1]仇恨类型 [2]第N位 [3]目标类型
// Branch:      [1]Success [2]Failed
// Remark:      在指定类型仇恨表中选择第N个作为指定候选目标,
//              正数表示顺序,负数表示逆序,0为非法,超出则为次序的最后一个
int AISelectNthThreat(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult         = 0;
    BOOL                    bRetCode        = false;
    int                     nBranchSuccess  = 1;
    int                     nBranchFailed   = 2;
    THREAT_TYPE             eThreatType     = (THREAT_TYPE)pActionData->m_nParam[0];
    int                     nThreatIndex    = pActionData->m_nParam[1];
    int                     nTargetType     = pActionData->m_nParam[2];
    int                     nThreatCount    = 0;
    const KSimpThreatNode*  cpThreatNode    = NULL;

    KGLOG_PROCESS_ERROR(nThreatIndex != 0);
    KGLOG_PROCESS_ERROR(eThreatType > thtInvalid  && eThreatType < thtTotal);
    KGLOG_PROCESS_ERROR(nTargetType > aittInvalid && nTargetType < aittTotal);

    bRetCode = pCharacter->m_SimpThreatList.GetThreatListCount(eThreatType, nThreatCount);
    KGLOG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR_RET_CODE(nThreatCount > 0, nBranchFailed);

    if (nThreatIndex > 0)
    {
        nThreatIndex = min(nThreatCount, nThreatIndex);
    }
    else
    {
        nThreatIndex = max(1, nThreatCount + nThreatIndex + 1);
    }

    cpThreatNode = pCharacter->m_SimpThreatList.GetNthThreat(eThreatType, nThreatIndex);
    KGLOG_PROCESS_ERROR(cpThreatNode);

    bRetCode = SetAIChoiceTarget(nTargetType, pCharacter, cpThreatNode->pCharacter);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    仇恨筛选
// Param:       [1]筛选类型 [2]随机参数1 [3]随机参数2 [4]是否不包括当前目标
// Branch:      [1]Success [2]Failed
int AIFilterThreat(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult             = 0;
    int     nCount              = 0;
    int     nBranchSuccess      = 1;
    int     nBranchFailed       = 2;
    int     nFilterType         = pActionData->m_nParam[0];
    int     nFilterParam1       = pActionData->m_nParam[1];
    int     nFilterParam2       = pActionData->m_nParam[2];
    BOOL    bExceptSelectTarget = pActionData->m_nParam[3];

    KGLOG_PROCESS_ERROR(nFilterType > airtInvalid && nFilterType < airtTotal);

    switch (nFilterType)
    {
    case airtBaseTarget:
        nCount = FilterTargetGroupByBase(pCharacter, bExceptSelectTarget);
        KG_PROCESS_ERROR_RET_CODE(nCount > 0, nBranchFailed);
        break;
    case airtRangeTarget:
        nCount = FilterTargetGroupByRange(pCharacter, bExceptSelectTarget, nFilterParam1, nFilterParam2);
        KG_PROCESS_ERROR_RET_CODE(nCount > 0, nBranchFailed);
        break;
    case airtNearestTarget:
        nCount = FilterTargetGroupByNearest(pCharacter, bExceptSelectTarget);
        KG_PROCESS_ERROR_RET_CODE(nCount > 0, nBranchFailed);
        break;
    default:
        KGLOG_PROCESS_ERROR(false);
    }

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    交换目标
// Param:       [1]Src仇恨类型 [2]Dst仇恨类型
// Branch:      [1]Success [2]Failed
int AIChangeTarget(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    BOOL        bRetCode        = false;
    int         nBranchSuccess  = 1;
    int         nBranchFailed   = 2;
    int         nSrcTargetType  = pActionData->m_nParam[0];
    int         nDstTargetType  = pActionData->m_nParam[1];
    KCharacter* pSrcTarget      = NULL;
    KCharacter* pDstTarget      = NULL;

    KGLOG_PROCESS_ERROR(nSrcTargetType > aittInvalid && nSrcTargetType < aittTotal);
    KGLOG_PROCESS_ERROR(nDstTargetType > aittInvalid && nDstTargetType < aittTotal);

    bRetCode = GetAIChoiceTarget(nSrcTargetType, pCharacter, &pDstTarget);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    bRetCode = GetAIChoiceTarget(nDstTargetType, pCharacter, &pSrcTarget);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    bRetCode = SetAIChoiceTarget(nSrcTargetType, pCharacter, pSrcTarget);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    bRetCode = SetAIChoiceTarget(nDstTargetType, pCharacter, pDstTarget);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    设置目标
// Param:       [1]Src仇恨类型 [2]Dst仇恨类型
// Branch:      [1]Success [2]Failed
int AISetTarget(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    BOOL        bRetCode        = false;
    int         nBranchSuccess  = 1;
    int         nBranchFailed   = 2;
    int         nSrcTargetType  = pActionData->m_nParam[0];
    int         nDstTargetType  = pActionData->m_nParam[1];
    KCharacter* pTarget         = NULL;

    KGLOG_PROCESS_ERROR(nSrcTargetType > aittInvalid && nSrcTargetType < aittTotal);
    KGLOG_PROCESS_ERROR(nDstTargetType > aittInvalid && nDstTargetType < aittTotal);

    bRetCode = GetAIChoiceTarget(nSrcTargetType, pCharacter, &pTarget);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    bRetCode = SetAIChoiceTarget(nDstTargetType, pCharacter, pTarget);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    拷贝指定仇恨表到TargetGroup
// Param:       [1]仇恨类型
// Branch:      [1]Success
int AICopyThreatListToTargetGroup(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult         = 0;
    int                     nBranchSuccess  = 1;
    THREAT_TYPE             eThreatType     = (THREAT_TYPE)pActionData->m_nParam[0];
    int                     nCount          = 0;
    const KSimpThreatNode*  cpThreatNode    = NULL;

    KGLOG_PROCESS_ERROR(eThreatType > thtInvalid  && eThreatType < thtTotal);

    cpThreatNode = pCharacter->m_SimpThreatList.GetFirstThreat(eThreatType);
    while (cpThreatNode)
    {
        if (cpThreatNode->pCharacter->m_eMoveState != cmsOnDeath)
        {
            pCharacter->m_AIData.TargetGroup[nCount++] = cpThreatNode->pCharacter;

            if (nCount >= AI_TARGET_GROUP_MAX_COUNT)
                break;
        }

        cpThreatNode = pCharacter->m_SimpThreatList.GetNextThreat(eThreatType, cpThreatNode);
    }
    pCharacter->m_AIData.nTargetGroupCount = nCount;

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    将TargetGroup的第N个放入指定类型的Target中
// Param:       [1]nIndex [2]目标类型
// Branch:      [1]Success [2]Failed
// Remark:      在TargetGroup中选择第N个作为指定候选目标,
//              正数表示顺序,负数表示逆序,0为随机,超出则为次序的最后一个
int AIExtractionTargetGroup(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult                                 = 0;
    BOOL        bRetCode                                = false;
    int         nBranchSuccess                          = 1;
    int         nBranchFailed                           = 2;
    int         nTargetGroupIndex                       = pActionData->m_nParam[0];
    int         nDstTargetType                          = pActionData->m_nParam[1];
    int         nTargetGroupCount                       = 0;
    KCharacter* TempGroup[AI_TARGET_GROUP_MAX_COUNT]    = {NULL};

    KGLOG_PROCESS_ERROR(nDstTargetType > aittInvalid && nDstTargetType < aittTotal);

    for (int nIndex = 0; nIndex < pCharacter->m_AIData.nTargetGroupCount; nIndex++)
    {
        if (pCharacter->m_AIData.TargetGroup[nIndex])
            TempGroup[nTargetGroupCount++] = pCharacter->m_AIData.TargetGroup[nIndex];
    }
    KG_PROCESS_ERROR_RET_CODE(nTargetGroupCount > 0, nBranchFailed);

    if (nTargetGroupIndex == 0)
        nTargetGroupIndex = g_Random(nTargetGroupCount) + 1;

    if (nTargetGroupIndex > 0)
    {
        nTargetGroupIndex = min(nTargetGroupCount, nTargetGroupIndex);
    }
    else
    {
        nTargetGroupIndex = max(1, nTargetGroupCount + nTargetGroupIndex + 1);
    }

    bRetCode = SetAIChoiceTarget(nDstTargetType, pCharacter, TempGroup[nTargetGroupIndex - 1]);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function: 清空总伤害量
int AIClearTotalDamageValue(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nBranchSuccess = 1;

    pCharacter->m_AIData.nAITotalDamageValue = 0;

    return nBranchSuccess;
}

// Function:    总伤害量比较(The Value is TotalDamageValue)
// Param:       [1] Compare
// Branch:      [1] Larger [2] Equal [3] Smaller
int AITotalDamageValueCompare(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nBranchLarger   = 1;
    int     nBranchEqual    = 2;
    int     nBranchSmaller  = 3;
    int     nCompare        = pActionData->m_nParam[0];

    if (pCharacter->m_AIData.nAITotalDamageValue > nCompare)
    {
        nResult = nBranchLarger;
        goto Exit0;
    }

    if (pCharacter->m_AIData.nAITotalDamageValue < nCompare)
    {
        nResult = nBranchSmaller;
        goto Exit0;
    }

    nResult = nBranchEqual;
Exit0:
    return nResult;
}

// Function:    离开返回点距离比较
// Param:       [1] Compare
// Branch:      [1] Larger [2] Equal [3] Smaller
int AIReturnPositionDistanceCompare(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nBranchLarger   = 1;
    int     nBranchEqual    = 2;
    int     nBranchSmaller  = 3;
    int     nCompare        = pActionData->m_nParam[0];
    int     nDistance2      = 0;

    nDistance2 = g_GetDistance2(pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_AIData.nReturnPosX, pCharacter->m_AIData.nReturnPosY);
    if (nDistance2 > nCompare * nCompare)
    {
        nResult = nBranchLarger;
        goto Exit0;
    }

    if (nDistance2 < nCompare * nCompare)
    {
        nResult = nBranchSmaller;
        goto Exit0;
    }

    nResult = nBranchEqual;
Exit0:
    return nResult;
}

// 判断AI的所属人物是否退出战斗
// 分支1：不在战斗状态
// 分支2：在战斗中
int AIIsInFight(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult         = 0;
    int                     nBranchNotOut   = 1;
    int                     nBranchOutFight = 2;
    KNpc*                   pSelf           = NULL;
    const KSimpThreatNode*  cpThreatNode    = NULL;
    KNpcTeam*               pNpcTeam        = NULL;

    KG_PROCESS_ERROR(IS_NPC(pCharacter->m_dwID));
    pSelf = (KNpc*)pCharacter;

    // 检查仇恨表是否为空
    cpThreatNode = pSelf->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    KG_PROCESS_ERROR_RET_CODE(!cpThreatNode, nBranchNotOut);

    nResult = nBranchOutFight;
Exit0:
    return nResult;
}

// Function:    判断目标是否在范围内
// Param:       [1]Range
// Branch:      [1]InRange [2]OutRange
// Remark:      
int AIIsTargetInRange(KCharacter* pCharacter, KAIAction* pActionData)
{
    int             nResult         = 0;
    BOOL            bRetCode        = false;
    int             nBranchIn       = 1;
    int             nBranchOut      = 2;
    int             nRange          = pActionData->m_nParam[0];
    int             nDistance       = 0;
    KCharacter*     pTarget         = NULL;
    
    bRetCode = pCharacter->m_SelectTarget.GetTarget(&pTarget);
    KGLOG_PROCESS_ERROR(bRetCode);

    nDistance = g_GetDistance2(pCharacter->m_nX, pCharacter->m_nY, pTarget->m_nX, pTarget->m_nY);
    KG_PROCESS_ERROR_RET_CODE(nDistance <= nRange * nRange, nBranchOut);

    nResult = nBranchIn;
Exit0:
    return nResult;
}

// Function:    判断目标是否存在
// Param:       N/A
// Branch:      [1]Exist [2]Not Exist
int AIIsTargetExist(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    int         nBranchExist    = 1;
    int         nBranchNotExist = 2;
    KCharacter* pTarget         = NULL;

    pCharacter->m_SelectTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR_RET_CODE(pTarget, nBranchNotExist);

    KG_PROCESS_ERROR_RET_CODE(pTarget->m_eMoveState != cmsOnDeath, nBranchNotExist);    

    nResult = nBranchExist;
Exit0:
    return nResult;
}

// Function:    判断目标是否在水中(暂时防玩家入水卡怪)
// Param:       N/A
// Branch:      [1]Yes [2]No
int AIIsTargeInWater(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    int         nBranchYes      = 1;
    int         nBranchNo       = 2;
    int         nWaterDeepth    = 0;
    BOOL        bRetCode        = false;
    KCharacter* pTarget         = NULL;

    bRetCode = pCharacter->m_SelectTarget.GetTarget(&pTarget);
    KGLOG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_SUCCESS(pTarget->m_eMoveState == cmsOnSwim);
    KG_PROCESS_SUCCESS(pTarget->m_eMoveState == cmsOnFloat);
    KG_PROCESS_SUCCESS(pTarget->m_eMoveState == cmsOnSwimJump);

    KG_PROCESS_ERROR_RET_CODE(pTarget->m_eMoveState == cmsOnJump, nBranchNo);
    KG_PROCESS_ERROR_RET_CODE(pTarget->m_pCell != NULL, nBranchNo);
    KG_PROCESS_ERROR_RET_CODE(pTarget->m_pCell->m_BaseInfo.dwCellType == ctWater, nBranchNo);

    nWaterDeepth = (pTarget->m_pCell->m_wHighLayer - pTarget->m_pCell->m_wLowLayer) * POINT_PER_ALTITUDE;
    KG_PROCESS_ERROR_RET_CODE(nWaterDeepth > GET_SWIM_WATER_LINE(pTarget->m_nHeight), nBranchNo);

Exit1:
    nResult = nBranchYes;
Exit0:
    return nResult;
}

// Function:    判断是否需要追击
// Param:       N/A
// Branch:      [1]Yes [2]No
// Remark:      该模块是针对上次选中的技能类型来决定,
//              当上次技能类型为aistInvalid或者aistPeriodTargetRange则为No,否则为Yes
int AIIsNeedKeepRange(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    int         nBranchYes      = 1;
    int         nBranchNo       = 2;

    switch(pCharacter->m_AIData.nSkillType)
    {
    case aistInvalid:
    case aistPeriodTargetRange:
        nResult = nBranchNo;
        break;
    default:
        nResult = nBranchYes;
        break;
    }
    return nResult;
}

// return 0: Error; 1: Already in range; 2: Moving
int KeepRange(KCharacter* pCharacter, int nMinRange, int nMaxRange)
{
    int                     nResult             = 0;
    int                     nRetCode            = false;
    int                     nCurrentDistance    = 0;
    int                     nKeepRange          = 0;
    int                     nFarRange           = 0;
    int                     nMoveDistance       = 0;
    int                     nMoveDirection      = 0;
    int                     nSelfDstX           = 0;
    int                     nSelfDstY           = 0;
    int                     nPadding            = 0;
    KCharacter*             pTarget             = NULL;
    const KSimpThreatNode*  cpThreatNode        = NULL;

    KGLOG_PROCESS_ERROR(nMinRange >= 0);
    KGLOG_PROCESS_ERROR(nMinRange <= nMaxRange);

    KG_PROCESS_ERROR(pCharacter->m_SelectTarget.GetTargetType() == ttNpc || pCharacter->m_SelectTarget.GetTargetType() == ttPlayer);

    nRetCode = pCharacter->m_SelectTarget.GetTarget(&pTarget);
    KGLOG_PROCESS_ERROR(nRetCode && pTarget);

    KG_PROCESS_ERROR_RET_CODE(pCharacter->m_dwID != pTarget->m_dwID, 1);    // 自己和自己不用保持距离

    nCurrentDistance = g_GetDistance2(pCharacter->m_nX, pCharacter->m_nY, pTarget->m_nX, pTarget->m_nY);
    nCurrentDistance = (int)sqrt((double)nCurrentDistance);

    nKeepRange = (pCharacter->m_nTouchRange + pTarget->m_nTouchRange + CELL_LENGTH / 2);
    nFarRange = (nMaxRange + pCharacter->m_nTouchRange + pTarget->m_nTouchRange - CELL_LENGTH / 2);

    nKeepRange = max(nKeepRange, nMinRange);
    nFarRange = max(nFarRange, nKeepRange);

    KG_PROCESS_ERROR_RET_CODE(nCurrentDistance < nKeepRange || nCurrentDistance > nFarRange, 1);

    if (nCurrentDistance < nKeepRange)
    {
        nMoveDistance = nKeepRange + CELL_LENGTH / 2;
        nMoveDirection = g_GetDirection(pTarget->m_nX, pTarget->m_nY, pCharacter->m_nX, pCharacter->m_nY);
    }
    else if (nCurrentDistance > nFarRange)
    {
        nMoveDistance = nCurrentDistance - (nFarRange + nKeepRange) / 2;
        nMoveDirection = g_GetDirection(pCharacter->m_nX, pCharacter->m_nY, pTarget->m_nX, pTarget->m_nY);
    }

    KG_PROCESS_ERROR_RET_CODE(nMoveDistance > CELL_LENGTH / 2, 1);

    nSelfDstX = pCharacter->m_nX + nMoveDistance * g_Cos(nMoveDirection) / SIN_COS_NUMBER;
    nSelfDstY = pCharacter->m_nY + nMoveDistance * g_Sin(nMoveDirection) / SIN_COS_NUMBER;

    // 如果目标仇恨表中有多个人，就左右随机移动一段距离，避免重叠。
    cpThreatNode = pTarget->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    if (cpThreatNode && pTarget->m_SimpThreatList.GetNextThreat(thtMainThreat, cpThreatNode))
    {
        int nRand = g_Random(1024);
        if (nRand > 512)
        {
            int aAngle[7] = {0, 21, 42, 64, -64, -42, -21};
            int nRandAngle = aAngle[pCharacter->m_dwID % 7];
            int nDeltaX = nSelfDstX - pTarget->m_nX;
            int nDeltaY = nSelfDstY - pTarget->m_nY;
            nSelfDstX = pTarget->m_nX + (nDeltaX * g_Cos(nRandAngle) - nDeltaY * g_Sin(nRandAngle)) / SIN_COS_NUMBER;
            nSelfDstY = pTarget->m_nY + (nDeltaX * g_Sin(nRandAngle) + nDeltaY * g_Cos(nRandAngle)) / SIN_COS_NUMBER;
        }
    }

    pCharacter->RunTo(nSelfDstX, nSelfDstY, true);

    nResult = 2;
Exit0:
    return nResult;
}

// 尝试着保持距离
// 参数1：最小距离
// 参数2：最大距离
// 分支1：在范围内
// 分支2：已经尝试着去保持距离
int AIKeepRange(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nResult             = 0;
    int nRetCode            = false;
    int nBranchInRange      = 1;
    int nBranchKeep         = 2;
    int nMinRange           = pActionData->m_nParam[0];
    int nMaxRange           = pActionData->m_nParam[1];

    KGLOG_PROCESS_ERROR(nMinRange >= 0);
    KGLOG_PROCESS_ERROR(nMinRange <= nMaxRange);

    nRetCode = KeepRange(pCharacter, nMinRange, nMaxRange);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    if (nRetCode == 1)
    {
        nResult = nBranchInRange;
        goto Exit0;
    }

    nResult = nBranchKeep;
Exit0:
    return nResult;
}

// Function:    根据技能选择器选择的技能，保持技能释放距离
// Param:       N/A
// Branch:      [1]InRange [2]Keep
// Remark:      根据所选技能的释放范围调整距离。
//              当前距离已经符合技能释放范围要求是走InRange分支，否者走Keep分支。
int AINpcKeepSkillCastRange(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult             = 0;
    int     nRetCode            = false;
    int     nBranchInRange      = 1;
    int     nBranchKeep         = 2;
    int     nMinRange           = 0;
    int     nMaxRange           = 0;
    DWORD   dwSkillID           = 0;
    DWORD   dwSkillLevel        = 0;
    KNpc*   pNpc                = NULL;
    KSkill* pSkill              = NULL;

    nRetCode = IS_NPC(pCharacter->m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    pNpc = (KNpc*)pCharacter;
    assert(pNpc->m_pTemplate);

    KG_PROCESS_ERROR_RET_CODE(pNpc->m_nSkillSelectIndex >= 0, nBranchKeep);

    dwSkillID       = pNpc->m_pTemplate->dwSkillIDList[pNpc->m_nSkillSelectIndex];
    dwSkillLevel    = pNpc->m_pTemplate->dwSkillLevelList[pNpc->m_nSkillSelectIndex];

    pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW_S(dwSkillID, dwSkillLevel);
    KGLOG_PROCESS_ERROR(pSkill);

    nMinRange = pSkill->m_nMinRadius;
    nMaxRange = pSkill->m_nMaxRadius;

    KGLOG_PROCESS_ERROR(nMinRange >= 0);
    KGLOG_PROCESS_ERROR(nMinRange <= nMaxRange);

    nRetCode = KeepRange(pCharacter, nMinRange, nMaxRange);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    if (nRetCode == 1)
    {
        nResult = nBranchInRange;
        goto Exit0;
    }

    nResult = nBranchKeep;
Exit0:
    return nResult;
}

// return 0: Error; 1: Success; 2: Failed
int CastSkill(KCharacter* pCharacter, DWORD dwSkillID, DWORD dwSkillLevel, int nAISkillType)
{
    int                 nResult                 = 0;
    int                 nRetCode                = false;
    BOOL                bResetSkillFailedCount  = true;
    KCharacter*         pTarget                 = NULL;
    KSkill*             pSkill                  = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;

    KGLOG_PROCESS_ERROR(dwSkillID);
    KGLOG_PROCESS_ERROR(dwSkillLevel);
    KGLOG_PROCESS_ERROR(nAISkillType >= aistInvalid && nAISkillType < aistTotal);

    KG_PROCESS_ERROR_RET_CODE(pCharacter->m_eMoveState < cmsOnKnockedDown, 2);  // 受技能控制状态

    pCharacter->m_AIData.bSpecialSelectTarget = false;
    pCharacter->m_AIData.ChoiceTarget[aittLastSkillTarget] = pCharacter->m_SkillTarget;
    pCharacter->m_SkillTarget = pCharacter->m_SelectTarget;

    // 获取技能目标
    if (pCharacter->m_SkillTarget.GetTargetType() == ttNpc || pCharacter->m_SkillTarget.GetTargetType() == ttPlayer)
    {
        nRetCode = pCharacter->m_SkillTarget.GetTarget(&pTarget);
        KGLOG_PROCESS_ERROR(nRetCode && pTarget);
    }

    if (IS_PLAYER(pCharacter->m_dwID))
    {
        nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, pCharacter);
        if (!nRetCode)
        {
            KGLogPrintf(
                KGLOG_ERR, 
                "Can't find npc skill, npc skill id = %lu, skill level = %lu. Please check you config file. ", 
                dwSkillID, dwSkillLevel
            );
            goto Exit0;
        }

        pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    }
    else
    {
        pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW_S(dwSkillID, dwSkillLevel);
    }
    KGLOG_PROCESS_ERROR(pSkill);

    // 朝向
    if (pTarget && pTarget != pCharacter && pCharacter->m_eMoveState == cmsOnStand)
    {
        int nSubDirection   = 0;
        int nDirection      = g_GetDirection(pCharacter->m_nX, pCharacter->m_nY, pTarget->m_nX, pTarget->m_nY);
        assert(nDirection >= 0 && nDirection <= DIRECTION_COUNT);

        nSubDirection = nDirection - pCharacter->m_nFaceDirection;
        if (nSubDirection >  DIRECTION_COUNT / 2)
            nSubDirection -= DIRECTION_COUNT;
        if (nSubDirection < -DIRECTION_COUNT / 2)
            nSubDirection += DIRECTION_COUNT;

        if (nSubDirection < -pSkill->m_nAngleRange / 2 || nSubDirection > pSkill->m_nAngleRange / 2)
            pCharacter->TurnTo(nDirection, true);
    }

    nRetCode = pSkill->CanCast(pCharacter, pCharacter->m_SkillTarget);    
    switch (nRetCode)
    {
        // 以下情况技能释放失败计数器才计数
    case srcInvalidTarget:
    case srcTooCloseTarget:
    case srcTooFarTarget:
    case srcTargetInvisible:
        pCharacter->m_AIData.nSkillFailedCount ++;
        break;
    }
    if (pCharacter->m_AIVM.m_nDebugCount > 0 && nRetCode != srcSuccess)
    {
        KGLogPrintf(
            KGLOG_ERR, "[AI] CanCast skill failed skill(%lu, %lu), err code: %d\n", 
            dwSkillID, dwSkillLevel, nRetCode
        );
    }
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, 2);

    nRetCode = pCharacter->CastSkill(dwSkillID, dwSkillLevel, pCharacter->m_SkillTarget);
    switch (nRetCode)
    {
        // 以下情况技能释放失败计数器才计数
    case srcInvalidTarget:
    case srcTooCloseTarget:
    case srcTooFarTarget:
    case srcTargetInvisible:
        pCharacter->m_AIData.nSkillFailedCount ++;
        break;
    }
    if (pCharacter->m_AIVM.m_nDebugCount > 0 && nRetCode != srcSuccess)
    {
        KGLogPrintf(
            KGLOG_ERR, "[AI] Cast skill failed skill(%lu, %lu), err code: %d\n", 
            dwSkillID, dwSkillLevel, nRetCode
            );
    }
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, 2);

    pCharacter->m_AIData.nRecordSkillID[airstCastSkill] = dwSkillID;
    pCharacter->m_AIData.nRecordSkillLevel[airstCastSkill] = dwSkillLevel;

    // 技能参数和AI参数中对于自身释放的技能成功后不重置nSkillFailedCount
    switch (nAISkillType)
    {
    case aistPeriodSelf:
    case aistSelfHP:
    case aistPeriodTargetNotClear:
    case aistTargetCountNotClear:
    case aistPeriodTargetRangeNotClear:
        bResetSkillFailedCount = false;
        break;
    default:
        break;
    }

    if (pSkill->m_pBaseInfo)
    {
        switch (pSkill->m_pBaseInfo->nCastMode)
        {
        case scmSector:         // 扇形范围,扇心固定在自己
        case scmCasterArea:     // 以自己为中心的圆形区域
        case scmCasterSingle:   // 对单体对象(限于自己)施放
            bResetSkillFailedCount = false;
            break;
        default:
            break;
        }        
    }
    
    if (bResetSkillFailedCount)
    {
        pCharacter->m_AIData.nCannotAttactCount = 0;
        pCharacter->m_AIData.nSkillFailedCount = 0;
    }

    nResult = 1;
Exit0:
    return nResult;
}

// 释放技能
// 参数1：技能的ID
// 参数2：技能的LEVEL
// 分支1：技能成功的释放
// 分支2：技能释放失败
int AICastSkill(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nRetCode        = false;
    int     nBranchSuccess  = 1;
    int     nBranchFailed   = 2;
    DWORD   dwSkillID       = pActionData->m_nParam[0];
    DWORD   dwSkillLevel    = pActionData->m_nParam[1];

    KGLOG_PROCESS_ERROR(dwSkillID);
    KGLOG_PROCESS_ERROR(dwSkillLevel);

    nRetCode = CastSkill(pCharacter, dwSkillID, dwSkillLevel, aistInvalid);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    if (nRetCode == 2)
    {
        nResult = nBranchFailed;
        goto Exit0;
    }

    if (IS_NPC(pCharacter->m_dwID))
    {
        KNpc* pSelf = (KNpc*)pCharacter;
        KCharacter* pTarget = NULL;
     
        nRetCode = pCharacter->m_SelectTarget.GetTarget(&pTarget);
        if (
            nRetCode && pTarget && (pTarget->m_dwID != pCharacter->m_dwID) && 
            (pTarget->m_eMoveState == cmsOnStand || pTarget->m_eMoveState == cmsOnSit || pTarget->m_eMoveState == cmsOnFloat)
        )
            pSelf->m_SimpThreatList.UpdateKeepThreatField();
    }

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// NPC释放技能选择器选择的技能
// 分支1：技能成功的释放
// 分支2：技能释放失败
int AINpcCastSelectSkill(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nRetCode        = false;
    int     nBranchSuccess  = 1;
    int     nBranchFailed   = 2;
    int     nSelectIndex    = 0;
    KNpc*   pNpc            = NULL;
    DWORD   dwSkillID       = 0;
    DWORD   dwSkillLevel    = 0;

    nRetCode = IS_NPC(pCharacter->m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    pNpc = (KNpc*)pCharacter;
    assert(pNpc->m_pTemplate);

    nSelectIndex    = pNpc->m_nSkillSelectIndex;
    dwSkillID       = pNpc->m_pTemplate->dwSkillIDList[nSelectIndex];
    dwSkillLevel    = pNpc->m_pTemplate->dwSkillLevelList[nSelectIndex];

    KGLOG_PROCESS_ERROR(dwSkillID);
    KGLOG_PROCESS_ERROR(dwSkillLevel);

    KGLOG_PROCESS_ERROR_RET_CODE(g_pSO3World->m_nGameLoop > pNpc->m_nSkillCommomCD, nBranchFailed);
    if (pNpc->m_AIData.nSkillType == aistPeriodTargetRange)
        KG_PROCESS_ERROR_RET_CODE(g_pSO3World->m_nGameLoop > pNpc->m_nSkillCastFrame[nSelectIndex], nBranchFailed);
    else
        KGLOG_PROCESS_ERROR_RET_CODE(g_pSO3World->m_nGameLoop > pNpc->m_nSkillCastFrame[nSelectIndex], nBranchFailed);

    nRetCode = CastSkill(pCharacter, dwSkillID, dwSkillLevel, pCharacter->m_AIData.nSkillType);
    KG_PROCESS_ERROR(nRetCode > 0);

    if (nRetCode == 1)
    {
        KCharacter* pTarget = NULL;

        nRetCode = pCharacter->m_SelectTarget.GetTarget(&pTarget);
        if (
            nRetCode && pTarget && (pTarget->m_dwID != pCharacter->m_dwID) && 
            (pTarget->m_eMoveState == cmsOnStand || pTarget->m_eMoveState == cmsOnSit || pTarget->m_eMoveState == cmsOnFloat)
        )
            pNpc->m_SimpThreatList.UpdateKeepThreatField();

        pNpc->m_nSkillCastFrame[nSelectIndex] = g_pSO3World->m_nGameLoop + pNpc->m_pTemplate->nSkillCastInterval[nSelectIndex];
        pNpc->m_nSkillCommomCD = g_pSO3World->m_nGameLoop + g_pSO3World->m_Settings.m_ConstList.nNpcSkillCommonCD;

        nResult = nBranchSuccess;
        goto Exit0;
    }

    if (pNpc->m_pTemplate->nSkillCastInterval[nSelectIndex] > g_pSO3World->m_Settings.m_ConstList.nNpcCommonShortCD)
        pNpc->m_nSkillCastFrame[nSelectIndex] = g_pSO3World->m_nGameLoop + g_pSO3World->m_Settings.m_ConstList.nNpcCommonShortCD;
    else
        pNpc->m_nSkillCastFrame[nSelectIndex] = g_pSO3World->m_nGameLoop + pNpc->m_pTemplate->nSkillCastInterval[nSelectIndex];

    nResult = nBranchFailed;
Exit0:
    return nResult;
}

// AI设置了系统无敌
// 参数1：是否无敌
// 分支1：设置之后的操作
int AISetSystemShield(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nBranchSuccess  = 1;
    BOOL    bEnable         = pActionData->m_nParam[0];

    pCharacter->m_bSystemShield = bEnable;
    
    //KGLogPrintf(KGLOG_INFO, "[AI] Action SET_SYSTEM_SHIELD = %d\n", bEnable);
    return nBranchSuccess;
}

// AI存储返回的点，下次return的时候返回该点
// 分支1：成功操作后
int AIRecordReturnPosition(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nBranchSuccess  = 1;

    pCharacter->m_AIData.nReturnPosX = pCharacter->m_nX;
    pCharacter->m_AIData.nReturnPosY = pCharacter->m_nY;
    pCharacter->m_AIData.nReturnPosZ = pCharacter->m_nZ;

//     KGLogPrintf(
//         KGLOG_INFO, "[AI] CharacterID:%lu, Action RecordReturnPosition (%d, %d, %d)\n", 
//         pCharacter->m_dwID, 
//         pCharacter->m_AIData.nReturnPosX, pCharacter->m_AIData.nReturnPosY, pCharacter->m_AIData.nReturnPosZ
//     );

    return nBranchSuccess;
}

// AI返回上次的存储的return点
// 参数1：是否直接移动过去
// 分支1：成功的到达return点
// 分支2：在返回的跑动中
int AIReturn(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nRetCode        = false;
    int     nBranchSuccess  = 1;
    int     nBranchOnReturn = 2;
    BOOL    bMoveTo         = pActionData->m_nParam[0];

    KGLOG_PROCESS_ERROR(pCharacter->m_AIData.nReturnPosX >= 0);
    KGLOG_PROCESS_ERROR(pCharacter->m_AIData.nReturnPosY >= 0);

    // 清除仇恨
    if (IS_NPC(pCharacter->m_dwID))
    {
        KNpc* pNpc = (KNpc*)pCharacter;

        if (pNpc->m_nNpcTeamID)
        {
            assert(pNpc->m_pScene);

            nRetCode = pNpc->m_pScene->m_ThreatLink.ClearAllMemberThreat(pNpc->m_nNpcTeamID);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            pCharacter->m_SimpThreatList.ClearAllThreat();
        }
    }
    else
    {
        pCharacter->m_SimpThreatList.ClearAllThreat();
    }

    if ((IS_NPC(pCharacter->m_dwID)) && pCharacter->m_eMoveState != cmsOnDeath)
    {
        // Npc脱离战斗状态立即回复属性
        pCharacter->m_nCurrentLife = pCharacter->m_nMaxLife;
        pCharacter->m_nCurrentMana = pCharacter->m_nMaxMana;
    }

    // 清除目标和部分AI战斗相关参数
    pCharacter->m_SelectTarget.ClearTarget();
    pCharacter->m_AIData.nEscapeCount           = 0;
    pCharacter->m_AIData.nSkillFailedCount      = 0;
    pCharacter->m_AIData.nCannotAttactCount     = 0;
    pCharacter->m_AIData.nAITotalDamageValue    = 0;
    pCharacter->m_AIData.bSpecialSelectTarget   = false;

    if (bMoveTo)
    {
        if (pCharacter->m_AIData.nPatrolPathID > 0)
        {
            KPatrolGroup*   pGroup = NULL; 

            pGroup = pCharacter->m_pScene->m_AIPatrolGroupManager.GetPatrolGroup(pCharacter->m_AIData.nPatrolPathID);
            if (pGroup)
            {
                if (pGroup->nCurrentX || pGroup->nCurrentY || pGroup->nCurrentZ)
                {
                    pCharacter->m_AIData.nReturnPosX = pGroup->nCurrentX;
                    pCharacter->m_AIData.nReturnPosY = pGroup->nCurrentY;
                    pCharacter->m_AIData.nReturnPosZ = pGroup->nCurrentZ;
                }
            }
        }


        // 瞬移到ReturnPoint
        nRetCode = pCharacter->MoveTo(
            pCharacter->m_AIData.nReturnPosX, pCharacter->m_AIData.nReturnPosY, 
            pCharacter->m_AIData.nReturnPosZ, false, true
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(pCharacter->m_pScene);

        pCharacter->Stop();
        KG_PROCESS_ERROR_RET_CODE(false, nBranchSuccess);
    }
    else
    {
        // 跑回到ReturnPoint
        pCharacter->RunTo(pCharacter->m_AIData.nReturnPosX, pCharacter->m_AIData.nReturnPosY, true);
    }

    nResult = nBranchOnReturn;
Exit0:
    return nResult;
}

// 只用于人数限制小于30的地图
// 降低对当前目标的仇恨
// 参数:None
// 分支:成功,失败
// 说明:如果只有一个仇恨目标,则即使降低仇恨也没有意义,按照失败处理
//      连续清除达到上限也按照失败处理,上限为地图允许最大人数
int AIClearThreatForTarget(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult             = 0;
    int                     nBranchSuccess      = 1;
    int                     nBranchFailed       = 2;
    int                     nDeltaThreat        = 0;
    const KSimpThreatNode*  cpFirstThreatNode   = NULL;
    const KSimpThreatNode*  cpSecondThreatNode  = NULL;
    KMapParams*             pMapParam           = NULL;

    assert(pCharacter->m_pScene);

    cpFirstThreatNode = pCharacter->m_SimpThreatList.GetFirstThreat(thtMainThreat);
    KG_PROCESS_ERROR_RET_CODE(cpFirstThreatNode, nBranchFailed);

    cpSecondThreatNode = pCharacter->m_SimpThreatList.GetNextThreat(thtMainThreat, cpFirstThreatNode);
    KG_PROCESS_ERROR_RET_CODE(cpSecondThreatNode, nBranchFailed);

    nDeltaThreat = min(cpSecondThreatNode->nThreat[thtMainThreat] - 50, 0) - cpFirstThreatNode->nThreat[thtMainThreat];
    pCharacter->m_SimpThreatList.ModifyThreat(thtMainThreat, cpFirstThreatNode->pCharacter, nDeltaThreat);
    pCharacter->m_AIData.nCannotAttactCount++;

    pMapParam = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(pCharacter->m_pScene->m_dwMapID);
    KGLOG_PROCESS_ERROR(pMapParam && pMapParam->nMaxPlayerCount <= 30);
    KG_PROCESS_ERROR_RET_CODE(pCharacter->m_AIData.nCannotAttactCount <= pMapParam->nMaxPlayerCount, nBranchFailed);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// 为AI设置巡逻路经
// 参数1：巡逻路经ID
// 分支1：操作之后
int AIEnterPatrolPath(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult             = 0;
    int                     nBranchSuccess      = 1;
    int                     nPatrolPathID       = pActionData->m_nParam[0];
    int                     nPatrolOrderIndex   = pActionData->m_nParam[1];
    int                     nPatrolPointIndex   = 0;
    int                     nTotalPoints        = 0;
    KPatrolPath*            pPatrolPath         = NULL;
    KAIPatrolGroupManager*  pPatrolGroupManager = NULL;

    KGLOG_PROCESS_ERROR(nPatrolOrderIndex >= 0);

    assert(pCharacter->m_pScene);
    pPatrolGroupManager = &(pCharacter->m_pScene->m_AIPatrolGroupManager);

    if (pCharacter->m_AIData.nPatrolPathID)
        pPatrolGroupManager->LeaveGroup(pCharacter->m_AIData.nPatrolPathID, pCharacter->m_dwID);

    if (!nPatrolPathID)
    {
        pCharacter->m_AIData.nPatrolOrderIndex  = nPatrolOrderIndex;
        pCharacter->m_AIData.nPatrolPathID      = nPatrolPathID;
        pCharacter->m_AIData.pPatrolPath        = NULL;
        goto Exit1;
    }

    pPatrolPath = g_pSO3World->m_Settings.m_PatrolPathList.GetPatrolPath(pCharacter->m_pScene->m_dwMapID, nPatrolPathID);
    KGLOG_PROCESS_ERROR(pPatrolPath);

    pCharacter->m_AIData.nPatrolOrderIndex  = nPatrolOrderIndex;
    pCharacter->m_AIData.nPatrolPathID      = nPatrolPathID;
    pCharacter->m_AIData.pPatrolPath        = pPatrolPath;

    nPatrolPointIndex = pPatrolGroupManager->GetGroupPatrolPathIndex(pCharacter->m_AIData.nPatrolPathID);
    nTotalPoints = pPatrolPath->GetPointCount();

    if (nPatrolPointIndex >= nTotalPoints)
        pPatrolGroupManager->SetGroupPatrolPathIndex(pCharacter->m_AIData.nPatrolPathID, 0);

    pPatrolGroupManager->JoinGroup(pCharacter->m_AIData.nPatrolPathID, pCharacter->m_dwID);

Exit1:
    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

int AILeavePatrolPath(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult             = 0;
    int                     nBranchSuccess      = 1;
    KAIPatrolGroupManager*  pPatrolGroupManager = NULL;

    assert(pCharacter->m_pScene);
    pPatrolGroupManager = &(pCharacter->m_pScene->m_AIPatrolGroupManager);

    pPatrolGroupManager->LeaveGroup(pCharacter->m_AIData.nPatrolPathID, pCharacter->m_dwID);

    pCharacter->m_AIData.pPatrolPath = NULL;

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    AI巡逻
// Param:       N/A
// Branch:      [1]Success [2]End
// Remark:      如果角色已经到达当前巡逻点则走Node分支。
//              否则继续走向当前巡逻点，并走Success分支。
int AIPatrol(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult             = 0;
    int                     nRetCode            = false;
    int                     nBranchSuccess      = 1;
    int                     nBranchEnd          = 2;
    KNpc*                   pNpc                = NULL;

    KAIPatrolGroupManager*  pPatrolGroupManager = NULL;
    int                     nPatrolPathIndex    = 0;
    KPatrolNode             PatrolNode;

    int                     nGroupWalkSpeed     = 0;
    int                     nSpeedStep          = 0;

    int                     nX                  = 0;
    int                     nY                  = 0;
    int                     nDistance           = 0;
    int                     nDirection          = 0;

    DWORD                   dwLeader            = ERROR_ID;
    int                     nNpcOrderIndex      = 0;
    int                     nRestFrame          = 0;

    KGLOG_PROCESS_ERROR(IS_NPC(pCharacter->m_dwID));

    nNpcOrderIndex = pCharacter->m_AIData.nPatrolOrderIndex;
    pNpc = (KNpc*)pCharacter;
    if (!nNpcOrderIndex)
        nNpcOrderIndex = pNpc->m_nNpcTeamIndex + 1;

    assert(pCharacter->m_pScene);
    pPatrolGroupManager = &(pCharacter->m_pScene->m_AIPatrolGroupManager);

    KG_PROCESS_SUCCESS(!(pCharacter->m_AIData.nPatrolPathID));

    KGLOG_PROCESS_ERROR(pCharacter->m_AIData.pPatrolPath);

    nGroupWalkSpeed = max(1, pCharacter->m_AIData.pPatrolPath->m_nWalkSpeed);

    dwLeader = pPatrolGroupManager->GetLeader(pCharacter->m_AIData.nPatrolPathID);
    if (pCharacter->m_dwID != dwLeader) // 向队伍的位置移动
    {
        KPatrolGroup* pGroup = pPatrolGroupManager->GetPatrolGroup(pCharacter->m_AIData.nPatrolPathID);

        // 貌似队长还没有走，这次先不走，等下次再走
        if (!pGroup || pGroup->nDirection == -1)
        {
            pCharacter->m_AIVM.SetSecondaryTimer(GAME_FPS);
            goto Exit1;
        }

        nRetCode = pPatrolGroupManager->GetMemberPosition(pCharacter->m_AIData.nPatrolPathID, nNpcOrderIndex - 1, &nX, &nY, &nDirection);
        KGLOG_PROCESS_ERROR(nRetCode);

        nDistance = g_GetDistance2(pCharacter->m_nX, pCharacter->m_nY, nX, nY);
        nDistance = (int)sqrt((float)nDistance);

        if (nDistance <= CELL_LENGTH / 2 + CELL_LENGTH / 4)
            nGroupWalkSpeed = 0;
        else if (nDistance <= nGroupWalkSpeed * GAME_FPS * 3)
            nGroupWalkSpeed = (int)((double)nDistance / GAME_FPS + 0.5);
        else
            nGroupWalkSpeed = nGroupWalkSpeed * 3;

        if (nGroupWalkSpeed == 0)
        {
            pCharacter->Stand(true);
        }
        else
        {
            nDirection = g_GetDirection(pCharacter->m_nX, pCharacter->m_nY, nX, nY);

            nX = pCharacter->m_nX + nGroupWalkSpeed * (GAME_FPS + GAME_FPS / 2) * g_Cos(nDirection) / SIN_COS_NUMBER;
            nY = pCharacter->m_nY + nGroupWalkSpeed * (GAME_FPS + GAME_FPS / 2) * g_Sin(nDirection) / SIN_COS_NUMBER;

            if (nGroupWalkSpeed < 15)
                pCharacter->WalkTo(nX, nY, true, nGroupWalkSpeed);
            else
                pCharacter->RunTo(nX, nY, true, nGroupWalkSpeed);
        }

        pCharacter->m_AIVM.SetSecondaryTimer(GAME_FPS);

        goto Exit1;
    }

    // 自己就是领队，按自己的巡逻路线走。
    nRestFrame = pPatrolGroupManager->GetRestFrame(pCharacter->m_AIData.nPatrolPathID);
    if (nRestFrame > 0)
    {
        pCharacter->m_AIVM.SetSecondaryTimer(nRestFrame);
        goto Exit1;
    }

    nPatrolPathIndex = pPatrolGroupManager->GetGroupPatrolPathIndex(pCharacter->m_AIData.nPatrolPathID);

    nRetCode = pCharacter->m_AIData.pPatrolPath->GetPoint(nPatrolPathIndex, PatrolNode);
    KGLOG_PROCESS_ERROR(nRetCode);

    nDistance = g_GetDistance2(pCharacter->m_nX, pCharacter->m_nY, PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY);
    if (nDistance <= CELL_LENGTH * CELL_LENGTH + nGroupWalkSpeed * nGroupWalkSpeed * GAME_FPS * GAME_FPS / 4) // 到达节点
    {
        int nTotalPoints = pCharacter->m_AIData.pPatrolPath->GetPointCount();

        nRetCode = pPatrolGroupManager->UpdateGroupPosition(
            pCharacter->m_AIData.nPatrolPathID, nNpcOrderIndex - 1, pCharacter->m_nX, pCharacter->m_nY, 
            pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_nZ, pCharacter->m_nFaceDirection
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        if (PatrolNode.nRestFrame > 0)
        {
            pCharacter->Stand(true);

            if (PatrolNode.nFaceTo)
            {
                pCharacter->TurnTo(PatrolNode.nFaceTo, true);
                
                nRetCode = pPatrolGroupManager->UpdateGroupPosition(
                    pCharacter->m_AIData.nPatrolPathID, nNpcOrderIndex - 1, pCharacter->m_nX, pCharacter->m_nY, 
                    pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_nZ, pCharacter->m_nFaceDirection
                );
                KGLOG_PROCESS_ERROR(nRetCode);
            }

            if (PatrolNode.nAnimationID)
            {
                // TODO: Animation
            }

            pPatrolGroupManager->SetRestFrame(pCharacter->m_AIData.nPatrolPathID, PatrolNode.nRestFrame);
            pCharacter->m_AIVM.SetSecondaryTimer(PatrolNode.nRestFrame);
        }
        else
        {
            pCharacter->m_AIVM.SetSecondaryTimer(1);
        }

        //触发Npc寻路到达的脚本
        if (PatrolNode.szScriptName[0] != '\0')
        {
            nRetCode= g_pSO3World->m_ScriptCenter.IsScriptExist(PatrolNode.szScriptName);
            if (nRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(PatrolNode.szScriptName, SCRIPT_ON_PATROL))
            {
                int nTopIndex = 0;
                g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

                g_pSO3World->m_ScriptCenter.PushValueToStack(pNpc);
                g_pSO3World->m_ScriptCenter.PushValueToStack(nPatrolPathIndex + 1);

                g_pSO3World->m_ScriptCenter.CallFunction(PatrolNode.szScriptName, SCRIPT_ON_PATROL, 0);
                g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
            }
        }

        nPatrolPathIndex++;

        if (nPatrolPathIndex >= nTotalPoints)   // End
        {
            pPatrolGroupManager->SetGroupPatrolPathIndex(pCharacter->m_AIData.nPatrolPathID, 0);

            nResult = nBranchEnd;
            goto Exit0;
        }

        pPatrolGroupManager->SetGroupPatrolPathIndex(pCharacter->m_AIData.nPatrolPathID, nPatrolPathIndex);

        goto Exit1;
    }

    nDirection = g_GetDirection(pCharacter->m_nX, pCharacter->m_nY, PatrolNode.nPatrolPointX, PatrolNode.nPatrolPointY);

    nDistance = (int)sqrt((float)nDistance);
    nDistance = min(nDistance, nGroupWalkSpeed * GAME_FPS);

    nX = pCharacter->m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
    nY = pCharacter->m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    nRetCode = pPatrolGroupManager->UpdateGroupPosition(
        pCharacter->m_AIData.nPatrolPathID, nNpcOrderIndex - 1, nX, nY, 
        pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_nZ, nDirection
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nDistance += nGroupWalkSpeed * GAME_FPS / 2;

    nX = pCharacter->m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
    nY = pCharacter->m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    if (nGroupWalkSpeed < 15)
        pCharacter->WalkTo(nX, nY, true, nGroupWalkSpeed);
    else
        pCharacter->RunTo(nX, nY, true, nGroupWalkSpeed);

    pCharacter->m_AIVM.SetSecondaryTimer(nDistance / nGroupWalkSpeed - GAME_FPS / 2);

Exit1:
    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// 设置AI所属人物的重生点
// 分支1：下一步操作
int AIRecordOriginPosition(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nRetCode        = false;
    int     nBranchSuccess  = 1;

    pCharacter->m_AIData.nOriginPosX        = pCharacter->m_nX;
    pCharacter->m_AIData.nOriginPosY        = pCharacter->m_nY;
    pCharacter->m_AIData.nOriginPosZ        = pCharacter->m_nZ;
    pCharacter->m_AIData.nOriginDirection   = pCharacter->m_nFaceDirection;

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// 设置AI所属人物的面朝方向
// 分支1：下一步操作
int AIKeepOriginDirection(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nRetCode        = false;
    int     nBranchSuccess  = 1;

    if (pCharacter->m_eMoveState == cmsOnStand && pCharacter->m_nFaceDirection != pCharacter->m_AIData.nOriginDirection)
    {
        nRetCode = pCharacter->TurnTo(pCharacter->m_AIData.nOriginDirection, true);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    面向目标
// Param:       [1]Enable
// Branch:      [1]Success
// Remark:      当Enable参数为非0时，朝向目标；Enable参数为0时，什么都不做。
int AIFaceTarget(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    int         nRetCode        = false;
    int         nBranchSuccess  = 1;
    int         nEnable         = pActionData->m_nParam[0];
    int         nDirection      = 0;
    KCharacter* pTarget         = NULL;

    KG_PROCESS_SUCCESS(!nEnable);

    nRetCode = pCharacter->m_SelectTarget.GetTarget(&pTarget);
    KGLOG_PROCESS_ERROR(nRetCode);
    assert(pTarget);

    KG_PROCESS_SUCCESS(pCharacter->m_dwID == pTarget->m_dwID);    // 自己和自己不用转向

    nDirection = g_GetDirection(pCharacter->m_nX, pCharacter->m_nY, pTarget->m_nX, pTarget->m_nY);

    if (pCharacter->m_eMoveState == cmsOnStand && pCharacter->m_nFaceDirection != nDirection)
    {
        nRetCode = pCharacter->TurnTo(nDirection, true);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    闲逛一次
// Param:       [1]Range [2]Distance
// Branch:      [1]Success
// Remark:      Range参数限制闲逛的范围，如果当前位置到原点位置的距离大于Range，则向原点方向走；
//              否则随机选一个方向走。Distance控制每次闲逛的最大距离。
int AIWander(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    int         nRetCode        = false;
    int         nBranchSuccess  = 1;
    int         nRange          = pActionData->m_nParam[0];
    int         nDistance       = pActionData->m_nParam[1];
    int         nDirection      = 0;
    int         nOriginDistance = 0;
    int         nDestX          = 0;
    int         nDestY          = 0;

    nDirection = g_Random(DIRECTION_COUNT);

    nDistance = (int)g_Random(nDistance) + 1;

    if (nDistance < (nRange / 4))
    {
        nDistance = nRange / 4;
    }

    nDestX = pCharacter->m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
    nDestY = pCharacter->m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    nOriginDistance = g_GetDistance2(nDestX, nDestY, pCharacter->m_AIData.nOriginPosX, pCharacter->m_AIData.nOriginPosY);
    if (nOriginDistance > nRange * nRange)
    {
        nDirection = g_GetDirection(pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_AIData.nOriginPosX, pCharacter->m_AIData.nOriginPosY);
        nDestX = pCharacter->m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
        nDestY = pCharacter->m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;
    }

    pCharacter->WalkTo(nDestX, nDestY, true);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// 寻找四周的玩家
// 参数1: 搜索的范围半径
// 分支1: 成功搜索到的情况
// 分支2: 搜索失败的情况
int AISearchPlayer(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult             = 0;
    int                     nRetCode            = false;
    int                     nBranchFound        = 1;
    int                     nBranchNotFound     = 2;
    int                     nRange              = pActionData->m_nParam[0];
    BOOL                    bSearchByLevel      = pActionData->m_nParam[1];
    BOOL                    bSearchByTargetLife = pActionData->m_nParam[2];
    KSearchForAnyCharacter  Tactic;

    Tactic.m_pSelf                      = pCharacter;
    Tactic.m_nDistance2                 = nRange * nRange;
    Tactic.m_nLifeConversionLevel       = g_pSO3World->m_Settings.m_ConstList.nAISearchByLifeConversionLevel;
    Tactic.m_nAngle                     = DIRECTION_COUNT;
    Tactic.m_nRelation                  = (int)sortAll;
    Tactic.m_bAdjustRangeByTargetLife   = bSearchByTargetLife;
    Tactic.m_bAdjustRangeByLevelDiff    = bSearchByLevel;
    Tactic.m_bAdjustByAttribute         = true;
    Tactic.m_bAdjustByVisible           = true;

    AISearchPlayer(Tactic);
    KG_PROCESS_ERROR_RET_CODE(Tactic.m_pResult, nBranchNotFound);

    KGLOG_PROCESS_ERROR(IS_PLAYER(Tactic.m_pResult->m_dwID));

    nRetCode = pCharacter->SelectTarget(ttPlayer, Tactic.m_pResult->m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = nBranchFound;
Exit0:
    return nResult;
}

// 寻找四周的玩家,无视任何效果,无3D视野障碍
// 参数1: 搜索的范围半径
// 分支1: 成功搜索到的情况
// 分支2: 搜索失败的情况
int AISearchPlayerWithoutAnyAffect(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                     nResult             = 0;
    int                     nRetCode            = false;
    int                     nBranchFound        = 1;
    int                     nBranchNotFound     = 2;
    int                     nRange              = pActionData->m_nParam[0];
    KSearchForAnyCharacter  Tactic;

    Tactic.m_pSelf                      = pCharacter;
    Tactic.m_nDistance2                 = nRange * nRange;
    Tactic.m_nLifeConversionLevel       = g_pSO3World->m_Settings.m_ConstList.nAISearchByLifeConversionLevel;
    Tactic.m_nAngle                     = DIRECTION_COUNT;
    Tactic.m_nRelation                  = (int)sortAll;
    Tactic.m_bAdjustRangeByTargetLife   = false;
    Tactic.m_bAdjustRangeByLevelDiff    = false;
    Tactic.m_bAdjustByAttribute         = false;
    Tactic.m_bAdjustByVisible           = false;

    AISearchPlayer(Tactic);
    KG_PROCESS_ERROR_RET_CODE(Tactic.m_pResult, nBranchNotFound);

    KGLOG_PROCESS_ERROR(IS_PLAYER(Tactic.m_pResult->m_dwID));

    nRetCode = pCharacter->SelectTarget(ttPlayer, Tactic.m_pResult->m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = nBranchFound;
Exit0:
    return nResult;
}

// Function:    选中事件发起者
// Param:       N/A
// Branch:      [1]Success [2]Failed
// Remark:      
int AISelectEventSrc(KCharacter* pCharacter, KAIAction* pActionData)
{
    int             nResult         = 0;
    bool            bRetCode        = false;
    int             nBranchSuccess  = 1;
    int             nBranchFailed   = 2;
    DWORD           dwEventSrc      = pCharacter->m_AIData.dwAIEventSrc;

    KG_PROCESS_ERROR_RET_CODE(dwEventSrc != ERROR_ID, nBranchFailed);

    if (IS_PLAYER(dwEventSrc))
    {
        bRetCode = pCharacter->SelectTarget(ttPlayer, dwEventSrc);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);
    }

    if (IS_NPC(dwEventSrc))
    {
        bRetCode = pCharacter->SelectTarget(ttNpc, dwEventSrc);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, nBranchFailed);
    }

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    NPC打开对话面板
// Param:       [1]AutoTurn
// Branch:      [1]Dialog [2]NotDialog
// Remark:      
int AINpcOpenDialogWindow(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult             = 0;
    int         nRetCode            = false;
    int         nBranchDialog       = 1;
    int         nBranchNotDialog    = 2;
    BOOL        bAutoTurn           = pActionData->m_nParam[0] != 0;
    KNpc*       pNpc                = NULL;
    KPlayer*    pPlayer             = NULL;
    static char szAutoDialog[MAX_NPC_DIALOG_LEN];


    nRetCode = IS_NPC(pCharacter->m_dwID);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, nBranchNotDialog);

    pNpc = (KNpc*)pCharacter;

    nRetCode = pCharacter->m_SelectTarget.GetTarget(&pPlayer);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, nBranchNotDialog);
    KG_PROCESS_ERROR_RET_CODE(pPlayer, nBranchNotDialog);

    // nRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pNpc->m_dwScriptID);
    nRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pNpc->m_dwScriptID, "OnDialogue");
    if (nRetCode)
    {
        int nTopIndex = 0;
        g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

        g_pSO3World->m_ScriptCenter.PushValueToStack(pNpc);
        g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);

        g_pSO3World->m_ScriptCenter.CallFunction(pNpc->m_dwScriptID, "OnDialogue", 0);

        g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
    }
    else
    {
        // 在没有脚本的对话状态下，服务器自动生成对话脚本
        nRetCode = pNpc->GetAutoDialogString(pPlayer, szAutoDialog, MAX_NPC_DIALOG_LEN);
        KGLOG_PROCESS_ERROR(nRetCode);

        g_PlayerServer.DoOpenWindow(0, ttNpc, pNpc->m_dwID, szAutoDialog, pPlayer->m_nConnIndex);
    }

    if (bAutoTurn)
    {
        int nDirection = g_GetDirection(pNpc->m_nX, pNpc->m_nY, pPlayer->m_nX, pPlayer->m_nY);
        pNpc->TurnTo(nDirection, true);
    }

    // 巡逻中开启对话则设置该人为队长,以便整组停下来
    if (pCharacter->m_AIData.nPatrolPathID && pCharacter->m_AIData.pPatrolPath)
    {
        assert(pCharacter->m_pScene);
        pCharacter->m_pScene->m_AIPatrolGroupManager.SetLeader(pCharacter->m_AIData.nPatrolPathID, pCharacter->m_dwID);
    }

    nResult = nBranchDialog;
Exit0:
    return nResult;
}

// Function:    NPC和选中的玩家说话
// Param:       [1]SentenceID
// Branch:      [1]Success
// Remark:      nSentenceID为说的话的ID
int AINpcSayToTarget(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult             = 0;
    int         nRetCode            = false;
    int         nBranchSuccess      = 1;
    int         nSentenceID         = pActionData->m_nParam[0];
    KNpc*       pNpc                = NULL;
    KPlayer*    pPlayer             = NULL;

    nRetCode = IS_NPC(pCharacter->m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    pNpc = (KNpc*)pCharacter;

    nRetCode = pCharacter->m_SelectTarget.GetTargetType();
    KGLOG_PROCESS_ERROR(nRetCode == ttPlayer);

    nRetCode = pCharacter->m_SelectTarget.GetTarget(&pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer);

    pNpc->SayToTargetUseSentenceID(pPlayer, (DWORD)nSentenceID);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}


// Function:    判断对象血量相对于总血量的百分比
// Param:       [1] 期望的血量的总血量百分比
// Branch:      [1] 当前血量小于期望血量
//              [2] 当前血量等于期望血量
//              [3] 当前血量大于期望血量
int AICheckLife(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult             = 0;
    int         nBranchLower        = 1;
    int         nBranchEqual        = 2;
    int         nBranchGreater      = 3;
    int         nDesignatedLifeCof  = pActionData->m_nParam[0];
    int         nDesignatedLife     = pCharacter->m_nMaxLife * nDesignatedLifeCof / 100;

    if (pCharacter->m_nCurrentLife < nDesignatedLife)
    {
        nResult = nBranchLower;
        goto Exit0;
    }

    if (pCharacter->m_nCurrentLife > nDesignatedLife)
    {
        nResult = nBranchGreater;
        goto Exit0;
    }

    if (pCharacter->m_nCurrentLife == nDesignatedLife)
    {
        nResult = nBranchEqual;
        goto Exit0;
    }

Exit0:
    return nResult;
}

// Function:    NPC初始化技能CD
// Param:       N/A
// Branch:      [1]Success
// Remark:      
int AINpcInitSkillCD(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult             = 0;
    int         nRetCode            = false;
    int         nBranchSuccess      = 1;
    KNpc*       pNpc                = NULL;

    nRetCode = IS_NPC(pCharacter->m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    pNpc = (KNpc*)pCharacter;
    assert(pNpc->m_pTemplate);

    for (int i = 0; i < MAX_NPC_AI_SKILL; i++)
    {
        DWORD   dwSkillID       = pNpc->m_pTemplate->dwSkillIDList[i];
        DWORD   dwSkillLevel    = pNpc->m_pTemplate->dwSkillLevelList[i];
        int     nSkillCD        = pNpc->m_pTemplate->nSkillCastInterval[i];

        if (dwSkillID == 0 || dwSkillLevel == 0)
            continue;

        switch(pNpc->m_pTemplate->nAISkillType[i])
        {
        case aistPeriodSelf:
        case aistChannelingBreak:
        case aistTargetCount:
        case aistTargetCountNotClear:
        case aistSelfHP:
        case aistMainThreatNThreat:
        case aistBaseThreatNThreat:
        case aistDamageThreatNThreat:
        case aistTherapyThreatNThreat:
        case aistSpecialThreatNThreat:
        case aistBaseRandomTarget:
        case aistBaseRandomNotSelectTarget:
        case aistInRangeRandomTarget:
        case aistInRangeRandomNotSelectTarget:
        case aistOutRangeRandomTarget:
        case aistOutRangeRandomNotSelectTarget:
        case aistNearestTarget:
        case aistNearestNotSelectTarget:
            pNpc->m_nSkillCastFrame[i] = 0; // A类CD
            break;

        case aistPeriodTarget:
        case aistPeriodTargetNotClear:
        case aistPeriodTargetRange:
        case aistPeriodTargetRangeNotClear:
            // C类CD，技能的CD初始化为正常CD的10%到100%之间,保证随机性
            pNpc->m_nSkillCastFrame[i] = g_pSO3World->m_nGameLoop + nSkillCD / 10 + g_Random(nSkillCD / 10 * 9);
            break;

        case aistPeriodFriend:
        case aistPassiveRespond:
            break;

        default:
            KGLOG_PROCESS_ERROR(false);
            break;
        }
    }

    nResult = nBranchSuccess;
Exit0:
    return nResult;

}

// Function:    NPC清除当前选中的技能CD
// Param:       N/A
// Branch:      [1]Success
// Remark:      重转公共CD,清除技能CD
int AINpcClearSelectSkillCD(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nBranchSuccess  = 1;
    int     nSelectIndex    = 0;
    KNpc*   pNpc            = NULL;
    DWORD   dwSkillID       = 0;
    DWORD   dwSkillLevel    = 0;

    KGLOG_PROCESS_ERROR(IS_NPC(pCharacter->m_dwID));

    pNpc = (KNpc*)pCharacter;
    assert(pNpc->m_pTemplate);

    nSelectIndex = pNpc->m_nSkillSelectIndex;
    KG_PROCESS_SUCCESS(nSelectIndex < 0 || nSelectIndex >= MAX_NPC_AI_SKILL);

    dwSkillID       = pNpc->m_pTemplate->dwSkillIDList[nSelectIndex];
    dwSkillLevel    = pNpc->m_pTemplate->dwSkillLevelList[nSelectIndex];

    KGLOG_PROCESS_ERROR(dwSkillID);
    KGLOG_PROCESS_ERROR(dwSkillLevel);

    pNpc->m_nSkillCastFrame[nSelectIndex] -= pNpc->m_pTemplate->nSkillCastInterval[nSelectIndex];
    pNpc->m_nSkillCommomCD += g_pSO3World->m_Settings.m_ConstList.nNpcSkillCommonCD;

Exit1:
    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    NPC重置当前选中的技能CD
// Param:       N/A
// Branch:      [1]Success
// Remark:      重转技能CD和公共CD
int AINpcResetSelectSkillCD(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nBranchSuccess  = 1;
    int     nSelectIndex    = 0;
    KNpc*   pNpc            = NULL;
    DWORD   dwSkillID       = 0;
    DWORD   dwSkillLevel    = 0;

    KGLOG_PROCESS_ERROR(IS_NPC(pCharacter->m_dwID));

    pNpc = (KNpc*)pCharacter;
    assert(pNpc->m_pTemplate);

    nSelectIndex = pNpc->m_nSkillSelectIndex;
    KG_PROCESS_SUCCESS(nSelectIndex < 0 || nSelectIndex >= MAX_NPC_AI_SKILL);

    dwSkillID       = pNpc->m_pTemplate->dwSkillIDList[nSelectIndex];
    dwSkillLevel    = pNpc->m_pTemplate->dwSkillLevelList[nSelectIndex];

    KGLOG_PROCESS_ERROR(dwSkillID);
    KGLOG_PROCESS_ERROR(dwSkillLevel);

    pNpc->m_nSkillCastFrame[nSelectIndex] = g_pSO3World->m_nGameLoop + pNpc->m_pTemplate->nSkillCastInterval[nSelectIndex];
    pNpc->m_nSkillCommomCD = g_pSO3World->m_nGameLoop + g_pSO3World->m_Settings.m_ConstList.nNpcSkillCommonCD;

Exit1:
    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    NPC标准技能选择器
// Param:       N/A
// Branch:      [1]Success [2]Failed
// Remark:      NPC的技能从NpcTemplate表指定。
//              当选择器选择了一个可以立刻释放的技能时，走Success分支；当没有技能可以放时，走Failed分支。
int AINpcStandardSkillSelector(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                 nResult             = 0;
    int                 nBranchSuccess      = 1;
    int                 nBranchFailed       = 2;
    KNpc*               pNpc                = NULL;
    int                 nSelectedSkillIndex = -1;
    int                 nTotalReadyRateSum  = 0;
    KTarget             Targets[MAX_NPC_AI_SKILL];
    std::vector<int>    ReadySkills;

    KGLOG_PROCESS_ERROR(IS_NPC(pCharacter->m_dwID));

    pNpc = (KNpc*)pCharacter;
    assert(pNpc->m_pTemplate);

    ReadySkills.reserve(MAX_NPC_AI_SKILL);

    KG_PROCESS_ERROR_RET_CODE(g_pSO3World->m_nGameLoop > pNpc->m_nSkillCommomCD, nBranchFailed);

    for (int i = MAX_NPC_AI_SKILL - 1; i >= 0; i--)
    {
        DWORD   dwSelectSkillID     = pNpc->m_pTemplate->dwSkillIDList[i];
        DWORD   dwSelectSkillLevel  = pNpc->m_pTemplate->dwSkillLevelList[i];
        int     nSkillRate          = pNpc->m_pTemplate->nSkillRate[i];
        int     nSkillType          = pNpc->m_pTemplate->nAISkillType[i];
        KSkill* pSkill              = NULL;
        int     nSkillSelectResult  = 0;
        KTarget TempTarget          = pNpc->m_SelectTarget;

        if (
            dwSelectSkillID == 0 || dwSelectSkillLevel == 0 || 
            (g_pSO3World->m_nGameLoop <= pNpc->m_nSkillCastFrame[i] && nSkillType != aistPeriodTargetRange)
        )
            continue;

        pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW_S(dwSelectSkillID, dwSelectSkillLevel);
        if (!pSkill)
        {
            KGLogPrintf(
                KGLOG_ERR, 
                "AINpcStandardSkillSelector skill is not exist : skillID = %d, skillLevel = %d, selectid = %d.", 
                dwSelectSkillID, dwSelectSkillLevel, i
            );
            goto Exit0;
        }

        switch (nSkillType)
        {
        case aistPeriodTarget:
        case aistPeriodTargetNotClear:
            nSkillSelectResult = StandardTargetPeriodEvaluator(pNpc, &TempTarget);
            break;
        case aistPeriodTargetRange:
        case aistPeriodTargetRangeNotClear:
            nSkillSelectResult = StandardTargetPeriodRangeEvaluator(pNpc, &TempTarget);
            break;
        case aistPeriodSelf:
            nSkillSelectResult = StandardSelfPeriodEvaluator(pNpc, &TempTarget);
            break;
        case aistPeriodFriend:
            //nSkillSelectResult = GetSkillExp_PeriodFriend(i, &Target);
            KGLOG_CHECK_ERROR(false && "aistPeriodFriend");
            break;
        case aistPassiveRespond:
            nSkillSelectResult = StandardPassiveRespond(pNpc, &TempTarget);
            break;
        case aistChannelingBreak:
            nSkillSelectResult = StandardChannelingBreak(pNpc, &TempTarget, nSkillRate);
            break;
        case aistTargetCount:
        case aistTargetCountNotClear:
            nSkillSelectResult = StandardTargetCountEvaluator(pNpc, &TempTarget, nSkillRate);
            break;
        case aistSelfHP:
            nSkillSelectResult = StandardSelfHPEvaluator(pNpc, &TempTarget, nSkillRate);
            break;
        case aistMainThreatNThreat:
            nSkillSelectResult = StandardNThreat(pNpc, &TempTarget, thtMainThreat, nSkillRate);
            break;
        case aistBaseThreatNThreat:
            nSkillSelectResult = StandardNThreat(pNpc, &TempTarget, thtBaseThreat, nSkillRate);
            break;
        case aistDamageThreatNThreat:
            nSkillSelectResult = StandardNThreat(pNpc, &TempTarget, thtDamageThreat, nSkillRate);
            break;
        case aistTherapyThreatNThreat:
            nSkillSelectResult = StandardNThreat(pNpc, &TempTarget, thtTherapyThreat, nSkillRate);
            break;
        case aistSpecialThreatNThreat:
            nSkillSelectResult = StandardNThreat(pNpc, &TempTarget, thtSpecialThreat, nSkillRate);
            break;
        case aistBaseRandomTarget:
            nSkillSelectResult = StandardRandomTargetByBase(pNpc, &TempTarget, false);
            break;
        case aistBaseRandomNotSelectTarget:
            nSkillSelectResult = StandardRandomTargetByBase(pNpc, &TempTarget, true);
            break;
        case aistInRangeRandomTarget:
            nSkillSelectResult = StandardRandomTargetByRange(pNpc, &TempTarget, 0, nSkillRate, false);
            break;
        case aistInRangeRandomNotSelectTarget:
            nSkillSelectResult = StandardRandomTargetByRange(pNpc, &TempTarget, 0, nSkillRate, true);
            break;
        case aistOutRangeRandomTarget:
            nSkillSelectResult = StandardRandomTargetByRange(pNpc, &TempTarget, nSkillRate, 8192, false);
            break;
        case aistOutRangeRandomNotSelectTarget:
            nSkillSelectResult = StandardRandomTargetByRange(pNpc, &TempTarget, nSkillRate, 8192, true);
            break;
        case aistNearestTarget:
            nSkillSelectResult = StandardRandomTargetByNearest(pNpc, &TempTarget, false);            
            break;
        case aistNearestNotSelectTarget:
            nSkillSelectResult = StandardRandomTargetByNearest(pNpc, &TempTarget, true);
            break;
        default:
            KGLOG_PROCESS_ERROR(false);
            break;
        }

        Targets[i] = TempTarget;

        if (nSkillSelectResult == SKILL_SELECTOR_READY)
        {
            ReadySkills.push_back(i);
            nTotalReadyRateSum += nSkillRate;
        }

        if (nSkillSelectResult == SKILL_SELECTOR_CAST)
        {
            nSelectedSkillIndex = i;
            break;
        }

        if (nSkillSelectResult == SKILL_SELECTOR_CAST_SPECIAL)
        {
            pCharacter->m_AIData.bSpecialSelectTarget = true;
            nSelectedSkillIndex = i;
            break;
        }
    }

    if (nSelectedSkillIndex < 0)
    {
        int nSize = (int)ReadySkills.size();
        int nRate = g_Random(nTotalReadyRateSum);

        for (int i = 0; i < nSize; i++)
        {
            int nSkillRate = pNpc->m_pTemplate->nSkillRate[ReadySkills[i]];

            if (nRate < nSkillRate)
            {
                nSelectedSkillIndex = ReadySkills[i];
                break;
            }

            nRate -= nSkillRate;
        }
    }

    KG_PROCESS_ERROR_RET_CODE(nSelectedSkillIndex >= 0, nBranchFailed);
    KG_PROCESS_ERROR_RET_CODE(
        g_pSO3World->m_nGameLoop > pNpc->m_nSkillCastFrame[nSelectedSkillIndex] || 
        pNpc->m_pTemplate->nAISkillType[nSelectedSkillIndex] != aistPeriodTargetRange, 
        nBranchFailed
    );

    pNpc->m_nSkillSelectIndex                           = nSelectedSkillIndex;
    pNpc->m_AIData.nSkillType                           = pNpc->m_pTemplate->nAISkillType[nSelectedSkillIndex];
    pNpc->m_AIData.nRecordSkillID[airstSelectSkill]     = pNpc->m_pTemplate->dwSkillIDList[nSelectedSkillIndex];
    pNpc->m_AIData.nRecordSkillLevel[airstSelectSkill]  = pNpc->m_pTemplate->dwSkillLevelList[nSelectedSkillIndex];
    pNpc->m_AIData.ChoiceTarget[aittLastSelectTarget]   = pNpc->m_SelectTarget;
    pNpc->m_SelectTarget                                = Targets[nSelectedSkillIndex];

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

// Function:    做一个动作
// Param:       [1] AnimationID
// Branch:      [1] Success
int AIDoAnimation(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    int         nRetCode        = false;
    int         nBranchSuccess  = 1;
    int         nAnimationID    = pActionData->m_nParam[0];
    KCharacter* pTarget         = NULL;
    DWORD       dwTargetID      = ERROR_ID;

    nRetCode = pCharacter->m_SelectTarget.GetTarget(&pTarget);
    if (nRetCode)
    {
        assert(pTarget);
        dwTargetID = pTarget->m_dwID;
    }

    g_PlayerServer.DoCharacterAction(pCharacter, dwTargetID, nAnimationID);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

int AIGroupRevive(KCharacter* pCharacter, KAIAction* pActionData)
{
    int         nResult         = 0;
    int         nRetCode        = false;
    int         nBranchSuccess  = 1;
    KNpc*       pNpc            = NULL;

    nRetCode = IS_NPC(pCharacter->m_dwID);
    KGLOG_PROCESS_ERROR(nRetCode);

    pNpc = (KNpc*)pCharacter;
    KGLOG_PROCESS_ERROR(pNpc->m_pScene);
    
    if (pNpc->m_dwReliveID > 0)
    {
        nRetCode = pNpc->m_pScene->m_NpcReviveManager.ForceGroupNpcRevive(pNpc->m_dwReliveID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }    

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

int AICleanBuffByFunction(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nRetCode        = false;
    int     nBranchSuccess  = 1;
    int     nFunctionType   = pActionData->m_nParam[0];

    KGLOG_PROCESS_ERROR(nFunctionType > sftInvalid && nFunctionType < sftTotal);

    pCharacter->m_BuffList.DelMultiBuffByFunctionType(nFunctionType, INT_MAX);

    nResult = nBranchSuccess;
Exit0:
    return nResult;
}

int AINeedEscape(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult             = 0;
    int     nRetCode            = false;
    int     nBranchNotEscape    = 1;
    int     nBranchEscape       = 2;
    int     nEscapeRange        = pActionData->m_nParam[0]; 
    int     nEscapeLifePercent  = pActionData->m_nParam[1];

    if (nEscapeRange > 0 && pCharacter->m_AIData.nEscapeCount < 1)
    {
        int nEscapeLife = pCharacter->m_nMaxLife * nEscapeLifePercent / 100;
        
        if (pCharacter->m_nCurrentLife < nEscapeLife)
        {
            pCharacter->m_AIData.nEscapeCount++;
            nResult = nBranchEscape;
            goto Exit0;
        }
    }

    nResult = nBranchNotEscape;
Exit0:
    return nResult;
}

// Function:    参数比较
// Param:       [1] Value [2] Compare
// Branch:      [1] Larger [2] Equal [3] Smaller
int AIParamCompare(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nBranchLarger   = 1;
    int     nBranchEqual    = 2;
    int     nBranchSmaller  = 3;
    int     nValue          = pActionData->m_nParam[0];
    int     nCompare        = pActionData->m_nParam[1];

    if (nValue > nCompare)
    {
        nResult = nBranchLarger;
        goto Exit0;
    }

    if (nValue < nCompare)
    {
        nResult = nBranchSmaller;
        goto Exit0;
    }

    nResult = nBranchEqual;
Exit0:
    return nResult;
}

int AISelectState(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nRetCode        = false;
    int     nBranchPatrol   = 1;
    int     nBranchWander   = 2;
    int     nBranchIdle     = 3;
    int     nWanderRange    = pActionData->m_nParam[0];

    KG_PROCESS_ERROR_RET_CODE(pCharacter->m_AIData.nPatrolPathID <= 0, nBranchPatrol);
    KG_PROCESS_ERROR_RET_CODE(nWanderRange <= 0, nBranchWander);

    nResult = nBranchIdle;
Exit0:
    return nResult;
}

int AIRandomRun(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult         = 0;
    int     nRetCode        = false;
    int     nBrachSuccess   = 1;
    int     nRange          = pActionData->m_nParam[0];
    int     nDistance       = 0;
    int     nDestX          = 0;
    int     nDestY          = 0;
    int     nDirection      = 0;

    nDirection = g_Random(DIRECTION_COUNT);

    pCharacter->TurnTo(nDirection, true);

    nDistance = g_Random(nRange);

    nDestX = pCharacter->m_nX + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
    nDestY = pCharacter->m_nY + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    pCharacter->RunTo(nDestX, nDestY, true);

    nResult = nBrachSuccess;
Exit0:
    return nResult;
}

int AICleanDebuff(KCharacter* pCharacter, KAIAction* pActionData)
{
    int nResult         = 0;
    int nBrachSuccess   = 1;

    pCharacter->m_BuffList.CleanBuffByCanCancel(false);

    nResult = nBrachSuccess;
Exit0:
    return nResult;
}

// 只用于人数限制小于30的地图
int AISearchMultiPlayerAddThreatList(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                                     nResult             = 0;
    int                                     nRetCode            = false;
    int                                     nBranchFound        = 1;
    int                                     nBranchNotFound     = 2;
    int                                     nRange              = pActionData->m_nParam[0];
    BOOL                                    bSearchByLevel      = pActionData->m_nParam[1];
    BOOL                                    bSearchByTargetLife = pActionData->m_nParam[2];
    KSearchForMultiCharacterAddThreatList   Tactic;

    assert(pCharacter->m_pScene);

    KMapParams* pMapParam = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(pCharacter->m_pScene->m_dwMapID);
    KGLOG_PROCESS_ERROR(pMapParam);

    KGLOG_PROCESS_ERROR(pMapParam->nMaxPlayerCount <= 30);

    Tactic.m_pSelf                      = pCharacter;
    Tactic.m_nDistance2                 = nRange * nRange;
    Tactic.m_nLifeConversionLevel       = g_pSO3World->m_Settings.m_ConstList.nAISearchByLifeConversionLevel;
    Tactic.m_nAngle                     = DIRECTION_COUNT;
    Tactic.m_nRelation                  = (int)sortEnemy;
    Tactic.m_nCharacterCounter          = 30;
    Tactic.m_bAdjustRangeByTargetLife   = bSearchByTargetLife;
    Tactic.m_bAdjustRangeByLevelDiff    = bSearchByLevel;
    Tactic.m_bAdjustByAttribute         = true;
    Tactic.m_bAdjustByVisible           = false;

    AISearchPlayer(Tactic);
    KG_PROCESS_ERROR_RET_CODE(Tactic.m_pResult, nBranchNotFound);

    nResult = nBranchFound;
Exit0:
    return nResult;
}

// 守卫用于搜索非法战斗的玩家并加入仇恨表,取前30个按照距离排序
struct _BustupCharacter
{
    _BustupCharacter() : m_nDistance2(INT_MAX), m_pSelf(NULL){}
    int m_nDistance2;
    KCharacter* m_pSelf;
};

BOOL _RangeComp(const _BustupCharacter crLeft, const _BustupCharacter crRight)
{
    return crLeft.m_nDistance2 < crRight.m_nDistance2;
}

int AISearchBustupPlayerAddThreatList(KCharacter* pCharacter, KAIAction* pActionData)
{
    int                                                 nResult             = 0;
    int                                                 nRetCode            = false;
    int                                                 nBranchFound        = 1;
    int                                                 nBranchNotFound     = 2;
    int                                                 nRange              = pActionData->m_nParam[0];
    BOOL                                                bSearchByLevel      = pActionData->m_nParam[1];
    BOOL                                                bSearchByTargetLife = pActionData->m_nParam[2];
    KSearchBustupCharacterList                          Tactic;
    KSearchBustupCharacterList::KResultSet::iterator    itSetBegin;
    KSearchBustupCharacterList::KResultSet::iterator    itSetEnd;
    static std::vector<_BustupCharacter>                BustupVecter;
    std::vector<_BustupCharacter>::iterator             itVecterBegin;
    std::vector<_BustupCharacter>::iterator             itVecterEnd;

    Tactic.m_pSelf                      = pCharacter;
    Tactic.m_nDistance2                 = nRange * nRange;
    Tactic.m_nLifeConversionLevel       = g_pSO3World->m_Settings.m_ConstList.nAISearchByLifeConversionLevel;
    Tactic.m_nAngle                     = DIRECTION_COUNT;
    Tactic.m_nRelation                  = (int)sortAll;
    Tactic.m_nCharacterCounter          = 30;
    Tactic.m_bAdjustRangeByTargetLife   = bSearchByTargetLife;
    Tactic.m_bAdjustRangeByLevelDiff    = bSearchByLevel;
    Tactic.m_bAdjustByAttribute         = true;
    Tactic.m_bAdjustByVisible           = true;

    AISearchPlayer(Tactic);
    KG_PROCESS_ERROR_RET_CODE(!Tactic.m_Result.empty(), nBranchNotFound);

    // 距离排序
    BustupVecter.clear();
    itSetBegin = Tactic.m_Result.begin();
    itSetEnd   = Tactic.m_Result.end();
    for (KSearchBustupCharacterList::KResultSet::iterator itSet = itSetBegin; itSet != itSetEnd; ++itSet)
    {
        _BustupCharacter BustupNode;

        BustupNode.m_pSelf      = *itSet;
        BustupNode.m_nDistance2 = g_GetDistance2(pCharacter->m_nX, pCharacter->m_nY, BustupNode.m_pSelf->m_nX, BustupNode.m_pSelf->m_nY);

        BustupVecter.push_back(BustupNode);
    }
    sort(BustupVecter.begin(), BustupVecter.end(), _RangeComp);

    // 加仇恨
    itVecterBegin = BustupVecter.begin();
    itVecterEnd   = BustupVecter.end();
    for (std::vector<_BustupCharacter>::iterator itVecter = itVecterBegin; itVecter != itVecterEnd; ++itVecter)
    {
        pCharacter->m_SimpThreatList.ModifyThreat(thtSpecialThreat, itVecter->m_pSelf, 0);

        Tactic.m_nCharacterCounter--;
        if (Tactic.m_nCharacterCounter <= 0)
            break;
    }

    nResult = nBranchFound;
Exit0:
    return nResult;
}

int AICheckSkillFailedCounter(KCharacter* pCharacter, KAIAction* pActionData)
{
    int     nResult             = 0;
    int     nRetCode            = false;
    int     nBranchPass         = 1;
    int     nBranchFailed       = 2;
    int     nCounter            = pActionData->m_nParam[0];

    if (pCharacter->m_AIData.nSkillFailedCount >= nCounter)
    {
        pCharacter->m_AIData.nSkillFailedCount = 0;
        nResult = nBranchFailed;
        goto Exit0;
    }

    nResult = nBranchPass;
Exit0:
    return nResult;
}

#endif

void KAIManager::RegisterActionFunctions()
{
    memset(m_ActionFunctionTable, 0, sizeof(m_ActionFunctionTable));

    REGISTER_AI_ACTION_FUNC(SetState);
    REGISTER_AI_ACTION_FUNC(SetPrimaryTimer);    
    REGISTER_AI_ACTION_FUNC(SetSecondaryTimer);
    REGISTER_AI_ACTION_FUNC(SetTertiaryTimer);
    REGISTER_AI_ACTION_FUNC(RandomBiBranch);
    REGISTER_AI_ACTION_FUNC(RandomTriBranch);
    REGISTER_AI_ACTION_FUNC(Stand);

#ifdef _SERVER
    REGISTER_AI_ACTION_FUNC(NpcSetSecTimerBySkillAni);
    REGISTER_AI_ACTION_FUNC(NpcSetSecTimerBySkillRest);    
    REGISTER_AI_ACTION_FUNC(SearchEnemy);
    REGISTER_AI_ACTION_FUNC(AddTargetToThreatList);
    REGISTER_AI_ACTION_FUNC(SelectFirstThreat);
    REGISTER_AI_ACTION_FUNC(IsInFight);
    REGISTER_AI_ACTION_FUNC(KeepRange);
    REGISTER_AI_ACTION_FUNC(CastSkill);
    REGISTER_AI_ACTION_FUNC(SetSystemShield);
    REGISTER_AI_ACTION_FUNC(RecordReturnPosition);
    REGISTER_AI_ACTION_FUNC(Return);
    REGISTER_AI_ACTION_FUNC(EnterPatrolPath);
    REGISTER_AI_ACTION_FUNC(LeavePatrolPath);
    REGISTER_AI_ACTION_FUNC(Patrol);
    REGISTER_AI_ACTION_FUNC(RecordOriginPosition);
    REGISTER_AI_ACTION_FUNC(KeepOriginDirection);
    REGISTER_AI_ACTION_FUNC(FaceTarget);
    REGISTER_AI_ACTION_FUNC(Wander);
    REGISTER_AI_ACTION_FUNC(SearchPlayer);
    REGISTER_AI_ACTION_FUNC(SelectEventSrc);
    REGISTER_AI_ACTION_FUNC(NpcOpenDialogWindow);
    REGISTER_AI_ACTION_FUNC(NpcSayToTarget);
    REGISTER_AI_ACTION_FUNC(CheckLife);
    REGISTER_AI_ACTION_FUNC(NpcInitSkillCD);
    REGISTER_AI_ACTION_FUNC(NpcClearSelectSkillCD);
    REGISTER_AI_ACTION_FUNC(NpcResetSelectSkillCD);
    REGISTER_AI_ACTION_FUNC(NpcStandardSkillSelector);
    REGISTER_AI_ACTION_FUNC(NpcKeepSkillCastRange);
    REGISTER_AI_ACTION_FUNC(NpcCastSelectSkill);
    REGISTER_AI_ACTION_FUNC(DoAnimation);
    REGISTER_AI_ACTION_FUNC(GroupRevive);
    REGISTER_AI_ACTION_FUNC(IsTargetInRange);
    REGISTER_AI_ACTION_FUNC(IsTargetExist);
    REGISTER_AI_ACTION_FUNC(IsTargeInWater);
    REGISTER_AI_ACTION_FUNC(IsNeedKeepRange);
    REGISTER_AI_ACTION_FUNC(CleanBuffByFunction);
    REGISTER_AI_ACTION_FUNC(NeedEscape);
    REGISTER_AI_ACTION_FUNC(ParamCompare);
    REGISTER_AI_ACTION_FUNC(SelectState);
    REGISTER_AI_ACTION_FUNC(RandomRun);
    REGISTER_AI_ACTION_FUNC(CleanDebuff);
    REGISTER_AI_ACTION_FUNC(SearchMultiPlayerAddThreatList);
    REGISTER_AI_ACTION_FUNC(CheckSkillFailedCounter);
    REGISTER_AI_ACTION_FUNC(SearchPlayerWithoutAnyAffect);
    REGISTER_AI_ACTION_FUNC(ClearThreatForTarget);
    REGISTER_AI_ACTION_FUNC(SelectNthThreat);
    REGISTER_AI_ACTION_FUNC(FilterThreat);
    REGISTER_AI_ACTION_FUNC(ChangeTarget);
    REGISTER_AI_ACTION_FUNC(SetTarget);
    REGISTER_AI_ACTION_FUNC(ClearTotalDamageValue);
    REGISTER_AI_ACTION_FUNC(TotalDamageValueCompare);
    REGISTER_AI_ACTION_FUNC(SearchBustupPlayerAddThreatList);
    REGISTER_AI_ACTION_FUNC(CopyThreatListToTargetGroup);
    REGISTER_AI_ACTION_FUNC(ExtractionTargetGroup);
    REGISTER_AI_ACTION_FUNC(ReturnPositionDistanceCompare);
#endif
}

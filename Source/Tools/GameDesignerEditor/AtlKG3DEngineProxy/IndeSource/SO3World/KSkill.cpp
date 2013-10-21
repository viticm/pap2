#include "stdafx.h"
#include <algorithm>
#include "KSkill.h"
#include "KRegion.h"
#include "KScene.h"
#include "KMath.h"
#include "KSO3World.h"
#include "KItemLib.h"
#include "KAISearchTactics.h"
#include "KPlayerServer.h"
#include "KPlayerClient.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "KPlayer.h"
#include "Engine/KMemory.h"

#define NPC_FIX_SKILL_RANGE         (GAME_FPS * 20) //NPC修正技能释放距离,相当于4秒之内玩家跑步的距离
#define MAX_SKILL_CAST_STACK_DEPTH  8
#define PLAYER_FIX_SKILL_RANGE      (POINT_PER_ALTITUDE * 3)

// 为了与 AI 配合，技能的距离判断是用圆柱体的方式，而不是球体。
BOOL SkillInRange(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ, int nRange)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;

    int         nDistanceZ      = 0;
    long long   llDistanceXY    = 0;

    assert(nSrcX >= 0);
    assert(nSrcY >= 0);

    assert(nDstX >= 0);
    assert(nDstY >= 0);

    assert(nRange >= 0);

    llDistanceXY = g_GetDistance2(
        (long long)nSrcX, (long long)nSrcY, 
        (long long)nDstX, (long long)nDstY
    );
    KG_PROCESS_ERROR(llDistanceXY <= (long long)nRange * nRange);

    nDistanceZ = abs(nSrcZ - nDstZ);
    KG_PROCESS_ERROR(nDistanceZ <= nRange);

    bResult = true;
Exit0:
    return bResult;
}

IN_RANGE_RESULT SkillInRange(int nSrcX, int nSrcY, int nSrcZ, int nDstX, int nDstY, int nDstZ, int nMinRange, int nMaxRange)
{
    IN_RANGE_RESULT nResult         = irrInvalid;
    BOOL            bRetCode        = false;

    int             nDistanceZ      = 0;
    long long       llDistanceXY    = 0;

    assert(nSrcX >= 0);
    assert(nSrcY >= 0);

    assert(nDstX >= 0);
    assert(nDstY >= 0);

    assert(nMinRange >= 0);
    assert(nMaxRange >= 0);
    assert(nMinRange <= nMaxRange);

    llDistanceXY = g_GetDistance2(
        (long long)nSrcX, (long long)nSrcY, 
        (long long)nDstX, (long long)nDstY
    );

    nDistanceZ = abs(nSrcZ - nDstZ);

    KG_PROCESS_ERROR_RET_CODE(llDistanceXY <= (long long)nMaxRange * nMaxRange, irrTooFar); // 不在最大范围就失败
    KG_PROCESS_ERROR_RET_CODE(nDistanceZ <= nMaxRange, irrTooFar);

    KG_PROCESS_ERROR_RET_CODE(llDistanceXY >= (long long)nMinRange * nMinRange || nDistanceZ >= nMinRange, irrTooClose);

    nResult = irrInRange;
Exit0:
    return nResult;
}


bool operator<(const KSKILL_RECIPE_KEY& lhs, const KSKILL_RECIPE_KEY& rhs)
{
    return (memcmp(&lhs, &rhs, sizeof(KSKILL_RECIPE_KEY)) < 0);
}

bool operator==(const KSKILL_RECIPE_KEY& lhs, const KSKILL_RECIPE_KEY& rhs)
{
    return (memcmp(&lhs, &rhs, sizeof(KSKILL_RECIPE_KEY)) == 0);
}

KSkill* KSkillRecipePointer::SetRecipeKey(const KSKILL_RECIPE_KEY& crRecipeKey)
{
    KSkill* pResult = NULL;

    m_SkillRecipeKey = crRecipeKey;

    m_pSkill = g_pSO3World->m_SkillManager.GetSkill(m_SkillRecipeKey);
    KG_PROCESS_ERROR(m_pSkill);

    m_dwSkillSN = m_pSkill->m_dwSerialNumber;

    pResult = m_pSkill;
Exit0:
    return pResult;
}

KSKILL_RECIPE_KEY& KSkillRecipePointer::GetRecipeKey()
{
    return m_SkillRecipeKey;
}

void KSkillRecipePointer::ClearRecipeKey()
{
    m_pSkill    = NULL;
    m_dwSkillSN = 0;

    memset(&m_SkillRecipeKey, 0, sizeof(m_SkillRecipeKey));
}

KSkill* KSkillRecipePointer::GetPointer()
{
    KSkill* pResult = NULL;

    KG_PROCESS_ERROR(m_pSkill);

    if (m_dwSkillSN != m_pSkill->m_dwSerialNumber)
    {
        m_pSkill = g_pSO3World->m_SkillManager.GetSkill(m_SkillRecipeKey);
        KG_PROCESS_ERROR(m_pSkill);

        m_dwSkillSN = m_pSkill->m_dwSerialNumber;
    }

    pResult = m_pSkill;
Exit0:
    return pResult;
}

KSkill::KSkill(void)
{
	m_pBaseInfo                         = NULL;
    m_dwLevel                           = 0;
    m_dwLevelUpExp                      = 0;
    m_nPlayerLevelLimit                 = 0;
    m_nExpAddOdds                       = KILO_NUM;

    m_nCostLife                         = 0;	
    m_nCostMana                         = 0;	
    m_nCostStamina                      = 0;	

    m_nPrepareFrames                    = 0;
    
	m_nThreatRevisePercent				= KILO_NUM;
    
    m_nMinRadius                        = 0;
    m_nMaxRadius                        = 0;
    m_nAreaRadius                       = 0;

    m_nAngleRange                       = 0;

    m_nWeaponDamagePercent              = KILO_NUM;

    m_nBulletVelocity                   = 0;

    m_nBreakRate                        = KILO_NUM;
    m_nBrokenRate                       = KILO_NUM;

    m_nBeatBackRate                     = KILO_NUM;

    m_nChannelFrame                     = 0;
    m_nChannelInterval                  = 0;

    m_nTargetCountLimit                 = -1;

    m_nAttackWeakPos                    = swpNone;
    
    m_nDamageAddPercent                 = 0;

    m_bIsFormationSkill                 = false;
    m_nFormationRange                   = 0;
    m_nLeastFormationPopulation         = 0;

    m_bIsAccumulate                     = false;

    m_nChainDepth                       = 0;
    m_nChainBranch                      = 0;

    m_nDismountingRate                  = 0;
    m_nBaseThreat                       = 0;

    m_nDamageToLifeForParty             = 0;
    m_nDamageToManaForParty             = 0;

    m_nTargetLifePercentMax             = 100;
    m_nTargetLifePercentMin             = 0;

    m_nSelfLifePercentMax               = 100;
    m_nSelfLifePercentMin               = 0;

    m_pAttrbuteEffectToSelfAndRollback  = NULL;	
    m_pAttrbuteEffectToSelfNotRollback  = NULL;	
    m_pAttrbuteEffectToDestAndRollback  = NULL;	
    m_pAttrbuteEffectToDestNotRollback  = NULL;
    m_pAttrbuteEffectToDestOnMinorWeak  = NULL;
    m_pAttrbuteEffectToDestOnMajorWeak  = NULL;
    m_pAttrbuteEffectToDestOnMaxWeak    = NULL;
    
    for (int i = 0; i < sizeof(m_BindBuffs) / sizeof(m_BindBuffs[0]); i++)
    {
        m_BindBuffs[i].ClearRecipeKey();
    }

    m_dwPublicCoolDownID = 0;

    memset(m_dwCoolDownID, 0, sizeof(m_dwCoolDownID));

    memset(m_nCoolDownAlter, 0 ,sizeof(m_nCoolDownAlter));

    memset(m_SubsectionSkill, 0, sizeof(m_SubsectionSkill));
}

KSkill::~KSkill(void)
{

}

BOOL KSkill::Init(KSKILL_BASE_INFO *pBaseInfo, DWORD dwLevel)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nLuaTopIndex    = 0;
//  Lua_State*      L               = NULL;

    assert(m_pBaseInfo == NULL);
    m_pBaseInfo = pBaseInfo;
    m_dwLevel   = dwLevel;

    bRetCode = CallInitializeScript();
    KGLOG_PROCESS_ERROR(bRetCode);

//Exit1:
    m_dwSerialNumber = g_pSO3World->m_SkillManager.m_dwSkillSNGenerator++;
    bResult = true;
Exit0:
    if (!bResult && m_pBaseInfo)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "[Skill] Init skill failed: skill id = %u, name = \"%s\", Level = %u, script = \"%s\"\n", 
            m_pBaseInfo->dwSkillID,
            m_pBaseInfo->szSkillName,
            m_dwLevel,
            m_pBaseInfo->szScriptFile
        );
        // clean all attribute list !
        FREE_ATTRIB_LIST(m_pAttrbuteEffectToSelfAndRollback);
        FREE_ATTRIB_LIST(m_pAttrbuteEffectToSelfNotRollback);
        FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestAndRollback);
        FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestNotRollback);
        FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestOnMinorWeak);
        FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestOnMajorWeak);
        FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestOnMaxWeak);

        m_SelfRequireBuffVector.clear();
        m_SelfRequireBuffVector.clear();
    }
    return bResult;
}

void KSkill::UnInit()
{
    m_dwSerialNumber = 0;

    FREE_ATTRIB_LIST(m_pAttrbuteEffectToSelfAndRollback);
    FREE_ATTRIB_LIST(m_pAttrbuteEffectToSelfNotRollback);
    FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestAndRollback);
    FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestNotRollback);
    FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestOnMinorWeak);
    FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestOnMajorWeak);
    FREE_ATTRIB_LIST(m_pAttrbuteEffectToDestOnMaxWeak);

    for (int i = 0; i < sizeof(m_BindBuffs) / sizeof(m_BindBuffs[0]); i++)
    {
        m_BindBuffs[i].ClearRecipeKey();
    }

    m_SelfRequireBuffVector.clear();
    m_DestRequireBuffVector.clear();
}

BOOL KSkill::CallInitializeScript()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nLuaTopIndex    = 0;

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_pBaseInfo->szScriptFile);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "[Skill] Init skill, get script \"%s\" failed, skill id = %u, name = \"%s\"\n", 
            m_pBaseInfo->szScriptFile,
            m_pBaseInfo->dwSkillID,
            m_pBaseInfo->szSkillName
        );
        goto Exit0;
    }

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);

	bRetCode = g_pSO3World->m_ScriptCenter.CallFunction(m_pBaseInfo->szScriptFile, "GetSkillLevelData", 1);
	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
    KGLOG_CHECK_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkill::CallLevelUpScript(KPlayer* pPlayer)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nLuaTopIndex    = 0;

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_pBaseInfo->szScriptFile);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "[Skill] Init skill, get script \"%s\" failed, skill id = %u, name = \"%s\"\n", 
            m_pBaseInfo->szScriptFile,
            m_pBaseInfo->dwSkillID,
            m_pBaseInfo->szSkillName
        );
        goto Exit0;
    }

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(this);
	g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);

	bRetCode = g_pSO3World->m_ScriptCenter.CallFunction(m_pBaseInfo->szScriptFile, "OnSkillLevelUp", 0);
	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

SKILL_RESULT_CODE KSkill::CanCast(KCharacter* pSrcCharacter, KTarget &rTarget)
{
    int               nResult   = srcFailed;
    int               nRetCode  = srcFailed;

    KG_ASSERT_EXIT(pSrcCharacter);

    nRetCode = CheckSelfState(pSrcCharacter);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    assert(m_pBaseInfo->nKindType > sktInvalid && m_pBaseInfo->nKindType < sktTotal);
    KG_PROCESS_ERROR_RET_CODE(pSrcCharacter->m_nSilenceRefCounter[m_pBaseInfo->nKindType - 1] == 0, srcOnSilence);

    if (IS_PLAYER(pSrcCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pSrcCharacter;
        nRetCode = CheckWeaponRequest(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = CheckMountRequest(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = CheckCoolDown(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    }
    //else: Npc无需检查

    nRetCode = CheckCost(pSrcCharacter);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    nRetCode = CheckSelfBuff(pSrcCharacter);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    // --------------------------------------------

    switch (m_pBaseInfo->nCastMode)
    {
    case scmSector:
        // 无需对目标做任何检查,这种技能"目标"即是自己,但是这时的目标是虚的,也不做任何检查
        // 技能具体作用到扇形区域内的具体对象是才检查时候可以对这个对象作用
        nRetCode = rTarget.SetTarget(pSrcCharacter);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case scmCasterArea:
        // 以自己为中心的圆形区域,自然,无需检查目标
        nRetCode = rTarget.SetTarget(pSrcCharacter);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case scmItem:
        // 对物品释放技能，目前没什么需要判断
        break;

    case scmCasterSingle:
        // 对自己施放的单体技能
        {
        // 无论怎么样,目标总是自己
        nRetCode = rTarget.SetTarget(pSrcCharacter);
        KG_PROCESS_ERROR(nRetCode);
        // 检查目标类型是否允许施放这个技能
        nRetCode = CheckTargetType(pSrcCharacter, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        // 检查目标对象的状态是否允许施放这个技能
        nRetCode = CheckTargetState(rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        if (m_pBaseInfo->bCheckBindBuff)
        {
            // 检查技能带的BUFF能不能加到目标身上
            nRetCode = CheckTargetBuffList(pSrcCharacter, rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        }
        // 检查目标身上的BUFF是否符合技能要求
        nRetCode = CheckTargetBuff(rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        }
        break;
    case scmTargetSingle:
    case scmTargetArea:
    case scmTargetChain:
        // 检查目标类型是否允许施放这个技能
        nRetCode = CheckTargetType(pSrcCharacter, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        // 检查目标对象的状态是否允许施放这个技能
        nRetCode = CheckTargetState(rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        // 检查目标是否在扇形范围之类
        nRetCode = CheckTargetRange(pSrcCharacter, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        if (m_pBaseInfo->bCheckBindBuff)
        {
            // 检查技能带的BUFF能不能加到目标身上
            nRetCode = CheckTargetBuffList(pSrcCharacter, rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        }
        // 检查目标身上的BUFF是否符合技能要求
        nRetCode = CheckTargetBuff(rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        break;

    case scmPointArea:
    case scmPoint:
        // 检查目标(可能是个对象,也可能是点)是否在扇形范围内
        nRetCode = CheckTargetRange(pSrcCharacter, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        break;

    case scmTargetLeader:
        if (IS_PLAYER(pSrcCharacter->m_dwID))
        {
            KPlayer*        pCaster     = (KPlayer*)pSrcCharacter;
            KPlayer*        pLeader     = NULL;
            int             nGroupIndex = pCaster->GetCurrentTeamGroupIndex();

#ifdef _SERVER
            KTeam*          pTeam   = NULL;

            pTeam = g_pSO3World->m_TeamServer.GetTeam(pCaster->m_dwTeamID);
            KG_PROCESS_ERROR_RET_CODE(pTeam, srcNotParty);

            pLeader = g_pSO3World->m_PlayerSet.GetObj(
                pTeam->MemberGroup[nGroupIndex].dwFormationLeader
            );
            KGLOG_PROCESS_ERROR_RET_CODE(pLeader, srcNotParty);
#else
            pLeader = g_pSO3World->m_PlayerSet.GetObj(
                g_pSO3World->m_TeamClient.m_TeamGroup[nGroupIndex].dwFormationLeader
            );
            KG_PROCESS_ERROR(pLeader);
#endif
            nRetCode = rTarget.SetTarget(pLeader);
            KGLOG_PROCESS_ERROR(nRetCode);

            KG_PROCESS_ERROR(pLeader->m_pScene != NULL);

            nRetCode = CheckTargetType(pCaster, rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

            nRetCode = CheckTargetState(rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

            nRetCode = CheckTargetRange(pCaster, rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

            if (m_pBaseInfo->bCheckBindBuff)
            {
                nRetCode = CheckTargetBuffList(pCaster, rTarget);
                KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
            }

            nRetCode = CheckTargetBuff(rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        }
        break;

    case scmPartyArea:
        nRetCode = rTarget.SetTarget(pSrcCharacter);
        KG_PROCESS_ERROR(nRetCode);
        break;

    default:
        nResult = srcInvalidSkill;
        goto Exit0;
    }


//Exit1:
	nResult = srcSuccess;
Exit0:
    if (m_pBaseInfo->bUseCastScript)
    {
        // 施放判断的脚本只有两个一个参数,那就是施放者对象和程序判断的结果,没有必要搞得太复杂
        nResult = CallCastScript(pSrcCharacter, (SKILL_RESULT_CODE)nResult);
    }
	return (SKILL_RESULT_CODE)nResult;
}

SKILL_RESULT_CODE KSkill::CanCastOnPrepare(KCharacter* pSrcCharacter, KTarget &rTarget)
{
    int               nResult   = srcFailed;
    int               nRetCode  = srcFailed;

    KG_ASSERT_EXIT(pSrcCharacter);

    if (IS_PLAYER(pSrcCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pSrcCharacter;
        nRetCode = CheckWeaponRequest(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

        nRetCode = CheckMountRequest(pPlayer);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
    }
    //else: Npc无需检查

    nRetCode = CheckSelfState(pSrcCharacter);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    nRetCode = CheckCost(pSrcCharacter);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    nRetCode = CheckSelfBuff(pSrcCharacter);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

    // --------------------------------------------

    switch (m_pBaseInfo->nCastMode)
    {
    case scmSector:
        // 无需对目标做任何检查,这种技能"目标"即是自己,但是这时的目标是虚的,也不做任何检查
        // 技能具体作用到扇形区域内的具体对象是才检查时候可以对这个对象作用
        nRetCode = rTarget.SetTarget(pSrcCharacter);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case scmCasterArea:
        // 以自己为中心的圆形区域,自然,无需检查目标
        nRetCode = rTarget.SetTarget(pSrcCharacter);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case scmItem:
        // 对物品释放技能，目前没什么需要判断
        break;
    case scmCasterSingle:
        // 对自己施放的单体技能
        {
        // 检查目标类型是否允许施放这个技能
        nRetCode = CheckTargetType(pSrcCharacter, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        // 检查目标对象的状态是否允许施放这个技能
        nRetCode = CheckTargetState(rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        if (m_pBaseInfo->bCheckBindBuff)
        {
            // 检查技能上带的BUFF能不能加到目标身上
            nRetCode = CheckTargetBuffList(pSrcCharacter, rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        }
        nRetCode = CheckTargetBuff(rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        }
        break;
    case scmTargetSingle:
    case scmTargetArea:
    case scmTargetChain:
        // 检查目标是否在扇形范围之类
        nRetCode = CheckTargetRange(pSrcCharacter, rTarget, PLAYER_FIX_SKILL_RANGE, NPC_FIX_SKILL_RANGE * 4);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        // 检查目标类型是否允许施放这个技能
        nRetCode = CheckTargetType(pSrcCharacter, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        // 检查目标对象的状态是否允许施放这个技能
        nRetCode = CheckTargetState(rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        if (m_pBaseInfo->bCheckBindBuff)
        {
            // 检查技能上带的BUFF能不能加到目标身上
            nRetCode = CheckTargetBuffList(pSrcCharacter, rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        }
        nRetCode = CheckTargetBuff(rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        break;
    case scmPointArea:
    case scmPoint:
        // 检查目标(可能是个对象,也可能是点)是否在扇形范围内
        nRetCode = CheckTargetRange(pSrcCharacter, rTarget);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        break;

    case scmTargetLeader:
        if (IS_PLAYER(pSrcCharacter->m_dwID))
        {
            KPlayer*        pCaster     = (KPlayer*)pSrcCharacter;
            KPlayer*        pLeader     = NULL;
            int             nGroupIndex = pCaster->GetCurrentTeamGroupIndex();

#ifdef _SERVER
            KTeam*          pTeam   = NULL;
            pTeam = g_pSO3World->m_TeamServer.GetTeam(pCaster->m_dwTeamID);
            KG_PROCESS_ERROR_RET_CODE(pTeam, srcNotParty);

            pLeader = g_pSO3World->m_PlayerSet.GetObj(
                pTeam->MemberGroup[nGroupIndex].dwFormationLeader
            );
            KGLOG_PROCESS_ERROR_RET_CODE(pLeader, srcNotParty);
#else
            pLeader = g_pSO3World->m_PlayerSet.GetObj(
                g_pSO3World->m_TeamClient.m_TeamGroup[nGroupIndex].dwFormationLeader
            );
            KG_PROCESS_ERROR(pLeader);
#endif
            nRetCode = rTarget.SetTarget(pLeader);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = CheckTargetType(pCaster, rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

            nRetCode = CheckTargetState(rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

            nRetCode = CheckTargetRange(pCaster, rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

            if (m_pBaseInfo->bCheckBindBuff)
            {
                nRetCode = CheckTargetBuffList(pCaster, rTarget);
                KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
            }

            nRetCode = CheckTargetBuff(rTarget);
            KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
        }
        break;

    case scmPartyArea:
        nRetCode = rTarget.SetTarget(pSrcCharacter);
        KG_PROCESS_ERROR(nRetCode);
        break;

    default:
        nResult = srcInvalidSkill;
        goto Exit0;
    }


//Exit1:
	nResult = srcSuccess;
Exit0:
    if (m_pBaseInfo->bUseCastScript)
    {
        // 施放判断的脚本只有两个一个参数,那就是施放者对象和程序判断的结果,没有必要搞得太复杂
        nResult = CallCastScript(pSrcCharacter, (SKILL_RESULT_CODE)nResult);
    }
	return (SKILL_RESULT_CODE)nResult;
}


#if defined(_SERVER)

SKILL_RESULT_CODE KSkill::Cast(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey)
{
	int         nResult         = srcFailed;
	int         nRetCode        = srcFailed;
    KCharacter* pTarget         = NULL;
    char        szTarget[64];

    assert(pDisplayCaster);
    assert(pLogicCaster);

    pLogicCaster->m_nSkillStackDepth++;
    if (pLogicCaster->m_nSkillStackDepth > MAX_SKILL_CAST_STACK_DEPTH)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "[Skill] %s(%u) cast skill(%u, %u) stack over !\n",
            pLogicCaster->m_szName, pLogicCaster->m_dwID, m_pBaseInfo->dwSkillID, m_dwLevel
        );
        goto Exit0;
    }

    if (IS_PLAYER(pLogicCaster->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pLogicCaster;
        KGPQ_STATISTICS_INCREMENT   StatisticsIncrement(&g_pSO3World->m_PQManager, pPlayer->m_dwID, psiSkillMark, m_pBaseInfo->nSkillMark);

        pPlayer->m_PQList.Traverse(StatisticsIncrement);
    }

    switch (rTarget.GetTargetType())
    {
    case ttPlayer:
    case ttNpc:
        nRetCode = rTarget.GetTarget(&pTarget);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;
    default:
        break;
    }

    szTarget[0] = '\0';
    if (pTarget)
    {
        snprintf(szTarget, sizeof(szTarget), " To(%s-%lu)", pTarget->m_szName, pTarget->m_dwID);
        szTarget[sizeof(szTarget) - 1] = '\0';
    }

    pLogicCaster->m_SkillDebug.Trace(
        1, "============================================================\n"
        "[技能调试][%d]:[技能流程] SkillCast:(%s-%u,%u) From(%s-%u)%s",
        g_pSO3World->m_nGameLoop,
        m_pBaseInfo->szSkillName, m_pBaseInfo->dwSkillID, m_dwLevel,
        pLogicCaster->m_szName, pLogicCaster->m_dwID,
        szTarget
    );

    switch (m_pBaseInfo->nCastMode)
    {
    case scmSector:
        nRetCode = CastOnSector(pDisplayCaster, pLogicCaster, crRecipeKey);
        break;

    case scmCasterArea:
        nRetCode = CastOnCasterArea(pDisplayCaster, pLogicCaster, crRecipeKey);
        break;

    case scmTargetArea:
        nRetCode = CastOnTargetArea(pDisplayCaster, pLogicCaster, rTarget, crRecipeKey);
        break;

    case scmPointArea:
        nRetCode = CastOnPointArea(pDisplayCaster, pLogicCaster, rTarget, crRecipeKey);
        break;

    case scmCasterSingle:
        nRetCode = CastOnCasterSingle(pDisplayCaster, pLogicCaster, crRecipeKey);
        break;

    case scmTargetSingle:
        nRetCode = CastOnTargetSingle(pDisplayCaster, pLogicCaster, rTarget, crRecipeKey);
        break;

    case scmTargetChain:
        nRetCode = CastOnTargetChain(pDisplayCaster, pLogicCaster, rTarget, crRecipeKey);
        break;

    case scmPoint:
        nRetCode = CastOnPoint(pDisplayCaster, pLogicCaster, rTarget, crRecipeKey);
        break;

    case scmTargetLeader:
        nRetCode = CastOnTargetLeader(pDisplayCaster, pLogicCaster, crRecipeKey);
        break;

    case scmPartyArea:
        nRetCode = CastOnPartyArea(pDisplayCaster, pLogicCaster, crRecipeKey);
        break;

    default:
        KG_ASSERT_EXIT(false);
    }

    pLogicCaster->m_SkillDebug.Trace(
        1, "[技能调试][%d]:[技能流程] (%s-%u) SkillCast (%s-%u,%u) %s 结果:%s 原因:%d",
        g_pSO3World->m_nGameLoop, pLogicCaster->m_szName, pLogicCaster->m_dwID,
        m_pBaseInfo->szSkillName, m_pBaseInfo->dwSkillID, m_dwLevel,
        szTarget, nRetCode ? "成功" : "失败", nRetCode
    );

    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);


    if (IS_PLAYER(pLogicCaster->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pLogicCaster;
        if (pTarget && pTarget->m_nLevel + g_pSO3World->m_Settings.m_ConstList.nNoAddSkillExp >= pPlayer->m_nLevel)
        {
            pPlayer->m_SkillList.AddSkillExp(m_pBaseInfo->dwSkillID, 1);
        }

        nRetCode = pPlayer->ProcessCommonSkillActive(crRecipeKey);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = srcSuccess;
Exit0:
    pLogicCaster->m_nSkillStackDepth--;
    return (SKILL_RESULT_CODE)nResult;
}

#else

SKILL_RESULT_CODE KSkill::Cast(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey)
{        
    return srcSuccess;
}

#endif


BOOL KSkill::IsAutoTurn()
{
    return m_pBaseInfo->bIsAutoTurn;
}

SKILL_RESULT_CODE KSkill::CheckWeaponRequest(KPlayer* pCaster)
{
    SKILL_RESULT_CODE nResult           = srcWeaponError;
    int               nRetCode          = false;
    KItem*            pWeapon           = NULL;
    int               nWeaponDetailType = 0; // 注意,0表示"任意武器,包括没有武器"
    int               nWeaponGenre      = 0;


    KG_ASSERT_EXIT(pCaster);

    // 0表示任意武器都可以,所以就不用查了
    KG_PROCESS_SUCCESS(m_pBaseInfo->dwWeaponRequest == 0);

    nWeaponGenre = g_pSO3World->m_Settings.m_WeaponTypeList.GetWeaponGenre(m_pBaseInfo->dwWeaponRequest);

    switch (nWeaponGenre)
    {
    case swrtMeleeWeapon:
        pWeapon = pCaster->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
        break;

    case swrtRangeWeapon:
        pWeapon = pCaster->m_ItemList.GetItem(ibEquip, eitRangeWeapon);
        break;

    default:
        KGLogPrintf(
            KGLOG_ERR, "[Skill] Skill(%u, %s) error: invalid weapon request ID (%u).\n",
            m_pBaseInfo->dwSkillID, m_pBaseInfo->szSkillName, m_pBaseInfo->dwWeaponRequest
        );
        goto Exit0;
    }

    if (pWeapon)
    {
        nRetCode = pWeapon->GetGenre();     
        KG_ASSERT_EXIT(nRetCode == igEquipment);
        nWeaponDetailType = pWeapon->m_Common.nDetail;
        KG_PROCESS_ERROR_RET_CODE(pWeapon->GetCurrentDurability() != 0, srcWeaponDestroy);
    }
    else
    {
        // 空手算是近程
        KG_PROCESS_ERROR(nWeaponGenre == swrtMeleeWeapon);
        // 空手当拳套
        nWeaponDetailType = wdtFist;                        
    }

    nRetCode = g_pSO3World->m_Settings.m_WeaponTypeList.CheckType(
        m_pBaseInfo->dwWeaponRequest, (WEAPON_DETAIL_TYPE)nWeaponDetailType
    );
    KG_PROCESS_ERROR(nRetCode);
    
Exit1:
    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckMountRequest(KPlayer* pCaster)
{
    SKILL_RESULT_CODE nResult       = srcMountError;
    KSkill*           pMountKungfu  = NULL;

    KG_PROCESS_SUCCESS(m_pBaseInfo->dwMountRequestType == 0 && m_pBaseInfo->dwMountRequestDetail == INVALID_SKILL_ID); // 不需要内功，不做判断了

    pMountKungfu = g_pSO3World->m_SkillManager.GetSkill_K(
        pCaster->m_SkillList.m_dwMountKungfuID, pCaster->m_SkillList.m_dwMountKungfuLevel
    );
    KG_PROCESS_ERROR_RET_CODE(pMountKungfu, srcMountError); // 没装备内功！那失败了

    if (m_pBaseInfo->dwMountRequestDetail == INVALID_SKILL_ID)
    {
        KG_PROCESS_ERROR_RET_CODE(m_pBaseInfo->dwMountRequestType == pMountKungfu->m_pBaseInfo->dwMountType, srcMountError); // 不需要具体内功，那就判断类型啦

        goto Exit1;
    }

    KG_PROCESS_ERROR_RET_CODE(m_pBaseInfo->dwMountRequestDetail == pMountKungfu->m_pBaseInfo->dwSkillID, srcMountError); // ID不同就要错了
    KG_PROCESS_ERROR_RET_CODE(m_pBaseInfo->dwMountRequestDetailLevel <= pMountKungfu->m_dwLevel, srcMountError);    // 判断等级是否满足需求的最低等级

Exit1:
    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckCoolDown(KPlayer *pSrcPlayer)
{
	SKILL_RESULT_CODE nResult  = srcFailed;
    BOOL              bRetCode = false;

    KG_ASSERT_EXIT(pSrcPlayer);

    if (m_dwPublicCoolDownID)
    {
        bRetCode = pSrcPlayer->m_TimerList.CheckTimer(m_dwPublicCoolDownID);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, srcSkillNotReady);
    }

    for (int i = 0; i < MAX_SKILL_COOL_DOWN_TIMER; ++i)
    {
        DWORD dwCoolDownID = m_dwCoolDownID[i];

        if (dwCoolDownID == 0)
            continue;

        bRetCode = pSrcPlayer->m_TimerList.CheckTimer(dwCoolDownID);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, srcSkillNotReady);        
    }

	nResult = srcSuccess;
Exit0:
	return nResult;
}

BOOL KSkill::ResetPublicCooldown(KPlayer* pCaster)
{
    BOOL    bResult         = false;
    int     nDuration       = 0;

#ifdef _SERVER
    assert(pCaster);

    KG_PROCESS_SUCCESS(m_dwPublicCoolDownID == 0);

    nDuration = pCaster->GetCDValue(m_dwPublicCoolDownID);

    nDuration = max(0, nDuration);

    KG_PROCESS_SUCCESS(nDuration == 0);

    pCaster->m_TimerList.ResetTimer(m_dwPublicCoolDownID, nDuration);
#endif

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkill::ClearPublicCooldown(KPlayer* pCaster)
{
    BOOL    bResult         = false;

#ifdef _SERVER
    assert(pCaster);

    KG_PROCESS_SUCCESS(m_dwPublicCoolDownID == 0);

    pCaster->m_TimerList.ClearTimer(m_dwPublicCoolDownID);
#endif

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkill::ResetNormalCooldown(KPlayer* pCaster)
{
    BOOL bResult = false;

#ifdef _SERVER
    KG_ASSERT_EXIT(pCaster);

    for (int i = 0; i < MAX_SKILL_COOL_DOWN_TIMER; ++i)
    {
        DWORD dwCoolDownID = m_dwCoolDownID[i];
        int   nDuration    = 0;

        if (dwCoolDownID == 0)
            continue;
        
        nDuration = pCaster->GetCDValue(dwCoolDownID);

        nDuration += m_nCoolDownAlter[i];
        nDuration = max(0, nDuration);

        if (nDuration == 0)
            continue;

        pCaster->m_TimerList.ResetTimer(dwCoolDownID, nDuration);
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}


SKILL_RESULT_CODE KSkill::CheckCost(KCharacter *pCaster)
{
    SKILL_RESULT_CODE nResult   = srcFailed;
    KPlayer*          pPlayer   = NULL;
    int               nCostLife = 0;
    int               nCostMana = 0;

    KG_ASSERT_EXIT(pCaster);

    nCostLife = m_nCostLife;
    nCostMana = m_nCostMana * pCaster->m_nModifyCostManaPercent / KILO_NUM;
    nCostMana = max(nCostMana, 0);

    KG_PROCESS_ERROR_RET_CODE(pCaster->m_nCurrentLife     > nCostLife,      srcNotEnoughLife);
    KG_PROCESS_ERROR_RET_CODE(pCaster->m_nCurrentMana     >= nCostMana,     srcNotEnoughMana);

    // 技能释放者是玩家, 才做下面的检查
    KG_PROCESS_SUCCESS(IS_NPC(pCaster->m_dwID));

    pPlayer = (KPlayer*)pCaster;
    KG_PROCESS_ERROR_RET_CODE(pPlayer->m_nCurrentStamina >= m_nCostStamina, srcNotEnoughStamina);

    // 技能消耗弹药检查
    if (m_pBaseInfo->bCostAmmo)
    {
        KItem* pWeapon = pPlayer->m_ItemList.GetItem(ibEquip, eitRangeWeapon);
        KItem* pItem = pPlayer->m_ItemList.GetItem(ibEquip, eitArrow);
        KG_PROCESS_ERROR_RET_CODE(pWeapon, srcWeaponError);
        KG_PROCESS_ERROR_RET_CODE(pItem, srcNotEquitAmmo);
        KG_PROCESS_ERROR_RET_CODE(pWeapon->m_Common.nDetail == pItem->m_Common.nDetail, srcAmmoError);
        KG_PROCESS_ERROR_RET_CODE(pItem->GetStackNum() >= 1, srcNotEnoughAmmo);
    } 

    // 技能消耗包裹里的物品检查(技能对物品的消耗, 都只消耗一个; 这是程序与策划的约定)
    if (m_pBaseInfo->nCostItemType != ittInvalid)
    {
        int nItemAmount = pPlayer->m_ItemList.GetItemAmountInEquipAndPackage(m_pBaseInfo->nCostItemType, m_pBaseInfo->nCostItemIndex);
        KG_PROCESS_ERROR_RET_CODE(nItemAmount > 0, srcNotEnoughItem);
    }
	
Exit1:
    nResult = srcSuccess;
Exit0:
    return nResult;
}

#ifdef _SERVER
void KSkill::DoCost(KCharacter* pCaster)
{
    KPlayer* pPlayer    = NULL;
    int      nCostLife  = 0;
    int      nCostMana  = 0;

    nCostLife = m_nCostLife;
    nCostMana = m_nCostMana * pCaster->m_nModifyCostManaPercent / KILO_NUM;
    nCostMana = max(nCostMana, 0);

    pCaster->m_nCurrentLife     -= nCostLife;
    pCaster->m_nCurrentMana     -= nCostMana;

    KG_PROCESS_SUCCESS(IS_NPC(pCaster->m_dwID));

    pPlayer = (KPlayer*)pCaster;
    pPlayer->m_nCurrentStamina   -= m_nCostStamina;
    
    // 技能消耗弹药
    if (m_pBaseInfo->bCostAmmo)
    {
        pPlayer->m_ItemList.ConsumeAmmo(1);
    }
    
    // 技能消耗包裹里的物品(技能对物品的消耗, 都只消耗一个; 这是程序与策划的约定)
    if (m_pBaseInfo->nCostItemType != ittInvalid)
    {
        pPlayer->m_ItemList.CostItemInEquipAndPackage(m_pBaseInfo->nCostItemType, m_pBaseInfo->nCostItemIndex, 1);
    }
    
Exit1:
//Exit0:
    return;
}
#endif

SKILL_RESULT_CODE KSkill::CheckTargetRange(KCharacter* pCaster, KTarget& rTarget, int nFixPlayerCastSkillRange, int nFixNpcCastSkillRange)
{
    SKILL_RESULT_CODE nResult           = srcInvalidTarget;
    int               nRetCode          = false;
    int               nDistance         = 0;
    int               nMinRange         = 0;
    int               nMaxRange         = 0;
    int               nCasterX          = 0;
    int               nCasterY          = 0;
    int               nCasterZ          = 0;
    int               nCasterAltitude   = 0;
    int               nTargetX          = 0;
    int               nTargetY          = 0;
    int               nTargetZ          = 0;
    int               nTargetAltitude   = 0;
    KSceneObject*     pTarget           = NULL;
    TARGET_TYPE       eTargetType       = ttInvalid;
    int               nTouchRange       = 0;
    int               nSubDirection     = 0;

    // 注意: 我们的坐标都是>=0的,并且不会超过2的17次方,相关数值参见场景Region,格子的定义

    assert(pCaster);
    nRetCode = pCaster->GetAbsoluteCoordinate(&nCasterX, &nCasterY, &nCasterZ);
    KG_PROCESS_ERROR(nRetCode);

    nCasterAltitude = ZPOINT_TO_ALTITUDE(nCasterZ);
    nCasterZ = ZPOINT_TO_XYPOINT(nCasterZ);

    nRetCode = rTarget.GetTarget(&nTargetX, &nTargetY, &nTargetZ);
    KG_PROCESS_ERROR(nRetCode);

    nTargetAltitude = ZPOINT_TO_ALTITUDE(nTargetZ);
    nTargetZ = ZPOINT_TO_XYPOINT(nTargetZ);

    eTargetType = rTarget.GetTargetType();
    if (eTargetType == ttNpc || eTargetType == ttPlayer || eTargetType == ttDoodad)
    {
        nRetCode = rTarget.GetTarget(&pTarget);
        KG_PROCESS_ERROR(nRetCode && pTarget);
        nTouchRange = pTarget->m_nTouchRange;
    }
    
    if (nCasterX != nTargetX || nCasterY != nTargetY)
    {
        nSubDirection = g_GetDirection(nCasterX, nCasterY, nTargetX, nTargetY);
        assert(nSubDirection >= 0 && nSubDirection <= DIRECTION_COUNT);

        nSubDirection -= pCaster->m_nFaceDirection;
        if (nSubDirection > DIRECTION_COUNT / 2)
        {
            nSubDirection -= DIRECTION_COUNT;
        }
        if (nSubDirection < - DIRECTION_COUNT / 2)
        {
            nSubDirection += DIRECTION_COUNT;
        }
        KG_PROCESS_ERROR_RET_CODE(nSubDirection >= - m_nAngleRange / 2, srcOutOfAngle);
        KG_PROCESS_ERROR_RET_CODE(nSubDirection <=   m_nAngleRange / 2, srcOutOfAngle);
    }

	nMaxRange = pCaster->m_nTouchRange + m_nMaxRadius + nTouchRange;
	if ((nFixNpcCastSkillRange > 0) && IS_NPC(pCaster->m_dwID))
	{
		//为Npc的情况下修正技能最大范围
		nMaxRange += nFixNpcCastSkillRange;
	}

    if ((nFixPlayerCastSkillRange > 0) && IS_PLAYER(pCaster->m_dwID))
    {
        nMaxRange += nFixPlayerCastSkillRange;
    }

    nRetCode = SkillInRange(nCasterX, nCasterY, nCasterZ, nTargetX, nTargetY, nTargetZ, m_nMinRadius, nMaxRange);
    KG_PROCESS_ERROR_RET_CODE(nRetCode != irrTooClose, srcTooCloseTarget);
    KG_PROCESS_ERROR_RET_CODE(nRetCode != irrTooFar, srcTooFarTarget);
    KG_PROCESS_ERROR(nRetCode == irrInRange);

    if (m_pBaseInfo->bUse3DObstacle)
    {
        int nCasterHeadZ = nCasterAltitude + ZPOINT_TO_ALTITUDE(pCaster->m_nHeight);
        int nTargetHeadZ = nTargetAltitude;

        if (pTarget)
        {
            nTargetHeadZ += ZPOINT_TO_ALTITUDE(pTarget->m_nHeight);
        }

        KScene* pScene = pCaster->m_pScene;
        KGLOG_PROCESS_ERROR(pScene);
        nRetCode = pScene->IsVisible(nCasterX, nCasterY, nCasterHeadZ, nTargetX, nTargetY, nTargetHeadZ);
        KG_PROCESS_ERROR_RET_CODE(nRetCode, srcTargetInvisible);
    }

    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckSelfBuff(KCharacter* pCaster)
{
    SKILL_RESULT_CODE   nResult             = srcInvalidTarget;
    BOOL                bRetCode            = 0;
    int                 nCurrentStackNum    = 0;
    int                 nCurrentBuffLevel   = 0;
    
    assert(pCaster);

    for (KSKILL_REQUIRE_BUFF_VECTOR::iterator it = m_SelfRequireBuffVector.begin(), itEnd = m_SelfRequireBuffVector.end(); it != itEnd; ++it)
    {
        bRetCode = pCaster->m_BuffList.GetBuffMostStackNum(it->dwBuffID, &nCurrentBuffLevel, &nCurrentStackNum);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, srcFailed);

        switch (it->eStackCompareFlag)
        {
        case bcfEqual:
            bRetCode = (it->nStackNum == nCurrentStackNum);
            break;

        case bcfGreaterEqual:
            bRetCode = (it->nStackNum <= nCurrentStackNum);
            break;

        default:
            KGLogPrintf(KGLOG_ERR, "Invalid Compare Flag.\n");
            goto Exit0;
        }
        KG_PROCESS_ERROR_RET_CODE(bRetCode, srcBuffError);

        switch (it->eLevelCompareFlag)
        {
        case bcfEqual:
            bRetCode = (it->nBuffLevel == nCurrentBuffLevel);
            break;

        case bcfGreaterEqual:
            bRetCode = (it->nBuffLevel <= nCurrentBuffLevel);
            break;

        default:
            KGLogPrintf(KGLOG_ERR, "Invalid Compare Flag.\n");
            goto Exit0;
        }
        KG_PROCESS_ERROR_RET_CODE(bRetCode, srcBuffError);
    }
    
    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckTargetBuff(KTarget& rTarget)
{
    SKILL_RESULT_CODE   nResult             = srcInvalidTarget;
    BOOL                bRetCode            = 0;
    KCharacter*         pDstCharacter       = NULL;
    int                 nCurrentStackNum    = 0;
    int                 nCurrentBuffLevel   = 0;

    bRetCode = rTarget.GetTarget(&pDstCharacter);
    KG_PROCESS_ERROR(bRetCode);

    assert(pDstCharacter);

    for (KSKILL_REQUIRE_BUFF_VECTOR::iterator it = m_DestRequireBuffVector.begin(), itEnd = m_DestRequireBuffVector.end(); it != itEnd; ++it)
    {
        bRetCode = pDstCharacter->m_BuffList.GetBuffMostStackNum(it->dwBuffID, &nCurrentBuffLevel, &nCurrentStackNum);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, srcFailed);

        switch (it->eStackCompareFlag)
        {
        case bcfEqual:
            bRetCode = (it->nStackNum == nCurrentStackNum);
            break;

        case bcfGreaterEqual:
            bRetCode = (it->nStackNum <= nCurrentStackNum);
            break;

        default:
            KGLogPrintf(KGLOG_ERR, "Invalid Compare Flag.\n");
            goto Exit0;
        }
        KG_PROCESS_ERROR_RET_CODE(bRetCode, srcBuffError);

        switch (it->eLevelCompareFlag)
        {
        case bcfEqual:
            bRetCode = (it->nBuffLevel == nCurrentBuffLevel);
            break;

        case bcfGreaterEqual:
            bRetCode = (it->nBuffLevel <= nCurrentBuffLevel);
            break;

        default:
            KGLogPrintf(KGLOG_ERR, "Invalid Compare Flag.\n");
            goto Exit0;
        }
        KG_PROCESS_ERROR_RET_CODE(bRetCode, srcBuffError);
    }

    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckTargetType(KCharacter *pCaster, KTarget &rTarget)
{
    SKILL_RESULT_CODE nResult           = srcInvalidTarget;
    BOOL              bRetCode          = 0;
    int               nRelation         = 0;
    TARGET_TYPE       eTargetType       = ttInvalid;
    KCharacter*       pTargetCharacter  = NULL;
    // 注意，在客户端，如果找不到目标对象，可能并不是什么大的错误，所以不要断言!
    KG_ASSERT_EXIT(pCaster);

    eTargetType = rTarget.GetTargetType();

    KG_PROCESS_ERROR_RET_CODE(eTargetType != ttNoTarget, srcNoTarget);

    bRetCode = rTarget.GetTarget(&pTargetCharacter);
    KG_PROCESS_ERROR(bRetCode && pTargetCharacter);

    KGLOG_PROCESS_ERROR(pTargetCharacter->m_pScene);

    KG_PROCESS_ERROR_RET_CODE(
        pTargetCharacter->m_eKind != ckCannotOperateNpc && 
        pTargetCharacter->m_eKind != ckClientNpc,
        srcInvalidTarget
    );

    KG_PROCESS_ERROR_RET_CODE(
        (eTargetType == ttPlayer && m_pBaseInfo->bTargetTypePlayer) ||
        (eTargetType == ttNpc && m_pBaseInfo->bTargetTypeNpc),
        srcInvalidTarget
    );

    nRelation = g_GetRelation(pCaster, pTargetCharacter);
    KG_PROCESS_ERROR(nRelation & m_pBaseInfo->nTargetRelationMask);

    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckFormationRequire(KCharacter *pCaster)
{
    SKILL_RESULT_CODE           nResult         = srcFailed;
    int                         nRetCode        = 0;
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckSelfState(KCharacter *pCaster)
{
    SKILL_RESULT_CODE nResult             = srcFailed;
    int               nRetCode            = 0;
    int               nCurrentLifePercent = 0;

    KG_ASSERT_EXIT(pCaster);

    nRetCode = (1 << pCaster->m_eMoveState);
    KG_PROCESS_ERROR_RET_CODE(nRetCode & m_pBaseInfo->nSelfMoveStateMask, srcMoveStateError);

    KGLOG_PROCESS_ERROR_RET_CODE(pCaster->m_pScene, srcMapBan);
    KG_PROCESS_ERROR_RET_CODE(!(pCaster->m_pScene->m_dwBanSkillMask & m_pBaseInfo->dwMapBanMask), srcMapBan);

    if (m_bIsAccumulate)
    {
        KG_PROCESS_ERROR_RET_CODE(pCaster->m_nAccumulateValue > 0, srcNotStartAccumulate);
    }

    if (m_pBaseInfo->bNeedOutOfFight)
    {
        KG_PROCESS_ERROR_RET_CODE(!pCaster->m_bFightState, srcNotInFight);
    }

    KG_PROCESS_ERROR_RET_CODE(
        ((pCaster->m_nFearCounter > 0 && m_pBaseInfo->bSelfOnFear) || (!pCaster->m_nFearCounter)), 
        srcMoveStateError
    );

    KGLOG_PROCESS_ERROR(pCaster->m_nMaxLife && "MaxLife Equal Zero");
    nCurrentLifePercent = pCaster->m_nCurrentLife * 100 / pCaster->m_nMaxLife;

    MAKE_IN_RANGE(nCurrentLifePercent, 0, 100);

    KG_PROCESS_ERROR_RET_CODE(nCurrentLifePercent <= m_nSelfLifePercentMax, srcSelfLifeError);
    KG_PROCESS_ERROR_RET_CODE(nCurrentLifePercent >= m_nSelfLifePercentMin, srcSelfLifeError);

    if (IS_PLAYER(pCaster->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pCaster;

        // 骑马判断
        switch (m_pBaseInfo->nSelfHorseStateRequest)
        {
        case shcAny:
            break;
        case shcHorse:
            KG_PROCESS_ERROR_RET_CODE(pPlayer->m_bOnHorse, srcErrorByHorse);
            break;
        case shcNotHorse:
            KG_PROCESS_ERROR_RET_CODE(!pPlayer->m_bOnHorse, srcErrorByHorse);
            break;
        default:
            KGLogPrintf(
                KGLOG_ERR, "[Skill] Unexpected skill self horse state request %d !\n", 
                m_pBaseInfo->nSelfHorseStateRequest
            );
            goto Exit0;
        }
    }

    if (m_pBaseInfo->dwSkillCastFlag)
    {
        nRetCode = (m_pBaseInfo->dwSkillCastFlag & pCaster->m_dwSkillCastFlag);
        KG_PROCESS_ERROR(nRetCode);
    }

    if (m_bIsFormationSkill)
    {
        SKILL_RESULT_CODE eRetCode = srcFailed;
        eRetCode = CheckFormationRequire(pCaster);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == srcSuccess, eRetCode);
    }

    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckTargetState(KTarget &rTarget)
{
    SKILL_RESULT_CODE nResult               = srcFailed;
    int               nRetCode              = false;
    int               nCurrentLifePercent   = 0;
    KCharacter       *pTargetCharacter      = NULL;

    nRetCode = rTarget.GetTarget(&pTargetCharacter);
    KG_PROCESS_ERROR(nRetCode && pTargetCharacter);

    nRetCode = (1 << pTargetCharacter->m_eMoveState);
    KG_PROCESS_ERROR_RET_CODE(nRetCode & m_pBaseInfo->nTargetMoveStateMask, srcDstMoveStateError);

    KG_PROCESS_ERROR_RET_CODE(
        ((pTargetCharacter->m_nFearCounter > 0 && m_pBaseInfo->bTargetOnFear) || (!pTargetCharacter->m_nFearCounter)), 
        srcMoveStateError
    );

    KGLOG_PROCESS_ERROR(pTargetCharacter->m_nMaxLife && "MaxLife Equal Zero");
    nCurrentLifePercent = pTargetCharacter->m_nCurrentLife * 100 / pTargetCharacter->m_nMaxLife;

    MAKE_IN_RANGE(nCurrentLifePercent, 0, 100);

    KG_PROCESS_ERROR_RET_CODE(nCurrentLifePercent <= m_nTargetLifePercentMax, srcTargetLifeError);
    KG_PROCESS_ERROR_RET_CODE(nCurrentLifePercent >= m_nTargetLifePercentMin, srcTargetLifeError);

    if (IS_PLAYER(pTargetCharacter->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)pTargetCharacter;

        // 骑马判断
        switch (m_pBaseInfo->nTargetHorseStateRequest)
        {
        case shcAny:
            break;
        case shcHorse:
            KG_PROCESS_ERROR_RET_CODE(pPlayer->m_bOnHorse, srcErrorByHorse);
            break;
        case shcNotHorse:
            KG_PROCESS_ERROR_RET_CODE(!pPlayer->m_bOnHorse, srcErrorByHorse);
            break;
        default:
            KGLogPrintf(
                KGLOG_ERR, "[Skill] Unexpected skill target horse state request %d !\n", 
                m_pBaseInfo->nTargetHorseStateRequest
            );
            goto Exit0;
        }
    }

Exit1:
    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CheckTargetBuffList(KCharacter* pCaster, KTarget& rTarget)
{
    SKILL_RESULT_CODE   nResult     = srcFailed;
    SKILL_RESULT_CODE   eRetCode    = srcFailed;
    BOOL                bRetCode    = false;
    KCharacter*         pTarget     = NULL;

    bRetCode = rTarget.GetTarget(&pTarget);
    KG_PROCESS_ERROR(bRetCode && pTarget);

    for (int i = 0; i < sizeof(m_BindBuffs) / sizeof(m_BindBuffs[0]); i++)
    {
        KBuff* pBuff = m_BindBuffs[i].GetPointer();
        if (!pBuff)
            continue;
        
        eRetCode = pTarget->m_BuffList.CanAddBuff(pCaster->m_dwID, pBuff);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == srcSuccess, eRetCode);
    }

    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::CallCastScript(KCharacter *pCaster, SKILL_RESULT_CODE nPreResult)
{
    SKILL_RESULT_CODE   nResult         = srcFailed;
    BOOL                bRetCode        = false;
    SKILL_RESULT_CODE   nScriptRetCode  = srcFailed;
    int                 nLuaTopIndex    = 0;
    Lua_State          *L               = NULL;
    KPlayer            *pPlayer         = NULL;

    KG_ASSERT_EXIT(pCaster);
    assert(m_pBaseInfo);

    bRetCode = IS_PLAYER(pCaster->m_dwID);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, nPreResult);
    pPlayer = (KPlayer *)pCaster;

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_pBaseInfo->szScriptFile);
    if (!bRetCode)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "[Skill] Init skill, get script \"%s\" failed, skill id = %u, name = \"%s\"\n", 
            m_pBaseInfo->szScriptFile,
            m_pBaseInfo->dwSkillID,
            m_pBaseInfo->szSkillName
        );
        goto Exit0;
    }

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nLuaTopIndex);
   
	g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
	g_pSO3World->m_ScriptCenter.PushValueToStack((int)nPreResult);

	g_pSO3World->m_ScriptCenter.CallFunction(m_pBaseInfo->szScriptFile, "CanCast", 1);
    g_pSO3World->m_ScriptCenter.GetValuesFromStack("d", &nScriptRetCode);
	g_pSO3World->m_ScriptCenter.SafeCallEnd(nLuaTopIndex);

    KG_PROCESS_ERROR_RET_CODE(nScriptRetCode == srcSuccess, nScriptRetCode);

    nResult = srcSuccess;
Exit0:
    return nResult;
}

BOOL KSkill::AutoSelectTarget(KCharacter* pCaster)
{
    BOOL                bResult             = false;
    BOOL                bRetCode            = false;
    int                 nSize               = 0;
    KCharacter*         pCharacter          = NULL;
    KSearchForCharacter SearchForCharacter;
    std::vector<KSearchNode>::iterator it;

    assert(m_pBaseInfo);

    KGLOG_PROCESS_ERROR(pCaster);
    KGLOG_PROCESS_ERROR(m_pBaseInfo->bAutoSelectTarget);
    KGLOG_PROCESS_ERROR(
        m_pBaseInfo->nCastMode == scmTargetArea || m_pBaseInfo->nCastMode == scmTargetSingle
    );

    SearchForCharacter.m_pSelf      = pCaster;
    SearchForCharacter.m_nDistance2 = m_nMaxRadius * m_nMaxRadius;
    SearchForCharacter.m_nAngle     = m_nAngleRange;
    SearchForCharacter.m_nRelation  = m_pBaseInfo->nTargetRelationMask;

    AISearchCharacter(SearchForCharacter);

    nSize = (int)SearchForCharacter.m_Result.size();
	KG_PROCESS_ERROR(nSize > 0);

    //按距离排序
	it = min_element(SearchForCharacter.m_Result.begin(), SearchForCharacter.m_Result.end(), g_CompByDistance);
    
    pCharacter = it->m_pCharacter;

    if (IS_PLAYER(pCharacter->m_dwID))
    {
        bRetCode = pCaster->SelectTarget(ttPlayer, pCharacter->m_dwID);
    }
    else
    {
        bRetCode = pCaster->SelectTarget(ttNpc, pCharacter->m_dwID);
    }
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _CLIENT
struct KSKILL_ATTRIBUTE_INFO
{
    int     nSkillPhysicsDamage;
    int     nSkillPhysicsDamageRand;
    int     nSkillPhysicsDamagePercent;
    int     nCallPhysicsDamage;

    int     nSkillSolarDamage;
    int     nSkillSolarDamageRand;
    int     nSkillSolarDamagePercent;
    int     nCallSolarDamage;

    int     nSkillNeutralDamage;
    int     nSkillNeutralDamageRand;
    int     nSkillNeutralDamagePercent;
    int     nCallNeutralDamage;

    int     nSkillLunarDamage;
    int     nSkillLunarDamageRand;
    int     nSkillLunarDamagePercent;
    int     nCallLunarDamage;

    int     nSkillPoisonDamage;
    int     nSkillPoisonDamageRand;
    int     nSkillPoisonDamagePercent;
    int     nCallPoisonDamage;

    int     nSkillTherapy;
    int     nSkillTherapyRand;
    int     nSkillTherapyPercent;
    int     nCallTherapy;
};


int _GetSkillDisplayInfoByAttributeList(KAttribute* pAttributeHandle, KSKILL_ATTRIBUTE_INFO* pSkillAttribute)
{
    BOOL        bResult     = false;
    KAttribute* pAttribute  = NULL;

    // pAttribute为空为没有魔法属性，是正常的情况
    assert(pSkillAttribute);

    for (pAttribute = pAttributeHandle; pAttribute; pAttribute = pAttribute->pNext)
    {
        switch (pAttribute->nKey)
        {
        case atSkillPhysicsDamage:
            pSkillAttribute->nSkillPhysicsDamage += pAttribute->nValue1;
            break;

        case atSkillPhysicsDamageRand:
            pSkillAttribute->nSkillPhysicsDamageRand += pAttribute->nValue1;
            break;

        case atSkillPhysicsDamagePercent:
            pSkillAttribute->nSkillPhysicsDamagePercent += pAttribute->nValue1;
            break;

        case atCallPhysicsDamage:
            pSkillAttribute->nCallPhysicsDamage += pAttribute->nValue1;
            break;

        case atSkillSolarDamage:
            pSkillAttribute->nSkillSolarDamage += pAttribute->nValue1;
            break;

        case atSkillSolarDamageRand:
            pSkillAttribute->nSkillSolarDamageRand += pAttribute->nValue1;
            break;

        case atSkillSolarDamagePercent:
            pSkillAttribute->nSkillSolarDamagePercent += pAttribute->nValue1;
            break;

        case atCallSolarDamage:
            pSkillAttribute->nCallSolarDamage += pAttribute->nValue1;
            break;

        case atSkillNeutralDamage:
            pSkillAttribute->nSkillNeutralDamage += pAttribute->nValue1;
            break;

        case atSkillNeutralDamageRand:
            pSkillAttribute->nSkillNeutralDamageRand += pAttribute->nValue1;
            break;

        case atSkillNeutralDamagePercent:
            pSkillAttribute->nSkillNeutralDamagePercent += pAttribute->nValue1;
            break;

        case atCallNeutralDamage:
            pSkillAttribute->nCallNeutralDamage += pAttribute->nValue1;
            break;

        case atSkillLunarDamage:
            pSkillAttribute->nSkillLunarDamage += pAttribute->nValue1;
            break;

        case atSkillLunarDamageRand:
            pSkillAttribute->nSkillLunarDamageRand += pAttribute->nValue1;
            break;

        case atSkillLunarDamagePercent:
            pSkillAttribute->nSkillLunarDamagePercent += pAttribute->nValue1;
            break;

        case atCallLunarDamage:
            pSkillAttribute->nCallLunarDamage += pAttribute->nValue1;
            break;

        case atSkillPoisonDamage:
            pSkillAttribute->nSkillPoisonDamage += pAttribute->nValue1;
            break;
            
        case atSkillPoisonDamageRand:
            pSkillAttribute->nSkillPoisonDamageRand += pAttribute->nValue1;
            break;

        case atSkillPoisonDamagePercent:
            pSkillAttribute->nSkillPoisonDamagePercent += pAttribute->nValue1;
            break;

        case atCallPoisonDamage:
            pSkillAttribute->nCallPoisonDamage += pAttribute->nValue1;
            break;

        case atSkillTherapy:
            pSkillAttribute->nSkillTherapy += pAttribute->nValue1;
            break;

        case atSkillTherapyRand:
            pSkillAttribute->nSkillTherapyRand += pAttribute->nValue1;
            break;

        case atSkillTherapyPercent:
            pSkillAttribute->nSkillTherapyPercent += pAttribute->nValue1;
            break;

        case atCallTherapy:
            pSkillAttribute->nCallTherapy += pAttribute->nValue1;
            break;
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

int KSkill::GetSkillDisplayInfo(KSKILL_DISPLAY_INFO* pSkillDisplayInfo)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KAttribute*             pAttribute      = NULL;
    KSKILL_ATTRIBUTE_INFO   SelfNotRollBack = {0};
    KSKILL_ATTRIBUTE_INFO   SelfAndRollBack = {0};
    KSKILL_ATTRIBUTE_INFO   DestNotRollBack = {0};
    KSKILL_ATTRIBUTE_INFO   DestAndRollBack = {0};
    KSKILL_ATTRIBUTE_INFO   DotAttribute    = {0};
    KSKILL_ATTRIBUTE_INFO   HotAttribute    = {0};
    KPlayer*                pPlayer         = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);

    assert(pSkillDisplayInfo);

    KGLOG_PROCESS_ERROR(pPlayer);

    memset(pSkillDisplayInfo, 0, sizeof(KSKILL_DISPLAY_INFO));

    pSkillDisplayInfo->nCastTime    = m_nPrepareFrames;
    pSkillDisplayInfo->nAreaRadius  = m_nAreaRadius;
    pSkillDisplayInfo->nMinRadius   = m_nMinRadius;
    pSkillDisplayInfo->nMaxRadius   = m_nMaxRadius;
    pSkillDisplayInfo->nCostMana    = m_nCostMana * pPlayer->m_nModifyCostManaPercent / KILO_NUM;
    pSkillDisplayInfo->nCostMana    = max(pSkillDisplayInfo->nCostMana, 0);
	pSkillDisplayInfo->nCostLife	= m_nCostLife;

    for (int i = 0; i < MAX_SKILL_COOL_DOWN_TIMER; i++)
    {
        DWORD dwCoolDownID = m_dwCoolDownID[i];

        if (!dwCoolDownID)
            continue;

        pSkillDisplayInfo->nCoolDown[i]  = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(dwCoolDownID);
        pSkillDisplayInfo->nCoolDown[i] += m_nCoolDownAlter[i];
    }

    bRetCode = _GetSkillDisplayInfoByAttributeList(m_pAttrbuteEffectToSelfAndRollback, &SelfAndRollBack);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = _GetSkillDisplayInfoByAttributeList(m_pAttrbuteEffectToSelfNotRollback, &SelfNotRollBack);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = _GetSkillDisplayInfoByAttributeList(m_pAttrbuteEffectToDestAndRollback, &DestAndRollBack);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = _GetSkillDisplayInfoByAttributeList(m_pAttrbuteEffectToDestNotRollback, &DestNotRollBack);
    KGLOG_PROCESS_ERROR(bRetCode);


    pSkillDisplayInfo->nMinPhysicsDamage = SelfAndRollBack.nSkillPhysicsDamage 
        * (KILO_NUM + SelfAndRollBack.nSkillPhysicsDamagePercent) / KILO_NUM;

    pSkillDisplayInfo->nMaxPhysicsDamage = 
        (SelfAndRollBack.nSkillPhysicsDamage + SelfAndRollBack.nSkillPhysicsDamageRand)
        * (KILO_NUM + SelfAndRollBack.nSkillPhysicsDamagePercent) / KILO_NUM;


    pSkillDisplayInfo->nMinSolarDamage = SelfAndRollBack.nSkillSolarDamage 
        * (KILO_NUM + SelfAndRollBack.nSkillSolarDamagePercent) / KILO_NUM;

    pSkillDisplayInfo->nMaxSolarDamage = 
        (SelfAndRollBack.nSkillSolarDamage + SelfAndRollBack.nSkillSolarDamageRand)
        * (KILO_NUM + SelfAndRollBack.nSkillSolarDamagePercent) / KILO_NUM;


    pSkillDisplayInfo->nMinNeutralDamage = SelfAndRollBack.nSkillNeutralDamage 
        * (KILO_NUM + SelfAndRollBack.nSkillNeutralDamagePercent) / KILO_NUM;

    pSkillDisplayInfo->nMaxNeutralDamage = 
        (SelfAndRollBack.nSkillNeutralDamage + SelfAndRollBack.nSkillNeutralDamageRand)
        * (KILO_NUM + SelfAndRollBack.nSkillNeutralDamagePercent) / KILO_NUM;


    pSkillDisplayInfo->nMinLunarDamage = SelfAndRollBack.nSkillLunarDamage 
        * (KILO_NUM + SelfAndRollBack.nSkillLunarDamagePercent) / KILO_NUM;

    pSkillDisplayInfo->nMaxLunarDamage = 
        (SelfAndRollBack.nSkillLunarDamage + SelfAndRollBack.nSkillLunarDamageRand)
        * (KILO_NUM + SelfAndRollBack.nSkillLunarDamagePercent) / KILO_NUM;


    pSkillDisplayInfo->nMinPoisonDamage = SelfAndRollBack.nSkillPoisonDamage 
        * (KILO_NUM + SelfAndRollBack.nSkillPoisonDamagePercent) / KILO_NUM;

    pSkillDisplayInfo->nMaxPoisonDamage = 
        (SelfAndRollBack.nSkillPoisonDamage + SelfAndRollBack.nSkillPoisonDamageRand)
        * (KILO_NUM + SelfAndRollBack.nSkillPoisonDamagePercent) / KILO_NUM;


    pSkillDisplayInfo->nMinTherapy = SelfAndRollBack.nSkillTherapy
        * (KILO_NUM + SelfAndRollBack.nSkillTherapyPercent) / KILO_NUM;

    pSkillDisplayInfo->nMaxTherapy = 
        (SelfAndRollBack.nSkillTherapy + SelfAndRollBack.nSkillTherapyRand)
        * (KILO_NUM + SelfAndRollBack.nSkillTherapyPercent) / KILO_NUM;


    for (int i = 0; i < sizeof(m_BindBuffs) / sizeof(m_BindBuffs[0]); i++)
    {
        KBuff*                  pBuff = m_BindBuffs[i].GetPointer();
        KSKILL_ATTRIBUTE_INFO   BuffAttributeInfo;

        if (!pBuff)
            continue;

        pSkillDisplayInfo->nBuffCount[i] = pBuff->m_nActiveCount;
        pSkillDisplayInfo->nBuffIntervalFrame[i] = pBuff->m_nIntervalFrame;

        bRetCode = _GetSkillDisplayInfoByAttributeList(pBuff->m_pActiveAttribList, &BuffAttributeInfo);
        KGLOG_PROCESS_ERROR(bRetCode);

        pSkillDisplayInfo->nBuffDamage[i] = BuffAttributeInfo.nCallPhysicsDamage;
        pSkillDisplayInfo->nBuffDamage[i] = BuffAttributeInfo.nCallSolarDamage;
        pSkillDisplayInfo->nBuffDamage[i] = BuffAttributeInfo.nCallNeutralDamage;
        pSkillDisplayInfo->nBuffDamage[i] = BuffAttributeInfo.nCallLunarDamage;
        pSkillDisplayInfo->nBuffDamage[i] = BuffAttributeInfo.nCallPoisonDamage;

        pSkillDisplayInfo->nBuffDamage[i] = pSkillDisplayInfo->nBuffDamage[i] * (KILO_NUM) / KILO_NUM;

        pSkillDisplayInfo->nBuffTherapy[i] = BuffAttributeInfo.nCallTherapy * (KILO_NUM);
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

#ifdef _SERVER


SKILL_RESULT_CODE KSkill::CastOnSector(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey)
{
	SKILL_RESULT_CODE   nResult         = srcFailed;
    int                 nRetCode        = srcFailed;
    KSKILL_BULLET*      pBullet         = NULL;   
    KTarget             Target;
    
	// 对扇形范围攻击而言,其开始的目标并不是最终作用的对象,而是自己
    nRetCode = Target.SetTarget(pDisplayCaster);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, srcFailed);

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);

    nRetCode = SetupBullet(pBullet, pLogicCaster, Target, crRecipeKey);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ApplyOnSector(pBullet);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, (SKILL_RESULT_CODE)nRetCode);

	nResult = srcSuccess;
Exit0:
    if (pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
	return nResult;
}

SKILL_RESULT_CODE KSkill::CastOnCasterArea(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey)
{
	int                 nResult         = srcFailed;
    int                 nRetCode        = srcFailed;
    KSKILL_BULLET*      pBullet         = NULL;   
    KTarget             Target;
    
    // 对这种范围攻击而言,其开始的目标并不是最终作用的对象,而是自己
    nRetCode = Target.SetTarget(pDisplayCaster);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, srcFailed);

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);

    nRetCode = SetupBullet(pBullet, pLogicCaster, Target, crRecipeKey);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ApplyOnArea(pBullet, pDisplayCaster->m_nX, pDisplayCaster->m_nY);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

	nResult = srcSuccess;
Exit0:
    if (pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
	return (SKILL_RESULT_CODE)nResult;
}

SKILL_RESULT_CODE KSkill::CastOnTargetArea(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget &rTarget, const KSKILL_RECIPE_KEY& crRecipeKey)
{
	SKILL_RESULT_CODE   nResult         = srcFailed;
    int                 nRetCode        = srcFailed;
    KSKILL_BULLET*      pBullet         = NULL; 
    KSceneObject*       pTarget         = NULL;
    BOOL                bAddFlag        = false;
    int                 nTargetX        = 0;
    int                 nTargetY        = 0;
    int                 nTargetZ        = 0;

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);
        
    nRetCode = SetupBullet(pBullet, pLogicCaster, rTarget, crRecipeKey);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    if (pBullet->nEndFrame > g_pSO3World->m_nGameLoop)
    {
        pLogicCaster->AddBullet(pBullet);
        bAddFlag = true;
    }
    else
    {
	    nRetCode = rTarget.GetTarget(&nTargetX, &nTargetY, &nTargetZ);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = ApplyOnSingle(pBullet);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, ((SKILL_RESULT_CODE)nRetCode));
        
        nRetCode = ApplyOnArea(pBullet, nTargetX, nTargetY);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, (SKILL_RESULT_CODE)nRetCode);
    }

	nResult = srcSuccess;
Exit0:
    if ((!bAddFlag) && pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
	return nResult;
}

SKILL_RESULT_CODE KSkill::CastOnPointArea(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey)
{
	SKILL_RESULT_CODE   nResult         = srcFailed;
    int                 nRetCode        = srcFailed;
    KSKILL_BULLET*      pBullet         = NULL; 
    KSceneObject*       pTarget         = NULL;
    BOOL                bAddFlag        = false;
    int                 nTargetX        = 0;
    int                 nTargetY        = 0;
    int                 nTargetZ        = 0;

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);
        
    nRetCode = SetupBullet(pBullet, pLogicCaster, rTarget, crRecipeKey);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    if (pBullet->nEndFrame > g_pSO3World->m_nGameLoop)
    {
        pLogicCaster->AddBullet(pBullet);
        bAddFlag = true;
    }
    else
    {
	    nRetCode = rTarget.GetTarget(&nTargetX, &nTargetY, &nTargetZ);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        nRetCode = ApplyOnArea(pBullet, nTargetX, nTargetY);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, (SKILL_RESULT_CODE)nRetCode);        
    }

	nResult = srcSuccess;
Exit0:
    if ((!bAddFlag) && pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
	return nResult;
}

SKILL_RESULT_CODE KSkill::CastOnCasterSingle(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey)
{
	SKILL_RESULT_CODE   nResult         = srcFailed;
    int                 nRetCode        = false;
    KSKILL_BULLET*      pBullet         = NULL;  
    KTarget             Target;
    
    // 对这种范围攻击而言,其开始的目标并不是最终作用的对象,而是自己
    nRetCode = Target.SetTarget(pDisplayCaster);
    KGLOG_PROCESS_ERROR(nRetCode);

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);

    nRetCode = SetupBullet(pBullet, pLogicCaster, Target, crRecipeKey);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ApplyOnSingle(pBullet);
    KG_PROCESS_ERROR(nRetCode);

	nResult = srcSuccess;
Exit0:
    if (pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
	return nResult;
}

SKILL_RESULT_CODE KSkill::CastOnTargetSingle(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget &rTarget, const KSKILL_RECIPE_KEY& crRecipeKey)
{
	SKILL_RESULT_CODE   nResult         = srcFailed;
    int                 nRetCode        = false;
    KSKILL_BULLET*      pBullet         = NULL;
    BOOL                bAddFlag        = false;

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);

    nRetCode = SetupBullet(pBullet, pLogicCaster, rTarget, crRecipeKey);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 添加或应用子弹
    if (pBullet->nEndFrame > g_pSO3World->m_nGameLoop)
    {
        pLogicCaster->AddBullet(pBullet);
        bAddFlag = true;
    }
    else
    {
        ApplyOnSingle(pBullet);
    }

	nResult = srcSuccess;
Exit0:
    if ((!bAddFlag) && pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
	return nResult;
}

SKILL_RESULT_CODE KSkill::CastOnPoint(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget &rTarget, const KSKILL_RECIPE_KEY& crRecipeKey)
{
    SKILL_RESULT_CODE nResult = srcFailed;

    // 轻功之类的 ...

    nResult = srcSuccess;
//Exit0:
    return nResult;
}



struct FindTarget
{
    static int DistanceCmp(const KSKILL_CHAIN_TARGET& crTargetA, const KSKILL_CHAIN_TARGET& crTargetB)
    {
        return (crTargetA.nDistance < crTargetB.nDistance);
    }

    BOOL operator () (KCharacter* pCharacter)
    {
        BOOL                                        bResult     = false;
        int                                         nRetCode    = false;
        int                                         nDistance   = 0;
        KTarget                                     Target;
        KSKILL_CHAIN_TARGET                         InsertTarget;
        KCharacter*                                 pCenter;
        std::vector<KSKILL_CHAIN_TARGET>::iterator  itFind;

        assert(!AttackArray.empty());
        pCenter = AttackArray.begin()->pCharacter;
        KG_PROCESS_SUCCESS(pCharacter == pCenter);

        nRetCode = Target.SetTarget(pCharacter);
        KGLOG_PROCESS_ERROR(nRetCode);

        // 检查该技能是否能对目标施放
        nRetCode = pSrcSkill->CheckTargetType(pCaster, Target);
        KG_PROCESS_SUCCESS(nRetCode != srcSuccess);
        // 检查目标对象的状态是否允许施放这个技能
        nRetCode = pSrcSkill->CheckTargetState(Target);
        KG_PROCESS_SUCCESS(nRetCode != srcSuccess);
        // 检查技能带的BUFF能不能加到目标身上
        if (pSrcSkill->m_pBaseInfo->bCheckBindBuff)
        {
            nRetCode = pSrcSkill->CheckTargetBuffList(pCaster, Target);
            KG_PROCESS_SUCCESS(nRetCode != srcSuccess);
        }

        nRetCode = SkillInRange(
            pCenter->m_nX, pCenter->m_nY, pCenter->m_nZ,
            pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_nZ,
            nRadius
        );
        KG_PROCESS_SUCCESS(!nRetCode);
        
        nDistance = g_GetDistance3(
            pCenter->m_nX, pCenter->m_nY, pCenter->m_nZ,
            pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_nZ
        );

        InsertTarget = KSKILL_CHAIN_TARGET(pCharacter, nDistance);
        itFind = std::lower_bound(AttackArray.begin(), AttackArray.end(), InsertTarget, DistanceCmp);
        AttackArray.insert(itFind, InsertTarget);

        if (AttackArray.size() > uAttackNum)
        {
            AttackArray.resize(uAttackNum);
        }

    Exit1:
        bResult = true;
    Exit0:
        return bResult;
    }

    std::vector<KSKILL_CHAIN_TARGET> AttackArray;
    size_t                     uAttackNum;
    KCharacter*                pCaster;
    KSkill*                    pSrcSkill;
    int                        nRadius;
};

SKILL_RESULT_CODE KSkill::CastOnTargetChain(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, KTarget& rTarget, const KSKILL_RECIPE_KEY& crRecipeKey)
{
    SKILL_RESULT_CODE                       nResult         = srcFailed;
    int                                     nRetCode        = 0;
    BOOL                                    bAddFlag        = false;
    KSKILL_BULLET*                          pBullet         = NULL;

    assert(pDisplayCaster);
    assert(pLogicCaster);

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);

    nRetCode = SetupBullet(pBullet, pLogicCaster, rTarget, crRecipeKey);
    KGLOG_PROCESS_ERROR(nRetCode);

    if (pBullet->nEndFrame > g_pSO3World->m_nGameLoop)
    {
        pLogicCaster->AddBullet(pBullet);
        bAddFlag = true;
    }
    else
    {
        ApplyOnChain(pBullet);
    }

    nResult = srcSuccess;
Exit0:
    if ((!bAddFlag) && pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }

    return nResult;
}

SKILL_RESULT_CODE KSkill::CastOnTargetLeader(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey)
{
    SKILL_RESULT_CODE   nResult     = srcFailed;
    BOOL                bRetCode    = false;
    BOOL                bAddFlag    = false;
    KPlayer*            pPlayer     = NULL;
    KPlayer*            pLeader     = NULL;
    KSKILL_BULLET*      pBullet     = NULL;
    KTeam*              pTeam       = NULL;
    int                 nGroupIndex = 0;
    KTarget             Target;

    assert(pDisplayCaster);
    assert(pLogicCaster);

    KGLOG_PROCESS_ERROR(IS_PLAYER(pLogicCaster->m_dwID));

    pPlayer = (KPlayer*)pLogicCaster;

    nGroupIndex = pPlayer->GetCurrentTeamGroupIndex();

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR_RET_CODE(pTeam, srcNotParty);

    pLeader = g_pSO3World->m_PlayerSet.GetObj(
        pTeam->MemberGroup[nGroupIndex].dwFormationLeader
    );
    KGLOG_PROCESS_ERROR(pLeader);

    bRetCode = Target.SetTarget(pLeader);
    KGLOG_PROCESS_ERROR(bRetCode);

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);

    bRetCode = SetupBullet(pBullet, pPlayer, Target, crRecipeKey);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (pBullet->nEndFrame > g_pSO3World->m_nGameLoop)
    {
        pLeader->AddBullet(pBullet);
        bAddFlag = true;
    }
    else
    {
        pLeader->ApplyBullet(pBullet);
    }

    nResult = srcSuccess;
Exit0:

    if (!bAddFlag && pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
    return nResult;
}

SKILL_RESULT_CODE KSkill::CastOnPartyArea(KCharacter* pDisplayCaster, KCharacter* pLogicCaster, const KSKILL_RECIPE_KEY& crRecipeKey)
{
    SKILL_RESULT_CODE   nResult     = srcFailed;
    int                 nRetCode    = 0;
    KPlayer*            pPlayer     = NULL;
    KTeam*              pTeam       = NULL;
    KSKILL_BULLET*      pBullet     = NULL;
    int                 nGroupIndex = 0;
    KTarget             Target;

    assert(pDisplayCaster);
    assert(pLogicCaster);

    KGLOG_PROCESS_ERROR(IS_PLAYER(pLogicCaster->m_dwID));

    pPlayer = (KPlayer*)pLogicCaster;

    nGroupIndex = pPlayer->GetCurrentTeamGroupIndex();

    Target.SetTarget(pDisplayCaster);

    pBullet = KMemory::New<KSKILL_BULLET>();
    KGLOG_PROCESS_ERROR(pBullet);

    nRetCode = SetupBullet(pBullet, pLogicCaster, Target, crRecipeKey);
    KGLOG_PROCESS_ERROR(nRetCode);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pPlayer->m_dwTeamID);
    if (!pTeam)
    {
        nRetCode = Target.SetTarget(pDisplayCaster);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = CheckTargetType(pLogicCaster, Target);
        KGLOG_PROCESS_ERROR(nRetCode == srcSuccess);

        nRetCode = CheckTargetState(Target);
        KGLOG_PROCESS_ERROR(nRetCode == srcSuccess);

        if (m_pBaseInfo->bCheckBindBuff)
        {
            nRetCode = CheckTargetBuffList(pLogicCaster, Target);
            KGLOG_PROCESS_ERROR(nRetCode == srcSuccess);
        }

        pLogicCaster->ApplyBullet(pBullet);

        goto Exit1;
    }

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nGroupIndex].MemberList.end();
        ++it
    )
    {
        KTEAM_MEMBER_INFO*  pMemberInfo = &(*it);
        KPlayer*            pMember     = g_pSO3World->m_PlayerSet.GetObj(pMemberInfo->dwMemberID);

        if (!pMember)
            continue;

        nRetCode = Target.SetTarget(pMember);
        if (!nRetCode)
            continue;

        if (pMember->m_pScene != pDisplayCaster->m_pScene)
            continue;

        nRetCode = CheckTargetType(pLogicCaster, Target);
        if (nRetCode != srcSuccess)
            continue;

        nRetCode = CheckTargetState(Target);
        if (nRetCode != srcSuccess)
            continue;

        if (m_pBaseInfo->bCheckBindBuff)
        {
            nRetCode = CheckTargetBuffList(pLogicCaster, Target);
            if (nRetCode != srcSuccess)
                continue;
        }

        nRetCode = SkillInRange(
            pDisplayCaster->m_nX, pDisplayCaster->m_nY, pDisplayCaster->m_nZ,
            pMember->m_nX, pMember->m_nY, pMember->m_nZ,
            m_nAreaRadius
        );
        if (!nRetCode)
            continue;

        if (m_pBaseInfo->bUse3DObstacle)
        {
            int nSrcZ = ZPOINT_TO_ALTITUDE(XYPOINT_TO_ZPOINT(pDisplayCaster->m_nZ));
            int nDstZ = ZPOINT_TO_ALTITUDE(XYPOINT_TO_ZPOINT(pMember->m_nZ));

            KScene* pScene = pDisplayCaster->m_pScene;
            nRetCode = pScene->IsVisible(pDisplayCaster->m_nX, pDisplayCaster->m_nY, nSrcZ, pMember->m_nX, pMember->m_nY, nDstZ);
            if (!nRetCode)
                continue;
        }

        pMember->ApplyBullet(pBullet);
    }

Exit1:
    nResult = srcSuccess;
Exit0:

    if (pBullet)
    {
        KMemory::Delete(pBullet);
        pBullet = NULL;
    }
    return nResult;
}

BOOL KSkill::SetupBullet(KSKILL_BULLET *pBullet, KCharacter* pCaster, KTarget &rTarget, const KSKILL_RECIPE_KEY& crRecipeKey)
{
	BOOL                bResult             = false;
    int                 nRetCode            = false;
    int                 nCasterX            = 0;
    int                 nCasterY            = 0;
    int                 nCasterZ            = 0;
    int                 nTargetX            = 0;
    int                 nTargetY            = 0;
    int                 nTargetZ            = 0;
    int                 nTouchRange         = 0;
    int                 nHitDelay           = 0;
    KSceneObject*       pTarget             = NULL;
    int                 nDistance           = 0;
    BOOL                bRollbackAttrib     = false;

    int                 nWeaponDamage       = 0;
    int                 nPhysicsAP          = 0;
    int                 nPhysicsDamage      = 0;
    int                 nWeaponLow          = 0;
    int                 nWeaponHigh         = 0;

    int                 nSolarAP            = 0;    
    int                 nSolarDamage        = 0;
    int                 nLunarAP            = 0;
    int                 nLunarDamage        = 0;
    int                 nNeutralAP          = 0;
    int                 nNeutralDamage      = 0;
    int                 nPoisonAP           = 0;
    int                 nPoisonDamage       = 0;
    int                 nTherapy            = 0;
    int                 nSkillCoefficient   = 0;
    int                 nRandValue          = 0;
    int                 nWeaponDetailType   = 0;

    int                 nSkillPhysicsDamage = 0;
    int                 nSkillSolarDamage   = 0;
    int                 nSkillNeutralDamage = 0;
    int                 nSkillLunarDamage   = 0;
    int                 nSkillPosionDamage  = 0;
    int                 nDenominator        = 0;

    KGLOG_PROCESS_ERROR(pBullet);
    KGLOG_PROCESS_ERROR(pCaster);
    
    pBullet->dwBulletID	    = g_pSO3World->m_SkillManager.m_dwBulletIDIndex++;
    pBullet->Target		    = rTarget;
    pBullet->pSkillSrc      = pCaster;
    pBullet->dwSkillSrcID   = pCaster->m_dwID;
    pBullet->nSkillSrcLevel = pCaster->m_nLevel;
    pBullet->SkillRecipePointer.SetRecipeKey(crRecipeKey);
    pBullet->pNext		    = NULL;

    pCaster->ApplyAttribute(m_pAttrbuteEffectToSelfAndRollback);
    bRollbackAttrib = true;
    
    nSkillCoefficient = max((m_nPrepareFrames + m_nChannelInterval), GAME_FPS);

    if (IS_PLAYER(pCaster->m_dwID))
    {        
        int         nWeaponGenre                = 0;
        KPlayer*    pPlayer                     = NULL;
        KItem*      pWeapon                     = NULL;

        pPlayer = (KPlayer*)pCaster;

        nWeaponGenre = g_pSO3World->m_Settings.m_WeaponTypeList.GetWeaponGenre(m_pBaseInfo->dwWeaponRequest);
        KG_PROCESS_ERROR(nWeaponGenre != -1);
        
        if (nWeaponGenre == swrtMeleeWeapon)
        {
            pWeapon = pPlayer->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
            // 外功伤害 = 武器伤害 +　基础武学附加的伤害 + 人物本身属性造成的附加伤害 + 技能所带的伤害            
            if (pWeapon)
            {
                nWeaponDamage = 
                    (pCaster->m_nMeleeWeaponDamageBase + g_Random(pCaster->m_nMeleeWeaponDamageRand + 1)) * 
                    m_nWeaponDamagePercent / KILO_NUM;
                nWeaponDetailType = pWeapon->m_Common.nDetail;

                nWeaponLow  = pCaster->m_nMeleeWeaponDamageBase;
                nWeaponHigh = pCaster->m_nMeleeWeaponDamageBase + pCaster->m_nMeleeWeaponDamageRand;
            }
            else
            {
                nWeaponDamage = 
                    (pCaster->m_nCurrentStrength / 10 + pCaster->m_nCurrentSpirit / 12 + 3) * 
                    m_nWeaponDamagePercent / KILO_NUM;
                nWeaponDetailType = wdtFist;

                nWeaponLow  = nWeaponDamage;
                nWeaponHigh = nWeaponDamage;
            }

            if (g_pSO3World->m_SkillManager.IsCommonSkill(m_pBaseInfo->dwSkillID))
            {
                nSkillCoefficient = max((pCaster->m_nMeleeWeaponAttackSpeed), GAME_FPS);
            }
        }

        if (nWeaponGenre == swrtRangeWeapon)
        {
            nWeaponDamage = 
                (pCaster->m_nRangeWeaponDamageBase + g_Random(pCaster->m_nRangeWeaponDamageRand + 1)) * 
                m_nWeaponDamagePercent / KILO_NUM;

            pWeapon = pPlayer->m_ItemList.GetItem(ibEquip, eitRangeWeapon);
            KGLOG_PROCESS_ERROR(pWeapon);

            nWeaponLow  = pCaster->m_nRangeWeaponDamageBase;
            nWeaponHigh = pCaster->m_nRangeWeaponDamageBase + pCaster->m_nRangeWeaponDamageRand;

            nWeaponDetailType = pWeapon->m_Common.nDetail;

            if (g_pSO3World->m_SkillManager.IsCommonSkill(m_pBaseInfo->dwSkillID))
            {
                nSkillCoefficient = max((pCaster->m_nRangeWeaponAttackSpeed), GAME_FPS);
            }
        }
        
        nPhysicsAP = pCaster->m_nPhysicsAttackPower;
        
        nSolarAP   = pCaster->m_nSolarAttackPower;
        nNeutralAP = pCaster->m_nNeutralAttackPower;
        nLunarAP   = pCaster->m_nLunarAttackPower;
        nPoisonAP  = pCaster->m_nPoisonAttackPower;
    }
    else
    {
        if (m_pBaseInfo->nNPCWeaponType == swrtMeleeWeapon)
        {
            nWeaponDamage = 
                (pCaster->m_nMeleeWeaponDamageBase + g_Random(pCaster->m_nMeleeWeaponDamageRand + 1)) * 
                m_nWeaponDamagePercent / KILO_NUM;
            
            nWeaponLow  = pCaster->m_nMeleeWeaponDamageBase;
            nWeaponHigh = pCaster->m_nMeleeWeaponDamageBase + pCaster->m_nMeleeWeaponDamageRand;
        }
        else if (m_pBaseInfo->nNPCWeaponType == swrtRangeWeapon)
        {
            nWeaponDamage = 
                (pCaster->m_nRangeWeaponDamageBase + g_Random(pCaster->m_nRangeWeaponDamageRand + 1)) * 
                m_nWeaponDamagePercent / KILO_NUM;

            nWeaponLow  = pCaster->m_nRangeWeaponDamageBase;
            nWeaponHigh = pCaster->m_nRangeWeaponDamageBase + pCaster->m_nRangeWeaponDamageRand;
        }
        // else 不予处理
    }

    nRandValue = g_Random(pCaster->m_nSkillPhysicsDamageRand + 1);
    nSkillPhysicsDamage = (pCaster->m_nSkillPhysicsDamage + nRandValue) * (KILO_NUM + pCaster->m_nSkillPhysicsDamagePercent) / KILO_NUM;
    nPhysicsDamage = nSkillPhysicsDamage + nWeaponDamage + nPhysicsAP * nSkillCoefficient / PHYSICS_AP_TO_DPS / GAME_FPS;  

    pCaster->m_SkillDebug.Trace(
        2, "[技能调试][%d]:[伤害计算] 外功伤害计算:%d  武器伤害:%d(%d-%d) + AP伤害:%d*%d/10 + 技能伤害:%d(%d-%d) * %d/1024 From(%s-%u)",
        g_pSO3World->m_nGameLoop, nPhysicsDamage,
        nWeaponDamage, nWeaponLow, nWeaponHigh,
        nPhysicsAP, nSkillCoefficient / GAME_FPS,
        nSkillPhysicsDamage, pCaster->m_nSkillPhysicsDamage, pCaster->m_nSkillPhysicsDamage + pCaster->m_nSkillPhysicsDamageRand,
        pCaster->m_nSkillPhysicsDamagePercent,
        pCaster->m_szName, pCaster->m_dwID
    );
    
    nRandValue = g_Random(pCaster->m_nSkillSolarDamageRand + 1);
    nSkillSolarDamage = (pCaster->m_nSkillSolarDamage + nRandValue) * (KILO_NUM + pCaster->m_nSkillSolarDamagePercent) / KILO_NUM;
    nSolarDamage = nSkillSolarDamage + nSolarAP * nSkillCoefficient / MAGIC_AP_TO_DPS / GAME_FPS;

    pCaster->m_SkillDebug.Trace(
        2, "[技能调试][%d]:[伤害计算] 阳性内功伤害计算:%d AP伤害:%d*%d/12 + 技能伤害:%d(%d-%d) * %d/1024 From(%s-%u) ",
        g_pSO3World->m_nGameLoop, nSolarDamage,
        nSolarAP, nSkillCoefficient / GAME_FPS,
        nSkillSolarDamage, pCaster->m_nSkillSolarDamage, pCaster->m_nSkillSolarDamage + pCaster->m_nSkillSolarDamageRand,
        pCaster->m_nSkillSolarDamagePercent,
        pCaster->m_szName, pCaster->m_dwID
    );

    nRandValue = g_Random(pCaster->m_nSkillNeutralDamageRand + 1);
    nSkillNeutralDamage = (pCaster->m_nSkillNeutralDamage + nRandValue) * (KILO_NUM + pCaster->m_nSkillNeutralDamagePercent) / KILO_NUM;
    nNeutralDamage = nSkillNeutralDamage + nNeutralAP * nSkillCoefficient / MAGIC_AP_TO_DPS / GAME_FPS;

    pCaster->m_SkillDebug.Trace(
        2, "[技能调试][%d]:[伤害计算] 中性内功伤害计算:%d AP伤害:%d*%d/12 + 技能伤害:%d(%d-%d) * %d/1024 From(%s-%u) ",
        g_pSO3World->m_nGameLoop, nNeutralDamage,
        nNeutralAP, nSkillCoefficient / GAME_FPS,
        nSkillNeutralDamage, pCaster->m_nSkillNeutralDamage, pCaster->m_nSkillNeutralDamage + pCaster->m_nSkillNeutralDamageRand,
        pCaster->m_nSkillNeutralDamagePercent,
        pCaster->m_szName, pCaster->m_dwID
    );

    nRandValue = g_Random(pCaster->m_nSkillLunarDamageRand + 1);
    nSkillLunarDamage = (pCaster->m_nSkillLunarDamage + nRandValue) * (KILO_NUM + pCaster->m_nSkillLunarDamagePercent) / KILO_NUM;
    nLunarDamage = nSkillLunarDamage + nLunarAP * nSkillCoefficient / MAGIC_AP_TO_DPS / GAME_FPS;

    pCaster->m_SkillDebug.Trace(
        2, "[技能调试][%d]:[伤害计算] 阴性内功伤害计算:%d AP伤害:%d*%d/12 + 技能伤害:%d(%d-%d) * %d/1024 From(%s-%u) ",
        g_pSO3World->m_nGameLoop, nLunarDamage,
        nLunarAP, nSkillCoefficient / GAME_FPS,
        nSkillLunarDamage, pCaster->m_nSkillLunarDamage, pCaster->m_nSkillLunarDamage + pCaster->m_nSkillLunarDamageRand,
        pCaster->m_nSkillLunarDamagePercent,
        pCaster->m_szName, pCaster->m_dwID
    );

    nRandValue = g_Random(pCaster->m_nSkillPoisonDamageRand + 1);
    nSkillPosionDamage = (pCaster->m_nSkillPoisonDamage + nRandValue) * (KILO_NUM + pCaster->m_nSkillPoisonDamagePercent) / KILO_NUM;
    nPoisonDamage = nSkillPosionDamage + nPoisonAP * nSkillCoefficient / MAGIC_AP_TO_DPS / GAME_FPS;

    pCaster->m_SkillDebug.Trace(
        2, "[技能调试][%d]:[伤害计算] 毒性伤害计算:%d AP伤害:%d*%d/12 + 技能伤害:%d(%d-%d) * %d/1024 From(%s-%u) ",
        g_pSO3World->m_nGameLoop, nPoisonDamage,
        nPoisonDamage, nSkillCoefficient / GAME_FPS,
        nSkillPosionDamage, pCaster->m_nSkillPoisonDamage, pCaster->m_nSkillPoisonDamage + pCaster->m_nSkillPoisonDamageRand,
        pCaster->m_nSkillPoisonDamagePercent,
        pCaster->m_szName, pCaster->m_dwID
    );

    nRandValue = g_Random(pCaster->m_nSkillTherapyRand + 1);
    nTherapy = 
        (pCaster->m_nSkillTherapy + nRandValue) * (KILO_NUM + pCaster->m_nSkillTherapyPercent) / KILO_NUM + 
        pCaster->m_nTherapyPower * nSkillCoefficient / THERAPY_AP_TO_DPS / GAME_FPS;

    pBullet->nPhysicsDamage             = nPhysicsDamage;
    pBullet->nSolarDamage               = nSolarDamage;  
    pBullet->nNeutralDamage             = nNeutralDamage;   
    pBullet->nLunarDamage               = nLunarDamage;
    pBullet->nPoisonDamage              = nPoisonDamage; 
    pBullet->nTherapy	                = nTherapy; 
    
    pBullet->nDotPhysicsAP              = nPhysicsAP;
    pBullet->nDotLunarAP                = nLunarAP;
    pBullet->nDotSolarAP                = nSolarAP;
    pBullet->nDotNeutralAP              = nNeutralAP;
    pBullet->nDotPoisonAP               = nPoisonAP;
    pBullet->nDotTherapyAP              = pCaster->m_nTherapyPower;

    pBullet->nDamageToLifeForSelf       = pCaster->m_nDamageToLifeForSelf; 
    pBullet->nDamageToManaForSelf       = pCaster->m_nDamageToManaForSelf;
    
    pBullet->nBaseThreatCoefficient     = pCaster->m_nActiveBaseThreatCoefficient;
    pBullet->nDamageThreatCoefficient   = pCaster->m_nActiveDamageThreatCoefficient;
    pBullet->nTherapyThreatCoefficient  = pCaster->m_nActiveTherapyThreatCoefficient;
    // 拷贝技能释放者身上的技能事件表 
    pBullet->SkillEventVector           = pCaster->m_SkillEventVector;

    nDenominator = pCaster->m_nPhysicsHitValue + 95 * pCaster->m_nLevel;
    KGLOG_PROCESS_ERROR(nDenominator > 0);
    pBullet->nPhysicsHitValue =
        pCaster->m_nPhysicsHitBaseRate + MAX_HIT_VALUE * pCaster->m_nPhysicsHitValue / nDenominator;

    nDenominator = pCaster->m_nSolarHitValue + 95 * pCaster->m_nLevel;
    KGLOG_PROCESS_ERROR(nDenominator > 0);
    pBullet->nSolarHitValue =
        pCaster->m_nSolarHitBaseRate + MAX_HIT_VALUE * pCaster->m_nSolarHitValue / nDenominator;

    nDenominator = pCaster->m_nLunarHitValue + 95 * pCaster->m_nLevel;
    KGLOG_PROCESS_ERROR(nDenominator > 0);
    pBullet->nLunarHitValue = 
        pCaster->m_nLunarHitBaseRate + MAX_HIT_VALUE * pCaster->m_nLunarHitValue / nDenominator;

    nDenominator = pCaster->m_nNeutralHitValue + 95 * pCaster->m_nLevel;
    KGLOG_PROCESS_ERROR(nDenominator > 0);
    pBullet->nNeutralHitValue =
        pCaster->m_nNeutralHitBaseRate + MAX_HIT_VALUE * pCaster->m_nNeutralHitValue / nDenominator;

    nDenominator = pCaster->m_nPoisonHitValue + 95 * pCaster->m_nLevel;
    KGLOG_PROCESS_ERROR(nDenominator > 0);
    pBullet->nPoisonHitValue  = 
        pCaster->m_nPoisonHitBaseRate + MAX_HIT_VALUE * pCaster->m_nPoisonHitValue / nDenominator;

    switch (m_pBaseInfo->nKindType)
    {
    case sktPhysics:
        nDenominator = pCaster->m_nPhysicsCriticalStrike + 74 * pCaster->m_nLevel + 320;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        pBullet->nCriticalStrike = 
            pCaster->m_nPhysicsCriticalStrikeBaseRate +
            MAX_HIT_VALUE * pCaster->m_nPhysicsCriticalStrike / nDenominator;
        break;
    case sktSolarMagic:
        nDenominator = pCaster->m_nSolarCriticalStrike + 74 * pCaster->m_nLevel + 320;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        pBullet->nCriticalStrike = 
            pCaster->m_nSolarCriticalStrikeBaseRate +
            MAX_HIT_VALUE * pCaster->m_nSolarCriticalStrike / nDenominator;
        break;
    case sktNeutralMagic:
        nDenominator = pCaster->m_nNeutralCriticalStrike + 74 * pCaster->m_nLevel + 320;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        pBullet->nCriticalStrike =
            pCaster->m_nNeutralCriticalStrikeBaseRate +
            MAX_HIT_VALUE * pCaster->m_nNeutralCriticalStrike / nDenominator;
        break;
    case sktLunarMagic:
        nDenominator = pCaster->m_nLunarCriticalStrike + 74 * pCaster->m_nLevel + 320;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        pBullet->nCriticalStrike =
            pCaster->m_nLunarCriticalStrikeBaseRate +
            MAX_HIT_VALUE * pCaster->m_nLunarCriticalStrike / nDenominator;
        break;
    case sktPoison:
        nDenominator = pCaster->m_nPoisonCriticalStrike + 74 * pCaster->m_nLevel + 320;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        pBullet->nCriticalStrike =
            pCaster->m_nPoisonCriticalStrikeBaseRate +
            MAX_HIT_VALUE * pCaster->m_nPoisonCriticalStrike / nDenominator;
        break;
    case sktNone:
        break;
    default:
        goto Exit0;
    }
    
    switch (m_pBaseInfo->nCastMode)
    {
    case scmSector:
    case scmCasterArea:
    case scmCasterSingle:
    case scmPartyArea:
        pBullet->nEndFrame           = 0;
        goto Exit1;
        break;
    case scmTargetArea:
    case scmTargetSingle: 
    case scmTargetChain:
    case scmPointArea:
    case scmPoint:
    case scmTargetLeader:
        break;
    default:
        KG_ASSERT_EXIT(!"Unexpected cast mode !");
    }

    // 取得Caster坐标
    nRetCode = pCaster->GetAbsoluteCoordinate(&nCasterX, &nCasterY, &nCasterZ);
    KG_PROCESS_ERROR(nRetCode);

    nCasterZ = ZPOINT_TO_XYPOINT(nCasterZ);

    // 取得Target坐标
    nRetCode = rTarget.GetTarget(&nTargetX, &nTargetY, &nTargetZ);
    KG_PROCESS_ERROR(nRetCode);

    nTargetZ = ZPOINT_TO_XYPOINT(nTargetZ);
    
    nRetCode = rTarget.GetTarget(&pTarget);
    if (pTarget)
    {
        nTouchRange = pTarget->m_nTouchRange;
    }

    // 计算子弹飞行时间
    nDistance = g_GetDistance3(
        nCasterX, nCasterY, nCasterZ, 
        nTargetX, nTargetY, nTargetZ
    );
    nDistance = (int)sqrtl((long double)nDistance);

    if (nDistance > nTouchRange)
    {
        nDistance -= nTouchRange;
    }

    if (m_nBulletVelocity > 0)
    {
        nHitDelay = (int)(nDistance / m_nBulletVelocity);
    }

    pBullet->nEndFrame = g_pSO3World->m_nGameLoop + nHitDelay;

Exit1:
	bResult = true;
Exit0:
    if (bRollbackAttrib)
    {
        pCaster->UnApplyAttribute(m_pAttrbuteEffectToSelfAndRollback);
    }
	return bResult;
}


SKILL_RESULT_CODE KSkill::ApplyOnSector(KSKILL_BULLET* pBullet)
{
	SKILL_RESULT_CODE                   nResult         = srcFailed;
    KScene*                             pScene          = NULL;
    KCharacter*                         pCaster         = NULL;
    KSkill*                             pSkill          = NULL;
    KSkillSectorTravFunc                SectorTravFunc;

    pCaster = pBullet->pSkillSrc;
    KG_ASSERT_EXIT(pCaster);

    pSkill = pBullet->SkillRecipePointer.GetPointer();
    KG_ASSERT_EXIT(pSkill);

    SectorTravFunc.pBullet      = pBullet;
    SectorTravFunc.nLeftCount   = pSkill->m_nTargetCountLimit;

    pScene = pCaster->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    pScene->TraverseRangePlayer(pCaster->m_pRegion, SectorTravFunc);
    pScene->TraverseRangeNpc(pCaster->m_pRegion, SectorTravFunc);
    pScene->TraverseRangeDoodad(pCaster->m_pRegion, SectorTravFunc);
                         
	nResult = srcSuccess;
Exit0:
	return nResult;
}

SKILL_RESULT_CODE KSkill::ApplyOnArea(KSKILL_BULLET* pBullet, int nX, int nY)
{
	SKILL_RESULT_CODE                   nResult         = srcFailed;
    KSkillAreaTravFunc                  AreaTravFunc;
    KScene*                             pScene          = NULL;
    KRegion*                            pRegion         = NULL;
    KCharacter*                         pCaster         = NULL;
    KSkill*                             pSkill          = NULL;
    
    pSkill = pBullet->SkillRecipePointer.GetPointer();
    KG_ASSERT_EXIT(pSkill);

    pCaster = pBullet->pSkillSrc;
    KG_ASSERT_EXIT(pCaster);
    KG_PROCESS_ERROR(pCaster->m_dwID == pBullet->dwSkillSrcID);

    pScene  = pCaster->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    pRegion = pScene->GetRegionByPoint(nX, nY);
    KGLOG_PROCESS_ERROR(pRegion);

    AreaTravFunc.pBullet     = pBullet;
    AreaTravFunc.nLeftCount  = pSkill->m_nTargetCountLimit;

    if (m_pBaseInfo->nCastMode == scmTargetArea)
    {
        KSceneObject* pObject = NULL;
        pBullet->Target.GetTarget(&pObject);
        KGLOG_PROCESS_ERROR(pObject);

        AreaTravFunc.bTargetArea = true;
        AreaTravFunc.nTargetType = pBullet->Target.GetTargetType();
        AreaTravFunc.dwTargetID  = pObject->m_dwID;
        AreaTravFunc.nLeftCount  -= 1;  // 选中的Target已作用，所以数量要减1
    }
    else
    {
        AreaTravFunc.bTargetArea = false;
        AreaTravFunc.nTargetType = ttInvalid;
        AreaTravFunc.dwTargetID  = 0;
    }

    pScene->TraverseRangePlayer(pRegion, AreaTravFunc);
    pScene->TraverseRangeNpc(pRegion, AreaTravFunc);
    pScene->TraverseRangeDoodad(pRegion, AreaTravFunc);

	nResult = srcSuccess;
Exit0:
	return nResult;
}

SKILL_RESULT_CODE KSkill::ApplyOnSingle(KSKILL_BULLET* pBullet)
{
    SKILL_RESULT_CODE nResult       = srcFailed;
    BOOL              bRetCode      = false;
    TARGET_TYPE       eTargetType   = ttInvalid;
    KCharacter*       pCharacter    = NULL;
    KDoodad*          pDoodad       = NULL;

    assert(pBullet);
    eTargetType = pBullet->Target.GetTargetType();

    switch (eTargetType)
    {
	case ttCoordination:
        // Do nothing !
        // 对这种单体技能,允许玩家对地上的一点施放也是合理的,当然,实际上不会有任何效果,除非是拿来Show一下
		break;
    case ttNpc:
    case ttPlayer:
        bRetCode = pBullet->Target.GetTarget(&pCharacter);
        KG_PROCESS_ERROR(bRetCode && pCharacter);
        
        pCharacter->ApplyBullet(pBullet);
        break;

    case ttDoodad:
        bRetCode = pBullet->Target.GetTarget(&pDoodad);
        KG_PROCESS_ERROR(bRetCode && pDoodad);

        // pDoodad->ApplyBullet(pBullet);
        break;

    default:
        KG_ASSERT_EXIT(false);
    }

    nResult = srcSuccess;
Exit0:
    return nResult;
}

SKILL_RESULT_CODE KSkill::ApplyOnPoint(KSKILL_BULLET* pBullet)
{
    return srcSuccess;
}

SKILL_RESULT_CODE KSkill::ApplyOnChain(KSKILL_BULLET* pBullet)
{
    SKILL_RESULT_CODE                           nResult         = srcFailed;
    int                                         nRetCode        = 0;
    int                                         nCurrentCount   = 0;
    KCharacter*                                 pCaster         = NULL;
    KCharacter*                                 pCharacter      = NULL;
    KScene*                                     pScene          = NULL;
    KSKILL_BULLET*                              pBulletSub      = NULL;
    FindTarget                                  FindFunc;
    KTarget                                     SkillTarget;
    std::vector<KSKILL_CHAIN_TARGET>::iterator  it;
    std::vector<KSKILL_CHAIN_TARGET>::iterator  itEnd;

    pCaster = pBullet->pSkillSrc;
    assert(pCaster);

    nRetCode = pBullet->Target.GetTarget(&pCharacter);
    KGLOG_PROCESS_ERROR(nRetCode);

    ApplyOnSingle(pBullet);

    // 计算闪电链反弹

    FindFunc.AttackArray.reserve(MAX_CHAIN_TARGET + 1);
    FindFunc.AttackArray.push_back(KSKILL_CHAIN_TARGET(pCharacter, 0));
    FindFunc.pSrcSkill  = this;
    FindFunc.uAttackNum = MAX_CHAIN_TARGET;
    FindFunc.pCaster    = pCaster;
    FindFunc.nRadius    = m_nAreaRadius;

    pScene = pCharacter->m_pScene;
    KGLOG_PROCESS_ERROR(pScene);

    pScene->TraverseRangePlayer(pCharacter->m_pRegion, FindFunc);
    pScene->TraverseRangeNpc(pCharacter->m_pRegion, FindFunc);

    g_PlayerServer.DoSkillChainEffect(
        pCaster, m_pBaseInfo->dwSkillID, m_dwLevel,
        FindFunc.AttackArray.size(), &FindFunc.AttackArray[0]
    );

    nCurrentCount   = m_nChainBranch;
    it              = FindFunc.AttackArray.begin() + 1; // 闪电链的开头并不受传递的伤害。
    itEnd           = FindFunc.AttackArray.end();
    for (int i = 0; i < m_nChainDepth && it != itEnd; i++)
    {
        KSKILL_RECIPE_KEY   RecipeKey;

        nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, m_SubsectionSkill[i].dwSubSkillID, m_SubsectionSkill[i].dwSubSkillLevel, pCaster);
        KGLOG_PROCESS_ERROR(nRetCode);

        for (int j = 0; j < nCurrentCount && it != itEnd; j++, ++it)
        {
            if (pBulletSub)
            {
                KMemory::Delete(pBulletSub);
                pBulletSub = NULL;
            }

            SkillTarget.SetTarget(it->pCharacter);

            pBulletSub = KMemory::New<KSKILL_BULLET>();
            KGLOG_PROCESS_ERROR(pBulletSub);
            
            nRetCode = SetupBullet(pBulletSub, pCaster, SkillTarget, RecipeKey);
            KGLOG_PROCESS_ERROR(nRetCode);
            
            ApplyOnSingle(pBulletSub);
        }

        nCurrentCount *= m_nChainBranch;
    }

    nResult = srcSuccess;
Exit0:

    if (pBulletSub)
    {
        KMemory::Delete(pBulletSub);
        pBulletSub = NULL;
    }
    return nResult;
}

BOOL KSkillSectorTravFunc::CanApply(KTarget& rTarget)
{
    BOOL            bResult     = false;
	int             nRetCode    = false;
    KSkill*         pSkill      = NULL;
    KCharacter*     pCaster     = NULL;

    pSkill = pBullet->SkillRecipePointer.GetPointer();
    KG_ASSERT_EXIT(pSkill);

    pCaster = pBullet->pSkillSrc;
    KG_ASSERT_EXIT(pCaster);
    KG_PROCESS_ERROR(pCaster->m_dwID == pBullet->dwSkillSrcID);

    KGLOG_PROCESS_ERROR(pCaster->m_pScene);

    nRetCode = pSkill->CheckTargetType(pCaster, rTarget);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    nRetCode = pSkill->CheckTargetRange(pCaster, rTarget);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    nRetCode = pSkill->CheckTargetState(rTarget);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    if (pSkill->m_pBaseInfo->bCheckBindBuff)
    {
        nRetCode = pSkill->CheckTargetBuffList(pCaster, rTarget);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);
    }

    nRetCode = pSkill->CheckTargetBuff(rTarget);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KSkillSectorTravFunc::operator()(KCharacter* pCharacter)
{
	BOOL            bRetCode    = false;
    KTarget         Target;

    if (nLeftCount == 0)
    {
        return false;
    }
    
    bRetCode = Target.SetTarget(pCharacter);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CanApply(Target);
    KG_PROCESS_ERROR(bRetCode);

    nLeftCount--;

    pCharacter->ApplyBullet(pBullet);

Exit0:
    return true;
}

BOOL KSkillSectorTravFunc::operator()(KDoodad *pDoodad)
{
	BOOL            bRetCode    = false;
    KTarget         Target;

    if (nLeftCount == 0)
    {
        return false;
    }

    bRetCode = Target.SetTarget(pDoodad);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CanApply(Target);
    KG_PROCESS_ERROR(bRetCode);

    nLeftCount--;

    //pDoodad->ApplyBullet(pBullet);
Exit0:
    return true;
}

BOOL KSkillAreaTravFunc::CanApply(KTarget& rTarget)
{
    BOOL            bResult         = false;
	int             nRetCode        = false;
    KSkill*         pSkill          = NULL;
    KCharacter*     pCaster         = NULL;
    KSceneObject*   pCenterObj      = NULL; // 圆心对象

    // 为了便于X,Y,Z三轴联合算距离,所有尺度单位都统一为水平方向的"点"
    int             nSrcX           = 0;
    int             nSrcY           = 0;
    int             nSrcZ           = 0;
    KSceneObject*   pTargetObj      = NULL; // Target对象
    int             nTouchRange     = 0;
    int             nDstX           = 0;
    int             nDstY           = 0;
    int             nDstZ           = 0;
    int             nDeltaX         = 0;    // 技能释放点与目标点连线和圆的交点在X轴上的投影(XY平面)        
    int             nDeltaZ         = 0;
    int             nDistance       = 0;
    int             nAttackRange    = 0;
    int             nDirectionSub   = 0;

    pSkill = pBullet->SkillRecipePointer.GetPointer();
    KG_ASSERT_EXIT(pSkill);

    pCaster = pBullet->pSkillSrc;
    KG_ASSERT_EXIT(pCaster);
    KG_PROCESS_ERROR(pCaster->m_dwID == pBullet->dwSkillSrcID);

    KGLOG_PROCESS_ERROR(pCaster->m_pScene);

    nRetCode = pSkill->CheckTargetType(pCaster, rTarget);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    nRetCode = pSkill->CheckTargetState(rTarget);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    if (pSkill->m_pBaseInfo->bCheckBindBuff)
    {
        nRetCode = pSkill->CheckTargetBuffList(pCaster, rTarget);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);
    }

    nRetCode = pSkill->CheckTargetBuff(rTarget);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    // -------------------- 获取圆心坐标 --------------------------------------

    // 注意,对这种技能,Bullet的Target仅仅是圆心坐标,并不是真正的技能作用目标

    nRetCode = pBullet->Target.GetTarget(&nSrcX, &nSrcY, &nSrcZ);
    KGLOG_PROCESS_ERROR(nRetCode);

    nSrcZ = ZPOINT_TO_XYPOINT(nSrcZ);

    nRetCode = pBullet->Target.GetTarget(&pCenterObj);
    if (pCenterObj)
    {
        nSrcZ += ZPOINT_TO_XYPOINT(pCenterObj->m_nHeight);
    }

    // ------------------------ 获取目标对象坐标及其受击范围 -------------------

    // 在这里, 目标肯定是场景物体, 不可能是坐标;
    nRetCode = rTarget.GetTarget(&pTargetObj);
    KGLOG_PROCESS_ERROR(nRetCode && pTargetObj);

    KG_ASSERT_EXIT(pTargetObj);
    KG_PROCESS_ERROR(pTargetObj->m_pRegion);
    KG_PROCESS_ERROR(pTargetObj->m_pCell);

    nRetCode = pTargetObj->GetAbsoluteCoordinate(&nDstX, &nDstY, &nDstZ);
    KG_PROCESS_ERROR(nRetCode);

    nDstZ = ZPOINT_TO_XYPOINT(nDstZ + pTargetObj->m_nHeight);

    nTouchRange = pTargetObj->m_nTouchRange;

    nAttackRange  = pSkill->m_nAreaRadius + nTouchRange;
    nRetCode = SkillInRange(nSrcX, nSrcY, nSrcZ, nDstX, nDstY, nDstZ, nAttackRange);
    KG_PROCESS_ERROR(nRetCode);

    // -------------- 3D障碍判断 --------------------------------------
    if (pSkill->m_pBaseInfo->bUse3DObstacle)
    {
        nSrcZ = ZPOINT_TO_ALTITUDE(XYPOINT_TO_ZPOINT(nSrcZ));
        nDstZ = ZPOINT_TO_ALTITUDE(XYPOINT_TO_ZPOINT(nDstZ));

        KScene* pScene = pCaster->m_pScene;
        nRetCode = pScene->IsVisible(nSrcX, nSrcY, nSrcZ, nDstX, nDstY, nDstZ);
        KG_PROCESS_ERROR(nRetCode);
    }

	bResult = true;
Exit0:
	return bResult;
}


BOOL KSkillAreaTravFunc::operator()(KCharacter* pCharacter)
{
	BOOL            bRetCode    = false;
    KTarget         Target;

    if (nLeftCount == 0)
    {
        return false;
    }

    if (!bTargetArea || dwTargetID != pCharacter->m_dwID)
    {
        bRetCode = Target.SetTarget(pCharacter);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = CanApply(Target);
        KG_PROCESS_ERROR(bRetCode);

        nLeftCount--;

        pCharacter->ApplyBullet(pBullet);
    }    

Exit0:
    return true;
}

BOOL KSkillAreaTravFunc::operator()(KDoodad* pDoodad)
{
	BOOL            bRetCode    = false;
    KTarget         Target;

    if (nLeftCount == 0)
    {
        return false;
    }

    bRetCode = Target.SetTarget(pDoodad);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = CanApply(Target);
    KG_PROCESS_ERROR(bRetCode);

    nLeftCount--;

    // TODO: 对物体
    //pDoodad->ApplyBullet(&Bullet);
Exit0:
    return true;
}

#endif



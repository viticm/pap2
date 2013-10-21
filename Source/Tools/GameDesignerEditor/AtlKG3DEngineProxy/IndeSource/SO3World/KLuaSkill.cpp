#include "stdafx.h"
#include <vector>
#include <algorithm>
#include "KAttribute.h"
#include "KSkill.h"
#include "KMath.h"
#include "KItemLib.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "Engine/KMemory.h"
#include "KBuff.h"
#include "KScene.h"
using namespace std;

int KSkill::LuaAddAttribute(Lua_State* L)
{
    BOOL            bResult                     = false;
    int             nRetCode                    = 0;
    int             nAttributeEffectMode        = 0;
    int             nAttributeKey               = 0;
    int             nAttributeValue1            = 0;
    int             nAttributeValue2            = 0;
    KAttribute*     pAttribute                  = NULL;

    assert(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);

    nAttributeEffectMode = (int)Lua_ValueToNumber(L, 1);
    nAttributeKey        = (int)Lua_ValueToNumber(L, 2);

    if (nAttributeKey == atExecuteScript)
    {
        const char* pszFileName = NULL;
        char szFileFullPath[MAX_PATH];

        pszFileName = Lua_ValueToString(L, 3);
        KGLOG_PROCESS_ERROR(pszFileName);

        snprintf(szFileFullPath, sizeof(szFileFullPath), "%s/%s", SCRIPT_DIR, pszFileName);
        szFileFullPath[sizeof(szFileFullPath) - 1] = '\0';

        nAttributeValue1 = (int)g_FileNameHash(szFileFullPath);
    }
    else
    {
        nAttributeValue1 = (int)Lua_ValueToNumber(L, 3);
    }

    nAttributeValue2     = (int)Lua_ValueToNumber(L, 4);

    KGLOG_PROCESS_ERROR((nAttributeEffectMode > aemInvalid) && (nAttributeEffectMode < aemTotal));
    KGLOG_PROCESS_ERROR((nAttributeKey > atInvalid) && (nAttributeKey < atTotal));

    pAttribute = KMemory::New<KAttribute>();
    KGLOG_PROCESS_ERROR(pAttribute);

    pAttribute->nKey    = nAttributeKey;
    pAttribute->nValue1 = nAttributeValue1;
    pAttribute->nValue2 = nAttributeValue2;
    pAttribute->pNext   = NULL;

    switch (nAttributeEffectMode)
    {
    case aemEffectToSelfAndRollback:
        ADD_ATTRIBUTE_TO_END(m_pAttrbuteEffectToSelfAndRollback, pAttribute);
        break;
    case aemEffectToSelfNotRollback:
        ADD_ATTRIBUTE_TO_END(m_pAttrbuteEffectToSelfNotRollback, pAttribute);
        break;
    case aemEffectToDestAndRollback:
        ADD_ATTRIBUTE_TO_END(m_pAttrbuteEffectToDestAndRollback, pAttribute);
        break;
    case aemEffectToDestNotRollback:
        ADD_ATTRIBUTE_TO_END(m_pAttrbuteEffectToDestNotRollback, pAttribute);
        break;
    case aemEffectToDestOnMinorWeak:
        ADD_ATTRIBUTE_TO_END(m_pAttrbuteEffectToDestOnMinorWeak, pAttribute);
        break;
    case aemEffectToDestOnMajorWeak:
        ADD_ATTRIBUTE_TO_END(m_pAttrbuteEffectToDestOnMajorWeak, pAttribute);
        break;
    case aemEffectToDestOnMaxWeak:
        ADD_ATTRIBUTE_TO_END(m_pAttrbuteEffectToDestOnMaxWeak, pAttribute);
        break;
    default:
        KG_ASSERT_EXIT(!"Unexpected skill attribute effect mode !\n");
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        KGLogPrintf(
            KGLOG_ERR, 
            "[Skill] Add skill magic attribute failed, Skill(\"%s\", %u), Script(\"%s\")\n",
            m_pBaseInfo->szSkillName, m_dwLevel, m_pBaseInfo->szScriptFile
        );

        if (pAttribute)
        {
             KMemory::Delete(pAttribute);
             pAttribute = NULL;
        }
    }

    Lua_PushBoolean(L, bResult);
    return 1;
}

int KSkill::LuaAddSlowCheckSelfBuff(Lua_State* L)
{
    BOOL                        bResult     = false;
    int                         nTopIndex   = 0;
    KSKILL_REQUIRE_BUFF_NODE    CheckBuff;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 5);

    CheckBuff.dwBuffID          = (DWORD)Lua_ValueToNumber(L, 1);
    CheckBuff.nStackNum         = (int)Lua_ValueToNumber(L, 2);
    CheckBuff.eStackCompareFlag = (KBUFF_COMPARE_FLAG)(int)Lua_ValueToNumber(L, 3);
    CheckBuff.nBuffLevel        = (int)Lua_ValueToNumber(L, 4);
    CheckBuff.eLevelCompareFlag = (KBUFF_COMPARE_FLAG)(int)Lua_ValueToNumber(L, 5);

    m_SelfRequireBuffVector.push_back(CheckBuff);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}

int KSkill::LuaAddSlowCheckDestBuff(Lua_State* L)
{
    BOOL                        bResult     = false;
    int                         nTopIndex   = 0;
    KSKILL_REQUIRE_BUFF_NODE    CheckBuff;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 5);

    CheckBuff.dwBuffID          = (DWORD)Lua_ValueToNumber(L, 1);
    CheckBuff.nStackNum         = (int)Lua_ValueToNumber(L, 2);
    CheckBuff.eStackCompareFlag = (KBUFF_COMPARE_FLAG)(int)Lua_ValueToNumber(L, 3);
    CheckBuff.nBuffLevel        = (int)Lua_ValueToNumber(L, 4);
    CheckBuff.eLevelCompareFlag = (KBUFF_COMPARE_FLAG)(int)Lua_ValueToNumber(L, 5);

    m_DestRequireBuffVector.push_back(CheckBuff);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}

int KSkill::LuaBindBuff(Lua_State* L)
{
    BOOL                bResult     = false;
    int                 nRetCode    = 0;
    DWORD               dwID        = 0;
    int                 nLevel      = 0;
    int                 nIndex      = 0;
    KBuff*              pBuffInfo   = NULL;
    KBUFF_RECIPE_KEY    BuffRecipeKey;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    nIndex  = (int)Lua_ValueToNumber(L, 1);
    dwID    = (DWORD)Lua_ValueToNumber(L, 2);
    nLevel  = (int)Lua_ValueToNumber(L, 3);

    KGLOG_PROCESS_ERROR(nIndex > 0 && nIndex <= MAX_SKILL_BIND_BUFF_COUNT);

    BuffRecipeKey.dwID = dwID;
    BuffRecipeKey.nLevel = nLevel;
    memset(BuffRecipeKey.dwRecipeKey, 0, sizeof(BuffRecipeKey.dwRecipeKey));

    pBuffInfo = m_BindBuffs[nIndex - 1].SetRecipeKey(BuffRecipeKey);
    if (!pBuffInfo)
    {
        KGLogPrintf(
            KGLOG_ERR, "[Skill] Skill(%u, %s) error: invalid buff(%u, %d) in LuaBindBuff.\n",
            m_pBaseInfo->dwSkillID, m_pBaseInfo->szSkillName, dwID, nLevel
        );
        goto Exit0;
    }
    
    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KSkill::LuaSetBuffRecipe(Lua_State* L)
{
    BOOL                bResult         = false;
    int                 nRetCode        = 0;
    DWORD               dwRecipeID      = 0;
    int                 nLevel          = 0;
    int                 nIndex          = 0;
    DWORD               dwRecipeKey     = 0;
    KBuff*              pBuffInfo       = NULL;
    KBUFF_RECIPE_INFO*  pBuffRecipeInfo = NULL;
    KBUFF_RECIPE_KEY    BuffRecipeKey;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    nIndex      = (int)Lua_ValueToNumber(L, 1);
    dwRecipeID  = (DWORD)Lua_ValueToNumber(L, 2);
    nLevel      = (int)Lua_ValueToNumber(L, 3);

    KGLOG_PROCESS_ERROR(nIndex > 0 && nIndex <= MAX_SKILL_BIND_BUFF_COUNT);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, nLevel);

    pBuffRecipeInfo = g_pSO3World->m_BuffManager.GetBuffRecipeInfo(dwRecipeKey);
    KGLOG_PROCESS_ERROR(pBuffRecipeInfo);

    BuffRecipeKey = m_BindBuffs[nIndex - 1].GetRecipeKey();

    KGLOG_PROCESS_ERROR(BuffRecipeKey.dwID);
    KGLOG_PROCESS_ERROR(BuffRecipeKey.nLevel);

    KGLOG_PROCESS_ERROR(BuffRecipeKey.dwID == pBuffRecipeInfo->dwID);
    KGLOG_PROCESS_ERROR(BuffRecipeKey.nLevel == pBuffRecipeInfo->nLevel || !pBuffRecipeInfo->nLevel);

    BuffRecipeKey.dwRecipeKey[0] = dwRecipeKey;

    pBuffInfo = m_BindBuffs[nIndex - 1].SetRecipeKey(BuffRecipeKey);
    KGLOG_PROCESS_ERROR(pBuffInfo);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KSkill::LuaSetPublicCoolDown(Lua_State* L)
{
    BOOL    bResult         = false;
    DWORD   dwCoolDownID    = 0;
    int     nTopIndex       = 0;

    assert(L);

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwCoolDownID = (DWORD)Lua_ValueToNumber(L, 1);

    m_dwPublicCoolDownID = dwCoolDownID;

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KSkill::LuaSetNormalCoolDown(Lua_State* L)
{    
    BOOL                bResult          = false;
    int                 nRetCode         = 0;
    int                 nCoolDownIndex   = 0;
    DWORD               dwCoolDownID     = 0;

    KG_ASSERT_EXIT(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nCoolDownIndex           = (int)Lua_ValueToNumber(L, 1);
    dwCoolDownID             = (DWORD)Lua_ValueToNumber(L, 2);

    // 策划在脚本里面填1, 2, 3,所以减1成C下标
    nCoolDownIndex--;
    KGLOG_PROCESS_ERROR(nCoolDownIndex >= 0);
    KGLOG_PROCESS_ERROR(nCoolDownIndex < MAX_SKILL_COOL_DOWN_TIMER);

    m_dwCoolDownID[nCoolDownIndex] = dwCoolDownID;

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}


int KSkill::LuaGetNormalCooldownCount(Lua_State* L)
{
    Lua_PushNumber(L, MAX_SKILL_COOL_DOWN_TIMER);
    return 1;
}


int KSkill::LuaGetNormalCooldownID(Lua_State* L)
{   
    BOOL                bResult          = false;
    int                 nRetCode         = 0;
    int                 nCoolDownIndex   = 0;
    DWORD               dwCoolDownID     = 0;

    KG_ASSERT_EXIT(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nCoolDownIndex           = (int)Lua_ValueToNumber(L, 1);

    // 策划在脚本里面填1, 2, 3, 4,所以减1成C下标
    nCoolDownIndex--;

    KGLOG_PROCESS_ERROR(nCoolDownIndex >= 0);
    KGLOG_PROCESS_ERROR(nCoolDownIndex < MAX_SKILL_COOL_DOWN_TIMER);

    dwCoolDownID = m_dwCoolDownID[nCoolDownIndex];

    bResult = true;
Exit0:
    Lua_PushNumber(L, dwCoolDownID);
    return 1;
}

int KSkill::LuaGetAttribute(Lua_State* L)
{
    int         nRetCode                = 0;
    int         nIndex                  = 0;
    int         nTable                  = 0;
    int         nAttributeEffectMode    = 0;
    KAttribute* pAttribute              = NULL;
    int         nAttrib                 = 0;
    
    KG_ASSERT_EXIT(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nAttributeEffectMode = (int)Lua_ValueToNumber(L, 1);
    
    switch (nAttributeEffectMode)
    {
    case aemEffectToSelfAndRollback:
        pAttribute = m_pAttrbuteEffectToSelfAndRollback;
        break;
    case aemEffectToSelfNotRollback:
        pAttribute = m_pAttrbuteEffectToSelfNotRollback;
        break;
    case aemEffectToDestAndRollback:
        pAttribute = m_pAttrbuteEffectToDestAndRollback;
        break;
    case aemEffectToDestNotRollback:
        pAttribute = m_pAttrbuteEffectToDestNotRollback;
        break;
    case aemEffectToDestOnMinorWeak:
        pAttribute = m_pAttrbuteEffectToDestOnMinorWeak;
        break;
    case aemEffectToDestOnMajorWeak:
        pAttribute = m_pAttrbuteEffectToDestOnMajorWeak;
        break;
    case aemEffectToDestOnMaxWeak:
        pAttribute = m_pAttrbuteEffectToDestOnMaxWeak;
        break;
    default:
        KG_ASSERT_EXIT(false && "Unexpected skill attribute effect mode !\n");
    }

    Lua_NewTable(L);    
    nTable = Lua_GetTopIndex(L);
	for (NULL; pAttribute; pAttribute = pAttribute->pNext)
	{
		if (pAttribute->nKey != atInvalid)
		{
			++nIndex;

			Lua_PushNumber(L, nIndex);

			Lua_NewTable(L);
			nAttrib = Lua_GetTopIndex(L);

			Lua_PushString(L, "nID");
			Lua_PushNumber(L, pAttribute->nKey);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nValue1");
			Lua_PushNumber(L, pAttribute->nValue1);
			Lua_SetTable(L, nAttrib);

			Lua_PushString(L, "nValue2");
			Lua_PushNumber(L, pAttribute->nValue2);
			Lua_SetTable(L, nAttrib);

			Lua_SetTable(L, nTable);
		}
	}
    
Exit0:
    return 1;
}

int KSkill::LuaGetBuff(Lua_State* L)
{
    int     nRetCode  = 0;
    KBuff*  pBuffInfo = NULL;
    int     nIndex    = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nIndex = (int)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(nIndex > 0 && nIndex <= MAX_SKILL_BIND_BUFF_COUNT);

    pBuffInfo = m_BindBuffs[nIndex - 1].GetPointer();
    KG_PROCESS_ERROR(pBuffInfo);

	return pBuffInfo->LuaGetObj(L);
Exit0:
	return 0;
}

int KSkill::LuaSetSubsectionSkill(Lua_State* L)
{
    int     nResult         = false;
    int     nRetCode        = false;
    int     nBeginInterval  = 0;
    int     nEndInterval    = 0;
    DWORD   dwSkillID       = 0;
    DWORD   dwSkillLevel    = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);

    nBeginInterval  = (int)Lua_ValueToNumber(L, 1);
    nEndInterval    = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nBeginInterval <= nEndInterval);
    KGLOG_PROCESS_ERROR(nBeginInterval > 0);
    KGLOG_PROCESS_ERROR(nEndInterval <= MAX_ACCUMULATE_SUB_COUNT);

    dwSkillID       = (DWORD)Lua_ValueToNumber(L, 3);
    dwSkillLevel    = (DWORD)Lua_ValueToNumber(L, 4);

    for (int i = nBeginInterval; i <= nEndInterval; i++)
    {
        assert(i > 0 && i <= MAX_ACCUMULATE_SUB_COUNT);
        
        m_SubsectionSkill[i - 1].dwSubSkillID    = dwSkillID;
        m_SubsectionSkill[i - 1].dwSubSkillLevel = dwSkillLevel;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "[Skill] SetSubsectionSkill Failed, SkillID = %d", m_pBaseInfo->dwSkillID);
    }
    
    Lua_PushBoolean(L, nResult);
    return 1;
}

#ifdef _CLIENT
int KSkill::LuaUITestCast(Lua_State* L)
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    int         nParamNum   = 0;
    DWORD       dwID        = ERROR_ID;
    BOOL        bIsPlayer   = false;
    KSkill*     pSkill      = this;
    KCharacter* pCaster     = NULL;
    int         nTargetX    = 0;
    int         nTargetY    = 0;
    int         nTargetZ    = 0;
    int         nCostMana   = 0;
    KTarget     Target;

    int         nCurrentLifePercent = 0;

    nParamNum = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamNum == 1 || nParamNum == 4);

    dwID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwID != ERROR_ID);

    if (IS_PLAYER(dwID))
    {
        KSKILL_RECIPE_KEY   SkillRecipeKey;
        KPlayer*            pPlayer         = NULL;

        pCaster = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCaster);

        pPlayer = (KPlayer*)pCaster;

        nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, m_pBaseInfo->dwSkillID, m_dwLevel, pCaster);
        KG_PROCESS_ERROR(nRetCode);

        KG_PROCESS_SUCCESS(m_pBaseInfo->bIsMountable);

        pSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
        if (pSkill == NULL)
        {
            KGLogPrintf(KGLOG_ERR, "UITestCast Skill Invalid. ID=%d, Level=%d.", SkillRecipeKey.dwSkillID, SkillRecipeKey.dwSkillLevel);
            goto Exit0;
        }

        nRetCode = pSkill->CheckWeaponRequest(pPlayer);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);

        nRetCode = pSkill->CheckMountRequest(pPlayer);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);

        nRetCode = pSkill->_LuaCheckCostItemRequest(pPlayer);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);

        switch (m_pBaseInfo->nSelfHorseStateRequest)
        {
        case shcAny:
            break;
        case shcHorse:
            KG_PROCESS_ERROR(pPlayer->m_bOnHorse);
            break;
        case shcNotHorse:
            KG_PROCESS_ERROR(!pPlayer->m_bOnHorse);
            break;
        default:
            KGLogPrintf(
                KGLOG_ERR, "[Skill] Unexpected skill self horse state request %d !\n", 
                m_pBaseInfo->nSelfHorseStateRequest
                );
            goto Exit0;
        }
    }
    else
    {
        pCaster = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCaster);
    }

    if (nParamNum == 1)
    {
        Target = pCaster->m_SelectTarget;
    }

    if (nParamNum == 4)
    {
        nTargetX = (DWORD)Lua_ValueToNumber(L, 2);
        nTargetY = (DWORD)Lua_ValueToNumber(L, 3);
        nTargetZ = (DWORD)Lua_ValueToNumber(L, 4);

        nRetCode = Target.SetTarget(nTargetX, nTargetY, nTargetZ);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    // 取技能上的信息，都需要用pSkill因为可能被秘籍修改
    if (pSkill->m_bIsAccumulate)
    {
        KG_PROCESS_ERROR(pCaster->m_nAccumulateValue > 0);
    }

    if (pSkill->m_pBaseInfo->bNeedOutOfFight)
    {
        KG_PROCESS_ERROR(!pCaster->m_bFightState);
    }

    KGLOG_PROCESS_ERROR(pCaster->m_pScene);
    KG_PROCESS_ERROR(!(pCaster->m_pScene->m_dwBanSkillMask & pSkill->m_pBaseInfo->dwMapBanMask));

    KG_PROCESS_ERROR((pCaster->m_nFearCounter > 0 && pSkill->m_pBaseInfo->bSelfOnFear) || (!pCaster->m_nFearCounter));

    KGLOG_PROCESS_ERROR(pCaster->m_nMaxLife && "MaxLife Equal Zero");
    nCurrentLifePercent = pCaster->m_nCurrentLife * 100 / pCaster->m_nMaxLife;

    MAKE_IN_RANGE(nCurrentLifePercent, 0, 100);

    KG_PROCESS_ERROR(nCurrentLifePercent <= pSkill->m_nSelfLifePercentMax);
    KG_PROCESS_ERROR(nCurrentLifePercent >= pSkill->m_nSelfLifePercentMin);

    nCostMana = pSkill->m_nCostMana * pCaster->m_nModifyCostManaPercent / KILO_NUM;
    nCostMana = max(nCostMana, 0);

    KG_PROCESS_ERROR(pCaster->m_nCurrentLife >= pSkill->m_nCostLife);
    KG_PROCESS_ERROR(pCaster->m_nCurrentMana >= nCostMana);

    nRetCode = pSkill->CheckSelfBuff(pCaster);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    if (pSkill->m_pBaseInfo->dwSkillCastFlag)
    {
        nRetCode = (pSkill->m_pBaseInfo->dwSkillCastFlag & pCaster->m_dwSkillCastFlag);
        KG_PROCESS_ERROR(nRetCode);
    }

    if (pSkill->m_pBaseInfo->nCastMode == scmTargetSingle)
    {
        KG_PROCESS_SUCCESS(Target.m_eTargetType != ttNpc && Target.m_eTargetType != ttPlayer);

        nRetCode = pSkill->_LuaCheckDistance(pCaster, &Target);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);

        nRetCode = pSkill->_LuaCheckAngle(pCaster, &Target);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);

        nRetCode = pSkill->CheckTargetState(Target);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);

        nRetCode = pSkill->CheckTargetType(pCaster, Target);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);

        nRetCode = pSkill->CheckTargetBuff(Target);
        KG_PROCESS_ERROR(nRetCode == srcSuccess);
    }

Exit1:
    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KSkill::LuaCheckWeaponRequest(Lua_State* L)
{
    int         nResult    = srcFailed;
    int         nRetCode   = 0;
    DWORD       dwID       = ERROR_ID;
    KPlayer*    pPlayer    = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwID = (DWORD)Lua_ValueToNumber(L, 1);
    
    // NPC没有装备一说
    KG_PROCESS_SUCCESS(IS_NPC(dwID));

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwID);
    KGLOG_PROCESS_ERROR(pPlayer);
    
    nRetCode = CheckWeaponRequest(pPlayer);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

Exit1:
    nResult = srcSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KSkill::LuaCheckMountRequest(Lua_State* L)
{
    int      nResult    = srcFailed;
    int      nRetCode   = 0;
    DWORD    dwID       = ERROR_ID;
    KPlayer* pPlayer    = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwID = (DWORD)Lua_ValueToNumber(L, 1);
    
    KG_PROCESS_SUCCESS(IS_NPC(dwID));

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nRetCode = CheckMountRequest(pPlayer);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

Exit1:
    nResult = srcSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KSkill::LuaCheckCostItemRequest(Lua_State* L)
{
    int      nResult    = srcFailed;
    int      nRetCode   = 0;
    DWORD    dwID       = ERROR_ID;
    KPlayer* pPlayer    = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_SUCCESS(IS_NPC(dwID));

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nRetCode = _LuaCheckCostItemRequest(pPlayer);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

Exit1:
    nResult = srcSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KSkill::LuaCheckDistance(Lua_State* L)
{   
    int         nResult         = srcFailed;
    int         nRetCode        = 0;
    int         nParamNum       = 0;
    DWORD       dwID            = ERROR_ID;
    KCharacter* pCharacter      = NULL;
    int         nTargetX        = 0;
    int         nTargetY        = 0;
    int         nTargetZ        = 0;
    KTarget     Target;
  
    nParamNum = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamNum == 1 || nParamNum == 4);

    dwID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwID != ERROR_ID);
    
    if (nParamNum == 4)
    {
        nTargetX = (int)Lua_ValueToNumber(L, 2);
        nTargetY = (int)Lua_ValueToNumber(L, 3);
        nTargetZ = (int)Lua_ValueToNumber(L, 4);

        nRetCode = Target.SetTarget(nTargetX, nTargetY, nTargetZ);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (IS_PLAYER(dwID))
    {
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCharacter);
    }
    else
    {
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCharacter);
    }

    if (nParamNum == 1)
    {    
        Target = pCharacter->m_SelectTarget;
    }

    nRetCode = _LuaCheckDistance(pCharacter, &Target);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);    

    nResult = srcSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KSkill::LuaCheckAngle(Lua_State* L)
{
    int         nResult     = srcFailed;
    int         nRetCode    = 0;
    int         nParamNum   = 0;
    DWORD       dwID        = ERROR_ID;
    KCharacter* pCaster     = NULL; 
    int         nTargetX    = 0;
    int         nTargetY    = 0;
    int         nTargetZ    = 0;
    KTarget     Target;

    nParamNum = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamNum == 1 || nParamNum == 4);

    dwID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwID != ERROR_ID);

    if (nParamNum == 4)
    {
        nTargetX = (int)Lua_ValueToNumber(L, 2);
        nTargetY = (int)Lua_ValueToNumber(L, 3);
        nTargetZ = (int)Lua_ValueToNumber(L, 4);
        
        nRetCode = Target.SetTarget(nTargetX, nTargetY, nTargetZ);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    
    if (IS_PLAYER(dwID))
    {
        pCaster = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCaster);
    }
    else
    {
        pCaster = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCaster);
    }

    if (nParamNum == 1)
    {
        Target = pCaster->m_SelectTarget;
    }
    
    nRetCode = _LuaCheckAngle(pCaster, &Target);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);

    nResult = srcSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KSkill::LuaCheckCasterState(Lua_State* L)
{
    int         nResult     = srcFailed;
    int         nRetCode    = 0;
    DWORD       dwID        = ERROR_ID;
    KCharacter* pCaster     = NULL;
    
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwID != ERROR_ID);
 
    if (IS_PLAYER(dwID))
    {
        pCaster = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCaster);
    }
    else
    {
        pCaster = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCaster);
    }

    nRetCode = CheckSelfState(pCaster);
    KG_PROCESS_ERROR(nRetCode == srcSuccess);
        
    nResult = srcSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KSkill::LuaCheckTargetState(Lua_State* L)
{
    int         nResult     = srcFailed;
    int         nRetCode    = 0;
    DWORD       dwID        = ERROR_ID;
    KCharacter* pCaster     = NULL;
    KTarget     Target;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwID != ERROR_ID);

    KG_PROCESS_SUCCESS(m_pBaseInfo->nCastMode == scmPoint || m_pBaseInfo->nCastMode == scmPointArea);

    if (IS_PLAYER(dwID))
    {
        pCaster = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCaster);
    }
    else
    {
        pCaster = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwID);
        KGLOG_PROCESS_ERROR(pCaster);
    }
    
    Target = pCaster->m_SelectTarget;
    nRetCode = CheckTargetState(Target);
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);

Exit1:
    nResult = srcSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KSkill::_LuaCheckCostItemRequest(KPlayer* pPlayer)
{
    int nResult = srcFailed;
    
    KG_PROCESS_ERROR(pPlayer);

    if (m_pBaseInfo->bCostAmmo)
    {
        // TODO..
    }

    if (m_pBaseInfo->nCostItemType != ittInvalid)
    {
        int nItemAmount = pPlayer->m_ItemList.GetItemAmountInEquipAndPackage(m_pBaseInfo->nCostItemType, m_pBaseInfo->nCostItemIndex);
        KG_PROCESS_ERROR(nItemAmount > 0);
    }

    nResult = srcSuccess;
Exit0:
    return nResult;
}

int KSkill::_LuaCheckDistance(KCharacter* pCaster, KTarget* pTarget)
{
    int         nResult         = srcFailed;
    int         nRetCode        = false;
    int         nCasterX        = 0;
    int         nCasterY        = 0;
    int         nCasterZ        = 0;
    int         nTargetX        = 0;
    int         nTargetY        = 0;
    int         nTargetZ        = 0;
    TARGET_TYPE eTargetType     = ttInvalid;
    int         nMinRange       = 0;
    int         nMaxRange       = 0;
    int         nTouchRange     = 0;

    KGLOG_PROCESS_ERROR(pCaster);
    KGLOG_PROCESS_ERROR(pTarget);

    nRetCode = pCaster->GetAbsoluteCoordinate(&nCasterX, &nCasterY, &nCasterZ);
    KGLOG_PROCESS_ERROR(nRetCode);

    nCasterZ = ZPOINT_TO_XYPOINT(nCasterZ);

    nRetCode = pTarget->GetTarget(&nTargetX, &nTargetY, &nTargetZ);
    KG_PROCESS_ERROR(nRetCode);

    nTargetZ = ZPOINT_TO_XYPOINT(nTargetZ);

    eTargetType = pTarget->GetTargetType();
    if (eTargetType == ttNpc || eTargetType == ttPlayer || eTargetType == ttDoodad)
    {
        KSceneObject* pSceneObject = NULL;

        nRetCode = pTarget->GetTarget(&pSceneObject);
        KGLOG_PROCESS_ERROR(nRetCode && pSceneObject);

        nTouchRange = pSceneObject->m_nTouchRange;
    }

    nRetCode = SkillInRange(
        nCasterX, nCasterY, nCasterZ, 
        nTargetX, nTargetY, nTargetZ, 
        m_nMinRadius, 
        pCaster->m_nTouchRange + m_nMaxRadius + nTouchRange
    );
    KG_PROCESS_ERROR_RET_CODE(nRetCode != irrTooClose, srcTooCloseTarget);
    KG_PROCESS_ERROR_RET_CODE(nRetCode != irrTooFar, srcTooFarTarget);

    nResult = srcSuccess;
Exit0:
    return nResult;
}

int KSkill::_LuaCheckAngle(KCharacter* pCaster, KTarget* pTarget)
{
    int     nResult     = srcFailed;
    BOOL    bRetCode    = false;
    int     nCasterX    = 0;
    int     nCasterY    = 0;
    int     nTargetX    = 0;
    int     nTargetY    = 0;
    int     nTargetZ    = 0;

    KGLOG_PROCESS_ERROR(pCaster);
    KGLOG_PROCESS_ERROR(pTarget);

    nCasterX = pCaster->m_nX;
    nCasterY = pCaster->m_nY;
    
    bRetCode = pTarget->GetTarget(&nTargetX, &nTargetY, &nTargetZ);
    KG_PROCESS_ERROR(bRetCode);

    if (nCasterX != nTargetX || nCasterY != nTargetY)
    {
        int nSubDirection = g_GetDirection(nCasterX, nCasterY, nTargetX, nTargetY);
        KG_ASSERT_EXIT(nSubDirection >= 0 && nSubDirection <= DIRECTION_COUNT);
        nSubDirection -= pCaster->m_nFaceDirection;
        if (nSubDirection > DIRECTION_COUNT / 2)
        {
            nSubDirection -= DIRECTION_COUNT;
        }
        if (nSubDirection < - DIRECTION_COUNT / 2)
        {
            nSubDirection += DIRECTION_COUNT;
        }

        KG_PROCESS_ERROR(nSubDirection >= - m_nAngleRange / 2);
        KG_PROCESS_ERROR(nSubDirection <=   m_nAngleRange / 2);
    }

    nResult = srcSuccess;
Exit0:
    return nResult;
}

#endif

DEFINE_LUA_CLASS_BEGIN(KSkill)

    REGISTER_LUA_STRING_READONLY(KSkill,    SkillName)
    REGISTER_LUA_DWORD_READONLY(KSkill,     SkillID) 
    REGISTER_LUA_DWORD_READONLY(KSkill,     MaxLevel)
//	REGISTER_LUA_INTEGER_READONLY(KSkill,   FunctionType)
    REGISTER_LUA_INTEGER_READONLY(KSkill,   UIType)
    REGISTER_LUA_DWORD_READONLY(KSkill,     BelongKungfu)
    REGISTER_LUA_DWORD_READONLY(KSkill,     BelongSchool)
    REGISTER_LUA_INTEGER_READONLY(KSkill,   CastMode)
    REGISTER_LUA_DWORD_READONLY(KSkill,     WeaponRequest)
    REGISTER_LUA_INTEGER(KSkill,            CostItemType)
    REGISTER_LUA_INTEGER(KSkill,            CostItemIndex)
    REGISTER_LUA_DWORD_READONLY(KSkill,     MountRequestType)
    REGISTER_LUA_DWORD_READONLY(KSkill,     MountRequestDetail)
    REGISTER_LUA_DWORD_READONLY(KSkill,     MountRequestDetailLevel)
    REGISTER_LUA_DWORD_READONLY(KSkill,     MountType)
    REGISTER_LUA_BOOL_READONLY(KSkill,      IsMountAble)
    REGISTER_LUA_BOOL_READONLY(KSkill,      IsPassiveSkill)
    REGISTER_LUA_BOOL_READONLY(KSkill,      IsChannelSkill)
    REGISTER_LUA_BOOL_READONLY(KSkill,      IsExpSkill)
    REGISTER_LUA_BOOL_READONLY(KSkill,      IsExactHit)
    REGISTER_LUA_INTEGER_READONLY(KSkill,   EffectType)
    REGISTER_LUA_BOOL_READONLY(KSkill,      CauseAbradeEquipment)
    REGISTER_LUA_BOOL_READONLY(KSkill,      UseCastScript)


    REGISTER_LUA_DWORD_READONLY(KSkill,      Level)
    REGISTER_LUA_DWORD(KSkill,               LevelUpExp)
    REGISTER_LUA_INTEGER(KSkill,             PlayerLevelLimit)
    REGISTER_LUA_INTEGER(KSkill,             ExpAddOdds)

    REGISTER_LUA_INTEGER(KSkill,             CostLife)
    REGISTER_LUA_INTEGER(KSkill,             CostMana)
    REGISTER_LUA_INTEGER(KSkill,             CostStamina)
    
    REGISTER_LUA_INTEGER(KSkill,             PrepareFrames)	
  
    REGISTER_LUA_INTEGER(KSkill,             ThreatRevisePercent)	

    REGISTER_LUA_INTEGER(KSkill,             MinRadius)	
    REGISTER_LUA_INTEGER(KSkill,             MaxRadius)	
    REGISTER_LUA_INTEGER(KSkill,             AreaRadius)	

    REGISTER_LUA_INTEGER(KSkill,             AngleRange)		

    REGISTER_LUA_INTEGER(KSkill,             BulletVelocity)  
    REGISTER_LUA_INTEGER(KSkill,             WeaponDamagePercent)

    REGISTER_LUA_INTEGER(KSkill,             BreakRate)		
    REGISTER_LUA_INTEGER(KSkill,             BrokenRate)	

    REGISTER_LUA_INTEGER(KSkill,             BeatBackRate)

    REGISTER_LUA_INTEGER(KSkill,             ChannelFrame)		
    REGISTER_LUA_INTEGER(KSkill,             ChannelInterval)	

    REGISTER_LUA_INTEGER(KSkill,             TargetCountLimit)

    REGISTER_LUA_INTEGER(KSkill,             AttackWeakPos)

    REGISTER_LUA_BOOL(KSkill,                IsFormationSkill)
    REGISTER_LUA_INTEGER(KSkill,             FormationRange)
    REGISTER_LUA_INTEGER(KSkill,             LeastFormationPopulation)

    REGISTER_LUA_BOOL(KSkill,                IsAccumulate)

    REGISTER_LUA_INTEGER(KSkill,             DismountingRate)

    REGISTER_LUA_INTEGER(KSkill,             BaseThreat)

    REGISTER_LUA_INTEGER(KSkill,             ChainBranch)
    REGISTER_LUA_INTEGER(KSkill,             ChainDepth)

    REGISTER_LUA_INTEGER(KSkill,             DamageToLifeForParty)
    REGISTER_LUA_INTEGER(KSkill,             DamageToManaForParty)

    REGISTER_LUA_INTEGER(KSkill,             TargetLifePercentMax)
    REGISTER_LUA_INTEGER(KSkill,             TargetLifePercentMin)

    REGISTER_LUA_INTEGER(KSkill,             SelfLifePercentMax)
    REGISTER_LUA_INTEGER(KSkill,             SelfLifePercentMin)

    REGISTER_LUA_FUNC(KSkill,                AddSlowCheckSelfBuff)
    REGISTER_LUA_FUNC(KSkill,                AddSlowCheckDestBuff)

    REGISTER_LUA_FUNC(KSkill,                AddAttribute)
    REGISTER_LUA_FUNC(KSkill,                GetAttribute)

    REGISTER_LUA_FUNC(KSkill,                BindBuff)

    REGISTER_LUA_FUNC(KSkill,                GetBuff)

    REGISTER_LUA_FUNC(KSkill,                SetBuffRecipe)

    REGISTER_LUA_FUNC(KSkill,                SetPublicCoolDown)
    REGISTER_LUA_FUNC(KSkill,                SetNormalCoolDown)
	REGISTER_LUA_FUNC(KSkill,                GetNormalCooldownCount)
	REGISTER_LUA_FUNC(KSkill,                GetNormalCooldownID)

    REGISTER_LUA_FUNC(KSkill,                SetSubsectionSkill)

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KSkill,                UITestCast)

    REGISTER_LUA_FUNC(KSkill,                CheckWeaponRequest)
    REGISTER_LUA_FUNC(KSkill,                CheckMountRequest)
    REGISTER_LUA_FUNC(KSkill,                CheckCostItemRequest)
    REGISTER_LUA_FUNC(KSkill,                CheckDistance)
    REGISTER_LUA_FUNC(KSkill,                CheckAngle)
    REGISTER_LUA_FUNC(KSkill,                CheckCasterState)
    REGISTER_LUA_FUNC(KSkill,                CheckTargetState)
#endif

DEFINE_LUA_CLASS_END(KSkill)

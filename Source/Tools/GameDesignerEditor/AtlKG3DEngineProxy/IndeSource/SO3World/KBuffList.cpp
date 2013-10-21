#include "stdafx.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KBuffList.h"
#include "KPlayerServer.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "KCharacter.h"
#include "KBuffManager.h"
#include "KScene.h"
#include "KMath.h"

#ifdef _SERVER
int KBUFF_LIST_NODE::getID()
{
    KBUFF_RECIPE_KEY& rBuffRecipeKey  = BuffRecipePointer.GetRecipeKey();

    return rBuffRecipeKey.dwID;
}

int KBUFF_LIST_NODE::getLevel()
{
    KBUFF_RECIPE_KEY& rBuffRecipeKey  = BuffRecipePointer.GetRecipeKey();

    return rBuffRecipeKey.nLevel;
}

int KBUFF_LIST_NODE::LuaGetIntervalFrame(Lua_State* L)
{
    int     nResult     = 0;
    int     nTopIndex   = 0;
    KBuff*  pBuff       = BuffRecipePointer.GetPointer();

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    lua_pushinteger(L, pBuff->m_nIntervalFrame);

    nResult = 1;
Exit0:
    return nResult;
}
int KBUFF_LIST_NODE::LuaGetEndTime(Lua_State* L)
{
    int     nResult     = 0;
    int     nTopIndex   = 0;
    int     nEndTime    = 0;
    KBuff*  pBuff       = BuffRecipePointer.GetPointer();

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    nEndTime = nNextActiveFrame + (nLeftActiveCount - 1) * pBuff->m_nIntervalFrame;

    lua_pushinteger(L, nEndTime);

    nResult = 1;
Exit0:
    return nResult;
}

DEFINE_LUA_CLASS_BEGIN(KBUFF_LIST_NODE)
    REGISTER_LUA_DWORD_READONLY(KBUFF_LIST_NODE, ID)
    REGISTER_LUA_INTEGER_READONLY(KBUFF_LIST_NODE, Level)
    REGISTER_LUA_INTEGER_READONLY(KBUFF_LIST_NODE, Index)
    REGISTER_LUA_INTEGER_READONLY(KBUFF_LIST_NODE, StackNum)
    REGISTER_LUA_DWORD_READONLY(KBUFF_LIST_NODE, SkillSrcID)
    REGISTER_LUA_INTEGER(KBUFF_LIST_NODE, NextActiveFrame)
    REGISTER_LUA_INTEGER(KBUFF_LIST_NODE, LeftActiveCount)
    REGISTER_LUA_INTEGER(KBUFF_LIST_NODE, CustomValue)
    REGISTER_LUA_FUNC(KBUFF_LIST_NODE, GetEndTime)
    REGISTER_LUA_FUNC(KBUFF_LIST_NODE, GetIntervalFrame)
DEFINE_LUA_CLASS_END(KBUFF_LIST_NODE)
#endif


KBuffList::KBuffList()
{
    m_pOwner = NULL;
#ifdef _SERVER
    m_nIndexAlloc = 0;
#endif
}

KBuffList::~KBuffList()
{
}

BOOL KBuffList::Init(KCharacter *pOwner)
{
    m_pOwner = pOwner;
    m_dwRepresentState = 0;

    m_BuffList.clear();

    return true;
}

void KBuffList::UnInit()
{
    m_pOwner = NULL;
    m_dwRepresentState = 0;

    m_BuffList.clear();
}

#ifdef _SERVER
BOOL KBuffList::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    size_t              uLeftSize   = uDataLen;
    BYTE*               pbyOffset   = pbyData;
    KBUFF_DB_DATA*      pBuffDBData = NULL;
    int                 nCount      = 0;
    KBuff*              pBuffInfo   = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA));
    pBuffDBData = (KBUFF_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(KBUFF_DB_DATA);
    pbyOffset += sizeof(KBUFF_DB_DATA);

    nCount = pBuffDBData->byCount;

    KGLOG_PROCESS_ERROR(uLeftSize >= nCount * sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM));
    uLeftSize -= nCount * sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM);
    pbyOffset += nCount * sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM);

    for (int nIndex = 0; nIndex < nCount; nIndex++)
    {
        pBuffInfo = g_pSO3World->m_BuffManager.GetBuff(pBuffDBData->Items[nIndex].RecipeKey);
        if (!pBuffInfo)
        {
            continue;
        }

        for (int i = 0; i < pBuffDBData->Items[nIndex].byStackNum; i++)
        {
            KBUFF_LIST_NODE     UserBuffInfo;
            int                 nActiveFrame    = 0;

            memset(&UserBuffInfo, 0, sizeof(UserBuffInfo));

            UserBuffInfo.SkillRecipePointer.ClearRecipeKey();

            UserBuffInfo.dwSkillSrcID   = pBuffDBData->Items[nIndex].dwSkillSrcID;
            UserBuffInfo.nSkillSrcLevel = pBuffDBData->Items[nIndex].bySkillSrcLevel;
            UserBuffInfo.nStackNum      = 1;
            UserBuffInfo.nCustomValue   = pBuffDBData->Items[nIndex].nCustomValue;

            if (pBuffDBData->Items[nIndex].nLeftFrame == INT_MAX)
            {
                UserBuffInfo.nNextActiveFrame = INT_MAX;
            }
            else
            {
                nActiveFrame = g_pSO3World->m_nGameLoop + pBuffDBData->Items[nIndex].nLeftFrame;

                if (pBuffInfo->m_bIsOffLineClock)
                {
                    assert(IS_PLAYER(m_pOwner->m_dwID));
                    KPlayer* pPlayer = (KPlayer*)m_pOwner;

                    time_t nOfflineTime = g_pSO3World->m_nCurrentTime - pPlayer->m_nLastSaveTime;
                    nOfflineTime = max((int)nOfflineTime, 0);

                    nActiveFrame -= (int)nOfflineTime * GAME_FPS;

                    nActiveFrame = max(nActiveFrame, 0);
                }

                assert(pBuffInfo->m_nIntervalFrame);
                UserBuffInfo.nLeftActiveCount = pBuffDBData->Items[nIndex].nLeftFrame / pBuffInfo->m_nIntervalFrame;
                if (pBuffDBData->Items[nIndex].nLeftFrame % pBuffInfo->m_nIntervalFrame)
                {
                    UserBuffInfo.nLeftActiveCount++;
                }
                UserBuffInfo.nNextActiveFrame = nActiveFrame - pBuffInfo->m_nIntervalFrame * (UserBuffInfo.nLeftActiveCount - 1);
            }

            UserBuffInfo.BuffRecipePointer.SetRecipeKey(pBuffDBData->Items[nIndex].RecipeKey);

            AddBuff(UserBuffInfo);
        }
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0); 

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                bResult     = false;
    size_t              uLeftSize   = uBufferSize;
    BYTE*               pbyOffset   = pbyBuffer;
    KBUFF_DB_DATA*      pBuffDBData = NULL;
    int                 nCount      = 0;

    assert(m_pOwner);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA));
    pBuffDBData = (KBUFF_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(KBUFF_DB_DATA);
    pbyOffset += sizeof(KBUFF_DB_DATA);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        int     nActiveFrame    = 0;
        int     nLeftFrame      = 0;
        KBuff*  pBuffInfo       = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        pBuffInfo = (it->BuffRecipePointer).GetPointer();
        KGLOG_PROCESS_ERROR(pBuffInfo);

        if (!pBuffInfo->m_bSave)
        {
            continue;
        }

        if (pBuffInfo->m_nIntervalFrame == INT_MAX)
        {
            nLeftFrame = INT_MAX;
        }
        else
        {
            nActiveFrame = it->nNextActiveFrame + pBuffInfo->m_nIntervalFrame * (it->nLeftActiveCount - 1);
            nLeftFrame = nActiveFrame - g_pSO3World->m_nGameLoop;
        }

        if (nLeftFrame < LOGOUT_WAITING_TIME)
        {
            continue;
        }

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM));
        uLeftSize -= sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM);
        pbyOffset += sizeof(KBUFF_DB_DATA::KBUFF_DB_ITEM);

        pBuffDBData->Items[nCount].RecipeKey        = (it->BuffRecipePointer).GetRecipeKey();
        pBuffDBData->Items[nCount].byStackNum       = (BYTE)it->nStackNum;
        pBuffDBData->Items[nCount].dwSkillSrcID     = it->dwSkillSrcID;
        pBuffDBData->Items[nCount].bySkillSrcLevel  = (BYTE)it->nSkillSrcLevel;
        pBuffDBData->Items[nCount].nLeftFrame       = nLeftFrame;
        pBuffDBData->Items[nCount].nCustomValue     = it->nCustomValue;

        ++nCount;

        if (nCount == UCHAR_MAX) // 最多255个
        {
            break;
        }
    }

    pBuffDBData->byCount = (BYTE)nCount;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}
#endif

void KBuffList::Activate()
{
    if (!m_pOwner)
        return;

    // 0.25 second precision. 
    if ((g_pSO3World->m_nGameLoop - m_pOwner->m_dwID) % (GAME_FPS / 4) == 0)
    {
#ifdef _SERVER
        CheckBuffActive();
#endif
    }

    if ((g_pSO3World->m_nGameLoop - m_pOwner->m_dwID) % GAME_FPS == 0)
    {
#ifdef _SERVER
        CleanBuffByPersist();
#endif
        DelRemovedBuff();
    }
}

SKILL_RESULT_CODE KBuffList::CanAddBuff(DWORD dwSkillSrcID, KBuff *pBuff)
{
    SKILL_RESULT_CODE   nResult     = srcFailed;
    BOOL                bRetCode    = false;

    assert(m_pOwner);
    KG_PROCESS_SUCCESS(!pBuff);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff* pOldBuff = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        pOldBuff = (it->BuffRecipePointer).GetPointer();
        KGLOG_PROCESS_ERROR(pOldBuff);

        if (pBuff->m_dwID == pOldBuff->m_dwID)
        {
            if (pBuff->m_bIsStackable)
            {
                KG_PROCESS_ERROR_RET_CODE(pBuff->m_nIntensity >= pOldBuff->m_nIntensity, srcForceEffect);
                break;
            }

#ifdef _SERVER
            if (pBuff->m_bExclude && dwSkillSrcID == it->dwSkillSrcID)
            {
                KG_PROCESS_ERROR_RET_CODE(pBuff->m_nIntensity >= pOldBuff->m_nIntensity, srcForceEffect);
                break;
            }
#endif
            continue;
        }

#ifdef _SERVER
        if (
            pBuff->m_nAppendType == pOldBuff->m_nAppendType &&
            (pBuff->m_bGlobalExclude || dwSkillSrcID == it->dwSkillSrcID)
        )
        {
            KG_PROCESS_ERROR_RET_CODE(pBuff->m_nIntensity >= pOldBuff->m_nIntensity, srcForceEffect);
            break;
        }
#endif
    }

Exit1:
    nResult = srcSuccess;
Exit0:
    return nResult;
}

#ifdef _SERVER
SKILL_RESULT_CODE KBuffList::CallBuff(DWORD dwCasterID, int nCasterLevel, KBUFF_RECIPE_KEY BuffRecipeKey, int nCount, int nFristActiveFrame)
{
    SKILL_RESULT_CODE   nResult             = srcFailed;
    BOOL                bRetCode            = false;
    SKILL_RESULT_CODE   nRetAddBuff         = srcFailed;
    KCharacter*         pCharacter          = NULL;
    KBuff*              pBuff               = NULL;
    KSKILL_BULLET*      pBullet             = NULL;
    int                 nBuffCoefficient    = 0;
    int                 nResistBase         = 0;
    int                 nResistValue        = 0;
    int                 nNextActiveFrame    = 0;
    BOOL                bResistFlag         = false;
    BOOL                bImmunityFlag       = false;
    int                 nDenominator        = 0;
    KBUFF_LIST_NODE     BuffNode;

    memset(&BuffNode, 0, sizeof(BuffNode));

    pBuff = BuffNode.BuffRecipePointer.SetRecipeKey(BuffRecipeKey);
    if (!pBuff)
    {
        KGLogPrintf(KGLOG_ERR, "[BUFF] Can't find BUFF with ID=%u, Level=%d.", BuffRecipeKey.dwID, BuffRecipeKey.nLevel);
        nResult = srcBuffInvalid;
        goto Exit0;
    }

    if (pBuff->m_nDecayType >= 0 && pBuff->m_nDecayType < MAX_BUFF_DECAY_TYPE && IS_PLAYER(m_pOwner->m_dwID))
    {
        KPlayer* pPlayer = (KPlayer*)m_pOwner;
        if (g_pSO3World->m_nGameLoop > pPlayer->m_nBuffDecayNextFrame[pBuff->m_nDecayType])
        {
            pPlayer->m_nBuffDecayCount[pBuff->m_nDecayType] = 0;
        }

        if (pPlayer->m_nBuffDecayCount[pBuff->m_nDecayType] >= MAX_DECAY_COUNT)
        {
            bImmunityFlag = true;

            goto Exit0;
        }
    }

    bRetCode = CheckImmunity(pBuff->m_nFunctionType);
    bImmunityFlag = !bRetCode;
    KG_PROCESS_ERROR(!bImmunityFlag);

    switch (pBuff->m_nKindType)
    {
    case sktSolarMagic:
        nResistBase = m_pOwner->m_nSolarMagicShield;
        break;
    case sktNeutralMagic:
        nResistBase = m_pOwner->m_nNeutralMagicShield;
        break;
    case sktLunarMagic:
        nResistBase = m_pOwner->m_nLunarMagicShield;
        break;
    case sktPoison:
        nResistBase = m_pOwner->m_nPoisonMagicShield;
        break;
    }

    BuffNode.dwSkillSrcID       = dwCasterID;
    BuffNode.nSkillSrcLevel     = nCasterLevel;
    nNextActiveFrame = nFristActiveFrame ? nFristActiveFrame : pBuff->m_nIntervalFrame;
    BuffNode.nLeftActiveCount   = nCount ? nCount : pBuff->m_nActiveCount;
    if (nNextActiveFrame == INT_MAX)
    {
        BuffNode.nNextActiveFrame   = INT_MAX;
    }
    else
    {
        BuffNode.nNextActiveFrame   = g_pSO3World->m_nGameLoop + nNextActiveFrame;
    }

    if (pBuff->m_nDecayType >= 0 && pBuff->m_nDecayType < MAX_BUFF_DECAY_TYPE && IS_PLAYER(m_pOwner->m_dwID))
    {
        KPlayer*    pPlayer         = (KPlayer*)m_pOwner;
        int         nDecayPercent   = KILO_NUM;
        int         nTotalFrame     = 0;

        KGLOG_PROCESS_ERROR(BuffNode.nNextActiveFrame != INT_MAX); // 读取BUFF的时候就判断过，不限时间的BUFF无法有递减效果

        nDecayPercent = nDecayPercent >> pPlayer->m_nBuffDecayCount[pBuff->m_nDecayType];
        // nDecayPercent / (2^pPlayer->m_nBuffDecayCount[pBuff->m_nDecayType])

        nTotalFrame = nNextActiveFrame + (BuffNode.nLeftActiveCount - 1) * pBuff->m_nIntervalFrame;

        nTotalFrame = nTotalFrame * nDecayPercent / KILO_NUM;

        nTotalFrame = max(0, nTotalFrame);

        BuffNode.nLeftActiveCount = nTotalFrame / pBuff->m_nIntervalFrame;
        if (nTotalFrame % pBuff->m_nIntervalFrame)
        {
            BuffNode.nLeftActiveCount++;
        }

        BuffNode.nNextActiveFrame = 
            g_pSO3World->m_nGameLoop + nTotalFrame - (BuffNode.nLeftActiveCount - 1) * pBuff->m_nIntervalFrame;
    }

    bRetCode = IS_PLAYER(dwCasterID);
    if (bRetCode)
    {
        pCharacter = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(dwCasterID);
    }
    else
    {
        pCharacter = (KCharacter*)g_pSO3World->m_NpcSet.GetObj(dwCasterID);
    }
    KG_PROCESS_SUCCESS(!pCharacter);
    KG_PROCESS_SUCCESS(!pCharacter->m_pSkillAttributeParam);

    pBullet = pCharacter->m_pSkillAttributeParam->pBullet;
    KG_PROCESS_ERROR_RET_CODE(pBullet, srcNotInFight);

    if (pBuff->m_nKindType != sktPhysics && pBuff->m_nKindType == sktNone)
    {
        int nOddsRange = 0;

        nDenominator = nResistBase + 5 * pBullet->nSkillSrcLevel + 100;
        KGLOG_PROCESS_ERROR(nDenominator > 0);
        nResistValue = nResistBase * TEN_THOUSAND_NUM / nDenominator;

        nOddsRange = (int)g_Random(TEN_THOUSAND_NUM);
        bResistFlag = (nResistValue + nOddsRange >= TEN_THOUSAND_NUM) ? true : false;
    }
    KG_PROCESS_ERROR(!bResistFlag);

    BuffNode.SkillRecipePointer = pCharacter->m_pSkillAttributeParam->SkillRecipePointer;

    nBuffCoefficient = max(pBuff->m_nActiveCount * pBuff->m_nIntervalFrame / 12, 16);

    KGLOG_PROCESS_ERROR(pBuff->m_nActiveCount > 0);

    BuffNode.nPhysicsDamage             = pBullet->nDotPhysicsAP * nBuffCoefficient / PHYSICS_AP_TO_DPS / pBuff->m_nActiveCount / GAME_FPS;
    BuffNode.nSolarDamage               = pBullet->nDotSolarAP * nBuffCoefficient / MAGIC_AP_TO_DPS / pBuff->m_nActiveCount / GAME_FPS;
    BuffNode.nNeutralDamage             = pBullet->nDotNeutralAP * nBuffCoefficient / MAGIC_AP_TO_DPS / pBuff->m_nActiveCount / GAME_FPS;
    BuffNode.nLunarDamage               = pBullet->nDotLunarAP * nBuffCoefficient / MAGIC_AP_TO_DPS / pBuff->m_nActiveCount / GAME_FPS;
    BuffNode.nPoisonDamage              = pBullet->nDotPoisonAP * nBuffCoefficient / MAGIC_AP_TO_DPS / pBuff->m_nActiveCount / GAME_FPS;
    BuffNode.nTherapy                   = pBullet->nDotTherapyAP * nBuffCoefficient / THERAPY_AP_TO_DPS / pBuff->m_nActiveCount / GAME_FPS;
    BuffNode.nStealLifePercent          = pBullet->nDamageToLifeForSelf;
    BuffNode.nStealManaPercent          = pBullet->nDamageToManaForSelf;
    BuffNode.nBaseThreatCoefficient     = pBullet->nBaseThreatCoefficient;
    BuffNode.nDamageThreatCoefficient   = pBullet->nDamageThreatCoefficient;
    BuffNode.nTherapyThreatCoefficient  = pBullet->nTherapyThreatCoefficient;
    BuffNode.nCriticalStrike            = pBullet->nCriticalStrike;

Exit1:

    m_pOwner->m_SkillDebug.Trace(
        2, "[技能调试][%d]:[添加BUFF] BuffID:%u, BuffLevel:%d",
        g_pSO3World->m_nGameLoop, BuffRecipeKey.dwID, BuffRecipeKey.nLevel
    );
    nRetAddBuff = AddBuff(BuffNode);

    if (
        nRetAddBuff == srcSuccess && IS_PLAYER(m_pOwner->m_dwID) &&
        pBuff->m_nDecayType >= 0 && pBuff->m_nDecayType < MAX_BUFF_DECAY_TYPE
    )
    {
        KPlayer* pPlayer        = (KPlayer*)m_pOwner;
        int      nDecayFrame    = 0;
        int      nImmunityFrame = 0;
        int      nAddFrame      = 0;

        assert(pPlayer);

        bRetCode = g_pSO3World->m_BuffManager.GetBuffDecayInfo(pBuff->m_nDecayType, &nDecayFrame, &nImmunityFrame);
        KGLOG_PROCESS_ERROR(bRetCode);

        pPlayer->m_nBuffDecayCount[pBuff->m_nDecayType]++;

        if (pPlayer->m_nBuffDecayCount[pBuff->m_nDecayType] < MAX_DECAY_COUNT)
        {
            nAddFrame = nDecayFrame;
        }
        else
        {
            nAddFrame = nImmunityFrame;
        }

        pPlayer->m_nBuffDecayNextFrame[pBuff->m_nDecayType] = g_pSO3World->m_nGameLoop + nAddFrame;
    }

    nResult = srcSuccess;
Exit0:

    if (m_pOwner->m_pDstSkillCalculateResult && (bImmunityFlag || bResistFlag))
    {
        m_pOwner->m_pDstSkillCalculateResult->nBuffID       = pBuff->m_dwID;
        m_pOwner->m_pDstSkillCalculateResult->nBuffLevel    = pBuff->m_nLevel;
        m_pOwner->m_pDstSkillCalculateResult->bBuffImmunity = bImmunityFlag;
        m_pOwner->m_pDstSkillCalculateResult->bBuffResist   = bResistFlag;
    }
    return nResult;
}
#endif

#ifdef _CLIENT
BOOL KBuffList::UpdateBuff(KBUFF_LIST_NODE &rBuffNode)
{
    BOOL                        bResult         = false;
    BOOL                        bAdd            = false;
    BOOL                        bShowText       = false;
    int                         nOldStackNum    = 0;
    KBuff*                      pBuff           = NULL;
    KUIEventBuffLog             BuffLog;
    KUIEventPlayerStateUpdate   param;
    KUIBuffUpdata               UIBuff;
    KBUFF_LIST::iterator        it;
    KBUFF_LIST::iterator        itEnd;

    assert(m_pOwner);
    KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);

    for (it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        if (it->bDeleteFlag)
        {
            continue;
        }

        if (it->nIndex == rBuffNode.nIndex)
        {
            break;
        }
    }

    if (it != itEnd)
    {
        nOldStackNum = it->nStackNum;
        DelGroupBuff(*it);
    }

    pBuff = rBuffNode.BuffRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pBuff);

    if (rBuffNode.nStackNum > 0)
    {
        bAdd = (rBuffNode.nStackNum >= nOldStackNum);
        bShowText = (rBuffNode.nStackNum > nOldStackNum);

        AddBuff(rBuffNode);
    }

    // 界面图标提示
    UIBuff.dwPlayerID = m_pOwner->m_dwID;
    UIBuff.bDelete	  = rBuffNode.nStackNum == 0;
    UIBuff.nIndex     = rBuffNode.nIndex;
    UIBuff.bCanCancel = pBuff->m_bCanCancel;
    UIBuff.dwBuffID   = pBuff->m_dwID;
    UIBuff.nBuffLevel = pBuff->m_nLevel;
    UIBuff.nCount     = rBuffNode.nStackNum;
    if (pBuff->m_nIntervalFrame == INT_MAX)
    {
        UIBuff.nEndFrame  = INT_MAX;
    }
    else
    {
        UIBuff.nEndFrame  = rBuffNode.nNextActiveFrame + pBuff->m_nIntervalFrame * (rBuffNode.nLeftActiveCount - 1);
    }
    UIBuff.bInit	  = false;
    g_pGameWorldUIHandler->OnBuffUpdata(UIBuff);

    if (bShowText && (g_pSO3World->m_dwClientPlayerID == m_pOwner->m_dwID))
    {
        KUIEventSkillBuffText BuffText;
        BuffText.dwCharacterID = m_pOwner->m_dwID;
        BuffText.bCanCancel    = pBuff->m_bCanCancel;
        BuffText.dwID          = pBuff->m_dwID;
        BuffText.nLevel        = pBuff->m_nLevel;

        g_pGameWorldUIHandler->OnSkillBuffText(BuffText);
    }

    BuffLog.dwTarget    = m_pOwner->m_dwID;
    BuffLog.bCanCancel  = pBuff->m_bCanCancel;
    BuffLog.dwID        = pBuff->m_dwID;
    BuffLog.nLevel      = pBuff->m_nLevel;
    BuffLog.bAddOrDel   = bAdd;

    g_pGameWorldUIHandler->OnBuffLog(BuffLog);

    param.dwPlayerID = g_pSO3World->m_dwClientPlayerID;
    g_pGameWorldUIHandler->OnPlayerStateUpdate(param);

    bResult = true;
Exit0:
    return bResult;
}
#endif

KBUFF_LIST_NODE* KBuffList::GetBuff(DWORD dwBuffID, int nBuffLevel)
{
    KBUFF_LIST_NODE*        pRetBuff    = NULL;

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*  pBuff = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        if (pBuff->m_dwID != dwBuffID || (nBuffLevel != 0 && pBuff->m_nLevel != nBuffLevel))
        {
            continue;
        }

        pRetBuff = &(*it);
        break;
    }

Exit0:
    return pRetBuff;
}

BOOL KBuffList::GetBuffMostStackNum(DWORD dwBuffID, int* pnRetBuffLevel, int* pnRetStackNum)
{
    BOOL    bResult         = false;
    KBuff*  pBuff           = NULL;
    int     nMaxStackNum    = 0;
    int     nMaxBuffLevel   = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        pBuff = (it->BuffRecipePointer).GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        if (pBuff->m_dwID == dwBuffID && it->nStackNum > nMaxStackNum)
        {
            nMaxStackNum = it->nStackNum;
        }

        if (pBuff->m_dwID == dwBuffID && pBuff->m_nLevel > nMaxBuffLevel)
        {
            nMaxBuffLevel = pBuff->m_nLevel;
        }
    }

    *pnRetBuffLevel = nMaxBuffLevel;
    *pnRetStackNum  = nMaxStackNum;

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KBuffList::ConsumeBuff(DWORD dwCasterID, DWORD dwBuffID)
{
    BOOL bResult    = false;
    int  nLoopCount = 0;
    int  nStackNum  = 0;

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff* pBuff = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        if (pBuff->m_dwID != dwBuffID)
        {
            continue;
        }

        if (it->dwSkillSrcID != dwCasterID)
        {
            continue;
        }

        nStackNum = it->nStackNum;
        assert(nStackNum > 0);
        assert(it->nLeftActiveCount > 0);
        it->nStackNum = nStackNum * it->nLeftActiveCount; // 我们此时认为BUFF的应用次数等于现有层数的剩下跳数倍

        pBuff->Active(m_pOwner, *it);

        it->nStackNum = nStackNum; // 恢复原来的层数，因为层数要在反应用和EndTime的时候作用
        it->nLeftActiveCount = 0;  // 表示已经跳完帧数了

        DelGroupBuff(*it);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::SetBuffNextActiveFrame(int nIndex, int nNextActiveFrame)
{
    BOOL bResult    = false;
    int  nLoopCount = 0;
    
    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*              pBuff    = NULL;
        int                 nEndTime = 0;
        KBUFF_RECIPE_KEY    BuffInfo;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        if (it->nIndex != nIndex)
        {
            continue;
        }

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        BuffInfo = it->BuffRecipePointer.GetRecipeKey();

        it->nNextActiveFrame = nNextActiveFrame;

        nEndTime = it->nNextActiveFrame + pBuff->m_nIntervalFrame * (it->nLeftActiveCount - 1);
        g_PlayerServer.DoSyncBuffSingle(
            m_pOwner, it->nIndex,
            BuffInfo.dwID, BuffInfo.nLevel, BuffInfo.dwRecipeKey,
            it->nStackNum, nEndTime
        );
            
        break;
    }
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::SetBuffLeftActiveCount(int nIndex, int nLeftActiveCount)
{
    BOOL bResult = false;
    int  nLoopCount = 0;

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*              pBuff    = NULL;
        int                 nEndTime = 0;
        KBUFF_RECIPE_KEY    BuffInfo;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        if (it->nIndex != nIndex)
        {
            continue;
        }

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        BuffInfo = it->BuffRecipePointer.GetRecipeKey();

        it->nLeftActiveCount = nLeftActiveCount;

        nEndTime = it->nNextActiveFrame + pBuff->m_nIntervalFrame * (it->nLeftActiveCount - 1);
        g_PlayerServer.DoSyncBuffSingle(
            m_pOwner, it->nIndex,
            BuffInfo.dwID, BuffInfo.nLevel, BuffInfo.dwRecipeKey,
            it->nStackNum, nEndTime
        );

        break;
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

#ifdef _SERVER
SKILL_RESULT_CODE KBuffList::AddBuff(KBUFF_LIST_NODE &rBuffNode)
{
    SKILL_RESULT_CODE       nResult         = srcFailed;
    BOOL                    bRetCode        = false;
    KBuff*                  pNewBuff        = NULL;
    KAttribute*             pNewAttribute   = NULL;
    int                     nListCount      = 0;
    int                     nEndFrame       = 0;
    KBUFF_LIST::iterator    itInsertPos;
    KBUFF_RECIPE_KEY        NewBuffRecipeKey;

    assert(m_pOwner);

    rBuffNode.bDeleteFlag   = false;
    rBuffNode.nIndex        = m_nIndexAlloc++;
    rBuffNode.nStackNum     = 1;
    pNewBuff                = rBuffNode.BuffRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR_RET_CODE(pNewBuff, srcBuffInvalid);
    NewBuffRecipeKey        = rBuffNode.BuffRecipePointer.GetRecipeKey();


    rBuffNode.nLeftActiveCount = max(0, rBuffNode.nLeftActiveCount); // 保证不把负的跳数加入BuffList

    nListCount = 0;
    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*              pOldBuff        = NULL;
        KBUFF_RECIPE_KEY    OldBuffRecipeKey;

        if (it->bDeleteFlag)
            continue;

        nListCount++;
        KGLOG_PROCESS_ERROR(nListCount < MAX_LIST_COUNT);

        pOldBuff = (it->BuffRecipePointer).GetPointer();
        KGLOG_PROCESS_ERROR(pOldBuff);

        OldBuffRecipeKey = (it->BuffRecipePointer).GetRecipeKey();

        if (pNewBuff->m_dwID == pOldBuff->m_dwID)
        {
            if (pNewBuff->m_bIsStackable && pNewBuff->m_nLevel == pOldBuff->m_nLevel)   // 叠加
            {
                KG_PROCESS_ERROR_RET_CODE(pNewBuff->m_nIntensity >= pOldBuff->m_nIntensity, srcForceEffect);

                if (pNewBuff->m_nIntensity == pOldBuff->m_nIntensity)
                {
                    pOldBuff->UnApply(m_pOwner, *it);

                    rBuffNode.nStackNum = it->nStackNum + 1;
                    MAKE_IN_RANGE(rBuffNode.nStackNum, 0, pNewBuff->m_nMaxStackNum);

                    rBuffNode.nIndex = it->nIndex;

                    *it = rBuffNode;

                    pNewBuff->Apply(m_pOwner, *it);
                }

                if (pNewBuff->m_nIntensity > pOldBuff->m_nIntensity)
                {
                    DelGroupBuff(*it);
                }
                break;
            }

            if (pNewBuff->m_bExclude || rBuffNode.dwSkillSrcID == it->dwSkillSrcID) // 同一个释放者释放的相同Buff
            {
                KG_PROCESS_ERROR_RET_CODE(pNewBuff->m_nIntensity >= pOldBuff->m_nIntensity, srcForceEffect);

                if (pNewBuff->m_nIntensity == pOldBuff->m_nIntensity && pNewBuff->m_nLevel == pOldBuff->m_nLevel)
                {
                    rBuffNode.nIndex = it->nIndex;

                    *it = rBuffNode;
                    goto Exit1;
                }
                else
                {
                    DelGroupBuff(*it);
                }
                break;
            }

            continue;
        }

        // 类型互斥
        if (
            pOldBuff->m_nAppendType == pNewBuff->m_nAppendType && 
            (pNewBuff->m_bGlobalExclude || rBuffNode.dwSkillSrcID == it->dwSkillSrcID)
        )
        {
            if (pNewBuff->m_bGlobalExclude)
            {
                KG_PROCESS_ERROR_RET_CODE(pNewBuff->m_nIntensity >= pOldBuff->m_nIntensity, srcForceEffect);
            }

            DelGroupBuff(*it);
            break;
        }
    }

    if (pNewBuff->m_bUniqueTarget)
    {
        bRetCode = ProcessUniqueBuff(rBuffNode);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    if (rBuffNode.nStackNum == 1) // 只有一层的时候是新加入链表的
    {
        pNewBuff->Apply(m_pOwner, rBuffNode);
        m_BuffList.push_back(rBuffNode);
    }

    UpdateRepresentState();

Exit1:
    // 同步给客户端相关信息
    nEndFrame = rBuffNode.nNextActiveFrame + pNewBuff->m_nIntervalFrame * (rBuffNode.nLeftActiveCount - 1);
    g_PlayerServer.DoSyncBuffSingle(
        m_pOwner, rBuffNode.nIndex,
        pNewBuff->m_dwID, pNewBuff->m_nLevel, NewBuffRecipeKey.dwRecipeKey,
        rBuffNode.nStackNum, nEndFrame
    );

    nResult = srcSuccess;
Exit0:
    return nResult;
}
#else
SKILL_RESULT_CODE KBuffList::AddBuff(KBUFF_LIST_NODE &rBuffNode)
{
    SKILL_RESULT_CODE   nResult     = srcFailed;
    KBuff*              pNewBuff    = NULL;
    KBuff*              pOldBuff    = NULL;
    BOOL                bClientSelf = false;

    rBuffNode.bDeleteFlag = false;

    if (m_pOwner->m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        pNewBuff = rBuffNode.BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR_RET_CODE(pNewBuff, srcBuffInvalid);

        pNewBuff->Apply(m_pOwner, rBuffNode);
    }

    m_BuffList.push_back(rBuffNode);

    nResult = srcSuccess;
Exit0:
    return nResult;
}
#endif

#ifdef _SERVER
BOOL KBuffList::DelBuff(KBUFF_LIST_NODE &rBuffNode)
{
    BOOL                bResult     = false;
    KBuff*              pBuff       = NULL;
    int                 nEndFrame   = 0;
    KBUFF_RECIPE_KEY    BuffRecipeKey;

    pBuff = rBuffNode.BuffRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pBuff);

    BuffRecipeKey =  rBuffNode.BuffRecipePointer.GetRecipeKey();

    pBuff->UnApply(m_pOwner, rBuffNode);

    rBuffNode.nStackNum--;

    pBuff->Apply(m_pOwner, rBuffNode);

    if (!rBuffNode.nStackNum)
    {
        DelGroupBuff(rBuffNode);
    }
    else
    {
        nEndFrame = rBuffNode.nNextActiveFrame + pBuff->m_nIntervalFrame * (rBuffNode.nLeftActiveCount - 1);
        g_PlayerServer.DoSyncBuffSingle(
            m_pOwner, rBuffNode.nIndex,
            pBuff->m_dwID, pBuff->m_nLevel, BuffRecipeKey.dwRecipeKey,
            rBuffNode.nStackNum, nEndFrame
        );
    }

    bResult = true;
Exit0:
    return bResult;
}
#else
BOOL KBuffList::DelBuff(KBUFF_LIST_NODE &rBuffNode)
{
    BOOL    bResult     = false;
    KBuff*  pBuff       = NULL;
    BOOL    bClientSelf = false;

    bClientSelf = (m_pOwner->m_dwID == g_pSO3World->m_dwClientPlayerID);

    pBuff = rBuffNode.BuffRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pBuff);

    if (bClientSelf)
    {
        pBuff->UnApply(m_pOwner, rBuffNode);
    }

    rBuffNode.nStackNum--;

    if (bClientSelf)
    {
        pBuff->Apply(m_pOwner, rBuffNode);
    }

    if (!rBuffNode.nStackNum)
    {
        DelGroupBuff(rBuffNode);
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

#ifdef _SERVER
BOOL KBuffList::DelGroupBuff(KBUFF_LIST_NODE &rBuffNode)
{
    BOOL                bResult = false;
    KBuff*              pBuff   = NULL;
    KBUFF_RECIPE_KEY    BuffRecipeKey;

    pBuff = rBuffNode.BuffRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pBuff);

    BuffRecipeKey = rBuffNode.BuffRecipePointer.GetRecipeKey();

    pBuff->UnApply(m_pOwner, rBuffNode);

    // 同步客户端
    g_PlayerServer.DoSyncBuffSingle(
        m_pOwner, rBuffNode.nIndex,
        pBuff->m_dwID, pBuff->m_nLevel, BuffRecipeKey.dwRecipeKey,
        0, 0
    );

    rBuffNode.bDeleteFlag = true;

    CallRemoveBuffScript(rBuffNode);
    UpdateRepresentState();

    bResult = true;
Exit0:
    return bResult;
}
#else
BOOL KBuffList::DelGroupBuff(KBUFF_LIST_NODE &rBuffNode)
{
    BOOL    bResult = false;

    if (m_pOwner->m_dwID == g_pSO3World->m_dwClientPlayerID)
    {
        KBuff*  pBuff   = NULL;

        pBuff = rBuffNode.BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        pBuff->UnApply(m_pOwner, rBuffNode);
    }

    rBuffNode.bDeleteFlag = true;

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KBuffList::CleanBuff()
{
    BOOL bResult    = false;
    int  nListCount = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        if (it->bDeleteFlag)
        {
            continue;
        }

        nListCount++;
        KGLOG_PROCESS_ERROR(nListCount < MAX_LIST_COUNT);

        DelGroupBuff(*it);
    }

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KBuffList::DelSingleBuff(DWORD dwID, int nLevel)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nLoopCount = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*  pBuff   = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();

        if (pBuff->m_dwID != dwID || pBuff->m_nLevel != nLevel)
        {
            continue;
        }

        bRetCode = DelBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::DelSingleGroupBuff(DWORD dwID, int nLevel)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nLoopCount = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*  pBuff   = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();

        if (pBuff->m_dwID != dwID || pBuff->m_nLevel != nLevel)
        {
            continue;
        }

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::CancelBuff(int nIndex)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;
    KBuff*  pBuff      = NULL;
    int     nLoopCount = 0;

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        if (it->nIndex != nIndex)
        {
            continue;
        }

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        KGLOG_PROCESS_ERROR(pBuff->m_bCanCancel);

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::DetachSingleBuff(int nDetachType, int nIntensity)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nLoopCount = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*  pBuff   = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();

        if (pBuff->m_nDetachType != nDetachType || pBuff->m_nIntensity > nIntensity)
        {
            continue;
        }

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::DetachMultiBuff(int nDetachType, int nIntensity)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nLoopCount = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*  pBuff   = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();

        if (pBuff->m_nDetachType != nDetachType || pBuff->m_nIntensity > nIntensity)
        {
            continue;
        }

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::DelSingleBuffByFunctionType(int nFunctionType, int nIntensity)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nLoopCount = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*  pBuff   = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();

        if (pBuff->m_nFunctionType != nFunctionType || pBuff->m_nIntensity > nIntensity)
        {
            continue;
        }

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
        break;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::DelMultiBuffByFunctionType(int nFunctionType, int nIntensity)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nLoopCount = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff*  pBuff   = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();

        if (pBuff->m_nFunctionType != nFunctionType || pBuff->m_nIntensity > nIntensity)
        {
            continue;
        }

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::CleanBuffByType(int nBuffType)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    KBuff*  pBuff       = NULL;
    int     nLoopCount  = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        if (pBuff->m_nBuffType != nBuffType)
        {
            continue;
        }

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::CleanBuffByCaster(DWORD dwCaster)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nLoopCount  = 0;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        if (it->dwSkillSrcID != dwCaster)
        {
            continue;
        }

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBuffList::CleanBuffByCanCancel(BOOL bCanCancel)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nLoopCount  = 0;
    KBuff*  pBuff       = NULL;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        if (it->bDeleteFlag)
        {
            continue;
        }

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        if (pBuff->m_bCanCancel != bCanCancel)
        {
            continue;
        }

        bRetCode = DelGroupBuff(*it);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KBuffList::GetTransferData(BYTE byBuffer[], size_t uBufferSize, size_t* puUsedSize)
{
    BOOL                    bResult         = false;
    KBUFF_TRANSFER_DATA*  pTransferData   = NULL;
    size_t                  uLeftSize       = uBufferSize;

    pTransferData = (KBUFF_TRANSFER_DATA*)byBuffer;

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        KBuff* pBuff        = NULL;

        if (it->bDeleteFlag)
        {
            continue;
        }

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KBUFF_TRANSFER_DATA));

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        pTransferData->nIndex       = it->nIndex;
        pTransferData->RecipeKey    = it->BuffRecipePointer.GetRecipeKey();
        pTransferData->byStackNum   = (BYTE)it->nStackNum;
        if (pBuff->m_nIntervalFrame == INT_MAX)
        {
            pTransferData->nEndFrame    = INT_MAX;
        }
        else
        {
            pTransferData->nEndFrame    = it->nNextActiveFrame + pBuff->m_nIntervalFrame * (it->nLeftActiveCount - 1);
        }

        pTransferData++;
        uLeftSize -= sizeof(KBUFF_TRANSFER_DATA);
    }

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _CLIENT
BOOL KBuffList::UpdateBuffData(BYTE byBuffer[], DWORD dwBufferSize)
{
    BOOL                    bResult         = false;
    KBUFF_TRANSFER_DATA*  pTransferData   = NULL;
    int                     nCount          = 0;
    KBuff*                  pBuff           = NULL;
    KBUFF_LIST_NODE         BuffListNode;
    assert(m_pOwner);

    KGLOG_PROCESS_ERROR((dwBufferSize % sizeof(KBUFF_TRANSFER_DATA)) == 0);

    nCount = dwBufferSize / sizeof(KBUFF_TRANSFER_DATA);

    CleanBuff();

    pTransferData = (KBUFF_TRANSFER_DATA*)byBuffer;

    for (int i = 0; i < nCount; i++)
    {
        BuffListNode.BuffRecipePointer.SetRecipeKey(pTransferData->RecipeKey);

        pBuff = BuffListNode.BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        BuffListNode.nIndex     = pTransferData->nIndex;
        BuffListNode.nStackNum  = pTransferData->byStackNum;

        assert(pBuff->m_nIntervalFrame);
        if (pBuff->m_nIntervalFrame == INT_MAX)
        {
            BuffListNode.nLeftActiveCount = 1;
            BuffListNode.nNextActiveFrame = INT_MAX;
        }
        else
        {
            BuffListNode.nLeftActiveCount = (pTransferData->nEndFrame - g_pSO3World->m_nGameLoop) / pBuff->m_nIntervalFrame;
            if ((pTransferData->nEndFrame - g_pSO3World->m_nGameLoop) % pBuff->m_nIntervalFrame)
            {
                BuffListNode.nLeftActiveCount++;
            }
            BuffListNode.nNextActiveFrame = pTransferData->nEndFrame - pBuff->m_nIntervalFrame * (BuffListNode.nLeftActiveCount - 1);
        }

        AddBuff(BuffListNode);

        pTransferData++;
    }

    if (g_pGameWorldUIHandler)
    {
        KUIBuffUpdata UIBuff;
        UIBuff.dwPlayerID = m_pOwner->m_dwID;
        UIBuff.bInit	  = true;
        g_pGameWorldUIHandler->OnBuffUpdata(UIBuff);

        KUIEventPlayerStateUpdate param = { g_pSO3World->m_dwClientPlayerID };
        g_pGameWorldUIHandler->OnPlayerStateUpdate(param);
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

void KBuffList::CleanBuffByPersist()
{
    int nLoopCount = 0;

    assert(m_pOwner);
    assert(m_pOwner->m_pScene);
    KG_PROCESS_ERROR(m_pOwner->m_pCell);

#ifdef _SERVER
    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        BOOL    bMoveStatePass  = false;
        BOOL    bShieldPass     = false;
        BOOL    bFightStatePass = false;
        BOOL    bHorseStatePass = false;
        BOOL    bLeaderPass     = false;
        BOOL    bTerrainPass    = false;
        BOOL    bMapEnablePass  = false;
        KBuff*  pBuff           = NULL;

        if (it->bDeleteFlag)
            continue;

        nLoopCount++;
        KGLOG_PROCESS_ERROR(nLoopCount < MAX_LIST_COUNT);

        pBuff = it->BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        bMapEnablePass = !(m_pOwner->m_pScene->m_dwBanSkillMask & pBuff->m_dwMapBanMask);

        bMoveStatePass = (pBuff->m_nPersistMoveStateMask & (1 << m_pOwner->m_eMoveState));

        switch (pBuff->m_nPersistManaShield)
        {
        case pmsNone:
            bShieldPass = true;
            break;

        case pmsGlobalMana:
            bShieldPass = (m_pOwner->m_nGlobalDamageManaShield > 0);
            break;
        case pmsPhysicsMana:
            bShieldPass = (m_pOwner->m_nPhysicsDamageManaShield > 0);
            break;
        case pmsSolarMana:
            bShieldPass = (m_pOwner->m_nSolarDamageManaShield > 0);
            break;
        case pmsNeutralMana:
            bShieldPass = (m_pOwner->m_nNeutralDamageManaShield > 0);
            break;
        case pmsLunarMana:
            bShieldPass = (m_pOwner->m_nLunarDamageManaShield > 0);
            break;
        case pmsPoisonMana:
            bShieldPass = (m_pOwner->m_nPoisonDamageManaShield > 0);
            break;

        case pmsGlobalAbsorb:
            bShieldPass = (m_pOwner->m_nGlobalDamageAbsorb > 0);
            break;
        case pmsPhysicsAbsorb:
            bShieldPass = (m_pOwner->m_nPhysicsDamageAbsorb > 0);
            break;
        case pmsSolarAbsorb:
            bShieldPass = (m_pOwner->m_nSolarDamageAbsorb > 0);
            break;
        case pmsNeutralAbsorb:
            bShieldPass = (m_pOwner->m_nNeutralDamageAbsorb > 0);
            break;
        case pmsLunarAbsorb:
            bShieldPass = (m_pOwner->m_nLunarDamageAbsorb > 0);
            break;
        case pmsPoisonAbsorb:
            bShieldPass = (m_pOwner->m_nPoisonDamageAbsorb > 0);
            break;

        default:
            assert(false);
        }

        switch (pBuff->m_nPersistFight)
        {
        case pofAny:
            bFightStatePass = true;
            break;
        case pofFight:
            bFightStatePass = m_pOwner->m_bFightState;
            break;
        case pofNotFight:
            bFightStatePass = !(m_pOwner->m_bFightState);
            break;
        default:
            assert(false);
        }

        bHorseStatePass = true;
        if (IS_PLAYER(m_pOwner->m_dwID)) // 只有Player才有骑马的状态
        {
            KPlayer* pPlayer = (KPlayer*)m_pOwner;
            assert(pPlayer);

            switch (pBuff->m_nPersistHorse)
            {
            case pohAny:
                bHorseStatePass = true;
                break;
            case pohHorse:
                bHorseStatePass = pPlayer->m_bOnHorse;
                break;
            case pohNotHorse:
                bHorseStatePass = !(pPlayer->m_bOnHorse);
                break;
            default:
                assert(false);
            }
        }

        switch (pBuff->m_nPersistTerrain)
        {
        case potAny:
            bTerrainPass = true;
            break;
        case potLand:
            bTerrainPass = (m_pOwner->m_pCell->m_BaseInfo.dwCellType == ctPlain);
            break;
        case potWater:
            bTerrainPass = (m_pOwner->m_pCell->m_BaseInfo.dwCellType == ctWater);
            break;
        default:
            assert(false);
        }

        bLeaderPass = true;
        if (pBuff->m_bPersistFormationLeader)
        {
            if (IS_PLAYER(m_pOwner->m_dwID))
            {
                KPlayer* pPlayer = (KPlayer*)m_pOwner;
                assert(pPlayer);

                bLeaderPass = pPlayer->IsFormationLeader();
            }
        }

        if (
            bMoveStatePass && bShieldPass && bFightStatePass && bHorseStatePass &&
            bLeaderPass && bTerrainPass && bMapEnablePass
        )
            continue;

        DelGroupBuff(*it);
    }
#endif

Exit0:
    return;
}

BOOL KBuffList::CheckBuffActive()
{
    BOOL bResult = false;

    assert(m_pOwner);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd =m_BuffList.end(); it != itEnd; ++it)
    {
        KBUFF_LIST_NODE&    rBuffNode = *it;
        KBuff*              pBuffInfo = NULL;

        if (rBuffNode.bDeleteFlag)
            continue;

        if (g_pSO3World->m_nGameLoop < rBuffNode.nNextActiveFrame)
            continue;

        pBuffInfo = rBuffNode.BuffRecipePointer.GetPointer();
        KGLOG_PROCESS_ERROR(pBuffInfo);

#ifdef _SERVER
        pBuffInfo->Active(m_pOwner, *it);
#endif

        rBuffNode.nNextActiveFrame += pBuffInfo->m_nIntervalFrame;
        rBuffNode.nLeftActiveCount--;

        assert(rBuffNode.nLeftActiveCount >= 0);

        if (rBuffNode.nLeftActiveCount == 0)    // Buff 结束
        {
            DelGroupBuff(rBuffNode);
#ifdef _SERVER
            pBuffInfo->EndTime(m_pOwner, rBuffNode);
#endif
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

void KBuffList::DelRemovedBuff()
{
    KBUFF_LIST::iterator it     = m_BuffList.begin();
    KBUFF_LIST::iterator itEnd  = m_BuffList.end();

    while (it != itEnd)
    {
        if (it->bDeleteFlag)
        {
            it = m_BuffList.erase(it);
            continue;
        }

        ++it;
    }
}

BOOL KBuffList::CallRemoveBuffScript(KBUFF_LIST_NODE &rBuffNode)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;

#ifdef _SERVER
    int     nTopIndex       = 0;
    int     nLeftFrame      = 0;
    int     nActiveFrame    = 0;
    KBuff*  pBuff           = NULL;

    assert(m_pOwner);

    pBuff = rBuffNode.BuffRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pBuff);

    if (pBuff->m_nIntervalFrame == INT_MAX)
    {
        nLeftFrame = INT_MAX;
    }
    else
    {
        nActiveFrame = rBuffNode.nNextActiveFrame + pBuff->m_nIntervalFrame * (rBuffNode.nLeftActiveCount - 1);

        nLeftFrame = nActiveFrame - g_pSO3World->m_nGameLoop;
    }

    if (rBuffNode.nLeftActiveCount == 0)
    {
        nLeftFrame = 0;
    }

    nLeftFrame = max(nLeftFrame, 0);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pBuff->m_szScript);
    KG_PROCESS_SUCCESS(bRetCode == false);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(m_pOwner->m_dwID);
    g_pSO3World->m_ScriptCenter.PushValueToStack(pBuff->m_dwID);
    g_pSO3World->m_ScriptCenter.PushValueToStack(pBuff->m_nLevel);
    g_pSO3World->m_ScriptCenter.PushValueToStack(nLeftFrame);

    g_pSO3World->m_ScriptCenter.CallFunction(pBuff->m_szScript, "OnRemove", 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
Exit1:
#endif

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KBuffList::ProcessUniqueBuff(KBUFF_LIST_NODE &rBuffInfo)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    KCharacter*         pSrcCharacter   = NULL;
    KCharacter*         pLastCharacter  = NULL;
    KBuff*              pBuff           = NULL;
    KBUFF_LIST_NODE*    pLastBuff       = NULL;

    assert(m_pOwner);

    pBuff = rBuffInfo.BuffRecipePointer.GetPointer();
    KGLOG_PROCESS_ERROR(pBuff);

    bRetCode = IS_PLAYER(rBuffInfo.dwSkillSrcID);

    if (bRetCode)
    {
        pSrcCharacter = g_pSO3World->m_PlayerSet.GetObj(rBuffInfo.dwSkillSrcID);
    }
    else
    {
        pSrcCharacter = g_pSO3World->m_NpcSet.GetObj(rBuffInfo.dwSkillSrcID);
    }
    KGLOG_PROCESS_ERROR(pSrcCharacter);

    KG_PROCESS_SUCCESS(pSrcCharacter->m_dwBuffEffectSoleTargetID == m_pOwner->m_dwID);

    bRetCode = IS_PLAYER(pSrcCharacter->m_dwBuffEffectSoleTargetID);
    if (bRetCode)
    {
        pLastCharacter = g_pSO3World->m_PlayerSet.GetObj(pSrcCharacter->m_dwBuffEffectSoleTargetID);
    }
    else
    {
        pLastCharacter = g_pSO3World->m_NpcSet.GetObj(pSrcCharacter->m_dwBuffEffectSoleTargetID);
    }
    KG_PROCESS_SUCCESS(!pLastCharacter);

    pLastBuff = pLastCharacter->m_BuffList.GetBuff(pBuff->m_dwID, pBuff->m_nLevel);
    KG_PROCESS_SUCCESS(!pLastBuff);

    KG_PROCESS_SUCCESS(pLastBuff->dwSkillSrcID != pSrcCharacter->m_dwID);

    bRetCode = pLastCharacter->m_BuffList.DelGroupBuff(*pLastBuff);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    pSrcCharacter->m_dwBuffEffectSoleTargetID = m_pOwner->m_dwID;

    bResult = true;
Exit0:
    return bResult;
}
#endif

void KBuffList::UpdateRepresentState()
{
    DWORD   dwMask              = 0;
    DWORD   dwRepresentInPos[3] = {0};

    assert(m_pOwner);

    m_dwRepresentState = 0;

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end(); it != itEnd; ++it)
    {
        DWORD   dwRepresentPos  = 0;
        KBuff*  pBuff           = NULL;

        if (it->bDeleteFlag)
            continue;

        pBuff = (it->BuffRecipePointer).GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        dwRepresentPos = pBuff->m_nRepresentPos - 1;

        if (dwRepresentPos < 0 || dwRepresentPos > 2)
        {
            KGLogPrintf(KGLOG_ERR, "[BUFF] Invalid buff represent pos. BuffID: %lu, Represent Position: %d", pBuff->m_dwID, pBuff->m_nRepresentPos);
            continue;
        }

        if (dwRepresentInPos[dwRepresentPos] > pBuff->m_dwRepresentID)
            continue;

        dwRepresentInPos[dwRepresentPos] = pBuff->m_dwRepresentID;
    }

    dwMask = 0x00007FE0;
    m_dwRepresentState &= dwMask;
    m_dwRepresentState |= dwRepresentInPos[0];

    dwMask = 0x7C1F;
    m_dwRepresentState &= dwMask;
    dwRepresentInPos[1] = dwRepresentInPos[1] << 5;
    m_dwRepresentState |= dwRepresentInPos[1];

    dwMask = 0x3FF;
    m_dwRepresentState &= dwMask;
    dwRepresentInPos[2] = dwRepresentInPos[2] << 10;
    m_dwRepresentState |= dwRepresentInPos[2];

    m_pOwner->m_dwRepresentState = m_dwRepresentState;

#ifdef _SERVER
    g_PlayerServer.DoSyncRepresentState(m_pOwner);
#endif

Exit0:
    return;
}

BOOL KBuffList::CheckImmunity(int nFunctionType)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    assert(nFunctionType > sftInvalid && nFunctionType < sftTotal);

    bRetCode = m_pOwner->m_ImmunityFlag.test(nFunctionType - 1);
    KG_PROCESS_ERROR(!bRetCode);

    bResult = true;
Exit0:
    return bResult;
}
#ifdef _CLIENT
int KBuffList::_LuaGetBuffList(Lua_State* L)
{
    int nResult     = 0;
    int nBuffCount  = 0;
    int nTable      = 0;
    int nCount      = 0;

    nBuffCount = (int)m_BuffList.size();
    KG_PROCESS_ERROR(nBuffCount > 0);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    for (KBUFF_LIST::iterator it = m_BuffList.begin(), itEnd = m_BuffList.end();  it != itEnd; ++it)
    {
        KBuff*  pBuff       = NULL;
        DWORD   dwID        = 0;
        int     nLevel      = 0;
        int     nEndFrame   = 0;
        int     nIndex      = 0;
        int     nStackNum   = 0;
        BOOL    bCanCancel  = false;

        if (it->bDeleteFlag)
        {
            continue;
        }

        pBuff = (it->BuffRecipePointer).GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        if (pBuff->m_nIntervalFrame == INT_MAX)
        {
            nEndFrame = INT_MAX;
        }
        else
        {
            nEndFrame = it->nNextActiveFrame + pBuff->m_nIntervalFrame * (it->nLeftActiveCount - 1);
        }

        nIndex    = it->nIndex;
        nStackNum = it->nStackNum;

        dwID   = pBuff->m_dwID;
        nLevel = pBuff->m_nLevel;

        bCanCancel = pBuff->m_bCanCancel;

        nCount++;
        Lua_PushNumber(L, nCount);

        Lua_NewTable(L);
        int nTableCount = Lua_GetTopIndex(L);

        Lua_PushString(L, "dwID");
        Lua_PushNumber(L, dwID);
        Lua_SetTable(L, nTableCount);

        Lua_PushString(L, "nLevel");
        Lua_PushNumber(L, nLevel);
        Lua_SetTable(L, nTableCount);

        Lua_PushString(L, "bCanCancel");
        Lua_PushBoolean(L, bCanCancel);
        Lua_SetTable(L, nTableCount);

        Lua_PushString(L, "nEndFrame");
        Lua_PushNumber(L, nEndFrame);
        Lua_SetTable(L, nTableCount);

        Lua_PushString(L, "nIndex");
        Lua_PushNumber(L, nIndex);
        Lua_SetTable(L, nTableCount);

        Lua_PushString(L, "nStackNum");
        Lua_PushNumber(L, nStackNum);
        Lua_SetTable(L, nTableCount);

#ifdef _SERVER
        Lua_PushString(L, "dwSkillSrcID");
        Lua_PushNumber(L, it->dwSkillSrcID);
        Lua_SetTable(L, nTableCount);
#endif
        Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}
#endif

#include "stdafx.h"
#include "KSkillList.h"
#include "KSkill.h"
#include "KPlayer.h"
#include "KNpcTemplateList.h"
#include "KPlayerServer.h"
#include "KSO3World.h"

KSkillList::KSkillList(void)
{
    m_pPlayer             = NULL;
    m_dwMountKungfuID     = INVALID_SKILL_ID;
    m_dwMountKungfuLevel  = 0;
}

KSkillList::~KSkillList(void)
{
}

void KSkillList::Init(KPlayer *pPlayer)
{
    assert(!m_pPlayer);
    m_pPlayer             = pPlayer;
    m_dwMountKungfuID     = INVALID_SKILL_ID;
    m_dwMountKungfuLevel  = 0;
}

void KSkillList::UnInit()
{
    m_pPlayer             = NULL;
    m_PlayerSkillList.clear();
    m_dwMountKungfuID     = INVALID_SKILL_ID;
    m_dwMountKungfuLevel  = 0;
}

#ifdef _SERVER
BOOL KSkillList::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult     = false;
    BOOL                bRetCode    = false;
    size_t              uLeftSize   = uDataLen;
    BYTE*               pbyOffset   = pbyData;
    KDB_SKILL_DATA*   pSkillData  = NULL;
    int                 nItemCount  = 0;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_SKILL_DATA));
    pSkillData = (KDB_SKILL_DATA*)pbyOffset;
    uLeftSize -= sizeof(KDB_SKILL_DATA);
    pbyOffset += sizeof(KDB_SKILL_DATA);


    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM) * pSkillData->nSkillCount);
    uLeftSize -= sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM) * pSkillData->nSkillCount;
    pbyOffset += sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM) * pSkillData->nSkillCount;

    for (int nIndex = 0; nIndex < pSkillData->nSkillCount; nIndex++)
    {
        bRetCode = UpdateSkill(
            pSkillData->SkillItems[nIndex].wSkillID,
            pSkillData->SkillItems[nIndex].byMaxLevel,
            pSkillData->SkillItems[nIndex].dwExp,
            false
        );

        if (!bRetCode)
        {
            KGLogPrintf(
                KGLOG_ERR, 
                "[Skill] Role \"%s\", Load skill(%u, %u) failed !\n", 
                m_pPlayer->m_szName,
                pSkillData->SkillItems[nIndex].wSkillID, 
                pSkillData->SkillItems[nIndex].byMaxLevel
            );
        }
    }

    if (pSkillData->dwMountKungfuID != INVALID_SKILL_ID)
    {
        m_pPlayer->MountKungfu(pSkillData->dwMountKungfuID, pSkillData->dwMountKungfuLevel);
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillList::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL            bResult     = false;
    int             nItemCount  = 0;
    size_t          uLeftSize   = uBufferSize;
    BYTE*           pbyOffset   = pbyBuffer;
    KDB_SKILL_DATA* pSkillData  = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_SKILL_DATA));
    pSkillData = (KDB_SKILL_DATA*)pbyOffset;
    uLeftSize -= sizeof(KDB_SKILL_DATA);
    pbyOffset += sizeof(KDB_SKILL_DATA);

    for (PLAYER_SKILL_LIST::iterator it = m_PlayerSkillList.begin(); it != m_PlayerSkillList.end(); ++it)
    {
        KPlayerSkill* pPlayerSkill = &it->second;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM));
        uLeftSize -= sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM);
        pbyOffset += sizeof(KDB_SKILL_DATA::KDB_SKILL_ITEM);

        pSkillData->SkillItems[nItemCount].wSkillID     = (WORD)it->first;
        pSkillData->SkillItems[nItemCount].byMaxLevel   = (BYTE)pPlayerSkill->dwLevel;
        pSkillData->SkillItems[nItemCount].dwExp        = pPlayerSkill->dwExp;
        nItemCount++;
    }

    KGLOG_PROCESS_ERROR(nItemCount <= MAX_PLAYER_SKILL_COUNT);
    pSkillData->nSkillCount = nItemCount;

    pSkillData->dwMountKungfuID     = m_dwMountKungfuID;
    pSkillData->dwMountKungfuLevel  = m_dwMountKungfuLevel;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillList::LearnSkill(DWORD dwSkillID, BOOL bNotifyPlayer, DWORD dwMasterID /* = 0 */)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    DWORD                               dwNewLevel      = 0;
    const KPlayerSkill*                 cpPlayerSkill   = NULL;
    KSkill*                             pOldSkill       = NULL;
    KSkill*                             pNewSkill       = NULL;
    int                                 nSkillCount     = 0;
    PLAYER_SKILL_LIST::const_iterator   cit;

    KGLOG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);

    cit = m_PlayerSkillList.find(dwSkillID);
	if (cit == m_PlayerSkillList.end())
	{
        nSkillCount = (int)m_PlayerSkillList.size();
        KGLOG_PROCESS_ERROR(nSkillCount < MAX_PLAYER_SKILL_COUNT);

        pNewSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, 1);
        KGLOG_PROCESS_ERROR(pNewSkill);

        if (pNewSkill->m_pBaseInfo->dwBelongKungfu != INVALID_SKILL_ID)
        {
            DWORD dwKungfuLevel = GetSkillLevel(pNewSkill->m_pBaseInfo->dwBelongKungfu);
            KGLOG_PROCESS_ERROR(dwKungfuLevel);
        }

		dwNewLevel = 1;
	}
    else
    {
        cpPlayerSkill = &cit->second;

        pOldSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, cpPlayerSkill->dwLevel);
        KGLOG_PROCESS_ERROR(pOldSkill);

        KG_PROCESS_ERROR(cpPlayerSkill->dwLevel < pOldSkill->m_pBaseInfo->dwMaxLevel);

        pNewSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, cpPlayerSkill->dwLevel + 1);
        KGLOG_PROCESS_ERROR(pNewSkill);

        dwNewLevel = cpPlayerSkill->dwLevel + 1;
    }

    bRetCode = UpdateSkill(dwSkillID, dwNewLevel, 0, bNotifyPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pNewSkill->CallLevelUpScript(m_pPlayer);
    KGLOG_CHECK_ERROR(bRetCode);

    if (dwSkillID == m_dwMountKungfuID) // 如果升级的技能是目前装备的内功，则更换内功
    {
        m_pPlayer->UmountKungfu();
        m_pPlayer->MountKungfu(dwSkillID, dwNewLevel);
    }

    if (dwMasterID)
    {
        KSKILL_LEARNING_INFO*   pSkillLearningInfo  = NULL;
        int                     nPlayerMoney        = 0;
        int                     nCostTrain          = 0;

        pSkillLearningInfo = g_pSO3World->m_SkillManager.GetSkillLearningInfo(
            dwMasterID, dwSkillID, dwNewLevel
        );
        KGLOG_PROCESS_ERROR(pSkillLearningInfo);

        nPlayerMoney = m_pPlayer->m_ItemList.GetMoney();
        nPlayerMoney -= pSkillLearningInfo->nPrice;
        nPlayerMoney = max(nPlayerMoney, 0);

        nCostTrain = min(m_pPlayer->m_nCurrentTrainValue, pSkillLearningInfo->nCostTrain);
        if (nCostTrain)
        {
            m_pPlayer->AddTrain(-nCostTrain);
        }

        bRetCode = m_pPlayer->m_ItemList.SetMoney(nPlayerMoney);
        KGLOG_CHECK_ERROR(bRetCode);

        g_LogClient.LogPlayerMoneyChange(-pSkillLearningInfo->nPrice, m_pPlayer, "learn skill");
    }

    bResult = true;
Exit0:
	return bResult;
}

BOOL KSkillList::ForgetSkill(DWORD dwSkillID)
{
    BOOL    bResult    = false;
    BOOL    bRetCode   = false;

    bRetCode = UpdateSkill(dwSkillID, 0, 0);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	return bResult;
}


BOOL KSkillList::AddSkillExp(DWORD dwSkillID, DWORD dwExp)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    int                 nOdds           = 0;
    DWORD               dwSkillExp      = 0;
	KSkill*             pSkill          = NULL;
    const KPlayerSkill* cpPlayerSkill   = NULL;
    PLAYER_SKILL_LIST::const_iterator cit;
    
    cit = m_PlayerSkillList.find(dwSkillID);
	KG_PROCESS_ERROR(cit != m_PlayerSkillList.end());
    cpPlayerSkill = &cit->second;

	pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, cit->second.dwLevel);
	KG_PROCESS_ERROR(pSkill);
	KG_PROCESS_ERROR(pSkill->m_dwLevelUpExp);

    nOdds = (int)g_Random(KILO_NUM);
    KG_PROCESS_SUCCESS(nOdds + pSkill->m_nExpAddOdds < KILO_NUM);

    dwSkillExp = cpPlayerSkill->dwExp + dwExp;

    MAKE_IN_RANGE(dwSkillExp, 0, pSkill->m_dwLevelUpExp);

    bRetCode = UpdateSkill(dwSkillID, cpPlayerSkill->dwLevel, dwSkillExp);

    if (CanRealizeSkill(dwSkillID))
    {
        LearnSkill(dwSkillID, true);
    }

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

void KSkillList::OnPlayerLevelUp()
{
    BOOL                bRetCode        = false;
    DWORD               dwSkillID       = 0;
    DWORD               dwSkillLevel    = 0;
    KSkill*             pSkill          = NULL;
    std::vector<DWORD>  LevelUpSkillIDVector;

    LevelUpSkillIDVector.reserve(8);

    for (
        PLAYER_SKILL_LIST::iterator it = m_PlayerSkillList.begin(), itEnd = m_PlayerSkillList.end(); 
        it != itEnd; 
        ++it
    )
    {
        dwSkillID = it->first;

        dwSkillLevel = GetSkillLevel(dwSkillID);
        KGLOG_PROCESS_ERROR(dwSkillLevel > 0);

        pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, dwSkillLevel);
        KGLOG_PROCESS_ERROR(pSkill);

        if (pSkill->m_pBaseInfo->dwBelongKungfu == INVALID_SKILL_ID)
        {
            continue;
        }

        if (!pSkill->m_pBaseInfo->bIsExpSkill)
        {
            continue;
        }

        bRetCode = CanRealizeSkill(dwSkillID);

        if (bRetCode)
        {
            LevelUpSkillIDVector.push_back(dwSkillID);
        }
    }

    for (
        std::vector<DWORD>::iterator it = LevelUpSkillIDVector.begin(), itEnd = LevelUpSkillIDVector.end(); 
        it != itEnd;
        ++it
    )
    {
        if (CanRealizeSkill(*it))
        {
            LearnSkill(*it, true);
        }
    }

Exit0:
    return;
}

#endif

BOOL KSkillList::CanRealizeSkill(DWORD dwSkillID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KPlayerSkill*               pPlayerSkill    = NULL;
    KSkill*                     pSkill          = NULL;
	PLAYER_SKILL_LIST::iterator it;

    KGLOG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);

    it = m_PlayerSkillList.find(dwSkillID);
    bRetCode = it != m_PlayerSkillList.end();
    KGLOG_PROCESS_ERROR(bRetCode);

    pPlayerSkill = &it->second;

    pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW_S(dwSkillID, pPlayerSkill->dwLevel);
    KG_PROCESS_ERROR(pSkill);

    KG_PROCESS_ERROR(pPlayerSkill->dwLevel < pSkill->m_pBaseInfo->dwMaxLevel);

    KG_PROCESS_ERROR(pSkill->m_dwLevelUpExp);    // Skill is not realizable when LevelUpExp equals to zero. 

    KG_PROCESS_ERROR(pPlayerSkill->dwExp >= pSkill->m_dwLevelUpExp);

    KG_PROCESS_ERROR(m_pPlayer->m_nLevel >= pSkill->m_nPlayerLevelLimit);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillList::CanLearnSkill(DWORD dwSkillID, DWORD dwMasterID)
{
    BOOL                        bResult             = false;
    BOOL                        bRetCode            = false;
    DWORD                       dwSkillLevel        = 0;
    DWORD                       dwSkillExp          = 0;
    KSKILL_LEARNING_INFO*       pSkillLearningInfo  = NULL;
	PLAYER_SKILL_LIST::iterator it;

    KGLOG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);

    it = m_PlayerSkillList.find(dwSkillID);
    if (it == m_PlayerSkillList.end())
    {
        int     nSkillCount = 0;
        KSkill* pSkill      = NULL;

        nSkillCount = (int)m_PlayerSkillList.size();
        KGLOG_PROCESS_ERROR(nSkillCount < MAX_PLAYER_SKILL_COUNT);

        pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, 1);
        KGLOG_PROCESS_ERROR(pSkill);

        if (pSkill->m_pBaseInfo->dwBelongKungfu != INVALID_SKILL_ID)
        {
            DWORD dwKungfuLevel = GetSkillLevel(pSkill->m_pBaseInfo->dwBelongKungfu);
            KG_PROCESS_ERROR(dwKungfuLevel);
        }

        dwSkillLevel = 1;
    }
    else
    {
        KPlayerSkill* pPlayerSkill = NULL;
        pPlayerSkill = &it->second;

        dwSkillLevel = pPlayerSkill->dwLevel + 1;
        dwSkillExp = pPlayerSkill->dwExp;
    }

    pSkillLearningInfo = g_pSO3World->m_SkillManager.GetSkillLearningInfo(dwMasterID, dwSkillID, dwSkillLevel);
    KG_PROCESS_ERROR(pSkillLearningInfo);

    KG_PROCESS_ERROR(dwSkillLevel == 1 || dwSkillExp >= pSkillLearningInfo->dwRequireExp);

    KG_PROCESS_ERROR(m_pPlayer->m_nLevel >= pSkillLearningInfo->nRequirePlayerLevel);

    if (pSkillLearningInfo->dwReputationID)
    {
        int nReputationLevel = m_pPlayer->m_ReputeList.GetReputeLevel(pSkillLearningInfo->dwReputationID);
        KG_PROCESS_ERROR(nReputationLevel >= pSkillLearningInfo->nReputationLevel);
    }

    if (pSkillLearningInfo->nPrice)
    {
        int nMoney = m_pPlayer->m_ItemList.GetMoney();
        KG_PROCESS_ERROR(nMoney >= pSkillLearningInfo->nPrice);
    }

    if (pSkillLearningInfo->nCostTrain)
    {
        KG_PROCESS_ERROR(m_pPlayer->m_nCurrentTrainValue >= pSkillLearningInfo->nCostTrain);
    }

    bResult = true;
Exit0:
    return bResult;
}


BOOL KSkillList::UpdateSkill(DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwExp, BOOL bNotifyPlayer /* = true */)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KSkill*                     pOldSkill       = NULL;
    KSkill*                     pNewSkill       = NULL;
    DWORD                       dwOldKungfuID   = INVALID_SKILL_ID;
    int                         nSkillCount     = 0;
    KSKILL_RECIPE_KEY           SkillRecipeKey;
	PLAYER_SKILL_LIST::iterator it;

    assert(m_pPlayer);

    if (dwSkillLevel)
    {
        bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, dwSkillID, dwSkillLevel, m_pPlayer);
        KGLOG_PROCESS_ERROR(bRetCode);

        pNewSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
        // 这里 pNewSkill 只能获取“技能”的指针，不能获取“套路”的指针，所以为NULL也是正常的。
    }

	it = m_PlayerSkillList.find(dwSkillID);
	if (it == m_PlayerSkillList.end())
	{
	    std::pair<PLAYER_SKILL_LIST::iterator, bool> InsRet;
		KPlayerSkill PlayerSkill;

        KGLOG_PROCESS_ERROR(dwSkillLevel > 0);

        nSkillCount = (int)m_PlayerSkillList.size();
        KGLOG_PROCESS_ERROR(nSkillCount < MAX_PLAYER_SKILL_COUNT);

		PlayerSkill.dwLevel    = dwSkillLevel;
		PlayerSkill.dwExp      = dwExp;

        InsRet = m_PlayerSkillList.insert(std::make_pair(dwSkillID, PlayerSkill));
		KGLOG_PROCESS_ERROR(InsRet.second);
	}
    else
    {
        KPlayerSkill* pPlayerSkill = NULL;

        pPlayerSkill = &it->second;

        bRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, dwSkillID, pPlayerSkill->dwLevel, m_pPlayer);
        KGLOG_PROCESS_ERROR(bRetCode);

        pOldSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
        // 这里 pOldSkill 只能获取“技能”的指针，不能获取“套路”的指针，所以为NULL也是正常的。

        if (dwSkillLevel > 0)
        {
            pPlayerSkill            = &it->second;
            pPlayerSkill->dwLevel   = dwSkillLevel;
            pPlayerSkill->dwExp     = dwExp;
        }
        else
        {
            pPlayerSkill = NULL;

            for (int i = 0; i < MAX_SKILL_REICPE_COUNT; i++)
            {
                KSKILL_RECIPE_BASE_INFO* pSkillRecipe = NULL;

                if (SkillRecipeKey.dwRecipeKey[i] == 0)
                    continue;

                pSkillRecipe = g_pSO3World->m_SkillManager.GetSkillRecipeBaseInfo(SkillRecipeKey.dwRecipeKey[i]);
                KGLOG_PROCESS_ERROR(pSkillRecipe);

                if (!pSkillRecipe->bIsSystemRecipe && !pSkillRecipe->nSkillRecipeType && pSkillRecipe->dwSkillID == dwSkillID)
                    m_pPlayer->m_SkillRecipeList.Deactive(SkillRecipeKey.dwRecipeKey[i]);

            }

            m_PlayerSkillList.erase(it);
        }
    }

#ifdef _SERVER
    g_PlayerServer.DoSyncSkillData(m_pPlayer->m_nConnIndex, dwSkillID, dwSkillLevel, dwExp, bNotifyPlayer);
#endif

    // 装备套路

    dwOldKungfuID = m_dwMountKungfuID;
    if (dwSkillID == m_dwMountKungfuID)
    {
        bRetCode = m_pPlayer->UmountKungfu();
        KGLOG_CHECK_ERROR(bRetCode);
    }

    if (dwSkillID == dwOldKungfuID && dwSkillLevel)
    {
        bRetCode = m_pPlayer->MountKungfu(dwSkillID, dwSkillLevel);
        KGLOG_CHECK_ERROR(bRetCode);
    }

    KG_PROCESS_SUCCESS(pNewSkill == pOldSkill);

    // 对Player应用被动技能上带的属性

    if (pOldSkill && pOldSkill->m_pBaseInfo->bIsPassiveSkill)
        m_pPlayer->UnApplyAttribute(pOldSkill->m_pAttrbuteEffectToSelfAndRollback);

    if (pNewSkill && pNewSkill->m_pBaseInfo->bIsPassiveSkill)
        m_pPlayer->ApplyAttribute(pNewSkill->m_pAttrbuteEffectToSelfAndRollback);

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

BOOL KSkillList::IsSkillExist(DWORD dwID, DWORD dwLevel)
{
    BOOL    bResult         = false;
    DWORD   dwLearnLevel    = GetSkillLevel(dwID);

    KG_PROCESS_ERROR(dwLevel > 0);
    KG_PROCESS_ERROR(dwLearnLevel > 0);
    KG_PROCESS_ERROR(dwLevel == dwLearnLevel);

    bResult = true;
Exit0:
    return bResult;
}

DWORD KSkillList::GetSkillExp(DWORD dwSkillID)
{
	PLAYER_SKILL_LIST::iterator it = m_PlayerSkillList.find(dwSkillID);

	KG_PROCESS_ERROR(it != m_PlayerSkillList.end());

	return it->second.dwExp;
Exit0:
	return 0;
}


DWORD KSkillList::GetSkillLevel(DWORD dwSkillID)
{
	PLAYER_SKILL_LIST::iterator it = m_PlayerSkillList.find(dwSkillID);

	KG_PROCESS_ERROR(it != m_PlayerSkillList.end());

	return it->second.dwLevel;
Exit0:
	return 0;
}

int KSkillList::_LuaGetSkillSchoolList(Lua_State* L)
{
	std::map<DWORD, int> SchoolList;
	for (PLAYER_SKILL_LIST::iterator it = m_PlayerSkillList.begin(); it != m_PlayerSkillList.end(); ++it)
	{
		KSkill* pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(it->first, it->second.dwLevel);
		if(pSkill)
			SchoolList[pSkill->m_pBaseInfo->dwBelongSchool] = true;
	}

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);
	int nIndex = 0;
	for (std::map<DWORD, int>::iterator it = SchoolList.begin(); it != SchoolList.end(); ++it)
	{
		++nIndex;
		Lua_PushNumber(L, nIndex);
		Lua_PushNumber(L, it->first);
		Lua_SetTable(L, nTable);
	}

	return 1;
}

int KSkillList::_LuaGetSkillKungfuList(Lua_State* L, DWORD dwSchollID)
{
	std::map<DWORD, DWORD> KungfuList;
	for (PLAYER_SKILL_LIST::iterator it = m_PlayerSkillList.begin(); it != m_PlayerSkillList.end(); ++it)
	{
		KSkill* pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(it->first, it->second.dwLevel);
		if(pSkill && 
			dwSchollID == pSkill->m_pBaseInfo->dwBelongSchool && 
			pSkill->m_pBaseInfo->dwBelongKungfu == 0)
		{
			KungfuList[it->first] = it->second.dwLevel;
		}
	}

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);
	for (std::map<DWORD, DWORD>::iterator it = KungfuList.begin(); it != KungfuList.end(); ++it)
	{
		Lua_PushNumber(L, it->first);
		Lua_PushNumber(L, it->second);
		Lua_SetTable(L, nTable);
	}

	return 1;
}


int KSkillList::_LuaGetSkillList(Lua_State* L, DWORD dwKungfuID)
{
	std::map<DWORD, DWORD> SkillList;
	for (PLAYER_SKILL_LIST::iterator it = m_PlayerSkillList.begin(); it != m_PlayerSkillList.end(); ++it)
	{
		KSkill* pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(it->first, it->second.dwLevel);
		if(pSkill && pSkill->m_pBaseInfo->dwBelongKungfu == dwKungfuID)
		{
			SkillList[it->first] = it->second.dwLevel;
		}
	}

	Lua_NewTable(L);
	int nTable = Lua_GetTopIndex(L);
	for (std::map<DWORD, DWORD>::iterator it = SkillList.begin(); it != SkillList.end(); ++it)
	{
		Lua_PushNumber(L, it->first);
		Lua_PushNumber(L, it->second);
		Lua_SetTable(L, nTable);
	}

	return 1;
}

int	KSkillList::_LuaGetAllSkillList(Lua_State* L)
{
	Lua_NewTable(L);
	for (PLAYER_SKILL_LIST::iterator it = m_PlayerSkillList.begin(); it != m_PlayerSkillList.end(); ++it)
	{
		Lua_PushNumber(L, it->first);
		Lua_PushNumber(L, it->second.dwLevel);
		Lua_SetTable(L, -3);
	}
	return 1;
}

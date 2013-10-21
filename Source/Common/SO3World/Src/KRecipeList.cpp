#include "stdafx.h"
#include "KRecipeList.h"
#include "KCraft.h"
#include "KRecipe.h"
#include "KPlayerServer.h"
#include "KRecipeMaster.h"
#include "KSO3World.h"

BOOL KRecipeList::Init(KPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	ZeroMemory(m_byRecipeState, sizeof(m_byRecipeState));

	return true;
}

void KRecipeList::UnInit()
{
    m_pPlayer = NULL;
	return;
}

BOOL KRecipeList::Load(BYTE* pbyData, size_t uDataLen, int nVersion)
{
    BOOL  bResult = false;
    BOOL  bRetCode = false;

    if (nVersion == RECIPE_LIST_VERSION_V0)
    {
        bRetCode = Load_V0(pbyData, uDataLen);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else if (nVersion == CURRENT_RECIPE_LIST_VERSION)
    {
        bRetCode = Load_V1(pbyData, uDataLen);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRecipeList::Load_V1(BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult         = false;
    int             nMaxRecipeIndex = (MAX_RECIPE_ID + CHAR_BIT - 1) / CHAR_BIT;
    KRecipeDBData*  pRecipeDBData   = (KRecipeDBData*)pbyData;
    KRecipeDBItem*  pRecipeDBItem   = NULL;
    BOOL            bRecipeLearned  = false;

    assert(pbyData);

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KRecipeDBData) + pRecipeDBData->byCount * sizeof(KRecipeDBItem));
    
    KGLOG_PROCESS_ERROR(pRecipeDBData->byCount <= MAX_CRAFT_COUNT);

    for (int i = 0; i < pRecipeDBData->byCount; ++i)
    {
        pRecipeDBItem = &(pRecipeDBData->RecipeDBItem[i]);

        KGLOG_PROCESS_ERROR(pRecipeDBItem->byCraftID >= 1);
        KGLOG_PROCESS_ERROR(pRecipeDBItem->byCraftID <= MAX_CRAFT_COUNT);

        for (int j = 0; j < nMaxRecipeIndex; ++j)
        {
            BYTE byBitData = pRecipeDBItem->byRecipeData[j];

            for (int nSubIndex = 0; nSubIndex < CHAR_BIT; ++nSubIndex)
            {
                int nRecipeIndex = j * CHAR_BIT + nSubIndex;
                if (nRecipeIndex < MAX_RECIPE_ID)
                {
                    bRecipeLearned = (byBitData & (1 << (CHAR_BIT - nSubIndex - 1))) ? true : false;
                    m_byRecipeState[pRecipeDBItem->byCraftID - 1][nRecipeIndex] = (BYTE)bRecipeLearned;
                }
            }
        }
    }
    
#ifdef _SERVER
    g_PlayerServer.DoSyncRecipeState(m_pPlayer->m_nConnIndex, pbyData, uDataLen, CURRENT_RECIPE_LIST_VERSION);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRecipeList::Load_V0(BYTE* pbyData, size_t uDataLen)
{
    BOOL                bResult         = false;
    int                 nMaxRecipeIndex = (OLD_MAX_RECIPE_ID + CHAR_BIT - 1) / CHAR_BIT;
    KRecipeDBData_V0*   pRecipeDBData   = (KRecipeDBData_V0*)pbyData;
    KRecipeDBItem_V0*   pRecipeDBItem   = NULL;
    BOOL                bRecipeLearned  = false;

    assert(pbyData);

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KRecipeDBData_V0) + pRecipeDBData->byCount * sizeof(KRecipeDBItem_V0));

    KGLOG_PROCESS_ERROR(pRecipeDBData->byCount <= MAX_CRAFT_COUNT);

    for (int i = 0; i < pRecipeDBData->byCount; ++i)
    {
        pRecipeDBItem = &(pRecipeDBData->RecipeDBItem[i]);

        KGLOG_PROCESS_ERROR(pRecipeDBItem->byCraftID >= 1);
        KGLOG_PROCESS_ERROR(pRecipeDBItem->byCraftID <= MAX_CRAFT_COUNT);

        for (int j = 0; j < nMaxRecipeIndex; ++j)
        {
            BYTE byBitData = pRecipeDBItem->byRecipeData[j];

            for (int nSubIndex = 0; nSubIndex < CHAR_BIT; ++nSubIndex)
            {
                int nRecipeIndex = j * CHAR_BIT + nSubIndex;
                if (nRecipeIndex < OLD_MAX_RECIPE_ID)
                {
                    bRecipeLearned = (byBitData & (1 << (CHAR_BIT - nSubIndex - 1))) ? true : false;
                    m_byRecipeState[pRecipeDBItem->byCraftID - 1][nRecipeIndex] = (BYTE)bRecipeLearned;
                }
            }
        }
    }

#ifdef _SERVER
    g_PlayerServer.DoSyncRecipeState(m_pPlayer->m_nConnIndex, pbyData, uDataLen, RECIPE_LIST_VERSION_V0);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KRecipeList::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL            bResult	        = false;
	int		        nMaxRecipeIndex = (MAX_RECIPE_ID + CHAR_BIT - 1) / CHAR_BIT;
    size_t          uLeftSize       = uBufferSize;
    KRecipeDBItem*  pRecipeDBItem   = NULL;
    KRecipeDBData*  pRecipeDBData   = NULL;
    int             nSaveCraftIndex = 0;

    assert(puUsedSize);
    assert(pbyBuffer);
    
    pRecipeDBData = (KRecipeDBData*)pbyBuffer;
    uLeftSize -= sizeof(KRecipeDBData);

    for (int i = 0; i < MAX_CRAFT_COUNT; ++i)
    {
	    DWORD dwCraftID	    = i + 1;
    	DWORD dwRecipeCount	= GetRecipeCountByCraftID(dwCraftID);
        
        if (dwRecipeCount == 0)
        {
            continue;
        }
        
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KRecipeDBItem));
        pRecipeDBItem = &(pRecipeDBData->RecipeDBItem[nSaveCraftIndex++]);
        uLeftSize -= sizeof(KRecipeDBItem);
        
        pRecipeDBItem->byCraftID = (BYTE)dwCraftID;

        for (int nRecipeIndex = 0; nRecipeIndex < nMaxRecipeIndex; ++nRecipeIndex)
        {
            BYTE byBitData = 0;
            for (int nIndex = 0; nIndex < CHAR_BIT; nIndex++)
            {
                int nRecipeID = nRecipeIndex * CHAR_BIT + nIndex;
				if (nRecipeID < MAX_RECIPE_ID && m_byRecipeState[i][nRecipeID])
				{
					byBitData |= 1 << (CHAR_BIT - nIndex - 1);
				}
            }

            pRecipeDBItem->byRecipeData[nRecipeIndex] = byBitData;
        }
    }
    
    pRecipeDBData->byCount = (BYTE)nSaveCraftIndex;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

//能否学习新配方
BOOL KRecipeList::CanLearnRecipe(DWORD dwCraftID, DWORD dwRecipeID, KRecipeLearningInfo* pLearningInfo)
{
    BOOL            bResult             = false;
	int             nProfessionLevel    = 0;
    int             nMaxLevel           = 0;
	KCraft*         pCraft              = NULL;
	KRecipeBase*    pRecipe             = NULL;
	DWORD           dwBranchID          = 0;
	DWORD           dwMoney             = 0; 

	KGLOG_PROCESS_ERROR(pLearningInfo);
	KG_PROCESS_ERROR(dwCraftID > 0);
	KG_PROCESS_ERROR(dwRecipeID > 0);
	KGLOG_PROCESS_ERROR(dwCraftID <= MAX_CRAFT_COUNT);
	KGLOG_PROCESS_ERROR(dwRecipeID <= MAX_RECIPE_ID);
	KGLOG_PROCESS_ERROR(m_pPlayer);

	dwMoney = m_pPlayer->m_ItemList.GetMoney();
	KG_PROCESS_ERROR((int)dwMoney >= pLearningInfo->nPrice);

	// 保证已经学会相应的Profession并且等级足够
	pCraft = g_pSO3World->m_ProfessionManager.GetCraft(dwCraftID);
	KG_PROCESS_ERROR(pCraft);

	pRecipe = pCraft->GetRecipe(dwRecipeID);
	KG_PROCESS_ERROR(pRecipe);

	nProfessionLevel   = m_pPlayer->m_ProfessionList.GetProfessionLevel(pRecipe->dwProfessionID);
    nProfessionLevel  += m_pPlayer->m_ProfessionList.GetProfessionAdjustLevel(pRecipe->dwProfessionID);
    nMaxLevel          = m_pPlayer->m_ProfessionList.GetProfessionMaxLevel(pRecipe->dwProfessionID);
    
    // 这里策划不希望玩家能通过加生活技能的东西来突破技能限制
    nProfessionLevel   = min(nMaxLevel, nProfessionLevel); 

	KG_PROCESS_ERROR(nProfessionLevel > 0);
	KG_PROCESS_ERROR(nProfessionLevel >= (int)pRecipe->dwRequireProfessionLevel);

	dwBranchID = m_pPlayer->m_ProfessionList.GetBranchID(pRecipe->dwProfessionID);

	if (pRecipe->eCraftType == actProduce)
	{
		KCraftRecipe* pCraftRecipe = (KCraftRecipe*)pRecipe;
		KG_PROCESS_ERROR(pCraftRecipe->dwRequireBranchID == 0 || pCraftRecipe->dwRequireBranchID == dwBranchID);
	}

	if (pRecipe->eCraftType == actEnchant)
	{
		KCraftEnchant* pCraftEnchant = (KCraftEnchant*)pRecipe;
		KG_PROCESS_ERROR(pCraftEnchant->dwRequireBranchID == 0 || pCraftEnchant->dwRequireBranchID == dwBranchID);
	}

	// 保证配方还没学
	KG_PROCESS_ERROR(m_byRecipeState[dwCraftID - 1][dwRecipeID - 1] == false);

	bResult = true;
Exit0:
	return bResult;
}

//学习新配方
BOOL KRecipeList::LearnRecipe(DWORD dwCraftID, DWORD dwRecipeID)
{
	BOOL bResult = false;

	KG_PROCESS_ERROR(dwCraftID > 0);
	KG_PROCESS_ERROR(dwRecipeID > 0);
	KGLOG_PROCESS_ERROR(dwCraftID <= MAX_CRAFT_COUNT);
	KGLOG_PROCESS_ERROR(dwRecipeID <= MAX_RECIPE_ID);
	KGLOG_PROCESS_ERROR(m_pPlayer);

	m_byRecipeState[dwCraftID - 1][dwRecipeID - 1] = true;

	bResult = true;
Exit0:
	return bResult;
}

//遗忘某个Craft下的所有配方
BOOL KRecipeList::ForgetRecipe(DWORD dwCraftID)
{
	BOOL    bResult     = false;
	int     nIndex      = 0;

	KG_PROCESS_ERROR(dwCraftID > 0);
	KGLOG_PROCESS_ERROR(dwCraftID <= MAX_CRAFT_COUNT);
	KGLOG_PROCESS_ERROR(m_pPlayer);

	ZeroMemory(m_byRecipeState[dwCraftID - 1], sizeof(m_byRecipeState[dwCraftID - 1]));

	bResult = true;
Exit0:
	return bResult;
}

BOOL KRecipeList::IsRecipeLearned(DWORD dwCraftID, DWORD dwRecipeID)
{
	BOOL bResult = false;

	KG_PROCESS_ERROR(dwCraftID > 0);
	KG_PROCESS_ERROR(dwRecipeID > 0);
	KGLOG_PROCESS_ERROR(dwCraftID <= MAX_CRAFT_COUNT);
	KGLOG_PROCESS_ERROR(dwRecipeID <= MAX_RECIPE_ID);
	KGLOG_PROCESS_ERROR(m_pPlayer);

	KG_PROCESS_ERROR(m_byRecipeState[dwCraftID - 1][dwRecipeID - 1]);

	bResult = true;
Exit0:
	return bResult;
}

DWORD KRecipeList::GetRecipeCountByCraftID(DWORD dwCraftID)
{
	DWORD   dwResult    = 0;
	int     nIndex      = 0;

	KG_PROCESS_ERROR(dwCraftID > 0);
	KGLOG_PROCESS_ERROR(dwCraftID <= MAX_CRAFT_COUNT);

	for (nIndex = 0; nIndex < MAX_RECIPE_ID; nIndex++)
	{
		if (m_byRecipeState[dwCraftID - 1][nIndex])
			dwResult++;
	}

Exit0:
	return dwResult;
}

BOOL KRecipeList::GetRecipeList(DWORD dwCraftID, std::vector<DWORD>& vecRecipeIDList)
{
	BOOL    bResult = false;
	int     nIndex  = 0;

	KG_PROCESS_ERROR(dwCraftID > 0);
	KGLOG_PROCESS_ERROR(dwCraftID <= MAX_CRAFT_COUNT);

	vecRecipeIDList.clear();

	for (nIndex = 0; nIndex < MAX_RECIPE_ID; nIndex++)
	{
		if (m_byRecipeState[dwCraftID - 1][nIndex])
		{
			vecRecipeIDList.push_back(nIndex + 1);
		}
	}

	bResult = true;
Exit0:
	return bResult;
}

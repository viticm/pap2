#include "stdafx.h"
#include "KCraft.h"
#include "KLootList.h"
#include "KPlayerServer.h"
#include "KSO3World.h"
#include "KDoodad.h"

KCraft::KCraft(void)
{
	memset(m_szName, 0, sizeof(m_szName));
	m_eCraftType = actInvalid;
	m_dwID = 0;
	m_dwProfessionID = 0;
	m_bHide = 0;
}

BOOL KCraft::Init(const char* szFileName)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KGLOG_PROCESS_ERROR(szFileName);
	
	if (szFileName[0] != '\0')
	{
		switch(m_eCraftType)
		{
		case actCollection:
			bRetCode = m_CraftCollection.Init(szFileName);
			KGLOG_PROCESS_ERROR(bRetCode);
			break;
		case actProduce:
			bRetCode = m_CraftRecipe.Init(szFileName);
			KGLOG_PROCESS_ERROR(bRetCode);
			break;
		case actRead:
			bRetCode = m_CraftRead.Init(szFileName);
			KGLOG_PROCESS_ERROR(bRetCode);
			break;
		case actEnchant:
			bRetCode = m_CraftEnchant.Init(szFileName);
			KGLOG_PROCESS_ERROR(bRetCode);
			break;
		case actRadar:
			//nothing
			break;
        case actCopy:
            bRetCode = m_CraftCopy.Init(szFileName);
			KGLOG_PROCESS_ERROR(bRetCode);
            break;
		default:
			KGLOG_PROCESS_ERROR(false && "ERROR CRAFT TYPE!");
		    break;
		}
	}

	bResult = true;
Exit0:
	return bResult;
}

void KCraft::UnInit()
{
    BOOL bRetCode = false;

	switch(m_eCraftType)
	{
	case actCollection:
		bRetCode = m_CraftCollection.UnInit();
		KGLOG_PROCESS_ERROR(bRetCode);
		break;
	case actProduce:
		bRetCode = m_CraftRecipe.UnInit();
		KGLOG_PROCESS_ERROR(bRetCode);
	    break;
	case actRead:
		bRetCode = m_CraftRead.UnInit();
		KGLOG_PROCESS_ERROR(bRetCode);
		break;
	case actEnchant:
		bRetCode = m_CraftEnchant.UnInit();
		KGLOG_PROCESS_ERROR(bRetCode);
		break;
	case actRadar:
		break;
    case actCopy:
        bRetCode = m_CraftCopy.UnInit();
		KGLOG_PROCESS_ERROR(bRetCode);
        break;
	default:
		KGLOG_PROCESS_ERROR(false && "ERROR CRAFT TYPE!");
	    break;
	}

Exit0:
	return;
}

BOOL KCraft::IsRecipeExist(DWORD dwRecipeID)
{
	BOOL            bResult     = false;
	KRecipeBase*    pRecipe     = NULL;

	switch(m_eCraftType)
	{
	case actCollection:
	case actRadar:
		//这个分支一定返回false
		goto Exit0;
		break;
	case actProduce:
		pRecipe = m_CraftRecipe.GetRecipe(dwRecipeID);
		KG_PROCESS_ERROR(pRecipe);
	    break;
	case actRead:
		pRecipe = m_CraftRead.GetRecipe(dwRecipeID);
		KG_PROCESS_ERROR(pRecipe);
		break;
	case actEnchant:
		pRecipe = m_CraftEnchant.GetRecipe(dwRecipeID);
		KGLOG_PROCESS_ERROR(pRecipe);
		break;
    case actCopy:
		pRecipe = m_CraftCopy.GetRecipe(dwRecipeID);
		KGLOG_PROCESS_ERROR(pRecipe);
        break;
	default:
		KGLOG_PROCESS_ERROR(false && "ERROR CRAFT TYPE!");
	    break;
	}

	bResult = true;
Exit0:
	return bResult;
}

KRecipeBase* KCraft::GetRecipe(DWORD dwRecipeID)
{
	KRecipeBase* pRecipe = NULL;

	switch(m_eCraftType)
	{
	case actCollection:
		pRecipe = m_CraftCollection.GetRecipe(dwRecipeID);
		break;
	case actProduce:
		pRecipe = m_CraftRecipe.GetRecipe(dwRecipeID);
	    break;
	case actRead:
		pRecipe = m_CraftRead.GetRecipe(dwRecipeID);
		break;
	case actEnchant:
		pRecipe = m_CraftEnchant.GetRecipe(dwRecipeID);
		break;
	case actRadar:
		break;
    case actCopy:
		pRecipe = m_CraftCopy.GetRecipe(dwRecipeID);
		break;
	default:
		KGLOG_PROCESS_ERROR(false && "ERROR CRAFT TYPE!");
	    break;
	}
Exit0:
	return pRecipe;
}

CRAFT_RESULT_CODE KCraft::CanCast(KPlayer* pCaster, DWORD dwRecipeID, DWORD dwSourceItemID, KTarget& Target)
{
	CRAFT_RESULT_CODE   nResult     = crcFailed;
    CRAFT_RESULT_CODE   eRetCode    = crcFailed;
	BOOL                bRetCode    = false;
	KRecipeBase*        pRecipe     = NULL;
	TARGET_TYPE         eTargetType = Target.GetTargetType();

	KGLOG_PROCESS_ERROR(eTargetType != ttInvalid);
	KGLOG_PROCESS_ERROR(pCaster);

	switch(m_eCraftType)
	{
	case actCollection:
		{
			KDoodad*            pDoodad             = NULL;
			KCraftCollection*   pCraftCollection    = NULL;

			KG_PROCESS_ERROR(eTargetType == ttDoodad);

			bRetCode = Target.GetTarget(&pDoodad);
			KGLOG_PROCESS_ERROR(bRetCode && pDoodad);

			//是否在采集表中
			pRecipe = m_CraftCollection.GetRecipe(pDoodad->m_dwTemplateID);
			KG_PROCESS_ERROR(pRecipe);
			KGLOG_PROCESS_ERROR(pRecipe->eCraftType == actCollection);

			pCraftCollection = (KCraftCollection*)pRecipe;
			KGLOG_PROCESS_ERROR(pCraftCollection);

			eRetCode = pCraftCollection->CanCast(pCaster, Target);
			KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
		}
		break;
	case actProduce:
		{
			KCraftRecipe* pCraftRecipe = NULL;

			//合成类技能
			KG_PROCESS_ERROR(eTargetType == ttNoTarget);
			KGLOG_PROCESS_ERROR(dwRecipeID != 0);

			pRecipe = m_CraftRecipe.GetRecipe(dwRecipeID);
			KG_PROCESS_ERROR(pRecipe);
			KGLOG_PROCESS_ERROR(pRecipe->eCraftType == actProduce);

			pCraftRecipe = (KCraftRecipe*)pRecipe;
			KGLOG_PROCESS_ERROR(pCraftRecipe);

			// 身上是否学会了相应的配方
			bRetCode = pCaster->m_RecipeList.IsRecipeLearned(m_dwID, dwRecipeID);
			KGLOG_PROCESS_ERROR(bRetCode);

			eRetCode = pCraftRecipe->CanCast(pCaster, Target);
			KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
		}
		break;
	case actRead:
		{
			KItem*      pBookItem   = NULL;
			KCraftRead* pCraftRead  = NULL;

			KGLOG_PROCESS_ERROR(eTargetType == ttItem);

			bRetCode = Target.GetTarget(&pBookItem);
			KGLOG_PROCESS_ERROR(bRetCode && pBookItem);
			KGLOG_PROCESS_ERROR(dwRecipeID == (DWORD)pBookItem->m_nCurrentDurability);

			pRecipe = m_CraftRead.GetRecipe(pBookItem->m_nCurrentDurability);
			KGLOG_PROCESS_ERROR(pRecipe);
			KGLOG_PROCESS_ERROR(pRecipe->eCraftType == actRead);

			pCraftRead = (KCraftRead*)pRecipe;
			KGLOG_PROCESS_ERROR(pCraftRead);

			eRetCode = pCraftRead->CanCast(pCaster, Target);
			KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
		}
		break;
	case actEnchant:
		{
			DWORD           dwBoxIndex		= 0;
			DWORD           dwX				= 0;
			KCraftEnchant*  pCraftEnchant	= NULL;

			KGLOG_PROCESS_ERROR(dwRecipeID);

			pRecipe = m_CraftEnchant.GetRecipe(dwRecipeID);
			KGLOG_PROCESS_ERROR(pRecipe);
			KGLOG_PROCESS_ERROR(pRecipe->eCraftType == actEnchant);

			pCraftEnchant = (KCraftEnchant*)pRecipe;
			KGLOG_PROCESS_ERROR(pCraftEnchant);

			bRetCode = pCaster->m_RecipeList.IsRecipeLearned(m_dwID, dwRecipeID);
			KGLOG_PROCESS_ERROR(bRetCode);

			eRetCode = pCraftEnchant->CanCast(pCaster, Target);
			KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
		}
		break;
	case actRadar:
		break;
    case actCopy:
		{
			KCraftCopy* pCraftCopy = NULL;

			KG_PROCESS_ERROR(eTargetType == ttNoTarget || eTargetType == ttDoodad);
			KGLOG_PROCESS_ERROR(dwRecipeID != 0);

			pRecipe = m_CraftCopy.GetRecipe(dwRecipeID);
			KG_PROCESS_ERROR_RET_CODE(pRecipe, crcBookCannotBeCopy);
			KGLOG_PROCESS_ERROR(pRecipe->eCraftType == actCopy);

			pCraftCopy = (KCraftCopy*)pRecipe;
			KGLOG_PROCESS_ERROR(pCraftCopy);

			eRetCode = pCraftCopy->CanCast(pCaster, Target);
			KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
		}
		break;
	default:
		KGLOG_PROCESS_ERROR(false && "ERROR CRAFT TYPE!");
		break;
	}
    
    KG_PROCESS_ERROR_RET_CODE(
        pCaster->m_eMoveState == cmsOnStand || pCaster->m_eMoveState == cmsOnFloat || 
        (pCaster->m_eMoveState == cmsOnSit && m_eCraftType == actRead),              // 阅读可以坐着
        crcMoveStateError
    );

	nResult = crcSuccess;
Exit0:
	return nResult;
}

#ifdef _SERVER
CRAFT_RESULT_CODE KCraft::Cast(KPlayer* pCaster, DWORD dwRecipeID, DWORD dwSourceItemID, KTarget& Target)
{
	CRAFT_RESULT_CODE nResult = crcFailed;
	BOOL bRetCode = false;
	TARGET_TYPE eTargetType = Target.GetTargetType();
	KGLOG_PROCESS_ERROR(eTargetType != ttInvalid);
	KGLOG_PROCESS_ERROR(pCaster);

	nResult = CanCast(pCaster, dwRecipeID, dwSourceItemID, Target);
	KG_PROCESS_ERROR(nResult == crcSuccess);

	switch(m_eCraftType)
	{
	case actCollection:
		{
			KDoodad* pDoodad = NULL;

			KGLOG_PROCESS_ERROR(eTargetType == ttDoodad);

			bRetCode = Target.GetTarget(&pDoodad);
			KGLOG_PROCESS_ERROR(bRetCode && pDoodad);

			nResult = Collection(pCaster, pDoodad);
			KG_PROCESS_ERROR(nResult == crcSuccess);
		}
		break;
	case actProduce:
		{
			KGLOG_PROCESS_ERROR(eTargetType == ttNoTarget);
			KGLOG_PROCESS_ERROR(dwRecipeID != 0);

			nResult = Produce(pCaster, dwRecipeID);
			KG_PROCESS_ERROR(nResult == crcSuccess);

            g_pSO3World->m_StatDataServer.UpdateRecipeCastState(m_dwID, dwRecipeID);
		}
		break;
	case actRead:
		{
			nResult = MemorizeBook(pCaster, Target);
			KG_PROCESS_ERROR(nResult == crcSuccess);
		}
		break;
	case actEnchant:
		{
			KItem* pTargetItem = NULL;

			KGLOG_PROCESS_ERROR(dwRecipeID);

			bRetCode = Target.GetTarget(&pTargetItem);
			KGLOG_PROCESS_ERROR(bRetCode && pTargetItem);

			nResult = Enchant(pCaster, dwRecipeID, pTargetItem);
			KG_PROCESS_ERROR(nResult == crcSuccess);

            g_pSO3World->m_StatDataServer.UpdateRecipeCastState(m_dwID, dwRecipeID);
		}
		break;
	case actRadar:
		break;
    case actCopy:
		{
            if (eTargetType == ttDoodad) // 抄录doodad
            {
                KDoodad* pDoodad = NULL;

                bRetCode = Target.GetTarget(&pDoodad);
                KGLOG_PROCESS_ERROR(bRetCode && pDoodad);                
            }
            
			nResult = Copy(pCaster, dwRecipeID);
			KG_PROCESS_ERROR(nResult == crcSuccess);
		}
		break;
	default:
		KGLOG_PROCESS_ERROR(false && "ERROR CRAFT TYPE!");
		break;
	}

	nResult= crcSuccess;
Exit0:
	if (pCaster)
	{
		KCraftCastState CraftCastState;
		CraftCastState.dwCraftID = m_dwID;
		CraftCastState.dwRecipeID = dwRecipeID;
		CraftCastState.eTargetType = eTargetType;
		CraftCastState.dwTargetID = Target.GetTargetID();

		g_PlayerServer.DoMessageNotify(pCaster->m_nConnIndex, ectCraftErrorCode, nResult, (void*)&CraftCastState, sizeof(CraftCastState));
	}
	return nResult;
}	

void KCraft::CallBeginScriptFunction(KPlayer* pPlayer, DWORD dwRecipeID)
{
    BOOL            bRetCode    = false;
    KRecipeBase*    pRecipe     = NULL;
    int             nTopIndex   = 0;

    assert(pPlayer);

    pRecipe = GetRecipe(dwRecipeID);
    KGLOG_PROCESS_ERROR(pRecipe);

    KG_PROCESS_ERROR(pRecipe->dwScriptID);

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pRecipe->dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pRecipe->dwScriptID, SCRIPT_ON_BEGIN_CAST_CRAFT);
    KG_PROCESS_ERROR(bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
    g_pSO3World->m_ScriptCenter.PushValueToStack(m_dwID);
    g_pSO3World->m_ScriptCenter.PushValueToStack(dwRecipeID);

	g_pSO3World->m_ScriptCenter.CallFunction(pRecipe->dwScriptID, SCRIPT_ON_BEGIN_CAST_CRAFT, 0);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

Exit0:
    return;
}

void KCraft::CallBreakScriptFunction(KPlayer* pPlayer, DWORD dwRecipeID)
{
    BOOL            bRetCode    = false;
    KRecipeBase*    pRecipe     = NULL;
    int             nTopIndex   = 0;

    assert(pPlayer);

    pRecipe = GetRecipe(dwRecipeID);
    KGLOG_PROCESS_ERROR(pRecipe);

    KG_PROCESS_ERROR(pRecipe->dwScriptID);

	bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pRecipe->dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pRecipe->dwScriptID, SCRIPT_ON_BREAK_CAST_CRAFT);
    KG_PROCESS_ERROR(bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
    g_pSO3World->m_ScriptCenter.PushValueToStack(m_dwID);
    g_pSO3World->m_ScriptCenter.PushValueToStack(dwRecipeID);

	g_pSO3World->m_ScriptCenter.CallFunction(pRecipe->dwScriptID, SCRIPT_ON_BREAK_CAST_CRAFT, 0);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

Exit0:
    return;
}

void KCraft::CallEndScriptFunction(KPlayer* pPlayer, DWORD dwRecipeID, KItem* pItem)
{
    BOOL            bRetCode    = false;
    KRecipeBase*    pRecipe     = NULL;
    int             nTopIndex   = 0;

    assert(pPlayer);

    pRecipe = GetRecipe(dwRecipeID);
    KGLOG_PROCESS_ERROR(pRecipe);

    KG_PROCESS_ERROR(pRecipe->dwScriptID);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pRecipe->dwScriptID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pRecipe->dwScriptID, SCRIPT_ON_END_CAST_CRAFT);
    KG_PROCESS_ERROR(bRetCode);

	g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

	g_pSO3World->m_ScriptCenter.PushValueToStack(pPlayer);
    g_pSO3World->m_ScriptCenter.PushValueToStack(m_dwID);
    g_pSO3World->m_ScriptCenter.PushValueToStack(dwRecipeID);

    if (pItem)
    {
        g_pSO3World->m_ScriptCenter.PushValueToStack(pItem);
    }

	g_pSO3World->m_ScriptCenter.CallFunction(pRecipe->dwScriptID, SCRIPT_ON_END_CAST_CRAFT, 0);

	g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);

Exit0:
    return;
}

#endif // _SERVER

DWORD KCraft::GetRecipeIDByDoodadTemplateID(DWORD dwDoodadTemplateID)
{
    DWORD dwRecipeID = 0;

    switch(m_eCraftType)
    {
    case actCollection:
        dwRecipeID = dwDoodadTemplateID;
        break;
    case actCopy:
        dwRecipeID = m_CraftCopy.GetRecipeID(dwDoodadTemplateID);
        break;
    case actProduce:	
    case actRead:
    case actEnchant:	
    case actRadar:
        break;    
    default:
        assert(false);
    }

    return dwRecipeID;
}

#ifdef _SERVER
CRAFT_RESULT_CODE KCraft::Collection(KPlayer* pCaster, KDoodad* pDoodad)
{
    CRAFT_RESULT_CODE   nResult	            = crcFailed;
    BOOL                bRetCode	        = false;
	KLootList*          pLootList		    = NULL;
    KCraftCollection*   pCollection         = NULL;
	DWORD               dwProfessinLevel	= 0;
	int                 nDiffLevel			= 0;
	int                 nAddExp				= 0;

	assert(pCaster);
	assert(pDoodad);

	pCollection = (KCraftCollection*)GetRecipe(pDoodad->m_dwTemplateID);
	KGLOG_PROCESS_ERROR(pCollection);

    bRetCode = pCaster->CostThew(pCollection->nThew);
	KGLOG_PROCESS_ERROR(bRetCode);
    
	dwProfessinLevel = pCaster->m_ProfessionList.GetProfessionLevel(pCollection->dwProfessionID);
	KGLOG_PROCESS_ERROR(dwProfessinLevel);

    nDiffLevel = (int)dwProfessinLevel - (int)pCollection->dwRequireProfessionLevel;
    
    MAKE_IN_RANGE(nDiffLevel, 0, 20);	

	if (pDoodad->m_pLootList != NULL)
	{
		pDoodad->m_pLootList->UnInit();
		KMemory::Delete(pDoodad->m_pLootList);
		pDoodad->m_pLootList = NULL;
	}

	pDoodad->m_pLootList = pDoodad->GenerateLootList(pCaster, true);
	KGLOG_PROCESS_ERROR(pDoodad->m_pLootList);

	nAddExp = GetModifedExp(pCollection->dwProfessionExp, nDiffLevel);
    KGLOG_PROCESS_ERROR(nAddExp >= 0);
    
    pCaster->AddProfessionProficiency(pCollection->dwProfessionID, nAddExp);

	nResult = crcSuccess;
Exit0:
	return nResult;
}

CRAFT_RESULT_CODE KCraft::Produce(KPlayer* pCaster, DWORD dwRecipeID)
{
	CRAFT_RESULT_CODE   nResult	            = crcFailed;
    BOOL                bRetCode		    = false;
    KCraftRecipe*       pRecipe		        = NULL;
	KItem*              pItem			    = NULL;
	DWORD               dwProfessinLevel    = 0;
	int                 nDiffLevel          = 0;
	int                 nRandom			    = 0;
	int                 nAddExp			    = 0;
	int                 nIndex			    = 0;
    KITEM_LOG_INFO      ItemLogInfo;

	KGLOG_PROCESS_ERROR(pCaster);
	KGLOG_PROCESS_ERROR(dwRecipeID);
    
    pRecipe = m_CraftRecipe.GetRecipe(dwRecipeID);
	KGLOG_PROCESS_ERROR(pRecipe);

	dwProfessinLevel = pCaster->m_ProfessionList.GetProfessionLevel(pRecipe->dwProfessionID);
	KG_PROCESS_ERROR(dwProfessinLevel);
    
	nDiffLevel = (int)dwProfessinLevel - (int)pRecipe->dwRequireProfessionLevel;
	MAKE_IN_RANGE(nDiffLevel, 0, 20);

	nRandom = g_Random(MILLION_NUM);

	//根据等级差修正随机数的范围，可选
	if (pRecipe->bResultOnLevel)
	{
		nRandom = (int)(nRandom * (1.0 - 0.03 * nDiffLevel));
	}

	for (nIndex = 0; nIndex < MAX_RESULTANT; nIndex++)
	{
		if (nRandom < (int)pRecipe->dwCreateItemProbability[nIndex])
		{
			if (
                pRecipe->dwCreateItemType[nIndex] != 0 && pRecipe->dwCreateItemIndex[nIndex] != 0
            )
			{
				pItem = g_pSO3World->GenerateItem(pRecipe->dwCreateItemType[nIndex], pRecipe->dwCreateItemIndex[nIndex]);
				KGLOG_PROCESS_ERROR(pItem);

				if (pItem->IsStackable())
				{		
					int nRandom2 = 0;
					int nAmount = 0;
					int nCount = pRecipe->dwCreateItemMaxCount[nIndex] - pRecipe->dwCreateItemMinCount[nIndex];
					KGLOG_PROCESS_ERROR(nCount < 1024);
					if (nCount <= 0)
						nCount = 0;
					nRandom2 = g_Random((nCount + 1) << 4);
					nRandom2 = nRandom2 >> 4;
					nCount = nRandom2 + pRecipe->dwCreateItemMinCount[nIndex];
					if (nCount <= 0)
						nCount = 1;

					bRetCode = pItem->SetStackNum(nCount);
					KGLOG_PROCESS_ERROR_RET_CODE(bRetCode, crcAddItemFailed);
				}
			}

			break;
		}
		nRandom -= pRecipe->dwCreateItemProbability[nIndex];
	}

    if (pItem)
    {
        bRetCode = pCaster->m_ItemList.CanAddItem(pItem);
        KG_PROCESS_ERROR_RET_CODE(bRetCode == aircSuccess, crcAddItemFailed);
    }

    //消耗道具
    for (nIndex = 0; nIndex < MAX_MATERIAL; nIndex++)
    {
        if (
            pRecipe->dwRequireItemType[nIndex] != 0 &&
            pRecipe->dwRequireItemIndex[nIndex] != 0 &&
            pRecipe->dwRequireItemCount[nIndex] != 0
        )
        {
            bRetCode = pCaster->m_ItemList.CostItemInEquipAndPackage(
                pRecipe->dwRequireItemType[nIndex],
                pRecipe->dwRequireItemIndex[nIndex],
                pRecipe->dwRequireItemCount[nIndex]
            );
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    bRetCode = pCaster->CostStamina(pRecipe->nStamina);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    if (pItem)
    {
        g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);

        bRetCode = pCaster->m_ItemList.AddItem(pItem);
        assert(bRetCode == aircSuccess);
        g_LogClient.LogPlayerItemChange(pCaster, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "craft make");
        pItem = NULL;
    }
    
	nAddExp = GetModifedExp(pRecipe->dwProfessionExp, nDiffLevel);
    KGLOG_PROCESS_ERROR(nAddExp >= 0);

    pCaster->AddProfessionProficiency(pRecipe->dwProfessionID, nAddExp);

    if (pRecipe->dwCoolDownID != 0)
    {
        int nCDInterval = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(pRecipe->dwCoolDownID);
        pCaster->m_TimerList.ResetTimer(pRecipe->dwCoolDownID, nCDInterval);
    }

    CallEndScriptFunction(pCaster, dwRecipeID, pItem);

	nResult = crcSuccess;
Exit0:
	if (pItem != NULL)
	{
		g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
	}
	return nResult;
}

#define READ_PAGE "OnReadPage"

CRAFT_RESULT_CODE KCraft::MemorizeBook(KPlayer* pCaster, KTarget& rTarget)
{
	CRAFT_RESULT_CODE	nResult			= crcFailed;
    BOOL				bRetCode		= false;
    KCraftRead*			pRecipe			= NULL;
	int					nAddExp			= 0;
	DWORD				dwProfessinLevel= 0;
	int  				nDiffLevel		= 0;
	int					nTopIndex		= 0;
    KItem*              pAddItem        = NULL;
    KItem*              pBookItem       = NULL;
    DWORD               dwBookID        = 0;
    KITEM_LOG_INFO      ItemLogInfo;

	KGLOG_PROCESS_ERROR(pCaster);

    KGLOG_PROCESS_ERROR(rTarget.m_eTargetType == ttItem);

    bRetCode = rTarget.GetTarget(&pBookItem);
    KGLOG_PROCESS_ERROR(bRetCode && pBookItem);

    dwBookID = pBookItem->m_nCurrentDurability;
    KGLOG_PROCESS_ERROR(dwBookID >= 0);
    
    pRecipe = m_CraftRead.GetRecipe(dwBookID);
    KGLOG_PROCESS_ERROR(pRecipe);

    if (pRecipe->dwCreateItemTab != 0 && pRecipe->dwCreateItemIndex != 0)
    {
        pAddItem = g_pSO3World->GenerateItem(
            pRecipe->dwCreateItemTab, pRecipe->dwCreateItemIndex
        );
        KGLOG_PROCESS_ERROR(pAddItem);

        if (pAddItem->IsStackable() && pRecipe->nStackNum > 0)
        {
            pAddItem->SetStackNum(pRecipe->nStackNum);
        }
        else if (pAddItem->m_Common.nGenre == igBook)
        {
            pAddItem->m_nCurrentDurability = pRecipe->nStackNum;
        }
        
        bRetCode = pCaster->m_ItemList.CanAddItem(pAddItem);
        KG_PROCESS_ERROR_RET_CODE(bRetCode == aircSuccess, crcAddItemFailed);
    }

    bRetCode = pCaster->m_ItemList.DestroyItem(pBookItem);
    KGLOG_PROCESS_ERROR(bRetCode);
    pBookItem = NULL;

    bRetCode = pCaster->CostStamina(pRecipe->nStamina);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pCaster->m_BookList.AddBook(pRecipe->dwID, pRecipe->dwSubID);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    if (pRecipe->nPlayerExp != 0)
    {
        // 如果是免费玩家，到18级就不再获得经验
        if (
            pCaster->m_nLevel < g_pSO3World->m_Settings.m_ConstList.nFreeMaxLevel ||
            !pCaster->m_bFreeLimitFlag
        )
        {
            pCaster->AddExp(pRecipe->nPlayerExp);

            g_pSO3World->m_StatDataServer.UpdateExpStat(pCaster, pRecipe->nPlayerExp, "OTHER");
        }
    }
    
    dwProfessinLevel = pCaster->m_ProfessionList.GetProfessionLevel(pRecipe->dwProfessionID);
	KG_PROCESS_ERROR(dwProfessinLevel);

	nDiffLevel = (int)dwProfessinLevel - (int)pRecipe->dwRequireProfessionLevel;
    MAKE_IN_RANGE(nDiffLevel, 0, 20);

	nAddExp = GetModifedExp(pRecipe->dwProfessionExp, nDiffLevel);
    KGLOG_PROCESS_ERROR(nAddExp >= 0);

	pCaster->AddProfessionProficiency(pRecipe->dwProfessionID, nAddExp);    
    
    if (pRecipe->dwExtendProfessionID1 != 0)
    {
        pCaster->AddProfessionProficiency(pRecipe->dwExtendProfessionID1, pRecipe->dwExtendExp1);
    }
    
    if (pRecipe->dwExtendProfessionID2 != 0)
    {
        pCaster->AddProfessionProficiency(pRecipe->dwExtendProfessionID2, pRecipe->dwExtendExp2);
    }
    
    if (pAddItem)
    {
        g_LogClient.GetItemLogInfo(pAddItem, &ItemLogInfo);
        bRetCode = pCaster->m_ItemList.AddItem(pAddItem);
        assert(bRetCode == aircSuccess);
        pAddItem = NULL;
        g_LogClient.LogPlayerItemChange(pCaster, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "read");
    }
    
	if (pRecipe->dwBuffID != 0)
	{
        KBUFF_RECIPE_KEY BuffRecipeKey;

        memset(&BuffRecipeKey, 0, sizeof(BuffRecipeKey));

        BuffRecipeKey.dwID      = pRecipe->dwBuffID;
        BuffRecipeKey.nLevel    = pRecipe->dwBuffLevel;

        pCaster->m_BuffList.CallBuff(pCaster->m_dwID, pCaster->m_nLevel, BuffRecipeKey, 0, 0);
	}
    
    if (pRecipe->nTrain != 0)
    {
        bRetCode = pCaster->AddTrain(pRecipe->nTrain);
        KGLOG_CHECK_ERROR(bRetCode);
    }
    
    if (pRecipe->dwScriptID != 0)
    {
	    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pRecipe->dwScriptID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(pRecipe->dwScriptID, READ_PAGE);
        if (bRetCode)
        {
            g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);
            g_pSO3World->m_ScriptCenter.PushValueToStack(pCaster);
            g_pSO3World->m_ScriptCenter.CallFunction(pRecipe->dwScriptID, READ_PAGE, 0);
            g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
        }
    }

	nResult = crcSuccess;
Exit0:
    if (pAddItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pAddItem);
        pAddItem = NULL;
    }
	return nResult;
}

CRAFT_RESULT_CODE KCraft::Enchant(KPlayer* pCaster, DWORD dwRecipeID, KItem* pItem)
{
    CRAFT_RESULT_CODE   nResult             = crcFailed;
    BOOL                bRetCode			= false;
	KCraftEnchant*      pRecipe		        = NULL;
	int                 nIndex				= 0;
	int                 nAddExp				= 0;
	DWORD               dwProfessinLevel	= 0;
	int                 nDiffLevel			= 0;
	DWORD               dwBoxIndex			= 0;
	DWORD               dwX					= 0;

	assert(pCaster);
	assert(pItem);
	assert(dwRecipeID);

	pRecipe = m_CraftEnchant.GetRecipe(dwRecipeID);
	KGLOG_PROCESS_ERROR(pRecipe);
    
	for (nIndex = 0; nIndex < MAX_MATERIAL; ++nIndex)
	{
		if (
            pRecipe->dwRequireItemType[nIndex] != 0 && pRecipe->dwRequireItemIndex[nIndex] != 0 &&
			pRecipe->dwRequireItemCount[nIndex] != 0
        )
		{
			bRetCode = pCaster->m_ItemList.CostItemInEquipAndPackage(
                pRecipe->dwRequireItemType[nIndex],
				pRecipe->dwRequireItemIndex[nIndex],
				pRecipe->dwRequireItemCount[nIndex]
            );
			KGLOG_PROCESS_ERROR(bRetCode);
		}
	}
    
    bRetCode = pCaster->CostStamina(pRecipe->nStamina);
	KGLOG_PROCESS_ERROR(bRetCode);

	dwProfessinLevel = pCaster->m_ProfessionList.GetProfessionLevel(pRecipe->dwProfessionID);
    KGLOG_PROCESS_ERROR(dwProfessinLevel);

	nDiffLevel = (int)dwProfessinLevel - (int)pRecipe->dwRequireProfessionLevel;
    MAKE_IN_RANGE(nDiffLevel, 0, 20);

	// 现在只能对自己背包里的道具副魔,还不能给别人进行交易副魔
	bRetCode = pCaster->m_ItemList.GetItemPos(pItem->m_dwID, dwBoxIndex, dwX);
	KG_PROCESS_ERROR(bRetCode);

    bRetCode = pCaster->m_ItemList.AddEnchant(dwBoxIndex, dwX, (int)pRecipe->dwEnchantID);
	KG_PROCESS_ERROR(bRetCode);

	nAddExp = GetModifedExp(pRecipe->dwProfessionExp, nDiffLevel);
    KGLOG_PROCESS_ERROR(nAddExp >= 0);

	pCaster->AddProfessionProficiency(pRecipe->dwProfessionID, nAddExp);	

    CallEndScriptFunction(pCaster, dwRecipeID);

	nResult = crcSuccess;
Exit0:
	return nResult;
}

CRAFT_RESULT_CODE KCraft::Copy(KPlayer* pCaster, DWORD dwRecipeID)
{
    CRAFT_RESULT_CODE   nResult	            = crcFailed;
    KCraftCopy*         pCraftCopy          = NULL;
	BOOL                bRetCode            = false;
	KItem*              pItem               = NULL;
	DWORD               dwProfessinLevel    = 0;
	int                 nDiffLevel          = 0;
	int                 nRandom             = 0;
	int                 nAddExp             = 0;
	int                 nIndex              = 0;
    KITEM_LOG_INFO      ItemLogInfo;

    KGLOG_PROCESS_ERROR(pCaster);
	KGLOG_PROCESS_ERROR(dwRecipeID);

	pCraftCopy = m_CraftCopy.GetRecipe(dwRecipeID);
	KGLOG_PROCESS_ERROR(pCraftCopy);
    
    if (pCraftCopy->dwCreateItemType != 0 && pCraftCopy->dwCreateItemIndex != 0)
    {
        pItem = g_pSO3World->GenerateItem(
            pCraftCopy->dwCreateItemType, pCraftCopy->dwCreateItemIndex
        );
        KGLOG_PROCESS_ERROR(pItem);

        if (pItem->m_Common.nGenre == igBook)
        {
            pItem->m_nCurrentDurability = dwRecipeID;
        }
        else
        {
            if (pItem->IsStackable() && pCraftCopy->nStackNum > 0)
            {
                pItem->m_nStackNum = pCraftCopy->nStackNum;
                if (pItem->m_nStackNum > pItem->m_nMaxStackNum)
                {
                    pItem->m_nStackNum = pItem->m_nMaxStackNum;
                }
            }
        }

        bRetCode = pCaster->m_ItemList.CanAddItem(pItem);
        KG_PROCESS_ERROR_RET_CODE(bRetCode == aircSuccess, crcAddItemFailed);
    }

    //消耗道具
    for (nIndex = 0; nIndex < MAX_COPY_MATERIAL; nIndex++)
    {
        if (
            pCraftCopy->dwRequireItemType[nIndex] != 0 && pCraftCopy->dwRequireItemIndex[nIndex] != 0 &&
            pCraftCopy->dwRequireItemCount[nIndex] != 0
        )
        {
            bRetCode = pCaster->m_ItemList.CostItemInEquipAndPackage(
                pCraftCopy->dwRequireItemType[nIndex],
                pCraftCopy->dwRequireItemIndex[nIndex],
                pCraftCopy->dwRequireItemCount[nIndex]
            );
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    bRetCode = pCaster->CostThew(pCraftCopy->nThew);
	KGLOG_PROCESS_ERROR(bRetCode);
   
    if (pItem)
	{
        g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);
        bRetCode = pCaster->m_ItemList.AddItem(pItem);
        assert(bRetCode == aircSuccess);
        pItem = NULL;
        g_LogClient.LogPlayerItemChange(pCaster, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "copy");
	}
	
    dwProfessinLevel = pCaster->m_ProfessionList.GetProfessionLevel(pCraftCopy->dwProfessionID);
    KGLOG_PROCESS_ERROR(dwProfessinLevel);

    nDiffLevel = (int)dwProfessinLevel - (int)pCraftCopy->dwRequireProfessionLevel;
    MAKE_IN_RANGE(nDiffLevel, 0, 20);

	nAddExp = GetModifedExp(pCraftCopy->dwProfessionExp, nDiffLevel);
    KGLOG_PROCESS_ERROR(nAddExp >= 0);

    pCaster->AddProfessionProficiency(pCraftCopy->dwProfessionID, nAddExp);
    
    if (pCraftCopy->dwCoolDownID != 0)
    {
        int nCDInterval = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(pCraftCopy->dwCoolDownID);
        pCaster->m_TimerList.ResetTimer(pCraftCopy->dwCoolDownID, nCDInterval);
    }
    
    nResult = crcSuccess;
Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
	return nResult;
}

#endif //_SERVER
//<--------- 生活技能 -----------------------------------------


int KCraft::GetModifedExp(int nExp, int nDiffLevel)
{
	int nResult = 0;
	KGLOG_PROCESS_ERROR (nDiffLevel >= 0);
	
	if (nDiffLevel >= 0 && nDiffLevel < 10)
		nResult = (int)(nExp * (1.0 - 0.02 * nDiffLevel));
	else if (nDiffLevel >= 10 && nDiffLevel < 20)
		nResult = (int)(nExp * (1.1 - 0.03 * nDiffLevel));
	else
		nResult = 0;
Exit0:
	return nResult;
}

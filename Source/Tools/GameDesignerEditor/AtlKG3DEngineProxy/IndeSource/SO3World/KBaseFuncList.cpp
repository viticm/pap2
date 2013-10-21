#include "stdafx.h"
#include "KMath.h"
#include "KScriptFuncList.h"
#include "KSO3World.h"
#include "KPlayerClient.h"
#include "KProfession.h"
#include "KRecipe.h"
#include "KDoodadTemplateList.h"
#include "KShop.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "KWeaponTypeList.h"
#include "KAISearchTactics.h"
#include "KCraft.h"
#include "KRelayClient.h"
#include "KRecipeMaster.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "KAIAction.h"
#include "KAIState.h"
#include "KAILogic.h"
#include "KSkill.h"
#include "KItemLib.h"
#include "KPlayerServer.h"

namespace KScriptFuncList
{
	//测试函数
	int LuaTest(Lua_State* L)
	{
		return 0;
	}

	/************************************************************************/
	/* 服务端和客户端公共脚本函数                                           */
	/************************************************************************/
    
    int LuaGetCurrentTime(Lua_State* L)
    {
        int nResult  = false;
        int nRetCode = false;
 
        Lua_PushNumber(L, g_pSO3World->m_nCurrentTime);

        nResult = 1;
Exit0:
        return nResult;
    }

    int LuaGetLogicDirection(Lua_State* L)
    {
        int nResult     = 0;
        int nRetCode    = 0;
        int nX          = 0;
        int nY          = 0;
        int nDirection  = 0;

        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 2);

        nX = (int)Lua_ValueToNumber(L, 1);
        nY = (int)Lua_ValueToNumber(L, 2);

        nDirection = g_GetDirection(0, 0, nX, nY);

        Lua_PushNumber(L, nDirection);

        nResult = 1;
Exit0:
        return nResult;
    }

    int LuaGetLogicFrameCount(Lua_State* L)
    {
        Lua_PushNumber(L, g_pSO3World->m_nGameLoop);
        return 1;
    }

    int LuaGetFileNameHash(Lua_State* L)
    {
        int         nResult     = 0;
        int         nID         = 0;
		int         nRetCode    = false;
        const char* pszFileName = NULL;

        nRetCode = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		pszFileName = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pszFileName);

        nID = (int)g_FileNameHash(pszFileName);

        lua_pushnumber(L, nID);

        nResult = 1;
Exit0:
        return nResult;
    }

	int LuaGetItemInfo(Lua_State* L)
	{
        int                 nResult             = 0;
        int                 nRetCode            = 0;
		DWORD               dwTabType           = 0;
		DWORD               dwIndex             = 0;
		int                 nVersion            = CURRENT_ITEM_VERSION;
        KItemInfo*          pItemInfo           = NULL;
        KCustomEquipInfo*   pCustomEquipInfo    = NULL;
        KOtherItemInfo*     pOtherItemInfo      = NULL;
        int                 nParamCount         = Lua_GetTopIndex(L);
		int                 nParamIndex         = 1;
        int                 nType               = iitInvaild;

        KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 3);

        if (nParamCount ==  3)
		{
			nVersion = (int)Lua_ValueToNumber(L, nParamIndex++);
		}
        
        dwTabType   = (DWORD)Lua_ValueToNumber(L, nParamIndex++);
		dwIndex     = (DWORD)Lua_ValueToNumber(L, nParamIndex);

		pItemInfo = g_pSO3World->m_ItemManager.GetItemInfo(nVersion, dwTabType, dwIndex);
		KGLOG_PROCESS_ERROR(pItemInfo);

        switch (dwTabType)
        {
        case ittCustWeapon:
        case ittCustArmor:
        case ittCustTrinket:
            pCustomEquipInfo = (KCustomEquipInfo*)pItemInfo;
            nRetCode = pCustomEquipInfo->LuaGetObj(L);
            KGLOG_PROCESS_ERROR(nRetCode);
            nType = iitCustEquipInfo;
            break;
        case ittOther:
            pOtherItemInfo = (KOtherItemInfo*)pItemInfo;
            nRetCode = pOtherItemInfo->LuaGetObj(L);
            KGLOG_PROCESS_ERROR(nRetCode);
            nType = iitOtherInfo;
            break;
        default:
            KGLOG_PROCESS_ERROR(false);
            break;
        }
        
        Lua_PushNumber(L, nType);
        
        nResult = 2;
Exit0:
		return nResult;
	}

	int LuaIsPlayer(Lua_State* L)
	{
        int     nResult   = 0;
        BOOL    bIsPlayer = false;
        DWORD   dwID      = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		dwID = (DWORD)Lua_ValueToNumber(L, 1);

        bIsPlayer = IS_PLAYER(dwID);

        Lua_PushBoolean(L, bIsPlayer);
		
        nResult = 1;
    Exit0:
		return nResult;
	}

	int LuaGetScene(Lua_State* L)
	{
        int     nResult     = 0;
        int     nRetCode    = 0;
        DWORD   dwID        = ERROR_ID;
        KScene* pScene      = NULL;

        nRetCode = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		dwID = (DWORD)Lua_ValueToNumber(L, 1);

		pScene = g_pSO3World->m_SceneSet.GetObj(dwID);
		KGLOG_PROCESS_ERROR(pScene);

		nResult = pScene->LuaGetObj(L);
Exit0:
		return nResult;
	}

	int LuaGetPlayer(Lua_State* L)
	{
        int         nResult     = 0;
        int         nRetCode    = false;
        DWORD       dwPlayerID  = 0;
        KPlayer*    pPlayer     = NULL;

        nRetCode = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);

		pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
		KG_PROCESS_ERROR(pPlayer);

        KG_PROCESS_ERROR(pPlayer->m_pScene);

        nResult = pPlayer->LuaGetObj(L);
Exit0:
		return nResult;
	}

	int LuaGetNpc(Lua_State* L)
	{
        int     nResult     = 0;
        int     nTopIndex   = 0;
		DWORD   dwID        = 0;
        KNpc*   pNpc        = NULL;

        nTopIndex = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nTopIndex == 1);

		dwID = (DWORD)Lua_ValueToNumber(L, 1);

		pNpc = g_pSO3World->m_NpcSet.GetObj(dwID);
		KG_PROCESS_ERROR(pNpc);

        KG_PROCESS_ERROR(pNpc->m_pScene);

        nResult = pNpc->LuaGetObj(L);
Exit0:
		return nResult;
	}    

#ifdef _CLIENT
	int LuaGetNpcName(Lua_State* L)
	{
		BOOL bRetCode = false;
		KNpcTemplate* pNpcTemplate;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		DWORD dwNpcTemplateID = (DWORD)Lua_ValueToNumber(L, 1);

		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(dwNpcTemplateID);
		KG_PROCESS_ERROR(pNpcTemplate);

		Lua_PushString(L, pNpcTemplate->szName);
		return 1;
Exit0:
		return 0;
	}
#endif //_CLIENT

	int LuaGetNpcTemplate(Lua_State* L)
	{
		BOOL bRetCode = false;
		int nResult = 0;
		KNpcTemplate* pNpcTemplate = NULL;
        DWORD dwNpcTemplateID = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		dwNpcTemplateID = (DWORD)Lua_ValueToNumber(L, 1);

		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(dwNpcTemplateID);
		KG_PROCESS_ERROR(pNpcTemplate);

		nResult = pNpcTemplate->LuaGetObj(L);
		return nResult;
Exit0:
		return 0;
	}

	int LuaGetDoodad(Lua_State* L)
	{
        int         nResult     = 0;
        int         nRetCode    = 0;
        DWORD       dwID        = ERROR_ID;
        KDoodad*    pDoodad     = NULL;

        nRetCode = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		dwID = (DWORD)Lua_ValueToNumber(L, 1);

		pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwID);
		KG_PROCESS_ERROR(pDoodad);

        KG_PROCESS_ERROR(pDoodad->m_pScene);

		nResult = pDoodad->LuaGetObj(L);
Exit0:
		return nResult;
	}    

	int LuaGetQuestInfo(Lua_State* L)
	{
		DWORD dwQuestID = 0;
        KQuestInfo* pQuestInfo = NULL;

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);

		pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
		KGLOG_PROCESS_ERROR(pQuestInfo);

		return pQuestInfo->LuaGetObj(L);
Exit0:

		return 0;
	}

	int LuaGetQuestString(Lua_State* L)
	{
        DWORD dwQuestID     = 0;
		DWORD eTargetType   = 0;
        DWORD dwTypeID      = 0;
        char szTemp[64];
            
        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);
		dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
		eTargetType = (DWORD)Lua_ValueToNumber(L, 2);
		dwTypeID = (DWORD)Lua_ValueToNumber(L, 3);

		snprintf(szTemp, sizeof(szTemp), "<O%lu %lu><Q%lu>", eTargetType, dwTypeID, dwQuestID);
        szTemp[sizeof(szTemp) - 1] = '\0';

		Lua_PushString(L, szTemp);

		return 1;
Exit0:
		return 0;
	}

    int LuaGetSkill(Lua_State* L)
    {
        int     nResult         = 0;
        int     nRetCode        = 0;
        DWORD   dwSkillID       = 0;
        DWORD   dwSkillLevel    = 0;
        KSkill* pSkill          = NULL;

        
        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 2);

        dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);
        dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

        pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, dwSkillLevel);
		if (!pSkill)
        {
            KGLogPrintf(KGLOG_ERR, "[Skill] Failed to GetSkill: Skill: %u, Level: %u\n", dwSkillID, dwSkillLevel);
            goto Exit0;
        }

        nResult = pSkill->LuaGetObj(L);
Exit0:
        return nResult;
    }

	int LuaLoadProfession(Lua_State* L)
	{
		Luna<KProfession>::Register(L);
		Luna<KCraftRecipe>::Register(L);
		Luna<KCraftEnchant>::Register(L);
		Luna<KCraftCollection>::Register(L);
        Luna<KCraftRead>::Register(L);
        Luna<KCraftCopy>::Register(L);

		return 0;
	}

	int LuaGetProfession(Lua_State* L)
	{
		DWORD dwProfessionID = 0;
		KProfession* pProfession = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);

		pProfession = g_pSO3World->m_ProfessionManager.GetProfession(dwProfessionID);
		KGLOG_PROCESS_ERROR(pProfession);

		return pProfession->LuaGetObj(L);
Exit0:
		return 0;
	}

	int LuaGetCraft(Lua_State* L)
	{
		BOOL	bRetCode = false;
		int		nTopIndex = 0;
		DWORD	dwCraftID = 0;
		KCraft* pCraft = NULL;

		nTopIndex = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nTopIndex == 1);

		dwCraftID = (DWORD)Lua_ValueToNumber(L, 1);

		pCraft = g_pSO3World->m_ProfessionManager.GetCraft(dwCraftID);
		KGLOG_PROCESS_ERROR(pCraft);

		Lua_NewTable(L);
		nTopIndex = Lua_GetTopIndex(L);

		Lua_PushString(L, "ID");
		Lua_PushNumber(L, dwCraftID);
		Lua_SetTable(L, nTopIndex);

		Lua_PushString(L, "ProfessionID");
		Lua_PushNumber(L, pCraft->m_dwProfessionID);
		Lua_SetTable(L, nTopIndex);

		Lua_PushString(L, "Name");
		Lua_PushString(L, pCraft->m_szName);
		Lua_SetTable(L, nTopIndex);

		Lua_PushString(L, "CraftType");
		Lua_PushNumber(L, (int)pCraft->m_eCraftType);
		Lua_SetTable(L, nTopIndex);

		return 1;
Exit0:
		return 0;
	}

	int LuaGetCraftList(Lua_State* L)
	{
		BOOL	bRetCode		= false;
		int		nTopIndex		= 0;
		int		nTableIndex		= 0;
		int		nIndex			= 0;
		DWORD	dwProfessionID	= 0;
		DWORD	dwCraftID		= 0;
		KCraft* pCraft			= NULL;
		std::vector<DWORD> vecCraftList;

		nTopIndex = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nTopIndex == 1);

		dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);
		KGLOG_PROCESS_ERROR(dwProfessionID);
	
		bRetCode = g_pSO3World->m_ProfessionManager.GetCraftIDList(dwProfessionID, vecCraftList);
		KG_PROCESS_ERROR(bRetCode);

		Lua_NewTable(L);
		nTopIndex = Lua_GetTopIndex(L);

		for (nIndex = 0; nIndex < (int)vecCraftList.size(); nIndex++)
		{
			dwCraftID = vecCraftList[nIndex];
			pCraft = g_pSO3World->m_ProfessionManager.GetCraft(dwCraftID);
			if (pCraft == NULL)
				continue;

			Lua_PushNumber(L, nIndex + 1);

			Lua_NewTable(L);
			nTableIndex = Lua_GetTopIndex(L);

			Lua_PushString(L, "ID");
			Lua_PushNumber(L, dwCraftID);
			Lua_SetTable(L, nTableIndex);

			Lua_PushString(L, "Name");
			Lua_PushString(L, pCraft->m_szName);
			Lua_SetTable(L, nTableIndex);

			Lua_PushString(L, "CraftType");
			Lua_PushNumber(L, (int)pCraft->m_eCraftType);
			Lua_SetTable(L, nTableIndex);

			Lua_PushString(L, "Hide");
			Lua_PushBoolean(L, pCraft->m_bHide);
			Lua_SetTable(L, nTableIndex);

			Lua_SetTable(L, nTopIndex);
		}

		return 1;
Exit0:
		return 0;
	}

	int LuaGetRecipeName(Lua_State* L)
	{
		DWORD           dwCraftID   = 0;
		DWORD           dwRecipeID  = 0;
		KCraft*         pCraft      = NULL;
		KRecipeBase*    pRecipe     = NULL;
		const char*     szName      = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		dwCraftID   = (DWORD)Lua_ValueToNumber(L, 1);
		dwRecipeID  = (DWORD)Lua_ValueToNumber(L, 2);

		pCraft = g_pSO3World->m_ProfessionManager.GetCraft(dwCraftID);
		KGLOG_PROCESS_ERROR(pCraft);

		pRecipe = pCraft->GetRecipe(dwRecipeID);
		KGLOG_PROCESS_ERROR(pRecipe);

		szName = pRecipe->GetName();
		KG_PROCESS_ERROR(szName && szName[0] != '\0');

		Lua_PushString(L, szName);
		return 1;
Exit0:
		return 0;
	}

	int LuaGetRecipe(Lua_State* L)
	{
		DWORD dwCraftID = 0;
		DWORD dwRecipeID = 0;
		KCraft* pCraft = NULL;
		KRecipeBase* pRecipe = NULL;
		const char* szName = NULL;

        int nTopIndex = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 3);

        if (nTopIndex == 2)
        {
		    dwCraftID  = (DWORD)Lua_ValueToNumber(L, 1);
		    dwRecipeID = (DWORD)Lua_ValueToNumber(L, 2);
        }
        else
        {
            DWORD dwBookID    = 0;
            DWORD dwBookSubID = 0;

            dwCraftID   = (DWORD)Lua_ValueToNumber(L, 1);
            dwBookID    = (DWORD)Lua_ValueToNumber(L, 2);
            dwBookSubID = (DWORD)Lua_ValueToNumber(L, 3);
            dwRecipeID  = (DWORD)BookID2RecipeID(dwBookID, dwBookSubID);
        }

		pCraft = g_pSO3World->m_ProfessionManager.GetCraft(dwCraftID);
		KGLOG_PROCESS_ERROR(pCraft);

		pRecipe = pCraft->GetRecipe(dwRecipeID);
		KGLOG_PROCESS_ERROR(pRecipe);

		switch(pCraft->m_eCraftType)
		{
		case actProduce:
			{
				KCraftRecipe* pCraftRecipe = (KCraftRecipe*)(pRecipe);
				return pCraftRecipe->LuaGetObj(L);
			}
			break;
		case actEnchant:
			{
				KCraftEnchant* pCraftEnchant = (KCraftEnchant*)(pRecipe);
				return pCraftEnchant->LuaGetObj(L);
			}
			break;
		case actCollection:
			{
				KCraftCollection* pCraftCollection = (KCraftCollection*)(pRecipe);
				return pCraftCollection->LuaGetObj(L);
			}
        case actRead:
            {
                KCraftRead* pCraftRead = (KCraftRead*)pRecipe;
                return pCraftRead->LuaGetObj(L);
            }
        case actCopy:
            {
                KCraftCopy* pCraftCopy = (KCraftCopy*)pRecipe;
                return pCraftCopy->LuaGetObj(L);
            }
		default:
			KGLOG_PROCESS_ERROR(false && "Error Craft Type!");
		    break;
		}
Exit0:
		return 0;
	}

	int LuaGetItem(Lua_State* L)
	{
        int     nResult     = 0;
        int     nRetCode    = 0;
        DWORD   dwItemID    = 0;
        KItem*  pItem       = NULL;

        nRetCode = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		dwItemID = (DWORD)Lua_ValueToNumber(L, 1);
		pItem = g_pSO3World->m_ItemSet.GetObj(dwItemID);
		KGLOG_PROCESS_ERROR(pItem);

		nResult = pItem->LuaGetObj(L);
Exit0:
		return nResult;
	}

	int LuaGetLevelUpData(Lua_State* L)
	{
		int nType       = 0;
        int nLevel      = 0;
        int nLevelTable = 0;
        KLevelUpData* pLevelUpData = NULL;

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

        nType = (int)Lua_ValueToNumber(L, 1);
		nLevel = (int)Lua_ValueToNumber(L, 2);

		pLevelUpData = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(nType, nLevel);
		KGLOG_PROCESS_ERROR(pLevelUpData);

		Lua_NewTable(L);
		nLevelTable = Lua_GetTopIndex(L);

		Lua_PushString(L, "Experience");
		Lua_PushNumber(L, pLevelUpData->nExperience);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "Strength");
		Lua_PushNumber(L, pLevelUpData->nStrength);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "Agility");
		Lua_PushNumber(L, pLevelUpData->nAgility);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "Vigor");
		Lua_PushNumber(L, pLevelUpData->nVigor);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "Spirit");
		Lua_PushNumber(L, pLevelUpData->nSpirit);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "MaxLife");
		Lua_PushNumber(L, pLevelUpData->nMaxLife);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "MaxMana");
		Lua_PushNumber(L, pLevelUpData->nMaxMana);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "MaxStamina");
		Lua_PushNumber(L, pLevelUpData->nMaxStamina);
		Lua_SetTable(L, nLevelTable);

        Lua_PushString(L, "MaxThew");
        Lua_PushNumber(L, pLevelUpData->nMaxThew);
        Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "RunSpeed");
		Lua_PushNumber(L, pLevelUpData->nRunSpeed);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "Height");
		Lua_PushNumber(L, pLevelUpData->nHeight);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "LifeReplenish");
		Lua_PushNumber(L, pLevelUpData->nLifeReplenish);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "LifeReplenishPercent");
		Lua_PushNumber(L, pLevelUpData->nLifeReplenishPercent);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "ManaReplenish");
		Lua_PushNumber(L, pLevelUpData->nManaReplenish);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "ManaReplenishPercent");
		Lua_PushNumber(L, pLevelUpData->nManaReplenishPercent);
		Lua_SetTable(L, nLevelTable);

		Lua_PushString(L, "PhysicsCriticalStrike");
		Lua_PushNumber(L, pLevelUpData->nPhysicsCriticalStrike);
		Lua_SetTable(L, nLevelTable);

        Lua_PushString(L, "SolarCriticalStrike");
        Lua_PushNumber(L, pLevelUpData->nSolarCriticalStrike);
        Lua_SetTable(L, nLevelTable);

        Lua_PushString(L, "NeutralCriticalStrike");
        Lua_PushNumber(L, pLevelUpData->nNeutralCriticalStrike);
        Lua_SetTable(L, nLevelTable);

        Lua_PushString(L, "LunarCriticalStrike");
        Lua_PushNumber(L, pLevelUpData->nLunarCriticalStrike);
        Lua_SetTable(L, nLevelTable);

        Lua_PushString(L, "PoisonCriticalStrike");
        Lua_PushNumber(L, pLevelUpData->nPoisonCriticalStrike);
        Lua_SetTable(L, nLevelTable);

        Lua_PushString(L, "NoneWeaponAttackSpeedBase");
        Lua_PushNumber(L, pLevelUpData->nNoneWeaponAttackSpeedBase);
        Lua_SetTable(L, nLevelTable);

        Lua_PushString(L, "MaxPhysicsDefence");
        Lua_PushNumber(L, pLevelUpData->nMaxPhysicsDefence);
        Lua_SetTable(L, nLevelTable);

		return 1;
Exit0:
		return 0;
	}
	
	int LuaFormatString(Lua_State* L)
	{
		static char szDest[2048];
		szDest[0] = '\0';
		int nParamCount = 0;
		const char *pszSrc = NULL;
		char *pszDest = szDest;

		int nInpare = false;
		int nInPareString = false;
		const char *pszPairValue1 = NULL; 
		const char *pszPairValue2 = NULL; 
		static char szPaireString[128];
		szPaireString[0] = '\0';
		char *pszDestInPaire = szPaireString;

		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount > 0);

		pszSrc = (const char *)Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pszSrc);

		assert(strlen(pszSrc) < 2048);
		while(*pszSrc)
		{
			if (*pszSrc == '<')
			{
				switch (*(pszSrc + 1))
				{
				case 'd':
				case 'D':
					{
						int nIndex = atoi(pszSrc + 2);
						if (nIndex < 0 || nIndex >= nParamCount)
							break;
						const char *pszValue = (const char *)Lua_ValueToString(L, nIndex + 2);
						if (!pszValue)
							break;

						if (nInpare && !pszPairValue1)
						{
							pszPairValue1 = pszValue;
							nInPareString = true;
						}
						else if(nInpare && !pszPairValue2)
						{
							pszPairValue2 = pszValue;
							*pszDestInPaire = '\0';
							pszDestInPaire = szPaireString;
							if (strcmp(pszPairValue1, pszPairValue2) == 0)
								pszValue = pszPairValue1;
							else
							{
								while(*pszPairValue1)
								{
									*pszDest++ = *pszPairValue1++;
								}
								while(*pszDestInPaire)
								{
									*pszDest++ = *pszDestInPaire++;
								}
								pszValue = pszPairValue2;
							}
							while(*pszValue)
							{
								*pszDest++ = *pszValue++;
							}
							nInPareString = false;
							pszDestInPaire = szPaireString;
							pszPairValue1 = NULL;
							pszPairValue1 = NULL;
						}
						else
						{
							while(*pszValue)
							{
								*pszDest++ = *pszValue++;
							}
						}
						while(*pszSrc && *pszSrc != '>')
						{
							++pszSrc;
						}
						if (*pszSrc)
						{
							++pszSrc;
						}
						continue;
					}
					break;
				case 'r':
				case 'R':
					if (nInpare)
						break;
					++pszSrc;
					++pszSrc;
					nInpare = true;
					continue;
					break;
				default:
					break;
				}
			}
			if (*pszSrc == '>' && nInpare)
			{
				nInpare = false;
				nInPareString = false;
				pszPairValue1 = NULL;
				pszPairValue2 = NULL;
				pszDestInPaire = szPaireString;

				++pszSrc;
				continue;
			}

			if (nInPareString)
			{
				assert(pszDestInPaire != szPaireString + 127);
				*pszDestInPaire++ = *pszSrc++;
			}
			else
			{
				*pszDest++ = *pszSrc++;
			}
		}

Exit0:
		*pszDest = '\0';
		Lua_PushString(L, szDest);
		return 1;
	}


    int LuaCheckWeaponType(Lua_State* L)
    {
        BOOL    bResult     = false;
        int     nRetCode    = false;
        DWORD   dwType      = 0;
        int     nDetailType = 0;

        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 2);

        dwType = (DWORD)Lua_ValueToNumber(L, 1);
        nDetailType = (int)Lua_ValueToNumber(L, 2);

        nRetCode = g_pSO3World->m_Settings.m_WeaponTypeList.CheckType(dwType, (WEAPON_DETAIL_TYPE)nDetailType);
        KG_PROCESS_ERROR(nRetCode);

        bResult = true;
Exit0:
        Lua_PushBoolean(L, bResult);
        return 1;
    }

#ifdef _CLIENT
	int LuaLootItem(Lua_State* L)
	{
		DWORD   dwItemID    = 0;
		DWORD   dwDoodadID  = 0;
        int     nTopIndex   = Lua_GetTopIndex(L);

		KGLOG_PROCESS_ERROR(nTopIndex == 2);

		dwDoodadID  = (DWORD)Lua_ValueToNumber(L, 1);
		dwItemID    = (DWORD)Lua_ValueToNumber(L, 2);

		g_PlayerClient.DoLootItem(dwDoodadID, dwItemID);
Exit0:
		return 0;
	}

    int LuaRollItem(Lua_State* L)
	{
		DWORD   dwItemID    = 0;
		DWORD   dwDoodadID  = 0;
		BOOL    bCancel     = false;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

		dwDoodadID  = (DWORD)Lua_ValueToNumber(L, 1);
		dwItemID    = (DWORD)Lua_ValueToNumber(L, 2);
		bCancel     = (BOOL)Lua_ValueToBoolean(L, 3);

		g_PlayerClient.DoRollItem(dwDoodadID, dwItemID, bCancel);
Exit0:
		return 0;
	}

	int LuaLootMoney(Lua_State* L)
	{
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		DWORD dwDoodadID = (DWORD)Lua_ValueToNumber(L, 1);

		g_PlayerClient.DoLootMoney(dwDoodadID);

Exit0:
		return 0;
	}

	int LuaGetClientPlayer(Lua_State* L)
	{
		KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);

		KG_PROCESS_ERROR(pPlayer && pPlayer->m_pScene);

		return pPlayer->LuaGetObj(L);
Exit0:
		return 0;
	}

	int LuaGetClientScene(Lua_State* L)
	{
        int     nResult = 0;
        KScene* pScene  = NULL;

        assert(g_pSO3World);
        
        pScene = g_pSO3World->GetScene(
            g_pSO3World->m_dwClientMapID, g_pSO3World->m_nClientMapCopyIndex
        );
        KGLOG_PROCESS_ERROR(pScene);

        nResult = pScene->LuaGetObj(L);
Exit0:
		return nResult;
	}


	int LuaSendGMCommand(Lua_State* L)
	{
		BOOL bRetCode = false;

		const char* pszCommand = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pszCommand);

		bRetCode = g_PlayerClient.DoGmCommand(pszCommand);
		KGLOG_PROCESS_ERROR(bRetCode);

Exit0:

		return 0;
	}

	int LuaGetMapParams(Lua_State* L)
	{
		BOOL bRetCode = false;
		DWORD dwMapID = 0;
		KMapParams* pMapParams = NULL;
		char szDir[MAX_PATH];
		szDir[0] = _T('\0');

		bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		dwMapID = static_cast<DWORD>(Lua_ValueToNumber(L, 1));

		pMapParams = g_pSO3World->m_Settings.m_MapListFile.GetMapParamByID(dwMapID);
		KGLOG_PROCESS_ERROR(pMapParams);

		Lua_PushString(L, pMapParams->szMapName);
		strncpy(szDir, pMapParams->szResourceFilePath, sizeof(szDir));
		szDir[sizeof(szDir) - 1] = _T('\0');
		char *pszClip = strrchr(szDir, '\\');
		if (pszClip)
		{
			char *pszNext = strrchr(pszClip, '/');
			if (pszNext)
				pszClip = pszNext;
			*pszClip = '\0';
		}
		else
		{
			pszClip = strrchr(szDir, '/');
			if (pszClip)
				*pszClip = '\0';
		}
		Lua_PushString(L, szDir);

		return 2;
Exit0:
		return 0;
	}
	
	int LuaGetMapList(Lua_State* L)
	{
		int nRelIndex = 0;

		assert(L);

		Lua_NewTable(L);
		int nTable = Lua_GetTopIndex(L);
		const KMAP_PARAM_TABLE &MapList = g_pSO3World->m_Settings.m_MapListFile.GetMapList();
		for (KMAP_PARAM_TABLE::const_iterator it = MapList.begin(); it != MapList.end(); ++it)
		{
			nRelIndex++;
			Lua_PushNumber(L, nRelIndex);
			Lua_PushNumber(L, it->first);
			Lua_SetTable(L, nTable);
		}
		return 1;
	}

    int _LuaFillSkillRecipe(Lua_State* L, int nTableIndex, KSKILL_RECIPE_KEY* pSkillRecipeKey)
    {
        int             nResult             = false;
        int             nRetCode            = false;
        int             nRestoreExit        = 0;
        char            szRecipeKey[64];

        assert(L);
        assert(pSkillRecipeKey);
        assert(nTableIndex > 0);

        nRetCode = Lua_IsTable(L, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRestoreExit = Lua_GetTopIndex(L);

        lua_getfield(L, nTableIndex, "skill_id");
        nRetCode = Lua_IsNumber(L, -1);
        KGLOG_PROCESS_ERROR(nRetCode);

        pSkillRecipeKey->dwSkillID = (DWORD)Lua_ValueToNumber(L, -1);

            
        lua_getfield(L, nTableIndex, "skill_level");
        nRetCode = Lua_IsNumber(L, -1);
        KGLOG_PROCESS_ERROR(nRetCode);

        pSkillRecipeKey->dwSkillLevel = (DWORD)Lua_ValueToNumber(L, -1);

        for (int i = 0; i < MAX_SKILL_REICPE_COUNT; i++)
        {
            snprintf(szRecipeKey, sizeof(szRecipeKey), "recipe%d", i + 1);
            szRecipeKey[sizeof(szRecipeKey) - 1] = 0;

            lua_getfield(L, nTableIndex, szRecipeKey);
            nRetCode = Lua_IsNumber(L, -1);
            KGLOG_PROCESS_ERROR(nRetCode);

            pSkillRecipeKey->dwRecipeKey[i] = (DWORD)Lua_ValueToNumber(L, -1);
        }


        std::sort(pSkillRecipeKey->dwRecipeKey, (pSkillRecipeKey->dwRecipeKey) + MAX_SKILL_REICPE_COUNT);

        nResult = true;
    Exit0:

        if (nRestoreExit != 0)
        {
            Lua_SetTopIndex(L, nRestoreExit);
        }

        return nResult;
    }

#define LUA_FILL_TABLE_INT_ARRAY(L, nTable, Name, nIndex, Store)    \
    do                                                              \
    {                                                               \
        Lua_PushString(L, #Name#nIndex);                            \
        Lua_PushNumber(L, (Store).n##Name[##nIndex - 1]);           \
        Lua_SetTable(L, nTable);                                    \
    } while(false)
    

#define LUA_FILL_TABLE_INT(L, nTable, Name, Store)      \
    do                                                  \
    {                                                   \
        Lua_PushString(L, #Name);                       \
        Lua_PushNumber(L, (Store).n##Name);             \
        Lua_SetTable(L, nTable);                        \
    } while(false)

    int LuaGetSkillInfo(Lua_State* L)
    {
        int                 nResult             = 0;
        int                 nRetCode            = false;
        int                 nTable              = 0;
        KSkill*             pSkill              = NULL;
        KSKILL_DISPLAY_INFO SkillDisplayInfo    = {0};
        KSKILL_RECIPE_KEY   SkillRecipeKey;

        assert(L);

        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        nRetCode = _LuaFillSkillRecipe(L, 1, &SkillRecipeKey);
        KGLOG_PROCESS_ERROR(nRetCode);

        pSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
        KG_PROCESS_ERROR(pSkill);

        nRetCode = pSkill->GetSkillDisplayInfo(&SkillDisplayInfo);
        KGLOG_PROCESS_ERROR(nRetCode);

        Lua_NewTable(L);
        nTable = Lua_GetTopIndex(L);

        LUA_FILL_TABLE_INT(L, nTable, CastTime, SkillDisplayInfo);
        LUA_FILL_TABLE_INT_ARRAY(L, nTable, CoolDown, 1, SkillDisplayInfo);
        LUA_FILL_TABLE_INT_ARRAY(L, nTable, CoolDown, 2, SkillDisplayInfo);
        LUA_FILL_TABLE_INT_ARRAY(L, nTable, CoolDown, 3, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, AreaRadius, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, MinRadius, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, MaxRadius, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, CostMana, SkillDisplayInfo);
		LUA_FILL_TABLE_INT(L, nTable, CostLife, SkillDisplayInfo);

        LUA_FILL_TABLE_INT(L, nTable, MinPhysicsDamage, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, MaxPhysicsDamage, SkillDisplayInfo);

        LUA_FILL_TABLE_INT(L, nTable, MinSolarDamage, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, MaxSolarDamage, SkillDisplayInfo);

        LUA_FILL_TABLE_INT(L, nTable, MinNeutralDamage, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, MaxNeutralDamage, SkillDisplayInfo);

        LUA_FILL_TABLE_INT(L, nTable, MinLunarDamage, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, MaxLunarDamage, SkillDisplayInfo);

        LUA_FILL_TABLE_INT(L, nTable, MinPoisonDamage, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, MaxPoisonDamage, SkillDisplayInfo);

        LUA_FILL_TABLE_INT(L, nTable, MinTherapy, SkillDisplayInfo);
        LUA_FILL_TABLE_INT(L, nTable, MaxTherapy, SkillDisplayInfo);

        nResult = 1;
    Exit0:
        return nResult;
    }

    int GetBuffAttributeData(KAttribute* pAttributeHandle, int* pnAttributeData, size_t uSize)
    {
        int         nResult     = false;
        KAttribute* pAttribute  = NULL;

        assert(pnAttributeData);

        for (pAttribute = pAttributeHandle; pAttribute; pAttribute = pAttribute->pNext)
        {
            assert(pAttribute->nKey > atInvalid && pAttribute->nKey < atTotal);

            KGLOG_PROCESS_ERROR(pAttribute->nKey < (int)uSize);

            pnAttributeData[pAttribute->nKey] += pAttribute->nValue1;
        }

        nResult = true;
    Exit0:
        return nResult;
    }

	int LuaSkillRecipeKeyToIDAndLevel(Lua_State* L)
	{
		DWORD dwRecipeID = 0;
		DWORD dwRecipeLevel = 0;
		DWORD dwRecipeKey = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		dwRecipeKey = (DWORD)Lua_ValueToNumber(L, 1);
		GET_RECIPE_ID_OF_KEY(dwRecipeID, dwRecipeKey);
		GET_RECIPE_LEVEL_OF_KEY(dwRecipeLevel, dwRecipeKey);

Exit0:
		Lua_PushNumber(L, dwRecipeID);
		Lua_PushNumber(L, dwRecipeLevel);
		return 2;
	}

	int LuaSkillRecipeIDAndLevelToKey(Lua_State* L)
	{
		DWORD dwRecipeID = 0;
		DWORD dwRecipeLevel = 0;
		DWORD dwRecipeKey = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		dwRecipeID = (DWORD)Lua_ValueToNumber(L, 1);
		dwRecipeLevel = (DWORD)Lua_ValueToNumber(L, 2);
		MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

Exit0:
		Lua_PushNumber(L, dwRecipeKey);
		return 1;
	}

    int LuaGetBuffInfo(Lua_State* L)
    {
        int                 nResult                 = 0;
        int                 nRetCode                = 0;
        int                 nIndex                  = 0;
        int                 nTable                  = 0;
        DWORD               dwBuffID                = 0;
        int                 nBuffLevel              = 0;
        int                 nBuffDetachType         = 0;
        char*               pszGetAttributeName     = NULL;
        KBuff*              pBuff                   = NULL;
        KBuffRecipePointer  BuffRecipePointer;
        static int          s_nAttributeValue[atTotal];
        KBUFF_RECIPE_KEY    BuffRecipeKey;

        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 3);
        
        dwBuffID    = (DWORD)Lua_ValueToNumber(L, 1);
        nBuffLevel  = (int)Lua_ValueToNumber(L, 2);

        BuffRecipeKey.dwID      = dwBuffID;
        BuffRecipeKey.nLevel    = nBuffLevel;
        memset(BuffRecipeKey.dwRecipeKey, 0, sizeof(BuffRecipeKey.dwRecipeKey));

        pBuff = BuffRecipePointer.SetRecipeKey(BuffRecipeKey);
        KGLOG_PROCESS_ERROR(pBuff);

        nBuffDetachType = pBuff->m_nDetachType;

        memset(s_nAttributeValue, 0, sizeof(s_nAttributeValue));

        nRetCode = GetBuffAttributeData(pBuff->m_pBeginAttribList, s_nAttributeValue, sizeof(s_nAttributeValue) / sizeof(int));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetBuffAttributeData(pBuff->m_pActiveAttribList, s_nAttributeValue, sizeof(s_nAttributeValue) / sizeof(int));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetBuffAttributeData(pBuff->m_pEndTimeAttribList, s_nAttributeValue, sizeof(s_nAttributeValue) / sizeof(int));
        KGLOG_PROCESS_ERROR(nRetCode);

        Lua_NewTable(L);
        nTable = Lua_GetTopIndex(L);

        Lua_PushString(L, "nDetachType");
        Lua_PushNumber(L, nBuffDetachType);
        Lua_SetTable(L, nTable);

        Lua_PushNil(L);
        while (Lua_Next(L, 3))
        {
            pszGetAttributeName = (char*)Lua_ValueToString(L, -1);
            KGLOG_PROCESS_ERROR(pszGetAttributeName);

            nRetCode = AttributeStringToID(pszGetAttributeName, nIndex);
            KGLOG_PROCESS_ERROR(nRetCode);

            Lua_PushString(L, pszGetAttributeName);
            Lua_PushNumber(L, s_nAttributeValue[nIndex]);
            Lua_SetTable(L, nTable);

            Lua_Pop(L, 1);
        }

        Lua_SetTopIndex(L, nTable);

        nResult = 1;
    Exit0:
        return nResult;
    }

    int LuaGetBindBuffInfo(Lua_State* L)
    {
        int                 nResult                 = 0;
        int                 nRetCode                = 0;
        int                 nIndex                  = 0;
        int                 nTable                  = 0;
        DWORD               dwBuffIndex             = 0;
        int                 nBuffDetachType         = 0;
        char*               pszGetAttributeName     = NULL;
        KSkill*             pSkill                  = NULL;
        KBuff*              pBuff                   = NULL;
        static int          s_nAttributeValue[atTotal];
        KSKILL_RECIPE_KEY   RecipeKey;

        assert(L);

        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 3);

        dwBuffIndex = (DWORD)Lua_ValueToNumber(L, 1);
        nRetCode = _LuaFillSkillRecipe(L, 2, &RecipeKey);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(dwBuffIndex > 0 && dwBuffIndex <= MAX_SKILL_BIND_BUFF_COUNT);

        pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
        KGLOG_PROCESS_ERROR(pSkill);

        memset(s_nAttributeValue, 0, sizeof(s_nAttributeValue));

        pBuff = pSkill->m_BindBuffs[dwBuffIndex - 1].GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        nBuffDetachType = pBuff->m_nDetachType;

        nRetCode = GetBuffAttributeData(pBuff->m_pBeginAttribList, s_nAttributeValue, sizeof(s_nAttributeValue) / sizeof(int));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetBuffAttributeData(pBuff->m_pActiveAttribList, s_nAttributeValue, sizeof(s_nAttributeValue) / sizeof(int));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetBuffAttributeData(pBuff->m_pEndTimeAttribList, s_nAttributeValue, sizeof(s_nAttributeValue) / sizeof(int));
        KGLOG_PROCESS_ERROR(nRetCode);

        Lua_NewTable(L);
        nTable = Lua_GetTopIndex(L);

        nResult = 1;

        Lua_PushString(L, "nDetachType");
        Lua_PushNumber(L, nBuffDetachType);
        Lua_SetTable(L, nTable);

        Lua_PushNil(L);
        while (Lua_Next(L, 3))
        {
            pszGetAttributeName = (char*)Lua_ValueToString(L, -1);
            KGLOG_PROCESS_ERROR(pszGetAttributeName);

            nRetCode = AttributeStringToID(pszGetAttributeName, nIndex);
            KGLOG_PROCESS_ERROR(nRetCode);

            Lua_PushString(L, pszGetAttributeName);
            Lua_PushNumber(L, s_nAttributeValue[nIndex]);
            Lua_SetTable(L, nTable);

            Lua_Pop(L, 1);
        }

        Lua_SetTopIndex(L, nTable);

    Exit0:
        return nResult;
    }

    int LuaGetBuffTime(Lua_State* L)
    {
        int                 nResult                 = 0;
        int                 nRetCode                = 0;
        DWORD               dwBuffID                = 0;
        int                 nBuffLevel              = 0;
        int                 nDurationCount          = 0;
        int                 nIntervalFrame          = 0;
        KBuff*              pBuff                   = NULL;
        KBUFF_RECIPE_KEY    BuffRecipeKey;
        KBuffRecipePointer  BuffRecipePointer;

        assert(L);

        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 2);

        dwBuffID = (DWORD)Lua_ValueToNumber(L, 1);
        nBuffLevel = (int)Lua_ValueToNumber(L, 2);

        BuffRecipeKey.dwID      = dwBuffID;
        BuffRecipeKey.nLevel    = nBuffLevel;
        memset(BuffRecipeKey.dwRecipeKey, 0, sizeof(BuffRecipeKey.dwRecipeKey));

        pBuff = BuffRecipePointer.SetRecipeKey(BuffRecipeKey);
        KGLOG_PROCESS_ERROR(pBuff);

        nDurationCount = pBuff->m_nActiveCount;
        nIntervalFrame = pBuff->m_nIntervalFrame;

		lua_pushinteger(L, nDurationCount * nIntervalFrame);
        lua_pushinteger(L, nDurationCount);
        lua_pushinteger(L, nIntervalFrame);

        nResult = 3;
Exit0:
        return nResult;
    }

    int LuaGetBindBuffTime(Lua_State* L)
    {
        int                 nResult         = 0;
        int                 nRetCode        = 0;
        DWORD               dwSkillID       = 0;
        DWORD               dwSkillLevel    = 0;
        DWORD               dwBuffIndex     = 0;
        int                 nDurationCount  = 0;
        int                 nIntervalFrame  = 0;
        KSkill*             pSkill          = NULL;
        KBuff*              pBuff           = NULL;
        KSKILL_RECIPE_KEY   RecipeKey;

        assert(L);

        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 2);

        dwBuffIndex = (DWORD)Lua_ValueToNumber(L, 1);
        nRetCode    = _LuaFillSkillRecipe(L, 2, &RecipeKey);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(dwBuffIndex > 0 && dwBuffIndex < MAX_SKILL_BIND_BUFF_COUNT);

        pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
        KGLOG_PROCESS_ERROR(pSkill);
    
        pBuff = pSkill->m_BindBuffs[dwBuffIndex - 1].GetPointer();
        KGLOG_PROCESS_ERROR(pBuff);

        nDurationCount = pBuff->m_nActiveCount;
        nIntervalFrame = pBuff->m_nIntervalFrame;

		lua_pushinteger(L, nDurationCount * nIntervalFrame);
        lua_pushinteger(L, nDurationCount);
        lua_pushinteger(L, nIntervalFrame);

        nResult = 3;
Exit0:
        return nResult;
    }
#endif	//_CLIENT

	static SCENE_OBJ_RELATION_TYPE GetRelation(Lua_State* L)
	{
        DWORD dwSrcCharacterID              = 0;
        DWORD dwDstCharacterID              = 0;
        KCharacter* pSrcCharacter           = NULL;
        KCharacter* pDstCharacter           = NULL;
        SCENE_OBJ_RELATION_TYPE eRelation   = sortInvalid;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		dwSrcCharacterID = (DWORD)Lua_ValueToNumber(L, 1);
		dwDstCharacterID = (DWORD)Lua_ValueToNumber(L, 2);

		if (IS_PLAYER(dwSrcCharacterID))
		{
			pSrcCharacter = g_pSO3World->m_PlayerSet.GetObj(dwSrcCharacterID);
		}
		else
		{
			pSrcCharacter = g_pSO3World->m_NpcSet.GetObj(dwSrcCharacterID);
		}
		KGLOG_PROCESS_ERROR(pSrcCharacter);
        KGLOG_PROCESS_ERROR(pSrcCharacter->m_pScene);

		if (IS_PLAYER(dwDstCharacterID))
		{
			pDstCharacter = g_pSO3World->m_PlayerSet.GetObj(dwDstCharacterID);
		}
		else
		{
			pDstCharacter = g_pSO3World->m_NpcSet.GetObj(dwDstCharacterID);
		}
		KGLOG_PROCESS_ERROR(pDstCharacter);
        KGLOG_PROCESS_ERROR(pDstCharacter->m_pScene);

		eRelation = (SCENE_OBJ_RELATION_TYPE)g_GetRelation(pSrcCharacter, pDstCharacter);
Exit0:
		return eRelation;
	}

	int LuaGetRelation(Lua_State* L)
	{
		SCENE_OBJ_RELATION_TYPE eRelation = GetRelation(L);
		Lua_PushNumber(L, eRelation);
		return 1;
	}

	int LuaIsNeutrality(Lua_State* L)
	{
		SCENE_OBJ_RELATION_TYPE eRelation = GetRelation(L);
		Lua_PushBoolean(L, (eRelation & sortNeutrality) == sortNeutrality);
		return 1;
	}

	int LuaIsAlly(Lua_State* L)
	{
		SCENE_OBJ_RELATION_TYPE eRelation = GetRelation(L);
		Lua_PushBoolean(L, (eRelation & sortAlly) == sortAlly);
		return 1;
	}

	int LuaIsEnemy(Lua_State* L)
	{
		SCENE_OBJ_RELATION_TYPE eRelation = GetRelation(L);
		Lua_PushBoolean(L, (eRelation & sortEnemy) == sortEnemy);
		return 1;
	}

	int LuaIsParty(Lua_State* L)
	{
		SCENE_OBJ_RELATION_TYPE eRelation = GetRelation(L);
		Lua_PushBoolean(L, (eRelation & sortParty) == sortParty);
		return 1;
	}

	int LuaIsSelf(Lua_State* L)
	{
		SCENE_OBJ_RELATION_TYPE eRelation = GetRelation(L);
		Lua_PushBoolean(L, (eRelation & sortSelf) == sortSelf);
		return 1;
	}

    int LuaIsCommonSkill(Lua_State* L)
    {
        int     nRetCode        = 0;
        BOOL    bCommonSkill    = false;
        BOOL    bMeleeeSkill    = false;
        DWORD   dwSkillID       = 0;
        int     nWeaponType     = 0;
        
        nRetCode = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nRetCode == 1);
    
        dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);
        bCommonSkill = g_pSO3World->m_SkillManager.IsCommonSkill(dwSkillID);
        nWeaponType  = g_pSO3World->m_SkillManager.CommonSkillMapWeapon(dwSkillID);

        switch (nWeaponType)
        {
        case wdtWand:
        case wdtSpear:       
        case wdtSword:       
        case wdtFist:        
        case wdtDoubleWeapon:
        case wdtPen:
            bMeleeeSkill = true;
            break;
        default:
            KG_PROCESS_ERROR(false);
        }
        
Exit0:
        Lua_PushBoolean(L, bCommonSkill);
        Lua_PushBoolean(L, bMeleeeSkill);
        return 2; 
    }

	int LuaRandom(Lua_State* L)
	{
		int nResult = 0;
		int nRetCode = 0;
		int nX = 0;
		int nY = 0;

		nRetCode = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nRetCode == 1 || nRetCode == 2);

		if (nRetCode == 1)
		{
			nX = 1;
			nY = (int)Lua_ValueToNumber(L, 1);
		}
		else if (nRetCode == 2)
		{
			nX = (int)Lua_ValueToNumber(L, 1);
			nY = (int)Lua_ValueToNumber(L, 2);
		}
		else
		{
			KGLOG_PROCESS_ERROR(false);
		}

		KGLOG_PROCESS_ERROR(nX >= 1 && nY >= nX);

		nResult = g_Random(nY - nX + 1) + nX;
Exit0:
		lua_pushinteger(L, nResult);
		return 1;
	}

#ifdef _CLIENT
    int LuaGetPingValue(Lua_State* L)
    {
        int nPingValue = g_PlayerClient.GetPingValue();

        lua_pushinteger(L, nPingValue);

        return 1;
    }

	int LuaGetPlayerByName(Lua_State* L)
	{
		assert(L);

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		LPCTSTR pcszName = (LPCTSTR)Lua_ValueToString(L, 1);
		KG_PROCESS_ERROR(pcszName);

		struct KPlayerFuc
		{
			LPCTSTR pcszName;
			KPlayer *pPlayer;
			KPlayerFuc(LPCTSTR pcszName)
			{
				assert(pcszName);
				this->pcszName = pcszName;
				pPlayer = NULL;
			};
			
			int operator()(DWORD dwID, KPlayer *pInPlayer)
			{
				if (pInPlayer && _tcscmp(pInPlayer->m_szName, pcszName) == 0)
				{
					pPlayer = pInPlayer;
					return false;
				}
				return true;
			}
		};
		{
			KPlayerFuc pf(pcszName);
			g_pSO3World->m_PlayerSet.Traverse(pf);
			KG_PROCESS_ERROR(pf.pPlayer);
			pf.pPlayer->LuaGetObj(L);
		}

		return 1;
Exit0:
		return 0;

	}
#endif

	int LuaGetDoodadTemplate(Lua_State* L)
	{
		BOOL bRetCode = false;
		KDoodadTemplate* pDoodadTemplate = NULL;
		DWORD dwDoodadTemplateID = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		dwDoodadTemplateID = (DWORD)Lua_ValueToNumber(L, 1);
		KG_PROCESS_ERROR(dwDoodadTemplateID);

		pDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(dwDoodadTemplateID);
		KGLOG_PROCESS_ERROR(pDoodadTemplate);

		return pDoodadTemplate->LuaGetObj(L);
Exit0:
		return 0;
	}

	int LuaGetLocalTime(Lua_State* L)
	{
		int nResult = 0;
		DATE_FORMAT eFormat = dfInvalid;
		struct tm* pLocalTime = localtime(&g_pSO3World->m_nCurrentTime);
		
		if (Lua_GetTopIndex(L) == 1)
		{
			eFormat = (DATE_FORMAT)((DWORD)Lua_ValueToNumber(L, 1));

			switch(eFormat)
			{
			case dfSecond:
				nResult = pLocalTime->tm_sec;
				break;
			case dfMinute:
				nResult = pLocalTime->tm_min;
				break;
			case dfHour:
				nResult = pLocalTime->tm_hour;
				break;
			case dfDay:
				nResult = pLocalTime->tm_mday;
				break;
			case dfMonth:
				nResult = pLocalTime->tm_mon + 1;
				break;
			case dfYear:
				nResult = pLocalTime->tm_year + 1900;
				break;
			case dfWeek:
				nResult = pLocalTime->tm_wday;
				break;
			default:
				KGLogPrintf(KGLOG_ERR, "GetLocalTime Error date format!");
				break;
			}
			lua_pushinteger(L, nResult);
		}
		else
		{
			Lua_NewTable(L);

			Lua_PushString(L, "year");
			Lua_PushNumber(L, pLocalTime->tm_year + 1900);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "month");
			Lua_PushNumber(L, pLocalTime->tm_mon + 1);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "day");
			Lua_PushNumber(L, pLocalTime->tm_mday);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "hour");
			Lua_PushNumber(L, pLocalTime->tm_hour);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "minute");
			Lua_PushNumber(L, pLocalTime->tm_min);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "second");
			Lua_PushNumber(L, pLocalTime->tm_sec);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "weekday");
			Lua_PushNumber(L, pLocalTime->tm_wday);
			Lua_SetTable(L, -3);
		}

		return 1;
	}

	int LuaGetCharacterDistance(Lua_State* L)
	{
		int         nResult             = -1;
        int         nRetCode            = false;
		KCharacter* pSrcCharacter       = NULL;
		KCharacter* pDstCharacter       = NULL;
        DWORD       dwSrcCharacterID    = ERROR_ID;
        DWORD       dwDstCharacterID    = ERROR_ID;
		int         nSrcX               = 0;
		int         nSrcY               = 0;
		int         nSrcZ               = 0;
		int         nDstX               = 0;
		int         nDstY               = 0;
		int         nDstZ               = 0;
		long long   llDistance          = 0;

        nRetCode = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nRetCode == 2);

		dwSrcCharacterID = (DWORD)Lua_ValueToNumber(L, 1);
		dwDstCharacterID = (DWORD)Lua_ValueToNumber(L, 2);

		if (IS_PLAYER(dwSrcCharacterID))
			pSrcCharacter = g_pSO3World->m_PlayerSet.GetObj(dwSrcCharacterID);
		else
			pSrcCharacter = g_pSO3World->m_NpcSet.GetObj(dwSrcCharacterID);

    	KG_PROCESS_ERROR(pSrcCharacter);

		if (IS_PLAYER(dwDstCharacterID))
			pDstCharacter = g_pSO3World->m_PlayerSet.GetObj(dwDstCharacterID);
		else
			pDstCharacter = g_pSO3World->m_NpcSet.GetObj(dwDstCharacterID);

    	KG_PROCESS_ERROR(pDstCharacter);

        nRetCode = pSrcCharacter->GetAbsoluteCoordinate(&nSrcX, &nSrcY, &nSrcZ);
        KG_PROCESS_ERROR(nRetCode);

        nRetCode = pDstCharacter->GetAbsoluteCoordinate(&nDstX, &nDstY, &nDstZ);
        KG_PROCESS_ERROR(nRetCode);

		llDistance = g_GetDistance3(
            (long long)nSrcX, (long long)nSrcY, (long long)ZPOINT_TO_XYPOINT(nSrcZ), 
            (long long)nDstX, (long long)nDstY, (long long)ZPOINT_TO_XYPOINT(nDstZ)
        );
		nResult = (int)sqrt((double)llDistance);
Exit0:
		Lua_PushNumber(L, nResult);
		return 1;
	}

	int LuaGetDistanceSq(Lua_State* L)
	{
		int nX1 = 0;
		int nY1 = 0;
		int nZ1 = 0;
		int nX2 = 0;
		int nY2 = 0;
        int nZ2 = 0;
        long long llDistance = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 6);

		nX1 = (int)Lua_ValueToNumber(L, 1);
		nY1 = (int)Lua_ValueToNumber(L, 2);
		nZ1 = (int)Lua_ValueToNumber(L, 3);
		nX2 = (int)Lua_ValueToNumber(L, 4);
		nY2 = (int)Lua_ValueToNumber(L, 5);
		nZ2 = (int)Lua_ValueToNumber(L, 6);

		llDistance = g_GetDistance3(
            (long long)nX1, (long long)nY1, (long long)ZPOINT_TO_XYPOINT(nZ1),
            (long long)nX2, (long long)nY2, (long long)ZPOINT_TO_XYPOINT(nZ2)
        );

Exit0:
        Lua_PushNumber(L, (double)llDistance);
		return 1;
	}

#if defined(_CLIENT)

	int LuaLoadingComplete(Lua_State* L)
	{
		g_PlayerClient.DoApplyEnterScene();
		return 0;
	}

	int LuaSetUserPreferences(Lua_State* L)
	{
		BOOL bRetCode = false;
		KPlayer* pPlayer = NULL;
		int nTotalLength = 0;
		int nTotalOffSet = 0;
		int nOffset = 0;
		int nParam = Lua_GetTopIndex(L);
		int nCurrent = 3;

		KGLOG_PROCESS_ERROR(nParam > 2);
		pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
		KGLOG_PROCESS_ERROR(pPlayer);

		nTotalOffSet = nOffset = (int)Lua_ValueToNumber(L, 1);
		char *pcszEncodeType = (char *)Lua_ValueToString(L, 2);
		KG_PROCESS_ERROR(pcszEncodeType);
		while (*pcszEncodeType)
		{
			KG_PROCESS_ERROR(nCurrent <= nParam);
			switch(*pcszEncodeType)
			{
			case 'd':
			case 'D':
				{
					DWORD dwData = (DWORD)Lua_ValueToNumber(L, nCurrent);
					bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(nOffset, 4, (BYTE*)(&dwData));
					KGLOG_PROCESS_ERROR(bRetCode);
					nTotalLength += 4;
					nOffset += 4;
				}
				break;
			case 'n':
			case 'N':
				{
					int nData = (int)Lua_ValueToNumber(L, nCurrent);
					bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(nOffset, 4, (BYTE*)(&nData));
					KGLOG_PROCESS_ERROR(bRetCode);
					nTotalLength += 4;
					nOffset += 4;
				}
				break;
			case 'c':
			case 'C':
				{
					char cData = (char)Lua_ValueToNumber(L, nCurrent);
					bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(nOffset, 1, (BYTE*)(&cData));
					KGLOG_PROCESS_ERROR(bRetCode);
					nTotalLength += 1;
					nOffset += 1;
				}
				break;
			case 'b':
			case 'B':
				{
					char cData = 0x00;
					if(Lua_ValueToBoolean(L, nCurrent))
						cData = 0x01;
					bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(nOffset, 1, (BYTE*)(&cData));
					KGLOG_PROCESS_ERROR(bRetCode);
					nTotalLength += 1;
					nOffset += 1;
				}
				break;
			case 'w':
			case 'W':
				{
					WORD wData = (WORD)Lua_ValueToNumber(L, nCurrent);
					bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(nOffset, 2, (BYTE*)(&wData));
					KGLOG_PROCESS_ERROR(bRetCode);
					nTotalLength += 2;
					nOffset += 2;
				}
				break;
			case 'f':
			case 'F':
				{
					float fData = (float)Lua_ValueToNumber(L, nCurrent);
					bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(nOffset, 4, (BYTE*)(&fData));
					KGLOG_PROCESS_ERROR(bRetCode);
					nTotalLength += 4;
					nOffset += 4;
				}
				break;
			case 's':
			case 'S':
				{
					char *pcszData = (char *)Lua_ValueToString(L, nCurrent);
					KGLOG_PROCESS_ERROR(pcszData);
					int nLen = (int)strlen(pcszData) + 1;
					bRetCode = pPlayer->m_UserPreferences.SetUserPreferences(nOffset, nLen, (BYTE*)(&pcszData));
					KGLOG_PROCESS_ERROR(bRetCode);
					nTotalLength += nLen;
					nOffset += nLen;
				}
				break;
			}
			++nCurrent;
			++pcszEncodeType;
		}

		BYTE *pData = pPlayer->m_UserPreferences.GetUserPreferences(nTotalOffSet);
		KGLOG_PROCESS_ERROR(pData);
		bRetCode = g_PlayerClient.DoSyncUserPreferences(nTotalOffSet, nTotalLength, pData);
		KGLOG_PROCESS_ERROR(bRetCode);
		
Exit0:
		return 0;
	}

	int LuaGetUserPreferences(Lua_State* L)
	{
		KPlayer* pPlayer = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		int nOffset = (int)Lua_ValueToNumber(L, 1);
		char *pcszType = (char *)Lua_ValueToString(L, 2);

		pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
		KGLOG_PROCESS_ERROR(pPlayer);

		BYTE *pData = pPlayer->m_UserPreferences.GetUserPreferences(nOffset);
		KGLOG_PROCESS_ERROR(pData);

		switch(*pcszType)
		{
		case 'd':
		case 'D':
			Lua_PushNumber(L, *((DWORD *)pData));
			break;
		case 'n':
		case 'N':
			Lua_PushNumber(L, *((int *)pData));
			break;
		case 'c':
		case 'C':
			Lua_PushNumber(L, *((char *)pData));
			break;
		case 'b':
		case 'B':
			Lua_PushBoolean(L, (*((char *)pData)) ? TRUE : FALSE);
			break;
		case 'w':
		case 'W':
			Lua_PushNumber(L, *((WORD *)pData));
			break;
		case 'f':
		case 'F':
			Lua_PushNumber(L, *((float *)pData));
			break;
		case 's':
		case 'S':
			Lua_PushString(L, (char *)pData);
			break;
		default:
			KGLOG_PROCESS_ERROR(false && "Error Param!");
		}

		return 1;
Exit0:
		return 0;
	}

	int LuaGetReputeLimit(Lua_State* L)
	{
		int nLImit = -1;
		int nLevel = 0;
		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		nLevel = (int)Lua_ValueToNumber(L, 1);
		nLImit = g_pSO3World->m_Settings.m_ReputeLimit.GetReputeLimit(nLevel);
Exit0:
		Lua_PushNumber(L, nLImit);
		return 1;
	}

	int LuaResetGameworld(Lua_State* L)
	{
		// TODO: 重置游戏逻辑

        g_PlayerClient.Disconnect();
        g_pSO3World->m_TeamClient.Reset();
        g_pSO3World->m_FellowshipMgr.Reset();
        g_pSO3World->m_TongClient.Reset();
        g_pSO3World->m_ClientMailList.clear();

        KDeleteSceneFunc DelFunc;

        g_pSO3World->m_SceneSet.Traverse(DelFunc);

        g_pSO3World->m_dwClientPlayerID     = ERROR_ID;
        g_pSO3World->m_dwClientMapID        = ERROR_ID;
        g_pSO3World->m_nClientMapCopyIndex  = 0;
        g_pSO3World->m_nGameLoop            = 0;
        g_pSO3World->m_dwStartTime          = 0;
        g_pSO3World->m_nStartLoop           = 0;

		return 0;
	}

#endif //_CLIENT

	int LuaGetBankPackagePrice(Lua_State* L)
	{
		int nLevel = 0;
		int nTopIndex = 0;
		int nPrice = 0;

		nTopIndex = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nTopIndex == 1);

		nLevel = (int)Lua_ValueToNumber(L, 1);
		KGLOG_PROCESS_ERROR(nLevel >= 1 && nLevel <= MAX_BANK_PACKAGE_COUNT);

		nPrice = g_pSO3World->m_Settings.m_ConstList.nBankPackagePrice[nLevel - 1];
		KGLOG_PROCESS_ERROR(nPrice >= 0);

		Lua_PushNumber(L, nPrice);
		return 1;
Exit0:
		return 0;
	}

#ifdef _SERVER
int LuaSendSystemMail(Lua_State* L)
{
    int     nRetCode        = false;
    int     nTopIndex       = 0;
    char*   pszSender       = NULL;
    char*   pszReciever     = NULL;
    char*   pszMailTitle    = NULL;
    char*   pszText         = NULL;
    int     nMoney          = 0;
    DWORD   dwTabType       = 0;
    DWORD   dwIndex         = 0;
    KItem*  pItem           = NULL;
    BYTE    byItemData[1024];
    size_t  uItemDataSize   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex >= 4);
    KGLOG_PROCESS_ERROR(nTopIndex <= 7);

    pszSender = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszSender);

    pszReciever     = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszReciever);

    pszMailTitle    = (char*)Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pszMailTitle);

    pszText         = (char*)Lua_ValueToString(L, 4);
    KGLOG_PROCESS_ERROR(pszText);

    if (nTopIndex >= 5)
    {
        nMoney      = (int)Lua_ValueToNumber(L, 5);
    }
    KGLOG_PROCESS_ERROR(nMoney >= 0);

    if (nTopIndex == 7)
    {
        dwTabType   = (DWORD)Lua_ValueToNumber(L, 6);
        dwIndex     = (DWORD)Lua_ValueToNumber(L, 7);

        pItem = g_pSO3World->m_ItemManager.GenerateItem(CURRENT_ITEM_VERSION, dwTabType, dwIndex);
        KGLOG_PROCESS_ERROR(pItem);

        nRetCode = pItem->GetBinaryData(&uItemDataSize, byItemData, sizeof(byItemData));
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    g_RelayClient.DoSendMailRequest(
        1, pszSender, pszReciever, pszMailTitle, 0, 
        true, nMoney, pszText, byItemData, uItemDataSize
    );

Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
    return 0;
}

int LuaSendGlobalMail(Lua_State* L)
{
    int     nRetCode        = false;
    int     nTopIndex       = 0;
    char*   pszSender       = NULL;
    char*   pszMailTitle    = NULL;
    char*   pszText         = NULL;
    int     nLifeTime       = 60 * 60 * 24 * 7;
    int     nMoney          = 0;
    DWORD   dwTabType       = 0;
    DWORD   dwIndex         = 0;
    KItem*  pItem           = NULL;
    size_t  uItemDataSize   = 0;
    BYTE    byItemData[1024];

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex >= 3);
    KGLOG_PROCESS_ERROR(nTopIndex <= 7);

    pszSender = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszSender);

    pszMailTitle    = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszMailTitle);

    pszText         = (char*)Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pszText);

    if (nTopIndex >= 4)
    {
        nLifeTime   = (int)Lua_ValueToNumber(L, 4);
    }
    KGLOG_PROCESS_ERROR(nLifeTime > 0);

    if (nTopIndex >= 5)
    {    
        nMoney      = (int)Lua_ValueToNumber(L, 5);
    }
    KGLOG_PROCESS_ERROR(nMoney >= 0);

    if (nTopIndex == 7)
    {
        dwTabType   = (DWORD)Lua_ValueToNumber(L, 6);
        dwIndex     = (DWORD)Lua_ValueToNumber(L, 7);

        pItem = g_pSO3World->m_ItemManager.GenerateItem(CURRENT_ITEM_VERSION, dwTabType, dwIndex);
        KGLOG_PROCESS_ERROR(pItem);

        nRetCode = pItem->GetBinaryData(&uItemDataSize, byItemData, sizeof(byItemData));
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    g_RelayClient.DoSendGlobalMailRequest(
        pszSender, pszMailTitle, nLifeTime, 
        nMoney, pszText, byItemData, uItemDataSize
    );

Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
    return 0;
}

#endif

#ifdef _CLIENT

int LuaSendMail(Lua_State* L)
{
    int         nResult         = mrcFailed;
    int         nRetCode        = 0;
    int         nRequestID      = 0;
    KPlayer*    pPlayer         = NULL;
    char*       pDestName       = NULL;
    DWORD       dwBox           = 0;
    DWORD       dwX             = 0;
    int         nMoney          = 0;
    char*       pszMailTitle    = NULL;
    char*       pszText         = NULL;
    int         nPlayerMoney    = 0;
    
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 7);

    nRequestID  = (int)Lua_ValueToNumber(L, 1);
    pDestName   = (char*)Lua_ValueToString(L, 2);
    dwBox       = (DWORD)Lua_ValueToNumber(L, 3);
    dwX         = (DWORD)Lua_ValueToNumber(L, 4);
    nMoney      = (int)Lua_ValueToNumber(L, 5);
    pszMailTitle= (char*)Lua_ValueToString(L, 6);
    pszText     = (char*)Lua_ValueToString(L, 7);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pDestName);
    KGLOG_PROCESS_ERROR(pszMailTitle);
    KGLOG_PROCESS_ERROR(pszText);

    if (dwBox != ibInvalid)
    {
        KItem* pItem = NULL;

        int nBoxType = pPlayer->m_ItemList.GetBoxType(dwBox);
        KG_PROCESS_ERROR_RET_CODE(nBoxType == ivtPackage, mrcItemNotInPackage);

        pItem = pPlayer->m_ItemList.GetItem(dwBox, dwX);
        KGLOG_PROCESS_ERROR(pItem);

        KG_PROCESS_ERROR_RET_CODE(!pItem->IsBind(), mrcItemBeBind);
    }

    nPlayerMoney = pPlayer->m_ItemList.GetMoney();
    KG_PROCESS_ERROR_RET_CODE(nPlayerMoney >= (nMoney + KMAIL_COST), mrcNotEnoughMoney);

    nRetCode =  g_PlayerClient.DoSendMailRequest(nRequestID, pDestName, dwBox, dwX, nMoney, pszMailTitle, pszText);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = mrcSucceed;
Exit0:
    if (nResult != mrcSucceed)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventMailRespond MailRespond = { nResult };
            g_pGameWorldUIHandler->OnMailRespond(MailRespond);
        }
    }
    return 0;
}

struct IsSameMailID 
{
    IsSameMailID(DWORD dwID) : dwMailID(dwID) {};

    inline bool operator() (const _KMAIL& crClientMail)
    {
        return (crClientMail.dwMailID == dwMailID);
    }

    DWORD dwMailID;
};

int LuaDeleteMail(Lua_State* L)
{
    int     nRetCode    = false;
    DWORD   dwMailID    = 0;
    KSO3World::KCLIENT_MAIL_VECTOR::iterator it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

	dwMailID = (int)Lua_ValueToNumber(L, 1);

    it = std::find_if(
        g_pSO3World->m_ClientMailList.begin(), 
        g_pSO3World->m_ClientMailList.end(), 
        IsSameMailID(dwMailID)
    );

    nRetCode = (it != g_pSO3World->m_ClientMailList.end());
    KG_PROCESS_ERROR(nRetCode);

    if (it->dwItemID)
    {
        KItem* pItem = g_pSO3World->m_ItemSet.GetObj(it->dwItemID);
        KGLOG_PROCESS_ERROR(pItem);

        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;

        it->dwItemID = ERROR_ID;
    }

    g_pSO3World->m_ClientMailList.erase(it);

    nRetCode = g_PlayerClient.DoDeleteMail(dwMailID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int LuaCountMail(Lua_State* L)
{
    int nUnreadCount    = 0;
	int nSysCount		= 0;
	int nSysUnreadCount	= 0;
    int nTotalCount     = (int)g_pSO3World->m_ClientMailList.size();

    nUnreadCount = 0;
    for (
        KSO3World::KCLIENT_MAIL_VECTOR::iterator it = g_pSO3World->m_ClientMailList.begin(), itEnd = g_pSO3World->m_ClientMailList.end(); 
        it != itEnd; 
        ++it
    )
    {
        _KMAIL& rMail = *it;

		if (rMail.bSystemFlag)
		{
			nSysCount++;
			if (rMail.bUnReadFlag)
			{
				nSysUnreadCount++;
			}
		}

        if (!rMail.bUnReadFlag)
            continue;

        nUnreadCount++;
    }

    Lua_PushNumber(L, nUnreadCount);
    Lua_PushNumber(L, nTotalCount);
	Lua_PushNumber(L, nSysUnreadCount);
	Lua_PushNumber(L, nSysCount);

    return 4;
}

int LuaApplyMailList(Lua_State* L)
{
    bool bResult    = false;
    BOOL bRetCode   = false;

    bRetCode = g_PlayerClient.DoGetMailListRequest();
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaGetMailList(Lua_State* L)
{
    int nResult     = 0;
    int nRetCode    = 0;
    int nCount      = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    Lua_NewTable(L);

    for (
        KSO3World::KCLIENT_MAIL_VECTOR::reverse_iterator 
            rit = g_pSO3World->m_ClientMailList.rbegin(), 
            ritEnd = g_pSO3World->m_ClientMailList.rend()
        ;
        rit != ritEnd;
        ++rit
    )
    {
        _KMAIL* pClientMail = &(*rit);
        assert(pClientMail);

        assert(pClientMail->dwMailID);

        Lua_PushNumber(L, ++nCount);
        Lua_NewTable(L);

		Lua_PushString(L, "MailID");
        Lua_PushNumber(L, pClientMail->dwMailID);
		Lua_SetTable(L, -3);

        Lua_PushString(L, "SenderName");
        Lua_PushString(L, pClientMail->szSenderName);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "Title");
        Lua_PushString(L, pClientMail->szTitle);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "LeftSeconds");
        Lua_PushNumber(L, pClientMail->GetLeftSecond(g_pSO3World->m_Settings.m_ConstList.nMailSurvivalTime));
        Lua_SetTable(L, -3);

        Lua_PushString(L, "UnReadFlag");
        Lua_PushBoolean(L, pClientMail->bUnReadFlag);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "MoneyFlag");
        Lua_PushBoolean(L, pClientMail->bMoneyFlag);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "TextFlag");
        Lua_PushBoolean(L, pClientMail->bTextFlag);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "ItemFlag");
        Lua_PushBoolean(L, pClientMail->bItemFlag);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "SystemFlag");
        Lua_PushBoolean(L, pClientMail->bSystemFlag);
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetMailContent(Lua_State* L)
{
    int     nResult     = 0;
    int     nRetCode    = 0;
    DWORD   dwMailID    = 0;
    KSO3World::KCLIENT_MAIL_VECTOR::iterator it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMailID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwMailID);

    it = std::find_if(
        g_pSO3World->m_ClientMailList.begin(), 
        g_pSO3World->m_ClientMailList.end(), 
        IsSameMailID(dwMailID)
    );

    if (
        it == g_pSO3World->m_ClientMailList.end() ||
        !it->bGotContentFlag
    )
    {
        nRetCode = g_PlayerClient.DoGetMailContent(dwMailID);
        KGLOG_PROCESS_ERROR(nRetCode);

        Lua_PushBoolean(L, false);
        nResult = 1;
        goto Exit0;
    }

    Lua_PushBoolean(L, true);


    Lua_PushString(L, it->szSenderName);
    Lua_PushString(L, it->szTitle);
    Lua_PushNumber(L, it->nMoney);
    Lua_PushString(L, it->szText);

    if (it->dwItemID)
    {
        KItem* pItem = g_pSO3World->m_ItemSet.GetObj(it->dwItemID);
        KGLOG_PROCESS_ERROR(pItem);

        pItem->LuaGetObj(L);
    }
    else
    {
        Lua_PushNil(L);
    }

    nResult = 6;
Exit0:
    return nResult;
}

int LuaGetMailAttachment(Lua_State* L)
{
    int     nResult     = false;
    int     nRetCode    = 0;
    DWORD   dwMailID    = 0;
    BOOL    bGetMoney   = false;
    BOOL    bGetText    = false;
    BOOL    bGetItem    = false;
    KSO3World::KCLIENT_MAIL_VECTOR::iterator it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);

    dwMailID    = (DWORD)Lua_ValueToNumber(L, 1);
    bGetMoney   = (BOOL)Lua_ValueToBoolean(L, 2);
    bGetText    = (BOOL)Lua_ValueToBoolean(L, 3);
    bGetItem    = (BOOL)Lua_ValueToBoolean(L, 4);

    KGLOG_PROCESS_ERROR(dwMailID);

    it = std::find_if(
        g_pSO3World->m_ClientMailList.begin(), 
        g_pSO3World->m_ClientMailList.end(), 
        IsSameMailID(dwMailID)
    );

    nRetCode = (it != g_pSO3World->m_ClientMailList.end());
    KG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(it->bGotContentFlag);

    if (bGetMoney)
    {
        KGLOG_PROCESS_ERROR(it->nMoney);
    }

    if (bGetText)
    {
        KGLOG_PROCESS_ERROR(it->szText[0] != '\0');
    }

    if (bGetItem)
    {
        KGLOG_PROCESS_ERROR(it->dwItemID);
    }

    nRetCode = g_PlayerClient.DoGetMailAttachment(dwMailID, bGetMoney, bGetText, bGetItem);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    Lua_PushBoolean(L, nResult);
    return 1;
}

int LuaSetMailRead(Lua_State* L)
{
    int     nResult     = false;
    int     nRetCode    = 0;
    DWORD   dwMailID    = 0;
    KSO3World::KCLIENT_MAIL_VECTOR::iterator it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMailID    = (DWORD)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(dwMailID);

    it = std::find_if(
        g_pSO3World->m_ClientMailList.begin(), 
        g_pSO3World->m_ClientMailList.end(), 
        IsSameMailID(dwMailID)
    );

    nRetCode = (it != g_pSO3World->m_ClientMailList.end());
    KG_PROCESS_ERROR(nRetCode);

    it->bUnReadFlag = false;

    nRetCode = g_PlayerClient.DoSetMailRead(dwMailID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    Lua_PushBoolean(L, nResult);
    return 1;
}

int LuaReturnMail(Lua_State* L)
{
    int     nResult     = false;
    int     nRetCode    = 0;
    DWORD   dwMailID    = 0;
    KSO3World::KCLIENT_MAIL_VECTOR::iterator it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMailID    = (DWORD)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(dwMailID);

    it = std::find_if(
        g_pSO3World->m_ClientMailList.begin(), 
        g_pSO3World->m_ClientMailList.end(), 
        IsSameMailID(dwMailID)
    );

    nRetCode = (it != g_pSO3World->m_ClientMailList.end());
    KG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(!it->bSystemFlag && "SystemMail Can't Return.");

    if (it->dwItemID)
    {
        KItem* pItem = g_pSO3World->m_ItemSet.GetObj(it->dwItemID);
        KGLOG_PROCESS_ERROR(pItem);

        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;

        it->dwItemID = ERROR_ID;
    }

    g_pSO3World->m_ClientMailList.erase(it);

    nRetCode = g_PlayerClient.DoReturnMail(dwMailID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    Lua_PushBoolean(L, nResult);
    return 1;
}

#endif

#ifdef _SERVER
int LuaBindNpcMasterID(Lua_State* L)
{
    int     nResult     = false;
    int     nRetCode    = 0;
    DWORD   dwNpcID     = ERROR_ID;
    KNpc*   pNpc        = NULL;
    DWORD   dwMasterID  = ERROR_ID;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwNpcID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwMasterID  = (DWORD)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(dwNpcID     != ERROR_ID);
    KGLOG_PROCESS_ERROR(dwMasterID  != ERROR_ID);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    pNpc->m_dwSkillMasterID = dwMasterID;

    nResult = true;
Exit0:
    Lua_PushBoolean(L, nResult);
    return 1;
}

int LuaGetNpcMasterID(Lua_State* L)
{
    DWORD   dwResult    = ERROR_ID;
    int     nRetCode    = 0;
    DWORD   dwNpcID     = ERROR_ID;
    KNpc*   pNpc        = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwNpcID != ERROR_ID);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    dwResult = pNpc->m_dwSkillMasterID;

Exit0:
    Lua_PushNumber(L, dwResult);
    return 1;
}

int LuaBindRecipeMasterID(Lua_State* L)
{
	BOOL    bResult	    = false;
	int		nTopIndex	= 0;
	DWORD	dwNpcID		= ERROR_ID;
    KNpc*   pNpc        = NULL;
	DWORD	dwMasterID	= ERROR_ID;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2);

	dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);
	dwMasterID = (DWORD)Lua_ValueToNumber(L, 2);

	KGLOG_PROCESS_ERROR(dwNpcID != ERROR_ID);
	KGLOG_PROCESS_ERROR(dwMasterID != ERROR_ID);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

	pNpc->m_dwCraftMasterID = dwMasterID;

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int LuaGetRecipeMasterID(Lua_State* L)
{
	DWORD	dwResult	= ERROR_ID;
	int		nTopIndex	= 0;
	DWORD	dwNpcID		= ERROR_ID;
    KNpc*   pNpc        = NULL;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(dwNpcID != ERROR_ID);
    
    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    dwResult = pNpc->m_dwCraftMasterID;
Exit0:
	Lua_PushNumber(L, dwResult);
	return 1;
}

int LuaBindNpcMailbox(Lua_State* L)
{
    int     nResult     = false;
    int     nRetCode    = 0;
    DWORD   dwNpcID     = ERROR_ID;
    KNpc*   pNpc        = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(dwNpcID != ERROR_ID);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    pNpc->m_bMailbox = true;

    nResult = true;
Exit0:
    Lua_PushBoolean(L, nResult);
    return 1;
}

int LuaIsNpcMailbox(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nRetCode    = 0;
    DWORD   dwNpcID     = ERROR_ID;
    KNpc*   pNpc        = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwNpcID != ERROR_ID);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    bResult = pNpc->m_bMailbox;

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

#endif

#ifdef _CLIENT

int LuaGetSkillLearningInfo(Lua_State* L)
{
	BOOL		    bRetCode    = 0;
	DWORD dwMastrID = 0;
	DWORD dwSkillID = 0;
	DWORD dwSkillLevel = 0;
	KSKILL_LEARNING_INFO* pInfo = NULL;

	assert(L);

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	dwMastrID   = (DWORD)Lua_ValueToNumber(L, 1);
	dwSkillID   = (DWORD)Lua_ValueToNumber(L, 2);
	dwSkillLevel   = (DWORD)Lua_ValueToNumber(L, 3);

	pInfo = g_pSO3World->m_SkillManager.GetSkillLearningInfo(dwMastrID, dwSkillID, dwSkillLevel);
	KG_PROCESS_ERROR(pInfo);

	Lua_PushNumber(L, pInfo->dwRequireExp);
	Lua_PushNumber(L, pInfo->nRequirePlayerLevel);
	Lua_PushNumber(L, pInfo->dwReputationID);
	Lua_PushNumber(L, pInfo->nReputationLevel);
	Lua_PushNumber(L, pInfo->nPrice);

	return 5;
Exit0:
	return 0;
}

int LuaGetMasterSkillList(Lua_State* L)
{
	BOOL		    bRetCode    = 0;
	DWORD		    dwMastrID   = 0;
	size_t		    uArraySize  = 0;
	KSKILL_ID_LEVEL	SkillArray[MAX_SKILLS_FOR_EACH_MASTER];
	int				nCan		= false;
	int			    nHas	    = false;
	int			    nCannot     = false;
	KPlayer*	    pPlayer     = NULL;
	int			    nIndex      = 1;
	int				nNew        = false;
	struct KLEARNSKILLINFO
	{
		DWORD				dwID;
		DWORD				dwLevel;
		std::vector<KSKILL_ID_LEVEL>	aSkill;
	};
	typedef std::vector<KLEARNSKILLINFO> LearnSkillArray;
	LearnSkillArray KungfuArray;

	Lua_NewTable(L);
	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

	dwMastrID   = (DWORD)Lua_ValueToNumber(L, 1);
	nCan		= (int)Lua_ValueToBoolean(L, 2);
    nHas        = (int)Lua_ValueToBoolean(L, 3);
	nCannot     = (int)Lua_ValueToBoolean(L, 4);

    bRetCode = g_pSO3World->m_SkillManager.GetMasterSkillList(dwMastrID, MAX_SKILLS_FOR_EACH_MASTER, &uArraySize, SkillArray);
    KG_PROCESS_ERROR(bRetCode);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

    for (int i = 0; i < (int)uArraySize; i++)
    {
		DWORD dwSkillID = SkillArray[i].dwSkillID;
        DWORD dwSkillLevel = SkillArray[i].dwSkillLevel;
		KSkill* pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, dwSkillLevel);
		if (pSkill)
		{
			if (pSkill->m_pBaseInfo->dwBelongKungfu == 0)
			{
				int nFind = false;
				for (LearnSkillArray::iterator it = KungfuArray.begin(); it != KungfuArray.end(); ++it)
				{
					if (it->dwID == dwSkillID)
					{
						if (it->dwLevel < dwSkillLevel)
							it->dwLevel = dwSkillLevel;
						nFind = true;
						break;
					}
				}
				if (!nFind)
				{
					KLEARNSKILLINFO kf;
					kf.dwID = dwSkillID;
					kf.dwLevel = dwSkillLevel;
					KungfuArray.push_back(kf);
				}
			}
			else
			{
				int nFind = false;
				for (LearnSkillArray::iterator it = KungfuArray.begin(); it != KungfuArray.end(); ++it)
				{
					if (it->dwID == pSkill->m_pBaseInfo->dwBelongKungfu)
					{
						int nFindS = false;
						for (std::vector<KSKILL_ID_LEVEL>::iterator itS = it->aSkill.begin();  itS != it->aSkill.end(); ++itS)
						{
							if (itS->dwSkillID == dwSkillID)
							{
								if (itS->dwSkillLevel < dwSkillLevel)
									itS->dwSkillLevel = dwSkillLevel;
								nFindS = true;
								break;
							}
						}
						if (!nFindS)
						{
							KSKILL_ID_LEVEL sL;
							sL.dwSkillID = dwSkillID;
							sL.dwSkillLevel = dwSkillLevel;
							it->aSkill.push_back(sL);
						}
						nFind = true;
						break;
					}
				}
				if (!nFind)
				{
					KLEARNSKILLINFO kf;
					kf.dwID = pSkill->m_pBaseInfo->dwBelongKungfu;
					kf.dwLevel = 1;
					KSKILL_ID_LEVEL sL;
					sL.dwSkillID = dwSkillID;
					sL.dwSkillLevel = dwSkillLevel;
					kf.aSkill.push_back(sL);
					KungfuArray.push_back(kf);
				}
			}
		}
    }

	for (LearnSkillArray::iterator it = KungfuArray.begin(); it != KungfuArray.end(); ++it)
	{
		int nKunfuLevel = pPlayer->m_SkillList.GetSkillLevel(it->dwID);
		int nKunfuHas = nKunfuLevel != 0;
		int nKunfuCan = pPlayer->m_SkillList.CanLearnSkill(it->dwID, dwMastrID);

		if (nKunfuLevel < (int)(it->dwLevel))
		{
			nKunfuHas = false;
			nKunfuLevel++;
		}

		int nShowGroup = false;
		for (std::vector<KSKILL_ID_LEVEL>::iterator itS = it->aSkill.begin();  itS != it->aSkill.end(); ++itS)
		{
			int nSkillLevel = pPlayer->m_SkillList.GetSkillLevel(itS->dwSkillID);
			int nSkillHas = nSkillLevel != 0;
			if (nSkillLevel < (int)(itS->dwSkillLevel))
			{
				nSkillHas = false;
				nSkillLevel++;
			}
			int nSkillCan = pPlayer->m_SkillList.CanLearnSkill(itS->dwSkillID, dwMastrID);
			if ((nCan && nSkillCan) || (nHas && nSkillHas) || (nCannot && !nSkillCan && !nSkillHas))
			{
				nShowGroup = true;
				break;
			}
		}
		if (!nShowGroup)
		{
			if ((nCan && nKunfuCan) || (nHas && nKunfuHas) || (nCannot && !nKunfuCan && !nKunfuHas))
				nShowGroup = true;
		}

		if (nShowGroup)
		{
			Lua_PushNumber(L, nIndex);
			Lua_NewTable(L);

			Lua_PushString(L, "dwID");
			Lua_PushNumber(L, it->dwID);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "dwLevel");
			Lua_PushNumber(L, nKunfuLevel);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "bHas");
			Lua_PushBoolean(L, nKunfuHas);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "bCannot");
			Lua_PushBoolean(L, !nKunfuCan);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "nRLevel");
			KSKILL_LEARNING_INFO* pInfo = g_pSO3World->m_SkillManager.GetSkillLearningInfo(dwMastrID, it->dwID, nKunfuLevel ? nKunfuLevel : 1);
			int nRLevel = 0;
			if(pInfo)
				nRLevel = pInfo->nRequirePlayerLevel;
			Lua_PushNumber(L, nRLevel);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "aSkill");
			Lua_NewTable(L);
			int nSkillIndex = 1;
			for (std::vector<KSKILL_ID_LEVEL>::iterator itS = it->aSkill.begin();  itS != it->aSkill.end(); ++itS)
			{
				int nSkillLevel = pPlayer->m_SkillList.GetSkillLevel(itS->dwSkillID);
				int nSkillHas = nSkillLevel != 0;
				if (nSkillLevel < (int)(itS->dwSkillLevel))
				{
					nSkillHas = false;
					nSkillLevel++;
				}
				int nSkillCan = pPlayer->m_SkillList.CanLearnSkill(itS->dwSkillID, dwMastrID);
				if (!((nCan && nSkillCan) || (nHas && nSkillHas) || (nCannot && !nSkillCan && !nSkillHas)))
					continue;

				Lua_PushNumber(L, nSkillIndex);
				Lua_NewTable(L);

				Lua_PushString(L, "dwID");
				Lua_PushNumber(L, itS->dwSkillID);
				Lua_SetTable(L, -3);

				Lua_PushString(L, "dwLevel");
				Lua_PushNumber(L, nSkillLevel);
				Lua_SetTable(L, -3);

				Lua_PushString(L, "bHas");
				Lua_PushBoolean(L, nSkillHas);
				Lua_SetTable(L, -3);

				Lua_PushString(L, "bCannot");
				Lua_PushBoolean(L, !nSkillCan);
				Lua_SetTable(L, -3);

				Lua_PushString(L, "nRLevel");
				KSKILL_LEARNING_INFO* pInfo = g_pSO3World->m_SkillManager.GetSkillLearningInfo(dwMastrID, itS->dwSkillID, nSkillLevel ? nSkillLevel : 1);
				int nRLevel = 0;
				if(pInfo)
					nRLevel = pInfo->nRequirePlayerLevel;
				Lua_PushNumber(L, nRLevel);
				Lua_SetTable(L, -3);

				Lua_SetTable(L, -3);
				nSkillIndex++;
			}
			Lua_SetTable(L, -3);

			Lua_SetTable(L, -3);
			nIndex++;
		}
	}

Exit0:
	return 1;
}

int LuaGetRecipeLearnInfo(Lua_State* L)
{
	BOOL	bRetCode	= false;
	int		nTopIndex	= 0;
	DWORD	dwMasterID	= 0;
	DWORD	dwCraftID	= 0;
	DWORD	dwRecipeID	= 0;
	KRecipeMaster*			pRecipeMaster		= NULL;
	KRecipeLearningInfo*	pRecipeLearnInfo	= NULL;

	nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 3);

	dwMasterID	= (DWORD)Lua_ValueToNumber(L, 1);
	dwCraftID	= (DWORD)Lua_ValueToNumber(L, 2);
	dwRecipeID	= (DWORD)Lua_ValueToNumber(L, 3);

	pRecipeMaster = g_pSO3World->m_ProfessionManager.GetRecipeMaster(dwMasterID);
	KGLOG_PROCESS_ERROR(pRecipeMaster);

	pRecipeLearnInfo = pRecipeMaster->GetLearnInfo(dwCraftID, dwRecipeID);
	KGLOG_PROCESS_ERROR(pRecipeLearnInfo);

	Lua_NewTable(L);
	nTopIndex = Lua_GetTopIndex(L);

	Lua_PushString(L, "dwProfessionID");
	Lua_PushNumber(L, pRecipeLearnInfo->dwProfessionID);
	Lua_SetTable(L, nTopIndex);

	Lua_PushString(L, "nProfessionLevel");
	Lua_PushNumber(L, pRecipeLearnInfo->nProfessionLevel);
	Lua_SetTable(L, nTopIndex);

	Lua_PushString(L, "dwBranchID");
	Lua_PushNumber(L, pRecipeLearnInfo->dwBranchID);
	Lua_SetTable(L, nTopIndex);

	Lua_PushString(L, "dwReputationID");
	Lua_PushNumber(L, pRecipeLearnInfo->dwReputationID);
	Lua_SetTable(L, nTopIndex);

	Lua_PushString(L, "nReputationLevel");
	Lua_PushNumber(L, pRecipeLearnInfo->nReputationLevel);
	Lua_SetTable(L, nTopIndex);

	Lua_PushString(L, "nPrice");
	Lua_PushNumber(L, pRecipeLearnInfo->nPrice);
	Lua_SetTable(L, nTopIndex);

	return 1;
Exit0:
	return 0;
}

int LuaGetMasterRecipeList(Lua_State* L)
{
	BOOL		bRetCode	= false;
	int			nTopIndex	= 0;
	int			nTableIndex = 0;
	DWORD		dwMasterID	= 0;
	BOOL		bCan		= false;
	BOOL		bHas		= false;
	BOOL		bCannot		= false;
	BOOL		bPlayerHas	= false;
	BOOL		bPlayerCanLearn = false;
	DWORD		dwCraftID	= 0;
	DWORD		dwRecipeID	= 0;
	int			nIndex		= 0;
	int			nSize		= 0;
	int			nRecipeIndex = 0;
	KPlayer*	pPlayer		= NULL;
	KRecipeMaster* pRecipeMaster		= NULL;
	std::vector<KRecipeLearningInfo> vecLearnInfoList;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 4);

	dwMasterID	= (DWORD)Lua_ValueToNumber(L, 1);
	bCan		= (BOOL)Lua_ValueToBoolean(L, 2);
	bHas		= (BOOL)Lua_ValueToBoolean(L, 3);
	bCannot		= (BOOL)Lua_ValueToBoolean(L, 4);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	pRecipeMaster = g_pSO3World->m_ProfessionManager.GetRecipeMaster(dwMasterID);
	KGLOG_PROCESS_ERROR(pRecipeMaster);

	pRecipeMaster->GetLearnInfoList(vecLearnInfoList);
	nSize = (int)vecLearnInfoList.size();
	KG_PROCESS_ERROR(nSize > 0);

	Lua_NewTable(L);
	nTopIndex = Lua_GetTopIndex(L);

	for (nIndex = 0; nIndex < nSize; nIndex++)
	{
		dwCraftID = vecLearnInfoList[nIndex].dwCraftID;
		dwRecipeID = vecLearnInfoList[nIndex].dwRecipeID;
	
		bPlayerHas = pPlayer->m_RecipeList.IsRecipeLearned(dwCraftID, dwRecipeID);
		bPlayerCanLearn = pPlayer->m_RecipeList.CanLearnRecipe(dwCraftID, dwRecipeID, &vecLearnInfoList[nIndex]);

		if (!((bCan && bPlayerCanLearn) || (bHas && bPlayerHas) || (bCannot && !bPlayerCanLearn && !bPlayerHas)))
			continue;

		nRecipeIndex++;
		Lua_PushNumber(L, nRecipeIndex);

		Lua_NewTable(L);
		nTableIndex = Lua_GetTopIndex(L);

		Lua_PushString(L, "dwCraftID");
		Lua_PushNumber(L, dwCraftID);
		Lua_SetTable(L, nTableIndex);

		Lua_PushString(L, "dwRecipeID");
		Lua_PushNumber(L, dwRecipeID);
		Lua_SetTable(L, nTableIndex);

		Lua_PushString(L, "bHas");
		Lua_PushBoolean(L, bPlayerHas);
		Lua_SetTable(L, nTableIndex);

		Lua_PushString(L, "bCannot");
		Lua_PushBoolean(L, !bPlayerCanLearn);
		Lua_SetTable(L, nTableIndex);

		Lua_SetTable(L, nTopIndex);
	}
	
	return 1;
Exit0:
	return 0;
}
#endif


#ifdef _SERVER
int LuaSendGlobalSysMsg(Lua_State* L)
{
	int         nTopIndex   = 0;
    const char* pszMsg      = NULL;
    size_t      uDataLen    = 0;
    IKG_Buffer* piBuffer    = NULL;
    BYTE*       pbyBuffer   = NULL;

    nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszMsg = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszMsg);

    uDataLen = 1 + strlen(pszMsg) + 1;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

    *pbyBuffer = tdbText;
    strcpy((char*)pbyBuffer + 1, pszMsg);

    g_RelayClient.DoTalkMessage(trGlobalSys, ERROR_ID, NULL, ERROR_ID, NULL, uDataLen, pbyBuffer);

Exit0:
    KG_COM_RELEASE(piBuffer);
    return 0;
}
#endif

#ifdef _CLIENT
int LuaPeekOtherPlayerEquip(Lua_State* L)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nTopIndex       = 0;
    DWORD       dwPlayerID      = ERROR_ID;
    KPlayer*    pTarget         = NULL;
    KPlayer*    pClientPlayer   = NULL;
    int         nBoxSize        = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(dwPlayerID);
    
    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pClientPlayer);

    KGLOG_PROCESS_ERROR(IS_PLAYER(dwPlayerID));
    KGLOG_PROCESS_ERROR(dwPlayerID != g_pSO3World->m_dwClientPlayerID);

    pTarget = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pTarget);

    bRetCode = pClientPlayer->CanDialog(pTarget);
    KGLOG_PROCESS_ERROR(bRetCode);

    nBoxSize = pTarget->m_ItemList.GetBoxSize(ibEquip);
    for (int i = 0; i < nBoxSize; i++)
    {
        TItemPos Pos = {ibEquip, i};
        pTarget->m_ItemList.DestroyItem(Pos);
    }
    
    g_PlayerClient.DoPeekOtherPlayerEquip(dwPlayerID);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaPeekOtherPlayerBook(Lua_State* L)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nTopIndex       = 0;
    DWORD       dwPlayerID      = ERROR_ID;
    KPlayer*    pTarget         = NULL;
    KPlayer*    pClientPlayer   = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(dwPlayerID);

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pClientPlayer);

    KGLOG_PROCESS_ERROR(IS_PLAYER(dwPlayerID));
    KGLOG_PROCESS_ERROR(dwPlayerID != g_pSO3World->m_dwClientPlayerID);

    pTarget = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pTarget);

    bRetCode = pClientPlayer->CanDialog(pTarget);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pTarget->m_BookList.Clear();

    g_PlayerClient.DoPeekOtherPlayerBook(dwPlayerID);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaPeekOtherPlayerQuest(Lua_State* L)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nTopIndex       = 0;
    DWORD       dwPlayerID      = ERROR_ID;
    KPlayer*    pTarget         = NULL;
    KPlayer*    pClientPlayer   = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(dwPlayerID);

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pClientPlayer);

    KGLOG_PROCESS_ERROR(IS_PLAYER(dwPlayerID));
    KGLOG_PROCESS_ERROR(dwPlayerID != g_pSO3World->m_dwClientPlayerID);

    pTarget = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pTarget);

    bRetCode = pClientPlayer->CanDialog(pTarget);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pTarget->m_QuestList.ClearQuestList();

    g_PlayerClient.DoPeekOtherPlayerQuest(dwPlayerID);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaLogoutGame(Lua_State* L)
{
	g_PlayerClient.DoPlayerLogout();
	return 0;
}
#endif

#ifdef _SERVER
int LuaSetRouteNodeForce(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    int             nRouteNode  = 0;
    DWORD           dwForce     = 0;
    KROUTE_NODE*    pNode       = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nRouteNode  = (int)Lua_ValueToNumber(L, 1);
    dwForce     = (DWORD)Lua_ValueToNumber(L, 2);

    pNode       = g_pSO3World->m_RoadManager.GetNodeInfo(nRouteNode);
    KGLOG_PROCESS_ERROR(pNode);

    KGLOG_PROCESS_ERROR(dwForce >= 0 && dwForce < MAX_FORCE_COUNT);

    g_RelayClient.DoSyncRoadTrackForce(nRouteNode, dwForce);

    nResult = 0;
Exit0:
    return nResult;
}
#endif

#ifdef _CLIENT
int LuaCheckRoadTrack(Lua_State* L)
{
	BOOL            bResult     = false;
	BOOL            bRetCode    = false;
	BOOL            bForceRet   = false;
	int             nForceLevel = 0;
	int             nTopIndex   = 0;
	int             nFromNodeID = 0;
	int             nToNodeID   = 0;
	KROUTE_NODE*    pNode       = NULL;
	KPlayer*        pClient     = NULL;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2);

	nFromNodeID = (int)Lua_ValueToNumber(L, 1);
	nToNodeID   = (int)Lua_ValueToNumber(L, 2);

	pClient = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pClient);

	bRetCode = g_pSO3World->m_RoadManager.IsOpenRouteNode(nFromNodeID, &(pClient->m_OpenRouteNodeList));
	KG_PROCESS_ERROR(bRetCode);

	bRetCode = g_pSO3World->m_RoadManager.IsOpenRouteNode(nToNodeID, &(pClient->m_OpenRouteNodeList));
	KG_PROCESS_ERROR(bRetCode);

	pNode = g_pSO3World->m_RoadManager.GetNodeInfo(nFromNodeID);
	KGLOG_PROCESS_ERROR(pNode);

	if (!pNode->bEnable)
	{
		goto Exit1;
	}


	if (pNode->eCamp != cNeutral && pNode->eCamp != pClient->m_eCamp)
	{
		goto Exit1;
	}

	pNode = g_pSO3World->m_RoadManager.GetNodeInfo(nToNodeID);
	KGLOG_PROCESS_ERROR(pNode);

	if (!pNode->bEnable)
	{
		goto Exit1;
	}

	if (pNode->eCamp != cNeutral && pNode->eCamp != pClient->m_eCamp)
	{
		goto Exit1;
	}

	bForceRet = true;
Exit1:
	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	Lua_PushBoolean(L, bForceRet);

	return 2;
}

int LuaCalculateTrackCost(Lua_State* L)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    int         nFromNodeID = 0;
    int         nTargetCity = 0;
    int         nCost       = 0;
    KPlayer*    pClient     = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nFromNodeID = (int)Lua_ValueToNumber(L, 1);
    nTargetCity = (int)Lua_ValueToNumber(L, 2);

    pClient = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClient);

    bRetCode = g_pSO3World->m_RoadManager.CalcCostMoney(&nCost, nFromNodeID, nTargetCity, pClient);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);    // 是否有结果
    Lua_PushNumber(L, nCost);       // 结果是什么
    return 2;
}

int LuaRoadTrackStartOut(Lua_State* L)
{
    BOOL            bResult     = false;
    int             nTopIndex   = 0;
    int             nFromNodeID = 0;
    int             nTargetCity = 0;
    int             nCost       = 0;
    KROUTE_NODE*    pNode       = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    nFromNodeID = (int)Lua_ValueToNumber(L, 1);
    nTargetCity = (int)Lua_ValueToNumber(L, 2);
    nCost       = (int)Lua_ValueToNumber(L, 3);

    pNode = g_pSO3World->m_RoadManager.GetNodeInfo(nFromNodeID);
    KGLOG_PROCESS_ERROR(pNode);

    KG_PROCESS_SUCCESS(pNode->dwCityID == (DWORD)nTargetCity);

    g_PlayerClient.DoRoadTrackStartOut(nFromNodeID, nTargetCity, nCost);

Exit1:
    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}

int LuaRoadTrackForceRequest(Lua_State* L)
{
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    g_PlayerClient.DoRoadTrackForceRequest();

Exit0:
    return 0;
}

int LuaRoadTrackIsCityOpen(Lua_State* L)
{
    int                                 nResult     = false;
    int                                 nTopIndex   = 0;
    int                                 nCityID     = 0;
    KPlayer*                            pClient     = NULL;
    KROUTE_NODE*                        pNode       = NULL;
    KOPEN_ROUTE_NODE_VECTOER::iterator  it;
    KOPEN_ROUTE_NODE_VECTOER::iterator  itEnd;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nCityID = (int)Lua_ValueToNumber(L, 1);

    pClient = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClient);

    itEnd = pClient->m_OpenRouteNodeList.end();
    for (it = pClient->m_OpenRouteNodeList.begin(); it != itEnd; it++)
    {
        pNode = g_pSO3World->m_RoadManager.GetNodeInfo(*it);
        KGLOG_PROCESS_ERROR(pNode);

        KG_PROCESS_SUCCESS(pNode->dwCityID == (DWORD)nCityID);
    }
    goto Exit0;

Exit1:
    nResult = true;
Exit0:
    Lua_PushBoolean(L, nResult);

    return 1;
}

int LuaGetNearbyPlayerList(Lua_State* L)
{
    int         nResult     = 0;
    int         nTopIndex   = 0;
    int         nTable      = 0;
    KPlayer*    pPlayer     = NULL;

    struct KGetPlayerListFunc
    {
        std::vector<DWORD> m_vPlayerTable;

	    BOOL operator()(KPlayer* pPlayer)
        {
            m_vPlayerTable.push_back(pPlayer->m_dwID);
            return true;
        };

    } GetPlayerFunc;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KG_PROCESS_ERROR(pPlayer);
    
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    pPlayer->m_pScene->TraverseRangePlayer(pPlayer->m_pRegion, GetPlayerFunc);
    
    for (int i = 0; i < (int)GetPlayerFunc.m_vPlayerTable.size(); i++)
    {
        int nPushID = (int)GetPlayerFunc.m_vPlayerTable[i];

        Lua_PushNumber(L, i + 1);
	    Lua_PushNumber(L, nPushID);
	    Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetNearbyNpcList(Lua_State* L)
{
    int         nResult     = 0;
    int         nTopIndex   = 0;
    int         nTable      = 0;
    KPlayer*    pPlayer     = NULL;

    struct KGetNpcFunc
    {
        std::vector<DWORD> m_vNpcTable;
	    
        BOOL operator()(KNpc* pNpc)
        {
            m_vNpcTable.push_back(pNpc->m_dwID);
            return true;
        };

    } GetNpcFunc;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    pPlayer->m_pScene->TraverseRangeNpc(pPlayer->m_pRegion, GetNpcFunc);

    for (int i = 0; i < (int)GetNpcFunc.m_vNpcTable.size(); i++)
    {
        int nPushID = (int)GetNpcFunc.m_vNpcTable[i];

        Lua_PushNumber(L, i + 1);
	    Lua_PushNumber(L, nPushID);
	    Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetNearbyDoodadList(Lua_State* L)
{
    int         nResult     = 0;
    int         nTopIndex   = 0;
    int         nTable      = 0;
    KPlayer*    pPlayer     = NULL;

    struct KGetDoodadFunc
    {
        std::vector<DWORD> m_vDoodadTable;

	    BOOL operator()(KDoodad* pDoodad)
        {
            m_vDoodadTable.push_back(pDoodad->m_dwID);
            return true;
        };

    } GetDoodadFunc;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    pPlayer->m_pScene->TraverseRangeDoodad(pPlayer->m_pRegion, GetDoodadFunc);

    for (int i = 0; i < (int)GetDoodadFunc.m_vDoodadTable.size(); i++)
    {
        int nPushID = (int)GetDoodadFunc.m_vDoodadTable[i];

        Lua_PushNumber(L, i + 1);
	    Lua_PushNumber(L, nPushID);
	    Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}
#endif

int LuaReloadQuestTable(Lua_State* L)
{
	int nTopIndex = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 0);

	g_pSO3World->m_Settings.m_QuestInfoList.Init();

Exit0:
	return 0;
}

int LuaIsTempEnchantAttribute(Lua_State* L)
{
    BOOL        bRetCode       = false;
    int         nTopIndex      = Lua_GetTopIndex(L);
    int         nEnchantID     = 0;
    KENCHANT*   pEnchant       = NULL;
    BOOL        bIsTempEnchant = false;

    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    
    nEnchantID = (int)Lua_ValueToNumber(L, 1);

    pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(nEnchantID);
    KGLOG_PROCESS_ERROR(pEnchant);
    
    if (pEnchant->nTime != 0)
    {
        bIsTempEnchant = true;
    }
    
    Lua_PushBoolean(L, bIsTempEnchant);
    return 1;
Exit0:
    return 0;    
}

int LuaGetEnchantAttribute(Lua_State* L)
{
    DWORD       dwEnchantID     = 0;
    KENCHANT*   pEnchant        = NULL;
    int         nIndex          = 0;
    int         nTopIndex       = Lua_GetTopIndex(L);

    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    
    dwEnchantID = (DWORD)Lua_ValueToNumber(L, 1);

    pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(dwEnchantID);
    KGLOG_PROCESS_ERROR(pEnchant);
    
    KG_PROCESS_ERROR(pEnchant->pAttribute);
    
    Lua_NewTable(L);

    for (KAttribute* pAttribute = pEnchant->pAttribute; pAttribute; pAttribute = pAttribute->pNext)
    {
        if (pAttribute->nKey != atInvalid)
        {
            nIndex++;            
			Lua_PushNumber(L, nIndex);
            Lua_NewTable(L);

            Lua_PushString(L, "nID");
            Lua_PushNumber(L, pAttribute->nKey);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nValue1");
            Lua_PushNumber(L, pAttribute->nValue1);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nValue2");
            Lua_PushNumber(L, pAttribute->nValue2);
            Lua_SetTable(L, -3);

            Lua_SetTable(L, -3);
        }
    }

    Lua_PushNumber(L, pEnchant->nTime);
    Lua_PushNumber(L, pEnchant->nDestItemSubType);

    return 3;
Exit0:
    return 0;
}

int LuaBookID2GlobelRecipeID(Lua_State* L)
{
    int   nTopIndex      = Lua_GetTopIndex(L);
    DWORD dwRecipeID = 0;
    DWORD dwBookID = 0;
    DWORD dwSubID = 0;

    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    dwBookID = (DWORD)Lua_ValueToNumber(L, 1);
    dwSubID = (DWORD)Lua_ValueToNumber(L, 2);

    dwRecipeID = BookID2RecipeID(dwBookID, dwSubID);

    Lua_PushNumber(L, dwRecipeID);

    return 1;
Exit0:
    return 0;
}

int LuaGlobelRecipeID2BookID(Lua_State* L)
{
    BOOL  bRetCode       = false;
    int   nTopIndex      = Lua_GetTopIndex(L);
    DWORD dwRecipeID = 0;
    DWORD dwBookID = 0;
    DWORD dwSubID = 0;

    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    dwRecipeID = (DWORD)Lua_ValueToNumber(L, 1);

    bRetCode = RecipeID2BookID(dwRecipeID, dwBookID, dwSubID);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushNumber(L, dwBookID);
    Lua_PushNumber(L, dwSubID);

    return 2;
Exit0:
    return 0;    
}

int LuaGetCoolDownFrame(Lua_State* L)
{
    int     nFrame          = 0;
    int     nTopIndex       = Lua_GetTopIndex(L);
    DWORD   dwCoolDownID    = 0;

    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    dwCoolDownID = (DWORD)Lua_ValueToNumber(L, 1);

    nFrame = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(dwCoolDownID);
    KGLOG_PROCESS_ERROR(nFrame > 0);
    
    Lua_PushNumber(L, nFrame);

    return 1;
Exit0:
    return 0;
}

int LuaGetSkillRecipeBaseInfo(Lua_State* L)
{
    int                         nTopIndex               = Lua_GetTopIndex(L);
    DWORD                       dwRecipeID              = 0;
    DWORD                       dwRecipeLevel           = 0;
    DWORD                       dwRecipeKey             = 0;
    KSKILL_RECIPE_BASE_INFO*    pSkillRecipeBaseInfo    = NULL;

    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    dwRecipeID = (DWORD)Lua_ValueToNumber(L, 1);
    dwRecipeLevel = (DWORD)Lua_ValueToNumber(L, 2);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    pSkillRecipeBaseInfo = g_pSO3World->m_SkillManager.GetSkillRecipeBaseInfo(dwRecipeKey);
    KGLOG_PROCESS_ERROR(pSkillRecipeBaseInfo);

    Lua_PushNumber(L, pSkillRecipeBaseInfo->dwSkillID);
    Lua_PushNumber(L, pSkillRecipeBaseInfo->dwSkillLevel);
    Lua_PushNumber(L, pSkillRecipeBaseInfo->bIsSystemRecipe);
    Lua_PushNumber(L, pSkillRecipeBaseInfo->nSkillRecipeType);

    return 4;
Exit0:
    return 0;
}

int LuaSendGmMessage(Lua_State* L)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = Lua_GetTopIndex(L);
    const char* pszGmName   = NULL;
    const char* pszMessage  = NULL;

#ifdef _SERVER
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    pszGmName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszGmName);

    pszMessage = Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszMessage);

    bRetCode = g_RelayClient.DoSendGmMessage(pszGmName, pszMessage);
    KGLOG_PROCESS_ERROR(bRetCode);
#else  // _CLIENT
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszMessage = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszMessage);

    bRetCode = g_PlayerClient.DoSendGmMessage(pszMessage);
    KGLOG_PROCESS_ERROR(bRetCode);
#endif

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

#ifdef _SERVER
int LuaGSCommand(Lua_State* L)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    const char* pszRoleName = NULL;
    const char* pszCommand  = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    pszRoleName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszRoleName);

    pszCommand  = Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszCommand);

    bRetCode = g_RelayClient.DoSendGMCommandGS(pszRoleName, pszCommand);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaGCCommand(Lua_State* L)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    const char* pszCommand  = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszCommand  = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszCommand);

    bRetCode = g_RelayClient.DoSendGMCommandGC(pszCommand);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

class KPutNpcToRelive  
{
public:
    BOOL operator() (DWORD dwID, KNpc* pNpc)
    {
        BOOL    bResult     = false;
        BOOL    bRetCode    = false;
        int     nAIType     = 0;
        KScene* pScene      = NULL;

        KGLOG_PROCESS_ERROR(dwID);
        KGLOG_PROCESS_ERROR(pNpc);
        
        nAIType = pNpc->m_AIVM.GetAIType();
        KG_PROCESS_SUCCESS(nAIType != m_nAIType);

        pScene = pNpc->m_pScene;
        KGLOG_PROCESS_ERROR(pScene);

        pNpc->Stop();

        g_pSO3World->RemoveNpc(pNpc);

        bRetCode = pScene->AddReviveNpc(pNpc, 1);
        if (!bRetCode)
        {
            g_pSO3World->DeleteNpc(pNpc);
            goto Exit0;
        }

    Exit1:
        bResult = true;
    Exit0:
        return bResult;
    }

    int m_nAIType;
};

int LuaReloadAI(Lua_State* L)
{
    BOOL            bRetCode    = false;
    int             nTopIndex   = 0;
    int             nAIType     = 0;
    KPutNpcToRelive PutNpcToRelive;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAIType  = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nAIType);

    bRetCode = g_pSO3World->m_AIManager.ReloadAILogic(nAIType);
    KGLOG_PROCESS_ERROR(bRetCode);

    PutNpcToRelive.m_nAIType = nAIType;
    g_pSO3World->m_NpcSet.Traverse(PutNpcToRelive);

Exit0:
    return 0;
}

#endif

#ifdef _CLIENT
int LuaSetPartyMark(Lua_State* L)
{
    return g_pSO3World->m_TeamClient.LuaSetTeamMark(L);
}
#endif

int LuaLog(Lua_State* L)
{
    int         nTopIndex  = 0;
    const char* pszLogText = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszLogText = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszLogText);

    KGLogPrintf(KGLOG_DEBUG, "%s\n", pszLogText);

Exit0:
    return 0;
}

#ifdef _SERVER
int LuaIsEscortQuest(Lua_State* L)
{
    BOOL        bResult    = false;
    int         nTopIndex  = 0;
    DWORD       dwQuestID  = 0;
    KQuestInfo* pQuestInfo = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
    
    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
    KGLOG_PROCESS_ERROR(pQuestInfo);
    KG_PROCESS_ERROR(pQuestInfo->bEscortQuest);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}
#endif

#ifdef _CLIENT
int LuaGetClientTeam(Lua_State* L)
{
    int nResult = g_pSO3World->m_TeamClient.LuaGetObj(L);

    return nResult;
}

int LuaJoinBattleFieldQueue(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nTopIndex       = 0;
    int     nGroupID        = 0;
    DWORD   dwNpcID         = 0;
    DWORD   dwMapID         = 0;
    BOOL    bTeamJoin       = false;
    int     nCopyIndex      = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 5);

    dwNpcID         = (DWORD)Lua_ValueToNumber(L, 1);
    dwMapID         = (DWORD)Lua_ValueToNumber(L, 2);
    nGroupID        = (int)Lua_ValueToNumber(L, 3);
    nCopyIndex      = (int)Lua_ValueToNumber(L, 4);
    bTeamJoin       = (BOOL)Lua_ValueToBoolean(L, 5);

    g_PlayerClient.DoJoinBattleFieldQueueRequest(dwNpcID, dwMapID, nCopyIndex, nGroupID, bTeamJoin);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaLeaveBattleFieldQueue(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nTopIndex   = 0;
    DWORD   dwMapID     = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMapID = (DWORD)Lua_ValueToNumber(L, 1);

    g_PlayerClient.DoLeaveBattleFieldQueueRequest(dwMapID);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaAcceptJoinBattleField(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nTopIndex       = 0;
    DWORD   dwMapID         = 0;
    int     nCopyIndex      = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMapID         = (DWORD)Lua_ValueToNumber(L, 1);
    nCopyIndex      = (int)Lua_ValueToNumber(L, 2);

    g_PlayerClient.DoAcceptJoinBattleField(dwMapID, nCopyIndex);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaGetBattleFieldList(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nTopIndex   = 0;
    DWORD   dwMapID     = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMapID = (DWORD)Lua_ValueToNumber(L, 1);

    g_PlayerClient.DoGetBattleFieldList(dwMapID);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaLeaveBattleField(Lua_State* L)
{
    BOOL bResult    = false;
    int  nTopIndex  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    g_PlayerClient.DoLeaveBattleField();

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}
#endif

int LuaIsNpcExist(Lua_State* L)
{
    BOOL    bIsExist  = false;
    DWORD   dwNpcID   = ERROR_ID;
    KNpc*   pNpc      = NULL;
    int     nTopIndex = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);

    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KG_PROCESS_ERROR(pNpc);

    bIsExist = true;
Exit0:
    Lua_PushBoolean(L, bIsExist);
    return 1;
}

int LuaIsDoodadExist(Lua_State* L)
{
    BOOL        bIsExist    = false;
    DWORD       dwDoodadID  = ERROR_ID;
    KDoodad*    pDoodad     = NULL;
    int         nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwDoodadID = (DWORD)Lua_ValueToNumber(L, 1);

    pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
    KG_PROCESS_ERROR(pDoodad);

    bIsExist = true;
Exit0:
    Lua_PushBoolean(L, bIsExist);
    return 1;
}

int LuaIsPlayerExist(Lua_State* L)
{
    BOOL        bIsExist    = false;
    DWORD       dwPlayerID  = ERROR_ID;
    KPlayer*    pPlayer     = NULL;
    int         nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);

    bIsExist = true;
Exit0:
    Lua_PushBoolean(L, bIsExist);
    return 1;
}

#ifdef _SERVER
int LuaAddCampScore(Lua_State* L)
{
    int         nTopIndex       = 0;
    int         nCamp           = 0;
    int         nAddCampScore   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == (int)cGood || nCamp == (int)cEvil);

    nAddCampScore = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nAddCampScore > 0);
    
    if (nCamp == cEvil) // 正方加分，反方减分
    {
        nAddCampScore = -nAddCampScore;
    }

    g_RelayClient.DoAddCampScore(nAddCampScore);

Exit0:
    return 0;
}
#endif

int LuaGetCampInfo(Lua_State* L)
{
    int         nResult         = 0;
    int         nTopIndex       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);
    
    nResult = g_pSO3World->m_CampInfo.LuaGetObj(L);
Exit0:
    return nResult;
}

#ifdef _SERVER
int LuaBattleFieldEnd(Lua_State* L)
{
    int     nTopIndex       = 0;
    DWORD   dwMapID         = 0;
    int     nCopyIndex      = 0;
    KScene* pScene          = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMapID     = (DWORD)Lua_ValueToNumber(L, 1);
    nCopyIndex  = (int)Lua_ValueToNumber(L, 2);

    g_PlayerServer.DoBattleFieldEnd(dwMapID, nCopyIndex);

Exit0:
    return 0;
}

int LuaCreatePQ(Lua_State* L)
{
    DWORD   dwPQTemplateID  = ERROR_ID;
    int     nTopIndex       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQTemplateID = (DWORD)Lua_ValueToNumber(L, 1);

    g_RelayClient.DoApplyCreatePQ(dwPQTemplateID);

Exit0:
    return 0;
}

int LuaDelPQ(Lua_State* L)
{
    DWORD   dwPQID      = ERROR_ID;
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQID = (DWORD)Lua_ValueToNumber(L, 1);

    g_RelayClient.DoApplyDeletePQ(dwPQID);

Exit0:
    return 0;
}

int LuaChangePQValue(Lua_State* L)
{
    DWORD   dwPQID      = ERROR_ID;
    int     nIndex      = 0;
    int     nAdd        = 0;
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwPQID = (DWORD)Lua_ValueToNumber(L, 1);
    nIndex = (int)Lua_ValueToNumber(L, 2);
    nAdd   = (int)Lua_ValueToNumber(L, 3);

    g_RelayClient.DoApplyChangePQValue(dwPQID, nIndex, nAdd);

Exit0:
    return 0;
}

int LuaGetPQ(Lua_State* L)
{
    int     nResult     = 0;
    DWORD   dwPQID      = ERROR_ID;
    KGPQ*   pPQ         = NULL;
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQID = (DWORD)Lua_ValueToNumber(L, 1);

    pPQ = g_pSO3World->m_PQManager.GetPQ(dwPQID);
    KG_PROCESS_ERROR(pPQ);

    nResult = pPQ->LuaGetObj(L);
Exit0:
    return nResult;
}

struct FIND_PQID_BY_TEMPLATE  
{
    BOOL operator () (const KGPQ& crPQ)
    {
        if (crPQ.m_pPQTemplate->dwTemplateID != dwTemplateID)
            return true;

        PQIDVector.push_back(crPQ.m_dwPQID);

        return true;
    }

    DWORD               dwTemplateID;
    std::vector<DWORD>  PQIDVector;
};

int LuaGetPQByTemplate(Lua_State* L)
{
    int     nResult         = 0;
    BOOL    bRetCode        = 0;
    DWORD   dwPQTemplateID  = ERROR_ID;
    int     nTopIndex       = 0;
    int     nIndex          = 0;
    FIND_PQID_BY_TEMPLATE   FindPQ;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQTemplateID = (DWORD)Lua_ValueToNumber(L, 1);

    FindPQ.dwTemplateID = dwPQTemplateID;
    FindPQ.PQIDVector.clear();

    bRetCode = g_pSO3World->m_PQManager.Traverse(FindPQ);
    KG_PROCESS_ERROR(bRetCode);

    Lua_NewTable(L);

    for (
        std::vector<DWORD>::iterator it = FindPQ.PQIDVector.begin(), itEnd = FindPQ.PQIDVector.end(); 
        it != itEnd; 
        ++it
    )
    {
        Lua_PushNumber(L, ++nIndex);
        Lua_PushNumber(L, *it);

        Lua_SetTable(L, -3);
    }

Exit0:
    return 1;
}
#endif

#ifdef _CLIENT
int LuaGetTongClient(Lua_State* L)
{
    int         nResult         = 0;
    int         nTopIndex       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);
    
    nResult = g_pSO3World->m_TongClient.LuaGetObj(L);
Exit0:
    return nResult;
}

int LuaApplyBattleFieldStatistics(Lua_State* L)
{
    int nTopIndex = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    g_PlayerClient.DoApplyBattleFieldStatistics();

Exit0:
    return 0;
}

int LuaGetBattleFieldStatistics(Lua_State* L)
{
    int nTopIndex   = 0;
    KGPQ::KPQ_STATISTICS_MAP::iterator it;
    KGPQ::KPQ_STATISTICS_MAP::iterator itEnd;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_NewTable(L);

    for (it = g_pSO3World->m_BattleFieldStatisticsMap.begin(), itEnd = g_pSO3World->m_BattleFieldStatisticsMap.end(); it != itEnd; ++it)
    {
        Lua_PushNumber(L, it->first);
        Lua_NewTable(L);
        
        Lua_PushString(L, "Name");
        Lua_PushString(L, it->second.szPlayerName);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "ForceID");
        Lua_PushNumber(L, it->second.dwPlayerForceID);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "BattleFieldSide");
        Lua_PushNumber(L, it->second.nBattleFieldSide);
        Lua_SetTable(L, -3);

        for (int i = 0; i < psiTotal; i++)
        {
            Lua_PushNumber(L, i);
            Lua_PushNumber(L, it->second.nValueArray[i]);
            Lua_SetTable(L, -3);
        }

        Lua_SetTable(L, -3);
    }

Exit0:
    return 1;
}

int LuaGetBattleFieldObjective(Lua_State* L)
{
    int nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_NewTable(L);

    for (int i = 0; i < PQ_PARAM_COUNT; i++)
    {
        Lua_PushNumber(L, i + 1);
        Lua_NewTable(L);

        Lua_PushNumber(L, 1);
        Lua_PushNumber(L, g_pSO3World->m_nBattleFieldObjectiveCurrent[i]);
        Lua_SetTable(L, -3);

        Lua_PushNumber(L, 2);
        Lua_PushNumber(L, g_pSO3World->m_nBattleFieldObjectiveTarget[i]);
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }

Exit0:
    return 1;
}

int LuaGetBattleFieldPQInfo(Lua_State* L)
{
    int nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_PushNumber(L, g_pSO3World->m_dwBattleFieldPQID);
    Lua_PushNumber(L, g_pSO3World->m_dwBattleFieldPQTemplateID);
    Lua_PushNumber(L, g_pSO3World->m_nBattleFieldPQBeginTime);
    Lua_PushNumber(L, g_pSO3World->m_nBattleFieldPQEndTime);

Exit0:
    return 4;
}
#endif //_CLIENT

#ifdef _SERVER
int LuaApplyCreateTong(Lua_State* L)
{
    int         nTopIndex       = 0;
    DWORD       dwPlayerID      = 0;
    const char* pszTongName     = NULL;
    size_t      uNameLen        = 0;
    int         nTemplateIndex  = -1;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwPlayerID      = (DWORD)Lua_ValueToNumber(L, 1);
    
    pszTongName     = Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszTongName);

    uNameLen = strlen(pszTongName);
    KGLOG_PROCESS_ERROR(uNameLen < _NAME_LEN);

    nTemplateIndex  = (int)Lua_ValueToNumber(L,3);

    g_RelayClient.DoApplyCreateTongRequest(dwPlayerID, pszTongName, nTemplateIndex);
Exit0:
    return 0;
}
#endif // _SERVER

#ifdef _CLIENT
int LuaGetAuctionClient(Lua_State* L)
{
    int nResult     = 0;
    int nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    nResult = g_pSO3World->m_AuctionClient.LuaGetObj(L);
Exit0:
    return nResult;
}

#endif // _CLIENT

#ifdef _SERVER
int LuaRemoteCallToCenter(Lua_State* L)
{
    return g_pSO3World->m_CenterRemote.Call(L);
}

int LuaRemoteCallToClient(Lua_State* L)
{
    return g_pSO3World->m_ScriptServer.Call(L);
}
#else
int LuaRemoteCallToServer(Lua_State* L)
{
    return g_pSO3World->m_ScriptClient.Call(L);
}
#endif

int LuaFormatTime(Lua_State* L)
{
    int         nTopIndex   = 0;
    time_t      nTime       = 0;
    struct tm*  pLocalTime  = NULL;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	nTime = (time_t)Lua_ValueToNumber(L, 1);
	pLocalTime = localtime(&nTime);
	KGLOG_PROCESS_ERROR(pLocalTime);

    Lua_NewTable(L);

	Lua_PushString(L, "year");
	Lua_PushNumber(L, pLocalTime->tm_year + 1900);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "month");
	Lua_PushNumber(L, pLocalTime->tm_mon + 1);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "day");
	Lua_PushNumber(L, pLocalTime->tm_mday);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "hour");
	Lua_PushNumber(L, pLocalTime->tm_hour);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "minute");
	Lua_PushNumber(L, pLocalTime->tm_min);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "second");
	Lua_PushNumber(L, pLocalTime->tm_sec);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "weekday");
	Lua_PushNumber(L, pLocalTime->tm_wday);
	Lua_SetTable(L, -3);

	return 1;
Exit0:
    return 0;
}

int LuaGetTimezone(Lua_State* L)
{
    Lua_PushNumber(L, timezone);
    return 1;
}

#ifdef _SERVER
int LuaGetLocalIP(Lua_State* L)
{
    Lua_PushString(L, g_PlayerServer.m_szLocalIP);
    return 1;
}
#endif


#ifdef _CLIENT
template <class TFunc>
struct KTraverseGetAllObject
{
    std::vector<TFunc*> m_Result;
    BOOL operator () (DWORD dwID, TFunc* pObj)
    {
        m_Result.push_back(pObj);
        return true;
    }
};

int LuaGetAllPlayer(Lua_State* L)
{
    int                             nResult   = 0;
    int                             nRetCode  = 0;
    int                             nTopIndex = 0;
    int                             nTable    = 0;
    int                             nIndex    = 1;
    KTraverseGetAllObject<KPlayer>  Func;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    g_pSO3World->m_PlayerSet.Traverse(Func);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    for (std::vector<KPlayer*>::iterator it = Func.m_Result.begin(); it != Func.m_Result.end(); ++it)
    {
        KPlayer* pPlayer = *it;

        Lua_PushNumber(L, nIndex++);

        nRetCode = pPlayer->LuaGetObj(L);
        KGLOG_PROCESS_ERROR(nRetCode);

        Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}


int LuaGetAllNpc(Lua_State* L)
{
    int                         nResult   = 0;
    int                         nRetCode  = 0;
    int                         nTopIndex = 0;
    int                         nTable    = 0;
    int                         nIndex    = 1;
    KTraverseGetAllObject<KNpc> Func;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    g_pSO3World->m_NpcSet.Traverse(Func);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    for (std::vector<KNpc*>::iterator it = Func.m_Result.begin(); it != Func.m_Result.end(); ++it)
    {
        KNpc* pNpc = *it;
        Lua_PushNumber(L, nIndex++);

        nRetCode = pNpc->LuaGetObj(L);
        KGLOG_PROCESS_ERROR(nRetCode);

        Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetAllDoodad(Lua_State* L)
{
    int                             nResult   = 0;
    int                             nRetCode  = 0;
    int                             nTopIndex = 0;
    int                             nTable    = 0;
    int                             nIndex    = 1;
    KTraverseGetAllObject<KDoodad>  Func;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    g_pSO3World->m_DoodadSet.Traverse(Func);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    for (std::vector<KDoodad*>::iterator it = Func.m_Result.begin(); it != Func.m_Result.end(); ++it)
    {
        KDoodad* pDoodad = *it;
        Lua_PushNumber(L, nIndex++);

        nRetCode = pDoodad->LuaGetObj(L);
        KGLOG_PROCESS_ERROR(nRetCode);

        Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int LuaBoruoboluomi(Lua_State* L)
{
    int nTopIndex = 0;
    int nNumber   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nNumber = (int)Lua_ValueToNumber(L, 1);

    g_PlayerClient.DoQueryProductIdentity(nNumber);

Exit0:
    return 0;
}
#endif

#ifdef _SERVER
int LuaGetGlobalSystemValueCache(Lua_State* L)
{
    return g_pSO3World->m_GlobalSystemValueCache.LuaGetObj(L);
}

int LuaApplyChangeTongCamp(Lua_State* L)
{
    int     nTopIndex   = 0;
    DWORD   dwPlayerID  = ERROR_ID;
    int     nCamp       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwPlayerID);

    nCamp = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nCamp == cGood || nCamp == cEvil);

    g_RelayClient.DoChangeTongCampRequest(dwPlayerID, (KCAMP)nCamp);
Exit0:
    return 0;
}

int LuaSetupScriptServer(Lua_State* L)
{
    g_pSO3World->m_ScriptServer.Setup();
    return 0;
}
#endif // _SERVER

	/************************************************************************/
	KLuaFunc BaseFuncList[] = 
	{
		{"Test",					LuaTest},
        {"GetCurrentTime",          LuaGetCurrentTime},
        {"GetLogicDirection",       LuaGetLogicDirection},
		{"LoadScene",				Luna<KScene>::Register},
		{"LoadCell",				Luna<KCell>::Register},
		{"LoadPlayer",				Luna<KPlayer>::Register},
		{"LoadNpc",					Luna<KNpc>::Register},
		{"LoadNpcTemplate",			Luna<KNpcTemplate>::Register},
		{"LoadDoodad",				Luna<KDoodad>::Register},
		{"LoadDoodadTemplate",		Luna<KDoodadTemplate>::Register},
		{"LoadItem",				Luna<KItem>::Register},
        {"LoadSkill",               Luna<KSkill>::Register},
		{"LoadQuestInfo",			Luna<KQuestInfo>::Register},
        {"LoadCustomEquipInfo",		Luna<KCustomEquipInfo>::Register},
        {"LoadOtherItemInfo",		Luna<KOtherItemInfo>::Register},
        {"LoadShop",                Luna<KShop>::Register},

        {"LoadAILogic",             Luna<KAILogic>::Register},
        {"LoadAIState",             Luna<KAIState>::Register},
        {"LoadAIAction",            Luna<KAIAction>::Register},
#ifdef _CLIENT
        {"LoadTeamClient",          Luna<KTeamClient>::Register},
#endif
        {"LoadCampInfo",            Luna<KCampInfo>::Register},
#ifdef _CLIENT
        {"LoadTongClient",		    Luna<KTongClient>::Register},
        {"LoadAuctionClient",       Luna<KAuctionClient>::Register},
#endif
		{"LoadProfession",			LuaLoadProfession},
		{"GetCraftList",			LuaGetCraftList},
		{"GetCraft",				LuaGetCraft},

        {"GetLogicFrameCount",      LuaGetLogicFrameCount},
        {"IsPlayer",				LuaIsPlayer},
		{"GetScene",				LuaGetScene},
		{"GetPlayer",				LuaGetPlayer},
		{"GetNpc",					LuaGetNpc},
		{"GetDoodad",				LuaGetDoodad},
		{"GetSkill",				LuaGetSkill},
		{"GetItem",					LuaGetItem},
		{"GetItemInfo",				LuaGetItemInfo},
		{"GetQuestInfo",			LuaGetQuestInfo},
		{"GetQuestString",			LuaGetQuestString},
		{"GetProfession",			LuaGetProfession},
		{"GetRecipeName",			LuaGetRecipeName},
		{"GetRecipe",				LuaGetRecipe},
		{"GetLevelUpData",			LuaGetLevelUpData},
        {"GetFileNameHash",         LuaGetFileNameHash},

		{"FormatString",			LuaFormatString},

        {"CheckWeaponType",         LuaCheckWeaponType},
		{"GetDoodadTemplate",		LuaGetDoodadTemplate},

		{"GetRelation",				LuaGetRelation},
		{"IsNeutrality",			LuaIsNeutrality},
		{"IsAlly",					LuaIsAlly},
		{"IsEnemy",					LuaIsEnemy},
		{"IsParty",					LuaIsParty},
		{"IsSelf",					LuaIsSelf},
		{"Random",					LuaRandom},
		{"GetLocalTime",			LuaGetLocalTime},
		{"GetCharacterDistance",	LuaGetCharacterDistance},
		{"GetDistanceSq",			LuaGetDistanceSq },
    
		{"GetNpcTemplate",			LuaGetNpcTemplate},
  
#ifdef _CLIENT
		{"GetNpcName",				LuaGetNpcName},
        {"LootItem",                LuaLootItem},
        {"RollItem",                LuaRollItem},
		{"LootMoney",				LuaLootMoney},

		{"GetClientPlayer",			LuaGetClientPlayer},
        {"GetClientScene",          LuaGetClientScene},
		{"SendGMCommand",			LuaSendGMCommand},

		{"GetMapParams",			LuaGetMapParams},
		{"GetMapList",				LuaGetMapList},
		{"LoadingComplete",			LuaLoadingComplete},
        {"GetSkillInfo",            LuaGetSkillInfo},
		{"SkillRecipeKeyToIDAndLevel",LuaSkillRecipeKeyToIDAndLevel},
		{"SkillRecipeIDAndLevelToKey",LuaSkillRecipeIDAndLevelToKey},
        {"GetBuffInfo",             LuaGetBuffInfo},
        {"GetBindBuffInfo",         LuaGetBindBuffInfo},
        {"GetBuffTime",             LuaGetBuffTime},
        {"GetBindBuffTime",         LuaGetBindBuffTime},

#endif	//_CLIENT

        {"IsCommonSkill",           LuaIsCommonSkill},
		{"GetBankPackagePrice",		LuaGetBankPackagePrice},

#ifdef _SERVER
        {"SendSystemMail",          LuaSendSystemMail},
        {"SendGlobalMail",     		LuaSendGlobalMail},
#endif

#ifdef _CLIENT
        {"GetPingValue",            LuaGetPingValue},
		{"GetPlayerByName",			LuaGetPlayerByName},
		{"SetUserPreferences",		LuaSetUserPreferences},
		{"GetUserPreferences",		LuaGetUserPreferences},

		{"GetReputeLimit",			LuaGetReputeLimit},

        {"SendMail",                LuaSendMail},
        {"DeleteMail",              LuaDeleteMail},
        {"CountMail",               LuaCountMail},
        {"ApplyMailList",           LuaApplyMailList},
        {"GetMailList",             LuaGetMailList},
        {"GetMailContent",          LuaGetMailContent},
        {"GetMailAttachment",       LuaGetMailAttachment},
        {"SetMailRead",             LuaSetMailRead},
        {"ReturnMail",              LuaReturnMail},

		{"ResetGameworld",			LuaResetGameworld},
#endif	//_CLIENT

#ifdef _SERVER
        {"BindNpcMasterID",         LuaBindNpcMasterID},
        {"GetNpcMasterID",          LuaGetNpcMasterID},
		{"BindRecipeMasterID",		LuaBindRecipeMasterID},
		{"GetRecipeMasterID",		LuaGetRecipeMasterID},
        {"BindNpcMailbox",          LuaBindNpcMailbox},
        {"IsNpcMailbox",            LuaIsNpcMailbox},
#endif

#ifdef _CLIENT
        {"GetMasterSkillList",      LuaGetMasterSkillList},
		{"GetSkillLearningInfo",	LuaGetSkillLearningInfo},
		{"GetRecipeLearnInfo",		LuaGetRecipeLearnInfo},
		{"GetMasterRecipeList",		LuaGetMasterRecipeList},
#endif

#ifdef _SERVER
        {"SendGlobalSysMsg",        LuaSendGlobalSysMsg},
#endif

#ifdef _CLIENT
        {"PeekOtherPlayer",         LuaPeekOtherPlayerEquip},
        {"PeekOtherPlayerBook",     LuaPeekOtherPlayerBook},
        {"PeekOtherPlayerQuest",    LuaPeekOtherPlayerQuest},
		{"LogoutGame",				LuaLogoutGame},
#endif

#ifdef _SERVER
        {"SetRouteNodeForce",       LuaSetRouteNodeForce},
#endif

#ifdef _CLIENT
        {"CheckRoadTrack",          LuaCheckRoadTrack},
        {"CalculateTrackCost",      LuaCalculateTrackCost},
        {"RoadTrackStartOut",       LuaRoadTrackStartOut},
        {"RoadTrackForceRequest",   LuaRoadTrackForceRequest},
        {"RoadTrackIsCityOpen",     LuaRoadTrackIsCityOpen},

        {"GetNearbyPlayerList",     LuaGetNearbyPlayerList},
        {"GetNearbyNpcList",        LuaGetNearbyNpcList},
        {"GetNearbyDoodadList",     LuaGetNearbyDoodadList},
#endif

		{"ReloadQuestTable",		LuaReloadQuestTable},
        {"IsTempEnchantAttribute",  LuaIsTempEnchantAttribute},
        {"GetEnchantAttribute",     LuaGetEnchantAttribute},

        {"BookID2GlobelRecipeID",   LuaBookID2GlobelRecipeID},
        {"GlobelRecipeID2BookID",   LuaGlobelRecipeID2BookID},
        {"GetCoolDownFrame",        LuaGetCoolDownFrame},
        {"GetSkillRecipeBaseInfo",  LuaGetSkillRecipeBaseInfo},

        {"SendGmMessage",           LuaSendGmMessage},

#ifdef _SERVER
        {"GSCommand",               LuaGSCommand},
        {"GCCommand",               LuaGCCommand},
        {"ReloadAI",                LuaReloadAI},
#endif

#ifdef _CLIENT
        {"SetPartyMark",            LuaSetPartyMark},
#endif

        {"Log",                     LuaLog},
#ifdef _SERVER
        {"IsEscortQuest",           LuaIsEscortQuest},
#endif
#ifdef _CLIENT
        {"GetClientTeam",           LuaGetClientTeam},
        {"JoinBattleFieldQueue",    LuaJoinBattleFieldQueue},
        {"LeaveBattleFieldQueue",   LuaLeaveBattleFieldQueue},
        {"AcceptJoinBattleField",   LuaAcceptJoinBattleField},
        {"GetBattleFieldList",      LuaGetBattleFieldList},
        {"LeaveBattleField",        LuaLeaveBattleField},
#endif
        {"IsNpcExist",              LuaIsNpcExist},
        {"IsDoodadExist",           LuaIsDoodadExist},
        {"IsPlayerExist",           LuaIsPlayerExist},
#ifdef _SERVER
        {"AddCampScore",            LuaAddCampScore},
#endif
        {"GetCampInfo",             LuaGetCampInfo},

#ifdef _SERVER
        {"BattleFieldEnd",          LuaBattleFieldEnd},
        {"CreatePQ",                LuaCreatePQ},
        {"DelPQ",                   LuaDelPQ},
        {"ChangePQValue",           LuaChangePQValue},
        {"GetPQ",                   LuaGetPQ},
        {"GetPQByTemplate",         LuaGetPQByTemplate},
#endif

#ifdef _CLIENT
        {"GetTongClient",           LuaGetTongClient},

        {"ApplyBattleFieldStatistics",  LuaApplyBattleFieldStatistics},
        {"GetBattleFieldStatistics",    LuaGetBattleFieldStatistics},
        {"GetBattleFieldObjective",     LuaGetBattleFieldObjective},
        {"GetBattleFieldPQInfo",        LuaGetBattleFieldPQInfo},
#endif

#ifdef _SERVER
        {"ApplyCreateTong",         LuaApplyCreateTong},
#endif

#ifdef _CLIENT
        {"GetAuctionClient",        LuaGetAuctionClient},
#endif

#ifdef _SERVER
        {"RemoteCallToCenter",         LuaRemoteCallToCenter},
        {"RemoteCallToClient",         LuaRemoteCallToClient},
#else
        {"RemoteCallToServer",         LuaRemoteCallToServer},
#endif

        {"FormatTime",              LuaFormatTime},
        {"GetTimezone",             LuaGetTimezone},

#ifdef _SERVER
        {"GetLocalIP",              LuaGetLocalIP},
#endif

#ifdef _CLIENT
        {"GetAllPlayer",            LuaGetAllPlayer},
        {"GetAllNpc",               LuaGetAllNpc},
        {"GetAllDoodad",            LuaGetAllDoodad},
        {"Boruoboluomi",            LuaBoruoboluomi},
#endif

#ifdef _SERVER
        {"GetGlobalSystemValueCache",   LuaGetGlobalSystemValueCache},
        {"ApplyChangeTongCamp",         LuaApplyChangeTongCamp},
        {"SetupScriptServer",           LuaSetupScriptServer},
#endif // _SERVER
    };

	/************************************************************************/
	int GetBaseFuncList(KLuaFunc*& pFuncList)
	{
		pFuncList = BaseFuncList;
		return sizeof(BaseFuncList) / sizeof(KLuaFunc);
	}

}	//end namespace KScriptFuncList

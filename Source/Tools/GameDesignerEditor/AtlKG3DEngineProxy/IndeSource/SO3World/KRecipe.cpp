#include "stdafx.h"
#include "KRecipe.h"
#include "KAISearchTactics.h"
#include "KSO3World.h"

class KPlayer;

#define RECIPE_STAMINA							"CostStamina"
#define RECIPE_THEW							    "CostThew"
#define RECIPE_PROFESSION_ID					"ProfessionID"
#define RECIPE_REQUIRE_PROFESSION_LEVEL			"RequireLevel"
#define RECIPE_PROFESSION_ID_EXT				"ProfessionIDExt"
#define RECIPE_REQUIRE_PROFESSION_LEVEL_EXT		"RequireLevelExt"
#define RECIPE_TOOL_ITEM_TYPE					"ToolItemType"
#define RECIPE_TOOL_ITEM_INDEX					"ToolItemIndex"
#define RECIPE_EQUIPMENT_TYPE					"EquipmentType"
#define RECIPE_EXP								"Exp"
#define RECIPE_PREPARE_FRAME					"PrepareFrame"

#define RECIPE_DOODAD_TEMPLATE_ID				"DoodadTemplateID"

#define RECIPE_ID								"ID"
#define RECIPE_NAME								"Name"
#define RECIPE_BELONG							"Belong"
#define RECIPE_RESULT_ON_LEVEL					"ResultOnLevel"
#define RECIPE_REQUIRE_ITEM_TYPE				"RequireItemType"
#define RECIPE_REQUIRE_ITEM_INDEX				"RequireItemIndex"
#define RECIPE_REQUIRE_ITEM_COUNT				"RequireItemCount"
#define RECIPE_CREATE_ITEM_TYPE					"CreateItemType"
#define RECIPE_CREATE_ITEM_INDEX				"CreateItemIndex"
#define RECIPE_CREATE_ITEM_MIN					"CreateItemMin"
#define RECIPE_CREATE_ITEM_MAX					"CreateItemMax"
#define RECIPE_CREATE_ITEM_PROBABILITY			"CreateItemProbability"
#define RECIPE_REQUIRE_BRANCH_ID				"RequireBranchID"
#define RECIPE_REQUIRE_DOODAD					"RequireDoodad"
#define RECIPE_SCRIPT_NAME                      "ScriptName"

#define COPY_SUB_ID                             "SubID"

#define ENCHANT_RECIPE_ID						"ID"
#define ENCHANT_NAME							"Name"
#define ENCHANT_BELONG							"Belong"
#define ENCHANT_ID	        				    "EnchantID"
#define ENCHANT_REQUIRE_ITEM_TYPE				"RequireItemType"
#define ENCHANT_REQUIRE_ITEM_INDEX				"RequireItemIndex"
#define ENCHANT_REQUIRE_ITEM_COUNT				"RequireItemCount"
#define ENCHANT_REQUIRE_BRANCH_ID				"RequireBranchID"
#define ENCHANT_REQUIRE_DOODAD					"RequireDoodad"


CRAFT_RESULT_CODE KRecipeBase::CheckCast(
    KPlayer* pCaster, KTarget& Target, DWORD dwProfessionIDExt, DWORD dwRequireProfessionLevelExt
)
{
	CRAFT_RESULT_CODE   nResult             = crcFailed;
	BOOL                bRetCode            = false;
	int                 nRetCode            = 0;
	DWORD               dwProfessionLevel   = 0; 
    
	KGLOG_PROCESS_ERROR(pCaster);

	// 是否学了相应的专业
	bRetCode = pCaster->m_ProfessionList.IsProfessionLearned(dwProfessionID);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, crcProfessionNotLearned);

	// 专业等级是否足够
	dwProfessionLevel = pCaster->m_ProfessionList.GetProfessionLevel(dwProfessionID);
	KG_PROCESS_ERROR_RET_CODE(dwProfessionLevel >= dwRequireProfessionLevel, crcTooLowProfessionLevel);
    
    if (dwProfessionIDExt != 0)
    {
        DWORD dwProfessionLevelExt = 0;
        // 是否学了扩展专业
	    bRetCode = pCaster->m_ProfessionList.IsProfessionLearned(dwProfessionIDExt);
	    KG_PROCESS_ERROR_RET_CODE(bRetCode, crcExtProfessionNotLearned);

	    // 扩展专业等级是否足够
	    dwProfessionLevelExt = pCaster->m_ProfessionList.GetProfessionLevel(dwProfessionIDExt);
	    KG_PROCESS_ERROR_RET_CODE(dwProfessionLevelExt >= dwRequireProfessionLevelExt, crcTooLowExtProfessionLevel);
    }

	// 是否带了相应的工具
	if (dwToolItemType != 0 && dwToolItemIndex != 0)
	{
		nRetCode = pCaster->m_ItemList.GetItemAmountInEquipAndPackage(dwToolItemType, dwToolItemIndex);
		KG_PROCESS_ERROR_RET_CODE(nRetCode > 0, crcErrorTool);
	}

	// 主手的武器类型是否符合
	if (nEquipmentType >= 0)
	{
		nRetCode = pCaster->m_ItemList.GetMainHandType();
		KG_PROCESS_ERROR_RET_CODE(nRetCode == nEquipmentType, crcWeaponError);
	}

	nResult = crcSuccess;
Exit0:
	return nResult;
}

const char* KRecipeBase::GetName()
{
	return NULL;
}

CRAFT_RESULT_CODE KCraftRecipe::CanCast(KPlayer* pCaster, KTarget& Target)
{
	CRAFT_RESULT_CODE   nResult	                = crcFailed;
	BOOL                bRetCode				= false;
	return nResult;
}

const char* KCraftRecipe::GetName()
{
	return const_cast<const char*>(szName);
}

CRAFT_RESULT_CODE KCraftCollection::CanCast(KPlayer* pCaster, KTarget& Target)
{
	CRAFT_RESULT_CODE nResult   = crcFailed;
    CRAFT_RESULT_CODE eRetCode  = crcFailed;
    BOOL              bRetCode  = false;
    KDoodad*          pDoodad   = NULL;

	KGLOG_PROCESS_ERROR(pCaster);

	eRetCode = CheckCast(pCaster, Target);
	KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
    
    bRetCode = Target.GetTarget(&pDoodad);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    KG_PROCESS_ERROR_RET_CODE(pDoodad->m_pLootList || pCaster->m_nCurrentThew >= nThew, crcNotEnoughThew);

	nResult = crcSuccess;
Exit0:
	return nResult;
}

CRAFT_RESULT_CODE KCraftRead::CanCast(KPlayer* pCaster, KTarget& Target)
{
    CRAFT_RESULT_CODE nResult               = crcFailed;
    CRAFT_RESULT_CODE eRetCode              = crcFailed;
    BOOL              bRetCode              = false;
    
	KGLOG_PROCESS_ERROR(pCaster);
    
	eRetCode = CheckCast(pCaster, Target);
	KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
    
    KG_PROCESS_ERROR_RET_CODE(pCaster->m_nCurrentStamina >= nStamina, crcNotEnoughStamina);

    bRetCode = pCaster->m_BookList.IsBookMemorized(dwID, dwSubID); // 必须是未阅读过的
	KG_PROCESS_ERROR_RET_CODE(!bRetCode, crcBookIsAlreadyMemorized);
    
	nResult = crcSuccess;
Exit0:
	return nResult;
}

const char* KCraftRead::GetName()
{
	return const_cast<const char*>(szName);
}

CRAFT_RESULT_CODE KCraftEnchant::CanCast(KPlayer* pCaster, KTarget& Target)
{
	CRAFT_RESULT_CODE   nResult	            = crcFailed;
	BOOL                bRetCode			= false;
	return nResult;
}

const char* KCraftEnchant::GetName()
{
	return const_cast<const char*>(szName);
}

CRAFT_RESULT_CODE KCraftCopy::CanCast(KPlayer* pCaster, KTarget& Target)
{
    CRAFT_RESULT_CODE   nResult	                = crcFailed;
    CRAFT_RESULT_CODE   eRetCode                = crcFailed;
    BOOL                bRetCode                = false; 
    DWORD               dwCurrentItemCount      = 0;
    int                 nFreeRoom               = 0;

    KGLOG_PROCESS_ERROR(pCaster);
        
    eRetCode = CheckCast(pCaster, Target, dwProfessionIDExt, dwRequireProfessionLevelExt);
	KG_PROCESS_ERROR_RET_CODE(eRetCode == crcSuccess, eRetCode);
    
    KG_PROCESS_ERROR_RET_CODE(pCaster->m_nCurrentThew >= nThew, crcNotEnoughThew);
    KG_PROCESS_ERROR_RET_CODE(pCaster->m_nLevel >= nRequirePlayerLevel, crcTooLowLevel);
    
    // 身上道具是否足够
	for (int nIndex = 0; nIndex < MAX_COPY_MATERIAL; ++nIndex)
	{
		if (dwRequireItemType[nIndex] == 0 || dwRequireItemIndex[nIndex] == 0)
        {
			continue;
        }

		dwCurrentItemCount = pCaster->m_ItemList.GetItemAmountInEquipAndPackage(dwRequireItemType[nIndex], dwRequireItemIndex[nIndex]);
		KG_PROCESS_ERROR_RET_CODE(dwCurrentItemCount >= dwRequireItemCount[nIndex], crcNotEnoughItem);
	}
    
    if (dwCreateItemType != 0 && dwCreateItemIndex != 0)
    {
        // 身上是否有足够的空格子
        nFreeRoom = pCaster->m_ItemList.GetAllPackageFreeRoomSize();
        KG_PROCESS_ERROR_RET_CODE(nFreeRoom >= 1, crcInventoryIsFull);    
    }
    
    // 如果该书不是从doodad上抄录，则必须是玩家阅读过的
    if (dwDoodadTemplateID == 0)
    {
        bRetCode = pCaster->m_BookList.IsBookMemorized(dwID, dwSubID);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    if (dwCoolDownID != 0)
    {
        bRetCode = pCaster->m_TimerList.CheckTimer(dwCoolDownID);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, crcSkillNotReady);
    }

    nResult = crcSuccess;
Exit0:
	return nResult;
}

const char* KCraftCopy::GetName()
{
    return const_cast<const char*>(szName);
}

//----------------------------------------------------------------------------------------

template<>
BOOL KRecipe<KCraftRecipe>::Init(const char* szFileName)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	int         nIndex      = 0;
	int         nHeight     = 0;
	ITabFile*   piTabFile   = NULL;
	KCraftRecipe CraftRecipe;
	KCraftRecipe DefaultCraftRecipe;
	char szFilePath[MAX_PATH];

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, szFileName);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);

	memset(&DefaultCraftRecipe, 0, sizeof(DefaultCraftRecipe));
	bRetCode = LoadLine(piTabFile, 2, DefaultCraftRecipe, DefaultCraftRecipe);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 3; nIndex <= nHeight; nIndex++)
	{
		bRetCode = LoadLine(piTabFile, nIndex, CraftRecipe, DefaultCraftRecipe);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(CraftRecipe.dwID < MAX_RECIPE_ID);

		CraftRecipe.eCraftType = actProduce;

		if (CraftRecipe.dwID)
		{
			m_mapRecipe[CraftRecipe.dwID] = CraftRecipe;
		}
		else
		{
			KGLOG_PROCESS_ERROR(false && "Error Recipe ID\n");
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

template<>
BOOL KRecipe<KCraftRecipe>::LoadLine(ITabFile* piTabFile, int nLine, KCraftRecipe& Recipe, const KCraftRecipe& DefaultRecipe)
{
    BOOL    bResult  = false;
	BOOL    bRetCode = false;
    int     nRetCode = 0;
	int     nIndex   = 0;
	char    szTempColName[_NAME_LEN];
    char    szScriptName[MAX_PATH];

	KGLOG_PROCESS_ERROR(piTabFile);
	
	bRetCode = piTabFile->GetInteger(nLine, RECIPE_STAMINA, DefaultRecipe.nStamina, (int*)&(Recipe.nStamina));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_PROFESSION_ID, DefaultRecipe.dwProfessionID, (int*)&(Recipe.dwProfessionID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_REQUIRE_PROFESSION_LEVEL, DefaultRecipe.dwRequireProfessionLevel, (int*)&(Recipe.dwRequireProfessionLevel));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_TOOL_ITEM_TYPE, DefaultRecipe.dwToolItemType, (int*)&(Recipe.dwToolItemType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_TOOL_ITEM_INDEX, DefaultRecipe.dwToolItemIndex, (int*)&(Recipe.dwToolItemIndex));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_EQUIPMENT_TYPE, DefaultRecipe.nEquipmentType, (int*)&(Recipe.nEquipmentType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_EXP, DefaultRecipe.dwProfessionExp, (int*)&(Recipe.dwProfessionExp));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_PREPARE_FRAME, DefaultRecipe.nPrepareFrame, (int*)&(Recipe.nPrepareFrame));
	KGLOG_PROCESS_ERROR(bRetCode);

	//以下是合成的物品需求
	bRetCode = piTabFile->GetInteger(nLine, RECIPE_ID, DefaultRecipe.dwID, (int*)&(Recipe.dwID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nLine, RECIPE_NAME, DefaultRecipe.szName, Recipe.szName, MAX_RECIPE_NAME_LEN);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nLine, RECIPE_BELONG, DefaultRecipe.szBelong, Recipe.szBelong, MAX_BELONG_NAME_LEN);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_RESULT_ON_LEVEL, DefaultRecipe.bResultOnLevel, (int*)&(Recipe.bResultOnLevel));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_REQUIRE_BRANCH_ID, DefaultRecipe.dwRequireBranchID, (int*)&(Recipe.dwRequireBranchID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_REQUIRE_DOODAD, DefaultRecipe.dwRequireDoodadID, (int*)&(Recipe.dwRequireDoodadID));
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 0; nIndex < MAX_MATERIAL; nIndex++)
	{
		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_REQUIRE_ITEM_TYPE, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwRequireItemType[nIndex], (int*)&(Recipe.dwRequireItemType[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_REQUIRE_ITEM_INDEX, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwRequireItemIndex[nIndex], (int*)&(Recipe.dwRequireItemIndex[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_REQUIRE_ITEM_COUNT, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwRequireItemCount[nIndex], (int*)&(Recipe.dwRequireItemCount[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	for (nIndex = 0; nIndex < MAX_RESULTANT; nIndex++)
	{
		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_CREATE_ITEM_TYPE, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwCreateItemType[nIndex], (int*)&(Recipe.dwCreateItemType[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_CREATE_ITEM_INDEX, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwCreateItemIndex[nIndex], (int*)&(Recipe.dwCreateItemIndex[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_CREATE_ITEM_MIN, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwCreateItemMinCount[nIndex], (int*)&(Recipe.dwCreateItemMinCount[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_CREATE_ITEM_MAX, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwCreateItemMaxCount[nIndex], (int*)&(Recipe.dwCreateItemMaxCount[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);
	
		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_CREATE_ITEM_PROBABILITY, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwCreateItemProbability[nIndex], (int*)&(Recipe.dwCreateItemProbability[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);
	}
	
    Recipe.dwScriptID = 0;
    bRetCode = piTabFile->GetString(nLine, RECIPE_SCRIPT_NAME, "", szScriptName, sizeof(szScriptName));
    KGLOG_PROCESS_ERROR(bRetCode);

    if (szScriptName[0] != '\0')
    {
        Recipe.dwScriptID = g_FileNameHash(szScriptName);

        assert(Recipe.dwScriptID != 0);
    }

	bResult = true;
Exit0:
	return bResult;
}

template<>
KCraftRecipe* KRecipe<KCraftRecipe>::GetRecipe(DWORD dwID)
{
	KCraftRecipe* pResult = NULL;
	RECIPE_MAP::iterator it;

	it = m_mapRecipe.find(dwID);
	if (it != m_mapRecipe.end())
	{
		pResult = &(it->second);
	}
Exit0:
	return pResult;
}

//--------------------------------------------------------------------------------------------

template<>
BOOL KRecipe<KCraftCollection>::Init(const char* szFileName)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	int         nIndex      = 0;
	int         nHeight     = 0;
	ITabFile*   piTabFile   = NULL;
	KCraftCollection CraftCollection;
	KCraftCollection DefaultCraftCollection;
	char szFilePath[MAX_PATH];

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, szFileName);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);

	memset(&DefaultCraftCollection, 0, sizeof(DefaultCraftCollection));
	bRetCode = LoadLine(piTabFile, 2, DefaultCraftCollection, DefaultCraftCollection);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 3; nIndex <= nHeight; nIndex++)
	{
		bRetCode = LoadLine(piTabFile, nIndex, CraftCollection, DefaultCraftCollection);
		KGLOG_PROCESS_ERROR(bRetCode);

		CraftCollection.eCraftType = actCollection;

		if (CraftCollection.dwDoodadTemplateID)
		{
			m_mapRecipe[CraftCollection.dwDoodadTemplateID] = CraftCollection;
		}
		else
		{
			KGLOG_PROCESS_ERROR(false && "Error DoodadTemplateID");
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

template<>
BOOL KRecipe<KCraftCollection>::LoadLine(ITabFile* piTabFile, int nLine, KCraftCollection& Recipe, const KCraftCollection& DefaultRecipe)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KGLOG_PROCESS_ERROR(piTabFile);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_THEW, DefaultRecipe.nThew, (int*)&(Recipe.nThew));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_PROFESSION_ID, DefaultRecipe.dwProfessionID, (int*)&(Recipe.dwProfessionID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_REQUIRE_PROFESSION_LEVEL, DefaultRecipe.dwRequireProfessionLevel, (int*)&(Recipe.dwRequireProfessionLevel));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_TOOL_ITEM_TYPE, DefaultRecipe.dwToolItemType, (int*)&(Recipe.dwToolItemType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_TOOL_ITEM_INDEX, DefaultRecipe.dwToolItemIndex, (int*)&(Recipe.dwToolItemIndex));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_EQUIPMENT_TYPE, DefaultRecipe.nEquipmentType, (int*)&(Recipe.nEquipmentType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_EXP, DefaultRecipe.dwProfessionExp, (int*)&(Recipe.dwProfessionExp));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_PREPARE_FRAME, DefaultRecipe.nPrepareFrame, (int*)&(Recipe.nPrepareFrame));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_DOODAD_TEMPLATE_ID, DefaultRecipe.dwDoodadTemplateID, (int*)&(Recipe.dwDoodadTemplateID));
	KGLOG_PROCESS_ERROR(bRetCode);

    Recipe.dwScriptID = 0;

	bResult = true;
Exit0:
	return bResult;
}

template<>
KCraftCollection* KRecipe<KCraftCollection>::GetRecipe(DWORD dwID)
{
	KCraftCollection* pResult = NULL;
	RECIPE_MAP::iterator it;

	it = m_mapRecipe.find(dwID);
	if (it != m_mapRecipe.end())
	{
		pResult = &(it->second);
	}
Exit0:
	return pResult;
}

//--------------------------------------------------------------------------------------------

template<>
BOOL KRecipe<KCraftRead>::Init(const char* szFileName)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	int         nIndex      = 0;
	int         nHeight     = 0;
	ITabFile*   piTabFile   = NULL;
    int         nRecipeID   = 0;
    KCraftRead  CraftRead;
	KCraftRead  DefaultCraftRead;
	char        szFilePath[MAX_PATH];
    
    KGLOG_PROCESS_ERROR(szFileName);

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, szFileName);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);

	memset(&DefaultCraftRead, 0, sizeof(DefaultCraftRead));
	bRetCode = LoadLine(piTabFile, 2, DefaultCraftRead, DefaultCraftRead);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 3; nIndex <= nHeight; nIndex++)
	{
		bRetCode = LoadLine(piTabFile, nIndex, CraftRead, DefaultCraftRead);
		KGLOG_PROCESS_ERROR(bRetCode);

		CraftRead.eCraftType = actRead;
		nRecipeID = BookID2RecipeID(CraftRead.dwID, CraftRead.dwSubID);
        KGLOG_PROCESS_ERROR(nRecipeID >= 0 && "Error Read Book ID\n");

		m_mapRecipe[nRecipeID] = CraftRead;		
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

template<>
BOOL KRecipe<KCraftRead>::LoadLine(ITabFile* piTabFile, int nLine, KCraftRead& Recipe, const KCraftRead& DefaultRecipe)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    int     nRetCode    = 0;
    char    szScriptName[MAX_PATH];

	KGLOG_PROCESS_ERROR(piTabFile);
    
    bRetCode = piTabFile->GetInteger(nLine, "ID", DefaultRecipe.dwID, (int*)&(Recipe.dwID));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "SubID", DefaultRecipe.dwSubID, (int*)&(Recipe.dwSubID));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetString(nLine, "Name", DefaultRecipe.szName, Recipe.szName, MAX_RECIPE_NAME_LEN);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "CostStamina", DefaultRecipe.nStamina, (int*)&(Recipe.nStamina));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ProfessionID", DefaultRecipe.dwProfessionID, (int*)&(Recipe.dwProfessionID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "RequireLevel", DefaultRecipe.dwRequireProfessionLevel, (int*)&(Recipe.dwRequireProfessionLevel));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ToolItemType", DefaultRecipe.dwToolItemType, (int*)&(Recipe.dwToolItemType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ToolItemIndex", DefaultRecipe.dwToolItemIndex, (int*)&(Recipe.dwToolItemIndex));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "EquipmentType", DefaultRecipe.nEquipmentType, (int*)&(Recipe.nEquipmentType));
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "PrepareFrame", DefaultRecipe.nPrepareFrame, (int*)&(Recipe.nPrepareFrame));
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "PlayerExp", DefaultRecipe.nPlayerExp, &Recipe.nPlayerExp);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ProfessionExp", DefaultRecipe.dwProfessionExp, (int*)&(Recipe.dwProfessionExp));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ExtendProfessionID1", DefaultRecipe.dwExtendProfessionID1, (int*)&(Recipe.dwExtendProfessionID1));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ExtendExp1", DefaultRecipe.dwExtendExp1, (int*)&(Recipe.dwExtendExp1));
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "ExtendProfessionID2", DefaultRecipe.dwExtendProfessionID2, (int*)&(Recipe.dwExtendProfessionID2));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "ExtendExp2", DefaultRecipe.dwExtendExp2, (int*)&(Recipe.dwExtendExp2));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "CreateItemTab", DefaultRecipe.dwCreateItemTab, (int*)&(Recipe.dwCreateItemTab));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "CreateItemIndex", DefaultRecipe.dwCreateItemIndex, (int*)&(Recipe.dwCreateItemIndex));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "CreateItemStackNum", DefaultRecipe.nStackNum, &Recipe.nStackNum);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "BuffID", DefaultRecipe.dwBuffID, (int*)&(Recipe.dwBuffID));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "BuffLevel", DefaultRecipe.dwBuffLevel, (int*)&(Recipe.dwBuffLevel));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "Train", DefaultRecipe.nTrain, &Recipe.nTrain);
    KGLOG_PROCESS_ERROR(bRetCode);
						
    Recipe.dwScriptID = 0;
    bRetCode = piTabFile->GetString(nLine, RECIPE_SCRIPT_NAME, "", szScriptName, sizeof(szScriptName));
    KGLOG_PROCESS_ERROR(bRetCode);

    if (szScriptName[0] != '\0')
    {
        Recipe.dwScriptID = g_FileNameHash(szScriptName);

        assert(Recipe.dwScriptID != 0);
    }

	bResult = true;
Exit0:
	return bResult;
}

template<>
KCraftRead* KRecipe<KCraftRead>::GetRecipe(DWORD dwID)
{
	KCraftRead* pResult = NULL;
	RECIPE_MAP::iterator it;

	it = m_mapRecipe.find(dwID);
	if (it != m_mapRecipe.end())
	{
		pResult = &(it->second);
	}
Exit0:
	return pResult;
}

//--------------------------------------------------------------------------------------------

template<>
BOOL KRecipe<KCraftEnchant>::Init(const char* szFileName)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	int         nIndex      = 0;
	int         nHeight     = 0;
	ITabFile*   piTabFile   = NULL;
	KCraftEnchant CraftEnchant;
	KCraftEnchant DefaultCraftEnchant;
	char          szFilePath[MAX_PATH];
    
    KGLOG_PROCESS_ERROR(szFileName);

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, szFileName);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);

	memset(&DefaultCraftEnchant, 0, sizeof(DefaultCraftEnchant));
	bRetCode = LoadLine(piTabFile, 2, DefaultCraftEnchant, DefaultCraftEnchant);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 3; nIndex <= nHeight; nIndex++)
	{
		bRetCode = LoadLine(piTabFile, nIndex, CraftEnchant, DefaultCraftEnchant);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(CraftEnchant.dwID < MAX_RECIPE_ID);

		CraftEnchant.eCraftType = actEnchant;

		if (CraftEnchant.dwID)
		{
			m_mapRecipe[CraftEnchant.dwID] = CraftEnchant;
		}
		else
		{
			KGLOG_PROCESS_ERROR(false && "Error Enchant Recipe ID\n");
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

template<>
BOOL KRecipe<KCraftEnchant>::LoadLine(ITabFile* piTabFile, int nLine, KCraftEnchant& Recipe, const KCraftEnchant& DefaultRecipe)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
    int         nRetCode    = 0;
	int         nIndex      = 0;
	char        szTempColName[_NAME_LEN];
    char        szScriptName[MAX_PATH];

	KGLOG_PROCESS_ERROR(piTabFile);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_STAMINA, DefaultRecipe.nStamina, (int*)&(Recipe.nStamina));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_PROFESSION_ID, DefaultRecipe.dwProfessionID, (int*)&(Recipe.dwProfessionID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_REQUIRE_PROFESSION_LEVEL, DefaultRecipe.dwRequireProfessionLevel, (int*)&(Recipe.dwRequireProfessionLevel));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_TOOL_ITEM_TYPE, DefaultRecipe.dwToolItemType, (int*)&(Recipe.dwToolItemType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_TOOL_ITEM_INDEX, DefaultRecipe.dwToolItemIndex, (int*)&(Recipe.dwToolItemIndex));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_EQUIPMENT_TYPE, DefaultRecipe.nEquipmentType, (int*)&(Recipe.nEquipmentType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_EXP, DefaultRecipe.dwProfessionExp, (int*)&(Recipe.dwProfessionExp));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_PREPARE_FRAME, DefaultRecipe.nPrepareFrame, (int*)&(Recipe.nPrepareFrame));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, ENCHANT_RECIPE_ID, DefaultRecipe.dwID, (int*)&(Recipe.dwID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nLine, ENCHANT_NAME, DefaultRecipe.szName, Recipe.szName, MAX_RECIPE_NAME_LEN);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nLine, RECIPE_BELONG, DefaultRecipe.szBelong, Recipe.szBelong, MAX_BELONG_NAME_LEN);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, ENCHANT_ID, DefaultRecipe.dwEnchantID, (int*)&(Recipe.dwEnchantID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, ENCHANT_REQUIRE_BRANCH_ID, DefaultRecipe.dwRequireBranchID, (int*)&(Recipe.dwRequireBranchID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, ENCHANT_REQUIRE_DOODAD, DefaultRecipe.dwRequireDoodadID, (int*)&(Recipe.dwRequireDoodadID));
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 0; nIndex < MAX_MATERIAL; nIndex++)
	{
		snprintf(szTempColName, sizeof(szTempColName), "%s%d", ENCHANT_REQUIRE_ITEM_TYPE, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwRequireItemType[nIndex], (int*)&(Recipe.dwRequireItemType[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", ENCHANT_REQUIRE_ITEM_INDEX, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwRequireItemIndex[nIndex], (int*)&(Recipe.dwRequireItemIndex[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", ENCHANT_REQUIRE_ITEM_COUNT, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultRecipe.dwRequireItemCount[nIndex], (int*)&(Recipe.dwRequireItemCount[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);
	}

    Recipe.dwScriptID = 0;
    bRetCode = piTabFile->GetString(nLine, RECIPE_SCRIPT_NAME, "", szScriptName, sizeof(szScriptName));
    KGLOG_PROCESS_ERROR(bRetCode);

    if (szScriptName[0] != '\0')
    {
        Recipe.dwScriptID = g_FileNameHash(szScriptName);

        assert(Recipe.dwScriptID != 0);
    }

	bResult = true;
Exit0:
	return bResult;
}

template<>
KCraftEnchant* KRecipe<KCraftEnchant>::GetRecipe(DWORD dwID)
{
	KCraftEnchant* pResult = NULL;
	RECIPE_MAP::iterator it;

	it = m_mapRecipe.find(dwID);
	if (it != m_mapRecipe.end())
	{
		pResult = &(it->second);
	}
Exit0:
	return pResult;
}
//----------------------------------------------------------------------------------------------------------
template<>
BOOL KRecipe<KCraftCopy>::Init(const char* szFileName)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	int         nIndex      = 0;
	int         nHeight     = 0;
	ITabFile*   piTabFile   = NULL;
    int         nRecipeID   = 0;
    KCraftCopy  CraftCopy;
	KCraftCopy  DefaultCraftCopy;
	char        szFilePath[MAX_PATH];
    
    KGLOG_PROCESS_ERROR(szFileName);

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, szFileName);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);

	memset(&DefaultCraftCopy, 0, sizeof(DefaultCraftCopy));
	bRetCode = LoadLine(piTabFile, 2, DefaultCraftCopy, DefaultCraftCopy);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 3; nIndex <= nHeight; nIndex++)
	{
        ZeroMemory(&CraftCopy, sizeof(KCraftCopy));

		bRetCode = LoadLine(piTabFile, nIndex, CraftCopy, DefaultCraftCopy);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(CraftCopy.dwID < MAX_RECIPE_ID);

		CraftCopy.eCraftType = actCopy;
        nRecipeID = BookID2RecipeID(CraftCopy.dwID, CraftCopy.dwSubID);
        KGLOG_PROCESS_ERROR((nRecipeID >= 0) && "Error Copy Recipe ID\n");

		m_mapRecipe[nRecipeID] = CraftCopy;
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

template<>
BOOL KRecipe<KCraftCopy>::LoadLine(ITabFile* piTabFile, int nLine, KCraftCopy& Copy, const KCraftCopy& DefaultCopy)
{
    BOOL    bResult  = false;
	BOOL    bRetCode = false;
	int     nIndex = 0;
	char    szTempColName[_NAME_LEN];
	
    KGLOG_PROCESS_ERROR(piTabFile);
	
    bRetCode = piTabFile->GetInteger(nLine, RECIPE_ID, DefaultCopy.dwID, (int*)&(Copy.dwID));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, COPY_SUB_ID, DefaultCopy.dwSubID, (int*)&(Copy.dwSubID));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetString(nLine, RECIPE_NAME, DefaultCopy.szName, Copy.szName, MAX_RECIPE_NAME_LEN);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_THEW, DefaultCopy.nThew, (int*)&(Copy.nThew));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_PROFESSION_ID, DefaultCopy.dwProfessionID, (int*)&(Copy.dwProfessionID));
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, RECIPE_REQUIRE_PROFESSION_LEVEL, DefaultCopy.dwRequireProfessionLevel, (int*)&(Copy.dwRequireProfessionLevel));
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, RECIPE_PROFESSION_ID_EXT, DefaultCopy.dwProfessionIDExt, (int*)&(Copy.dwProfessionIDExt));
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, RECIPE_REQUIRE_PROFESSION_LEVEL_EXT, DefaultCopy.dwRequireProfessionLevelExt, (int*)&(Copy.dwRequireProfessionLevelExt));
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, RECIPE_TOOL_ITEM_TYPE, DefaultCopy.dwToolItemType, (int*)&(Copy.dwToolItemType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_TOOL_ITEM_INDEX, DefaultCopy.dwToolItemIndex, (int*)&(Copy.dwToolItemIndex));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_EQUIPMENT_TYPE, DefaultCopy.nEquipmentType, (int*)&(Copy.nEquipmentType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_EXP, DefaultCopy.dwProfessionExp, (int*)&(Copy.dwProfessionExp));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, RECIPE_PREPARE_FRAME, DefaultCopy.nPrepareFrame, (int*)&(Copy.nPrepareFrame));
	KGLOG_PROCESS_ERROR(bRetCode);

	for (nIndex = 0; nIndex < MAX_COPY_MATERIAL; nIndex++)
	{
		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_REQUIRE_ITEM_TYPE, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultCopy.dwRequireItemType[nIndex], (int*)&(Copy.dwRequireItemType[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_REQUIRE_ITEM_INDEX, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultCopy.dwRequireItemIndex[nIndex], (int*)&(Copy.dwRequireItemIndex[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);

		snprintf(szTempColName, sizeof(szTempColName), "%s%d", RECIPE_REQUIRE_ITEM_COUNT, nIndex + 1);
        szTempColName[sizeof(szTempColName) - 1] = '\0';

		bRetCode = piTabFile->GetInteger(nLine, szTempColName, DefaultCopy.dwRequireItemCount[nIndex], (int*)&(Copy.dwRequireItemCount[nIndex]));
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	bRetCode = piTabFile->GetInteger(nLine, "CreateItemType", DefaultCopy.dwCreateItemType, (int*)&(Copy.dwCreateItemType));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "CreateItemIndex", DefaultCopy.dwCreateItemIndex, (int*)&(Copy.dwCreateItemIndex));
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "CreateItemStackNum", DefaultCopy.nStackNum, &Copy.nStackNum);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "CoolDownID", DefaultCopy.dwCoolDownID, (int*)&(Copy.dwCoolDownID));
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(nLine, "DoodadTemplateID", DefaultCopy.dwDoodadTemplateID, (int*)&(Copy.dwDoodadTemplateID));
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(nLine, "RequirePlayerLevel", DefaultCopy.nRequirePlayerLevel, &Copy.nRequirePlayerLevel);
    KGLOG_PROCESS_ERROR(bRetCode);

    Copy.dwScriptID = 0;

	bResult = true;
Exit0:
	return bResult;
}

template<>
KCraftCopy* KRecipe<KCraftCopy>::GetRecipe(DWORD dwID)
{
	KCraftCopy* pResult = NULL;
	RECIPE_MAP::iterator it;

	it = m_mapRecipe.find(dwID);
	if (it != m_mapRecipe.end())
	{
		pResult = &(it->second);
	}
Exit0:
	return pResult;
}

template<>
DWORD KRecipe<KCraftCopy>::GetRecipeID(DWORD dwDoodadTemplateID)
{
    DWORD   dwResult    = 0;
    
    for (RECIPE_MAP::iterator it = m_mapRecipe.begin(); it != m_mapRecipe.end(); ++it)
    {
        if (it->second.dwDoodadTemplateID == dwDoodadTemplateID)
        {
            dwResult = it->first;
        }
    }

Exit0:
    return dwResult;
}

int BookID2RecipeID(DWORD dwBookID, DWORD dwSubID)
{
	int nResult = -1;

	KGLOG_PROCESS_ERROR(dwBookID >= 1 && dwBookID <= MAX_READ_BOOK_ID);
	KGLOG_PROCESS_ERROR(dwSubID >= 1 && dwSubID <= MAX_READ_BOOK_SUB_ID);

	nResult = (int)(((dwBookID - 1) << MAX_READ_BOOK_SUB_ID_BIT) | (dwSubID - 1));
Exit0:
	return nResult;
}

BOOL RecipeID2BookID(DWORD dwRecipeID, DWORD& dwBookID, DWORD& dwSubID)
{
	BOOL bResult = false;

	dwBookID = (dwRecipeID >> MAX_READ_BOOK_SUB_ID_BIT) + 1;
	dwSubID = (dwRecipeID % (1 << MAX_READ_BOOK_SUB_ID_BIT)) + 1;
    
    KGLOG_PROCESS_ERROR(dwBookID >= 1 && dwBookID <= MAX_READ_BOOK_ID);
	KGLOG_PROCESS_ERROR(dwSubID >= 1 && dwSubID <= MAX_READ_BOOK_SUB_ID);

	bResult = true;
Exit0:
	return bResult;
}


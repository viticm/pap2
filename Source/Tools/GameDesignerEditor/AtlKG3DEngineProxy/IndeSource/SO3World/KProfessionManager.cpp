#include "stdafx.h"

#include "KProfessionManager.h"
#include "KProfession.h"
#include "KCraft.h"
#include "KRecipeMaster.h"

#define PROFESSION_FILE_NAME		"profession.tab"
#define PROFESSION_ID				"ID"
#define PROFESSION_NAME				"Name"
#define PROFESSION_RADAR_ID			"RadarID"
#define PROFESSION_EXP_TAB_PATH		"ExpTab"	
#define PROFESSION_PRICE1			"Price1"
#define PROFESSION_PRICE2			"Price2"
#define PROFESSION_PRICE3			"Price3"
#define PROFESSION_BRANCH_CRAFT		"BranchCraft"


#define CRAFT_FILE_NAME				"craft.tab"
#define CRAFT_ID					"ID"
#define CRAFT_NAME					"Name"
#define CRAFT_TYPE					"CraftType"
#define CRAFT_RECIPE_PATH			"RecipeTabPath"
#define CRAFT_PROFESSION_ID			"ProfessionID"
#define CRAFT_HIDE					"Hide"

#define CRAFT_MASTER_FILE_NAME		"Master.tab"

KProfessionManager::KProfessionManager(void)
{
	m_pProfession = NULL;
}

KProfessionManager::~KProfessionManager(void)
{
}

BOOL KProfessionManager::Init(void)
{
	BOOL bResult    = false;
    BOOL bRetCode   = false;

	m_pProfession = NULL;

	bRetCode = LoadProfessions();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = LoadCraft();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = LoadMasterTable();
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

void KProfessionManager::UnInit(void)
{
    SAFE_DELETE_ARRAY(m_pProfession);
	return;
}

BOOL KProfessionManager::LoadProfessions()
{
    BOOL            bResult                 = false;
	BOOL            bRetCode                = false;
	int             nHeight                 = 0;
	DWORD           dwProfessionID          = 0;
	ITabFile*       pProfessionFile         = NULL;
	KProfession*    pProfession             = NULL;
	char            szFilePath[MAX_PATH];
	char            szExpTabPath[MAX_PATH];
	char            szBranchCraftPath[MAX_PATH];

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, PROFESSION_FILE_NAME);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	pProfessionFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(pProfessionFile);

	nHeight = pProfessionFile->GetHeight() - 1;
	KGLOG_PROCESS_ERROR(nHeight >= 0);

	m_pProfession = new KProfession[nHeight];
	KGLOG_PROCESS_ERROR(m_pProfession);

	pProfession = m_pProfession;

	for(int nIndex = 1; nIndex <= nHeight; ++nIndex)
	{
		ZeroMemory(pProfession, sizeof(KProfession));

		bRetCode = pProfessionFile->GetInteger(nIndex + 1, PROFESSION_ID, 0, (int*)(&dwProfessionID));
		KGLOG_PROCESS_ERROR(bRetCode); 
		KGLOG_PROCESS_ERROR(dwProfessionID < MAX_PROFESSION_COUNT);

		bRetCode = pProfessionFile->GetString(nIndex + 1, PROFESSION_NAME, "", pProfession->m_szProfessionName, PROFESSION_NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);
        pProfession->m_szProfessionName[PROFESSION_NAME_LEN - 1] = '\0';

		bRetCode = pProfessionFile->GetInteger(nIndex + 1, PROFESSION_RADAR_ID, 0, (int*)&pProfession->m_dwCraftRadarID);
		KGLOG_PROCESS_ERROR(bRetCode);
			
		bRetCode = pProfessionFile->GetInteger(nIndex + 1, PROFESSION_PRICE1, 0, (int*)&pProfession->m_nPrice[0]);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pProfessionFile->GetInteger(nIndex + 1, PROFESSION_PRICE2, 0, (int*)&pProfession->m_nPrice[1]);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pProfessionFile->GetInteger(nIndex + 1, PROFESSION_PRICE3, 0, (int*)&pProfession->m_nPrice[2]);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pProfessionFile->GetString(nIndex + 1, PROFESSION_EXP_TAB_PATH, "", szExpTabPath, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);
        szExpTabPath[MAX_PATH - 1] = '\0';

		bRetCode = pProfessionFile->GetString(nIndex + 1, PROFESSION_BRANCH_CRAFT, "", szBranchCraftPath, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);
        szBranchCraftPath[MAX_PATH - 1] = '\0';

		bRetCode = pProfession->LoadFromFile(szExpTabPath);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (szBranchCraftPath[0] != '\0')
		{
			bRetCode = pProfession->LoadBranchData(szBranchCraftPath);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		m_Professions[dwProfessionID] = pProfession;
		++pProfession;
	}

	bResult = true;
Exit0:
	if (!bResult)
	{
        SAFE_DELETE_ARRAY(m_pProfession);
	}

	KG_COM_RELEASE(pProfessionFile);
	return bResult;
}

BOOL KProfessionManager::LoadCraft()
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	int         nHeight     = 0; 
	ITabFile*   pCraftFile  = NULL;
	char        szFilePath[MAX_PATH];
	char        szRecipePath[MAX_PATH];
   	KCraft      Craft;

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, CRAFT_FILE_NAME);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	pCraftFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(pCraftFile);

	nHeight = pCraftFile->GetHeight() - 1;
	KGLOG_PROCESS_ERROR(nHeight >= 0);

	for (int nIndex = 1; nIndex <= nHeight; nIndex++)
	{
		bRetCode = pCraftFile->GetInteger(nIndex + 1, CRAFT_ID, 0, (int*)&(Craft.m_dwID));
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(Craft.m_dwID < MAX_CRAFT_COUNT);

		bRetCode = pCraftFile->GetString(nIndex + 1, CRAFT_NAME, "", Craft.m_szName, MAX_CRAFT_NAME_LEN);
		KGLOG_PROCESS_ERROR(bRetCode);
		
		bRetCode = pCraftFile->GetInteger(nIndex +  1, CRAFT_TYPE, 0, (int*)&(Craft.m_eCraftType));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pCraftFile->GetInteger(nIndex + 1, CRAFT_PROFESSION_ID, 0, (int*)&(Craft.m_dwProfessionID));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pCraftFile->GetInteger(nIndex + 1, CRAFT_HIDE, 0, (int*)&(Craft.m_bHide));
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pCraftFile->GetString(nIndex + 1, CRAFT_RECIPE_PATH, "", szRecipePath, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (Craft.m_dwID > 0)
		{
			m_Craft[Craft.m_dwID] = Craft;
			bRetCode = m_Craft[Craft.m_dwID].Init(szRecipePath);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else
		{
			KGLOG_PROCESS_ERROR(false && "Craft ID <= 0!");
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(pCraftFile);
	return bResult;
}

KProfession* KProfessionManager::GetProfession(DWORD dwProfessionID)
{
	PROFESSION_LIST::iterator it;
	KProfession* pKProfession = NULL;

	KGLOG_PROCESS_ERROR(dwProfessionID < MAX_PROFESSION_COUNT);

	it = m_Professions.find(dwProfessionID);
	KGLOG_PROCESS_ERROR(it != m_Professions.end());
	pKProfession = it->second;

Exit0:
	return pKProfession;
}

char* KProfessionManager::GetProfessionName(DWORD dwProfessionID)
{
	PROFESSION_LIST::iterator it;
	KProfession* pKProfession = NULL;

	KGLOG_PROCESS_ERROR(dwProfessionID < MAX_PROFESSION_COUNT);

	it = m_Professions.find(dwProfessionID);
	KGLOG_PROCESS_ERROR(it != m_Professions.end());
	pKProfession = it->second;

	return pKProfession->m_szProfessionName;
Exit0:
	return 0;
}

DWORD KProfessionManager::GetProfessionCount()
{
	return (DWORD)m_Professions.size();
}

//
//
//出错返回-1
//
int KProfessionManager::GetLevelProficiency(DWORD dwProfessionID, DWORD dwLevel)
{
	PROFESSION_LIST::iterator it;
	KProfession* pKProfession = NULL;

	KGLOG_PROCESS_ERROR(dwProfessionID < MAX_PROFESSION_COUNT);
	KGLOG_PROCESS_ERROR(dwLevel <= PROFESSION_MAX_LEVEL);

	it = m_Professions.find(dwProfessionID);
	KGLOG_PROCESS_ERROR(it != m_Professions.end());
	pKProfession = it->second;
	
	return pKProfession->m_nMaxProficiencyPerLevel[dwLevel];

Exit0:
	return -1;
}

BOOL KProfessionManager::IsProfessionExist(DWORD dwProfessionID)
{
	PROFESSION_LIST::iterator it;

	it = m_Professions.find(dwProfessionID);
	KGLOG_PROCESS_ERROR(it != m_Professions.end());

	return true;
Exit0:
	return false;
}

BOOL KProfessionManager::GetCraftIDList(DWORD dwProfessionID, std::vector<DWORD>& vecCraftIDList)
{
	BOOL    bResult = false;
	KCraft* pCraft  = NULL;
    CRAFT_LIST::iterator iter;

	KGLOG_PROCESS_ERROR(dwProfessionID < MAX_PROFESSION_COUNT);
		
	for (iter = m_Craft.begin(); iter != m_Craft.end(); ++iter)
	{
		pCraft = &(iter->second);

		if (pCraft->m_dwProfessionID == dwProfessionID)
        {
			vecCraftIDList.push_back(pCraft->m_dwID);
        }
	}
	
	bResult = true;
Exit0:
	return bResult;
}

KCraft* KProfessionManager::GetCraft(DWORD dwCraftID)
{
	KCraft* pResult = NULL;
	CRAFT_LIST::iterator it;

	it = m_Craft.find(dwCraftID);
	if (it != m_Craft.end())
		pResult = &(it->second);
Exit0:
	return pResult;
}

BOOL KProfessionManager::IsCraftExist(DWORD dwCraftID)
{
	BOOL bResult = false;
	KCraft* pResult = NULL;
	
	pResult = GetCraft(dwCraftID);
	KGLOG_PROCESS_ERROR(pResult);

	bResult = true;
Exit0:
	return bResult;
	
}

BOOL KProfessionManager::IsRecipeExist(DWORD dwCraftID, DWORD dwRecipeID)
{
    BOOL     bResult    = false;
	BOOL     bRetCode   = false;
	KCraft*  pCraft     = NULL;

	pCraft = GetCraft(dwCraftID);
	KGLOG_PROCESS_ERROR(pCraft);

	bRetCode = pCraft->IsRecipeExist(dwRecipeID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	return bResult;
}

KRecipeBase* KProfessionManager::GetRecipe(DWORD dwCraftID, DWORD dwRecipeID)
{
	KRecipeBase* pResult = NULL;
	KCraft* pCraft = NULL;

	pCraft = GetCraft(dwCraftID);
	KGLOG_PROCESS_ERROR(pCraft);

	pResult = pCraft->GetRecipe(dwRecipeID);
	KGLOG_PROCESS_ERROR(pResult);

Exit0:
	return pResult;
}

//加载技能训练师表
BOOL KProfessionManager::LoadMasterTable()
{
    BOOL        bResult     = false;
	BOOL		bRetCode	= false;
	int			nIndex		= 0;
	int			nHeight		= 0;
	DWORD		nMasterID	= 0;
	ITabFile*   piTabFile	= NULL;
	char		szRecipeFilePath[MAX_PATH];
	char		szFilePath[MAX_PATH];

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, CRAFT_MASTER_FILE_NAME);
    szFilePath[sizeof(szFilePath) - 1] = '\0';    

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 1);

	for (nIndex = 2; nIndex <= nHeight; nIndex++)
	{
		KRecipeMaster RecipeMaster;
		std::pair<RECIPE_MASTER_LIST::iterator, bool> InsRet;

		bRetCode = piTabFile->GetInteger(nIndex, "ID", 0, (int*)&nMasterID);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(nMasterID > 0);

		bRetCode = piTabFile->GetString(nIndex, "TabFile", "", szRecipeFilePath, MAX_PATH);
		KGLOG_PROCESS_ERROR(bRetCode);

		InsRet = m_RecipeMasterList.insert(std::make_pair(nMasterID, RecipeMaster));
		KGLOG_PROCESS_ERROR(InsRet.second);

		bRetCode = InsRet.first->second.Init(nMasterID, szRecipeFilePath);
		KGLOG_PROCESS_ERROR(bRetCode);
	}
		
	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

KRecipeMaster* KProfessionManager::GetRecipeMaster(DWORD dwMasterID)
{
	KRecipeMaster* pResult = NULL;
	RECIPE_MASTER_LIST::iterator iter;

	iter = m_RecipeMasterList.find(dwMasterID);
	if (iter != m_RecipeMasterList.end())
	{
		pResult = &(iter->second);
	}

	return pResult;
}

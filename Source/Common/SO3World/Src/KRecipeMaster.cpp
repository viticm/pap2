#include "stdafx.h"
#include "KRecipeMaster.h"

KRecipeMaster::KRecipeMaster(void)
{
	m_dwMasterID = 0;
	m_mapRecipeMaster.clear();
}

KRecipeMaster::~KRecipeMaster(void)
{
}

BOOL KRecipeMaster::Init(DWORD dwMasterID, char szFileName[])
{
    BOOL        bResult     = false;
	BOOL		bRetCode	= false;
	int			nIndex		= 0;
	int			nHeight		= 0;
	DWORD		dwCraftID	= 0;
	DWORD		dwRecipeID	= 0;
	ITabFile*   piTabFile	= NULL;
    int64_t		nRecipeKey	= 0;
    char		szFilePath[MAX_PATH];
	KRecipeLearningInfo LearnInfo;
    KRecipeLearningInfo DefaultLearningInfo;

	KGLOG_PROCESS_ERROR(dwMasterID);
	KGLOG_PROCESS_ERROR(szFileName);
	KG_PROCESS_ERROR(szFileName[0] != '\0');

	m_dwMasterID = dwMasterID;

	snprintf(szFilePath, sizeof(szFilePath), "%s/Craft/%s", SETTING_DIR, szFileName);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = (int)piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);
    
    ZeroMemory(&DefaultLearningInfo, sizeof(DefaultLearningInfo));
	LoadLine(2, piTabFile, DefaultLearningInfo, DefaultLearningInfo);

	for (nIndex = 3; nIndex <= nHeight; nIndex++)
	{
        ZeroMemory(&LearnInfo, sizeof(LearnInfo));
		bRetCode = LoadLine(nIndex, piTabFile, DefaultLearningInfo, LearnInfo);
		KGLOG_PROCESS_ERROR(bRetCode);
		KGLOG_PROCESS_ERROR(LearnInfo.dwCraftID);
		KGLOG_PROCESS_ERROR(LearnInfo.dwRecipeID);

		nRecipeKey = MAKE_INT64(LearnInfo.dwCraftID, LearnInfo.dwRecipeID);

		m_mapRecipeMaster[nRecipeKey] = LearnInfo;
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

void KRecipeMaster::UnInit()
{
	m_mapRecipeMaster.clear();
	return;
}

BOOL KRecipeMaster::LoadLine(int nLine, ITabFile* piTabFile, KRecipeLearningInfo& DefaultLearningInfo, KRecipeLearningInfo& LearnInfo)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	KGLOG_PROCESS_ERROR(piTabFile);
	KGLOG_PROCESS_ERROR(nLine > 0);

	bRetCode = piTabFile->GetInteger(nLine, "CraftID", DefaultLearningInfo.dwCraftID, (int*)&LearnInfo.dwCraftID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "RecipeID", DefaultLearningInfo.dwRecipeID, (int*)&LearnInfo.dwRecipeID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ProfessionID", DefaultLearningInfo.dwProfessionID, (int*)&LearnInfo.dwProfessionID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ProfessionLevel", DefaultLearningInfo.nProfessionLevel, (int*)&LearnInfo.nProfessionLevel);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "BranchID", DefaultLearningInfo.dwBranchID, (int*)&LearnInfo.dwBranchID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "ReputationID", DefaultLearningInfo.dwReputationID, (int*)&LearnInfo.dwReputationID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "nReputationLevel", DefaultLearningInfo.nReputationLevel, (int*)&LearnInfo.nReputationLevel);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nLine, "nPrice", DefaultLearningInfo.nPrice, (int*)&LearnInfo.nPrice);
	KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(LearnInfo.nPrice >= 0);

	bResult = true;
Exit0:
	return bResult;
}

void KRecipeMaster::GetLearnInfoList(std::vector<KRecipeLearningInfo>& vecLearnInfoList)
{
	KRECIPE_MASTER::iterator iter;

	vecLearnInfoList.clear();

	for (iter = m_mapRecipeMaster.begin(); iter != m_mapRecipeMaster.end(); iter++)
	{
		vecLearnInfoList.push_back(iter->second);
	}
}

KRecipeLearningInfo*  KRecipeMaster::GetLearnInfo(DWORD dwCraftID, DWORD dwRecipeID)
{
	KRecipeLearningInfo*	pResult		= NULL;
	int64_t					nRecipeKey	= 0;
	KRECIPE_MASTER::iterator iter;

	KGLOG_PROCESS_ERROR(dwCraftID);
	KGLOG_PROCESS_ERROR(dwRecipeID);

	nRecipeKey = MAKE_INT64(dwCraftID, dwRecipeID);

	iter = m_mapRecipeMaster.find(nRecipeKey);
	if (iter != m_mapRecipeMaster.end())
	{
		pResult = &(iter->second);
	}

Exit0:
	return pResult;
}

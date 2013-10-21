#include "stdafx.h"
#include "KSmartDialogList.h"
#include "KSO3World.h"
#include "KNpc.h"
#include "KTarget.h"
#include "KPlayerServer.h"

#define MAX_DIALOG_RATE_PER_ACTION	1000
#define MAX_COL_NAME_LEN			64
#define TAB_FILE_NAME				"SmartDialog.tab"

#define SMART_DIALOG_ID				"ID"			
#define SMART_DIALOG_TEXT			"Text"		
#define SMART_DIALOG_TYPE			"Type"
#define SMART_DIALOG_RATE			"Rate"

char szActionTypeName[ntatTotal][MAX_COL_NAME_LEN] = {"TurnToFight", "Death", "TeammateDeath", "Idle", "Custom", "TurnToIdle", "KillEnemy", "Escape"};

BOOL KSmartDialogList::Init()
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	int nHeight = 0;
	int nIndex1 = 0;
	int nIndex2 = 0;
	int nLine = 0;
	int nMaxRate = 0;
	BOOL bInsertFlag = false;;
	int64_t nKey = 0;
	ITabFile* piTabFile = NULL;
	char szFilePath[MAX_PATH];
	char szColName[MAX_COL_NAME_LEN];
	KNpcDialog NpcDialog;
	KSMART_DIALOG_LIST::iterator iter;

	m_mapSmartDialogList.clear();

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, TAB_FILE_NAME);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 1);

	for (nLine = 2; nLine <= nHeight; nLine++)
	{
		bRetCode = piTabFile->GetInteger(nLine, SMART_DIALOG_ID, 0, (int*)&(NpcDialog.nIndex));
		KGLOG_PROCESS_ERROR(bRetCode);

		for (nIndex1 = 0; nIndex1 < ntatTotal; nIndex1++)
		{
			NpcDialog.eNpcTalkAction = (NPC_TALK_ACTION_TYPE)nIndex1;
			bInsertFlag = false;
			nMaxRate = 0;

			for (nIndex2 = 0; nIndex2 < MAX_DIALOG_PER_ACTION; nIndex2++)
			{
				snprintf(szColName, sizeof(szColName), "%s_%s_%d", szActionTypeName[nIndex1], SMART_DIALOG_TEXT, nIndex2 + 1);
                szColName[sizeof(szColName) - 1] = '\0';

				bRetCode = piTabFile->GetString(nLine, szColName, "", NpcDialog.szDialogText[nIndex2], MAX_NPC_SENTENCE_LEN);
				KGLOG_PROCESS_ERROR(bRetCode);

				snprintf(szColName, sizeof(szColName), "%s_%s_%d", szActionTypeName[nIndex1], SMART_DIALOG_TYPE, nIndex2 + 1);
                szColName[sizeof(szColName) - 1] = '\0';

				bRetCode = piTabFile->GetInteger(nLine, szColName, 1, (int*)&(NpcDialog.eDialogType[nIndex2]));
				KGLOG_PROCESS_ERROR(bRetCode);

				snprintf(szColName, sizeof(szColName), "%s_%s_%d", szActionTypeName[nIndex1], SMART_DIALOG_RATE, nIndex2 + 1);
                szColName[sizeof(szColName) - 1] = '\0';

				bRetCode = piTabFile->GetInteger(nLine, szColName, 0, (int*)&(NpcDialog.nDialogRate[nIndex2]));
				KGLOG_PROCESS_ERROR(bRetCode);

				nMaxRate += NpcDialog.nDialogRate[nIndex2];

				if (NpcDialog.szDialogText[nIndex2][0] != '\0' && NpcDialog.nDialogRate[nIndex2] != 0)
					bInsertFlag = true;
			}

			KGLOG_PROCESS_ERROR(nMaxRate <= MAX_DIALOG_RATE_PER_ACTION);

			if (bInsertFlag)
			{
				nKey = MAKE_INT64(NpcDialog.nIndex, nIndex1);
				m_mapSmartDialogList[nKey] = NpcDialog;
			}
		}
	}

	bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

BOOL KSmartDialogList::UnInit()
{
	m_mapSmartDialogList.clear();
	return true;
}

#ifdef _SERVER
BOOL KSmartDialogList::FireEvent(KNpc* pNpc, NPC_TALK_ACTION_TYPE eAction, KTarget& rTarget)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
	KNpcTemplate* pNpcTemplate = NULL;
	KNpcDialog* pNpcDialog = NULL;
	int64_t nKey = 0;
	int nIndex = 0;
	int nRandom = 0;
	KSMART_DIALOG_LIST::iterator iter;

	KGLOG_PROCESS_ERROR(pNpc);
	KGLOG_PROCESS_ERROR(eAction < ntatTotal);

	pNpcTemplate = pNpc->m_pTemplate;
	if (pNpcTemplate == NULL)
	{
		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(pNpc->m_dwTemplateID);
		KGLOG_PROCESS_ERROR(pNpcTemplate);
	}
	
	//ID为0表示没有可以说的话
	KG_PROCESS_SUCCESS(pNpcTemplate->nNpcDialogID == 0);

	//从Npc说话列表中取一句话
	nKey = MAKE_INT64(pNpcTemplate->nNpcDialogID, (int)eAction);
	
	iter = m_mapSmartDialogList.find(nKey);
	if (iter != m_mapSmartDialogList.end())
	{
		pNpcDialog = &(iter->second);

		nRandom = g_Random(MAX_DIALOG_RATE_PER_ACTION);
		
		for (nIndex = 0; nIndex < MAX_DIALOG_PER_ACTION; nIndex++)
		{
			if (pNpcDialog->nDialogRate[nIndex] > 0 && nRandom < pNpcDialog->nDialogRate[nIndex])
				break;
			else
				nRandom -= pNpcDialog->nDialogRate[nIndex];
		}

		if (nIndex < MAX_DIALOG_PER_ACTION && pNpcDialog->szDialogText[nIndex][0] != '\0')
		{
			g_PlayerServer.DoNpcTalkToPlayer(pNpc, pNpcTemplate->nNpcDialogID, eAction, 
										nIndex, pNpcDialog->eDialogType[nIndex], rTarget);
		}
	}

Exit1:
	bResult = true;
Exit0:
	return bResult;
}
#endif //_SERVER


BOOL KSmartDialogList::GetDialogKey(NPC_TALK_ACTION_TYPE eAction, DWORD dwIndex, char szKey[], unsigned uLength)
{
    BOOL bResult  = false;

    KGLOG_PROCESS_ERROR(eAction >= 0);
    KGLOG_PROCESS_ERROR(eAction < ntatTotal);
    KGLOG_PROCESS_ERROR(dwIndex >= 0);
    KGLOG_PROCESS_ERROR(dwIndex < MAX_DIALOG_PER_ACTION);
    KGLOG_PROCESS_ERROR(szKey);
	KGLOG_PROCESS_ERROR(uLength > 0);

    snprintf(szKey, uLength, "%s_%s_%ld", szActionTypeName[eAction], SMART_DIALOG_TEXT, dwIndex + 1);
    szKey[uLength - 1] = '\0';

    bResult = true;
Exit0:
    return bResult;
}


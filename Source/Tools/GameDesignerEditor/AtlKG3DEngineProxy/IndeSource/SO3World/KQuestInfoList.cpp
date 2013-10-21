#include "stdafx.h"
#include "KQuestInfoList.h"
#include "Global.h"

#define QUEST_FILE_NAME	"Quests.tab"

BOOL KQuestInfoList::Init(void)
{
    BOOL        bResult     = false;
    int         nRetCode    = false;
    int         nHeight     = 0;
    ITabFile*   piTabFile   = NULL;
	char        szFilePath[MAX_PATH];
    std::pair<MAP_ID_2_QUEST_INFO::iterator, bool> InsRet;

	nRetCode = (int)snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, QUEST_FILE_NAME);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szFilePath));

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight > 1);

    memset(&m_DefaultQuestInfo, 0, sizeof(m_DefaultQuestInfo));

	nRetCode = LoadQuestInfo(piTabFile, 2, &m_DefaultQuestInfo);
	KGLOG_PROCESS_ERROR(nRetCode);

	for (int nIndex = 3; nIndex <= nHeight; nIndex++)
	{
		KQuestInfo	TempQuestInfo;

		nRetCode = LoadQuestInfo(piTabFile, nIndex, &TempQuestInfo);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = MakeQuestMap(&TempQuestInfo);
        KGLOG_PROCESS_ERROR(nRetCode);

#ifdef _SERVER
        InsRet = m_mapID2QuestInfo.insert(std::make_pair(TempQuestInfo.dwQuestID, TempQuestInfo));
        KGLOG_PROCESS_ERROR(InsRet.second);
#endif
	}

    bResult = true;
Exit0:
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

void KQuestInfoList::UnInit()
{
}

KQuestInfo* KQuestInfoList::GetQuestInfo(DWORD dwQuestID)
{
    KQuestInfo* pResult     = NULL;
	BOOL        bRetCode    = false;
    ITabFile*   piTabFile   = NULL;
    int         nHeight     = 0;
    KQuestInfo  TempQuest;
    char        szFilePath[MAX_PATH];

	MAP_ID_2_QUEST_INFO::iterator it;
    std::pair<MAP_ID_2_QUEST_INFO::iterator, bool> InsRet;

    KGLOG_PROCESS_ERROR(dwQuestID > 0);

	it = m_mapID2QuestInfo.find(dwQuestID);

    if (it != m_mapID2QuestInfo.end())
    {
        pResult = &(it->second);
        goto Exit0;
    }
	
    snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, QUEST_FILE_NAME);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    piTabFile = g_OpenTabFile(szFilePath);
    KGLOG_PROCESS_ERROR(piTabFile);

    nHeight = piTabFile->GetHeight();
    KGLOG_PROCESS_ERROR(nHeight > 1);

    bRetCode = LoadQuestInfoByID(piTabFile, dwQuestID, &TempQuest);
    KGLOG_PROCESS_ERROR(bRetCode);

    InsRet = m_mapID2QuestInfo.insert(std::make_pair(dwQuestID, TempQuest));
    KGLOG_PROCESS_ERROR(InsRet.second);

    pResult = &((InsRet.first)->second);
Exit0:
    KG_COM_RELEASE(piTabFile);
	return pResult;
}

int KQuestInfoList::GetNpcQuestString(
    DWORD dwMapID, DWORD dwNpcTemplateID, char* pszBuffer, size_t uBufferSize
)
{
	int                             nResult     = 0;
    int                             nRetCode    = 0;
    char*                           pszPos      = pszBuffer;
    size_t                          uLeftSize   = uBufferSize;
    MAP_NPC_2_QUEST_INFO::iterator  it;
    MAP_NPC_2_QUEST_INFO::iterator  itUpper;

	assert(pszBuffer);

    it = m_mapNpc2QuestInfo.lower_bound(dwNpcTemplateID);
    itUpper = m_mapNpc2QuestInfo.upper_bound(dwNpcTemplateID);
	for (;it != itUpper; ++it)
	{
		nRetCode = (int)snprintf(pszPos, uLeftSize, "<Q%lu>", it->second);
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)uLeftSize);

        uLeftSize -= nRetCode;
        pszPos    += nRetCode;
	}

    nResult = (int)(uBufferSize - uLeftSize);
Exit0:
	return nResult;
}

BOOL KQuestInfoList::GetNpcQuest(DWORD dwNpcTemplateID, std::vector<DWORD>* pvQuestID)
{
	BOOL bResult = false;
    MAP_NPC_2_QUEST_INFO::iterator it;
    MAP_NPC_2_QUEST_INFO::iterator itUpper;

    assert(pvQuestID);

	pvQuestID->clear();
    
    it = m_mapNpc2QuestInfo.lower_bound(dwNpcTemplateID);
    itUpper = m_mapNpc2QuestInfo.upper_bound(dwNpcTemplateID);
	for (;it != itUpper; ++it)
	{
		pvQuestID->push_back(it->second);
	}

    bResult = true;
Exit0:
	return bResult;
}

BOOL KQuestInfoList::IsNpcQuestExist(DWORD dwNpcTemplateID)
{
    BOOL                           bResult = false;
    MAP_NPC_2_QUEST_INFO::iterator ItLower;
    MAP_NPC_2_QUEST_INFO::iterator ItUpper;

    ItLower = m_mapNpc2QuestInfo.lower_bound(dwNpcTemplateID);
    ItUpper = m_mapNpc2QuestInfo.upper_bound(dwNpcTemplateID);
    KG_PROCESS_ERROR(ItLower != ItUpper);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestInfoList::LoadQuestInfoByID(ITabFile* piTabFile, DWORD dwQuestID, KQuestInfo* pQuestInfo)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nIndex     = 0;
    char szRowName[MAX_PATH];

    assert(piTabFile);
    assert(pQuestInfo);

    snprintf(szRowName, sizeof(szRowName), "%lu", dwQuestID);
    szRowName[sizeof(szRowName) - 1] = '\0';

    nIndex = piTabFile->FindRow(szRowName);
    KGLOG_PROCESS_ERROR(nIndex > 1); // 寻找到的信息肯定是在2行之后。

    bRetCode = LoadQuestInfo(piTabFile, nIndex, pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestInfoList::LoadQuestInfo(ITabFile* piTabFile, int nIndex, KQuestInfo* pQuestInfo)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;
    char szColName[64];

	bRetCode = piTabFile->GetInteger(
        nIndex, "QuestID",
        m_DefaultQuestInfo.dwQuestID, (int*)&(pQuestInfo->dwQuestID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, "QuestName",
        m_DefaultQuestInfo.szName, pQuestInfo->szName, QUEST_NAME_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
#ifdef _CLIENT
	bRetCode = piTabFile->GetString(
        nIndex, "Objective",
        m_DefaultQuestInfo.szObjective, pQuestInfo->szObjective, QUEST_OBJECTIVE_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, "Description",
        m_DefaultQuestInfo.szDescription, pQuestInfo->szDescription, QUEST_DESCRIPTION_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
	bRetCode = piTabFile->GetString(
        nIndex, "DunningDialogue",
		m_DefaultQuestInfo.szDunningDialogue, pQuestInfo->szDunningDialogue, QUEST_UNFINISHED_DIALOGUE_LEN
    );

	bRetCode = piTabFile->GetString(
        nIndex, "UnfinishedDialogue", 
		m_DefaultQuestInfo.szUnfinishedDialogue, pQuestInfo->szUnfinishedDialogue, QUEST_UNFINISHED_DIALOGUE_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, "FinishedDialogue", 
		m_DefaultQuestInfo.szFinishedDialogue, pQuestInfo->szFinishedDialogue, QUEST_FINISHED_DIALOGUE_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

	bRetCode = piTabFile->GetString(
        nIndex, "Class",
        m_DefaultQuestInfo.szQuestClass, pQuestInfo->szQuestClass, QUEST_NAME_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "CanShare",
        m_DefaultQuestInfo.bShare, (int*)&(pQuestInfo->bShare)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "CanRepeat",
        m_DefaultQuestInfo.bRepeat, (int*)&(pQuestInfo->bRepeat)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "CanAssist",
        m_DefaultQuestInfo.bAssist, (int*)&(pQuestInfo->bAssist)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "NeedAccept",
        m_DefaultQuestInfo.bAccept, (int*)&(pQuestInfo->bAccept)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "IsEscortQuest",
        m_DefaultQuestInfo.bEscortQuest, (int*)&(pQuestInfo->bEscortQuest)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    if (pQuestInfo->bEscortQuest && pQuestInfo->bShare)
    {
        KGLogPrintf(KGLOG_WARNING, "Escort quest can't be share! Quest ID : %lu \n", pQuestInfo->dwQuestID);
    }

	bRetCode = piTabFile->GetString(
        nIndex, "Difficulty",
        m_DefaultQuestInfo.szQuestDiff, pQuestInfo->szQuestDiff, QUEST_NAME_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, "QuestLevel",
        m_DefaultQuestInfo.nLevel, (int*)&(pQuestInfo->nLevel)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "StartMapID",
        m_DefaultQuestInfo.dwStartMapID, (int*)&(pQuestInfo->dwStartMapID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "StartNpcTemplateID",
        m_DefaultQuestInfo.dwStartNpcTemplateID, (int*)&(pQuestInfo->dwStartNpcTemplateID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "StartDoodadTemplateID",
        m_DefaultQuestInfo.dwStartDoodadTemplateID, (int*)&(pQuestInfo->dwStartDoodadTemplateID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "StartItemType",
        m_DefaultQuestInfo.dwStartItemType, (int*)&(pQuestInfo->dwStartItemType)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "StartItemIndex",
        m_DefaultQuestInfo.dwStartItemIndex, (int*)&(pQuestInfo->dwStartItemIndex)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
	bRetCode = piTabFile->GetInteger(
        nIndex, "StartGossipType",
		m_DefaultQuestInfo.nStartGossipType, (int*)&(pQuestInfo->nStartGossipType)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
    {
        sprintf(szColName, "StartGossip%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';
        
        bRetCode = piTabFile->GetString(
            nIndex, szColName, 
		    m_DefaultQuestInfo.szStartGossip[i], pQuestInfo->szStartGossip[i], QUEST_GOSSIP_LEN
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }
#endif

	bRetCode = piTabFile->GetInteger(
        nIndex, "MinLevel",
        m_DefaultQuestInfo.nMinLevel, (int*)&(pQuestInfo->nMinLevel)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "MaxLevel",
        m_DefaultQuestInfo.nMaxLevel, (int*)&(pQuestInfo->nMaxLevel)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "RequireGender",
        m_DefaultQuestInfo.nRequireGender, (int*)&(pQuestInfo->nRequireGender)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "RequireRoleType",
        m_DefaultQuestInfo.nRequireRoleType, (int*)&(pQuestInfo->nRequireRoleType)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "CooldownID",
        m_DefaultQuestInfo.dwCoolDownID, (int*)&(pQuestInfo->dwCoolDownID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "PrequestLogic",
        m_DefaultQuestInfo.bPrequestLogic, (int*)&(pQuestInfo->bPrequestLogic)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
    {
        sprintf(szColName, "PrequestID%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';
        
        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
            m_DefaultQuestInfo.dwPrequestID[i], (int*)&(pQuestInfo->dwPrequestID[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    bRetCode = piTabFile->GetInteger(
        nIndex, "SubsequenceID", 
		m_DefaultQuestInfo.dwSubsequenceID, (int*)&(pQuestInfo->dwSubsequenceID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "StartTime",
		m_DefaultQuestInfo.nStartTime, (int*)&(pQuestInfo->nStartTime)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "EndTime",
		m_DefaultQuestInfo.nEndTime, (int*)&(pQuestInfo->nEndTime)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "RequireSkillID",
		m_DefaultQuestInfo.dwRequireSkillID, (int*)&(pQuestInfo->dwRequireSkillID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "MinSkillLevel",
		m_DefaultQuestInfo.dwMinSkillLevel, (int*)&(pQuestInfo->dwMinSkillLevel)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "RequireLifeSkillID",
		m_DefaultQuestInfo.dwRequireLifeSkillID, (int*)&(pQuestInfo->dwRequireLifeSkillID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "MinLifeSkillLevel",
        m_DefaultQuestInfo.dwMinLifeSkillLevel, (int*)&(pQuestInfo->dwMinLifeSkillLevel)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
    {
        sprintf(szColName, "StartRequireItemType%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwStartRequireItemType[i], (int*)&(pQuestInfo->dwStartRequireItemType[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "StartRequireItemIndex%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwStartRequireItemIndex[i], (int*)&(pQuestInfo->dwStartRequireItemIndex[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "StartRequireItemAmount%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwStartRequireItemAmount[i], (int*)&(pQuestInfo->dwStartRequireItemAmount[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
    {
        sprintf(szColName, "OfferItemType%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwOfferItemType[i], (int*)&(pQuestInfo->dwOfferItemType[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "OfferItemIndex%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwOfferItemIndex[i], (int*)&(pQuestInfo->dwOfferItemIndex[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "OfferItemAmount%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwOfferItemAmount[i], (int*)&(pQuestInfo->dwOfferItemAmount[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    bRetCode = piTabFile->GetInteger(
        nIndex, "EndMapID",
        m_DefaultQuestInfo.dwEndMapID, (int*)&(pQuestInfo->dwEndMapID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "EndNpcTemplateID",
        m_DefaultQuestInfo.dwEndNpcTemplateID, (int*)&(pQuestInfo->dwEndNpcTemplateID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "EndDoodadTemplateID", 
		m_DefaultQuestInfo.dwEndDoodadTemplateID, (int*)&(pQuestInfo->dwEndDoodadTemplateID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "EndItemType", 
		m_DefaultQuestInfo.dwEndItemType, (int*)&(pQuestInfo->dwEndItemType)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "EndItemIndex", 
        m_DefaultQuestInfo.dwEndItemIndex, (int*)&(pQuestInfo->dwEndItemIndex)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, "FinishTime", 
		m_DefaultQuestInfo.nFinishTime, (int*)&(pQuestInfo->nFinishTime)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
    bRetCode = piTabFile->GetInteger(
        nIndex, "EndGossipType",
        m_DefaultQuestInfo.nEndGossipType, (int*)&(pQuestInfo->nEndGossipType)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
    {
        sprintf(szColName, "EndGossip%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';
        
        bRetCode = piTabFile->GetString(
            nIndex, szColName, 
		    m_DefaultQuestInfo.szEndGossip[i], pQuestInfo->szEndGossip[i], QUEST_GOSSIP_LEN
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }
#endif

    for (int i = 0; i < QUEST_END_ITEM_COUNT; ++i)
    {
        sprintf(szColName, "EndRequireItemType%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName, 
            m_DefaultQuestInfo.dwEndRequireItemType[i], (int*)&(pQuestInfo->dwEndRequireItemType[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "EndRequireItemIndex%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwEndRequireItemIndex[i], (int*)&(pQuestInfo->dwEndRequireItemIndex[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "EndRequireItemAmount%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwEndRequireItemAmount[i], (int*)&(pQuestInfo->dwEndRequireItemAmount[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "IsDeleteEndRequireItem%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.bIsDeleteEndRequireItem[i], (int*)&(pQuestInfo->bIsDeleteEndRequireItem[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "DropNpcTemplateID%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwDropItemNpcTemplateID[i], (int*)&(pQuestInfo->dwDropItemNpcTemplateID[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "DropDoodadTemplateID%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwDropItemDoodadTemplateID[i], (int*)&(pQuestInfo->dwDropItemDoodadTemplateID[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "DropItemRate%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwDropItemRate[i], (int*)&(pQuestInfo->dwDropItemRate[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "IsDropForEach%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.bDropForEach[i], (int*)&(pQuestInfo->bDropForEach[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
    {
        sprintf(szColName, "KillNpcTemplateID%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwKillNpcTemplateID[i], (int*)&(pQuestInfo->dwKillNpcTemplateID[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "KillNpcAmount%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwKillNpcAmount[i], (int*)&(pQuestInfo->dwKillNpcAmount[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }
    
    for (int i = 0; i < QUEST_PARAM_COUNT * 2; ++i)
    {
        sprintf(szColName, "QuestValue%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.nQuestValue[i], (int*)&(pQuestInfo->nQuestValue[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
        sprintf(szColName, "QuestValueStr%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetString(
            nIndex, szColName,
		    m_DefaultQuestInfo.szQuestValueStr[i], pQuestInfo->szQuestValueStr[i], QUEST_VALUE_STR_LEN
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
#endif
    }
    
    for (int i = 0; i < QUEST_PARAM_COUNT * 2; ++i)
    {
        sprintf(szColName, "RelationDoodadID%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwRelationDoodadID[i], (int*)&(pQuestInfo->dwRelationDoodadID[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }

	bRetCode = piTabFile->GetInteger(
        nIndex, "HidePresent",
		m_DefaultQuestInfo.bHidePresent, (int*)&(pQuestInfo->bHidePresent)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "PresentAll1",
		m_DefaultQuestInfo.bPresentAll[0], (int*)&(pQuestInfo->bPresentAll[0])
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "PresentAll2",
		m_DefaultQuestInfo.bPresentAll[1], (int*)&(pQuestInfo->bPresentAll[1])
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < QUEST_PARAM_COUNT * 2; ++i)
    {
        sprintf(szColName, "PresentItemType%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwPresentItemType[i], (int*)&(pQuestInfo->dwPresentItemType[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "PresentItemIndex%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwPresentItemIndex[i], (int*)&(pQuestInfo->dwPresentItemIndex[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "PresentItemAmount%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

	    bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwPresentItemAmount[i], (int*)&(pQuestInfo->dwPresentItemAmount[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }

	bRetCode = piTabFile->GetInteger(
        nIndex, "PresentExp",
		m_DefaultQuestInfo.nPresentExp, (int*)&(pQuestInfo->nPresentExp)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "PresentMoney",
		m_DefaultQuestInfo.nPresentMoney, (int*)&(pQuestInfo->nPresentMoney)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
    {
        sprintf(szColName, "AffectForceIndex%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.dwAffectForceID[i], (int*)&(pQuestInfo->dwAffectForceID[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
        
        sprintf(szColName, "AffectValue%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
		    m_DefaultQuestInfo.nAffectForceValue[i], (int*)&(pQuestInfo->nAffectForceValue[i])
        );
	    KGLOG_PROCESS_ERROR(bRetCode);
    }

	bRetCode = piTabFile->GetInteger(
        nIndex, "RepeatCutPercent",
		m_DefaultQuestInfo.nRepeatCutPercent, (int*)&(pQuestInfo->nRepeatCutPercent)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetString(
        nIndex, "ScriptName", "", pQuestInfo->szScriptName, MAX_PATH
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, "RewardSkillID",
		m_DefaultQuestInfo.nPresentSkill, (int*)&(pQuestInfo->nPresentSkill)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, "CostMoney",
		m_DefaultQuestInfo.nCostMoney, (int*)&(pQuestInfo->nCostMoney)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    KGLOG_PROCESS_ERROR(pQuestInfo->nCostMoney >= 0);

    bRetCode = piTabFile->GetInteger(
        nIndex, "RequireSchoolMask",
        m_DefaultQuestInfo.dwRequireSchoolMask, (int*)&(pQuestInfo->dwRequireSchoolMask)
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, "RequireReputeAll",
        m_DefaultQuestInfo.bRequireReputeAll, (int*)&(pQuestInfo->bRequireReputeAll)
    );

    for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
    {
        sprintf(szColName, "RequireForceID%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
            m_DefaultQuestInfo.dwRequireForceID[i], (int*)&(pQuestInfo->dwRequireForceID[i])
        );

        sprintf(szColName, "RequireReputeLevelMin%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
            m_DefaultQuestInfo.nReputeLevelMin[i], (int*)&(pQuestInfo->nReputeLevelMin[i])
        );
        
        sprintf(szColName, "RequireReputeLevelMax%d", i + 1);
        szColName[sizeof(szColName) - 1] = '\0';

        bRetCode = piTabFile->GetInteger(
            nIndex, szColName,
            m_DefaultQuestInfo.nReputeLevelMax[i], (int*)&(pQuestInfo->nReputeLevelMax[i])
        );
    }

	bRetCode = piTabFile->GetInteger(
        nIndex, "PresentAssistThew",
		m_DefaultQuestInfo.nPresentAssistThew, &pQuestInfo->nPresentAssistThew
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "PresentAssistStamina",
		m_DefaultQuestInfo.nPresentAssistStamina, &pQuestInfo->nPresentAssistStamina
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, "PresentAssistFriendship",
		m_DefaultQuestInfo.nPresentAssistFriendship, &pQuestInfo->nPresentAssistFriendship
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, "RequireCamp", m_DefaultQuestInfo.nRequireCampMask, (int*)&(pQuestInfo->nRequireCampMask)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, "PresentPrestige", m_DefaultQuestInfo.nPresentPrestige, (int*)&(pQuestInfo->nPresentPrestige)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, "PresentContribution", m_DefaultQuestInfo.nPresentContribution, (int*)&(pQuestInfo->nPresentContribution)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = piTabFile->GetInteger(
        nIndex, "PresentTrain", m_DefaultQuestInfo.nPresentTrain, (int*)&(pQuestInfo->nPresentTrain)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

#ifdef _SERVER
    bRetCode = piTabFile->GetInteger(
        nIndex, "AchievementID", m_DefaultQuestInfo.nAchievementID, &pQuestInfo->nAchievementID
    );
    KGLOG_PROCESS_ERROR(bRetCode);
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KQuestInfoList::MakeQuestMap(KQuestInfo* pQuestInfo)
{
    BOOL    bResult     = false;
    DWORD   dwQuestID   = 0;
    DWORD   adwTemplateID[QUEST_END_ITEM_COUNT];

    assert(pQuestInfo);

    dwQuestID = pQuestInfo->dwQuestID;
    KGLOG_PROCESS_ERROR(dwQuestID < MAX_QUEST_COUNT);

    m_mapNpc2QuestInfo.insert(std::make_pair(pQuestInfo->dwStartNpcTemplateID, dwQuestID));

    if (pQuestInfo->dwStartNpcTemplateID != pQuestInfo->dwEndNpcTemplateID)
    {
        m_mapNpc2QuestInfo.insert(std::make_pair(pQuestInfo->dwEndNpcTemplateID, dwQuestID));
    }
    
#ifdef _SERVER

    // 去重
    memcpy(adwTemplateID, pQuestInfo->dwDropItemNpcTemplateID, sizeof(adwTemplateID));
    for (int i = 0; i < QUEST_END_ITEM_COUNT; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (adwTemplateID[i] != 0 && adwTemplateID[i] == adwTemplateID[j])
            {
                adwTemplateID[i] = 0;
                break;
            }
        }
    }

    for (int nDropIndex = 0; nDropIndex < QUEST_END_ITEM_COUNT; ++nDropIndex)
    {
        if (adwTemplateID[nDropIndex])
        {
            m_mapNpcID2QuestID.insert(std::make_pair(adwTemplateID[nDropIndex], dwQuestID));
        }
    }
#endif

    // 去重
    memcpy(adwTemplateID, pQuestInfo->dwDropItemDoodadTemplateID, sizeof(adwTemplateID));
    for (int i = 0; i < QUEST_END_ITEM_COUNT; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (adwTemplateID[i] != 0 && adwTemplateID[i] == adwTemplateID[j])
            {
                adwTemplateID[i] = 0;
                break;
            }
        }
    }

    for (int nDropIndex = 0; nDropIndex < QUEST_END_ITEM_COUNT; ++nDropIndex)
    {
        if (adwTemplateID[nDropIndex])
        {
            m_mapDoodadID2QuestID.insert(std::make_pair(adwTemplateID[nDropIndex], dwQuestID));
        }
    }

    for (int nDoodadIndex = 0; nDoodadIndex < QUEST_PARAM_COUNT * 2; ++nDoodadIndex)
    {
        if (pQuestInfo->dwRelationDoodadID[nDoodadIndex])
        {
            m_mapRDoodadID2QuestID.insert(
                std::make_pair(pQuestInfo->dwRelationDoodadID[nDoodadIndex], dwQuestID)
            );
        }
    }

    bResult = true;
Exit0:
    return bResult;
}


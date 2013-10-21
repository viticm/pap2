#include "stdafx.h"

#if defined(_SERVER)
#include "KPlayer.h"
#include "KSO3World.h"
#include "KIndividualDropList.h"
#include "KLevelDropList.h"
#include "KLevelMoneyDropList.h"
#include "KLootList.h"

BOOL KDropCenter::Init(void)
{
	BOOL bResult  = false;
    BOOL bRetCode = false;
    
    bRetCode = NpcTemplateDropInit();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = NpcClassDropInit();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = MapDropInit();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = DoodadTemplateDropInit();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = DoodadClassDropInit();
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = MoneyDropInit();
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
	return bResult;
}

BOOL KDropCenter::UnInit(void)
{
	for (int nDropIndex = 0; nDropIndex < MAX_DROP_PER_NPC; nDropIndex++)
		m_mapNpcTemplateID2DropList[nDropIndex].clear();
	m_mapNpcClassID2DropList.clear();
	m_mapMapID2DropList.clear();

	for (int nDropIndex = 0; nDropIndex < MAX_DROP_PER_DOODAD; nDropIndex++)
	{
		m_mapDoodadTemplateID2DropList[nDropIndex].clear();
		m_mapDoodadClassID2DropList[nDropIndex].clear();
	}

	m_mapNpcTemplateID2MoneyDropList.clear();
	m_mapNpcClassID2MoneyDropList.clear();

	m_mapDoodadTemplateID2MoneyDropList.clear();
	m_mapDoodadClassID2MoneyDropList.clear();


	return true;
}

BOOL KDropCenter::NpcMapDrop(KLootList* pLootList, std::vector<DWORD>& vecLooterList, DWORD dwMapID)
{
    BOOL            bResult     = false;
	BOOL			bRetCode	= false;
	KItem*			pItem		= NULL;
	DROP_DATA		DropData;
	MAP_DWORD_2_DROP_LIST::iterator it;
	
	it = m_mapMapID2DropList.find(dwMapID);
	KG_PROCESS_SUCCESS(it == m_mapMapID2DropList.end());

	pItem = it->second.GetRandomItem(DropData);
	if (pItem)
	{
		bRetCode = AddItemToLootList(pLootList, pItem, DropData, vecLooterList);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

Exit1:
	bResult = true;
Exit0:
	if (pItem)
	{
		g_pSO3World->m_ItemManager.FreeItem(pItem);
		pItem = NULL;
	}
	return bResult;
}

BOOL KDropCenter::NpcClassDrop(KLootList* pLootList, std::vector<DWORD>& vecLooterList, DWORD dwClassID, int nLevel)
{
    BOOL            bResult     = false;
	BOOL			bRetCode	= false;
	KItem*			pItem		= NULL;
	DROP_DATA		DropData;
	MAP_INT64_2_DROP_LIST::iterator it;
	
	it = m_mapNpcClassID2DropList.find(MAKE_INT64(dwClassID, nLevel));
	KG_PROCESS_SUCCESS(it == m_mapNpcClassID2DropList.end());

	pItem = it->second.GetRandomItem(DropData);
	if (pItem)
	{
		bRetCode = AddItemToLootList(pLootList, pItem, DropData, vecLooterList);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

Exit1:
	bResult = true;
Exit0:
	if (pItem)
	{
		g_pSO3World->m_ItemManager.FreeItem(pItem);
		pItem = NULL;
	}
	return bResult;
}

BOOL KDropCenter::NpcTemplateDrop(KLootList* pLootList, std::vector<DWORD>& vecLooterList, DWORD dwNpcTemplateID, int nDropIndex)
{
    BOOL            bResult     = false;
	BOOL			bRetCode	= false;
	KItem*			pItem		= NULL;
	DROP_DATA		DropData;
	MAP_DWORD_2_DROP_LIST::iterator it;

	KGLOG_PROCESS_ERROR(nDropIndex >= 0);
	
	it = m_mapNpcTemplateID2DropList[nDropIndex].find(dwNpcTemplateID);
	KG_PROCESS_SUCCESS(it == m_mapNpcTemplateID2DropList[nDropIndex].end());

	pItem = it->second.GetRandomItem(DropData);
	if (pItem)
	{
		bRetCode = AddItemToLootList(pLootList, pItem, DropData, vecLooterList);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

Exit1:
	bResult = true;
Exit0:
	if (pItem)
	{
		g_pSO3World->m_ItemManager.FreeItem(pItem);
		pItem = NULL;
	}
	return bResult;
}

BOOL KDropCenter::DoodadClassDrop(
    KLootList* pLootList, KPlayer* pOpener, DWORD dwClassID, DWORD dwDropIndex, int nLevel, BOOL bIsAbsoluteFree
)
{
    BOOL            bResult     = false;
	BOOL			bRetCode	= false;
	KItem*			pItem		= NULL;
    std::vector<DWORD> vecLooterList;
	DROP_DATA		DropData;
	MAP_INT64_2_DROP_LIST::iterator it;
    
    KGLOG_PROCESS_ERROR(pLootList);
    KGLOG_PROCESS_ERROR(pOpener);

	it = m_mapDoodadClassID2DropList[dwDropIndex].find(MAKE_INT64(dwClassID, nLevel));
	KG_PROCESS_SUCCESS(it == m_mapDoodadClassID2DropList[dwDropIndex].end());

	pItem = it->second.GetRandomItem(DropData);
	if (pItem == NULL)
	{
        goto Exit1;
    }
    
    if (bIsAbsoluteFree)
    {
        bRetCode = AddItemToLootListAbsoluteFree(pLootList, pItem, DropData, pOpener);
        KGLOG_PROCESS_ERROR(bRetCode);
        goto Exit1;
    }
    
    pLootList->GetLooterList(vecLooterList);

    bRetCode = AddItemToLootList(pLootList, pItem, DropData, vecLooterList);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
	bResult = true;
Exit0:
	if (pItem)
	{
		g_pSO3World->m_ItemManager.FreeItem(pItem);
		pItem = NULL;
	}
	return bResult;
}

BOOL KDropCenter::DoodadTemplateDrop(
    KLootList* pLootList, KPlayer* pOpener, DWORD dwDoodadTemplateID, DWORD dwDropIndex, BOOL bIsAbsoluteFree
)
{
    BOOL            bResult     = false;
	BOOL			bRetCode	= false;
	KItem*			pItem		= NULL;
    std::vector<DWORD> vecLooterList;
	DROP_DATA		DropData;
	MAP_DWORD_2_DROP_LIST::iterator it;
	
    KGLOG_PROCESS_ERROR(pLootList);
    KGLOG_PROCESS_ERROR(pOpener);

	it = m_mapDoodadTemplateID2DropList[dwDropIndex].find(dwDoodadTemplateID);
	KG_PROCESS_SUCCESS(it == m_mapDoodadTemplateID2DropList[dwDropIndex].end());

	pItem = it->second.GetRandomItem(DropData);
	if (pItem == NULL)
	{
        goto Exit1;
    }
    
    if (bIsAbsoluteFree)
    {
		bRetCode = AddItemToLootListAbsoluteFree(pLootList, pItem, DropData, pOpener);
		KGLOG_PROCESS_ERROR(bRetCode);
        goto Exit1;
	}
    
    pLootList->GetLooterList(vecLooterList);

    bRetCode = AddItemToLootList(pLootList, pItem, DropData, vecLooterList);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
	bResult = true;
Exit0:
	if (pItem)
	{
		g_pSO3World->m_ItemManager.FreeItem(pItem);
		pItem = NULL;
	}
	return bResult;
}

int KDropCenter::NpcDropMoney(DWORD dwNpcTemplateID)
{
	int             nResult = 0;
	KNpcTemplate*   pNpcTemplate = NULL;
	BOOL            bIsDrop = false;
  	int64_t         nKey    = 0;

	pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(dwNpcTemplateID);
	KGLOG_PROCESS_ERROR(pNpcTemplate);

	//模板
	{
		MAP_DWORD_2_MONEY_DROP_LIST::iterator it = m_mapNpcTemplateID2MoneyDropList.find(dwNpcTemplateID);

		if (it != m_mapNpcTemplateID2MoneyDropList.end())
		{
			bIsDrop = it->second.IsDropInRandom();
			if (bIsDrop)
				nResult += it->second.GetRandomMoney();
		}
	}

	//分类
	{
		nKey = MAKE_INT64(pNpcTemplate->nDropClassID, pNpcTemplate->nLevel);

		MAP_INT64_2_MONEY_DROP_LIST::iterator it = m_mapNpcClassID2MoneyDropList.find(nKey);

		if (it != m_mapNpcClassID2MoneyDropList.end())
		{
			if (bIsDrop)
			{
				nResult += it->second.GetRandomMoney();
			}
		}
	}

Exit0:
	return nResult;
}

int KDropCenter::DoodadDropMoney(DWORD dwDoodadTempalteID)
{
	int                 nResult         = 0;
	KDoodadTemplate*    pDoodadTemplate = NULL;
	int64_t             nKey            = 0;
	BOOL                bIsDrop         = false;

	pDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplate(dwDoodadTempalteID);
	KGLOG_PROCESS_ERROR(pDoodadTemplate);

	//模板
	{
		MAP_DWORD_2_MONEY_DROP_LIST::iterator it = m_mapDoodadTemplateID2MoneyDropList.find(dwDoodadTempalteID);

		if (it != m_mapDoodadTemplateID2MoneyDropList.end())
		{
			bIsDrop = it->second.IsDropInRandom();
			if (bIsDrop)
			{			
				nResult += it->second.GetRandomMoney();
			}
		}
	}

	//分类
	{
		nKey = MAKE_INT64(pDoodadTemplate->m_dwClassID, pDoodadTemplate->m_nLevel);

		MAP_INT64_2_MONEY_DROP_LIST::iterator it = m_mapDoodadClassID2MoneyDropList.find(nKey);

		if (it != m_mapDoodadClassID2MoneyDropList.end())
		{
			bIsDrop = it->second.IsDropInRandom();
			if (bIsDrop)
			{
				nResult += it->second.GetRandomMoney();
			}
		}
	}

Exit0:
	return nResult;
}

BOOL KDropCenter::NpcDropQuestItem(
    DWORD dwTemplateID, KPlayer* pDropTarget, KLootList* pLootList, std::vector<DWORD>& vecLooterList
)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    std::multimap<DWORD, DWORD>::iterator it;
    std::multimap<DWORD, DWORD>::iterator UpperBoundIt;
    
    assert(pDropTarget);
    assert(pLootList);
    
    it = g_pSO3World->m_Settings.m_QuestInfoList.m_mapNpcID2QuestID.lower_bound(dwTemplateID);
    UpperBoundIt = g_pSO3World->m_Settings.m_QuestInfoList.m_mapNpcID2QuestID.upper_bound(dwTemplateID); 
    
    for (; it != UpperBoundIt; ++it)
    {
        DropQuestItem(it->second, dwTemplateID, pDropTarget, pLootList, vecLooterList, true);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDropCenter::DoodadDropQuestItem(DWORD dwTemplateID, KPlayer* pDropTarget, KLootList* pLootList, BOOL bIsAbsoluteFree)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    std::multimap<DWORD, DWORD>::iterator it;
    std::multimap<DWORD, DWORD>::iterator UpperBoundIt;
    std::vector<DWORD> vecLooterList;

    assert(pDropTarget);
    assert(pLootList);

    it = g_pSO3World->m_Settings.m_QuestInfoList.m_mapDoodadID2QuestID.lower_bound(dwTemplateID);
    UpperBoundIt = g_pSO3World->m_Settings.m_QuestInfoList.m_mapDoodadID2QuestID.upper_bound(dwTemplateID);
    
    if (bIsAbsoluteFree)
    {
        for (; it != UpperBoundIt; ++it)
        {
            DropQuestItemFromDoodadAbsoluteFree(it->second, dwTemplateID, pDropTarget, pLootList);
        }

        goto Exit1;
    }

    pLootList->GetLooterList(vecLooterList);
    
    for (; it != UpperBoundIt; ++it)
    {
        DropQuestItem(it->second, dwTemplateID, pDropTarget, pLootList, vecLooterList, false);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KDropCenter::NpcTemplateDropInit(void)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    int  nNpcCount  = 0;
    char szFileName[MAX_PATH];

    nNpcCount = g_pSO3World->m_Settings.m_NpcTemplateList.GetNpcTemplateCount();
	for (int i = 0; i < nNpcCount; ++i)
	{
		KNpcTemplate* pNpcTemplate = NULL;

		pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplateByIndex(i);
		if (pNpcTemplate == NULL)
			continue;

		for (int nDropIndex = 0; nDropIndex < MAX_DROP_PER_NPC; ++nDropIndex)
		{
            KDropList*          pDropList           = NULL;
			KIndividualDropList IndividualDropList;
            KDropList           DropList;
			std::pair<MAP_DWORD_2_DROP_LIST::iterator, bool> InsRet;

			if (pNpcTemplate->szDropName[nDropIndex][0] == '\0')
				continue;

			snprintf(szFileName, sizeof(szFileName), "%s/%s/%s", SETTING_DIR, DROP_LIST_DIR, pNpcTemplate->szDropName[nDropIndex]);
            szFileName[sizeof(szFileName) - 1] = '\0';

			bRetCode = IndividualDropList.Init(szFileName);
			KGLOG_PROCESS_ERROR(bRetCode);

			InsRet = m_mapNpcTemplateID2DropList[nDropIndex].insert(std::make_pair(pNpcTemplate->dwTemplateID, DropList));
			KGLOG_PROCESS_ERROR(InsRet.second);
            
            pDropList = &(InsRet.first->second);

			bRetCode = pDropList->Init(IndividualDropList.GetSize());
			KGLOG_PROCESS_ERROR(bRetCode);

			for (DWORD dwIndex = 0; dwIndex < IndividualDropList.GetSize(); dwIndex++)
			{
				DROP_DATA DropData;

				bRetCode = IndividualDropList.GetData(dwIndex, DropData);
				KGLOG_PROCESS_ERROR(bRetCode);

				bRetCode = pDropList->AddItem(DropData);
				KGLOG_PROCESS_ERROR(bRetCode);
			}

			bRetCode = pDropList->PreProcess();
			KGLOG_PROCESS_ERROR(bRetCode);
		}
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDropCenter::NpcClassDropInit(void)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    DWORD   dwMaxClassID    = 0;
    
    dwMaxClassID = g_pSO3World->m_Settings.m_NpcClassList.GetMaxClassID();

	for (DWORD i = 0; i < dwMaxClassID; ++i)
	{
		KNpcClass*      pNpcClass               = NULL;
		char            szFileName[MAX_PATH];
		KLevelDropList  LevelDropList;

		pNpcClass = g_pSO3World->m_Settings.m_NpcClassList.GetData(i);
		if (pNpcClass == NULL)
			continue;

		if (pNpcClass->szDropName[0] == '\0')
			continue;

		snprintf(szFileName, sizeof(szFileName), "%s/%s/%s", SETTING_DIR, DROP_LIST_DIR, pNpcClass->szDropName);
        szFileName[sizeof(szFileName) - 1] = '\0';

		bRetCode = LevelDropList.Init(szFileName);
		KGLOG_PROCESS_ERROR(bRetCode);

		for (DWORD dwIndex = 0; dwIndex < LevelDropList.GetSize(); dwIndex++)
		{
			LEVEL_DROP_DATA                 LevelDropData;
            MAP_INT64_2_DROP_LIST::iterator it;

			bRetCode = LevelDropList.GetData(dwIndex, LevelDropData);
			KGLOG_PROCESS_ERROR(bRetCode);

			it = m_mapNpcClassID2DropList.find(MAKE_INT64(i, LevelDropData.nLevel));

			if (it == m_mapNpcClassID2DropList.end())
			{
				KDropList                                           DropList;
				std::pair<MAP_INT64_2_DROP_LIST::iterator, bool>    InsRet;

				InsRet = m_mapNpcClassID2DropList.insert(std::make_pair(MAKE_INT64(i, LevelDropData.nLevel), DropList));
				KGLOG_PROCESS_ERROR(InsRet.second);

				it = InsRet.first;

				bRetCode = it->second.Init(LevelDropList.GetLevelCount(LevelDropData.nLevel));
				KGLOG_PROCESS_ERROR(bRetCode);
			}

			bRetCode = it->second.AddItem(LevelDropData);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
	}

	for (MAP_INT64_2_DROP_LIST::iterator it = m_mapNpcClassID2DropList.begin(); it != m_mapNpcClassID2DropList.end(); ++it)
	{
		bRetCode = it->second.PreProcess();
		KGLOG_PROCESS_ERROR(bRetCode);
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDropCenter::MapDropInit(void)
{
    BOOL             bResult  = false;
    BOOL             bRetCode = false;
    KMAP_PARAM_TABLE MapList  = g_pSO3World->m_Settings.m_MapListFile.GetMapList();

	for (KMAP_PARAM_TABLE::iterator it = MapList.begin(); it != MapList.end(); ++it)
	{
        KMapParams*         pMapParams  = NULL;
        KDropList*          pDropList   = NULL;
        char                szFileName[MAX_PATH];
		KIndividualDropList IndividualDropList;
		KDropList           DropList;
  		std::pair<MAP_DWORD_2_DROP_LIST::iterator, bool> InsRet;

        pMapParams = &it->second;

		if (pMapParams->szDropName[0] == '\0')
			continue;

		snprintf(szFileName, sizeof(szFileName), "%s/%s/%s", SETTING_DIR, DROP_LIST_DIR, pMapParams->szDropName);
        szFileName[sizeof(szFileName) - 1] = '\0';

		bRetCode = IndividualDropList.Init(szFileName);
		KGLOG_PROCESS_ERROR(bRetCode);

		InsRet = m_mapMapID2DropList.insert(std::make_pair(it->first, DropList));
		KGLOG_PROCESS_ERROR(InsRet.second);
        
        pDropList = &(InsRet.first->second);

		bRetCode = pDropList->Init(IndividualDropList.GetSize());
		KGLOG_PROCESS_ERROR(bRetCode);

		for (DWORD dwIndex = 0; dwIndex < IndividualDropList.GetSize(); dwIndex++)
		{
			DROP_DATA DropData;

			bRetCode = IndividualDropList.GetData(dwIndex, DropData);
			KGLOG_PROCESS_ERROR(bRetCode);

			bRetCode = pDropList->AddItem(DropData);
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		bRetCode = pDropList->PreProcess();
		KGLOG_PROCESS_ERROR(bRetCode);		
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDropCenter::DoodadTemplateDropInit(void)
{
    BOOL bResult        = false;
    BOOL bRetCode       = false;
    int  nDoodadCount   = 0;
    
    nDoodadCount = g_pSO3World->m_Settings.m_DoodadTemplateList.GetDoodadTemplateCount();

	for (int i = 0; i < nDoodadCount;  ++i)
	{
		KDoodadTemplate* pDoodadTemplate = NULL;

		pDoodadTemplate = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplateByIndex(i);
		if (pDoodadTemplate == NULL)
			continue;

		for (int nDropIndex = 0; nDropIndex < MAX_DROP_PER_DOODAD; nDropIndex++)
		{
            KDropList*          pDropList   = NULL;
			char                szFileName[MAX_PATH];
			KIndividualDropList IndividualDropList;
			KDropList           DropList;
			std::pair<MAP_DWORD_2_DROP_LIST::iterator, bool> InsRet;

			if (pDoodadTemplate->m_szDropName[nDropIndex][0] == '\0')
				continue;

			snprintf(szFileName, sizeof(szFileName), "%s/%s/%s", SETTING_DIR, DOODAD_DROP_LIST_DIR, pDoodadTemplate->m_szDropName[nDropIndex]);
            szFileName[sizeof(szFileName) - 1] = '\0';

			bRetCode = IndividualDropList.Init(szFileName);
			KGLOG_PROCESS_ERROR(bRetCode);

			InsRet = m_mapDoodadTemplateID2DropList[nDropIndex].insert(std::make_pair(pDoodadTemplate->m_dwTemplateID, DropList));
			KGLOG_PROCESS_ERROR(InsRet.second);
            
            pDropList = &(InsRet.first->second);

			bRetCode = pDropList->Init(IndividualDropList.GetSize());
			KGLOG_PROCESS_ERROR(bRetCode);

			for (DWORD dwIndex = 0; dwIndex < IndividualDropList.GetSize(); dwIndex++)
			{
				DROP_DATA DropData;

				bRetCode = IndividualDropList.GetData(dwIndex, DropData);
				KGLOG_PROCESS_ERROR(bRetCode);

				bRetCode = pDropList->AddItem(DropData);
				KGLOG_PROCESS_ERROR(bRetCode);
			}

			bRetCode = pDropList->PreProcess();
			KGLOG_PROCESS_ERROR(bRetCode);
		}
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDropCenter::DoodadClassDropInit(void)
{
    BOOL    bResult             = false;
    BOOL    bRetCode            = false;
    DWORD   dwMaxDoodadClassID  = 0;
    
    dwMaxDoodadClassID = g_pSO3World->m_Settings.m_DoodadClassList.GetMaxClassID();

	for (DWORD dwClassID = 0; dwClassID <= dwMaxDoodadClassID; ++dwClassID)
	{
		KDoodadClass*   pDoodadClass            = NULL;
		char            szFileName[MAX_PATH];

		pDoodadClass = g_pSO3World->m_Settings.m_DoodadClassList.GetData(dwClassID);
		if (pDoodadClass == NULL)
			continue;

		for (DWORD dwDropIndex = 0; dwDropIndex < MAX_DROP_PER_DOODAD; dwDropIndex++)
		{
			KLevelDropList LevelDropList;

			snprintf(szFileName, sizeof(szFileName), "%s/%s/%s", SETTING_DIR, DOODAD_DROP_LIST_DIR, pDoodadClass->szDropName[dwDropIndex]);
            szFileName[sizeof(szFileName) - 1] = '\0';

			if (pDoodadClass->szDropName[dwDropIndex][0] == '\0')
				continue;

			bRetCode = LevelDropList.Init(szFileName);
			KGLOG_PROCESS_ERROR(bRetCode);

			for (DWORD dwIndex = 0; dwIndex < LevelDropList.GetSize(); dwIndex++)
			{
				LEVEL_DROP_DATA LevelDropData;

				bRetCode = LevelDropList.GetData(dwIndex, LevelDropData);
				KGLOG_PROCESS_ERROR(bRetCode);

				MAP_INT64_2_DROP_LIST::iterator it;
				it = m_mapDoodadClassID2DropList[dwDropIndex].find(MAKE_INT64(dwClassID, LevelDropData.nLevel));

				if (it == m_mapDoodadClassID2DropList[dwDropIndex].end())
				{
					KDropList DropList;
					std::pair<MAP_INT64_2_DROP_LIST::iterator, bool> InsRet;

					InsRet = m_mapDoodadClassID2DropList[dwDropIndex].insert(
                        std::make_pair(MAKE_INT64(dwClassID, LevelDropData.nLevel), DropList));
					KGLOG_PROCESS_ERROR(InsRet.second);

					it = InsRet.first;

					bRetCode = it->second.Init(LevelDropList.GetLevelCount(LevelDropData.nLevel));
					KGLOG_PROCESS_ERROR(bRetCode);
				}

				bRetCode = it->second.AddItem(LevelDropData);
				KGLOG_PROCESS_ERROR(bRetCode);
			}
		}
	}

	for (DWORD dwDropIndex = 0; dwDropIndex < MAX_DROP_PER_DOODAD; dwDropIndex++)
	{
		for (MAP_INT64_2_DROP_LIST::iterator it = m_mapDoodadClassID2DropList[dwDropIndex].begin();
			it != m_mapDoodadClassID2DropList[dwDropIndex].end(); ++it)
		{
			bRetCode = it->second.PreProcess();
			KGLOG_PROCESS_ERROR(bRetCode);
		}
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDropCenter::MoneyDropInit(void)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nNpcCount       = 0;
    int     nDoodadCount    = 0;
    int     nClassID        = 0;
	int     nLevel          = 0;
	int     nMinMoney       = 0;
	int     nMaxMoney       = 0;
    char    szFileName[MAX_PATH];
    KLevelMoneyDropList LevelDoodadMoneyDropList;
    KLevelMoneyDropList LevelNpcMoneyDropList;

    nNpcCount = g_pSO3World->m_Settings.m_NpcTemplateList.GetNpcTemplateCount();

	for (int nIndex = 0; nIndex < nNpcCount; ++nIndex)
	{
        KNpcTemplate*       pNpcTemplate = NULL;
        std::pair<MAP_DWORD_2_MONEY_DROP_LIST::iterator, bool> InsRet;

        pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplateByIndex(nIndex);
		if (pNpcTemplate == NULL)
			continue;

		if (pNpcTemplate->nMinMoney == 0 && pNpcTemplate->nMaxMoney == 0 && pNpcTemplate->nMoneyRate == 0)
			continue;
        
		InsRet = m_mapNpcTemplateID2MoneyDropList.insert(std::make_pair(pNpcTemplate->dwTemplateID, KMoneyDropList()));
		KGLOG_PROCESS_ERROR(InsRet.second);

		InsRet.first->second.Init();
		InsRet.first->second.AddMoney(pNpcTemplate->nMinMoney, pNpcTemplate->nMaxMoney);
		InsRet.first->second.SetMoneyDropRate(pNpcTemplate->nMoneyRate);
	}
    
    nDoodadCount = g_pSO3World->m_Settings.m_DoodadTemplateList.GetDoodadTemplateCount();

	for (int nIndex = 0; nIndex < nDoodadCount; ++nIndex)
	{
        KDoodadTemplate*        pDoodadTempalte = NULL;
        std::pair<MAP_DWORD_2_MONEY_DROP_LIST::iterator, bool> InsRet;

        pDoodadTempalte = g_pSO3World->m_Settings.m_DoodadTemplateList.GetTemplateByIndex(nIndex);

		if(pDoodadTempalte == NULL)
			continue;

		if (pDoodadTempalte->m_nMinMoney == 0 && pDoodadTempalte->m_nMaxMoney == 0 && pDoodadTempalte->m_nMoneyRate == 0)
			continue;

		InsRet = m_mapDoodadTemplateID2MoneyDropList.insert(std::make_pair(pDoodadTempalte->m_dwTemplateID, KMoneyDropList()));
		KGLOG_PROCESS_ERROR(InsRet.second);

		InsRet.first->second.Init();
		InsRet.first->second.AddMoney(pDoodadTempalte->m_nMinMoney, pDoodadTempalte->m_nMaxMoney);
		InsRet.first->second.SetMoneyDropRate(pDoodadTempalte->m_nMoneyRate);
	}

	snprintf(szFileName, sizeof(szFileName), "%s/%s/%s", SETTING_DIR, DROP_LIST_DIR, MONEY_DROP_FILE);
    szFileName[sizeof(szFileName) - 1] = '\0';
	
	bRetCode = LevelNpcMoneyDropList.Init(szFileName);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (DWORD dwIndex = 0; dwIndex < LevelNpcMoneyDropList.GetSize(); dwIndex++)
	{
        MAP_INT64_2_MONEY_DROP_LIST::iterator it;

        bRetCode = LevelNpcMoneyDropList.GetData(dwIndex, nLevel, nClassID, nMinMoney, nMaxMoney);
		KGLOG_PROCESS_ERROR(bRetCode);

		it = m_mapNpcClassID2MoneyDropList.find(MAKE_INT64(nClassID, nLevel));

		if (it == m_mapNpcClassID2MoneyDropList.end())
		{
			std::pair<MAP_INT64_2_MONEY_DROP_LIST::iterator, bool> InsRet;

			InsRet = m_mapNpcClassID2MoneyDropList.insert(std::make_pair(MAKE_INT64(nClassID, nLevel), KMoneyDropList()));
			KGLOG_PROCESS_ERROR(InsRet.second);

			it = InsRet.first;

			bRetCode = it->second.Init();
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		bRetCode = it->second.AddMoney(nMinMoney, nMaxMoney);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	snprintf(szFileName, sizeof(szFileName), "%s/%s/%s", SETTING_DIR, DOODAD_DROP_LIST_DIR, MONEY_DROP_FILE);
    szFileName[sizeof(szFileName) - 1] = '\0';

	bRetCode = LevelDoodadMoneyDropList.Init(szFileName);
	KGLOG_PROCESS_ERROR(bRetCode);

	for (DWORD dwIndex = 0; dwIndex < LevelDoodadMoneyDropList.GetSize(); dwIndex++)
	{
        MAP_INT64_2_MONEY_DROP_LIST::iterator it;

        bRetCode = LevelDoodadMoneyDropList.GetData(dwIndex, nLevel, nClassID, nMinMoney, nMaxMoney);
		KGLOG_PROCESS_ERROR(bRetCode);

		it = m_mapDoodadClassID2MoneyDropList.find(MAKE_INT64(nClassID, nLevel));

		if (it == m_mapDoodadClassID2MoneyDropList.end())
		{
			std::pair<MAP_INT64_2_MONEY_DROP_LIST::iterator, bool> InsRet;

			InsRet = m_mapDoodadClassID2MoneyDropList.insert(std::make_pair(MAKE_INT64(nClassID, nLevel), KMoneyDropList()));
			KGLOG_PROCESS_ERROR(InsRet.second);

			it = InsRet.first;

			bRetCode = it->second.Init();
			KGLOG_PROCESS_ERROR(bRetCode);
		}

		bRetCode = it->second.AddMoney(nMinMoney, nMaxMoney);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KDropCenter::GetNeedQuestItemPlayer(
    IN KItem* pItem, IN DROP_DATA& DropData, IN std::vector<DWORD>& vecLooterList, 
    OUT std::vector<DWORD>& vecNeedQuestItemPlayer
)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    KPlayer*    pLooter             = NULL;
    size_t      uPlayerCount        = 0;
    KQuestInfo* pQuestInfo          = NULL;
    DWORD       dwItemCount         = 0;
    BOOL        bQuestItemEnough    = false;

    assert(pItem);
    
    uPlayerCount = vecLooterList.size();
    KG_PROCESS_SUCCESS(uPlayerCount == 0);

    for (size_t i = 0; i < uPlayerCount; ++i)
	{
		pLooter = g_pSO3World->m_PlayerSet.GetObj(vecLooterList[i]);
		if (pLooter == NULL)
        {
    		continue;
        }

        if (DropData.dwQuestID == 0)
        {
            vecNeedQuestItemPlayer.push_back(pLooter->m_dwID);
            continue;
        }

	    int nQuestPhase = pLooter->m_QuestList.GetQuestPhase(DropData.dwQuestID);
	    if (nQuestPhase != (int)DropData.dwQuestState)
        {
            continue;
        }

        if (nQuestPhase != 1) // 如果是已接未完成还要检查任务物品是否已经足够
        {
            vecNeedQuestItemPlayer.push_back(pLooter->m_dwID);
            continue;
        }

        pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(DropData.dwQuestID);
	    KGLOG_PROCESS_ERROR(pQuestInfo);

	    for (int j = 0; j < QUEST_END_ITEM_COUNT; ++j)
	    {
		    if (
                pQuestInfo->byEndRequireItemType[j] == pItem->m_GenParam.dwTabType && 
                pQuestInfo->wEndRequireItemIndex[j] == pItem->m_GenParam.dwIndex
            )
            {
                bRetCode = pLooter->m_QuestList.IsQuestEndItemEnoughByIndex(pQuestInfo, j, true);
                if (bRetCode)
                {
                    bQuestItemEnough = true;
                    break;
                }
            }
	    }

        if (!bQuestItemEnough)
        {
            vecNeedQuestItemPlayer.push_back(pLooter->m_dwID);
            continue;
        }
    }

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

BOOL KDropCenter::AddItemToLootList(
    KLootList* pLootList, KItem* pItem, DROP_DATA& DropData, std::vector<DWORD>& vecLooterList
)
{
    BOOL        bResult         = false;
	BOOL		bRetCode		= false;
	size_t		uPlayerCount	= 0;
    KLootItem*  pAddLootItem    = NULL;
    KItem*      pTempItem       = NULL;
    std::vector<DWORD> vecNeedQuestItemPlayer;

	KGLOG_PROCESS_ERROR(pLootList);
	KGLOG_PROCESS_ERROR(pItem);

    bRetCode = GetNeedQuestItemPlayer(pItem, DropData, vecLooterList, vecNeedQuestItemPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

    uPlayerCount = vecNeedQuestItemPlayer.size();
	KG_PROCESS_SUCCESS(uPlayerCount == 0);

    if (DropData.bDropForEach)
    {
        KPlayer*	pPlayer		= NULL;	

	    for (size_t i = 0; i < uPlayerCount; ++i)
	    {
		    pPlayer = g_pSO3World->m_PlayerSet.GetObj(vecNeedQuestItemPlayer[i]);
		    if (pPlayer == NULL)
            {
			    continue;
            }
			
            pTempItem = g_pSO3World->m_ItemManager.CloneItem(pItem);
            KGLOG_PROCESS_ERROR(pTempItem);

            pAddLootItem = pLootList->AddItem(pTempItem, DropData.dwQuestID, DropData.dwQuestState);
            if (pAddLootItem == NULL)
            {
                g_pSO3World->m_ItemManager.FreeItem(pTempItem);
	            pTempItem = NULL;
                goto Exit0;
            }   

            bRetCode = pLootList->SetItemLooter(pLootList->GetItemCount() - 1, pPlayer->m_dwID);
            KGLOG_PROCESS_ERROR(bRetCode);

            pAddLootItem->eLootItemType = litOwnerLoot;
        }
    }
    else
    {
	    pTempItem = g_pSO3World->m_ItemManager.CloneItem(pItem);
	    KGLOG_PROCESS_ERROR(pTempItem);

	    pAddLootItem = pLootList->AddItem(pTempItem, DropData.dwQuestID, DropData.dwQuestState);
	    if (pAddLootItem == NULL)
        {
            g_pSO3World->m_ItemManager.FreeItem(pTempItem);
            pTempItem = NULL;
            goto Exit0;
        }
               
        if (DropData.dwQuestID != 0) // 跟任务有关的Npc掉落，设置成需要的人自由捡
        {
            for (size_t i = 0; i < uPlayerCount; ++i)
            {
                bRetCode = pLootList->SetItemLooter(pLootList->GetItemCount() - 1, vecNeedQuestItemPlayer[i]);
                KGLOG_PROCESS_ERROR(bRetCode);

                pAddLootItem->eLootItemType = litOwnerLoot;
            }
        }
    }

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

BOOL KDropCenter::AddItemToLootListAbsoluteFree(
    KLootList* pLootList, KItem* pItem, DROP_DATA& DropData, KPlayer* pOpener
)
{
    BOOL        bResult         = false;
	BOOL		bRetCode		= false;
    KItem*      pTempItem       = NULL;
    KLootItem*  pAddLootItem    = NULL;
    size_t		uPlayerCount	= 0;
    std::vector<DWORD> vecLooter;
    std::vector<DWORD> vecNeedQuestItemPlayer;
    
	KGLOG_PROCESS_ERROR(pLootList);
	KGLOG_PROCESS_ERROR(pItem);
    KGLOG_PROCESS_ERROR(pOpener);

    vecLooter.push_back(pOpener->m_dwID);

    bRetCode = GetNeedQuestItemPlayer(pItem, DropData, vecLooter, vecNeedQuestItemPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

    uPlayerCount = vecNeedQuestItemPlayer.size();
	KG_PROCESS_SUCCESS(uPlayerCount == 0);

	pTempItem = g_pSO3World->m_ItemManager.CloneItem(pItem);
	KGLOG_PROCESS_ERROR(pTempItem);

	pAddLootItem = pLootList->AddItem(pTempItem, DropData.dwQuestID, DropData.dwQuestState);
	if (pAddLootItem == NULL)
    {
        g_pSO3World->m_ItemManager.FreeItem(pTempItem);
        pTempItem = NULL;
        goto Exit0;
    }
    
    pAddLootItem->eLootItemType = litAbsoluteFree;

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

void KDropCenter::DropQuestItem(
    DWORD dwQuestID, DWORD dwTemplateID, KPlayer* pDropTarget, 
    KLootList* pLootList, std::vector<DWORD>& vecLooterList, BOOL bIsNpc
)
{
    BOOL                bRetCode                    = false; 
    size_t              uNeedQuestItemLooterCount   = 0;
    DWORD               dwTeamID                    = ERROR_ID;
	KQuestInfo*         pQuestInfo                  = NULL;
    KItem*              pItem                       = NULL;
    BOOL                bIsInRaid                   = false;
    std::vector<DWORD>  vecNeedQuestItemLooter;
    
    assert(pDropTarget);
    assert(pLootList);

    bIsInRaid = pDropTarget->IsInRaid();

    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	if (pQuestInfo == NULL)
    {
        KGLogPrintf(KGLOG_DEBUG, "The quest:[%lu] is not exist!", dwQuestID);
        goto Exit0;        
    }

    dwTeamID = pDropTarget->GetCurrentTeamID();

    if (
        (bIsInRaid && pQuestInfo->byTeamRequireMode == qtrmRequireNotRaid) ||
        (!bIsInRaid && pQuestInfo->byTeamRequireMode == qtrmRequireRaid)
    )
    {
        goto Exit0;
    }

	for (int nIndex = 0; nIndex < QUEST_END_ITEM_COUNT; ++nIndex)
	{
		DWORD           dwTabType     = pQuestInfo->byEndRequireItemType[nIndex];
		DWORD           dwItemIndex   = pQuestInfo->wEndRequireItemIndex[nIndex];
		DWORD           dwItemCount   = pQuestInfo->wEndRequireItemAmount[nIndex];
        KLootItem*      pAddLootItem  = NULL;
        
        if (bIsNpc)
        {
            if (pQuestInfo->dwDropItemNpcTemplateID[nIndex] != dwTemplateID)
            {
                continue;
            }
        }
        else // doodad
        {
            if (pQuestInfo->dwDropItemDoodadTemplateID[nIndex] != dwTemplateID)
            {
                continue;
            }
        }

        bRetCode = pDropTarget->IsQuestDrop(pQuestInfo->dwQuestID, nIndex, pQuestInfo->dwDropItemRate[nIndex]);
        if (!bRetCode)
        {
            continue;
        }

        vecNeedQuestItemLooter.clear();
        for (size_t i = 0; i < vecLooterList.size(); ++i)
        {
            KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(vecLooterList[i]);

            if (pMember == NULL)
            {
				continue;
            }

			int nQuestIndex = pMember->m_QuestList.GetQuestIndex(dwQuestID);
			if (nQuestIndex == -1)
            {
				continue;
            }
            
            bRetCode = pMember->m_QuestList.IsQuestEndItemEnoughByIndex(pQuestInfo, nIndex, true);
            if (bRetCode)
            {
                continue;
            }

            vecNeedQuestItemLooter.push_back(vecLooterList[i]);
        }
        
        uNeedQuestItemLooterCount = vecNeedQuestItemLooter.size();
        if (uNeedQuestItemLooterCount == 0)
        {
            continue;
        }

		if (pQuestInfo->bDropForEach[nIndex])
		{	
			for (size_t i = 0; i < uNeedQuestItemLooterCount; ++i)
			{
                KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(vecNeedQuestItemLooter[i]);
                KGLOG_PROCESS_ERROR(pMember);

                pItem = g_pSO3World->GenerateItem(dwTabType, dwItemIndex);
				KGLOG_PROCESS_ERROR(pItem);

				if (pItem->IsStackable())
				{
					bRetCode = pItem->SetStackNum(1);
					KGLOG_PROCESS_ERROR(bRetCode);
				}
                
                if (pItem->m_Common.nGenre == igBook)
                {
                    pItem->m_nCurrentDurability = dwItemCount; // 书的配方
                }

				pAddLootItem = pLootList->AddItem(pItem, dwQuestID, 1);
				if (pAddLootItem == NULL)
				{
					g_pSO3World->m_ItemManager.FreeItem(pItem);
					pItem = NULL;
                    continue;
				}

                pItem = NULL;
                
                pAddLootItem->eLootItemType = litOwnerLoot;
				pLootList->SetItemLooter(pLootList->GetItemCount() - 1, vecNeedQuestItemLooter[i]);
                pMember->m_QuestList.RegisterTeamAssistance(dwQuestID);
			}
        }
        else
        {
            KTeam* pTeam = NULL;
            
            assert(uNeedQuestItemLooterCount > 0);

		    pItem = g_pSO3World->GenerateItem(dwTabType, dwItemIndex);
		    KGLOG_PROCESS_ERROR(pItem);

		    if (pItem->IsStackable())
		    {
			    bRetCode = pItem->SetStackNum(1);
			    KGLOG_PROCESS_ERROR(bRetCode);
		    }
            
            if (pItem->m_Common.nGenre == igBook)
            {
                pItem->m_nCurrentDurability = dwItemCount; // 书的配方
            }

		    pAddLootItem = pLootList->AddItem(pItem, dwQuestID, 1);
            if (pAddLootItem == NULL)
            {
                g_pSO3World->m_ItemManager.FreeItem(pItem);
                pItem = NULL;			
                continue;
            }
            
            pItem = NULL;

            if (dwTeamID == ERROR_ID) // 一个人的情况
            {
                assert(pDropTarget->m_dwID == vecNeedQuestItemLooter[0]);
                pAddLootItem->eLootItemType = litOwnerLoot;
                pLootList->SetItemLooter(pLootList->GetItemCount() - 1, pDropTarget->m_dwID);
                pDropTarget->m_QuestList.RegisterTeamAssistance(dwQuestID);
                continue;
            }
            
            pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
            KGLOG_PROCESS_ERROR(pTeam);

            if (pTeam->nLootMode == ilmFreeForAll) // 队伍自由拾取模式有需求的人自由拾取
            {
                for (size_t i = 0; i < uNeedQuestItemLooterCount; ++i)
                {
                    KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(vecNeedQuestItemLooter[i]);
                    KGLOG_PROCESS_ERROR(pMember);
                    
                    pAddLootItem->eLootItemType = litOwnerLoot;
                    pLootList->SetItemLooter(pLootList->GetItemCount() - 1, vecNeedQuestItemLooter[i]);
                    pMember->m_QuestList.RegisterTeamAssistance(dwQuestID);
                }
            }
            else // 其它情况下有需求的人轮流拾取
            {
                KPlayer* pRoundLooter       = NULL;
                int      nCurMaxRoundCount  = 0;

                for (size_t i = 0; i < uNeedQuestItemLooterCount; ++i)
                {
                    int nQuestIndex = -1;
                    int nRoundCount = 0;
                    KPlayer* pMember = NULL;

                    pMember = g_pSO3World->m_PlayerSet.GetObj(vecNeedQuestItemLooter[i]);
                    KGLOG_PROCESS_ERROR(pMember);
                    
                    nRoundCount = pMember->m_QuestList.GetQuestRoundCount(dwQuestID);
                    KGLOG_PROCESS_ERROR(nRoundCount >= 0);

                    bRetCode = pMember->m_QuestList.SetQuestRoundCount(dwQuestID, ++nRoundCount);
                    KGLOG_PROCESS_ERROR(bRetCode);

                    if (nRoundCount > nCurMaxRoundCount)
                    {
                        pRoundLooter = pMember;
                        nCurMaxRoundCount = nRoundCount;
                    }
                }
                
                assert(pRoundLooter);
                bRetCode = pRoundLooter->m_QuestList.SetQuestRoundCount(dwQuestID, 0);
                KGLOG_PROCESS_ERROR(bRetCode);

                pAddLootItem->eLootItemType = litOwnerLoot;
                pLootList->SetItemLooter(pLootList->GetItemCount() - 1, pRoundLooter->m_dwID);
                pRoundLooter->m_QuestList.RegisterTeamAssistance(dwQuestID);
            }
	    }
    }

Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
    return;
}

void KDropCenter::DropQuestItemFromDoodadAbsoluteFree(
    DWORD dwQuestID, DWORD dwTemplateID, KPlayer* pDropTarget, KLootList* pLootList
)
{
    BOOL            bRetCode        = false; 
	KQuestInfo*     pQuestInfo      = NULL;
    KItem*          pItem           = NULL;
    int             nQuestIndex     = 0;

    assert(pDropTarget);
    assert(pLootList);

    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	if (pQuestInfo == NULL)
    {
        KGLogPrintf(KGLOG_DEBUG, "The quest:[%lu] is not exist!", dwQuestID);
        goto Exit0;        
    }
    
    nQuestIndex = pDropTarget->m_QuestList.GetQuestIndex(dwQuestID); // 没有接任务就不掉
	KG_PROCESS_ERROR(nQuestIndex != -1);

	for (int nIndex = 0; nIndex < QUEST_END_ITEM_COUNT; ++nIndex)
	{
		DWORD           dwTabType     = pQuestInfo->byEndRequireItemType[nIndex];
		DWORD           dwItemIndex   = pQuestInfo->wEndRequireItemIndex[nIndex];
		DWORD           dwItemCount   = pQuestInfo->wEndRequireItemAmount[nIndex];
        KLootItem*      pAddLootItem  = NULL;
        
        if (pQuestInfo->dwDropItemDoodadTemplateID[nIndex] != dwTemplateID)
        {
            continue;
        }

        bRetCode = pDropTarget->IsQuestDrop(pQuestInfo->dwQuestID, nIndex, pQuestInfo->dwDropItemRate[nIndex]);
        if (!bRetCode)
        {
            continue;
        }

        bRetCode = pDropTarget->m_QuestList.IsQuestEndItemEnoughByIndex(pQuestInfo, nIndex, true);
        if (bRetCode)
        {
            continue;
        }
 
        pItem = g_pSO3World->GenerateItem(dwTabType, dwItemIndex);
		KGLOG_PROCESS_ERROR(pItem);

		if (pItem->IsStackable())
		{
			bRetCode = pItem->SetStackNum(1);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
        
        if (pItem->m_Common.nGenre == igBook)
        {
            pItem->m_nCurrentDurability = dwItemCount; // 书的配方
        }

		pAddLootItem = pLootList->AddItem(pItem, dwQuestID, 1);
		if (pAddLootItem == NULL)
		{
			g_pSO3World->m_ItemManager.FreeItem(pItem);
			pItem = NULL;
            continue;
		}
        
        pItem = NULL;           
        pAddLootItem->eLootItemType = litAbsoluteFree;
    }

Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
    return;
}
#endif	//_SERVER

#include "stdafx.h"
#include "KDoodadTemplateList.h"

#include "KDoodad.h"

#define DOODAD_TEMPLATE_FILENAME	"DoodadTemplate.tab"

#define DOODAD_TEMPLATE_ID			"ID"
#define DOODAD_KIND_ID				"Kind"				//Doodad类型
#define DOODAD_CLASS_ID				"ClassID"			//Doodad掉落分类
#define DOODAD_LEVEL				"Level"				//Doodad等级
#define DOODAD_NAME					"Name"				//名称
#define DOODAD_MAP_NAME				"MapName"			//地图名称

#define DOODAD_SELECTABLE			"IsSelectable"		//可否被角色点选
#define DOODAD_REMOVE_DELAY			"RemoveDelay"		//物件消失时间
#define DOODAD_REVIVE_DELAY			"ReviveDelay"		//物件再生时间间隔
#define DOODAD_CRAFT_ID				"CraftID"			//能够操作的技艺ID

#define DOODAD_CAN_OPERATE_EACH		"CanOperateEach"	//是否能够多人同时操作
#define DOODAD_CAN_PICK				"CanPick"			//打开后是否进拾取流程
#define DOODAD_OPEN_PREPARE_FRAME	"OpenPrepareFrame"  //打开时间

#define DOODAD_DROP_NAME1			"Drop1"
#define DOODAD_DROP_COUNT1			"Count1"
#define DOODAD_DROP_NAME2			"Drop2"
#define DOODAD_DROP_COUNT2			"Count2"
#define DOODAD_DROP_NAME3			"Drop3"
#define DOODAD_DROP_COUNT3			"Count3"
#define DOODAD_DROP_NAME4			"Drop4"
#define DOODAD_DROP_COUNT4			"Count4"

#define DOODAD_MIN_MONEY			"MoneyMin"
#define DOODAD_MAX_MONEY			"MoneyMax"
#define DOODAD_MONEY_DROP_RATE		"MoneyDropRate"
#define DOODAD_OVER_LOOT_FRAME      "OverLootFrame"
#define DOODAD_DYNAMIC_OBSTACLE     "DynamicObstacle"

#define DOODAD_REPRESENT_ID			"RepresentID"		//表现逻辑ID
#define DOODAD_BAR_TEXT				"BarText"
#define DOODAD_SCRIPT               "Script"
#define DOODAD_PROGRESS_ID          "ProgressID"

KDoodadTemplateList::KDoodadTemplateList()
{
	m_pDoodadTemplateList = NULL;
	m_nDoodadTemplateCount = 0;
}

KDoodadTemplateList::~KDoodadTemplateList()
{
	assert(m_pDoodadTemplateList == NULL);
	assert(m_nDoodadTemplateCount == 0);
}

BOOL KDoodadTemplateList::Init()
{
    BOOL            bResult         = false;
	BOOL            bRetCode        = false;
	ITabFile*       piTabFile       = NULL;
	int             nHeight         = 0;
    int             nDoodadCount    = 0;
    KDoodadTemplate DoodadTemplate;
	char            szFilePath[MAX_PATH];

	snprintf(szFilePath, sizeof(szFilePath), "%s/%s", SETTING_DIR, DOODAD_TEMPLATE_FILENAME);
	szFilePath[sizeof(szFilePath) - 1] = '\0';

	piTabFile = g_OpenTabFile(szFilePath);
	KGLOG_PROCESS_ERROR(piTabFile);

	nHeight = piTabFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);
    
	memset(&m_DefaultDoodadTemplate, 0, sizeof(m_DefaultDoodadTemplate));

	bRetCode = LoadDoodadTemplate(2, piTabFile, m_DefaultDoodadTemplate);
	KGLOG_PROCESS_ERROR(bRetCode);

	KG_PROCESS_SUCCESS(nHeight == 2);
    
    nDoodadCount = nHeight - 2;

	m_pDoodadTemplateList = new KDoodadTemplate[nDoodadCount];
	KGLOG_PROCESS_ERROR(m_pDoodadTemplateList);
	
	for (int i = 0; i < nDoodadCount; ++i)
	{
        memset(&DoodadTemplate, 0, sizeof(DoodadTemplate));

		bRetCode = LoadDoodadTemplate(i + 3, piTabFile, DoodadTemplate);
		KGLOG_PROCESS_ERROR(bRetCode);

		if (DoodadTemplate.m_dwTemplateID == 0)
		{
			KGLogPrintf(
                KGLOG_ERR, "[KDoodadTemplateList]Line:%d ID:(%d) in %s was repeated!",
				i + 3, DoodadTemplate.m_dwTemplateID, DOODAD_TEMPLATE_FILENAME
            );
			goto Exit0;
		}

        m_pDoodadTemplateList[i] = DoodadTemplate;
		m_mapDoodadTemplateList[DoodadTemplate.m_dwTemplateID] = m_pDoodadTemplateList + i;
	}

	m_nDoodadTemplateCount = nDoodadCount;

Exit1:
	bResult = true;
Exit0:
    if (!bResult)
    {
        SAFE_DELETE_ARRAY(m_pDoodadTemplateList);
    }
	KG_COM_RELEASE(piTabFile);
	return bResult;
}

void KDoodadTemplateList::UnInit(void)
{
	m_nDoodadTemplateCount = 0;
	m_mapDoodadTemplateList.clear();
	KG_DELETE_ARRAY(m_pDoodadTemplateList);
}

BOOL KDoodadTemplateList::LoadFromTemplate(DWORD dwTemplateID, KDoodad* pDoodad)
{
    BOOL                bResult     = false;
	BOOL                bRetCode    = false;
	KDoodadTemplate*    pTemplate   = NULL;

	pTemplate = GetTemplate(dwTemplateID);
	if (pTemplate == NULL)
	{
        KGLogPrintf(KGLOG_ERR, "Invalid doodad template: %u\n", dwTemplateID);

		pTemplate = GetTemplate(0);
		KGLOG_PROCESS_ERROR(pTemplate);
	}

	pDoodad->m_dwTemplateID     = dwTemplateID;
	pDoodad->m_eKind            = pTemplate->m_eKind;
	pDoodad->m_nOpenFrames      = pTemplate->m_dwOpenPrepareFrame;
	pDoodad->m_dwRepresentID    = pTemplate->m_dwRepresentID;
	pDoodad->m_pTemplate        = pTemplate;

	if (pTemplate->szScript[0] != '\0')
	{
		pDoodad->m_dwScriptID = g_FileNameHash(pTemplate->szScript);
	}

	strncpy(pDoodad->m_szName, pTemplate->m_szName, _NAME_LEN);
	pDoodad->m_szName[_NAME_LEN - 1] = 0;

	bResult = true;
Exit0:
	return bResult;
}

KDoodadTemplate* KDoodadTemplateList::GetTemplate(DWORD dwTemplateID)
{
	KDoodadTemplate* pResult = NULL;
	KDOODAD_TEMPLATE_LIST::iterator iter;

	if (dwTemplateID == 0)
	{
		pResult = &(m_DefaultDoodadTemplate);
		goto Exit0;
	}

	iter = m_mapDoodadTemplateList.find(dwTemplateID);
	if (iter != m_mapDoodadTemplateList.end())
	{
		pResult = iter->second;
	}
Exit0:
	return pResult;
}

KDoodadTemplate* KDoodadTemplateList::GetTemplateByIndex(int nIndex)
{
	KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_nDoodadTemplateCount);
	KGLOG_PROCESS_ERROR(m_pDoodadTemplateList);

	return &(m_pDoodadTemplateList[nIndex]);
Exit0:
	return NULL;
}

int KDoodadTemplateList::GetDoodadTemplateCount()
{
	return m_nDoodadTemplateCount;
}

BOOL KDoodadTemplateList::LoadDoodadTemplate(int nIndex, ITabFile* piTabFile, KDoodadTemplate& fDoodadTemplate)
{
    BOOL bResult  = false;
	BOOL bRetCode = false;

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_TEMPLATE_ID,
		m_DefaultDoodadTemplate.m_dwTemplateID, (int*)&(fDoodadTemplate.m_dwTemplateID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nIndex, DOODAD_NAME, 
		m_DefaultDoodadTemplate.m_szName, fDoodadTemplate.m_szName, _NAME_LEN);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nIndex, DOODAD_MAP_NAME,
		m_DefaultDoodadTemplate.m_szMapName, fDoodadTemplate.m_szMapName, _NAME_LEN);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_KIND_ID,
		m_DefaultDoodadTemplate.m_eKind, (int*)&fDoodadTemplate.m_eKind);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_CLASS_ID, 
		m_DefaultDoodadTemplate.m_dwClassID, (int*)&fDoodadTemplate.m_dwClassID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_SELECTABLE, 
		m_DefaultDoodadTemplate.m_bSelectable, (int*)&(fDoodadTemplate.m_bSelectable));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_REMOVE_DELAY, 
		m_DefaultDoodadTemplate.m_nRemoveDelay, (int*)&(fDoodadTemplate.m_nRemoveDelay));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_REVIVE_DELAY, 
		m_DefaultDoodadTemplate.m_nReviveDelay, (int*)&(fDoodadTemplate.m_nReviveDelay));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_CAN_OPERATE_EACH, 
		m_DefaultDoodadTemplate.m_bCanOperateEach, (int*)&(fDoodadTemplate.m_bCanOperateEach));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_CAN_PICK, 
		m_DefaultDoodadTemplate.m_bCanPick, (int*)&(fDoodadTemplate.m_bCanPick));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_OPEN_PREPARE_FRAME,
		m_DefaultDoodadTemplate.m_dwOpenPrepareFrame, (int*)&(fDoodadTemplate.m_dwOpenPrepareFrame));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_CRAFT_ID,
		m_DefaultDoodadTemplate.m_dwCraftID, (int*)&(fDoodadTemplate.m_dwCraftID));
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nIndex, DOODAD_DROP_NAME1, 
		m_DefaultDoodadTemplate.m_szDropName[0], fDoodadTemplate.m_szDropName[0], MAX_PATH);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_DROP_COUNT1,
		m_DefaultDoodadTemplate.m_dwDropCount[0], (int*)&fDoodadTemplate.m_dwDropCount[0]);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nIndex, DOODAD_DROP_NAME2, 
		m_DefaultDoodadTemplate.m_szDropName[1], fDoodadTemplate.m_szDropName[1], MAX_PATH);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_DROP_COUNT2,
		m_DefaultDoodadTemplate.m_dwDropCount[1], (int*)&fDoodadTemplate.m_dwDropCount[1]);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nIndex, DOODAD_DROP_NAME3, 
		m_DefaultDoodadTemplate.m_szDropName[2], fDoodadTemplate.m_szDropName[2], MAX_PATH);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_DROP_COUNT3,
		m_DefaultDoodadTemplate.m_dwDropCount[2], (int*)&fDoodadTemplate.m_dwDropCount[2]);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(nIndex, DOODAD_DROP_NAME4, 
		m_DefaultDoodadTemplate.m_szDropName[3], fDoodadTemplate.m_szDropName[3], MAX_PATH);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_DROP_COUNT4,
		m_DefaultDoodadTemplate.m_dwDropCount[3], (int*)&fDoodadTemplate.m_dwDropCount[3]);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(nIndex, DOODAD_LEVEL, 
		m_DefaultDoodadTemplate.m_nLevel, (int*)&fDoodadTemplate.m_nLevel);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, DOODAD_OVER_LOOT_FRAME, m_DefaultDoodadTemplate.m_nOverLootFrame, &fDoodadTemplate.m_nOverLootFrame
    );
    KGLOG_PROCESS_ERROR(bRetCode);
    
    bRetCode = piTabFile->GetInteger(
        nIndex, DOODAD_DYNAMIC_OBSTACLE, m_DefaultDoodadTemplate.m_bDynamicObstacle, &fDoodadTemplate.m_bDynamicObstacle
    );
    KGLOG_PROCESS_ERROR(bRetCode);

#if defined(_SERVER)
	bRetCode = piTabFile->GetInteger(
        nIndex, DOODAD_MIN_MONEY, m_DefaultDoodadTemplate.m_nMinMoney, &fDoodadTemplate.m_nMinMoney
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, DOODAD_MAX_MONEY, m_DefaultDoodadTemplate.m_nMaxMoney, &fDoodadTemplate.m_nMaxMoney
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetInteger(
        nIndex, DOODAD_MONEY_DROP_RATE, m_DefaultDoodadTemplate.m_nMoneyRate, &fDoodadTemplate.m_nMoneyRate
    );
	KGLOG_PROCESS_ERROR(bRetCode);
#endif //_SERVER

#if defined(_CLIENT)
	bRetCode = piTabFile->GetString(
        nIndex, DOODAD_BAR_TEXT, m_DefaultDoodadTemplate.m_szBarText, fDoodadTemplate.m_szBarText, _NAME_LEN
    );
	KGLOG_PROCESS_ERROR(bRetCode);
#endif //_CLIENT

	bRetCode = piTabFile->GetInteger(
        nIndex, DOODAD_REPRESENT_ID, m_DefaultDoodadTemplate.m_dwRepresentID, (int*)&(fDoodadTemplate.m_dwRepresentID)
    );
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = piTabFile->GetString(
        nIndex, DOODAD_SCRIPT, m_DefaultDoodadTemplate.szScript, fDoodadTemplate.szScript, sizeof(fDoodadTemplate.szScript)
    );
	KGLOG_PROCESS_ERROR(bRetCode);
	
#ifdef _SERVER
    bRetCode = piTabFile->GetInteger(
        nIndex, DOODAD_PROGRESS_ID, m_DefaultDoodadTemplate.m_nProgressID, (int*)&fDoodadTemplate.m_nProgressID
    );
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

	bResult = true;
Exit0:
	return bResult;
}

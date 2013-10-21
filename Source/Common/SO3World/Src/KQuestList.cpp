#include "stdafx.h"
#include "Global.h"
#include "KQuestList.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "KPlayerServer.h"
#include "KMath.h"
#include "KRegion.h"
#include "KScene.h"

#ifdef _CLIENT
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "SO3Represent/SO3RepresentHandler.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#endif

#define MAX_ACCEPT_QUEST_DISTANCE	(CELL_LENGTH * REGION_GRID_WIDTH * CELL_LENGTH * REGION_GRID_WIDTH)
#define MAX_QUEST_ASSIST_DISTANCE   (CELL_LENGTH * 64)

BOOL KQuestList::Init(KPlayer* pPlayer)
{
	assert(pPlayer);

	m_nAcceptedCount    = 0;
    m_nAssistDailyCount = 0;
	m_pPlayer           = pPlayer;

	memset(m_byQuestStateList, 0, sizeof(m_byQuestStateList));
	memset(m_AcceptedQuestList, 0, sizeof(m_AcceptedQuestList));
    memset(m_DailyQuest, 0, sizeof(m_DailyQuest));

	m_mapNpc2Quest.clear();

#ifdef _SERVER
    m_AssistQuestList.clear();
#endif

    return true;
}

void KQuestList::UnInit()
{
	m_mapNpc2Quest.clear();
#ifdef _SERVER
    m_AssistQuestList.clear();
#endif
}

void KQuestList::Activate()
{    
    assert(g_pSO3World);
    assert(m_pPlayer);

	if ((g_pSO3World->m_nGameLoop - m_pPlayer->m_dwID) % GAME_FPS == 0) // 每秒更新一次
	{
        ProcessLimitTimeQuest();
    }
    
    if ((g_pSO3World->m_nGameLoop - m_pPlayer->m_dwID) % (GAME_FPS * 10) == 0)
	{
        ProcessDailyQuest();
    }

    return;
}

BOOL KQuestList::LoadQuestState(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen)
{
    BOOL    bResult         = false;
    size_t  uLeftSize       = uDataLen;
    BYTE*   pbyOffset       = pbyData;
    int     nDwordBitCount  = sizeof(DWORD) * CHAR_BIT;

    for (int nQuestIndex = 0; nQuestIndex < MAX_QUEST_COUNT / nDwordBitCount; nQuestIndex++)
    {
        DWORD dwQuestState = 0;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD));
        dwQuestState = *(DWORD*)pbyOffset;
        uLeftSize -= sizeof(DWORD);
        pbyOffset += sizeof(DWORD);

        for (int nIndex = 0; nIndex < nDwordBitCount; nIndex++)
        {
            if (dwQuestState & (0x01 << nIndex))
            {
                m_byQuestStateList[nQuestIndex * nDwordBitCount + nIndex] = qsFinished;
            }
        }
    }

    *puUsedSize = uDataLen - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::LoadQuestList(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen)
{
    BOOL    bResult             = false;
    size_t  uLeftSize           = uDataLen;
    BYTE*   pbyOffset           = pbyData;
    int     nDBAcceptedCount    = 0;
    int     nDBAssistCount      = 0;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(BYTE));
    nDBAcceptedCount = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    KGLOG_PROCESS_ERROR(nDBAcceptedCount <= MAX_ACCEPT_QUEST_COUNT);

	// 载入已接任务表
    for (int nQuestIndex = 0; nQuestIndex < nDBAcceptedCount; nQuestIndex++)
    {
        KDB_ACCEPT_QUEST*   pDBQuest    = NULL;
        KQuestInfo*         pQuestInfo  = NULL;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_ACCEPT_QUEST));
        pDBQuest = (KDB_ACCEPT_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_ACCEPT_QUEST);
        pbyOffset += sizeof(KDB_ACCEPT_QUEST);

        pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(pDBQuest->wQuestID);
        if (pQuestInfo == NULL)
        {
            // 任务已经被策划删除了
            continue;
        }

        m_AcceptedQuestList[nQuestIndex].pQuestInfo = pQuestInfo;

        for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
        {
            m_AcceptedQuestList[nQuestIndex].dwKillNpcCount[nIndex] = pDBQuest->byKillNpcCount[nIndex];
        }

        for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT * 2; nIndex++)
        {
            m_AcceptedQuestList[nQuestIndex].nQuestValue[nIndex] = pDBQuest->nQuestValue[nIndex];
        }

        m_AcceptedQuestList[nQuestIndex].bFailed    = pDBQuest->byFailed;

        m_AcceptedQuestList[nQuestIndex].nLimitTime = pDBQuest->nLimitTime;

        for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
        {
            KQuestRequirement QuestRequirement;

            if (pQuestInfo->dwKillNpcTemplateID[nIndex] == 0)
            {
                continue;
            }

            QuestRequirement.nQuestIndex = nQuestIndex;
            QuestRequirement.nParamIndex = nIndex;

            m_mapNpc2Quest.insert(
                std::make_pair(pQuestInfo->dwKillNpcTemplateID[nIndex], QuestRequirement)
            );
        }

        m_nAcceptedCount++;
    }

    *puUsedSize = uDataLen - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::LoadDailyQuest(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult             = false;
    KDB_DAILY_QUEST_DATA*   pDailyQuestData     = NULL;
    int                     nDailyQuestCount    = 0;
    
    assert(puUsedSize);
    assert(pbyData);
    
    KGLOG_PROCESS_ERROR(uDataLen >= sizeof(KDB_DAILY_QUEST_DATA));
    pDailyQuestData = (KDB_DAILY_QUEST_DATA*)pbyData;

    KGLOG_PROCESS_ERROR(uDataLen >= (sizeof(KDB_DAILY_QUEST_DATA) + pDailyQuestData->byCount * sizeof(KDB_DAILY_QUEST)));
    
    nDailyQuestCount = pDailyQuestData->byCount;
    if (nDailyQuestCount > MAX_DAILY_QUEST_COUNT)
    {
        nDailyQuestCount = MAX_DAILY_QUEST_COUNT;
    }

    for (int i = 0; i < nDailyQuestCount; ++i)
    {
        m_DailyQuest[i].dwQuestID = pDailyQuestData->DailyQuest[i].wQuestID;
        m_DailyQuest[i].nNextAcceptTime = pDailyQuestData->DailyQuest[i].nNextAcceptTime;
        
        if (m_DailyQuest[i].dwQuestID != 0 && m_DailyQuest[i].nNextAcceptTime == 0)
        {
            int nQuestIndex = GetQuestIndex(m_DailyQuest[i].dwQuestID);
            if (nQuestIndex == -1)
            {
                m_DailyQuest[i].dwQuestID = 0; // 任务已经不存在
            }
        }
    }
    
    *puUsedSize = sizeof(KDB_DAILY_QUEST_DATA) + pDailyQuestData->byCount * sizeof(KDB_DAILY_QUEST);

    bResult = true;
Exit0:
    return bResult;
}

#ifdef _SERVER
BOOL KQuestList::SaveQuestState(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL    bResult         = false;
    size_t  uLeftSize       = uBufferSize;
    BYTE*   pbyOffset       = pbyBuffer;
    int     nDwordBitCount  = sizeof(DWORD) * CHAR_BIT;

    for (int nQuestIndex = 0; nQuestIndex < MAX_QUEST_COUNT / nDwordBitCount; nQuestIndex++)
    {
        DWORD*  pdwData         = NULL;
        DWORD   dwQuestState    = 0;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(DWORD));
        pdwData = (DWORD*)pbyOffset;
        uLeftSize -= sizeof(DWORD);
        pbyOffset += sizeof(DWORD);

        for (int nIndex = 0; nIndex < nDwordBitCount; nIndex++)
        {
            if (m_byQuestStateList[nQuestIndex * nDwordBitCount + nIndex] == qsFinished)
            {
                dwQuestState |= (0x01 << nIndex);
            }
        }

        *pdwData = dwQuestState;
    }

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::SaveQuestList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL    bResult             = false;
    size_t  uLeftSize           = uBufferSize;
    BYTE*   pbyOffset           = pbyBuffer;
    int     nAcceptedCount      = 0;
    BYTE*   pbyAcceptedCount    = NULL;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(BYTE));
    pbyAcceptedCount = (BYTE*)pbyOffset;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    for (int nQuestIndex = 0; nQuestIndex < MAX_ACCEPT_QUEST_COUNT; nQuestIndex++)
    {
        KDB_ACCEPT_QUEST* pDBQuest = NULL;

        if (m_AcceptedQuestList[nQuestIndex].pQuestInfo == NULL)
        {
            continue;
        }

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_ACCEPT_QUEST));
        pDBQuest = (KDB_ACCEPT_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_ACCEPT_QUEST);
        pbyOffset += sizeof(KDB_ACCEPT_QUEST);

        nAcceptedCount++;

        pDBQuest->wQuestID = (WORD)m_AcceptedQuestList[nQuestIndex].pQuestInfo->dwQuestID;

        for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
        {
            pDBQuest->byKillNpcCount[nIndex] = (BYTE)m_AcceptedQuestList[nQuestIndex].dwKillNpcCount[nIndex];
        }

        for (int nIndex = 0; nIndex < (QUEST_PARAM_COUNT * 2); nIndex++)
        {
            pDBQuest->nQuestValue[nIndex] = m_AcceptedQuestList[nQuestIndex].nQuestValue[nIndex];
        }

		pDBQuest->byFailed = (BYTE)(m_AcceptedQuestList[nQuestIndex].bFailed);

		pDBQuest->nLimitTime = m_AcceptedQuestList[nQuestIndex].nLimitTime;
    }
  
    assert(nAcceptedCount <= UCHAR_MAX);
	*pbyAcceptedCount = (BYTE)nAcceptedCount;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::SaveDailyQuest(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL                    bResult         = false;
    size_t                  uLeftSize       = uBufferSize;
    KDB_DAILY_QUEST_DATA*   pDailyQuestData = NULL;
    KDB_DAILY_QUEST*        pDailyQuest     = NULL;
    int                     nSaveIndex      = 0;
    
    assert(puUsedSize);
    assert(pbyBuffer);
    
    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_DAILY_QUEST_DATA));
    pDailyQuestData = (KDB_DAILY_QUEST_DATA*)pbyBuffer;
    uLeftSize -= sizeof(KDB_DAILY_QUEST_DATA);

    for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
    {
        if (m_DailyQuest[i].dwQuestID == 0)
        {
            continue;
        }

        if (m_DailyQuest[i].nNextAcceptTime != 0 && g_pSO3World->m_nCurrentTime > m_DailyQuest[i].nNextAcceptTime)
        {
            continue;
        }
        
        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_DAILY_QUEST));

        pDailyQuest = &(pDailyQuestData->DailyQuest[nSaveIndex++]);
        
        pDailyQuest->wQuestID        = (WORD)m_DailyQuest[i].dwQuestID;
        pDailyQuest->nNextAcceptTime = m_DailyQuest[i].nNextAcceptTime;

        uLeftSize -= sizeof(KDB_DAILY_QUEST);
    }
    
    pDailyQuestData->byCount = (BYTE)nSaveIndex;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::LoadAssistQuestList(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen)
{
    BOOL    bResult             = false;
    size_t  uLeftSize           = uDataLen;
    BYTE*   pbyOffset           = pbyData;
    int     nDBAssistCount      = 0;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(BYTE));
    nDBAssistCount = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(BYTE));
    m_nAssistDailyCount = *(BYTE*)pbyOffset;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    KGLOG_PROCESS_ERROR(nDBAssistCount <= MAX_ASSIST_QUEST_COUNT);

    for (int i = 0; i < nDBAssistCount; i++)
    {
        KDB_ASSIST_QUEST* pDBQuest = NULL;
        KASSIST_QUEST     AssistQuest;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_ASSIST_QUEST));
        pDBQuest = (KDB_ASSIST_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_ASSIST_QUEST);
        pbyOffset += sizeof(KDB_ASSIST_QUEST);

        AssistQuest.dwPlayerID = pDBQuest->dwPlayerID;
        AssistQuest.dwQuestID  = pDBQuest->wQuestID;

        m_AssistQuestList.push_back(AssistQuest);
    }
    
    *puUsedSize = uDataLen - uLeftSize;

    assert(m_pPlayer);
    g_PlayerServer.DoMessageNotify(
        m_pPlayer->m_nConnIndex, ectSetQuestAssistDailyCount, m_nAssistDailyCount
    );

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::SaveAssistQuestList(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL    bResult             = false;
    size_t  uLeftSize           = uBufferSize;
    BYTE*   pbyOffset           = pbyBuffer;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(BYTE));
    assert(m_AssistQuestList.size() <= UCHAR_MAX);
    *(BYTE*)pbyOffset = (BYTE)m_AssistQuestList.size();
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(BYTE));
    assert(m_nAssistDailyCount <= UCHAR_MAX);
    *(BYTE*)pbyOffset = (BYTE)m_nAssistDailyCount;
    uLeftSize -= sizeof(BYTE);
    pbyOffset += sizeof(BYTE);

    for (KASSIST_QUEST_LIST::iterator it = m_AssistQuestList.begin(); it != m_AssistQuestList.end(); ++it)
    {
        KDB_ASSIST_QUEST* pAssistQuest = NULL;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KDB_ASSIST_QUEST));
        pAssistQuest = (KDB_ASSIST_QUEST*)pbyOffset;
        uLeftSize -= sizeof(KDB_ASSIST_QUEST);
        pbyOffset += sizeof(KDB_ASSIST_QUEST);

        pAssistQuest->dwPlayerID    = it->dwPlayerID;
        pAssistQuest->wQuestID      = (WORD)it->dwQuestID;
    }

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}
#endif

QUEST_RESULT_CODE KQuestList::CanAccept(KQuestInfo* pQuestInfo, KTarget* pTarget)
{
    QUEST_RESULT_CODE   nResult         = qrcFailed;
    QUEST_RESULT_CODE   eRetCode        = qrcFailed;
    BOOL                bRetCode        = false;
    BOOL                bShared         = false;
    TARGET_TYPE         eTargetType     = ttInvalid;
	int					nQuestIndex     = 0;
    int                 nFreeRoomSize   = 0;
    int                 nTime           = 0;
    int                 nOfferItemCount = 0;
    struct tm*          pLocalTime      = NULL;
    DWORD               dwExistAmount   = 0;
    
    KGLOG_PROCESS_ERROR(pTarget);
	KGLOG_PROCESS_ERROR(pQuestInfo);
	KGLOG_PROCESS_ERROR(pQuestInfo->dwQuestID < MAX_QUEST_COUNT);

    assert(m_pPlayer);

	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_eMoveState != cmsOnDeath, qrcFailed);
    
    // 检测是否已经接受该任务
	for (nQuestIndex = 0; nQuestIndex < MAX_ACCEPT_QUEST_COUNT; nQuestIndex++)
	{
		KG_PROCESS_ERROR_RET_CODE(
			m_AcceptedQuestList[nQuestIndex].pQuestInfo == NULL || 
			m_AcceptedQuestList[nQuestIndex].pQuestInfo->dwQuestID != pQuestInfo->dwQuestID,
			qrcAlreadyAcceptQuest
        );
	}
	
    KG_PROCESS_ERROR_RET_CODE(pQuestInfo->bAccept, qrcNoNeedAccept);
    
    if (pQuestInfo->bRequireApprentice)
    {
#ifdef _CLIENT
        bRetCode = g_pSO3World->m_MentorClient.IsApprentice();
#else
        bRetCode = g_pSO3World->m_MentorCache.IsApprentice(m_pPlayer->m_dwID);
#endif
        KG_PROCESS_ERROR_RET_CODE(bRetCode, qrcRequireApprentice);
    }

    eRetCode = CommonCheck(pQuestInfo);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);

    eTargetType = pTarget->GetTargetType();

    if (eTargetType == ttNpc)
    {
        KNpc*   pAcceptNpc = NULL;

        bRetCode = pTarget->GetTarget(&pAcceptNpc);
        KGLOG_PROCESS_ERROR(bRetCode);
        KGLOG_PROCESS_ERROR(pAcceptNpc);
        
        if (pAcceptNpc->m_pTemplate && pAcceptNpc->m_pTemplate->nDailyQuestCycle > 0 && pQuestInfo->bRepeat)
        {
            eRetCode = CanAcceptDailyQuest(pQuestInfo, pAcceptNpc);
            KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
        }
    }

#ifdef _SERVER
	// 判断任务是否已经满了。玩家任务满了，客户端也要有显示，所以这个条件客户端不检查
	nQuestIndex = 0;
	while ((nQuestIndex < MAX_ACCEPT_QUEST_COUNT) && m_AcceptedQuestList[nQuestIndex].pQuestInfo)
	{
		++nQuestIndex;
	}
	KG_PROCESS_ERROR_RET_CODE(nQuestIndex < MAX_ACCEPT_QUEST_COUNT, qrcQuestListFull);

	if (eTargetType == ttPlayer)
	{
        // 检测是否可以共享任务或者是否团队任务
		if (pQuestInfo->bShare || pQuestInfo->bEscortQuest)
		{
            KPlayer* pPlayer = NULL;
            bRetCode = pTarget->GetTarget(&pPlayer);
            KGLOG_PROCESS_ERROR(bRetCode && pPlayer);

			for (int nIndex = 0; nIndex < MAX_ACCEPT_QUEST_COUNT; nIndex++)
			{
                DWORD dwQuestID = pPlayer->m_QuestList.GetQuestID(nIndex);
				if (dwQuestID == pQuestInfo->dwQuestID)
				{
					bShared = true;
					break;
				}
			}
		}
        
		KG_PROCESS_ERROR_RET_CODE(bShared, qrcFailed);
	}
#endif
#ifdef _CLIENT
    if (eTargetType == ttPlayer)
    {
        bShared = true;
    }
#endif

    if (!bShared) // 非共享的任务才需要做下面的检查
    {
        // 检测目标NPC身上是否有该任务
	    if (pQuestInfo->dwStartNpcTemplateID != 0)
	    {
            KNpc*   pNpc        = NULL;
            int     nDistance2  = 0;

		    KG_PROCESS_ERROR_RET_CODE(eTargetType == ttNpc, qrcErrorStartNpcTarget);
            
            bRetCode = pTarget->GetTarget(&pNpc);
            KGLOG_PROCESS_ERROR_RET_CODE(bRetCode && pNpc, qrcErrorStartNpcTarget);

		    KGLOG_PROCESS_ERROR(pNpc->m_pScene == m_pPlayer->m_pScene);

#ifdef _SERVER // 客户端不用检查
		    nDistance2 = g_GetDistance2(pNpc->m_nX, pNpc->m_nY, m_pPlayer->m_nX, m_pPlayer->m_nY);
		    KG_PROCESS_ERROR_RET_CODE(nDistance2 < MAX_ACCEPT_QUEST_DISTANCE, qrcErrorStartNpcTarget);
#endif

		    KG_PROCESS_ERROR_RET_CODE(pNpc->m_dwTemplateID == pQuestInfo->dwStartNpcTemplateID, qrcErrorStartNpcTarget);
	    }

	    // 检测目标Doodad身上是否有该任务
	    if (pQuestInfo->dwStartDoodadTemplateID != 0)
	    {
            KDoodad* pDoodad    = NULL;
            int      nDistance2 = 0;

		    KG_PROCESS_ERROR_RET_CODE(eTargetType == ttDoodad, qrcErrorStartDoodadTarget);

            bRetCode = pTarget->GetTarget(&pDoodad);
            KGLOG_PROCESS_ERROR_RET_CODE(bRetCode && pDoodad, qrcErrorStartDoodadTarget);

		    KGLOG_PROCESS_ERROR(pDoodad->m_pScene == m_pPlayer->m_pScene);

#ifdef _SERVER
		    nDistance2 = g_GetDistance2(pDoodad->m_nX , pDoodad->m_nY, m_pPlayer->m_nX, m_pPlayer->m_nY);
		    KG_PROCESS_ERROR_RET_CODE(nDistance2 < MAX_ACCEPT_QUEST_DISTANCE, qrcErrorStartDoodadTarget);
#endif

            KG_PROCESS_ERROR_RET_CODE(pDoodad->m_dwTemplateID == pQuestInfo->dwStartDoodadTemplateID, qrcErrorStartDoodadTarget);
	    }

        // 检测玩家身上是否有任务起始道具
	    if ((pQuestInfo->byStartItemType != 0  || pQuestInfo->wStartItemIndex != 0) && !bShared)
	    {
		    eTargetType = pTarget->GetTargetType();

		    DWORD dwAmount = m_pPlayer->m_ItemList.GetItemAmountInEquipAndPackage(
                pQuestInfo->byStartItemType, pQuestInfo->wStartItemIndex
            );
		    KG_PROCESS_ERROR_RET_CODE(dwAmount > 0 && eTargetType == ttItem, qrcNoStartQuestItem);
	    }
    }

	// 检测接任务时段限制
	pLocalTime = localtime(&g_pSO3World->m_nCurrentTime);
	nTime = pLocalTime->tm_hour * 3600 + pLocalTime->tm_min * 60 + pLocalTime->tm_sec;
	KG_PROCESS_ERROR_RET_CODE(nTime >= pQuestInfo->nStartTime, qrcTooEarlyTime);
	KG_PROCESS_ERROR_RET_CODE(nTime <= pQuestInfo->nEndTime, qrcTooLateTime);
	
#ifdef _SERVER
	nOfferItemCount = 0;
	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		if (pQuestInfo->byOfferItemType[nIndex] != 0 || pQuestInfo->wOfferItemIndex[nIndex] != 0)
		{
            // 检查接任务发的道具的拥有限制，这里不考虑多个发放道具是同类道具的情况
            KItemInfo* pItemInfo = g_pSO3World->m_ItemManager.GetItemInfo(
                pQuestInfo->byOfferItemType[nIndex], pQuestInfo->wOfferItemIndex[nIndex]
            );
            KGLOG_PROCESS_ERROR(pItemInfo);
            
            if (pItemInfo->nMaxExistAmount > 0)
            {
                dwExistAmount = m_pPlayer->m_ItemList.GetItemTotalAmount(
                    pQuestInfo->byOfferItemType[nIndex], pQuestInfo->wOfferItemIndex[nIndex]
                );

                KG_PROCESS_ERROR_RET_CODE(
                    dwExistAmount + pQuestInfo->wOfferItemAmount[nIndex] <= (DWORD)pItemInfo->nMaxExistAmount,
                    qrcTooManyQuestOfferItem
                );
            }
            
			nOfferItemCount++;
		}
	}

    nFreeRoomSize = m_pPlayer->m_ItemList.GetAllPackageFreeRoomSize();
    KG_PROCESS_ERROR_RET_CODE(nOfferItemCount <= nFreeRoomSize, qrcNotEnoughFreeRoom);
    
    if (pQuestInfo->bEscortQuest && pTarget->GetTargetType() == ttNpc)
    {
        KGLOG_PROCESS_ERROR(m_pPlayer->m_pScene);
        bRetCode = m_pPlayer->m_pScene->m_EscortQuestMgr.CanAcceptEscortQuest(pQuestInfo->dwQuestID);
        KG_PROCESS_ERROR_RET_CODE(bRetCode, qrcEscortQuestIsRunning);
    }
#endif
    
	nResult = qrcSuccess;
Exit0:
	return nResult;
}

time_t KQuestList::GetDailyQuestNextAcceptTime(int nDailyQuestCycle, int nDailyQuestOffset)
{
    time_t  nResult         = 0;
    time_t  nGSCurrentTime  = g_pSO3World->m_nCurrentTime;

#ifdef _CLIENT
    nGSCurrentTime = g_pSO3World->m_nBaseTime + (g_pSO3World->m_nGameLoop / GAME_FPS);
#endif

    nResult = nGSCurrentTime - (nGSCurrentTime - nDailyQuestOffset) % nDailyQuestCycle + nDailyQuestCycle;

    return nResult;
}

QUEST_RESULT_CODE KQuestList::CanAcceptDailyQuest(KQuestInfo* pQuestInfo, KNpc* pAcceptNpc)
{
    QUEST_RESULT_CODE  nResult               = qrcFailed;
    BOOL               bRetCode              = false; 
    BOOL               bDailyQuestNotFull    = false;
    unsigned           uOldRandSeed          = 0;
    unsigned           uRandSeed             = 0;
    unsigned           uRandNum              = 0;
    size_t             uDailyQuestCount      = 0;
    std::vector<DWORD> vecQuestID;
    std::vector<DWORD> vecDailyQuestID;

    assert(pQuestInfo);
    assert(pAcceptNpc);

    for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
    {
        if (m_DailyQuest[i].dwQuestID == 0)
        {
            bDailyQuestNotFull = true;
            continue;
        }

        if (m_DailyQuest[i].dwQuestID == pQuestInfo->dwQuestID)
        {
            if (m_DailyQuest[i].nNextAcceptTime == 0)
            {
                nResult = qrcAlreadyAcceptQuest;
            }
            goto Exit0;
        }
    }

#ifdef _SERVER
    KG_PROCESS_ERROR_RET_CODE(bDailyQuestNotFull, qrcDailyQuestFull);
#endif
    
    KGLOG_PROCESS_ERROR(pAcceptNpc->m_pTemplate);

    if (!(pAcceptNpc->m_pTemplate->bIsRandomDailyQuest)) // 随机每日任务
    {
        goto Exit1;
    }

    bRetCode = g_pSO3World->m_Settings.m_QuestInfoList.GetNpcQuest(pAcceptNpc->m_dwTemplateID, &vecQuestID);
    KGLOG_PROCESS_ERROR(bRetCode);

    for (size_t i = 0; i < vecQuestID.size(); ++i)
    {
        KQuestInfo* pNpcQuest = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(vecQuestID[i]);
        KGLOG_PROCESS_ERROR(pNpcQuest);

        if (pNpcQuest->bRepeat)
        {
            for (int j = 0; j < MAX_DAILY_QUEST_COUNT; ++j)
            {
                if (m_DailyQuest[j].dwQuestID == pNpcQuest->dwQuestID)
                {
                    // 已经接了这个NPC的某个每日任务
                    goto Exit0;
                }
            }

            vecDailyQuestID.push_back(pNpcQuest->dwQuestID);
        }
    }
    
    uDailyQuestCount = vecDailyQuestID.size();
    KG_PROCESS_SUCCESS(uDailyQuestCount == 1);

    assert(pAcceptNpc->m_pTemplate->nDailyQuestCycle > 0);
    uRandSeed = (unsigned)GetDailyQuestNextAcceptTime(
        pAcceptNpc->m_pTemplate->nDailyQuestCycle, pAcceptNpc->m_pTemplate->nDailyQuestOffset
    );
    
    uOldRandSeed = g_GetRandomSeed();

    g_RandomSeed(uRandSeed);
    uRandNum = g_Random(KILO_NUM);
    g_RandomSeed(uOldRandSeed);
    
    uRandNum = uRandNum * uDailyQuestCount / KILO_NUM;
    if (vecDailyQuestID[uRandNum] != pQuestInfo->dwQuestID)
    {
        goto Exit0;
    }

Exit1:
    nResult = qrcSuccess;
Exit0:
    return nResult;
}

void KQuestList::CheckDailyQuestFinished(KQuestInfo* pQuestInfo)
{
    KNpcTemplate* pNpcTemplate      = NULL;
    int           nDailyQuestIndex  = -1;

    assert(pQuestInfo);

    // 每日任务一定是可重复而且是必须要接的
    KG_PROCESS_ERROR(pQuestInfo->bRepeat && pQuestInfo->bAccept);

    for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
    {
        if (m_DailyQuest[i].dwQuestID != pQuestInfo->dwQuestID)
        {
            continue;
        }

        nDailyQuestIndex = i;
        break;
    }
    
    KG_PROCESS_ERROR(nDailyQuestIndex != -1);

    pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(
        pQuestInfo->dwStartNpcTemplateID
    );
    KGLOG_PROCESS_ERROR(pNpcTemplate);

    m_DailyQuest[nDailyQuestIndex].nNextAcceptTime = GetDailyQuestNextAcceptTime(
        pNpcTemplate->nDailyQuestCycle, pNpcTemplate->nDailyQuestOffset
    );
    
#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnDailyQuestUpdate();
    }
#endif

Exit0:
    return;
}

int KQuestList::GetFinishedDailyQuestCount()
{
    int     nResult         = 0;
    time_t  nGSCurrentTime  = g_pSO3World->m_nCurrentTime;

#ifdef _CLIENT
    nGSCurrentTime = g_pSO3World->m_nBaseTime + (g_pSO3World->m_nGameLoop / GAME_FPS);
#endif

    for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
    {
        if (m_DailyQuest[i].dwQuestID == ERROR_ID)
        {
            continue;
        }

        if (m_DailyQuest[i].nNextAcceptTime > nGSCurrentTime)
        {
            ++nResult;
        }
    }

    return nResult;
}

#ifdef _SERVER
QUEST_RESULT_CODE KQuestList::Accept(KTarget* pTarget, DWORD dwQuestID, BOOL bIgnoreRules)
{
	BOOL                bRetCode    = false;
	QUEST_RESULT_CODE   nResult     = qrcFailed;
	KQuestInfo*         pQuestInfo  = NULL;
	int                 nQuestIndex = 0;
    KItem*              pItem       = NULL;
    BOOL                bDailyQuest = false;
    KITEM_LOG_INFO      ItemLogInfo;

	KG_PROCESS_ERROR(pTarget);
	KGLOG_PROCESS_ERROR_RET_CODE(dwQuestID < MAX_QUEST_COUNT, qrcErrorQuestID);

    assert(g_pSO3World);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR_RET_CODE(pQuestInfo, qrcCannotFindQuest);
    
	// 检测是否满足接任务的条件
	if (!bIgnoreRules)
	{
		nResult = CanAccept(pQuestInfo, pTarget);
		KG_PROCESS_ERROR(nResult == qrcSuccess);
	}
    
    while ((nQuestIndex < MAX_ACCEPT_QUEST_COUNT) && m_AcceptedQuestList[nQuestIndex].pQuestInfo)
	{
		++nQuestIndex;
	}
	KG_PROCESS_ERROR_RET_CODE(nQuestIndex < MAX_ACCEPT_QUEST_COUNT, qrcQuestListFull);

    if (pTarget->GetTargetType() == ttNpc)
    {
        KNpc*   pAcceptNpc = NULL;

        bRetCode = pTarget->GetTarget(&pAcceptNpc);
        KGLOG_PROCESS_ERROR(bRetCode);
        KGLOG_PROCESS_ERROR(pAcceptNpc);
        
        if (pAcceptNpc->m_pTemplate && pAcceptNpc->m_pTemplate->nDailyQuestCycle > 0 && pQuestInfo->bRepeat)
        {
            bDailyQuest = true;

            for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
            {
                if (m_DailyQuest[i].dwQuestID == ERROR_ID)
                {
                    m_DailyQuest[i].dwQuestID = dwQuestID;
                    m_DailyQuest[i].nNextAcceptTime = 0;
                    break;
                }
            }
        }
    }
             
	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		if (pQuestInfo->byOfferItemType[nIndex] == 0 || pQuestInfo->wOfferItemIndex[nIndex] == 0)
        {
            continue;
        }

		pItem = g_pSO3World->GenerateItem(
            pQuestInfo->byOfferItemType[nIndex], pQuestInfo->wOfferItemIndex[nIndex]
        );
		KGLOG_PROCESS_ERROR(pItem);

        if (pItem->m_Common.nGenre == igBook)
        {
            pItem->m_nCurrentDurability = pQuestInfo->wPresentItemAmount[nIndex];
        }
        else if (pItem->IsStackable())
		{
            pItem->SetStackNum(pQuestInfo->wOfferItemAmount[nIndex]);
		}
        
        g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);
        
		bRetCode = m_pPlayer->m_ItemList.AddItem(pItem);
		KGLOG_PROCESS_ERROR_RET_CODE(bRetCode == aircSuccess, qrcNotEnoughFreeRoom);

        g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "accept quest : ", pQuestInfo->szQuestName);

        pItem = NULL;
    }

	// 注册需要杀的怪
	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		KQuestRequirement QuestRequirement;

		if (pQuestInfo->dwKillNpcTemplateID[nIndex] != 0)
		{
			QuestRequirement.nQuestIndex = nQuestIndex;
			QuestRequirement.nParamIndex = nIndex;

			m_mapNpc2Quest.insert(std::make_pair(
				pQuestInfo->dwKillNpcTemplateID[nIndex], QuestRequirement));
		}
	}

	// 添加任务
	memset(&(m_AcceptedQuestList[nQuestIndex]), 0, sizeof(KQuest));
	m_AcceptedQuestList[nQuestIndex].pQuestInfo = pQuestInfo;
	m_nAcceptedCount++;
	m_AcceptedQuestList[nQuestIndex].nLimitTime = (pQuestInfo->nFinishTime) ? (g_pSO3World->m_nCurrentTime + pQuestInfo->nFinishTime) : 0;

	if (pTarget->GetTargetType() == ttPlayer)
	{
		// 如果是玩家共享任务，就通知任务的源玩家
        KPlayer* pSrcPlayer = NULL;
        bRetCode = pTarget->GetTarget(&pSrcPlayer);
        KGLOG_PROCESS_ERROR(bRetCode);

		if (pSrcPlayer)
		{
            KSHARE_QUEST_RESULT     ShareQuestResult;
            
            ShareQuestResult.dwQuestID          = dwQuestID;
            ShareQuestResult.dwTargetPlayerID   = m_pPlayer->m_dwID;

            g_PlayerServer.DoMessageNotify(pSrcPlayer->m_nConnIndex, ectShareQuestErrorCode, sqrcAcceptQuest, &ShareQuestResult, sizeof(ShareQuestResult));
		}
	}
	else if (pQuestInfo->bEscortQuest && pTarget->GetTargetType() == ttNpc)
	{
		KNpc* pTargetNpc = NULL;
		bRetCode = pTarget->GetTarget(&pTargetNpc);
		KGLOG_PROCESS_ERROR(bRetCode && pTargetNpc);

		//从NPC上接的小队任务,会触发场景的StartTeamQuest
		KGLOG_PROCESS_ERROR(m_pPlayer->m_pScene);

		bRetCode = m_pPlayer->m_pScene->m_EscortQuestMgr.AddEscortQuest(
            pQuestInfo->dwQuestID, m_pPlayer, pTargetNpc
        );
        KGLOG_CHECK_ERROR(bRetCode);
	}

	g_PlayerServer.DoAcceptQuest(nQuestIndex, dwQuestID, m_pPlayer->m_nConnIndex, bDailyQuest);

	if (pQuestInfo->dwScriptID)
	{	
		bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pQuestInfo->dwScriptID);
		if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(pQuestInfo->dwScriptID, SCRIPT_ON_ACCEPT_QUEST))
		{
			int nTopIndex = 0;
			g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

			g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer);
            g_pSO3World->m_ScriptCenter.PushValueToStack(dwQuestID);
            
			g_pSO3World->m_ScriptCenter.CallFunction(pQuestInfo->dwScriptID, SCRIPT_ON_ACCEPT_QUEST, 0);
			g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
		}
	}
	
    g_LogClient.LogAcceptQuest(pQuestInfo->dwQuestID, pQuestInfo->szQuestName, pQuestInfo->byLevel, m_pPlayer->m_szAccount, m_pPlayer->m_szName);

	nResult = qrcSuccess;
Exit0:
    if (nResult != qrcSuccess && pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
	return nResult;
}

QUEST_RESULT_CODE KQuestList::Finish(
    DWORD dwQuestID, KTarget& rTarget, int nPresentChoice1, int nPresentChoice2, BOOL bIgnoreRules
)
{
	QUEST_RESULT_CODE   nResult             = qrcFailed;
    QUEST_RESULT_CODE   eRetCode            = qrcFailed;
	BOOL                bRetCode            = false;
    KQuestInfo*         pQuestInfo          = NULL;
	int                 nQuestIndex         = -1;
    KScene*             pScene              = NULL;
    int                 nQuestCountAchID    = -1;

	KGLOG_PROCESS_ERROR_RET_CODE(dwQuestID < MAX_QUEST_COUNT, qrcErrorQuestID);
    KGLOG_PROCESS_ERROR(nPresentChoice1 >= 0 && nPresentChoice1 < QUEST_PARAM_COUNT);
    KGLOG_PROCESS_ERROR(nPresentChoice2 >= QUEST_PARAM_COUNT && nPresentChoice2 < QUEST_PARAM_COUNT * 2);
    
	// 取任务的详细信息
	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR_RET_CODE(pQuestInfo, qrcCannotFindQuest);

	// 检测该任务是否需要接后才能交
	if (pQuestInfo->bAccept)
	{
		nQuestIndex = GetQuestIndex(dwQuestID);
		KGLOG_PROCESS_ERROR_RET_CODE(nQuestIndex != -1, qrcNeedAccept);
	}

	// 检测是否已经完成
	if (!bIgnoreRules)
	{
		nResult = CanFinish(pQuestInfo, &rTarget);
		KG_PROCESS_ERROR(nResult == qrcSuccess);
	}

	// 检测能否成功发放任务奖励
    eRetCode = CanPresentItems(pQuestInfo, nPresentChoice1, nPresentChoice2);
	KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);

    bRetCode = UnregisterQuestKillNpc(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CostQuestRequireItem(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CostQuestOfferItem(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

	// 扣除金钱
	bRetCode = m_pPlayer->m_ItemList.AddMoney(-(pQuestInfo->nCostMoney));
	KGLOG_PROCESS_ERROR(bRetCode);

    g_LogClient.LogPlayerMoneyChange(-pQuestInfo->nCostMoney, m_pPlayer, "quest cost money");

    g_pSO3World->m_StatDataServer.UpdateMoneyStat(m_pPlayer, -(pQuestInfo->nCostMoney), "OTHER");

    if (!pQuestInfo->bRepeat)
    {
        m_byQuestStateList[dwQuestID] = qsFinished;
    }

    g_LogClient.LogFinishQuest(pQuestInfo->dwQuestID, pQuestInfo->szQuestName, pQuestInfo->byLevel, m_pPlayer->m_szAccount, m_pPlayer->m_szName);

	// 发放任务奖励
	nResult = Present(pQuestInfo, nPresentChoice1, nPresentChoice2);
	KGLOG_PROCESS_ERROR(nResult == qrcSuccess);

	// 切换状态
	if (nQuestIndex != -1)
	{
		m_AcceptedQuestList[nQuestIndex].pQuestInfo = NULL;
		m_nAcceptedCount--;
	}

	if (pQuestInfo->dwCoolDownID != 0)
	{
		int nCDInterval = g_pSO3World->m_Settings.m_CoolDownList.GetCoolDownValue(pQuestInfo->dwCoolDownID);
		m_pPlayer->m_TimerList.ResetTimer(pQuestInfo->dwCoolDownID, nCDInterval);
	}
    
    CheckDailyQuestFinished(pQuestInfo);

    pScene = m_pPlayer->m_pScene;

    // 加任务成就计数器
    if (pQuestInfo->nAchievementID >= 0 && pQuestInfo->nAchievementID < MAX_ACHIEVEMENT_ID && pScene)
    {
        m_pPlayer->m_Achievement.AddCount(pQuestInfo->nAchievementID, 1);
    }

    // 加场景任务计数成就计数器
    if (
        !pQuestInfo->bRepeat && pQuestInfo->bAccept && pScene &&
        (pScene->m_nQuestCountAchID >= 0) && (pScene->m_nQuestCountAchID < MAX_ACHIEVEMENT_ID)
    )
    {
        m_pPlayer->m_Achievement.AddCount(pScene->m_nQuestCountAchID, 1);
    }

    // 加全局任务计数成就计数器
    nQuestCountAchID = g_pSO3World->m_Settings.m_ConstList.nQuestCountAchievementID;
    if (
        !pQuestInfo->bRepeat && pQuestInfo->bAccept && pScene &&
        (nQuestCountAchID >= 0) && (nQuestCountAchID < MAX_ACHIEVEMENT_ID)
    )
    {
        m_pPlayer->m_Achievement.AddCount(nQuestCountAchID, 1);
    }

	g_PlayerServer.DoFinishQuest(dwQuestID, m_pPlayer->m_nConnIndex, bIgnoreRules);

	if (pQuestInfo->dwScriptID)
	{
		bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pQuestInfo->dwScriptID);
		if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(pQuestInfo->dwScriptID, SCRIPT_ON_FINISH_QUEST))
		{
			int nTopIndex = 0;
			g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

			g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer);
			g_pSO3World->m_ScriptCenter.PushValueToStack(nPresentChoice1);
			g_pSO3World->m_ScriptCenter.PushValueToStack(nPresentChoice2);
            g_pSO3World->m_ScriptCenter.PushValueToStack(dwQuestID);

			g_pSO3World->m_ScriptCenter.CallFunction(pQuestInfo->dwScriptID, SCRIPT_ON_FINISH_QUEST, 0);
			g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
		}
	}

    g_pSO3World->m_StatDataServer.UpdateQuestStat(pQuestInfo->dwQuestID, "FINISH");

    if (pQuestInfo->bRepeat)
        g_pSO3World->m_StatDataServer.UpdateQuestRepeat(pQuestInfo->dwQuestID);

	nResult = qrcSuccess;
Exit0:
	return nResult;
}

void KQuestList::OnKillNpc(DWORD dwTemplateID)
{
    int                         nQuestIndex     = 0;
    int                         nParamIndex     = 0;
    MAP_NPC_2_QUEST::iterator   QuestIt         = m_mapNpc2Quest.lower_bound(dwTemplateID);
    MAP_NPC_2_QUEST::iterator   UpperBoundIt    = m_mapNpc2Quest.upper_bound(dwTemplateID);
    BOOL                        bIsInRaid       = m_pPlayer->IsInRaid();
    
    while (QuestIt != UpperBoundIt)
    {
        nQuestIndex = QuestIt->second.nQuestIndex;
        nParamIndex = QuestIt->second.nParamIndex;

		assert(nQuestIndex >= 0 && nQuestIndex < MAX_ACCEPT_QUEST_COUNT);
        assert(nParamIndex >= 0 && nParamIndex < QUEST_PARAM_COUNT);

        if (
            (
                bIsInRaid &&
                m_AcceptedQuestList[nQuestIndex].pQuestInfo->byTeamRequireMode == qtrmRequireNotRaid
            ) ||
            (
                !bIsInRaid &&
                m_AcceptedQuestList[nQuestIndex].pQuestInfo->byTeamRequireMode == qtrmRequireRaid
            )
        )
        {
            ++QuestIt;
            continue;
        }

		m_AcceptedQuestList[nQuestIndex].dwKillNpcCount[nParamIndex]++;

        RegisterTeamAssistance(m_AcceptedQuestList[nQuestIndex].pQuestInfo->dwQuestID);

		g_PlayerServer.DoSyncQuestValue(nQuestIndex, m_pPlayer->m_nConnIndex);

        ++QuestIt;
    }

	return;
}

BOOL KQuestList::ResetAssistDailyCount()
{
    m_nAssistDailyCount = 0;

    g_PlayerServer.DoMessageNotify(m_pPlayer->m_nConnIndex, ectSetQuestAssistDailyCount, 0);

    return true;
}

int KQuestList::GetQuestRoundCount(DWORD dwQuestID)
{
    int nResult = -1;

    for (int nIndex = 0; nIndex < MAX_ACCEPT_QUEST_COUNT; ++nIndex)
	{
		if (m_AcceptedQuestList[nIndex].pQuestInfo == NULL)
            continue;

        if (m_AcceptedQuestList[nIndex].pQuestInfo->dwQuestID == dwQuestID)
		{
			nResult = m_AcceptedQuestList[nIndex].nRoundCount;
            goto Exit1;
		}
	}

Exit1:
    return nResult;
}

BOOL KQuestList::SetQuestRoundCount(DWORD dwQuestID, int nQuestRoundCount)
{
    BOOL bResult = false;

    for (int nIndex = 0; nIndex < MAX_ACCEPT_QUEST_COUNT; ++nIndex)
	{
		if (m_AcceptedQuestList[nIndex].pQuestInfo == NULL)
            continue;

        if (m_AcceptedQuestList[nIndex].pQuestInfo->dwQuestID == dwQuestID)
		{
			m_AcceptedQuestList[nIndex].nRoundCount = nQuestRoundCount;
            goto Exit1;
		}
	}
    
    goto Exit0;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}
#else
QUEST_RESULT_CODE KQuestList::Accept(DWORD dwQuestID, BOOL bDailyQuest)
{
	BOOL                bRetCode    = false;
	QUEST_RESULT_CODE   nResult     = qrcFailed;
	KQuestInfo*         pQuestInfo  = NULL;
	int                 nQuestIndex = 0;

	KGLOG_PROCESS_ERROR_RET_CODE(dwQuestID < MAX_QUEST_COUNT, qrcErrorQuestID);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR_RET_CODE(pQuestInfo, qrcCannotFindQuest);

	while (m_AcceptedQuestList[nQuestIndex].pQuestInfo && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT))
	{
		nQuestIndex++;
	}

	KG_PROCESS_ERROR_RET_CODE(nQuestIndex < MAX_ACCEPT_QUEST_COUNT, qrcQuestListFull);

	memset(&(m_AcceptedQuestList[nQuestIndex]), 0, sizeof(KQuest));
	m_AcceptedQuestList[nQuestIndex].pQuestInfo = pQuestInfo;
	m_nAcceptedCount++;

	m_AcceptedQuestList[nQuestIndex].nLimitTime = (pQuestInfo->nFinishTime) ? (g_pSO3World->m_nCurrentTime + pQuestInfo->nFinishTime) : 0;
    
    if (bDailyQuest)
    {
        for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
        {
            if (m_DailyQuest[i].dwQuestID == 0)
            {
                m_DailyQuest[i].dwQuestID = dwQuestID;
                m_DailyQuest[i].nNextAcceptTime = 0;
                break;
            }
        }
    }

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnUpdateDoodadQuestEffect();
    }

	nResult = qrcSuccess;
Exit0:
	return nResult;
}

QUEST_RESULT_CODE KQuestList::Finish(DWORD dwQuestID)
{
	QUEST_RESULT_CODE   nResult     = qrcFailed;
	QUEST_RESULT_CODE   eRetCode    = qrcFailed;
	int                 nQuestIndex = -1;
    KQuestInfo*         pQuestInfo  = NULL;

	KGLOG_PROCESS_ERROR_RET_CODE(dwQuestID < MAX_QUEST_COUNT, qrcErrorQuestID);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR_RET_CODE(pQuestInfo, qrcCannotFindQuest);

	nQuestIndex = GetQuestIndex(dwQuestID);
	if (pQuestInfo->bAccept)
	{
		KGLOG_PROCESS_ERROR_RET_CODE(nQuestIndex != -1, qrcNeedAccept);
	}

	if (!pQuestInfo->bRepeat)
	{
		m_byQuestStateList[dwQuestID] = qsFinished;
	}

	if (nQuestIndex != -1)
	{
		m_AcceptedQuestList[nQuestIndex].pQuestInfo = NULL;
		m_nAcceptedCount--;
	}
    
    CheckDailyQuestFinished(pQuestInfo);

	nResult = qrcSuccess;
Exit0:
	return nResult;
}

void KQuestList::UpdateNpcQuestMark(int nQuestID)
{
	BOOL    bRetCode    = false;
    KScene* pScene      = NULL;

	KGLOG_PROCESS_ERROR(nQuestID >= -1 && nQuestID < MAX_QUEST_COUNT);

	pScene = m_pPlayer->m_pScene;
	KG_PROCESS_ERROR(pScene);

	m_UpdateQuestMarkFunc.m_nQuestID = nQuestID;

    pScene->TraverseRangeNpc(m_pPlayer->m_pRegion, m_UpdateQuestMarkFunc);

Exit0:
	return;
}

void KQuestList::OnUpdateItem(DWORD dwType, DWORD dwIndex, BOOL bGetItem)
{
    BOOL        bRetCode        = false;
    KQuestInfo* pQuestInfo      = NULL;

	for (int nQuestIndex = 0; nQuestIndex < MAX_ACCEPT_QUEST_COUNT; nQuestIndex++)
	{
        pQuestInfo = m_AcceptedQuestList[nQuestIndex].pQuestInfo;
		
        if (!pQuestInfo)
		{
            continue;
        }

		for (int i = 0; i < QUEST_END_ITEM_COUNT; i++)
		{
			if (pQuestInfo->byEndRequireItemType[i] != dwType || pQuestInfo->wEndRequireItemIndex[i] != dwIndex)
			{
                continue;
            }

            if (g_pGameWorldUIHandler)
            {
                KUIEventQuestDataUpdate QuestDataUpdate;
                QuestDataUpdate.nQuestIndex = nQuestIndex;

                if (bGetItem)			
                    QuestDataUpdate.eEventType = qetGetItem;
                else
                    QuestDataUpdate.eEventType = qetDeleteItem;

                QuestDataUpdate.nValue1 = dwType;
                QuestDataUpdate.nValue2 = dwIndex;

                g_pGameWorldUIHandler->OnQuestDataUpdate(QuestDataUpdate);
            }

			// 表现逻辑
			UpdateNpcQuestMark((int)pQuestInfo->dwQuestID);
            
            bRetCode = m_pPlayer->m_QuestList.IsQuestEndItemEnoughByIndex(pQuestInfo, i, true);
            
            // 任务道具已经足够或者删除任务道具的时候更新下Doodad特效
            if (bRetCode || !bGetItem)
            {
                if (g_pGameWorldRepresentHandler)
                {
                    g_pGameWorldRepresentHandler->OnUpdateDoodadQuestEffect();
                }
            }

			break;
		}
	}

	return;
}

void KQuestList::SetAssistDailyCount(int nValue)
{
    m_nAssistDailyCount = nValue;
}

int KQuestList::GetAssistDailyCount()
{
    return m_nAssistDailyCount;
}
#endif

QUEST_RESULT_CODE KQuestList::Cancel(int nQuestIndex)
{
   	BOOL              bRetCode   = false;
    DWORD             dwQuestID  = 0;
    KQuestInfo*       pQuestInfo = NULL;
	QUEST_RESULT_CODE nResult    = qrcFailed;

	KG_PROCESS_ERROR_RET_CODE(
        (nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT), qrcErrorQuestIndex
    );

	//取任务的详细信息
	pQuestInfo = m_AcceptedQuestList[nQuestIndex].pQuestInfo;
	KGLOG_PROCESS_ERROR_RET_CODE(pQuestInfo, qrcCannotFindQuest);

	dwQuestID = pQuestInfo->dwQuestID;

#if defined(_SERVER)
    bRetCode = UnregisterQuestKillNpc(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CostQuestRequireItem(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CostQuestOfferItem(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);
#endif	//_SERVER

	//删除任务
	m_AcceptedQuestList[nQuestIndex].pQuestInfo = NULL;
	--m_nAcceptedCount;
    
    for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
    {
        if (m_DailyQuest[i].dwQuestID == dwQuestID)
        {
            m_DailyQuest[i].dwQuestID = 0;
            m_DailyQuest[i].nNextAcceptTime = 0;
            break;
        }
    }

#ifdef _SERVER
    if (pQuestInfo->bEscortQuest && m_pPlayer->m_pScene)
    {
        m_pPlayer->m_pScene->m_EscortQuestMgr.SetPlayerFailed(m_pPlayer, dwQuestID);
    }

	g_PlayerServer.DoCancelQuest(nQuestIndex, m_pPlayer->m_nConnIndex);

	if (pQuestInfo->dwScriptID)
	{
		bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(pQuestInfo->dwScriptID);
		if (bRetCode && g_pSO3World->m_ScriptCenter.IsFuncExist(pQuestInfo->dwScriptID, SCRIPT_ON_CANCEL_QUEST))
		{
			int nTopIndex = 0;
			g_pSO3World->m_ScriptCenter.SafeCallBegin(&nTopIndex);

			g_pSO3World->m_ScriptCenter.PushValueToStack(m_pPlayer);
            g_pSO3World->m_ScriptCenter.PushValueToStack(dwQuestID);
			
            g_pSO3World->m_ScriptCenter.CallFunction(pQuestInfo->dwScriptID, SCRIPT_ON_CANCEL_QUEST, 0);
			g_pSO3World->m_ScriptCenter.SafeCallEnd(nTopIndex);
		}
	}

    g_LogClient.LogCancelQuest(pQuestInfo->dwQuestID, pQuestInfo->szQuestName, pQuestInfo->byLevel, m_pPlayer->m_szAccount, m_pPlayer->m_szName);

    g_pSO3World->m_StatDataServer.UpdateQuestStat(pQuestInfo->dwQuestID, "CANCEL");

#else
    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnUpdateDoodadQuestEffect();
    }
#endif
	nResult = qrcSuccess;
Exit0:
	return nResult;
}

BOOL KQuestList::ClearQuest(DWORD dwQuestID)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nQuestIndex     = -1;
    KQuestInfo*     pQuestInfo      = NULL;

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR(pQuestInfo);    

	m_byQuestStateList[dwQuestID] = qsUnfinished;

    nQuestIndex = GetQuestIndex(dwQuestID);
	if (nQuestIndex != -1)
	{
        assert(nQuestIndex >= 0 && nQuestIndex < MAX_ACCEPT_QUEST_COUNT);
		m_AcceptedQuestList[nQuestIndex].pQuestInfo = NULL;
		m_nAcceptedCount--;
	}

#ifdef _SERVER
    bRetCode = UnregisterQuestKillNpc(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CostQuestRequireItem(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CostQuestOfferItem(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoClearQuest(m_pPlayer->m_nConnIndex, dwQuestID);
#else
    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnUpdateDoodadQuestEffect();
    }
#endif

    bResult = true;
Exit0:
    return bResult;
}

QUEST_RESULT_CODE KQuestList::CanFinish(KQuestInfo* pQuestInfo, KTarget* pTarget)
{
	QUEST_RESULT_CODE   nResult         = qrcFailed;
	BOOL                bRetCode        = false;
	int                 nQuestIndex     = 0;
    KQuest*             pQuest          = NULL;
	int                 nDistance2      = 0;
    int                 nMoney          = 0;
    DWORD               dwTabType       = 0;
    DWORD               dwIndex         = 0;
    DWORD               dwCurrentAmount = 0;
    DWORD               dwRequireAmount = 0;
    KItemInfo*          pItemInfo       = NULL;

	KGLOG_PROCESS_ERROR(m_pPlayer);
	KGLOG_PROCESS_ERROR(pQuestInfo);

	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_eMoveState != cmsOnDeath, qrcFailed);

#ifdef _CLIENT
    if (m_pPlayer->m_bFreeLimitFlag)
    {
        KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel < g_pSO3World->m_Settings.m_ConstList.nFreeMaxLevel, qrcFailed);
    }
#endif

	KG_PROCESS_ERROR_RET_CODE(
        m_byQuestStateList[pQuestInfo->dwQuestID] == qsUnfinished || pQuestInfo->bRepeat, qrcErrorQuestState
    );
    
	nQuestIndex = GetQuestIndex(pQuestInfo->dwQuestID);
	if (nQuestIndex != -1)
	{
		pQuest = &(m_AcceptedQuestList[nQuestIndex]);
	}

	if (pQuestInfo->bAccept)
	{
		KG_PROCESS_ERROR_RET_CODE(nQuestIndex != -1, qrcNeedAccept);
	}
    else // 不用接的任务要检查一些条件
    {
        QUEST_RESULT_CODE   eRetCode     = qrcFailed;

        eRetCode = CommonCheck(pQuestInfo);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
    }

	//检测玩家的目标Npc身上是否能交该任务
    if (pTarget != NULL && pQuestInfo->dwEndNpcTemplateID != 0)
	{
		KNpc*       pNpc            = NULL;
		DWORD       dwNpcTemplateID = 0;

        bRetCode = pTarget->GetTarget(&pNpc);
        KGLOG_PROCESS_ERROR_RET_CODE(bRetCode && pNpc, qrcErrorEndNpcTarget);

        dwNpcTemplateID = pNpc->m_dwTemplateID;

		KG_PROCESS_ERROR_RET_CODE(dwNpcTemplateID == pQuestInfo->dwEndNpcTemplateID, qrcErrorEndNpcTarget);

#ifdef _SERVER
		bRetCode = pNpc->CanDialog(m_pPlayer);
		KGLOG_PROCESS_ERROR(bRetCode);
#endif
    }

	//检测玩家的目标Doodad身上是否能交该任务
	if (pTarget != NULL && pQuestInfo->dwEndDoodadTemplateID != 0)
	{
		DWORD       dwDoodadTemplateID  = 0;
		KDoodad*    pDoodad             = NULL;

        bRetCode = pTarget->GetTarget(&pDoodad);
        KG_PROCESS_ERROR_RET_CODE(bRetCode && pDoodad, qrcErrorEndDoodadTarget);

        dwDoodadTemplateID = pDoodad->m_dwTemplateID;

		KG_PROCESS_ERROR_RET_CODE(dwDoodadTemplateID == pQuestInfo->dwEndDoodadTemplateID, qrcErrorEndDoodadTarget);

#ifdef _SERVER
        bRetCode = g_InRange(m_pPlayer, pDoodad, COMMON_PLAYER_OPERATION_DISTANCE);
        KG_PROCESS_ERROR(bRetCode);
#endif
	}

	//检测玩家身上是否有任务结束道具
	if (pQuestInfo->byEndItemType != 0  || pQuestInfo->wEndItemIndex != 0)
	{
		DWORD dwAmount = m_pPlayer->m_ItemList.GetItemAmountInEquipAndPackage(
			pQuestInfo->byEndItemType, pQuestInfo->wEndItemIndex);
		KG_PROCESS_ERROR_RET_CODE(dwAmount > 0, qrcNoEndQuestItem);
	}

	//Cooldown计时器
	bRetCode = m_pPlayer->m_TimerList.CheckTimer(pQuestInfo->dwCoolDownID);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, qrcCooldown);

	if (pQuest)
	{
		//检测任务是否已经失败
		KG_PROCESS_ERROR_RET_CODE(pQuest->bFailed == false, qrcQuestFailed);

		//检测任务变量的数值
		for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT * 2; nIndex++)
		{
			if (pQuestInfo->nQuestValue[nIndex] != 0)
			{
				KG_PROCESS_ERROR_RET_CODE(
					pQuest->nQuestValue[nIndex] >= pQuestInfo->nQuestValue[nIndex],
					qrcErrorTaskValue);
			}
		}

		//检测杀怪数量
		for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
		{
			if (pQuestInfo->dwKillNpcTemplateID[nIndex] != ERROR_ID)
			{
				KG_PROCESS_ERROR_RET_CODE(pQuest->dwKillNpcCount[nIndex] >= pQuestInfo->wKillNpcAmount[nIndex], qrcErrorKillNpcCount);
			}
		}
	}
    
    if (pQuestInfo->nCostMoney > 0)
    {
        nMoney = m_pPlayer->m_ItemList.GetMoney();
	    KG_PROCESS_ERROR_RET_CODE(nMoney >= pQuestInfo->nCostMoney, qrcNotEnoughMoney);
    }
    
    bRetCode = IsQuestEndItemEnough(pQuestInfo, false);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, qrcErrorItemCount);

#ifdef _SERVER
    bRetCode = IsQuestEndBookMemorized(pQuestInfo);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, qrcQuestEndBookNotMemorized);

    if (pQuestInfo->nPresentMoney > 0)
    {
        nMoney = m_pPlayer->m_ItemList.GetMoney();
        KG_PROCESS_ERROR_RET_CODE(nMoney + pQuestInfo->nPresentMoney > nMoney, qrcMoneyLimit); // 防止溢出
    }
#endif

	nResult = qrcSuccess;
Exit0:
	return nResult;
}

int KQuestList::GetAcceptedCount()
{
	return m_nAcceptedCount;
}

//获取指定任务状态
QUEST_STATE	KQuestList::GetQuestState(DWORD dwQuestID)
{
	KGLOG_PROCESS_ERROR(dwQuestID < MAX_QUEST_COUNT);

	return (QUEST_STATE)m_byQuestStateList[dwQuestID];
Exit0:
	return qsInvalid;
}

int KQuestList::GetQuestPhase(DWORD dwQuestID)
{
	int         nResult         = -1;
	BOOL        bRetCode        = false;
	DWORD       dwQuestIndex    = 0;
	KQuest*     pQuest          = NULL;
	KQuestInfo* pQuestInfo      = NULL;

	if (m_byQuestStateList[dwQuestID] == qsFinished)
	{
		//已完成任务
		nResult = 3;
		goto Exit0;
	}
	
	KGLOG_PROCESS_ERROR(dwQuestID >= 0 && dwQuestID < MAX_QUEST_COUNT);

	nResult = 0;
	for (dwQuestIndex = 0; dwQuestIndex < MAX_ACCEPT_QUEST_COUNT; dwQuestIndex++)
	{
		if (m_AcceptedQuestList[dwQuestIndex].pQuestInfo && 
			m_AcceptedQuestList[dwQuestIndex].pQuestInfo->dwQuestID == dwQuestID)
		break;
	}
	KG_PROCESS_ERROR(dwQuestIndex < MAX_ACCEPT_QUEST_COUNT);

	nResult = 1;
	pQuest = &(m_AcceptedQuestList[dwQuestIndex]);
	pQuestInfo = pQuest->pQuestInfo;
	if (pQuest)
	{			
		//检测任务变量的数值
		for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT * 2; nIndex++)
		{
			if (pQuestInfo->nQuestValue[nIndex] != 0)
			{
				KG_PROCESS_ERROR(pQuest->nQuestValue[nIndex] >= pQuestInfo->nQuestValue[nIndex]);
			}
		}

		//检测杀怪数量
		for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
		{
			if (pQuestInfo->dwKillNpcTemplateID[nIndex] != ERROR_ID)
			{
				KG_PROCESS_ERROR(pQuest->dwKillNpcCount[nIndex] >= pQuestInfo->wKillNpcAmount[nIndex]);
			}
		}
	}

	// 检测任务物品
    bRetCode = IsQuestEndItemEnough(pQuestInfo, false);
    KG_PROCESS_ERROR(bRetCode);

	nResult = 2;
Exit0:
	return nResult;
}

DWORD KQuestList::GetQuestID(int nQuestIndex)
{
	KG_PROCESS_ERROR((nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT));

	if (m_AcceptedQuestList[nQuestIndex].pQuestInfo)
    {
		return m_AcceptedQuestList[nQuestIndex].pQuestInfo->dwQuestID;
    }

Exit0:
	return 0;
}

int KQuestList::GetQuestIndex(DWORD dwQuestID)
{
	for (int nIndex = 0; nIndex < MAX_ACCEPT_QUEST_COUNT; nIndex++)
	{
		if (m_AcceptedQuestList[nIndex].pQuestInfo == NULL)
            continue;

        if (m_AcceptedQuestList[nIndex].pQuestInfo->dwQuestID == dwQuestID)
		{
			return nIndex;
		}
	}

	return -1;
}

int KQuestList::GetQuestValue(int nQuestIndex, int nValueIndex)
{
	KGLOG_PROCESS_ERROR((nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT));
	KGLOG_PROCESS_ERROR((nValueIndex >= 0) && (nValueIndex < QUEST_PARAM_COUNT * 2));

	return m_AcceptedQuestList[nQuestIndex].nQuestValue[nValueIndex];
Exit0:
	return 0;
}

BOOL KQuestList::SetQuestValue(int nQuestIndex, int nValueIndex, int nNewValue)
{
    BOOL bResult = false;

	KGLOG_PROCESS_ERROR((nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT));
	KGLOG_PROCESS_ERROR((nValueIndex >= 0) && (nValueIndex < QUEST_PARAM_COUNT * 2));
    
    if (m_AcceptedQuestList[nQuestIndex].nQuestValue[nValueIndex] == nNewValue)
    {
        goto Exit1;
    }
    
    m_AcceptedQuestList[nQuestIndex].nQuestValue[nValueIndex] = nNewValue;

#ifdef _SERVER
    assert(m_pPlayer);
	g_PlayerServer.DoSyncQuestValue(nQuestIndex, m_pPlayer->m_nConnIndex);
#else    
    if (g_pGameWorldUIHandler)
    {
        KUIEventQuestDataUpdate QuestDataUpdate;
        QuestDataUpdate.eEventType  = qetSetQuestValue;
        QuestDataUpdate.nQuestIndex = nQuestIndex;
        QuestDataUpdate.nValue1     = nValueIndex;
        QuestDataUpdate.nValue2     = nNewValue;
        g_pGameWorldUIHandler->OnQuestDataUpdate(QuestDataUpdate);
    }
#endif

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

DWORD KQuestList::GetKillNpcCount(int nQuestIndex, int nCountIndex)
{
    DWORD dwResult = 0;

	KGLOG_PROCESS_ERROR((nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT));
	KGLOG_PROCESS_ERROR((nCountIndex >= 0) && (nCountIndex < QUEST_PARAM_COUNT));

	dwResult = m_AcceptedQuestList[nQuestIndex].dwKillNpcCount[nCountIndex];
Exit0:
	return dwResult;
}

BOOL KQuestList::SetKillNpcCount(int nQuestIndex, int nCountIndex, int nNewCount)
{
	BOOL bResult = false;

	KGLOG_PROCESS_ERROR((nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT));
	KGLOG_PROCESS_ERROR((nCountIndex >= 0) && (nCountIndex < QUEST_PARAM_COUNT));

	if (m_AcceptedQuestList[nQuestIndex].dwKillNpcCount[nCountIndex] == (DWORD)nNewCount)
    {
        goto Exit1;
    }

	m_AcceptedQuestList[nQuestIndex].dwKillNpcCount[nCountIndex] = nNewCount;

#ifdef _SERVER
	assert(m_pPlayer);
	g_PlayerServer.DoSyncQuestValue(nQuestIndex, m_pPlayer->m_nConnIndex);
#else
    if (g_pGameWorldUIHandler)
    {
        KUIEventQuestDataUpdate QuestDataUpdate;
        QuestDataUpdate.eEventType = qetKillNpc;
        QuestDataUpdate.nQuestIndex =  nQuestIndex;
        QuestDataUpdate.nValue1 = nCountIndex;
        QuestDataUpdate.nValue2 = nNewCount;
        g_pGameWorldUIHandler->OnQuestDataUpdate(QuestDataUpdate);
    }
#endif

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

BOOL KQuestList::SetQuestFailedFlag(int nQuestIndex, BOOL bFlag)
{
    BOOL bResult = false;

	KGLOG_PROCESS_ERROR((nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT));
    
    KG_PROCESS_ERROR(m_AcceptedQuestList[nQuestIndex].pQuestInfo);

    if (m_AcceptedQuestList[nQuestIndex].bFailed == bFlag)
    {
        goto Exit1;
    }

	m_AcceptedQuestList[nQuestIndex].bFailed = bFlag;

#ifdef _SERVER
	assert(m_pPlayer);
    
    if (m_AcceptedQuestList[nQuestIndex].pQuestInfo->bEscortQuest && m_pPlayer->m_pScene)
    {
        m_pPlayer->m_pScene->m_EscortQuestMgr.SetPlayerFailed(
            m_pPlayer, m_AcceptedQuestList[nQuestIndex].pQuestInfo->dwQuestID
        );
    }

	g_PlayerServer.DoSyncQuestValue(nQuestIndex, m_pPlayer->m_nConnIndex);

    g_pSO3World->m_StatDataServer.UpdateQuestStat(m_AcceptedQuestList[nQuestIndex].pQuestInfo->dwQuestID, "FAILED");
#else
    if (g_pGameWorldUIHandler)
	{
		KUIEventQuestDataUpdate QuestDataUpdate;
		QuestDataUpdate.eEventType  = qetUpdateAll;
		QuestDataUpdate.nQuestIndex = nQuestIndex;
		QuestDataUpdate.nValue1     = 0;
		QuestDataUpdate.nValue2     = 0;
		g_pGameWorldUIHandler->OnQuestDataUpdate(QuestDataUpdate);
	}
#endif

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

BOOL KQuestList::GetQuestFailedFlag(int nQuestIndex)
{
    BOOL bResult = false;

	KGLOG_PROCESS_ERROR((nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT));

	bResult = m_AcceptedQuestList[nQuestIndex].bFailed;

Exit0:
	return bResult;
}

int KQuestList::GetLastTime(int nQuestIndex)
{
	time_t nLastTime = 0;

	KGLOG_PROCESS_ERROR((nQuestIndex >= 0) && (nQuestIndex < MAX_ACCEPT_QUEST_COUNT));
    
    assert(g_pSO3World);
	nLastTime = m_AcceptedQuestList[nQuestIndex].nLimitTime - g_pSO3World->m_nCurrentTime;
	KG_PROCESS_ERROR(nLastTime > 0);

	return (int)nLastTime;
Exit0:
	return 0;
}

#ifdef _SERVER
BOOL KQuestList::RegisterTeamAssistance(DWORD dwQuestID)
{
    BOOL        bResult     = false;
    KQuestInfo* pQuestInfo  = NULL;
    KTeam*      pTeam       = NULL;

    assert(m_pPlayer);

    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
    KGLOG_PROCESS_ERROR(pQuestInfo);

    KG_PROCESS_ERROR(pQuestInfo->bAssist);

    KG_PROCESS_ERROR(m_pPlayer->m_dwTeamID != ERROR_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(m_pPlayer->m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == m_pPlayer->m_dwID)
                continue;

            RegisterQuestAssistInfo(it->dwMemberID, dwQuestID);
        }
    }

    bResult = true;
Exit0:
    return bResult;
}
#endif

BOOL KQuestList::GetQuestRepeatCutPercent(KQuestInfo* pQuestInfo, int* pnRepeatCutPercent)
{
    assert(pQuestInfo);
    assert(pnRepeatCutPercent);
    
    *pnRepeatCutPercent = 100;
    if (pQuestInfo->bRepeat)
    {
        *pnRepeatCutPercent = pQuestInfo->byRepeatCutPercent;
    }
    
    return true;
}

BOOL KQuestList::GetQuestLevelCutPercent(KQuestInfo* pQuestInfo, int* pnLevelCutPercent)
{
    int nLevelMargin = 0;

    assert(pQuestInfo);
    assert(pnLevelCutPercent);

    if (pQuestInfo->byLevel == 0)
    {
        *pnLevelCutPercent = 100;
    }
    else
    {
        if (m_pPlayer->m_nLevel >= pQuestInfo->byLevel)
        {
            nLevelMargin = min(m_pPlayer->m_nLevel - pQuestInfo->byLevel, 9);
        }

        // 增加经验值
        if (nLevelMargin < 4)
        {
            *pnLevelCutPercent = 100;
        }
        else
        {
            *pnLevelCutPercent = (85 - (nLevelMargin - 4) * 15);
        }
    }

    return true;
}

BOOL KQuestList::IsQuestEndItemEnoughByIndex(KQuestInfo* pQuestInfo, int nIndex, BOOL bAllPackage)
{
    BOOL        bResult         = false;
    DWORD       dwTabType       = 0; 
    DWORD       dwTabIndex      = 0;
    KItemInfo*  pItemInfo       = NULL;
    DWORD       dwCurrentAmount = 0;
    DWORD       dwRequireAmount = 0;
    int         nBookRecipeID   = -1;

    assert(pQuestInfo);
    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < QUEST_END_ITEM_COUNT);

    dwTabType       = pQuestInfo->byEndRequireItemType[nIndex];
    dwTabIndex      = pQuestInfo->wEndRequireItemIndex[nIndex];
    dwRequireAmount = pQuestInfo->wEndRequireItemAmount[nIndex];

	if (dwTabType == 0 || dwTabIndex == 0)
	{
        goto Exit1;
    }
    
    pItemInfo = g_pSO3World->m_ItemManager.GetItemInfo(dwTabType, dwTabIndex);
    KGLOG_PROCESS_ERROR(pItemInfo);
    
    if (pItemInfo->nGenre == igBook)
    {
        dwRequireAmount = 1;
        nBookRecipeID   = pQuestInfo->wEndRequireItemAmount[nIndex]; // 书的ID
    }

    if (bAllPackage)
    {
        dwCurrentAmount = m_pPlayer->m_ItemList.GetItemTotalAmount(dwTabType, dwTabIndex, nBookRecipeID);
    }
    else
    {
        dwCurrentAmount = m_pPlayer->m_ItemList.GetItemAmountInEquipAndPackage(dwTabType, dwTabIndex, nBookRecipeID);
    }
	
	KG_PROCESS_ERROR(dwCurrentAmount >= dwRequireAmount);
    
Exit1:
    bResult = true;
Exit0:
    return bResult;
}

void KQuestList::ClearQuestList()
{
    m_mapNpc2Quest.clear();
    memset(m_AcceptedQuestList, 0, sizeof(m_AcceptedQuestList));
}

BOOL KQuestList::IsQuestEndItemEnough(KQuestInfo* pQuestInfo, BOOL bAllPackage)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;

    assert(pQuestInfo);

    for (int nIndex = 0; nIndex < QUEST_END_ITEM_COUNT; nIndex++)
	{
        bRetCode = IsQuestEndItemEnoughByIndex(pQuestInfo, nIndex, bAllPackage);
        KG_PROCESS_ERROR(bRetCode);
	}
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::IsQuestEndBookMemorized(KQuestInfo* pQuestInfo)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    DWORD       dwTabType       = 0; 
    DWORD       dwTabIndex      = 0;
    KItemInfo*  pItemInfo       = NULL;
    int         nBookRecipeID   = -1;

    assert(pQuestInfo);

    for (int i = 0; i < QUEST_END_ITEM_COUNT; i++)
	{
        dwTabType       = pQuestInfo->byEndRequireItemType[i];
        dwTabIndex      = pQuestInfo->wEndRequireItemIndex[i];
        
        if (dwTabType == 0 || dwTabIndex == 0)
	    {
            continue;
        }

        pItemInfo = g_pSO3World->m_ItemManager.GetItemInfo(dwTabType, dwTabIndex);
        KGLOG_PROCESS_ERROR(pItemInfo);
        
        if (pItemInfo->nGenre != igBook)
        {
            continue;
        }

        nBookRecipeID = pQuestInfo->wEndRequireItemAmount[i];
        
        bRetCode = m_pPlayer->m_BookList.IsBookMemorized(nBookRecipeID);
        KG_PROCESS_ERROR(bRetCode);
	}

    bResult = true;
Exit0:
    return bResult;
}

void KQuestList::ProcessLimitTimeQuest()
{
	time_t  nNowTime = g_pSO3World->m_nCurrentTime;

	for (int nIndex = 0; nIndex < MAX_ACCEPT_QUEST_COUNT; ++nIndex)
	{
		if (m_AcceptedQuestList[nIndex].pQuestInfo == NULL)
        {
            continue;
        }

        if (
            m_AcceptedQuestList[nIndex].pQuestInfo->nFinishTime == 0 || 
            m_AcceptedQuestList[nIndex].bFailed
        )
        {
            continue;
        }

#ifdef _SERVER
		if (m_AcceptedQuestList[nIndex].nLimitTime <= nNowTime)
		{
            SetQuestFailedFlag(nIndex, true);
		}
#endif
#ifdef _CLIENT
		// 每秒通知更新UI
		if (m_AcceptedQuestList[nIndex].nLimitTime >= nNowTime)
		{
            if (g_pGameWorldUIHandler)
            {
                KUIEventQuestTimeUpdate UIParam;
                UIParam.nLastTime   = (int)(m_AcceptedQuestList[nIndex].nLimitTime - nNowTime);
                UIParam.nQuestIndex = nIndex;

                g_pGameWorldUIHandler->OnQuestTimeUpdate(UIParam);
            }
		}
#endif 
	}

    return;
}

void KQuestList::ProcessDailyQuest()
{
    time_t  nGSCurrentTime  = g_pSO3World->m_nCurrentTime;

#ifdef _CLIENT
    nGSCurrentTime = g_pSO3World->m_nBaseTime + (g_pSO3World->m_nGameLoop / GAME_FPS);
#endif

    for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
    {
        if (m_DailyQuest[i].dwQuestID == ERROR_ID)
        {
            continue;
        }

        if (m_DailyQuest[i].nNextAcceptTime == 0)
        {
            continue;
        }
        
        if (nGSCurrentTime > m_DailyQuest[i].nNextAcceptTime)
        {
            m_DailyQuest[i].dwQuestID = 0;
            m_DailyQuest[i].nNextAcceptTime = 0;            

#ifdef _CLIENT
            UpdateNpcQuestMark(-1); // 仅对上面的dwQuestID做update还不够，因为NPC身上可能是一组随机任务

            if (g_pGameWorldUIHandler)
            {
                g_pGameWorldUIHandler->OnDailyQuestUpdate();
            }
#endif
        }
    }
}

QUEST_RESULT_CODE KQuestList::CommonCheck(KQuestInfo* pQuestInfo)
{
    QUEST_RESULT_CODE nResult       = qrcFailed;
    QUEST_RESULT_CODE nRetCode      = qrcFailed;
    BOOL              bRetCode      = false;
    DWORD             dwSkillLevel  = 0;

    assert(pQuestInfo);

    KG_PROCESS_ERROR_RET_CODE(m_byQuestStateList[pQuestInfo->dwQuestID] == qsUnfinished, qrcAlreadyFinishedQuest);
    KG_PROCESS_ERROR_RET_CODE(pQuestInfo->byRequireCampMask & (0x1 << (int)m_pPlayer->m_eCamp), qrcErrorCamp); // 检查阵营需求
    
    // 门派掩码
    if (pQuestInfo->dwRequireSchoolMask)
    {
        KGLOG_PROCESS_ERROR(m_pPlayer->m_dwForceID < sizeof(DWORD) * CHAR_BIT);
        KG_PROCESS_ERROR_RET_CODE(pQuestInfo->dwRequireSchoolMask & (1 << m_pPlayer->m_dwForceID), qrcErrorForceID);
    }
    
    // 检查声望等级
    if (pQuestInfo->bRequireReputeAll)
    {
        for (int i = 0; i < QUEST_PARAM_COUNT; i++)
        {
            int nReputeLevel = 0;

            if (pQuestInfo->byRequireForceID[i] == 0)
                continue;

            nReputeLevel = m_pPlayer->m_ReputeList.GetReputeLevel(pQuestInfo->byRequireForceID[i]);
            KG_PROCESS_ERROR_RET_CODE(nReputeLevel >= pQuestInfo->byReputeLevelMin[i], qrcErrorRepute);
            KG_PROCESS_ERROR_RET_CODE(nReputeLevel <= pQuestInfo->byReputeLevelMax[i], qrcErrorRepute);
        }
    }
    else
    {
        BOOL bReputeMatch = true; // 如果后面的势力需求一个都没填，默认是满足条件的

        for (int i = 0; i < QUEST_PARAM_COUNT; i++)
        {
            int nReputeLevel = 0;

            if (pQuestInfo->byRequireForceID[i] == 0)
                continue;

            bReputeMatch = false;

            nReputeLevel = m_pPlayer->m_ReputeList.GetReputeLevel(pQuestInfo->byRequireForceID[i]);
            if (nReputeLevel >= pQuestInfo->byReputeLevelMin[i] && nReputeLevel <= pQuestInfo->byReputeLevelMax[i])
            {
                bReputeMatch = true;
                break;
            }
        }
        KG_PROCESS_ERROR_RET_CODE(bReputeMatch, qrcErrorRepute);
    }
    
    // 检测前置任务状态
	if (pQuestInfo->bPrequestLogic)
	{
		for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
		{
			if (pQuestInfo->dwPrequestID[nIndex] == 0)
                continue;

            KGLOG_PROCESS_ERROR(pQuestInfo->dwPrequestID[nIndex] < MAX_QUEST_COUNT);
			
            KG_PROCESS_ERROR_RET_CODE(
                m_byQuestStateList[pQuestInfo->dwPrequestID[nIndex]] == qsFinished, qrcPrequestUnfinished
            );
		}
	}
	else
	{
        BOOL bPrequestMatch = false;
		for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
		{
			if (pQuestInfo->dwPrequestID[nIndex] == 0)
                continue;
		
			KGLOG_PROCESS_ERROR(pQuestInfo->dwPrequestID[nIndex] < MAX_QUEST_COUNT);

			if (m_byQuestStateList[pQuestInfo->dwPrequestID[nIndex]] == qsFinished)
            {
                bPrequestMatch = true;
				break;
            }
		}
		KG_PROCESS_ERROR_RET_CODE(bPrequestMatch, qrcPrequestUnfinished);
	}

	KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel >= pQuestInfo->byMinLevel, qrcTooLowLevel);
    KG_PROCESS_ERROR_RET_CODE(m_pPlayer->m_nLevel <= pQuestInfo->byMaxLevel, qrcTooHighLevel);
    
	KG_PROCESS_ERROR_RET_CODE(pQuestInfo->byRequireGender == 0 || m_pPlayer->m_eGender == pQuestInfo->byRequireGender, qrcErrorGender);
    
    // 检测玩家体型
	KG_PROCESS_ERROR_RET_CODE(pQuestInfo->byRequireRoleType == 0 || m_pPlayer->m_eRoleType == pQuestInfo->byRequireRoleType, qrcErrorRoleType);

    // Cooldown计时器
	bRetCode = m_pPlayer->m_TimerList.CheckTimer(pQuestInfo->dwCoolDownID);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, qrcCooldown);
    
    // 检测武功技能等级
	dwSkillLevel = m_pPlayer->m_SkillList.GetSkillLevel(pQuestInfo->wRequireSkillID);
	KG_PROCESS_ERROR_RET_CODE(dwSkillLevel >= pQuestInfo->byMinSkillLevel, qrcTooLowSkillLevel);

	// 检测生活技能等级
	if (pQuestInfo->byRequireLifeSkillID)
	{
		int nProfessionLevel = m_pPlayer->m_ProfessionList.GetProfessionLevel(pQuestInfo->byRequireLifeSkillID);

        nProfessionLevel += m_pPlayer->m_ProfessionList.GetProfessionAdjustLevel(pQuestInfo->byRequireLifeSkillID);

		KG_PROCESS_ERROR_RET_CODE(nProfessionLevel >= pQuestInfo->byMinLifeSkillLevel, qrcTooLowLifeLevel);
	}
    
    // 检测任务起始道具是否足够
	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		if (pQuestInfo->byStartRequireItemType[nIndex] != 0 || pQuestInfo->wStartRequireItemIndex[nIndex] != 0)
		{
			DWORD dwAmount = m_pPlayer->m_ItemList.GetItemAmountInEquipAndPackage(
				pQuestInfo->byStartRequireItemType[nIndex], pQuestInfo->wStartRequireItemIndex[nIndex]
            );
			KG_PROCESS_ERROR_RET_CODE(
                dwAmount >= pQuestInfo->wStartRequireItemAmount[nIndex], qrcNotEnoughStartItem
            );
		}
	}

    nResult = qrcSuccess;
Exit0:
    return nResult;
}

#ifdef _SERVER
QUEST_RESULT_CODE KQuestList::Present(KQuestInfo* pQuestInfo, int nPresentChoice1, int nPresentChoice2)
{
	QUEST_RESULT_CODE	nResult		        = qrcFailed;
    QUEST_RESULT_CODE	eRetCode            = qrcFailed;
	BOOL				bRetCode	        = false;
	int					nPresentPercent     = 100;
	int					nIndex		        = 0;
    int                 nLevelPercent       = 0;
    int                 nAddExp             = 0;
    int                 nMoneyPercent       = KILO_NUM;
    int                 nAddMoney           = 0;

	assert(pQuestInfo);

	// 检测是否是重复完成
    if (
        m_byQuestStateList[pQuestInfo->dwQuestID] == qsFinished ||
        (pQuestInfo->bRepeat && pQuestInfo->byRepeatCutPercent == 100)
    )
    {
	    eRetCode = PresentItems(pQuestInfo, nPresentChoice1, nPresentChoice2);
        KGLOG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
    }
    else
    {
        nPresentPercent = pQuestInfo->byRepeatCutPercent;
    }

	// 增加声望奖励
	for (nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		if (pQuestInfo->byAffectForceID[nIndex] && pQuestInfo->nAffectForceValue[nIndex])
		{
			bRetCode = m_pPlayer->m_ReputeList.ModifyReputation(
                pQuestInfo->byAffectForceID[nIndex], pQuestInfo->nAffectForceValue[nIndex]
            );
			KGLOG_PROCESS_ERROR_RET_CODE(bRetCode, qrcAddReputationFailed);
		}	
	}
    
    bRetCode = GetQuestLevelCutPercent(pQuestInfo, &nLevelPercent);
    KGLOG_PROCESS_ERROR(bRetCode);

    nAddExp = pQuestInfo->nPresentExp * nLevelPercent / 100;

	m_pPlayer->AddExp(nAddExp);

    g_pSO3World->m_StatDataServer.UpdateExpStat(m_pPlayer, nAddExp, "QUEST");

	// 奖励技能
	if (pQuestInfo->wPresentSkill > 0)
	{
        KSkill* pSkill = NULL;

        pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(pQuestInfo->wPresentSkill, 1);
        if (pSkill == NULL)
        {
            KGLogPrintf(
                KGLOG_DEBUG, 
                "Quest: %lu present skill %d not found\n", 
                pQuestInfo->dwQuestID,
                pQuestInfo->wPresentSkill
            );
            goto Exit0;
        }

        if (pSkill->m_pBaseInfo->dwBelongKungfu > 0)
        {
            int nLevel = m_pPlayer->m_SkillList.GetSkillLevel(pSkill->m_pBaseInfo->dwBelongKungfu);

            if (nLevel == 0)
            {
                bRetCode = m_pPlayer->m_SkillList.LearnSkill(pSkill->m_pBaseInfo->dwBelongKungfu, true);
                KGLOG_PROCESS_ERROR_RET_CODE(bRetCode, qrcAddSkillFailed);
            }
        }

		m_pPlayer->m_SkillList.LearnSkill(pQuestInfo->wPresentSkill, true);
	}
    
    if (m_pPlayer->m_eCamp != cNeutral)
    {
        for (int i = 0; i < MAX_DAILY_QUEST_COUNT; ++i)
        {
            if (m_DailyQuest[i].dwQuestID == pQuestInfo->dwQuestID)
            {
                int nCampLevel = g_pSO3World->m_CampInfo.GetCampLevel();
                nMoneyPercent = g_pSO3World->m_CampInfo.GetMoneyPercent(m_pPlayer->m_eCamp, nCampLevel);
                break;
            }
        }
    }
    
    nAddMoney = (pQuestInfo->nPresentMoney * nPresentPercent / 100) * nMoneyPercent / KILO_NUM;

	bRetCode = m_pPlayer->m_ItemList.AddMoney(nAddMoney);
	KGLOG_PROCESS_ERROR_RET_CODE(bRetCode, qrcAddMoneyFailed);

    g_LogClient.LogPlayerMoneyChange(nAddMoney, m_pPlayer, "quest present money");

    g_pSO3World->m_StatDataServer.UpdateMoneyStat(m_pPlayer, nAddMoney, "QUEST");

    bRetCode = AssistPresent(pQuestInfo);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    if (m_pPlayer->m_eCamp != cNeutral && pQuestInfo->nPresentPrestige > 0)
    {
        bRetCode = m_pPlayer->AddPrestige(pQuestInfo->nPresentPrestige);
        KGLOG_PROCESS_ERROR(bRetCode);

        g_pSO3World->m_StatDataServer.UpdatePrestigeStat(m_pPlayer, pQuestInfo->nPresentPrestige, "QUEST");
    }
    
    if (m_pPlayer->m_dwTongID != ERROR_ID && pQuestInfo->nPresentContribution > 0)
    {
        int nOldContribution = 0;
        int nNewContribution = 0;

        nOldContribution = m_pPlayer->m_nContribution;
        bRetCode = m_pPlayer->AddContribution(pQuestInfo->nPresentContribution);
        KGLOG_PROCESS_ERROR(bRetCode);
        nNewContribution = m_pPlayer->m_nContribution;

        if (nNewContribution != nOldContribution)
            g_pSO3World->m_StatDataServer.UpdateContributeStat(m_pPlayer, nNewContribution - nOldContribution, "QUEST");
    }

    if (pQuestInfo->nPresentTrain > 0)
    {
        bRetCode = m_pPlayer->AddTrain(pQuestInfo->nPresentTrain);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

	nResult = qrcSuccess;
Exit0:
	return nResult;
}

BOOL KQuestList::AssistPresent(KQuestInfo* pQuestInfo)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    KGFellowship*   pFellowship         = NULL;
    int             nAttractionLevel    = 0;
    BOOL            bAssistSuccess      = false;
    BOOL            bMentorAssist       = false;
    int             nQuestIndex         = -1;

    assert(pQuestInfo);
    assert(m_pPlayer);

    for (KASSIST_QUEST_LIST::iterator it = m_AssistQuestList.begin(); it != m_AssistQuestList.end(); NULL)
    {
        KPlayer*                pAssister       = NULL;
        KLevelUpData*           pLevelUpData    = NULL;
        int                     nOldThew        = 0;
        int                     nOldStamina     = 0;
        KASSIST_PRESENT_NOTIFY  Notify;

        assert(it->dwPlayerID != m_pPlayer->m_dwID);

        if (it->dwQuestID != pQuestInfo->dwQuestID)
        {
            ++it;
            continue;
        }

        pAssister = g_pSO3World->m_PlayerSet.GetObj(it->dwPlayerID);
        if (pAssister == NULL)
        {
            ++it;
            continue;
        }

        bRetCode = g_InRange(m_pPlayer, pAssister, MAX_QUEST_ASSIST_DISTANCE);
        if (!bRetCode)
        {
            ++it;
            continue;
        }

        nQuestIndex = pAssister->m_QuestList.GetQuestIndex(pQuestInfo->dwQuestID);
        if (nQuestIndex != -1)
        {
            ++it;
            continue;
        }

        // 师徒协助奖励不受每日次数限制
        bRetCode = g_pSO3World->m_MentorCache.IsMentorship(pAssister->m_dwID, m_pPlayer->m_dwID);
        if (bRetCode)
        {
            // 重复任务不能获得师徒值
            if (!pQuestInfo->bRepeat)
            {
                g_pSO3World->m_MentorCache.AddMentorValue(
                    pAssister->m_dwID, m_pPlayer->m_dwID, pQuestInfo->nAssistMentorValue
                );
            }

            bMentorAssist = true;
        }

        pLevelUpData = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(m_pPlayer->m_eRoleType, m_pPlayer->m_nLevel);
        KGLOG_PROCESS_ERROR(pLevelUpData);

        if (pAssister->m_QuestList.m_nAssistDailyCount > pLevelUpData->nMaxAssistTimes)
        {
            ++it;
            continue;
        }

        g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(
            m_pPlayer->m_dwID, it->dwPlayerID, pQuestInfo->nPresentAssistFriendship
        );

        nOldStamina = pAssister->m_nCurrentStamina;
        pAssister->m_nCurrentStamina += pQuestInfo->nPresentAssistStamina;
        MAKE_IN_RANGE(pAssister->m_nCurrentStamina, 0, pAssister->m_nMaxStamina);

        nOldThew = pAssister->m_nCurrentThew;
        pAssister->m_nCurrentThew += pQuestInfo->nPresentAssistThew;
        MAKE_IN_RANGE(pAssister->m_nCurrentThew, 0, pAssister->m_nMaxThew);

        if (pAssister->m_dwTongID != ERROR_ID && pAssister->m_dwTongID == m_pPlayer->m_dwTongID)
        {
            pAssister->AddContribution(g_pSO3World->m_Settings.m_ConstList.nAssistPresentContribution);
        }

        pAssister->m_QuestList.m_nAssistDailyCount++;
        g_PlayerServer.DoMessageNotify(pAssister->m_nConnIndex, ectSetQuestAssistDailyCount, pAssister->m_QuestList.m_nAssistDailyCount);

        Notify.dwQuestID    = pQuestInfo->dwQuestID;
        Notify.nAddStamina  = pAssister->m_nCurrentStamina - nOldStamina;
        Notify.nAddThew     = pAssister->m_nCurrentThew - nOldThew;
        g_PlayerServer.DoMessageNotify(pAssister->m_nConnIndex, ectFinishAssistQuest, 0, &Notify, sizeof(Notify));

        if (
            g_pSO3World->m_Settings.m_ConstList.nAssistQuestCountAchievementID >= 0 &&
            g_pSO3World->m_Settings.m_ConstList.nAssistQuestCountAchievementID < MAX_ACHIEVEMENT_ID && pAssister->m_pScene
        )
        {
            pAssister->m_Achievement.AddCount(g_pSO3World->m_Settings.m_ConstList.nAssistQuestCountAchievementID, 1);
        }

        it = m_AssistQuestList.erase(it);

        bAssistSuccess = true;
    }

    if (bAssistSuccess)
    {
        if (
            g_pSO3World->m_Settings.m_ConstList.nFinishQuestAssistByOtherAchievementID >= 0 &&
            g_pSO3World->m_Settings.m_ConstList.nFinishQuestAssistByOtherAchievementID < MAX_ACHIEVEMENT_ID && m_pPlayer->m_pScene
        )
        {
            m_pPlayer->m_Achievement.AddCount(g_pSO3World->m_Settings.m_ConstList.nFinishQuestAssistByOtherAchievementID, 1);
        }
        
        g_pSO3World->m_StatDataServer.UpdateAssistQuest(pQuestInfo->dwQuestID);
    }

    if (bMentorAssist)
    {
        g_pSO3World->m_StatDataServer.UpdateMentorAssistQuest(pQuestInfo->dwQuestID);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::CheckFreeRoomForPresent(KQuestInfo* pQuestInfo)
{
    BOOL    bResult             = false;
	int     nPresentItemCount   = 0;
    int     nPackageFreeRoom    = 0;

    assert(pQuestInfo);

	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		if (pQuestInfo->byPresentItemType[nIndex] == 0 || pQuestInfo->wPresentItemIndex[nIndex] == 0) 
            continue;

		nPresentItemCount++;

        if (!pQuestInfo->bPresentAll[0])
            break;
	}

	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		if (pQuestInfo->byPresentItemType[QUEST_PARAM_COUNT + nIndex] == 0)
            continue;

        if (pQuestInfo->wPresentItemIndex[QUEST_PARAM_COUNT + nIndex] == 0)
            continue;

		nPresentItemCount++;
        
        if (!pQuestInfo->bPresentAll[1])
            break;
	}

    nPackageFreeRoom = m_pPlayer->m_ItemList.GetAllPackageFreeRoomSize();
	KG_PROCESS_ERROR(nPackageFreeRoom >= nPresentItemCount);

    bResult = true;
Exit0:
    return bResult;
}

QUEST_RESULT_CODE KQuestList::CanPresentItems(KQuestInfo* pQuestInfo, int nPresentChoice1, int nPresentChoice2)
{
    QUEST_RESULT_CODE   nResult     = qrcFailed;
    QUEST_RESULT_CODE   eRetCode    = qrcFailed;
    BOOL                bRetCode    = false;

    assert(pQuestInfo);
    
    bRetCode = CheckFreeRoomForPresent(pQuestInfo);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, qrcNotEnoughFreeRoom);

	if (pQuestInfo->bPresentAll[0])
	{
		for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
		{
			if (pQuestInfo->byPresentItemType[nIndex] != 0 || pQuestInfo->wPresentItemIndex[nIndex] != 0)
			{
                eRetCode = CanPresentItem(
                    pQuestInfo->byPresentItemType[nIndex], pQuestInfo->wPresentItemIndex[nIndex], 
                    pQuestInfo->wPresentItemAmount[nIndex]
                );
                KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
			}
		}
	}
	else
	{
		if (pQuestInfo->byPresentItemType[nPresentChoice1] != 0 || pQuestInfo->wPresentItemIndex[nPresentChoice1] != 0)
		{
			eRetCode = CanPresentItem(
                pQuestInfo->byPresentItemType[nPresentChoice1], pQuestInfo->wPresentItemIndex[nPresentChoice1], 
                pQuestInfo->wPresentItemAmount[nPresentChoice1]
            );
            KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
		}
	}

	if (pQuestInfo->bPresentAll[1])
	{
		for (int nIndex = QUEST_PARAM_COUNT; nIndex < (2 * QUEST_PARAM_COUNT); nIndex++)
		{
			if (pQuestInfo->byPresentItemType[nIndex] != 0 || pQuestInfo->wPresentItemIndex[nIndex] != 0)
			{
				eRetCode = CanPresentItem(
                    pQuestInfo->byPresentItemType[nIndex], pQuestInfo->wPresentItemIndex[nIndex], 
                    pQuestInfo->wPresentItemAmount[nIndex]
                );
                KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
			}
		}
	}
	else
	{
		if (pQuestInfo->byPresentItemType[nPresentChoice2] != 0 || pQuestInfo->wPresentItemIndex[nPresentChoice2] != 0)
		{
			eRetCode = CanPresentItem(
                pQuestInfo->byPresentItemType[nPresentChoice2], pQuestInfo->wPresentItemIndex[nPresentChoice2], 
                pQuestInfo->wPresentItemAmount[nPresentChoice2]
            );
            KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
		}
	}
    
    nResult = qrcSuccess;
Exit0:
	return nResult;
}

QUEST_RESULT_CODE KQuestList::CanPresentItem(DWORD dwTabType, DWORD dwIndex, int nAmount)
{
    QUEST_RESULT_CODE   nResult         = qrcFailed;
    QUEST_RESULT_CODE   nRetCode        = qrcFailed;
    int                 nExistAmount    = 0;
    KItemInfo*          pItemInfo       = NULL;

    pItemInfo = g_pSO3World->m_ItemManager.GetItemInfo(dwTabType, dwIndex);
    KGLOG_PROCESS_ERROR(pItemInfo);

    nExistAmount = m_pPlayer->m_ItemList.GetItemTotalAmount(dwTabType, dwIndex);
    
    KG_PROCESS_ERROR_RET_CODE(pItemInfo->nMaxExistAmount == 0 || (nExistAmount + nAmount) <= pItemInfo->nMaxExistAmount, qrcAddPresentItemFailed);

    nResult = qrcSuccess;
Exit0:
	return nResult;
}

QUEST_RESULT_CODE KQuestList::PresentItems(KQuestInfo* pQuestInfo, int nPresentChoice1, int nPresentChoice2)
{
    QUEST_RESULT_CODE   nResult     = qrcFailed;
    QUEST_RESULT_CODE   nRetCode    = qrcFailed;

    assert(pQuestInfo);

    // 发放第一组任务奖励
	if (pQuestInfo->bPresentAll[0])
	{
		for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
		{
			if (pQuestInfo->byPresentItemType[nIndex] != 0 || pQuestInfo->wPresentItemIndex[nIndex] != 0)
			{
                nRetCode = PresentItem(
                    pQuestInfo->byPresentItemType[nIndex], pQuestInfo->wPresentItemIndex[nIndex], 
                    pQuestInfo->wPresentItemAmount[nIndex]
                );
                KGLOG_PROCESS_ERROR_RET_CODE(nRetCode == qrcSuccess, nRetCode);
			}
		}
	}
	else
	{
		if (pQuestInfo->byPresentItemType[nPresentChoice1] != 0 || pQuestInfo->wPresentItemIndex[nPresentChoice1] != 0)
		{
			nRetCode = PresentItem(
                pQuestInfo->byPresentItemType[nPresentChoice1], pQuestInfo->wPresentItemIndex[nPresentChoice1], 
                pQuestInfo->wPresentItemAmount[nPresentChoice1]
            );
            KGLOG_PROCESS_ERROR_RET_CODE(nRetCode == qrcSuccess, nRetCode);
		}
	}

	// 发放第二组任务道具
	if (pQuestInfo->bPresentAll[1])
	{
		for (int nIndex = QUEST_PARAM_COUNT; nIndex < (2 * QUEST_PARAM_COUNT); nIndex++)
		{
			if (pQuestInfo->byPresentItemType[nIndex] != 0 || pQuestInfo->wPresentItemIndex[nIndex] != 0)
			{
				nRetCode = PresentItem(
                    pQuestInfo->byPresentItemType[nIndex], pQuestInfo->wPresentItemIndex[nIndex], 
                    pQuestInfo->wPresentItemAmount[nIndex]
                );
                KGLOG_PROCESS_ERROR_RET_CODE(nRetCode == qrcSuccess, nRetCode);
			}
		}
	}
	else
	{
		if (pQuestInfo->byPresentItemType[nPresentChoice2] != 0 || pQuestInfo->wPresentItemIndex[nPresentChoice2] != 0)
		{
			nRetCode = PresentItem(
                pQuestInfo->byPresentItemType[nPresentChoice2], pQuestInfo->wPresentItemIndex[nPresentChoice2], 
                pQuestInfo->wPresentItemAmount[nPresentChoice2]
            );
            KGLOG_PROCESS_ERROR_RET_CODE(nRetCode == qrcSuccess, nRetCode);
		}
	}
    
    nResult = qrcSuccess;
Exit0:
	return nResult;
}

QUEST_RESULT_CODE KQuestList::PresentItem(DWORD dwTabType, DWORD dwIndex, int nAmount)
{
    QUEST_RESULT_CODE	    nResult		= qrcFailed;
    ADD_ITEM_RESULT_CODE    eRetCode    = aircFailed;
    KItem*				    pItem		= NULL;
    KITEM_LOG_INFO          ItemLogInfo;

    pItem = g_pSO3World->GenerateItem(dwTabType, dwIndex);
	KGLOG_PROCESS_ERROR_RET_CODE(pItem, qrcErrorGenerateItem);

	if (pItem->m_Common.nGenre == igBook)
	{
		pItem->m_nCurrentDurability = nAmount;
	}
	else if (pItem->IsStackable())
	{
        if (nAmount > pItem->m_nMaxStackNum)
        {
            KGLogPrintf(KGLOG_DEBUG, "The quest %d present item (SetStackNum) error!\n");
        }

		pItem->SetStackNum(nAmount);
	}
    
    g_LogClient.GetItemLogInfo(pItem, &ItemLogInfo);

	eRetCode = m_pPlayer->m_ItemList.AddItem(pItem);
	KGLOG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, qrcAddItemFailed);

    g_LogClient.LogPlayerItemChange(m_pPlayer, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "finish quest");

    nResult = qrcSuccess;
Exit0:
    if (nResult != qrcSuccess)
    {
	    if (pItem)
	    {
		    g_pSO3World->m_ItemManager.FreeItem(pItem);
		    pItem = NULL;
	    }
    }
	return nResult;
}

BOOL KQuestList::UnregisterQuestKillNpc(KQuestInfo* pQuestInfo)
{
    BOOL                        bResult = false;
    MAP_NPC_2_QUEST::iterator   it;
    MAP_NPC_2_QUEST::iterator   UpperBoundIt;

    assert(pQuestInfo);

	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; ++nIndex)
	{
		DWORD dwTemplateID = pQuestInfo->dwKillNpcTemplateID[nIndex];

		if (dwTemplateID == ERROR_ID)
            continue;
        
        it =  m_mapNpc2Quest.lower_bound(dwTemplateID);
        UpperBoundIt = m_mapNpc2Quest.upper_bound(dwTemplateID);

		while (it != UpperBoundIt)
		{
			int nQuestIndex = it->second.nQuestIndex;

			KGLOG_PROCESS_ERROR(nQuestIndex >= 0 && nQuestIndex < MAX_ACCEPT_QUEST_COUNT);

			if (m_AcceptedQuestList[nQuestIndex].pQuestInfo && m_AcceptedQuestList[nQuestIndex].pQuestInfo->dwQuestID == pQuestInfo->dwQuestID)
			{
				m_mapNpc2Quest.erase(it);
				break;
			}

            ++it;
		}
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::CostQuestRequireItem(KQuestInfo* pQuestInfo)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
	DWORD       dwItemType	    = 0;
	DWORD       dwItemIndex     = 0;
    KItemInfo*  pItemInfo       = NULL;
    int         nBookRecipeID   = -1;

	for (int nIndex = 0; nIndex < QUEST_END_ITEM_COUNT; nIndex++)
	{
		int     nIndex2         = 0;
        DWORD   dwCurrentAmount = 0;
        DWORD   dwCostAmount    = 0;
        
        dwItemType	    = pQuestInfo->byEndRequireItemType[nIndex];
        dwItemIndex     = pQuestInfo->wEndRequireItemIndex[nIndex];
        dwCostAmount    = pQuestInfo->wEndRequireItemAmount[nIndex];
        
        if (dwItemType == 0 || dwItemIndex == 0)
            continue;
        
        if (!pQuestInfo->bIsDeleteEndRequireItem[nIndex])
            continue;

        pItemInfo = g_pSO3World->m_ItemManager.GetItemInfo(dwItemType, dwItemIndex);
        KGLOG_PROCESS_ERROR(pItemInfo);

		for (nIndex2 = 0; nIndex2 < nIndex; nIndex2++)
		{
			if (
                dwItemType == pQuestInfo->byEndRequireItemType[nIndex2] && 
                dwItemIndex == pQuestInfo->wEndRequireItemIndex[nIndex2]
            )
                break;
		}
        
        // 与前面一个任务道具相同,就不删除了;策划有这个需求！
        // 书籍都要扣
		if (pItemInfo->nGenre != igBook && nIndex2 != nIndex)
			continue; 

        if (pItemInfo->nGenre == igBook)
        {
            dwCostAmount = 1;
            nBookRecipeID = pQuestInfo->wEndRequireItemAmount[nIndex];
        }
        
        dwCurrentAmount = m_pPlayer->m_ItemList.GetItemTotalAmount(dwItemType, dwItemIndex, nBookRecipeID);
        dwCostAmount = min(dwCurrentAmount, dwCostAmount);

        if (dwCostAmount == 0)
        {
            continue;
        }

		bRetCode = m_pPlayer->m_ItemList.CostItemInAllBox(
            dwItemType, dwItemIndex, dwCostAmount, nBookRecipeID
        );
		KGLOG_PROCESS_ERROR(bRetCode);
	}

    bResult = true;
Exit0:
    return bResult;
}

BOOL KQuestList::CostQuestOfferItem(KQuestInfo* pQuestInfo)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    DWORD   dwCurrentAmount = 0;
    DWORD   dwCostAmount    = 0;

	for (int nIndex = 0; nIndex < QUEST_PARAM_COUNT; nIndex++)
	{
		DWORD dwTabType	    = pQuestInfo->byOfferItemType[nIndex];
		DWORD dwTabIndex    = pQuestInfo->wOfferItemIndex[nIndex];
        
        if (dwTabType == 0 || dwTabIndex == 0)
        {
            continue;
        }

		dwCurrentAmount = m_pPlayer->m_ItemList.GetItemTotalAmount(dwTabType, dwTabIndex);
        dwCostAmount = pQuestInfo->wOfferItemAmount[nIndex];

        if (dwCostAmount > dwCurrentAmount)
        {
            dwCostAmount = dwCurrentAmount;
        }
        
        if (dwCostAmount == 0)
        {
            continue;
        }

		bRetCode = m_pPlayer->m_ItemList.CostItemInAllBox(dwTabType, dwTabIndex, dwCostAmount);
		KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

void KQuestList::RegisterQuestAssistInfo(DWORD dwPlayerID, DWORD dwQuestID)
{
    BOOL            bRetCode        = 0;
    KPlayer*        pAssister       = NULL;
    QUEST_STATE     eQuestState     = qsInvalid;
    int             nQuestIndex     = 0;
    KASSIST_QUEST   AssistQuest;

    // 注意,每日协助次数不在Register的时候限制,而在协助奖励的时候限制
    // 因为师徒奖励不受协助次数限制

    assert(m_pPlayer);
    assert(dwPlayerID != m_pPlayer->m_dwID);

    pAssister = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pAssister);
    
    bRetCode = g_InRange(m_pPlayer, pAssister, MAX_QUEST_ASSIST_DISTANCE);
    KG_PROCESS_ERROR(bRetCode);

    KG_PROCESS_ERROR(pAssister->m_nLevel >= m_pPlayer->m_nLevel);

    nQuestIndex = pAssister->m_QuestList.GetQuestIndex(dwQuestID);
    KG_PROCESS_ERROR(nQuestIndex == -1); // 必须没有这个任务

    for (KASSIST_QUEST_LIST::iterator it = m_AssistQuestList.begin(); it != m_AssistQuestList.end(); ++it)
    {
        if (it->dwPlayerID == dwPlayerID && it->dwQuestID == dwQuestID)
        {
            goto Exit0;
        }
    }

    AssistQuest.dwPlayerID  = dwPlayerID;
    AssistQuest.dwQuestID   = dwQuestID;

    m_AssistQuestList.push_back(AssistQuest);

    if (m_AssistQuestList.size() > MAX_ASSIST_QUEST_COUNT)
    {
        m_AssistQuestList.pop_front();
    }

Exit0:
    return;
}
#endif

#ifdef _CLIENT
BOOL KQuestList::KUpdateQuestMarkFunc::operator ()(KNpc *pNpc)
{
    KUIEventQuestMarkUpdate Param = {pNpc->m_dwID};
	if (m_nQuestID == -1)
	{
		if (g_pGameWorldUIHandler)
        {
            g_pGameWorldUIHandler->OnQuestMarkUpdate(Param);
        }
	}
	else
	{ 
		KQuestInfo *pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(m_nQuestID);
		KGLOG_PROCESS_ERROR(pQuestInfo);

		if (
            (pQuestInfo->dwStartNpcTemplateID == pNpc->m_dwTemplateID) ||
			(pQuestInfo->dwEndNpcTemplateID == pNpc->m_dwTemplateID) 
        )
		{
            if (g_pGameWorldUIHandler)
            {
                g_pGameWorldUIHandler->OnQuestMarkUpdate(Param);
            }
		}
	}
Exit0:
	return true;
}
#endif

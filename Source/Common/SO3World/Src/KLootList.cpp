#include <vector>
#include "stdafx.h"
#include "KLootList.h"
#include "KPlayer.h"
#include "KThreatList.h"
#include "KPlayerServer.h"
#include "KMath.h"
#include "KSO3World.h"
#include "KDoodad.h"
#include "KScene.h"

BOOL g_IsInLootRange(KSceneObject* pObjectA, KSceneObject* pObjectB, int nRegionRange)
{
    BOOL bResult            = false;
    BOOL bRetCode           = false;
    int  nObjectARegionX    = 0;
    int  nObjectARegionY    = 0;
    int  nObjectBRegionX    = 0;
    int  nObjectBRegionY    = 0;

    assert(pObjectA);
    assert(pObjectB);

    KG_PROCESS_ERROR(pObjectA->m_pScene && pObjectA->m_pScene == pObjectB->m_pScene);
    
    nObjectARegionX = pObjectA->m_nX / REGION_GRID_WIDTH / CELL_LENGTH;
    nObjectARegionY = pObjectA->m_nY / REGION_GRID_WIDTH / CELL_LENGTH;
    
    nObjectBRegionX = pObjectB->m_nX / REGION_GRID_WIDTH / CELL_LENGTH;
    nObjectBRegionY = pObjectB->m_nY / REGION_GRID_WIDTH / CELL_LENGTH;
    
    KG_PROCESS_ERROR(abs(nObjectARegionX - nObjectBRegionX) <= nRegionRange);
    KG_PROCESS_ERROR(abs(nObjectARegionY - nObjectBRegionY) <= nRegionRange);

    bResult = true;
Exit0:
    return bResult;
}

KLootList::~KLootList(void)
{
	for (int i = 0; i < MAX_LOOT_SIZE; ++i)
	{
		assert(m_ItemList[i].pItem == NULL); // 检查是否调用了UnInit
	}
}

BOOL KLootList::Init(int nOverLootFrame)
{
	memset(m_ItemList, 0, sizeof(m_ItemList));
    memset(m_LooterList, 0, sizeof(m_LooterList));

    m_nLooterCount      = 0;
	m_nItemCount        = 0;
	m_nMoney            = 0;
	
#ifdef _SERVER
    m_nLootedCount      = 0;
    m_nOverLootFrame    = g_pSO3World->m_nGameLoop + nOverLootFrame;
    m_nEndRollFrame     = 0;
    m_bIsOpened         = false;
    memset(m_OfflineFlag, 0, sizeof(m_OfflineFlag));
    m_bLootListChanged = false;
#endif

	return true;
}

void KLootList::UnInit(void)
{
	for (int i = 0; i < MAX_LOOT_SIZE; ++i)
	{
		if (m_ItemList[i].pItem)
		{
			g_pSO3World->m_ItemManager.FreeItem(m_ItemList[i].pItem);
			m_ItemList[i].pItem = NULL;
		}
	}

	return;
}

#ifdef _SERVER
void KLootList::Activate(KDoodad* pDoodad)
{
    BOOL bRetCode           = false;

    assert(pDoodad);
    KG_PROCESS_ERROR(pDoodad->m_pScene);

    ProcessRollerOffline(pDoodad);
    ProcessOverRollTime(pDoodad);
    ProcessOverLootTime(pDoodad);    

    KG_PROCESS_ERROR(m_bLootListChanged);

    for (int i = 0; i < m_nLooterCount; ++i)
    {
        KPlayer* pLooter = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);
		
        if (pLooter == NULL)
        {
            continue;
        }

        bRetCode = g_IsInLootRange(pLooter, pDoodad, pDoodad->m_pScene->m_nBroadcastRegion);
	    if (!bRetCode)
        {
		    continue;
        }

        g_PlayerServer.DoSyncLootList(pLooter, pDoodad);
    }

    m_bLootListChanged = false;

Exit0:
	return;
}

BOOL KLootList::IsEmpty()
{
	return (m_nItemCount == m_nLootedCount && m_nMoney == 0);
}

BOOL KLootList::CanLoot(int nItemIndex, KPlayer* pPlayer)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
    BOOL        bItemOwner  = false;
    
	assert(nItemIndex >= 0 && nItemIndex < MAX_LOOT_SIZE);
	assert(pPlayer);

	KGLOG_PROCESS_ERROR(m_ItemList[nItemIndex].pItem);
    KGLOG_PROCESS_ERROR(m_ItemList[nItemIndex].eLootItemType != litInvalid);
    KG_PROCESS_ERROR(m_ItemList[nItemIndex].eLootItemType != litNeedDistribute);
    
    bItemOwner = IsItemOwner(nItemIndex, pPlayer->m_dwID);

    switch(m_ItemList[nItemIndex].eLootItemType)
    {
    case litOwnerLoot:
    case litOverTimeLooterFree:
        KG_PROCESS_ERROR(bItemOwner);
    	break;
    case litAbsoluteFree:
        bRetCode = CanLootAbsoluteFreeQuestItem(nItemIndex, pPlayer);
        KG_PROCESS_ERROR(bRetCode);
    case litLooterFree:
        break;
    case litNeedRoll:
        KG_PROCESS_ERROR(m_ItemList[nItemIndex].byRollCount == 0); // 还有人没有roll完
        KG_PROCESS_ERROR(m_ItemList[nItemIndex].dwOwnerIDSet == 0 || bItemOwner); // 没有拥有者或者自己就是拥有者
        break;
    default:
        break;
    }

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

BOOL KLootList::CanRoll(int nItemIndex, KPlayer* pPlayer)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
    int     nPlayerIndex    = 0;

	assert(nItemIndex >= 0 && nItemIndex < MAX_LOOT_SIZE);
	assert(pPlayer);

    KGLOG_PROCESS_ERROR(m_bIsOpened);   // 必须是已经打开的
    KG_PROCESS_ERROR(m_ItemList[nItemIndex].eLootItemType == litNeedRoll);
	KG_PROCESS_ERROR(m_ItemList[nItemIndex].pItem);
    
    nPlayerIndex = GetLooterPos(pPlayer->m_dwID);
    KG_PROCESS_ERROR(nPlayerIndex >= 0 && nPlayerIndex < m_nLooterCount);

    KG_PROCESS_ERROR(!(m_ItemList[nItemIndex].bRollFlag[nPlayerIndex])); // bRollFlag为true表示没权限

	bResult = true;
Exit0:
	return bResult;
}

BOOL KLootList::CanDisplayInClient(int nItemIndex, KPlayer* pPlayer)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    
    assert(nItemIndex >= 0 && nItemIndex < MAX_LOOT_SIZE);
    assert(pPlayer);

    bRetCode = CanLoot(nItemIndex, pPlayer);
    if (bRetCode)
    {
        bResult = true;
        goto Exit0;
    }
    
    if (
        m_ItemList[nItemIndex].eLootItemType == litNeedRoll ||
        m_ItemList[nItemIndex].eLootItemType == litNeedDistribute
    )
    {
        bRetCode = IsInLooterList(pPlayer->m_dwID);
        if (bRetCode)
        {
            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

BOOL KLootList::SetItemLooter(int nItemIndex, DWORD dwLooterID)
{
    BOOL bResult        = false;
    int  nLooterIndex   = -1;

    assert(nItemIndex >= 0 && nItemIndex < MAX_LOOT_SIZE);
    
    nLooterIndex = GetLooterPos(dwLooterID);
    KGLOG_PROCESS_ERROR(nLooterIndex != -1);
    
    m_ItemList[nItemIndex].dwOwnerIDSet |= (0x01 << nLooterIndex);

	bResult = true;
Exit0:
    return bResult;
}

// 返回第一个拥有者
DWORD KLootList::GetItemLooter(int nItemIndex)
{
    DWORD dwResult      = ERROR_ID;
    DWORD dwOwnerID     = ERROR_ID;
    DWORD dwOwnerIDSet  = 0;

    assert(nItemIndex >= 0 && nItemIndex < MAX_LOOT_SIZE);
    
    dwOwnerIDSet = m_ItemList[nItemIndex].dwOwnerIDSet;
    KG_PROCESS_ERROR(dwOwnerIDSet != 0);
    
    for (int i = 0; i < m_nLooterCount; ++i)
    {
        if (dwOwnerIDSet & (0x01 << i))
        {
            dwOwnerID = m_LooterList[i];
            break;
        }
    }
    
    dwResult = dwOwnerID;
Exit0:
    return dwResult;
}

BOOL KLootList::IsItemOwner(int nItemIndex, DWORD dwLooterID)
{
    BOOL    bResult         = false;
    DWORD   dwOwnerIDSet    = 0;
    int     nLootPos        = -1;

    assert(nItemIndex >= 0 && nItemIndex < MAX_LOOT_SIZE);
    
    dwOwnerIDSet = m_ItemList[nItemIndex].dwOwnerIDSet;
    KG_PROCESS_ERROR(dwOwnerIDSet != 0);
    
    nLootPos = GetLooterPos(dwLooterID);
    KG_PROCESS_ERROR(nLootPos != -1);
    KG_PROCESS_ERROR(dwOwnerIDSet & (0x01 << nLootPos));

	bResult = true;
Exit0:
    return bResult;
}

void KLootList::OnOpen(KDoodad* pDoodad)
{    
    assert(pDoodad);
    KG_PROCESS_ERROR(pDoodad->m_pScene);
    KG_PROCESS_ERROR(!m_bIsOpened);

    for (int i = 0; i < m_nItemCount; ++i)
	{
        if (m_ItemList[i].eLootItemType != litNeedRoll)
        {
            continue;
        }

        if (m_ItemList[i].pItem == NULL)
        {
            KGLogPrintf(KGLOG_ERR, "Item is null!\n");
            continue;
        }

		m_ItemList[i].byRollCount = 0;
        
        for (int j = 0; j < m_nLooterCount; ++j)
        {
            KPlayer* pRoller = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[j]);
			if (pRoller == NULL)
				continue;

			if (g_IsInLootRange(pRoller, pDoodad, pDoodad->m_pScene->m_nBroadcastRegion))
			{
                g_PlayerServer.DoSyncLootList(pRoller, pDoodad);

                ++m_ItemList[i].byRollCount;
				g_PlayerServer.DoBeginRollItem(pRoller->m_nConnIndex, m_ItemList[i].pItem, pDoodad, MAX_ROLL_FRAME);
			}
        }

        if (m_nEndRollFrame == 0) // 首次点击且有物品需要roll
        {
            m_nEndRollFrame = g_pSO3World->m_nGameLoop + MAX_ROLL_FRAME;
            if (m_nOverLootFrame < m_nEndRollFrame)
            {
                assert(pDoodad->m_pTemplate);
                m_nOverLootFrame = g_pSO3World->m_nGameLoop + pDoodad->m_pTemplate->m_nOverLootFrame;
            }

            if (pDoodad->m_nDisappearFrames < m_nEndRollFrame)
            {
                assert(pDoodad->m_pTemplate);
		        pDoodad->SetDisappearFrames(pDoodad->m_pTemplate->m_nRemoveDelay);
            }
        }
	}

	// 非尸体类型的Doodad被打开了,设置删除时间,尸体类型在生成时已经设置了
	if (pDoodad->m_eKind != dkCorpse)
	{
		pDoodad->SetDelayRemove();
	}    

    m_bIsOpened = true;
Exit0:
	return;
}

BOOL KLootList::GenerateLooterList(KPlayer* pAttacker, KDoodad* pDoodad)
{
    BOOL    bResult   = false;
	BOOL    bRetCode  = false;
    DWORD   dwTeamID  = ERROR_ID;
    KTeam*  pTeam     = NULL;

	assert(pAttacker);
    assert(pDoodad);
    KGLOG_PROCESS_ERROR(pDoodad->m_pScene);

    m_nLooterCount = 0;

    dwTeamID = pAttacker->GetCurrentTeamID();
    if (dwTeamID == ERROR_ID)
    {
        // 当达到18级的免费玩家生成掉落的时候，将无法拾取
        if (
            pAttacker->m_nLevel < g_pSO3World->m_Settings.m_ConstList.nFreeMaxLevel ||
            !pAttacker->m_bFreeLimitFlag
        )
        {
            m_LooterList[m_nLooterCount++] = pAttacker->m_dwID;
        }
        goto Exit1;
    }

    pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
	KGLOG_PROCESS_ERROR(pTeam);
    
	for (int i = 0; i < pTeam->nGroupNum; i++)
	{
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);
            if (pMember == NULL)
            {
                continue;
            }

            bRetCode = g_IsInLootRange(pMember, pDoodad, pDoodad->m_pScene->m_nBroadcastRegion);
            if (!bRetCode)
            {
                continue;
            }

            // 当达到18级的免费玩家生成掉落的时候，将无法拾取
            if (
                pMember->m_nLevel < g_pSO3World->m_Settings.m_ConstList.nFreeMaxLevel ||
                !pMember->m_bFreeLimitFlag
            )
            {
                assert(m_nLooterCount < MAX_PARTY_SIZE);
                m_LooterList[m_nLooterCount++] = pMember->m_dwID;
            }
        }
	}

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

void KLootList::ProcessNpcDropItems()
{
    KPlayer*    pLooter         = NULL;
    DWORD       dwTeamID        = ERROR_ID;
    KTeam*      pTeam           = NULL;
    DWORD       dwMaxRollCount  = 0;
    size_t      uLooterCount    = 0;

	pLooter = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[0]);
	KG_PROCESS_ERROR(pLooter);
    
    dwTeamID = pLooter->GetCurrentTeamID();
    if (dwTeamID == ERROR_ID)
    {
        for (int i = 0; i < m_nItemCount; ++i)
	    {
            if (m_ItemList[i].dwOwnerIDSet != 0) // 已经设置过
            {
                continue;
            }

            SetItemLooter(i, pLooter->m_dwID);
            m_ItemList[i].eLootItemType = litOwnerLoot;
        }
        goto Exit0;
    }

	for (int i = 0; i < m_nLooterCount; ++i)
	{
		KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);
		
        if (pPlayer == NULL)
        {
            continue;
        }

        ++pPlayer->m_dwRoundRollCount;

		if (pPlayer->m_dwRoundRollCount > dwMaxRollCount)
		{
			dwMaxRollCount = pPlayer->m_dwRoundRollCount;
			pLooter = pPlayer;
		}
	}
    
    assert(pLooter);
	pLooter->m_dwRoundRollCount = 0;
    
    pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

	for (int i = 0; i < m_nItemCount; ++i)
	{
        int nQuality = 0;

        KGLOG_PROCESS_ERROR(m_ItemList[i].pItem);

		nQuality = m_ItemList[i].pItem->m_GenParam.nQuality;

		// 已经设置了归属就不用再设置了,一般是任务道具
		if (m_ItemList[i].dwOwnerIDSet != 0)
        {
            continue;
        }

		switch(pTeam->nLootMode)
		{
		case ilmFreeForAll:
			m_ItemList[i].eLootItemType = litLooterFree;
			break;
		case ilmDistribute:
			if (nQuality >= pTeam->nRollQuality)
			{
                m_ItemList[i].eLootItemType = litNeedDistribute;
			}
            else
            {
                SetItemLooter(i, pLooter->m_dwID);
                m_ItemList[i].eLootItemType = litOverTimeLooterFree;
            }
			break;
		case ilmGroupLoot:
			if (nQuality >= pTeam->nRollQuality)
			{
                m_ItemList[i].eLootItemType = litNeedRoll;
			}
            else
            {
                SetItemLooter(i, pLooter->m_dwID);
                m_ItemList[i].eLootItemType = litOverTimeLooterFree;
            }
            
			break;
		default:
			assert(false);
		}

        KGLOG_PROCESS_ERROR(m_ItemList[i].eLootItemType != litInvalid);
	}

Exit0:
	return;
}

LOOT_ITEM_RESULT_CODE KLootList::LootItem(KPlayer* pLooter, KDoodad* pDoodad, DWORD dwItemID)
{
    LOOT_ITEM_RESULT_CODE nResult       = lircFailed;
    ADD_ITEM_RESULT_CODE  eRetCode      = aircFailed;
    BOOL                  bRetCode      = false;
    int                   nItemIndex    = MAX_LOOT_SIZE;
    int                   nItemAmount   = 0;
    KItem*                pItemToLoot   = NULL;
    int                   nBindType     = ibtTotal;
    LOOT_ITEM_TYPE        eType         = litInvalid;
    KITEM_LOG_INFO        ItemLogInfo;

    assert(pLooter);
    assert(pDoodad);
    
    KG_PROCESS_ERROR(m_bIsOpened);   // 必须是已经打开的

	for (int i = 0; i < MAX_LOOT_SIZE; ++i)
	{
		if (m_ItemList[i].pItem != NULL && m_ItemList[i].pItem->m_dwID == dwItemID)
        {
            nItemIndex = i;
            break;
        }
	}
	KG_PROCESS_ERROR_RET_CODE(nItemIndex < MAX_LOOT_SIZE, lircNotExistLootItem);

	bRetCode = CanLoot(nItemIndex, pLooter);
    KG_PROCESS_ERROR(bRetCode);
    
    pItemToLoot = m_ItemList[nItemIndex].pItem;

    eRetCode = pLooter->m_ItemList.CanAddItem(pItemToLoot);
    KG_PROCESS_ERROR_RET_CODE(eRetCode != aircItemAmountLimited, lircOverItemLimit);
    KG_PROCESS_ERROR_RET_CODE(eRetCode != aircNotEnoughFreeRoom, lircInventoryIsFull);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, lircAddLootItemFailed);
    
    eType = m_ItemList[nItemIndex].eLootItemType;
    nBindType = pItemToLoot->m_Common.nBindType;

    m_ItemList[nItemIndex].eLootItemType = litInvalid;
    m_ItemList[nItemIndex].pItem = NULL;
	++m_nLootedCount;
    
    g_PlayerServer.DoSyncLootList(pLooter, pDoodad); // 添加item前通知客户端删掉即将要添加的物品

    nItemAmount = pItemToLoot->GetStackNum();
    
    g_LogClient.GetItemLogInfo(pItemToLoot, &ItemLogInfo);
	eRetCode = pLooter->m_ItemList.AddItem(pItemToLoot);
	KGLOG_PROCESS_ERROR(eRetCode == aircSuccess);

    if (m_ItemList[nItemIndex].dwQuestID != 0)
    {
        pLooter->m_QuestList.RegisterTeamAssistance(m_ItemList[nItemIndex].dwQuestID);
    }
    
    for (int i = 0; i < m_nLooterCount; ++i)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);

        if (pPlayer && pPlayer->m_dwID != pLooter->m_dwID)
        {
            g_PlayerServer.DoAddItemNotify(pLooter, dwItemID, nItemAmount, pPlayer->m_nConnIndex);
            g_PlayerServer.DoSyncLootList(pPlayer, pDoodad);
        }
    }
    
    if (pItemToLoot->m_GenParam.nQuality >= g_pSO3World->m_Settings.m_ConstList.nLootItemQualityToLog)
    {
        if (pDoodad->m_eKind == dkCorpse)
        {
            g_LogClient.LogPlayerItemChange(pLooter, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "npc drop item");
        }
        else
        {
            g_LogClient.LogPlayerItemChange(pLooter, ITEM_OPERATION_TYPE_IN, &ItemLogInfo, "doodad drop item");
        }
    }
    
    if (nBindType == ibtBindOnTimeLimitation)
    {
        ProcessBindOnTimeLimitationItem(pDoodad, dwItemID, pLooter->m_dwID, eType);
    }

    nResult = lircSuccess;
Exit0:
    return nResult;
}

LOOT_ITEM_RESULT_CODE KLootList::CancelRollItem(KPlayer* pRoller, KDoodad* pDoodad, DWORD dwItemID)
{
    LOOT_ITEM_RESULT_CODE nResult       = lircFailed;
    BOOL                  bRetCode      = false;
    int                   nItemIndex    = MAX_LOOT_SIZE;
    int                   nRollerIndex  = MAX_PARTY_SIZE;

    assert(pRoller);
    assert(pDoodad);

	for (int i = 0; i < MAX_LOOT_SIZE; ++i)
	{
		if (m_ItemList[i].pItem != NULL && m_ItemList[i].pItem->m_dwID == dwItemID)
        {
            nItemIndex = i;
            break;
        }
	}
	KG_PROCESS_ERROR_RET_CODE(nItemIndex < MAX_LOOT_SIZE, lircNotExistLootItem);

    bRetCode = CanRoll(nItemIndex, pRoller);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < m_nLooterCount; ++i)
	{
        if (m_LooterList[i] == pRoller->m_dwID)
        {
            nRollerIndex = (int)i;
            break;
        }
	}
    KGLOG_PROCESS_ERROR(nRollerIndex < m_nLooterCount);
    
    m_ItemList[nItemIndex].bRollFlag[nRollerIndex] = true;
    --m_ItemList[nItemIndex].byRollCount;
    
    for (int i = 0; i < m_nLooterCount; ++i)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);

        if (pPlayer)
        {
            g_PlayerServer.DoRollItemMessage(pPlayer->m_nConnIndex, pRoller, -1, pDoodad->m_dwID, dwItemID);
        }
    }
    
    if (m_ItemList[nItemIndex].byRollCount == 0)
    {
        bRetCode = EndRollItem(nItemIndex, pDoodad);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    nResult = lircSuccess;
Exit0:
    return nResult;
}

LOOT_ITEM_RESULT_CODE KLootList::RollItem(KPlayer* pRoller, KDoodad* pDoodad, DWORD dwItemID)
{
    LOOT_ITEM_RESULT_CODE nResult       = lircFailed;
    ADD_ITEM_RESULT_CODE  eRetCode      = aircFailed;
    BOOL                  bRetCode      = false;
    int                   nItemIndex    = MAX_LOOT_SIZE;
    int                   nRollPoint    = -1;
    int                   nRollerIndex  = MAX_PARTY_SIZE;

    assert(pRoller);
    assert(pDoodad);

	for (int i = 0; i < MAX_LOOT_SIZE; ++i)
	{
		if (m_ItemList[i].pItem != NULL && m_ItemList[i].pItem->m_dwID == dwItemID)
        {
            nItemIndex = i;
            break;
        }
	}
	KG_PROCESS_ERROR_RET_CODE(nItemIndex < MAX_LOOT_SIZE, lircNotExistLootItem);

    bRetCode = CanRoll(nItemIndex, pRoller);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < m_nLooterCount; ++i)
	{
        if (m_LooterList[i] == pRoller->m_dwID)
        {
            nRollerIndex = i;
            break;
        }
	}
    KGLOG_PROCESS_ERROR(nRollerIndex < m_nLooterCount);
    
	nRollPoint = g_Random(ROLL_ITEM_NUMBER) + 1;
	if (nRollPoint > m_ItemList[nItemIndex].nMaxRollPoint)
	{
        m_ItemList[nItemIndex].dwOwnerIDSet = 0;
        SetItemLooter(nItemIndex, pRoller->m_dwID);
		m_ItemList[nItemIndex].nMaxRollPoint = nRollPoint;
	}
    else if (nRollPoint == m_ItemList[nItemIndex].nMaxRollPoint)
	{
		int nRoll = g_Random(KILO_NUM);
		if (nRoll > KILO_NUM / 2)
		{
            m_ItemList[nItemIndex].dwOwnerIDSet = 0;
			SetItemLooter(nItemIndex, pRoller->m_dwID);
		}
	}

	m_ItemList[nItemIndex].bRollFlag[nRollerIndex] = true;
	--m_ItemList[nItemIndex].byRollCount;
    
    for (int i = 0; i < m_nLooterCount; ++i)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);
        if (pPlayer == NULL)
        {
            continue;
        }

        g_PlayerServer.DoRollItemMessage(
            pPlayer->m_nConnIndex, pRoller, nRollPoint, pDoodad->m_dwID, dwItemID
        );
    }

    if (m_ItemList[nItemIndex].byRollCount == 0)
    {
        bRetCode = EndRollItem(nItemIndex, pDoodad);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    nResult = lircSuccess;
Exit0:
    return nResult;
}

LOOT_ITEM_RESULT_CODE KLootList::LootMoney(KPlayer* pLooter, KDoodad* pDoodad)
{
	LOOT_ITEM_RESULT_CODE   nResult             = lircFailed;
	BOOL                    bRetCode            = false;
    DWORD                   dwTeamID            = ERROR_ID;
    int                     nAddMoneyBase       = 0;
	int                     nMoney              = 0; 
    int                     nLooterCount        = 0;
   	std::vector<DWORD>      vecLooterList;

    assert(pLooter);
    assert(pDoodad);

	KG_PROCESS_SUCCESS(pDoodad->m_pLootList == NULL); // 已经被人捡光了

    dwTeamID = pLooter->GetCurrentTeamID();

	nMoney = pDoodad->m_pLootList->GetMoney();
	KG_PROCESS_SUCCESS(nMoney <= 0); // 钱刚好已经被其他玩家拾取完

    bRetCode = pDoodad->m_pLootList->SetMoney(0);
	KGLOG_PROCESS_ERROR(bRetCode);

	if (dwTeamID == ERROR_ID)
    {
		bRetCode = pLooter->m_ItemList.AddMoney(nMoney);
		KGLOG_PROCESS_ERROR(bRetCode);
        
        if (nMoney >= 100000)
        {
            g_LogClient.LogPlayerMoneyChange(nMoney, pLooter, "loot from npc or doodad");
        }

        g_pSO3World->m_StatDataServer.UpdateMoneyStat(pLooter, nMoney, "LOOT");
        
        g_PlayerServer.DoSyncLootList(pLooter, pDoodad);
    }
    else
    {
        // in party
	    GetLooterList(vecLooterList);

        nLooterCount = (int)vecLooterList.size();
        KGLOG_PROCESS_ERROR(nLooterCount > 0);

        nAddMoneyBase = nMoney / nLooterCount;

	    for (int nIndex = 0; nIndex < nLooterCount; nIndex++)
	    {
		    KPlayer* pMember    = g_pSO3World->m_PlayerSet.GetObj(vecLooterList[nIndex]);
            int      nAddMoney  = nAddMoneyBase;

            if (pMember == NULL)
            {
                continue;
            }

		    if (nIndex < (nMoney % nLooterCount))
            {
                nAddMoney = nAddMoneyBase + 1;
            }
    		
		    bRetCode = pMember->m_ItemList.AddMoney(nAddMoney);
		    KGLOG_CHECK_ERROR(bRetCode);
            
            if (nAddMoney > 100000)
            {
                g_LogClient.LogPlayerMoneyChange(nAddMoney, pMember, "loot from npc or doodad");
            }

            g_pSO3World->m_StatDataServer.UpdateMoneyStat(pMember, nAddMoney, "LOOT");

		    g_PlayerServer.DoSyncLootList(pMember, pDoodad);
	    }
    }

Exit1:
	nResult = lircSuccess;
Exit0:
	return nResult;
}

LOOT_ITEM_RESULT_CODE KLootList::DistributeItem(
    KPlayer* pDistributer, KPlayer* pDstPlayer, KDoodad* pDoodad, DWORD dwItemID
)
{
	LOOT_ITEM_RESULT_CODE   nResult             = lircFailed;
    ADD_ITEM_RESULT_CODE    eRetCode            = aircFailed;
    BOOL                    bRetCode            = false;
    int                     nItemIndex          = MAX_LOOT_SIZE;
    int                     nItemAmount         = 0;
    KItem*                  pItemToAdd          = NULL;
    DWORD                   dwTeamID            = ERROR_ID;
    DWORD                   dwLooter            = ERROR_ID;
    KTeam*                  pTeam               = NULL;
    DWORD                   dwDistributeManID   = ERROR_ID;
    int                     nBindType           = ibtTotal;

	assert(pDistributer);
	assert(pDstPlayer);
	assert(pDoodad);

	KG_PROCESS_ERROR(pDoodad->m_pLootList);

	for (int i = 0; i < MAX_LOOT_SIZE; ++i)
	{
		if ((m_ItemList[i].pItem != NULL) && (m_ItemList[i].pItem->m_dwID == dwItemID))
        {
            nItemIndex = i;
            break;
        }
	}

	KG_PROCESS_ERROR_RET_CODE(nItemIndex < MAX_LOOT_SIZE, lircNotExistLootItem);    
    KGLOG_PROCESS_ERROR(m_ItemList[nItemIndex].eLootItemType == litNeedDistribute);

    dwTeamID = pDistributer->GetCurrentTeamID();
    KGLOG_PROCESS_ERROR(dwTeamID != ERROR_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);
    KGLOG_PROCESS_ERROR(pTeam->dwAuthority[tatDistribute] == pDistributer->m_dwID);

	bRetCode = IsInLooterList(pDstPlayer->m_dwID);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    for (int i = 0; i < m_nLooterCount; ++i)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);

        if (pPlayer)
        {
            g_PlayerServer.DoDistributeItem(pPlayer->m_nConnIndex, pDstPlayer->m_dwID, dwItemID);
        }
    }

    pItemToAdd = m_ItemList[nItemIndex].pItem;
    assert(pItemToAdd);

    nBindType = pItemToAdd->m_Common.nBindType;

    eRetCode = pDstPlayer->m_ItemList.CanAddItem(pItemToAdd);
    if (eRetCode != aircSuccess)
    {
        g_PlayerServer.DoMessageNotify(pDstPlayer->m_nConnIndex, ectAddItemErrorCode, (int)eRetCode);
        
        m_ItemList[nItemIndex].dwOwnerIDSet = 0;
        SetItemLooter(nItemIndex, pDstPlayer->m_dwID);
        m_ItemList[nItemIndex].eLootItemType = litOwnerLoot;

        for (int i = 0; i < m_nLooterCount; ++i)
        {
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);

            if (pPlayer)
            {
                g_PlayerServer.DoSyncLootList(pPlayer, pDoodad);
            }
        }

        goto Exit1;
    }
    
    m_ItemList[nItemIndex].eLootItemType = litInvalid;
    m_ItemList[nItemIndex].pItem = NULL;
	++m_nLootedCount;
    
    g_PlayerServer.DoSyncLootList(pDstPlayer, pDoodad);
    
    nItemAmount = pItemToAdd->GetStackNum();
    
    if (pItemToAdd->m_GenParam.nQuality >= g_pSO3World->m_Settings.m_ConstList.nLootItemQualityToLog)
    {
        g_LogClient.LogPlayerItemChange(pDstPlayer, ITEM_OPERATION_TYPE_IN, pItemToAdd, "get item by distribute");
    }
    
	eRetCode = pDstPlayer->m_ItemList.AddItem(pItemToAdd);
	assert(eRetCode == aircSuccess);
        
	for (int i = 0; i < m_nLooterCount; ++i)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);

        if (pPlayer && pPlayer->m_dwID != pDstPlayer->m_dwID)
        {
            g_PlayerServer.DoAddItemNotify(pDstPlayer, dwItemID, nItemAmount, pPlayer->m_nConnIndex);
            g_PlayerServer.DoSyncLootList(pPlayer, pDoodad);
        }
    }
    
Exit1:
    if (nBindType == ibtBindOnTimeLimitation)
    {
        ProcessBindOnTimeLimitationItem(pDoodad, dwItemID, pDstPlayer->m_dwID, litNeedDistribute);
    }

	nResult = lircSuccess;
Exit0:
	return nResult;
}

void KLootList::GetLooterList(std::vector<DWORD>& vecLooterList)
{
	vecLooterList.clear();

	for (int i = 0; i < m_nLooterCount; ++i)
	{
		KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);
		if (pMember == NULL)
			continue;

		vecLooterList.push_back(m_LooterList[i]);
	}
    
	return;
}

int KLootList::GetLooterPos(DWORD dwPlayerID)
{
    for (int i = 0; i < m_nLooterCount; ++i)
    {
        if (m_LooterList[i] == dwPlayerID)
        {
            return i;
        }
    }

	return -1;
}

void KLootList::ProcessBindOnTimeLimitationItem(
    KDoodad* pDoodad, DWORD dwItemID, DWORD dwLooterID, LOOT_ITEM_TYPE eType
)
{
    BOOL bRetCode = false;
    
    assert(pDoodad);
    
    KG_PROCESS_ERROR(pDoodad->m_pScene);

    switch (eType)
    {
    case litLooterFree:
        KG_PROCESS_ERROR(g_pSO3World->m_Settings.m_ConstList.bFreeForAll);
    	break;
    case litNeedRoll:
        KG_PROCESS_ERROR(g_pSO3World->m_Settings.m_ConstList.bGroupLoot);
    	break;
    case litNeedDistribute:
        KG_PROCESS_ERROR(g_pSO3World->m_Settings.m_ConstList.bDistribute);
        break;
    default:
        goto Exit0;
    }

    bRetCode = pDoodad->m_pScene->m_TimeLimitationBindItemMgr.AddItem(dwItemID, dwLooterID);
    KGLOG_PROCESS_ERROR(bRetCode);

    for (int i = 0; i < m_nLooterCount; ++i)
    {
        pDoodad->m_pScene->m_TimeLimitationBindItemMgr.AddPlayer(dwItemID, m_LooterList[i]);
    }

Exit0:
    return;
}
#endif

KLootItem* KLootList::AddItem(KItem* pItem, DWORD dwQuestID, DWORD dwQuestState)
{
    KLootItem* pResult = NULL;

	assert(pItem);
	KG_PROCESS_ERROR(m_nItemCount >= 0 && m_nItemCount < MAX_LOOT_SIZE);

	m_ItemList[m_nItemCount].pItem          = pItem;
#ifdef _SERVER
	m_ItemList[m_nItemCount].dwQuestID      = dwQuestID;
	m_ItemList[m_nItemCount].dwQuestState   = dwQuestState;

    g_pSO3World->m_StatDataServer.UpdateItemDrop(pItem->m_GenParam.dwTabType, pItem->m_GenParam.dwIndex);
#endif

    pResult = &m_ItemList[m_nItemCount++];
Exit0:
	return pResult;
}

BOOL KLootList::AddMoney(int nMoney)
{
	BOOL    bResult = false;

	KGLOG_PROCESS_ERROR(nMoney >= 0);

	m_nMoney += nMoney;

	bResult = true;
Exit0:
	return bResult;
}

BOOL KLootList::SetMoney(int nMoney)
{
	BOOL bResult = false;

	KGLOG_PROCESS_ERROR(nMoney >= 0);

	m_nMoney = nMoney;

	bResult = true;
Exit0:
	return bResult;
}

KLootItem* KLootList::GetLootItem(DWORD dwLootIndex)
{
    KLootItem* pResult = NULL;

	KGLOG_PROCESS_ERROR(dwLootIndex < MAX_LOOT_SIZE);

	pResult = &(m_ItemList[dwLootIndex]);
Exit0:
	return pResult;
}

#ifdef _CLIENT
BOOL KLootList::IsVisible(KPlayer* pPlayer, KDoodad* pDoodad)
{
	BOOL    bResult     = false;

    assert(pPlayer);
    assert(pDoodad);

	KG_PROCESS_ERROR(!pDoodad->m_bLooted);

    KG_PROCESS_SUCCESS(m_nMoney > 0); // 有钱要返回可见
    
    KG_PROCESS_ERROR(m_nItemCount > 0); // 没有东西就不可见

    for (int i = 0; i < m_nItemCount; ++i)
    {
	    if (m_ItemList[i].bDisplay)
        {
            goto Exit1; // 有东西要显示返回可见
        }
    }
    
    goto Exit0; // 没有能显示的东西返回不可见

Exit1:
	bResult = true;
Exit0:
	return bResult;
}
#endif

BOOL KLootList::IsInLooterList(DWORD dwPlayerID)
{
	BOOL bResult = false;
    
    for (int i = 0; i < m_nLooterCount; ++i)
    {
        if (m_LooterList[i] == dwPlayerID)
        {
            goto Exit1;
        }
    }

    goto Exit0;

Exit1:
	bResult = true;
Exit0:
	return bResult;
}

#ifdef _SERVER
void KLootList::ProcessRollerOffline(KDoodad* pDoodad)
{
    BOOL bRetCode = false;

    assert(pDoodad);
    KG_PROCESS_ERROR(m_bIsOpened);
    KG_PROCESS_ERROR(m_nEndRollFrame != 0); // 有物品在roll
    KG_PROCESS_ERROR(g_pSO3World->m_nGameLoop < m_nEndRollFrame);

    for (int nLooterIndex = 0; nLooterIndex < m_nLooterCount; ++nLooterIndex)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[nLooterIndex]);
		
        if (pPlayer == NULL)
        {
            m_OfflineFlag[nLooterIndex] = true;
            continue;
        }

        if (!m_OfflineFlag[nLooterIndex])
            continue;

        if (pPlayer->m_eGameStatus != gsPlaying)
            continue;            
        
        m_OfflineFlag[nLooterIndex] = false;

        g_PlayerServer.DoSyncNewDoodad(pPlayer->m_nConnIndex, pDoodad); // 玩家刚登陆,需要同步doodad和lootlist
        g_PlayerServer.DoSyncLootList(pPlayer, pDoodad);
        
        for (int nItemIndex = 0; nItemIndex < m_nItemCount; ++nItemIndex)
        {
            if (m_ItemList[nItemIndex].pItem == NULL)
            {
                continue;
            }

            if (m_ItemList[nItemIndex].eLootItemType != litNeedRoll)
            {
                continue;
            }

            bRetCode = CanRoll(nItemIndex, pPlayer);
            if (!bRetCode)
            {
                continue;
            }
            
            g_PlayerServer.DoBeginRollItem(
                pPlayer->m_nConnIndex, m_ItemList[nItemIndex].pItem, pDoodad, (m_nEndRollFrame - g_pSO3World->m_nGameLoop)
            );
        }
    }

Exit0:
    return;
}

void KLootList::ProcessOverRollTime(KDoodad* pDoodad)
{
    ADD_ITEM_RESULT_CODE eRetCode = aircFailed;
    KItem*  pItemToAdd      = NULL;
    int     nItemAmount     = 0;
    DWORD   dwItemID        = ERROR_ID;

    assert(pDoodad);
    
    KG_PROCESS_ERROR(m_bIsOpened);

    KG_PROCESS_ERROR(m_nEndRollFrame != 0 && g_pSO3World->m_nGameLoop > m_nEndRollFrame);

    m_nEndRollFrame = 0;

    for (int i = 0; i < m_nItemCount; ++i)
    {
        DWORD    dwWinnerID = ERROR_ID;
	    KPlayer* pWinner    = NULL;
        
        if (m_ItemList[i].pItem == NULL)
        {
            continue;
        }

        if (m_ItemList[i].eLootItemType != litNeedRoll)
        {
            continue;
        }
           
        dwWinnerID = GetItemLooter(i);
        if (dwWinnerID == ERROR_ID) // 没有拥有者
        {   
            m_ItemList[i].dwOwnerIDSet = 0;
            m_ItemList[i].byRollCount = 0;
            m_ItemList[i].eLootItemType = litLooterFree;
            m_bLootListChanged = true;
	        continue;
        }

        pWinner = g_pSO3World->m_PlayerSet.GetObj(dwWinnerID);
        if (pWinner == NULL || pWinner->m_eGameStatus != gsPlaying) // 掉线了，再上线还可以捡
        {
            m_ItemList[i].byRollCount = 0;
            m_ItemList[i].eLootItemType = litOverTimeLooterFree; // 此时仅胜利者可以拾取
            m_bLootListChanged = true;
	        continue;
        }
        
        pItemToAdd = m_ItemList[i].pItem;

        eRetCode = pWinner->m_ItemList.CanAddItem(pItemToAdd);
        if (eRetCode != aircSuccess)
        {
            m_ItemList[i].byRollCount = 0;
            m_ItemList[i].eLootItemType = litOverTimeLooterFree; // 此时仅胜利者可以拾取
            m_bLootListChanged = true;
	        continue;
        }
        
        m_ItemList[i].pItem = NULL;
        ++m_nLootedCount;
        
        g_PlayerServer.DoSyncLootList(pWinner, pDoodad); // 删掉客户端这个物品

        if (pItemToAdd->m_Common.nBindType == ibtBindOnTimeLimitation)
        {
            ProcessBindOnTimeLimitationItem(pDoodad, pItemToAdd->m_dwID, dwWinnerID, litNeedRoll);
        }
        
        nItemAmount = pItemToAdd->GetStackNum();
        dwItemID    = pItemToAdd->m_dwID;

        eRetCode = pWinner->m_ItemList.AddItem(pItemToAdd);
        assert(eRetCode == aircSuccess);
        
        for (int j = 0; j < m_nLooterCount; ++j)
        {
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[j]);

            if (pPlayer && pPlayer->m_dwID != pWinner->m_dwID)
            {
                g_PlayerServer.DoAddItemNotify(pWinner, dwItemID, nItemAmount, pPlayer->m_nConnIndex);
            }
        }

        m_bLootListChanged = true;
    }

Exit0:
    return;
}

void KLootList::ProcessOverLootTime(KDoodad* pDoodad)
{
    assert(pDoodad);

    KG_PROCESS_ERROR (m_nOverLootFrame > 0 && g_pSO3World->m_nGameLoop > m_nOverLootFrame);
    m_nOverLootFrame = 0;

    for (int i = 0; i < m_nItemCount; ++i)
    {
        if (m_ItemList[i].pItem == NULL)
        {
            continue;
        }

	    if (m_ItemList[i].eLootItemType == litOverTimeLooterFree || m_ItemList[i].eLootItemType == litNeedDistribute)
        {
            m_ItemList[i].dwOwnerIDSet = 0;
            m_ItemList[i].eLootItemType = litLooterFree;
            m_bLootListChanged = true;
            continue;
        }
    }

Exit0:
    return;
}

BOOL KLootList::EndRollItem(int nItemIndex, KDoodad* pDoodad)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    ADD_ITEM_RESULT_CODE    eRetCode    = aircFailed;
    DWORD                   dwWinnerID  = ERROR_ID;
    KPlayer*                pWinner     = NULL;
    int                     nItemAmount = 0;
    KItem*                  pItemToAdd  = NULL;
    DWORD                   dwItemID    = 0;
    BOOL                    bOverTimeLooterFree = true;
    int                     nBindType   = ibTotal;
    
    assert(pDoodad);

    pItemToAdd = m_ItemList[nItemIndex].pItem;
    assert(pItemToAdd);

    nBindType   = pItemToAdd->m_Common.nBindType;
    dwItemID    = pItemToAdd->m_dwID;

    dwWinnerID = GetItemLooter(nItemIndex);
    
    pWinner = g_pSO3World->m_PlayerSet.GetObj(dwWinnerID);
    
    if (pWinner && pWinner->m_eGameStatus == gsPlaying)
    {
        bOverTimeLooterFree = false;
        eRetCode = pWinner->m_ItemList.CanAddItem(pItemToAdd);
        if (eRetCode != aircSuccess)
        {
            bOverTimeLooterFree = true;
            g_PlayerServer.DoMessageNotify(pWinner->m_nConnIndex, ectAddItemErrorCode, eRetCode);
        }
    }

    if (bOverTimeLooterFree)
    {
        if (nBindType == ibtBindOnTimeLimitation)
        {
            ProcessBindOnTimeLimitationItem(pDoodad, dwItemID, dwWinnerID, litNeedRoll);
        }

        m_ItemList[nItemIndex].eLootItemType = litOverTimeLooterFree;

        for (int i = 0; i < m_nLooterCount; ++i)
        {
	        KPlayer* pLooter = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);
	        if (pLooter)
	        {
		        g_PlayerServer.DoSyncLootList(pLooter, pDoodad);
	        }
        }

        goto Exit1;
    }
    
    m_ItemList[nItemIndex].eLootItemType = litInvalid;
    m_ItemList[nItemIndex].pItem = NULL;
	++m_nLootedCount;
    
    g_PlayerServer.DoSyncLootList(pWinner, pDoodad);
    
    nItemAmount = pItemToAdd->GetStackNum();

    if (pItemToAdd->m_GenParam.nQuality >= g_pSO3World->m_Settings.m_ConstList.nLootItemQualityToLog)
    {
        g_LogClient.LogPlayerItemChange(pWinner, ITEM_OPERATION_TYPE_IN, pItemToAdd, "get item by win roll");
    }

    eRetCode = pWinner->m_ItemList.AddItem(pItemToAdd);
    assert(eRetCode == aircSuccess);

    for (int i = 0; i < m_nLooterCount; ++i)
    {
        KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);

        if (pPlayer && m_LooterList[i] != pWinner->m_dwID)
        {
            g_PlayerServer.DoAddItemNotify(pWinner, dwItemID, nItemAmount, pPlayer->m_nConnIndex);
            g_PlayerServer.DoSyncLootList(pPlayer, pDoodad);
        }
    }
    
    if (nBindType == ibtBindOnTimeLimitation)
    {
        ProcessBindOnTimeLimitationItem(pDoodad, dwItemID, dwWinnerID, litNeedRoll);
    }

Exit1:
    bResult = true;
Exit0:
	return bResult;
}

BOOL KLootList::CanLootAbsoluteFreeQuestItem(int nItemIndex, KPlayer* pPlayer)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    KQuestInfo* pQuestInfo  = NULL;
    KItem*      pDropItem   = NULL;
    int         nQuestPhase = 0;

    assert(nItemIndex >= 0 && nItemIndex < MAX_LOOT_SIZE);
	assert(pPlayer);

    KGLOG_PROCESS_ERROR(m_ItemList[nItemIndex].eLootItemType == litAbsoluteFree);

    if (m_ItemList[nItemIndex].dwQuestID == 0) 
    {
        goto Exit1;
    }

    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(m_ItemList[nItemIndex].dwQuestID);
    KGLOG_PROCESS_ERROR(pQuestInfo);
    
    pDropItem = m_ItemList[nItemIndex].pItem;
    KGLOG_PROCESS_ERROR(pDropItem);
    
    nQuestPhase = pPlayer->m_QuestList.GetQuestPhase(m_ItemList[nItemIndex].dwQuestID);
    KG_PROCESS_ERROR(nQuestPhase == (int)m_ItemList[nItemIndex].dwQuestState);
    
    if (nQuestPhase == 1) // 要检查任务物品是否足够
    {
        for (int nIndex = 0; nIndex < QUEST_END_ITEM_COUNT; ++nIndex)
        {
            DWORD           dwTabType     = pQuestInfo->byEndRequireItemType[nIndex];
            DWORD           dwItemIndex   = pQuestInfo->wEndRequireItemIndex[nIndex];
            int             nItemCount    = (int)pQuestInfo->wEndRequireItemAmount[nIndex];
            
            if (dwTabType != pDropItem->m_GenParam.dwTabType || dwItemIndex != pDropItem->m_GenParam.dwIndex)
            {
                continue;
            }
            
            if (pDropItem->m_Common.nGenre == igBook && nItemCount != pDropItem->m_nCurrentDurability)
            {
                continue;
            }

            bRetCode = pPlayer->m_QuestList.IsQuestEndItemEnoughByIndex(pQuestInfo, nIndex, true);
            if (!bRetCode) 
            {
                goto Exit1; // 该任务道具不够
            }
        }
        
        goto Exit0; // 该任务道具已经足够
    }

Exit1:
    bResult = true;
Exit0:
	return bResult;
}
#endif

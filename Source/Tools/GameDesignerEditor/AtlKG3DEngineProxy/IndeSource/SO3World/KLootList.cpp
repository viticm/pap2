#include <vector>
#include "stdafx.h"
#include "KLootList.h"
#include "KPlayer.h"
#include "KThreatList.h"
#include "KPlayerServer.h"
#include "KMath.h"
#include "KSO3World.h"
#include "KDoodad.h"

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
    BOOL bLootListChanged   = false;

    assert(pDoodad);
    
    ProcessRollerOffline(pDoodad);
    ProcessOverRollTime(pDoodad, bLootListChanged);
    ProcessOverLootTime(pDoodad, bLootListChanged);    

    KG_PROCESS_ERROR(bLootListChanged);

    for (int i = 0; i < m_nLooterCount; ++i)
    {
        KPlayer* pLooter = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);
		
        if (pLooter == NULL)
        {
            continue;
        }

        bRetCode = IsInLootRange(pLooter, pDoodad);
	    if (!bRetCode)
        {
		    continue;
        }

        g_PlayerServer.DoSyncLootList(pLooter, pDoodad);
    }

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
        goto Exit1;
    }
    
    if (
        m_ItemList[nItemIndex].eLootItemType == litNeedRoll ||
        m_ItemList[nItemIndex].eLootItemType == litNeedDistribute
    )
    {
        bRetCode = IsInLooterList(pPlayer->m_dwID);
        if (bRetCode)
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

    KG_PROCESS_ERROR(!m_bIsOpened);

    for (int i = 0; i < m_nItemCount; ++i)
	{
        if (m_ItemList[i].eLootItemType != litNeedRoll)
        {
            continue;
        }

		m_ItemList[i].byRollCount = 0;
        
        for (int j = 0; j < m_nLooterCount; ++j)
        {
            KPlayer* pRoller = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[j]);
			if (pRoller == NULL)
				continue;

			if (IsInLootRange(pRoller, pDoodad))
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
    KTeam*  pTeam     = NULL;

	assert(pAttacker);
    assert(pDoodad);

    m_nLooterCount = 0;

    if (pAttacker->m_dwTeamID == ERROR_ID)
    {
        m_LooterList[m_nLooterCount++] = pAttacker->m_dwID;
        goto Exit1;
    }

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pAttacker->m_dwTeamID);
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

            bRetCode = IsInLootRange(pMember, pDoodad);
            if (!bRetCode)
            {
                continue;
            }

            m_LooterList[m_nLooterCount++] = pMember->m_dwID;
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
    KTeam*      pTeam           = NULL;
    DWORD       dwMaxRollCount  = 0;
    size_t      uLooterCount    = 0;

	pLooter = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[0]);
	KG_PROCESS_ERROR(pLooter);
    
    if (pLooter->m_dwTeamID == ERROR_ID)
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
    
    pTeam = g_pSO3World->m_TeamServer.GetTeam(pLooter->m_dwTeamID);
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

    assert(pLooter);
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

	bRetCode = CanLoot(nItemIndex, pLooter);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    pItemToLoot = m_ItemList[nItemIndex].pItem;

    eRetCode = pLooter->m_ItemList.CanAddItem(pItemToLoot);
    KG_PROCESS_ERROR_RET_CODE(eRetCode != aircItemAmountLimited, lircOverItemLimit);
    KG_PROCESS_ERROR_RET_CODE(eRetCode != aircNotEnoughFreeRoom, lircInventoryIsFull);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, lircAddLootItemFailed);
    
    m_ItemList[nItemIndex].pItem = NULL;
	++m_nLootedCount;
    
    g_PlayerServer.DoSyncLootList(pLooter, pDoodad); // 添加item前通知客户端删掉即将要添加的物品

    nItemAmount = pItemToLoot->GetStackNum();

	eRetCode = pLooter->m_ItemList.AddItem(pItemToLoot);
	KGLOG_PROCESS_ERROR(eRetCode == aircSuccess);

    if (m_ItemList[nItemIndex].dwQuestID != 0)
    {
        pLooter->m_QuestList.RegisterPartyAssistance(m_ItemList[nItemIndex].dwQuestID);
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
    int                     nAddMoneyBase       = 0;
	int                     nMoney              = 0; 
    int                     nLooterCount        = 0;
   	std::vector<DWORD>      vecLooterList;

    assert(pLooter);
    assert(pDoodad);

	KG_PROCESS_SUCCESS(pDoodad->m_pLootList == NULL); // 已经被人捡光了

	nMoney = pDoodad->m_pLootList->GetMoney();
	KG_PROCESS_SUCCESS(nMoney <= 0); // 钱刚好已经被其他玩家拾取完

    bRetCode = pDoodad->m_pLootList->SetMoney(0);
	KGLOG_PROCESS_ERROR(bRetCode);

	if (pLooter->m_dwTeamID == ERROR_ID)
    {
		bRetCode = pLooter->m_ItemList.AddMoney(nMoney);
		KGLOG_PROCESS_ERROR(bRetCode);
        
        g_PlayerServer.DoSyncLootList(pLooter, pDoodad);
    }
    else
    {
        // in party
	    bRetCode = GetNearByLooterList(pDoodad, vecLooterList);
        KGLOG_PROCESS_ERROR(bRetCode);

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
    DWORD                   dwLooter            = ERROR_ID;
    KTeam*                  pTeam               = NULL;
    DWORD                   dwDistributeManID   = ERROR_ID;

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

    KGLOG_PROCESS_ERROR(pDistributer->m_dwTeamID != ERROR_ID);

    pTeam = g_pSO3World->m_TeamServer.GetTeam(pDistributer->m_dwTeamID);
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

    m_ItemList[nItemIndex].pItem = NULL;
	++m_nLootedCount;
    
    g_PlayerServer.DoSyncLootList(pDstPlayer, pDoodad);
    
    nItemAmount = pItemToAdd->GetStackNum();

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
	nResult = lircSuccess;
Exit0:
	return nResult;
}

BOOL KLootList::GetNearByLooterList(KDoodad* pDoodad, std::vector<DWORD>& vecLooterList)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;

	vecLooterList.clear();

	for (int i = 0; i < m_nLooterCount; ++i)
	{
		KPlayer* pMember = g_pSO3World->m_PlayerSet.GetObj(m_LooterList[i]);
		if (pMember == NULL)
			continue;

		bRetCode = IsInLootRange(pMember, pDoodad);
		if (!bRetCode)
			continue;

		vecLooterList.push_back(m_LooterList[i]);
	}
	
	bResult = true;
Exit0:
	return bResult;
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

BOOL KLootList::IsInLootRange(KPlayer* pPlayer, KDoodad* pDoodad)
{
    BOOL bResult    = false;
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

void KLootList::ProcessOverRollTime(KDoodad* pDoodad, BOOL& rbLootListChanged)
{
    ADD_ITEM_RESULT_CODE eRetCode = aircFailed;
    KItem*  pItemToAdd  = NULL;
    
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
            rbLootListChanged = true;
	        continue;
        }

        pWinner = g_pSO3World->m_PlayerSet.GetObj(dwWinnerID);
        if (pWinner == NULL) // 掉线了，再上线还可以捡
        {
            m_ItemList[i].byRollCount = 0;
            m_ItemList[i].eLootItemType = litOverTimeLooterFree; // 此时仅胜利者可以拾取
            rbLootListChanged = true;
	        continue;
        }
        
        pItemToAdd = m_ItemList[i].pItem;

        eRetCode = pWinner->m_ItemList.CanAddItem(pItemToAdd);
        if (eRetCode != aircSuccess)
        {
            m_ItemList[i].byRollCount = 0;
            m_ItemList[i].eLootItemType = litOverTimeLooterFree; // 此时仅胜利者可以拾取
            rbLootListChanged = true;
	        continue;
        }
        
        m_ItemList[i].pItem = NULL;
        ++m_nLootedCount;
        
        g_PlayerServer.DoSyncLootList(pWinner, pDoodad); // 删掉客户端这个物品

        eRetCode = pWinner->m_ItemList.AddItem(pItemToAdd);
        assert(eRetCode == aircSuccess);

        rbLootListChanged = true;
    }

Exit0:
    return;
}

void KLootList::ProcessOverLootTime(KDoodad* pDoodad, BOOL& rbLootListChanged)
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
            rbLootListChanged = true;
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
    
    assert(pDoodad);

    pItemToAdd = m_ItemList[nItemIndex].pItem;
    assert(pItemToAdd);
    
    dwWinnerID = GetItemLooter(nItemIndex);
    
    pWinner = g_pSO3World->m_PlayerSet.GetObj(dwWinnerID);
    
    if (pWinner)
    {
        eRetCode = pWinner->m_ItemList.CanAddItem(pItemToAdd);

        if (eRetCode != aircSuccess)
        {
            g_PlayerServer.DoMessageNotify(pWinner->m_nConnIndex, ectAddItemErrorCode, eRetCode);
            
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
    }
    else
    {
        m_ItemList[nItemIndex].dwOwnerIDSet = 0;
        m_ItemList[nItemIndex].eLootItemType = litLooterFree;
        
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

    m_ItemList[nItemIndex].pItem = NULL;
	++m_nLootedCount;
    
    g_PlayerServer.DoSyncLootList(pWinner, pDoodad);
    
    nItemAmount = pItemToAdd->GetStackNum();
    dwItemID    = pItemToAdd->m_dwID;

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
            DWORD           dwTabType     = pQuestInfo->dwEndRequireItemType[nIndex];
            DWORD           dwItemIndex   = pQuestInfo->dwEndRequireItemIndex[nIndex];
            int             nItemCount    = (int)pQuestInfo->dwEndRequireItemAmount[nIndex];
            
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

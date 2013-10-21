#include "StdAfx.h"
#include "KLootList.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KItemLib.h"
#include "KObjectManager.h"
#include "KThreatList.h"

KLootList::KLootList(void)
{
	memset(m_LootList, 0, sizeof(m_LootList));
}

KLootList::~KLootList(void)
{
	UnInit();
}

BOOL KLootList::Init(void)
{
	memset(m_LootList, 0, sizeof(m_LootList));
	memset(m_pLooterList, 0, sizeof(m_pLooterList));
	//m_nGameLoop = g_SO3World.m_nGameLoop;
	m_nItemCount = 0;
	m_nLootedCount = 0;
	m_nMoney = 0;

	m_pDoodad = NULL;
	m_nLooterCount = 0;

	m_nLootMode = plmFreeForAll;
	m_nRollQuality = 1; 

	m_bIsOpened = FALSE;

	m_bOverLootRollTime = FALSE;
	m_bOverFreeLootTime = FALSE;

	return TRUE;
}

BOOL KLootList::UnInit(void)
{
	BOOL bRetCode = FALSE;

	for (int nIndex = 0; nIndex < MAX_LOOT_SIZE; nIndex++)
	{
		if (m_LootList[nIndex].pItem)
		{
			bRetCode = g_ItemLib.FreeItem(m_LootList[nIndex].pItem);
			KGLOG_CHECK_ERROR(bRetCode);

			m_LootList[nIndex].pItem = NULL;
		}
	}

	return TRUE;
}

//往拾取表中加入物品，返回序号，从1开始计数
int KLootList::AddItem(KItem* pItem)
{
	KGLOG_PROCESS_ERROR(pItem);
	KGLOG_PROCESS_ERROR(m_nItemCount >= 0 && m_nItemCount < MAX_LOOT_SIZE);

	memset(m_LootList[m_nItemCount].dwFlag, 0, sizeof(m_LootList[m_nItemCount].dwFlag));

	m_LootList[m_nItemCount].pItem = pItem;
	m_LootList[m_nItemCount].pWinner = NULL;
	m_LootList[m_nItemCount].nRollCount = 0;

	m_nItemCount++;

	return m_nItemCount;

Exit0:

	return 0;
}

BOOL KLootList::RemoveItem(KItem* pItem)
{
	BOOL bRetCode = FALSE;
	int nItemIndex = 0;

	for (nItemIndex = 0; nItemIndex < m_nItemCount; nItemIndex++)
	{
		if (m_LootList[nItemIndex].pItem && m_LootList[nItemIndex].pItem->m_dwID == pItem->m_dwID)
		{
			m_LootList[nItemIndex].pItem = NULL;
			m_nLootedCount++;
			break;
		}
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

#if defined(_SERVER)
BOOL KLootList::PreProcess(KPlayer *pAttacker, KNpc *pNpc)
{
	//int nLooterCount = 0;

	KGLOG_PROCESS_ERROR(pAttacker);
	KGLOG_PROCESS_ERROR(pNpc);

	if (pAttacker->m_dwPartyID != INVALID_PARTY_ID)
	{
		//仇恨列表有问题，所以暂时把全队加入拾取列表，测试代码。
		/*
		m_pLooterList[m_nLooterCount++] = pAttacker;

		const KPARTY_INFO* pcPartyInfo = g_PartyCache.GetParty(pAttacker->m_dwPartyID);
		KGLOG_PROCESS_ERROR(pcPartyInfo);

		KThreatNode *pNode = pNpc->m_ThreatList.GetFirstThreat();

		while (pNode)
		{
			if (g_IsPlayer(pNode->pCharacter->m_dwID))
			{
				KPlayer *pPlayer = (KPlayer*)pNode->pCharacter;

				if (pPlayer == pAttacker)
				{
					pNode = pNpc->m_ThreatList.GetNextNode(pNode);
					continue;
				}

				for (it = pcPartyInfo->PlayerTable.begin();
					it != pcPartyInfo->PlayerTable.end(); ++it)
				{
					KPlayer* pMember = g_ObjectManager.m_PlayerSet.GetObj(it->first);
					if (pPlayer == pMember)
						break;
				}

				if (it != pcPartyInfo->PlayerTable.end())
				{
					m_pLooterList[m_nLooterCount++] = pPlayer;
				}
			}

			pNode = pNpc->m_ThreatList.GetNextNode(pNode);
		}
		*/
		const KPARTY_INFO* pcPartyInfo = g_PartyCache.GetParty(pAttacker->m_dwPartyID);
		KGLOG_PROCESS_ERROR(pcPartyInfo);

		for (DWORD dwIndex = 0; dwIndex < pcPartyInfo->PlayerList.size(); ++dwIndex)
		{
			KPlayer* pMember = g_ObjectManager.m_PlayerSet.GetObj(pcPartyInfo->PlayerList[dwIndex].dwMemberID);
			if (!pMember)
				continue;

			m_pLooterList[m_nLooterCount++] = pMember;
		}
	}
	else
	{
		m_pLooterList[0] = pAttacker;
	}

	return TRUE;
Exit0:
	return FALSE;
}
#endif //_SERVER

//获取表中物品总数
int KLootList::GetItemCount(void)
{
	return m_nItemCount;
}

KLootItem* KLootList::GetLootItem(DWORD dwLootIndex)
{
	KGLOG_PROCESS_ERROR(dwLootIndex < MAX_LOOT_SIZE);

	return &(m_LootList[dwLootIndex]);

Exit0:

	return NULL;
}

BOOL KLootList::CanLoot(DWORD dwLootIndex, KPlayer* pPlayer)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(dwLootIndex < MAX_LOOT_SIZE);
	KGLOG_PROCESS_ERROR(pPlayer);

	//TODO: Doodad是否加锁的判断

	if (m_nLootMode != plmAbsoluteFree)
	{
		bRetCode = IsInLooterList(pPlayer);
		KG_PROCESS_ERROR(bRetCode);
	}

	bRetCode = FALSE;

	//除了自由模式外,要自由winner有直接拾取的权利
	switch(m_nLootMode)
	{
	case plmAbsoluteFree:
	case plmFreeForAll:
		if (m_LootList[dwLootIndex].pWinner == NULL )
			bRetCode = TRUE;
		else if (m_LootList[dwLootIndex].pWinner == pPlayer)
			bRetCode = TRUE;
		break;
	case plmRoundRobin:
	case plmLeaderLoot:
		if (m_LootList[dwLootIndex].pWinner == pPlayer ||
			m_LootList[dwLootIndex].pWinner == NULL)
		{
			bRetCode = TRUE;
		}
	    break;
	case plmGroupLoot:
		if (m_LootList[dwLootIndex].pWinner == NULL && m_LootList[dwLootIndex].nRollCount == 0)
			bRetCode = TRUE;
		else if (m_LootList[dwLootIndex].pWinner == pPlayer && m_LootList[dwLootIndex].nRollCount == 0)
			bRetCode = TRUE;
		break;
	default:
		KGLOG_PROCESS_ERROR(FALSE && "[ERROR]: LootMode Error In KLootList::CanLoot() !");
	}

Exit0:
	return bRetCode;
}

BOOL KLootList::CanRoll(DWORD dwRollIndex, KPlayer* pPlayer, BOOL bCheckRollFlag)
{
	BOOL bRetCode = FALSE;
	int nIndex = 0;
	int nRandom = -1;

	KGLOG_PROCESS_ERROR(dwRollIndex < MAX_LOOT_SIZE);
	KGLOG_PROCESS_ERROR(pPlayer);
	
	KG_PROCESS_ERROR(m_nLootMode == plmGroupLoot);
	KG_PROCESS_ERROR(m_LootList[dwRollIndex].pItem);
	KG_PROCESS_ERROR(m_LootList[dwRollIndex].nRollCount > 0);
	
	//检查是否在Looter队列中，且有权限投骰子
	for (nIndex = 0; nIndex < m_nLooterCount; nIndex++)
	{
		if (bCheckRollFlag)
		{
			if (m_pLooterList[nIndex] == pPlayer && 0 == m_LootList[dwRollIndex].dwFlag[nIndex])
				break;
		}
		else
		{
			if (m_pLooterList[nIndex] == pPlayer)
				break;
		}
	}
	KG_PROCESS_ERROR(nIndex < m_nLooterCount);
	
	bRetCode =  TRUE;
Exit0:
	return bRetCode;
}

BOOL KLootList::IsVisible(KPlayer* pPlayer)
{
	BOOL bRetCode = FALSE;
	int nItemIndex = 0;
	KG_PROCESS_ERROR(m_pDoodad);
	KG_PROCESS_ERROR(m_pDoodad->m_bLooted == FALSE);

	for (nItemIndex = 0; nItemIndex < m_nItemCount; nItemIndex++)
	{
		if (CanLoot(nItemIndex, pPlayer) || CanRoll(nItemIndex, pPlayer, FALSE))
			break;
	}

	KG_PROCESS_ERROR((m_nItemCount != 0 && nItemIndex < m_nItemCount) || m_nMoney > 0);

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

//拾取表中的物品
#if defined(_SERVER)
SKILL_RESULT_CODE KLootList::LootOrRollItem(KPlayer* pLooter, DWORD dwDoodadID, DWORD dwItemID, BOOL bCancel)
{
	SKILL_RESULT_CODE nResult = srcFailed;
	BOOL bRetCode = FALSE;
	BOOL bResult = FALSE;
	int nRandom = -1;

	KGLOG_PROCESS_ERROR(pLooter);

	KDoodad *pDoodad = g_ObjectManager.m_DoodadSet.GetObj(dwDoodadID);
	KGLOG_PROCESS_ERROR(pDoodad);

	//在表中找到指定的物品
	int nItemIndex = 0;
	for (; nItemIndex < MAX_LOOT_SIZE; nItemIndex++)
	{
		if (m_LootList[nItemIndex].pItem == NULL)
			continue;
		if (m_LootList[nItemIndex].pItem->m_dwID == dwItemID)
			break;
	}
	KG_PROCESS_ERROR_RET_CODE(nItemIndex < MAX_LOOT_SIZE, srcNotExistLootItem);

	bResult = CanLoot(nItemIndex, pLooter);
	if (bResult)
	{
		//拾取
		bRetCode = pLooter->m_ItemList.AddItem(m_LootList[nItemIndex].pItem);
		KG_PROCESS_ERROR_RET_CODE(bRetCode, srcAddLootItemFailed);

		m_LootList[nItemIndex].pItem = NULL;
		m_nLootedCount++;

		bRetCode = g_PlayerServer.DoRollItemMessage(pLooter, dwDoodadID, dwItemID, TRUE, nRandom);
		KGLOG_CHECK_ERROR(bRetCode);

		return srcSuccess;
	}

	bResult = CanRoll(nItemIndex, pLooter, TRUE);
	if (bResult)
	{
		for (int nOwnerIndex = 0; nOwnerIndex < m_nLooterCount; nOwnerIndex++)
		{
			if (m_pLooterList[nOwnerIndex] != pLooter)
				continue;

			//投一个100面的骰子
			if (!bCancel)
			{
				nRandom = g_Random(ROLL_ITEM_NUMBER) + 1;
				if (nRandom > m_LootList[nItemIndex].nRandom)
				{
					//掷赢了，则更改胜利者
					m_LootList[nItemIndex].pWinner = m_pLooterList[nOwnerIndex];
					m_LootList[nItemIndex].nRandom = nRandom;
				}
				else if (nRandom == m_LootList[nItemIndex].nRandom)
				{
					//平局，自动再掷一次
					int nRoll = g_Random(2);
					if (nRoll == 0)
					{
						m_LootList[nItemIndex].pWinner = m_pLooterList[nOwnerIndex];
						m_LootList[nItemIndex].nRandom = nRandom;							
					}
				}
			}

			//取消掷骰子的权利
			m_LootList[nItemIndex].dwFlag[nOwnerIndex] = 1;
			m_LootList[nItemIndex].nRollCount--;

			//如果所有人都掷过了，那么自动放到胜利者的背包中
			KPlayer* pWinner = m_LootList[nItemIndex].pWinner;

			
			//同步LootList
			for (int nOwnIndex = 0; nOwnIndex < m_nLooterCount; nOwnIndex++)
			{
				bRetCode = g_PlayerServer.DoSyncLootList(m_pLooterList[nOwnIndex], m_pDoodad);
				KGLOG_CHECK_ERROR(bRetCode);
			}			

			if (m_LootList[nItemIndex].nRollCount == 0 && pWinner)
			{
				bRetCode = pWinner->m_ItemList.AddItem(m_LootList[nItemIndex].pItem);
				KG_PROCESS_ERROR_RET_CODE(bRetCode, srcAddLootItemFailed);

				m_LootList[nItemIndex].pItem = NULL;
				m_nLootedCount++;

				bRetCode = g_PlayerServer.DoRollItemMessage(pWinner, dwDoodadID, dwItemID, 
					TRUE, nRandom);
				KGLOG_CHECK_ERROR(bRetCode);
			}
			else
			{
				bRetCode = g_PlayerServer.DoRollItemMessage(pLooter, dwDoodadID, dwItemID, 
					FALSE, nRandom);
				KGLOG_CHECK_ERROR(bRetCode);
			}
		}
		return srcSuccess;
	}
Exit0:
	//这里同步是因为有可能有玩家部分拾取了某个可叠加的物品
	bRetCode = g_PlayerServer.DoRollItemMessage(pLooter, dwDoodadID, dwItemID, FALSE, -2);
	KGLOG_CHECK_ERROR(bRetCode);
		
	return nResult;
}
#endif	//_SERVER

#if defined(_SERVER)
SKILL_RESULT_CODE KLootList::LootMoney(KPlayer *pLooter, KDoodad *pDoodad)
{
	SKILL_RESULT_CODE nResult = srcFailed;
	BOOL bRetCode = FALSE;
	int nPartyMemberCount = 1;
	int nMoney = 0;

	KGLOG_PROCESS_ERROR(pLooter);
	KGLOG_PROCESS_ERROR(pDoodad);

	KG_PROCESS_ERROR(pDoodad->m_pLootList);

	nMoney = pDoodad->m_pLootList->GetMoney();
	KGLOG_PROCESS_ERROR(nMoney);

	//缺少组队相关
	if (pLooter->m_dwPartyID != INVALID_PARTY_ID)
	{
		//组队分配
		const KPARTY_INFO* pcPartyInfo = g_PartyCache.GetParty(pLooter->m_dwPartyID);
		KGLOG_PROCESS_ERROR(pcPartyInfo);

		KGLOG_PROCESS_ERROR(pcPartyInfo->PlayerList.size());
		//nMoney /= (int)pcPartyInfo->PlayerList.size();

		for (DWORD dwIndex = 0; dwIndex < pcPartyInfo->PlayerList.size(); ++dwIndex)
		{
			KPlayer* pPlayer = g_ObjectManager.m_PlayerSet.GetObj(pcPartyInfo->PlayerList[dwIndex].dwMemberID);
			if (pPlayer)
			{
				int nAddMoney = nMoney / ((int)pcPartyInfo->PlayerList.size());
				if (pPlayer == pLooter)
					nAddMoney += nMoney % ((int)pcPartyInfo->PlayerList.size());

				bRetCode = pPlayer->m_ItemList.AddMoney(nAddMoney);
				KGLOG_PROCESS_ERROR(bRetCode);
			}
		}
	}
	else
	{
		bRetCode = pLooter->m_ItemList.AddMoney(nMoney);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	bRetCode = g_PlayerServer.DoRollMoneyMessage(pLooter, pDoodad->m_dwID);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pDoodad->m_pLootList->SetMoney(0);
	KGLOG_PROCESS_ERROR(bRetCode);

	nResult = srcSuccess;
	
Exit0:
	return nResult;
}
#endif //_SERVER

#if defined(_SERVER)
SKILL_RESULT_CODE KLootList::LeaderDistributeItem(KPlayer* pLeader, KPlayer* pDstPlayer, 
												  KDoodad* pDoodad, DWORD dwItemID)
{
	SKILL_RESULT_CODE nResult = srcFailed;
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pLeader);
	KGLOG_PROCESS_ERROR(pDstPlayer);
	KGLOG_PROCESS_ERROR(pDoodad);

	KG_PROCESS_ERROR(pDoodad->m_pLootList);
	KGLOG_PROCESS_ERROR(m_nLootMode == plmLeaderLoot);

	//在表中找到指定的物品
	int nItemIndex = 0;
	for (; nItemIndex < MAX_LOOT_SIZE; nItemIndex++)
	{
		if (m_LootList[nItemIndex].pItem == NULL)
			continue;
		if (m_LootList[nItemIndex].pItem->m_dwID == dwItemID)
			break;
	}

	KGLOG_PROCESS_ERROR(m_LootList[nItemIndex].pWinner == pLeader);

	bRetCode = IsInLooterList(pDstPlayer);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = pDstPlayer->m_ItemList.AddItem(m_LootList[nItemIndex].pItem);
	KG_PROCESS_ERROR_RET_CODE(bRetCode, srcAddLootItemFailed);

	m_LootList[nItemIndex].pItem = NULL;
	m_nLootedCount++;

	bRetCode = g_PlayerServer.DoRollItemMessage(pDstPlayer, pDoodad->m_dwID, dwItemID, 
		TRUE, -1);
	KGLOG_CHECK_ERROR(bRetCode); 

	nResult = srcSuccess;
Exit0:
	return nResult;
}
#endif //_SERVER

BOOL KLootList::AddMoney(int nMoney)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(nMoney >= 0);

	m_nMoney += nMoney;

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KLootList::SetMoney(int nMoney)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(nMoney >= 0);

	m_nMoney = nMoney;

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

#if defined(_SERVER)
BOOL KLootList::IsEmpty()
{
	BOOL bRetCode = FALSE;
	int nItem = GetSize();
	int nMoney = GetMoney();
	
	KG_PROCESS_ERROR(nItem == 0 && nMoney == 0);

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}
#endif //defined(_SERVER)



BOOL KLootList::IsInLooterList(KPlayer* pPlayer)
{
	BOOL bRetCode = FALSE;
	int nIndex = 0;

	KG_PROCESS_SUCCESS(m_nLootMode == plmAbsoluteFree);

	for (nIndex = 0; nIndex < MAX_PARTY_SIZE; nIndex++)
	{
		if (m_pLooterList[nIndex] == pPlayer)
			break;
	}

	KG_PROCESS_ERROR(nIndex < MAX_PARTY_SIZE);

Exit1:
	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KLootList::SetWinner(int nItemIndex, KPlayer *pWinner, BOOL bCanOTFreeLoot)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(nItemIndex >= 0 && nItemIndex < m_nItemCount);
	KGLOG_PROCESS_ERROR(pWinner);

	KGLOG_PROCESS_ERROR(g_IsPlayer(pWinner->m_dwID));

	m_LootList[nItemIndex].pWinner = pWinner;
	m_LootList[nItemIndex].bCanOTFreeLoot = bCanOTFreeLoot;

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

void KLootList::OnOpen()
{
	BOOL bRetCode = FALSE;

#if defined(_SERVER)
	if (m_bIsOpened == FALSE)
	{
		// 初次打开时的操作
		for (int nItemIndex = 0; nItemIndex < m_nItemCount; nItemIndex++)
		{
			if (m_nLootMode == plmGroupLoot && m_LootList[nItemIndex].pItem && 
				m_LootList[nItemIndex].pItem->m_GenParam.nQuality >= m_nRollQuality)
			{
				for (int nIndex = 0; nIndex < m_nLooterCount; nIndex++)
				{
					if (m_pLooterList[nIndex])
					{
						bRetCode = g_PlayerServer.DoBeginRollItem(m_pLooterList[nIndex]->m_nConnIndex, 
											m_LootList[nItemIndex].pItem, m_pDoodad);
						KGLOG_CHECK_ERROR(bRetCode);
					}
				}
			}
		}

		KGLOG_PROCESS_ERROR(m_pDoodad);
		//非尸体类型的Doodad被打开了,设置删除时间,尸体类型在生成时已经设置了
		if (m_pDoodad->m_eKind != dkCorpse)
		{
			bRetCode = m_pDoodad->SetDelayRemove();
			KGLOG_CHECK_ERROR(bRetCode);
		}
	}
#endif //_SERVER
	m_bIsOpened = TRUE;

Exit0:
	return;
}

void KLootList::OnClose()
{

}
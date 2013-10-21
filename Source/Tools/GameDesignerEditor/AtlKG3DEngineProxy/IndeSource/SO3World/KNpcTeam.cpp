#include "stdafx.h"

#include "KNpc.h"
#include "KNpcTeam.h"
#include "KSO3World.h"
#include "KThreatList.h"

KNpcTeam::KNpcTeam(void)
{
	m_nMemberCount = 0;
	m_pLeader = NULL;
	m_pKing = NULL;
	m_pOrder = NULL;
	m_nOrder = -1;

	memset(m_pMember, 0, sizeof(m_pMember));
}

KNpcTeam::~KNpcTeam(void)
{

}

void KNpcTeam::Activate(void)
{
	int nIndex = 0;
	KNpc* pNpc = NULL;

	for (nIndex = 0; nIndex < m_nMemberCount; nIndex++)
	{
		pNpc = m_pMember[nIndex];
		if (pNpc == NULL)
			continue;

		//检查返回状态

		//检查巡逻状态
		
	}

}

BOOL KNpcTeam::Init(DWORD dwOrder)
{
    // 注意,dwOrder为空也是正常的,某些小队并不巡逻,所以不需要Order
	m_pOrder = g_pSO3World->m_Settings.m_OrderManager.GetOrder(dwOrder);

	m_nOrder = (int)dwOrder;

    return true;
}

void KNpcTeam::UnInit()
{
    m_pOrder = NULL;
}

void KNpcTeam::SetOrder(DWORD dwOrder)
{
	m_pOrder = g_pSO3World->m_Settings.m_OrderManager.GetOrder(dwOrder);
	m_nOrder = (int)dwOrder;
}

int KNpcTeam::GetOrder()
{
	return m_nOrder;
}

BOOL KNpcTeam::SetLeader(DWORD dwLeader)
{
	BOOL bRetCode = FALSE;
	KGLOG_PROCESS_ERROR(dwLeader < MAX_NPC_TEAM_COUNT);
	KGLOG_PROCESS_ERROR(m_pMember[dwLeader]);

	m_pLeader = m_pMember[dwLeader];

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KNpcTeam::SetLeader()
{
	BOOL bResult = FALSE;
	int nIndex = 0;

	m_pLeader = NULL;
	for (nIndex = 0; nIndex < m_nMemberCount; nIndex++)
	{
		if (m_pMember[nIndex] && m_pMember[nIndex]->m_eMoveState != cmsOnDeath)
		{
			m_pLeader = m_pMember[nIndex];
			bResult = TRUE;
			break;
		}
	}
	
Exit0:
	return bResult;
}

BOOL KNpcTeam::SetKing(DWORD dwKing)
{
	BOOL bRetCode = FALSE;
	KGLOG_PROCESS_ERROR(dwKing >= 0);
	KGLOG_PROCESS_ERROR(dwKing < MAX_NPC_TEAM_COUNT);
	KGLOG_PROCESS_ERROR(m_pMember[dwKing]);

	m_pKing = m_pMember[dwKing];

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KNpcTeam::AddMember(KNpc *pNewMember, int nIndex)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pNewMember);
	KGLOG_PROCESS_ERROR(nIndex >= 0);
	KGLOG_PROCESS_ERROR(m_pMember[nIndex] == NULL);

	m_pMember[nIndex] = pNewMember;

	m_nMemberCount++;
	if (m_nMemberCount == 1)
	{
		bRetCode = SetLeader(nIndex);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KNpcTeam::RemoveMember(int nIndex)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(nIndex >= 0);
	KGLOG_PROCESS_ERROR(m_pMember[nIndex] != NULL);

	m_pMember[nIndex] = NULL;
	m_nMemberCount--;

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

KNpc* KNpcTeam::GetMember(int nIndex)
{
	KNpc* pResult = NULL;
	KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_NPC_TEAM_COUNT);

	pResult = m_pMember[nIndex];
Exit0:
	return pResult;
}

BOOL KNpcTeam::GetMemberPostion(int nIndex, int* pnDistance, int* pnAngle)
{
    BOOL    bResult     = false;
	int     nSize       = 0;

    ASSERT(pnDistance);
    ASSERT(pnAngle);

    KGLOG_PROCESS_ERROR(m_pOrder);

    nSize = (int)m_pOrder->size();

    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < nSize);

    *pnDistance = (*m_pOrder)[nIndex].nRadius;
    *pnAngle    = (*m_pOrder)[nIndex].nAngel;
	
	bResult = true;
Exit0:
	return bResult;
}

#ifdef _SERVER
BOOL KNpcTeam::OnMemberDeath(int nIndex)
{
	BOOL bResult = FALSE;
	
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < MAX_NPC_TEAM_COUNT);
	KGLOG_PROCESS_ERROR(nIndex < m_nMemberCount);
	
	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KNpcTeam::OnMemberRevive(int nIndex, OUT int& nX, OUT int& nY, OUT int& nZ)
{
	BOOL bResult = FALSE;

	ASSERT(nIndex >= 0);
	ASSERT(nIndex < MAX_NPC_TEAM_COUNT);
	KGLOG_PROCESS_ERROR(nIndex < m_nMemberCount);
	
	// TODO:

	bResult = TRUE;
Exit0:
	return bResult;
}

// 得到一个队友的目标加到自己的仇恨列表里,是谁无所谓
KCharacter* KNpcTeam::GetMemberTarget()
{
	KCharacter* pCharacter = NULL;
	KThreatNode* pNode = NULL;
	int nIndex = 0;

	for (nIndex = 0; nIndex < m_nMemberCount; nIndex++)
	{
		if (m_pMember[nIndex])
		{
			pNode = m_pMember[nIndex]->m_ThreatList.GetFirstThreat();
			if (pNode && pNode->pCharacter)
			{
				pCharacter = pNode->pCharacter;
				break;
			}
		}
	}

	return pCharacter;
}

// 检查Npc的工作是否都完成了, 一般是队伍的Leader调用
BOOL KNpcTeam::CheckNpcWorkDone()
{
	BOOL bResult = FALSE;
	int nIndex = 0;
	KNpc* pNpc = NULL;
	KAI_STATE eAiState = aisInvalid; 

	// 保证所有的Npc都在Wait状态
	for (nIndex = 0; nIndex < m_nMemberCount; nIndex++)
	{
		pNpc = m_pMember[nIndex];
		if (pNpc)
		{
			eAiState = pNpc->m_AIController.GetAIState();
			if (eAiState != aisWait && pNpc->m_eMoveState != cmsOnDeath)
				break;
		}
	}

	bResult = (nIndex == m_nMemberCount) ? TRUE : FALSE;
Exit0:
	return bResult;
}

#endif //_SERVER

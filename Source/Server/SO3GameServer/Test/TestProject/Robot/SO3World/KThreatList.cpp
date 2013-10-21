#include "stdafx.h"

#include "KThreatList.h"
#include "KSO3World.h"
#include "KObjectManager.h"
#include "KMath.h"

KThreatList::KThreatList(void)
{
	m_pSelf = NULL;
}

KThreatList::~KThreatList(void)
{

}

BOOL KThreatList::Init(KCharacter* pSelf)
{
	KGLOG_PROCESS_ERROR(pSelf);
	KGLOG_PROCESS_ERROR(m_ThreatList.IsEmpty());
	KGLOG_PROCESS_ERROR(m_ThreatenList.IsEmpty());

	m_pSelf = pSelf;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL KThreatList::UnInit(void)
{	
	BOOL				bRetCode		= FALSE;
	KThreatLiteNode*	pThreatNode		= NULL;
	KThreatLiteNode*	pThreatenNode	= NULL;

	pThreatNode = (KThreatLiteNode*)m_ThreatList.GetHead();
	while (pThreatNode)
	{
		KThreatLiteNode* pNext = (KThreatLiteNode*)pThreatNode->GetNext();

		bRetCode = ClearThreat(pThreatNode);
		KGLOG_CHECK_ERROR(bRetCode);

		pThreatNode = pNext;
	}

	pThreatenNode = (KThreatLiteNode*)m_ThreatenList.GetHead();
	while (pThreatenNode)
	{
		KThreatLiteNode* pNext = (KThreatLiteNode*)pThreatenNode->GetNext();

		bRetCode = ClearThreat(pThreatenNode);
		KGLOG_CHECK_ERROR(bRetCode);

		pThreatenNode = pNext;
	}

	return TRUE;
}

BOOL KThreatList::ModifyThreat(DWORD dwCharacterID)
{
	BOOL bRetCode = FALSE;
	KThreatLiteNode* pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);
	
	bRetCode = IsInThreatList(dwCharacterID);

	if (bRetCode == FALSE)
	{
		//插入新节点
		pNode = g_ObjectManager.m_ThreatLiteNodeSet.New();
		pNode->dwCharacterID = dwCharacterID;
		m_ThreatList.AddTail(pNode);
	}
	
	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KThreatList::ModifyThreaten(DWORD dwCharacterID)
{
	BOOL bRetCode = FALSE;
	KThreatLiteNode* pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	bRetCode = IsInThreatenList(dwCharacterID);

	if (bRetCode == FALSE)
	{
		//插入新节点
		pNode = g_ObjectManager.m_ThreatLiteNodeSet.New();
		pNode->dwCharacterID = dwCharacterID;
		m_ThreatenList.AddTail(pNode);
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KThreatList::ClearThreat(DWORD dwCharacterID)
{
	BOOL bRetCode = FALSE;
	KThreatLiteNode* pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	pNode = (KThreatLiteNode*)m_ThreatList.GetHead();
	while (pNode)
	{
		if (pNode->dwCharacterID == dwCharacterID)
		{
			bRetCode = ClearThreat(pNode);
			KGLOG_PROCESS_ERROR(bRetCode);
			break;
		}

		pNode = (KThreatLiteNode*)pNode->GetNext();
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KThreatList::ClearThreaten(DWORD dwCharacterID)
{
	BOOL bRetCode = FALSE;
	KThreatLiteNode* pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	pNode = (KThreatLiteNode*)m_ThreatenList.GetHead();
	while (pNode)
	{
		if (pNode->dwCharacterID == dwCharacterID)
		{
			bRetCode = ClearThreat(pNode);
			KGLOG_PROCESS_ERROR(bRetCode);
			break;
		}

		pNode = (KThreatLiteNode*)pNode->GetNext();
	}

	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

BOOL KThreatList::ClearThreat(KThreatLiteNode* pNode)
{
	KGLOG_PROCESS_ERROR(pNode);
	
	pNode->Remove();
	g_ObjectManager.m_ThreatLiteNodeSet.Delete(pNode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL KThreatList::IsInThreatList(DWORD dwCharacterID)
{
	KThreatLiteNode *pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	pNode = (KThreatLiteNode*)m_ThreatList.GetHead();
	while (pNode)
	{
		if (pNode->dwCharacterID == dwCharacterID)
			return TRUE;

		pNode = (KThreatLiteNode*)pNode->GetNext();
	}

Exit0:
	return FALSE;
}

BOOL KThreatList::IsInThreatenList(DWORD dwCharacterID)
{
	KThreatLiteNode *pNode = NULL;

	KGLOG_PROCESS_ERROR(dwCharacterID != ERROR_ID);

	pNode = (KThreatLiteNode*)m_ThreatenList.GetHead();
	while (pNode)
	{
		if (pNode->dwCharacterID == dwCharacterID)
			return TRUE;

		pNode = (KThreatLiteNode*)pNode->GetNext();
	}
Exit0:
	return FALSE;
}

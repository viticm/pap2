#include "stdafx.h"
#include "KPlayer.h"
#include "KWorldSettings.h"
#include "KReputation.h"

KReputation::KReputation(void)
{
}

KReputation::~KReputation(void)
{
}

char* KReputation::Load(char* pData)
{
	KGLOG_PROCESS_ERROR(pData);
	ASSERT(m_pPlayer);

	BYTE* pbyID = (BYTE*)pData;
	pData += sizeof(BYTE);

	m_pPlayer->m_eCamp = (KCAMP)*pbyID;

	pbyID = (BYTE*)pData;
	pData += sizeof(BYTE);

	m_pPlayer->m_dwForceID = *pbyID;

	WORD* pwCount = (WORD*)pData;
	pData += sizeof(WORD);

	int i = 0;
	for (i = 0; i < *pwCount; i++)
	{
		short* pshRepute = (short*)pData;
		pData += sizeof(short);

		m_ForceInfo[i].nRepute = *pshRepute;
	}

	for (i = 0; i < *pwCount; i++)
	{
		BYTE* pbyFightFlag = (BYTE*)pData;
		pData += sizeof(BYTE);

		m_ForceInfo[i].bFightFlag = (BOOL)*pbyFightFlag;
	}
	
	return pData;
Exit0:
	return NULL;
}

char* KReputation::Save(char* pData)
{
	KGLOG_PROCESS_ERROR(pData);
	ASSERT(m_pPlayer);
	
	// ËùÊôÕóÓª
	BYTE*	pbyID = (BYTE*)pData;
	pData += sizeof(BYTE);

	*pbyID = (BYTE)m_pPlayer->m_eCamp;

	pbyID = (BYTE*)pData;
	pData += sizeof(BYTE);

	*pbyID = (BYTE)m_pPlayer->m_dwForceID;

	WORD* pwCount = (WORD*)pData;
	pData += sizeof(WORD);

	*pwCount = MAX_FORCE_COUNT;
	
	int i = 0;
	for (i = 0; i < MAX_FORCE_COUNT; i++)
	{
		short* pshRepute = (short*)pData;
		pData += sizeof(short);

		*pshRepute = (short)m_ForceInfo[i].nRepute;
	}

	for (i = 0; i < MAX_FORCE_COUNT; i++)
	{
		BYTE* pbyFightFlag = (BYTE*)pData;
		pData += sizeof(BYTE);

		*pbyFightFlag = (BYTE)m_ForceInfo[i].bFightFlag;
	}

	return pData;
Exit0:
	return NULL;
}

int KReputation::GetReputation(DWORD dwForceID)
{
	int nResult = 0;
	
	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

	nResult = m_ForceInfo[dwForceID].nRepute;

Exit0:
	return nResult;
}

BOOL KReputation::SetReputation(DWORD dwForceID, int nReputation)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);
	KGLOG_PROCESS_ERROR(nReputation >= MIN_REPUTE && nReputation <= MAX_REPUTE);

	m_ForceInfo[dwForceID].nRepute = nReputation;

#if defined(_SERVER)
	ASSERT(m_pPlayer);
	bRetCode = g_PlayerServer.DoSyncRepute(m_pPlayer->m_nConnIndex, dwForceID, nReputation);
	KGLOG_CHECK_ERROR(bRetCode);
#endif

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KReputation::ModifyReputation(DWORD dwForceID, int nAdjustValue)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

	m_ForceInfo[dwForceID].nRepute += nAdjustValue;

	if (m_ForceInfo[dwForceID].nRepute < MIN_REPUTE)
		m_ForceInfo[dwForceID].nRepute = MIN_REPUTE;
	else if (m_ForceInfo[dwForceID].nRepute > MAX_REPUTE)
		m_ForceInfo[dwForceID].nRepute = MAX_REPUTE;

#if defined(_SERVER)
	ASSERT(m_pPlayer);
	bRetCode = g_PlayerServer.DoSyncRepute(m_pPlayer->m_nConnIndex, 
		dwForceID, m_ForceInfo[dwForceID].nRepute);
	KGLOG_CHECK_ERROR(bRetCode);
#endif

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KReputation::Init(KPlayer* pPlayer)
{
	BOOL bResult = FALSE;
	int nSize = g_WorldSettings.m_ForceDefaultList.GetSize();
	KGLOG_PROCESS_ERROR(pPlayer);
	KGLOG_PROCESS_ERROR(nSize <= MAX_FORCE_COUNT);

	ZeroMemory(m_ForceInfo, sizeof(m_ForceInfo));
	for (int i = 0; i < nSize; i++)
	{
		FORCE_DEFAULT* pForceInfo = g_WorldSettings.m_ForceDefaultList.GetDefaultRepute((DWORD)i);
		KGLOG_PROCESS_ERROR(pForceInfo);
		m_ForceInfo[i].bFightFlag = pForceInfo->bDefaultFlag;
		m_ForceInfo[i].nRepute = pForceInfo->nRepute;
	}
	m_pPlayer = pPlayer;

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL KReputation::GetFightFlag(DWORD dwForceID)
{
	BOOL bResult = FALSE;
	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

	bResult = m_ForceInfo[dwForceID].bFightFlag;
Exit0:
	return bResult;
}

BOOL KReputation::SetFightFlag(DWORD dwForceID, BOOL bFight)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;
	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);
	
	FORCE_DEFAULT* pForceDefault = g_WorldSettings.m_ForceDefaultList.GetDefaultRepute(dwForceID);
	KG_PROCESS_ERROR(pForceDefault && !pForceDefault->bLockedFlag);

	m_ForceInfo[dwForceID].bFightFlag = bFight;

#if defined(_SERVER)
	ASSERT(m_pPlayer);
	bRetCode = g_PlayerServer.DoSyncFightFlag(m_pPlayer->m_nConnIndex, dwForceID, bFight);
	KGLOG_CHECK_ERROR(bRetCode);
#endif
	bResult = TRUE;
Exit0:
	return bResult;
}

int KReputation::GetReputeLevel(DWORD dwForceID)
{
	int nResult = 0;
	int nRepute = 0;

#define MAX_FORCE_REPUTE_LEVEL	13
	static int arynReputeLimit[MAX_FORCE_REPUTE_LEVEL - 1] =
	{
		-1000,
		0,
		200,
		600,
		1300,
		2400,
		4000,
		6200,
		9100,
		12800,
		17400,
		23000,
	};

	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

	nRepute = m_ForceInfo[dwForceID].nRepute;

	for (nResult = 0; nResult < MAX_FORCE_REPUTE_LEVEL - 1; nResult++)
	{
		if (nRepute < arynReputeLimit[nResult])
			break;
	}
Exit0:
	return nResult;
}
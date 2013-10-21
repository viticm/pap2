#include "stdafx.h"
#include "KPlayer.h"
#include "KUserPreferences.h"
#include "KPlayerServer.h"

KUserPreferences::KUserPreferences(void)
{
    m_pPlayer = NULL;
}

KUserPreferences::~KUserPreferences(void)
{
    m_pPlayer = NULL;
}

BOOL KUserPreferences::Init(KPlayer* pPlayer)
{
	memset(m_byUserPrefences, 0, sizeof(m_byUserPrefences));
    m_pPlayer = pPlayer;

	return true;
}

void KUserPreferences::UnInit()
{	
    m_pPlayer = NULL;
	return;
}

BOOL KUserPreferences::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL    bResult     = false;
    size_t  uLeftSize   = uBufferSize;
    BYTE*   pbyOffset   = pbyBuffer;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    *(int*)pbyOffset = MAX_USER_PREFERENCES_LEN;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= MAX_USER_PREFERENCES_LEN);
    memcpy(pbyOffset, m_byUserPrefences, MAX_USER_PREFERENCES_LEN);
    uLeftSize -= MAX_USER_PREFERENCES_LEN;
    pbyOffset += MAX_USER_PREFERENCES_LEN;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KUserPreferences::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL    bResult     = false;
    int     nSize       = 0;
    size_t  uLeftSize   = uDataLen;
    BYTE*   pbyOffset   = pbyData;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(int));
    nSize = *(int*)pbyOffset;
    uLeftSize -= sizeof(int);
    pbyOffset += sizeof(int);

    KGLOG_PROCESS_ERROR(uLeftSize >= (size_t)nSize);

	if (nSize > MAX_USER_PREFERENCES_LEN)
		memcpy(m_byUserPrefences, pbyOffset, MAX_USER_PREFERENCES_LEN);
	else
		memcpy(m_byUserPrefences, pbyOffset, nSize);

    uLeftSize -= nSize;
    pbyOffset += nSize;

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

#ifdef _SERVER
    g_PlayerServer.DoSyncUserPreferences(m_pPlayer->m_nConnIndex, 0, MAX_USER_PREFERENCES_LEN, m_byUserPrefences);
#endif

    bResult = true;
Exit0:
    return bResult;
}

BOOL KUserPreferences::SetUserPreferences(int nOffset, int nLength, BYTE* pbyData)
{
	BOOL    bResult = false;
	int     nIndex  = 0;

	assert(pbyData);
	KGLOG_PROCESS_ERROR(nOffset >= 0 && nOffset < MAX_USER_PREFERENCES_LEN);
    KGLOG_PROCESS_ERROR(nLength > 0 && nLength <= MAX_USER_PREFERENCES_LEN);
	KGLOG_PROCESS_ERROR(nOffset + nLength <= MAX_USER_PREFERENCES_LEN);

    memcpy(m_byUserPrefences + nOffset, pbyData, nLength);

	bResult = true;
Exit0:
	return bResult;
}

BOOL KUserPreferences::GetUserPreferences(int nOffset, int nLength, BYTE* pbyData)
{
	BOOL bResult = false;
	int nIndex = 0;

	KGLOG_PROCESS_ERROR(pbyData);
	KGLOG_PROCESS_ERROR(nOffset >= 0);
	KGLOG_PROCESS_ERROR(nLength > 0 && nLength <= MAX_USER_PREFERENCES_LEN);

	for (nIndex = 0; nIndex < nLength; nIndex++)
	{
		pbyData[nIndex] = m_byUserPrefences[nOffset + nIndex];
	}

	bResult = true;
Exit0:
	return bResult;
}

BYTE* KUserPreferences::GetUserPreferences(int nOffset)
{
	KGLOG_PROCESS_ERROR(nOffset >= 0 && nOffset < MAX_USER_PREFERENCES_LEN);

	return m_byUserPrefences + nOffset;
Exit0:
	return NULL;
}

BYTE* KUserPreferences::GetData()
{
	return m_byUserPrefences;
}

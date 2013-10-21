#include "stdafx.h"
#include "KPlayer.h"
#include "KSO3World.h"
#include "KPlayerServer.h"
#include "KReputation.h"

BOOL KReputation::Init(KPlayer* pPlayer)
{
    BOOL            bResult     = false;
    int             nSize       = g_pSO3World->m_Settings.m_ForceDefaultList.GetSize();
    FORCE_DEFAULT*  pForceInfo  = NULL;

    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(nSize <= MAX_FORCE_COUNT);

    ZeroMemory(m_ForceInfo, sizeof(m_ForceInfo));
    for (int i = 0; i < nSize; i++)
    {
        pForceInfo = g_pSO3World->m_Settings.m_ForceDefaultList.GetDefaultRepute((DWORD)i);
        KGLOG_PROCESS_ERROR(pForceInfo);

        m_ForceInfo[i].bFightFlag   = pForceInfo->bDefaultFlag;
        m_ForceInfo[i].nRepute      = pForceInfo->nRepute;
        m_ForceInfo[i].nReputeLevel = pForceInfo->nLevel;
        m_ForceInfo[i].bUsed        = true;
        m_ForceInfo[i].bShow        = false;
    }

    m_pPlayer = pPlayer;

    m_dwPrinkForceID = MAX_FORCE_COUNT;
    m_nPrinkForceLevel = 0;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KReputation::Load(BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    KFORCE_DB_DATA* pForceDBData    = NULL;
    size_t          uLeftSize       = uDataLen;
    BYTE*           pbyOffset       = pbyData;

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA));
    pForceDBData = (KFORCE_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(KFORCE_DB_DATA);
    pbyOffset += sizeof(KFORCE_DB_DATA);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA::FORECE_INFO) * pForceDBData->wCount);
    uLeftSize -= sizeof(KFORCE_DB_DATA::FORECE_INFO) * pForceDBData->wCount;
    pbyOffset += sizeof(KFORCE_DB_DATA::FORECE_INFO) * pForceDBData->wCount;

    for (int nIndex = 0; nIndex < pForceDBData->wCount; nIndex++)
    {
        SetFightFlag(pForceDBData->ForceInfo[nIndex].byForce, pForceDBData->ForceInfo[nIndex].byFightFlag, true);

        bRetCode = SetReputation(
            pForceDBData->ForceInfo[nIndex].byForce,
            pForceDBData->ForceInfo[nIndex].byLevel,
            pForceDBData->ForceInfo[nIndex].shRepute
        );
        if (!bRetCode)
        {
#ifdef _SERVER
            KGLogPrintf(
                KGLOG_ERR, "[KReputation] Load Error. Accout:%s, Name:%s, ForceID:%u, Level:%u, Repute:%u.\n",
                m_pPlayer->m_szAccount, m_pPlayer->m_szName, pForceDBData->ForceInfo[nIndex].byForce,
                pForceDBData->ForceInfo[nIndex].byLevel, pForceDBData->ForceInfo[nIndex].shRepute
            );
#endif
            continue;
        }

        if (pForceDBData->ForceInfo[nIndex].byShow)
        {
            Show(pForceDBData->ForceInfo[nIndex].byForce, true);
        }
    }

    KGLOG_PROCESS_ERROR(uLeftSize == 0);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KReputation::Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
{
    BOOL            bResult         = false;
    size_t          uLeftSize       = uBufferSize;
    BYTE*           pbyOffset       = pbyBuffer;
    KFORCE_DB_DATA* pForceDBData    = NULL;
    int             nCount          = 0;

    assert(m_pPlayer);

    KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA));
    pForceDBData = (KFORCE_DB_DATA*)pbyOffset;
    uLeftSize -= sizeof(KFORCE_DB_DATA);
    pbyOffset += sizeof(KFORCE_DB_DATA);

    for (int i = 0; i < MAX_FORCE_COUNT; i++)
    {
        if (!m_ForceInfo[i].bUsed)
        {
            continue;
        }

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KFORCE_DB_DATA::FORECE_INFO));
        uLeftSize -= sizeof(KFORCE_DB_DATA::FORECE_INFO);
        pbyOffset += sizeof(KFORCE_DB_DATA::FORECE_INFO);

        pForceDBData->ForceInfo[nCount].byForce     = (BYTE)i;
        pForceDBData->ForceInfo[nCount].byFightFlag = (BYTE)m_ForceInfo[i].bFightFlag;
        pForceDBData->ForceInfo[nCount].byLevel     = (BYTE)m_ForceInfo[i].nReputeLevel;
        pForceDBData->ForceInfo[nCount].shRepute    = (short)m_ForceInfo[i].nRepute;
        pForceDBData->ForceInfo[nCount].byShow      = (BYTE)m_ForceInfo[i].bShow;

        nCount++;
    }

    pForceDBData->wCount = (WORD)nCount;

    *puUsedSize = uBufferSize - uLeftSize;

    bResult = true;
Exit0:
    return bResult;
}

int KReputation::GetReputation(DWORD dwForceID)
{
	int nResult = 0;
	
	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

	nResult = m_ForceInfo[dwForceID].nRepute;

Exit0:
	return nResult;
}

BOOL KReputation::SetReputation(DWORD dwForceID, int nLevel, int nReputation)
{
	BOOL bResult            = false;
	BOOL bRetCode           = false;
    BOOL bShow              = false;
    int  nMaxReputeLevel    = 0;
    int  nMaxRepute         = 0;
    int  nOldLevel          = 0;

	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

	nMaxReputeLevel = g_pSO3World->m_Settings.m_ReputeLimit.GetMaxLevel();
	KGLOG_PROCESS_ERROR(nLevel >= 0 && nLevel <= nMaxReputeLevel);

	nMaxRepute = g_pSO3World->m_Settings.m_ReputeLimit.GetReputeLimit(nLevel);
	KGLOG_PROCESS_ERROR(nReputation >= 0 && nReputation <= nMaxRepute);
    
    nOldLevel = m_ForceInfo[dwForceID].nReputeLevel;

	m_ForceInfo[dwForceID].nReputeLevel = nLevel;
	m_ForceInfo[dwForceID].nRepute      = nReputation;

#ifdef _SERVER
	assert(m_pPlayer);
	g_PlayerServer.DoSyncRepute(m_pPlayer->m_nConnIndex, dwForceID, nLevel, nReputation, 0);

    if (nOldLevel <= ENEMY_REPUTE_LEVEL && m_ForceInfo[dwForceID].nReputeLevel > ENEMY_REPUTE_LEVEL)
    {
        SetFightFlag(dwForceID, false, true);
    }
    else if (nOldLevel > ENEMY_REPUTE_LEVEL && m_ForceInfo[dwForceID].nReputeLevel <= ENEMY_REPUTE_LEVEL)
    {
        SetFightFlag(dwForceID, true, true);
    }
#endif

	bResult = true;
Exit0:
	return bResult;
}

BOOL KReputation::ModifyReputation(
    DWORD dwForceID, int nAdjustValue, int nLowLimit /* = -1 */, int nHighLimit /* = -1 */
)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    BOOL    bShow           = false;
    int     nMaxReputeLevel = 0;
    int     nMinReputeLevel = 0;
    int     nMaxReputeLimit = 0;
    int     nNowReputeValue = 0;
    int     nOldLevel       = 0;
    int     nAddRepute      = 0;

    KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

    nOldLevel = m_ForceInfo[dwForceID].nReputeLevel;

    nMaxReputeLevel = g_pSO3World->m_Settings.m_ReputeLimit.GetMaxLevel();

    if (nHighLimit >= 0)
    {
        nMaxReputeLevel = min(nMaxReputeLevel, nHighLimit);
        KG_PROCESS_SUCCESS(nAdjustValue >= 0 && nMaxReputeLevel < m_ForceInfo[dwForceID].nReputeLevel);
    }

    if (nLowLimit >= 0)
    {
        nMinReputeLevel = max(nMinReputeLevel, nLowLimit);
        KG_PROCESS_SUCCESS(nAdjustValue <= 0 && nMinReputeLevel > m_ForceInfo[dwForceID].nReputeLevel);
    }

    for (int i = 0; i <= nMaxReputeLevel; i++)
    {
        int nMaxRepute = g_pSO3World->m_Settings.m_ReputeLimit.GetReputeLimit(i);

        nMaxReputeLimit += nMaxRepute;
    }

    nNowReputeValue = m_ForceInfo[dwForceID].nRepute;
    for (int i = 0; i < m_ForceInfo[dwForceID].nReputeLevel; i++)
    {
        int nLevelRepute = g_pSO3World->m_Settings.m_ReputeLimit.GetReputeLimit(i);

        nNowReputeValue += nLevelRepute;
    }

    nNowReputeValue += nAdjustValue;
    nAddRepute = nAdjustValue;
    if (nNowReputeValue > nMaxReputeLimit)
    {
        nAddRepute -= (nNowReputeValue - nMaxReputeLimit);
        nNowReputeValue = nMaxReputeLimit;
    }

    if (nNowReputeValue < 0)
    {
        nAddRepute -= nNowReputeValue;
        nNowReputeValue = 0;
    }

    m_ForceInfo[dwForceID].nReputeLevel = 0;
    while (true)
    {
        int nLevelRepute = g_pSO3World->m_Settings.m_ReputeLimit.GetReputeLimit(m_ForceInfo[dwForceID].nReputeLevel);
        KGLOG_PROCESS_ERROR(nLevelRepute != -1);

        if (nNowReputeValue < nLevelRepute || m_ForceInfo[dwForceID].nReputeLevel == nMaxReputeLevel)
        {
            m_ForceInfo[dwForceID].nRepute = nNowReputeValue;
            break;
        }

        m_ForceInfo[dwForceID].nReputeLevel++;
        nNowReputeValue -= nLevelRepute;
    }

#ifdef _SERVER
    assert(m_pPlayer);
    g_PlayerServer.DoSyncRepute(
        m_pPlayer->m_nConnIndex, dwForceID, m_ForceInfo[dwForceID].nReputeLevel, m_ForceInfo[dwForceID].nRepute, nAddRepute
    );

    bShow = IsShow(dwForceID);
    if (!bShow)
    {
        Show(dwForceID);
    }

    //声望等级等级超过仇恨线后,改变战斗标记 By zhuchengmin
    if (nOldLevel <= ENEMY_REPUTE_LEVEL && m_ForceInfo[dwForceID].nReputeLevel > ENEMY_REPUTE_LEVEL)
    {
        SetFightFlag(dwForceID, false, true);
    }
    else if (nOldLevel > ENEMY_REPUTE_LEVEL && m_ForceInfo[dwForceID].nReputeLevel <= ENEMY_REPUTE_LEVEL)
    {
        SetFightFlag(dwForceID, true, true);
    }
#endif
Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KReputation::GetFightFlag(DWORD dwForceID)
{
	BOOL bResult = false;

	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);
    
    if (m_dwPrinkForceID != dwForceID)
    {
        bResult = m_ForceInfo[dwForceID].bFightFlag || m_ForceInfo[dwForceID].nReputeLevel <= ENEMY_REPUTE_LEVEL;
    }
    else
    {
        bResult = m_nPrinkForceLevel <= ENEMY_REPUTE_LEVEL;
    }

Exit0:
	return bResult;
}

BOOL KReputation::SetFightFlag(DWORD dwForceID, BOOL bFight, BOOL bForce/* = false*/)
{
	BOOL bResult = false;
	BOOL bRetCode = false;
	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

	if (!bForce)
	{
		bRetCode = IsFightFlagLocked(dwForceID);
		KG_PROCESS_ERROR(!bRetCode);
	}

	m_ForceInfo[dwForceID].bFightFlag = bFight;

#if defined(_SERVER)
	assert(m_pPlayer);
	g_PlayerServer.DoSyncFightFlag(m_pPlayer->m_nConnIndex, dwForceID, bFight);
#endif
	bResult = true;
Exit0:
	return bResult;
}

int KReputation::GetReputeLevel(DWORD dwForceID)
{
	int nResult = 0;

	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);
    
    if (m_dwPrinkForceID == dwForceID)
    {
        nResult = m_nPrinkForceLevel;
    }
    else
    {
	    nResult = m_ForceInfo[dwForceID].nReputeLevel;
    }

Exit0:
	return nResult;
}

BOOL KReputation::IsFightFlagLocked(DWORD dwForceID)
{
	BOOL bResult = false;
	int nNowLevel = 0;
    FORCE_DEFAULT* pDefault = NULL;

	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);
	pDefault = g_pSO3World->m_Settings.m_ForceDefaultList.GetDefaultRepute(dwForceID);
	KGLOG_PROCESS_ERROR(pDefault);

	nNowLevel = GetReputeLevel(dwForceID);
	bResult = pDefault->bLockedFlag || (nNowLevel <= ENEMY_REPUTE_LEVEL);

Exit0:
	return bResult;
}

BOOL KReputation::Show(DWORD dwForceID, BOOL bLoginLoading)
{
	BOOL bResult = false;
	
	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);
	
	m_ForceInfo[dwForceID].bShow = true;

#ifdef _SERVER
    assert(m_pPlayer);
	g_PlayerServer.DoShowReputation(m_pPlayer->m_nConnIndex, dwForceID, bLoginLoading);
#endif 

	bResult = true;
Exit0:
	return bResult;
}

BOOL KReputation::IsShow(DWORD dwForceID)
{
	BOOL bResult = false;

	KGLOG_PROCESS_ERROR(dwForceID < MAX_FORCE_COUNT);

    bResult = m_ForceInfo[dwForceID].bShow;
Exit0:
	return bResult;
}

BOOL KReputation::SetPrinkForce(DWORD dwForceID, int nLevel)
{
    BOOL bResult = false;

    assert(m_pPlayer);

    m_dwPrinkForceID = dwForceID;
    m_nPrinkForceLevel = nLevel;
    
#ifdef _SERVER
    g_PlayerServer.DoSyncPrinkForce(m_pPlayer, dwForceID, nLevel);
#endif

    bResult = true;
Exit0:
    return bResult;
}

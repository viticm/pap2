#include "stdafx.h"
#include "KSO3World.h"
#include "KMentorCache.h"
#include "KRelayClient.h"
#include "KPlayerServer.h"
#include "KPlayer.h"
#include "KMentorDef.h"

#ifdef _SERVER
BOOL KMentorCache::AddMentorData(DWORD dwMentor, DWORD dwApprentice, const KMentorRecordBase& crMentorInfo)
{
    BOOL                    bResult  = false;
    uint64_t                uMKey    = MAKE_INT64(dwApprentice, dwMentor);
    uint64_t                uAKey    = MAKE_INT64(dwMentor, dwApprentice);
    BOOL                    bMInsRet = false;
    std::pair<KCacheMTable::iterator, BOOL> MInsRet;
    std::pair<KCacheATable::iterator, BOOL> AInsRet;
    
    MInsRet = m_CacheMTable.insert(std::make_pair(uMKey, crMentorInfo));
    KGLOG_PROCESS_ERROR(MInsRet.second);
    bMInsRet = true;

    AInsRet = m_CacheATable.insert(std::make_pair(uAKey, &(MInsRet.first->second)));
    KGLOG_PROCESS_ERROR(AInsRet.second);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bMInsRet)
        {
            m_CacheMTable.erase(MInsRet.first);
        }
    }
    return bResult;
}

BOOL KMentorCache::UpdateMentorData(DWORD dwMentor, DWORD dwApprentice, const KMentorRecordBase& crMentorInfo)
{
    BOOL                    bResult  = false;
    BOOL                    bRetcode = false;
    KCacheMTable::iterator  itM;

    itM  = m_CacheMTable.find(MAKE_INT64(dwApprentice, dwMentor));
    if (itM == m_CacheMTable.end())
    {
        bRetcode = AddMentorData(dwMentor, dwApprentice, crMentorInfo);
        KGLOG_PROCESS_ERROR(bRetcode);
    }
    else
    {
        itM->second = crMentorInfo;
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMentorCache::DeleteMentorRecord(uint64_t uMKey)
{
    DWORD    dwMID    = LOW_DWORD_IN_UINT64(uMKey);
    DWORD    dwAID    = HIGH_DWORD_IN_UINT64(uMKey);
    uint64_t uAKey    = MAKE_INT64(dwMID, dwAID);
    KPlayer* pMPlayer = NULL;
    KPlayer* pAPlayer = NULL;

    pMPlayer = g_pSO3World->m_PlayerSet.GetObj(dwMID);
    if (pMPlayer != NULL)
    {
        g_PlayerServer.DoDeleteMentorData(pMPlayer->m_nConnIndex, dwMID, dwAID);
    }

    pAPlayer = g_pSO3World->m_PlayerSet.GetObj(dwAID);
    if (pAPlayer != NULL)
    {
        g_PlayerServer.DoDeleteMentorData(pAPlayer->m_nConnIndex, dwMID, dwAID);
    }

    m_CacheATable.erase(uAKey);
    m_CacheMTable.erase(uMKey);

    return true;
}

void KMentorCache::Clear()
{
    m_CacheATable.clear();
    m_CacheMTable.clear();

    return;
}

void KMentorCache::SyncPlayerMentorData(KPlayer* pPlayer)
{
    uint64_t                    uKeyLower = 0;
    uint64_t                    uKeyUpper = 0;
    KCacheMTable::iterator      ItMLower;
    KCacheMTable::iterator      ItMUpper;
    KCacheATable::iterator      ItALower;
    KCacheATable::iterator      ItAUpper;

    assert(pPlayer);

    uKeyLower = MAKE_INT64(pPlayer->m_dwID, 0);
    uKeyUpper = MAKE_INT64(pPlayer->m_dwID + 1, 0);

    ItMLower = m_CacheMTable.lower_bound(uKeyLower);
    ItMUpper = m_CacheMTable.lower_bound(uKeyUpper);

    for (KCacheMTable::iterator it = ItMLower; it != ItMUpper; ++it)
    {
        g_PlayerServer.DoSyncMentorData(
            pPlayer->m_nConnIndex,
            LOW_DWORD_IN_UINT64(it->first), HIGH_DWORD_IN_UINT64(it->first),
            it->second
        );
    }

    ItALower = m_CacheATable.lower_bound(uKeyLower);
    ItAUpper = m_CacheATable.lower_bound(uKeyUpper);

    for (KCacheATable::iterator it = ItALower; it != ItAUpper; ++it)
    {
        g_PlayerServer.DoSyncMentorData(
            pPlayer->m_nConnIndex,
            HIGH_DWORD_IN_UINT64(it->first), LOW_DWORD_IN_UINT64(it->first),
            *it->second
        );
    }

    return;
}

BOOL KMentorCache::IsMentorship(DWORD dwMentorID, DWORD dwApprenticeID)
{
    BOOL                    bResult = false;
    BYTE                    byState = emrsInvalid;
    KCacheMTable::iterator  it;

    it = m_CacheMTable.find(MAKE_INT64(dwApprenticeID, dwMentorID));
    KG_PROCESS_ERROR(it != m_CacheMTable.end());

    byState = it->second.byState;

    KG_PROCESS_ERROR(byState == emrsNormal || byState == emrsABreak || byState == emrsMBreak);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMentorCache::IsMentor(DWORD dwPlayerID)
{
    BOOL                   bResult   = false;
    BOOL                   bRetCode  = false;
    BYTE                   byState   = emrsInvalid;
    KCacheATable::iterator itLower   = m_CacheATable.lower_bound(MAKE_INT64(dwPlayerID, 0));
    KCacheATable::iterator itUpper   = m_CacheATable.lower_bound(MAKE_INT64(dwPlayerID + 1, 0));

    KG_PROCESS_ERROR(itLower != itUpper);

    for (KCacheATable::iterator it = itLower; it != itUpper; ++it)
    {
        byState = it->second->byState;

        bRetCode = (byState == emrsNormal || byState == emrsABreak || byState == emrsMBreak);

        if (bRetCode)
            break;
    }
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMentorCache::IsApprentice(DWORD dwPlayerID)
{
    BOOL                   bResult   = false;
    BOOL                   bRetCode  = false;
    BYTE                   byState   = emrsInvalid;
    KCacheMTable::iterator itLower   = m_CacheMTable.lower_bound(MAKE_INT64(dwPlayerID, 0));
    KCacheMTable::iterator itUpper   = m_CacheMTable.lower_bound(MAKE_INT64(dwPlayerID + 1, 0));

    KG_PROCESS_ERROR(itLower != itUpper);

    for (KCacheMTable::iterator it = itLower; it != itUpper; ++it)
    {
        byState = it->second.byState;

        bRetCode = (byState == emrsNormal || byState == emrsABreak || byState == emrsMBreak);

        if (bRetCode)
            break;
    }
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMentorCache::AddMentorValue(DWORD dwMentorID, DWORD dwApprenticeID, int nDeltaValue)
{
    BOOL bResult = false;

    bResult = g_RelayClient.DoAddMentorValueRequest(dwMentorID, dwApprenticeID, nDeltaValue);

    return bResult;
}

#endif // _SERVER

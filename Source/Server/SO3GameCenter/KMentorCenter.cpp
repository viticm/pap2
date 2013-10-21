#include "stdafx.h"
#include "KMentorCenter.h"
#include "KSO3GameCenter.h"
#include "KGameServer.h"
#include "KMentorDef.h"
#include "KRole.h"

void KMentorCenter::Activate()
{
    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 30) == 0)
    {
        SaveAll();
    }

    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 60) == 0)
    {
        CheckStateEnd();
    }

    // 每10分钟处理一次
    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 60 * 10) == 0)
    {
        ActiveShip();
    }

    return;
}

BOOL KMentorCenter::Load(DWORD dwMentorID, DWORD dwApprenticeID, BYTE* pbyData, size_t uDataLen)
{
    BOOL                    bResult     = false;
    BOOL                    bRetCode    = false;
    KMentorRecordDB*        pRecord     = NULL;
    KMentorInfo             MentorInfo;

    assert(pbyData);

    KGLOG_PROCESS_ERROR(uDataLen == sizeof(KMentorRecordDB));
    pRecord = (KMentorRecordDB*)pbyData;

    KGLOG_PROCESS_ERROR(pRecord->nVersion == MENTOR_DATA_VERSION);

    MentorInfo.nCreateTime      = pRecord->nCreateTime;     
    MentorInfo.nMentorValue     = pRecord->nMentorValue;    
    MentorInfo.nEndTime         = pRecord->nEndTime;        
    MentorInfo.byState          = pRecord->byState;
    MentorInfo.byOriginalALevel = pRecord->byOriginalALevel;
    MentorInfo.bSaved           = true;

    bRetCode = AddRecord(dwMentorID, dwApprenticeID, MentorInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMentorCenter::AddRecord(DWORD dwMentorID, DWORD dwApprenticeID, const KMentorInfo& crMentorInfo)
{
    BOOL                                        bResult           = false;
    uint64_t                                    uMentorKey        = MAKE_INT64(dwApprenticeID, dwMentorID);
    uint64_t                                    uApprenticeKey    = MAKE_INT64(dwMentorID, dwApprenticeID);
    BOOL                                        bMentorInsRet     = false;
    std::pair<KMentorTable::iterator, BOOL>     MentorInsRet;
    std::pair<KApprenticeTable::iterator, BOOL> ApprenticeInsRet;

    MentorInsRet = m_A2MTable.insert(std::make_pair(uMentorKey, crMentorInfo));
    KGLOG_PROCESS_ERROR(MentorInsRet.second);
    bMentorInsRet = true;

    ApprenticeInsRet = m_M2ATable.insert(std::make_pair(uApprenticeKey, &MentorInsRet.first->second));
    KGLOG_PROCESS_ERROR(ApprenticeInsRet.second);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (bMentorInsRet)
        {
            m_A2MTable.erase(uMentorKey);
            bMentorInsRet = false;
        }
    }
    return bResult;
}

BOOL KMentorCenter::Delete(DWORD dwMentorID, DWORD dwApprenticeID)
{
    uint64_t        uMentorKey      = MAKE_INT64(dwApprenticeID, dwMentorID);
    uint64_t        uApprenticeKey  = MAKE_INT64(dwMentorID, dwApprenticeID);

    m_M2ATable.erase(uApprenticeKey);
    m_A2MTable.erase(uMentorKey);

    g_pSO3GameCenter->m_MiscDB.DoDeleteMentorRecordRequest(dwMentorID, dwApprenticeID);
    
    g_pSO3GameCenter->m_GameServer.DoDeleteMentorRecord(uMentorKey);

    g_pSO3GameCenter->m_StatDataManager.UpdateMentorDeleteStat();

    return true;
}

KMentorInfo* KMentorCenter::GetMentorInfo(DWORD dwMentorID, DWORD dwApprenticeID)
{
    KMentorTable::iterator  it = m_A2MTable.find(MAKE_INT64(dwApprenticeID, dwMentorID));
    if (it != m_A2MTable.end())
    {
        return &it->second;
    }

    return NULL;
}

int KMentorCenter::GetMentorCount(DWORD dwApprenticeID)
{
    int                     nResult         = 0;
    uint64_t                uKeyLower       = 0;
    uint64_t                uKeyUpper       = 0;
    KMentorTable::iterator  ItLower;
    KMentorTable::iterator  ItUpper;

    uKeyLower = MAKE_INT64(dwApprenticeID, 0);
    uKeyUpper = MAKE_INT64(dwApprenticeID + 1, 0);

    KGLOG_PROCESS_ERROR(uKeyLower < uKeyUpper);
    ItLower = m_A2MTable.lower_bound(uKeyLower);
    ItUpper = m_A2MTable.lower_bound(uKeyUpper);

    for (KMentorTable::iterator it = ItLower; it != ItUpper; ++it)
    {
        int nState = it->second.byState;

        KGLOG_PROCESS_ERROR(nState > emrsInvalid && nState < emrsTotal);

        if (nState == emrsBroken || nState == emrsGraduating || nState == emrsGraduated)
            continue;

        nResult++;
    }

Exit0:
    return nResult;
}

int KMentorCenter::GetApprenticeCount(DWORD dwMentorID)
{
    int                         nResult         = 0;
    uint64_t                    uKeyLower       = 0;
    uint64_t                    uKeyUpper       = 0;
    KApprenticeTable::iterator  ItLower;
    KApprenticeTable::iterator  ItUpper;

    uKeyLower = MAKE_INT64(dwMentorID, 0);
    uKeyUpper = MAKE_INT64(dwMentorID + 1, 0);

    KGLOG_PROCESS_ERROR(uKeyLower < uKeyUpper);
    ItLower = m_M2ATable.lower_bound(uKeyLower);
    ItUpper = m_M2ATable.lower_bound(uKeyUpper);   

    for (KApprenticeTable::iterator it = ItLower; it != ItUpper && it != m_M2ATable.end(); ++it)
    {
        nResult++;
    }

Exit0:
    return nResult;
}

int KMentorCenter::GetMentorList(DWORD dwApprenticeID, DWORD dwMentorList[MAX_MENTOR_NUM])
{
    int                     nCount          = 0;
    uint64_t                uKeyLower       = 0;
    uint64_t                uKeyUpper       = 0;
    KMentorTable::iterator  ItLower;
    KMentorTable::iterator  ItUpper;

    uKeyLower = MAKE_INT64(dwApprenticeID, 0);
    uKeyUpper = MAKE_INT64(dwApprenticeID + 1, 0);

    KGLOG_PROCESS_ERROR(uKeyLower < uKeyUpper);
    ItLower = m_A2MTable.lower_bound(uKeyLower);
    ItUpper = m_A2MTable.lower_bound(uKeyUpper);

    for (KMentorTable::iterator it = ItLower; it != ItUpper; ++it)
    {
        int nState = it->second.byState;

        KGLOG_PROCESS_ERROR(nState > emrsInvalid && nState < emrsTotal);

        if (nState == emrsBroken || nState == emrsGraduating || nState == emrsGraduated)
            continue;

        dwMentorList[nCount++] = LOW_DWORD_IN_UINT64(it->first);
    }

Exit0:
    return nCount;
}

int KMentorCenter::GetApprenticeList(DWORD dwMentorID, DWORD dwApprenticeList[MAX_APPRENTICE_NUM])
{
    int                         nCount          = 0;
    uint64_t                    uKeyLower       = 0;
    uint64_t                    uKeyUpper       = 0;
    KApprenticeTable::iterator  ItLower;
    KApprenticeTable::iterator  ItUpper;

    uKeyLower = MAKE_INT64(dwMentorID, 0);
    uKeyUpper = MAKE_INT64(dwMentorID + 1, 0);

    KGLOG_PROCESS_ERROR(uKeyLower < uKeyUpper);
    ItLower = m_M2ATable.lower_bound(uKeyLower);
    ItUpper = m_M2ATable.lower_bound(uKeyUpper);   

    for (KApprenticeTable::iterator it = ItLower; it != ItUpper && it != m_M2ATable.end(); ++it)
    {
        dwApprenticeList[nCount++] = LOW_DWORD_IN_UINT64(it->first);
    }

Exit0:
    return nCount;
}

void KMentorCenter::SaveAll()
{
    DWORD                   dwMentorID          = ERROR_ID;
    DWORD                   dwApprenticeID      = ERROR_ID;
    KMentorRecordDB         Record;

    for (KMentorTable::iterator it = m_A2MTable.begin(); it != m_A2MTable.end(); ++it)
    {
        KMentorInfo* pInfo = &it->second;

        if (pInfo->bSaved)
            continue;

        dwMentorID      = LOW_DWORD_IN_UINT64(it->first);
        dwApprenticeID  = HIGH_DWORD_IN_UINT64(it->first);

        Record.nVersion         = MENTOR_DATA_VERSION;
        Record.nCreateTime      = pInfo->nCreateTime;
        Record.nMentorValue     = pInfo->nMentorValue;
        Record.nEndTime         = pInfo->nEndTime;
        Record.byState          = pInfo->byState;
        Record.byOriginalALevel = pInfo->byOriginalALevel;

        memset(Record.byReserved, 0, sizeof(Record.byReserved));

        g_pSO3GameCenter->m_MiscDB.DoSaveMentorRecordRequest(dwMentorID, dwApprenticeID, (BYTE*)&Record, sizeof(Record));

        pInfo->bSaved = true;
    }

    return;
}

void KMentorCenter::CheckStateEnd()
{
    DWORD               dwMentorID      = ERROR_ID;
    DWORD               dwApprenticeID  = ERROR_ID;
    KMentorInfo*        pMentorRecordDB = NULL;
    KRole*              pARole          = NULL;
    uint64_t            uMentorKey      = 0;

    for (KMentorTable::iterator it = m_A2MTable.begin(); it != m_A2MTable.end(); NULL)
    {
        uMentorKey      = it->first;
        pMentorRecordDB = &it->second;

        ++it;

        dwApprenticeID = HIGH_DWORD_IN_UINT64(uMentorKey);
        dwMentorID     = LOW_DWORD_IN_UINT64(uMentorKey);

        if (g_pSO3GameCenter->m_nTimeNow < pMentorRecordDB->nEndTime)
            continue;

        pMentorRecordDB->bSaved = false;
        switch (pMentorRecordDB->byState)
        {
        case emrsNormal:
            Delete(dwMentorID, dwApprenticeID);
            
            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, 0, "Auto Break"
            );

            break;

        case emrsMBreak:
            Delete(dwMentorID, dwApprenticeID);

            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, 0, "Mentor Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);

            break;

        case emrsABreak:
            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);

            pARole = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);
            if (pARole == NULL || pARole->m_byLevel < LOWEST_ALEVEL_ACCQUIRE_MENTOR_VALUE)
            {
                Delete(dwMentorID, dwApprenticeID);

                g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                    dwMentorID, dwApprenticeID, 0, "Apprentice Break"
                );
            }
            else
            {
                pMentorRecordDB->byState      = emrsBroken;
                pMentorRecordDB->nEndTime     = g_pSO3GameCenter->m_nTimeNow + MENTOR_BROKEN_TIMEOUT;
                pMentorRecordDB->nMentorValue = (int)(pMentorRecordDB->nMentorValue * EVIL_ABREAK_MENTOR_VALUE_RATE);
            
                g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                    dwMentorID, dwApprenticeID, pMentorRecordDB->nMentorValue, "Apprentice Break"
                );

                if (pMentorRecordDB->nMentorValue == 0)
                {
                    Delete(dwMentorID, dwApprenticeID);
                }
                else
                {
                    g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pMentorRecordDB);
                }
            }
            break;

        case emrsBroken:
        case emrsGraduating:
        case emrsGraduated:
            Delete(dwMentorID, dwApprenticeID);
            break;

        default:
            assert(false);
        }
    }

    return;
}

void KMentorCenter::Clear()
{
    m_M2ATable.clear();
    m_A2MTable.clear();
    
    m_ApplyApprenticeTable.clear();
    m_ApplyMentorTable.clear();

    return;
}

BOOL KMentorCenter::AddMentorValue(DWORD dwMentorID, DWORD dwApprenticeID, int nDeltaValue)
{
    BOOL                        bResult = false;
    KMentorInfo*                pInfo   = NULL;

    pInfo = GetMentorInfo(dwMentorID, dwApprenticeID);
    KGLOG_PROCESS_ERROR(pInfo);

    KGLOG_PROCESS_ERROR(pInfo->byState > emrsInvalid && pInfo->byState < emrsTotal);
    KGLOG_PROCESS_ERROR(pInfo->byState != emrsBroken);

    pInfo->nMentorValue += nDeltaValue;
    pInfo->bSaved        = false;

    g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pInfo);

    bResult = true;
Exit0:
    return bResult;
}

int KMentorCenter::GetSyncMentorRecord(uint64_t uMentorKey, KMentorSyncInfo* pSyncRecord, int nMaxCount)
{
    int               nCount    = 0;
    BYTE              byState   = 0;
    KMentorInfo*      pInfo     = NULL;

    assert(pSyncRecord);

    for (
        KMentorTable::iterator it = m_A2MTable.upper_bound(uMentorKey); 
        it != m_A2MTable.end() && nCount < nMaxCount;
        ++it
    )
    {        
        pInfo = &it->second;

        pSyncRecord[nCount].dwMentor             = LOW_DWORD_IN_UINT64(it->first);
        pSyncRecord[nCount].dwApprentice         = HIGH_DWORD_IN_UINT64(it->first);
        pSyncRecord[nCount].nValue               = it->second.nMentorValue;
        pSyncRecord[nCount].byState              = it->second.byState;

        nCount++;
    }

    return nCount;
}

BOOL KMentorCenter::UpdateMaxApprenticeNum(DWORD dwMentorID, int nMaxApprenticeNum)
{
    m_MaxApprenticeNumTable[dwMentorID] = nMaxApprenticeNum;

    return true;
}

BOOL KMentorCenter::ClearMaxApprenticeNum(DWORD dwMentorID)
{
    m_MaxApprenticeNumTable.erase(dwMentorID);

    return true;
}

BOOL KMentorCenter::CreateShip(DWORD dwMentorID, DWORD dwApprenticeID, BYTE byALevel)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KMentorInfo             MentorInfo;

    MentorInfo.nCreateTime      = g_pSO3GameCenter->m_nTimeNow;
    MentorInfo.nMentorValue     = 0;
    MentorInfo.nEndTime         = g_pSO3GameCenter->m_nTimeNow + MENTOR_NORMAL_TIMEOUT;
    MentorInfo.byState          = emrsNormal;
    MentorInfo.byOriginalALevel = byALevel;
    MentorInfo.bSaved           = false;

    bRetCode = AddRecord(dwMentorID, dwApprenticeID, MentorInfo);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, MentorInfo);

    g_pSO3GameCenter->m_StatDataManager.UpdateMentorCreateStat(dwMentorID, dwApprenticeID);

    bResult = true;
Exit0:
    return bResult;
}

void KMentorCenter::ActiveShip()
{
    for (KMentorTable::iterator it = m_A2MTable.begin(); it != m_A2MTable.end(); ++it)
    {
        uint64_t        uMentorKey     = it->first;
        DWORD           dwApprenticeID = HIGH_DWORD_IN_UINT64(uMentorKey);
        DWORD           dwMentorID     = LOW_DWORD_IN_UINT64(uMentorKey);
        KMentorInfo*    pInfo          = &it->second;
        KRole*          pMRole         = NULL;
        KRole*          pARole         = NULL;
        BOOL            bMActivate     = false;
        BOOL            bAActivate     = false;

        if (pInfo->byState != emrsNormal)
            continue;

        pMRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwMentorID);

        pARole = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);

        if (pMRole && pMRole->m_nLastSaveTime > g_pSO3GameCenter->m_nTimeNow - 3600 * 24)
        {
            bMActivate = true;
        }

        if (pARole && pARole->m_nLastSaveTime > g_pSO3GameCenter->m_nTimeNow - 3600 * 24)
        {
            bAActivate = true;
        }

        if (bMActivate || bAActivate)
        {
            time_t nNewTime = g_pSO3GameCenter->m_nTimeNow + MENTOR_NORMAL_TIMEOUT;

            if (nNewTime - pInfo->nEndTime > 3600)
            {
                pInfo->nEndTime   = g_pSO3GameCenter->m_nTimeNow + MENTOR_NORMAL_TIMEOUT;
                pInfo->bSaved     = false;
            }
        }
    }
}

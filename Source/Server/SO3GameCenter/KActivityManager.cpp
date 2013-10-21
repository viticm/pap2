#include "stdafx.h"
#include "KActivityManager.h"
#include "KSO3GameCenter.h"

KActivityManager::KActivityManager()
{
    m_nNextSaveTime         = 0;
    m_nNextCheckTimeoutTime = 0;
}

KActivityManager::~KActivityManager()
{
}

void KActivityManager::Activate()
{
    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 5) == 0)
    {
        for (KActivityTable::iterator it = m_ActivityTable.begin(); it != m_ActivityTable.end(); NULL)
        {
            if (it->second.bUpdate)
            {
                g_pSO3GameCenter->m_MiscDB.DoUpdateActivityData(it->second);
                it->second.bUpdate = false;
            }

            if (g_pSO3GameCenter->m_nTimeNow > it->second.nEndTime)
            {
                g_pSO3GameCenter->m_MiscDB.DoDeleteActivityData(it->second.dwRoleID, it->second.nType);
                m_ActivityTable.erase(it++);

                continue;
            }

            ++it;
        }
    }
}

BOOL KActivityManager::Add(const KActivityRecord& rRecord)
{
    BOOL                                        bResult = false;
    uint64_t                                    uKey    = 0;
    KActivityTable::iterator                    it;
    std::pair<KActivityTable::iterator, bool>   InsRet;

    uKey = MAKE_INT64(rRecord.dwRoleID, rRecord.nType);

    it = m_ActivityTable.find(uKey);
    KGLOG_PROCESS_ERROR(it == m_ActivityTable.end());

    InsRet = m_ActivityTable.insert(std::make_pair(uKey, rRecord));
    KGLOG_PROCESS_ERROR(InsRet.second);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KActivityManager::Delete(DWORD dwRoleID, int nType)
{
    BOOL                        bResult     = false;
    BOOL                        bRetCode    = false;
    uint64_t                    uKey        = 0;
    KActivityTable::iterator    it;

    uKey = MAKE_INT64(dwRoleID, nType);

    it = m_ActivityTable.find(uKey);
    KGLOG_PROCESS_ERROR(it != m_ActivityTable.end());

    m_ActivityTable.erase(it);

    bRetCode = g_pSO3GameCenter->m_MiscDB.DoDeleteActivityData(dwRoleID, nType);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

KActivityRecord* KActivityManager::GetRecord(DWORD dwRoleID, int nType)
{
    KActivityRecord*            pResult     = NULL;
    uint64_t                    uKey        = 0;
    KActivityTable::iterator    it;

    uKey = MAKE_INT64(dwRoleID, nType);

    it = m_ActivityTable.find(uKey);
    KG_PROCESS_ERROR(it != m_ActivityTable.end());

    pResult = &(it->second);
Exit0:
    return pResult;
}

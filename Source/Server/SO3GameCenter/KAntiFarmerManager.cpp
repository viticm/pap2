#include "stdafx.h"
#include "Global.h"
#include "KSO3GameCenter.h"
#include "KAntiFarmerManager.h"

void KAntiFarmerManager::Clear()
{
    m_AntiFarmerTable.clear();
}

void KAntiFarmerManager::Activate()
{
    BOOL        bRetCode    = false;
    time_t      nTimeNow    = 0;
    const char* pszAccount  = NULL;
    KRole*      pRole       = NULL;

    KG_PROCESS_ERROR(g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 60 * 10) == 0);

    nTimeNow = g_pSO3GameCenter->m_nTimeNow;

    for (KAntiFarmerTable::iterator it = m_AntiFarmerTable.begin(); it != m_AntiFarmerTable.end(); NULL)
    {
        if (it->second > nTimeNow)
        {
            ++it;
            continue;
        }

        pszAccount = it->first.c_str();

        g_pSO3GameCenter->m_MiscDB.DoDeleteFarmerRecordRequest(pszAccount);

        pRole = g_pSO3GameCenter->m_RoleManager.GetOnlineRoleForAccount(pszAccount);
        if (pRole)
        {
            g_pSO3GameCenter->m_GameServer.DoSetFarmerLimitFlag(
                pRole->m_nConnIndex, pRole->m_dwPlayerID, false
            );
        }

        m_AntiFarmerTable.erase(it++);
    }

Exit0:
    return;
}

BOOL KAntiFarmerManager::AddFarmerRecord(const char cszAccount[], time_t nPunishEndTime)
{
    m_AntiFarmerTable[cszAccount] = nPunishEndTime;

    return true;
}

BOOL KAntiFarmerManager::CheckFarmerLimit(const char cszAccount[])
{
    BOOL                        bResult = false;
    KAntiFarmerTable::iterator  it      = m_AntiFarmerTable.find(cszAccount);

    KG_PROCESS_ERROR(g_pSO3GameCenter->m_Settings.m_bAntiFarmerEnable);
    KG_PROCESS_ERROR(it != m_AntiFarmerTable.end());
    KG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow < it->second);

    bResult = true;
Exit0:
    return bResult;
}

int KAntiFarmerManager::GetFarmerCount()
{
    return (int)m_AntiFarmerTable.size();
}



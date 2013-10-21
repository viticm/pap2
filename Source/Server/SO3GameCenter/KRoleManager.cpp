/************************************************************************/
/* 角色管理		                                                        */
/* Copyright : Kingsoft 2007										    */
/* Author	 : Zhao chunfeng										    */
/* History	 : 基于原来的PlayerData重构								    */
/*		2007.11.13	Create												*/
/************************************************************************/
#include "stdafx.h"
#include "KRoleManager.h"
#include "KSO3GameCenter.h"
#include "KRoleDBDataDef.h"

#define ACTIVE_PLAYER_TIME 7 * 24 * 3600

KRoleManager::KRoleManager()
{
}

BOOL KRoleManager::Init()
{
    // do nothing !
    return true;
}

void KRoleManager::UnInit()
{
    for (
        KACCOUNT_NAME_INDEX::iterator it = m_AccountNameIndex.begin(); 
        it != m_AccountNameIndex.end(); ++it
    )
    {
        KMemory::Delete(it->second);
    }
    m_AccountNameIndex.clear();
    m_RoleNameIndex.clear();
    m_RoleTable.clear();
}

void KRoleManager::Activate()
{
    for (KROLE_BLACK_LIST::iterator it = m_RoleBlackList.begin(); it != m_RoleBlackList.end(); NULL)
    {
        if (g_pSO3GameCenter->m_nTimeNow > it->second)
        {
            g_pSO3GameCenter->m_MiscDB.DoUpdateRoleBlackList(it->first, it->second);

            m_RoleBlackList.erase(it++);

            continue;
        }

        ++it;
    }


    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 30) == 0)
    {
        ProcessDeleteList();
    }

    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 3600) == 0)
    {
        CalculateCampActivePlayerCount();
    }
}

KAccount* KRoleManager::GetAccount(const char cszAccount[])
{
    KACCOUNT_NAME_INDEX::iterator it = m_AccountNameIndex.find(cszAccount);
    if (it != m_AccountNameIndex.end())
    {
        return it->second;
    }
    return NULL;
}

KRole* KRoleManager::AddRole(
    const char cszAccountName[], const char cszRoleName[], 
    DWORD dwID, BYTE byLevel,  BYTE byType, KCAMP eCamp, BYTE byForceID, time_t nLastSaveTime, time_t nCreateTime
)
{
    KRole*      pResult         = NULL;
    size_t      uNameLen        = 0;
    KAccount*   pAccount        = NULL;
    KRole*      pRole           = NULL;
    BOOL        bRoleInsert     = false;
    BOOL        bNameInsert     = false;
    BOOL        bNewAccount     = false;
    KACCOUNT_NAME_INDEX::iterator AccIter;
    pair<KROLE_TABLE::iterator, BOOL> RoleInsRet;
    pair<KROLE_NAME_INDEX::iterator, BOOL> NameIndexInsRet;

    RoleInsRet = m_RoleTable.insert(std::make_pair(dwID, KRole()));
    KGLOG_PROCESS_ERROR(RoleInsRet.second);
    bRoleInsert = true;

    pRole = &RoleInsRet.first->second;

    pRole->m_dwPlayerID     = dwID;
    pRole->m_byLevel        = byLevel;
    pRole->m_byType         = byType;
    pRole->m_eCamp          = eCamp;
    pRole->m_dwForceID      = byForceID;
    pRole->m_nLastSaveTime  = nLastSaveTime;
    pRole->m_nCreateTime    = nCreateTime;

    uNameLen = strlen(cszRoleName);
    KGLOG_PROCESS_ERROR(uNameLen < sizeof(pRole->m_szName));
    strcpy(pRole->m_szName, cszRoleName);

    NameIndexInsRet = m_RoleNameIndex.insert(std::make_pair(pRole->m_szName, pRole));
    KGLOG_PROCESS_ERROR(RoleInsRet.second);
    bNameInsert = true;

    AccIter = m_AccountNameIndex.find(cszAccountName);
    if (AccIter != m_AccountNameIndex.end())
    {
        pAccount = AccIter->second;
    }
    else
    {
        pair<KACCOUNT_NAME_INDEX::iterator, BOOL> AccInsRet;

        pAccount = KMemory::New<KAccount>();
        KGLOG_PROCESS_ERROR(pAccount);
        bNewAccount = true;

        uNameLen = strlen(cszAccountName);
        KGLOG_PROCESS_ERROR(uNameLen < sizeof(pAccount->szName));
        strcpy(pAccount->szName, cszAccountName);

        AccInsRet = m_AccountNameIndex.insert(std::make_pair(pAccount->szName, pAccount));
        KGLOG_PROCESS_ERROR(AccInsRet.second);        
    }

    pAccount->vRoleList.push_back(pRole);
    pRole->m_pszAccount = pAccount->szName;

    pResult = pRole;
Exit0:
    if (pResult == NULL)
    {
        AccIter = m_AccountNameIndex.find(cszAccountName);
        if (AccIter != m_AccountNameIndex.end())
        {
            int nRoleCount = 0;

            pAccount = AccIter->second;

            nRoleCount = (int)pAccount->vRoleList.size();

            for (int i = 0; i < nRoleCount; i++)
            {
                if (pAccount->vRoleList[i] == pRole)
                {
                    pAccount->vRoleList[i] = pAccount->vRoleList[nRoleCount - 1];
                    pAccount->vRoleList.resize(nRoleCount-1);
                    nRoleCount--;
                    break;
                }
            }
        }

        if (bNameInsert)
        {
            m_RoleNameIndex.erase(cszRoleName);
        }

        if (bRoleInsert)
        {
            m_RoleTable.erase(dwID);
        }

        if (bNewAccount)
        {
            KMemory::Delete(pAccount);
        }
    }
    return pResult;
}


KRole* KRoleManager::GetRole(DWORD dwID)
{
    KROLE_TABLE::iterator it = m_RoleTable.find(dwID);
    if (it != m_RoleTable.end())
    {
        return &it->second;
    }
    return NULL;
}

KRole* KRoleManager::GetRole(const char cszName[])
{
    KROLE_NAME_INDEX::iterator it = m_RoleNameIndex.find(cszName);
    if (it != m_RoleNameIndex.end())
    {
        return it->second;
    }
    return NULL;
}

KRole* KRoleManager::GetOnlineRoleForAccount(const char cszAccount[])
{
    KRole*      pResult     = NULL;
    KAccount*   pAccount    = NULL;
    KACCOUNT_NAME_INDEX::iterator AccIter = m_AccountNameIndex.find(cszAccount);

    KG_PROCESS_ERROR(AccIter != m_AccountNameIndex.end());
            
    pAccount = AccIter->second;

    for (KACC_ROLE_TAB::iterator it = pAccount->vRoleList.begin(); it != pAccount->vRoleList.end(); ++it)
    {
        KRole*      pRole   = *it;
        KROLE_STATE eState  = rsInvalid;

        assert(pRole);

        eState = pRole->GetRoleState();
        if (eState != rsOffline)
        {
            pResult = pRole;
            break;
        }
    }

Exit0:
    return pResult;
}

void KRoleManager::OnGSCrash(int nConnIndex)
{
	KROLE_TABLE::iterator it = m_RoleTable.begin();

	while (it != m_RoleTable.end())
	{
        KRole* pRole = &it->second;

		if (pRole->m_nConnIndex == nConnIndex || pRole->m_nDestConnIndex == nConnIndex)
        {
            pRole->OnLeaveGS(nConnIndex);
        }

		++it;
	}
}

BOOL KRoleManager::IsRoleBelongToAccount(DWORD dwRoleID, const char cszAccountName[])
{
    BOOL                            bResult     = false;
    KAccount*                       pAccount    = NULL;
    KACCOUNT_NAME_INDEX::iterator   AccIter     = m_AccountNameIndex.find(cszAccountName);
    KRole*                          pUserRole   = GetRole(dwRoleID);

    KGLOG_PROCESS_ERROR(AccIter != m_AccountNameIndex.end());
    KGLOG_PROCESS_ERROR(pUserRole != NULL);

    pAccount = AccIter->second;

    for (KACC_ROLE_TAB::iterator it = pAccount->vRoleList.begin(); it != pAccount->vRoleList.end(); ++it)
    {
        if (*it == pUserRole)
        {
            bResult = true; 
            break;
        }
    }

Exit0:
    return bResult;
}

BOOL KRoleManager::FreezeRole(DWORD dwRoleID, time_t nFreezeEndTime)
{
    m_RoleBlackList[dwRoleID] = nFreezeEndTime;

    return true;
}

time_t KRoleManager::GetRoleFreezeTime(DWORD dwRoleID)
{
    KROLE_BLACK_LIST::iterator  it = m_RoleBlackList.find(dwRoleID);

    if (it != m_RoleBlackList.end())
    {
        return it->second;
    }

    return 0;
}

void KRoleManager::RegisterDelRole(DWORD dwID, time_t nEndTime)
{
    m_RoleDeleteList[dwID] = nEndTime;
}

void KRoleManager::UnregisterDelRole(DWORD dwID)
{
    KROLE_DELETE_LIST::iterator it = m_RoleDeleteList.find(dwID);
    if (it != m_RoleDeleteList.end())
    {
        m_RoleDeleteList.erase(it);
        g_pSO3GameCenter->m_MiscDB.DoUpdateRoleDeleteList(dwID, 0);
    }
}

time_t KRoleManager::GetRoleDeleteTime(DWORD dwID)
{
    KROLE_DELETE_LIST::iterator it = m_RoleDeleteList.find(dwID);

    if (it != m_RoleDeleteList.end())
    {
        return it->second;
    }

    return 0;
}

void KRoleManager::ProcessDeleteList()
{
    for (KROLE_DELETE_LIST::iterator it = m_RoleDeleteList.begin(); it != m_RoleDeleteList.end(); NULL)
    {
        if (g_pSO3GameCenter->m_nTimeNow > it->second)
        {
            DelRole(it->first);

            g_pSO3GameCenter->m_MiscDB.DoUpdateRoleDeleteList(it->first, it->second);
            m_RoleDeleteList.erase(it++);

            continue;
        }

        ++it;
    }
}

BOOL KRoleManager::DelRole(DWORD dwID)
{
    BOOL                            bResult         = false;
    int                             nRetCode        = false;
    KAccount*                       pAccount        = NULL;
    KRole*                          pRole           = NULL;
    KTong*                          pTong           = NULL; 
    KROLE_STATE                     eState          = rsInvalid;
    int                             nRoleCount      = 0;
    time_t                          nFreezeTime     = 0;
    KROLE_TABLE::iterator           RoleIter;
    KACCOUNT_NAME_INDEX::iterator   AccIter;
    KROLE_NAME_INDEX::iterator      NameIter;

    RoleIter = m_RoleTable.find(dwID);
    KGLOG_PROCESS_ERROR(RoleIter != m_RoleTable.end());

    pRole = &RoleIter->second;

    KGLOG_PROCESS_ERROR(pRole->GetRoleState() == rsOffline);

    // 冻结中的角色不允许删除
    nFreezeTime = g_pSO3GameCenter->m_RoleManager.GetRoleFreezeTime(dwID);
    KG_PROCESS_ERROR(g_pSO3GameCenter->m_nTimeNow >= nFreezeTime);

    // 从帮会中移除成员，如果是帮主会操作失败
    if (pRole->m_dwTongID != ERROR_ID)
    {
        pTong = g_pSO3GameCenter->m_TongManager.GetTong(pRole->m_dwTongID);
        KGLOG_PROCESS_ERROR(pTong);

        nRetCode = pTong->RemoveMember(pRole);
        KG_PROCESS_ERROR(nRetCode);
    }

    // 如果玩家属于某个队伍，要先从队伍删除
    if (pRole->m_dwTeamID != ERROR_ID)
    {
        nRetCode = g_pSO3GameCenter->m_TeamCenter.DelMember(
            pRole->m_dwTeamID, pRole->m_dwPlayerID
        );
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    NameIter = m_RoleNameIndex.find(pRole->m_szName);
    KGLOG_PROCESS_ERROR(NameIter != m_RoleNameIndex.end());

    AccIter = m_AccountNameIndex.find(pRole->m_pszAccount);
    KGLOG_PROCESS_ERROR(AccIter != m_AccountNameIndex.end());

    pRole->m_pszAccount = NULL;
    pAccount = AccIter->second;

    nRoleCount = (int)pAccount->vRoleList.size();
    for (int i = 0; i < nRoleCount; i++)
    {
        if (pAccount->vRoleList[i] == pRole)
        {
            pAccount->vRoleList[i] = pAccount->vRoleList[nRoleCount - 1];
            pAccount->vRoleList.resize(nRoleCount - 1);
            nRoleCount--;
            break;
        }
    }

    if (nRoleCount == 0)
    {
        KMemory::Delete(pAccount);
        pAccount = NULL;
        m_AccountNameIndex.erase(AccIter);
    }

    m_RoleNameIndex.erase(NameIter);
    m_RoleTable.erase(RoleIter);

    g_pSO3GameCenter->m_RoleDB.DoDeleteRole(pRole->m_dwPlayerID);

    g_pSO3GameCenter->m_MailManager.PostRequestDeleteAll(pRole->m_dwPlayerID);

    bResult = true;
Exit0:
    return bResult;
}

struct KCalculateCampActivePlayerCount
{
    BOOL operator () (DWORD dwID, KRole& rRole)
    {
        int nLevel = 0;

        if (g_pSO3GameCenter->m_nTimeNow - rRole.m_nLastSaveTime > ACTIVE_PLAYER_TIME)
        {
            return true;
        }

        assert(rRole.m_eCamp == cNeutral || rRole.m_eCamp == cEvil || rRole.m_eCamp == cGood);

        ++m_nActivePlayerCount[rRole.m_eCamp];

        return true;
    }

    int m_nActivePlayerCount[cTotal];
};

void KRoleManager::CalculateCampActivePlayerCount()
{
    KCalculateCampActivePlayerCount Calculate;

    memset(Calculate.m_nActivePlayerCount, 0, sizeof(Calculate.m_nActivePlayerCount));

    Traverse(Calculate);
    
    memcpy(
        g_pSO3GameCenter->m_CampManager.m_nActivePlayerCount, 
        Calculate.m_nActivePlayerCount, 
        sizeof(g_pSO3GameCenter->m_CampManager.m_nActivePlayerCount)
    );

    g_pSO3GameCenter->m_GameServer.DoSyncCampInfo(-1);
}

BOOL KRoleManager::RoleRename(DWORD dwID, char szNewName[])
{
    BOOL                                    bResult = false;
    KRole*                                  pRole   = NULL;
    KROLE_NAME_INDEX::iterator              itFind;
    KROLE_NAME_INDEX::iterator              itFindNew;
    pair<KROLE_NAME_INDEX::iterator, BOOL>  NameIndexInsRet;

    pRole = GetRole(dwID);
    KGLOG_PROCESS_ERROR(pRole);

    itFind = m_RoleNameIndex.find(pRole->m_szName);
    KGLOG_PROCESS_ERROR(itFind != m_RoleNameIndex.end());

    itFindNew = m_RoleNameIndex.find(szNewName);
    KGLOG_PROCESS_ERROR(itFindNew == m_RoleNameIndex.end());

    m_RoleNameIndex.erase(itFind);

    strncpy(pRole->m_szName, szNewName, sizeof(pRole->m_szName));
    pRole->m_szName[sizeof(pRole->m_szName) - 1] = '\0';

    NameIndexInsRet = m_RoleNameIndex.insert(std::make_pair(pRole->m_szName, pRole));
    KGLOG_PROCESS_ERROR(NameIndexInsRet.second);

    bResult = true;
Exit0:
    return bResult;
}

void KRoleManager::RecordRenameRole(DWORD dwRoleID)
{
    m_RenameRoleTable.insert(dwRoleID);
}

BOOL KRoleManager::IsCanRename(DWORD dwRoleID)
{
    BOOL                            bResult     = false;
    KRENAME_ROLE_TABLE::iterator    itFind;

    itFind = m_RenameRoleTable.find(dwRoleID);
    KG_PROCESS_ERROR(itFind != m_RenameRoleTable.end());

    bResult = true;
Exit0:
    return bResult;
}

void KRoleManager::RemoveRenameRole(DWORD dwRoleID)
{
    KRENAME_ROLE_TABLE::iterator    itFind;

    itFind = m_RenameRoleTable.find(dwRoleID);
    KGLOG_PROCESS_ERROR(itFind != m_RenameRoleTable.end());

    m_RenameRoleTable.erase(itFind);

Exit0:
    return;
}

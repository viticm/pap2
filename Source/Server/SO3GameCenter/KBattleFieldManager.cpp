#include "stdafx.h"
#include "KGPublic.h"
#include "KSO3GameCenter.h"
#include "KTeamCenter.h"
#include "KBattleFieldManager.h"

#define BATTLE_FIELD_SETTING "BattleField.tab"

KBattleFieldManager::KBattleFieldManager()
{
}

BOOL KBattleFieldManager::Init()
{
    BOOL bResult    = false;
    int  nRetCode   = false;
    char szFileName[MAX_PATH];

    nRetCode = snprintf(szFileName, sizeof(szFileName), "%s/%s", SETTING_DIR, BATTLE_FIELD_SETTING);
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode <= (int)sizeof(szFileName));

    nRetCode = LoadBattleFieldTable(szFileName);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = CreateQueue();
    KGLOG_PROCESS_ERROR(nRetCode);

    m_dwCreatingMapID       = ERROR_ID;
    m_nCreatingMapCopyIndex = 0;

    bResult = true;
Exit0:
    return bResult;
}

void KBattleFieldManager::UnInit()
{
    m_BaseInfoTable.clear();
    m_QueueTable.clear();
    m_BattleFieldTable.clear();
}

void KBattleFieldManager::Activate()
{
    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 2) == 0)
    {
        for (KQUEUE_TABLE::iterator it = m_QueueTable.begin(); it != m_QueueTable.end(); ++it)
        {
            ProcessQueue(&it->second);
        }
    }

    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 7) == 0)
    {
        for (KBATTLE_FIELD_TABLE::iterator it = m_BattleFieldTable.begin(); it != m_BattleFieldTable.end(); ++it)
        {
            ProcessBattleFieldCopyTable(it->first, &it->second);

            ProcessNewBattleField(it->first);
        }
    }

    if (g_pSO3GameCenter->m_nWorkLoop % (GAME_FPS * 3) == 0)
    {
        ProcessBlackList();
    }
}

BOOL KBattleFieldManager::IsBattleField(DWORD dwMapID)
{
    KBASE_INFO_TABLE::iterator  itFind = m_BaseInfoTable.find(dwMapID);
    if (itFind != m_BaseInfoTable.end())
    {
        return true;
    }

    return false;
}

BATTLE_FIELD_RESULT_CODE KBattleFieldManager::JoinQueue(DWORD dwRoleID, DWORD dwMapID, int nCopyIndex, DWORD dwGroupID, BOOL bTeamJoin, DWORD dwJoinValue)
{
    BATTLE_FIELD_RESULT_CODE    nResult     = bfrcFailed;
    BATTLE_FIELD_RESULT_CODE    nRetCode    = bfrcFailed;
    KQUEUE*                     pQueue      = NULL;
    KBASE_INFO*                 pBaseInfo   = NULL;
    KQUEUE_TABLE::iterator      itFind;
    KQUEUE_NODE                 QueueNode;

    itFind = m_QueueTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_QueueTable.end());

    pQueue = &itFind->second;
    assert(pQueue);

    QueueNode.nDstCopyIndex = nCopyIndex;

    pBaseInfo = GetBattleFieldBaseInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pBaseInfo);

    if (bTeamJoin)
    {
        const KTeam*    pcTeam          = NULL;
        const KRole*    pcRole          = NULL;
        int             nCountMember    = 0;

        pcRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
        KGLOG_PROCESS_ERROR(pcRole);

        pcTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(pcRole->m_dwTeamID);
        KGLOG_PROCESS_ERROR(pcTeam);

        KGLOG_PROCESS_ERROR(dwRoleID == pcTeam->dwAuthority[tatLeader]);

        nCountMember = g_pSO3GameCenter->m_TeamCenter.GetTeamMemberCount(pcRole->m_dwTeamID);

        KGLOG_PROCESS_ERROR_RET_CODE(nCountMember <= pBaseInfo->nMaxPlayerPerSide, bfrcTeamSizeError);

        for (int i = 0; i < pcTeam->nGroupNum; i++)
        {
            for (
                KTEAM_MEMBER_LIST::const_iterator it = pcTeam->MemberGroup[i].MemberList.begin();
                it != pcTeam->MemberGroup[i].MemberList.end(); ++it
            )
            {
                nRetCode = CanJoinQueue(it->dwMemberID, pQueue, dwGroupID);

                if (nRetCode != bfrcSuccess)
                {
                    nResult = nRetCode;
                    if (it->dwMemberID != dwRoleID)
                    {
                        nResult = bfrcTeamMemberError;
                    }
                    goto Exit0;
                }
            }
        }

        for (int i = 0; i < pcTeam->nGroupNum; i++)
        {
            for (
                KTEAM_MEMBER_LIST::const_iterator it = pcTeam->MemberGroup[i].MemberList.begin();
                it != pcTeam->MemberGroup[i].MemberList.end(); ++it
            )
            {
                KROLE_INFO RoleInfo;

                RoleInfo.dwRoleID           = it->dwMemberID;
                RoleInfo.dwJoinValue        = dwJoinValue;
                RoleInfo.nLastActiveTime    = g_pSO3GameCenter->m_nTimeNow;
                RoleInfo.nJoinTime          = g_pSO3GameCenter->m_nTimeNow;

                LeaveQueue(it->dwMemberID, dwMapID);
                QueueNode.RoleList.push_back(RoleInfo);
            }
        }
    }
    else
    {
        KROLE_INFO RoleInfo;

        RoleInfo.dwRoleID           = dwRoleID;
        RoleInfo.dwJoinValue        = dwJoinValue;
        RoleInfo.nLastActiveTime    = g_pSO3GameCenter->m_nTimeNow;
        RoleInfo.nJoinTime          = g_pSO3GameCenter->m_nTimeNow;

        nRetCode = CanJoinQueue(dwRoleID, pQueue, dwGroupID);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == bfrcSuccess, nRetCode);

        LeaveQueue(dwRoleID, dwMapID);

        QueueNode.RoleList.push_back(RoleInfo);
    }

    pQueue->QueueList[dwGroupID].push_back(QueueNode);

    nResult = bfrcSuccess;
Exit0:
    return nResult;
}

BOOL KBattleFieldManager::LeaveQueue(DWORD dwRoleID, DWORD dwMapID)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    KQUEUE*                         pQueue              = NULL;
    KBATTLE_FIELD_COPY_TABLE*       pBattleFieldCopy    = NULL;
    KQUEUE_TABLE::iterator          itFind;
    KBATTLE_FIELD_TABLE::iterator   BattleFieldItFind;

    itFind = m_QueueTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_QueueTable.end());

    pQueue = &itFind->second;
    assert(pQueue);

    for (int i = 0; i < MAX_BATTLE_SIDE; i++)
    {
        for (KROLE_QUEUE::iterator it = pQueue->QueueList[i].begin(); it != pQueue->QueueList[i].end(); ++it)
        {
            for (KROLE_LIST::iterator ListIter = it->RoleList.begin(); ListIter != it->RoleList.end(); ++ListIter)
            {
                if (ListIter->dwRoleID == dwRoleID)
                {
                    ListIter->dwRoleID = ERROR_ID;

                    bResult = true;
                    goto Exit0;
                }
            }
        }
    }

    // 可能人已经在战场的Waiting表里
    BattleFieldItFind = m_BattleFieldTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(BattleFieldItFind != m_BattleFieldTable.end());

    pBattleFieldCopy = &BattleFieldItFind->second;

    for (KBATTLE_FIELD_COPY_TABLE::iterator it = pBattleFieldCopy->begin(); it != pBattleFieldCopy->end(); ++it)
    {
        for (KSOLDER_LIST::iterator ListIter = it->second.WaitingList.begin(); ListIter != it->second.WaitingList.end(); ++ListIter)
        {
            if (ListIter->dwRoleID == dwRoleID)
            {
                KRole* pRole = NULL;

                it->second.WaitingList.erase(ListIter);

                pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
                if (pRole && pRole->IsOnline())
                {
                    g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
                        pRole->m_nConnIndex, dwRoleID, 0, 0,
                        dwMapID, it->first, bfntLeaveBattleField
                    );
                }

                bResult = true;
                goto Exit0;
            }
        }
    }

Exit0:
    return bResult;
}

BOOL KBattleFieldManager::CheckPermission(DWORD dwRoleID, DWORD dwMapID, int nCopyIndex)
{
    BOOL                                bResult = false;
    KBATTLE_FIELD*                      pBattleField    = NULL;

    pBattleField = GetBattleField(dwMapID, nCopyIndex);
    KG_PROCESS_ERROR(pBattleField);

    for (KSOLDER_LIST::iterator it = pBattleField->WaitingList.begin(); it != pBattleField->WaitingList.end(); ++it)
    {
        if (it->dwRoleID == dwRoleID)
        {
            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

BOOL KBattleFieldManager::ConfirmEnter(DWORD dwRoleID, DWORD dwMapID, int nCopyIndex)
{
    BOOL                                bResult = false;
    KBATTLE_FIELD*                      pBattleField    = NULL;

    pBattleField = GetBattleField(dwMapID, nCopyIndex);
    KG_PROCESS_ERROR(pBattleField);

    for (KSOLDER_LIST::iterator it = pBattleField->WaitingList.begin(); it != pBattleField->WaitingList.end(); ++it)
    {
        if (it->dwRoleID == dwRoleID)
        {
            it->bConfirmEnter = true;

            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

BOOL KBattleFieldManager::GetBattleFieldList(DWORD dwRoleID, DWORD dwMapID)
{
    BOOL                            bResult     = false;
    KRole*                          pRole       = NULL;
    KBATTLE_FIELD_COPY_TABLE*       pCopyTable  = NULL;
    KBATTLE_FIELD_TABLE::iterator   itFind;

    itFind = m_BattleFieldTable.find(dwMapID);
    KG_PROCESS_ERROR(itFind != m_BattleFieldTable.end());

    pCopyTable = &itFind->second;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    for (KBATTLE_FIELD_COPY_TABLE::iterator it = pCopyTable->begin(); it != pCopyTable->end(); ++it)
    {
        g_pSO3GameCenter->m_GameServer.DoSyncBattleFieldList(pRole->m_nConnIndex, dwRoleID, dwMapID, it->first);
    }
    
    bResult = true;
Exit0:
    return bResult;
}

int KBattleFieldManager::GetBattleFieldSide(DWORD dwMapID, int nCopyIndex, DWORD dwRoleID)
{
    int            nResult      = INVALID_BATTLE_SIDE;
    KBATTLE_FIELD* pBattleField = NULL;

    pBattleField = GetBattleField(dwMapID, nCopyIndex);
    KG_PROCESS_ERROR(pBattleField);

    for (KSOLDER_LIST::iterator it = pBattleField->WaitingList.begin(); it != pBattleField->WaitingList.end(); ++it)
    {
        if (it->dwRoleID == dwRoleID)
        {
            nResult = it->dwGroupID;
            goto Exit0;
        }
    }

    for (KSOLDER_LIST::iterator it = pBattleField->PlayingList.begin(); it != pBattleField->PlayingList.end(); ++it)
    {
        if (it->dwRoleID == dwRoleID)
        {
            nResult = it->dwGroupID;
            goto Exit0;
        }
    }

Exit0:
    return nResult;
}

DWORD KBattleFieldManager::GetBattleFieldJoinSign(DWORD dwMapID, int nCopyIndex, DWORD dwRoleID)
{
    DWORD          dwResult     = ERROR_ID;
    KBATTLE_FIELD* pBattleField = NULL;

    pBattleField = GetBattleField(dwMapID, nCopyIndex);
    KG_PROCESS_ERROR(pBattleField);

    for (KSOLDER_LIST::iterator it = pBattleField->WaitingList.begin(); it != pBattleField->WaitingList.end(); ++it)
    {
        if (it->dwRoleID == dwRoleID)
        {
            dwResult = it->dwJoinValue;
            goto Exit0;
        }
    }

    for (KSOLDER_LIST::iterator it = pBattleField->PlayingList.begin(); it != pBattleField->PlayingList.end(); ++it)
    {
        if (it->dwRoleID == dwRoleID)
        {
            dwResult = it->dwJoinValue;
            goto Exit0;
        }
    }

Exit0:
    return dwResult;
}

BOOL KBattleFieldManager::GetEnterBattlePos(int* pnRetX, int* pnRetY, int* pnRetZ, DWORD dwMapID, int nGroupID, DWORD dwJoinValue)
{
    BOOL        bResult        = false;
    BOOL        bRetCode       = false;
    int         nLuaTopIndex   = 0;
    KBASE_INFO* pBaseInfo      = NULL;

    assert(pnRetX);
    assert(pnRetY);
    assert(pnRetZ);

    pBaseInfo = GetBattleFieldBaseInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pBaseInfo);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsFuncExist(pBaseInfo->szScript, GET_ENTER_POS_FUNCTION);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_ScriptManager.SafeCallBegin(&nLuaTopIndex);

    g_pSO3GameCenter->m_ScriptManager.Push(dwMapID);
    g_pSO3GameCenter->m_ScriptManager.Push(nGroupID);
    g_pSO3GameCenter->m_ScriptManager.Push(dwJoinValue);

    g_pSO3GameCenter->m_ScriptManager.Call(pBaseInfo->szScript, GET_ENTER_POS_FUNCTION, 3);

    g_pSO3GameCenter->m_ScriptManager.GetValuesFromStack("ddd", pnRetX, pnRetY, pnRetZ);

    g_pSO3GameCenter->m_ScriptManager.SafeCallEnd(nLuaTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldManager::GetLeaveBattlePos(DWORD* pdwRetMapID, int* pnRetX, int* pnRetY, int* pnRetZ, DWORD dwMapID, int nGroupID, DWORD dwJoinValue)
{
    BOOL        bResult        = false;
    BOOL        bRetCode       = false;
    int         nLuaTopIndex   = 0;
    KBASE_INFO* pBaseInfo      = NULL;

    assert(pnRetX);
    assert(pnRetY);
    assert(pnRetZ);

    pBaseInfo = GetBattleFieldBaseInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pBaseInfo);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsFuncExist(pBaseInfo->szScript, GET_LEAVE_POS_FUNCTION);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_ScriptManager.SafeCallBegin(&nLuaTopIndex);

    g_pSO3GameCenter->m_ScriptManager.Push(dwMapID);
    g_pSO3GameCenter->m_ScriptManager.Push(nGroupID);
    g_pSO3GameCenter->m_ScriptManager.Push(dwJoinValue);

    g_pSO3GameCenter->m_ScriptManager.Call(pBaseInfo->szScript, GET_LEAVE_POS_FUNCTION, 4);

    g_pSO3GameCenter->m_ScriptManager.GetValuesFromStack("dddd", (int*)pdwRetMapID, pnRetX, pnRetY, pnRetZ);

    g_pSO3GameCenter->m_ScriptManager.SafeCallEnd(nLuaTopIndex);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldManager::AddInBlackList(DWORD dwRoleID, int nTime)
{
    BOOL                bResult = false;
    KRole*              pRole   = NULL;
    KBLACK_LIST_NODE    Node;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    if (pRole->IsOnline())
    {
        g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
            pRole->m_nConnIndex, dwRoleID, 0, nTime,
            0, 0, bfntInBlackList
        );
    }

    Node.dwRoleID   = dwRoleID;
    Node.nDelTime   = g_pSO3GameCenter->m_nTimeNow + nTime;

    m_BlackList.push_back(Node);

    bResult = true;
Exit0:
    return bResult;
}

void KBattleFieldManager::LeaveOutBlackList(DWORD dwRoleID)
{
    for (KBLACK_LIST::iterator it = m_BlackList.begin(); it != m_BlackList.end(); ++it)
    {
        if (it->dwRoleID == dwRoleID)
        {
            KRole* pRole = NULL;

            m_BlackList.erase(it);

            pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwRoleID);
            
            if (pRole && pRole->IsOnline())
            {
                g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
                    pRole->m_nConnIndex, it->dwRoleID, 0, 0,
                    0, 0, bfntLeaveBlackList
                );
            }
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////

KBattleFieldManager::KBASE_INFO* KBattleFieldManager::GetBattleFieldBaseInfo(DWORD dwMapID)
{
    KBASE_INFO*                 pResult = NULL;
    KBASE_INFO_TABLE::iterator  itFind;
    
    itFind = m_BaseInfoTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_BaseInfoTable.end());

    pResult = &itFind->second;
Exit0:
    return pResult;
}

KBattleFieldManager::KBATTLE_FIELD* KBattleFieldManager::GetBattleField(DWORD dwMapID, int nCopyIndex)
{
    KBATTLE_FIELD*                      pResult     = NULL;
    KBATTLE_FIELD_COPY_TABLE*           pCopyTable  = NULL;
    KBATTLE_FIELD_TABLE::iterator       itFind;
    KBATTLE_FIELD_COPY_TABLE::iterator  CopyItFind;

    itFind = m_BattleFieldTable.find(dwMapID);
    KG_PROCESS_ERROR(itFind != m_BattleFieldTable.end());

    pCopyTable = &itFind->second;

    CopyItFind = pCopyTable->find(nCopyIndex);
    KG_PROCESS_ERROR(CopyItFind != pCopyTable->end());

    pResult = &CopyItFind->second;
Exit0:
    return pResult;
}

BOOL KBattleFieldManager::LoadBattleFieldTable(const char cszFileName[])
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTemp       = 0;
    ITabFile*   piTabFile   = NULL;
    char        szTemp[MAX_PATH];

    assert(cszFileName);

    piTabFile = g_OpenTabFile(cszFileName);
    KGLOG_PROCESS_ERROR(piTabFile);

    KGLOG_PROCESS_ERROR(piTabFile->GetHeight() > 1);

    for (int nIndex = 2; nIndex <= piTabFile->GetHeight(); nIndex++)
    {
        KBASE_INFO  BaseInfo;

        bRetCode = piTabFile->GetInteger(nIndex, "MapID", 0, (int*)&BaseInfo.dwMapID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "MinLevelRequire", 0, &BaseInfo.nMinJoinLevel);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "MaxLevelRequire", 0, &BaseInfo.nMaxJoinLevel);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "MaxPlayerPerSide", 0, &BaseInfo.nMaxPlayerPerSide);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "MinPlayerPerSide", 0, &BaseInfo.nMinPlayerPerSide);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "EnableGroup0", 0, &nTemp);
        KGLOG_PROCESS_ERROR(bRetCode);
        BaseInfo.bEnableGroup[0] = (nTemp != 0);

        bRetCode = piTabFile->GetInteger(nIndex, "EnableGroup1", 0, &nTemp);
        KGLOG_PROCESS_ERROR(bRetCode);
        BaseInfo.bEnableGroup[1] = (nTemp != 0);

        bRetCode = piTabFile->GetInteger(nIndex, "EnableGroup2", 0, &nTemp);
        KGLOG_PROCESS_ERROR(bRetCode);
        BaseInfo.bEnableGroup[2] = (nTemp != 0);

        bRetCode = piTabFile->GetInteger(nIndex, "EnableGroup3", 0, &nTemp);
        KGLOG_PROCESS_ERROR(bRetCode);
        BaseInfo.bEnableGroup[3] = (nTemp != 0);

        bRetCode = piTabFile->GetInteger(nIndex, "ForceMask0", 0, (int*)&BaseInfo.dwForcMask[0]);
        KGLOG_PROCESS_ERROR(bRetCode);
        
        bRetCode = piTabFile->GetInteger(nIndex, "ForceMask1", 0, (int*)&BaseInfo.dwForcMask[1]);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "ForceMask2", 0, (int*)&BaseInfo.dwForcMask[2]);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "ForceMask3", 0, (int*)&BaseInfo.dwForcMask[3]);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "CampRequire0", 0, (int*)&BaseInfo.eCampRequire[0]);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "CampRequire1", 0, (int*)&BaseInfo.eCampRequire[1]);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "CampRequire2", 0, (int*)&BaseInfo.eCampRequire[2]);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "CampRequire3", 0, (int*)&BaseInfo.eCampRequire[3]);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetInteger(nIndex, "PQTemplateID", 0, (int*)&BaseInfo.dwPQTemplateID);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = piTabFile->GetString(nIndex, "Script", "", szTemp, sizeof(szTemp));
        KGLOG_PROCESS_ERROR(bRetCode);

        snprintf(BaseInfo.szScript, sizeof(BaseInfo.szScript), "%s/%s", CENTER_SCRIPTS_DIR, szTemp);
        BaseInfo.szScript[sizeof(BaseInfo.szScript) - 1] = '\0';

        m_BaseInfoTable.insert(make_pair(BaseInfo.dwMapID, BaseInfo));
    }

    bResult = true;
Exit0:
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

BOOL KBattleFieldManager::CreateQueue()
{
    for (KBASE_INFO_TABLE::iterator it = m_BaseInfoTable.begin(); it != m_BaseInfoTable.end(); ++it)
    {
        KQUEUE Queue;
        KBATTLE_FIELD_COPY_TABLE CopyTable;

        Queue.dwMapID       = it->first;
        Queue.nMinJoinLevel = it->second.nMinJoinLevel;
        Queue.nMaxJoinLevel = it->second.nMaxJoinLevel;

        for (int i = 0; i < MAX_BATTLE_SIDE; i++)
        {
            Queue.dwForcMask[i]     = it->second.dwForcMask[i];
            Queue.eCampRequire[i]   = it->second.eCampRequire[i];
            Queue.nAvgQueueTime[i]  = 0;
            Queue.QueueList[i].clear();
        }

        m_QueueTable.insert(make_pair(Queue.dwMapID, Queue));
        m_BattleFieldTable.insert(make_pair(it->first, CopyTable));
    }

    return true;
}

BOOL KBattleFieldManager::SearchRoleInQueue(
    KROLE_LIST* pSearchResult, int nCopyIndex, 
    KROLE_QUEUE::iterator itBegin, KROLE_QUEUE::iterator itEnd,
    int nMinRequire, int nMaxRequire
)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    for (KROLE_QUEUE::iterator it = itBegin; it != itEnd; ++it)
    {
        KROLE_QUEUE::iterator itNext             = it;
        int                   nActiveMemberCount = 0;

        itNext++;

        if (it->nDstCopyIndex != 0 && it->nDstCopyIndex != nCopyIndex)
            continue;

        // 统计当前队伍排队中的活跃人数
        for (KROLE_LIST::iterator IterList = it->RoleList.begin(); IterList != it->RoleList.end(); ++IterList)
        {
            KRole* pRole = NULL;

            pRole = g_pSO3GameCenter->m_RoleManager.GetRole(IterList->dwRoleID);
            if (pRole && pRole->IsOnline())
            {
                nActiveMemberCount++;
            }
        }

        if (nActiveMemberCount > nMaxRequire)
            continue;

        bRetCode = SearchRoleInQueue(
            pSearchResult, nCopyIndex, itNext, itEnd, 
            nMinRequire - nActiveMemberCount, nMaxRequire - nActiveMemberCount
        );
        if (bRetCode)
        {
            bResult = true;

            // 把活跃的人加入战场中
            for (KROLE_LIST::iterator IterList = it->RoleList.begin(); IterList != it->RoleList.end(); ++IterList)
            {
                KRole* pRole = NULL;

                pRole = g_pSO3GameCenter->m_RoleManager.GetRole(IterList->dwRoleID);
                if (pRole && pRole->IsOnline())
                {
                    pSearchResult->push_back(*IterList);
                }
            }

            goto Exit0;
        }
    }

    if (nMinRequire <= 0)
    {
        bResult = true;
    }

Exit0:
    return bResult;
}

BOOL KBattleFieldManager::GetCanJoinRole(
    KROLE_LIST* pRoleList, int nMinCount, int nMaxCount,
    DWORD dwMapID, int nCopyIndex, DWORD dwGroup
)
{
    BOOL                    bResult     = false;
    KQUEUE*                 pQueue      = NULL;
    KQUEUE_TABLE::iterator  itFind;

    assert(pRoleList);

    itFind = m_QueueTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_QueueTable.end());

    pQueue = &itFind->second;

    pRoleList->clear();

    SearchRoleInQueue(
        pRoleList, nCopyIndex, 
        pQueue->QueueList[dwGroup].begin(), pQueue->QueueList[dwGroup].end(),
        nMinCount, nMaxCount
    );

    bResult = true;
Exit0:
    return bResult;
}

void KBattleFieldManager::CalcAvgQueueTime(DWORD dwMapID, DWORD dwGroup, const KROLE_LIST& rRoleList)
{
    KQUEUE*                 pQueue          = NULL;
    int                     nJoinTimeSum    = 0;
    int                     nCountRole      = 0;
    KQUEUE_TABLE::iterator  itFind;

    itFind = m_QueueTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_QueueTable.end());

    pQueue = &(itFind->second);

    for (KROLE_LIST::const_iterator it = rRoleList.begin(); it != rRoleList.end(); ++it)
    {
        nJoinTimeSum += ((int)g_pSO3GameCenter->m_nTimeNow - (int)it->nJoinTime);
    }

    pQueue->nAvgQueueTime[dwGroup] += nJoinTimeSum / rRoleList.size();
    pQueue->nAvgQueueTime[dwGroup] /= 2;

Exit0:
    return;
}

BOOL KBattleFieldManager::CheckJoinTime(DWORD dwMapID)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    BOOL        bCheckRet       = false;
    int         nLuaTopIndex    = 0;
    KBASE_INFO* pBaseInfo       = NULL;

    pBaseInfo = GetBattleFieldBaseInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pBaseInfo);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsFuncExist(pBaseInfo->szScript, CHECK_JOIN_TIME_FUNCTION);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_ScriptManager.SafeCallBegin(&nLuaTopIndex);

    g_pSO3GameCenter->m_ScriptManager.Push((int)g_pSO3GameCenter->m_nTimeNow);

    g_pSO3GameCenter->m_ScriptManager.Call(pBaseInfo->szScript, CHECK_JOIN_TIME_FUNCTION, 1);

    g_pSO3GameCenter->m_ScriptManager.GetValuesFromStack("b", &bCheckRet);

    g_pSO3GameCenter->m_ScriptManager.SafeCallEnd(nLuaTopIndex);

    KG_PROCESS_ERROR(bCheckRet);

    bResult = true;
Exit0:
    return bResult;
}

BATTLE_FIELD_RESULT_CODE KBattleFieldManager::CanJoinQueue(DWORD dwRoleID, KQUEUE* pQueue, DWORD dwGroupID)
{
    BATTLE_FIELD_RESULT_CODE    nResult         = bfrcFailed;
    BOOL                        bRetCode        = false;
    KRole*                      pRole           = NULL;
    DWORD                       dwForceMask     = 0;
    int                         nJoinedQueue    = 0;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    bRetCode = CheckJoinTime(pQueue->dwMapID);
    KG_PROCESS_ERROR_RET_CODE(bRetCode, bfrcTimeError);

    bRetCode = IsInBlackList(dwRoleID);
    KG_PROCESS_ERROR_RET_CODE(!bRetCode, bfrcInBlackList);

    KG_PROCESS_ERROR_RET_CODE(
        pRole->m_byLevel >= pQueue->nMinJoinLevel && pRole->m_byLevel <= pQueue->nMaxJoinLevel, bfrcLevelError
    );

    KGLOG_PROCESS_ERROR(pRole->m_dwForceID < sizeof(DWORD) * CHAR_BIT);
    KGLOG_PROCESS_ERROR(dwGroupID < MAX_BATTLE_SIDE);

    dwForceMask = (1 << pRole->m_dwForceID);
    KG_PROCESS_ERROR_RET_CODE(pQueue->dwForcMask[dwGroupID] & dwForceMask, bfrcForceError);

    if (pRole->m_eCamp != cNeutral)
    {
        KG_PROCESS_ERROR_RET_CODE(pQueue->eCampRequire[dwGroupID] == pRole->m_eCamp, bfrcCampError);
    }

    KG_PROCESS_ERROR_RET_CODE(pRole->m_dwMapID != pQueue->dwMapID, bfrcNotInSameMap);

    nJoinedQueue = CountRoleJoinedQueue(dwRoleID);
    KG_PROCESS_ERROR_RET_CODE(nJoinedQueue < MAX_QUEUE_PER_PLAYER, bfrcTooManyJoin);

    nResult = bfrcSuccess;
Exit0:
    return nResult;
}

BOOL KBattleFieldManager::IsRoleInQueue(KQUEUE* pQueue, DWORD dwRoleID)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    KQUEUE_TABLE::iterator  itFind;

    assert(pQueue);

    for (int i = 0; i < MAX_BATTLE_SIDE; i++)
    {
        for (KROLE_QUEUE::iterator it = pQueue->QueueList[i].begin(); it != pQueue->QueueList[i].end(); ++it)
        {
            for (KROLE_LIST::iterator ListIter = it->RoleList.begin(); ListIter != it->RoleList.end(); ++ListIter)
            {
                if (ListIter->dwRoleID == dwRoleID)
                {
                    bResult = true;
                    goto Exit0;
                }
            }
        }
    }

Exit0:
    return bResult;
}

BOOL KBattleFieldManager::CountRoleJoinedQueue(DWORD dwRoleID)
{
    BOOL bRetCode = false;
    int  nCount   = 0;

    for (KQUEUE_TABLE::iterator it = m_QueueTable.begin(); it != m_QueueTable.end(); ++it)
    {
        bRetCode = IsRoleInQueue(&it->second, dwRoleID);
        if (bRetCode)
        {
            nCount++;
        }
    }

    return nCount;
}

void KBattleFieldManager::ProcessQueue(KQUEUE* pQueue)
{
    for (int i = 0; i < MAX_BATTLE_SIDE; i++)
    {
        for (KROLE_QUEUE::iterator it = pQueue->QueueList[i].begin(); it != pQueue->QueueList[i].end();)
        {
            for (KROLE_LIST::iterator ListIter = it->RoleList.begin(); ListIter != it->RoleList.end();)
            {
                KRole* pRole = NULL;

                if (g_pSO3GameCenter->m_nTimeNow - ListIter->nLastActiveTime > TIME_OUT_QUEUE_OFF_LINE)
                {
                    ListIter->dwRoleID = ERROR_ID;
                }

                pRole = g_pSO3GameCenter->m_RoleManager.GetRole(ListIter->dwRoleID);
                if (pRole == NULL)
                {
                    it->RoleList.erase(ListIter++);
                    continue;
                }

                if (pRole->IsOnline())
                {
                    g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
                        pRole->m_nConnIndex, ListIter->dwRoleID, pQueue->nAvgQueueTime[i],
                        g_pSO3GameCenter->m_nTimeNow - ListIter->nJoinTime,
                        pQueue->dwMapID, it->nDstCopyIndex, bfntQueueInfo
                    );

                    ListIter->nLastActiveTime = g_pSO3GameCenter->m_nTimeNow;
                }

                ++ListIter;
            }

            if (it->RoleList.empty())
            {
                pQueue->QueueList[i].erase(it++);
                continue;
            }

            ++it;
        }
    }
}

DWORD KBattleFieldManager::CreateBattleFieldPQ(DWORD dwMapID, int nCopyIndex)
{
    KBASE_INFO* pBaseInfo   = NULL;
    DWORD       dwPQID      = ERROR_ID;                

    pBaseInfo = GetBattleFieldBaseInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pBaseInfo);

    if (pBaseInfo->dwPQTemplateID)
    {
        dwPQID = g_pSO3GameCenter->m_PQCenter.CreatePQ(
            pBaseInfo->dwPQTemplateID, 
            pqotBattleField, dwMapID, nCopyIndex, 0
        );
        KGLOG_PROCESS_ERROR(dwPQID != ERROR_ID);
    }

Exit0:
    return dwPQID;
}

void KBattleFieldManager::DestroyBattleFieldPQ(DWORD dwMapID, int nCopyIndex)
{
    KBATTLE_FIELD* pBattleField = NULL;

    pBattleField = GetBattleField(dwMapID, nCopyIndex);
    KGLOG_PROCESS_ERROR(pBattleField);

    g_pSO3GameCenter->m_PQCenter.DeletePQ(pBattleField->dwPQID);
Exit0:
    return;
}

void KBattleFieldManager::ProcessBattleFieldCopyTable(DWORD dwMapID, KBATTLE_FIELD_COPY_TABLE* pBattleFieldCopyTable)
{
    KMapCopy* pMapCopy = NULL;

    if (dwMapID == m_dwCreatingMapID)
    {
        pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(m_dwCreatingMapID, m_nCreatingMapCopyIndex);
        if (pMapCopy != NULL && pMapCopy->m_eState == eMapStateNormal)
        {
            KBASE_INFO*   pBaseinfo   = NULL;
            KBATTLE_FIELD BattleField;

            pBaseinfo = GetBattleFieldBaseInfo(m_dwCreatingMapID);

            BattleField.dwMapID             = m_dwCreatingMapID;
            BattleField.nCopyIndex          = m_nCreatingMapCopyIndex;
            BattleField.nMaxPlayerPerSide   = pBaseinfo->nMaxPlayerPerSide;
            BattleField.nMinPlayerPerSide   = pBaseinfo->nMinPlayerPerSide;
            BattleField.PlayingList.clear();
            BattleField.WaitingList.clear();

            BattleField.dwPQID = CreateBattleFieldPQ(m_dwCreatingMapID, m_nCreatingMapCopyIndex);

            memset(BattleField.dwBattleTeam, 0, sizeof(BattleField.dwBattleTeam));
            for (int nSide = 0; nSide < MAX_BATTLE_SIDE; nSide++)
            {
                if (pBaseinfo->bEnableGroup[nSide])
                {
                    KTeam* pTeam = NULL;

                    BattleField.dwBattleTeam[nSide] = g_pSO3GameCenter->m_TeamCenter.CreateTeam(true);
                    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(BattleField.dwBattleTeam[nSide]);
                    if (!pTeam)
                        continue;

                    pTeam->nGroupNum = MAX_TEAM_GROUP_NUM;
                    g_pSO3GameCenter->m_GameServer.DoTeamLevelUpRaidNotify(
                        BattleField.dwBattleTeam[nSide], MAX_TEAM_GROUP_NUM
                    );
                }
            }

            pBattleFieldCopyTable->insert(make_pair(m_nCreatingMapCopyIndex, BattleField));
        }

        if (pMapCopy == NULL || pMapCopy->m_eState != eMapStateCreating)
        {
            m_dwCreatingMapID       = 0;
            m_nCreatingMapCopyIndex = 0;
        }
    }

    for (KBATTLE_FIELD_COPY_TABLE::iterator it = pBattleFieldCopyTable->begin(); it != pBattleFieldCopyTable->end();)
    {
        pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(dwMapID, it->first);
        if (pMapCopy == NULL)
        {
            DestroyBattleFieldPQ(dwMapID, it->first);

            for (int nSide = 0; nSide < MAX_BATTLE_SIDE; nSide++)
            {
                if (it->second.dwBattleTeam[nSide] != ERROR_ID)
                {
                    g_pSO3GameCenter->m_TeamCenter.DestroyTeam(it->second.dwBattleTeam[nSide]);
                }
            }

            pBattleFieldCopyTable->erase(it++);
            continue;
        }
    
        if (pMapCopy->m_eState == eMapStateNormal)
        {
            ProcessBattleField(&it->second);
        }

        ++it;
    }
}

void KBattleFieldManager::ProcessBattleField(KBATTLE_FIELD* pBattleField)
{
    BOOL        bRetCode                        = false;
    KBASE_INFO* pBaseInfo                       = NULL;
    int         nCountPlayer[MAX_BATTLE_SIDE];

    memset(nCountPlayer, 0, sizeof(nCountPlayer));

    pBaseInfo = GetBattleFieldBaseInfo(pBattleField->dwMapID);
    KGLOG_PROCESS_ERROR(pBaseInfo);

    for (KSOLDER_LIST::iterator it = pBattleField->PlayingList.begin(); it != pBattleField->PlayingList.end();)
    {
        KRole* pRole    = NULL;

        assert(it->dwGroupID < MAX_BATTLE_SIDE);

        nCountPlayer[it->dwGroupID]++;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwRoleID);
        if (pRole == NULL)
        {
            pBattleField->PlayingList.erase(it++);
            continue;
        }

        if (pRole->GetRoleState() == rsOffline)
        {
            KSOLDER SolderInfo = *it;

            SolderInfo.nJoinTime        = g_pSO3GameCenter->m_nTimeNow;
            SolderInfo.nLastOnlineTime  = g_pSO3GameCenter->m_nTimeNow;

            pBattleField->WaitingList.push_back(SolderInfo);
            pBattleField->PlayingList.erase(it++);
            continue;
        }

        if (pRole->m_dwMapID != pBattleField->dwMapID || pRole->m_nMapCopyIndex != pBattleField->nCopyIndex)
        {
            KGPQ*   pPQ = NULL;

            LeaveBattleTeam(pRole->m_dwPlayerID, pBattleField->dwBattleTeam[it->dwGroupID]);

            pBattleField->PlayingList.erase(it++);

            pPQ = g_pSO3GameCenter->m_PQCenter.GetPQ(pBattleField->dwPQID);
            KGLOG_PROCESS_ERROR(pPQ);

            if (!pPQ->m_bFinished)
            {
                for (KBASE_INFO_TABLE::iterator ListIter = m_BaseInfoTable.begin(); ListIter != m_BaseInfoTable.end(); ++ListIter)
                {
                    LeaveQueue(pRole->m_dwPlayerID, ListIter->second.dwMapID);
                }

                AddInBlackList(pRole->m_dwPlayerID, MAX_BLACKLIST_TIME);
            }

            continue;
        }

        it->nLastOnlineTime = g_pSO3GameCenter->m_nTimeNow;

        ++it;
    }

    for (int i = 0; i < MAX_BATTLE_SIDE; i++)
    {
        if (nCountPlayer[i] > pBattleField->nMaxPlayerPerSide)
        {
            KGLogPrintf(KGLOG_ERR, "[BATTLEFIELD] BattleField:%lu, Side:%n, member error.\n", pBattleField->dwMapID, i);
        }
    }

    for (KSOLDER_LIST::iterator it = pBattleField->WaitingList.begin(); it != pBattleField->WaitingList.end(); NULL)
    {
        KRole* pRole    = NULL;

        assert(it->dwGroupID < MAX_BATTLE_SIDE);

        nCountPlayer[it->dwGroupID]++;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwRoleID);
        if (pRole == NULL)
        {
            pBattleField->WaitingList.erase(it++);
            continue;
        }

        if (pRole->GetRoleState() == rsOffline)
        {
            if (g_pSO3GameCenter->m_nTimeNow - it->nLastOnlineTime > TIME_OUT_OFF_LINE)
            {
                LeaveBattleTeam(pRole->m_dwPlayerID, pBattleField->dwBattleTeam[it->dwGroupID]);
                pBattleField->WaitingList.erase(it++);
            }
            else
            {
                ++it;
            }
            continue;
        }

        it->nLastOnlineTime = g_pSO3GameCenter->m_nTimeNow;

        if (pRole->m_dwMapID == pBattleField->dwMapID && pRole->m_nMapCopyIndex == pBattleField->nCopyIndex)
        {
            KSOLDER SolderInfo = *it;

            SolderInfo.nJoinTime        = g_pSO3GameCenter->m_nTimeNow;
            SolderInfo.nLastOnlineTime  = g_pSO3GameCenter->m_nTimeNow;

            JoinBattleTeam(it->dwRoleID, pBattleField->dwBattleTeam[it->dwGroupID]);

            pBattleField->PlayingList.push_back(SolderInfo);
            pBattleField->WaitingList.erase(it++);
            continue;
        }

        if (it->bConfirmEnter) // 一旦用户点击了确认进入，则以下操作将不再进行。
        {
            ++it;
            continue;
        }

        if (pRole->GetRoleState() == rsOnline && g_pSO3GameCenter->m_nTimeNow - it->nJoinTime > TIME_OUT_WAIT_BATTLE_FIELD)
        {
            g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
                pRole->m_nConnIndex, it->dwRoleID, 0, 0,
                pBattleField->dwMapID, pBattleField->nCopyIndex, bfntLeaveBattleField
            );
            pBattleField->WaitingList.erase(it++);
            continue;
        }

        g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
            pRole->m_nConnIndex, it->dwRoleID, 0, 
            g_pSO3GameCenter->m_nTimeNow - it->nJoinTime,
            pBattleField->dwMapID, pBattleField->nCopyIndex, bfntJoinBattleField
        );

        ++it;
    }

    for (int nGroupID = 0; nGroupID < MAX_BATTLE_SIDE; nGroupID++)
    {
        KROLE_LIST  RoleList;
        int         nGetPlayerCount = pBattleField->nMaxPlayerPerSide - nCountPlayer[nGroupID];
        int         nMinGetCount    = max(0, pBattleField->nMinPlayerPerSide - nCountPlayer[nGroupID]);

        if (nGetPlayerCount <= 0)
            continue;

        assert(nGetPlayerCount <= pBattleField->nMaxPlayerPerSide);

        if (!pBaseInfo->bEnableGroup[nGroupID])
            continue;

        bRetCode = GetCanJoinRole(
            &RoleList, nMinGetCount, nGetPlayerCount,
            pBattleField->dwMapID, pBattleField->nCopyIndex, nGroupID
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        if (RoleList.size() > 0)
        {
            CalcAvgQueueTime(pBattleField->dwMapID, nGroupID, RoleList);
        }

        if ((int)RoleList.size() > nGetPlayerCount)
        {
            KGLogPrintf(KGLOG_ERR, "[BATTLEFIELD] Battle:%lu, GetRole Error.\n", pBattleField->dwMapID);
            continue;
        }

        for (KROLE_LIST::iterator it = RoleList.begin(); it != RoleList.end(); ++it)
        {
            KSOLDER Solder;
            KRole*  pRole = NULL;

            LeaveQueue(it->dwRoleID, pBattleField->dwMapID);

            Solder.dwRoleID         = it->dwRoleID;
            Solder.nJoinTime        = g_pSO3GameCenter->m_nTimeNow;
            Solder.nLastOnlineTime  = g_pSO3GameCenter->m_nTimeNow;
            Solder.dwGroupID        = nGroupID;
            Solder.dwJoinValue      = it->dwJoinValue;
            Solder.bConfirmEnter    = false;

            pBattleField->WaitingList.push_back(Solder);

            pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwRoleID);
            if (pRole != NULL && pRole->IsOnline())
            {
                g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
                    pRole->m_nConnIndex, pRole->m_dwPlayerID, 0, 
                    g_pSO3GameCenter->m_nTimeNow - Solder.nJoinTime,
                    pBattleField->dwMapID, pBattleField->nCopyIndex, bfntJoinBattleField
                );
            }
        }
    }

Exit0:
    return;
}

void KBattleFieldManager::ProcessNewBattleField(DWORD dwMapID)
{
    BOOL                        bResult         = false;
    BOOL                        bRetCode        = false;
    KBASE_INFO*                 pBase           = NULL;
    KMapInfo*                   pMapInfo        = NULL;
    KMapCopy*                   pMapCopy        = NULL;
    int                         nDestConnIndex  = 0;
    int                         nMapCopyIndex   = 0;

    pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(m_dwCreatingMapID, m_nCreatingMapCopyIndex);
    if (pMapCopy && pMapCopy->m_eState == eMapStateCreating)
    {
        goto Exit0;
    }

    pBase = GetBattleFieldBaseInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pBase);

    for (int i = 0; i < MAX_BATTLE_SIDE; i++)
    {
        KROLE_LIST RoleList;

        if (!pBase->bEnableGroup[i])
            continue;

        bRetCode = GetCanJoinRole(&RoleList, pBase->nMinPlayerPerSide, pBase->nMaxPlayerPerSide, dwMapID, 0, i);
        KGLOG_PROCESS_ERROR(bRetCode);

        KG_PROCESS_ERROR((int)RoleList.size() >= pBase->nMinPlayerPerSide);
    }

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    bRetCode = g_pSO3GameCenter->m_MapManager.IsMapCopyCountLimit(pMapInfo);
    KG_PROCESS_ERROR(!bRetCode);

    nDestConnIndex = g_pSO3GameCenter->m_GameServer.GetBestGSConnectionForCreateMap(pMapInfo);
    KG_PROCESS_ERROR(nDestConnIndex);

    nMapCopyIndex = pMapInfo->m_nSNGenerator++;

    pMapCopy = pMapInfo->CreateMapCopy(
        nMapCopyIndex, g_pSO3GameCenter->m_nTimeNow, 0, 
        nDestConnIndex, true
    );
    KGLOG_PROCESS_ERROR(pMapCopy);

    g_pSO3GameCenter->m_GameServer.DoCreateMapNotify(nDestConnIndex, dwMapID, nMapCopyIndex);

    pMapCopy->m_eState = eMapStateCreating;

    m_dwCreatingMapID       = dwMapID;
    m_nCreatingMapCopyIndex = nMapCopyIndex;

Exit0:
    return;
}

void KBattleFieldManager::ProcessBlackList()
{
    for (KBLACK_LIST::iterator it = m_BlackList.begin(); it != m_BlackList.end();)
    {
        KRole* pRole = NULL;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwRoleID);

        if (g_pSO3GameCenter->m_nTimeNow > it->nDelTime)
        {
            m_BlackList.erase(it++);

            if (pRole && pRole->IsOnline())
            {
                g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
                    pRole->m_nConnIndex, pRole->m_dwPlayerID, 0, 0,
                    0, 0, bfntLeaveBlackList
                );
            }

            continue;
        }

        if (pRole && pRole->IsOnline())
        {
            g_pSO3GameCenter->m_GameServer.DoBattleFieldNotify(
                pRole->m_nConnIndex, it->dwRoleID, 0,
                it->nDelTime -  g_pSO3GameCenter->m_nTimeNow,
                0, 0, bfntInBlackList
            );
        }

        ++it;
    }
}

BOOL KBattleFieldManager::IsInBlackList(DWORD dwRoleID)
{
    BOOL bResult = false;

    for (KBLACK_LIST::iterator it = m_BlackList.begin(); it != m_BlackList.end(); ++it)
    {
        if (it->dwRoleID == dwRoleID)
        {
            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

BOOL KBattleFieldManager::JoinBattleTeam(DWORD dwRoleID, DWORD dwTeamID)
{
    BOOL    bResult = false;
    KRole*  pRole   = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    if (pRole->m_dwSystemTeamID != ERROR_ID)
    {
        g_pSO3GameCenter->m_TeamCenter.DelMember(pRole->m_dwSystemTeamID, pRole->m_dwPlayerID);
    }

    g_pSO3GameCenter->m_TeamCenter.AddMember(dwTeamID, dwRoleID);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldManager::LeaveBattleTeam(DWORD dwRoleID, DWORD dwTeamID)
{
    BOOL    bResult = false;
    KRole*  pRole   = NULL;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    KG_PROCESS_ERROR(dwTeamID == pRole->m_dwSystemTeamID);

    g_pSO3GameCenter->m_TeamCenter.DelMember(dwTeamID, dwRoleID);

    bResult = true;
Exit0:
    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KBattleField.cpp
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2007-04-10 11:21:31
//  Comment     : 战场控制器相关代码实现
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KMapManager.h"
#include "KSO3GameCenter.h"
#include "KBattleField.h"
#include "KBattleFieldQueue.h"

KBattleFieldController::KBattleFieldController()
{
}

BOOL KBattleFieldController::Init()
{
    return true;
}

void KBattleFieldController::UnInit()
{
}

void KBattleFieldController::Activate()
{
    for (KBATTLE_FIELD_TABLE::iterator it = m_BattleFieldTable.begin(); it != m_BattleFieldTable.end(); ++it)
    {
        ProcessBattleFieldGroup(&it->second);
    }
}

BOOL KBattleFieldController::OnCreateBattleField(DWORD dwMapID, int64_t nCopyIndex)
{
    BOOL                                bResult     = false;
    KBattleFieldStateInfo*              pStateInfo  = NULL;
    KBattleField                        BattleField;
    KSTATE_INFO_TABLE::iterator         itFind;

    itFind = m_BattleFieldStateInfo.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_BattleFieldStateInfo.end());

    pStateInfo = &itFind->second;
    assert(pStateInfo);

    BattleField.dwMapID             = pStateInfo->dwMapID;
    BattleField.bActive             = false;
    BattleField.nCopyIndex          = nCopyIndex;
    BattleField.nMaxPlayerPerSide   = pStateInfo->nMaxPlayerPerSide;
    BattleField.PlayingList.clear();
    BattleField.WaitingList.clear();

    m_BattleFieldTable[dwMapID].insert(make_pair(nCopyIndex, BattleField));

    bResult = true;
Exit0:
    return bResult;
}


BOOL KBattleFieldController::OnDestroyBattleField(DWORD dwMapID, int64_t nCopyIndex)
{
    BOOL                                bResult     = false;
    KBATTLE_FIELD_COPY_TABLE*           pCopyTable  = NULL;
    KBATTLE_FIELD_TABLE::iterator       itFind;
    KBATTLE_FIELD_COPY_TABLE::iterator  itCopyFind;

    itFind = m_BattleFieldTable.find(dwMapID);
    KGLOG_PROCESS_ERROR(itFind != m_BattleFieldTable.end());

    pCopyTable = &itFind->second;
    assert(pCopyTable);

    itCopyFind = pCopyTable->find(nCopyIndex);
    KGLOG_PROCESS_ERROR(itCopyFind != pCopyTable->end());

    pCopyTable->erase(itCopyFind);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldController::CheckPermission(DWORD dwPlayerID, DWORD dwMapID, int64_t nCopyIndex)
{
    BOOL                                bResult = false;
    KBATTLE_FIELD_COPY_TABLE*           pCopyTable  = NULL;
    KBATTLE_FIELD_TABLE::iterator       itFind;
    KBATTLE_FIELD_COPY_TABLE::iterator  itCopyFind;

    itFind = m_BattleFieldTable.find(dwMapID);
    KG_PROCESS_SUCCESS(itFind == m_BattleFieldTable.end());

    pCopyTable = &itFind->second;
    assert(pCopyTable);

    itCopyFind = pCopyTable->find(nCopyIndex);
    KG_PROCESS_ERROR(itCopyFind != pCopyTable->end());

    KG_PROCESS_ERROR(itCopyFind->second.bActive);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldController::OnPlayerLeave(DWORD dwPlayerID, DWORD dwMapID, int64_t nCopyIndex)
{
    return true;
}

BOOL KBattleFieldController::OnPlayerEnter(DWORD dwPlayerID, DWORD dwMapID, int64_t nCopyIndex)
{
    BOOL                                bResult         = false;
    BOOL                                bRetCode        = false;
    KBATTLE_FIELD_COPY_TABLE*           pCopyTable      = NULL;
    KBattleField*                       pBattleField    = NULL;
    KBATTLE_FIELD_TABLE::iterator       itFind;
    KBATTLE_FIELD_COPY_TABLE::iterator  itFind_CopyTable;

    itFind = m_BattleFieldTable.find(dwMapID);
    KG_PROCESS_ERROR(itFind != m_BattleFieldTable.end());

    pCopyTable = &itFind->second;
    assert(pCopyTable);

    itFind_CopyTable = pCopyTable->find(nCopyIndex);
    KGLOG_PROCESS_ERROR(itFind_CopyTable != pCopyTable->end());

    pBattleField = &itFind_CopyTable->second;
    assert(pBattleField);

    // 可能已经在战场里面了，一些处理就可以忽略了。
    for (KPLAYER_LIST::iterator it = pBattleField->PlayingList.begin(); it != pBattleField->PlayingList.end(); ++it)
    {
        if (it->dwRoleID == dwPlayerID)
        {
            goto Exit1;
        }
    }

    bRetCode = OnPlayerAccpetJoin(dwPlayerID, dwMapID, nCopyIndex);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KBattleFieldController::OnPlayerAccpetJoin(DWORD dwPlayerID, DWORD dwMapID, int64_t nCopyIndex)
{
    BOOL                                bResult         = false;
    KBATTLE_FIELD_COPY_TABLE*           pCopyTable      = NULL;
    KBattleField*                       pBattleField    = NULL;
    KSolder                             Solder;
    KBATTLE_FIELD_TABLE::iterator       itFind;
    KBATTLE_FIELD_COPY_TABLE::iterator  itFind_CopyTable;

    itFind = m_BattleFieldTable.find(dwMapID);
    KG_PROCESS_ERROR(itFind != m_BattleFieldTable.end());

    pCopyTable = &itFind->second;
    assert(pCopyTable);

    itFind_CopyTable = pCopyTable->find(nCopyIndex);
    KGLOG_PROCESS_ERROR(itFind_CopyTable != pCopyTable->end());

    pBattleField = &itFind_CopyTable->second;
    assert(pBattleField);

    for (KPLAYER_LIST::iterator it = pBattleField->WaitingList.begin(); it != pBattleField->WaitingList.end(); ++it)
    {
        if (it->dwRoleID == dwPlayerID)
        {
            Solder = *it;
            pBattleField->WaitingList.erase(it);
            break;
        }
    }

    Solder.nJoinTime = g_pSO3GameCenter->m_nTimeNow;

    pBattleField->PlayingList.push_back(Solder);

    bResult = true;
Exit0:
    return bResult;
}

void KBattleFieldController::ProcessBattleFieldGroup(KBATTLE_FIELD_COPY_TABLE* pCopyList)
{
    assert(pCopyList);

    for (KBATTLE_FIELD_COPY_TABLE::iterator it = pCopyList->begin(); it != pCopyList->end(); ++it)
    {
        if (!(it->second).bActive)
            continue;

        ProcessBattleField(&it->second);
    }
}

void KBattleFieldController::ProcessBattleField(KBattleField* pBattleField)
{
    BOOL    bRetCode                        = false;
    int     nCountPlayer[QUEUE_PER_BATTLE];

    assert(pBattleField);

    for (KPLAYER_LIST::iterator it = pBattleField->PlayingList.begin(); it != pBattleField->PlayingList.end(); ++it)
    {
        KRole* pRole    = NULL;

        nCountPlayer[it->dwGroupID]++;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwRoleID);

        if (pRole != NULL || pRole->GetRoleState() != rsOffline)
        {
            it->nLastOnlineTime = g_pSO3GameCenter->m_nTimeNow;
        }
    }

    for (KPLAYER_LIST::iterator it = pBattleField->WaitingList.begin(); it != pBattleField->WaitingList.end(); ++it)
    {
        KRole* pRole    = NULL;

        nCountPlayer[it->dwGroupID]++;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwRoleID);

        if (pRole != NULL || pRole->GetRoleState() != rsOffline)
        {
            it->nLastOnlineTime = g_pSO3GameCenter->m_nTimeNow;
        }
    }

    for (int nGroupID = 0; nGroupID < QUEUE_PER_BATTLE; nGroupID++)
    {
        DWORD   dwGetRoleList[MAX_PLAYER_IN_BATTLE_FIELD];
        int     nGetPlayerCount     = pBattleField->nMaxPlayerPerSide - nCountPlayer[nGroupID];
        int     nRetGetPlayerCount  = 0;

        KGLOG_PROCESS_ERROR(nGetPlayerCount > MAX_PLAYER_IN_BATTLE_FIELD);

        if (nGetPlayerCount <= 0)
            continue;

        bRetCode = g_pSO3GameCenter->m_BattleFieldQueueManager.GetCanJoinRole(
            &nRetGetPlayerCount, dwGetRoleList, nGetPlayerCount,
            pBattleField->dwMapID, pBattleField->nCopyIndex, nGroupID
        );
        KGLOG_PROCESS_ERROR(bRetCode);

        for (int i = 0; i < nRetGetPlayerCount; i++)
        {
            g_pSO3GameCenter->m_BattleFieldQueueManager.LeaveQueue(dwGetRoleList[i], pBattleField->dwMapID);

            bRetCode = JoinInBattleField(dwGetRoleList[i], pBattleField, nGroupID);
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

Exit0:
    return;
}

BOOL KBattleFieldController::JoinInBattleField(DWORD dwPlayerID, KBattleField *pBattleField, DWORD dwGroupID)
{
    KSolder Solder;

    assert(pBattleField);

    Solder.dwRoleID         = dwPlayerID;
    Solder.nJoinTime        = g_pSO3GameCenter->m_nTimeNow;
    Solder.nLastOnlineTime  = g_pSO3GameCenter->m_nTimeNow;
    Solder.dwGroupID        = dwGroupID;

    pBattleField->WaitingList.push_back(Solder);

    // g_pSO3GameCenter->m_GameServer.DoJoinInBattleField();
    return true;
}

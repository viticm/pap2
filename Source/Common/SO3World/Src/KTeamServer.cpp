#include "stdafx.h"
#include "KPlayer.h"
#include "KTeamServer.h"

#ifdef _SERVER
KTeamServer::KTeamServer()
{
}

BOOL KTeamServer::Init()
{
    return true;
}

void KTeamServer::UnInit()
{
}

KTeam* KTeamServer::GetTeam(DWORD dwTeamID)
{
    KTeam*                  pTeam   = NULL;
    KTEAM_TABLE::iterator   itFind;

    itFind = m_TeamTable.find(dwTeamID);
    KG_PROCESS_ERROR(itFind != m_TeamTable.end());

    pTeam = &itFind->second;
Exit0:
    return pTeam;
}

BOOL KTeamServer::Create(DWORD dwTeamID)
{
    BOOL    bResult = false;
    KTeam   Team;

    memset(Team.dwAuthority, 0, sizeof(Team.dwAuthority));
    memset(Team.dwMark, 0, sizeof(Team.dwMark));

    Team.eCamp      = cNeutral;
    Team.bSystem    = false;
    
    for (int i = 0; i < MAX_TEAM_GROUP_NUM; i++)
    {
        Team.MemberGroup[i].dwFormationLeader = ERROR_ID;
        Team.MemberGroup[i].MemberList.clear();
    }

    Team.nGroupNum      = MIN_TEAM_GROUP_NUM;
    Team.nLootMode      = ilmGroupLoot;
    Team.nRollQuality   = 2;

    m_TeamTable.insert(make_pair(dwTeamID, Team));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamServer::Destroy(DWORD dwTeamID)
{
    BOOL                    bResult = false;
    KTeam*                  pTeam   = NULL;
    KTEAM_TABLE::iterator   itFind;

    itFind = m_TeamTable.find(dwTeamID);
    KGLOG_PROCESS_ERROR(itFind != m_TeamTable.end());

    pTeam = &itFind->second;

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            KPlayer* pPlayer = g_pSO3World->m_PlayerSet.GetObj(it->dwMemberID);

            if (!pPlayer)
                continue;

            pPlayer->SetCurrentTeamID(pTeam->bSystem, ERROR_ID);
        }
    }

    m_TeamTable.erase(itFind);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamServer::AddMember(DWORD dwTeamID, int nGroupIndex, DWORD dwPlayerID)
{
    BOOL                bResult     = false;
    KTeam*              pTeam       = NULL;
    KPlayer*            pPlayer     = NULL;
    KTEAM_MEMBER_INFO   MemberInfo;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(nGroupIndex < pTeam->nGroupNum);

    KGLOG_PROCESS_ERROR(pTeam->MemberGroup[nGroupIndex].MemberList.size() < MAX_PLAYER_PER_GROUP);

    memset(&MemberInfo, 0, sizeof(MemberInfo));

    MemberInfo.dwMemberID   = dwPlayerID;
    MemberInfo.bOnlineFlag  = true;
    MemberInfo.nLifePercent = UCHAR_MAX;
    MemberInfo.nManaPercent = UCHAR_MAX;
    MemberInfo.nRagePercent = UCHAR_MAX;

    pTeam->MemberGroup[nGroupIndex].MemberList.push_back(MemberInfo);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    if (pPlayer)
    {
        pPlayer->SetCurrentTeamID(pTeam->bSystem, dwTeamID);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamServer::DelMember(DWORD dwTeamID, int nGroupIndex, DWORD dwPlayerID)
{
    BOOL     bResult = false;
    KPlayer* pPlayer = NULL;
    KTeam*   pTeam   = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(nGroupIndex < pTeam->nGroupNum);

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID == dwPlayerID)
        {
            pTeam->MemberGroup[nGroupIndex].MemberList.erase(it);

            pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
            if (pPlayer)
            {
                pPlayer->SetCurrentTeamID(pTeam->bSystem, ERROR_ID);
            }

            bResult = true;
            goto Exit0;
        }
    }
Exit0:
    return bResult;
}

BOOL KTeamServer::ChangeMember(DWORD dwTeamID, int nSrcGroupIndex, int nDstGroupIndex, DWORD dwSrcPlayerID, DWORD dwDstPlayerID)
{
    BOOL                bResult         = false;
    KPlayer*            pPlayer         = NULL;
    KTeam*              pTeam           = NULL;
    KTEAM_MEMBER_INFO   TmpMemberInfo;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(nSrcGroupIndex < pTeam->nGroupNum);
    KGLOG_PROCESS_ERROR(nDstGroupIndex < pTeam->nGroupNum);

    memset(&TmpMemberInfo, 0, sizeof(TmpMemberInfo));
    TmpMemberInfo.dwMemberID = ERROR_ID;

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nDstGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nDstGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID == dwDstPlayerID)
        {
            TmpMemberInfo = *it;

            pTeam->MemberGroup[nDstGroupIndex].MemberList.erase(it);
            break;
        }
    }

    KGLOG_PROCESS_ERROR(pTeam->MemberGroup[nDstGroupIndex].MemberList.size() < MAX_PLAYER_PER_GROUP);

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nSrcGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nSrcGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID != dwSrcPlayerID)
            continue;
        
        pTeam->MemberGroup[nDstGroupIndex].MemberList.push_back(*it);
        pTeam->MemberGroup[nSrcGroupIndex].MemberList.erase(it);

        if (TmpMemberInfo.dwMemberID != ERROR_ID)
            pTeam->MemberGroup[nSrcGroupIndex].MemberList.push_back(TmpMemberInfo);

        bResult = true;
        goto Exit0;
    }

Exit0:
    return bResult;
}

BOOL KTeamServer::IsPlayerInTeam(DWORD dwTeamID, DWORD dwPlayerID)
{
    BOOL                bResult = false;
    KTEAM_MEMBER_INFO*  pMember = NULL;

    pMember = GetMember(dwTeamID, dwPlayerID);
    KG_PROCESS_ERROR(pMember);

    bResult = true;
Exit0:
    return bResult;
}

KTEAM_MEMBER_INFO* KTeamServer::GetMember(DWORD dwTeamID, DWORD dwPlayerID)
{
    KTEAM_MEMBER_INFO*  pResult = NULL;
    KTeam*              pTeam   = NULL;

    pTeam = GetTeam(dwTeamID);
    KG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == dwPlayerID)
            {
                pResult = &(*it);
                goto Exit0;
            }
        }
    }

Exit0:
    return pResult;
}

BOOL KTeamServer::SetAuthority(DWORD dwTeamID, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID)
{
    BOOL    bResult = false;
    KTeam*  pTeam   = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(eType > tatInvalid && eType < tatTotal);

    pTeam->dwAuthority[eType] = dwTargetID;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamServer::SetFormationLeader(DWORD dwTeamID, int nGroupIndex, DWORD dwTargetID)
{
    BOOL    bResult = false;
    KTeam*  pTeam   = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);
    
    KGLOG_PROCESS_ERROR(nGroupIndex < pTeam->nGroupNum);

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID == dwTargetID)
        {
            pTeam->MemberGroup[nGroupIndex].dwFormationLeader = dwTargetID;

            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

BOOL KTeamServer::SetMark(DWORD dwTeamID, int nMarkType, DWORD dwTargetID)
{
    BOOL    bResult = false;
    KTeam*  pTeam   = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < PARTY_MARK_COUNT; i++)
    {
        if (pTeam->dwMark[i] == dwTargetID)
        {
            pTeam->dwMark[i] = 0;
        }
    }

    KG_PROCESS_SUCCESS(nMarkType == 0);
    KGLOG_PROCESS_ERROR(nMarkType > 0 && nMarkType <= PARTY_MARK_COUNT);
    pTeam->dwMark[nMarkType - 1] = dwTargetID;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

int KTeamServer::GetMemberGroupIndex(DWORD dwTeamID, DWORD dwMemberID)
{
    int     nResult = INVALID_TEAM_GROUP;
    KTeam*  pTeam   = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == dwMemberID)
            {
                nResult = i;

                goto Exit0;
            }
        }
    }

Exit0:
    return nResult;
}

BOOL KGetAllTeamMember::operator ()(KTEAM_MEMBER_INFO* pMemberInfo)
{
    BOOL bResult = false;

    m_vecResult.push_back(pMemberInfo->dwMemberID);

    bResult = true;
Exit0:
    return bResult;
}
#endif

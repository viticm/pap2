#include "stdafx.h"
#include "KTeamCenter.h"
#include "KSO3GameCenter.h"

KTeamCenter::KTeamCenter()
{
}

BOOL KTeamCenter::Init()
{
    m_dwTeamSNGenerator = ERROR_ID;

    return true;
}

void KTeamCenter::UnInit()
{
}

void KTeamCenter::Activate()
{
    for (KTEAM_TABLE::iterator it = m_TeamTable.begin(); it != m_TeamTable.end(); NULL)
    {
        if ((g_pSO3GameCenter->m_nWorkLoop + (int)it->first) % GAME_FPS)
        {
            ProcessTeam(it->first, &it->second);
        }

        if (it->second.bDelete)
        {
            KTeam* pTeam = &it->second;

            g_pSO3GameCenter->m_GameServer.DoTeamDisbandNotify(it->first);

            for (int i = 0; i < pTeam->nGroupNum; i++)
            {
                for (
                    KTEAM_MEMBER_LIST::iterator ListIter = pTeam->MemberGroup[i].MemberList.begin();
                    ListIter != pTeam->MemberGroup[i].MemberList.end(); ++ListIter
                )
                {
                    KRole* pRole = NULL;

                    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(ListIter->dwMemberID);
                    if(!pRole)
                        continue;

                    SetRoleTeamID(pRole, pTeam->bSystem, ERROR_ID);
                }
            }

            m_TeamTable.erase(it++);
            continue;
        }

        ++it;
    }

    if (g_pSO3GameCenter->m_nWorkLoop & (GAME_FPS * 2))
    {
        ProcessInviteAndApplyTimeOut();
    }
}

DWORD KTeamCenter::CreateTeam(BOOL bSystem)
{
    DWORD dwResult  = ERROR_ID;
    DWORD dwTeamID  = ++m_dwTeamSNGenerator;
    KTeam Team;

    std::pair<KTEAM_TABLE::iterator, BOOL> InsertRet;

    Team.bDelete            = false;
    memset(Team.dwAuthority, 0, sizeof(Team.dwAuthority));
    memset(Team.dwTeamMark, 0, sizeof(Team.dwTeamMark));
    Team.eCamp              = cNeutral;
    Team.nLootMode          = ilmGroupLoot;
    Team.nRollQuality       = 2;
    Team.nGroupNum          = MIN_TEAM_GROUP_NUM;
    Team.nLastActiveTime    = g_pSO3GameCenter->m_nTimeNow;
    Team.bSystem            = bSystem;

    for (int i = 0; i < MAX_TEAM_GROUP_NUM; i++)
    {
        Team.MemberGroup[i].dwFormationLeader = ERROR_ID;
        Team.MemberGroup[i].MemberList.clear();
    }

    InsertRet = m_TeamTable.insert(make_pair(dwTeamID, Team));
    KGLOG_PROCESS_ERROR(InsertRet.second);

    g_pSO3GameCenter->m_GameServer.DoTeamCreateNotify(dwTeamID);

    dwResult = dwTeamID;
Exit0:
    return dwResult;
}

BOOL KTeamCenter::DestroyTeam(DWORD dwTeamID)
{
    BOOL    bResult = false;
    KTeam*  pTeam   = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    pTeam->bDelete = true;

    bResult = true;
Exit0:
    return bResult;
}

KTeam* KTeamCenter::GetTeam(DWORD dwTeamID)
{
    KTeam*                  pTeam   = NULL;
    KTEAM_TABLE::iterator   itFind;

    itFind = m_TeamTable.find(dwTeamID);
    KG_PROCESS_ERROR(itFind != m_TeamTable.end());

    pTeam = &itFind->second;
Exit0:
    return pTeam;
}

BOOL KTeamCenter::AddMember(DWORD dwTeamID, DWORD dwRoleID)
{
    BOOL                    bResult     = false;
    KTeam*                  pTeam       = NULL;
    KRole*                  pRole       = NULL;
    int                     nGroupIndex = 0;
    KTEAM_MEMBER_INFO       MemberInfo;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (nGroupIndex = 0; nGroupIndex < pTeam->nGroupNum; nGroupIndex++)
    {
        if (pTeam->MemberGroup[nGroupIndex].MemberList.size() < MAX_PLAYER_PER_GROUP)
            break;
    }

    KG_PROCESS_ERROR(nGroupIndex < pTeam->nGroupNum); // 可能是队伍人数满了

    MemberInfo.dwMemberID = dwRoleID;
    MemberInfo.bIsOnline  = true;

    pTeam->MemberGroup[nGroupIndex].MemberList.push_back(MemberInfo);

    g_pSO3GameCenter->m_GameServer.DoTeamAddMemberNotify(dwTeamID, nGroupIndex, dwRoleID);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    SetRoleTeamID(pRole, pTeam->bSystem, dwTeamID);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::DelMember(DWORD dwTeamID, DWORD dwRoleID)
{
    BOOL                    bResult     = false;
    KTeam*                  pTeam       = NULL;
    KRole*                  pRole       = NULL;
    int                     nGroupIndex = 0;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (nGroupIndex = 0; nGroupIndex < pTeam->nGroupNum; nGroupIndex++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
            it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == dwRoleID)
            {
                pTeam->MemberGroup[nGroupIndex].MemberList.erase(it);

                pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
                KGLOG_PROCESS_ERROR(pRole);

                SetRoleTeamID(pRole, pTeam->bSystem, ERROR_ID);

                g_pSO3GameCenter->m_GameServer.DoTeamDelMemberNotify(dwTeamID, nGroupIndex, dwRoleID);

                bResult = true;
                goto Exit0;
            }
        }
    }

Exit0:
    return bResult;
}

BOOL KTeamCenter::ChangeGroup(DWORD dwTeamID, int nDstGroupIndex, DWORD dwDstRoleID, DWORD dwRoleID)
{
    BOOL                    bResult     = false;
    KTeam*                  pTeam       = NULL;
    KTEAM_MEMBER_INFO       TmpMemberInfo;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(nDstGroupIndex >= 0 && nDstGroupIndex < pTeam->nGroupNum);

    TmpMemberInfo.dwMemberID = ERROR_ID;
    TmpMemberInfo.bIsOnline  = false;

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nDstGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nDstGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID == dwDstRoleID)
        {
            TmpMemberInfo = *it;

            pTeam->MemberGroup[nDstGroupIndex].MemberList.erase(it);
            break;
        }
    }

    KG_PROCESS_ERROR(pTeam->MemberGroup[nDstGroupIndex].MemberList.size() < MAX_PLAYER_PER_GROUP);

    for (int nGroupIndex = 0; nGroupIndex < pTeam->nGroupNum; nGroupIndex++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
            it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID != dwRoleID)
                continue;

            pTeam->MemberGroup[nDstGroupIndex].MemberList.push_back(*it);
            pTeam->MemberGroup[nGroupIndex].MemberList.erase(it);

            if (TmpMemberInfo.dwMemberID != ERROR_ID)
                pTeam->MemberGroup[nGroupIndex].MemberList.push_back(TmpMemberInfo);

            g_pSO3GameCenter->m_GameServer.DoTeamChangeMemberGroupNotify(
                dwTeamID, dwRoleID, nGroupIndex, dwDstRoleID, nDstGroupIndex
            );

            bResult = true;
            goto Exit0;
        }
    }
Exit0:
    return bResult;
}

BOOL KTeamCenter::SetAuthority(DWORD dwTeamID, KTEAM_AUTHORITY_TYPE eType, DWORD dwTargetID)
{
    BOOL                    bResult     = false;
    KTeam*                  pTeam       = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KGLOG_PROCESS_ERROR(eType > tatInvalid && eType < tatTotal);

    pTeam->dwAuthority[eType] = dwTargetID;

    g_pSO3GameCenter->m_GameServer.DoTeamChangeAuthorityNotify(dwTeamID, eType, dwTargetID);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::SetFormationLeader(DWORD dwTeamID, int nGroupIndex, DWORD dwRequestSrcID, DWORD dwTargetID)
{
    BOOL                    bResult     = false;
    KTeam*                  pTeam       = NULL;
    KTEAM_TABLE::iterator   itFind;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < pTeam->nGroupNum);

    KG_PROCESS_ERROR(
        dwRequestSrcID == pTeam->dwAuthority[tatLeader] ||
        dwRequestSrcID == pTeam->MemberGroup[nGroupIndex].dwFormationLeader
    );

    for (
        KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
        it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID == dwTargetID)
        {
            pTeam->MemberGroup[nGroupIndex].dwFormationLeader = dwTargetID;

            g_pSO3GameCenter->m_GameServer.DoTeamSetFormationLeaderNotify(dwTeamID, nGroupIndex, dwTargetID);

            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

BOOL KTeamCenter::SetMark(DWORD dwTeamID, int nMarkType, DWORD dwTargetID)
{
    BOOL    bResult = false;
    KTeam*  pTeam   = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < PARTY_MARK_COUNT; i++)
    {
        if (pTeam->dwTeamMark[i] == dwTargetID)
        {
            pTeam->dwTeamMark[i] = 0;
        }
    }

    KG_PROCESS_SUCCESS(nMarkType == 0); // 等于0的时候表示取消标记
    KGLOG_PROCESS_ERROR(nMarkType > 0 && nMarkType <= PARTY_MARK_COUNT);

    pTeam->dwTeamMark[nMarkType - 1] = dwTargetID;

Exit1:
    g_pSO3GameCenter->m_GameServer.DoTeamSetMarkRespond(dwTeamID, nMarkType, dwTargetID);

    bResult = true;
Exit0:
    return bResult;
}

void KTeamCenter::SyncNewServer(int nConnIndex)
{
    for (KTEAM_TABLE::iterator it = m_TeamTable.begin(); it != m_TeamTable.end(); ++it)
    {
        g_pSO3GameCenter->m_GameServer.DoSyncTeamInfo(nConnIndex, it->first, &it->second);
    }
}

int KTeamCenter::GetTeamMemberCount(DWORD dwTeamID)
{
    int     nCount = 0;
    KTeam*  pTeam  = NULL;

    pTeam = GetTeam(dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        nCount += (int)pTeam->MemberGroup[i].MemberList.size();
    }

Exit0:
    return nCount;
}

BOOL KTeamCenter::IsRoleInTeam(DWORD dwTeamID, DWORD dwRoleID)
{
    BOOL    bResult = false;
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
            if (dwRoleID == it->dwMemberID)
            {
                bResult = true;
                goto Exit0;
            }
        }
    }

Exit0:
    return bResult;
}

BOOL KTeamCenter::RegisterInvite(DWORD dwInviteSrc, DWORD dwInviteDst)
{
    BOOL                    bResult = false;
    int64_t                 nKey    = MAKE_INT64(dwInviteSrc, dwInviteDst);
    KINVITE_TABLE::iterator itFind;

    itFind = m_InviteTable.find(nKey);
    KG_PROCESS_ERROR(itFind == m_InviteTable.end());

    m_InviteTable.insert(make_pair(nKey, g_pSO3GameCenter->m_nTimeNow));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::UnregisterInvite(DWORD dwInviteSrc, DWORD dwInviteDst)
{
    BOOL                    bResult = false;
    int64_t                 nKey    = MAKE_INT64(dwInviteSrc, dwInviteDst);
    KINVITE_TABLE::iterator itFind;

    itFind = m_InviteTable.find(nKey);
    KG_PROCESS_ERROR(itFind != m_InviteTable.end());

    m_InviteTable.erase(itFind);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::ClearInvite(DWORD dwInviteSrc)
{
    BOOL                    bResult  = false;
    int64_t                 nKeyLow  = MAKE_INT64(dwInviteSrc, 0);
    int64_t                 nKeyHigh = MAKE_INT64(dwInviteSrc + 1, 0);
    KINVITE_TABLE::iterator itLow;
    KINVITE_TABLE::iterator itHigh;
    KINVITE_TABLE::iterator it;

    KG_PROCESS_ERROR(nKeyLow < nKeyHigh);
    itLow  = m_InviteTable.lower_bound(nKeyLow);
    itHigh = m_InviteTable.upper_bound(nKeyHigh);

    for (it = itLow; it != itHigh;)
    {
        m_InviteTable.erase(it++);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::IsInviteExist(DWORD dwInviteSrc, DWORD dwInviteDst)
{
    BOOL                    bResult = false;
    int64_t                 nKey    = MAKE_INT64(dwInviteSrc, dwInviteDst);
    KINVITE_TABLE::iterator itFind;
    
    itFind = m_InviteTable.find(nKey);
    KG_PROCESS_ERROR(itFind != m_InviteTable.end());

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::RegisterApply(DWORD dwApplySrc, DWORD dwApplyDst)
{
    BOOL                    bResult = false;
    int64_t                 nKey    = MAKE_INT64(dwApplySrc, dwApplyDst);
    KAPPLY_TABLE::iterator itFind;

    itFind = m_ApplyTable.find(nKey);
    KG_PROCESS_ERROR(itFind == m_ApplyTable.end());

    m_ApplyTable.insert(make_pair(nKey, g_pSO3GameCenter->m_nTimeNow));

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::UnregisterApply(DWORD dwApplySrc, DWORD dwApplyDst)
{
    BOOL                    bResult = false;
    int64_t                 nKey    = MAKE_INT64(dwApplySrc, dwApplyDst);
    KAPPLY_TABLE::iterator itFind;

    itFind = m_ApplyTable.find(nKey);
    KG_PROCESS_ERROR(itFind != m_ApplyTable.end());

    m_ApplyTable.erase(itFind);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::ClearApply(DWORD dwApplySrc)
{
    BOOL                    bResult  = false;
    int64_t                 nKeyLow  = MAKE_INT64(dwApplySrc, 0);
    int64_t                 nKeyHigh = MAKE_INT64(dwApplySrc + 1, 0);
    KAPPLY_TABLE::iterator  itLow;
    KAPPLY_TABLE::iterator  itHigh;
    KAPPLY_TABLE::iterator  it;

    KG_PROCESS_ERROR(nKeyLow < nKeyHigh);
    itLow  = m_ApplyTable.lower_bound(nKeyLow);
    itHigh = m_ApplyTable.upper_bound(nKeyHigh);

    for (it = itLow; it != itHigh;)
    {
        m_InviteTable.erase(it++);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamCenter::IsApplyExist(DWORD dwApplySrc, DWORD dwApplyDst)
{
    BOOL                    bResult = false;
    int64_t                 nKey    = MAKE_INT64(dwApplySrc, dwApplyDst);
    KAPPLY_TABLE::iterator  itFind;

    itFind = m_ApplyTable.find(nKey);
    KG_PROCESS_ERROR(itFind != m_ApplyTable.end());

    bResult = true;
Exit0:
    return bResult;
}

int KTeamCenter::GetTeamCount()
{
    return (int)m_TeamTable.size();
}

void KTeamCenter::ProcessTeam(DWORD dwTeamID, KTeam* pTeam)
{
    ProcessAuthority(dwTeamID, pTeam);

    ProcessFormation(dwTeamID, pTeam);

    ProcessMember(dwTeamID, pTeam);
}

void KTeamCenter::ProcessAuthority(DWORD dwTeamID, KTeam* pTeam)
{
    // ----------------- 如果拥有权限的人没有在线或不在队伍中，则回收他的权限----------->>
    for (int i = tatInvalid + 1; i < tatTotal; i++)
    {
        KRole* pRole = NULL;

        if (pTeam->dwAuthority[i] == ERROR_ID)
            continue;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pTeam->dwAuthority[i]);
        if (
            !pRole || !pRole->IsOnline() ||
            (pRole->m_dwTeamID != dwTeamID && pRole->m_dwSystemTeamID != dwTeamID)
        )
        {
            pTeam->dwAuthority[i] = ERROR_ID;
        }
    }
    // <<---------------------------------------------------------------------------------

    // ----------------- 如果队长权限没有人拥有，则按规则分配 -------------------------->>
    if (pTeam->dwAuthority[tatLeader] == ERROR_ID && pTeam->dwAuthority[tatDistribute] != ERROR_ID)
    {
        SetAuthority(dwTeamID, tatLeader, pTeam->dwAuthority[tatDistribute]);

        OnChangeLeader(pTeam->dwAuthority[tatLeader]);
    }

    if (pTeam->dwAuthority[tatLeader] == ERROR_ID && pTeam->dwAuthority[tatMark] != ERROR_ID)
    {
        SetAuthority(dwTeamID, tatLeader, pTeam->dwAuthority[tatMark]);

        OnChangeLeader(pTeam->dwAuthority[tatLeader]);
    }

    if (pTeam->dwAuthority[tatLeader] != ERROR_ID)
    {
        if (pTeam->dwAuthority[tatDistribute] == ERROR_ID)
        {
            SetAuthority(dwTeamID, tatDistribute, pTeam->dwAuthority[tatLeader]);
        }

        if (pTeam->dwAuthority[tatMark] == ERROR_ID)
        {
            SetAuthority(dwTeamID, tatMark, pTeam->dwAuthority[tatLeader]);
        }
    }
    // <<---------------------------------------------------------------------------------
}

void KTeamCenter::ProcessFormation(DWORD dwTeamID, KTeam* pTeam)
{
    for (int i = 0; i < pTeam->nGroupNum; i++)
    {
        KRole* pRole        = NULL;
        BOOL   bIsInGroup   = false;

        pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pTeam->MemberGroup[i].dwFormationLeader);

        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == pTeam->MemberGroup[i].dwFormationLeader)
            {
                bIsInGroup = true;
                break;
            }
        }

        // ------------- 如果阵眼不在线或者不在队伍中，则收回他的权限。----------------->>
        if (!pRole || !pRole->IsOnline() || !bIsInGroup)
        {
            pTeam->MemberGroup[i].dwFormationLeader = ERROR_ID;
        }
        // <<-----------------------------------------------------------------------------
    }
}

void KTeamCenter::ProcessMember(DWORD dwTeamID, KTeam* pTeam)
{
    int     nActiveMemberCount  = 0;
    int     nMemberCount        = GetTeamMemberCount(dwTeamID);
    // BOOL    bNotNeutral         = false;

    // ----------------- 如果队伍所有人下线超过一定时间或者所有人都退出了，则解散掉 ---->>
    if (
        !pTeam->bSystem &&
        (g_pSO3GameCenter->m_nTimeNow - pTeam->nLastActiveTime > EMPTY_TEAM_TIME_OUT || nMemberCount < 2)
    )
    {
        DestroyTeam(dwTeamID);
    }
    // <<---------------------------------------------------------------------------------

    for (int nGroupIndex = 0; nGroupIndex < pTeam->nGroupNum; nGroupIndex++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[nGroupIndex].MemberList.begin();
            it != pTeam->MemberGroup[nGroupIndex].MemberList.end(); NULL
        )
        {
            KRole* pRole = NULL;

            pRole = g_pSO3GameCenter->m_RoleManager.GetRole(it->dwMemberID);

            if (!pRole)
            {
                KGLogPrintf(KGLOG_ERR, "[TEAM] Error Role in team.\n");

                pTeam->MemberGroup[nGroupIndex].MemberList.erase(it++);
                continue;
            }

            // if (pRole->m_eCamp != cNeutral)
            // {
            //     if (pTeam->eCamp == cNeutral)
            //     {
            //         pTeam->eCamp = pRole->m_eCamp;
            // 
            //         g_pSO3GameCenter->m_GameServer.DoTeamCampChange(dwTeamID, pTeam->eCamp);
            //     }
            // 
            //     bNotNeutral = true;
            // }

            // --------- 如果队伍和该成员的阵营是冲突的，则该成员离开队伍 -------------->>
            // if (pRole->m_eCamp != cNeutral && pTeam->eCamp != cNeutral && pRole->m_eCamp != pTeam->eCamp)
            // {
            //     KGLogPrintf(KGLOG_ERR, "[TEAM] Camp error, in Team(%lu), Role:%s\n", dwTeamID, pRole->m_szName);
            // 
            //     pTeam->MemberGroup[nGroupIndex].MemberList.erase(it++);
            // 
            //     if (pTeam->bSystem)
            //     {
            //         pRole->m_dwSystemTeamID = ERROR_ID;
            //     }
            //     else
            //     {
            //         pRole->m_dwTeamID = ERROR_ID;
            //     }
            // 
            //     g_pSO3GameCenter->m_GameServer.DoTeamDelMemberNotify(dwTeamID, nGroupIndex, pRole->m_dwPlayerID);
            // }
            // <<-------------------------------------------------------------------------

            if (!pRole->IsOnline())
            {
                if (it->bIsOnline)
                {
                    it->bIsOnline = false;
                    g_pSO3GameCenter->m_GameServer.DoSyncTeamMemberOnlineFlag(
                        dwTeamID, it->dwMemberID, false
                    );
                }

                ++it;
                continue;
            }

            if (pRole->IsOnline() && !it->bIsOnline)
            {
                it->bIsOnline = true;
                g_pSO3GameCenter->m_GameServer.DoSyncTeamMemberOnlineFlag(
                    dwTeamID, it->dwMemberID, true
                );
            }

            nActiveMemberCount++;

            if (pTeam->dwAuthority[tatLeader] == ERROR_ID) // 如果这时候队长权限还没分配出去，就给这个角色
            {
                SetAuthority(dwTeamID, tatLeader, pRole->m_dwPlayerID);

                OnChangeLeader(pTeam->dwAuthority[tatLeader]);
            }

            if (pTeam->MemberGroup[nGroupIndex].dwFormationLeader == ERROR_ID)
            {
                pTeam->MemberGroup[nGroupIndex].dwFormationLeader = pRole->m_dwPlayerID;

                g_pSO3GameCenter->m_GameServer.DoTeamSetFormationLeaderNotify(
                    dwTeamID, nGroupIndex, pTeam->MemberGroup[nGroupIndex].dwFormationLeader
                );
            }

            ++it;
        }
    }

    if (nMemberCount > 0 && pTeam->dwAuthority[tatLeader] == ERROR_ID)
    {
        for (int nGroupIndex = 0; nGroupIndex < pTeam->nGroupNum; nGroupIndex++)
        {
            if (pTeam->MemberGroup[nGroupIndex].MemberList.empty())
                continue;

            pTeam->dwAuthority[tatLeader] = pTeam->MemberGroup[nGroupIndex].MemberList.front().dwMemberID;
            break;
        }
    }

    if (nActiveMemberCount > 0)
    {
        pTeam->nLastActiveTime = g_pSO3GameCenter->m_nTimeNow;
    }

    // if (!bNotNeutral && pTeam->eCamp != cNeutral)
    // {
    //     pTeam->eCamp = cNeutral;
    //     g_pSO3GameCenter->m_GameServer.DoTeamCampChange(dwTeamID, pTeam->eCamp);
    // }
}

void KTeamCenter::ProcessInviteAndApplyTimeOut()
{
    for (KINVITE_TABLE::iterator it = m_InviteTable.begin(); it != m_InviteTable.end(); NULL)
    {
        if (g_pSO3GameCenter->m_nTimeNow - it->second > INVITE_AND_APPLT_TIME_OUT)
        {
            m_InviteTable.erase(it++);
            continue;
        }

        ++it;
    }

    for (KAPPLY_TABLE::iterator it = m_ApplyTable.begin(); it != m_ApplyTable.end(); NULL)
    {
        if (g_pSO3GameCenter->m_nTimeNow - it->second > INVITE_AND_APPLT_TIME_OUT)
        {
            m_ApplyTable.erase(it++);
            continue;
        }

        ++it;
    }
}

void KTeamCenter::OnChangeLeader(DWORD dwLeaderID)
{
    BOOL        bRetCode    = false;
    KRole*      pLeader     = NULL;
    KMapInfo*   pMapInfo    = NULL;

    pLeader = g_pSO3GameCenter->m_RoleManager.GetRole(dwLeaderID);
    KGLOG_PROCESS_ERROR(pLeader);

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(pLeader->m_dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    KG_PROCESS_ERROR(pMapInfo->m_nType == emtDungeon);

    bRetCode = g_pSO3GameCenter->m_MapManager.SetMapCopyOwner(pLeader->m_dwMapID, pLeader->m_nMapCopyIndex, dwLeaderID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return;
}

void KTeamCenter::SetRoleTeamID(KRole* pRole, BOOL bSystem, DWORD dwTeamID)
{
    KTeam* pTeam = NULL;

    assert(pRole);

    if (bSystem)
    {
        pRole->m_dwSystemTeamID = dwTeamID;
    }
    else
    {
        pRole->m_dwTeamID = dwTeamID;
    }
}

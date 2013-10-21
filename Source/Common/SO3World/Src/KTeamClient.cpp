#include"stdafx.h"
#include "KMath.h"
#include "KGPublic.h"
#include "KSO3World.h"
#include "KRelayClient.h"
#include "Engine/KGLog.h"
#include "KScene.h"
#include "KPlayer.h"
#include "KTeamClient.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "KPlayerClient.h"

#ifdef _CLIENT
KTeamClient::KTeamClient()
{
}

KTeamClient::~KTeamClient()
{
}

BOOL KTeamClient::Init()
{
    m_dwTeamID              = ERROR_ID;
    m_nGroupNum             = MIN_TEAM_GROUP_NUM;
    m_nLootMode             = 0;
    m_nRollQuality          = 0;
    m_eCamp                 = cNeutral;
    memset(m_dwAuthority, 0, sizeof(m_dwAuthority));
    memset(m_dwTeamMark, 0, sizeof(m_dwTeamMark));

    for (int i = 0; i < MAX_TEAM_GROUP_NUM; i++)
    {
        m_TeamGroup[i].dwFormationLeader = ERROR_ID;
        m_TeamGroup[i].MemberList.clear();
    }
    
    return true;
}

void KTeamClient::UnInit()
{
    m_dwTeamID              = ERROR_ID;
    m_nGroupNum             = MIN_TEAM_GROUP_NUM;
    m_nLootMode             = 0;
    m_nRollQuality          = 0;
    m_eCamp                 = cNeutral;
    memset(m_dwAuthority, 0, sizeof(m_dwAuthority));
    memset(m_dwTeamMark, 0, sizeof(m_dwTeamMark));

    for (int i = 0; i < MAX_TEAM_GROUP_NUM; i++)
    {
        m_TeamGroup[i].dwFormationLeader = ERROR_ID;
        m_TeamGroup[i].MemberList.clear();
    }
}

BOOL KTeamClient::Create()
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(m_dwTeamID == ERROR_ID);

    m_bSystem = false;

    for (int i = 0; i < MAX_TEAM_GROUP_NUM; i++)
    {
        m_TeamGroup[i].dwFormationLeader = ERROR_ID;
        m_TeamGroup[i].MemberList.clear();
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamClient::Destroy()
{
    BOOL bResult = false;

    m_dwTeamID              = ERROR_ID;
    m_nGroupNum             = MIN_TEAM_GROUP_NUM;
    m_nLootMode             = 0;
    m_nRollQuality          = 0;
    m_eCamp                 = cNeutral;
    m_bSystem               = false;
    memset(m_dwAuthority, 0, sizeof(m_dwAuthority));
    memset(m_dwTeamMark, 0, sizeof(m_dwTeamMark));

    for (int i = 0; i < MAX_TEAM_GROUP_NUM; i++)
    {
        m_TeamGroup[i].dwFormationLeader = ERROR_ID;
        m_TeamGroup[i].MemberList.clear();
    }

    for (int i = 0; i < PARTY_MARK_COUNT; i++)
    {
        KCharacter* pClearCharacter = NULL;
        if (!m_dwTeamMark[i])
            continue;

        if (IS_PLAYER(m_dwTeamMark[i]))
        {
            pClearCharacter = g_pSO3World->m_PlayerSet.GetObj(m_dwTeamMark[i]);
        }
        else
        {
            pClearCharacter = g_pSO3World->m_NpcSet.GetObj(m_dwTeamMark[i]);
        }

        if (!pClearCharacter)
            continue;

        m_dwTeamMark[i] = 0;

        if (g_pGameWorldRepresentHandler)
        {
            g_pGameWorldRepresentHandler->OnCharacterUpdateTitleMark(pClearCharacter);
        }
    }
    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamClient::AddMember(DWORD dwMemberID, int nGroupIndex, char szMemberName[])
{
    BOOL                bResult     = false;
    KPlayer*            pPlayer     = NULL;
    KTEAM_MEMBER_INFO   MemberInfo;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);
    KGLOG_PROCESS_ERROR(nGroupIndex < m_nGroupNum);
    KG_PROCESS_ERROR(m_TeamGroup[nGroupIndex].MemberList.size() < MAX_PLAYER_PER_GROUP);

    memset(&MemberInfo, 0, sizeof(MemberInfo));

    strncpy(MemberInfo.szRoleName, szMemberName, sizeof(MemberInfo.szRoleName));
    MemberInfo.szRoleName[sizeof(MemberInfo.szRoleName) - 1] = '\0';

    MemberInfo.dwMemberID   = dwMemberID;
    MemberInfo.bDeathFlag   = false;
    MemberInfo.bOnlineFlag  = true;
    MemberInfo.nLifePercent = UCHAR_MAX;
    MemberInfo.nManaPercent = UCHAR_MAX;
    MemberInfo.nRagePercent = UCHAR_MAX;

    m_TeamGroup[nGroupIndex].MemberList.push_back(MemberInfo);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwMemberID);
    if (pPlayer)
    {
        pPlayer->SetCurrentTeamID(m_bSystem, m_dwTeamID);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KTeamClient::DelMember(DWORD dwMemberID, int nGroupIndex)
{
    BOOL        bResult = false;
    KPlayer*    pPlayer = NULL;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);
    KG_PROCESS_ERROR(nGroupIndex < m_nGroupNum);

    for (
        KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[nGroupIndex].MemberList.begin();
        it != m_TeamGroup[nGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID == dwMemberID)
        {
            m_TeamGroup[nGroupIndex].MemberList.erase(it);

            pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwMemberID);
            if (pPlayer)
            {
                pPlayer->SetCurrentTeamID(m_bSystem, ERROR_ID);
            }

            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

BOOL KTeamClient::ChangeMember(DWORD dwMemberID, int nSrcGroupIndex, DWORD dwDstMemberID, int nDstGroupIndex)
{
    BOOL                bResult         = false;
    KTEAM_MEMBER_INFO   TmpMemberInfo;

    KGLOG_PROCESS_ERROR(nSrcGroupIndex < m_nGroupNum);
    KGLOG_PROCESS_ERROR(nDstGroupIndex < m_nGroupNum);

    memset(&TmpMemberInfo, 0, sizeof(TmpMemberInfo));
    TmpMemberInfo.dwMemberID = ERROR_ID;

    for (
        KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[nDstGroupIndex].MemberList.begin();
        it != m_TeamGroup[nDstGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID == dwDstMemberID)
        {
            TmpMemberInfo = *it;
            m_TeamGroup[nDstGroupIndex].MemberList.erase(it);
            break;
        }
    }

    KGLOG_PROCESS_ERROR(m_TeamGroup[nDstGroupIndex].MemberList.size() < MAX_PLAYER_PER_GROUP);

    for (
        KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[nSrcGroupIndex].MemberList.begin();
        it != m_TeamGroup[nSrcGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID != dwMemberID)
            continue;

        m_TeamGroup[nDstGroupIndex].MemberList.push_back(*it);
        m_TeamGroup[nSrcGroupIndex].MemberList.erase(it);

        if (TmpMemberInfo.dwMemberID != ERROR_ID)
            m_TeamGroup[nSrcGroupIndex].MemberList.push_back(TmpMemberInfo);

        bResult = true;
        goto Exit0;
    }

Exit0:
    return bResult;
}

BOOL KTeamClient::IsPlayerInTeam(DWORD dwPlayerID)
{
    BOOL                            bResult = false;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    for (int nGroupIndex = 0; nGroupIndex < m_nGroupNum; nGroupIndex++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[nGroupIndex].MemberList.begin();
            it != m_TeamGroup[nGroupIndex].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == dwPlayerID)
            {
                bResult = true;
                goto Exit0;
            }
        }
    }

Exit0:
    return bResult;
}

KTEAM_MEMBER_INFO* KTeamClient::GetMemberInfo(DWORD dwPlayerID)
{
    KTEAM_MEMBER_INFO* pResult = NULL;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    for (int nGroupIndex = 0; nGroupIndex < m_nGroupNum; nGroupIndex++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[nGroupIndex].MemberList.begin();
            it != m_TeamGroup[nGroupIndex].MemberList.end(); ++it
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

KTEAM_MEMBER_INFO* KTeamClient::GetMemberInfoByName(const char cszMemberName[])
{
    KTEAM_MEMBER_INFO* pResult = NULL;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    for (int nGroupIndex = 0; nGroupIndex < m_nGroupNum; nGroupIndex++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[nGroupIndex].MemberList.begin();
            it != m_TeamGroup[nGroupIndex].MemberList.end(); ++it
        )
        {
            if (STR_CASE_CMP(it->szRoleName, cszMemberName) == 0)
            {
                pResult = &(*it);
                goto Exit0;
            }
        }
    }

Exit0:
    return pResult;
}

void KTeamClient::Reset()
{
    m_dwTeamID              = ERROR_ID;
    m_nGroupNum             = MIN_TEAM_GROUP_NUM;
    m_nLootMode             = 0;
    m_nRollQuality          = 0;
    m_eCamp                 = cNeutral;
    m_bSystem               = false;
    memset(m_dwAuthority, 0, sizeof(m_dwAuthority));
    memset(m_dwTeamMark, 0, sizeof(m_dwTeamMark));

    for (int i = 0; i < MAX_TEAM_GROUP_NUM; i++)
    {
        m_TeamGroup[i].dwFormationLeader = ERROR_ID;
        m_TeamGroup[i].MemberList.clear();
    }
}

BOOL KTeamClient::SetFormationLeader(int nGroupIndex, DWORD dwFormationLeader)
{
    BOOL        bResult = false;

    KGLOG_PROCESS_ERROR(nGroupIndex < m_nGroupNum);

    for (
        KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[nGroupIndex].MemberList.begin();
        it != m_TeamGroup[nGroupIndex].MemberList.end(); ++it
    )
    {
        if (it->dwMemberID == dwFormationLeader)
        {
            m_TeamGroup[nGroupIndex].dwFormationLeader = dwFormationLeader;

            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    return bResult;
}

BOOL KTeamClient::SetTeamMark(int nMarkType, DWORD dwTargetID)
{
    BOOL        bResult     = false;
    KCharacter* pCharacter  = NULL;

    for (int i = 0; i < PARTY_MARK_COUNT; i++)
    {
        if (m_dwTeamMark[i] == dwTargetID)
        {
            m_dwTeamMark[i] = 0;
        }
    }

    KG_PROCESS_SUCCESS(nMarkType == 0); // 0表示取消标记
    KGLOG_PROCESS_ERROR(nMarkType > 0 && nMarkType <= PARTY_MARK_COUNT);

    if (IS_PLAYER(m_dwTeamMark[nMarkType - 1]))
    {
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(m_dwTeamMark[nMarkType - 1]);
    }
    else
    {
        pCharacter = g_pSO3World->m_NpcSet.GetObj(m_dwTeamMark[nMarkType - 1]);
    }
    if (pCharacter && g_pGameWorldRepresentHandler) // 取不到Character是很正常的逻辑
    {
        m_dwTeamMark[nMarkType - 1] = 0;
        g_pGameWorldRepresentHandler->OnCharacterUpdateTitleMark(pCharacter);
    }

    m_dwTeamMark[nMarkType - 1] = dwTargetID;

Exit1:
    if (IS_PLAYER(dwTargetID))
    {
        pCharacter = g_pSO3World->m_PlayerSet.GetObj(dwTargetID);
    }
    else
    {
        pCharacter = g_pSO3World->m_NpcSet.GetObj(dwTargetID);
    }

    if (pCharacter && g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterUpdateTitleMark(pCharacter);
    }

    bResult = true;
Exit0:
    return bResult;
}

//////////////////////////////////////////////////////////////////////////

int KTeamClient::LuaInviteJoinTeam(Lua_State* L)
{
    int         nRetCode        = 0;
    char*       pszInviteDst    = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pszInviteDst = (char *)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszInviteDst);

    g_PlayerClient.DoInviteJoinTeamRequest(pszInviteDst);

Exit0:
    return 0;
}

int KTeamClient::LuaRespondTeamInvite(Lua_State* L)
{
    int         nRetCode        = 0;
    char*       pszInviteSrc    = NULL;
    BOOL        bAgreeFlag      = false;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    pszInviteSrc = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszInviteSrc);

    bAgreeFlag = (BOOL)Lua_ValueToNumber(L, 2);

    g_PlayerClient.DoInviteJoinTeamRespond(pszInviteSrc, bAgreeFlag);

Exit0:
    return 0;
}

int KTeamClient::LuaApplyJoinTeam(Lua_State* L)
{
    int         nRetCode        = 0;
    char*       pszApplyDst     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pszApplyDst = (char *)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszApplyDst);

    g_PlayerClient.DoApplyJoinTeamRequest(pszApplyDst);

Exit0:
    return 0;
}

int KTeamClient::LuaRespondTeamApply(Lua_State* L)
{
    int         nRetCode        = 0;
    char*       pszApplySrc     = NULL;
    BOOL        bAgreeFlag      = false;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    pszApplySrc = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszApplySrc);

    bAgreeFlag = (BOOL)Lua_ValueToNumber(L, 2);

    g_PlayerClient.DoApplyJoinTeamRespond(pszApplySrc, bAgreeFlag);

Exit0:
    return 0;
}

int KTeamClient::LuaRequestLeaveTeam(Lua_State* L)
{
    int         nRetCode        = 0;

    nRetCode = Lua_GetTopIndex(L);
    KG_ASSERT_EXIT(nRetCode == 0);

    nRetCode = g_PlayerClient.DoLeaveTeamRequest();
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KTeamClient::LuaTeamKickoutMember(Lua_State* L)
{
    int         nRetCode        = 0;
    char*       pszMemberName   = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KG_ASSERT_EXIT(nRetCode == 1);

    pszMemberName = (char *)Lua_ValueToString(L, 1);
    KG_ASSERT_EXIT(pszMemberName);

    nRetCode = g_PlayerClient.DoKickoutTeamMember(pszMemberName);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KTeamClient::LuaChangeTeamLeader(Lua_State* L)
{
    int                 nRetCode        = 0;
    char*               pszNewLeader    = NULL;
    KTEAM_MEMBER_INFO*  pMemberInfo     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KG_ASSERT_EXIT(nRetCode == 1);

    pszNewLeader = (char *)Lua_ValueToString(L, 1);
    KG_ASSERT_EXIT(pszNewLeader);

    pMemberInfo = GetMemberInfoByName(pszNewLeader);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    g_PlayerClient.DoTeamChangeAuthorityRequest(tatLeader, pMemberInfo->dwMemberID);
    g_PlayerClient.DoTeamChangeAuthorityRequest(tatMark, pMemberInfo->dwMemberID);

Exit0:
    return 0;
}

int KTeamClient::LuaSetTeamLootMode(Lua_State* L)
{
    int         nRetCode      = 0;
    int         nLootMode     = 0;
    DWORD		dwID		  = g_pSO3World->m_dwClientPlayerID;
    KPlayer*    pPlayer       = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KG_ASSERT_EXIT(nRetCode == 1);

    nLootMode = (int)Lua_ValueToNumber(L, 1);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nRetCode = g_PlayerClient.DoTeamSetLootMode(nLootMode, pPlayer);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KTeamClient::LuaSetTeamRollQuality(Lua_State* L)
{
    int			nRetCode	= 0;
    int			nQuality	= 0;
    DWORD		dwID		= g_pSO3World->m_dwClientPlayerID;
    KPlayer*    pPlayer     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nQuality = (int)Lua_ValueToNumber(L, 1);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwID);
    KGLOG_PROCESS_ERROR(pPlayer);

    nRetCode = g_PlayerClient.DoTeamSetRollQuality(nQuality, pPlayer);
    KG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KTeamClient::LuaTeamNotifySignpost(Lua_State* L)
{
    int   nRetCode = 0;
    int	  nX       = 0;
    int   nY       = 0;

    nRetCode = Lua_GetTopIndex(L);
    KG_ASSERT_EXIT(nRetCode == 2);

    nX = (int)Lua_ValueToNumber(L, 1);
    nY = (int)Lua_ValueToNumber(L, 2);

    nRetCode = g_PlayerClient.DoTeamMemberNotifySignpost(nX, nY);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KTeamClient::LuaGetTeamSize(Lua_State* L)
{
    int   nResult   = 0;
    int   nTopIndex = 0;
    DWORD nTeamSize = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    for (int i = 0; i < m_nGroupNum; i++)
    {
        nTeamSize += (int)m_TeamGroup[i].MemberList.size();
    }

    Lua_PushNumber(L, nTeamSize);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetTeamMemberList(Lua_State* L)
{
    int nResult     = 0;
    int nRetCode    = 0;
    int nToble      = 0;                                
    int nIndex      = 1;
    int nTopIndex   = 0;
    
    nTopIndex = Lua_GetTopIndex(L);
    // KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_NewTable(L);
    int nTable = Lua_GetTopIndex(L);

    for (int i = 0; i < m_nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[i].MemberList.begin();
            it != m_TeamGroup[i].MemberList.end(); ++it
        )
        {
            Lua_PushNumber(L, nIndex);
            Lua_PushNumber(L, it->dwMemberID);
            Lua_SetTable(L, nTable);

            nIndex++;
        }
    }

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetTeamOnlineMemberCount(Lua_State* L)
{
    int nCount      = 0;
    int nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    for (int i = 0; i < m_nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[i].MemberList.begin();
            it != m_TeamGroup[i].MemberList.end(); ++it
        )
        {
            if (it->bOnlineFlag)
                nCount++;
        }
    }

Exit0:
    Lua_PushNumber(L, nCount);
    return 1;
}

int KTeamClient::LuaCanSetTeamDistributeMan(Lua_State* L)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KPlayer*    pClientPlayer   = NULL;

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    KGLOG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    bRetCode = (
        pClientPlayer->m_dwID == m_dwAuthority[tatDistribute] ||
        pClientPlayer->m_dwID == m_dwAuthority[tatLeader]
    );
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTeamClient::LuaCanSetTeamFormationLeader(Lua_State* L)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KPlayer*    pClientPlayer   = NULL;

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pClientPlayer);

    KGLOG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    KG_PROCESS_ERROR(pClientPlayer->m_dwID == m_dwAuthority[tatLeader]);

    for (int i = 0; i < m_nGroupNum; i++)
    {
        if (pClientPlayer->m_dwID == m_TeamGroup[i].dwFormationLeader)
        {
            bResult = true;
            goto Exit0;
        }
    }

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTeamClient::LuaSetTeamDistributeMan(Lua_State* L)
{
    int                     nResult             = 0;
    int                     nTopIndex           = 0;
    DWORD                   dwNewDistributeMan  = ERROR_ID;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwNewDistributeMan = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwAuthority[tatDistribute] != dwNewDistributeMan);

    g_PlayerClient.DoTeamChangeAuthorityRequest(tatDistribute, dwNewDistributeMan);

    nResult = 0;
Exit0:
    return nResult;
}

int KTeamClient::LuaSetTeamFormationLeader(Lua_State* L)
{
    int         nResult              = 0;
    KPlayer*    pClientPlayer        = NULL;
    int         nTopIndex            = 0;
    DWORD       dwNewFormationLeader = ERROR_ID;
    int         nGroupIndex          = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1 || nTopIndex == 2);

    dwNewFormationLeader = (DWORD)Lua_ValueToNumber(L, 1);
    if (nTopIndex == 2)
    {
        nGroupIndex      = (int)Lua_ValueToNumber(L, 2);
    }

    pClientPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(pClientPlayer);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    KG_PROCESS_ERROR(m_TeamGroup[nGroupIndex].dwFormationLeader != dwNewFormationLeader);

    g_PlayerClient.DoTeamSetFormationLeader(dwNewFormationLeader, nGroupIndex, pClientPlayer);

    nResult = 0;
Exit0:
    return nResult;
}

int KTeamClient::LuaIsPlayerInTeam(Lua_State* L)
{
    BOOL            bResult     = false;
    int             nTopIndex   = 0;
    DWORD           dwPlayerID  = 0;
    KTEAM_MEMBER_INFO*   pMemberinfo = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);

    pMemberinfo = GetMemberInfo(dwPlayerID);
    KG_PROCESS_ERROR(pMemberinfo);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTeamClient::LuaGetClientTeamMemberName(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushString(L, pMemberInfo->szRoleName);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberPortraitID(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L, pMemberInfo->PortraitInfo.dwHelmStyle);
    Lua_PushNumber(L, pMemberInfo->PortraitInfo.dwHelmColor);
    Lua_PushNumber(L, pMemberInfo->PortraitInfo.dwFaceStyle);
    Lua_PushNumber(L, pMemberInfo->PortraitInfo.dwHairStyle);

    Lua_PushNumber(L, pMemberInfo->nRoleType);

    nResult = 5;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberRoleType(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L, pMemberInfo->nRoleType);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberOnlineFlag(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushBoolean(L, pMemberInfo->bOnlineFlag);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberMapID(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L, pMemberInfo->dwMapID);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberLevel(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L, pMemberInfo->nLevel);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberMaxLife(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L, pMemberInfo->nMaxLife);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberMaxMana(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L, pMemberInfo->nMaxMana);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberCurrentLife(Lua_State* L)
{
    int             nResult      = 0;
    int             nRetCode     = 0;
    int             nCurrentLife = 0;
    DWORD           dwMemberID   = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo  = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    nCurrentLife = pMemberInfo->nMaxLife * pMemberInfo->nLifePercent / UCHAR_MAX;

    Lua_PushNumber(L, nCurrentLife);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberCurrentMana(Lua_State* L)
{
    int             nResult      = 0;
    int             nRetCode     = 0;
    int             nCurrentMana = 0;
    DWORD           dwMemberID   = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo  = NULL;


    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    nCurrentMana = pMemberInfo->nMaxMana * pMemberInfo->nManaPercent / UCHAR_MAX;

    Lua_PushNumber(L, nCurrentMana);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberCurrentPosition(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L,  pMemberInfo->nPosX);
    Lua_PushNumber(L,  pMemberInfo->nPosY);

    nResult = 2;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberDeathFlag(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushBoolean(L, pMemberInfo->bDeathFlag);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberFormationCoefficient(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L, pMemberInfo->nFormationCoefficient);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetClientTeamMemberForceID(Lua_State* L)
{
    int             nResult     = 0;
    int             nRetCode    = 0;
    DWORD           dwMemberID  = 0;
    KTEAM_MEMBER_INFO*   pMemberInfo = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_PushNumber(L, pMemberInfo->dwForceID);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetTeamMark(Lua_State *L)
{
    int         nResult = 0;

    KGLOG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    lua_newtable(L);
    for (int i = 0; i < _countof(m_dwTeamMark); i++)
    {
        if (m_dwTeamMark[i] == 0)
            continue;

        Lua_PushNumber(L, m_dwTeamMark[i]);
        Lua_PushNumber(L, i + 1);	// 0 = Cancel, so MarkID begin with 1
        Lua_SetTable(L, -3);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaSetTeamMark(Lua_State* L)
{
    BOOL        bResult    = false;
    int         nTopIndex  = 0;
    int         nMarkType  = 0;
    DWORD       dwTargetID = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nMarkType  = (int)Lua_ValueToNumber(L, 1);
    dwTargetID = (DWORD)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nMarkType >= 0 && nMarkType <= PARTY_MARK_COUNT);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    g_PlayerClient.DoTeamSetMarkRequest(nMarkType, dwTargetID);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTeamClient::LuaGetGroupInfo(Lua_State* L)
{
    int nResult     = 0;
    int nTopIndex   = 0;
    int nTable      = 0;
    int nSubTable   = 0;
    int nTeamGroup  = 0;
    int nIndex      = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    KGLOG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    nTeamGroup = (int)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(nTeamGroup >= 0 && nTeamGroup < m_nGroupNum);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    Lua_PushString(L, "dwFormationLeader");
    Lua_PushNumber(L, m_TeamGroup[nTeamGroup].dwFormationLeader);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "MemberList");
    Lua_NewTable(L);
    nSubTable = Lua_GetTopIndex(L);

    for (
        KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[nTeamGroup].MemberList.begin();
        it != m_TeamGroup[nTeamGroup].MemberList.end(); ++it
    )
    {
        Lua_PushNumber(L, ++nIndex);
        Lua_PushNumber(L, it->dwMemberID);
        Lua_SetTable(L, nSubTable);
    }

    Lua_SetTable(L, nTable);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetMemberInfo(Lua_State* L)
{
    int                 nResult         = 0;
    int                 nTopIndex       = 0;
    int                 nTable          = 0;
    int                 nCurrentLife    = 0;
    int                 nCurrentMana    = 0;
    DWORD               dwMemberID      = 0;
    KTEAM_MEMBER_INFO*  pMemberInfo     = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    KGLOG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    pMemberInfo = GetMemberInfo(dwMemberID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    Lua_PushString(L, "szName");
    Lua_PushString(L, pMemberInfo->szRoleName);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nLevel");
    Lua_PushNumber(L, pMemberInfo->nLevel);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "bIsOnLine");
    Lua_PushBoolean(L, pMemberInfo->bOnlineFlag);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "bDeathFlag");
    Lua_PushBoolean(L, pMemberInfo->bDeathFlag);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nCamp");
    Lua_PushNumber(L, pMemberInfo->eCamp);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "dwForceID");
    Lua_PushNumber(L, pMemberInfo->dwForceID);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nMaxLife");
    Lua_PushNumber(L, pMemberInfo->nMaxLife);
    Lua_SetTable(L, nTable);

    nCurrentLife = pMemberInfo->nMaxLife * pMemberInfo->nLifePercent / UCHAR_MAX;
    Lua_PushString(L, "nCurrentLife");
    Lua_PushNumber(L, nCurrentLife);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nMaxMana");
    Lua_PushNumber(L, pMemberInfo->nMaxMana);
    Lua_SetTable(L, nTable);

    nCurrentMana = pMemberInfo->nMaxMana * pMemberInfo->nManaPercent / UCHAR_MAX;
    Lua_PushString(L, "nCurrentMana");
    Lua_PushNumber(L, nCurrentMana);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "dwMapID");
    Lua_PushNumber(L, pMemberInfo->dwMapID);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nMapCopyIndex");
    Lua_PushNumber(L, pMemberInfo->nMapCopyIndex);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nPosX");
    Lua_PushNumber(L, pMemberInfo->nPosX);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nPosY");
    Lua_PushNumber(L, pMemberInfo->nPosY);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nRoleType");
    Lua_PushNumber(L, pMemberInfo->nRoleType);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "nFormationCoefficient");
    Lua_PushNumber(L, pMemberInfo->nFormationCoefficient);
    Lua_SetTable(L, nTable);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaGetMemberGroupIndex(Lua_State* L)
{
    int     nResult     = 0;
    int     nTopIndex   = 0;
    DWORD   dwMemberID  = ERROR_ID;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    for (int i = 0; i < m_nGroupNum; i++)
    {
        for (
            KTEAM_MEMBER_LIST::iterator it = m_TeamGroup[i].MemberList.begin();
            it != m_TeamGroup[i].MemberList.end(); ++it
        )
        {
            if (it->dwMemberID == dwMemberID)
            {
                Lua_PushNumber(L, i);
                nResult = 1;
                goto Exit0;
            }
        }
    }

Exit0:
    return nResult;
}

int KTeamClient::LuaLevelUpRaid(Lua_State* L)
{
    int nTopIndex = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    g_PlayerClient.DoTeamLevelUpRaidRequest();
Exit0:
    return 0;
}

int KTeamClient::LuaChangeMemberGroup(Lua_State* L)
{
    int      nTopIndex      = 0;
    DWORD    dwMemberID     = ERROR_ID;
    DWORD    dwDstMemberID  = ERROR_ID;
    int      nDstGroup      = -1;
    KPlayer* pPlayer        = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 3);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);
    nDstGroup  = (int)Lua_ValueToNumber(L, 2);

    if (nTopIndex == 3)
    {
        dwDstMemberID = (DWORD)Lua_ValueToNumber(L, 3);
    }

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    KGLOG_PROCESS_ERROR(nDstGroup >= 0 && nDstGroup < m_nGroupNum);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwID == m_dwAuthority[tatLeader]);

    g_PlayerClient.DoTeamChangeMemberGroupRequest(dwMemberID, nDstGroup, dwDstMemberID);

Exit0:
    return 0;
}

int KTeamClient::LuaGetAuthorityInfo(Lua_State* L)
{
    int     nResult     = 0;
    int     nTopIndex   = 0;
    DWORD   dwAuthority = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwAuthority = (DWORD)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(dwAuthority > tatInvalid && dwAuthority < tatTotal);

    Lua_PushNumber(L, m_dwAuthority[dwAuthority]);

    nResult = 1;
Exit0:
    return nResult;
}

int KTeamClient::LuaSetAuthorityInfo(Lua_State* L)
{
    int                     nTopIndex   = 0;
    KTEAM_AUTHORITY_TYPE    eAuthority  = tatInvalid;
    DWORD                   dwTargetID  = ERROR_ID;
    KTEAM_MEMBER_INFO*      pMemberInfo = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    eAuthority = (KTEAM_AUTHORITY_TYPE)(int)Lua_ValueToNumber(L, 1);
    dwTargetID   = (DWORD)Lua_ValueToNumber(L, 2);

    pMemberInfo = GetMemberInfo(dwTargetID);
    KGLOG_PROCESS_ERROR(pMemberInfo);

    g_PlayerClient.DoTeamChangeAuthorityRequest(eAuthority, dwTargetID);

Exit0:
    return 0;
}

BOOL KGetAllTeamMember::operator ()(KTEAM_MEMBER_INFO* pMemberInfo)
{
    BOOL bResult = false;

    m_vecResult.push_back(*pMemberInfo);

    bResult = true;
Exit0:
    return bResult;
}

DEFINE_LUA_CLASS_BEGIN(KTeamClient)
    REGISTER_LUA_DWORD_READONLY(KTeamClient, TeamID)
    REGISTER_LUA_BOOL_READONLY(KTeamClient, System)
    REGISTER_LUA_DWORD_READONLY(KTeamClient, TeamLeader)
    REGISTER_LUA_INTEGER_READONLY(KTeamClient, LootMode)
    REGISTER_LUA_INTEGER_READONLY(KTeamClient, RollQuality)
    REGISTER_LUA_ENUM_READONLY(KTeamClient, Camp)
    REGISTER_LUA_DWORD_READONLY(KTeamClient, DistributeMan)
    REGISTER_LUA_DWORD_READONLY(KTeamClient, FormationLeader)
    REGISTER_LUA_INTEGER_READONLY(KTeamClient, GroupNum)

    REGISTER_LUA_FUNC(KTeamClient, InviteJoinTeam)
    REGISTER_LUA_FUNC(KTeamClient, RespondTeamInvite)
    REGISTER_LUA_FUNC(KTeamClient, ApplyJoinTeam)
    REGISTER_LUA_FUNC(KTeamClient, RespondTeamApply)
    REGISTER_LUA_FUNC(KTeamClient, RequestLeaveTeam)
    REGISTER_LUA_FUNC(KTeamClient, TeamKickoutMember)
    REGISTER_LUA_FUNC(KTeamClient, ChangeTeamLeader)
    REGISTER_LUA_FUNC(KTeamClient, SetTeamLootMode)
    REGISTER_LUA_FUNC(KTeamClient, SetTeamRollQuality)
    REGISTER_LUA_FUNC(KTeamClient, TeamNotifySignpost)
    REGISTER_LUA_FUNC(KTeamClient, GetTeamSize)
    REGISTER_LUA_FUNC(KTeamClient, GetTeamMemberList)
    REGISTER_LUA_FUNC(KTeamClient, GetTeamOnlineMemberCount)
    REGISTER_LUA_FUNC(KTeamClient, CanSetTeamDistributeMan)
    REGISTER_LUA_FUNC(KTeamClient, SetTeamDistributeMan)
    REGISTER_LUA_FUNC(KTeamClient, CanSetTeamFormationLeader)
    REGISTER_LUA_FUNC(KTeamClient, SetTeamFormationLeader)
    REGISTER_LUA_FUNC(KTeamClient, IsPlayerInTeam)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberName)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberPortraitID)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberRoleType)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberOnlineFlag)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberMapID)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberLevel)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberMaxLife)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberMaxMana)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberCurrentLife)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberCurrentMana)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberCurrentPosition)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberDeathFlag)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberFormationCoefficient)
    REGISTER_LUA_FUNC(KTeamClient, GetClientTeamMemberForceID)
    REGISTER_LUA_FUNC(KTeamClient, GetTeamMark)
    REGISTER_LUA_FUNC(KTeamClient, SetTeamMark)
    REGISTER_LUA_FUNC(KTeamClient, GetGroupInfo)
    REGISTER_LUA_FUNC(KTeamClient, GetMemberInfo)
    REGISTER_LUA_FUNC(KTeamClient, GetMemberGroupIndex)
    REGISTER_LUA_FUNC(KTeamClient, LevelUpRaid)
    REGISTER_LUA_FUNC(KTeamClient, ChangeMemberGroup)
    REGISTER_LUA_FUNC(KTeamClient, GetAuthorityInfo)
    REGISTER_LUA_FUNC(KTeamClient, SetAuthorityInfo)
DEFINE_LUA_CLASS_END(KTeamClient)
#endif

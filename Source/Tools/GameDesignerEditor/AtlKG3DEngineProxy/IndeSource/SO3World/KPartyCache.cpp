#include "stdafx.h"
#include "KMath.h"
#include "KGPublic.h"
#include "KSO3World.h"
#include "KPartyCache.h"
#include "KRelayClient.h"
#include "Engine/KGLog.h"
#include "KScene.h"
#include "KPlayer.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"

#ifdef _SERVER
KPartyCache::KPartyCache()
{
    m_pPartys          = NULL;
}

BOOL KPartyCache::Init()
{
    BOOL bResult = false;
    DWORD dwIndex = 0;

    assert(MAX_PARTY_COUNT < 0xffff);

    m_pPartys = new KPARTY_INFO[MAX_PARTY_COUNT];
    KG_PROCESS_ERROR(m_pPartys);

    for (dwIndex = 0; dwIndex < MAX_PARTY_COUNT; ++dwIndex) 
    {
        m_pPartys[dwIndex].dwPartyLeader = 0;
        m_pPartys[dwIndex].dwPartyID     = INVALID_PARTY_ID;
        m_pPartys[dwIndex].PlayerList.reserve(MAX_PARTY_SIZE);
        memset(m_pPartys[dwIndex].dwPartyMark, 0, sizeof(m_pPartys[dwIndex].dwPartyMark));
    }

    bResult = true;
Exit0:
    if (!bResult) 
    {
        KG_DELETE_ARRAY(m_pPartys);
    }
    return bResult;
}

void KPartyCache::UnInit()
{
    KG_DELETE_ARRAY(m_pPartys);
}

KPARTY_INFO* KPartyCache::GetParty(DWORD dwPartyID)
{
    KPARTY_INFO*    pRetParty       = NULL;
    KPARTY_INFO*    pParty          = NULL;
    DWORD           dwPartyIndex    = 0;

    dwPartyIndex = dwPartyID >> 16;
    KG_PROCESS_ERROR(dwPartyIndex < MAX_PARTY_COUNT);

    pParty = m_pPartys + dwPartyIndex;
    KG_PROCESS_ERROR(pParty->dwPartyID != INVALID_PARTY_ID);
    KGLOG_PROCESS_ERROR(pParty->dwPartyID == dwPartyID);

    pRetParty = pParty;
Exit0:
    return pRetParty;
}

int KPartyCache::CreateParty(
    DWORD dwPartyID, DWORD dwLeaderID, int nLootMode, int nRollQuality, DWORD dwFormationLeader, DWORD dwDistributeMan
)
{
    int          nResult        = false;
    KPARTY_INFO* pParty         = NULL;
    KPlayer*     pLeader        = NULL;
    DWORD        dwPartyIndex   = dwPartyID >> 16;

    KGLOG_PROCESS_ERROR(dwPartyIndex < MAX_PARTY_COUNT);
    pParty = m_pPartys + dwPartyIndex;
    
    KGLOG_PROCESS_ERROR(pParty->dwPartyID == INVALID_PARTY_ID);
    KGLOG_PROCESS_ERROR(pParty->PlayerList.size() == 0);
    pParty->PlayerList.clear();

    pParty->dwPartyID               = dwPartyID;
    pParty->nLootMode               = nLootMode;
    pParty->dwPartyLeader           = dwLeaderID;
	pParty->nRollQuality			= nRollQuality;
    pParty->dwFormationLeader       = dwFormationLeader;
    pParty->dwDistributeMan         = dwDistributeMan;

    nResult = true;
Exit0:
    return nResult;
}


int KPartyCache::PartyAddMember(DWORD dwPartyID, DWORD dwMemberID, const KPARTY_MEMBER_INFO &crMemberInfo)
{
    int             nResult         = false;
    KPARTY_INFO*    pParty          = NULL;
    KPlayer*        pMember         = NULL;
    KPARTY_PLAYER_LIST::iterator it;

    // 这里不检查m_bPartyDataValidFlag标志是因为同步队伍数据时也会调用这个方法,而那时候m_bPartyDataValidFlag应该是 false
    // 而普通的队伍添加成员的通知时,必须检查m_bPartyDataValidFlag标志,必须为true才能添加.
    // 所以,对这个标志的检查都是放在函数调用者处的

    pParty = GetParty(dwPartyID);
    KGLOG_PROCESS_ERROR(pParty);

    pParty->PlayerList.push_back(crMemberInfo);

    pMember = g_pSO3World->m_PlayerSet.GetObj(dwMemberID);
    if (pMember)
    {
        pMember->m_dwTeamID            = dwPartyID;
    }

    nResult = true;
Exit0:
    return nResult;
}


int KPartyCache::PartyDelMember(DWORD dwPartyID, DWORD dwPlayerID)
{
    int                                 nResult         = false;
    KPARTY_PLAYER_LIST::iterator        it;
    KPARTY_INFO*                        pParty          = NULL;
	KPlayer*                            pPlayer         = NULL;
    size_t                              uSize           = 0;

    pParty = GetParty(dwPartyID);
    KGLOG_PROCESS_ERROR(pParty);

    // 玩家不能是队长
    KGLOG_PROCESS_ERROR(dwPlayerID != pParty->dwPartyLeader);

    for (it = pParty->PlayerList.begin(); it != pParty->PlayerList.end(); ++it)
    {
        if (it->dwMemberID == dwPlayerID)
            break;
    }

    KGLOG_PROCESS_ERROR(it != pParty->PlayerList.end());

    uSize = pParty->PlayerList.size();
    *it = pParty->PlayerList[uSize - 1];
    pParty->PlayerList.resize(uSize - 1);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    if (pPlayer)
    {
        pPlayer->m_dwTeamID = INVALID_PARTY_ID;
    }
    //else: 玩家不在本地

    nResult = true;
Exit0:
    return nResult;
}


int KPartyCache::DestroyParty(DWORD dwPartyID)
{
    int                                 nResult         = false;
    DWORD                               dwPartyIndex    = 0;
    KPARTY_INFO*                        pParty          = NULL;
	KPlayer*                            pPlayer         = NULL;
    KPARTY_PLAYER_LIST::const_iterator  it;

    pParty = GetParty(dwPartyID);
    KGLOG_PROCESS_ERROR(pParty); // 当玩家刚上线，此时被踢出队伍，这里就找不到队伍

    for (it = pParty->PlayerList.begin(); it != pParty->PlayerList.end(); ++it) 
    {
        DWORD dwPlayerID = it->dwMemberID;
        pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
        // 如果玩家下线了或者不在这个服务器上,则这里会找不到
        if (!pPlayer)
            continue;
        pPlayer->m_dwTeamID = INVALID_PARTY_ID;
    }

    // Destroy
    pParty->dwPartyLeader = 0;
    pParty->dwPartyID     = INVALID_PARTY_ID;
    pParty->PlayerList.clear();

    // 小队标记清楚，如果是客户端，则通知表现
    for (int i = 0; i < PARTY_MARK_COUNT; i++)
    {
        KCharacter* pClearCharacter = NULL;
        if (!pParty->dwPartyMark[i])
            continue;

        pParty->dwPartyMark[i] = 0;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KPartyCache::PartyChangeLeader(DWORD dwPartyID, DWORD dwNewLeaderID)
{
    int             nResult         = false;
    KPARTY_INFO*    pParty          = NULL;
    KPARTY_PLAYER_LIST::const_iterator   it;

    pParty = GetParty(dwPartyID);
    KGLOG_PROCESS_ERROR(pParty);

    for (it = pParty->PlayerList.begin(); it != pParty->PlayerList.end(); ++it)
    {
        if (it->dwMemberID == dwNewLeaderID)
            break;
    }

    KG_ASSERT_EXIT(it != pParty->PlayerList.end());
    
    pParty->dwPartyLeader = dwNewLeaderID;

    nResult = true;
Exit0:
    return nResult;
}

int KPartyCache::SetPartyLootMode(DWORD dwPartyID, int nLootMode)
{
    int          nResult         = false;
    KPARTY_INFO* pParty          = NULL;

    pParty = GetParty(dwPartyID);
    KGLOG_PROCESS_ERROR(pParty);

    pParty->nLootMode = nLootMode;

    nResult = true;
Exit0:
    return nResult;
}

int KPartyCache::SetPartyRollQuality(DWORD dwPartyID, int nRollQuality)
{
	int			    nResult	= false;
	KPARTY_INFO*    pParty	= NULL;

	pParty = GetParty(dwPartyID);
	KGLOG_PROCESS_ERROR(pParty);

	pParty->nRollQuality = nRollQuality;

	nResult = true;
Exit0:
	return nResult;
}

int KPartyCache::GetPartyLootMode(DWORD dwPartyID)
{
	int			    nResult	= 0;
	KPARTY_INFO*    pParty	= NULL;

	pParty = GetParty(dwPartyID);
	KGLOG_PROCESS_ERROR(pParty);

	nResult = pParty->nLootMode;
Exit0:
	return nResult;
}

int KPartyCache::GetPartyRollQuality(DWORD dwPartyID)
{	
	int			    nResult	= 0;
	KPARTY_INFO*    pParty	= NULL;

	pParty = GetParty(dwPartyID);
	KGLOG_PROCESS_ERROR(pParty);

	nResult = pParty->nRollQuality;
Exit0:
	return nResult;
}

int KPartyCache::SetPartyMemberOnlineFlag(DWORD dwPartyID, DWORD dwMemberID, BOOL bOnlineFlag)
{
    int             nResult = false;
    KPARTY_INFO*    pParty  = NULL;
    KPARTY_PLAYER_LIST::iterator   it;

    pParty = GetParty(dwPartyID);
    KGLOG_PROCESS_ERROR(pParty);

    for (it = pParty->PlayerList.begin(); it != pParty->PlayerList.end(); ++it)
    {
        if (it->dwMemberID == dwMemberID)
            break;
    }

    KG_ASSERT_EXIT(it != pParty->PlayerList.end());

    it->bOnlineFlag = bOnlineFlag;    

    nResult = true;
Exit0:
    return nResult;
}

BOOL KPartyCache::GetPartyMemberOnlineFlag(DWORD dwPartyID, DWORD dwMemberID, BOOL* pbOnlineFlag)
{
    BOOL            bResult = false;
    KPARTY_INFO*    pParty  = NULL;
    KPARTY_PLAYER_LIST::iterator   it;

    pParty = GetParty(dwPartyID);
    KGLOG_PROCESS_ERROR(pParty);

    for (it = pParty->PlayerList.begin(); it != pParty->PlayerList.end(); ++it)
    {
        if (it->dwMemberID == dwMemberID)
            break;
    }

    KGLOG_PROCESS_ERROR(it != pParty->PlayerList.end());

    *pbOnlineFlag = it->bOnlineFlag;    

    bResult = true;
Exit0:
    return bResult;
}

BOOL KPartyCache::IsPlayerInParty(DWORD dwPartyID, DWORD dwPlayerID)
{
    BOOL            bResult = false;
    KPARTY_INFO*    pParty  = NULL;
    KPARTY_PLAYER_LIST::iterator   it;

    pParty = GetParty(dwPartyID);
    KGLOG_PROCESS_ERROR(pParty);

    for (it = pParty->PlayerList.begin(); it != pParty->PlayerList.end(); ++it)
    {
        if (it->dwMemberID == dwPlayerID)
        {
            bResult = true;
            break;
        }
    }

Exit0:
    return bResult;
}

BOOL KPartyCache::PartySetDistributeMan(DWORD dwPartyID, DWORD dwNewDistributeMan)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KPARTY_INFO*    pParty	    = NULL;
    
    pParty = GetParty(dwPartyID);
	KGLOG_PROCESS_ERROR(pParty);
    
    bRetCode = IsPlayerInParty(pParty->dwPartyID, dwNewDistributeMan);
    KGLOG_PROCESS_ERROR(bRetCode);

    pParty->dwDistributeMan = dwNewDistributeMan;
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPartyCache::PartySetFormationLeader(DWORD dwPartyID, DWORD dwNewFormationLeader)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KPARTY_INFO*    pParty	    = NULL;
    
    pParty = GetParty(dwPartyID);
	KGLOG_PROCESS_ERROR(pParty);
    
    bRetCode = IsPlayerInParty(pParty->dwPartyID, dwNewFormationLeader);
    KGLOG_PROCESS_ERROR(bRetCode);

    pParty->dwFormationLeader = dwNewFormationLeader;
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPartyCache::SetPartyMark(DWORD dwParty, int nMarkType, DWORD dwTargetID)
{
    BOOL            bResult = false;
    KPARTY_INFO*    pParty  = NULL;

    pParty = GetParty(dwParty);
    KGLOG_PROCESS_ERROR(pParty);

    for (int i = 0; i < PARTY_MARK_COUNT; i++)
    {
        if (pParty->dwPartyMark[i] == dwTargetID)
        {
            pParty->dwPartyMark[i] = 0;
        }
    }

    KG_PROCESS_SUCCESS(nMarkType == 0);
    KGLOG_PROCESS_ERROR(nMarkType > 0 && nMarkType <= PARTY_MARK_COUNT);
    pParty->dwPartyMark[nMarkType - 1] = dwTargetID;

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KPartyCache::ProcessPartyMemberAttraction(DWORD dwPlayerID)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KPlayer*        pPlayer     = NULL;
    KPARTY_INFO*    pPartyInfo  = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_pScene);

    pPartyInfo = GetParty(pPlayer->m_dwTeamID);
    KG_PROCESS_ERROR(pPartyInfo);

    for (int i = 0; i < (int)pPartyInfo->PlayerList.size(); i++)
    {
        const KPARTY_MEMBER_INFO&   crMemberInfo        = pPartyInfo->PlayerList[i];
        KPlayer*                    pMember             = NULL;
        KGFellowship*               pFellowship         = NULL;

        if (crMemberInfo.dwMemberID == dwPlayerID)
            continue;

        if (crMemberInfo.dwMapID != pPlayer->m_pScene->m_dwMapID)
            continue;

        pMember = g_pSO3World->m_PlayerSet.GetObj(crMemberInfo.dwMemberID);
        KG_PROCESS_ERROR(pMember);

        bRetCode = g_InRange(pPlayer, pMember, g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveRange);
        if (!bRetCode)
            continue;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(dwPlayerID, crMemberInfo.dwMemberID);
        if (!pFellowship)
            continue;

        if (pFellowship->m_nAttraction >= g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveUpLimit)
            continue;

        g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(dwPlayerID, crMemberInfo.dwMemberID, g_pSO3World->m_Settings.m_ConstList.nPartyAttractiveAdd, false);
    }

    bResult = true;
Exit0:
    return bResult;
}

#endif

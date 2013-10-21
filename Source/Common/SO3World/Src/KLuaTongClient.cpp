#include "stdafx.h"
#include "KMath.h"
#include "KGPublic.h"
#include "KSO3World.h"

#if defined(_CLIENT)
#include "KPlayer.h"
#include "KTongClient.h"
#include "KPlayerClient.h"

static bool SortTongMemberByNameRise(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return strcmp(pA->szName, pB->szName) < 0;
	return pA->bIsOnline;
}

static bool SortTongMemberByNameDescend(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return strcmp(pA->szName, pB->szName) > 0;
	return pA->bIsOnline;
}

static bool SortTongMemberByLevelRise(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->byLevel < pB->byLevel;
	return pA->bIsOnline;
}

static bool SortTongMemberByLevelDescend(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->byLevel > pB->byLevel;
	return pA->bIsOnline;
}

static bool SortTongMemberByMapRise(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->dwMapID < pB->dwMapID;
	return pA->bIsOnline;
}

static bool SortTongMemberByMapDescend(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->dwMapID > pB->dwMapID;
	return pA->bIsOnline;
}

static bool SortTongMemberByRemarkRise(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return strcmp(pA->szRemark, pB->szRemark) < 0;
	return pA->bIsOnline;
}

static bool SortTongMemberByRemarkDescend(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->szRemark > pB->szRemark;
	return pA->bIsOnline;
}

static bool SortTongMemberByGroupRise(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->nGroupID < pB->nGroupID;
	return pA->bIsOnline;
}

static bool SortTongMemberByGroupDescend(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->nGroupID > pB->nGroupID;
	return pA->bIsOnline;
}

static bool SortTongMemberByJoinTimeRise(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->nJoinTime < pB->nJoinTime;
	return pA->bIsOnline;
}

static bool SortTongMemberByJoinTimeDescend(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->nJoinTime > pB->nJoinTime;
	return pA->bIsOnline;
}

static bool SortTongMemberByForceRise(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->byForceID < pB->byForceID;
	return pA->bIsOnline;
}

static bool SortTongMemberByForceDescend(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->byForceID > pB->byForceID;
	return pA->bIsOnline;
}

static bool SortTongMemberByLastOffLineTimeRise(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->nLastOfflineTime < pB->nLastOfflineTime;
	return pA->bIsOnline;
}

static bool SortTongMemberByLastOffLineTimeDescend(KTongMemberInfo* pA, KTongMemberInfo* pB)
{
	if (pA->bIsOnline == pB->bIsOnline)
		return pA->nLastOfflineTime > pB->nLastOfflineTime;
	return pA->bIsOnline;
}

int KTongClient::LuaGetGroupInfo(Lua_State* L)
{
    int         nResult     = 0;
	int         nRetCode    = 0;
	int			nGroup		= 0;

	nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	nGroup = (int)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(nGroup >= 0 && nGroup < MAX_TONG_GROUP);

	Lua_NewTable(L);

	Lua_PushString(L, "szName");
	Lua_PushString(L, m_Schema.Data[nGroup].szName);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "nWage");
	Lua_PushNumber(L, m_Schema.Data[nGroup].nWage);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bEnable");
	Lua_PushBoolean(L, m_Schema.Data[nGroup].bEnable);
	Lua_SetTable(L, -3);

	nResult = 1;
Exit0:
	return nResult;
}

int KTongClient::LuaGetMemberList(Lua_State* L)
{
    int                             nResult         = 0;
    int                             nRetCode        = 0;
	int                             nIndex			= 0;
	BOOL                            bOffLine		= TRUE;
	const char*                     pcszSort		= NULL;
	BOOL                            bRise			= TRUE;
	int								nGroupFilter	= -1;
	int								nSchoolFilter	= -1;
	std::vector<KTongMemberInfo*>   MemberTable;
    KCLIENT_ROSTER::iterator        iter;

	nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 5);

	bOffLine = Lua_ValueToBoolean(L, 1);

	pcszSort = Lua_ValueToString(L, 2);
	KGLOG_PROCESS_ERROR(pcszSort);

	bRise = Lua_ValueToBoolean(L, 3);

	nGroupFilter = (int)Lua_ValueToNumber(L, 4);
	nSchoolFilter = (int)Lua_ValueToNumber(L, 5);
	//todo:现在还没有同步玩家门派，等同步门派后加上。
    MemberTable.reserve(m_Roster.size());

	if (nGroupFilter >= 0 && nGroupFilter < MAX_TONG_GROUP)
	{
		if (bOffLine)
		{
			for (iter = m_Roster.begin(); iter != m_Roster.end(); ++iter)
			{
				if (iter->second.nGroupID == nGroupFilter)
					MemberTable.push_back(&(iter->second));
			}
		}
		else
		{
			for (iter = m_Roster.begin(); iter != m_Roster.end(); ++iter)
			{
				if (iter->second.bIsOnline && iter->second.nGroupID == nGroupFilter)
					MemberTable.push_back(&(iter->second));
			}
		}
	}
	else if (nSchoolFilter >= 0)
	{
		if (bOffLine)
		{
			for (iter = m_Roster.begin(); iter != m_Roster.end(); ++iter)
			{
				if (iter->second.byForceID == nSchoolFilter)
					MemberTable.push_back(&(iter->second));
			}
		}
		else
		{
			for (iter = m_Roster.begin(); iter != m_Roster.end(); ++iter)
			{
				if (iter->second.bIsOnline && iter->second.byForceID == nSchoolFilter)
				{
					MemberTable.push_back(&(iter->second));
				}
			}
		}
	}
	else
	{
		if (bOffLine)
		{
			for (iter = m_Roster.begin(); iter != m_Roster.end(); ++iter)
				MemberTable.push_back(&(iter->second));
		}
		else
		{
			for (iter = m_Roster.begin(); iter != m_Roster.end(); ++iter)
			{
				if (iter->second.bIsOnline)
					MemberTable.push_back(&(iter->second));
			}
		}
	}

	if (strcmp(pcszSort, "name") == 0)
	{
		if (bRise)
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByNameRise);
		else
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByNameDescend);
	}
	else if (strcmp(pcszSort, "level") == 0)
	{
		if (bRise)
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByLevelRise);
		else
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByLevelDescend);
	}
	else if (strcmp(pcszSort, "school") == 0)
	{
		if (bRise)
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByForceRise);
		else
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByForceDescend);
	}
	else if (strcmp(pcszSort, "map") == 0)
	{
		if (bRise)
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByMapRise);
		else
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByMapDescend);
	}
	else if (strcmp(pcszSort, "remark") == 0)
	{
		if (bRise)
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByRemarkRise);
		else
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByRemarkDescend);
	}
	else if (strcmp(pcszSort, "group") == 0)
	{
		if (bRise)
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByGroupRise);
		else
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByGroupDescend);
	}
	else if (strcmp(pcszSort, "join_time") == 0)
	{
		if (bRise)
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByJoinTimeRise);
		else
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByJoinTimeDescend);
	}
	else if (strcmp(pcszSort, "last_offline_time") == 0)
	{
		if (bRise)
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByLastOffLineTimeRise);
		else
			std::sort(MemberTable.begin(), MemberTable.end(), SortTongMemberByLastOffLineTimeDescend);
	}

	Lua_NewTable(L);

	for (std::vector<KTongMemberInfo*>::iterator it = MemberTable.begin(); it != MemberTable.end(); ++it)
	{
		++nIndex;
		Lua_PushNumber(L, nIndex);
		Lua_PushNumber(L, (*it)->dwID);
		Lua_SetTable(L, -3);
	}
	
	nResult = 1;
Exit0:
	return nResult;
}

int KTongClient::LuaGetMemberInfo(Lua_State* L)
{
    int                         nResult     = 0;
    int                         nRetCode    = 0;
	DWORD                       dwID        = 0;
	struct tm*                  pLocalTime  = NULL;
    const KTongMemberInfo*      pMember     = NULL;
	KCLIENT_ROSTER::iterator    it;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	dwID = (DWORD)Lua_ValueToNumber(L, 1);
	it = m_Roster.find(dwID);
	KG_PROCESS_ERROR(it != m_Roster.end());

    pMember = &it->second;

	Lua_NewTable(L);

	Lua_PushString(L, "szName");
	Lua_PushString(L, pMember->szName);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "dwID");
	Lua_PushNumber(L, pMember->dwID);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "dwMapID");
	Lua_PushNumber(L, pMember->dwMapID);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "bIsOnline");
	Lua_PushBoolean(L, pMember->bIsOnline);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "nLevel");
	Lua_PushNumber(L, pMember->byLevel);
	Lua_SetTable(L, -3);

    Lua_PushString(L, "nForceID");
    Lua_PushNumber(L, pMember->byForceID);
    Lua_SetTable(L, -3);

	Lua_PushString(L, "szRemark");
	Lua_PushString(L, pMember->szRemark);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "nGroupID");
	Lua_PushNumber(L, pMember->nGroupID);
	Lua_SetTable(L, -3);

    Lua_PushString(L, "nJoinTime");
    Lua_PushNumber(L, pMember->nJoinTime);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "nLastOfflineTime");
    Lua_PushNumber(L, pMember->nLastOfflineTime);
    Lua_SetTable(L, -3);

	nResult = 1;
Exit0:
	return nResult;
}

int KTongClient::LuaGetDefaultGroupID(Lua_State* L)
{
	Lua_PushNumber(L, m_Schema.nDefaultGroup);
	return 1;
}

int KTongClient::LuaGetMasterGroupID(Lua_State* L)
{
	Lua_PushNumber(L, m_Schema.nMasterGroup);
	return 1;
}

int KTongClient::LuaGetStateTimer(Lua_State* L)
{
    Lua_PushNumber(L, m_nStateTimer);
    return 1;
}

int KTongClient::LuaGetMemorabilia(Lua_State* L)
{
	Lua_NewTable(L);

    for (int i = 0 ; i < (int)m_Memorabilia.size(); i++)
    {
		Lua_PushNumber(L, i + 1);
        Lua_NewTable(L);

		Lua_PushString(L, "dwID");
		Lua_PushNumber(L, m_Memorabilia[i].dwID);
		Lua_SetTable(L, -3);

	    Lua_PushString(L, "nTime");
        Lua_PushNumber(L, m_Memorabilia[i].nTime);
	    Lua_SetTable(L, -3);

	    Lua_PushString(L, "szDescription");
        Lua_PushString(L, m_Memorabilia[i].szDescription);
	    Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }

    return 1;
}

int KTongClient::LuaGetRepertoryItem(Lua_State* L)
{
    int     nResult     = 0;
    int     nTopIndex   = 0;
    int     nPageIndex  = 0;
    int     nPagePos    = 0;
    KItem*  pItem       = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nPageIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);

    nPagePos = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nPagePos >= 0 && nPagePos < TONG_REPERTORY_PAGE_CAPACITY);

    pItem = m_RepertoryCache.m_Pages[nPageIndex].m_pItems[nPagePos];
    KG_PROCESS_ERROR(pItem);

    nResult = pItem->LuaGetObj(L);
Exit0:
    return nResult;
}

int KTongClient::LuaCanBaseOperate(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = 0;
    int     nGroupIndex     = 0;
    int     nOperationType  = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

    nGroupIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < MAX_TONG_GROUP);

    nOperationType = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nOperationType > totInvalid && nOperationType < totTotal);

    bResult = CanBaseOperate(nGroupIndex, (KTONG_OPERATION_TYPE)nOperationType);

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTongClient::LuaCanAdvanceOperate(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = 0;
    int     nGroup          = 0;
    int     nTargetGroup    = 0;
    int     nOperationType  = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 3);

    nGroup = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroup >= 0 && nGroup < MAX_TONG_GROUP);

    nTargetGroup = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    nOperationType = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nOperationType > totInvalid && nOperationType < totTotal);

    bResult = CanAdvanceOperate(nGroup, nTargetGroup, (KTONG_OPERATION_TYPE)nOperationType);

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTongClient::LuaCanBaseGrant(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = 0;
    int     nGroup          = 0;
    int     nTargetGroup    = 0;
    int     nOperationIndex = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 3);

    nGroup = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroup >= 0 && nGroup < MAX_TONG_GROUP);

    nTargetGroup = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    nOperationIndex = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nOperationIndex >= 0 && nOperationIndex < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byBaseGrantMask[nTargetGroup] & (1 << nOperationIndex);

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTongClient::LuaCanAdvanceGrant(Lua_State* L)
{
    int     nRetCode        = 0;
    BOOL    bResult         = false;
    int     nGroup          = 0;
    int     nTargetGroupA   = 0;
    int     nTargetGroupB   = 0; 
    int     nOperationIndex = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);

    nGroup = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroup >= 0 && nGroup < MAX_TONG_GROUP);

    nTargetGroupA = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nTargetGroupA >= 0 && nTargetGroupA < MAX_TONG_GROUP);

    nTargetGroupB = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nTargetGroupB >= 0 && nTargetGroupB < MAX_TONG_GROUP);

    nOperationIndex = (int)Lua_ValueToNumber(L, 4);
    KGLOG_PROCESS_ERROR(nOperationIndex >= 0 && nOperationIndex < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byAdvanceGrantMask[nTargetGroupA][nTargetGroupB] & (1 << nOperationIndex);

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}


int KTongClient::LuaCheckBaseOperationGroup(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = 0;
    int     nGroup          = 0;
    int     nOperationIndex = 0;
    
    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

    nGroup = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroup >= 0 && nGroup < MAX_TONG_GROUP);

    nOperationIndex = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nOperationIndex >= 0 && nOperationIndex < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byBaseOperationMask & (1 << nOperationIndex);

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTongClient::LuaCheckAdvanceOperationGroup(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = 0;
    int     nGroup          = 0;
    int     nTargetGroup    = 0;
    int     nOperationIndex = 0;
    
    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 3);

    nGroup = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroup >= 0 && nGroup < MAX_TONG_GROUP);

    nTargetGroup = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    nOperationIndex = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nOperationIndex >= 0 && nOperationIndex < TONG_OPERATION_NUM);

    bResult = m_Schema.Data[nGroup].byAdvanceOperationMask[nTargetGroup] & (1 << nOperationIndex);

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTongClient::LuaApplyTongInfo(Lua_State* L)
{
    KPlayer* pPlayer = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

    g_PlayerClient.DoApplyTongInfoRequest();

Exit0:
    return 0;
}

int KTongClient::LuaApplyTongRoster(Lua_State* L)
{
    KPlayer* pPlayer = NULL;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);    
    
    g_PlayerClient.DoApplyTongRosterRequest();

Exit0:
    return 0;
}

int KTongClient::LuaApplyRepertoryPage(Lua_State* L)
{
    int         nTopIndex           = 0;
    int         nPageIndex          = 0;
    KPlayer*    pPlayer             = NULL;
    int         nLastUpdateCounter  = -1;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nPageIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

    nLastUpdateCounter = m_RepertoryCache.m_Pages[nPageIndex].m_nLastUpdateCounter;
    KGLOG_PROCESS_ERROR(nLastUpdateCounter >= -1);

    g_PlayerClient.DoApplyTongRepertoryPageRequest(nPageIndex, nLastUpdateCounter);
Exit0:
    return 0;
}

int KTongClient::LuaApplyModifyTongName(Lua_State* L)
{
    int         nRetCode    = 0;
    size_t      uStrLen     = 0;
    const char* cpszText    = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    cpszText = (const char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(cpszText);
    uStrLen = strlen(cpszText);
    KGLOG_PROCESS_ERROR(uStrLen >= 4 && uStrLen < sizeof(m_szTongName));

    KGLOG_PROCESS_ERROR(g_pSO3World->m_dwClientPlayerID == m_dwMaster);

    g_PlayerClient.DoModifyTongInfoRequest(tmrTongName, (BYTE*)cpszText, uStrLen + 1);

Exit0:
    return 0;
}

int KTongClient::LuaApplyModifyAnnouncement(Lua_State* L)
{
    int                 nRetCode    = 0;
    BOOL                bRetCode    = false;
    size_t              uStrLen     = 0;
    const char*         cpszText    = NULL;
    KTongMemberInfo*    pMember     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    cpszText = (const char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(cpszText);

    uStrLen = strlen(cpszText);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(m_szAnnouncement));

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totModifyAnnouncement);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoModifyTongInfoRequest(tmrAnnouncement, (BYTE*)cpszText, uStrLen + 1);

Exit0:
    return 0;
}

int KTongClient::LuaApplyModifyOnlineMessage(Lua_State* L)
{
    int                 nRetCode    = 0;
    BOOL                bRetCode    = false;
    size_t              uStrLen     = 0;
    const char*         cpszText    = NULL;
    KTongMemberInfo*    pMember     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    cpszText = (const char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(cpszText);

    uStrLen = strlen(cpszText);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(m_szOnlineMessage));

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totModifyOnlineMessage);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoModifyTongInfoRequest(tmrOnlineMessage, (BYTE*)cpszText, uStrLen + 1);

Exit0:
    return 0;
}

int KTongClient::LuaApplyModifyIntroduction(Lua_State* L)
{
    int                 nRetCode    = 0;
    BOOL                bRetCode    = false;
    size_t              uStrLen     = 0;
    const char*         cpszText    = NULL;
    KTongMemberInfo*    pMember     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    cpszText = (const char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(cpszText);

    uStrLen = strlen(cpszText);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(m_szIntroduction));

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totModifyIntroduction);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoModifyTongInfoRequest(tmrIntroduction, (BYTE*)cpszText, uStrLen + 1);

Exit0:
    return 0;
}

int KTongClient::LuaApplyModifyRules(Lua_State* L)
{
    int                 nRetCode    = 0;
    BOOL                bRetCode    = false;
    size_t              uStrLen     = 0;
    const char*         cpszText    = NULL;
    KTongMemberInfo*    pMember     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    cpszText = (const char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(cpszText);

    uStrLen = strlen(cpszText);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(m_szRules));

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totModifyRules);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoModifyTongInfoRequest(tmrRules, (BYTE*)cpszText, uStrLen + 1);

Exit0:
    return 0;
}

int KTongClient::LuaApplyAddMemorabilia(Lua_State* L)
{
    BOOL                    bRetCode        = false;
    int                     nTopIndex       = 0;
    const char*             pszDescription  = NULL;
    size_t                  uStrLen         = 0;
    KTongMemberInfo*        pMember         = NULL;
    KTONG_ADD_MEMORABILIA   AddInfo;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    AddInfo.nTime  = (time_t)Lua_ValueToNumber(L, 1);

    pszDescription = Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszDescription);

    uStrLen = strlen(pszDescription);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(AddInfo.szDescription));

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totModifyMemorabilia);
    KGLOG_PROCESS_ERROR(bRetCode);

    AddInfo.byOperationType = mmtAdd;

    strncpy(AddInfo.szDescription, pszDescription, sizeof(AddInfo.szDescription));
    AddInfo.szDescription[sizeof(AddInfo.szDescription) - 1] = '\0';

    g_PlayerClient.DoModifyTongInfoRequest(tmrMemorabilia, (BYTE*)&AddInfo, sizeof(AddInfo));
Exit0:
    return 0;
}

int KTongClient::LuaApplyModifyMemorabilia(Lua_State* L)
{
    BOOL                        bRetCode        = false;
    int                         nTopIndex       = 0;
    const char*                 pszDescription  = NULL;
    size_t                      uStrLen         = 0;
    KTongMemberInfo*            pMember         = NULL;
    KTONG_MODIFY_MEMORABILIA    ModifyInfo;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    ModifyInfo.dwID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(ModifyInfo.dwID);

    ModifyInfo.nTime = (time_t)Lua_ValueToNumber(L, 2);

    pszDescription = Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pszDescription);

    uStrLen = strlen(pszDescription);
    KGLOG_PROCESS_ERROR(uStrLen < sizeof(ModifyInfo.szDescription));

    ModifyInfo.byOperationType = mmtModify;

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totModifyMemorabilia);
    KGLOG_PROCESS_ERROR(bRetCode);

    strncpy(ModifyInfo.szDescription, pszDescription, sizeof(ModifyInfo.szDescription));
    ModifyInfo.szDescription[sizeof(ModifyInfo.szDescription) - 1] = '\0';

    g_PlayerClient.DoModifyTongInfoRequest(tmrMemorabilia, (BYTE*)&ModifyInfo, sizeof(ModifyInfo));
Exit0:
    return 0;
}

int KTongClient::LuaApplyDeleteMemorabilia(Lua_State* L)
{
    BOOL                        bRetCode    = false;
    int                         nTopIndex   = 0;
    KTongMemberInfo*            pMember     = NULL;
    KTONG_DELETE_MEMORABILIA    DeleteInfo;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    DeleteInfo.dwID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(DeleteInfo.dwID);

    DeleteInfo.byOperationType = mmtDelete;

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totModifyMemorabilia);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoModifyTongInfoRequest(tmrMemorabilia, (BYTE*)&DeleteInfo, sizeof(DeleteInfo));
Exit0:
    return 0;
}

int KTongClient::LuaInvitePlayerJoinTong(Lua_State* L)
{
	int         nRetCode          = 0;
    const char* pszNewMemberName  = NULL;
    KPlayer*    pPlayer           = NULL;

	nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	pszNewMemberName = (const char*)Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pszNewMemberName);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

	g_PlayerClient.DoInvitePlayerJoinTongRequest(pszNewMemberName);

Exit0:
	return 0;
}

int KTongClient::LuaRespondInviteJoinTong(Lua_State* L)
{
    int         nRetCode    = 0;
    DWORD       dwInviterID = ERROR_ID;
    DWORD       dwTongID    = ERROR_ID;
    BOOL        bAccept     = false;
    KPlayer*    pPlayer     = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    dwInviterID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwInviterID);

    dwTongID    = (DWORD)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(dwTongID);

    bAccept     = (BOOL)Lua_ValueToBoolean(L, 3);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID == ERROR_ID);

    g_PlayerClient.DoInvitePlayerJoinTongRespond(dwInviterID, dwTongID, bAccept);

Exit0:
    return 0;
}

int KTongClient::LuaApplyKickOutMember(Lua_State* L)
{
    int                         nRetCode            = 0;
    BOOL                        bRetCode            = false;
    DWORD                       dwTargetMemberID    = ERROR_ID;
    KTongMemberInfo*            pMember             = NULL;
    KTongMemberInfo*            pTargetMember       = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwTargetMemberID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwTargetMemberID);
    
    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    pTargetMember = GetMember(dwTargetMemberID);
    KGLOG_PROCESS_ERROR(pTargetMember);

    KGLOG_PROCESS_ERROR(pTargetMember->dwID != m_dwMaster); // 不能踢会长

    bRetCode = CanAdvanceOperate(pMember->nGroupID, pTargetMember->nGroupID, totMoveFromGroup);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoApplyKickOutTongMember(dwTargetMemberID);

Exit0:
    return 0;
}


int KTongClient::LuaModifyGroupName(Lua_State* L)
{
    int                     nRetCode        = 0;
    BOOL                    bRetCode        = false;
    int                     nGroupIndex     = 0;
    const char*             cpszGroupName   = NULL;
    KTongMemberInfo*        pMember     = NULL;
    KTONG_MODIFY_GROUP_NAME Data;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nGroupIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < MAX_TONG_GROUP);

    cpszGroupName = (const char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(cpszGroupName);

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanAdvanceOperate(pMember->nGroupID, nGroupIndex, totModifyGroupName);
    KGLOG_PROCESS_ERROR(bRetCode);

    strncpy(Data.szGroupName, cpszGroupName, sizeof(Data.szGroupName));
    Data.szGroupName[sizeof(Data.szGroupName) - 1] = '\0';

    g_PlayerClient.DoModifyTongSchemaRequest(nGroupIndex, tmsGroupName, (BYTE*)&Data, sizeof(Data));

Exit0:
    return 0;
}

int KTongClient::LuaModifyBaseOperationMask(Lua_State* L)
{
    int                                 nRetCode        = 0;
    BOOL                                bRetCode        = false;
    int                                 nTargetGroup    = 0;
    int                                 nOffset         = 0;
    KTongMemberInfo*                    pMember         = NULL;
    KTONG_MODIFY_BASE_OPERATION_MASK    Request;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    nTargetGroup = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    nOffset = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nOffset >= 0 && nOffset < TONG_OPERATION_NUM);

    Request.bValue = (bool)Lua_ValueToBoolean(L, 3);

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);
    KGLOG_PROCESS_ERROR(pMember->nGroupID >= 0 && pMember->nGroupID < MAX_TONG_GROUP);

    bRetCode = m_Schema.Data[pMember->nGroupID].byBaseGrantMask[nTargetGroup] & (1 << nOffset);
    KGLOG_PROCESS_ERROR(bRetCode);

    Request.byOffset = (BYTE)nOffset;

    g_PlayerClient.DoModifyTongSchemaRequest(nTargetGroup, tmsBaseOperation, (BYTE*)&Request, sizeof(Request));

Exit0:
    return 0;
}

int KTongClient::LuaModifyAdvanceOperationMask(Lua_State* L)
{
    int                                 nRetCode        = 0;
    BOOL                                bRetCode        = false;
    int                                 nTargetGroup    = 0;
    int                                 nGroupIndex     = 0;
    int                                 nOffset         = 0;
    KTongMemberInfo*                    pMember         = NULL;
    KTONG_MODIFY_ADVANCE_OPERATION_MASK Request;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 4);

    nTargetGroup = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    nGroupIndex = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < MAX_TONG_GROUP);
    
    nOffset = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nOffset >= 0 && nOffset < TONG_OPERATION_NUM);

    Request.bValue      = (bool)Lua_ValueToBoolean(L, 4);

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);
    KGLOG_PROCESS_ERROR(pMember->nGroupID >= 0 && pMember->nGroupID < MAX_TONG_GROUP);

    bRetCode = m_Schema.Data[pMember->nGroupID].byAdvanceGrantMask[nTargetGroup][nGroupIndex] & (1 << nOffset);
    KGLOG_PROCESS_ERROR(bRetCode);

    Request.byIndex     = (BYTE)nGroupIndex;
    Request.byOffset    = (BYTE)nOffset;

    g_PlayerClient.DoModifyTongSchemaRequest(nTargetGroup, tmsAdvanceOperation, (BYTE*)&Request, sizeof(Request));

Exit0:
    return 0;
}

int KTongClient::LuaModifyGroupWage(Lua_State* L)
{
    int                 nRetCode        = 0;
    BOOL                bRetCode        = false;
    int                 nTargetGroup    = 0;
    int                 nWage           = 0;
    KTongMemberInfo*    pMember         = NULL;

    nRetCode = (int)Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nTargetGroup = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);

    nWage = (int)Lua_ValueToNumber(L, 2);

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);
    KGLOG_PROCESS_ERROR(pMember->nGroupID >= 0 && pMember->nGroupID < MAX_TONG_GROUP);

    bRetCode = CanAdvanceOperate(pMember->nGroupID, nTargetGroup, totModifyWage);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoModifyTongSchemaRequest(nTargetGroup, tmsGroupWage, (BYTE*)&nWage, sizeof(nWage));

Exit0:    
    return 0;
}

int KTongClient::LuaQuit(Lua_State* L)
{
    KPlayer* pPlayer = NULL;
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

    KGLOG_PROCESS_ERROR(g_pSO3World->m_dwClientPlayerID != m_dwMaster);
    
    g_PlayerClient.DoApplyQuitTongRequest();

Exit0:
    return 0;
}

int KTongClient::LuaChangeMaster(Lua_State* L)
{
    int                 nRetCode            = 0;
    DWORD               dwTargetMemberID    = ERROR_ID;
    KTongMemberInfo*    pTargetMember       = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwTargetMemberID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwTargetMemberID != m_dwMaster);

    KGLOG_PROCESS_ERROR(m_dwMaster == g_pSO3World->m_dwClientPlayerID);

    pTargetMember = GetMember(dwTargetMemberID);
    KGLOG_PROCESS_ERROR(pTargetMember);

    g_PlayerClient.DoChangeTongMasterRequest(dwTargetMemberID);

Exit0:
    return 0;
}

int KTongClient::LuaChangeMemberGroup(Lua_State* L)
{
    int                 nRetCode            = 0;
    BOOL                bRetCode            = false;
    DWORD               dwTargetMemberID    = ERROR_ID;
    int                 nTargetGroup        = 0;
    KTongMemberInfo*    pMember             = NULL;
    KTongMemberInfo*    pTargetMember       = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwTargetMemberID = (DWORD)Lua_ValueToNumber(L, 1);

    nTargetGroup = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nTargetGroup >= 0 && nTargetGroup < MAX_TONG_GROUP);
    
    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);
    KGLOG_PROCESS_ERROR(pMember->nGroupID >= 0 && pMember->nGroupID < MAX_TONG_GROUP);

    pTargetMember = GetMember(dwTargetMemberID);
    KGLOG_PROCESS_ERROR(pTargetMember);
    KGLOG_PROCESS_ERROR(pTargetMember->nGroupID >= 0 && pTargetMember->nGroupID < MAX_TONG_GROUP);

    KGLOG_PROCESS_ERROR(pTargetMember->nGroupID != nTargetGroup);
    KGLOG_PROCESS_ERROR(dwTargetMemberID != m_dwMaster);
    KGLOG_PROCESS_ERROR(m_Schema.Data[nTargetGroup].bEnable);

    bRetCode = CanAdvanceOperate(pMember->nGroupID, pTargetMember->nGroupID, totMoveFromGroup);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = CanAdvanceOperate(pMember->nGroupID, nTargetGroup, totAddToGroup);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoChangeTongMemberGroupRequest(dwTargetMemberID, nTargetGroup);

Exit0:
    return 0;
}

int KTongClient::LuaChangeMemberRemark(Lua_State* L)
{
    int                 nRetCode            = 0;
    BOOL                bRetCode            = false;
    DWORD               dwTargetMemberID    = ERROR_ID;
    const char*         cpszRemark          = NULL;
    size_t              uStrLen             = 0;
    KTongMemberInfo*    pMember             = NULL;
    KTongMemberInfo*    pTargetMember       = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwTargetMemberID = (DWORD)Lua_ValueToNumber(L, 1);
        
    cpszRemark = (const char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(cpszRemark);

    uStrLen = strlen(cpszRemark);
    KGLOG_PROCESS_ERROR(uStrLen < _NAME_LEN);

    pMember = GetMember(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pMember);
    KGLOG_PROCESS_ERROR(pMember->nGroupID >= 0 && pMember->nGroupID < MAX_TONG_GROUP);

    pTargetMember = GetMember(dwTargetMemberID);
    KGLOG_PROCESS_ERROR(pTargetMember);
    KGLOG_PROCESS_ERROR(pTargetMember->nGroupID >= 0 && pTargetMember->nGroupID < MAX_TONG_GROUP);

    bRetCode = CanAdvanceOperate(pMember->nGroupID, pTargetMember->nGroupID, totModifyMemberRemark);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerClient.DoChangeMemberRemarkRequest(dwTargetMemberID, cpszRemark);

Exit0:
    return 0;
}

int KTongClient::LuaGetMemberCount(Lua_State* L)
{
	int nOnline = 0;

	for (KCLIENT_ROSTER::iterator it = m_Roster.begin(); it != m_Roster.end(); ++it)
	{
		if (it->second.bIsOnline)
		{
			++nOnline;
		}
	}

	Lua_PushNumber(L, (int)m_Roster.size());
	Lua_PushNumber(L, nOnline);
	return 2;
}

int KTongClient::LuaGetTechNodeLevel(Lua_State* L)
{
    int nResult     = 0;
    int nTopIndex   = 0;
    int nIndex      = 0;
        
    nTopIndex = Lua_GetTopIndex(L);
    KG_PROCESS_ERROR(nTopIndex == 1);

    nIndex = (int)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_TONG_TECHTREE_NODE_NUM);

    Lua_PushNumber(L, m_byTechTree[nIndex]);

    nResult = 1;
Exit0:
    return nResult;
}

int KTongClient::LuaGetGroupWage(Lua_State* L)
{
    int nResult     = 0;
    int nTopIndex   = 0; 
    int nWage       = 0;
    int nGroupID    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nGroupID = (int)Lua_ValueToNumber(L, 1); 
    KGLOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_TONG_GROUP);

    nWage = m_Schema.Data[nGroupID].nWage;
    Lua_PushNumber(L, nWage);

    nResult = 1;
Exit0:
    return nResult;
}

int KTongClient::LuaGetGroupID(Lua_State* L)
{
    int                         nResult     = 0;
    int                         nTopIndex   = 0;
    int                         nGroupID    = 0;
    DWORD                       dwPlayerID  = ERROR_ID;
    KCLIENT_ROSTER::iterator    it;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwPlayerID);

    it = m_Roster.find(dwPlayerID);
    KGLOG_PROCESS_ERROR(it != m_Roster.end());

    nGroupID = it->second.nGroupID;
    KGLOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_TONG_GROUP);

    Lua_PushNumber(L, nGroupID);
    nResult = 1;

Exit0:
    return nResult;
}

int KTongClient::LuaApplyOpenRepertory(Lua_State* L)
{
    int         nTopIndex   = 0;;
    DWORD       dwNpcID     = ERROR_ID;
    KPlayer*    pPlayer     = NULL;
    KNpc*       pNpc        = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

    dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);
    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    g_PlayerClient.DoApplyOpenTongRepertoryRequest(dwNpcID);
Exit0:
    return 0;
}

int KTongClient::LuaTakeRepertoryItem(Lua_State* L)
{
    BOOL                    bRetCode        = false;
    ADD_ITEM_RESULT_CODE    eRetCode        = aircFailed;
    int                     nTopIndex       = 0;
    int                     nPageIndex      = 0;
    int                     nPagePos        = 0;
    int                     nBoxIndex       = 0;
    int                     nBoxPos         = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 4);

    nPageIndex  = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < m_RepertoryCache.m_nMaxPageNum);
    nPagePos    = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nPagePos >= 0 && nPagePos < TONG_REPERTORY_PAGE_CAPACITY);
    
    if (nTopIndex == 2)
    {
        bRetCode = m_RepertoryCache.TakeItem(nPageIndex, nPagePos);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        nBoxIndex   = (int)Lua_ValueToNumber(L, 3);
        KGLOG_PROCESS_ERROR(nBoxIndex >= ibPackage && nBoxIndex <= ibPackage4);
        
        nBoxPos     = (int)Lua_ValueToNumber(L, 4);
        KGLOG_PROCESS_ERROR(nBoxPos >= 0 && nBoxPos < MAX_ITEM_PACKAGE_SIZE);

        bRetCode = m_RepertoryCache.TakeItemToPos(nPageIndex, nPagePos, nBoxIndex, nBoxPos);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

Exit0:
    return 0;
}

int KTongClient::LuaPutItemToRepertory(Lua_State* L)
{
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    int         nBoxIndex   = 0;
    int         nBoxPos     = 0;
    int         nPageIndex  = 0;
    int         nPagePos    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3 || nTopIndex == 4);

    nBoxIndex   = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nBoxIndex >= ibPackage && nBoxIndex <= ibPackage4);
    
    nBoxPos = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nBoxPos >= 0 && nBoxPos < MAX_ITEM_PACKAGE_SIZE);
    
    nPageIndex  = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);

    if (nTopIndex == 3)
    {
        bRetCode = m_RepertoryCache.PutItem(nBoxIndex, nBoxPos, nPageIndex);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        nPagePos    = (int)Lua_ValueToNumber(L, 4);
        KGLOG_PROCESS_ERROR(nPagePos >= 0 && nPagePos < TONG_REPERTORY_PAGE_CAPACITY);

        bRetCode = m_RepertoryCache.PutItemToPos(nBoxIndex, nBoxPos, nPageIndex, nPagePos);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
Exit0:
    return 0;
}

int KTongClient::LuaExchangeRepertoryItemPos(Lua_State* L)
{
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    int     nSrcPageIndex   = 0;
    int     nSrcPagePos     = 0;
    int     nDstPageIndex   = 0;
    int     nDstPagePos     = 0;
    
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 4);

    nSrcPageIndex = (int)Lua_ValueToNumber(L ,1);
    KGLOG_PROCESS_ERROR(nSrcPageIndex >= 0 && nSrcPageIndex < TONG_REPERTORY_PAGE_NUM);

    nSrcPagePos = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nSrcPagePos >= 0 && nSrcPagePos < TONG_REPERTORY_PAGE_CAPACITY);

    nDstPageIndex = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nDstPageIndex >= 0 && nDstPageIndex < TONG_REPERTORY_PAGE_NUM);

    nDstPagePos = (int)Lua_ValueToNumber(L ,4);
    KGLOG_PROCESS_ERROR(nDstPagePos >= 0 && nDstPagePos < TONG_REPERTORY_PAGE_CAPACITY);

    bRetCode = m_RepertoryCache.ExchangeItemPos(nSrcPageIndex, nSrcPagePos, nDstPageIndex, nDstPagePos);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KTongClient::LuaStackRepertoryItem(Lua_State* L)
{
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    int         nBoxIndex   = 0;
    int         nBoxPos     = 0;
    int         nPageIndex  = 0;
    int         nPagePos    = 0;
    int         nStackNum   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 5);

    nBoxIndex   = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nBoxIndex >= ibPackage && nBoxIndex <= ibPackage4);
    
    nBoxPos = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nBoxPos >= 0 && nBoxPos < MAX_ITEM_PACKAGE_SIZE);
    
    nPageIndex  = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);

    nPagePos = (int)Lua_ValueToNumber(L, 4);
    KGLOG_PROCESS_ERROR(nPagePos >= 0 && nPagePos < TONG_REPERTORY_PAGE_CAPACITY);

    nStackNum = (int)Lua_ValueToNumber(L, 5);
    KGLOG_PROCESS_ERROR(nStackNum > 0);

    bRetCode = m_RepertoryCache.StackItem(nPageIndex, nPagePos, nBoxIndex, nBoxPos, nStackNum);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KTongClient::LuaSaveMoney(Lua_State* L)
{
    int         nTopIndex   = 0;
    KPlayer*    pPlayer     = NULL;
    int         nOwnMoney   = 0;
    int         nSaveMoney  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nSaveMoney = (int)Lua_ValueToNumber(L, 1);
    // 存入帮会的资金以银为最小单位
    KGLOG_PROCESS_ERROR(nSaveMoney >= MONEY_CONVERSION_SILVER_RATE);
    KGLOG_PROCESS_ERROR(nSaveMoney % MONEY_CONVERSION_SILVER_RATE == 0);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);
    KGLOG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

    nOwnMoney = pPlayer->m_ItemList.GetMoney();
    KGLOG_PROCESS_ERROR(nOwnMoney >= nSaveMoney);

    KGLOG_PROCESS_ERROR(MAX_TONG_FUND_BY_SILVER - m_nFund >= nSaveMoney / MONEY_CONVERSION_SILVER_RATE);

    g_PlayerClient.DoSaveMoneyInTongRequest(nSaveMoney);
Exit0:
    return 0;
}

int KTongClient::LuaPaySalary(Lua_State* L)
{
    BOOL                bRetCode    = false;
    KPlayer*            pPlayer     = NULL;
    KTongMemberInfo*    pMember     = NULL;
    int                 nLeftSilver = 0;
    int                 nPaySilver  = 0;

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(pPlayer);
    KG_PROCESS_ERROR(pPlayer->m_dwTongID != ERROR_ID);

    pMember = GetMember(pPlayer->m_dwID);
    KGLOG_PROCESS_ERROR(pMember);

    bRetCode = CanBaseOperate(pMember->nGroupID, totPaySalary);
    KGLOG_PROCESS_ERROR(bRetCode);

    KGLOG_PROCESS_ERROR(m_nLastInfoUpdateFrame != -1);

    nLeftSilver = m_nFund;
    for (KCLIENT_ROSTER::iterator it = m_Roster.begin(); it != m_Roster.end(); ++it)
    {
        pMember = &it->second;
        nLeftSilver -= m_Schema.Data[pMember->nGroupID].nWage;
        KGLOG_PROCESS_ERROR(nLeftSilver >= 0);
    }

    nPaySilver = m_nFund - nLeftSilver;
    KG_PROCESS_ERROR(nPaySilver > 0);
    KGLOG_PROCESS_ERROR(nPaySilver <= m_nFund * (m_nMaxWageRate / 100.0));
    g_PlayerClient.DoPayTongSalaryRequest(nPaySilver);
Exit0:
    return 0;
}

int KTongClient::LuaGetSalary(Lua_State* L)
{
    g_PlayerClient.DoGetTongSalaryRequest();
    return 0;
}

int KTongClient::LuaApplyGetTongName(Lua_State* L)
{
    int     nResult     = 0;
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    DWORD   dwTongID    = ERROR_ID;
    char    szTongName[_NAME_LEN];

    nTopIndex = Lua_GetTopIndex(L);
    KG_PROCESS_ERROR(nTopIndex == 1);

    dwTongID = (DWORD)Lua_ValueToNumber(L ,1);
    KGLOG_PROCESS_ERROR(dwTongID != ERROR_ID);

    bRetCode = TryGetTongName(dwTongID, szTongName, sizeof(szTongName));
    KG_PROCESS_ERROR(bRetCode);
    
    Lua_PushString(L, szTongName);
    nResult = 1;
Exit0:
    return nResult;
}

int KTongClient::LuaGetHistoryRecord(Lua_State* L)
{
    int                         nResult     = false;
    int                         nTopIndex   = 0;
    int                         nType       = 0;
    DWORD                       dwIndex     = 0;
    KTONG_SYNC_HISTORY*         pRecord     = NULL;
    KTONG_HISTORY_CLIENT_TABLE* pHistory    = NULL;
    KTONG_HISTORY_CLIENT_TABLE::iterator it;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nType = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nType >= 0 && nType < ohtTotal);

    dwIndex = (DWORD)Lua_ValueToNumber(L, 2);
    KG_PROCESS_ERROR(dwIndex);

    pHistory = &m_History[nType];
    it = pHistory->find(dwIndex);
    KGLOG_PROCESS_ERROR(it != pHistory->end());

    pRecord = it->second;
    Lua_NewTable(L);
                
    Lua_PushString(L, "eType");
    Lua_PushNumber(L, pRecord->byRecordType);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "nTime");
    Lua_PushNumber(L, pRecord->nTime);
    Lua_SetTable(L, -3);
    
    switch (pRecord->byRecordType)
    {
    case hrtChangeGroupBaseRight:
        {
            KTONG_SYNC_HISTORY_CHANGE_GROUP_BASE_RIGHT* pInfo = (KTONG_SYNC_HISTORY_CHANGE_GROUP_BASE_RIGHT*)pRecord;
            KGLOG_PROCESS_ERROR(pInfo->byTargetGroup < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pInfo->byRightOffset < TONG_OPERATION_NUM);

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "szTargetGroup");
            Lua_PushString(L, m_Schema.Data[pInfo->byTargetGroup].szName);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nRightOffset");
            Lua_PushNumber(L, pInfo->byRightOffset);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "bIsOpen");
            Lua_PushBoolean(L, pInfo->byIsOpen);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtChangeGroupAdvanceRight:
        {
            KTONG_SYNC_HISTORY_CHANGE_GROUP_ADVANCE_RIGHT* pInfo = (KTONG_SYNC_HISTORY_CHANGE_GROUP_ADVANCE_RIGHT*)pRecord;
            KGLOG_PROCESS_ERROR(pInfo->bySrcGroup < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pInfo->byDstGroup < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pInfo->byRightOffset < TONG_OPERATION_NUM);

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "szSrcGroup");
            Lua_PushString(L, m_Schema.Data[pInfo->bySrcGroup].szName);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "szDstGroup");
            Lua_PushString(L, m_Schema.Data[pInfo->byDstGroup].szName);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nRightOffset");
            Lua_PushNumber(L, pInfo->byRightOffset);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "bIsOpen");
            Lua_PushBoolean(L, pInfo->bIsOpen);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtChangeMemberGroup:
        {
            KTONG_SYNC_HISTORY_CHANGE_MEMBER_GROUP* pInfo = (KTONG_SYNC_HISTORY_CHANGE_MEMBER_GROUP*)pRecord;
            KGLOG_PROCESS_ERROR(pInfo->bySrcGroup < MAX_TONG_GROUP);
            KGLOG_PROCESS_ERROR(pInfo->byDstGroup < MAX_TONG_GROUP);

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "szTargetMember");
            Lua_PushString(L, pInfo->szTargetMember);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nSrcGroup");
            Lua_PushString(L, m_Schema.Data[pInfo->bySrcGroup].szName);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nDstGroup");
            Lua_PushString(L, m_Schema.Data[pInfo->byDstGroup].szName);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtJoin:
    case hrtQuit:
        {
            KTONG_SYNC_HISTORY_JOIN_OR_QUIT* pInfo = (KTONG_SYNC_HISTORY_JOIN_OR_QUIT*)pRecord;

            Lua_PushString(L, "szMember");
            Lua_PushString(L, pInfo->szMember);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtKickOut:
        {
            KTONG_SYNC_HISTORY_KICK_OUT_MEMBER* pInfo = (KTONG_SYNC_HISTORY_KICK_OUT_MEMBER*)pRecord;

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "szTargetMember");
            Lua_PushString(L, pInfo->szTargetMember);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtChangeGroupWage:
        {
            KTONG_SYNC_HISTORY_CHANGE_GROUP_WAGE* pInfo = (KTONG_SYNC_HISTORY_CHANGE_GROUP_WAGE*)pRecord;
            KGLOG_PROCESS_ERROR(pInfo->byTargetGroup < MAX_TONG_GROUP);

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "szTargetGroup");
            Lua_PushString(L, m_Schema.Data[pInfo->byTargetGroup].szName);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nOldWage");
            Lua_PushNumber(L, pInfo->dwOldWage);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nNewWage");
            Lua_PushNumber(L, pInfo->dwNewWage);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtPaySalary:
        {
            KTONG_SYNC_HISTORY_PAY_SALARY* pInfo = (KTONG_SYNC_HISTORY_PAY_SALARY*)pRecord;

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nTotalSalary");
            Lua_PushNumber(L, pInfo->dwTotalSalary);
            Lua_PushNumber(L, -3);
        }
        break;
    case hrtSaveFund:
        {
            KTONG_SYNC_HISTORY_SAVE_FUND* pInfo = (KTONG_SYNC_HISTORY_SAVE_FUND*)pRecord;

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nSaveSilver");
            Lua_PushNumber(L, pInfo->dwSaveSilver);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtActivateTechtree:
        {
            KTONG_SYNC_HISTORY_ACTIVATE_TECHTREE* pInfo = (KTONG_SYNC_HISTORY_ACTIVATE_TECHTREE*)pRecord;

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nNode");
            Lua_PushNumber(L, pInfo->byNode);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nLevel");
            Lua_PushNumber(L, pInfo->byLevel);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nConsumeDevelopmentPoint");
            Lua_PushNumber(L, pInfo->dwConsumeDevelopmentPoint);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nConsumeFund");
            Lua_PushNumber(L, pInfo->dwConsumeFund);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtProductItem:
        {
            KTONG_SYNC_HISTORY_PRODUCT_ITEM* pInfo = (KTONG_SYNC_HISTORY_PRODUCT_ITEM*)pRecord;

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "dwRecipeID");
            Lua_PushNumber(L, pInfo->dwRecipeID);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nCount");
            Lua_PushNumber(L, pInfo->dwCount);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nConsumeDevelopmentPoint");
            Lua_PushNumber(L, pInfo->dwConsumeDevelopmentPoint);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nConsumeFund");
            Lua_PushNumber(L, pInfo->dwConsumeFund);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtMissionStart:
        {
            KTONG_SYNC_HISTORY_MISSION_START* pInfo = (KTONG_SYNC_HISTORY_MISSION_START*)pRecord;

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "dwMission");
            Lua_PushNumber(L, pInfo->dwMission);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nMissionStartTime");
            Lua_PushNumber(L, pInfo->nMissionStartTime);
            Lua_SetTable(L ,-3);
        }
        break;
    case hrtMissionComplete:
    case hrtMissionFailed:
        {
            KTONG_SYNC_HISTORY_MISSION_END* pInfo = (KTONG_SYNC_HISTORY_MISSION_END*)pRecord;

            Lua_PushString(L, "dwMission");
            Lua_PushNumber(L, pInfo->dwMission);
            Lua_SetTable(L, -3);
        }    
        break;
    case hrtWarStart:
        {
            KTONG_SYNC_HISTORY_WAR_START* pInfo = (KTONG_SYNC_HISTORY_WAR_START*)pRecord;

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "szEnemyTong");
            Lua_PushString(L, pInfo->szEnemyTong);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtWarWin:
    case hrtWarLose:
        {
            KTONG_SYNC_HISTORY_WAR_END* pInfo = (KTONG_SYNC_HISTORY_WAR_END*)pRecord;

            Lua_PushString(L, "szEnemyTong");
            Lua_PushString(L, pInfo->szEnemyTong);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtAchievement:
        {
            KTONG_SYNC_HISTORY_ACHIEVEMENT* pInfo = (KTONG_SYNC_HISTORY_ACHIEVEMENT*)pRecord;

            Lua_PushString(L, "dwAchievement");
            Lua_PushNumber(L, pInfo->dwAchievement);
            Lua_SetTable(L, -3);
        }
        break;
    case hrtRepertoryTakeItem:
    case hrtRepertoryPutItem:
        {
            KTONG_SYNC_HISTORY_REPERTORY_TAKE_OR_PUT_ITEM* pInfo = (KTONG_SYNC_HISTORY_REPERTORY_TAKE_OR_PUT_ITEM*)pRecord;

            Lua_PushString(L, "szOperator");
            Lua_PushString(L, pInfo->szOperator);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "dwTabType");
            Lua_PushNumber(L, pInfo->byTabType);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "dwTabIndex");
            Lua_PushNumber(L, pInfo->wTabIndex);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "nStackNum");
            Lua_PushNumber(L, pInfo->wStackNum);
            Lua_SetTable(L, -3);
        }
        break;

    default:
        KGLOG_PROCESS_ERROR(0);
    }

    nResult = 1;
Exit0:
    return nResult;
}

// ------------------------------------------------------------------------
DEFINE_LUA_CLASS_BEGIN(KTongClient)
    REGISTER_LUA_DWORD_READONLY(KTongClient, Master)
    //REGISTER_LUA_INTEGER_READONLY(KTongClient, Fund)
    //REGISTER_LUA_INTEGER_READONLY(KTongClient, MaxWageRate)
    REGISTER_LUA_INTEGER_READONLY(KTongClient, Level)
    //REGISTER_LUA_INTEGER_READONLY(KTongClient, DevelopmentPoint)
    //REGISTER_LUA_INTEGER_READONLY(KTongClient, UsedDevelopmentPoint)
    REGISTER_LUA_INTEGER_READONLY(KTongClient, MaxMemberCount)
	REGISTER_LUA_ENUM_READONLY(KTongClient, Camp)

    REGISTER_LUA_STRING_READONLY(KTongClient, TongName)
    REGISTER_LUA_STRING_READONLY(KTongClient, Announcement)
    REGISTER_LUA_STRING_READONLY(KTongClient, OnlineMessage)
    REGISTER_LUA_STRING_READONLY(KTongClient, Introduction)
    REGISTER_LUA_STRING_READONLY(KTongClient, Rules)

    REGISTER_LUA_ENUM_READONLY(KTongClient, State)

    REGISTER_LUA_FUNC(KTongClient, GetGroupInfo)
    REGISTER_LUA_FUNC(KTongClient, GetMemberList)
    REGISTER_LUA_FUNC(KTongClient, GetMemberInfo)
    REGISTER_LUA_FUNC(KTongClient, GetDefaultGroupID)
	REGISTER_LUA_FUNC(KTongClient, GetMasterGroupID)
    REGISTER_LUA_FUNC(KTongClient, GetStateTimer)
    REGISTER_LUA_FUNC(KTongClient, GetMemorabilia)

    REGISTER_LUA_FUNC(KTongClient, CanBaseOperate)
    REGISTER_LUA_FUNC(KTongClient, CanAdvanceOperate)
    REGISTER_LUA_FUNC(KTongClient, CanBaseGrant)
    REGISTER_LUA_FUNC(KTongClient, CanAdvanceGrant)
    REGISTER_LUA_FUNC(KTongClient, CheckBaseOperationGroup)
    REGISTER_LUA_FUNC(KTongClient, CheckAdvanceOperationGroup)

    REGISTER_LUA_FUNC(KTongClient, ApplyTongInfo)
    REGISTER_LUA_FUNC(KTongClient, ApplyTongRoster)
    //REGISTER_LUA_FUNC(KTongClient, ApplyRepertoryPage)

    REGISTER_LUA_FUNC(KTongClient, ApplyModifyTongName)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyAnnouncement)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyOnlineMessage)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyIntroduction)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyRules)

    REGISTER_LUA_FUNC(KTongClient, ApplyAddMemorabilia)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyMemorabilia)
    REGISTER_LUA_FUNC(KTongClient, ApplyDeleteMemorabilia)

    REGISTER_LUA_FUNC(KTongClient, InvitePlayerJoinTong)
    REGISTER_LUA_FUNC(KTongClient, RespondInviteJoinTong)
    REGISTER_LUA_FUNC(KTongClient, ApplyKickOutMember)
    REGISTER_LUA_FUNC(KTongClient, ModifyGroupName)
    REGISTER_LUA_FUNC(KTongClient, ModifyBaseOperationMask)
    REGISTER_LUA_FUNC(KTongClient, ModifyAdvanceOperationMask)
    //REGISTER_LUA_FUNC(KTongClient, ModifyGroupWage)
    REGISTER_LUA_FUNC(KTongClient, Quit)
    REGISTER_LUA_FUNC(KTongClient, ChangeMaster)
    REGISTER_LUA_FUNC(KTongClient, ChangeMemberGroup)
    REGISTER_LUA_FUNC(KTongClient, ChangeMemberRemark)
	REGISTER_LUA_FUNC(KTongClient, GetMemberCount)
    //REGISTER_LUA_FUNC(KTongClient, GetTechNodeLevel)
    //REGISTER_LUA_FUNC(KTongClient, GetGroupWage)
    REGISTER_LUA_FUNC(KTongClient, GetGroupID)

    //REGISTER_LUA_FUNC(KTongClient, ApplyOpenRepertory)
    //REGISTER_LUA_FUNC(KTongClient, TakeRepertoryItem)
    //REGISTER_LUA_FUNC(KTongClient, PutItemToRepertory)
    //REGISTER_LUA_FUNC(KTongClient, GetRepertoryItem)
    //REGISTER_LUA_FUNC(KTongClient, ExchangeRepertoryItemPos)
    //REGISTER_LUA_FUNC(KTongClient, StackRepertoryItem)

    //REGISTER_LUA_FUNC(KTongClient, SaveMoney)
    //REGISTER_LUA_FUNC(KTongClient, PaySalary)
    //REGISTER_LUA_FUNC(KTongClient, GetSalary)

    REGISTER_LUA_FUNC(KTongClient, ApplyGetTongName)
    //REGISTER_LUA_FUNC(KTongClient, GetHistoryRecord)

DEFINE_LUA_CLASS_END(KTongClient)

#endif // _CLIENT

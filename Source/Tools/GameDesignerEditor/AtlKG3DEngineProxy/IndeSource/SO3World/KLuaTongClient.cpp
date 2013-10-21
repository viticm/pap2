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
    KCLIENT_ROSTER::iterator        it;

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
			for (it = m_Roster.begin(); it != m_Roster.end(); ++it)
			{
				if (it->second.nGroupID == nGroupFilter)
					MemberTable.push_back(&(it->second));
			}
		}
		else
		{
			for (it = m_Roster.begin(); it != m_Roster.end(); ++it)
			{
				if (it->second.bIsOnline && it->second.nGroupID == nGroupFilter)
					MemberTable.push_back(&(it->second));
			}
		}
	}
	else if (nSchoolFilter >= 0)
	{
		if (bOffLine)
		{
			for (it = m_Roster.begin(); it != m_Roster.end(); ++it)
			{
				if (it->second.byForceID == nSchoolFilter)
					MemberTable.push_back(&(it->second));
			}
		}
		else
		{
			for (it = m_Roster.begin(); it != m_Roster.end(); ++it)
			{
				if (it->second.bIsOnline && it->second.byForceID == nSchoolFilter)
				{
					MemberTable.push_back(&(it->second));
				}
			}
		}
	}
	else
	{
		if (bOffLine)
		{
			for (it = m_Roster.begin(); it != m_Roster.end(); ++it)
				MemberTable.push_back(&(it->second));
		}
		else
		{
			for (it = m_Roster.begin(); it != m_Roster.end(); ++it)
			{
				if (it->second.bIsOnline)
					MemberTable.push_back(&(it->second));
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
	int                         nCount  = 0;
	int                         nOnline = 0;
	KCLIENT_ROSTER::iterator    it;

	for (KCLIENT_ROSTER::iterator it = m_Roster.begin(); it != m_Roster.end(); ++it)
	{
		++nCount;
		if (it->second.bIsOnline)
		{
			++nOnline;
		}
	}
	Lua_PushNumber(L, nCount);
	Lua_PushNumber(L, nOnline);
	return 2;
}

// ------------------------------------------------------------------------
DEFINE_LUA_CLASS_BEGIN(KTongClient)
    REGISTER_LUA_DWORD_READONLY(KTongClient, Master)
    REGISTER_LUA_INTEGER_READONLY(KTongClient, Fund)
    REGISTER_LUA_INTEGER_READONLY(KTongClient, Level)
    REGISTER_LUA_INTEGER_READONLY(KTongClient, DevelopmentPoint)
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

    REGISTER_LUA_FUNC(KTongClient, CanBaseOperate)
    REGISTER_LUA_FUNC(KTongClient, CanAdvanceOperate)
    REGISTER_LUA_FUNC(KTongClient, CanBaseGrant)
    REGISTER_LUA_FUNC(KTongClient, CanAdvanceGrant)
    REGISTER_LUA_FUNC(KTongClient, CheckBaseOperationGroup)
    REGISTER_LUA_FUNC(KTongClient, CheckAdvanceOperationGroup)

    REGISTER_LUA_FUNC(KTongClient, ApplyTongInfo)
    REGISTER_LUA_FUNC(KTongClient, ApplyTongRoster)

    REGISTER_LUA_FUNC(KTongClient, ApplyModifyTongName)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyAnnouncement)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyOnlineMessage)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyIntroduction)
    REGISTER_LUA_FUNC(KTongClient, ApplyModifyRules)

    REGISTER_LUA_FUNC(KTongClient, InvitePlayerJoinTong)
    REGISTER_LUA_FUNC(KTongClient, RespondInviteJoinTong)
    REGISTER_LUA_FUNC(KTongClient, ApplyKickOutMember)
    REGISTER_LUA_FUNC(KTongClient, ModifyGroupName)
    REGISTER_LUA_FUNC(KTongClient, ModifyBaseOperationMask)
    REGISTER_LUA_FUNC(KTongClient, ModifyAdvanceOperationMask)
    REGISTER_LUA_FUNC(KTongClient, ModifyGroupWage)
    REGISTER_LUA_FUNC(KTongClient, Quit)
    REGISTER_LUA_FUNC(KTongClient, ChangeMaster)
    REGISTER_LUA_FUNC(KTongClient, ChangeMemberGroup)
    REGISTER_LUA_FUNC(KTongClient, ChangeMemberRemark)
	REGISTER_LUA_FUNC(KTongClient, GetMemberCount)

DEFINE_LUA_CLASS_END(KTongClient)

#endif // _CLIENT

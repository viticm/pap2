#include "stdafx.h"
#include "StrMap.h"
#include "KTong.h"
#include "KRole.h"
#include "KSO3GameCenter.h"

int KTong::LuaSetLevel(Lua_State* L)
{
    int     nTopIndex   = 0;
    int     nNewLevel   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nNewLevel = (int)Lua_ValueToNumber(L ,1);
    KGLOG_PROCESS_ERROR(nNewLevel > 0);

    KGLOG_PROCESS_ERROR(nNewLevel != m_nLevel);

    m_nLevel = nNewLevel;
    m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;

    m_bUpdateFlag = true;

Exit0:
    return 0;
}

int KTong::LuaAddFund(Lua_State* L)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    int     nAddFund    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAddFund = (int)Lua_ValueToNumber(L ,1);

    bRetCode = AddFund(nAddFund);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KTong::LuaAddDevelopmentPoint(Lua_State* L)
{
    int  nTopIndex   = 0;
    int  nAddPoint   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    
    nAddPoint = (int)Lua_ValueToNumber(L ,1);
    KGLOG_PROCESS_ERROR(nAddPoint >= 0);
    
    if (nAddPoint > 0)
    {
        if (m_nDevelopmentPoint > MAX_TONG_DEVELOPMENT_POINT -  nAddPoint)
        {
            m_nDevelopmentPoint = MAX_TONG_DEVELOPMENT_POINT;
        }
        else
        {
            m_nDevelopmentPoint += nAddPoint;
        }

        m_nLastUpdateFrameTable[tdtDevelopmentInfo] = g_pSO3GameCenter->m_nWorkLoop;
        m_bUpdateFlag = true;
    }

Exit0:
    return 0;
}

int KTong::LuaConsumeDevelopmentPoint(Lua_State* L)
{
    int     nTopIndex       = 0;
    int     nConsumePoint   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nConsumePoint = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nConsumePoint >= 0);
    KGLOG_PROCESS_ERROR(nConsumePoint <= m_nDevelopmentPoint);
    
    if (nConsumePoint > 0)
    {
        KGLOG_PROCESS_ERROR(INT_MAX - m_nUsedDevelopmentPoint >= nConsumePoint);
        m_nDevelopmentPoint     -= nConsumePoint;
        m_nUsedDevelopmentPoint += nConsumePoint;

        m_nLastUpdateFrameTable[tdtDevelopmentInfo] = g_pSO3GameCenter->m_nWorkLoop;
        m_bUpdateFlag = true;
    }
Exit0:
    return 0;
}

int KTong::LuaGetTechNodeLevel(Lua_State* L)
{
    int nResult     = 0;
    int nTopIndex   = 0;
    int nIndex      = 0;
        
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nIndex = (int)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_TONG_TECHTREE_NODE_NUM);

    Lua_PushNumber(L, m_byTechTree[nIndex]);

    nResult = 1;
Exit0:
    return nResult;
}

int KTong::LuaSetTechNodeLevel(Lua_State* L)
{
    BOOL            bRetCode                = false;
    int             nTopIndex               = 0;
    int             nIndex                  = 0;
    int             nValue                  = 0;
    DWORD           dwPlayerID              = ERROR_ID;
    KTongMember*    pMember                 = NULL;
    int             nGroup                  = 0;
    int             nCostDevelopmentPoint   = 0;
    int             nCostFund               = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 5);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwPlayerID);

    nIndex = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_TONG_TECHTREE_NODE_NUM);

    nValue = (int)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(nValue > m_byTechTree[nIndex] && nValue <= UCHAR_MAX);

    nCostDevelopmentPoint = (int)Lua_ValueToNumber(L, 4);
    KGLOG_PROCESS_ERROR(nCostDevelopmentPoint >= m_nDevelopmentPoint);

    nCostFund = (int)Lua_ValueToNumber(L, 5);
    KGLOG_PROCESS_ERROR(nCostFund >= m_nFund);

    pMember = GetMember(dwPlayerID);
    KGLOG_PROCESS_ERROR(pMember);

    nGroup = pMember->nGroupID;
    KGLOG_PROCESS_ERROR(nGroup >= 0 && nGroup <= MAX_TONG_GROUP);
    bRetCode = CanBaseOperate(nGroup, totDevelopTechnology);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_byTechTree[nIndex] = (BYTE)nValue;

    bRetCode = m_History.RecordActivateTechTree(dwPlayerID, nIndex, nValue, nCostDevelopmentPoint, nCostFund);
	KGLOG_CHECK_ERROR(bRetCode);

    m_nLastUpdateFrameTable[tdtDevelopmentInfo] = g_pSO3GameCenter->m_nWorkLoop;

    m_bUpdateFlag = true;

Exit0:
    return 0;
}

int KTong::LuaIsGroupEnabled(Lua_State* L)
{
    int     nResult     = 0;
    BOOL    bRetCode    = 0;
    int     nTopIndex   = 0;
    int     nGroupID    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nGroupID = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_TONG_GROUP);

    bRetCode = m_Schema.Data[nGroupID].bEnable;
    Lua_PushBoolean(L, bRetCode);
    nResult = 1;
Exit0:
    return nResult;
}

int KTong::LuaEnableGroup(Lua_State* L)
{
    int                             nTopIndex   = 0;
    int                             nGroupID    = 0;
    KTONG_GROUP_ENABLED_BROADCAST   Pak;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nGroupID = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroupID >= 0 && nGroupID < MAX_TONG_GROUP);

    KGLOG_PROCESS_ERROR(!m_Schema.Data[nGroupID].bEnable);

    m_Schema.Data[nGroupID].bEnable = true;
    m_nLastUpdateFrameTable[tdtSchemaInfo] = g_pSO3GameCenter->m_nWorkLoop;

    m_bUpdateFlag = true;

    strncpy(Pak.szGroupName, m_Schema.Data[nGroupID].szName, sizeof(Pak.szGroupName));
    Pak.szGroupName[sizeof(Pak.szGroupName) - 1] = '\0';

    g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(this, tbmGroupEnabled, ERROR_ID, (BYTE*)&Pak, sizeof(Pak));
Exit0:
    return 0;
}

int KTong::LuaSetMaxMemberCount(Lua_State* L)
{
    int nTopIndex   = 0;
    int nValue      = 0;
    
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nValue = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nValue > m_nMaxMemberCount);

    m_nMaxMemberCount = nValue;
    m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;

    m_bUpdateFlag = true;

    g_pSO3GameCenter->m_GameServer.DoTongBroadcastMessage(
        this, tbmMaxMemberCountChange, ERROR_ID, (BYTE*)&m_nMaxMemberCount, sizeof(m_nMaxMemberCount)
    );
Exit0:
    return 0;
}

// GM调试专用 无条件转换阵营
int KTong::LuaSetCamp(Lua_State* L)
{
    int     nTopIndex   = 0;
    int     nValue      = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nValue = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nValue >= cNeutral && nValue <= cEvil);

    m_eCamp = (KCAMP)nValue;
    m_nLastUpdateFrameTable[tdtBaseInfo] = g_pSO3GameCenter->m_nWorkLoop;

    m_bUpdateFlag = true;
Exit0:
    return 0;
}

int KTong::LuaSetMaxWageRate(Lua_State* L)
{
    int nTopIndex   = 0;
    int nRate       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nRate = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nRate >= 0 && nRate <= TONG_MAX_WAGE_RATE_LIMIT);

    m_nMaxWageRate = nRate;
    m_nLastUpdateFrameTable[tdtDevelopmentInfo] = g_pSO3GameCenter->m_nWorkLoop;

    m_bUpdateFlag = true;
Exit0:
    return 0;
}

int KTong::LuaGetGroupID(Lua_State* L)
{
    int                 nResult     = 0;
    int                 nTopIndex   = 0;
    int                 nGroupID    = 0;
    DWORD               dwPlayerID  = ERROR_ID;
    KROSTER::iterator   it;

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

int KTong::LuaGetMemberCount(Lua_State* L)
{
	Lua_PushNumber(L, (int)m_Roster.size());
	return 1;
}

int KTong::LuaCanBaseOperate(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nTopIndex       = 0;
    int     nGroupIndex     = 0;
    int     nOperationType  = 0;

    nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nGroupIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nGroupIndex >= 0 && nGroupIndex < MAX_TONG_GROUP);

    nOperationType = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nOperationType > totInvalid && nOperationType < totTotal);

    bResult = CanBaseOperate(nGroupIndex, (KTONG_OPERATION_TYPE)nOperationType);

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KTong::LuaCanAdvanceOperate(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nTopIndex       = 0;
    int     nGroup          = 0;
    int     nTargetGroup    = 0;
    int     nOperationType  = 0;

    nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 3);

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

int KTong::LuaGetGroupWage(Lua_State* L)
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

int KTong::LuaActiveRepertoryPage(Lua_State* L)
{
    int nTopIndex   = 0;
    int nPageIndex  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nPageIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);

    m_Repertory.m_Page[nPageIndex].bEnable = true;
    m_bUpdateFlag = true;
Exit0:
    return 0;
}

int KTong::LuaGetMemberSalaryAmount(Lua_State* L)
{
    int             nResult     = 0;
    int             nTopIndex   = 0;
    int             nSilverWage = 0;
    DWORD           dwMemberID  = 0;
    KTongMember*    pMember     = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMemberID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwMemberID);

    pMember = GetMember(dwMemberID);
    KGLOG_PROCESS_ERROR(pMember);

    nSilverWage = pMember->nSalary;
    KGLOG_PROCESS_ERROR(nSilverWage >= 0);

    Lua_PushNumber(L, nSilverWage);
    nResult = 1;
Exit0:
    return nResult;
}

int KTong::LuaGetHistoryRange(Lua_State* L)
{
    int     nResult         = 0;
    int     nTopIndex       = 0;
    int     nType           = 0;
    DWORD   dwTopIndex      = 0;
    int     nRecordCount    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nType = (int)Lua_ValueToNumber(L ,1);
    KGLOG_PROCESS_ERROR(nType >= 0 && nType < ohtTotal);

    dwTopIndex      = m_History.m_dwIndex[nType];
    nRecordCount    = (int)m_History.m_RecordList[nType].size();

    Lua_PushNumber(L, dwTopIndex);
    Lua_PushNumber(L, nRecordCount);
    
    nResult = 2;
Exit0:
    return nResult;
}

int KTong::LuaSyncTongHistoryRecord(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nTopIndex       = 0;
    DWORD   dwPlayerID      = ERROR_ID;
    int     nType           = 0;
    DWORD   dwRangeBegin    = 0;
    DWORD   dwRangeEnd      = 0;

    nTopIndex =  Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 4);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwPlayerID);

    nType = (int)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(nType >= 0 && nType < ohtTotal);

    dwRangeBegin = (DWORD)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(dwRangeBegin);

    dwRangeEnd = (DWORD)Lua_ValueToNumber(L, 4);
    KGLOG_PROCESS_ERROR(dwRangeEnd);

    KGLOG_PROCESS_ERROR(dwRangeBegin <= dwRangeEnd);

    bResult = m_History.SyncRecord(dwPlayerID, (KTONG_OPERATION_HISTORY_TYPE)nType, dwRangeBegin, dwRangeEnd);
    KGLOG_PROCESS_ERROR(bResult);
Exit0:
    return 0;
}

///////////////////////////////////////////////////////////////////////////

DEFINE_LUA_CLASS_BEGIN(KTong)

    REGISTER_LUA_DWORD_READONLY(KTong, ID)
    REGISTER_LUA_STRING_READONLY(KTong, Name)
    REGISTER_LUA_DWORD_READONLY(KTong, Master)
    REGISTER_LUA_ENUM_READONLY(KTong, Camp)
    REGISTER_LUA_INTEGER_READONLY(KTong, Fund)
    REGISTER_LUA_INTEGER_READONLY(KTong, Level)
    REGISTER_LUA_INTEGER_READONLY(KTong, DevelopmentPoint)
    REGISTER_LUA_INTEGER_READONLY(KTong, UsedDevelopmentPoint)
    REGISTER_LUA_INTEGER_READONLY(KTong, MaxMemberCount)
    REGISTER_LUA_INTEGER_READONLY(KTong, MaxWageRate)

    REGISTER_LUA_FUNC(KTong, SetLevel)
    REGISTER_LUA_FUNC(KTong, AddFund)
    REGISTER_LUA_FUNC(KTong, AddDevelopmentPoint)
    REGISTER_LUA_FUNC(KTong, ConsumeDevelopmentPoint)

    REGISTER_LUA_FUNC(KTong, GetTechNodeLevel)
    REGISTER_LUA_FUNC(KTong, SetTechNodeLevel)

    REGISTER_LUA_FUNC(KTong, IsGroupEnabled)
    REGISTER_LUA_FUNC(KTong, EnableGroup)
    REGISTER_LUA_FUNC(KTong, SetMaxMemberCount)
    REGISTER_LUA_FUNC(KTong, SetCamp)
    REGISTER_LUA_FUNC(KTong, SetMaxWageRate)

    REGISTER_LUA_FUNC(KTong, GetMemberCount)
    REGISTER_LUA_FUNC(KTong, GetGroupID)
    REGISTER_LUA_FUNC(KTong, CanBaseOperate)
    REGISTER_LUA_FUNC(KTong, CanAdvanceOperate)
    REGISTER_LUA_FUNC(KTong, GetGroupWage)

    REGISTER_LUA_FUNC(KTong, ActiveRepertoryPage)
    REGISTER_LUA_FUNC(KTong, GetMemberSalaryAmount)

    REGISTER_LUA_FUNC(KTong, GetHistoryRange)
    REGISTER_LUA_FUNC(KTong, SyncTongHistoryRecord)
DEFINE_LUA_CLASS_END(KTong)

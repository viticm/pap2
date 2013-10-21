#include "stdafx.h"
#include "KMentorDef.h"
#include "KMentorCenter.h"
#include "KSO3GameCenter.h"
#include "KRole.h"

int KMentorCenter::LuaRegisterApplyMentor(Lua_State* L)
{
    int                                 nResult             = 0;
    DWORD                               dwMentorID          = ERROR_ID;
    DWORD                               dwApprenticeID      = ERROR_ID;
    uint64_t                            uKey                = 0;
    int                                 nMaxApprenticeNum   = 0;
    int                                 nApprenticeCount    = 0;
    int                                 nMentorCount        = 0;
    KMaxApprenticeNumTable::iterator    ItMaxANum;

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);

    uKey = MAKE_INT64(dwMentorID, dwApprenticeID);

    ItMaxANum = m_MaxApprenticeNumTable.find(dwMentorID);
    KGLOG_PROCESS_ERROR(ItMaxANum != m_MaxApprenticeNumTable.end());

    nMaxApprenticeNum = ItMaxANum->second;

    nApprenticeCount = GetApprenticeCount(dwMentorID);
    KG_PROCESS_ERROR(nApprenticeCount < nMaxApprenticeNum);

    nMentorCount = GetMentorCount(dwApprenticeID);
    KG_PROCESS_ERROR(nMentorCount < MAX_MENTOR_NUM);

    m_ApplyMentorTable[uKey] = g_pSO3GameCenter->m_nTimeNow + MENTOR_APPLY_TIMEOUT_TIME;

    Lua_PushBoolean(L, true);
    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCenter::LuaRegisterApplyApprentice(Lua_State* L)
{
    int                                 nResult             = 0;
    DWORD                               dwMentorID          = ERROR_ID;
    DWORD                               dwApprenticeID      = ERROR_ID;
    uint64_t                            uKey                = 0;
    int                                 nMaxApprenticeNum   = 0;
    int                                 nApprenticeCount    = 0;
    int                                 nMentorCount        = 0;
    KMaxApprenticeNumTable::iterator    ItMaxANum;

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);

    uKey = MAKE_INT64(dwMentorID, dwApprenticeID);

    ItMaxANum = m_MaxApprenticeNumTable.find(dwMentorID);
    KGLOG_PROCESS_ERROR(ItMaxANum != m_MaxApprenticeNumTable.end());

    nMaxApprenticeNum = ItMaxANum->second;

    nApprenticeCount = GetApprenticeCount(dwMentorID);
    KG_PROCESS_ERROR(nApprenticeCount < nMaxApprenticeNum);

    nMentorCount = GetMentorCount(dwApprenticeID);
    KG_PROCESS_ERROR(nMentorCount < MAX_MENTOR_NUM);

    m_ApplyApprenticeTable[uKey] = g_pSO3GameCenter->m_nTimeNow + MENTOR_APPLY_TIMEOUT_TIME;

    Lua_PushBoolean(L, true);
    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCenter::LuaGetMentorCount(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nMentor         = 0;
    int     nTopIndex       = 0;
    DWORD   dwApprenticeID  = ERROR_ID;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwApprenticeID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwApprenticeID != ERROR_ID);

    nMentor = GetMentorCount(dwApprenticeID);

    Lua_PushNumber(L, nMentor);

    bResult = true;
Exit0:
    if (!bResult)
    {
        Lua_PushNil(L);
    }
    return 1;
}

int KMentorCenter::LuaGetApprenticeCount(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nApprentice = 0;
    int     nTopIndex   = 0;
    DWORD   dwMentorID  = ERROR_ID;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMentorID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwMentorID != ERROR_ID);

    nApprentice = GetApprenticeCount(dwMentorID);

    Lua_PushNumber(L, nApprentice);

    bResult = true;
Exit0:
    if (!bResult)
    {
        Lua_PushNil(L);
    }
    return 1;
}

int KMentorCenter::LuaTakeMentor(Lua_State* L)
{
    int                                 nResult             = 0;
    BOOL                                bRetCode            = false;
    DWORD                               dwMentorID          = ERROR_ID;
    DWORD                               dwApprenticeID      = ERROR_ID;
    uint64_t                            uKey                = 0;
    int                                 nMaxApprenticeNum   = 0;
    int                                 nApprenticeCount    = 0;
    int                                 nMentorCount        = 0;
    KRole*                              pApprenticeRole     = NULL;
    KApplyMentorTable::iterator         ItApply;
    KMaxApprenticeNumTable::iterator    ItMaxANum;

    dwMentorID          = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID      = (DWORD)Lua_ValueToNumber(L, 2);

    uKey = MAKE_INT64(dwMentorID, dwApprenticeID);
    ItApply = m_ApplyMentorTable.find(uKey);
    KGLOG_PROCESS_ERROR(ItApply != m_ApplyMentorTable.end());

    m_ApplyMentorTable.erase(ItApply);

    ItMaxANum = m_MaxApprenticeNumTable.find(dwMentorID);
    KGLOG_PROCESS_ERROR(ItMaxANum != m_MaxApprenticeNumTable.end());

    nMaxApprenticeNum = ItMaxANum->second;

    nApprenticeCount = GetApprenticeCount(dwMentorID);
    KG_PROCESS_ERROR(nApprenticeCount < nMaxApprenticeNum);

    nMentorCount = GetMentorCount(dwApprenticeID);
    KG_PROCESS_ERROR(nMentorCount < MAX_MENTOR_NUM);

    pApprenticeRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);
    KGLOG_PROCESS_ERROR(pApprenticeRole);

    bRetCode = CreateShip(dwMentorID, dwApprenticeID, pApprenticeRole->m_byLevel);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_LogServerAgency.LogMentorshipCreate(dwMentorID, dwApprenticeID, "Apprentice Apply");

    Lua_PushBoolean(L, true);
    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCenter::LuaTakeApprentice(Lua_State* L)
{
    int                                 nResult             = 0;
    BOOL                                bRetCode            = false;
    DWORD                               dwMentorID          = ERROR_ID;
    DWORD                               dwApprenticeID      = ERROR_ID;
    uint64_t                            uKey                = 0;
    int                                 nMaxApprenticeNum   = 0;
    int                                 nApprenticeCount    = 0;
    int                                 nMentorCount        = 0;
    KRole*                              pApprenticeRole     = NULL;
    KApplyMentorTable::iterator         ItApply;
    KMaxApprenticeNumTable::iterator    ItMaxANum;

    dwMentorID          = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID      = (DWORD)Lua_ValueToNumber(L, 2);

    uKey = MAKE_INT64(dwMentorID, dwApprenticeID);
    ItApply = m_ApplyApprenticeTable.find(uKey);
    KGLOG_PROCESS_ERROR(ItApply != m_ApplyApprenticeTable.end());

    m_ApplyApprenticeTable.erase(ItApply);

    ItMaxANum = m_MaxApprenticeNumTable.find(dwMentorID);
    KGLOG_PROCESS_ERROR(ItMaxANum != m_MaxApprenticeNumTable.end());

    nMaxApprenticeNum = ItMaxANum->second;

    nApprenticeCount = GetApprenticeCount(dwMentorID);
    KG_PROCESS_ERROR(nApprenticeCount < nMaxApprenticeNum);

    nMentorCount = GetMentorCount(dwApprenticeID);
    KG_PROCESS_ERROR(nMentorCount < MAX_MENTOR_NUM);

    pApprenticeRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);
    KGLOG_PROCESS_ERROR(pApprenticeRole);
    
    bRetCode = CreateShip(dwMentorID, dwApprenticeID, pApprenticeRole->m_byLevel);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_LogServerAgency.LogMentorshipCreate(dwMentorID, dwApprenticeID, "Mentor Apply");

    Lua_PushBoolean(L, true);

    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCenter::LuaGetMentorList(Lua_State* L)
{
    int                     nResult     = 0;
    DWORD                   dwPlayerID  = ERROR_ID;
    uint64_t                uKeyLower   = 0;
    uint64_t                uKeyUpper   = 0;
    int                     nCount      = 1;
    KMentorTable::iterator  ItLower;
    KMentorTable::iterator  ItUpper;

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);

    uKeyLower = MAKE_INT64(dwPlayerID, 0);
    uKeyUpper = MAKE_INT64(dwPlayerID + 1, 0);

    KGLOG_PROCESS_ERROR(uKeyLower < uKeyUpper);
    ItLower = m_A2MTable.lower_bound(uKeyLower);
    ItUpper = m_A2MTable.lower_bound(uKeyUpper);

    Lua_NewTable(L);

    for (KMentorTable::iterator it = ItLower; it != ItUpper && it != m_A2MTable.end(); ++it)
    {
        int   nState     = it->second.byState;
        DWORD dwMentorID = LOW_DWORD_IN_UINT64(it->first);

        if (nState == emrsBroken || nState == emrsGraduating || nState == emrsGraduated)
            continue;
        
        Lua_PushNumber(L, nCount);
        Lua_PushNumber(L, dwMentorID);

        Lua_SetTable(L, -3);
        nCount++;
    }

    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCenter::LuaGetApprenticeList(Lua_State* L)
{
    int                         nResult     = 0;
    DWORD                       dwPlayerID  = ERROR_ID;
    uint64_t                    uKeyLower   = 0;
    uint64_t                    uKeyUpper   = 0;
    int                         nCount      = 0;
    KApprenticeTable::iterator  ItLower;
    KApprenticeTable::iterator  ItUpper;

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);

    uKeyLower = MAKE_INT64(dwPlayerID, 0);
    uKeyUpper = MAKE_INT64(dwPlayerID + 1, 0);

    KGLOG_PROCESS_ERROR(uKeyLower < uKeyUpper);
    ItLower = m_M2ATable.lower_bound(uKeyLower);
    ItUpper = m_M2ATable.lower_bound(uKeyUpper);

    Lua_NewTable(L);

    for (KApprenticeTable::iterator it = ItLower; it != ItUpper && it != m_M2ATable.end(); ++it)
    {
        DWORD dwApprenticeID = LOW_DWORD_IN_UINT64(it->first);

        Lua_PushNumber(L, ++nCount);
        Lua_PushNumber(L, dwApprenticeID);

        Lua_SetTable(L, -3);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int KMentorCenter::LuaGetRecord(Lua_State* L)
{
    BOOL                    bResult         = false;
    int                     nTopIndex       = 0;
    DWORD                   dwMentorID      = ERROR_ID;
    DWORD                   dwApprenticeID  = ERROR_ID;
    KMentorInfo*            pRecordDB       = NULL;
    
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);

    pRecordDB = GetMentorInfo(dwMentorID, dwApprenticeID);
    KG_PROCESS_ERROR(pRecordDB);

    Lua_NewTable(L);

    Lua_PushString(L, "CreateTime");
    Lua_PushNumber(L, pRecordDB->nCreateTime);
    Lua_SetTable(L, -3);
    
    Lua_PushString(L, "MentorValue");
    Lua_PushNumber(L, pRecordDB->nMentorValue);
    Lua_SetTable(L, -3);
    
    Lua_PushString(L, "State");
    Lua_PushNumber(L, pRecordDB->byState);
    Lua_SetTable(L, -3);
    
    Lua_PushString(L, "EndTime");
    Lua_PushNumber(L, pRecordDB->nEndTime);
    Lua_SetTable(L, -3);
    
    Lua_PushString(L, "OriginalALevel");
    Lua_PushNumber(L, pRecordDB->byOriginalALevel);
    Lua_SetTable(L, -3);

    bResult = true;
Exit0:
    if (!bResult)
    {
        Lua_PushNil(L);
    }
    return 1;
}

int KMentorCenter::LuaAddMentorValue(Lua_State* L)
{
    BOOL                    bResult         = false;
    BOOL                    bRetCode        = false;
    int                     nTopIndex       = 0;
    DWORD                   dwMentorID      = ERROR_ID;
    DWORD                   dwApprenticeID  = ERROR_ID;
    int                     nDeltaValue     = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);
    nDeltaValue     = (int)Lua_ValueToNumber(L, 3);

    bRetCode = AddMentorValue(dwMentorID, dwApprenticeID, nDeltaValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KMentorCenter::LuaBreakMentor(Lua_State* L)
{
    int                     nTopIndex       = 0;
    DWORD                   dwMentorID      = ERROR_ID;
    DWORD                   dwApprenticeID  = ERROR_ID;
    KMentorInfo*            pRecordDB       = NULL;
    KRole*                  pMRole          = NULL;
    KRole*                  pARole          = NULL;
    int                     nState          = emrsInvalid;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);

    pRecordDB = GetMentorInfo(dwMentorID, dwApprenticeID);
    KGLOG_PROCESS_ERROR(pRecordDB);

    switch (pRecordDB->byState)
    {
    case emrsNormal:
        pMRole  = g_pSO3GameCenter->m_RoleManager.GetRole(dwMentorID);
        pARole  = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);
        KGLOG_PROCESS_ERROR(pARole); // 徒弟发起的解除，角色肯定存在

        if (pMRole == NULL)
        {
            // 师傅都流失了，直接删除师徒记录
            Delete(dwMentorID, dwApprenticeID);

            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, 0, "Apprentice Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);

            goto Exit0;
        }

        if (g_pSO3GameCenter->m_nTimeNow - pMRole->m_nLastSaveTime < EVIL_BREAK_OFFLINE_TIME)
        {
            // 一周内师傅上过线，延迟三天解除
            pRecordDB->nEndTime = g_pSO3GameCenter->m_nTimeNow + MENTOR_ABREAK_TIMEOUT;
            pRecordDB->byState  = emrsABreak;
            pRecordDB->bSaved   = false;
            nState              = pRecordDB->byState;

            g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pRecordDB);
        }
        else if (pARole->m_byLevel < LOWEST_ALEVEL_ACCQUIRE_MENTOR_VALUE)
        {
            // 一周内师傅没上线且徒弟等级小于20，立即解除，给师傅的师徒值为0
            Delete(dwMentorID, dwApprenticeID);

            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, 0, "Apprentice Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);

        }
        else
        {
            // 一周内师傅没上线但徒弟等级大于20，立即解除，给师傅的师徒值为50%
            pRecordDB->nEndTime     = g_pSO3GameCenter->m_nTimeNow + MENTOR_BROKEN_TIMEOUT;
            pRecordDB->byState      = emrsBroken;
            pRecordDB->nMentorValue = (int)(pRecordDB->nMentorValue * ABREAK_MENTOR_VALUE_RATE);

            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, pRecordDB->nMentorValue,"Apprentice Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);

            if (pRecordDB->nMentorValue == 0)
            {
                Delete(dwMentorID, dwApprenticeID);
            }
            else
            {
                pRecordDB->bSaved       = false;
                nState                  = pRecordDB->byState;
                g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pRecordDB);
            }
        }
        break;

    case emrsABreak:
        nState = pRecordDB->byState;
        break;

    case emrsMBreak:
        // 被师傅解除且处于延迟解除中，立即解除，给师傅的师徒值为0
        Delete(dwMentorID, dwApprenticeID);

        g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
            dwApprenticeID, dwMentorID, 0, "Apprentice Break"
        );

        g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);

        break;

    case emrsBroken:
    case emrsGraduating:
    case emrsGraduated:
        nState = pRecordDB->byState;
        break;

    default:
        assert(false);
    }

Exit0:
    Lua_PushNumber(L, nState);
    return 1;
}

int KMentorCenter::LuaBreakApprentice(Lua_State* L)
{
    int                      nTopIndex         = 0;
    DWORD                    dwMentorID        = ERROR_ID;
    DWORD                    dwApprenticeID    = ERROR_ID;
    KMentorInfo*             pRecordDB         = NULL;
    KRole*                   pARole            = NULL;
    int                      nState           = emrsInvalid;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);

    pRecordDB = GetMentorInfo(dwMentorID, dwApprenticeID);
    KGLOG_PROCESS_ERROR(pRecordDB);

    // 注意：取不到Role当正常处理
    pARole  = g_pSO3GameCenter->m_RoleManager.GetRole(dwApprenticeID);

    switch (pRecordDB->byState)
    {
    case emrsNormal:  
        if (pARole == NULL)
        {
            // 徒弟流失了，直接删除师徒记录
            Delete(dwMentorID, dwApprenticeID);

            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, 0, "Mentor Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);


            goto Exit0;
        }

        if (g_pSO3GameCenter->m_nTimeNow - pARole->m_nLastSaveTime < EVIL_BREAK_OFFLINE_TIME)
        {
            // 徒弟一周内上过线，延迟三天解除
            pRecordDB->nEndTime = g_pSO3GameCenter->m_nTimeNow + MENTOR_MBREAK_TIMEOUT;
            pRecordDB->byState  = emrsMBreak;
            pRecordDB->bSaved   = false;
            nState              = pRecordDB->byState;

            g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pRecordDB);
        }
        else if (pARole->m_byLevel < LOWEST_ALEVEL_ACCQUIRE_MENTOR_VALUE)
        {
            // 徒弟一周内没上线且徒弟等级小于20，立即解除，给师傅的师徒值为0
            Delete(dwMentorID, dwApprenticeID);

            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, 0, "Mentor Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);

        }
        else
        {
            // 徒弟一周内没上线且徒弟等级大于20，立即解除，给师傅的师徒值为50%
            pRecordDB->nEndTime     = g_pSO3GameCenter->m_nTimeNow + MENTOR_BROKEN_TIMEOUT;
            pRecordDB->byState      = emrsBroken;
            pRecordDB->nMentorValue = (int)(pRecordDB->nMentorValue * MBREAK_MENTOR_VALUE_RATE);
            
            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, pRecordDB->nMentorValue, "Mentor Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);


            if (pRecordDB->nMentorValue == 0)
            {
                Delete(dwMentorID, dwApprenticeID);
            }
            else
            {
                pRecordDB->bSaved       = false;
                nState                  = pRecordDB->byState;
                g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pRecordDB);
            }
        }
        break;

    case emrsABreak: 
        if (pARole == NULL || pARole->m_byLevel < LOWEST_ALEVEL_ACCQUIRE_MENTOR_VALUE)
        {
            // 被徒弟解除中且徒弟已流失，直接删除师徒记录，或
            // 被徒弟解除中且徒弟等级小于20，立即解除，给师傅的师徒值为0
            Delete(dwMentorID, dwApprenticeID);

            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, 0, "Mentor Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);
        }
        else
        {
            // 被徒弟解除中且徒弟等级大于20，立即解除，给师傅的师徒值为60%
            pRecordDB->nEndTime     = g_pSO3GameCenter->m_nTimeNow + MENTOR_BROKEN_TIMEOUT;
            pRecordDB->byState      = emrsBroken;
            pRecordDB->nMentorValue = (int)(pRecordDB->nMentorValue * EVIL_ABREAK_MENTOR_VALUE_RATE);

            g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
                dwMentorID, dwApprenticeID, pRecordDB->nMentorValue, "Mentor Break"
            );

            g_pSO3GameCenter->m_StatDataManager.UpdateMentorBrokenStat(dwMentorID, dwApprenticeID);

            if (pRecordDB->nMentorValue == 0)
            {
                Delete(dwMentorID, dwApprenticeID);
            }
            else
            {
                pRecordDB->bSaved       = false;
                nState                  = pRecordDB->byState;
                g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pRecordDB);
            }
        }
        break;

    case emrsMBreak:
    case emrsBroken:
    case emrsGraduating:
    case emrsGraduated:
        nState = pRecordDB->byState;
        break;

    default:
        assert(false);
    }

Exit0:
    Lua_PushNumber(L, nState);
    return 1;
}

int KMentorCenter::LuaCancelBreak(Lua_State* L)
{
    int                      nTopIndex         = 0;
    DWORD                    dwMentorID        = ERROR_ID;
    DWORD                    dwApprenticeID    = ERROR_ID;
    uint64_t                 uKey              = 0;
    KMentorInfo*             pRecordDB         = NULL;
    int                      nState            = emrsInvalid;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);

    uKey = MAKE_INT64(dwApprenticeID, dwMentorID);

    pRecordDB = GetMentorInfo(dwMentorID, dwApprenticeID);
    KGLOG_PROCESS_ERROR(pRecordDB);

    KGLOG_PROCESS_ERROR(pRecordDB->byState == emrsMBreak || pRecordDB->byState == emrsABreak);

    pRecordDB->byState  = emrsNormal;
    pRecordDB->nEndTime = g_pSO3GameCenter->m_nTimeNow + MENTOR_NORMAL_TIMEOUT;
    pRecordDB->bSaved   = false;

    g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pRecordDB);

    nState = pRecordDB->byState;

Exit0:
    Lua_PushNumber(L, nState);
    return 1;
}

int KMentorCenter::LuaAcquireMentorValue(Lua_State* L)
{
    BOOL                    bResult         = false;
    int                     nTopIndex       = 0;
    DWORD                   dwMentorID      = ERROR_ID;
    DWORD                   dwApprenticeID  = ERROR_ID;
    KMentorInfo*            pRecordDB       = NULL;
    int                     nAcquireValue   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);
    nAcquireValue   = (int)Lua_ValueToNumber(L, 3);

    KGLOG_PROCESS_ERROR(nAcquireValue >= 0);

    pRecordDB = GetMentorInfo(dwMentorID, dwApprenticeID);
    KGLOG_PROCESS_ERROR(pRecordDB);
    
    KGLOG_PROCESS_ERROR(pRecordDB->byState == emrsBroken || pRecordDB->byState == emrsGraduated);
    KGLOG_PROCESS_ERROR(pRecordDB->nMentorValue >= nAcquireValue);
    
    pRecordDB->nMentorValue -= nAcquireValue;
    pRecordDB->bSaved        = false;

    g_pSO3GameCenter->m_LogServerAgency.LogAcquireMentorValue(
        dwMentorID, dwApprenticeID, nAcquireValue, "Acquire Mentor Value"
    );

    if (pRecordDB->nMentorValue == 0)
    {
        Delete(dwMentorID, dwApprenticeID);
    }
    else
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pRecordDB);
    }

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KMentorCenter::LuaSeekMentorYell(Lua_State* L)
{
    int     nTopIndex    = 0;
    DWORD   dwYellRoleID = ERROR_ID;
    int     nMentorCount = 0;
    KRole*  pRole        = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwYellRoleID = (DWORD)Lua_ValueToNumber(L, 1);
    
    nMentorCount = GetMentorCount(dwYellRoleID);
    KG_PROCESS_ERROR(nMentorCount < MAX_MENTOR_NUM);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwYellRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    g_pSO3GameCenter->m_GameServer.BroadcastSeekMentorYell(pRole);

Exit0:
    return 0;
}

int KMentorCenter::LuaSeekApprenticeYell(Lua_State* L)
{
    int     nTopIndex         = 0;
    DWORD   dwYellRoleID      = ERROR_ID;
    int     nApprenticeCount  = 0;
    int     nMaxApprenticeNum = 0;
    KRole*  pRole             = NULL;
    KMaxApprenticeNumTable::iterator    ItMaxANum;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwYellRoleID = (DWORD)Lua_ValueToNumber(L, 1);

    nApprenticeCount  = GetApprenticeCount(dwYellRoleID);
    
    ItMaxANum = m_MaxApprenticeNumTable.find(dwYellRoleID);
    KGLOG_PROCESS_ERROR(ItMaxANum != m_MaxApprenticeNumTable.end());

    nMaxApprenticeNum = ItMaxANum->second;

    KG_PROCESS_ERROR(nApprenticeCount < nMaxApprenticeNum);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwYellRoleID);
    KGLOG_PROCESS_ERROR(pRole);

    g_pSO3GameCenter->m_GameServer.BroadcastSeekApprenticeYell(pRole);

Exit0:
    return 0;
}

int KMentorCenter::LuaApprenticeGraduate(Lua_State* L)
{
    int                     nTopIndex       = 0;
    DWORD                   dwApprenticeID  = ERROR_ID;
    DWORD                   dwMentorID      = ERROR_ID;
    KMentorInfo*            pRecordDB       = NULL;
    KRole*                  pMRole          = NULL;
    int                     nState          = emrsInvalid;
    int                     bDelete         = false;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMentorID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwApprenticeID  = (DWORD)Lua_ValueToNumber(L, 2);

    pRecordDB = GetMentorInfo(dwMentorID, dwApprenticeID);
    KGLOG_PROCESS_ERROR(pRecordDB);

    KGLOG_PROCESS_ERROR(pRecordDB->byState != emrsBroken && pRecordDB->byState != emrsGraduated);

    pMRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwMentorID);
    if (!pMRole)
    {
        Delete(dwMentorID, dwApprenticeID);
        goto Exit0;
    }

    pRecordDB->bSaved = false;
    if (pMRole->IsOnline())
    {
        pRecordDB->nEndTime  = g_pSO3GameCenter->m_nTimeNow + MENTOR_GRATUATED_TIMEOUT;
        pRecordDB->byState   = emrsGraduated;

        if (pRecordDB->nMentorValue == 0)
            bDelete = true;

        g_pSO3GameCenter->m_LogServerAgency.LogMentorshipDelete(
            dwMentorID, dwApprenticeID, pRecordDB->nMentorValue, "Apprentice Graduate"
        );

        g_pSO3GameCenter->m_StatDataManager.UpdateMentorGraduateStat(dwMentorID, dwApprenticeID);
    }
    else
    {
        pRecordDB->nEndTime  = g_pSO3GameCenter->m_nTimeNow + MENTOR_GRATUATING_TIMEOUT;
        pRecordDB->byState   = emrsGraduating;
    }
    nState = pRecordDB->byState;

    if (bDelete)
    {
        Delete(dwMentorID, dwApprenticeID);
    }
    else
    {
        g_pSO3GameCenter->m_GameServer.DoUpdateMentorRecord(dwMentorID, dwApprenticeID, *pRecordDB);        
    }

Exit0:
    Lua_PushNumber(L, nState);
    return 1;
}

int KMentorCenter::LuaGetRelationCount(Lua_State* L)
{
    int nRelationCount = GetRelationCount();

    Lua_PushNumber(L, nRelationCount);
    return 1;
}

DEFINE_LUA_CLASS_BEGIN(KMentorCenter)
    REGISTER_LUA_FUNC(KMentorCenter, RegisterApplyMentor)
    REGISTER_LUA_FUNC(KMentorCenter, RegisterApplyApprentice)
    REGISTER_LUA_FUNC(KMentorCenter, GetMentorCount)
    REGISTER_LUA_FUNC(KMentorCenter, GetApprenticeCount)
    REGISTER_LUA_FUNC(KMentorCenter, TakeMentor)
    REGISTER_LUA_FUNC(KMentorCenter, TakeApprentice)
    REGISTER_LUA_FUNC(KMentorCenter, GetMentorList)
    REGISTER_LUA_FUNC(KMentorCenter, GetApprenticeList)
    REGISTER_LUA_FUNC(KMentorCenter, GetRecord)
    REGISTER_LUA_FUNC(KMentorCenter, AddMentorValue)
    REGISTER_LUA_FUNC(KMentorCenter, BreakMentor)
    REGISTER_LUA_FUNC(KMentorCenter, BreakApprentice)
    REGISTER_LUA_FUNC(KMentorCenter, CancelBreak)
    REGISTER_LUA_FUNC(KMentorCenter, AcquireMentorValue)
    REGISTER_LUA_FUNC(KMentorCenter, SeekMentorYell)
    REGISTER_LUA_FUNC(KMentorCenter, SeekApprenticeYell)
    REGISTER_LUA_FUNC(KMentorCenter, ApprenticeGraduate)
    REGISTER_LUA_FUNC(KMentorCenter, GetRelationCount)
DEFINE_LUA_CLASS_END(KMentorCenter)

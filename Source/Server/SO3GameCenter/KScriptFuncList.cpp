#include "stdafx.h"
#include "KSO3GameCenter.h"
#include "KMapInfo.h"
#include "KMapManager.h"
#include "KMapInfo.h"

int LuaTest(Lua_State* L)
{
    puts("Lua test function");
	return 0;
}

int LuaSendGmMessage(Lua_State* L)
{
    int         nTopIndex   = 0;
    const char* pszGmName  = NULL;
    const char* pszMessage = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    pszGmName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszGmName);

    pszMessage = Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszMessage);

    g_pSO3GameCenter->m_Eyes.DoSendGmMessage(pszGmName, pszMessage);

Exit0:
    return 0;
}

int LuaGetAccountRoleList(Lua_State* L)
{
    int         nRoleCount  = 0;
    int         nResult     = 0;
    int         nRetCode    = 0;
    int         nTopIndex   = 0;
    int         nTableIndex = 0;
    const char* pszAcc      = NULL;
    KAccount*   pAccount    = NULL;
    KRole*      pRole       = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszAcc = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszAcc);

    pAccount = g_pSO3GameCenter->m_RoleManager.GetAccount(pszAcc);
    KG_PROCESS_ERROR(pAccount);

    Lua_NewTable(L);
    nTableIndex = Lua_GetTopIndex(L);

    nRoleCount = (int)pAccount->vRoleList.size();

    for (int i = 0; i < nRoleCount; i++)
    {
        pRole = pAccount->vRoleList[i];

        assert(pRole);
        
        Lua_PushNumber(L, i + 1);
        nRetCode = pRole->LuaGetObj(L);
        KGLOG_PROCESS_ERROR(nRetCode);

        Lua_SetTable(L, nTableIndex);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int LuaKickout(Lua_State* L)
{
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    const char* pszRoleName = NULL;
    KRole*      pRole       = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszRoleName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszRoleName);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pszRoleName);
    KGLOG_PROCESS_ERROR(pRole);

    bRetCode = pRole->IsOnline();
    KGLOG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_GameServer.DoKickAccountNotify(pRole->m_dwPlayerID);

Exit0:
    return 0;
}

int LuaSendGlobalSysMsg(Lua_State* L)
{
	int         nTopIndex   = 0;
    const char* pszMsg      = NULL;

    nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszMsg = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszMsg);

    g_pSO3GameCenter->m_GameServer.SendGlobalSysMsg(pszMsg);

Exit0:
    return 0;
}

int LuaPlayerExecute(Lua_State* L)
{
    int         nRetCode    = false;
    const char* pszRoleName = NULL;
    KRole*      pRole       = NULL;
    const char* pszScript   = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    pszRoleName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszRoleName);

    pszScript = Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszScript);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pszRoleName);
    KGLOG_PROCESS_ERROR(pRole);

    nRetCode = pRole->IsOnline();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = g_pSO3GameCenter->m_GameServer.DoGmCommand(
        pRole->m_nConnIndex, pRole->m_dwPlayerID, 
        pszRoleName, pszScript
    );
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int LuaFreezeRole(Lua_State* L)
{
    int         nRetCode        = false;
    const char* pszRoleName     = NULL;
    int         nFreezeSecond   = 0;
    time_t      nFreezeEndTime  = 0;
    KRole*      pRole           = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    pszRoleName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszRoleName);

    nFreezeSecond = (int)Lua_ValueToNumber(L, 2);

    nFreezeEndTime = g_pSO3GameCenter->m_nTimeNow + nFreezeSecond;

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pszRoleName);
    KGLOG_PROCESS_ERROR(pRole);

    nRetCode = g_pSO3GameCenter->m_RoleManager.FreezeRole(pRole->m_dwPlayerID, nFreezeEndTime);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = g_pSO3GameCenter->m_MiscDB.DoUpdateRoleBlackList(pRole->m_dwPlayerID, nFreezeEndTime);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int LuaSendSystemMail(Lua_State* L)
{
    BOOL            bRetCode        = false;
    int             nTopIndex       = 0;
    char*           pszSender       = NULL;
    char*           pszReciever     = NULL;
    char*           pszMailTitle    = NULL;
    char*           pszText         = NULL;
    unsigned int    uTextLen        = 0;
    KRole*          pDstRole        = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 4);

    pszSender       = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszSender);

    pszReciever     = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszReciever);

    pszMailTitle    = (char*)Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pszMailTitle);

    pszText         = (char*)Lua_ValueToString(L, 4);
    KGLOG_PROCESS_ERROR(pszText);

    pDstRole = g_pSO3GameCenter->m_RoleManager.GetRole(pszReciever);
    KGLOG_PROCESS_ERROR(pDstRole);

    uTextLen = (unsigned)(strlen(pszText) + 1);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
        0, pDstRole->m_dwPlayerID, 0, eMailType_System,
        pszSender, pszMailTitle, 
        0,
        (BYTE*)pszText, uTextLen,
        NULL, 0, 0
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int LuaSendGmMessageMail(Lua_State* L)
{
    BOOL            bRetCode        = false;
    int             nTopIndex       = 0;
    char*           pszSender       = NULL;
    char*           pszReciever     = NULL;
    char*           pszMailTitle    = NULL;
    char*           pszText         = NULL;
    unsigned int    uTextLen        = 0;
    KRole*          pDstRole        = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 4);

    pszSender    = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszSender);

    pszReciever  = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszReciever);

    pszMailTitle = (char*)Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pszMailTitle);

    pszText      = (char*)Lua_ValueToString(L, 4);
    KGLOG_PROCESS_ERROR(pszText);
    KGLOG_PROCESS_ERROR(strlen(pszText) > 0);

    pDstRole = g_pSO3GameCenter->m_RoleManager.GetRole(pszReciever);
    KGLOG_PROCESS_ERROR(pDstRole);

    uTextLen = (unsigned)(strlen(pszText) + 1);

    bRetCode = g_pSO3GameCenter->m_MailManager.PostRequestSendMail(
        0, pDstRole->m_dwPlayerID, 0, eMailType_GmMessage,
        pszSender, pszMailTitle, 
        0,
        (BYTE*)pszText, uTextLen,
        NULL, 0, 0
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int LuaSendGlobalMail(Lua_State* L)
{
    BOOL            bRetCode        = false;
    int             nTopIndex       = 0;
    char*           pszSender       = NULL;
    char*           pszMailTitle    = NULL;
    char*           pszText         = NULL;
    int             nLifeTime       = 60 * 60 * 24 * 7;
    unsigned int    uTextLen        = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex >= 3);
    KGLOG_PROCESS_ERROR(nTopIndex <= 4);

    pszSender       = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszSender);

    pszMailTitle    = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszMailTitle);

    pszText         = (char*)Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pszText);

    if (nTopIndex == 4)
    {
        nLifeTime = (int)Lua_ValueToNumber(L, 4);
    }
    KGLOG_PROCESS_ERROR(nLifeTime > 0);

    uTextLen = (unsigned)(strlen(pszText) + 1);

    bRetCode = g_pSO3GameCenter->m_MailManager.SendGlobalMail(
        nLifeTime, eMailType_System, pszSender, pszMailTitle, 0, (BYTE*)pszText, uTextLen, NULL, 0, 0
    );
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int LuaDeleteGlobalMail(Lua_State* L)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    DWORD       dwMailID    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMailID = (DWORD)Lua_ValueToNumber(L, 1);

    bRetCode = g_pSO3GameCenter->m_MailManager.DeleteGlobalMail(dwMailID);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaDeleteAllGlobalMail(Lua_State* L)
{
    g_pSO3GameCenter->m_MailManager.DeleteAllGlobalMail();

    return 0;
}

int LuaGetPQ(Lua_State* L)
{
    int     nResult     = 0;
    int     nTopIndex   = 0;
    DWORD   dwPQID      = 0;
    KGPQ*   pPQ         = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQID = (DWORD)Lua_ValueToNumber(L, 1);

    pPQ = g_pSO3GameCenter->m_PQCenter.GetPQ(dwPQID);
    KGLOG_PROCESS_ERROR(pPQ);

    nResult = pPQ->LuaGetObj(L);
Exit0:
    return nResult;
}

int LuaCreatePQ(Lua_State* L)
{
    int     nResult         = 0;
    int     nTopIndex       = 0;
    DWORD   dwPQTemplateID  = 0;
    int     nOwnerType      = pqotInvalid;
    DWORD   dwOwnerID1      = 0;
    DWORD   dwOwnerID2      = 0;
    DWORD   dwOwnerID3      = 0;
    DWORD   dwPQID          = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1 || nTopIndex == 5);

    dwPQTemplateID = (DWORD)Lua_ValueToNumber(L, 1);

    if (nTopIndex == 5)
    {
        nOwnerType = (int)Lua_ValueToNumber(L, 2);
        dwOwnerID1 = (DWORD)Lua_ValueToNumber(L, 3);
        dwOwnerID2 = (DWORD)Lua_ValueToNumber(L, 4);
        dwOwnerID3 = (DWORD)Lua_ValueToNumber(L, 5);
    }

    dwPQID = g_pSO3GameCenter->m_PQCenter.CreatePQ(dwPQTemplateID, (KGPQ_OWNER_TYPE)nOwnerType, dwOwnerID1, dwOwnerID2, dwOwnerID3);

Exit0:
    Lua_PushNumber(L, dwPQID);
    return 1;
}

int LuaRandom(Lua_State* L)
{
    int nResult = 0;
    int nRetCode = 0;
    int nX = 0;
    int nY = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1 || nRetCode == 2);

    if (nRetCode == 1)
    {
        nX = 1;
        nY = (int)Lua_ValueToNumber(L, 1);
    }
    else if (nRetCode == 2)
    {
        nX = (int)Lua_ValueToNumber(L, 1);
        nY = (int)Lua_ValueToNumber(L, 2);
    }
    else
    {
        KGLOG_PROCESS_ERROR(false);
    }

    KGLOG_PROCESS_ERROR(nX >= 1 && nY >= nX);

    nResult = g_Random(nY - nX + 1) + nX;
Exit0:
    lua_pushinteger(L, nResult);
    return 1;
}

int LuaRefreshMapCopy(Lua_State* L)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    int         nTopIndex       = 0;
    DWORD       dwMapID         = 0;
    int         nMapCopyIndex   = 0;
    KMapCopy*   pMapCopy        = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMapID       = (DWORD)Lua_ValueToNumber(L, 1);
    nMapCopyIndex = (int)Lua_ValueToNumber(L, 2);

    pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(dwMapID, nMapCopyIndex);
    KGLOG_PROCESS_ERROR(pMapCopy);
    KGLOG_PROCESS_ERROR(pMapCopy->m_nConnIndex);

    bRetCode = g_pSO3GameCenter->m_GameServer.DoMapCopyDoClearPlayerPrepare(
        pMapCopy->m_nConnIndex, dwMapID, nMapCopyIndex, bpcMapRefresh, BANISH_PLAYER_WAIT_SECONDS
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    pMapCopy->m_eState       = eMapStateRefreshing;
    pMapCopy->m_nRefreshTime = g_pSO3GameCenter->m_nTimeNow + BANISH_PLAYER_WAIT_SECONDS;

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int LuaRemoteCall(Lua_State* L)
{
    return g_pSO3GameCenter->m_RemoteScript.Call(L);
}

int LuaLog(Lua_State* L)
{
    int         nTopIndex  = 0;
    const char* pszLogText = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszLogText = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszLogText);

    KGLogPrintf(KGLOG_DEBUG, "%s\n", pszLogText);

Exit0:
    return 0;
}

int LuaReloadScripts(Lua_State* L)
{
    g_pSO3GameCenter->m_ScriptManager.Reload();
    return 0;
}

struct FIND_PQID_BY_TEMPLATE  
{
    BOOL operator () (const KGPQ& crPQ)
    {
        if (crPQ.m_pPQTemplate->dwTemplateID != m_dwTemplateID)
            return true;

        m_PQIDVector.push_back(crPQ.m_dwPQID);

        return true;
    }

    DWORD               m_dwTemplateID;
    std::vector<DWORD>  m_PQIDVector;
};

int LuaGetPQByTemplate(Lua_State* L)
{
    int     nResult         = 0;
    BOOL    bRetCode        = 0;
    DWORD   dwPQTemplateID  = ERROR_ID;
    int     nTopIndex       = 0;
    int     nIndex          = 0;
    FIND_PQID_BY_TEMPLATE   FindPQ;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQTemplateID = (DWORD)Lua_ValueToNumber(L, 1);

    FindPQ.m_dwTemplateID = dwPQTemplateID;
    FindPQ.m_PQIDVector.clear();

    bRetCode = g_pSO3GameCenter->m_PQCenter.Traverse(FindPQ);
    KG_PROCESS_ERROR(bRetCode);

    Lua_NewTable(L);

    for (
        std::vector<DWORD>::iterator it = FindPQ.m_PQIDVector.begin(), itEnd = FindPQ.m_PQIDVector.end(); 
        it != itEnd; 
        ++it
    )
    {
        Lua_PushNumber(L, ++nIndex);
        Lua_PushNumber(L, *it);

        Lua_SetTable(L, -3);
    }

Exit0:
    return 1;
}

int LuaGetGlobalCustomDataManager(Lua_State* L)
{
    return g_pSO3GameCenter->m_GlobalCustomDataManager.LuaGetObj(L);
}

int LuaGetGlobalSystemValueManager(Lua_State* L)
{
    return g_pSO3GameCenter->m_GlobalSystemValueManager.LuaGetObj(L);
}

int LuaGetRole(Lua_State* L)
{
    int     nResult     = 0;
    int     nTopIndex   = 0;
    DWORD   dwRoleID    = 0;
    KRole*  pRole       = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwRoleID = (DWORD)Lua_ValueToNumber(L ,1);
    KGLOG_PROCESS_ERROR(dwRoleID);

    assert(g_pSO3GameCenter);
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KG_PROCESS_ERROR(pRole);

    nResult = pRole->LuaGetObj(L);
Exit0:
    return nResult;
}

int LuaGetRoleByName(Lua_State* L)
{
    int             nResult     = 0;
    int             nTopIndex   = 0;
    const char*     pszRoleName  = NULL;
    KRole*          pRole       = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszRoleName = Lua_ValueToString(L ,1);
    KGLOG_PROCESS_ERROR(pszRoleName);

    assert(g_pSO3GameCenter);
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pszRoleName);
    KGLOG_PROCESS_ERROR(pRole);

    nResult = pRole->LuaGetObj(L);
Exit0:
    return nResult;
}

struct KFuncGetOnlineRoleTable
{
    BOOL operator () (DWORD dwID, KRole& rRole)
    {
        int nLevel = 0;

        if (!rRole.IsOnline())
            return true;

        nLevel = rRole.getLevel();        
        if (nLevel >= m_nLevelLow && nLevel <= m_nLevelHigh)
        {
            m_vRoleTable.push_back(&rRole);
        }
        return true;
    }

    std::vector<KRole*, KMemory::KAllocator<KRole*> > m_vRoleTable;
    int m_nLevelLow;
    int m_nLevelHigh;
};

int LuaSelectOnlineRole(Lua_State* L)
{
    int     nResult         = 0;
    int     nRetCode        = 0;
    int     nTopIndex       = 0;
    int     nRequireCount   = 0;
    int     nRandIndex      = 0;
    int     nOnlineTotal    = 0;
    KRole*  pRole           = NULL;
    KFuncGetOnlineRoleTable Func;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    nRequireCount = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nRequireCount > 0);

    Func.m_nLevelLow  = (int)Lua_ValueToNumber(L, 2);
    Func.m_nLevelHigh = (int)Lua_ValueToNumber(L, 3);

    Func.m_vRoleTable.reserve(20000);
    g_pSO3GameCenter->m_RoleManager.Traverse(Func);

    nOnlineTotal = (int)Func.m_vRoleTable.size();

    Lua_NewTable(L);

    if (nRequireCount >= nOnlineTotal)
    {
        for (int i = 0; i < nOnlineTotal; i++)
        {
            pRole = Func.m_vRoleTable[i];
            assert(pRole);

            Lua_PushNumber(L, i + 1);
            pRole->LuaGetObj(L);
            
            Lua_SetTable(L, -3);
        }
    }
    else
    {
        for (int i = 0; i < nRequireCount; i++)
        {
            nRandIndex = g_Random(nOnlineTotal - i);
            
            pRole = Func.m_vRoleTable[nRandIndex];
            assert(pRole);

            Func.m_vRoleTable[nRandIndex] = Func.m_vRoleTable[nOnlineTotal - i - 1];

            Lua_PushNumber(L, i + 1);
            pRole->LuaGetObj(L);

            Lua_SetTable(L, -3);
        }
    }

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetTong(Lua_State* L)
{
    int     nResult     = 0;
    int     nTopIndex   = 0;
    DWORD   dwTongID    = ERROR_ID;
    KTong*  pTong       = NULL;
    
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwTongID = (DWORD)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(dwTongID != ERROR_ID);

    assert(g_pSO3GameCenter);
    pTong = g_pSO3GameCenter->m_TongManager.GetTong(dwTongID);
    KG_PROCESS_ERROR(pTong);
 
    nResult = pTong->LuaGetObj(L);
Exit0:
    return nResult;
}

int LuaCreateTong(Lua_State* L)
{
    int         nResult     = 0;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    DWORD       dwPlayerID  = 0;
    const char* pszTongName = 0;
    int         nTemplateID = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 1);

    pszTongName = Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszTongName);

    nTemplateID = (int)Lua_ValueToNumber(L, 3);

    bRetCode = g_pSO3GameCenter->m_TongManager.CreateTong(dwPlayerID, pszTongName, nTemplateID);
    
    Lua_PushBoolean(L, bRetCode);
    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetMapList(Lua_State* L)
{
    int nIndex = 1;
    std::vector<DWORD> vecMapList;
    
    g_pSO3GameCenter->m_MapManager.GetMapList(vecMapList);

    Lua_NewTable(L);

    for (std::vector<DWORD>::iterator it = vecMapList.begin(); it != vecMapList.end(); ++it)
    {
        Lua_PushNumber(L, nIndex);
        Lua_PushNumber(L, *it);

        Lua_SetTable(L, -3);
        nIndex++;
    }
    
    return 1;
}

int LuaGetPlayerSavedCopys(Lua_State* L)
{
    int         nResult     = 0;
    BOOL        bRetCode    = false;
    int         nTopIndex   = 0;
    DWORD       dwMapID     = 0;
    DWORD       dwPlayerID  = 0;
    KMapInfo*   pMapInfo    = NULL;
    int         nIndex      = 0;
    std::vector<int> vecSavedCopys;
    
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    dwMapID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 2);
    
    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    bRetCode = pMapInfo->GetPlayerSavedCopys(dwPlayerID, vecSavedCopys);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    Lua_NewTable(L);

    for (std::vector<int>::iterator it = vecSavedCopys.begin(); it != vecSavedCopys.end(); ++it)
    {
        Lua_PushNumber(L, ++nIndex);
        Lua_PushNumber(L, *it);

        Lua_SetTable(L, -3);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetPlayerEnterNewCopyTimesInfo(Lua_State* L)
{
    int         nResult             = 0;
    KMapInfo*   pMapInfo            = NULL;
    int         nTopIndex           = 0;
    DWORD       dwMapID             = 0;
    DWORD       dwPlayerID          = 0;
    int         nAlreadyEnterTimes  = 0;
    KNEW_MAP_LIMIT_TABLE::iterator it;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    dwMapID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 2);

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);
    
    KG_PROCESS_ERROR(pMapInfo->m_nType == emtDungeon || pMapInfo->m_nType == emtBattleField);

    it = pMapInfo->m_PlayerEnterNewCopyTimes.find(dwPlayerID);
    if (it != pMapInfo->m_PlayerEnterNewCopyTimes.end())
    {
        nAlreadyEnterTimes = it->second.nTimes;
    }   
    
    Lua_PushNumber(L, nAlreadyEnterTimes);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetCurrentTime(Lua_State* L)
{
    Lua_PushNumber(L, g_pSO3GameCenter->m_nTimeNow);
    return 1;
}

int LuaGetTimeZone(Lua_State* L)
{
    Lua_PushNumber(L, timezone);
    return 1;
}

int LuaTimeToDate(Lua_State* L)
{
    int         nTopIndex   = 0;
    time_t      nTime       = 0;
    struct tm*  ptmTime     = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nTime = (time_t)Lua_ValueToNumber(L, 1);
    ptmTime = localtime(&nTime);
    KGLOG_PROCESS_ERROR(ptmTime);

    Lua_NewTable(L);

    Lua_PushString(L, "year");
    Lua_PushNumber(L, ptmTime->tm_year + 1900);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "month");
    Lua_PushNumber(L, ptmTime->tm_mon + 1);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "day");
    Lua_PushNumber(L, ptmTime->tm_mday);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "hour");
    Lua_PushNumber(L, ptmTime->tm_hour);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "minute");
    Lua_PushNumber(L, ptmTime->tm_min);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "second");
    Lua_PushNumber(L, ptmTime->tm_sec);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "weekday");
    Lua_PushNumber(L, ptmTime->tm_wday);
    Lua_SetTable(L, -3);

    return 1;
Exit0:
    return 0;
}

int LuaDateToTime(Lua_State* L)
{
    int         nTopIndex = 0;
    int         nResult   = 0;
    time_t      nTime     = 0;
    struct tm   tmTime;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 6);

    tmTime.tm_year = (int)Lua_ValueToNumber(L, 1) - 1900;
    tmTime.tm_mon  = (int)Lua_ValueToNumber(L, 2) - 1;
    tmTime.tm_mday = (int)Lua_ValueToNumber(L, 3);
    tmTime.tm_hour = (int)Lua_ValueToNumber(L, 4);
    tmTime.tm_min  = (int)Lua_ValueToNumber(L, 5);
    tmTime.tm_sec  = (int)Lua_ValueToNumber(L, 6);

    nTime = mktime(&tmTime);
    KGLOG_PROCESS_ERROR(nTime != -1);

    lua_pushinteger(L, nTime);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetCurrentLoop(Lua_State* L)
{
    Lua_PushNumber(L, g_pSO3GameCenter->m_nWorkLoop);
    return 1;    
}

int LuaResetMap(Lua_State* L)
{
    int     nResult     = 0;
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    DWORD   dwPlayerID  = ERROR_ID;
    DWORD   dwMapID     = 0;
    int     nCopySN     = 0;
    int     i           = 1;
    std::vector<DWORD> vecFailedMap;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);
    
    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 1);
    dwMapID     = (DWORD)Lua_ValueToNumber(L, 2);
    nCopySN     = (int)Lua_ValueToNumber(L, 3);

    if (dwMapID == 0)
    {
        g_pSO3GameCenter->m_MapManager.ResetAllMap(dwPlayerID, vecFailedMap);
    }
    else
    {
        bRetCode = g_pSO3GameCenter->m_MapManager.ResetMap(dwPlayerID, dwMapID, nCopySN);
        if (!bRetCode)
        {
            vecFailedMap.push_back(dwMapID);
        }
    }
    
    Lua_NewTable(L);
    
    for (std::vector<DWORD>::iterator it = vecFailedMap.begin(); it != vecFailedMap.end(); ++it, ++i)
    {
        Lua_PushNumber(L, i);
        Lua_PushNumber(L, *it);
        Lua_SetTable(L, -3);
    }
    
    nResult = 1;
Exit0:
    return nResult;
}

int LuaGetSceneConnIndex(Lua_State* L)
{
    int         nResult         = 0;
    int         nTopIndex       = 0;
    DWORD       dwMapID         = 0;
    int         nMapCopyIndex   = 0;
    KMapCopy*   pMapCopy        = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMapID         = (DWORD)Lua_ValueToNumber(L, 1);
    nMapCopyIndex   = (int)Lua_ValueToNumber(L, 2);

    pMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(dwMapID, nMapCopyIndex);
    KG_PROCESS_ERROR(pMapCopy);
    
    nResult = pMapCopy->m_nConnIndex;
Exit0:
    if (nResult)
        Lua_PushNumber(L, nResult);
    else
        Lua_PushNil(L);

    return 1;
}

int LuaSetStatData(Lua_State* L)
{
    int         nTopIndex    = 0;
    int         nValue       = 0;
    size_t      uNameLen     = 0;
    const char* pszName      = NULL;
    const char* pszLuaPrefix = "lua_";
    char        szName[STAT_DATA_NAME_LEN];

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    pszName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszName);

    uNameLen = strlen(pszName) + strlen(pszLuaPrefix) + 1;
    KGLOG_PROCESS_ERROR(uNameLen <= sizeof(szName));

    nValue = (int)Lua_ValueToNumber(L, 2);

    snprintf(szName, sizeof(szName), "%s%s", pszLuaPrefix, pszName);
    szName[sizeof(szName) - 1] = '\0';

    g_pSO3GameCenter->m_StatDataManager.SetDataValue(szName, nValue);

Exit0:
    return 0;
}

int LuaGetActivityManager(Lua_State* L)
{
    int nResult     = 0;
    int nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    nResult = g_pSO3GameCenter->m_ActivityManager.LuaGetObj(L);
Exit0:
    return nResult;
}

int LuaSendTrickReport(Lua_State* L)
{
    int         nTopIndex       = 0;
    const char* pcszAccount     = NULL;
    const char* pcszRoleName    = NULL;
    const char* pcszText        = NULL;

    nTopIndex =  Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    pcszAccount = (const char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pcszAccount);

    pcszRoleName = (const char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pcszRoleName);

    pcszText = (const char*)Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pcszText);

    g_pSO3GameCenter->m_Eyes.DoSendGmChn(l2e_report_trick_def, pcszAccount, pcszRoleName, pcszText);   
Exit0:
    return 0;
}

int LuaSendCheatReport(Lua_State* L)
{
    int         nTopIndex       = 0;
    const char* pcszAccount     = NULL;
    const char* pcszRoleName    = NULL;
    const char* pcszText        = NULL;

    nTopIndex =  Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    pcszAccount = (const char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pcszAccount);

    pcszRoleName = (const char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pcszRoleName);

    pcszText = (const char*)Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pcszText);

    g_pSO3GameCenter->m_Eyes.DoSendGmChn(l2e_report_cheat_def, pcszAccount, pcszRoleName, pcszText);   
Exit0:
    return 0;
}

int LuaSendPrizeInfo(Lua_State* L)
{
    int         nRetCode        = 0;
    int         nTopIndex       = 0;
    DWORD       dwPlayerID      = ERROR_ID;
    KRole*      pRole           = NULL;
    const char* pszType         = NULL;
    int         nValue          = 0;

    nTopIndex =  Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwPlayerID);

    pszType = (const char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszType);

    nValue = (int)Lua_ValueToNumber(L, 3);

    assert(g_pSO3GameCenter);
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwPlayerID);
    KGLOG_PROCESS_ERROR(pRole);

    g_pSO3GameCenter->m_Eyes.DoSendPrizeInfo(pRole->m_pszAccount, pRole->m_szName, dwPlayerID, pszType, nValue);   
Exit0:
    return 0;
}

int LuaSendKingeyesReport(Lua_State* L)
{
    int         nTopIndex       = 0;
    int         nType           = 0;
    const char* pszKey          = NULL;
    const char* pszValue        = NULL;

    nTopIndex =  Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    nType = (int)Lua_ValueToNumber(L, 1);

    pszKey = (const char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszKey);

    pszValue = (const char*)Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pszValue);

    g_pSO3GameCenter->m_Eyes.DoSendReport(nType, pszKey, pszValue);

Exit0:
    return 0;
}

int LuaResetPlayerEnterNewCopyTime(Lua_State* L)
{
    g_pSO3GameCenter->m_MapManager.ResetPlayerEnterNewCopyTime();
    return 0;
}

int LuaAddEnterNewCopyTimes(Lua_State* L)
{
    int         nTopIndex   = 0;
    DWORD       dwRoleID    = ERROR_ID;
    KRole*      pRole       = NULL;
    KMapInfo*   pMapInfo    = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwRoleID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwRoleID != ERROR_ID);
    
    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(dwRoleID);
    KGLOG_PROCESS_ERROR(pRole);
    
    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(pRole->m_dwMapID);
    KGLOG_PROCESS_ERROR(pMapInfo);

    pMapInfo->AddPlayerEnterNewCopyTimes(dwRoleID, pRole->m_nMapCopyIndex);

Exit0:
    return 0;
}

int LuaGetRandomGS(Lua_State* L)
{
    int nResult    = 0;
    int nConnIndex = 0;

    nConnIndex = g_pSO3GameCenter->m_GameServer.GetRandomGS();
    KGLOG_PROCESS_ERROR(nConnIndex > 0);
    
    Lua_PushNumber(L, nConnIndex);

    nResult = 1;
Exit0:
    return nResult;
}

int LuaResetCamp(Lua_State* L)
{
    g_pSO3GameCenter->m_CampManager.Reset();
    KGLogPrintf(KGLOG_DEBUG, "Camp Reset!\n");
    return 0;
}

int LuaGetMentorCenter(Lua_State* L)
{
    return g_pSO3GameCenter->m_MentorCenter.LuaGetObj(L);
}

int LuaRenameConflictTong(Lua_State* L)
{
    BOOL        bRetCode        = false;
    int         nTopIndex       = 0;
    int         nConnIndex      = 0;
    DWORD       dwPlayerID      = ERROR_ID;
    DWORD       dwTongID        = ERROR_ID;
    const char* pszNewName      = NULL;
    size_t      uNewNameLen     = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 4);

    nConnIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nConnIndex);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(dwPlayerID != ERROR_ID);

    dwTongID = (DWORD)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(dwTongID != ERROR_ID);

    pszNewName = (const char*)Lua_ValueToString(L, 4);
    KGLOG_PROCESS_ERROR(pszNewName);

    uNewNameLen = strlen(pszNewName);
    KGLOG_PROCESS_ERROR(uNewNameLen < _NAME_LEN);

    g_pSO3GameCenter->m_TongManager.ProcessRenameConflictTong(nConnIndex, dwPlayerID, dwTongID, pszNewName);

Exit0:
    return 0;
}

int LuaRescueDeleteRole(Lua_State* L)
{
    BOOL        bRetCode        = false;
    int         nTopIndex       = 0;
    const char* pszRoleName     = 0;
    KRole*      pRole           = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    pszRoleName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszRoleName);

    pRole = g_pSO3GameCenter->m_RoleManager.GetRole(pszRoleName);
    KGLOG_PROCESS_ERROR(pRole);

    g_pSO3GameCenter->m_RoleManager.UnregisterDelRole(pRole->m_dwPlayerID);

Exit0:
    return 0;
}

int LuaGetMapInfo(Lua_State* L)
{
    int         nTopIndex   = 0;
    DWORD       dwMapID     = ERROR_ID;
    KMapInfo*   pMapInfo    = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMapID = (DWORD)Lua_ValueToNumber(L, 1);

    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(dwMapID);
    KG_PROCESS_ERROR(pMapInfo);

Exit0:
    if (pMapInfo)
    {
        pMapInfo->LuaGetObj(L);
    }
    else
    {
        Lua_PushNil(L);
    }
    return 1;
}

static KLuaFunc g_ScriptFuncList[] = 
{
    {"Test",                            LuaTest}, 
    {"SendGmMessage",                   LuaSendGmMessage},
    {"GetAccountRoleList",              LuaGetAccountRoleList},
    {"Kickout",                         LuaKickout},
    {"SendGlobalSysMsg",                LuaSendGlobalSysMsg},
    {"PlayerExecute",                   LuaPlayerExecute},
    {"FreezeRole",                      LuaFreezeRole},
    {"SendSystemMail",                  LuaSendSystemMail},
    {"SendGmMessageMail",               LuaSendGmMessageMail},
    {"SendGlobalMail",                  LuaSendGlobalMail},
    {"DeleteGlobalMail",                LuaDeleteGlobalMail},
    {"DeleteAllGlobalMail",             LuaDeleteAllGlobalMail},
    {"GetPQ",                           LuaGetPQ},
    {"CreatePQ",                        LuaCreatePQ},
    {"Random",                          LuaRandom},
    {"RefreshMapCopy",                  LuaRefreshMapCopy},
    {"RemoteCall",                      LuaRemoteCall},
    {"Log",                             LuaLog},
    {"ReloadScripts",                   LuaReloadScripts},
    {"GetPQByTemplate",                 LuaGetPQByTemplate},
    {"GetGlobalCustomDataManager",      LuaGetGlobalCustomDataManager},
    {"GetGlobalSystemValueManager",     LuaGetGlobalSystemValueManager},
    {"GetRole",                         LuaGetRole},
    {"GetRoleByName",                   LuaGetRoleByName},
    {"SelectOnlineRole",                LuaSelectOnlineRole},
    {"GetTong",                         LuaGetTong},
    {"CreateTong",                      LuaCreateTong},
    {"GetMapList",                      LuaGetMapList},
    {"GetPlayerSavedCopys",             LuaGetPlayerSavedCopys},
    {"GetPlayerEnterNewCopyTimesInfo",  LuaGetPlayerEnterNewCopyTimesInfo},
    {"GetCurrentTime",                  LuaGetCurrentTime},
    {"GetTimeZone",                     LuaGetTimeZone},
    {"GetCurrentLoop",                  LuaGetCurrentLoop},
    {"ResetMap",                        LuaResetMap},
    {"GetSceneConnIndex",               LuaGetSceneConnIndex},
    {"TimeToDate",                      LuaTimeToDate},
    {"DateToTime",                      LuaDateToTime},
    {"SetStatData",                     LuaSetStatData},
    {"GetActivityManager",              LuaGetActivityManager},
    {"SendTrickReport",                 LuaSendTrickReport},
    {"SendCheatReport",                 LuaSendCheatReport},
    {"SendPrizeInfo",                   LuaSendPrizeInfo},
    {"SendKingeyesReport",              LuaSendKingeyesReport},
    {"ResetPlayerEnterNewCopyTime",     LuaResetPlayerEnterNewCopyTime},
    {"AddEnterNewCopyTimes",            LuaAddEnterNewCopyTimes},
    {"GetRandomGS",                     LuaGetRandomGS},
    {"ResetCamp",                       LuaResetCamp},
    {"GetMentorCenter",                 LuaGetMentorCenter},
    {"RenameConflictTong",              LuaRenameConflictTong},
    {"RenameConflictTong",              LuaRenameConflictTong},
    {"RescueDeleteRole",                LuaRescueDeleteRole},
    {"GetMapInfo",                      LuaGetMapInfo}
};

BOOL RegisterScriptFunctions(ILuaScriptEx* piScript)
{
	int nCount = (int)(sizeof(g_ScriptFuncList) / sizeof(g_ScriptFuncList[0]));

	piScript->RegisterFunctions(g_ScriptFuncList, nCount);

    return true;
}

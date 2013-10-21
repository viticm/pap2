#include "stdafx.h"
#include "KMath.h"
#include "KGPublic.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KScriptServer.h"
#include "KRelayClient.h"
#include "KPlayerServer.h"
#include "KPlayerClient.h"
#include "Engine/KLuaPackage.h"

#ifdef _SERVER

KScriptServer::KScriptServer()
{
    m_dwScript   = 0;
}

BOOL KScriptServer::Setup()
{
    int  nIndex     = 0;

    m_RegisterTable.clear();

    m_dwScript  = g_FileNameHash(SCRIPT_DIR"/script_server.lua");

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nIndex);

    g_pSO3World->m_ScriptCenter.PushValueToStack(this);

    g_pSO3World->m_ScriptCenter.CallFunction(m_dwScript, "Setup", 0);

    g_pSO3World->m_ScriptCenter.SafeCallEnd(nIndex);

    return true;
}

BOOL KScriptServer::IsFunctionRegistered(const char cszFunction[])
{
    KREGISTER_TABLE::iterator it = m_RegisterTable.find(cszFunction);
    if (it != m_RegisterTable.end())
    {
        return true;
    }
    return false;
}

BOOL KScriptServer::Accept(KPlayer* pPlayer, const char cszFunction[], BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nIndex      = 0;
    ILuaScriptEx*   piScript    = NULL;
    Lua_State*      L           = NULL;
    BYTE*           pbyNext     = pbyData;
    BYTE*           pbyTail     = pbyData + uDataLen;

    assert(pPlayer);
    assert(pPlayer->m_pScene);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nIndex);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScript);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScript, cszFunction);
    KG_PROCESS_ERROR(bRetCode);

    piScript = g_pSO3World->m_ScriptCenter.GetScriptHolder();
    KGLOG_PROCESS_ERROR(piScript);

    L = piScript->GetLuaState();
    KGLOG_PROCESS_ERROR(L);

	pPlayer->LuaGetObj(L);
    piScript->AddParamCount();

    while (pbyNext < pbyTail)
    {
        pbyNext = LuaUnpack(L, pbyNext, (size_t)(pbyTail - pbyNext));
        KGLOG_PROCESS_ERROR(pbyNext);

        piScript->AddParamCount();
    }

    g_pSO3World->m_ScriptCenter.CallFunction(m_dwScript, cszFunction, 0);

    bResult = true;
Exit0:
    g_pSO3World->m_ScriptCenter.SafeCallEnd(nIndex);
    return bResult;
}

int KScriptServer::Call(Lua_State* L)
{
    int         nParamCount     = 0;
    DWORD       dwPlayerID      = ERROR_ID;
    KPlayer*    pPlayer         = NULL;
    const char* pszFunction     = NULL;
    BYTE*       pbyOffset       = m_byParamData;
    BYTE*       pbyTail         = m_byParamData + sizeof(m_byParamData);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount >= 2);

    dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);

	pszFunction = Lua_ValueToString(L, 2);
	KGLOG_PROCESS_ERROR(pszFunction);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    for (int i = 3; i <= nParamCount; i++)
    {
        pbyOffset = LuaPackup(L, i, pbyOffset, (size_t)(pbyTail - pbyOffset));
        KGLOG_PROCESS_ERROR(pbyOffset);
    }

    g_PlayerServer.DoRemoteLuaCall(
        pPlayer->m_nConnIndex, pszFunction, m_byParamData, (size_t)(pbyOffset - m_byParamData)
    );

Exit0:
    return 0;
}

int KScriptServer::LuaRegister(Lua_State* L)
{
    int         nParamCount     = 0;
    const char* pszFunction     = NULL;
    size_t      uFuncNameLen    = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1);

    pszFunction = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszFunction);
    
    uFuncNameLen = strlen(pszFunction);
    KGLOG_PROCESS_ERROR(uFuncNameLen < _NAME_LEN);

    m_RegisterTable.insert(pszFunction);

Exit0:
    return 0;
}

DEFINE_LUA_CLASS_BEGIN(KScriptServer)
    REGISTER_LUA_FUNC(KScriptServer, Register)
DEFINE_LUA_CLASS_END(KScriptServer)

#endif

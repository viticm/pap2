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
#include "lzo/lzo1x.h"

#ifdef _SERVER

KScriptServer::KScriptServer()
{
    m_dwScript   = 0;
    m_pbyPos     = NULL;
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
    int             nRetCode    = 0;
    int             nIndex      = 0;
    ILuaScriptEx*   piScript    = NULL;
    Lua_State*      L           = NULL;
    BYTE*           pbyNext     = NULL;
    BYTE*           pbyTail     = NULL;
    size_t          uSrcLen     = sizeof(m_byOriginalBuffer);

    assert(pPlayer);
    assert(pPlayer->m_pScene);

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nIndex);

    nRetCode = lzo1x_decompress(pbyData, (lzo_uint)uDataLen, m_byOriginalBuffer, (lzo_uint*)&uSrcLen, NULL);
    KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

    pbyNext = m_byOriginalBuffer;
    pbyTail = m_byOriginalBuffer + uSrcLen;

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
    int         nRetCode        = 0;
    int         nParamCount     = 0;
    DWORD       dwPlayerID      = ERROR_ID;
    KPlayer*    pPlayer         = NULL;
    const char* pszFunction     = NULL;
    BYTE*       pbyOffset       = m_byOriginalBuffer;
    BYTE*       pbyTail         = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);
    size_t      uSrcLen         = 0;
    size_t      uDstLen         = 0;

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

    uSrcLen = (size_t)(pbyOffset - m_byOriginalBuffer);

    nRetCode = lzo1x_1_compress(m_byOriginalBuffer, (lzo_uint)uSrcLen, m_byCompressBuffer, (lzo_uint*)&uDstLen, m_LzoWorkBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

    g_PlayerServer.DoRemoteLuaCall(
        pPlayer->m_nConnIndex, pszFunction, m_byCompressBuffer, uDstLen
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

void KScriptServer::Reset()
{
    m_pbyPos = m_byOriginalBuffer;
}

BYTE* KScriptServer::PushNumber(double fValue)
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackupNumber(m_pbyPos, (size_t)(pbyTail - m_pbyPos), fValue);
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}


BYTE* KScriptServer::PushString(const char cszValue[])
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackupString(m_pbyPos, (size_t)(pbyTail - m_pbyPos), cszValue);
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KScriptServer::PushNill()
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackupNill(m_pbyPos, (size_t)(pbyTail - m_pbyPos));
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KScriptServer::PushBool(BOOL bValue)
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackupBoolean(m_pbyPos, (size_t)(pbyTail - m_pbyPos), bValue);
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KScriptServer::NewTable()
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackage_NewTable(m_pbyPos, (size_t)(pbyTail - m_pbyPos));
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KScriptServer::SetTable(BYTE* pbyTableBegin, BYTE* pbyTableEnd)
{
    return LuaPackage_SetTable(pbyTableBegin, pbyTableEnd);
}

BOOL KScriptServer::Call(int nConnIndex, const char cszFunction[])
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    size_t      uOrgLen     = (size_t)(m_pbyPos - m_byOriginalBuffer);
    size_t      uCpsLen     = 0;

    nRetCode = lzo1x_1_compress(m_byOriginalBuffer, (lzo_uint)uOrgLen, m_byCompressBuffer, (lzo_uint*)&uCpsLen, m_LzoWorkBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

    g_PlayerServer.DoRemoteLuaCall(nConnIndex, cszFunction, m_byCompressBuffer, uCpsLen);

    bResult = true;
Exit0:
    return bResult;
}

DEFINE_LUA_CLASS_BEGIN(KScriptServer)
    REGISTER_LUA_FUNC(KScriptServer, Register)
DEFINE_LUA_CLASS_END(KScriptServer)

#endif

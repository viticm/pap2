#include "stdafx.h"
#include "KScriptFuncList.h"
#include "KScriptManager.h"
#include "KSO3GameCenter.h"
#include "KRemoteScript.h"
#include "Engine/KLuaPackage.h"
#include "lzo/lzo1x.h"

KRemoteScript::KRemoteScript()
{
    m_dwScriptID = 0;
    m_pbyPos     = NULL;
}

void KRemoteScript::Setup()
{
    m_dwScriptID = g_FileNameHash(CENTER_SCRIPTS_DIR"/remote.lua");
}

BOOL KRemoteScript::Accept(int nConnIndex, 
	const char cszFunction[], 
	BYTE* pbyData, 
	size_t uDataLen, 
	BOOL bCompressed)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nRetCode    = 0;
    ILuaScriptEx*   piScript    = NULL;
    int             nIndex      = 0;
    Lua_State*      L           = NULL;
    BYTE*           pbyNext     = pbyData;
    BYTE*           pbyTail     = pbyData + uDataLen;
    size_t          uOrgLen     = sizeof(m_byOriginalBuffer);

    g_pSO3GameCenter->m_ScriptManager.SafeCallBegin(&nIndex);

    if (bCompressed)
    {
        nRetCode = lzo1x_decompress(pbyData, (lzo_uint)uDataLen, m_byOriginalBuffer, (lzo_uint*)&uOrgLen, NULL);
        KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

        pbyNext = m_byOriginalBuffer;
        pbyTail = m_byOriginalBuffer + uOrgLen;
    }

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsScriptExist(m_dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsFuncExist(m_dwScriptID, cszFunction);
    KG_PROCESS_ERROR(bRetCode);

    piScript = g_pSO3GameCenter->m_ScriptManager.GetScriptHolder();
    KGLOG_PROCESS_ERROR(piScript);

    L = piScript->GetLuaState();
    KGLOG_PROCESS_ERROR(L);

	Lua_PushNumber(L, nConnIndex);
    piScript->AddParamCount();

    while (pbyNext < pbyTail)
    {
        pbyNext = LuaUnpack(L, pbyNext, (size_t)(pbyTail - pbyNext));
        KGLOG_PROCESS_ERROR(pbyNext);

        piScript->AddParamCount();
    }

    g_pSO3GameCenter->m_ScriptManager.Call(m_dwScriptID, cszFunction, 0);

    bResult = true;
Exit0:
    g_pSO3GameCenter->m_ScriptManager.SafeCallEnd(nIndex);
    return bResult;
}

int KRemoteScript::Call(Lua_State* L)
{
    int         nRetCode        = 0;
    int         nParamCount     = 0;
    int         nConnIndex      = 0;
    const char* pszFunction     = NULL;
    BYTE*       pbyTail         = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);
    BYTE*       pbyOffset       = m_byOriginalBuffer;
    size_t      uOrgLen         = 0;
    size_t      uCpsLen         = 0;

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount >= 2);

	nConnIndex = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nConnIndex != 0);

	pszFunction = Lua_ValueToString(L, 2);
	KGLOG_PROCESS_ERROR(pszFunction);

    for (int i = 3; i <= nParamCount; i++)
    {
        pbyOffset = LuaPackup(L, i, pbyOffset, (size_t)(pbyTail - pbyOffset));
        KGLOG_PROCESS_ERROR(pbyOffset);
    }

    uOrgLen = (size_t)(pbyOffset - m_byOriginalBuffer);

    nRetCode = lzo1x_1_compress(m_byOriginalBuffer, (lzo_uint)uOrgLen, m_byCompressBuffer, (lzo_uint*)&uCpsLen, m_LzoWorkBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

    nRetCode = g_pSO3GameCenter->m_GameServer.DoRemoteLuaCall(nConnIndex, pszFunction, m_byCompressBuffer, uCpsLen);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

void KRemoteScript::Reset()
{
    m_pbyPos = m_byOriginalBuffer;
}

BYTE* KRemoteScript::PushNumber(double fValue)
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackupNumber(m_pbyPos, (size_t)(pbyTail - m_pbyPos), fValue);
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KRemoteScript::PushString(const char cszValue[])
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackupString(m_pbyPos, (size_t)(pbyTail - m_pbyPos), cszValue);
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KRemoteScript::PushNill()
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackupNill(m_pbyPos, (size_t)(pbyTail - m_pbyPos));
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KRemoteScript::PushBool(BOOL bValue)
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackupBoolean(m_pbyPos, (size_t)(pbyTail - m_pbyPos), bValue);
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KRemoteScript::NewTable()
{
    BYTE*   pbyRet  = NULL;
    BYTE*   pbyTail = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);

    pbyRet = LuaPackage_NewTable(m_pbyPos, (size_t)(pbyTail - m_pbyPos));
    KGLOG_PROCESS_ERROR(pbyRet);

    m_pbyPos = pbyRet;
Exit0:
    return pbyRet;
}

BYTE* KRemoteScript::SetTable(BYTE* pbyTableBegin, BYTE* pbyTableEnd)
{
    return LuaPackage_SetTable(pbyTableBegin, pbyTableEnd);
}

BOOL KRemoteScript::Call(int nConnIndex, const char cszFunction[])
{
    BOOL        bResult     = false;
    int         nRetCode    = 0;
    size_t      uOrgLen     = (size_t)(m_pbyPos - m_byOriginalBuffer);
    size_t      uCpsLen     = 0;

    nRetCode = lzo1x_1_compress(m_byOriginalBuffer, (lzo_uint)uOrgLen, m_byCompressBuffer, (lzo_uint*)&uCpsLen, m_LzoWorkBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

    nRetCode = g_pSO3GameCenter->m_GameServer.DoRemoteLuaCall(nConnIndex, cszFunction, m_byCompressBuffer, uCpsLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

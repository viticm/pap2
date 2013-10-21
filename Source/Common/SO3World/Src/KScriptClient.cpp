#include "stdafx.h"
#include "KMath.h"
#include "KGPublic.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KScriptClient.h"
#include "KRelayClient.h"
#include "KPlayerServer.h"
#include "KPlayerClient.h"
#include "Engine/KLuaPackage.h"
#include "lzo/lzo1x.h"
#include "SO3UI/SO3GameWorldUIHandler.h"

#ifdef _CLIENT

KScriptClient::KScriptClient()
{
    m_dwScript   = 0;
}

BOOL KScriptClient::Setup()
{
    m_dwScript  = g_FileNameHash(SCRIPT_DIR"/script_client.lua");
    return true;
}

BOOL KScriptClient::Accept(const char cszFunction[], BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult     = false;
    int             nRetCode    = 0;
    size_t          uSrcLen     = sizeof(m_byOriginalBuffer);

    nRetCode = lzo1x_decompress(pbyData, (lzo_uint)uDataLen, m_byOriginalBuffer, (lzo_uint*)&uSrcLen, NULL);
    KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

    AcceptToScriptClient(cszFunction, m_byOriginalBuffer, uSrcLen);

    if (g_pGameWorldUIHandler)
    {
        g_pGameWorldUIHandler->OnRemoteLuaCall(
            cszFunction, m_byOriginalBuffer, uSrcLen
        );
    }

    bResult = true;
Exit0:
    return bResult;
}

int KScriptClient::Call(Lua_State* L)
{
    int         nRetCode        = 0;
    int         nParamCount     = 0;
    const char* pszFunction     = NULL;
    BYTE*       pbyOffset       = m_byOriginalBuffer;
    BYTE*       pbyTail         = m_byOriginalBuffer + sizeof(m_byOriginalBuffer);
    size_t      uSrcLen         = 0;
    size_t      uDstLen         = 0;

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount >= 1);

	pszFunction = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pszFunction);

    for (int i = 2; i <= nParamCount; i++)
    {
        pbyOffset = LuaPackup(L, i, pbyOffset, (size_t)(pbyTail - pbyOffset));
        KGLOG_PROCESS_ERROR(pbyOffset);
    }

    uSrcLen = (size_t)(pbyOffset - m_byOriginalBuffer);

    nRetCode = lzo1x_1_compress(m_byOriginalBuffer, (lzo_uint)uSrcLen, m_byCompressBuffer, (lzo_uint*)&uDstLen, m_LzoWorkBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == LZO_E_OK);

    g_PlayerClient.DoRemoteLuaCall(pszFunction, m_byCompressBuffer, uDstLen);

Exit0:
    return 0;
}

BOOL KScriptClient::AcceptToScriptClient(const char cszFunction[], BYTE* pbyData, size_t uDataLen)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    int             nIndex      = 0;
    ILuaScriptEx*   piScript    = NULL;
    Lua_State*      L           = NULL;
    BYTE*           pbyNext     = pbyData;
    BYTE*           pbyTail     = pbyData + uDataLen;     

    g_pSO3World->m_ScriptCenter.SafeCallBegin(&nIndex);

    bRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(m_dwScript);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScript, cszFunction);
    KG_PROCESS_ERROR(bRetCode);

    piScript = g_pSO3World->m_ScriptCenter.GetScriptHolder();
    KGLOG_PROCESS_ERROR(piScript);

    L = piScript->GetLuaState();
    KGLOG_PROCESS_ERROR(L);

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
#endif

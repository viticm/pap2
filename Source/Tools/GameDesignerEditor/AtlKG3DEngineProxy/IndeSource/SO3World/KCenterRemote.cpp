#include "stdafx.h"
#include "KMath.h"
#include "KGPublic.h"
#include "KSO3World.h"
#include "KPlayer.h"
#include "KCenterRemote.h"
#include "KRelayClient.h"
#include "KPlayerServer.h"
#include "KPlayerClient.h"
#include "Engine/KLuaPackage.h"

#ifdef _SERVER

KCenterRemote::KCenterRemote()
{
    m_dwScript  = 0;
}

void KCenterRemote::Setup()
{
    m_dwScript   = g_FileNameHash(SCRIPT_DIR"/center_remote.lua");
}

BOOL KCenterRemote::Accept(const char cszFunction[], BYTE* pbyData, size_t uDataLen)
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
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3World->m_ScriptCenter.IsFuncExist(m_dwScript, cszFunction);
    KGLOG_PROCESS_ERROR(bRetCode);

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

int KCenterRemote::Call(Lua_State* L)
{
    int         nParamCount     = 0;
    const char* pszFunction     = NULL;
    BYTE*       pbyOffset       = m_byParamData;
    BYTE*       pbyTail         = m_byParamData + sizeof(m_byParamData);

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount >= 1);

	pszFunction = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pszFunction);

    for (int i = 2; i <= nParamCount; i++)
    {
        pbyOffset = LuaPackup(L, i, pbyOffset, (size_t)(pbyTail - pbyOffset));
        KGLOG_PROCESS_ERROR(pbyOffset);
    }

    g_RelayClient.DoRemoteLuaCall(
        pszFunction, m_byParamData, (size_t)(pbyOffset - m_byParamData)
    );

Exit0:
    return 0;
}

#endif

#include "stdafx.h"
#include "KRegion.h"
#include "KScene.h"
#include "KSO3World.h"
#include "KPlayerServer.h"
#include "KRelayClient.h"

#if defined(_SERVER)
int KScene::LuaCreateNpc(Lua_State* L)
{
    int             nResult             = 0;
	BOOL            bRetCode            = false;
	int             nTopIndex           = 0;
    DWORD	        dwNpcTemplateId     = 0;
	int		        nX                  = 0;
	int		        nY                  = 0;
	int		        nZ                  = 0;
	int		        nDir                = 0;
    int             nDisappearFrame     = 0;
	const char*     pszNickName         = NULL;
    DWORD           dwOwner             = ERROR_ID;
	KNpc*           pNpc                = NULL;
	BOOL            bNpcRegisterFlag    = false;
    BOOL            bNpcInitFlag        = false;
    BOOL            bAddNickNameFlag    = false;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex >= 6);
    KGLOG_PROCESS_ERROR(nTopIndex <= 8);

	dwNpcTemplateId = (DWORD)Lua_ValueToNumber(L, 1);
	nX              = (int)Lua_ValueToNumber(L, 2);
	nY              = (int)Lua_ValueToNumber(L, 3);
	nZ              = (int)Lua_ValueToNumber(L, 4);
    nDir            = (int)Lua_ValueToNumber(L, 5);
    nDisappearFrame = (int)Lua_ValueToNumber(L, 6);

	if (nTopIndex == 7)
	{
		pszNickName = Lua_ValueToString(L, 7);
	}
    else if (nTopIndex == 8)
    {
        pszNickName = Lua_ValueToString(L, 7);
        dwOwner     = (DWORD)Lua_ValueToNumber(L, 8);
    }

    KGLOG_PROCESS_ERROR(nX >= 0);
    KGLOG_PROCESS_ERROR(nY >= 0);

	pNpc = KMemory::New<KNpc>();
	KGLOG_PROCESS_ERROR(pNpc);

	bRetCode = g_pSO3World->m_NpcSet.Register(pNpc, ERROR_ID);
	KGLOG_PROCESS_ERROR(bRetCode);
	bNpcRegisterFlag = true;

	bRetCode = pNpc->Init();
	KGLOG_PROCESS_ERROR(bRetCode);
    bNpcInitFlag = true;

	bRetCode = g_pSO3World->m_Settings.m_NpcTemplateList.LoadFromTemplate(dwNpcTemplateId, pNpc);
	KGLOG_PROCESS_ERROR(bRetCode);

    assert(pNpc->m_pTemplate);

    nDir = nDir % DIRECTION_COUNT;
    if (nDir < 0)
    {
        nDir += DIRECTION_COUNT;
    }

	pNpc->m_nFaceDirection = nDir;

	if (pszNickName)
	{
	    std::map<string, DWORD>::iterator it = m_NpcNickNameList.find(pszNickName);
	    if (it != m_NpcNickNameList.end())
	    {
            KGLogPrintf(KGLOG_WARNING, "Reduplicated npc nickname: %s", pszNickName);
	    }
        m_NpcNickNameList[pszNickName] = pNpc->m_dwID;
        bAddNickNameFlag = true;

		strncpy(pNpc->m_szName, pszNickName, sizeof(pNpc->m_szName));
        pNpc->m_szName[sizeof(pNpc->m_szName) - 1] = '\0';
	}

    pNpc->m_dwOwner = dwOwner;

	bRetCode = g_pSO3World->AddNpc(pNpc, this, nX, nY, nZ);
	KGLOG_PROCESS_ERROR(bRetCode);

    pNpc->SetRevivePoint(pNpc->m_nX, pNpc->m_nY, pNpc->m_nZ, pNpc->m_nFaceDirection);

    bRetCode = pNpc->m_AIVM.Setup(pNpc, pNpc->m_AIData.nAIType);
    KGLOG_PROCESS_ERROR(bRetCode);

    pNpc->m_nCurrentLife = pNpc->m_nMaxLife;
	pNpc->m_nCurrentMana = pNpc->m_nMaxMana;
    pNpc->m_nReviveTime  = 0;

    if (nDisappearFrame != -1)
    {
        bRetCode = pNpc->SetDisappearFrames(nDisappearFrame, 0);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

	nResult = pNpc->LuaGetObj(L);
Exit0:
    if (nResult != 1)
    {
        if (pNpc && pNpc->m_pScene)
        {
            g_pSO3World->RemoveNpc(pNpc);
        }
        
        if (bAddNickNameFlag)
        {
            m_NpcNickNameList.erase(pszNickName);
            bAddNickNameFlag = false;
        }

        if (bNpcInitFlag)
        {
            pNpc->UnInit();
            bNpcInitFlag = false;
        }

		if (bNpcRegisterFlag)
		{
			g_pSO3World->m_NpcSet.Unregister(pNpc);
			bNpcRegisterFlag = false;
		}

        if (pNpc)
        {
            KMemory::Delete(pNpc);
            pNpc = NULL;
        }
    }
	return nResult;
}

int KScene::LuaDestroyNpc(Lua_State* L)
{
	BOOL    bRetCode    = false;
	KNpc*   pNpc        = NULL;
	int     nTopIndex   = Lua_GetTopIndex(L);
	DWORD   dwNpcID     = 0;

	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);

	pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
	KGLOG_PROCESS_ERROR(pNpc);

    if (!pNpc->m_pScene)
    {
        // 如果Npc在场景中的话，则走统一的删除流程，不在场景中，作特殊的处理
        DeleteNpcNickname(pNpc); 

        bRetCode = m_NpcReviveManager.DestroyNpc(pNpc);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

	g_pSO3World->DeleteNpc(pNpc);

Exit0:
	return 0;
}

int KScene::LuaCreateDoodad(Lua_State *L)
{
    int             nResult             = 0;
	int             nRetCode            = false;
	KDoodad*        pDoodad             = NULL;
	BOOL            bDoodadRegisterFlag = false;
	BOOL            bDoodadInitFlag     = false;
    BOOL            bAddNickNameFlag    = false;
	KPlayer*	    pPlayer				= NULL;
	DWORD           dwDoodadTemplateID  = 0;
    const char*     pszNickName         = NULL;
	int		        nX                  = 0;
	int		        nY                  = 0;
	int		        nZ                  = 0;
	int		        nDir                = 0;
	DWORD		    dwOwnerID			= 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode >= 5);
    KGLOG_PROCESS_ERROR(nRetCode <= 7);

    dwDoodadTemplateID  = (DWORD)Lua_ValueToNumber(L, 1);
    nX                  = (int)Lua_ValueToNumber(L, 2);
    nY                  = (int)Lua_ValueToNumber(L, 3);
    nZ                  = (int)Lua_ValueToNumber(L,  4);
    nDir                = (int)Lua_ValueToNumber(L, 5);

	if (nRetCode == 6)
	{	
		pszNickName = Lua_ValueToString(L, 6);
	}

    if (nRetCode == 7)
	{	
        pszNickName = Lua_ValueToString(L, 6);

		dwOwnerID = (DWORD)Lua_ValueToNumber(L, 7);
		pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwOwnerID);
		KGLOG_PROCESS_ERROR(pPlayer);
	}

    KGLOG_PROCESS_ERROR(nX >= 0);
    KGLOG_PROCESS_ERROR(nY >= 0);

    pDoodad = KMemory::New<KDoodad>();
    KGLOG_PROCESS_ERROR(pDoodad);

	nRetCode = g_pSO3World->m_DoodadSet.Register(pDoodad, ERROR_ID);
	KGLOG_PROCESS_ERROR(nRetCode);
	bDoodadRegisterFlag = true;

	nRetCode = pDoodad->Init();
	KGLOG_PROCESS_ERROR(nRetCode);
    bDoodadInitFlag = true;

	nRetCode = g_pSO3World->m_Settings.m_DoodadTemplateList.LoadFromTemplate(dwDoodadTemplateID, pDoodad);
	KGLOG_PROCESS_ERROR(nRetCode);

    nDir = nDir % DIRECTION_COUNT;
    if (nDir < 0)
    {
        nDir += DIRECTION_COUNT;
    }

	pDoodad->m_nFaceDirection = nDir;
	pDoodad->m_nLastLootCount = 1;

    if (pszNickName)
	{
	    std::map<string, DWORD>::iterator it = m_DoodadNickNameList.find(pszNickName);
	    if (it != m_DoodadNickNameList.end())
	    {
            KGLogPrintf(KGLOG_WARNING, "Reduplicated doodad nickname: %s", pszNickName);
	    }
        m_DoodadNickNameList[pszNickName] = pDoodad->m_dwID;
        bAddNickNameFlag = true;

		strncpy(pDoodad->m_szName, pszNickName, sizeof(pDoodad->m_szName));
        pDoodad->m_szName[sizeof(pDoodad->m_szName) - 1] = '\0';
	}

	pDoodad->m_dwOwnerID = dwOwnerID;

	nRetCode = g_pSO3World->AddDoodad(pDoodad, this, nX, nY, nZ);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = pDoodad->LuaGetObj(L);
Exit0:
    if (nResult != 1)
    {
        if (bAddNickNameFlag)
        {
            m_DoodadNickNameList.erase(pszNickName);
            bAddNickNameFlag = false;
        }

        if (bDoodadInitFlag)
        {
	        pDoodad->UnInit();
            bDoodadInitFlag = false;
        }

		if (bDoodadRegisterFlag)
		{
			g_pSO3World->m_DoodadSet.Unregister(pDoodad);
			bDoodadRegisterFlag = false;
		}

	    if (pDoodad)
	    {
            KMemory::Delete(pDoodad);
            pDoodad = NULL;
	    }
    }

	return nResult;
}

int KScene::LuaDestroyDoodad(Lua_State *L)
{
	BOOL        bRetCode    = false;
	KDoodad*    pDoodad     = NULL;
	int         nTopIndex   = Lua_GetTopIndex(L);
	DWORD       dwDoodadID  = 0;

	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwDoodadID = (DWORD)Lua_ValueToNumber(L, 1);

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
	KGLOG_PROCESS_ERROR(pDoodad);

    if (!pDoodad->m_pScene)
    {
        DeleteDoodadNickname(pDoodad);

        bRetCode = m_DoodadReviveManager.DestroyDoodad(pDoodad);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

	g_pSO3World->DelDoodad(pDoodad);

Exit0:
	return 0;
}

int KScene::LuaGetNpcByNickName(Lua_State* L)
{
    int                         nResult     = 0;
	int                         nRetCode    = false;
	KNpc*                       pNpc        = NULL;
	const char*                 pszName     = NULL;
    KNICK_NAME_LIST::iterator   it;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	pszName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszName);

	it = m_NpcNickNameList.find(pszName);
	KG_PROCESS_ERROR(it != m_NpcNickNameList.end());

	pNpc = g_pSO3World->m_NpcSet.GetObj(it->second);
    KGLOG_PROCESS_ERROR(pNpc);

    KG_PROCESS_ERROR(pNpc->m_pScene);

	nResult = pNpc->LuaGetObj(L);	
Exit0:
	return nResult;
}

int KScene::LuaIsNickNameNpcExist(Lua_State* L)
{
    BOOL                        bResult     = false;
    int                         nRetCode    = false;
    KNpc*                       pNpc        = NULL;
    const char*                 pszName     = NULL;
    KNICK_NAME_LIST::iterator   it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pszName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszName);

    it = m_NpcNickNameList.find(pszName);
    KG_PROCESS_ERROR(it != m_NpcNickNameList.end());

    pNpc = g_pSO3World->m_NpcSet.GetObj(it->second);
    KGLOG_PROCESS_ERROR(pNpc);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KScene::LuaGetDoodadByNickName(Lua_State* L)
{
    int                         nResult     = 0;
	int                         nRetCode    = false;
	KDoodad*                    pDoodad     = NULL;
	const char*                 pszName     = NULL;
    KNICK_NAME_LIST::iterator   it;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	pszName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszName);

	it = m_DoodadNickNameList.find(pszName);
	KG_PROCESS_ERROR(it != m_DoodadNickNameList.end());

	pDoodad = g_pSO3World->m_DoodadSet.GetObj(it->second);
    KGLOG_PROCESS_ERROR(pDoodad);

    KG_PROCESS_ERROR(pDoodad->m_pScene);

	nResult = pDoodad->LuaGetObj(L);	
Exit0:
	return nResult;
}

int KScene::LuaIsNickNameDoodadExist(Lua_State* L)
{
    BOOL                        bResult     = false;
    int                         nRetCode    = false;
    KDoodad*                    pDoodad     = NULL;
    const char*                 pszName     = NULL;
    KNICK_NAME_LIST::iterator   it;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pszName = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszName);

    it = m_DoodadNickNameList.find(pszName);
    KG_PROCESS_ERROR(it != m_DoodadNickNameList.end());

    pDoodad = g_pSO3World->m_DoodadSet.GetObj(it->second);
    KGLOG_PROCESS_ERROR(pDoodad);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KScene::LuaSetTimer(Lua_State* L)
{
    int         nParamCount     = 0;
	DWORD       dwID            = 0;
	int         nTime           = 0;
	const char* pszScriptName   = NULL;
	int         nParam1         = 0;
	int         nParam2         = 0;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 4);

	nTime           =   (int)Lua_ValueToNumber(L, 1);
	pszScriptName   =   Lua_ValueToString(L, 2);
	nParam1         =   (int)Lua_ValueToNumber(L, 3);
	nParam2         =   (int)Lua_ValueToNumber(L, 4);

    KGLOG_PROCESS_ERROR(pszScriptName);

	dwID = m_ScriptTimerList.SetTimer(nTime, pszScriptName, nParam1, nParam2);
	KGLOG_PROCESS_ERROR(dwID);

Exit0:
	lua_pushinteger(L, dwID);
	return 1;
}

int KScene::LuaStopTimer(Lua_State* L)
{
    BOOL    bResult        = false;
	BOOL    bRetCode       = false;
    int     nParamCount    = 0;
    DWORD   dwTimerID      = 0;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 1);

	dwTimerID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ScriptTimerList.StopTimer(dwTimerID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KScene::LuaRestartTimer(Lua_State* L)
{
    BOOL    bResult        = false;
	BOOL    bRetCode       = false;
    int     nParamCount    = 0;
    DWORD   dwTimerID      = 0;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 1);

	dwTimerID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ScriptTimerList.RestartTimer(dwTimerID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KScene::LuaRemoveTimer(Lua_State* L)
{
	BOOL    bResult        = false;
    BOOL    bRetCode       = false;
    int     nParamCount    = 0;
    DWORD   dwTimerID      = 0;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 1);

	dwTimerID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ScriptTimerList.RemoveTimer(dwTimerID);
	KG_PROCESS_ERROR(bRetCode);
	
	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

// 用户变量
int KScene::LuaSetCustomInteger4(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t   nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= INT_MIN && nInput <= INT_MAX);
    
    bRetCode = m_CustomData.SetValue(nOffset, (int32_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaGetCustomInteger4(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int32_t   nValue         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    bRetCode = m_CustomData.GetValue(nOffset, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    Lua_PushNumber(L, nValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetCustomInteger2(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t   nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= SHRT_MIN && nInput <= SHRT_MAX);
    
    bRetCode = m_CustomData.SetValue(nOffset, (int16_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaGetCustomInteger2(Lua_State* L)
{   
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int16_t   nValue         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    bRetCode = m_CustomData.GetValue(nOffset, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushNumber(L, nValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetCustomInteger1(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t   nInput         = 0;
    
    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= SCHAR_MIN && nInput <= SCHAR_MAX);

    bRetCode = m_CustomData.SetValue(nOffset, (int8_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaGetCustomInteger1(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int8_t    nValue         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    bRetCode = m_CustomData.GetValue(nOffset, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushNumber(L, nValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetCustomUnsigned4(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t   nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);
    
    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= UINT_MAX);

    bRetCode = m_CustomData.SetValue(nOffset, (uint32_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaGetCustomUnsigned4(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    uint32_t  uValue         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    bRetCode = m_CustomData.GetValue(nOffset, &uValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushNumber(L, uValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetCustomUnsigned2(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t   nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= USHRT_MAX);

    bRetCode = m_CustomData.SetValue(nOffset, (uint16_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaGetCustomUnsigned2(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    uint16_t  uValue         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    bRetCode = m_CustomData.GetValue(nOffset, &uValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushNumber(L, uValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetCustomUnsigned1(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t   nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= UCHAR_MAX);

    bRetCode = m_CustomData.SetValue(nOffset, (uint8_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaGetCustomUnsigned1(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    uint8_t   uValue         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    bRetCode = m_CustomData.GetValue(nOffset, &uValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushNumber(L, uValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetCustomBoolean(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    BOOL    bInput         = false;

    KGLOG_PROCESS_ERROR(nParamCount == 2);
    KGLOG_PROCESS_ERROR(Lua_IsBoolean(L, 2));

    nOffset = (int)Lua_ValueToNumber(L, 1);
    bInput  = (BOOL)Lua_ValueToBoolean(L, 2);

    bRetCode = m_CustomData.SetValue(nOffset, (int8_t)bInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;  
}

int KScene::LuaGetCustomBoolean(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    BOOL    bValue         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    bRetCode = m_CustomData.GetValue(nOffset, (int8_t*)&bValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushBoolean(L, bValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetCustomBitValue(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nByteOffset    = 0;
    int     nBitOffset     = 0;
    BOOL    bInput         = false;

    KGLOG_PROCESS_ERROR(nParamCount == 3);
    KGLOG_PROCESS_ERROR(Lua_IsBoolean(L, 3));

    nByteOffset = (int)Lua_ValueToNumber(L, 1);
    nBitOffset  = (int)Lua_ValueToNumber(L, 2);
    bInput      = (BOOL)Lua_ValueToBoolean(L, 3);

    KGLOG_PROCESS_ERROR(nBitOffset >= 0 && nBitOffset < CHAR_BIT);

    bRetCode = m_CustomData.SetBit(nByteOffset * CHAR_BIT + nBitOffset, bInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaGetCustomBitValue(Lua_State* L)
{
    BOOL    nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nByteOffset    = 0;
    int     nBitOffset     = 0;
    BOOL    bValue         = false;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nByteOffset = (int)Lua_ValueToNumber(L, 1);
    nBitOffset  = (int)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nBitOffset >= 0 && nBitOffset < CHAR_BIT);

    bRetCode = m_CustomData.GetBit(nByteOffset * CHAR_BIT + nBitOffset, &bValue);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushBoolean(L, bValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetCustomString(Lua_State* L)
{
    BOOL            bRetCode       = false;
    int             nParamCount    = Lua_GetTopIndex(L);
    int             nOffset        = 0;
    const char*     pcszValue      = NULL;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset     = (int)Lua_ValueToNumber(L, 1);
    pcszValue    = Lua_ValueToString(L, 2);

    KGLOG_PROCESS_ERROR(pcszValue);

    bRetCode = m_CustomData.SetString(nOffset, pcszValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaGetCustomString(Lua_State* L)
{
    int             nResult        = 0;
    BOOL            bRetCode       = false;
    int             nParamCount    = Lua_GetTopIndex(L);
    int             nOffset        = 0;
    const char*     pcszValue      = NULL;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    pcszValue = m_CustomData.GetString(nOffset);
    KGLOG_PROCESS_ERROR(pcszValue);

    Lua_PushString(L, pcszValue);

    nResult = 1;
Exit0:
    return nResult;
}

int KScene::LuaSetEscortNpcByNickName(Lua_State* L)
{
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    DWORD   dwQuestID       = 0;
    char*   pszNpcNickName  = NULL;
    KNpc*   pNpc            = NULL;
    KNICK_NAME_LIST::iterator it;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
    pszNpcNickName = (char*)Lua_ValueToString(L, 2);

    it = m_NpcNickNameList.find(pszNpcNickName);
	KG_PROCESS_ERROR(it != m_NpcNickNameList.end());

	pNpc = g_pSO3World->m_NpcSet.GetObj(it->second);
    KGLOG_PROCESS_ERROR(pNpc);

    KG_PROCESS_ERROR(pNpc->m_pScene);

    bRetCode = m_EscortQuestMgr.SetEscortNpc(dwQuestID, pNpc->m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaSetEscortQuestFailFrames(Lua_State* L)
{
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    DWORD   dwQuestID       = 0;
    int     nFrames         = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);
    
    dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
    nFrames = (int)Lua_ValueToNumber(L, 2);

    bRetCode = m_EscortQuestMgr.SetEscortQuestEndFrames(dwQuestID, nFrames);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaSetEscortQuestValue(Lua_State* L)
{
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    DWORD   dwQuestID       = 0;
    int     nIndex          = 0;
    int     nValue          = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);
    
    dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
    nIndex = (int)Lua_ValueToNumber(L, 2);
    nValue = (int)Lua_ValueToNumber(L, 3);

    bRetCode = m_EscortQuestMgr.SetEscortQuestValue(dwQuestID, nIndex, nValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaEscortPlayerAddBuff(Lua_State* L)
{
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    DWORD   dwQuestID       = 0;
    DWORD   dwBuffID        = 0;
    int     nBuffLevel      = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);
    
    dwQuestID       = (DWORD)Lua_ValueToNumber(L, 1);
    dwBuffID        = (DWORD)Lua_ValueToNumber(L, 2);
    nBuffLevel      = (int)Lua_ValueToNumber(L, 3);

    bRetCode = m_EscortQuestMgr.AddPlayerBuff(dwQuestID, dwBuffID, nBuffLevel);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaEscortPlayerDelBuff(Lua_State* L)
{
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    DWORD   dwQuestID       = 0;
    DWORD   dwBuffID        = 0;
    int     nBuffLevel      = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);
    
    dwQuestID       = (DWORD)Lua_ValueToNumber(L, 1);
    dwBuffID        = (DWORD)Lua_ValueToNumber(L, 2);
    nBuffLevel      = (int)Lua_ValueToNumber(L, 3);

    bRetCode = m_EscortQuestMgr.DelPlayerBuff(dwQuestID, dwBuffID, nBuffLevel);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KScene::LuaCloseEscortQuest(Lua_State* L)
{
	BOOL    bRetCode   = false;
    int     nTopIndex  = 0;
    DWORD   dwQuestID  = 0;
    int     nFrames    = 0;

	nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1 || nTopIndex == 2);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);

    if (nTopIndex == 2)
    {
        nFrames = (int)Lua_ValueToNumber(L, 2);
    }

	bRetCode = m_EscortQuestMgr.CloseEscortQuest(dwQuestID, nFrames);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return 0;
}

int KScene::LuaGetEscortQuestPlayerList(Lua_State* L)
{
    int                 nResult        = 0;
    int                 nTopIndex      = 0;
    DWORD               dwQuestID      = 0;
    int                 nTableIndex    = 1;
    const KEscortQuest* pcEscortQuest  = NULL;

	nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);

	pcEscortQuest = m_EscortQuestMgr.GetEscortQuest(dwQuestID);
	KGLOG_PROCESS_ERROR(pcEscortQuest);
    
    Lua_NewTable(L);
    
	for (int i = 0; i < MAX_PARTY_SIZE; ++i)
	{
        if (pcEscortQuest->dwPlayerID[i] == ERROR_ID)
        {
            continue;
        }

		Lua_PushNumber(L, nTableIndex++);
		Lua_NewTable(L);

		Lua_PushString(L, "dwPlayerID");
		Lua_PushNumber(L, pcEscortQuest->dwPlayerID[i]);
		Lua_SetTable(L, -3);

		Lua_PushString(L, "bValid");
		Lua_PushBoolean(L, pcEscortQuest->bValid[i]);
		Lua_SetTable(L, -3);

		Lua_SetTable(L, -3);
	}

    nResult = 1;
Exit0:
	return nResult;
}

int KScene::LuaSaveMap(Lua_State* L)
{
    int nTopIndex = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    g_RelayClient.DoSaveScenePlayerList(this);

    g_RelayClient.DoSaveSceneData(this);
    
    m_bSaved = true;

	KGLogPrintf(KGLOG_INFO, "Save map %s(%.2u, %d)\n", m_szName, m_dwMapID, m_nCopyIndex);
    
Exit0:
	return 0;
}

int KScene::LuaSendMessage(Lua_State* L)
{
    int         nRetCode    = false;
    const char* pszMessage  = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pszMessage = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszMessage);

    g_PlayerServer.DoSceneMessage(this, pszMessage);

Exit0:
    return 0;
}

#endif //_SERVER

#ifdef _SERVER
int KScene::LuaDebugRevive(Lua_State* L)
{
    int nRetCode    = false;
    int nTopIndex   = 0;
    int nGroupID    = 0;

    int nGroupSize          = 0;
    int nGroupMinSize       = 0;
    int nGroupMaxSize       = 0;
    int nFristReviveTime    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nGroupID = (int)Lua_ValueToNumber(L, 1);

    nRetCode = m_NpcReviveManager.GetReviveGroupState(nGroupID, &nGroupSize, &nGroupMinSize, &nGroupMaxSize, &nFristReviveTime);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    Lua_PushNumber(L, nGroupSize);
    Lua_PushNumber(L, nGroupMinSize);
    Lua_PushNumber(L, nGroupMaxSize);
    Lua_PushNumber(L, nFristReviveTime);

    return 4;
}
#endif

int KScene::LuaIsReviveCycle(Lua_State* L)
{
    int nResult   = 0;
    int nTopIndex = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_PushBoolean(L, m_nReviveCycle > 0);

    nResult = 1;
Exit0:
    return nResult;
}

#ifdef _SERVER
int KScene::LuaSetProgress(Lua_State* L)
{
    BOOL    bRetCode      = false;
    int     nTopIndex     = 0;
    int     nProgressID   = 0;
    BOOL    bFlag         = false;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    nProgressID     = (int)Lua_ValueToNumber(L, 1);
    bFlag           = (BOOL)Lua_ValueToBoolean(L, 2);

    KGLOG_PROCESS_ERROR(nProgressID >= 0 && nProgressID <= MAX_SCENE_PROGRESS_ID);
    
    bRetCode = m_ProgressData.SetBit(nProgressID, bFlag);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_bProgressChanged = true;
    
Exit0:
    return 0;
}

int KScene::LuaGetProgress(Lua_State* L)
{
    int     nResult       = 0;
    BOOL    bRetCode      = false;
    int     nTopIndex     = 0;
    int     nProgressID   = 0;
    BOOL    bFlag         = false;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nProgressID = (int)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(nProgressID >= 0 && nProgressID <= MAX_SCENE_PROGRESS_ID);
    
    bRetCode = m_ProgressData.GetBit(nProgressID, &bFlag);
    KGLOG_PROCESS_ERROR(bRetCode);
        
    Lua_PushBoolean(L, bFlag);

    nResult = 1;
Exit0:
    return nResult;
}
#endif

DEFINE_LUA_CLASS_BEGIN(KScene)
	REGISTER_LUA_STRING_READONLY(KScene, Name)
	REGISTER_LUA_DWORD_READONLY(KScene, MapID)
    REGISTER_LUA_INTEGER_READONLY(KScene, CopyIndex)
	REGISTER_LUA_DWORD_READONLY(KScene, ID)
    REGISTER_LUA_INTEGER_READONLY(KScene, Type)
	REGISTER_LUA_ENUM_READONLY(KScene, SceneState)
    REGISTER_LUA_BOOL_READONLY(KScene, ReviveInSitu)
    REGISTER_LUA_INTEGER_READONLY(KScene, InFightPlayerCount)

#if defined(_SERVER)
	REGISTER_LUA_FUNC(KScene, CreateNpc)
	REGISTER_LUA_FUNC(KScene, DestroyNpc)
	REGISTER_LUA_FUNC(KScene, CreateDoodad)
	REGISTER_LUA_FUNC(KScene, DestroyDoodad)
	REGISTER_LUA_FUNC(KScene, GetNpcByNickName)
    REGISTER_LUA_FUNC(KScene, IsNickNameNpcExist)
    REGISTER_LUA_FUNC(KScene, GetDoodadByNickName)
    REGISTER_LUA_FUNC(KScene, IsNickNameDoodadExist)
	REGISTER_LUA_FUNC(KScene, SetTimer)
	REGISTER_LUA_FUNC(KScene, StopTimer)
	REGISTER_LUA_FUNC(KScene, RestartTimer)
	REGISTER_LUA_FUNC(KScene, RemoveTimer)
    
    REGISTER_LUA_FUNC(KScene, SetEscortNpcByNickName)
    REGISTER_LUA_FUNC(KScene, SetEscortQuestFailFrames)
    REGISTER_LUA_FUNC(KScene, SetEscortQuestValue)
    REGISTER_LUA_FUNC(KScene, EscortPlayerAddBuff)
    REGISTER_LUA_FUNC(KScene, EscortPlayerDelBuff)
    REGISTER_LUA_FUNC(KScene, CloseEscortQuest)
    REGISTER_LUA_FUNC(KScene, GetEscortQuestPlayerList)

    REGISTER_LUA_FUNC(KScene, SaveMap)
    REGISTER_LUA_FUNC(KScene, SendMessage)

    REGISTER_LUA_FUNC(KScene, SetCustomInteger4)
    REGISTER_LUA_FUNC(KScene, GetCustomInteger4)
    REGISTER_LUA_FUNC(KScene, SetCustomInteger2)
    REGISTER_LUA_FUNC(KScene, GetCustomInteger2)
    REGISTER_LUA_FUNC(KScene, SetCustomInteger1)
    REGISTER_LUA_FUNC(KScene, GetCustomInteger1)
    REGISTER_LUA_FUNC(KScene, SetCustomUnsigned4)
    REGISTER_LUA_FUNC(KScene, GetCustomUnsigned4)
    REGISTER_LUA_FUNC(KScene, SetCustomUnsigned2)
    REGISTER_LUA_FUNC(KScene, GetCustomUnsigned2)
    REGISTER_LUA_FUNC(KScene, SetCustomUnsigned1)
    REGISTER_LUA_FUNC(KScene, GetCustomUnsigned1)
    REGISTER_LUA_FUNC(KScene, SetCustomBoolean)
    REGISTER_LUA_FUNC(KScene, GetCustomBoolean)
    REGISTER_LUA_FUNC(KScene, SetCustomBitValue)
    REGISTER_LUA_FUNC(KScene, GetCustomBitValue)
    REGISTER_LUA_FUNC(KScene, SetCustomString)
    REGISTER_LUA_FUNC(KScene, GetCustomString)
#endif

    REGISTER_LUA_FUNC(KScene, IsReviveCycle)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KScene, DebugRevive)
#endif

#ifdef _SERVER
    REGISTER_LUA_FUNC(KScene, GetProgress)
    REGISTER_LUA_FUNC(KScene, SetProgress)
#endif
    
DEFINE_LUA_CLASS_END(KScene)

#include "stdafx.h"
#include "KDoodad.h"
#include "KLootList.h"
#include "KPlayer.h"
#include "KPlayerClient.h"
#include "KSO3World.h"
#include "KScene.h"
#include "KRecipe.h"
#include "KPlayerServer.h"

#ifdef _CLIENT
int KDoodad::LuaGetLootItem(Lua_State* L)
{
	int                         nRetCode        = 0;
    BOOL                        bNeedRoll       = false;
	BOOL                        bLeaderMode     = false;
    DWORD                       dwLootIndex     = 0;
    KPlayer*                    pPlayer         = NULL;
    KLootItem*                  pLootItem       = NULL;
    Luna<KPlayer>::KObjData*    pObj            = NULL;
    ITEM_LOOT_MODE              eLootMode       = ilmInvalid;
    int                         nRollQuality    = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwLootIndex = (DWORD)Lua_ValueToNumber(L, 1);
	pObj = (Luna<KPlayer>::KObjData*)Lua_ValueToUserData(L, 2);
	KGLOG_PROCESS_ERROR(pObj);

	pPlayer = pObj->pObj;
	KGLOG_PROCESS_ERROR(pPlayer);

	KGLOG_PROCESS_ERROR(m_pLootList);

	pLootItem = m_pLootList->GetLootItem(dwLootIndex);
	KGLOG_PROCESS_ERROR(pLootItem);

	KG_PROCESS_ERROR(pLootItem->pItem);
	
	if (pLootItem->eLootItemType == litNeedDistribute)
    {
        bLeaderMode = true;
    }

	KG_PROCESS_ERROR(pLootItem->bDisplay);

	nRetCode = pLootItem->pItem->LuaGetObj(L);
	KGLOG_PROCESS_ERROR(nRetCode);
    
    bNeedRoll = (pLootItem->eLootItemType == litNeedRoll);
    Lua_PushBoolean(L, bNeedRoll);

	Lua_PushBoolean(L, bLeaderMode);

	return 3;
Exit0:
	return 0;
}

int KDoodad::LuaGetLootMoney(lua_State *L)
{
	int nResult = 0;
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	KGLOG_PROCESS_ERROR(m_pLootList);

	nResult = m_pLootList->GetMoney();
Exit0:
	Lua_PushNumber(L, nResult);
	return 1;
}

int KDoodad::LuaCanLoot(Lua_State* L)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KPlayer*    pPlayer     = NULL;
    DWORD       dwPlayerID  = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 1);	
    
    KG_PROCESS_ERROR(m_pLootList);

	pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
	KG_PROCESS_ERROR(pPlayer);

	bRetCode = m_pLootList->IsVisible(pPlayer, this);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KDoodad::LuaDistributeItem(Lua_State* L)
{
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	DWORD dwItemID		= (DWORD)Lua_ValueToNumber(L, 1);
	DWORD dwDstPlayerID = (DWORD)Lua_ValueToNumber(L, 2);

	g_PlayerClient.DoDistributeItem(m_dwID, dwItemID, dwDstPlayerID);
Exit0:
	return 0;
}

int KDoodad::LuaGetLooterList(Lua_State* L)
{
	int                 nLooterCount    = 0;
	BOOL                bIsInLooterList = false;
	KPlayer*            pPlayer         = NULL;
    int                 nTable          = 0;
    KGetAllTeamMember   Func;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
	KGLOG_PROCESS_ERROR(m_pLootList);

    pPlayer = g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);
	
	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);
    
    KG_PROCESS_ERROR(pPlayer->m_dwTeamID != ERROR_ID);

    Func.m_dwExceptID = ERROR_ID;
    Func.m_vecResult.clear();

    g_pSO3World->m_TeamClient.Traverse(Func);

	for (DWORD dwIndex = 0; dwIndex < Func.m_vecResult.size(); dwIndex++)
	{
        bIsInLooterList = m_pLootList->IsInLooterList(Func.m_vecResult[dwIndex].dwMemberID);
        
		++nLooterCount;

		Lua_PushNumber(L, nLooterCount);

		Lua_NewTable(L);
		int nCount = Lua_GetTopIndex(L);

		Lua_PushString(L, "szName");
		Lua_PushString(L, Func.m_vecResult[dwIndex].szRoleName);
		Lua_SetTable(L, nCount);

		Lua_PushString(L, "dwID");
		Lua_PushNumber(L, Func.m_vecResult[dwIndex].dwMemberID);
		Lua_SetTable(L, nCount);

		Lua_PushString(L, "bOnlineFlag");
		Lua_PushBoolean(L, (Func.m_vecResult[dwIndex].bOnlineFlag && bIsInLooterList));
		Lua_SetTable(L, nCount);

		Lua_SetTable(L, nTable);
	}

	return 1;
Exit0:
	return 0;
}

int KDoodad::LuaCanSit(Lua_State* L)
{
    BOOL bResult  = false;
    
    KG_PROCESS_ERROR(m_eKind == dkChair);
    KG_PROCESS_ERROR(m_eState == dsIdle);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KDoodad::LuaGetRollFrame(Lua_State* L)
{
    Lua_PushNumber(L, MAX_ROLL_FRAME);
    return 1;
}
#endif // _CLIENT

#if defined(_SERVER)
int KDoodad::LuaSetScript(Lua_State *L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
	const char *pszTemp;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	pszTemp = Lua_ValueToString(L, 1);
	KGLOG_PROCESS_ERROR(pszTemp);

	bRetCode = SetScript(pszTemp);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KDoodad::LuaGenerateEmptyLootList(Lua_State *L)
{
    BOOL    bResult   = false;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
    KGLOG_PROCESS_ERROR(m_pLootList == NULL);

	m_pLootList = GenerateEmptyLootList();
	KGLOG_PROCESS_ERROR(m_pLootList);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KDoodad::LuaAddLootItem(Lua_State *L)
{
	BOOL            bRetCode        = false;
	KItem*          pItem           = NULL;
	DWORD           dwPlayerID      = ERROR_ID;
	DWORD           dwTabType       = 0;
	DWORD           dwIndex         = 0;
	int             nStackNum       = 0;
	KPlayer*        pPlayer         = NULL;
    KLootItem*      pAddLootItem    = NULL;
	int             nParamCount     = 0;
    DWORD           dwBookID        = 0;
    DWORD           dwSubID         = 0;
    int             nBookRecipeID   = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 3 || nParamCount == 4 || nParamCount == 5);
    
    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 1);
	dwTabType   = (DWORD)Lua_ValueToNumber(L, 2);
	dwIndex     = (DWORD)Lua_ValueToNumber(L, 3);
	
	if (nParamCount == 4)
	{
		nStackNum = (int)Lua_ValueToNumber(L, 4);
	}
    else if (nParamCount == 5)
    {
        dwBookID        = (DWORD)Lua_ValueToNumber(L, 4);
        dwSubID         = (DWORD)Lua_ValueToNumber(L, 5);
        nBookRecipeID   = BookID2RecipeID(dwBookID, dwSubID);
    }

	if (dwPlayerID)
	{
		pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
		KGLOG_PROCESS_ERROR(pPlayer);
	}

	pItem = g_pSO3World->m_ItemManager.GenerateItem(CURRENT_ITEM_VERSION, dwTabType, dwIndex);
	KGLOG_PROCESS_ERROR(pItem);

	if (nStackNum > 0)
	{
		pItem->m_nStackNum = nStackNum;
		if (pItem->m_nStackNum > pItem->m_nMaxStackNum)
        {
            pItem->m_nStackNum = pItem->m_nMaxStackNum;
        }
	}
	else
	{
		pItem->m_nCurrentDurability = pItem->m_nMaxDurability;
	}
    
    if (nBookRecipeID > 0)
    {
        KGLOG_PROCESS_ERROR(pItem->m_Common.nGenre == igBook);
        pItem->m_nCurrentDurability = nBookRecipeID; // 书用耐久度来表示某一本书
    }

	if (m_pLootList == NULL)
    {
		m_pLootList = GenerateEmptyLootList();
        KGLOG_PROCESS_ERROR(m_pLootList);
    }

	pAddLootItem = m_pLootList->AddItem(pItem);
	KGLOG_PROCESS_ERROR(pAddLootItem);
    pItem = NULL;
    
    pAddLootItem->eLootItemType = litAbsoluteFree;
    if (pPlayer)
    {
        m_pLootList->m_LooterList[m_pLootList->m_nLooterCount++] = pPlayer->m_dwID;

        pAddLootItem->eLootItemType = litOwnerLoot;
        bRetCode = m_pLootList->SetItemLooter(m_pLootList->GetItemCount() - 1, pPlayer->m_dwID);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
	
    assert(pAddLootItem->pItem);
	return pAddLootItem->pItem->LuaGetObj(L);
Exit0:
	if (pItem)
	{
		g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
	}
	return 0;
}

int KDoodad::LuaSetDisappearFrames(Lua_State* L)
{
	BOOL bResult            = false;
	int  nTopIndex          = 0;
    int  nFrames            = 0l;
	BOOL bGotoReviveList    = false;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR((nTopIndex == 1) || (nTopIndex == 2));

	nFrames = (int)Lua_ValueToNumber(L, 1);
	if (nTopIndex == 2)
	{
		bGotoReviveList = (BOOL)Lua_ValueToNumber(L, 2);
	}

	SetDisappearFrames(nFrames, bGotoReviveList);
    
    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KDoodad::LuaIsDoorClose(Lua_State* L)
{
    BOOL            bResult         = false;
    
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
    KGLOG_PROCESS_ERROR(m_pScene);

    bResult = m_pScene->GetDynamicObstacleState(m_nObstacleGroup);

Exit0:
	Lua_PushBoolean(L, bResult);
    return 1;
}

int KDoodad::LuaOpenDoor(Lua_State* L)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nParamCount     = Lua_GetTopIndex(L);
    BOOL            bState          = false;

    KGLOG_PROCESS_ERROR(nParamCount == 0);
    KGLOG_PROCESS_ERROR(m_eKind == dkDoor);     // 只有门类型才可以直接更改状态
    
    KGLOG_PROCESS_ERROR(m_pScene);

    bState = m_pScene->GetDynamicObstacleState(m_nObstacleGroup);
    KG_PROCESS_SUCCESS(!bState);

    bRetCode = m_pScene->SetDynamicObstacleState(m_nObstacleGroup, false);
    KG_PROCESS_ERROR(bRetCode);

    ChangeState(dsAfterHit);

Exit1:
    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KDoodad::LuaCloseDoor(Lua_State* L)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    int             nParamCount     = Lua_GetTopIndex(L);
    BOOL            bState          = false;

    KGLOG_PROCESS_ERROR(nParamCount == 0);
    KGLOG_PROCESS_ERROR(m_eKind == dkDoor);     // 只有门类型才可以直接更改状态
    
    KGLOG_PROCESS_ERROR(m_pScene);

    bState = m_pScene->GetDynamicObstacleState(m_nObstacleGroup);
    KG_PROCESS_SUCCESS(bState);

    bRetCode = m_pScene->SetDynamicObstacleState(m_nObstacleGroup, true);
    KG_PROCESS_ERROR(bRetCode);

    ChangeState(dsIdle);

Exit1:
    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KDoodad::LuaSetCustomInteger4(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t nInput         = 0;
    
    KGLOG_PROCESS_ERROR(nParamCount == 2);

    assert(m_pScene);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= INT_MIN && nInput <= INT_MAX);

    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetValue(nOffset, (int32_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomInteger4(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int32_t   nValue         = 0;

    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_SUCCESS(it == m_pScene->m_DoodadCustomDataTable.end());

    bRetCode = it->second.GetValue(nOffset, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    nResult = 1;
Exit0:
    if (nResult == 1)
    {
        Lua_PushNumber(L, nValue);
    }
    return nResult;
}

int KDoodad::LuaSetCustomInteger2(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);

    assert(m_pScene);

    KGLOG_PROCESS_ERROR(nInput >= SHRT_MIN && nInput <= SHRT_MAX);

    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetValue(nOffset, (int16_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomInteger2(Lua_State* L)
{   
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int16_t nValue         = 0;

    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_SUCCESS(it == m_pScene->m_DoodadCustomDataTable.end());

    bRetCode = it->second.GetValue(nOffset, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    nResult = 1;
Exit0:
    if (nResult == 1)
    {
        Lua_PushNumber(L, nValue);
    }
    return nResult;
}

int KDoodad::LuaSetCustomInteger1(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= SCHAR_MIN && nInput <= SCHAR_MAX);
    assert(m_pScene);

    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetValue(nOffset, (int8_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomInteger1(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int8_t    nValue         = 0;

    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_SUCCESS(it == m_pScene->m_DoodadCustomDataTable.end());

    bRetCode = it->second.GetValue(nOffset, &nValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    nResult = 1;
Exit0:
    if (nResult == 1)
    {
        Lua_PushNumber(L, nValue);
    }
    return nResult;
}

int KDoodad::LuaSetCustomUnsigned4(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);
    
    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= UINT_MAX);
    assert(m_pScene);

    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetValue(nOffset, (uint32_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomUnsigned4(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    uint32_t  uValue         = 0;
    
    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_SUCCESS(it == m_pScene->m_DoodadCustomDataTable.end());

    bRetCode = it->second.GetValue(nOffset, &uValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    nResult = 1;
Exit0:
    if (nResult == 1)
    {
        Lua_PushNumber(L, uValue);
    }
    return nResult;
}

int KDoodad::LuaSetCustomUnsigned2(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= USHRT_MAX);
    assert(m_pScene);
    
    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetValue(nOffset, (uint16_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomUnsigned2(Lua_State* L)
{
    int      nResult        = 0;
    BOOL     bRetCode       = false;
    int      nParamCount    = Lua_GetTopIndex(L);
    int      nOffset        = 0;
    uint16_t uValue         = 0;
    
    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_SUCCESS(it == m_pScene->m_DoodadCustomDataTable.end());

    bRetCode = it->second.GetValue(nOffset, &uValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    nResult = 1;
Exit0:
    if (nResult == 1)
    {
        Lua_PushNumber(L, uValue);
    }
    return nResult;
}

int KDoodad::LuaSetCustomUnsigned1(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    int64_t nInput         = 0;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset = (int)Lua_ValueToNumber(L, 1);
    nInput  = (int64_t)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nInput >= 0 && nInput <= UCHAR_MAX);
    assert(m_pScene);
   
    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetValue(nOffset, (int8_t)nInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomUnsigned1(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    uint8_t   uValue         = 0;

    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 1);
    
    nOffset = (int)Lua_ValueToNumber(L, 1);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_SUCCESS(it == m_pScene->m_DoodadCustomDataTable.end());

    bRetCode = it->second.GetValue(nOffset, &uValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    nResult = 1;
Exit0:
    if (nResult == 1)
    {
        Lua_PushNumber(L, uValue);
    }
    return nResult;
}

int KDoodad::LuaSetCustomBoolean(Lua_State* L)
{
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    BOOL    bInput         = false;

    KGLOG_PROCESS_ERROR(nParamCount == 2);
    KGLOG_PROCESS_ERROR(Lua_IsBoolean(L, 2));
    
    nOffset = (int)Lua_ValueToNumber(L, 1);
    bInput  = (BOOL)Lua_ValueToBoolean(L, 2);

    assert(m_pScene);

    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetValue(nOffset, (int8_t)bInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomBoolean(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nOffset        = 0;
    BOOL    bValue         = false;

    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_SUCCESS(it == m_pScene->m_DoodadCustomDataTable.end());

    bRetCode = it->second.GetValue(nOffset, (int8_t*)&bValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    nResult = 1;
Exit0:
    if (nResult == 1)
    {
        Lua_PushBoolean(L, bValue);
    }
    return nResult;
}

int KDoodad::LuaSetCustomBitValue(Lua_State* L)
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

    assert(m_pScene);

    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetBit(nByteOffset * CHAR_BIT + nBitOffset, bInput);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomBitValue(Lua_State* L)
{
    int     nResult        = 0;
    BOOL    bRetCode       = false;
    int     nParamCount    = Lua_GetTopIndex(L);
    int     nByteOffset    = 0;
    int     nBitOffset     = 0;
    BOOL    bValue         = false;
    
    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nByteOffset = (int)Lua_ValueToNumber(L, 1);
    nBitOffset  = (int)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nBitOffset >= 0 && nBitOffset < CHAR_BIT);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_SUCCESS(it == m_pScene->m_DoodadCustomDataTable.end());

    bRetCode = it->second.GetBit(nByteOffset * CHAR_BIT + nBitOffset, &bValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit1:
    nResult = 1;
Exit0:
    if (nResult == 1)
    {
        Lua_PushBoolean(L, bValue);
    }
    return nResult;
}

int KDoodad::LuaSetCustomString(Lua_State* L)
{
    BOOL            bRetCode       = false;
    int             nParamCount    = Lua_GetTopIndex(L);
    int             nOffset        = 0;
    const char*     pcszValue      = NULL;

    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nOffset     = (int)Lua_ValueToNumber(L, 1);
    pcszValue    = Lua_ValueToString(L, 2);

    KGLOG_PROCESS_ERROR(pcszValue);

    assert(m_pScene);

    bRetCode = m_pScene->m_DoodadCustomDataTable[m_dwID].SetValue(nOffset, pcszValue);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KDoodad::LuaGetCustomString(Lua_State* L)
{
    int             nResult        = 0;
    BOOL            bRetCode       = false;
    int             nParamCount    = Lua_GetTopIndex(L);
    int             nOffset        = 0;
    const char*     pcszValue      = NULL;

    KOBJ_CUSTOM_DATA_TABLE::iterator it;

    KGLOG_PROCESS_ERROR(nParamCount == 1);

    nOffset = (int)Lua_ValueToNumber(L, 1);

    assert(m_pScene);

    it = m_pScene->m_DoodadCustomDataTable.find(m_dwID);
    KG_PROCESS_ERROR(it != m_pScene->m_DoodadCustomDataTable.end());

    pcszValue = it->second.GetString(nOffset);
    KGLOG_PROCESS_ERROR(pcszValue);
    
    Lua_PushString(L, pcszValue);

    nResult = 1;
Exit0:
    return nResult;
}
#endif	//_SERVER

int KDoodad::LuaCanDialog(Lua_State* L)
{
    BOOL                     bResult    = false;
	BOOL                     bRetCode   = false;
	KPlayer*                 pPlayer    = NULL;
    Luna<KPlayer>::KObjData* pObj       = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	pObj  = (Luna<KPlayer>::KObjData*)Lua_ValueToUserData(L, 1);
	KG_PROCESS_ERROR(pObj);

	pPlayer = pObj->pObj;
	KG_PROCESS_ERROR(pPlayer);

	bRetCode = CheckDistance(pPlayer);
	KG_PROCESS_ERROR(bRetCode);
	
	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KDoodad::LuaIsSelectable(Lua_State* L)
{
	BOOL bResult = false;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	KGLOG_PROCESS_ERROR(m_pTemplate);
	KG_PROCESS_ERROR(m_pTemplate->m_bSelectable);
    
    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KDoodad::LuaCanSearch(Lua_State* L)
{
	Lua_PushBoolean(L, m_bLooted);
	return 1;
}

int KDoodad::LuaHaveQuest(Lua_State* L)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
	KPlayer*    pPlayer     = NULL;
    DWORD       dwPlayerID  = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
	
	dwPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
	pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
	KGLOG_PROCESS_ERROR(pPlayer);

	bRetCode = HaveQuest(pPlayer);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KDoodad::LuaGetRecipeID(Lua_State* L)
{
    DWORD dwRecipeID = 0;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

    dwRecipeID = GetRecipeID();

Exit0:
    Lua_PushNumber(L, dwRecipeID);
    return 1;
}

DEFINE_LUA_CLASS_BEGIN(KDoodad)
	REGISTER_LUA_DWORD_READONLY(KBaseObject, ID)

	REGISTER_LUA_INTEGER_READONLY(KSceneObject, X)
	REGISTER_LUA_INTEGER_READONLY(KSceneObject, Y)
	REGISTER_LUA_INTEGER_READONLY(KSceneObject, Z)
	REGISTER_LUA_FUNC(KSceneObject, GetAbsoluteCoordinate)
	REGISTER_LUA_INTEGER(KSceneObject, TouchRange)
	REGISTER_LUA_INTEGER_READONLY(KSceneObject, XCell)
	REGISTER_LUA_INTEGER_READONLY(KSceneObject, YCell)
	REGISTER_LUA_INTEGER(KSceneObject, Mass)
	REGISTER_LUA_INTEGER(KSceneObject, Width)
	REGISTER_LUA_INTEGER(KSceneObject, Length)
	REGISTER_LUA_INTEGER(KSceneObject, Height)
	REGISTER_LUA_INTEGER(KSceneObject, FaceDirection)
	REGISTER_LUA_INTEGER(KSceneObject, PitchDirection)

	REGISTER_LUA_DWORD(KDoodad, TemplateID)
	REGISTER_LUA_STRING(KDoodad, Name)
	REGISTER_LUA_ENUM(KDoodad, Kind)
	REGISTER_LUA_INTEGER(KDoodad, OpenFrames)
	REGISTER_LUA_INTEGER(KDoodad, DisappearFrames)
	REGISTER_LUA_DWORD(KDoodad, NpcTemplateID)
	REGISTER_LUA_DWORD(KDoodad, RepresentID)
	REGISTER_LUA_DWORD(KDoodad, ScriptID)

#if defined(_SERVER)
	REGISTER_LUA_DWORD(KDoodad, OwnerID)
#endif //_SERVER

	REGISTER_LUA_FUNC(KDoodad, GetScene)
	REGISTER_LUA_FUNC(KDoodad, CanDialog)
	REGISTER_LUA_FUNC(KDoodad, IsSelectable)
    REGISTER_LUA_FUNC(KDoodad, CanSearch)
	REGISTER_LUA_FUNC(KDoodad, HaveQuest)
    REGISTER_LUA_FUNC(KDoodad, GetRecipeID)

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KDoodad, GetLootItem)
	REGISTER_LUA_FUNC(KDoodad, GetLootMoney)
    REGISTER_LUA_FUNC(KDoodad, CanLoot)
	REGISTER_LUA_FUNC(KDoodad, DistributeItem)
	REGISTER_LUA_FUNC(KDoodad, GetLooterList)
    REGISTER_LUA_FUNC(KDoodad, CanSit)
    REGISTER_LUA_FUNC(KDoodad, GetRollFrame)
#endif //_CLIENT

#if defined(_SERVER)
	REGISTER_LUA_FUNC(KDoodad, SetScript)
	REGISTER_LUA_FUNC(KDoodad, GenerateEmptyLootList)
	REGISTER_LUA_FUNC(KDoodad, AddLootItem)
	REGISTER_LUA_FUNC(KDoodad, SetDisappearFrames)
    REGISTER_LUA_FUNC(KDoodad, IsDoorClose)
    REGISTER_LUA_FUNC(KDoodad, OpenDoor)
    REGISTER_LUA_FUNC(KDoodad, CloseDoor)

    REGISTER_LUA_FUNC(KDoodad, SetCustomInteger4)
    REGISTER_LUA_FUNC(KDoodad, GetCustomInteger4)
    REGISTER_LUA_FUNC(KDoodad, SetCustomInteger2)
    REGISTER_LUA_FUNC(KDoodad, GetCustomInteger2)
    REGISTER_LUA_FUNC(KDoodad, SetCustomInteger1)
    REGISTER_LUA_FUNC(KDoodad, GetCustomInteger1)
    REGISTER_LUA_FUNC(KDoodad, SetCustomUnsigned4)
    REGISTER_LUA_FUNC(KDoodad, GetCustomUnsigned4)
    REGISTER_LUA_FUNC(KDoodad, SetCustomUnsigned2)
    REGISTER_LUA_FUNC(KDoodad, GetCustomUnsigned2)
    REGISTER_LUA_FUNC(KDoodad, SetCustomUnsigned1)
    REGISTER_LUA_FUNC(KDoodad, GetCustomUnsigned1)
    REGISTER_LUA_FUNC(KDoodad, SetCustomBoolean)
    REGISTER_LUA_FUNC(KDoodad, GetCustomBoolean)
    REGISTER_LUA_FUNC(KDoodad, SetCustomBitValue)
    REGISTER_LUA_FUNC(KDoodad, GetCustomBitValue)
    REGISTER_LUA_FUNC(KDoodad, SetCustomString)
    REGISTER_LUA_FUNC(KDoodad, GetCustomString)
#endif //_SERVER      

DEFINE_LUA_CLASS_END(KDoodad)

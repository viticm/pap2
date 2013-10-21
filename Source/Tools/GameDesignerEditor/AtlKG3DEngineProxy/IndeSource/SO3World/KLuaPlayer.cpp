#include "stdafx.h"
#include <algorithm>
#include "KAISearchTactics.h"
#include "KCraft.h"
#include "KRecipeMaster.h"
#include "KSO3World.h"

#ifdef _CLIENT
#include "KPlayerClient.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3Represent/SO3GameWorldRepresentHandler.h"
#include "Engine/KTextFilter.h"
#else
#include "KPlayerServer.h"
#include "KRelayClient.h"
#endif

#ifdef _SERVER

int KPlayer::LuaSetLastEntry(Lua_State* L)
{
    BOOL    bResult         = false;
	int     nTopIndex       = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 6);

	m_LastEntry.dwMapID         = (DWORD)Lua_ValueToNumber(L, 1);
	m_LastEntry.nMapCopyIndex   = (int)Lua_ValueToNumber(L, 2);
    m_LastEntry.nX              = (int)Lua_ValueToNumber(L, 3);
    m_LastEntry.nY              = (int)Lua_ValueToNumber(L, 4);
    m_LastEntry.nZ              = (int)Lua_ValueToNumber(L, 5);
    m_LastEntry.nFaceDirection  = (int)Lua_ValueToNumber(L, 6);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaSwitchMap(Lua_State* L)
{
	int         nRetCode        = false;
	int         nParamCount     = 0;
    DWORD       dwMapID         = 0;
    int         nMapCopyIndex   = 0;
	int         nX              = 0;
	int         nY              = 0;
	int         nZ              = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 4 || nParamCount == 5);

    KGLOG_PROCESS_ERROR(m_eGameStatus == gsPlaying);
	KGLOG_PROCESS_ERROR(m_pScene && m_pRegion);

    if (nParamCount == 4)
    {
        dwMapID         = (DWORD)Lua_ValueToNumber(L, 1);
        nX              = (int)Lua_ValueToNumber(L, 2);
        nY              = (int)Lua_ValueToNumber(L, 3);
        nZ              = (int)Lua_ValueToNumber(L, 4);
    }
    else
    {
        dwMapID         = (DWORD)Lua_ValueToNumber(L, 1);
        nMapCopyIndex   = (int)Lua_ValueToNumber(L, 2);
        nX              = (int)Lua_ValueToNumber(L, 3);
        nY              = (int)Lua_ValueToNumber(L, 4);
        nZ              = (int)Lua_ValueToNumber(L, 5);
    }

    nRetCode = SwitchMap(dwMapID, nMapCopyIndex, nX, nY, nZ);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}

int KPlayer::LuaResetMap(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
	int     nTopIndex       = 0;
    DWORD   dwMapID         = 0;
    int     nMapCopyIndex   = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwMapID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = g_RelayClient.DoResetMapCopyRequest(m_dwID, dwMapID, nMapCopyIndex);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaAutoFly(Lua_State* L)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
	int     nParamCount     = 0;
    int     nFromNode       = 0;
    int     nToCity         = 0;

	KGLOG_PROCESS_ERROR(m_pScene && m_pRegion);
    KGLOG_PROCESS_ERROR(m_eGameStatus == gsPlaying);

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 2);

    nFromNode            = (int)Lua_ValueToNumber(L, 1);
    nToCity              = (int)Lua_ValueToNumber(L, 2);

    bRetCode = AutoFly(nFromNode, nToCity);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
	return 1;
}
#endif // _SERVER

int KPlayer::LuaGetBoxType(Lua_State* L)
{
    int     nResult     = 0;
	int     nType       = 0;
	DWORD   dwBox       = 0;
	int     nTopIndex   = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwBox = (DWORD)Lua_ValueToNumber(L, 1);

	nType = m_ItemList.GetBoxType(dwBox);
	
    Lua_PushNumber(L, nType);
    
    nResult = 1;
Exit0:
	return nResult;
}

int KPlayer::LuaGetBoxSize(Lua_State* L)
{
    int     nResult     = 0;
	DWORD   dwSize      = 0;
	DWORD   dwBox       = 0;
	int     nTopIndex   = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwBox = (DWORD)Lua_ValueToNumber(L, 1);

	dwSize = m_ItemList.GetBoxSize(dwBox);

	Lua_PushNumber(L, dwSize);
	
    nResult = 1;
Exit0:
	return nResult;
}

int KPlayer::LuaGetContainType(Lua_State* L)
{
	int     nResult     = 0;
    BOOL    bRetCode    = false;
	DWORD   dwBox       = 0;
	int     nTopIndex   = 0;
	int     nGenerType  = 0;
    int     nSubType    = 0;
    
	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwBox = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ItemList.GetContainType(dwBox, &nGenerType, &nSubType);
    KGLOG_PROCESS_ERROR(bRetCode);

	Lua_PushNumber(L, nGenerType);
    Lua_PushNumber(L, nSubType);

	nResult = 2;
Exit0:
	return nResult;
}

int KPlayer::LuaExchangeItem(Lua_State* L)
{
	ITEM_RESULT_CODE    eRetCode    = ircInvalid;
	int                 nTopIndex   = Lua_GetTopIndex(L);
    DWORD               dwSrcBox    = 0; 
    DWORD               dwSrcX      = 0;   
    DWORD               dwDestBox   = 0;  
    DWORD               dwDestX     = 0;    
    DWORD               dwAmount    = 0;    
    TItemPos            Src         = {0, 0};
    TItemPos            Dest        = {0, 0};

	KGLOG_PROCESS_ERROR(nTopIndex == 4 || nTopIndex == 5);
	
	dwSrcBox   = (DWORD)Lua_ValueToNumber(L, 1);
	dwSrcX     = (DWORD)Lua_ValueToNumber(L, 2);
	dwDestBox  = (DWORD)Lua_ValueToNumber(L, 3);
	dwDestX    = (DWORD)Lua_ValueToNumber(L, 4);

    if (nTopIndex == 5)
	{
		dwAmount = (DWORD)Lua_ValueToNumber(L, 5);
	}

#ifdef _SERVER
    Src.dwBox   = dwSrcBox; 
    Src.dwX     = dwSrcX;  
    Dest.dwBox  = dwDestBox;
    Dest.dwX    = dwDestX;  

	eRetCode = m_ItemList.ExchangeItem(Src, Dest, dwAmount);
	KGLOG_PROCESS_ERROR(eRetCode == ircSuccess);
#else
    KG_PROCESS_ERROR(dwSrcBox != dwDestBox || dwSrcX != dwDestX);
	KGLOG_PROCESS_ERROR(dwDestBox != ibInvalid); // 这个是删除物品。

	g_PlayerClient.DoApplyExchangeItem(dwSrcBox, dwSrcX, dwDestBox, dwDestX, dwAmount);
#endif

Exit0:
	return 0;
}

int KPlayer::LuaEnableBankPackage(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
	int     nCostMoney      = 0;
	int     nMoney          = 0;
	int     nEnabledCount   = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	nEnabledCount = m_ItemList.GetBankPackageCount();
	KG_PROCESS_ERROR(nEnabledCount < MAX_BANK_PACKAGE_COUNT);

	nMoney = m_ItemList.GetMoney();

	nCostMoney = g_pSO3World->m_Settings.m_ConstList.nBankPackagePrice[nEnabledCount];
	KG_PROCESS_ERROR(nMoney >= nCostMoney);

#ifdef _SERVER
	bRetCode = m_ItemList.EnableBankPackage();
	KG_PROCESS_ERROR(bRetCode);
#endif
#ifdef _CLIENT
    g_PlayerClient.DoEnableBankPackage();
#endif
	
    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

//获取激活的银行背包格子数
int	KPlayer::LuaGetBankPackageCount(Lua_State* L)
{
	int nBankPackageCount = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	nBankPackageCount = m_ItemList.GetBankPackageCount();
Exit0:
	Lua_PushNumber(L, nBankPackageCount);
	return 1;
}

int KPlayer::LuaIsBankPackageEnabled(Lua_State* L)
{
    BOOL    bResult             = false;
	BOOL    bRetCode            = false;
	int     nBankPackageIndex   = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nBankPackageIndex = (int)Lua_ValueToNumber(L, 1);
	
	bRetCode = m_ItemList.IsBankPackageEnabled((EQUIPMENT_INVENTORY_TYPE)nBankPackageIndex);
	KG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaDestroyItem(Lua_State* L)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
	KItem*  pItem       = NULL;
    DWORD   dwBox1      = 0;
    DWORD   dwX1        = 0;
    
	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwBox1    = (DWORD)Lua_ValueToNumber(L, 1);
	dwX1      = (DWORD)Lua_ValueToNumber(L, 2);
	
	KG_PROCESS_ERROR(dwBox1 != ibInvalid);

#ifdef _CLIENT
	g_PlayerClient.DoApplyExchangeItem(dwBox1, dwX1, ibInvalid, 0, 0);
#else
	pItem = m_ItemList.GetItem(dwBox1, dwX1);
	KGLOG_PROCESS_ERROR(pItem);
    
    bRetCode = m_ItemList.DestroyItem(pItem);
	KGLOG_PROCESS_ERROR(bRetCode == ircSuccess);
#endif 

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaGetRepresentID(Lua_State* L)
{
    lua_newtable(L);
	
	for (int nIndex = 0; nIndex < perRepresentCount; ++nIndex)
    {
        Lua_PushNumber(L, nIndex);
		Lua_PushNumber(L, m_wRepresentId[nIndex]);
        Lua_SetTable(L, -3);
    }
	
	return 1;
}

#ifdef _SERVER
int KPlayer::LuaSetRepresentID(Lua_State* L)
{
    BOOL    bRetCode        = false;
	int     nTopIndex       = 0;
    int     nRepresentIndex = 0;
    DWORD   dwEquipPos      = 0;
    KItem*  pItem           = NULL;
    WORD    wNewRepresentID = 0;
    
	nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 2);

    nRepresentIndex = (int)Lua_ValueToNumber(L, 1);
    wNewRepresentID = (WORD)Lua_ValueToNumber(L, 2);

    KGLOG_PROCESS_ERROR(nRepresentIndex >= 0 && nRepresentIndex < perRepresentCount);

    if (wNewRepresentID == 0)
    {
        bRetCode = m_ItemList.GetEquipPos(nRepresentIndex, &dwEquipPos);
        if (bRetCode)
        {            
            pItem = m_ItemList.GetItem(ibEquip, dwEquipPos);

            if (pItem)
            {
                wNewRepresentID = (WORD)pItem->m_nRepresentID;
            }
        }
    }

    m_wRepresentId[nRepresentIndex] = wNewRepresentID;

    g_PlayerServer.DoSyncEquipRepresent(this, nRepresentIndex, wNewRepresentID);

Exit0:
	return 0;
}
#endif

int KPlayer::LuaGetItem(Lua_State* L)
{
    DWORD   dwBoxIndex  = 0;
    DWORD   dwX         = 0;
    KItem*  pItem       = NULL;


	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwBoxIndex = (DWORD)Lua_ValueToNumber(L, 1);
	dwX = (DWORD)Lua_ValueToNumber(L, 2);

	pItem = m_ItemList.GetItem(dwBoxIndex, dwX);
	KG_PROCESS_ERROR(pItem);

	return pItem->LuaGetObj(L);
Exit0:
	return 0;
}

int KPlayer::LuaGetItemByIndex(Lua_State* L)
{
	BOOL    bRetCode	= false;
	DWORD   dwBox		= 0;
	DWORD   dwX		    = 0;
    DWORD   dwTab       = 0;
    DWORD   dwIndex     = 0;
    KItem*  pItem       = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwTab = (DWORD)Lua_ValueToNumber(L, 1);
	dwIndex = (DWORD)Lua_ValueToNumber(L, 2);

	bRetCode = m_ItemList.GetItemPos(CURRENT_ITEM_VERSION, dwTab, dwIndex, dwBox, dwX);
	KG_PROCESS_ERROR(bRetCode);

	pItem = m_ItemList.GetItem(dwBox, dwX);
	KG_PROCESS_ERROR(pItem);

	return pItem->LuaGetObj(L);
Exit0:
	return 0;
}

int KPlayer::LuaGetEquipItem(Lua_State* L)
{
	int     nResult     = 0;
	KItem*  pItem       = NULL;
    EQUIPMENT_INVENTORY_TYPE eEquipmentType = eitTotal;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	eEquipmentType = (EQUIPMENT_INVENTORY_TYPE)((DWORD)Lua_ValueToNumber(L, 1));

	pItem = m_ItemList.GetItem(ibEquip, eEquipmentType);  
	KG_PROCESS_ERROR(pItem);

	nResult = pItem->LuaGetObj(L);
Exit0:
	return nResult;
}


int KPlayer::LuaDelItem(Lua_State* L)
{
	return 0;
}

#ifdef _SERVER
int KPlayer::LuaCanAddItem(Lua_State* L)
{
    ADD_ITEM_RESULT_CODE    nResult         = aircFailed;
    ADD_ITEM_RESULT_CODE    eRetCode        = aircFailed;
    KItem*                  pItem           = NULL;
    DWORD                   dwTabType       = 0;
    DWORD                   dwIndex         = 0;
    int                     nStackNum       = 0;
    DWORD                   dwDestBox       = 0;
    DWORD                   dwDestPos       = 0;

    int nParamCount = Lua_GetTopIndex(L);

    KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 3 || nParamCount == 5);

    dwTabType   = (DWORD)Lua_ValueToNumber(L, 1);
    dwIndex     = (DWORD)Lua_ValueToNumber(L, 2);

    if (nParamCount >= 3)
    {
        nStackNum   = (int)Lua_ValueToNumber(L, 3);
    }

    if (nParamCount == 5)
    {
        dwDestBox  = (DWORD)Lua_ValueToNumber(L, 4);
        dwDestPos  = (DWORD)Lua_ValueToNumber(L, 5);
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
    
    if (nParamCount == 5)
    {
        eRetCode = m_ItemList.CanAddItemToPos(pItem, dwDestBox, dwDestPos);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, eRetCode);
    }
    else
    {
        eRetCode = m_ItemList.CanAddItem(pItem);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, eRetCode);
    }
    
    nResult = aircSuccess;
Exit0:
    if (pItem)
    {
        g_pSO3World->m_ItemManager.FreeItem(pItem);
        pItem = NULL;
    }
    Lua_PushNumber(L, nResult);
    return 1;
}

int KPlayer::LuaAddItem(Lua_State* L)
{
    ADD_ITEM_RESULT_CODE    nResult         = aircFailed;
    ADD_ITEM_RESULT_CODE    eRetCode        = aircFailed;
	KItem*                  pItem           = NULL;
	DWORD                   dwTabType       = 0;
	DWORD                   dwIndex         = 0;
	int                     nStackNum       = 0;
    DWORD                   dwBookID        = 0;
    DWORD                   dwSubID         = 0;
    int                     nBookRecipeID   = 0;
    DWORD                   dwBoxIndex      = 0;
    DWORD                   dwPosIndex      = 0;

	int nParamCount = Lua_GetTopIndex(L);
    
    KGLOG_PROCESS_ERROR(
        nParamCount == 2 || nParamCount == 3 || nParamCount == 4 ||
        nParamCount == 5 || nParamCount == 6
    );
    
    dwTabType   = (DWORD)Lua_ValueToNumber(L, 1);
    dwIndex     = (DWORD)Lua_ValueToNumber(L, 2);
    
    switch(nParamCount)
    {
    case 2:
        break;
    case 3:
        nStackNum   = (int)Lua_ValueToNumber(L, 3);
    	break;
    case 4:
        dwBookID        = (DWORD)Lua_ValueToNumber(L, 3);
        dwSubID         = (DWORD)Lua_ValueToNumber(L, 4);
        nBookRecipeID   = BookID2RecipeID(dwBookID, dwSubID);
    	break;
    case 5:
        nStackNum   = (int)Lua_ValueToNumber(L, 3);
        dwBoxIndex  = (DWORD)Lua_ValueToNumber(L, 4);
        dwPosIndex  = (DWORD)Lua_ValueToNumber(L, 5);
        break;
    case 6:
        dwBookID        = (DWORD)Lua_ValueToNumber(L, 3);
        dwSubID         = (DWORD)Lua_ValueToNumber(L, 4);
        dwBoxIndex      = (DWORD)Lua_ValueToNumber(L, 5);
        dwPosIndex      = (DWORD)Lua_ValueToNumber(L, 6);
        nBookRecipeID   = BookID2RecipeID(dwBookID, dwSubID);
        break;
    default:
        assert(false);
        break;
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

    if (nParamCount == 5 || nParamCount == 6)
    {
        eRetCode = m_ItemList.AddItemToPos(pItem, dwBoxIndex, dwPosIndex);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, eRetCode);

        goto Exit1;
    }

    eRetCode = m_ItemList.AddItem(pItem);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == aircSuccess, eRetCode);

Exit1:
	nResult = aircSuccess;
Exit0:
	if (nResult != aircSuccess)
	{
        if (pItem)
        {
            g_pSO3World->m_ItemManager.FreeItem(pItem);
            pItem = NULL;
        }
        g_PlayerServer.DoMessageNotify(m_nConnIndex, ectAddItemErrorCode, nResult);
	}

	Lua_PushBoolean(L, (nResult == aircSuccess));
	return 1;
}

int KPlayer::LuaCostItem(Lua_State* L)
{
    BOOL  bResult       = false;
    BOOL  bRetCode      = false;
    DWORD dwType	    = 0;
    DWORD dwIndex	    = 0;
    DWORD dwAmount	    = 0;
    DWORD dwBookID      = 0;
    DWORD dwSubID       = 0;
    int   nBookRecipeID = -1;
    int   nTopIndex     = Lua_GetTopIndex(L);

    KGLOG_PROCESS_ERROR(nTopIndex == 3 || nTopIndex == 5);

    dwType	    = (DWORD)Lua_ValueToNumber(L, 1);
    dwIndex	    = (DWORD)Lua_ValueToNumber(L, 2);
    dwAmount	= (DWORD)Lua_ValueToNumber(L, 3);
    
    if (nTopIndex == 5)
    {
        dwBookID = (DWORD)Lua_ValueToNumber(L, 4);
        dwSubID  = (DWORD)Lua_ValueToNumber(L, 5);
        nBookRecipeID = BookID2RecipeID(dwBookID, dwSubID);
    }
    
    bRetCode = m_ItemList.CostItemInEquipAndPackage(dwType, dwIndex, dwAmount, nBookRecipeID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaAddMoney(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
    int     nMoney          = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nMoney = (int)Lua_ValueToNumber(L, 1);

	bRetCode = m_ItemList.AddMoney(nMoney, true);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaAddContribution(Lua_State* L)
{
    BOOL    bResult             = false;
    BOOL    bRetCode            = false;
    int     nTopIndex           = 0;
    int     nAddContribution    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAddContribution = (int)Lua_ValueToNumber(L, 1);

    bRetCode = AddContribution(nAddContribution);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaOpenWindow(Lua_State* L)
{
    BOOL         bRetCode    = false;
    static DWORD s_dwIndex   = 0;
    DWORD        dwTargetID  = 0;
    TARGET_TYPE  eTargetType = ttInvalid;
    const char*  pcszText    = NULL;

    //检测参数个数
    int nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount <= (MAX_WINDOW_SELECT_COUNT + 3));

    eTargetType = (TARGET_TYPE)(int)Lua_ValueToNumber(L, 1);
    dwTargetID	= (DWORD)Lua_ValueToNumber(L, 2);

    pcszText = Lua_ValueToString(L, 3);
    KGLOG_PROCESS_ERROR(pcszText);

    for (int nIndex  = 0; nIndex < (nParamCount - 3); nIndex++)
    {
        const char* pcszFuncName = Lua_ValueToString(L, nIndex + 4);
        KGLOG_PROCESS_ERROR(pcszFuncName);

        strncpy(m_SelectList.szFuncName[nIndex], pcszFuncName, MAX_SELECT_FUNCNAME_LEN);
        m_SelectList.szFuncName[nIndex][MAX_SELECT_FUNCNAME_LEN - 1] = 0;
    }

    m_SelectList.nFuncCount = nParamCount - 3;

    KGLOG_PROCESS_ERROR(m_pScene);
    m_SelectList.dwMapID = m_pScene->m_dwMapID;
    m_SelectList.nMapCopyIndex = m_pScene->m_nCopyIndex;

    m_SelectList.nX = m_nX;
    m_SelectList.nY = m_nY;
    m_SelectList.nZ = m_nZ;
    m_SelectList.dwIndex = s_dwIndex;
    m_SelectList.dwScriptID = g_pSO3World->m_ScriptCenter.GetActiveScriptID();

    bRetCode = m_SelectList.Target.SetTarget(eTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(bRetCode);

    g_PlayerServer.DoOpenWindow(s_dwIndex, eTargetType, dwTargetID, pcszText, m_nConnIndex);

    s_dwIndex++;

Exit0:
    return 0;
}

int KPlayer::LuaGetLastSaveTime(Lua_State* L)
{
    Lua_PushNumber(L, m_nLastSaveTime);
    return 1;
}

int KPlayer::LuaGetLastLoginTime(Lua_State* L)
{
    Lua_PushNumber(L, m_nLastLoginTime);
    return 1;
}

int KPlayer::LuaGetCurrentLoginTime(Lua_State* L)
{
    Lua_PushNumber(L, m_nCurrentLoginTime);
    return 1;
}
#endif	//_SERVER

int KPlayer::LuaGetMoney(Lua_State *L)
{
	BOOL bRetCode = false;
	int nMoney = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	nMoney = m_ItemList.GetMoney();
Exit0:
	Lua_PushNumber(L, nMoney);
	return 1;
}

#if defined(_SERVER)
int KPlayer::LuaAddExp(Lua_State* L)
{
    int nRetCode      = 0;
	int nExpIncrement = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nExpIncrement = (int)Lua_ValueToNumber(L, 1);

	AddExp(nExpIncrement);

Exit0:
	return 0;
}
#endif	//_SERVER

#if defined(_SERVER)
int KPlayer::LuaAddStamina(Lua_State* L)
{
    int     nRetCode        = 0;
    int     nAddStamina     = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nAddStamina = (int)Lua_ValueToNumber(L, 1);

    m_nCurrentStamina += nAddStamina;

    MAKE_IN_RANGE(m_nCurrentStamina, 0, m_nMaxStamina);

    g_PlayerServer.DoSyncSelfCurrentST(this);

Exit0:
	return 0;
}
#endif

#if defined(_SERVER)
int KPlayer::LuaAddThew(Lua_State* L)
{
    int     nRetCode        = 0;
    int     nAddThew        = 0;
    
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nAddThew = (int)Lua_ValueToNumber(L, 1);

    m_nCurrentThew += nAddThew;

    MAKE_IN_RANGE(m_nCurrentThew, 0, m_nMaxThew);

    g_PlayerServer.DoSyncSelfCurrentST(this);

Exit0:
    return 0;
}
#endif

#ifdef _CLIENT
int KPlayer::LuaOpenBook(Lua_State* L)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    KItem*  pBook       = NULL;
    DWORD   dwBox       = 0;
    DWORD   dwX         = 0;
    DWORD   dwBookID    = 0;
    DWORD   dwSubID     = 0;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

    dwBox = (DWORD)Lua_ValueToNumber(L, 1);
    dwX = (DWORD)Lua_ValueToNumber(L, 2);

    pBook = m_ItemList.GetItem(dwBox, dwX);
    KGLOG_PROCESS_ERROR(pBook);

    KGLOG_PROCESS_ERROR(pBook->m_Common.nGenre == igBook);
    
    bRetCode = RecipeID2BookID((DWORD)pBook->m_nCurrentDurability, dwBookID, dwSubID);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (g_pGameWorldUIHandler)
    {
        KUIEventOpenBook UIParam;
        UIParam.dwBookID = dwBookID;
        UIParam.dwSubID = dwSubID;
        UIParam.dwItemID = pBook->m_dwID;
        UIParam.dwRecipeID = pBook->m_nCurrentDurability; // 道具的耐久用来描述具体是哪本书
        g_pGameWorldUIHandler->OnOpenBook(UIParam);
    }

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}
#endif 

int KPlayer::LuaCanLearnProfession(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL	bRetCode		= false;
	int		nTopIndex		= 0;
	DWORD	dwProfessionID	= 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ProfessionList.CanLearnProfession(dwProfessionID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaCanLearnBranch(Lua_State* L)
{
	Lua_PushBoolean(L, true);
	return 1;
}

int KPlayer::LuaCanLearnRecipe(Lua_State* L)
{
    BOOL    bResult     = false;
	BOOL	bRetCode	= false;
	int		nTopIndex	= 0;
	DWORD	dwCraftID	= 0;
	DWORD	dwRecipeID	= 0;
	DWORD	dwMasterID	= 0;
	KRecipeMaster*		pRecipeMaster	= NULL;
	KRecipeLearningInfo* pLearnInfo		= NULL;
	
	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 3);

	dwCraftID = (DWORD)Lua_ValueToNumber(L, 1);
	dwRecipeID = (DWORD)Lua_ValueToNumber(L, 2);
	dwMasterID = (DWORD)Lua_ValueToNumber(L, 3);

	pRecipeMaster = g_pSO3World->m_ProfessionManager.GetRecipeMaster(dwMasterID);
	KG_PROCESS_ERROR(pRecipeMaster);

	pLearnInfo = pRecipeMaster->GetLearnInfo(dwCraftID, dwRecipeID);
	KG_PROCESS_ERROR(pLearnInfo);

	bRetCode = m_RecipeList.CanLearnRecipe(dwCraftID, dwRecipeID, pLearnInfo);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

BOOL KPlayer::LuaIsProfessionLearnedByCraftID(Lua_State* L)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    DWORD   dwCraftID   = ERROR_ID;
    KCraft* pCraft      = NULL;
    int     nTopIndex   = Lua_GetTopIndex(L);

    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    dwCraftID = (DWORD)Lua_ValueToNumber(L, 1);

    pCraft = g_pSO3World->m_ProfessionManager.GetCraft(dwCraftID);
    KGLOG_PROCESS_ERROR(pCraft);

    bRetCode = m_ProfessionList.IsProfessionLearned(pCraft->m_dwProfessionID);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int	KPlayer::LuaIsRecipeLearned(Lua_State* L)
{
    BOOL    bResult     = false;
	BOOL	bRetCode	= false;
	int		nTopIndex	= 0;
	DWORD	dwCraftID	= 0;
	DWORD	dwRecipeID	= 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2);

	dwCraftID = (DWORD)Lua_ValueToNumber(L, 1);
	dwRecipeID = (DWORD)Lua_ValueToNumber(L, 2);

	bRetCode = m_RecipeList.IsRecipeLearned(dwCraftID, dwRecipeID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaGetSchoolList(Lua_State* L)
{
	assert(L);
	return m_SkillList._LuaGetSkillSchoolList(L);
}

int KPlayer::LuaGetKungfuList(Lua_State* L)
{
	assert(L);
	DWORD dwSchoolID = 0;
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwSchoolID = (DWORD)(Lua_ValueToNumber(L, 1));
	return m_SkillList._LuaGetSkillKungfuList(L, dwSchoolID);
Exit0:
	return 0;
}

int KPlayer::LuaGetSkillList(Lua_State* L)
{
	assert(L);
	DWORD dwKungfuID = 0;
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwKungfuID = (DWORD)(Lua_ValueToNumber(L, 1));
	return m_SkillList._LuaGetSkillList(L, dwKungfuID);
Exit0:
	return 0;
}

int KPlayer::LuaGetAllSkillList(Lua_State* L)
{
	assert(L);
	return m_SkillList._LuaGetAllSkillList(L);
}

#ifdef _CLIENT
int KPlayer::LuaGetSkillRecipeKey(Lua_State* L)
{
    int                 nResult         = 0;
    int                 nRetCode        = 0;
    int                 nTable          = 0;
    DWORD               dwSkillLevel    = 0;
    DWORD               dwSkillID       = 0;
    char                RecipeName[64];
    KSKILL_RECIPE_KEY   SkillRecipeKey;

    assert(L);
    
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwSkillID);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(dwSkillLevel);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    Lua_PushString(L, "skill_id");
    Lua_PushNumber(L, SkillRecipeKey.dwSkillID);
    Lua_SetTable(L, nTable);

    Lua_PushString(L, "skill_level");
    Lua_PushNumber(L, SkillRecipeKey.dwSkillLevel);
    Lua_SetTable(L, nTable);

    for (int i = 0; i < MAX_SKILL_REICPE_COUNT; i++)
    {
        DWORD                       dwRecipeKey         = SkillRecipeKey.dwRecipeKey[i];
        KSKILL_RECIPE_BASE_INFO*    pSkillRecipeInfo    = g_pSO3World->m_SkillManager.GetSkillRecipeBaseInfo(dwRecipeKey);

        if (!pSkillRecipeInfo || pSkillRecipeInfo->bIsSystemRecipe)
            dwRecipeKey = 0;

        snprintf(RecipeName, sizeof(RecipeName), "recipe%d", i + 1);
        RecipeName[sizeof(RecipeName) - 1] = '\0';

        Lua_PushString(L, RecipeName);
        Lua_PushNumber(L, dwRecipeKey);
        Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int KPlayer::LuaGetSkillRecipeKeyCount(Lua_State* L)
{
	int                 nResult         = 0;
	int                 nRetCode        = 0;
	DWORD               dwSkillLevel    = 0;
	DWORD               dwSkillID       = 0;
	KSKILL_RECIPE_KEY   SkillRecipeKey;

	assert(L);

	nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

	dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(dwSkillID);
	dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);
	KGLOG_PROCESS_ERROR(dwSkillLevel);

	nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, dwSkillID, dwSkillLevel, this);
	KGLOG_PROCESS_ERROR(nRetCode);

	for (int i = 0; i < MAX_SKILL_REICPE_COUNT; i++)
	{
        DWORD                       dwRecipeKey         = SkillRecipeKey.dwRecipeKey[i];
        KSKILL_RECIPE_BASE_INFO*    pSkillRecipeInfo    = g_pSO3World->m_SkillManager.GetSkillRecipeBaseInfo(dwRecipeKey);

        if (!pSkillRecipeInfo || pSkillRecipeInfo->bIsSystemRecipe)
            continue;

		nResult++;
	}

Exit0:
	Lua_PushNumber(L, nResult);
	return 1;
}

#endif

int KPlayer::LuaGetProfession(Lua_State* L)
{
    int   nTable    = 0;
	BOOL  bRetCode  = false;
	DWORD dwSize    = 0;
	DWORD dwIndex   = 0;
	std::vector<DWORD> vecProfessionIDList;

	bRetCode = m_ProfessionList.GetProfessionList(vecProfessionIDList);
	KGLOG_PROCESS_ERROR(bRetCode);

	dwSize = (DWORD)vecProfessionIDList.size();

	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);

	for (dwIndex = 0; dwIndex < dwSize; dwIndex++)
	{
		DWORD dwProfessionID	    = vecProfessionIDList[dwIndex];
		DWORD dwMaxLevel		    = m_ProfessionList.GetProfessionMaxLevel(dwProfessionID);
		DWORD dwCurrentLevel	    = m_ProfessionList.GetProfessionLevel(dwProfessionID);
		DWORD dwCurrentProficiency  = m_ProfessionList.GetProficiency(dwProfessionID);
		DWORD dwBranchID		    = m_ProfessionList.GetBranchID(dwProfessionID);

		Lua_PushNumber(L, dwIndex + 1);
		Lua_NewTable(L);
		int nSubTable = Lua_GetTopIndex(L);
		
		Lua_PushString(L, "ProfessionID");
		Lua_PushNumber(L, dwProfessionID);
		Lua_SetTable(L, nSubTable);

		Lua_PushString(L, "MaxLevel");
		Lua_PushNumber(L, dwMaxLevel);
		Lua_SetTable(L, nSubTable);

		Lua_PushString(L, "Level");
		Lua_PushNumber(L, dwCurrentLevel);
		Lua_SetTable(L, nSubTable);

		Lua_PushString(L, "Proficiency");
		Lua_PushNumber(L,dwCurrentProficiency);
		Lua_SetTable(L, nSubTable);

		Lua_PushString(L, "BranchID");
		Lua_PushNumber(L, dwBranchID);
		Lua_SetTable(L, nSubTable);

		Lua_SetTable(L, nTable);
	}

	return 1;
Exit0:
	return 0;
}

int KPlayer::LuaGetProfessionLevel(Lua_State* L)
{
	int     nTopIndex       = 0;
	DWORD   dwProfessionID  = 0;
    DWORD   dwLevel         = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(dwProfessionID);

    dwLevel = m_ProfessionList.GetProfessionLevel(dwProfessionID);
    Lua_PushNumber(L, dwLevel);

    return 1;
Exit0:
	return 0;
}

int KPlayer::LuaGetProfessionMaxLevel(Lua_State* L)
{
    int     nTopIndex       = 0;
    DWORD   dwProfessionID  = 0;
    DWORD   dwMaxLevel      = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwProfessionID);

    dwMaxLevel = m_ProfessionList.GetProfessionMaxLevel(dwProfessionID);
    Lua_PushNumber(L, dwMaxLevel);

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaGetProfessionProficiency(Lua_State* L)
{
    int     nTopIndex       = 0;
    DWORD   dwProfessionID  = 0;
    DWORD   dwProficiency   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwProfessionID);

    dwProficiency = m_ProfessionList.GetProficiency(dwProfessionID);
    Lua_PushNumber(L, dwProficiency);

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaGetProfessionBranch(Lua_State* L)
{
	DWORD dwResult = 0;
	int nTopIndex = 0;
	DWORD dwProfessionID = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(dwProfessionID);

	dwResult = m_ProfessionList.GetBranchID(dwProfessionID);

Exit0:
	Lua_PushNumber(L, dwResult);
	return 1;
}

// 获取一个Profession中所有的Recipe
int KPlayer::LuaGetRecipe(Lua_State* L)
{
	BOOL	bRetCode		= false;
	DWORD	dwProfessionID	= 0;
	DWORD	dwCraftID		= 0;
	DWORD	dwRecipeID		= 0;
	int		nCraftIndex		= 0;
	int		nRecipeIndex	= 0;
	int		nTopIndex		= 0;
	int		nTable			= 0;
	int		nSubTable		= 0;
	int		nIndex			= 1;
	std::vector<DWORD> vecCraftIDList;
	std::vector<DWORD> vecRecipeIDList;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(dwProfessionID);

	bRetCode = g_pSO3World->m_ProfessionManager.GetCraftIDList(dwProfessionID, vecCraftIDList);
	KGLOG_PROCESS_ERROR(bRetCode);

	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);

	for (nCraftIndex = 0; nCraftIndex < (int)vecCraftIDList.size(); nCraftIndex++)
	{
		dwCraftID = vecCraftIDList[nCraftIndex];

		bRetCode = m_RecipeList.GetRecipeList(dwCraftID, vecRecipeIDList);
		if (bRetCode == false)
			continue;

		for (nRecipeIndex = 0; nRecipeIndex < (int)vecRecipeIDList.size(); nRecipeIndex++)
		{
			dwRecipeID = vecRecipeIDList[nRecipeIndex];

			Lua_PushNumber(L, nIndex);
			Lua_NewTable(L);
			nSubTable = Lua_GetTopIndex(L);

			Lua_PushString(L, "CraftID");
			Lua_PushNumber(L, dwCraftID);
			Lua_SetTable(L, nSubTable);

			Lua_PushString(L, "RecipeID");
			Lua_PushNumber(L, dwRecipeID);
			Lua_SetTable(L, nSubTable);

			Lua_SetTable(L, nTable);
			nIndex = nIndex + 1;
		}
	}
	return 1;
Exit0:
	return 0;
}

int KPlayer::LuaForgetProfession(Lua_State* L)
{
	BOOL  bRetCode          = false;
    DWORD dwProfessionID    = 0;
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);

#if defined(_SERVER)
	bRetCode = ForgetProfession(dwProfessionID);
	KGLOG_PROCESS_ERROR(bRetCode);
#endif //_SERVER
Exit0:
	return 0;
}

int KPlayer::LuaCastProfessionSkill(Lua_State* L)
{
	BOOL                bRetCode            = false;
	CRAFT_RESULT_CODE   eResult	            = crcInvalid;
	DWORD               dwRecipeBookPackage	= 0;
	DWORD               dwRecipeBookBox		= 0;
	DWORD               dwCraftID			= 0;
	DWORD               dwRecipeID			= 0;
	TARGET_TYPE	        eTargetType		    = ttInvalid;
	DWORD               dwTargetID			= 0;
	KItem*              pTargetItem			= NULL;
	KTarget             target;
    
	int nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 3 || nTopIndex == 4);
    
    target.ClearTarget();

    if (nTopIndex == 2)
    {
        dwCraftID = (DWORD)Lua_ValueToNumber(L, 1);
        dwRecipeID = (DWORD)Lua_ValueToNumber(L, 2);
    }
    else if (nTopIndex == 3)
    {
        DWORD   dwBookID    = 0;
        DWORD   dwBookSubID = 0;

        dwCraftID   = (DWORD)Lua_ValueToNumber(L, 1);
        dwBookID    = (DWORD)Lua_ValueToNumber(L, 2);
        dwBookSubID = (DWORD)Lua_ValueToNumber(L, 3);
        dwRecipeID  = (((dwBookID - 1) << MAX_READ_BOOK_SUB_ID_BIT) | (dwBookSubID - 1));
    }
    else if (nTopIndex == 4)
	{   
        dwCraftID = (DWORD)Lua_ValueToNumber(L, 1);
        dwRecipeID = (DWORD)Lua_ValueToNumber(L, 2);

		eTargetType = (TARGET_TYPE)(DWORD)Lua_ValueToNumber(L, 3);
		dwTargetID = (DWORD)Lua_ValueToNumber(L, 4);
		
		KGLOG_PROCESS_ERROR(eTargetType != ttInvalid && eTargetType != ttNoTarget);

		bRetCode = target.SetTarget(eTargetType, dwTargetID);
		KGLOG_PROCESS_ERROR(bRetCode);
	}

#ifdef _CLIENT
	eResult = CanCastProfessionSkill(dwCraftID, dwRecipeID, 0, target);
	KG_PROCESS_ERROR(eResult == crcSuccess);

    g_PlayerClient.DoCastProfessionSkill(dwCraftID, dwRecipeID, 0, &target);
#else
    eResult = CastProfessionSkill(dwCraftID, dwRecipeID, 0, target);
	KGLOG_PROCESS_ERROR(eResult == crcSuccess);
#endif
	
	eResult = crcSuccess;
Exit0:
	Lua_ValueToNumber(L, eResult);
	return 1;
}

int KPlayer::LuaGetBookList(Lua_State* L)
{
	BYTE byBookState = 0;
	DWORD dwIndex = 0;
	int nTable = 0;
	int nCount = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);

	for (dwIndex = 1; dwIndex <= MAX_READ_BOOK_ID; dwIndex++)
	{
		byBookState = m_BookList.GetBookState(dwIndex);
		if (byBookState)
		{
			nCount++;
			Lua_PushNumber(L, nCount);
			Lua_PushNumber(L, dwIndex);
			Lua_SetTable(L, nTable);
		}
	}
	return 1;
Exit0:
	return 0;
}

int KPlayer::LuaGetBookSegmentList(Lua_State* L)
{
	BOOL bRetCode = false;
	DWORD dwBookID = 0;
	DWORD dwIndex = 0;
	int nTable = 0;
	int nCount = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwBookID = (DWORD)Lua_ValueToNumber(L, 1);

	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);

	for (dwIndex = 1; dwIndex <= MAX_READ_BOOK_SUB_ID; dwIndex++)
	{
		bRetCode = m_BookList.IsBookMemorized(dwBookID, dwIndex);
		if (bRetCode)
		{
			nCount++;
			Lua_PushNumber(L, nCount);
			Lua_PushNumber(L, dwIndex);
			Lua_SetTable(L, nTable);
		}
	}
	return 1;
Exit0:
	return 0;
}

int KPlayer::LuaIsBookMemorized(Lua_State* L)
{
    BOOL  bResult  = false;
    BOOL  bRetCode = false;
    DWORD dwBookID = 0;
    DWORD dwIndex  = 0;
    
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

    dwBookID = (DWORD)Lua_ValueToNumber(L, 1);
    dwIndex  = (DWORD)Lua_ValueToNumber(L, 2);

    bRetCode = m_BookList.IsBookMemorized(dwBookID, dwIndex);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaCanLearnSkill(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nRetCode    = 0;
    int     nParamCount = 0;
    DWORD   dwSkillID   = INVALID_SKILL_ID;
    DWORD   dwMasterID  = 0;

    nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount == 2);

    dwSkillID   = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);

    dwMasterID  = (DWORD)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(dwMasterID);

    nRetCode = m_SkillList.CanLearnSkill(dwSkillID, dwMasterID);
    KG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaLearnSkill(Lua_State* L)
{
	int     nRetCode        = 0;
    int     nParamCount     = 0;
    DWORD   dwSkillID       = INVALID_SKILL_ID;
    DWORD   dwNpcID         = ERROR_ID;
    BOOL    bNotifyPlayer   = true;

    nParamCount = Lua_GetTopIndex(L);

#ifdef _SERVER
	KGLOG_PROCESS_ERROR(nParamCount == 1 || nParamCount == 2);
#endif

#ifdef _CLIENT
	KGLOG_PROCESS_ERROR(nParamCount == 2);
#endif

	dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);

#ifdef _CLIENT
    dwNpcID = (DWORD)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(dwNpcID != ERROR_ID);
#endif

#ifdef _SERVER 
    if (nParamCount == 2)
    {
        bNotifyPlayer = Lua_ValueToBoolean(L, 2);
    }

    nRetCode = m_SkillList.LearnSkill(dwSkillID, bNotifyPlayer);
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

#ifdef _CLIENT
    nRetCode = g_PlayerClient.DoPlayerLearnSkill(dwSkillID, dwNpcID);
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
	return 0;
}


#if defined(_SERVER)

int KPlayer::LuaAddSkillExp(Lua_State* L)
{
	BOOL    bRetCode    = false;
    DWORD   dwSkillID   = 0;
    DWORD   dwAddExp    = 0;

	//检测参数个数
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	//获取输入参数
	dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);
    dwAddExp  = (DWORD)Lua_ValueToNumber(L, 2);

    bRetCode = m_SkillList.AddSkillExp(dwSkillID, dwAddExp);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
	return 0;
}

int KPlayer::LuaForgetSkill(Lua_State* L)
{
	int     nRetCode    = false;
    DWORD   dwSkillID   = INVALID_SKILL_ID;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwSkillID != INVALID_SKILL_ID);

    nRetCode = m_SkillList.ForgetSkill(dwSkillID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}

#endif	//_SERVER

//检测当前玩家是否可以接受指定任务
#if defined(_SERVER)
int KPlayer::LuaCanAcceptQuest(Lua_State* L)
{
	BOOL        bResult     = false;
    BOOL        bRetCode    = false;
	DWORD       dwQuestID   = 0;
    TARGET_TYPE eTargetType = ttInvalid;
	DWORD       dwTargetID  = ERROR_ID;
    KQuestInfo* pQuestInfo  = NULL;
    KTarget     Target;

	//检测参数个数
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	//获取输入参数
	eTargetType = (TARGET_TYPE)(int)Lua_ValueToNumber(L, 1);
	dwTargetID  = (DWORD)Lua_ValueToNumber(L, 2);
	dwQuestID   = (DWORD)Lua_ValueToNumber(L, 3);

    bRetCode = Target.SetTarget(eTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(bRetCode);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR(pQuestInfo);

	bResult = m_QuestList.CanAccept(pQuestInfo, &Target);

Exit0:

	//返回值压栈
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif	//_SERVER

#if defined(_CLIENT)
int KPlayer::LuaCanAcceptQuest(Lua_State* L)
{
	BOOL bRetCode = false;
	DWORD dwQuestID = 0;
	TARGET_TYPE eTargetType = ttInvalid;
	DWORD dwTargetID = 0;
	QUEST_RESULT_CODE nResult = qrcInvalid;
	KTarget Target;

	//检测参数个数
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	//获取输入参数
	dwQuestID   =	(DWORD)Lua_ValueToNumber(L, 1);
	eTargetType =	(TARGET_TYPE)(int)Lua_ValueToNumber(L, 2);
	dwTargetID  =	(DWORD)Lua_ValueToNumber(L, 3);

	KQuestInfo* pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KGLOG_PROCESS_ERROR(pQuestInfo);

	bRetCode = Target.SetTarget(eTargetType, dwTargetID);
	KGLOG_PROCESS_ERROR_RET_CODE(bRetCode, qrcFailed);

	nResult = m_QuestList.CanAccept(pQuestInfo, &Target);

Exit0:

	//返回值压栈
	Lua_PushNumber(L, nResult);
	return 1;
}
#endif	//_CLIENT

int KPlayer::LuaCanFinishQuest(Lua_State* L)
{
	QUEST_RESULT_CODE   nResult     = qrcInvalid;
    QUEST_RESULT_CODE   eRetCode    = qrcInvalid;
    BOOL                bRetCode    = false;
    int                 nTopIndex   = 0;
	DWORD               dwQuestID   = 0;
	KQuestInfo*         pQuestInfo  = NULL;
    
    nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1 || nTopIndex == 3);
    
    dwQuestID   = (DWORD)Lua_ValueToNumber(L, 1);

    pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);

    if (nTopIndex == 1)
    {
        eRetCode = m_QuestList.CanFinish(pQuestInfo);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
    }
    else
    {
        TARGET_TYPE         eTargetType = ttInvalid;
        DWORD               dwTargetID  = 0;
        KTarget             Target;

        eTargetType = (TARGET_TYPE)(int)Lua_ValueToNumber(L, 2);
        dwTargetID  = (DWORD)Lua_ValueToNumber(L, 3);

		bRetCode = Target.SetTarget(eTargetType, dwTargetID);
		KGLOG_PROCESS_ERROR_RET_CODE(bRetCode, qrcFailed);

        eRetCode = m_QuestList.CanFinish(pQuestInfo, &Target);
        KG_PROCESS_ERROR_RET_CODE(eRetCode == qrcSuccess, eRetCode);
    }

    nResult = qrcSuccess;
Exit0:
	Lua_PushNumber(L, nResult);
	return 1;
}

int KPlayer::LuaGetQuestState(Lua_State* L)
{
	DWORD       dwQuestID   = 0;
	QUEST_STATE	eQuestState = qsInvalid;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
	eQuestState = m_QuestList.GetQuestState(dwQuestID);
Exit0:
	Lua_PushNumber(L, eQuestState);
	return 1;
}

int KPlayer::LuaGetQuestPhase(lua_State *L)
{
	int     nResult     = -1;
	DWORD   dwQuestID   = 0;
    
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
	nResult = m_QuestList.GetQuestPhase(dwQuestID);
Exit0:
	Lua_PushNumber(L, nResult);
	return 1;
}

//接受任务
int KPlayer::LuaAcceptQuest(Lua_State* L)
{
	BOOL        bRetCode        = false;
	int         nTopIndex       = 0;
    DWORD       dwQuestID       = 0;
	BOOL        bIgnoreRules    = false;
    TARGET_TYPE eTargetType     = ttInvalid;
    DWORD       dwTargetID      = ERROR_ID;
    KTarget     Target;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 3 || nTopIndex == 4);

	eTargetType = (TARGET_TYPE)(int)Lua_ValueToNumber(L, 1);
	dwTargetID  = (DWORD)Lua_ValueToNumber(L, 2);
	dwQuestID   = (DWORD)Lua_ValueToNumber(L, 3);

    bRetCode = Target.SetTarget(eTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(bRetCode);

	if (nTopIndex == 4)
		bIgnoreRules = (BOOL)Lua_ValueToNumber(L, 4);

#if defined(_SERVER)
	bRetCode = m_QuestList.Accept(&Target, dwQuestID, bIgnoreRules);
	KGLOG_PROCESS_ERROR(bRetCode);
#endif	//_SERVER

#if defined(_CLIENT)
	g_PlayerClient.DoAcceptQuest(&Target, dwQuestID);
#endif	//_CLIENT

Exit0:
	return 0;
}

int KPlayer::LuaFinishQuest(Lua_State* L)
{
	BOOL    bRetCode        = false;
    DWORD   dwQuestID       = 0;
    int     nTargetType     = 0;
    DWORD   dwTargetID      = 0;
    int     nPresentChoice1 = 0;
    int     nPresentChoice2 = 0;
	KTarget rTarget;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

	dwQuestID       = (DWORD)Lua_ValueToNumber(L, 1);
	nTargetType     = (int)Lua_ValueToNumber(L, 2);
	dwTargetID      = (DWORD)Lua_ValueToNumber(L, 3);
	nPresentChoice1 = (int)Lua_ValueToNumber(L, 4);
	nPresentChoice2 = (int)Lua_ValueToNumber(L, 5);
    
    KGLOG_PROCESS_ERROR(nPresentChoice1 >= 0 && nPresentChoice1 < QUEST_PARAM_COUNT);
    KGLOG_PROCESS_ERROR(nPresentChoice2 >= QUEST_PARAM_COUNT && nPresentChoice2 < QUEST_PARAM_COUNT * 2);

    bRetCode = rTarget.SetTarget((TARGET_TYPE)nTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(bRetCode);

	KGLOG_PROCESS_ERROR(nTargetType == ttDoodad || nTargetType == ttNpc);

#ifdef _SERVER
	bRetCode = m_QuestList.Finish(dwQuestID, rTarget, nPresentChoice1, nPresentChoice2);
	KGLOG_PROCESS_ERROR(bRetCode);
#else
    g_PlayerClient.DoFinishQuest(dwQuestID, rTarget, nPresentChoice1, nPresentChoice2);
#endif

Exit0:
	return 0;
}

int KPlayer::LuaCancelQuest(Lua_State* L)
{
	BOOL bRetCode    = false;
    int  nQuestIndex = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);

#ifdef _SERVER
	bRetCode = m_QuestList.Cancel(nQuestIndex);
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

#ifdef _CLIENT
	g_PlayerClient.DoCancelQuest(nQuestIndex);
#endif

Exit0:
	return 0;
}

int KPlayer::LuaClearQuest(Lua_State* L)
{
	int     nRetCode    = false;
    DWORD   dwQuestID   = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);

#ifdef _SERVER
	nRetCode = m_QuestList.ClearQuest(dwQuestID);
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
    return 0;
}

int KPlayer::LuaShareQuest(Lua_State* L)
{
	return 1;
}

int KPlayer::LuaGetFinishedDailyQuestCount(Lua_State* L)
{
    int nResult = 0;

    nResult = m_QuestList.GetFinishedDailyQuestCount();

    Lua_PushNumber(L, nResult);

    return 1;
}

#ifdef _SERVER
int KPlayer::LuaForceFinishQuest(Lua_State* L)
{
    BOOL    bResult   = false;
	BOOL    bRetCode  = false;
    DWORD   dwQuestID = 0;
	KTarget target;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_QuestList.Finish(dwQuestID, target, 0, 4, true); //target本身没有用到,所以不用初始化
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif

#ifdef _CLIENT
int KPlayer::LuaAcceptEscortQuest(Lua_State* L)
{
    BOOL        bResult         = false;
	BOOL        bRetCode        = false;
	KQuestInfo* pQuestInfo      = NULL;
    DWORD       dwStartPlayerID = ERROR_ID;
    DWORD       dwQuestID       = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwStartPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
	dwQuestID       = (DWORD)Lua_ValueToNumber(L, 2);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KG_PROCESS_ERROR(pQuestInfo);
    
    KGLOG_PROCESS_ERROR(pQuestInfo->bEscortQuest);

	g_PlayerClient.DoAcceptEscortQuest(dwStartPlayerID, dwQuestID);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif //_CLIENT

#ifdef _SERVER
int KPlayer::LuaRegisterQuestAssistance(Lua_State* L)
{
    int     nRetCode    = false;
    DWORD   dwQuestID   = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);

    nRetCode = m_QuestList.RegisterPartyAssistance(dwQuestID);
    KG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}
#endif

int KPlayer::LuaGetQuestList(Lua_State *L)
{
	int nQuestCount = 0;
	int nQuestID    = 0;
    int nTable      = 0;
	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);

	for (int nIndex = 0; nIndex < MAX_ACCEPT_QUEST_COUNT; nIndex++)
	{
		nQuestID = m_QuestList.GetQuestID(nIndex);
		if (nQuestID)
		{
			Lua_PushNumber(L, ++nQuestCount);
			Lua_PushNumber(L, nQuestID);
			Lua_SetTable(L, nTable);
		}
	}

	Lua_PushNumber(L, nQuestCount);
	return 2;
}

//获取任务列表
int KPlayer::LuaGetQuestTree(Lua_State* L)
{
	std::map<std::string, std::vector<int> > QuestClassMap;
	for (int nQuestIndex = 0; nQuestIndex < MAX_ACCEPT_QUEST_COUNT; nQuestIndex++)
	{
		DWORD dwQuestID = m_QuestList.GetQuestID(nQuestIndex);
		if (dwQuestID)
		{
			KQuestInfo* pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
			if (pQuestInfo)
				QuestClassMap[pQuestInfo->szQuestClass].push_back(nQuestIndex);
		}
	}
	
	Lua_NewTable(L);
	for (std::map<std::string, std::vector<int> >::iterator it = QuestClassMap.begin(); it != QuestClassMap.end(); ++it)
	{
		std::vector<int> &raQuestIndex = (*it).second;
		Lua_PushString(L, (*it).first.c_str());
		Lua_NewTable(L);
		
		int nIndex = 1;
		for (std::vector<int>::iterator it = raQuestIndex.begin(); it != raQuestIndex.end(); ++it)
		{
			Lua_PushNumber(L, nIndex);
			Lua_PushNumber(L, *(it));
			Lua_SetTable(L, -3);
			nIndex++;
		}
		Lua_SetTable(L, -3);
	}

	return 1;
}

int KPlayer::LuaGetQuestID(Lua_State* L)
{
	int nQuestIndex = 0;
    DWORD dwQuestID = 0;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);

	dwQuestID = m_QuestList.GetQuestID(nQuestIndex);
	Lua_PushNumber(L, dwQuestID);

	return 1;

Exit0:

	return 0;
}

int KPlayer::LuaGetQuestExpAttenuation(Lua_State* L)
{
	DWORD dwQuestID = 0;
	KQuestInfo* pQuestInfo = NULL;
	int nExp = 0;
	int nRepeatCutPercent = 100;
	int nLevelCutPercent = 100;
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
	
	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KG_PROCESS_ERROR(pQuestInfo);

	m_QuestList.GetQuestExpAttenuation(pQuestInfo, &nExp, &nRepeatCutPercent, &nLevelCutPercent);

	Lua_PushNumber(L, nExp);
	Lua_PushNumber(L, nRepeatCutPercent);
	Lua_PushNumber(L, nLevelCutPercent);
	return 3;
Exit0:
	return 0;
}

int KPlayer::LuaGetQuestIndex(Lua_State *L)
{
	DWORD dwQuestID   = 0;
    int   nQuestIndex = 0;
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);

	nQuestIndex = m_QuestList.GetQuestIndex(dwQuestID);
	KG_PROCESS_ERROR(nQuestIndex >= 0);

	Lua_PushNumber(L, nQuestIndex);

	return 1;
Exit0:
	return 0;
}

#ifdef _CLIENT
int KPlayer::LuaGetQuestTraceInfo(Lua_State* L)
{
	DWORD dwQuestID = 0;
	int nQuestIndex = 0;
	int nIndex = 1;
	BOOL bHave = FALSE;
	KQuestInfo* pQuestInfo = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
	nQuestIndex = m_QuestList.GetQuestIndex(dwQuestID);
	KG_PROCESS_ERROR(nQuestIndex >= 0);

	pQuestInfo = g_pSO3World->m_Settings.m_QuestInfoList.GetQuestInfo(dwQuestID);
	KG_PROCESS_ERROR(pQuestInfo);

	Lua_NewTable(L);

	Lua_PushString(L, "name");
	Lua_PushString(L, pQuestInfo->szName);
	Lua_SetTable(L, -3);
	
	Lua_PushString(L, "finish");
	Lua_PushBoolean(L, m_QuestList.CanFinish(pQuestInfo) == qrcSuccess);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "fail");
	Lua_PushBoolean(L, m_QuestList.GetQuestFailedFlag(nQuestIndex));
	Lua_SetTable(L, -3);

	Lua_PushString(L, "diff");
	Lua_PushString(L, pQuestInfo->szQuestDiff);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "assist");
	Lua_PushBoolean(L, pQuestInfo->bAssist);
	Lua_SetTable(L, -3);

	if (pQuestInfo->nFinishTime)
	{
		int nLastTime = m_QuestList.GetLastTime(nQuestIndex);
		if (m_QuestList.GetQuestFailedFlag(nQuestIndex))
			nLastTime = 0;
		Lua_PushString(L, "time");
		Lua_PushNumber(L, nLastTime);
		Lua_SetTable(L, -3);
	}


	Lua_PushString(L, "quest_state");
	Lua_NewTable(L);
	nIndex = 1;
	for (int i = 0; i < QUEST_PARAM_COUNT * 2; ++i)
	{
		if (pQuestInfo->nQuestValue[i])
		{
			Lua_PushNumber(L, nIndex);
			Lua_NewTable(L);

			Lua_PushString(L, "i");
			Lua_PushNumber(L, i);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "need");
			Lua_PushNumber(L, pQuestInfo->nQuestValue[i]);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "have");
			Lua_PushNumber(L, m_QuestList.GetQuestValue(nQuestIndex, i));
			Lua_SetTable(L, -3);

			Lua_PushString(L, "name");
			Lua_PushString(L, pQuestInfo->szQuestValueStr[i]);
			Lua_SetTable(L, -3);

			Lua_SetTable(L, -3);
			++nIndex;
			bHave = TRUE;
		}
	}
	Lua_SetTable(L, -3);

	Lua_PushString(L, "kill_npc");
	Lua_NewTable(L);
	nIndex = 1;
	for (int i = 0; i < QUEST_PARAM_COUNT; ++i)
	{
		if (pQuestInfo->dwKillNpcTemplateID[i])
		{
			Lua_PushNumber(L, nIndex);
			Lua_NewTable(L);
			
			Lua_PushString(L, "i");
			Lua_PushNumber(L, i);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "need");
			Lua_PushNumber(L, pQuestInfo->dwKillNpcAmount[i]);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "have");
			Lua_PushNumber(L, m_QuestList.GetKillNpcCount(nQuestIndex, i));
			Lua_SetTable(L, -3);

			Lua_PushString(L, "name");
			KNpcTemplate* pNpcTemplate = g_pSO3World->m_Settings.m_NpcTemplateList.GetTemplate(pQuestInfo->dwKillNpcTemplateID[i]);
			if (pNpcTemplate)
				Lua_PushString(L, pNpcTemplate->szName);
			else
				Lua_PushString(L, "Unknown Npc");
			Lua_SetTable(L, -3);

			Lua_SetTable(L, -3);
			++nIndex;
			bHave = TRUE;
		}
	}
	Lua_SetTable(L, -3);

	Lua_PushString(L, "need_item");
	Lua_NewTable(L);
	nIndex = 1;
	for (int i = 0; i < QUEST_END_ITEM_COUNT; ++i)
	{
		if (pQuestInfo->dwEndRequireItemType[i] && pQuestInfo->dwEndRequireItemIndex[i])
		{
			BOOL bHaveItem = FALSE;
			for (int j = 0; j < i; ++j)
			{
				if (pQuestInfo->dwEndRequireItemType[i] == pQuestInfo->dwEndRequireItemType[i] && 
					pQuestInfo->dwEndRequireItemIndex[i] == pQuestInfo->dwEndRequireItemIndex[j])
				{
					bHaveItem = TRUE;
					break;
				}
			}
			if (!bHaveItem)
			{
				Lua_PushNumber(L, nIndex);
				Lua_NewTable(L);

				Lua_PushString(L, "i");
				Lua_PushNumber(L, i);
				Lua_SetTable(L, -3);

				Lua_PushString(L, "need");
				Lua_PushNumber(L, pQuestInfo->dwEndRequireItemAmount[i]);
				Lua_SetTable(L, -3);

				Lua_PushString(L, "have");
				Lua_PushNumber(L, m_ItemList.GetItemTotalAmount(pQuestInfo->dwEndRequireItemType[i], pQuestInfo->dwEndRequireItemIndex[i]));
				Lua_SetTable(L, -3);

				Lua_PushString(L, "type");
				Lua_PushNumber(L, pQuestInfo->dwEndRequireItemType[i]);
				Lua_SetTable(L, -3);

				Lua_PushString(L, "index");
				Lua_PushNumber(L, pQuestInfo->dwEndRequireItemIndex[i]);
				Lua_SetTable(L, -3);

				Lua_SetTable(L, -3);
				++nIndex;
				bHave = TRUE;
			}
		}
	}
	Lua_SetTable(L, -3);

	if (!bHave)
	{
		Lua_PushString(L, "objective");
		Lua_PushString(L, pQuestInfo->szObjective);
		Lua_SetTable(L, -3);
	}
	Lua_PushString(L, "have_trace");
	Lua_PushBoolean(L, bHave);
	Lua_SetTable(L, -3);

	return 1;
Exit0:
	return 0;

}

int KPlayer::LuaGetQuestAssistDailyCount(Lua_State* L)
{
    int             nDailyCount     = 0;
    int             nMaxAssistTimes = 0;
    KLevelUpData*   pLevelUpData    = NULL;

    pLevelUpData = g_pSO3World->m_Settings.m_LevelUpList.GetLevelUpData(m_eRoleType, m_nLevel);
    KGLOG_PROCESS_ERROR(pLevelUpData);

    nDailyCount = m_QuestList.GetAssistDailyCount();
    nMaxAssistTimes = pLevelUpData->nMaxAssistTimes;

Exit0:
    Lua_PushNumber(L, nDailyCount);
    Lua_PushNumber(L, nMaxAssistTimes);
    return 2;
}
#endif

int KPlayer::LuaSetQuestValue(Lua_State* L)
{
	int nRetCode    = 0;
	int nQuestIndex = 0;
	int nValueIndex = 0;
	int nNewValue   = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 3);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);
	nValueIndex = (int)Lua_ValueToNumber(L, 2);
	nNewValue   = (int)Lua_ValueToNumber(L, 3);

	nRetCode = m_QuestList.SetQuestValue(nQuestIndex, nValueIndex, nNewValue);
	KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}

int KPlayer::LuaGetQuestValue(Lua_State* L)
{
    int nResult     = 0;
    int nRetCode    = 0;
	int nQuestIndex = 0;
	int nValueIndex = 0;
    int nValue      = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

    nQuestIndex = (int)Lua_ValueToNumber(L, 1);
    nValueIndex = (int)Lua_ValueToNumber(L, 2);

    nValue = m_QuestList.GetQuestValue(nQuestIndex, nValueIndex);
	Lua_PushNumber(L, nValue);

	nResult = 1;
Exit0:
	return nResult;
}

int KPlayer::LuaSetQuestBitValue(Lua_State* L)
{
	int     nRetCode    = 0;
	int     nQuestIndex = 0;
	int     nValueIndex = 0;
	DWORD   dwBitIndex  = 0;
    DWORD   dwValue     = 0;
    BOOL    bValue      = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 4);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);
	nValueIndex = (int)Lua_ValueToNumber(L, 2);
    dwBitIndex  = (DWORD)Lua_ValueToNumber(L, 3);
    bValue      = (int)Lua_ValueToBoolean(L, 4);

    dwValue = (DWORD)m_QuestList.GetQuestValue(nQuestIndex, nValueIndex);

    if (bValue)
    {
        dwValue |= (1 << dwBitIndex);
    }
    else
    {
        dwValue &= ~(1 << dwBitIndex);
    }

    m_QuestList.SetQuestValue(nQuestIndex, nValueIndex, (int)dwValue);

Exit0:
	return 0;
}

int KPlayer::LuaGetQuestBitValue(Lua_State* L)
{
    int     nResult     = 0;
	int     nRetCode    = 0;
	int     nQuestIndex = 0;
	int     nValueIndex = 0;
	DWORD   dwBitIndex  = 0;
    DWORD   dwValue     = 0;
    BOOL    bValue      = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 3);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);
	nValueIndex = (int)Lua_ValueToNumber(L, 2);
    dwBitIndex  = (DWORD)Lua_ValueToNumber(L, 3);

    dwValue = (DWORD)m_QuestList.GetQuestValue(nQuestIndex, nValueIndex);

    bValue = (BOOL)((dwValue >> dwBitIndex) & 0x01);
    
	nResult = 1;
Exit0:
	Lua_PushBoolean(L, bValue);
	return nResult;
}

int KPlayer::LuaGetKillNpcCount(Lua_State* L)
{
    int     nResult     = 0;
    int     nRetCode    = 0;
	int     nQuestIndex = 0;
	int     nCountIndex = 0;
    DWORD   dwCount     = 0;


    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 2);

    nQuestIndex = (int)Lua_ValueToNumber(L, 1);
    nCountIndex = (int)Lua_ValueToNumber(L, 2);

    dwCount = m_QuestList.GetKillNpcCount(nQuestIndex, nCountIndex);
	Lua_PushNumber(L, dwCount);

    nResult = 1;
Exit0:
	return nResult;
}


int KPlayer::LuaSetKillNpcCount(Lua_State *L)
{
	BOOL bRetCode    = false;
    int  nQuestIndex = 0;
    int  nCountIndex = 0;
    int  nNewCount   = 0 ;
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 3);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);
	nCountIndex = (int)Lua_ValueToNumber(L, 2);
	nNewCount = (int)Lua_ValueToNumber(L, 3);

	bRetCode = m_QuestList.SetKillNpcCount(nQuestIndex, nCountIndex, nNewCount);
	KGLOG_PROCESS_ERROR(bRetCode);

	return 1;
Exit0:
	return 0;
}

int KPlayer::LuaGetQuestLastTime(Lua_State* L)
{
	int nQuestIndex = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);

	Lua_PushNumber(L, m_QuestList.GetLastTime(nQuestIndex));

	return 1;
Exit0:
	return 0;
}

int KPlayer::LuaSetQuestFailedFlag(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
	int     nQuestIndex     = 0;
	BOOL    bFailedFlag     = false;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);
	bFailedFlag = (BOOL)Lua_ValueToBoolean(L, 2);

	bRetCode = m_QuestList.SetQuestFailedFlag(nQuestIndex, bFailedFlag);
	KGLOG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaGetQuestFailedFlag(Lua_State* L)
{
	BOOL    bResult    = false;
	int     nQuestIndex = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	nQuestIndex = (int)Lua_ValueToNumber(L, 1);

	bResult = m_QuestList.GetQuestFailedFlag(nQuestIndex);
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

#ifdef _SERVER
int KPlayer::LuaLearnProfession(Lua_State* L)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
	DWORD   dwProfessionID  = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwProfessionID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = LearnProfession(dwProfessionID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaLearnBranch(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
	DWORD   dwProfessionID  = 0;
	DWORD   dwBranchID      = 0;
	int     nTopIndex       = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nTopIndex == 2);

	dwProfessionID  = (DWORD)Lua_ValueToNumber(L, 1);
	dwBranchID      = (DWORD)Lua_ValueToNumber(L, 2);

	bRetCode = LearnBranch(dwProfessionID, dwBranchID);
	KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaAddProfessionProficiency(Lua_State* L)
{
    BOOL  bResult           = false;
	BOOL  bRetCode          = false;
    DWORD dwProfessionID    = 0;
    DWORD dwExp             = 0;
	
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwProfessionID	= (DWORD)Lua_ValueToNumber(L, 1);
	dwExp			= (DWORD)Lua_ValueToNumber(L, 2);
    
	bRetCode = AddProfessionProficiency(dwProfessionID, dwExp);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaSetMaxProfessionLevel(Lua_State* L)
{
    BOOL  bResult        = false;
	BOOL  bRetCode       = false;
    DWORD dwProfessionID = 0;
    DWORD dwMaxLevel     = 0; 

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	dwProfessionID	= (DWORD)Lua_ValueToNumber(L, 1);
	dwMaxLevel		= (DWORD)Lua_ValueToNumber(L, 2);

	bRetCode = SetMaxProfessionLevel(dwProfessionID, dwMaxLevel);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaLearnRecipe(Lua_State* L)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
	DWORD   dwCraftID   = 0;
	DWORD   dwRecipeID  = 0;
	int     nTopIndex   = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2);

	dwCraftID	= (DWORD)Lua_ValueToNumber(L, 1);
	dwRecipeID	= (DWORD)Lua_ValueToNumber(L, 2);

	bRetCode = LearnRecipe(dwCraftID, dwRecipeID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif //_SERVER

#ifdef _CLIENT
int KPlayer::LuaLearnRecipe(Lua_State* L)
{
    BOOL    bResult     = false;
	DWORD   dwCraftID   = 0;
	DWORD   dwRecipeID  = 0;
	DWORD   dwNpcID     = 0;
	int     nTopIndex   = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 3);

	dwCraftID	= (DWORD)Lua_ValueToNumber(L, 1);
	dwRecipeID	= (DWORD)Lua_ValueToNumber(L, 2);
	dwNpcID		= (DWORD)Lua_ValueToNumber(L, 3);

	g_PlayerClient.DoPlayerLearnRecipe(dwCraftID, dwRecipeID, dwNpcID);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif

int KPlayer::LuaCanExchange(Lua_State* L)
{
	ITEM_RESULT_CODE    nResult         = ircFailed;
    DWORD               dwSrcBox        = 0;
    DWORD               dwSrcX          = 0;
    DWORD               dwDestBox       = 0;
    DWORD               dwDestX         = 0;
    
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 4);

	dwSrcBox   = (DWORD)Lua_ValueToNumber(L, 1);
	dwSrcX     = (DWORD)Lua_ValueToNumber(L, 2);
	dwDestBox  = (DWORD)Lua_ValueToNumber(L, 3);
	dwDestX    = (DWORD)Lua_ValueToNumber(L, 4);

	nResult = m_ItemList.CanExchangeItem(dwSrcBox, dwSrcX, dwDestBox, dwDestX);
    KG_PROCESS_ERROR_RET_CODE(nResult == ircSuccess, nResult);
    
    nResult = ircSuccess;
Exit0:
	Lua_PushNumber(L, nResult);
	return 1;
}

int KPlayer::LuaGetItemAmountInAllPackages(Lua_State* L)
{
    DWORD dwResult  = 0;
    DWORD dwTabType = 0;
    DWORD dwIndex   = 0;
    
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

    dwTabType = (DWORD)Lua_ValueToNumber(L, 1);
    dwIndex   = (DWORD)Lua_ValueToNumber(L, 2);

    dwResult = m_ItemList.GetItemTotalAmount(dwTabType, dwIndex);
Exit0:
    Lua_PushNumber(L, dwResult);
    return 1;
}

int KPlayer::LuaGetItemAmount(Lua_State* L)
{
	DWORD dwResult      = 0;
    DWORD dwTabType     = 0;
    DWORD dwIndex       = 0;
    DWORD dwBookID      = 0;
    DWORD dwSubID       = 0;
    int   nBookRecipeID = -1;
    int   nTopIndex     = Lua_GetTopIndex(L);
    
	KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 4);

	dwTabType   = (DWORD)Lua_ValueToNumber(L, 1);
	dwIndex     = (DWORD)Lua_ValueToNumber(L, 2);

    if (nTopIndex == 4)
    {
        dwBookID = (DWORD)Lua_ValueToNumber(L, 3);
        dwSubID  = (DWORD)Lua_ValueToNumber(L, 4);
        nBookRecipeID = BookID2RecipeID(dwBookID, dwSubID);
    }

	dwResult = m_ItemList.GetItemAmountInEquipAndPackage(dwTabType, dwIndex, nBookRecipeID);
Exit0:
	Lua_PushNumber(L, dwResult);
	return 1;
}

int KPlayer::LuaOpenBank(Lua_State* L)
{
	BOOL  bResult   = false;
	KNpc* pNpc      = NULL;
    DWORD dwNpcID   = 0;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwNpcID = (DWORD)Lua_ValueToNumber(L, 1);

	pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
	KG_PROCESS_ERROR(pNpc);

#ifdef _CLIENT
	g_PlayerClient.DoSetBankOpenedFlag(true, pNpc->m_dwID);
	
	m_dwBankNpcID = pNpc->m_dwID;

    if (g_pGameWorldUIHandler)
    {
        KUIEventOpenBank UIParam = { dwNpcID };
	    g_pGameWorldUIHandler->OnOpenBank(UIParam);
    }
#endif
    
    m_ItemList.m_bBankOpened = true;

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaCloseBank(Lua_State* L)
{
	BOOL bResult = false;

	KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

#ifdef _CLIENT
	g_PlayerClient.DoSetBankOpenedFlag(false, ERROR_ID);
    m_dwBankNpcID = ERROR_ID;
#endif

	m_ItemList.m_bBankOpened = false;
	
	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}	

int KPlayer::LuaGetEquipPos(Lua_State* L)
{
    ITEM_RESULT_CODE eRetCode     = ircFailed;
	int              nCanEquipPos = -1;
	int              nParamCount  = Lua_GetTopIndex(L);
	KItem*           pItem        = NULL;
    DWORD            dwBox        = 0;
    DWORD            dwX          = 0;
    
    KGLOG_PROCESS_ERROR(nParamCount == 2);
    
    dwBox = (DWORD)Lua_ValueToNumber(L, 1);
    dwX   = (DWORD)Lua_ValueToNumber(L, 2);

	pItem = m_ItemList.GetItem(dwBox, dwX);
    KGLOG_PROCESS_ERROR(pItem);
    
    KG_PROCESS_ERROR(pItem->m_Common.nGenre == igEquipment);

	eRetCode = m_ItemList.GetCanEquipPos(pItem, &nCanEquipPos);
    KG_PROCESS_ERROR(eRetCode == ircSuccess);

Exit0:
    Lua_PushNumber(L, eRetCode);
	Lua_PushNumber(L, nCanEquipPos);
	return 2;
}

int KPlayer::LuaGetFreeRoom(Lua_State* L)
{
	BOOL    bRetCode        = false;
	DWORD   dwIndex         = 0;
	DWORD   dwBoxIndex      = 0;
    int     nGenerType      = INVALID_CONTAIN_ITEM_TYPE;
    int     nSubType        = INVALID_CONTAIN_ITEM_TYPE;
    int     nBoxGenerType   = INVALID_CONTAIN_ITEM_TYPE;
    int     nBoxSubType     = INVALID_CONTAIN_ITEM_TYPE;
    int     nTopIndex       = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nTopIndex == 1 || nTopIndex == 2);

	dwBoxIndex = (DWORD)Lua_ValueToNumber(L, 1);
    if (nTopIndex == 3)
    {
        nGenerType = (int)Lua_ValueToNumber(L, 2);
        nSubType = (int)Lua_ValueToNumber(L, 3);
    }
    
    if (dwBoxIndex != ibSoldList)
    {
        bRetCode = m_ItemList.GetContainType(dwBoxIndex, &nBoxGenerType, &nBoxSubType);
        KG_PROCESS_ERROR(bRetCode);

        if (nBoxGenerType != INVALID_CONTAIN_ITEM_TYPE)
        {
            KG_PROCESS_ERROR(nBoxGenerType == nGenerType);

            if (nBoxSubType != INVALID_CONTAIN_ITEM_TYPE)
            {
                KG_PROCESS_ERROR(nBoxSubType == nSubType);
            }
        }
    }
	
    bRetCode = m_ItemList.FindFreeRoom(dwBoxIndex, dwIndex);
	KG_PROCESS_ERROR(bRetCode);
    
    Lua_PushNumber(L, dwIndex);

    return 1;
Exit0:
	return 0;
}

int KPlayer::LuaGetFreeRoomSize(Lua_State* L)
{
	int     nResult   = 0;
    int     nTopIndex = Lua_GetTopIndex(L);
    
    KG_PROCESS_ERROR(nTopIndex == 0);

	nResult = m_ItemList.GetAllPackageFreeRoomSize();
Exit0:
	Lua_PushNumber(L, nResult);
	return 1;
}

int KPlayer::LuaGetBoxFreeRoomSize(Lua_State* L)
{
    int     nResult         = 0;
    int     nRetCode        = 0;
    int     nBoxIndex       = 0;
    int     nTopIndex       = Lua_GetTopIndex(L);
    
    KG_PROCESS_ERROR(nTopIndex == 1);

    nBoxIndex = (int)Lua_ValueToNumber(L, 1);

	nRetCode = m_ItemList.GetBoxFreeRoomSize(nBoxIndex);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    
    nResult = nRetCode;
Exit0:
	Lua_PushNumber(L, nResult);
	return 1;
}

int KPlayer::LuaGetFreeRoomInPackage(Lua_State* L)
{
	BOOL  bRetCode       = false;
	DWORD dwBoxIndex     = 0;
	DWORD dwX            = 0;
    int   nMaterialType  = 0;
    int   nTopIndex      = Lua_GetTopIndex(L);

	KG_PROCESS_ERROR(nTopIndex == 0 || nTopIndex == 1);
    
    if (nTopIndex == 1)
    {
        nMaterialType = (int)Lua_ValueToNumber(L, 1);
    }

	bRetCode = m_ItemList.FindFreeRoomInPackage(dwBoxIndex, dwX, nMaterialType);
	KG_PROCESS_ERROR(bRetCode);

	Lua_PushNumber(L, dwBoxIndex);
	Lua_PushNumber(L, dwX);

    return 2;
Exit0:
	return 0;
}

int KPlayer::LuaGetStackRoomInPackage(Lua_State* L)
{
	BOOL bRetCode		= false;
	DWORD dwBoxIndex	= 0;
	DWORD dwX			= 0;
	DWORD dwSrcBox		= 0;
	DWORD dwSrcX		= 0;
	KItem* pItem		= NULL;
	int nTopIndex		= 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2);

	dwSrcBox = (DWORD)Lua_ValueToNumber(L, 1);
	dwSrcX = (DWORD)Lua_ValueToNumber(L, 2);

	pItem = m_ItemList.GetItem(dwSrcBox, dwSrcX);
	KGLOG_PROCESS_ERROR(pItem);

	bRetCode = m_ItemList.FindStackRoomInPackage(dwBoxIndex, dwX, pItem);
	KG_PROCESS_ERROR(bRetCode);
	
	Lua_PushNumber(L, dwBoxIndex);
	Lua_PushNumber(L, dwX);

	return 2;
Exit0:
	return 0;
}

int KPlayer::LuaGetStackRoomInBank(Lua_State* L)
{
	BOOL bRetCode		= false;
	DWORD dwBoxIndex	= 0;
	DWORD dwX			= 0;
	DWORD dwSrcBox		= 0;
	DWORD dwSrcX		= 0;
	KItem* pItem		= NULL;
	int nTopIndex		= 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2);

	dwSrcBox = (DWORD)Lua_ValueToNumber(L, 1);
	dwSrcX = (DWORD)Lua_ValueToNumber(L, 2);

	pItem = m_ItemList.GetItem(dwSrcBox, dwSrcX);
	KGLOG_PROCESS_ERROR(pItem);

	bRetCode = m_ItemList.FindStackRoomInBank(dwBoxIndex, dwX, pItem);
	KG_PROCESS_ERROR(bRetCode);

	Lua_PushNumber(L, dwBoxIndex);
	Lua_PushNumber(L, dwX);

	return 2;
Exit0:
	return 0;
}

int KPlayer::LuaSatisfyRequire(Lua_State* L)
{
	assert(L);

	int nSatisfied = false;
	int nParamCount = Lua_GetTopIndex(L);

	int nAttributeID = atInvalid;
	int nValue1 = 0;
	int nValue2 = 0;

	if (nParamCount < 2)
		goto Exit1;

	nAttributeID = (int)Lua_ValueToNumber(L, 1);
	nValue1 = (int)Lua_ValueToNumber(L, 2);
	if (nParamCount > 2)
		nValue2 = (int)Lua_ValueToNumber(L, 3);

	switch(nAttributeID)
	{
	case rqtFaction:
		// 门派是否相同
		//KG_PROCESS_ERROR();
		break;
	case rqtStrength:
		// 力量是否高于或等于装备要求
		KG_PROCESS_ERROR(m_nCurrentStrength >= nValue1);
		break;
	case rqtAgility:
		// 敏捷是否高于或等于装备要求
		KG_PROCESS_ERROR(m_nCurrentAgility >= nValue1);
		break;
	case rqtSpirit:
		// 根骨是否高于或等于装备要求
		KG_PROCESS_ERROR(m_nCurrentSpirit >= nValue1);
		break;
	case rqtVitality:
		// 体质是否高于或等于装备要求
		KG_PROCESS_ERROR(m_nCurrentVitality >= nValue1);
		break;
	case rqtLevel:
		// 等级是否高于或等于装备要求
		KG_PROCESS_ERROR(m_nLevel >= nValue1);
		break;
    case rqtGender:
        // 性别是否符合装备需求
        KG_PROCESS_ERROR(nValue1 == 0 || m_eGender == nValue1);
        break;
	default:
		break;
	}

Exit1:
	nSatisfied = true;
Exit0:
	Lua_PushBoolean(L, nSatisfied);
	return 1;
}

#if defined(_CLIENT)
int KPlayer::LuaWindowSelect(Lua_State* L)
{
	BOOL bRetCode = false;

	//检测参数个数
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	DWORD dwIndex = (DWORD)Lua_ValueToNumber(L, 1);
	BYTE bySelect = (BYTE)Lua_ValueToNumber(L, 2);

	bRetCode = g_PlayerClient.DoWindowSelect(dwIndex, bySelect);
	KGLOG_PROCESS_ERROR(bRetCode);

Exit0:

	return 0;
}
#endif	//_CLIENT



#if defined(_CLIENT)
int KPlayer::LuaDoDeathRespond(Lua_State* L)
{
	int nRetCode        = 0;
	int nRespondCode    = 0;

	nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	nRespondCode = (int)Lua_ValueToNumber(L, 1);
	KGLOG_PROCESS_ERROR(nRespondCode > drcInvalid && nRespondCode < drcTotal);

	g_PlayerClient.DoPlayerReviveRequest(nRespondCode);

Exit0:
	return 0;
}
#endif

int KPlayer::LuaGetItemCDProgress(Lua_State* L)
{
	return 4;
}

int KPlayer::LuaGetSkillCDProgress(Lua_State* L)
{
    int     nRetCode            = 0;
    DWORD   dwSkillID           = 0;
    DWORD   dwSkillLevel        = 0;
    KSkill *pSkill              = NULL;
    int     nTotalCooldown      = 0;
    int     nLeftCooldown       = 0;
    BOOL    bIsCooldownSkill    = false;
    int     nEndFrame           = g_pSO3World->m_nGameLoop;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    KG_PROCESS_ERROR(dwSkillID > 0);
    KG_PROCESS_ERROR(dwSkillLevel > 0);

    pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW(dwSkillID, dwSkillLevel);
    KG_PROCESS_ERROR(pSkill);

	KG_PROCESS_ERROR(!pSkill->m_pBaseInfo->bIsPassiveSkill); //被动技能不显示CD

    if (pSkill->m_pBaseInfo->bIsMountable) // 如果是内功，则CD的规则特殊处理
    {
        DWORD   dwCooldownID    = pSkill->m_dwCoolDownID[OTHER_SCHOOL_KUNGFU_CD_INDEX]; // 和策划规定，第2号CD计时器用作门派内，3号用作门派间
        int     nTimerEnd       = 0;
        int     nTimerInterval  = 0;
        KSkill* pKongfu         = NULL;

        if (m_SkillList.m_dwMountKungfuID != INVALID_SKILL_ID)
        {
            pKongfu = g_pSO3World->m_SkillManager.GetSkill_K(
                m_SkillList.m_dwMountKungfuID, m_SkillList.m_dwMountKungfuLevel
            );
        }

        // 这里pKongfu可能为空，在没有装备内功的时候
        // 没有装备或者不同门派之间的，认为是跨门派
        if (pKongfu && pKongfu->m_pBaseInfo->dwBelongSchool == pSkill->m_pBaseInfo->dwBelongSchool)
        {
            dwCooldownID = pSkill->m_dwCoolDownID[SAME_SCHOOL_KUNGFU_CD_INDEX];
        }

        nRetCode = m_TimerList.GetTimerData(dwCooldownID, nTimerInterval, nTimerEnd);
        KG_PROCESS_ERROR(nRetCode);

        bIsCooldownSkill = true;

        nEndFrame       = nTimerEnd;
        nLeftCooldown   = nTimerEnd - g_pSO3World->m_nGameLoop;
        nTotalCooldown  = nTimerInterval;

        goto Exit0;
    }

    if (pSkill->m_dwPublicCoolDownID)
    {
        int nTimerInterval  = 0;
        int nTimerEnd       = 0;

        bIsCooldownSkill = true;
        nRetCode = m_TimerList.GetTimerData(pSkill->m_dwPublicCoolDownID, nTimerInterval, nTimerEnd);

        if (nRetCode && nTimerEnd >= nEndFrame)
        {
            nEndFrame       = nTimerEnd;
            nLeftCooldown   = nTimerEnd - g_pSO3World->m_nGameLoop;
            nTotalCooldown  = nTimerInterval;
        }
    }

    for (int i = 0; i < MAX_SKILL_COOL_DOWN_TIMER; i++)
    {
        DWORD dwCooldownID      = pSkill->m_dwCoolDownID[i];
        int   nTimerEnd         = 0;
        int   nTimerInterval    = 0;

        if (dwCooldownID == 0)
            continue;

        bIsCooldownSkill = true;

        nRetCode = m_TimerList.GetTimerData(dwCooldownID, nTimerInterval, nTimerEnd);
        if (!nRetCode)
            continue;
        
        if (nTimerEnd <= nEndFrame)
            continue;
        
        nEndFrame       = nTimerEnd;
        nLeftCooldown   = nTimerEnd - g_pSO3World->m_nGameLoop;
        nTotalCooldown  = nTimerInterval;
    }
Exit0:

    // 如果nLeftCooldown, nTotalCooldown都为0, 表示已经到时间了.
    Lua_PushBoolean(L, bIsCooldownSkill);
    Lua_PushNumber(L, nLeftCooldown);
    Lua_PushNumber(L, nTotalCooldown);
    return 3;
}

int KPlayer::LuaGetCDInterval(Lua_State* L)
{
    int     nRetCode            = 0;
    DWORD   dwCooldownID        = 0;
    int     nTotalCooldown      = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwCooldownID    = (DWORD)Lua_ValueToNumber(L, 1);
    nTotalCooldown  = GetCDValue(dwCooldownID);

Exit0:    
    Lua_PushNumber(L, nTotalCooldown);
    return 1;    
}

int KPlayer::LuaGetCDLeft(Lua_State* L)
{
    int     nRetCode            = 0;
    DWORD   dwCooldownID        = 0;
    int     nTotalCooldown      = 0;
    int     nLeftCooldown       = 0;
    int     nTimerEnd           = 0;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    dwCooldownID = (DWORD)Lua_ValueToNumber(L, 1);

    // Get 失败表明这个Cooldown已经过了
    nRetCode = m_TimerList.GetTimerData(dwCooldownID, nTotalCooldown, nTimerEnd);
    KG_PROCESS_ERROR(nRetCode);

    nLeftCooldown = nTimerEnd - g_pSO3World->m_nGameLoop;

    if (nLeftCooldown < 0)
        nLeftCooldown = 0;

Exit0:
    Lua_PushNumber(L, nLeftCooldown);
    return 1;    
}


int KPlayer::LuaGetSkillLevel(Lua_State* L)
{
    int   nRetCode     = 0;
    DWORD dwSkillLevel = 0;
    DWORD dwSkillID    = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);

    dwSkillLevel = m_SkillList.GetSkillLevel(dwSkillID);

Exit0:
    Lua_PushNumber(L, dwSkillLevel);
    return 1;
}

int KPlayer::LuaGetSkillLifeCost(Lua_State* L)
{
    int                 nRetCode        = false;
    DWORD               dwSkillID       = 0;
    DWORD               dwSkillLevel    = 0;
    KSkill             *pSkill          = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;
    int                 nLifeCost       = -1;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    nLifeCost = pSkill->m_nCostLife;

Exit0:
    Lua_PushNumber(L, nLifeCost);
    return 1;
}

int KPlayer::LuaGetSkillManaCost(Lua_State* L)
{
    int                 nRetCode        = false;
    DWORD               dwSkillID       = 0;
    DWORD               dwSkillLevel    = 0;
    KSkill             *pSkill          = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;
    int                 nManaCost       = -1;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    nManaCost = pSkill->m_nCostMana * m_nModifyCostManaPercent / KILO_NUM;

    nManaCost = max(nManaCost, 0);


Exit0:
    Lua_PushNumber(L, nManaCost);
    return 1;
}

int KPlayer::LuaGetSkillStaminaCost(Lua_State* L)
{
    int                 nRetCode        = false;
    DWORD               dwSkillID       = 0;
    DWORD               dwSkillLevel    = 0;
    KSkill             *pSkill          = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;
    int                 nStaminaCost    = -1;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    nStaminaCost = pSkill->m_nCostStamina;

Exit0:
    Lua_PushNumber(L, nStaminaCost);
    return 1;
}

int KPlayer::LuaGetSkillPrepare(Lua_State* L)
{    
    int                 nRetCode        = false;
    DWORD               dwSkillID       = 0;
    DWORD               dwSkillLevel    = 0;
    KSkill             *pSkill          = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;
    int                 nPrepareFrame   = -1;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    nPrepareFrame = pSkill->m_nPrepareFrames;

Exit0:
    Lua_PushNumber(L, nPrepareFrame);
    return 1;
}

int KPlayer::LuaGetSkillChannel(Lua_State* L)
{
    int                 nRetCode        = false;
    DWORD               dwSkillID       = 0;
    DWORD               dwSkillLevel    = 0;
    KSkill             *pSkill          = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;
    int                 nChannelFrame   = -1;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    nChannelFrame = pSkill->m_nChannelFrame;

Exit0:
    Lua_PushNumber(L, nChannelFrame);
    return 1;
}

int KPlayer::LuaGetSkillMinRadius(Lua_State* L)
{
    int                 nRetCode        = false;
    DWORD               dwSkillID       = 0;
    DWORD               dwSkillLevel    = 0;
    KSkill             *pSkill          = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;
    int                 nMinRadius      = -1;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    nMinRadius = pSkill->m_nMinRadius;

Exit0:
    Lua_PushNumber(L, nMinRadius);
    return 1;
}

int KPlayer::LuaGetSkillMaxRadius(Lua_State* L)
{
    int                 nRetCode        = false;
    DWORD               dwSkillID       = 0;
    DWORD               dwSkillLevel    = 0;
    KSkill             *pSkill          = NULL;
    KSKILL_RECIPE_KEY   RecipeKey;
    int                 nMaxRadius      = -1;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwSkillID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel = (DWORD)Lua_ValueToNumber(L, 2);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&RecipeKey, dwSkillID, dwSkillLevel, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(RecipeKey);
    KGLOG_PROCESS_ERROR(pSkill);

    nMaxRadius = pSkill->m_nMaxRadius;

Exit0:
    Lua_PushNumber(L, nMaxRadius);
    return 1;
}


int KPlayer::LuaGetSkillExp(Lua_State* L)
{
    int     nRetCode        = false;
    DWORD   dwSkillID       = 0;
    DWORD   dwExp           = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwSkillID = (DWORD)Lua_ValueToNumber(L, 1);

    dwExp = m_SkillList.GetSkillExp(dwSkillID);

Exit0:
    Lua_PushNumber(L, dwExp);
    return 1;
}



int KPlayer::LuaMountKungfu(Lua_State* L)
{
    int     nRetCode        = false;
    DWORD   dwKungfuID      = INVALID_SKILL_ID;
    DWORD   dwKungfuLevel   = 0;
    KSkill* pKungfu         = NULL;


    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwKungfuID    = (DWORD)Lua_ValueToNumber(L, 1);
    dwKungfuLevel = (DWORD)Lua_ValueToNumber(L, 2);

#ifdef _SERVER
    KGLOG_PROCESS_ERROR(m_SkillList.m_dwMountKungfuID == INVALID_SKILL_ID);
#endif

    pKungfu = g_pSO3World->m_SkillManager.GetSkill_K(dwKungfuID, dwKungfuLevel);
    KGLOG_PROCESS_ERROR(pKungfu);

    KGLOG_PROCESS_ERROR(pKungfu->m_pBaseInfo->bIsMountable);

    nRetCode = m_SkillList.IsSkillExist(dwKungfuID, dwKungfuLevel);
    KGLOG_PROCESS_ERROR(nRetCode);

#ifdef _SERVER
	if (m_SkillList.m_dwMountKungfuID != INVALID_SKILL_ID)
	{
        // 如果原本已经装备了一个武功,则先把它卸载
        nRetCode = UmountKungfu();
        KGLOG_PROCESS_ERROR(nRetCode);
	}

    nRetCode = MountKungfu(dwKungfuID, dwKungfuLevel);
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

#ifdef _CLIENT
    nRetCode = g_PlayerClient.DoMountKungfu(dwKungfuID, dwKungfuLevel);
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
    return 0;
}

int KPlayer::LuaUmountKungfu(Lua_State* L)
{
    int nRetCode = false;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_SkillList.m_dwMountKungfuID != INVALID_SKILL_ID);
    nRetCode = g_PlayerClient.DoMountKungfu(INVALID_SKILL_ID, 0);
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

#ifdef _SERVER
    nRetCode = UmountKungfu();
    KGLOG_PROCESS_ERROR(nRetCode);
#endif

Exit0:
    return 0;
}

int KPlayer::LuaGetKungfuMount(Lua_State* L)
{
    KSkill* pKungfu = g_pSO3World->m_SkillManager.GetSkill_K(m_SkillList.m_dwMountKungfuID, m_SkillList.m_dwMountKungfuLevel);

	KG_PROCESS_ERROR(pKungfu);

    pKungfu->LuaGetObj(L);

    return 1;
Exit0:
	return 0;
}

#ifdef _CLIENT
int KPlayer::LuaUseItem(Lua_State* L)
{
	int                     nResult     = uircFailed;
    USE_ITEM_RESULT_CODE    eRetCode    = uircFailed;
    int                     nRetCode    = false;
	int                     nParamCount = 0;
	DWORD                   dwBox       = ibTotal;
	DWORD                   dwX         = 1000000;
	KSKILL_CAST_MODE        eCastMode   = scmCasterSingle;
    KItem*                  pItem       = NULL; 
    KUSE_ITEM               UseItemInfo = { 0 };
    KTarget                 Target;
    TItemPos                Pos;

	nParamCount = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nParamCount >= 2);

	dwBox = (DWORD)Lua_ValueToNumber(L, 1);
	dwX = (DWORD)Lua_ValueToNumber(L, 2);
    
    Pos.dwBox = dwBox;
    Pos.dwX   = dwX;

	if (nParamCount >= 3)
	{
		eCastMode = (KSKILL_CAST_MODE)(DWORD)Lua_ValueToNumber(L, 3);
	}

	if (eCastMode == scmPoint || eCastMode == scmPointArea)
	{
        int nX, nY, nZ;

		KGLOG_PROCESS_ERROR(nParamCount == 6);

		nX = (int)Lua_ValueToNumber(L, 4);
		nY = (int)Lua_ValueToNumber(L, 5);
		nZ = (int)Lua_ValueToNumber(L, 6);

        nRetCode = Target.SetTarget(nX, nY, nZ);
        KGLOG_PROCESS_ERROR(nRetCode);
	}
	else if (eCastMode == scmItem)
	{
		DWORD dwTargetBox = 0;
		DWORD dwTargetX = 0;

		KGLOG_PROCESS_ERROR(nParamCount == 5);

		dwTargetBox = (DWORD)Lua_ValueToNumber(L, 4);
		dwTargetX = (DWORD)Lua_ValueToNumber(L, 5);

		KGLOG_PROCESS_ERROR(dwTargetBox < ibTotal);
	
		nRetCode = Target.SetTarget(this, dwTargetBox, dwTargetX);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	else if (eCastMode == scmCasterSingle)
	{
		Target.SetTarget(this);
	}
	else
	{
		Target = m_SelectTarget;
	}

	pItem = m_ItemList.GetItem(dwBox, dwX);
	KG_PROCESS_ERROR_RET_CODE(pItem, uircItemNotExist);
    
    eRetCode = m_ItemList.CanUseItem(pItem, dwBox, &UseItemInfo);
    KG_PROCESS_ERROR_RET_CODE(eRetCode == uircSuccess, eRetCode);

	if (pItem->m_dwSkillID && pItem->m_dwSkillLevel)
	{
        KSKILL_RECIPE_KEY   SkillRecipeKey;
        KSkill*             pSkill = NULL;

        nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(
            &SkillRecipeKey, 
            pItem->m_dwSkillID, pItem->m_dwSkillLevel, 
            this
        );
        KGLOG_PROCESS_ERROR(nRetCode);

		pSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
		KG_PROCESS_ERROR(pSkill);

		// ----------------- 检查自身条件 ---------------------------
		nRetCode = pSkill->CheckTargetRange(this, Target);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, uircCannotCastSkill);

		nRetCode = pSkill->CheckWeaponRequest(this);
		KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, uircCannotCastSkill);

		nRetCode = pSkill->CheckMountRequest(this);
		KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, uircCannotCastSkill);

		nRetCode = pSkill->CheckCoolDown(this);
		KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, uircCannotCastSkill);

        nRetCode = pSkill->CheckSelfState(this);
        KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, uircCannotCastSkill);

		nRetCode = pSkill->CheckCost(this);
		KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, uircCannotCastSkill);
	}

	g_PlayerClient.DoApplyUseItem(dwBox, dwX, Target);

	nResult = uircSuccess;
Exit0:
	if (nResult != uircSuccess)
	{
        if (g_pGameWorldUIHandler)
        {
            KUIEventUseItemRespond UINotify;

            UINotify.nRespondCode           = nResult;
            UINotify.dwProfessionID         = UseItemInfo.dwProfessionID;
            UINotify.dwProfessionBranchID   = UseItemInfo.dwProfessionBranchID;
            UINotify.dwProfessionLevel      = UseItemInfo.dwProfessionLevel;

            g_pGameWorldUIHandler->OnUseItemRespond(UINotify);
        }
	}
	Lua_PushNumber(L, nResult);
	return 1;
}

int KPlayer::LuaGetItemPos(Lua_State* L)
{
	int nResult = false;
	int nParamCount = 0;
	int nRetCode = false;
	DWORD dwBox = ibTotal;
	DWORD dwX = 10000;

	nParamCount = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nParamCount > 0);

	switch (nParamCount)
	{
	case 1:
		nRetCode = m_ItemList.GetItemPos((DWORD)Lua_ValueToNumber(L, 1), dwBox, dwX);
		KG_PROCESS_ERROR(nRetCode);
		break;
	case 2:
		nRetCode = m_ItemList.GetItemPos(
			(DWORD)Lua_ValueToNumber(L, 1), 
			(DWORD)Lua_ValueToNumber(L, 2), dwBox, dwX);
		KG_PROCESS_ERROR(nRetCode);
		break;
	default:
		nRetCode = m_ItemList.GetItemPos(
			(DWORD)Lua_ValueToNumber(L, 1), 
			(DWORD)Lua_ValueToNumber(L, 2), 
			(DWORD)Lua_ValueToNumber(L, 3), dwBox, dwX);
		KG_PROCESS_ERROR(nRetCode);
		break;
	}

	nResult = true;
Exit0:
	if (!nResult)
		return 0;
	Lua_PushNumber(L, dwBox);
	Lua_PushNumber(L, dwX);
	return 2;
}

int KPlayer::IsEquiped(KCustomEquipInfo *pInfo)
{
	int nResult = false;

	if(pInfo)
	{
		for (DWORD i = eitMeleeWeapon; i < eitTotal; ++i)
		{
			KItem *pItem = m_ItemList.GetItem(ibEquip, i);
			if (pItem && 
				pItem->m_Common.dwSetID == (DWORD)pInfo->nSetID &&
				_tcscmp(pItem->m_Common.szItemName, pInfo->szName) == 0 &&
				pItem->m_Common.nSub == pInfo->nSub &&
				pItem->m_Common.nDetail == pInfo->nDetail &&
				pItem->m_nCurrentDurability > 0
				)
				return true;
		}
	}
	return false;
}

#endif	//_CLIENT

#ifdef _SERVER
int KPlayer::LuaUseItemOnNoTarget(Lua_State* L)
{
    int                     nTopIndex   = 0;
    USE_ITEM_RESULT_CODE    eRetCode    = uircFailed;
    BOOL                    bRetCode    = false;
    DWORD                   dwBox       = ibInvalid;
    DWORD                   dwX         = 0;
    KTarget                 Target;

    nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 2);

    dwBox   = (DWORD)Lua_ValueToNumber(L, 1);
    dwX     = (DWORD)Lua_ValueToNumber(L, 2);
    
    bRetCode = Target.ClearTarget();
    KGLOG_PROCESS_ERROR(bRetCode);

    eRetCode = m_ItemList.UseItem(dwBox, dwX, Target);
    KG_PROCESS_ERROR(eRetCode == uircSuccess);

Exit0:
    return 0;
}

int KPlayer::LuaUseItemOnNpc(Lua_State* L)
{
    int                     nTopIndex   = 0;
    USE_ITEM_RESULT_CODE    eRetCode    = uircFailed;
    BOOL                    bRetCode    = false;
    DWORD                   dwBox       = ibInvalid;
    DWORD                   dwX         = 0;
    DWORD                   dwNpcID     = ERROR_ID;
    KNpc*                   pNpc        = NULL;
    KTarget                 Target;

    nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 3);
    
    dwBox   = (DWORD)Lua_ValueToNumber(L, 1);
    dwX     = (DWORD)Lua_ValueToNumber(L, 2);
    dwNpcID = (DWORD)Lua_ValueToNumber(L, 3);
    
    pNpc = g_pSO3World->m_NpcSet.GetObj(dwNpcID);
    KGLOG_PROCESS_ERROR(pNpc);

    bRetCode = Target.SetTarget(pNpc);
    KGLOG_PROCESS_ERROR(bRetCode);

    eRetCode = m_ItemList.UseItem(dwBox, dwX, Target);
    KG_PROCESS_ERROR(eRetCode == uircSuccess);

Exit0:
    return 0;
}

int KPlayer::LuaUseItemOnPlayer(Lua_State* L)
{
    int                     nTopIndex   = 0;
    USE_ITEM_RESULT_CODE    eRetCode    = uircFailed;
    BOOL                    bRetCode    = false;
    DWORD                   dwBox       = ibInvalid;
    DWORD                   dwX         = 0;
    DWORD                   dwPlayerID  = ERROR_ID;
    KPlayer*                pPlayer     = NULL;
    KTarget                 Target;

    nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 3);
    
    dwBox       = (DWORD)Lua_ValueToNumber(L, 1);
    dwX         = (DWORD)Lua_ValueToNumber(L, 2);
    dwPlayerID  = (DWORD)Lua_ValueToNumber(L, 3);
    
    pPlayer = g_pSO3World->m_PlayerSet.GetObj(dwPlayerID);
    KGLOG_PROCESS_ERROR(pPlayer);

    bRetCode = Target.SetTarget(pPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

    eRetCode = m_ItemList.UseItem(dwBox, dwX, Target);
    KG_PROCESS_ERROR(eRetCode == uircSuccess);

Exit0:
    return 0;
}

int KPlayer::LuaUseItemOnItem(Lua_State* L)
{
    int                     nTopIndex   = 0;
    USE_ITEM_RESULT_CODE    eRetCode    = uircFailed;
    BOOL                    bRetCode    = false;
    DWORD                   dwBox       = ibInvalid;
    DWORD                   dwX         = 0;
    DWORD                   dwDestBox   = ibInvalid;
    DWORD                   dwDestX     = 0;
    KTarget                 Target;

    nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 4);
    
    dwBox       = (DWORD)Lua_ValueToNumber(L, 1);
    dwX         = (DWORD)Lua_ValueToNumber(L, 2);
    dwDestBox   = (DWORD)Lua_ValueToNumber(L, 3);
    dwDestX     = (DWORD)Lua_ValueToNumber(L, 4);

    bRetCode = Target.SetTarget(this, dwDestBox, dwDestX);
    KGLOG_PROCESS_ERROR(bRetCode);

    eRetCode = m_ItemList.UseItem(dwBox, dwX, Target);
    KG_PROCESS_ERROR(eRetCode == uircSuccess);

Exit0:
    return 0;
}

int KPlayer::LuaUseItemOnDoodad(Lua_State* L)
{
    int                     nTopIndex   = 0;
    USE_ITEM_RESULT_CODE    eRetCode    = uircFailed;
    BOOL                    bRetCode    = false;
    DWORD                   dwBox       = ibInvalid;
    DWORD                   dwX         = 0;
    DWORD                   dwDoodadID  = ERROR_ID;
    KDoodad*                pDoodad     = NULL;
    KTarget                 Target;

    nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 3);
    
    dwBox       = (DWORD)Lua_ValueToNumber(L, 1);
    dwX         = (DWORD)Lua_ValueToNumber(L, 2);
    dwDoodadID  = (DWORD)Lua_ValueToNumber(L, 3);
    
    pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
    KGLOG_PROCESS_ERROR(pDoodad);

    bRetCode = Target.SetTarget(pDoodad);
    KGLOG_PROCESS_ERROR(bRetCode);

    eRetCode = m_ItemList.UseItem(dwBox, dwX, Target);
    KG_PROCESS_ERROR(eRetCode == uircSuccess);

Exit0:
    return 0;
}

int KPlayer::LuaUseItemOnCoordinates(Lua_State* L)
{
    int                     nTopIndex   = 0;
    USE_ITEM_RESULT_CODE    eRetCode    = uircFailed;
    BOOL                    bRetCode    = false;
    DWORD                   dwBox       = ibInvalid;
    DWORD                   dwX         = 0;
    int                     nX          = 0;
    int                     nY          = 0;
    int                     nZ          = 0;
    KTarget                 Target;

    nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 5);
    
    dwBox   = (DWORD)Lua_ValueToNumber(L, 1);
    dwX     = (DWORD)Lua_ValueToNumber(L, 2);
    nX      = (int)Lua_ValueToNumber(L, 3);
    nY      = (int)Lua_ValueToNumber(L, 4);
    nZ      = (int)Lua_ValueToNumber(L, 5);

    bRetCode = Target.SetTarget(nX, nY, nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    eRetCode = m_ItemList.UseItem(dwBox, dwX, Target);
    KG_PROCESS_ERROR(eRetCode == uircSuccess);

Exit0:
    return 0;
}
#endif

int KPlayer::LuaSearchForPartyMember(Lua_State* L)
{
    int         nRetCode        = false;
    int         nDistance       = 0;
    BOOL        bSearchForward  = false;
    DWORD       dwLastResult    = 0;
    KPlayer*    pPartymember    = NULL;
    int         nResultNum      = 0;
   
    KSearchForPartyMember Tactic;


    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    nDistance       = (int)Lua_ValueToNumber(L, 1);
    bSearchForward  = (BOOL)Lua_ValueToNumber(L, 2);
    dwLastResult    = (DWORD)Lua_ValueToNumber(L, 3);

    Tactic.m_pSelf      = this;
    Tactic.m_nDistance  = nDistance;
    Tactic.m_Result.reserve(8);

    AISearchPlayer(Tactic);

    nResultNum = (int)Tactic.m_Result.size();
    KG_PROCESS_ERROR(nResultNum > 0);

    if (dwLastResult != 0)
    {
        std::vector<KPlayer*>::iterator it;
        for (it = Tactic.m_Result.begin(); it != Tactic.m_Result.end(); ++it)
        {
            KPlayer* pPlayer = *it;
            if (pPlayer->m_dwID != dwLastResult)
                continue;

            if (bSearchForward)
            {
                if (it + 1 != Tactic.m_Result.end())
                {
                    pPartymember = *(it + 1);
                }
                else
                {
                    pPartymember = Tactic.m_Result[0];
                }
            }
            else
            {
                if (it == Tactic.m_Result.begin())
                {
                    pPartymember = Tactic.m_Result[nResultNum - 1];
                }
                else
                {
                    pPartymember = *(it - 1);
                }
            }
            break;
        }
    }
    else
    {
        if (bSearchForward)
        {
            pPartymember = Tactic.m_Result[0];
        }
        else
        {
            pPartymember = Tactic.m_Result[nResultNum - 1];
        }
    }

Exit0:
    if (pPartymember)
    {
        pPartymember->LuaGetObj(L);
    }
    else
    {
        Lua_PushNil(L);
    }
    return 1;
}

int KPlayer::LuaGetOTActionState(Lua_State *L)
{
	KOT_ACTION_TYPE	eOTActionState = m_OTActionParam.eActionType;
	Lua_PushNumber(L, eOTActionState);

	return 1;
}

#ifdef _SERVER
int KPlayer::LuaAddSkillRecipe(Lua_State* L)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nParamCount     = 0;
    DWORD   dwRecipeID      = 0;
    DWORD   dwRecipeLevel   = 0;
    DWORD   dwRecipeKey     = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 2);

    dwRecipeID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwRecipeLevel   = (DWORD)Lua_ValueToNumber(L, 2);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    bRetCode = m_SkillRecipeList.Add(dwRecipeKey);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaDelSkillRecipe(Lua_State* L)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nParamCount     = 0;
    DWORD   dwRecipeID      = 0;
    DWORD   dwRecipeLevel   = 0;
    DWORD   dwRecipeKey     = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 2);

    dwRecipeID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwRecipeLevel   = (DWORD)Lua_ValueToNumber(L, 2);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    bRetCode = m_SkillRecipeList.Delete(dwRecipeKey);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = false;
Exit0:
    Lua_PushNumber(L, bResult);
    return 1;
}
#endif

int KPlayer::LuaGetSkillRecipeList(Lua_State* L)
{
    BOOL                        bRetCode        = false;
    int                         nParamCount     = 0;
    DWORD                       dwSkillID       = 0;
    DWORD                       dwSkillLevel    = 0;
    KSkill*                     pSkill          = NULL;
    size_t                      uCount          = 0;
    size_t                      uActiveCount    = 0;
    KG_SKILL_RECIPE_LIST_NODE   RecipeArray[32];

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 2);

    dwSkillID       = (DWORD)Lua_ValueToNumber(L, 1);
    dwSkillLevel    = (DWORD)Lua_ValueToNumber(L, 2);

    pSkill = g_pSO3World->m_SkillManager.GetSkill_RAW_S(dwSkillID, dwSkillLevel);
    KGLOG_PROCESS_ERROR(pSkill);

    bRetCode = m_SkillRecipeList.GetRecipeList(
        dwSkillID, dwSkillLevel, 
        sizeof(RecipeArray) / sizeof(RecipeArray[0]), &uCount, &uActiveCount, RecipeArray
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_NewTable(L);
    for (int i = 0; i < (int)uCount; i++)
    {
        DWORD dwRecipeID    = 0;
        DWORD dwRecipeLevel = 0;

        Lua_PushNumber(L, i + 1);

        GET_RECIPE_ID_OF_KEY(dwRecipeID, RecipeArray[i].dwRecipeKey);
        GET_RECIPE_LEVEL_OF_KEY(dwRecipeLevel, RecipeArray[i].dwRecipeKey);

        Lua_NewTable(L);

        Lua_PushString(L, "recipe_id");
        Lua_PushNumber(L, dwRecipeID);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "recipe_level");
        Lua_PushNumber(L, dwRecipeLevel);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "active");
        Lua_PushBoolean(L, RecipeArray[i].bActive);
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaIsSkillRecipeExist(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = 0;
    DWORD   dwRecipeKey     = 0;
    DWORD   dwRecipeID      = 0;
    DWORD   dwRecipeLevel   = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwRecipeID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwRecipeLevel   = (DWORD)Lua_ValueToNumber(L, 2);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    nRetCode = m_SkillRecipeList.Find(dwRecipeKey);
    KG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaCanLearnSkillRecipe(Lua_State* L)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    DWORD   dwRecipeID      = 0;
    DWORD   dwRecipeLevel   = 0;
    DWORD   dwRecipeKey     = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwRecipeID      = (DWORD)Lua_ValueToNumber(L, 1);
    dwRecipeLevel   = (DWORD)Lua_ValueToNumber(L, 2);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    bRetCode = m_SkillRecipeList.CanAdd(dwRecipeKey);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}

int KPlayer::LuaActiveSkillRecipe(Lua_State* L)
{
    int     nResult         = srrFailed;
    BOOL    bRetCode        = false;
    int     nParamCount     = 0;
    DWORD   dwRecipeID      = 0;
    DWORD   dwRecipeLevel   = 0;
    DWORD   dwRecipeKey     = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 2);

#ifdef _CLIENT
    KG_PROCESS_ERROR_RET_CODE(!m_bFightState, srrErrorInFight);
#endif

    dwRecipeID = (DWORD)Lua_ValueToNumber(L, 1);
    dwRecipeLevel = (DWORD)Lua_ValueToNumber(L, 2);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    bRetCode = m_SkillRecipeList.Find(dwRecipeKey);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_SkillRecipeList.Active(dwRecipeKey);
    KG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
    bRetCode = g_PlayerClient.DoPlayerActiveSkillRecipe(dwRecipeKey, true);
	KG_PROCESS_ERROR(bRetCode);
#endif

	nResult = srrSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KPlayer::LuaDeactiveSKillRecipe(Lua_State* L)
{
    int     nResult         = srrFailed;
    BOOL    bRetCode        = false;
    int     nParamCount     = 0;
    DWORD   dwRecipeID      = 0;
    DWORD   dwRecipeLevel   = 0;
    DWORD   dwRecipeKey     = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 2);

#ifdef _CLIENT
    KG_PROCESS_ERROR_RET_CODE(!m_bFightState, srrErrorInFight);
#endif

    dwRecipeID = (DWORD)Lua_ValueToNumber(L, 1);
    dwRecipeLevel = (DWORD)Lua_ValueToNumber(L, 2);

    MAKE_RECIPE_KEY(dwRecipeKey, dwRecipeID, dwRecipeLevel);

    bRetCode = m_SkillRecipeList.Find(dwRecipeKey);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = m_SkillRecipeList.Deactive(dwRecipeKey);
    KG_PROCESS_ERROR(bRetCode);

#ifdef _CLIENT
    bRetCode = g_PlayerClient.DoPlayerActiveSkillRecipe(dwRecipeKey, false);
	KG_PROCESS_ERROR(bRetCode);
#endif

	nResult = srrSuccess;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

#ifdef _CLIENT
int KPlayer::LuaOnCloseLootWindow(Lua_State* L)
{
	KDoodad* pDoodad = NULL;
	
    pDoodad = g_pSO3World->m_DoodadSet.GetObj(m_OTActionParam.Data.dwDoodadID);
	KGLOG_PROCESS_ERROR(pDoodad);

	g_PlayerClient.DoFinishLoot(pDoodad->m_dwID);
    
    if (m_OTActionParam.eActionType == otActionPickPrepare)
    {
        if (pDoodad->m_pScene)
        {
            pDoodad->ChangeState(dsIdle);
        }

        if (g_pGameWorldRepresentHandler)
        {
            // 如果跳过这个动作，后面的end动作会播放失败。玩家会一直处于prepare动作
            g_pGameWorldRepresentHandler->OnCharacterBeginPickDoodad(this, pDoodad);
        }
    }
	
Exit0:
	return 0;
}
#endif

int KPlayer::LuaGetCommonSkill(Lua_State* L)
{
    int      nRetCode       = 0;
    DWORD    dwSkillID      = 0;
    KItem*   pItem          = NULL;
    BOOL     bMelee         = false;
    int      nWeaponType    = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bMelee  = (BOOL)Lua_ValueToBoolean(L, 1);

    if (bMelee)
    {
        pItem = m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
        if (pItem)
            nWeaponType = pItem->m_Common.nDetail;
        else
            nWeaponType = wdtFist;
    }
    else
    {
        pItem = m_ItemList.GetItem(ibEquip, eitRangeWeapon);
        if (!pItem)
        {
            nRetCode = srcWeaponError;
            goto Exit0;
        }
        nWeaponType = pItem->m_Common.nDetail;
    }

    nRetCode = g_pSO3World->m_SkillManager.WeaponMapCommonSkill(nWeaponType, dwSkillID);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    Lua_PushNumber(L, dwSkillID);
    return 1;
}

#ifdef _CLIENT
int KPlayer::LuaCastCommonSkill(Lua_State* L)
{
    int                 nResult         = srcFailed;
    int                 nRetCode        = 0;
    BOOL                bMelee          = false;
    KItem*              pItem           = NULL;
    int                 nWeaponType     = 0;
    TARGET_TYPE         eTargetType     = ttInvalid;
    KTarget             Target;
    BOOL                bTryCastSkill   = false;
    DWORD               dwSkillID       = 0;
    KSkill*             pSkill          = NULL;
    KSKILL_RECIPE_KEY   SkillRecipeKey;

    nRetCode = Lua_GetTopIndex(L);
    KG_ASSERT_EXIT(nRetCode == 1);

    bMelee = (BOOL)Lua_ValueToBoolean(L, 1);

    if (bMelee)
    {
        pItem = this->m_ItemList.GetItem(ibEquip, eitMeleeWeapon);
        if (pItem)
            nWeaponType = pItem->m_Common.nDetail;
        else
            nWeaponType = wdtFist;
    }
    else
    {
        pItem = this->m_ItemList.GetItem(ibEquip, eitRangeWeapon);
        KG_PROCESS_ERROR_RET_CODE(pItem, srcWeaponError);
        nWeaponType = pItem->m_Common.nDetail;
    }

    nRetCode = g_pSO3World->m_SkillManager.WeaponMapCommonSkill(nWeaponType, dwSkillID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = g_pSO3World->m_SkillManager.GetSkillRecipeKey(&SkillRecipeKey, dwSkillID, 1, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    pSkill = g_pSO3World->m_SkillManager.GetSkill(SkillRecipeKey);
    KG_PROCESS_ERROR(pSkill);
    
    bTryCastSkill = true;

    eTargetType = m_SelectTarget.GetTargetType();
    if (eTargetType == ttNoTarget && pSkill->m_pBaseInfo->bAutoSelectTarget)
    {
        KCharacter* pCaster = (KCharacter*)g_pSO3World->m_PlayerSet.GetObj(g_pSO3World->m_dwClientPlayerID);
        assert(pCaster);
        nRetCode = pSkill->AutoSelectTarget(pCaster);
        KG_PROCESS_ERROR_RET_CODE(nRetCode, srcNoTarget);
    }

    Target = m_SelectTarget;
    
    nRetCode = pSkill->CanCast(this, Target);
    
    KG_PROCESS_SUCCESS(
        nRetCode == srcTooCloseTarget || nRetCode == srcTooFarTarget || nRetCode == srcOutOfAngle
    );
    
    KG_PROCESS_ERROR_RET_CODE(nRetCode == srcSuccess, nRetCode);
    
Exit1:
    g_PlayerClient.DoCharacterSkill(dwSkillID, 1, m_nFaceDirection, &Target);
    
    nResult = srcSuccess;
Exit0:
    if (nResult != srcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventSkillCastRespond UINotify = { nResult };
            g_pGameWorldUIHandler->OnSkillCastRespond(UINotify);

            if (bTryCastSkill)
            {
                KUIEventSkillCastRespondLog UIRespondLog;
                UIRespondLog.dwCaster   = m_dwID;
                UIRespondLog.dwSkillID  = dwSkillID;
                UIRespondLog.dwLevel    = 1;
                UIRespondLog.nRespond   = nResult;
                g_pGameWorldUIHandler->OnSkillCastRespondLog(UIRespondLog);        
            }
        }
    }
    return 0;
}

int KPlayer::LuaStopCurrentAction(Lua_State* L)
{
    BOOL bResult  = false;
    
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);

    if (
        (m_OTActionParam.eActionType != otActionIdle && m_OTActionParam.eActionType != otActionPicking) || // otActionPicking不通过Esc来打断
        (m_AutoCastSkillParam.SkillRecipePointer.GetPointer())
    )
    {
        g_PlayerClient.DoStopCurrentAction();
        bResult = true;
        goto Exit0;
    }

Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

#endif //_CLIENT

#if defined(_SERVER)
int KPlayer::LuaSetPartyLootMode(Lua_State* L)
{
	BOOL bResult        = false;
    int nLootMode       = 0;
    int nRollQuality    = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nLootMode = (int)Lua_ValueToNumber(L, 1);
	nRollQuality = (int)Lua_ValueToNumber(L, 2);

	KGLOG_PROCESS_ERROR(nLootMode > ilmInvalid && nLootMode < ilmTotal);
	KGLOG_PROCESS_ERROR(nRollQuality >= 0);

	KGLOG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

	g_RelayClient.DoTeamSetLootModeRequest(m_dwTeamID, m_dwID, nLootMode);
	g_RelayClient.DoTeamSetRollQualityRequest(m_dwTeamID, m_dwID, nRollQuality);
	
	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif //_SERVER

//---- AI ----------------------------------------------
#if defined(_CLIENT)
int KPlayer::LuaFollowSelectTarget(Lua_State *L)
{
    BOOL        bResult     = false;
	BOOL        bRetCode    = false;
    TARGET_TYPE eTargetType = ttInvalid;
	KCharacter* pTarget     = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

    eTargetType = m_SelectTarget.GetTargetType();

	if (eTargetType == ttPlayer || eTargetType == ttNpc)
	{
        bRetCode = m_SelectTarget.GetTarget(&pTarget);
        KGLOG_PROCESS_ERROR(bRetCode);
	}

	KGLOG_PROCESS_ERROR(pTarget);

	// m_AIController.DoFollow(pTarget);

    if (g_pGameWorldRepresentHandler)
	    g_pGameWorldRepresentHandler->OnCharacterFollowTarget((KCharacter*)this, pTarget);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

void KPlayer::HostStopFollow()
{
	// m_AIController.DoFollow(NULL);

	if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterFollowTarget((KCharacter*)this, NULL);
    }
}

int KPlayer::LuaStopFollow(lua_State *L)
{
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

	HostStopFollow();

Exit0:
	return 0;
}
#endif //_CLIENT

int KPlayer::LuaSearchForAllies(Lua_State* L)
{
	int nSize        = 0;
    int nDistance    = 0;
    int nAngle       = 0;
    int nRelation    = 0;
    int nTable       = 0;
	int nCharacterID = ERROR_ID;
	KSearchForCharacter Tactic;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nDistance	= (int)Lua_ValueToNumber(L, 1);
	nAngle		= (int)Lua_ValueToNumber(L, 2);

	nRelation = sortAlly | sortParty;

	Tactic.m_pSelf = (KCharacter*)this;
	Tactic.m_nDistance2 = nDistance * nDistance;
	Tactic.m_nAngle = nAngle;
	Tactic.m_nRelation = nRelation;

	AISearchCharacter(Tactic);

	//按距离排序
	sort(Tactic.m_Result.begin(), Tactic.m_Result.end(), g_CompByDistance);

	nSize = (DWORD)Tactic.m_Result.size();
	KG_PROCESS_ERROR(nSize > 0);

	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		Lua_PushNumber(L, nIndex + 1);
		lua_pushinteger(L, Tactic.m_Result[nIndex].m_pCharacter->m_dwID);

		Lua_SetTable(L, nTable);
	}
	Lua_PushNumber(L, nSize);
	return 2;
Exit0:
	Lua_PushNil(L);
	Lua_PushNumber(L, 0);
	return 2;
}

int KPlayer::LuaSearchForEnemy(Lua_State* L)
{
	int nSize        = 0;
    int nDistance    = 0;
    int nAngle       = 0;
    int nTable       = 0;
    int nRelation    = sortEnemy;
	int nCharacterID = ERROR_ID;
	KSearchForCharacter Tactic;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

	nDistance	= (int)Lua_ValueToNumber(L, 1);
	nAngle		= (int)Lua_ValueToNumber(L, 2);
	
	Tactic.m_pSelf = (KCharacter*)this;
	Tactic.m_nDistance2 = nDistance * nDistance;
	Tactic.m_nAngle = nAngle;
	Tactic.m_nRelation = nRelation;

	AISearchCharacter(Tactic);

	//按距离排序
	sort(Tactic.m_Result.begin(), Tactic.m_Result.end(), g_CompByDistance);

	nSize = (int)Tactic.m_Result.size();
	KG_PROCESS_ERROR(nSize > 0);

	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		Lua_PushNumber(L, nIndex + 1);
		lua_pushinteger(L, Tactic.m_Result[nIndex].m_pCharacter->m_dwID);

		Lua_SetTable(L, nTable);
	}
	Lua_PushNumber(L, nSize);
	return 2;
Exit0:
	Lua_PushNil(L);
	Lua_PushNumber(L, 0);
	return 2;
}

//--------------------声望-----------------------
#ifdef _SERVER
int KPlayer::LuaSetReputation(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nRetCode    = false;
	DWORD   dwForceID   = 0;
	int     nReputation = 0;
	int     nLevel      = 0;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 3);

	dwForceID   = (DWORD)Lua_ValueToNumber(L, 1);
	nLevel      = (int)Lua_ValueToNumber(L, 2);
	nReputation = (int)Lua_ValueToNumber(L, 3);

	nRetCode = m_ReputeList.SetReputation(dwForceID, nLevel, nReputation);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_ReputeList.IsShow(dwForceID);
    if (!nRetCode)
    {
        m_ReputeList.Show(dwForceID);
    }
    
    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif

#ifdef _SERVER
int KPlayer::LuaAddReputation(Lua_State* L)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    DWORD   dwForceID   = 0;
	int     nReputation = 0;
	int     nLowLimit   = -1;
	int     nHighLimit  = -1;
    int     nTopIndex   = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 4);

	dwForceID   = (DWORD)Lua_ValueToNumber(L, 1);
	nReputation = (int)Lua_ValueToNumber(L, 2);

	if (nTopIndex == 4)
	{
		nLowLimit   = (int)Lua_ValueToNumber(L, 3);
		nHighLimit  = (int)Lua_ValueToNumber(L, 4);
	}

	bRetCode = m_ReputeList.ModifyReputation(dwForceID, nReputation, nLowLimit, nHighLimit);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaOpenReputation(Lua_State* L)
{
    BOOL    bResult  = false;
	BOOL    bRetCode = false;
	DWORD   dwForceID = 0;
	int     nTopIndex = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwForceID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ReputeList.Show(dwForceID);
	KGLOG_PROCESS_ERROR(bRetCode);
    
    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif //_SERVER

int KPlayer::LuaGetReputeLevel(Lua_State* L)
{
    int     nRetCode        = 0;
    int     nRepateLevel    = 0;
    DWORD   dwForceID       = 0;
    
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwForceID = (DWORD)Lua_ValueToNumber(L, 1);

    nRepateLevel = m_ReputeList.GetReputeLevel(dwForceID);

Exit0:
    Lua_PushNumber(L, nRepateLevel);
    return 1;
}

int KPlayer::LuaIsReputationHide(Lua_State* L)
{
    BOOL    bShow           = false;
	int     nTopIndex       = 0;
	DWORD   dwForceID       = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	dwForceID = (DWORD)Lua_ValueToNumber(L, 1);

	bShow = m_ReputeList.IsShow(dwForceID);

Exit0:
	Lua_PushBoolean(L, !bShow);
	return 1;
}

int KPlayer::LuaGetReputation(Lua_State* L)
{
	DWORD dwForceID = 0;
	int nRetCode = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
	dwForceID = (DWORD)Lua_ValueToNumber(L, 1);

	nRetCode = m_ReputeList.GetReputation(dwForceID);
Exit0:
	Lua_PushNumber(L, nRetCode);
	return 1;
}

#if defined(_CLIENT)
int KPlayer::LuaSetFightFlag(Lua_State* L)
{
	DWORD dwForceID = 0;
	int nFlag = false;
	int nSuccess = false;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);
	dwForceID = (DWORD)Lua_ValueToNumber(L, 1);
	if (Lua_IsBoolean(L, 2))
	{
		nFlag = (int)Lua_ValueToBoolean(L, 2);
	}
	else
	{
		nFlag = (int)Lua_ValueToNumber(L, 2);
	}

	nSuccess = g_PlayerClient.DoApplyFightFlag(dwForceID, nFlag);
Exit0:
	Lua_PushBoolean(L, nSuccess);
	return 1;
}

int KPlayer::LuaGetFightFlag(Lua_State* L)
{
	DWORD dwForceID = 0;
	int nFlag = false;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
	dwForceID = (DWORD)Lua_ValueToNumber(L, 1);

	nFlag = m_ReputeList.GetFightFlag(dwForceID);

Exit0:
	Lua_PushBoolean(L, nFlag);
	return 1;
}

int KPlayer::LuaIsFightFlagLocked(Lua_State* L)
{
	DWORD dwForceID = 0;
	int nFlag = false;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
	dwForceID = (DWORD)Lua_ValueToNumber(L, 1);

	nFlag = m_ReputeList.IsFightFlagLocked(dwForceID);

Exit0:
	Lua_PushBoolean(L, nFlag);
	return 1;
}

#endif


//------------------------------------------------------

#if defined(_SERVER)
int KPlayer::LuaCanDoCustomOTAction(Lua_State* L)
{
    BOOL bResult = false;
    
    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
    KG_PROCESS_ERROR(m_eMoveState == cmsOnStand || m_eMoveState == cmsOnFloat);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaDoCustomOTAction(Lua_State* L)
{
	int         nRetCode            = false;
	DWORD		dwScriptID		    = 0;
	int         nType			    = 0;
	BOOL        bIsBreakable		= false;
	int         nFrame				= 0;
	int         nTargetType			= 0;
	DWORD       dwTargetID			= 0;
	const char* pszScriptFileName	= NULL;
	const char* pszActionName		= NULL;
	KTarget     Target;   

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 7);

    nType			    = (int)Lua_ValueToNumber(L, 1);
    bIsBreakable		= (BOOL)Lua_ValueToBoolean(L, 2);
    nFrame				= (int)Lua_ValueToNumber(L, 3);
    nTargetType			= (int)Lua_ValueToNumber(L, 4);
    dwTargetID			= (DWORD)Lua_ValueToNumber(L, 5);
    pszScriptFileName	= Lua_ValueToString(L, 6);
    pszActionName		= Lua_ValueToString(L, 7);
	
	KGLOG_PROCESS_ERROR(nFrame > 0 && pszScriptFileName && pszActionName);

	dwScriptID = g_FileNameHash(pszScriptFileName);

	nRetCode = g_pSO3World->m_ScriptCenter.IsScriptExist(dwScriptID);
	KGLOG_PROCESS_ERROR(nRetCode);

	switch(nTargetType)
	{
	case ttInvalid:
	case ttNoTarget:
		Target.ClearTarget(); // 置成没有目标
		break;
	case ttNpc:
	case ttPlayer:
	case ttDoodad:
	    nRetCode = Target.SetTarget((TARGET_TYPE)nTargetType, dwTargetID);
		KGLOG_PROCESS_ERROR(nRetCode);
	    break;
	default:
		KGLOG_PROCESS_ERROR(false && "Unexecpt Target Type!");
	    break;
	}

	if (nType == 0) // 非通道技能
	{
		nRetCode = DoCustomOTActionPrepare(bIsBreakable, nFrame, Target, dwScriptID);
	}
	else // 通道技
	{
		nRetCode = DoCustomOTActionChannel(bIsBreakable, nFrame, Target, dwScriptID);
	}
    KG_PROCESS_ERROR(nRetCode);

	g_PlayerServer.DoCustomOTAction(
        this, nType, bIsBreakable, nFrame, nTargetType, 
        dwTargetID, dwScriptID, pszActionName
    );

	nRetCode = true;
Exit0:
	lua_pushboolean(L, nRetCode);
	return 1;
}

int KPlayer::LuaAbradeEquipmentOnDeath(Lua_State* L)
{
    BOOL bResult        = false;
    BOOL bRetCode       = false;
    int nBoxType        = 0;
    int nAbradePercent  = 0;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);
    nBoxType = (int)Lua_ValueToNumber(L ,1);
    nAbradePercent = (int)Lua_ValueToNumber(L, 2);

    bRetCode = m_ItemList.AbradeEquipment(nBoxType, nAbradePercent);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

#endif //_SERVER

#if defined(_CLIENT)
int KPlayer::LuaTradingInviteRequest(Lua_State* L)
{
    TRADING_RESULT_CODE     nResult         = trcInvalid;
    int                     nRetCode        = 0;
    KPlayer*                pInviteDst      = NULL;
    DWORD                   dwInviteDst     = 0;
    int                     nRelation       = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwInviteDst = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(IS_PLAYER(dwInviteDst));

    assert(m_dwID == g_pSO3World->m_dwClientPlayerID);

    KG_PROCESS_ERROR_RET_CODE(m_pTradingBox == NULL, trcYouBusy);

    KG_PROCESS_ERROR_RET_CODE(m_eMoveState != cmsOnDeath, trcYouDead);

    pInviteDst = g_pSO3World->m_PlayerSet.GetObj(dwInviteDst);
    KG_PROCESS_ERROR_RET_CODE(pInviteDst, trcInvalidTarget);

    KG_PROCESS_ERROR_RET_CODE(pInviteDst->m_eMoveState != cmsOnDeath, trcTargetDead);

    KG_PROCESS_ERROR(m_dwID != dwInviteDst);
    
    KG_PROCESS_ERROR_RET_CODE(m_pScene == pInviteDst->m_pScene, trcTooFar);

    nRetCode = g_InRange(this, pInviteDst, MAX_TRADING_DISTANCE);
    KG_PROCESS_ERROR_RET_CODE(nRetCode, trcTooFar);

    nRelation = GetPlayerRelation(pInviteDst);
    KG_PROCESS_ERROR_RET_CODE(!(nRelation & sortEnemy), trcInvalidTarget);

    g_PlayerClient.DoTradingInviteRequest(dwInviteDst);
    
    nResult = trcSuccess;
Exit0:
    if (nResult != trcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventTradingRespond UINofity = { nResult };
            g_pGameWorldUIHandler->OnTradingRespond(UINofity);
        }
    }
    Lua_PushBoolean(L, (nResult == trcSuccess));
    return 1;
}

int KPlayer::LuaTradingInviteRespond(Lua_State* L)
{
    int         nRetCode    = 0;
    BOOL        bAccept     = false;
    
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);
    
    bAccept = (BOOL)Lua_ValueToBoolean(L, 1);

    assert(m_dwID == g_pSO3World->m_dwClientPlayerID);

    KG_PROCESS_ERROR(m_eMoveState != cmsOnDeath);

    g_PlayerClient.DoTradingInviteRespond(bAccept);
    
Exit0:
    return 0;
}

int KPlayer::LuaTradingAddItem(Lua_State* L)
{
    int             nResult     = trcInvalid;
    int             nRetCode    = 0;
    int             nBoxType    = 0;
    DWORD           dwBoxIndex  = 0;
    DWORD           dwPosIndex  = 0;
    DWORD           dwGridIndex = 0;
    KPlayer*        pOther      = NULL;
    KItem*          pItem       = NULL;

    KG_ASSERT_EXIT(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    dwBoxIndex = (DWORD)Lua_ValueToNumber(L, 1);
    dwPosIndex = (DWORD)Lua_ValueToNumber(L, 2);
    dwGridIndex = (DWORD)Lua_ValueToNumber(L, 3);

    assert(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(m_pTradingBox);
    KG_PROCESS_ERROR_RET_CODE(m_eMoveState != cmsOnDeath, trcYouDead);

    pOther = m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOther);
    KGLOG_PROCESS_ERROR(pOther->m_pTradingBox);
    KG_PROCESS_ERROR_RET_CODE(pOther->m_eMoveState != cmsOnDeath, trcTargetDead);

    nBoxType = m_ItemList.GetBoxType(dwBoxIndex);
    KG_PROCESS_ERROR_RET_CODE(nBoxType == ivtPackage, trcItemNotInPackage);

    pItem = m_ItemList.GetItem(dwBoxIndex, dwPosIndex);
    KGLOG_PROCESS_ERROR(pItem);
    KG_PROCESS_ERROR_RET_CODE(!pItem->IsBind(), trcItemBeBound);

    g_PlayerClient.DoTradingAddItem(dwBoxIndex, dwPosIndex, dwGridIndex);

    nResult = trcSuccess;
Exit0:
    if (nResult != trcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventTradingRespond UINofity = { nResult };
            g_pGameWorldUIHandler->OnTradingRespond(UINofity);
        }
    }
    return 0;
}
int KPlayer::LuaTradingDeleteItem(Lua_State* L)
{
    int             nRetCode    = 0;
    DWORD           dwGridIndex = 0;
    KItem*          pItem       = NULL;
    KPlayer*        pOther      = NULL;

    KG_ASSERT_EXIT(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwGridIndex = (DWORD)Lua_ValueToNumber(L, 1);

    assert(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(m_pTradingBox);
    KG_PROCESS_ERROR(m_eMoveState != cmsOnDeath);

    pOther = m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOther);
    KG_PROCESS_ERROR(pOther->m_pTradingBox);
    KG_PROCESS_ERROR(pOther->m_eMoveState != cmsOnDeath);
    
    pItem = m_pTradingBox->GetItem(dwGridIndex);
    KGLOG_PROCESS_ERROR(pItem);

    nRetCode = g_PlayerClient.DoTradingDeleteItem(dwGridIndex);
    KGLOG_PROCESS_ERROR(nRetCode);
    
Exit0:
    return 0;
}

int KPlayer::LuaTradingSetMoney(Lua_State* L)
{
    int             nResult         = trcFailed;
    int             nRetCode        = 0;
    int             nMoney          = 0;
    int             nPlayerMoney    = 0;
    KPlayer*        pOther          = NULL;

    KG_ASSERT_EXIT(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nMoney = (int)Lua_ValueToNumber(L, 1);

    assert(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(m_pTradingBox);
    KG_PROCESS_ERROR(m_eMoveState != cmsOnDeath);

    pOther = m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOther);
    KGLOG_PROCESS_ERROR(pOther->m_pTradingBox);
    KG_PROCESS_ERROR(pOther->m_eMoveState != cmsOnDeath);

    nPlayerMoney = m_ItemList.GetMoney();
    KG_PROCESS_ERROR_RET_CODE(nMoney <= nPlayerMoney, trcNotEnoughMoney);
   
    nRetCode = g_PlayerClient.DoTradingSetMoney(nMoney);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = trcSuccess;
Exit0:
    if (nResult != trcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventTradingRespond UINofity = { nResult };
		    g_pGameWorldUIHandler->OnTradingRespond(UINofity);
        }
    }
    return 0;
}

int KPlayer::LuaTradingConfirm(Lua_State* L)
{
    int             nRetCode    = 0;
    BOOL            bConfirm    = false;
    KPlayer*        pOther      = NULL;
    int             nKey        = 0;

    KG_ASSERT_EXIT(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bConfirm = (BOOL)Lua_ValueToBoolean(L, 1);

    assert(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(m_pTradingBox);
    
    pOther = m_pTradingBox->GetOtherSide();
    KGLOG_PROCESS_ERROR(pOther);
    KGLOG_PROCESS_ERROR(pOther->m_pTradingBox);

    if (bConfirm)
    {
        m_pTradingBox->Confirm();
        nKey = pOther->m_pTradingBox->GetKey();
    }
    else
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventTradingClose TradingClose;
            g_pGameWorldUIHandler->OnTradingClose(TradingClose);

            KUIEventTradingRespond TradingRespond = { trcTradingCancel };
            g_pGameWorldUIHandler->OnTradingRespond(TradingRespond);
        }

        m_pTradingBox->Cancel();
    }
   
    nRetCode = g_PlayerClient.DoTradingConfirm(bConfirm, nKey);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

int KPlayer::LuaGetTradingItem(Lua_State* L)
{
    int     nResult     = 0;
    int     nRetCode    = 0;
    DWORD   dwGridIndex = 0;
    KItem*  pItem       = NULL;

    KG_ASSERT_EXIT(L);
    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwGridIndex = (DWORD)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(m_pTradingBox);

    KG_PROCESS_ERROR(dwGridIndex < MAX_TRADING_BOX_SIZE);
    pItem = m_pTradingBox->GetItem(dwGridIndex);
    KG_PROCESS_ERROR(pItem);
    
    nResult = pItem->LuaGetObj(L);   
Exit0:
    return nResult;
}

int KPlayer::LuaGetTradingMoney(Lua_State* L)
{
    int nResult  = 0;
    int nMoney   = 0;

    KG_ASSERT_EXIT(L);

    KG_PROCESS_ERROR(m_pTradingBox);
    nMoney = m_pTradingBox->GetMoney();
    
    Lua_PushNumber(L, nMoney);
    
    nResult = 1;
Exit0:
    return nResult;
}

#endif


//--- Talk --------------------------------
#ifdef _CLIENT
static inline int _FillTalkData(Lua_State* L, int nTableIndex, BYTE* pbyBuf, unsigned uBufSize)
{
    int     nResult       = 0;
    int     nRetCode      = false;
    BYTE*   pbyBufEnd     = NULL;
    char*   pszType       = NULL;
    int     nRestore      = 0;
    int     nRestoreExit  = 0;
    DWORD   dwItemID      = 0;

    KGLOG_PROCESS_ERROR(L);
    KGLOG_PROCESS_ERROR(nTableIndex > 0);
    KGLOG_PROCESS_ERROR(pbyBuf);
    KGLOG_PROCESS_ERROR(uBufSize > 0);

    nRestoreExit = Lua_GetTopIndex(L);
    pbyBufEnd = pbyBuf;

    nRetCode = Lua_IsTable(L, nRestoreExit);
    KGLOG_PROCESS_ERROR(nRetCode);

    lua_pushnil(L);
    nRestore = Lua_GetTopIndex(L);
    while (Lua_Next(L, nTableIndex))
    {
        if (!Lua_IsTable(L, -1))
        {
            Lua_SetTopIndex(L, nRestore);
            continue;
        }

        lua_getfield(L, -1, "type");
        if (!lua_isstring(L, -1))
        {
            Lua_SetTopIndex(L, nRestore);
            continue;
        }

        pszType = (char*)Lua_ValueToString(L, -1);
		Lua_Pop(L, 1);

        if (!strcmp(pszType, "text"))
        {
            char*   pszText     = NULL;
            BYTE*   pbyBufText  = NULL;
            size_t  uTextLen    = 0;
            size_t  uTextNewLen = 0;

            lua_getfield(L, -1, "text");
            pszText = (char*)Lua_ValueToString(L, -1);
            KGLOG_PROCESS_ERROR(pszText);

            uTextLen = strlen(pszText);
            uTextNewLen = uTextLen;

            if ((pbyBufEnd + sizeof(BYTE) + uTextNewLen + sizeof('\0')) > pbyBuf + uBufSize)
            {
                uTextNewLen = uBufSize - (unsigned)(pbyBufEnd - pbyBuf) - sizeof(BYTE) - (sizeof(char) * 3) - sizeof('\0');
            }

            *pbyBufEnd = (BYTE)tdbText;
            pbyBufEnd += sizeof(BYTE);
            pbyBufText = pbyBufEnd;
            memcpy(pbyBufEnd, pszText, uTextNewLen);
            pbyBufEnd += uTextNewLen;
            if (uTextNewLen < uTextLen)
            {
                memcpy(pbyBufEnd, "...", 3);
                pbyBufEnd += 3;
            }
            *pbyBufEnd = '\0';

            nRetCode = g_pSO3World->m_piTextFilter->Replace((char*)pbyBufText);
            KGLOG_PROCESS_ERROR(nRetCode);

            pbyBufEnd = pbyBufText + strlen((const char*)pbyBufText) + sizeof('\0');
        }

        if (!strcmp(pszType, "name"))
        {
            char *pszName   = NULL;
            size_t uTextLen = 0;

            lua_getfield(L, -1, "name");
            pszName = (char*)Lua_ValueToString(L, -1);
            KGLOG_PROCESS_ERROR(pszName);

            uTextLen = strlen(pszName);
            KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE) + uTextLen + sizeof('\0')) > pbyBuf + uBufSize);

            *pbyBufEnd = (BYTE)tdbLinkName;
            pbyBufEnd += sizeof(BYTE);
            memcpy(pbyBufEnd, pszName, uTextLen);
            pbyBufEnd += uTextLen;
            *pbyBufEnd = '\0';
            pbyBufEnd += sizeof('\0');
        }

        if (!strcmp(pszType, "item"))
        {
            DWORD dwItemID  = 0;
            KItem* pItem = NULL;
            BYTE   byItemBuffer[256];
            struct TALK_SYNC_ITEM* pTalkSyncItem = (struct TALK_SYNC_ITEM*)byItemBuffer;
            assert(pTalkSyncItem);
            pTalkSyncItem->uDataSize = 256 - sizeof(int) - sizeof(unsigned);

            lua_getfield(L, -1, "item");
            dwItemID = (DWORD)Lua_ValueToNumber(L, -1);
            KGLOG_PROCESS_ERROR(dwItemID);

            pItem = g_pSO3World->m_ItemSet.GetObj(dwItemID);
            KGLOG_PROCESS_ERROR(pItem);

            nRetCode = pItem->GetBinaryData(&pTalkSyncItem->uDataSize, pTalkSyncItem->byData, pTalkSyncItem->uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode);

            KGLOG_PROCESS_ERROR(pTalkSyncItem->uDataSize <= (256 - sizeof(struct TALK_SYNC_ITEM)));
            KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE) + sizeof(struct TALK_SYNC_ITEM) + pTalkSyncItem->uDataSize) > pbyBuf + uBufSize);

            *pbyBufEnd = (BYTE)tdbLinkItem;
            pbyBufEnd += sizeof(BYTE);
            memcpy(pbyBufEnd, pTalkSyncItem, sizeof(struct TALK_SYNC_ITEM) + pTalkSyncItem->uDataSize);
			pbyBufEnd += (sizeof(struct TALK_SYNC_ITEM) + pTalkSyncItem->uDataSize);
        }

		if (!strcmp(pszType, "iteminfo"))
		{
			int	nVersion = 0;
			DWORD dwTabType = 0;
			DWORD dwIndex = 0;

			lua_getfield(L, -1, "version");
			nVersion = (int)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "tabtype");
			dwTabType = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "index");
			dwIndex = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE) + sizeof(nVersion) + sizeof(dwTabType) + sizeof(dwIndex)) > pbyBuf + uBufSize);

			*pbyBufEnd = (BYTE)tdbLinkItemInfo;
			pbyBufEnd += sizeof(BYTE);
			memcpy(pbyBufEnd, &nVersion, sizeof(nVersion));
			pbyBufEnd += sizeof(nVersion);
			memcpy(pbyBufEnd, &dwTabType, sizeof(dwTabType));
			pbyBufEnd += sizeof(dwTabType);
			memcpy(pbyBufEnd, &dwIndex, sizeof(dwIndex));
			pbyBufEnd += sizeof(dwIndex);
		}

		if (!strcmp(pszType, "quest"))
		{
			DWORD dwQuestID = 0;

			lua_getfield(L, -1, "questid");
			dwQuestID = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE) + sizeof(dwQuestID)) > pbyBuf + uBufSize);

			*pbyBufEnd = (BYTE)tdbLinkQuest;
			pbyBufEnd += sizeof(BYTE);
			memcpy(pbyBufEnd, &dwQuestID, sizeof(dwQuestID));
			pbyBufEnd += sizeof(dwQuestID);
		}

		if (!strcmp(pszType, "recipe"))
		{
			DWORD dwCraftID = 0;
			DWORD dwRecipeID = 0;

			lua_getfield(L, -1, "craftid");
			dwCraftID = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "recipeid");
			dwRecipeID = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE) + sizeof(dwCraftID) + sizeof(dwRecipeID)) > pbyBuf + uBufSize);

			*pbyBufEnd = (BYTE)tdbLinkRecipe;
			pbyBufEnd += sizeof(BYTE);
			memcpy(pbyBufEnd, &dwCraftID, sizeof(dwCraftID));
			pbyBufEnd += sizeof(dwCraftID);
			memcpy(pbyBufEnd, &dwRecipeID, sizeof(dwRecipeID));
			pbyBufEnd += sizeof(dwRecipeID);
		}

        if (!strcmp(pszType, "enchant"))
        {
            DWORD dwProID = 0;
            DWORD dwCraftID = 0;
            DWORD dwRecipeID = 0;

            lua_getfield(L, -1, "proid");
            dwProID = (DWORD)Lua_ValueToNumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "craftid");
            dwCraftID = (DWORD)Lua_ValueToNumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "recipeid");
            dwRecipeID = (DWORD)Lua_ValueToNumber(L, -1);
            lua_pop(L, 1);

            KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE) + sizeof(dwProID) + sizeof(dwCraftID) + sizeof(dwRecipeID)) > pbyBuf + uBufSize);

            *pbyBufEnd = (BYTE)tdbLinkEnchant;
            pbyBufEnd += sizeof(BYTE);
            memcpy(pbyBufEnd, &dwProID, sizeof(dwProID));
            pbyBufEnd += sizeof(dwProID);
            memcpy(pbyBufEnd, &dwCraftID, sizeof(dwCraftID));
            pbyBufEnd += sizeof(dwCraftID);
            memcpy(pbyBufEnd, &dwRecipeID, sizeof(dwRecipeID));
            pbyBufEnd += sizeof(dwRecipeID);
        }

		if (!strcmp(pszType, "skill"))
		{
			KSKILL_RECIPE_KEY skillKey;
			char szRecipeKey[32];

			lua_getfield(L, -1, "skill_id");
			skillKey.dwSkillID = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "skill_level");
			skillKey.dwSkillLevel = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			for (int i = 0; i < MAX_SKILL_REICPE_COUNT; i++)
			{
				snprintf(szRecipeKey, sizeof(szRecipeKey), "recipe%d", i + 1);
				szRecipeKey[sizeof(szRecipeKey) - 1] = 0;
				lua_getfield(L, -1, szRecipeKey);
				skillKey.dwRecipeKey[i] = (DWORD)Lua_ValueToNumber(L, -1);
				lua_pop(L, 1);
			}
			std::sort(skillKey.dwRecipeKey, (skillKey.dwRecipeKey) + MAX_SKILL_REICPE_COUNT);

			KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE) + sizeof(KSKILL_RECIPE_KEY)) > pbyBuf + uBufSize);

			*pbyBufEnd = (BYTE)tdbLinkSkill;
			pbyBufEnd += sizeof(BYTE);
			memcpy(pbyBufEnd, &skillKey, sizeof(KSKILL_RECIPE_KEY));
			pbyBufEnd += sizeof(KSKILL_RECIPE_KEY);
		}

		if (!strcmp(pszType, "skillrecipe"))
		{
			DWORD dwID = 0;
			DWORD dwLevel = 0;

			lua_getfield(L, -1, "id");
			dwID = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "level");
			dwLevel = (DWORD)Lua_ValueToNumber(L, -1);
			lua_pop(L, 1);

			KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE) + sizeof(dwID) + sizeof(dwLevel)) > pbyBuf + uBufSize);

			*pbyBufEnd = (BYTE)tdbLinkSkillRecipe;
			pbyBufEnd += sizeof(BYTE);
			memcpy(pbyBufEnd, &dwID, sizeof(dwID));
			pbyBufEnd += sizeof(dwID);
			memcpy(pbyBufEnd, &dwLevel, sizeof(dwLevel));
			pbyBufEnd += sizeof(dwLevel);
		}

		if (!strcmp(pszType, "emotion"))
		{
			KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE)) > pbyBuf + uBufSize);

			*pbyBufEnd = (BYTE)tdbEmotion;
			pbyBufEnd += sizeof(BYTE);
		}

        if (!strcmp(pszType, "book"))
        {
            int	nVersion = 0;
            DWORD dwTabType = 0;
            DWORD dwIndex = 0;
            int nBookRecipeID = 0;
              
            lua_getfield(L, -1, "version");
            nVersion = (int)Lua_ValueToNumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "tabtype");
            dwTabType = (DWORD)Lua_ValueToNumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "index");
            dwIndex = (DWORD)Lua_ValueToNumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "bookinfo");
            nBookRecipeID = (DWORD)Lua_ValueToNumber(L, -1);
            lua_pop(L, 1);

            KGLOG_PROCESS_SUCCESS((pbyBufEnd + sizeof(BYTE)) + sizeof(nVersion) + sizeof(dwTabType) + sizeof(dwIndex) + sizeof(nBookRecipeID) > pbyBuf + uBufSize);

            *pbyBufEnd = (BYTE)tdbLinkBook;
            pbyBufEnd += sizeof(BYTE);
            memcpy(pbyBufEnd, &nVersion, sizeof(nVersion));
            pbyBufEnd += sizeof(nVersion);
            memcpy(pbyBufEnd, &dwTabType, sizeof(dwTabType));
            pbyBufEnd += sizeof(dwTabType);
            memcpy(pbyBufEnd, &dwIndex, sizeof(dwIndex));
            pbyBufEnd += sizeof(dwIndex);
            memcpy(pbyBufEnd, &nBookRecipeID, sizeof(nBookRecipeID));
            pbyBufEnd += sizeof(nBookRecipeID);
        }


        Lua_SetTopIndex(L, nRestore);
    }

Exit1:
    nResult = (int)(pbyBufEnd - pbyBuf);
Exit0:
    if (nRestoreExit != 0)
    {
        Lua_SetTopIndex(L, nRestoreExit);
    }
    return nResult;
}

int KPlayer::LuaGetTalkData(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = false;
    BYTE*   pbyTalkData     = NULL;
    BYTE*   pbyTalkDataEnd  = NULL;
    int     nTalkDataSize   = 0;
    int     nTable          = 0;
    int     nIndex          = 0;
    KItem*  pDestItem       = NULL;
	int		nParamCount		= Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nParamCount == 0);
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(s_uTalkDataSize > 0 && s_uTalkDataSize <= MAX_CHAT_TEXT_LEN);

    pbyTalkData    = s_byTalkData;
    pbyTalkDataEnd = s_byTalkData + s_uTalkDataSize;

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    while (pbyTalkData != pbyTalkDataEnd)
    {
        BYTE byBlockType = (BYTE)*pbyTalkData;
        pbyTalkData += sizeof(BYTE);

        switch(byBlockType)
        {
        case tdbText:
            {
                int    nTextTable = 0;
                size_t uTextLen   = 0;
                char*  pszText    = (char*)pbyTalkData;
                KGLOG_PROCESS_ERROR(pszText);

                uTextLen = strlen(pszText);

                if (s_bFilterTalkText)
                {
                    nRetCode = g_pSO3World->m_piTextFilter->Replace(pszText);
                    KGLOG_PROCESS_ERROR(nRetCode);
                }

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nTextTable = Lua_GetTopIndex(L);
                
                Lua_PushString(L, "type");
                Lua_PushString(L, "text");
                Lua_SetTable(L, nTextTable);

                Lua_PushString(L, "text");
                Lua_PushString(L, pszText);
                Lua_SetTable(L, nTextTable);

                Lua_SetTable(L, nTable);

                pbyTalkData += (uTextLen + sizeof('\0'));
            }
            break;

        case tdbLinkName:
            {
                int    nLinkNameTable = 0;
                size_t uNameLen       = 0;
                char*  pszName        = (char*)pbyTalkData;
                KGLOG_PROCESS_ERROR(pszName);

                uNameLen = strlen(pszName);

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkNameTable = Lua_GetTopIndex(L);
                
                Lua_PushString(L, "type");
                Lua_PushString(L, "name");
                Lua_SetTable(L, nLinkNameTable);

                Lua_PushString(L, "name");
                Lua_PushString(L, pszName);
                Lua_SetTable(L, nLinkNameTable);

                Lua_SetTable(L, nTable);

                pbyTalkData += (uNameLen + sizeof('\0'));
            }
            break;

        case tdbLinkItem:
            {
                BOOL    bRetCode        = false;
                int     nLinkItemTable  = 0;
                DWORD   dwItemID        = 0;
                int     nItemID         = 0;
                struct  TALK_SYNC_ITEM* pTalkSyncItem = (TALK_SYNC_ITEM*)pbyTalkData;
                
                pDestItem = g_pSO3World->m_ItemManager.GenerateItemFromBinaryData(ERROR_ID, pTalkSyncItem->byData, pTalkSyncItem->uDataSize);
                KGLOG_PROCESS_ERROR(pDestItem);
                
                dwItemID = pDestItem->m_dwID;

                //If queue is full, then delete the first one.
                if (g_pSO3World->m_LinkItemQue.size() >= CLIENT_TALK_LINK_ITEM_BUFFER_SIZE)
                {
                    KItem* pDeleteItem = g_pSO3World->m_LinkItemQue.front();
                    if (pDeleteItem)
                    {
                        pDeleteItem->UnInit();
						g_pSO3World->m_ItemSet.Unregister(pDeleteItem);
						KMemory::Delete(pDeleteItem);
                        pDeleteItem = NULL;
                    }
                    g_pSO3World->m_LinkItemQue.pop_front();
                }

                g_pSO3World->m_LinkItemQue.push_back(pDestItem);
                pDestItem = NULL;

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkItemTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "item");
                Lua_SetTable(L, nLinkItemTable);

                nItemID = (int)dwItemID;
                Lua_PushString(L, "item");
                Lua_PushNumber(L, nItemID);
                Lua_SetTable(L, nLinkItemTable);

                Lua_SetTable(L, nTable);
                
                pbyTalkData += (sizeof(TALK_SYNC_ITEM) + pTalkSyncItem->uDataSize);
            }
            break;

		case tdbLinkItemInfo:
			{
				int		                nLinkItemInfoTable	= 0;
                TALK_SYNC_ITEM_INFO*    pTalkSyncItemInfo   = (TALK_SYNC_ITEM_INFO*)pbyTalkData;

				pbyTalkData += sizeof(TALK_SYNC_ITEM_INFO);

				Lua_PushNumber(L, ++nIndex);
				Lua_NewTable(L);
				nLinkItemInfoTable = Lua_GetTopIndex(L);

				Lua_PushString(L, "type");
				Lua_PushString(L, "iteminfo");
				Lua_SetTable(L, nLinkItemInfoTable);

				Lua_PushString(L, "version");
				Lua_PushNumber(L, pTalkSyncItemInfo->nVersion);
				Lua_SetTable(L, nLinkItemInfoTable);

				Lua_PushString(L, "tabtype");
				Lua_PushNumber(L, pTalkSyncItemInfo->dwTabType);
				Lua_SetTable(L, nLinkItemInfoTable);

				Lua_PushString(L, "index");
				Lua_PushNumber(L, pTalkSyncItemInfo->dwIndex);
				Lua_SetTable(L, nLinkItemInfoTable);

				Lua_SetTable(L, nTable);
			}
			break;

		case tdbLinkQuest:
			{
				int		            nLinkQuestTable	= 0;
                TALK_SYNC_QUEST*    pTalkSyncQuest  = (TALK_SYNC_QUEST*)pbyTalkData;

                pbyTalkData += sizeof(TALK_SYNC_QUEST);

				Lua_PushNumber(L, ++nIndex);
				Lua_NewTable(L);
				nLinkQuestTable = Lua_GetTopIndex(L);

				Lua_PushString(L, "type");
				Lua_PushString(L, "quest");
				Lua_SetTable(L, nLinkQuestTable);

				Lua_PushString(L, "questid");
				Lua_PushNumber(L, pTalkSyncQuest->dwQuestID);
				Lua_SetTable(L, nLinkQuestTable);

				Lua_SetTable(L, nTable);
			}
			break;

		case tdbLinkRecipe:
			{
				int		            nLinkRecipeTable    = 0;
                TALK_SYNC_RECIPE*   pTalkSyncRecipe     = (TALK_SYNC_RECIPE*)pbyTalkData;

				pbyTalkData += sizeof(TALK_SYNC_RECIPE);

				Lua_PushNumber(L, ++nIndex);
				Lua_NewTable(L);
				nLinkRecipeTable = Lua_GetTopIndex(L);

				Lua_PushString(L, "type");
				Lua_PushString(L, "recipe");
				Lua_SetTable(L, nLinkRecipeTable);

				Lua_PushString(L, "craftid");
				Lua_PushNumber(L, pTalkSyncRecipe->dwCraftID);
				Lua_SetTable(L, nLinkRecipeTable);

				Lua_PushString(L, "recipeid");
				Lua_PushNumber(L, pTalkSyncRecipe->dwRecipeID);
				Lua_SetTable(L, nLinkRecipeTable);

				Lua_SetTable(L, nTable);
			}
			break;

        case tdbLinkEnchant:
            {
                int		            nLinkEnchantTable	= 0;
                TALK_SYNC_ENCHANT*  pTalkSyncEnchant    = (TALK_SYNC_ENCHANT*)pbyTalkData;

                pbyTalkData += sizeof(TALK_SYNC_ENCHANT);

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nLinkEnchantTable = Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "enchant");
                Lua_SetTable(L, nLinkEnchantTable);

                Lua_PushString(L, "proid");
                Lua_PushNumber(L, pTalkSyncEnchant->dwProID);
                Lua_SetTable(L, nLinkEnchantTable);

                Lua_PushString(L, "craftid");
                Lua_PushNumber(L, pTalkSyncEnchant->dwCraftID);
                Lua_SetTable(L, nLinkEnchantTable);

                Lua_PushString(L, "recipeid");
                Lua_PushNumber(L, pTalkSyncEnchant->dwRecipeID);
                Lua_SetTable(L, nLinkEnchantTable);

                Lua_SetTable(L, nTable);
            }
            break;

		case tdbLinkSkill:
			{
				int		            nLinkSkillTable		= 0;
				KSKILL_RECIPE_KEY*  pSkillKey           = (KSKILL_RECIPE_KEY*)pbyTalkData;
				char                szRecipeKey[32];

				pbyTalkData += sizeof(KSKILL_RECIPE_KEY);

				Lua_PushNumber(L, ++nIndex);
				Lua_NewTable(L);
				nLinkSkillTable = Lua_GetTopIndex(L);

				Lua_PushString(L, "type");
				Lua_PushString(L, "skill");
				Lua_SetTable(L, nLinkSkillTable);

				Lua_PushString(L, "skill_id");
				Lua_PushNumber(L, pSkillKey->dwSkillID);
				Lua_SetTable(L, nLinkSkillTable);

				Lua_PushString(L, "skill_level");
				Lua_PushNumber(L, pSkillKey->dwSkillLevel);
				Lua_SetTable(L, nLinkSkillTable);

				for (int i = 0; i < MAX_SKILL_REICPE_COUNT; i++)
				{
					snprintf(szRecipeKey, sizeof(szRecipeKey), "recipe%d", i + 1);
					szRecipeKey[sizeof(szRecipeKey) - 1] = 0;

					Lua_PushString(L, szRecipeKey);
					Lua_PushNumber(L, pSkillKey->dwRecipeKey[i]);
					Lua_SetTable(L, nLinkSkillTable);
				}

				Lua_SetTable(L, nTable);
			}
			break;

		case tdbLinkSkillRecipe:
			{
				int		                nLinkSkillRecipeTable   = 0;
                TALK_SYNC_SKILL_RECIPE* pTalkSyncSkillRecipe    = (TALK_SYNC_SKILL_RECIPE*)pbyTalkData;

                pbyTalkData += sizeof(TALK_SYNC_SKILL_RECIPE);

				Lua_PushNumber(L, ++nIndex);
				Lua_NewTable(L);
				nLinkSkillRecipeTable = Lua_GetTopIndex(L);

				Lua_PushString(L, "type");
				Lua_PushString(L, "skillrecipe");
				Lua_SetTable(L, nLinkSkillRecipeTable);

				Lua_PushString(L, "id");
				Lua_PushNumber(L, pTalkSyncSkillRecipe->dwID);
				Lua_SetTable(L, nLinkSkillRecipeTable);

				Lua_PushString(L, "level");
				Lua_PushNumber(L, pTalkSyncSkillRecipe->dwLevel);
				Lua_SetTable(L, nLinkSkillRecipeTable);

				Lua_SetTable(L, nTable);
			}
			break;
		case tdbEmotion:
			{
				Lua_PushNumber(L, ++nIndex);
				Lua_NewTable(L);

				Lua_PushString(L, "type");
				Lua_PushString(L, "emotion");
				Lua_SetTable(L, -3);

				Lua_SetTable(L, nTable);
			}
			break;
        case tdbLinkBook:
            {
                int nBookTable = 0;
                TALK_SYNC_BOOK *pTalkSyncBook = (TALK_SYNC_BOOK*)pbyTalkData;
                
                pbyTalkData += sizeof(TALK_SYNC_BOOK);

                Lua_PushNumber(L, ++nIndex);
                Lua_NewTable(L);
                nBookTable =  Lua_GetTopIndex(L);

                Lua_PushString(L, "type");
                Lua_PushString(L, "book");
                Lua_SetTable(L, nBookTable);

                Lua_PushString(L, "version");
                Lua_PushNumber(L, pTalkSyncBook->ItemInfo.nVersion);
                Lua_SetTable(L, nBookTable);

                Lua_PushString(L, "tabtype");
                Lua_PushNumber(L, pTalkSyncBook->ItemInfo.dwTabType);
                Lua_SetTable(L, nBookTable);

                Lua_PushString(L, "index");
                Lua_PushNumber(L, pTalkSyncBook->ItemInfo.dwIndex);
                Lua_SetTable(L, nBookTable);

                Lua_PushString(L, "bookinfo");
                Lua_PushNumber(L,  pTalkSyncBook->nBookRecipeID);
                Lua_SetTable(L, nBookTable);

                Lua_SetTable(L, nTable);
            }
            break;
        default:
            KGLogPrintf(KGLOG_ERR, "Unexpected talk data: %d\n", byBlockType);
            goto Exit0;
        }
    }

    return 1;
Exit0:
	lua_settop(L, nParamCount);
    if (pDestItem)
    {
        pDestItem->UnInit();
		g_pSO3World->m_ItemSet.Unregister(pDestItem);
		KMemory::Delete(pDestItem);
        pDestItem = NULL;
    }
    return 0;
}

int KPlayer::LuaGetTalkLinkItem(Lua_State* L)
{
    int    nResult  = 0;
    int    nRetCode = false;
    DWORD  dwItemID = ERROR_ID;
    KItem* pItem    = NULL;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	dwItemID = (DWORD)Lua_ValueToNumber(L, 1);

	pItem = g_pSO3World->m_ItemSet.GetObj(dwItemID);
	KGLOG_PROCESS_ERROR(pItem);

	return pItem->LuaGetObj(L);
Exit0:
	return nResult;
}

int KPlayer::LuaTalk(Lua_State* L)
{
//    BOOL        bResult         = false;
//    int         nRetCode        = false;
//    int         nTalkRange      = trInvalid;
//    const char* pszReceiver     = NULL;
//    int         nTalkDataSize   = 0;
//    BYTE        byTalkData[MAX_CHAT_TEXT_LEN];
//
//    nRetCode = Lua_GetTopIndex(L);
//    KGLOG_PROCESS_ERROR(nRetCode == 3);
//
//    nTalkRange = (int)Lua_ValueToNumber(L, 1);
//    KGLOG_PROCESS_ERROR(nTalkRange > trInvalid && nTalkRange < trTotal);
//
//    pszReceiver = (const char*)Lua_ValueToString(L, 2);
//    KGLOG_PROCESS_ERROR(pszReceiver);
//
//    if (nTalkRange == trSence)  // 场景聊天检测CD
//    {
//        nRetCode = m_TimerList.CheckTimer(g_pSO3World->m_Settings.m_ConstList.nPlayerSceneTalkCD);
//        if (!nRetCode)
//        {
//            if (g_pGameWorldUIHandler)
//            {
//                KUIEventResponseMsg UIEventResponseMsg;
//                UIEventResponseMsg.nRespondCode = TALK_ERROR_E_SCENE_CD;
//                g_pGameWorldUIHandler->OnResponseMsg(UIEventResponseMsg);
//            }
//
//            goto Exit0;
//        }
//    }
//
//    nTalkDataSize = _FillTalkData(L, 3, byTalkData, MAX_CHAT_TEXT_LEN);
//    KGLOG_PROCESS_ERROR(nTalkDataSize > 0);
//
//    nRetCode = g_PlayerClient.DoTalkMessage(nTalkRange, pszReceiver, (size_t)nTalkDataSize, byTalkData);
//    KGLOG_PROCESS_ERROR(nRetCode);
//
//    bResult = true;
//Exit0:
//    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaDoAnimation(Lua_State* L)
{
    BOOL    bResult     = false;
    DWORD   dwAnimation = 0;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    dwAnimation = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAnimation != 0);

    if (g_pGameWorldRepresentHandler)
    {
        g_pGameWorldRepresentHandler->OnCharacterPlayAnimation(this, dwAnimation);
    }

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaIsInParty(Lua_State* L)
{
    BOOL bResult = false;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);
    
    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaIsPlayerInMyParty(Lua_State* L)
{
	return g_pSO3World->m_TeamClient.LuaIsPlayerInTeam(L);
}

int KPlayer::LuaIsPartyLeader(Lua_State* L)
{
	BOOL    bIsLeader   = false;

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);
    KG_PROCESS_ERROR(m_dwID == g_pSO3World->m_TeamClient.m_dwAuthority[tatLeader]);

    bIsLeader = true;
Exit0:
    Lua_PushBoolean(L, bIsLeader);
    return 1;
}

int KPlayer::LuaIsPartyFull(Lua_State* L)
{
    BOOL                bResult         = false;
    size_t              uPlayerCount    = 0;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    for (int i = 0; i < g_pSO3World->m_TeamClient.m_nGroupNum; i++)
    {
        if (g_pSO3World->m_TeamClient.m_TeamGroup[i].MemberList.size() < MAX_PLAYER_PER_GROUP)
        {
            goto Exit0;
        }
    }

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
    return 1;
}

#endif

//////////////////// 好友相关脚本接口 ////////////////////////////////////
int KPlayer::LuaDelFellowship(Lua_State* L)
{
    int             nResult             = rrcFailed;
    int             nRetCode            = false;
    DWORD           dwAlliedPlayerID    = 0;
    KGFellowship*   pFellowship         = NULL;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);
#endif

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, dwAlliedPlayerID);
    KGLOG_PROCESS_ERROR_RET_CODE(pFellowship, rrcFellowshipNotFound);

    g_pSO3World->m_FellowshipMgr.DelFellowship(m_dwID, dwAlliedPlayerID);

#ifdef _CLIENT
    g_PlayerClient.DoDelFellowshipRequest(dwAlliedPlayerID, 1);
#endif

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnFellowshipUpdate();
#endif

    nResult = rrcSuccess;
Exit0:
#ifdef _CLIENT
    if (nResult != rrcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventFellowshipRespond UIParam = { nResult };
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }
#endif
    lua_pushboolean(L, (nResult == rrcSuccess));
    return 1;
}

int KPlayer::LuaDelFoe(Lua_State* L)
{
    int     nResult             = rrcFailed;
    int     nRetCode            = false;
    DWORD   dwAlliedPlayerID    = 0;
    KGFoe*  pFoe                = NULL;
    
#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);
#endif

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    KG_PROCESS_ERROR_RET_CODE(!m_bFightState, rrcInFight);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pFoe = g_pSO3World->m_FellowshipMgr.GetFoe(m_dwID, dwAlliedPlayerID);
    KGLOG_PROCESS_ERROR_RET_CODE(pFoe, rrcFoeNotFound);

    g_pSO3World->m_FellowshipMgr.DelFoe(m_dwID, dwAlliedPlayerID);

#ifdef _CLIENT
    g_PlayerClient.DoDelFellowshipRequest(dwAlliedPlayerID, 2);
#endif

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
    {
        KUIEventUpdateRelation UIEventUpdateRelation;

        g_pGameWorldUIHandler->OnFoeUpdate();

        UIEventUpdateRelation.dwCharacterID = dwAlliedPlayerID;
        g_pGameWorldUIHandler->OnUpdateRelation(UIEventUpdateRelation);
    }

    if (g_pGameWorldRepresentHandler)
    {
        KPlayer* pDestPlayer = g_pSO3World->m_PlayerSet.GetObj(dwAlliedPlayerID);
        if (pDestPlayer)
        {
            g_pGameWorldRepresentHandler->OnCharacterUpdateRelation(pDestPlayer);  
        }
    }
#endif

    nResult = rrcSuccess;
Exit0:
#ifdef _CLIENT
    if (nResult != rrcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventFellowshipRespond UIParam = { nResult };
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }
#endif
    lua_pushboolean(L, (nResult == rrcSuccess));
    return 1;
}

int KPlayer::LuaDelBlackList(Lua_State* L)
{
    int             nResult             = rrcFailed;
    int             nRetCode            = false;
    DWORD           dwAlliedPlayerID    = 0;
    KGBlackNode*    pBlackNode          = NULL;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);
#endif

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pBlackNode = g_pSO3World->m_FellowshipMgr.GetBlackListNode(m_dwID, dwAlliedPlayerID);
    KGLOG_PROCESS_ERROR_RET_CODE(pBlackNode, rrcBlackListNotFound);

    g_pSO3World->m_FellowshipMgr.DelBlackList(m_dwID, dwAlliedPlayerID);

#ifdef _CLIENT
    g_PlayerClient.DoDelFellowshipRequest(dwAlliedPlayerID, 3);
#endif

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnBlackListUpdate();
#endif

    nResult = rrcSuccess;
Exit0:
#ifdef _CLIENT
    if (nResult != rrcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventFellowshipRespond UIParam = { nResult };
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }
#endif
    lua_pushboolean(L, (nResult == rrcSuccess));
    return 1;
}

int KPlayer::LuaAddFellowshipGroup(Lua_State* L)
{
    BOOL    bResult         = false;
    int     nRetCode        = false;
    char*   pszGroupName    = NULL;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);
#endif

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pszGroupName = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszGroupName);

    g_pSO3World->m_FellowshipMgr.AddFellowshipGroup(m_dwID, pszGroupName);
#ifdef _CLIENT
    g_PlayerClient.DoAddFellowshipGroupRequest(pszGroupName);
#endif

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnFellowshipUpdate();
#endif

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KPlayer::LuaDelFellowshipGroup(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nRetCode    = false;
    DWORD   dwGroupID   = 0;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);
#endif

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    dwGroupID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwGroupID > 0);
	dwGroupID = dwGroupID - 1;
    KGLOG_PROCESS_ERROR(dwGroupID <= KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    g_pSO3World->m_FellowshipMgr.DelFellowshipGroup(m_dwID, dwGroupID);

#ifdef _CLIENT
    g_PlayerClient.DoDelFellowshipGroupRequest(dwGroupID);
#endif

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnFellowshipUpdate();
#endif

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KPlayer::LuaRenameFellowshipGroup(Lua_State* L)
{
    int     nResult         = rrcFailed;
    int     nRetCode        = false;
    DWORD   dwGroupID       = 0;
    char*   pszGroupName    = NULL;
    size_t  uStrLen         = 0;

#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);
#endif

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwGroupID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwGroupID > 0);
	dwGroupID = dwGroupID - 1;
    KGLOG_PROCESS_ERROR(dwGroupID <= KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    pszGroupName = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszGroupName);

    uStrLen = strlen(pszGroupName);
    KG_PROCESS_ERROR_RET_CODE(uStrLen && uStrLen <= _NAME_LEN, rrcInvalidGroupName);

    g_pSO3World->m_FellowshipMgr.RenameFellowshipGroup(m_dwID, dwGroupID, pszGroupName);

#ifdef _CLIENT
    g_PlayerClient.DoRenameFellowshipGroupRequest(dwGroupID, pszGroupName);
#endif

#ifdef _CLIENT
    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnFellowshipUpdate();
#endif

    nResult = rrcSuccess;
Exit0:
#ifdef _CLIENT
    if (nResult != rrcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventFellowshipRespond UIParam = { nResult };
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }
#endif
    lua_pushboolean(L, (nResult == rrcSuccess));
    return 1;
}

int KPlayer::LuaSetFellowshipRemark(Lua_State* L)
{
    int             nResult             = rrcFailed;
    int             nRetCode            = false;
    DWORD           dwAlliedPlayerID    = ERROR_ID;
    char*           pszRemark           = NULL;
    KGFellowship*   pFellowship         = NULL;
    size_t          uStrLen             = 0;


#ifdef _CLIENT
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);
#endif

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pszRemark = (char*)Lua_ValueToString(L, 2);
    KGLOG_PROCESS_ERROR(pszRemark);

    uStrLen = strlen(pszRemark);
    KG_PROCESS_ERROR_RET_CODE(uStrLen <= _NAME_LEN, rrcInvalidRemark);

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, dwAlliedPlayerID);
    KGLOG_PROCESS_ERROR(pFellowship);

    strncpy(pFellowship->m_szRemark, pszRemark, sizeof(pFellowship->m_szRemark));
    pFellowship->m_szRemark[sizeof(pFellowship->m_szRemark) - sizeof('\0')] = '\0';

#ifdef _CLIENT
    g_PlayerClient.DoSetFellowshipRemarkRequest(dwAlliedPlayerID, pszRemark);
#endif

    nResult = rrcSuccess;
Exit0:
#ifdef _CLIENT
    if (nResult != rrcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventFellowshipRespond UIParam = { nResult };
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }
#endif
    lua_pushboolean(L, (nResult == rrcSuccess));
    return 1;
}

#ifdef _SERVER
int KPlayer::LuaAddFellowship(Lua_State* L)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    int             nParamCount         = 0;
    DWORD           dwAlliedPlayerID    = ERROR_ID;
    KPlayer*        pAlliedPlayer       = NULL;
    KGFellowship*   pFellowship         = NULL;
    char            szAlliedPlayerName[_NAME_LEN];

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pAlliedPlayer = g_pSO3World->m_PlayerSet.GetObj(dwAlliedPlayerID);

    szAlliedPlayerName[0] = '\0';
    if (pAlliedPlayer)
    {
        strncpy(szAlliedPlayerName, pAlliedPlayer->m_szName, sizeof(szAlliedPlayerName));
        szAlliedPlayerName[sizeof(szAlliedPlayerName) - 1] = '\0';
    }

    pFellowship = g_pSO3World->m_FellowshipMgr.AddFellowship(m_dwID, dwAlliedPlayerID, szAlliedPlayerName, true);
    KG_PROCESS_ERROR(pFellowship);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KPlayer::LuaAddFellowshipAttraction(Lua_State* L)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    DWORD           dwAlliedPlayerID    = 0;
    int             nAttraction         = 0;
    KGFellowship*   pFellowship         = NULL;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    nAttraction = (int)Lua_ValueToNumber(L, 2);

    bRetCode = g_pSO3World->m_FellowshipMgr.AddFellowshipAttraction(m_dwID, dwAlliedPlayerID, nAttraction);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KPlayer::LuaGetFellowshipAttraction(Lua_State* L)
{
    int             nResult             = 0;
    DWORD           dwAlliedPlayerID    = 0;
    KGFellowship*   pFellowship         = NULL;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, dwAlliedPlayerID);
    KG_PROCESS_ERROR(pFellowship);

    nResult = pFellowship->m_nAttraction;
Exit0:
    lua_pushboolean(L, (pFellowship != NULL));
    lua_pushinteger(L, nResult);
    return 2;
}

int KPlayer::LuaGetFellowshipAttractionLevel(Lua_State* L)
{
    int             nResult             = 0;
    DWORD           dwAlliedPlayerID    = 0;
    KGFellowship*   pFellowship         = NULL;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, dwAlliedPlayerID);
    KG_PROCESS_ERROR(pFellowship);

    nResult = KGFellowshipMgr::AttractionValue2Level(pFellowship->m_nAttraction);
Exit0:
    lua_pushboolean(L, (pFellowship != NULL));
    lua_pushinteger(L, nResult);
    return 2;
}

int KPlayer::LuaIsFoe(Lua_State* L)
{
    BOOL    bResult             = false;
    BOOL    bRetCode            = false;
    DWORD   dwAlliedPlayerID    = 0;
    KGFoe*  pFoe                = NULL;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    bRetCode = IsFoe(dwAlliedPlayerID);
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaAddFoe(Lua_State* L)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    int         nParamCount         = 0;
    DWORD       dwAlliedPlayerID    = ERROR_ID;
    KPlayer*    pAlliedPlayer       = NULL;
    KGFoe*      pFoe                = NULL;
    char*       pszAlliedPlayerName = NULL;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);
    
    pAlliedPlayer = g_pSO3World->m_PlayerSet.GetObj(dwAlliedPlayerID);

    if (pAlliedPlayer)
        pszAlliedPlayerName = pAlliedPlayer->m_szName;

    pFoe = g_pSO3World->m_FellowshipMgr.AddFoe(m_dwID, dwAlliedPlayerID, pszAlliedPlayerName, true);
    KG_PROCESS_ERROR(pFoe);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KPlayer::LuaAddBlackList(Lua_State* L)
{
    BOOL            bResult             = false;
    BOOL            bRetCode            = false;
    int             nParamCount         = 0;
    DWORD           dwAlliedPlayerID    = ERROR_ID;
    KPlayer*        pAlliedPlayer       = NULL;
    KGBlackNode*    pBlackNode          = NULL;
    char*           pszAlliedPlayerName = NULL;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    pAlliedPlayer = g_pSO3World->m_PlayerSet.GetObj(dwAlliedPlayerID);

    if (pAlliedPlayer)
        pszAlliedPlayerName = pAlliedPlayer->m_szName;

    pBlackNode = g_pSO3World->m_FellowshipMgr.AddBlackList(m_dwID, dwAlliedPlayerID, pszAlliedPlayerName, true);
    KG_PROCESS_ERROR(pBlackNode);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}
#endif

int KPlayer::LuaCanAddFoe(Lua_State* L)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    int         nParamCount         = 0;

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 0);
    
    bRetCode = CanAddFoe();
    KG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

#ifdef _CLIENT
int KPlayer::LuaAddFellowship(Lua_State* L)
{
    BOOL    bResult             = false;
    BOOL    bRetCode            = false;
    int     nParamCount         = 0;
    char*   pszAlliedPlayerName = NULL;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    //KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1);

    pszAlliedPlayerName = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszAlliedPlayerName);

    g_PlayerClient.DoAddFellowshipRequest(pszAlliedPlayerName, 1);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KPlayer::LuaAddFoe(Lua_State* L)
{
    BOOL    bResult             = false;
    BOOL    bRetCode            = false;
    int     nParamCount         = 0;
    char*   pszAlliedPlayerName = NULL;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);

    if (m_bFightState)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventFellowshipRespond UIParam = { rrcInFight };
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }

        goto Exit0;
    }

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1);
    
    bRetCode = CanAddFoe();
    KG_PROCESS_ERROR(bRetCode);

    pszAlliedPlayerName = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszAlliedPlayerName);

    g_PlayerClient.DoAddFellowshipRequest(pszAlliedPlayerName, 2);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KPlayer::LuaAddBlackList(Lua_State* L)
{
    BOOL    bResult             = false;
    BOOL    bRetCode            = false;
    int     nParamCount         = 0;
    char*   pszAlliedPlayerName = NULL;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    //KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    nParamCount = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nParamCount == 1);

    pszAlliedPlayerName = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszAlliedPlayerName);

    g_PlayerClient.DoAddFellowshipRequest(pszAlliedPlayerName, 3);

    bResult = true;
Exit0:
    lua_pushboolean(L, bResult);
    return 1;
}

int KPlayer::LuaSetFellowshipGroup(Lua_State* L)
{
    int             nResult             = rrcFailed;
    int             nRetCode            = false;
    DWORD           dwAlliedPlayerID    = ERROR_ID;
    DWORD           dwOldGroupID        = 0;
    DWORD           dwNewGroupID        = 0;
    KGFellowship*   pFellowship         = NULL;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 3);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(dwAlliedPlayerID != ERROR_ID);

    dwOldGroupID = (DWORD)Lua_ValueToNumber(L, 2);
    KGLOG_PROCESS_ERROR(dwOldGroupID <= KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    dwNewGroupID = (DWORD)Lua_ValueToNumber(L, 3);
    KGLOG_PROCESS_ERROR(dwNewGroupID <= KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    KGLOG_PROCESS_SUCCESS(dwOldGroupID == dwNewGroupID);

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, dwAlliedPlayerID);
    KGLOG_PROCESS_ERROR_RET_CODE(pFellowship, rrcFellowshipNotFound);

    pFellowship->m_BitFlag.dwGroupID = dwNewGroupID;
    g_PlayerClient.DoSetFellowshipGroupRequest(dwAlliedPlayerID, dwNewGroupID);

    if (g_pGameWorldUIHandler)
        g_pGameWorldUIHandler->OnFellowshipUpdate();

Exit1:
    nResult = rrcSuccess;
Exit0:
    if (nResult != rrcSuccess)
    {
        if (g_pGameWorldUIHandler)
        {
            KUIEventFellowshipRespond UIParam = { nResult };
            g_pGameWorldUIHandler->OnFellowshipRespond(UIParam);
        }
    }

    lua_pushboolean(L, (nResult == rrcSuccess));
    return 1;
}

int KPlayer::LuaUpdateFellowshipInfo(Lua_State* L)
{
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);

    g_PlayerClient.DoGetFellowshipListRequest(1);

Exit0:
    return 0;
}

int KPlayer::LuaUpdateFoeInfo(Lua_State* L)
{
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);

    g_PlayerClient.DoGetFellowshipListRequest(2);

Exit0:
    return 0;
}

int KPlayer::LuaUpdateBlackListInfo(Lua_State* L)
{
    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);

    g_PlayerClient.DoGetFellowshipListRequest(3);

Exit0:
    return 0;
}

int KPlayer::LuaGetFellowshipGroupName(Lua_State* L)
{
    BOOL    bRetCode    = false;
    int     nRetCode    = 0;
    int     nTable      = 0;
    DWORD   dwGroupID   = 0;
    char    szGroupName[_NAME_LEN];

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    nRetCode = Lua_GetTopIndex(L);
    KG_PROCESS_ERROR(nRetCode == 1);

    dwGroupID = (DWORD)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(dwGroupID);
    KG_PROCESS_ERROR(dwGroupID <= KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    strncpy(
        szGroupName, 
        g_pSO3World->m_FellowshipMgr.m_GroupNamesMap[m_dwID].szGroupName[dwGroupID - 1], 
        sizeof(szGroupName)
    );
    szGroupName[sizeof(szGroupName) - sizeof('\0')] = '\0';
    Lua_PushString(L, szGroupName);

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaGetFellowshipData(Lua_State* L)
{
    BOOL            bRetCode            = false;
    int             nRetCode            = 0;
    DWORD           dwAlliedPlayerID    = 0;
    DWORD           dwFlag              = 0;
    KGFellowship*   pFellowship         = NULL;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    nRetCode = Lua_GetTopIndex(L);
    KG_PROCESS_ERROR(nRetCode == 1);

    dwAlliedPlayerID = (DWORD)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(dwAlliedPlayerID != 0);

    pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, dwAlliedPlayerID);
    KGLOG_PROCESS_ERROR(pFellowship);

    Lua_NewTable(L);

	Lua_PushString(L, "id");
	Lua_PushNumber(L, dwAlliedPlayerID);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "married");
	Lua_PushBoolean(L, pFellowship->m_BitFlag.bMarried);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "brother");
	Lua_PushBoolean(L, pFellowship->m_BitFlag.bBrother);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "level");
	Lua_PushNumber(L, pFellowship->m_nPlayerLevel);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "name");
	Lua_PushString(L, pFellowship->m_szName);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "attraction");
	Lua_PushNumber(L, pFellowship->m_nAttraction);
	Lua_SetTable(L, -3);

	Lua_PushString(L, "mapid");
	Lua_PushNumber(L, pFellowship->m_dwMapID);
	Lua_SetTable(L, -3);

    Lua_PushString(L, "forceid");
    Lua_PushNumber(L, pFellowship->m_dwForceID);
    Lua_SetTable(L, -3);

	Lua_PushString(L, "remark");
	Lua_PushString(L, pFellowship->m_szRemark);
	Lua_SetTable(L, -3);

    return 1;
Exit0:
    return 0;
}

struct _GetFellowshipListByGroup  
{
    _GetFellowshipListByGroup()
    {
        m_dwGroupID     = 0;
        m_nCount        = 0;
    };

    BOOL operator ()(DWORD dwPlayerID, DWORD dwAlliedPLayerID)
    {
        KGFellowship* pFellowship = NULL;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(dwPlayerID, dwAlliedPLayerID);
        assert(pFellowship);

        KG_PROCESS_ERROR(pFellowship->m_BitFlag.dwGroupID == m_dwGroupID);

        m_FellowshipArray[m_nCount].dwAlliedPlayerID = dwAlliedPLayerID;
        m_FellowshipArray[m_nCount].pFellowship = pFellowship;

        m_nCount++;
Exit0:
        return true;
    };

    DWORD   m_dwGroupID;
    int     m_nCount;
    struct _FellowshipArray 
    {
        DWORD           dwAlliedPlayerID;
        KGFellowship*   pFellowship;
    } m_FellowshipArray[KG_FELLOWSHIP_MAX_RECORD];
};

struct _SortComp 
{
    inline bool operator ()(
        const _GetFellowshipListByGroup::_FellowshipArray& crLeft, 
        const _GetFellowshipListByGroup::_FellowshipArray& crRight
    )
    {
        if (!crLeft.pFellowship->m_nPlayerLevel == !crRight.pFellowship->m_nPlayerLevel)
            return (crLeft.pFellowship->m_nAttraction > crRight.pFellowship->m_nAttraction);

        return !!crLeft.pFellowship->m_nPlayerLevel;
    }
};

int KPlayer::LuaGetFellowshipInfo(Lua_State* L)
{
    int                         nRetCode    = 0;
    int                         nTable      = 0;
    DWORD                       dwGroupID   = 0;
    _GetFellowshipListByGroup   GetFellowshipListByGroup;
    _SortComp                   SortComp;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    nRetCode = Lua_GetTopIndex(L);
    KG_PROCESS_ERROR(nRetCode == 1);

    dwGroupID = (DWORD)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(dwGroupID <= KG_FELLOWSHIP_MAX_CUSTEM_GROUP);

    GetFellowshipListByGroup.m_dwGroupID = dwGroupID;

    nRetCode = g_pSO3World->m_FellowshipMgr.TraverseFellowshipID(m_dwID, GetFellowshipListByGroup);
    KG_PROCESS_ERROR(nRetCode);

    std::sort(
        GetFellowshipListByGroup.m_FellowshipArray, 
        GetFellowshipListByGroup.m_FellowshipArray + GetFellowshipListByGroup.m_nCount, 
        SortComp
    );

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);
    for (int i = 0; i < GetFellowshipListByGroup.m_nCount; i++)
    {
        KGFellowship* pFellowship = GetFellowshipListByGroup.m_FellowshipArray[i].pFellowship;
        assert(pFellowship);

        Lua_PushNumber(L, i + 1);   // Index
        {
            Lua_NewTable(L);

            Lua_PushString(L, "id");
            Lua_PushNumber(L, GetFellowshipListByGroup.m_FellowshipArray[i].dwAlliedPlayerID);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "married");
            Lua_PushBoolean(L, pFellowship->m_BitFlag.bMarried);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "brother");
            Lua_PushBoolean(L, pFellowship->m_BitFlag.bBrother);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "level");
            Lua_PushNumber(L, pFellowship->m_nPlayerLevel);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "name");
            Lua_PushString(L, pFellowship->m_szName);
            Lua_SetTable(L, -3);

			Lua_PushString(L, "attraction");
			Lua_PushNumber(L, pFellowship->m_nAttraction);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "mapid");
			Lua_PushNumber(L, pFellowship->m_dwMapID);
			Lua_SetTable(L, -3);

            Lua_PushString(L, "forceid");
            Lua_PushNumber(L, pFellowship->m_dwForceID);
            Lua_SetTable(L, -3);

			Lua_PushString(L, "remark");
			Lua_PushString(L, pFellowship->m_szRemark);
			Lua_SetTable(L, -3);
        }

        Lua_SetTable(L, nTable);
    }

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaGetFoeInfo(Lua_State* L)
{
    int                     nRetCode    = 0;
    int                     nTable      = 0;
    GetFellowshipIDListFunc GetFoeIDList;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    nRetCode = Lua_GetTopIndex(L);
    KG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = g_pSO3World->m_FellowshipMgr.TraverseFoeID(m_dwID, GetFoeIDList);
    KG_PROCESS_ERROR(nRetCode);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);
    for (int i = 0; i < GetFoeIDList.m_nIDListCount; i++)
    {
        KGFoe* pFoe = g_pSO3World->m_FellowshipMgr.GetFoe(m_dwID, GetFoeIDList.m_dwIDList[i]);
        assert(pFoe);

        Lua_PushNumber(L, i + 1);   // Index
        {
            Lua_NewTable(L);

            Lua_PushString(L, "id");
            Lua_PushNumber(L, GetFoeIDList.m_dwIDList[i]);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "level");
            Lua_PushNumber(L, pFoe->m_nPlayerLevel);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "name");
            Lua_PushString(L, pFoe->m_szName);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "forceid");
            Lua_PushNumber(L, pFoe->m_dwForceID);
            Lua_SetTable(L, -3);
        }

        Lua_SetTable(L, nTable);
    }

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaGetBlackListInfo(Lua_State* L)
{
    int                     nRetCode    = 0;
    int                     nTable      = 0;
    GetFellowshipIDListFunc GetBlackListIDList;

    KGLOG_PROCESS_ERROR(m_dwID == g_pSO3World->m_dwClientPlayerID);
    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    nRetCode = Lua_GetTopIndex(L);
    KG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = g_pSO3World->m_FellowshipMgr.TraverseBlackListID(m_dwID, GetBlackListIDList);
    KG_PROCESS_ERROR(nRetCode);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);
    for (int i = 0; i < GetBlackListIDList.m_nIDListCount; i++)
    {
        KGBlackNode* pBlackNode = g_pSO3World->m_FellowshipMgr.GetBlackListNode(m_dwID, GetBlackListIDList.m_dwIDList[i]);
        assert(pBlackNode);

        Lua_PushNumber(L, i + 1);   // Index
        {
            Lua_NewTable(L);

            Lua_PushString(L, "id");
            Lua_PushNumber(L, GetBlackListIDList.m_dwIDList[i]);
            Lua_SetTable(L, -3);

            Lua_PushString(L, "name");
            Lua_PushString(L, pBlackNode->m_szName);
            Lua_SetTable(L, -3);
        }

        Lua_SetTable(L, nTable);
    }

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaGetFellowshipGroupInfo(Lua_State* L)
{
    BOOL    bRetCode    = false;
    int     nTable      = 0;
	int     nIndex      = 1;
    char    szGroupName[_NAME_LEN];
    KG_FELLOWSHIP_GROUPNAMES_MAP::iterator it;

    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    it = g_pSO3World->m_FellowshipMgr.m_GroupNamesMap.find(g_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(it != g_pSO3World->m_FellowshipMgr.m_GroupNamesMap.end());

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    szGroupName[0] = '\0';

    for (int i = 0; i < it->second.nGroupCount; i++)
    {
        strncpy(szGroupName, it->second.szGroupName[i], sizeof(szGroupName));// 好友分组名字
        szGroupName[sizeof(szGroupName) - sizeof('\0')] = '\0';

		if (szGroupName[0])
		{
            Lua_PushNumber(L, nIndex++);
			Lua_NewTable(L);

			Lua_PushString(L, "id");
			Lua_PushNumber(L, i + 1);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "name");
			Lua_PushString(L, szGroupName);
			Lua_SetTable(L, -3);

			Lua_SetTable(L, nTable);
		}
    }

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaGetFellowshipNameList(Lua_State* L)
{
    BOOL                    bRetCode    = false;
    int                     nRetCode    = 0;
    int                     nTable      = 0;
    GetFellowshipIDListFunc GetIDListFunc;

    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    bRetCode = g_pSO3World->m_FellowshipMgr.TraverseFellowshipID(m_dwID, GetIDListFunc);
    KG_PROCESS_ERROR(bRetCode);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    for (int nIndex = 0; nIndex < GetIDListFunc.m_nIDListCount; nIndex++)
    {
        KGFellowship* pFellowship = NULL;

        pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(m_dwID, GetIDListFunc.m_dwIDList[nIndex]);
        KG_PROCESS_ERROR(pFellowship);

        Lua_PushNumber(L, nIndex + 1);
        Lua_PushString(L, pFellowship->m_szName);
        Lua_SetTable(L, nTable);
    }

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaIsExistFellowshipList(Lua_State* L)
{
    int                     nResult             = 0;
    BOOL                    bRetCode            = false;
    char*                   pszAlliedPlayerName = NULL;
    
    struct _FindFellowshipByName 
    {
        _FindFellowshipByName()
        {
            m_pszName       = NULL;
            m_bExistFlag    = false;
        }

        BOOL operator()(DWORD dwPlayerID, DWORD dwAlliedPlayerID)
        {
            KGFellowship* pFellowship = NULL;

            pFellowship = g_pSO3World->m_FellowshipMgr.GetFellowship(dwPlayerID, dwAlliedPlayerID);
            assert(pFellowship);

            if(strcmp(pFellowship->m_szName, m_pszName) == 0)
            {
                m_bExistFlag = true;
                return false;
            }

            return true;
        }

        char*   m_pszName;
        BOOL    m_bExistFlag;

    } FindFellowshipByName;

    KG_PROCESS_ERROR(!g_pSO3World->m_FellowshipMgr.m_bWaitInitSync);

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    pszAlliedPlayerName = (char*)Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszAlliedPlayerName);

    FindFellowshipByName.m_pszName = pszAlliedPlayerName;

    bRetCode = g_pSO3World->m_FellowshipMgr.TraverseFellowshipID(m_dwID, FindFellowshipByName);
    KG_PROCESS_ERROR(bRetCode);

    Lua_PushBoolean(L, FindFellowshipByName.m_bExistFlag);

    nResult = 1;
Exit0:
    return nResult;
}
#endif //_CLIENT

//////////////////////////////////////////////////////////////////////////

#ifdef _SERVER
int KPlayer::LuaSendSystemMessage(Lua_State* L)
{
    BOOL        bResult     = false;
    int         nRetCode    = false;
    const char* pszText     = NULL;
    size_t      uDataLen    = 0;
    IKG_Buffer* piBuffer    = NULL;
    BYTE*       pbyBuffer   = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pszText = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszText);

    uDataLen = 1 + strlen(pszText) + 1;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

    *pbyBuffer = tdbText;
    strcpy((char*)pbyBuffer + 1, pszText);

    g_PlayerServer.DoTalkMessage(trLocalSys, ERROR_ID, NULL, m_dwID, NULL, uDataLen, pbyBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    lua_pushboolean(L, bResult);
    return 1;    
}

int KPlayer::LuaGmSendMessage(Lua_State* L)
{
    BOOL        bResult     = false;
    int         nRetCode    = false;
    const char* pszText     = NULL;
    size_t      uDataLen    = 0;
    IKG_Buffer* piBuffer    = NULL;
    BYTE*       pbyBuffer   = NULL;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pszText = Lua_ValueToString(L, 1);
    KGLOG_PROCESS_ERROR(pszText);

    uDataLen = 1 + strlen(pszText) + 1;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uDataLen);
    KGLOG_PROCESS_ERROR(piBuffer);

    pbyBuffer = (BYTE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pbyBuffer);

    *pbyBuffer = tdbText;
    strcpy((char*)pbyBuffer + 1, pszText);

    g_PlayerServer.DoTalkMessage(trGmMessage, ERROR_ID, NULL, m_dwID, NULL, uDataLen, pbyBuffer);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    lua_pushboolean(L, bResult);
    return 1;    
}
#endif

#ifdef _SERVER
int KPlayer::LuaGetPartyMemberList(Lua_State* L)
{
	int     nResult     = 0;
	int     nRetCode    = 0;
	KTeam*  pTeam       = NULL;
	int     nIndex      = 1;
    int     nTable      = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
	KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

	pTeam = g_pSO3World->m_TeamServer.GetTeam(m_dwTeamID);
	KGLOG_PROCESS_ERROR(pTeam);

	Lua_NewTable(L);
	nTable = Lua_GetTopIndex(L);

	for (int i = 0; i < pTeam->nGroupNum; i++)
	{
        for (
            KTEAM_MEMBER_LIST::iterator it = pTeam->MemberGroup[i].MemberList.begin();
            it != pTeam->MemberGroup[i].MemberList.end(); ++it
        )
        {
            Lua_PushNumber(L, nIndex);
            Lua_PushNumber(L, it->dwMemberID);
            Lua_SetTable(L, nTable);

            nIndex++;
        }
	}

	nResult = 1;
Exit0:
	return nResult;
}

int KPlayer::LuaSetTimer(Lua_State* L)
{
	DWORD       dwID         = 0;
    int         nTime        = 0;
    int         nParam1      = 0;
    int         nParam2      = 0;
    const char* szScriptName = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 4);

	nTime = (int)Lua_ValueToNumber(L, 1);
	szScriptName = Lua_ValueToString(L, 2);
	nParam1 = (int)Lua_ValueToNumber(L, 3);
	nParam2 = (int)Lua_ValueToNumber(L, 4);
    
    KGLOG_PROCESS_ERROR(szScriptName);

	dwID = m_ScriptTimerList.SetTimer(nTime, szScriptName, nParam1, nParam2);
	KGLOG_PROCESS_ERROR(dwID);

Exit0:
	lua_pushinteger(L, dwID);
	return 1;
}

int KPlayer::LuaStopTimer(Lua_State* L)
{
    BOOL    bResult     = false;
	BOOL    bRetCode    = false;
    DWORD   dwTimerID   = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwTimerID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ScriptTimerList.StopTimer(dwTimerID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaRestartTimer(Lua_State* L)
{
    BOOL    bResult   = false;
	BOOL    bRetCode  = false;
    DWORD   dwTimerID = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwTimerID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ScriptTimerList.RestartTimer(dwTimerID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaRemoveTimer(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
    DWORD   dwTimerID       = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwTimerID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = m_ScriptTimerList.RemoveTimer(dwTimerID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaGetSelectCharacter(Lua_State* L)
{
	int         nRetCode    = false;
	KPlayer*    pPlayer     = NULL;
	KNpc*       pNpc        = NULL;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = m_SelectTarget.GetTarget(&pPlayer);
	if (nRetCode && pPlayer && pPlayer->m_pScene)
	{
		return pPlayer->LuaGetObj(L);
	}
	
	nRetCode = m_SelectTarget.GetTarget(&pNpc);
	if (nRetCode && pNpc && pNpc->m_pScene)
	{
		return pNpc->LuaGetObj(L);
	}
Exit0:
	return 0;
}

int KPlayer::LuaGetSelectDoodad(Lua_State* L)
{
	int         nRetCode    = false;
    KDoodad*    pDoodad     = NULL;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = m_SelectTarget.GetTarget(&pDoodad);
	if (nRetCode && pDoodad)
	{
		return pDoodad->LuaGetObj(L);
	}
	
Exit0:
	return 0;
}

int KPlayer::LuaAddMapMark(Lua_State* L)
{
	BOOL    bResult     = false;
	int     nX          = 0;
	int     nY          = 0;
	int     nZ          = 0;
	int     nType       = 0;
	const char* szComment = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

	nX = (int)Lua_ValueToNumber(L, 1);
	nY = (int)Lua_ValueToNumber(L, 2);
	nZ = (int)Lua_ValueToNumber(L, 3);
	nType = (int)Lua_ValueToNumber(L, 4);
	szComment = (const char*)Lua_ValueToString(L, 5);

	g_PlayerServer.DoUpdateMapMark(m_nConnIndex, nX, nY, nZ, nType, szComment);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaRevive(Lua_State* L)
{
	BOOL    bResult         = false;
	BOOL    bToRevivePoint  = false;
	int     nTopIndex       = 0;
    int     nReviveMode     = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 0 || nTopIndex == 1);

    KGLOG_PROCESS_ERROR(m_eMoveState == cmsOnDeath);

	if (nTopIndex == 1)
	{
		nReviveMode = (int)Lua_ValueToNumber(L, 1);
	}

	Revive(nReviveMode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaPlaySound(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
	DWORD   dwSoundID       = 0;
		
	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwSoundID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = g_PlayerServer.DoPlaySound(m_nConnIndex, dwSoundID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaPlayMiniGame(Lua_State* L)
{
    BOOL    bResult         = false;
	BOOL    bRetCode        = false;
	DWORD   dwGameID        = 0;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwGameID = (DWORD)Lua_ValueToNumber(L, 1);

	bRetCode = g_PlayerServer.DoPlayMiniGame(m_nConnIndex, dwGameID);
	KG_PROCESS_ERROR(bRetCode);

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaSendMessageNotify(Lua_State* L)
{
    BOOL    bResult         = false;
	DWORD   dwErrorCodeType = 0;
	DWORD   dwErrorCode     = 0;
	int     nTopIndex       = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KG_PROCESS_ERROR(nTopIndex == 2);

	dwErrorCodeType = (DWORD)Lua_ValueToNumber(L, 1);
	dwErrorCode = (DWORD)Lua_ValueToNumber(L, 2);

	g_PlayerServer.DoMessageNotify(m_nConnIndex, dwErrorCodeType, dwErrorCode);

    bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}
#endif

//中地图标记,同步玩家自己在地图上做的标记
int KPlayer::LuaSyncMidMapMark(Lua_State* L)	
{
    BOOL    bResult     = false;
	int     nMapID      = 0;
	int     nX          = 0;
	int     nY          = 0;
	int     nType       = 0;
	const char* pszComment = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 5);

	nMapID = (int)Lua_ValueToNumber(L, 1);
	nX	   = (int)Lua_ValueToNumber(L, 2);
	nY	   = (int)Lua_ValueToNumber(L, 3);
	nType  = (int)Lua_ValueToNumber(L, 4);
	pszComment = Lua_ValueToString(L, 5);

#ifdef _SERVER
	g_RelayClient.DoSyncMidMapMark(this, nMapID, nX, nY, nType, pszComment);
#endif

#ifdef _CLIENT
	g_PlayerClient.DoSyncMidMapMark(nMapID, nX, nY, nType, pszComment);
#endif

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

#ifdef _CLIENT
int KPlayer::LuaSetMinimapRadar(Lua_State* L)
{
	BOOL    bResult        = false;
    int     nRadarType     = 0;
    int     nRadarParam    = 0;
    KUIEventUpdateRadar UIParam;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

    nRadarType	= (int)Lua_ValueToNumber(L, 1);
	nRadarParam = (int)Lua_ValueToNumber(L, 2);

	KGLOG_PROCESS_ERROR(nRadarType > mrtInvalid && nRadarType < mrtTotal);

	m_eRadarType    = (MINIMAP_RADAR_TYPE)nRadarType;
	m_nRadarParam   = nRadarParam;

	UIParam.dwRadarType     = (DWORD)m_eRadarType;
	UIParam.dwRadarParam    = (DWORD)m_nRadarParam;

    if (g_pGameWorldUIHandler)
    {
	    g_pGameWorldUIHandler->OnUpdateRadar(UIParam);
    }

	bResult = true;
Exit0:
	Lua_PushBoolean(L, bResult);
	return 1;
}

int KPlayer::LuaGetMiniMapRadar(Lua_State* L)
{
	int nTopIndex = 0;
	
	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 0);

	Lua_PushNumber(L, (int)m_eRadarType);
	Lua_PushNumber(L, (int)m_nRadarParam);

	return 2;
Exit0:
	return 0;
}

#endif //_CLIENT
int KPlayer::LuaCanApplyDuel(Lua_State* L)
{
    BOOL            bResult         = false;
    PK_RESULT_CODE  eRetCode        = pkrcFailed;
    DWORD           dwTargetID      = ERROR_ID;
    KPlayer*        pTargetPlayer   = NULL;

    KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

    dwTargetID = (DWORD)Lua_ValueToNumber(L, 1);

    pTargetPlayer = g_pSO3World->m_PlayerSet.GetObj(dwTargetID);
    KGLOG_PROCESS_ERROR(pTargetPlayer);

    eRetCode = m_PK.CanApplyDuel(pTargetPlayer);
    KG_PROCESS_ERROR(eRetCode == pkrcSuccess);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaCanApplySlay(Lua_State* L)
{
    BOOL            bResult         = false;
    PK_RESULT_CODE  eRetCode        = pkrcFailed;
	
	eRetCode = m_PK.CanApplySlay();
	KG_PROCESS_ERROR(eRetCode == pkrcSuccess);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaApplyDuel(Lua_State* L)
{
    BOOL            bResult         = false;
    PK_RESULT_CODE  eRetCode        = pkrcApplyDuelFailed;
    DWORD           dwTargetID      = ERROR_ID;
	KPlayer*        pTargetPlayer   = NULL;

	KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

	dwTargetID = (DWORD)Lua_ValueToNumber(L, 1);
	
	pTargetPlayer = g_pSO3World->m_PlayerSet.GetObj(dwTargetID);
	KGLOG_PROCESS_ERROR(pTargetPlayer);

#ifdef _SERVER
	eRetCode = m_PK.ApplyDuel(dwTargetID);
	KGLOG_PROCESS_ERROR(eRetCode == pkrcSuccess);
#endif

#ifdef _CLIENT
	g_PlayerClient.DoApplyPKOperate(pkoApplyDuel, dwTargetID, 0);
#endif 

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaRefuseDuel(Lua_State* L)
{
    BOOL            bResult         = false;
    PK_RESULT_CODE  eRetCode        = pkrcFailed;

#ifdef _SERVER
	eRetCode = m_PK.RefuseDuel();
	KGLOG_PROCESS_ERROR(eRetCode == pkrcSuccess);
#endif 

#ifdef _CLIENT
	g_PlayerClient.DoApplyPKOperate(pkoRefuseDuel, 0, 0);
#endif

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaAcceptDuel(Lua_State* L)
{
    BOOL            bResult         = false;
    PK_RESULT_CODE  eRetCode        = pkrcFailed;

#ifdef _SERVER
	eRetCode = m_PK.AcceptDuel();
	KGLOG_PROCESS_ERROR(eRetCode == pkrcSuccess);
#endif

#ifdef _CLIENT
	g_PlayerClient.DoApplyPKOperate(pkoAcceptDuel, 0, 0);
#endif

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaLossDuel(Lua_State* L)
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;

#ifdef _SERVER
	bRetCode = m_PK.LossDuel();
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

#ifdef _CLIENT
	g_PlayerClient.DoApplyPKOperate(pkoLossDuel, 0, 0);
#endif

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaApplySlay(Lua_State* L)
{
    BOOL            bResult         = false;
    PK_RESULT_CODE  eRetCode        = pkrcApplySlayFailed;

#ifdef _SERVER
	eRetCode = m_PK.ApplySlay();
	KGLOG_PROCESS_ERROR(eRetCode == pkrcSuccess);
#endif

#ifdef _CLIENT
	g_PlayerClient.DoApplyPKOperate(pkoApplySlay, 0, 0);
#endif

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaCanCloseSlay(Lua_State* L)
{
    BOOL            bResult         = false;
    PK_STATE        ePKState        = m_PK.GetState();

    KG_PROCESS_ERROR(ePKState == pksSlaying);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaCloseSlay(Lua_State* L)
{
    BOOL            bResult         = false;
    PK_RESULT_CODE  eRetCode        = pkrcFailed;

#ifdef _SERVER
    eRetCode = m_PK.CloseSlay();
    KGLOG_PROCESS_ERROR(eRetCode == pkrcSuccess);
#endif

#ifdef _CLIENT
    g_PlayerClient.DoApplyPKOperate(pkoCloseSlay, 0, 0);
#endif

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaGetPKState(Lua_State* L)
{
	PK_STATE eState = pksInvalid;
	int nTopIndex = 0;

	nTopIndex = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nTopIndex == 0);

	eState = m_PK.GetState();
Exit0:
	lua_pushinteger(L, eState);
	return 1;
}


#ifdef _SERVER
int KPlayer::LuaRideHorse(Lua_State* L)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    
    bRetCode = RideHorse();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaDownHorse(Lua_State* L)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;
    
    bRetCode = DownHorse();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaAddEnchant(Lua_State* L)
{
    BOOL  bResult     = false;
    BOOL  bRetCode    = false;
    DWORD dwBoxIndex  = 0;
    DWORD dwPos       = 0;
    int   nEnchantID  = 0;
    int   nTopIndex   = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nTopIndex == 3);

	dwBoxIndex  = (DWORD)Lua_ValueToNumber(L, 1);
	dwPos       = (DWORD)Lua_ValueToNumber(L, 2);
    nEnchantID  = (int)Lua_ValueToNumber(L, 3);

    bRetCode = m_ItemList.AddEnchant(dwBoxIndex, dwPos, nEnchantID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;  
}

int KPlayer::LuaRemoveEnchant(Lua_State* L)
{
    BOOL  bResult             = false;
    BOOL  bRetCode            = false;
    DWORD dwBoxIndex          = 0;
    DWORD dwPos               = 0;
    BOOL  bPermanentEnchant   = true;
    int   nTopIndex           = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 3);

	dwBoxIndex          = (DWORD)Lua_ValueToNumber(L, 1);
	dwPos               = (DWORD)Lua_ValueToNumber(L, 2);

    if (nTopIndex == 3)
    {
        bPermanentEnchant   = (BOOL)Lua_ValueToNumber(L, 3);
    }

    bRetCode = m_ItemList.RemoveEnchant(dwBoxIndex, dwPos, bPermanentEnchant);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaGetEnchantDestItemSubType(Lua_State* L)
{
	int         nResult		= -1;
	int         nEnchantID  = 0;
	int			nTopIndex   = Lua_GetTopIndex(L);
	KENCHANT*   pEnchant    = NULL;

	KGLOG_PROCESS_ERROR(nTopIndex == 1);

	nEnchantID = (DWORD)Lua_ValueToNumber(L, 1);

	pEnchant = g_pSO3World->m_ItemManager.m_EnchantLib.GetEnchantInfo(nEnchantID);
	KGLOG_PROCESS_ERROR(pEnchant);
	
	nResult = pEnchant->nDestItemSubType;
Exit0:
	Lua_PushNumber(L, nResult);
	return 1;
}

int KPlayer::LuaSetEquipColor(Lua_State* L)
{
    BOOL  bResult     = false;
    BOOL  bRetCode    = false;
    DWORD dwBoxIndex  = 0;
    DWORD dwPos       = 0;
    int   nColorID    = 0;
    int   nTopIndex   = Lua_GetTopIndex(L);

	KGLOG_PROCESS_ERROR(nTopIndex == 3);

	dwBoxIndex  = (DWORD)Lua_ValueToNumber(L, 1);
	dwPos       = (DWORD)Lua_ValueToNumber(L, 2);
    nColorID    = (int)Lua_ValueToNumber(L, 3);

    bRetCode = m_ItemList.SetEquipColor(dwBoxIndex, dwPos, nColorID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;  
}

int KPlayer::LuaOpenRouteNode(Lua_State* L)
{
    BOOL                                        bResult     = false;
    BOOL                                        bRetCode    = false;
    int                                         nTopIndex   = 0;
    int                                         nNodeID     = 0;
    KROUTE_NODE*                                pNode       = NULL;
    KOPEN_ROUTE_NODE_VECTOER::iterator          itFind;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nNodeID = (int)Lua_ValueToNumber(L, 1);

    pNode = g_pSO3World->m_RoadManager.GetNodeInfo(nNodeID);
    KGLOG_PROCESS_ERROR(pNode && "Not Found Node");

    KGLOG_PROCESS_ERROR(pNode->bIsNeedOpen && "Not Need Open");

    itFind = find(m_OpenRouteNodeList.begin(), m_OpenRouteNodeList.end(), nNodeID);
    KG_PROCESS_ERROR(itFind == m_OpenRouteNodeList.end());

    m_OpenRouteNodeList.push_back(nNodeID);

    g_PlayerServer.DoSyncRouteNodeOpenList(m_nConnIndex, (int)m_OpenRouteNodeList.size(), &m_OpenRouteNodeList[0]);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}
#endif

int KPlayer::LuaIsOpenRouteNode(Lua_State* L)
{
    BOOL                                bResult     = false;
    int                                 nTopIndex   = 0;
    int                                 nNodeID     = 0;
    KOPEN_ROUTE_NODE_VECTOER::iterator  itFind;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nNodeID = (int)Lua_ValueToNumber(L, 1);

    itFind = find(m_OpenRouteNodeList.begin(), m_OpenRouteNodeList.end(), nNodeID);
    KG_PROCESS_ERROR(itFind != m_OpenRouteNodeList.end());

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}

#ifdef _SERVER
int KPlayer::LuaAddTrain(Lua_State* L)
{
    int  nResult     = false;
    BOOL bRetCode    = false;
    int  nTopIndex   = 0;
    int  nAddTrain   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAddTrain = (int)Lua_ValueToNumber(L, 1);
    
    bRetCode = AddTrain(nAddTrain);
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = true;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

int KPlayer::LuaAddTrainNoLimit(Lua_State* L)
{
    int  nResult     = false;
    BOOL bRetCode    = false;
    int  nTopIndex   = 0;
    int  nAddTrain   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAddTrain = (int)Lua_ValueToNumber(L, 1);

    bRetCode = AddTrainNoLimit(nAddTrain);
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = true;
Exit0:
    Lua_PushNumber(L, nResult);
    return 1;
}

#endif

int KPlayer::LuaIsAchievementAcquired(Lua_State* L)
{
    BOOL    bResult         = false;
    BOOL    bRetCode        = false;
    int     nResult         = 0;
    int     nTopIndex       = 0;
    int     nAchievementID  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAchievementID = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nAchievementID >= 0 && nAchievementID < MAX_ACHIEVEMENT_ID);

    bRetCode = m_AchievementSet.GetBit(nAchievementID, &bResult);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushBoolean(L, bResult);
    nResult = 1;
Exit0:
    return nResult;
}

#ifdef _SERVER
int KPlayer::LuaAcquireAchievement(Lua_State* L)
{
    BOOL                bResult         = false;
    BOOL                bRetCode        = false;
    int                 nTopIndex       = 0;
    int                 nAchievementID  = 0;
    BOOL                bValue          = false;
    KACHIEVEMENT_INFO*  pInfo           = NULL;
    BYTE                byType          = aatInvalid;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAchievementID = (int)Lua_ValueToNumber(L, 1);

    bRetCode = AcquireAchievement(nAchievementID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}
#endif // _SERVER

int KPlayer::LuaGetAchievementPoint(Lua_State* L)
{
    int nResult     = 0;
    int nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_PushNumber(L, m_nAchievementPoint);

    nResult = 1;
Exit0:
    return nResult;
}

int KPlayer::LuaGetAchievementRecord(Lua_State* L)
{
    int nResult     = 0;
    int nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_PushNumber(L, m_nAchievementRecord);

    nResult = 1;
Exit0:
    return nResult;
}

#ifdef _CLIENT
int KPlayer::LuaOpenVenation(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nTopIndex   = 0;
    int     nVenationID = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nVenationID = (int)Lua_ValueToNumber(L, 1);

    g_PlayerClient.DoOpenVenation(nVenationID);

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);

    return 1;
}
#endif

int KPlayer::LuaSetTargetAStar(Lua_State* L)
{
    int nResult     = false;
    int nRetCode    = false;

#ifdef _SERVER
    TARGET_TYPE eTargetType = m_SelectTarget.GetTargetType();

    if (eTargetType == ttNpc)
    {
        KNpc* pNpc = NULL;
        m_SelectTarget.GetTarget(&pNpc);
        assert(pNpc);

        pNpc->m_nPathFindAstar = 1;
    }
#endif

    nResult = true;
Exit0:
    Lua_PushNumber(L, nResult);  
    return 1;
}

struct KCountPlayerID
{
    bool operator () (KPlayer* pPlayer)
    {
        if (pPlayer != m_pSelf)
        {
            m_PlayerVector.push_back(pPlayer->m_dwID);
        }

        return true;
    }

    KPlayer*           m_pSelf;
    std::vector<DWORD> m_PlayerVector;
};

int KPlayer::LuaGetAroundPlayerID(Lua_State* L)
{
    int         nResult     = 0;
    int         nRetCode    = false;
    int         nTable      = 0;
	int			nIndex		= 0;
    int         nTotal      = 0;
	KCountPlayerID CountPlayer;

    CountPlayer.m_pSelf = this;

    m_pScene->TraverseRangePlayer(m_pRegion, CountPlayer);

    Lua_NewTable(L);
    nTable = Lua_GetTopIndex(L);

    nTotal = (int)CountPlayer.m_PlayerVector.size();

    for (int nIndex = 1; nIndex <= nTotal; nIndex++)
    {
		Lua_PushNumber(L, nIndex);
        Lua_PushNumber(L, CountPlayer.m_PlayerVector[nIndex - 1]);
        Lua_SetTable(L, nTable);
    }

    nResult = 1;
Exit0:
    return nResult;
}

int KPlayer::LuaIsOnSlay(Lua_State* L)
{
    BOOL    bIsSlay     = false;
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    bIsSlay = m_PK.IsOnSlay();

    Lua_PushBoolean(L, bIsSlay);

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaSetMapVisitFlag(Lua_State* L)
{
    int     nRetCode    = false;
    int     nMapID      = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nMapID = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nMapID > 0 && nMapID <= MAX_MAP_ID);

    nRetCode = m_SceneVisitRecord.SetBit(nMapID - 1, true);
    KGLOG_PROCESS_ERROR(nRetCode);

#ifdef _SERVER
    g_PlayerServer.DoSyncVisitMapInfo(this);
#endif

Exit0:
    return 0;
}

int KPlayer::LuaGetMapVisitFlag(Lua_State* L)
{
    BOOL    bVisistFlag = false;
    int     nRetCode    = false;
    int     nMapID      = 0;

    nRetCode = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nMapID = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nMapID > 0 && nMapID <= MAX_MAP_ID);

    nRetCode = m_SceneVisitRecord.GetBit(nMapID - 1, &bVisistFlag);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    Lua_PushBoolean(L, bVisistFlag);
    return 1;
}

#ifdef _CLIENT
int KPlayer::LuaGetReviveLeftFrame(Lua_State* L)
{
    int nResult     = 0;
    int nLeftFrame  = 0;

    KGLOG_PROCESS_ERROR(m_eMoveState == cmsOnDeath);

    if (m_ReviveParam.nSituReviveFrame > g_pSO3World->m_nGameLoop)
    {
        nLeftFrame = m_ReviveParam.nSituReviveFrame - g_pSO3World->m_nGameLoop;
    }

    Lua_PushNumber(L, nLeftFrame);

    nResult = 1;
Exit0:
    return nResult;
}

int KPlayer::LuaIsPartyMemberInSameScene(Lua_State* L)
{
    int                 nResult             = 0;
    DWORD               dwTargetPlayerID    = ERROR_ID;
    KTEAM_MEMBER_INFO*  pMemberInfo         = NULL;
    int                 nTopIndex           = Lua_GetTopIndex(L);
    
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwTargetPlayerID = (DWORD)Lua_ValueToNumber(L, 1);

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pMemberInfo = g_pSO3World->m_TeamClient.GetMemberInfo(dwTargetPlayerID);
    KG_PROCESS_ERROR(pMemberInfo);
    KG_PROCESS_ERROR(m_pScene);
    KG_PROCESS_ERROR(
        pMemberInfo->dwMapID == m_pScene->m_dwMapID && 
        pMemberInfo->nMapCopyIndex == m_pScene->m_nCopyIndex
    );

    Lua_PushBoolean(L, true);
    nResult = 1;
Exit0:
    return nResult;
}

int KPlayer::LuaOpen(Lua_State* L)
{
    int         nRetCode    = 0;
	KDoodad*    pDoodad     = NULL;

    nRetCode = Lua_GetTopIndex(L);
	KGLOG_PROCESS_ERROR(nRetCode == 1);

	if (Lua_IsNumber(L, 1))
	{
		DWORD dwDoodadID = (DWORD)Lua_ValueToNumber(L, 1);
		pDoodad = g_pSO3World->m_DoodadSet.GetObj(dwDoodadID);
	}
	else if (lua_isuserdata(L, 1))
	{
		Luna<KDoodad>::KObjData* pObjData = (Luna<KDoodad>::KObjData*)Lua_ValueToUserData(L, 1);
		KGLOG_PROCESS_ERROR(pObjData);

		pDoodad = pObjData->pObj;
	}
	KGLOG_PROCESS_ERROR(pDoodad);

	nRetCode = pDoodad->CheckOpen(this);
	KG_PROCESS_ERROR(nRetCode);

	g_PlayerClient.DoCharacterOpen(pDoodad->m_dwID);
    
Exit0:
	return 0;
}

int KPlayer::LuaGetQuestDiffcultyLevel(Lua_State* L)
{
    QUEST_DIFFICULTY_LEVEL  eResult = qdlInvalid;
    int                     nTopIndex   = 0;
    DWORD                   dwQuestID   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwQuestID = (DWORD)Lua_ValueToNumber(L, 1);
    
    eResult = GetQuestDiffcultyLevel(dwQuestID);
Exit0:
    Lua_PushNumber(L, (int)eResult);
    return 1;
}
#endif

#ifdef _SERVER
int KPlayer::LuaKickout(Lua_State* L)
{
    int     nTopIndex       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    if (m_nConnIndex >= 0)
    {
        KGLogPrintf(KGLOG_INFO, "Kick account from script, shutdown(%d)\n", m_nConnIndex);

        g_PlayerServer.DoAccountKickout(m_nConnIndex);

        g_PlayerServer.Detach(m_nConnIndex);
    }

Exit0:
    return 0;
}

int KPlayer::LuaBan(Lua_State* L)
{
    BOOL    bResult     = false;
    int     nBanSeconds = 0;
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nBanSeconds = (int)Lua_ValueToNumber(L, 1);
    nBanSeconds = max(nBanSeconds, 0);

    KGLOG_PROCESS_ERROR(g_pSO3World->m_nCurrentTime + nBanSeconds >= g_pSO3World->m_nCurrentTime);

    m_nBanTime = g_pSO3World->m_nCurrentTime + nBanSeconds;

    bResult = true;
Exit0:
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaGetLastKillPointReduceTime(Lua_State* L)
{
    Lua_PushNumber(L, (int)m_nLastKillPointReduceTime);
    return 1;
}

int KPlayer::LuaSetLastKillPointReduceTime(Lua_State* L)
{
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    m_nLastKillPointReduceTime = (time_t)Lua_ValueToNumber(L, 1);

Exit0:
    return 0;
}

int KPlayer::LuaSetCurrentKillPoint(Lua_State* L)
{
    int     nTopIndex           = 0;
    int     nCurrentKillPoint   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nCurrentKillPoint = (int)Lua_ValueToNumber(L, 1);
    
    if (m_nCurrentKillPoint == 0 && nCurrentKillPoint > 0)
    {
        m_nLastKillPointReduceTime = g_pSO3World->m_nCurrentTime;
    }
    
    m_nCurrentKillPoint = nCurrentKillPoint;

    g_PlayerServer.DoSyncKillPoint(this);

Exit0:
    return 0;
}

int KPlayer::LuaSetCannotDialogWithNPC(Lua_State* L)
{
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    m_bCannotDialogWithNPC = (BOOL)Lua_ValueToBoolean(L, 1);
    g_PlayerServer.DoSetPlayerDialogFlag(this);

Exit0:
    return 0;
}

int KPlayer::LuaSetRedName(Lua_State* L)
{
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    m_bRedName = (BOOL)Lua_ValueToBoolean(L, 1);
    g_PlayerServer.DoSetPlayerRedName(this);

Exit0:
    return 0;
}

int KPlayer::LuaSetOnlyReviveInSitu(Lua_State* L)
{
    int     nTopIndex   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    m_bOnlyReviveInSitu = (BOOL)Lua_ValueToBoolean(L, 1);
    g_PlayerServer.DoSetOnlyReviveInSitu(this);

Exit0:
    return 0;
}

int KPlayer::LuaAddPrestige(Lua_State* L)
{
    int     nTopIndex       = 0;
    BOOL    bRetCode        = false;
    int     nAddPrestige    = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nAddPrestige = (int)Lua_ValueToNumber(L, 1);
    KG_PROCESS_ERROR(nAddPrestige != 0);
    
    bRetCode = AddPrestige(nAddPrestige);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    return 0;
}

int KPlayer::LuaAddPQ(Lua_State* L)
{
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    DWORD   dwPQID      = 0;
    KGPQ*   pPQ         = NULL;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQID = (DWORD)Lua_ValueToNumber(L, 1);

    pPQ = g_pSO3World->m_PQManager.GetPQ(dwPQID);
    KGLOG_PROCESS_ERROR(pPQ);

    bRetCode = m_PQList.AddPQ(dwPQID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    Lua_PushBoolean(L, bRetCode);
    return 1;
}

int KPlayer::LuaDelPQ(Lua_State* L)
{
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    DWORD   dwPQID      = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQID = (DWORD)Lua_ValueToNumber(L, 1);

    bRetCode = m_PQList.DelPQ(dwPQID);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    Lua_PushBoolean(L, bRetCode);
    return 1;
}

int KPlayer::LuaGetPQIDTable(Lua_State* L)
{
    BOOL    bRetCode        = false;
    int     nTopIndex       = 0;
    int     nIndex          = 0;
    DWORD   dwPQTemplateID  = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwPQTemplateID = (DWORD)Lua_ValueToNumber(L, 1);

    Lua_NewTable(L);

    for (KGPQList::PQ_VECTOR::iterator it = m_PQList.m_PQVector.begin(), itEnd = m_PQList.m_PQVector.end(); it != itEnd; ++it)
    {
        KGPQ* pPQ = g_pSO3World->m_PQManager.GetPQ(*it);

        if (!pPQ)
            continue;
        
        if (dwPQTemplateID && pPQ->m_pPQTemplate->dwTemplateID != dwPQTemplateID)
            continue;

        Lua_PushNumber(L, ++nIndex);

        Lua_NewTable(L);
        Lua_PushString(L, "PQID");
        Lua_PushNumber(L, pPQ->m_dwPQID);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "PQTemplateID");
        Lua_PushNumber(L, pPQ->m_pPQTemplate->dwTemplateID);
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }

Exit0:
    return 1;
}

int KPlayer::LuaSetHeroMapCopyOpenFlag(Lua_State* L)
{
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    DWORD   dwMapID     = 0;
    BOOL    bOpenFlag   = false;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwMapID     = (DWORD)Lua_ValueToNumber(L, 1);
    bOpenFlag   = (BOOL)Lua_ValueToBoolean(L, 2);

    KGLOG_PROCESS_ERROR(dwMapID > 0 && dwMapID <= MAX_MAP_ID);
    
    bRetCode = m_HeroMapCopyOpenFlag.SetBit(dwMapID - 1, bOpenFlag);
    KGLOG_PROCESS_ERROR(bRetCode);
    
Exit0:
    return 0;
}

int KPlayer::LuaGetHeroMapCopyOpenFlag(Lua_State* L)
{
    int     nResult     = 0;
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    DWORD   dwMapID     = 0;
    BOOL    bOpenFlag   = false;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    dwMapID = (DWORD)Lua_ValueToNumber(L, 1);

    KGLOG_PROCESS_ERROR(dwMapID > 0 && dwMapID <= MAX_MAP_ID);
    
    bRetCode = m_HeroMapCopyOpenFlag.GetBit(dwMapID - 1, &bOpenFlag);
    KGLOG_PROCESS_ERROR(bRetCode);
        
    Lua_PushBoolean(L, bOpenFlag);

    nResult = 1;
Exit0:
    return nResult;
}
#endif

int KPlayer::LuaSetCamp(Lua_State* L)
{
    BOOL    bRetCode    = false;
    int     nTopIndex   = 0;
    int     nCamp       = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nCamp = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nCamp == cNeutral || nCamp == cGood || nCamp == cEvil);
    KG_PROCESS_ERROR(nCamp != m_eCamp);

    if (nCamp != cNeutral)
    {
        KGLOG_PROCESS_ERROR(m_eCamp == cNeutral);
    }

#ifdef _SERVER
    g_RelayClient.DoApplySetCampRequest(m_dwID, (KCAMP)nCamp);
#endif    

#ifdef _CLIENT
    g_PlayerClient.DoApplySetCampRequest((KCAMP)nCamp);
#endif

Exit0:
    return 0;
}

#ifdef _SERVER
int KPlayer::LuaGetReviveCtrl(Lua_State* L)
{
    int nTopIndex = 0;

    KGLOG_PROCESS_ERROR(g_pSO3World->m_Settings.m_ConstList.bUsePlayerReviveCtrl);
    
    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_NewTable(L);

    Lua_PushString(L, "nNextCheckReviveFrame");
    Lua_PushNumber(L, m_ReviveCtrl.nNextCheckReviveFrame);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "nReviveFrame");
    Lua_PushNumber(L, m_ReviveCtrl.nReviveFrame);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "bReviveInSitu");
    Lua_PushBoolean(L, m_ReviveCtrl.bReviveInSitu);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "bReviveInAltar");
    Lua_PushBoolean(L, m_ReviveCtrl.bReviveInAltar);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "bReviveByPlayer");
    Lua_PushBoolean(L, m_ReviveCtrl.bReviveByPlayer);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "nLastSituReviveTime");
    Lua_PushNumber(L, m_ReviveCtrl.nLastSituReviveTime);
    Lua_SetTable(L, -3);

    Lua_PushString(L, "nSituReviveCount");
    Lua_PushNumber(L, m_ReviveCtrl.nSituReviveCount);
    Lua_SetTable(L, -3);

    return 1;
Exit0:
    return 0;
}

int KPlayer::LuaSetReviveCtrl(Lua_State* L)
{
    BOOL                bResult     = false;
    int                 nRetCode    = false;
    int                 nTopIndex   = 0;
    KPLAYER_REVIVE_CTRL NewReviveCtrl;

    KGLOG_PROCESS_ERROR(g_pSO3World->m_Settings.m_ConstList.bUsePlayerReviveCtrl);

    memcpy(&NewReviveCtrl, &m_ReviveCtrl, sizeof(NewReviveCtrl));
    
    nTopIndex = Lua_GetTopIndex(L);

    nRetCode = Lua_IsTable(L, nTopIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    lua_getfield(L, -1, "nNextCheckReviveFrame");
    KGLOG_PROCESS_ERROR(Lua_IsNumber(L, -1));
    NewReviveCtrl.nNextCheckReviveFrame = (int)Lua_ValueToNumber(L, -1);
    Lua_Pop(L, 1);

    lua_getfield(L, -1, "nReviveFrame");
    KGLOG_PROCESS_ERROR(Lua_IsNumber(L, -1));
    NewReviveCtrl.nReviveFrame = (int)Lua_ValueToNumber(L, -1);
    Lua_Pop(L, 1);

    lua_getfield(L, -1, "bReviveInSitu");
    KGLOG_PROCESS_ERROR(Lua_IsBoolean(L, -1));
    NewReviveCtrl.bReviveInSitu = Lua_ValueToBoolean(L, -1);
    Lua_Pop(L, 1);

    lua_getfield(L, -1, "bReviveInAltar");
    KGLOG_PROCESS_ERROR(Lua_IsBoolean(L, -1));
    NewReviveCtrl.bReviveInAltar = Lua_ValueToBoolean(L, -1);
    Lua_Pop(L, 1);

    lua_getfield(L, -1, "bReviveByPlayer");
    KGLOG_PROCESS_ERROR(Lua_IsBoolean(L, -1));
    NewReviveCtrl.bReviveByPlayer = Lua_ValueToBoolean(L, -1);
    Lua_Pop(L, 1);

    lua_getfield(L, -1, "nLastSituReviveTime");
    KGLOG_PROCESS_ERROR(Lua_IsNumber(L, -1));
    NewReviveCtrl.nLastSituReviveTime = (time_t)Lua_ValueToNumber(L, -1);
    Lua_Pop(L, 1);

    lua_getfield(L, -1, "nSituReviveCount");
    KGLOG_PROCESS_ERROR(Lua_IsNumber(L, -1));
    NewReviveCtrl.nSituReviveCount = (int)Lua_ValueToNumber(L, -1);
    Lua_Pop(L, 1);

    m_ReviveCtrl = NewReviveCtrl;

    g_PlayerServer.DoSyncPlayerReviveCtrl(this); 

    bResult = true;
Exit0:
    if (nTopIndex)
    {
        Lua_SetTopIndex(L, nTopIndex);
        nTopIndex = 0;
    }
    Lua_PushBoolean(L, bResult);
    return 1;
}

int KPlayer::LuaSetMaxLevel(Lua_State* L)
{
    int nTopIndex = 0;
    int nMaxLevel = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);

    nMaxLevel = (int)Lua_ValueToNumber(L, 1);
    KGLOG_PROCESS_ERROR(nMaxLevel > 0);

    m_nMaxLevel = nMaxLevel;

    g_PlayerServer.DoSyncMaxLevel(m_nConnIndex, m_nMaxLevel);
Exit0:
    return 0;
}
#endif // _SERVER

#ifdef _CLIENT
void KPlayer::LuaHideHat(Lua_State* L)
{   
    int     nTopIndex    = 0;
    BOOL    bHideHatFlag = false;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 1);
    
	bHideHatFlag = Lua_ValueToBoolean(L, 1);

    m_bHideHat   = bHideHatFlag;
    g_PlayerClient.DoSetHideHatFlag(bHideHatFlag);
    
Exit0:
    return;
}
#endif

//////////////////////////////////////////////////////////////////////////

DEFINE_LUA_CLASS_BEGIN(KPlayer)
	//注册属性
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

	REGISTER_LUA_STRING(KCharacter, Name)
	REGISTER_LUA_STRING(KCharacter, Title)
    REGISTER_LUA_ENUM(KCharacter, Gender)
	REGISTER_LUA_ENUM(KCharacter, Kind)
	REGISTER_LUA_DWORD(KCharacter, ClassID)
	REGISTER_LUA_ENUM(KCharacter, Race)
	REGISTER_LUA_DWORD_READONLY(KCharacter, ForceID)
	REGISTER_LUA_INTEGER(KCharacter, Level)
    REGISTER_LUA_DWORD(KCharacter, ModelID)
	REGISTER_LUA_INTEGER(KPlayer, Experience)
    REGISTER_LUA_BOOL(KPlayer, OnPracticeRoom)
	REGISTER_LUA_INTEGER(KPlayer, CurrentStamina)
    REGISTER_LUA_INTEGER(KPlayer, CurrentThew)
    REGISTER_LUA_INTEGER(KPlayer, MaxStamina)
    REGISTER_LUA_INTEGER(KPlayer, MaxThew)
	REGISTER_LUA_DWORD(KPlayer, CharacterAction)

    REGISTER_LUA_INTEGER_READONLY(KPlayer, BattleFieldSide)

    REGISTER_LUA_DWORD(KPlayer, SchoolID)

    REGISTER_LUA_INTEGER_READONLY(KPlayer, CurrentTrainValue)
    REGISTER_LUA_INTEGER_READONLY(KPlayer, MaxTrainValue)
    REGISTER_LUA_INTEGER(KPlayer, UsedTrainValue)
#ifdef _SERVER
    REGISTER_LUA_INTEGER(KPlayer, AddTrainTimeInToday)
#endif

    REGISTER_LUA_INTEGER_READONLY(KPlayer, BaseVenationCof)
    REGISTER_LUA_INTEGER_READONLY(KPlayer, PhysicsVenationCof)
    REGISTER_LUA_INTEGER_READONLY(KPlayer, MagicVenationCof)
    REGISTER_LUA_INTEGER_READONLY(KPlayer, AssistVenationCof)

    // ------------------ 移动相关 ---------------------------->
    REGISTER_LUA_ENUM_READONLY(KCharacter, MoveState)

	REGISTER_LUA_INTEGER_READONLY(KCharacter, VelocityXY)
	REGISTER_LUA_INTEGER_READONLY(KCharacter, DirectionXY)
	REGISTER_LUA_INTEGER_READONLY(KCharacter, VelocityZ)

	REGISTER_LUA_INTEGER_READONLY(KCharacter, DestX)
	REGISTER_LUA_INTEGER_READONLY(KCharacter, DestY)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, RunSpeed)

	REGISTER_LUA_INTEGER_READONLY(KCharacter, RunSpeedBase)

	REGISTER_LUA_INTEGER_READONLY(KCharacter, MoveSpeedPercent)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, MaxJumpCount)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, Gravity)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, GravityBase)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, GravityPercent)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, JumpSpeed)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, JumpSpeedBase)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, JumpSpeedPercent)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, DropDefence)

    REGISTER_LUA_INTEGER_READONLY(KCharacter, DivingCount)         
    REGISTER_LUA_INTEGER_READONLY(KCharacter, DivingFrame)         
    REGISTER_LUA_INTEGER_READONLY(KCharacter, DivingFrameBase)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, DivingFramePercent) 

    REGISTER_LUA_INTEGER_READONLY(KCharacter, WaterFlyAbility)

    // ------------------- 潜能 ------------------------------->
	REGISTER_LUA_INTEGER(KCharacter, CurrentStrength)
    REGISTER_LUA_INTEGER(KCharacter, StrengthBase)
    REGISTER_LUA_INTEGER(KCharacter, StrengthBasePercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, CurrentAgility)
    REGISTER_LUA_INTEGER(KCharacter, AgilityBase)
    REGISTER_LUA_INTEGER(KCharacter, AgilityBasePercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, CurrentVitality)
    REGISTER_LUA_INTEGER(KCharacter, VitalityBase)
    REGISTER_LUA_INTEGER(KCharacter, VitalityBasePercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, CurrentSpirit)
    REGISTER_LUA_INTEGER(KCharacter, SpiritBase)
    REGISTER_LUA_INTEGER(KCharacter, SpiritBasePercentAdd)
    REGISTER_LUA_INTEGER(KCharacter, CurrentSpunk)
    REGISTER_LUA_INTEGER(KCharacter, SpunkBase)
    REGISTER_LUA_INTEGER(KCharacter, SpunkBasePercentAdd)

    // ------------------- 生命 ------------------------------->
	REGISTER_LUA_INTEGER(KCharacter, CurrentLife)
	REGISTER_LUA_INTEGER(KCharacter, MaxLife)
    REGISTER_LUA_INTEGER(KCharacter, MaxLifeBase)
	REGISTER_LUA_INTEGER(KCharacter, MaxLifePercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, MaxLifeAdditional)
	REGISTER_LUA_INTEGER(KCharacter, LifeReplenish)
	REGISTER_LUA_INTEGER(KCharacter, LifeReplenishPercent)
    REGISTER_LUA_INTEGER(KCharacter, LifeReplenishCoefficient)
    REGISTER_LUA_INTEGER(KCharacter, LifeReplenishExt)

    // ------------------- 破绽 ------------------------------->
    REGISTER_LUA_INTEGER(KCharacter, UpperWeakLevel)
    REGISTER_LUA_INTEGER(KCharacter, MiddleWeakLevel)
    REGISTER_LUA_INTEGER(KCharacter, LowerWeakLevel)

    // ------------------- 内力 ------------------------------->
	REGISTER_LUA_INTEGER(KCharacter, CurrentMana)
	REGISTER_LUA_INTEGER(KCharacter, MaxMana)
    REGISTER_LUA_INTEGER(KCharacter, MaxManaBase)
	REGISTER_LUA_INTEGER(KCharacter, MaxManaPercentAdd)
	REGISTER_LUA_INTEGER(KCharacter, MaxManaAdditional)
	REGISTER_LUA_INTEGER(KCharacter, ManaReplenish)
	REGISTER_LUA_INTEGER(KCharacter, ManaReplenishPercent)
    REGISTER_LUA_INTEGER(KCharacter, ManaReplenishCoefficient)
    REGISTER_LUA_INTEGER(KCharacter, ManaReplenishExt)

    // -------------------- 战斗杂项 -------------------------->
	REGISTER_LUA_INTEGER(KCharacter, Dodge)
    REGISTER_LUA_INTEGER(KCharacter, DodgeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, ParryBaseRate)
	REGISTER_LUA_INTEGER(KCharacter, Parry)
	REGISTER_LUA_INTEGER(KCharacter, ParryValue)
	REGISTER_LUA_INTEGER(KCharacter, Sense)
	REGISTER_LUA_INTEGER(KCharacter, Strain)
    REGISTER_LUA_INTEGER(KCharacter, ToughnessBaseRate)
	REGISTER_LUA_INTEGER(KCharacter, Toughness)
    REGISTER_LUA_INTEGER(KCharacter, CriticalDamagePowerBaseKiloNumRate)
	REGISTER_LUA_INTEGER(KCharacter, CriticalDamagePower)
	REGISTER_LUA_INTEGER(KCharacter, DecriticalDamagePowerBaseKiloNumRate)
	REGISTER_LUA_INTEGER(KCharacter, DecriticalDamagePower)
    REGISTER_LUA_BOOL(KCharacter,    FightState)
    REGISTER_LUA_BOOL(KCharacter,    SheathFlag)
    REGISTER_LUA_INTEGER(KCharacter, PositiveShield)
    REGISTER_LUA_INTEGER(KCharacter, NegativeShield)
    REGISTER_LUA_INTEGER(KCharacter, GlobalBlock)
    REGISTER_LUA_INTEGER(KCharacter, GlobalDamageAbsorb)
    REGISTER_LUA_INTEGER(KCharacter, GlobalDamageManaShield)
    REGISTER_LUA_INTEGER(KCharacter, DamageToLifeForSelf)
    REGISTER_LUA_INTEGER(KCharacter, DamageToManaForSelf)
    REGISTER_LUA_INTEGER(KCharacter, BeatBackRate)
    REGISTER_LUA_INTEGER(KCharacter, StunStrikeRate)
    REGISTER_LUA_INTEGER(KCharacter, KnockedOffRate)
    REGISTER_LUA_INTEGER(KCharacter, KnockedBackRate)
    REGISTER_LUA_INTEGER(KCharacter, MountingRate)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, AccumulateValue)

    // -------------------- 外功 ----------------------------->
    REGISTER_LUA_INTEGER(KCharacter, PhysicsAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, SkillPhysicsDamageRand)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsHitValue)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsOvercome)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsShield)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsReflection)		
    REGISTER_LUA_INTEGER(KCharacter, PhysicsReflectionPercent)
    REGISTER_LUA_INTEGER(KCharacter, PhysicsDamageAbsorb)    
    REGISTER_LUA_INTEGER(KCharacter, PhysicsDamageManaShield)

	REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponDamageBase)
	REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponDamageRand)

	REGISTER_LUA_INTEGER(KCharacter, RangeWeaponDamageBase)
	REGISTER_LUA_INTEGER(KCharacter, RangeWeaponDamageRand)

    // -------------------- 阳性内功 ----------------------------->
    REGISTER_LUA_INTEGER(KCharacter, SolarAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, SolarHitValue)
    REGISTER_LUA_INTEGER(KCharacter, SolarHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, SolarCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, SolarCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, SolarOvercome)
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicShield)
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicReflection)		
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicReflectionPercent)
    REGISTER_LUA_INTEGER(KCharacter, SolarMagicBlock)
    REGISTER_LUA_INTEGER(KCharacter, SolarDamageAbsorb)     
    REGISTER_LUA_INTEGER(KCharacter, SolarDamageManaShield)

    // -------------------- 中性内功 ----------------------------->
    REGISTER_LUA_INTEGER(KCharacter, NeutralAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, NeutralHitValue)
    REGISTER_LUA_INTEGER(KCharacter, NeutralHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, NeutralCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, NeutralCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, NeutralOvercome)
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicShield)
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicReflection)		
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicReflectionPercent)
    REGISTER_LUA_INTEGER(KCharacter, NeutralMagicBlock)
    REGISTER_LUA_INTEGER(KCharacter, NeutralDamageAbsorb)  
    REGISTER_LUA_INTEGER(KCharacter, NeutralDamageManaShield)

    // -------------------- 阴性内功 ----------------------------->
    REGISTER_LUA_INTEGER(KCharacter, LunarAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, LunarHitValue)
    REGISTER_LUA_INTEGER(KCharacter, LunarHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, LunarCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, LunarCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, LunarOvercome)
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicShield)
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicReflection)		
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicReflectionPercent)	
    REGISTER_LUA_INTEGER(KCharacter, LunarMagicBlock)
    REGISTER_LUA_INTEGER(KCharacter, LunarDamageAbsorb)     
    REGISTER_LUA_INTEGER(KCharacter, LunarDamageManaShield) 

    // -------------------- 毒性 --------------------------------->
    REGISTER_LUA_INTEGER(KCharacter, PoisonAttackPower)
    REGISTER_LUA_INTEGER(KCharacter, PoisonHitValue)
    REGISTER_LUA_INTEGER(KCharacter, PoisonHitBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, PoisonCriticalStrike)
    REGISTER_LUA_INTEGER(KCharacter, PoisonCriticalStrikeBaseRate)
    REGISTER_LUA_INTEGER(KCharacter, PoisonOvercome)
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicShield)
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicResistPercent)
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicReflection)		
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicReflectionPercent)	
    REGISTER_LUA_INTEGER(KCharacter, PoisonMagicBlock)
    REGISTER_LUA_INTEGER(KCharacter, PoisonDamageAbsorb)     
    REGISTER_LUA_INTEGER(KCharacter, PoisonDamageManaShield) 

    // -------------------- 治疗 --------------------------------->
    REGISTER_LUA_INTEGER(KCharacter, TherapyPower)
    REGISTER_LUA_INTEGER(KCharacter, SkillTherapy)
    
    // ------------------- 新手技能有关 -------------------------->
    REGISTER_LUA_INTEGER(KCharacter, NoneWeaponAttackSpeedBase)

    REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponAttackSpeed)
    REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponAttackSpeedBase)
    REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponAttackSpeedPercentAdd)
    REGISTER_LUA_INTEGER(KCharacter, MeleeWeaponAttackSpeedAdditional)

    REGISTER_LUA_INTEGER(KCharacter, RangeWeaponAttackSpeed)
    REGISTER_LUA_INTEGER(KCharacter, RangeWeaponAttackSpeedBase)
    REGISTER_LUA_INTEGER(KCharacter, RangeWeaponAttackSpeedPercentAdd)
    REGISTER_LUA_INTEGER(KCharacter, RangeWeaponAttackSpeedAdditional)

    REGISTER_LUA_DWORD_READONLY(KCharacter, AIEventSrc)
    REGISTER_LUA_INTEGER_READONLY(KCharacter, AIEventParam)
    // -------------------------------------------------------------

#ifdef _SERVER
	REGISTER_LUA_STRING_READONLY(KPlayer, Account)
    REGISTER_LUA_DWORD(KPlayer, KillerID)
    REGISTER_LUA_DWORD(KPlayer, LastScriptID)
#endif

    REGISTER_LUA_ENUM_READONLY(KPlayer, Camp)
    REGISTER_LUA_INTEGER_READONLY(KPlayer, CurrentPrestige)
	REGISTER_LUA_DWORD(KPlayer, TeamID)
	REGISTER_LUA_ENUM(KPlayer, RoleType)
	REGISTER_LUA_DWORD(KPlayer, BankNpcID)
    REGISTER_LUA_BOOL(KPlayer, OnHorse)

#ifdef _SERVER
    REGISTER_LUA_DWORD(KPlayer, Reviver)
    REGISTER_LUA_INTEGER(KPlayer, ReviveX)
    REGISTER_LUA_INTEGER(KPlayer, ReviveY)
    REGISTER_LUA_INTEGER(KPlayer, ReviveZ)
    REGISTER_LUA_INTEGER(KPlayer, ReviveLife)
    REGISTER_LUA_INTEGER(KPlayer, ReviveMana)
#endif

    REGISTER_LUA_DWORD(KPlayer, TongID)
    REGISTER_LUA_INTEGER_READONLY(KPlayer, MaxLevel)
    REGISTER_LUA_INTEGER_READONLY(KPlayer, Contribution)

#ifdef _CLIENT
    REGISTER_LUA_DWORD(KPlayer, FormationEffectID)
    REGISTER_LUA_INTEGER(KPlayer, FormationEffectLevel)

	REGISTER_LUA_BOOL_READONLY(KPlayer, HideHat)
#endif
    
    REGISTER_LUA_INTEGER_READONLY(KPlayer, CurrentKillPoint)
#ifdef _SERVER
    REGISTER_LUA_BOOL(KPlayer, AbradeEquipmentDouble)
    REGISTER_LUA_BOOL(KPlayer, AbradeEquipmentAll)
    REGISTER_LUA_BOOL(KPlayer, AttackByGuard)
#endif
    REGISTER_LUA_BOOL_READONLY(KPlayer, OnlyReviveInSitu)
    REGISTER_LUA_BOOL_READONLY(KPlayer, CannotDialogWithNPC)
    REGISTER_LUA_BOOL_READONLY(KPlayer, RedName)
    REGISTER_LUA_BOOL(KPlayer, HeroFlag)
    
	//注册函数
    REGISTER_LUA_FUNC(KCharacter, AutoFly)
    REGISTER_LUA_FUNC(KCharacter, SetModelID)

	REGISTER_LUA_FUNC(KPlayer, Stop)

    REGISTER_LUA_FUNC(KCharacter, Stand)
	REGISTER_LUA_FUNC(KCharacter, WalkTo)
	REGISTER_LUA_FUNC(KCharacter, RunTo)
    REGISTER_LUA_FUNC(KCharacter, SwimTo)
	REGISTER_LUA_FUNC(KCharacter, Jump)
	REGISTER_LUA_FUNC(KCharacter, SitDown)
	REGISTER_LUA_FUNC(KCharacter, Dash)

	REGISTER_LUA_FUNC(KCharacter, Patrol)

	REGISTER_LUA_FUNC(KCharacter, Die)

	REGISTER_LUA_FUNC(KCharacter, TurnTo)
    REGISTER_LUA_FUNC(KCharacter, PitchTo)
	REGISTER_LUA_FUNC(KCharacter, DoAction)

#ifdef _SERVER
	REGISTER_LUA_FUNC(KCharacter, SetPosition)
#endif

    REGISTER_LUA_FUNC(KCharacter, FireAIEvent)

	REGISTER_LUA_FUNC(KPlayer, GetItem)
	REGISTER_LUA_FUNC(KPlayer, DelItem)
	REGISTER_LUA_FUNC(KPlayer, GetItemByIndex)
	REGISTER_LUA_FUNC(KPlayer, GetEquipItem)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, CanAddItem)
    REGISTER_LUA_FUNC(KPlayer, AddItem)
    REGISTER_LUA_FUNC(KPlayer, CostItem)
    REGISTER_LUA_FUNC(KPlayer, AddMoney)
    REGISTER_LUA_FUNC(KPlayer, AddContribution)
    REGISTER_LUA_FUNC(KPlayer, OpenWindow)
    REGISTER_LUA_FUNC(KPlayer, GetLastSaveTime)
    REGISTER_LUA_FUNC(KPlayer, GetLastLoginTime)
    REGISTER_LUA_FUNC(KPlayer, GetCurrentLoginTime)
#endif
	REGISTER_LUA_FUNC(KPlayer, GetMoney)
	REGISTER_LUA_FUNC(KPlayer, ExchangeItem)

	REGISTER_LUA_FUNC(KPlayer, DestroyItem)

	REGISTER_LUA_FUNC(KPlayer, GetRepresentID)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, SetRepresentID)
#endif

	REGISTER_LUA_FUNC(KPlayer, EnableBankPackage)
	REGISTER_LUA_FUNC(KPlayer, GetBankPackageCount)
	REGISTER_LUA_FUNC(KPlayer, IsBankPackageEnabled)

	REGISTER_LUA_FUNC(KPlayer, GetBoxType)
	REGISTER_LUA_FUNC(KPlayer, GetBoxSize)
	REGISTER_LUA_FUNC(KPlayer, GetContainType)
	REGISTER_LUA_FUNC(KPlayer, CanExchange)
	REGISTER_LUA_FUNC(KPlayer, OpenBank)
	REGISTER_LUA_FUNC(KPlayer, CloseBank)
	REGISTER_LUA_FUNC(KPlayer, GetEquipPos)
	REGISTER_LUA_FUNC(KPlayer, GetFreeRoom)
	REGISTER_LUA_FUNC(KPlayer, GetFreeRoomSize)
    REGISTER_LUA_FUNC(KPlayer, GetBoxFreeRoomSize)
	REGISTER_LUA_FUNC(KPlayer, GetFreeRoomInPackage)
	REGISTER_LUA_FUNC(KPlayer, GetStackRoomInPackage)
	REGISTER_LUA_FUNC(KPlayer, GetStackRoomInBank)
	REGISTER_LUA_FUNC(KPlayer, SatisfyRequire)
    
    REGISTER_LUA_FUNC(KPlayer, GetItemAmountInAllPackages)
	REGISTER_LUA_FUNC(KPlayer, GetItemAmount)

    REGISTER_LUA_FUNC(KPlayer, SetTarget)
	REGISTER_LUA_FUNC(KPlayer, GetTarget)
    REGISTER_LUA_FUNC(KPlayer, CastSkillXYZ)
    REGISTER_LUA_FUNC(KPlayer, CastSkill)

	REGISTER_LUA_FUNC(KPlayer, GetSchoolList)
	REGISTER_LUA_FUNC(KPlayer, GetKungfuList)
    REGISTER_LUA_FUNC(KPlayer, GetSkillList)
	REGISTER_LUA_FUNC(KPlayer, GetAllSkillList)
#ifdef _CLIENT
    REGISTER_LUA_FUNC(KPlayer, GetSkillRecipeKey)
	REGISTER_LUA_FUNC(KPlayer, GetSkillRecipeKeyCount)
#endif

	REGISTER_LUA_FUNC(KPlayer, CanAcceptQuest)
	REGISTER_LUA_FUNC(KPlayer, CanFinishQuest)
	REGISTER_LUA_FUNC(KPlayer, GetQuestState)
	REGISTER_LUA_FUNC(KPlayer, GetQuestPhase)
	REGISTER_LUA_FUNC(KPlayer, GetQuestList)
	REGISTER_LUA_FUNC(KPlayer, GetQuestTree)
	REGISTER_LUA_FUNC(KPlayer, GetQuestIndex)
	REGISTER_LUA_FUNC(KPlayer, GetQuestID)
	REGISTER_LUA_FUNC(KPlayer, GetQuestExpAttenuation)
#ifdef _CLIENT
	REGISTER_LUA_FUNC(KPlayer, GetQuestTraceInfo)
    REGISTER_LUA_FUNC(KPlayer, GetQuestAssistDailyCount)
#endif
	REGISTER_LUA_FUNC(KPlayer, AcceptQuest)
	REGISTER_LUA_FUNC(KPlayer, FinishQuest)
	REGISTER_LUA_FUNC(KPlayer, CancelQuest)
    REGISTER_LUA_FUNC(KPlayer, ClearQuest) 
	REGISTER_LUA_FUNC(KPlayer, ShareQuest)
    REGISTER_LUA_FUNC(KPlayer, GetFinishedDailyQuestCount)
#ifdef _SERVER
	REGISTER_LUA_FUNC(KPlayer, ForceFinishQuest)
#endif
#ifdef _CLIENT
    REGISTER_LUA_FUNC(KPlayer, AcceptEscortQuest)
#endif
#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, RegisterQuestAssistance)
#endif

	REGISTER_LUA_FUNC(KPlayer, SetQuestValue)
	REGISTER_LUA_FUNC(KPlayer, GetQuestValue)
	REGISTER_LUA_FUNC(KPlayer, SetQuestBitValue)
	REGISTER_LUA_FUNC(KPlayer, GetQuestBitValue)

	REGISTER_LUA_FUNC(KPlayer, SetKillNpcCount)
	REGISTER_LUA_FUNC(KPlayer, GetKillNpcCount)
	REGISTER_LUA_FUNC(KPlayer, GetQuestLastTime)
	REGISTER_LUA_FUNC(KPlayer, SetQuestFailedFlag)
	REGISTER_LUA_FUNC(KPlayer, GetQuestFailedFlag)

	REGISTER_LUA_FUNC(KPlayer, GetProfession)
	REGISTER_LUA_FUNC(KPlayer, GetProfessionLevel)
    REGISTER_LUA_FUNC(KPlayer, GetProfessionMaxLevel)
    REGISTER_LUA_FUNC(KPlayer, GetProfessionProficiency)
	REGISTER_LUA_FUNC(KPlayer, GetProfessionBranch)
	REGISTER_LUA_FUNC(KPlayer, ForgetProfession)
	REGISTER_LUA_FUNC(KPlayer, CastProfessionSkill)
	REGISTER_LUA_FUNC(KPlayer, GetBookList)
	REGISTER_LUA_FUNC(KPlayer, GetBookSegmentList)
    REGISTER_LUA_FUNC(KPlayer, IsBookMemorized)
	REGISTER_LUA_FUNC(KPlayer, GetRecipe)

    REGISTER_LUA_FUNC(KPlayer, CanLearnSkill)
	REGISTER_LUA_FUNC(KPlayer, LearnSkill)

#if defined(_CLIENT)
	REGISTER_LUA_FUNC(KPlayer, LearnRecipe)
#endif

#if defined(_SERVER)
	REGISTER_LUA_FUNC(KPlayer, AddExp)
    REGISTER_LUA_FUNC(KPlayer, SetLastEntry)
    REGISTER_LUA_FUNC(KPlayer, SwitchMap)
    REGISTER_LUA_FUNC(KPlayer, ResetMap)
    REGISTER_LUA_FUNC(KPlayer, AddSkillExp)
    REGISTER_LUA_FUNC(KPlayer, ForgetSkill)

	REGISTER_LUA_FUNC(KPlayer, LearnProfession)
	REGISTER_LUA_FUNC(KPlayer, LearnBranch)
	REGISTER_LUA_FUNC(KPlayer, AddProfessionProficiency)
	REGISTER_LUA_FUNC(KPlayer, SetMaxProfessionLevel)
	REGISTER_LUA_FUNC(KPlayer, LearnRecipe)
	REGISTER_LUA_FUNC(KPlayer, SetPartyLootMode)
	REGISTER_LUA_FUNC(KPlayer, CanDoCustomOTAction)
    REGISTER_LUA_FUNC(KPlayer, DoCustomOTAction)
	REGISTER_LUA_FUNC(KPlayer, AddStamina)
    REGISTER_LUA_FUNC(KPlayer, AddThew)
#endif	//_SERVER

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KPlayer, OpenBook)
#endif    

	REGISTER_LUA_FUNC(KPlayer, CanLearnProfession)
	REGISTER_LUA_FUNC(KPlayer, CanLearnBranch)
	REGISTER_LUA_FUNC(KPlayer, CanLearnRecipe)
    REGISTER_LUA_FUNC(KPlayer, IsProfessionLearnedByCraftID)
	REGISTER_LUA_FUNC(KPlayer, IsRecipeLearned)

#if defined(_CLIENT)
	REGISTER_LUA_FUNC(KPlayer, WindowSelect)

    REGISTER_LUA_FUNC(KPlayer, DoDeathRespond)

	REGISTER_LUA_FUNC(KPlayer, GetItemPos)
	REGISTER_LUA_FUNC(KPlayer, UseItem)

	REGISTER_LUA_FUNC(KPlayer, OnCloseLootWindow)
#endif
#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, UseItemOnNoTarget)
    REGISTER_LUA_FUNC(KPlayer, UseItemOnNpc)
    REGISTER_LUA_FUNC(KPlayer, UseItemOnPlayer)
    REGISTER_LUA_FUNC(KPlayer, UseItemOnItem)
    REGISTER_LUA_FUNC(KPlayer, UseItemOnDoodad)
    REGISTER_LUA_FUNC(KPlayer, UseItemOnCoordinates)
#endif

    REGISTER_LUA_FUNC(KPlayer, GetCommonSkill)
#if defined(_CLIENT)
    REGISTER_LUA_FUNC(KPlayer, CastCommonSkill)
    REGISTER_LUA_FUNC(KPlayer, StopCurrentAction)
#endif	//_CLIENT

	REGISTER_LUA_FUNC(KPlayer, GetItemCDProgress)
    REGISTER_LUA_FUNC(KPlayer, GetSkillCDProgress)
    REGISTER_LUA_FUNC(KPlayer, GetCDInterval)
    REGISTER_LUA_FUNC(KPlayer, GetCDLeft)
    REGISTER_LUA_FUNC(KPlayer, GetSkillLevel)

    REGISTER_LUA_FUNC(KPlayer, GetSkillLifeCost)
    REGISTER_LUA_FUNC(KPlayer, GetSkillManaCost)
    REGISTER_LUA_FUNC(KPlayer, GetSkillStaminaCost)

    REGISTER_LUA_FUNC(KPlayer, GetSkillPrepare)
    REGISTER_LUA_FUNC(KPlayer, GetSkillChannel)

    REGISTER_LUA_FUNC(KPlayer, GetSkillMinRadius)
    REGISTER_LUA_FUNC(KPlayer, GetSkillMaxRadius)

    REGISTER_LUA_FUNC(KPlayer, GetSkillExp)

    REGISTER_LUA_FUNC(KPlayer, MountKungfu)
    REGISTER_LUA_FUNC(KPlayer, UmountKungfu)
    REGISTER_LUA_FUNC(KPlayer, GetKungfuMount)

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KPlayer, GetBuffList)
#endif

    REGISTER_LUA_FUNC(KPlayer, CancelBuff)

    REGISTER_LUA_FUNC(KPlayer, SetSheath)
	REGISTER_LUA_FUNC(KPlayer, GetScene)

    REGISTER_LUA_FUNC(KPlayer, SearchForPartyMember)

	REGISTER_LUA_FUNC(KPlayer, GetOTActionState)

#ifdef _SERVER
	REGISTER_LUA_FUNC(KPlayer, AddSkillRecipe)
	REGISTER_LUA_FUNC(KPlayer, DelSkillRecipe)
#endif
    REGISTER_LUA_FUNC(KPlayer, GetSkillRecipeList)
    REGISTER_LUA_FUNC(KPlayer, IsSkillRecipeExist)
    REGISTER_LUA_FUNC(KPlayer, CanLearnSkillRecipe)
    REGISTER_LUA_FUNC(KPlayer, ActiveSkillRecipe)
    REGISTER_LUA_FUNC(KPlayer, DeactiveSKillRecipe)

	REGISTER_LUA_FUNC(KPlayer, GetMapID)
	REGISTER_LUA_FUNC(KPlayer, CanDialog)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, AddBuff)

    REGISTER_LUA_FUNC(KPlayer, GetBuff)

    REGISTER_LUA_FUNC(KPlayer, DelBuff)
    REGISTER_LUA_FUNC(KPlayer, DelGroupBuff)

    REGISTER_LUA_FUNC(KPlayer, DelMultiGroupBuffByFunctionType)

    REGISTER_LUA_FUNC(KPlayer, SetBuffNextActiveFrame)
    REGISTER_LUA_FUNC(KPlayer, SetBuffLeftActiveCount)
#endif

	//--- AI ----------------------------------
#if defined(_CLIENT)
	REGISTER_LUA_FUNC(KPlayer, FollowSelectTarget)
	REGISTER_LUA_FUNC(KPlayer, StopFollow)
#endif //_CLINET

	REGISTER_LUA_FUNC(KPlayer, SearchForAllies)
	REGISTER_LUA_FUNC(KPlayer, SearchForEnemy)
#if defined(_SERVER)
	REGISTER_LUA_FUNC(KPlayer, SetReputation)
	REGISTER_LUA_FUNC(KPlayer, AddReputation)
	REGISTER_LUA_FUNC(KPlayer, OpenReputation)
	REGISTER_LUA_FUNC(KCharacter, ClearAllThreat)
	REGISTER_LUA_FUNC(KCharacter, SetForceID)

	REGISTER_LUA_FUNC(KPlayer, SetTimer)
	REGISTER_LUA_FUNC(KPlayer, StopTimer)
	REGISTER_LUA_FUNC(KPlayer, RestartTimer)
	REGISTER_LUA_FUNC(KPlayer, RemoveTimer)
#endif
    REGISTER_LUA_FUNC(KPlayer, GetReputeLevel)
	REGISTER_LUA_FUNC(KPlayer, GetReputation)
	REGISTER_LUA_FUNC(KPlayer, IsReputationHide)

#if defined(_CLIENT)
	REGISTER_LUA_FUNC(KPlayer, SetFightFlag)
	REGISTER_LUA_FUNC(KPlayer, GetFightFlag)
	REGISTER_LUA_FUNC(KPlayer, IsFightFlagLocked)
#endif

    REGISTER_LUA_FUNC(KCharacter, GetAIType)
    REGISTER_LUA_FUNC(KCharacter, SetAIType)

    //--- Talk --------------------------------
#if defined(_CLIENT)
    REGISTER_LUA_FUNC(KPlayer, GetTalkData)
    REGISTER_LUA_FUNC(KPlayer, GetTalkLinkItem)
    REGISTER_LUA_FUNC(KPlayer, Talk)
    REGISTER_LUA_FUNC(KPlayer, DoAnimation)

	REGISTER_LUA_FUNC(KPlayer, IsInParty)
	REGISTER_LUA_FUNC(KPlayer, IsPlayerInMyParty)
	REGISTER_LUA_FUNC(KPlayer, IsPartyLeader)
	REGISTER_LUA_FUNC(KPlayer, IsPartyFull)
#endif

    //--- 好友 --------------------------------
    REGISTER_LUA_FUNC(KPlayer, AddFellowship)
    REGISTER_LUA_FUNC(KPlayer, DelFellowship)
    REGISTER_LUA_FUNC(KPlayer, CanAddFoe)
    REGISTER_LUA_FUNC(KPlayer, AddFoe)
    REGISTER_LUA_FUNC(KPlayer, DelFoe)
    REGISTER_LUA_FUNC(KPlayer, AddBlackList)
    REGISTER_LUA_FUNC(KPlayer, DelBlackList)
    REGISTER_LUA_FUNC(KPlayer, AddFellowshipGroup)
    REGISTER_LUA_FUNC(KPlayer, DelFellowshipGroup)
    REGISTER_LUA_FUNC(KPlayer, RenameFellowshipGroup)

    REGISTER_LUA_FUNC(KPlayer, SetFellowshipRemark)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, AddFellowshipAttraction)
    REGISTER_LUA_FUNC(KPlayer, GetFellowshipAttraction)
    REGISTER_LUA_FUNC(KPlayer, GetFellowshipAttractionLevel)
    REGISTER_LUA_FUNC(KPlayer, IsFoe)
#endif

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KPlayer, SetFellowshipGroup)

    REGISTER_LUA_FUNC(KPlayer, UpdateFellowshipInfo)
    REGISTER_LUA_FUNC(KPlayer, UpdateFoeInfo)
    REGISTER_LUA_FUNC(KPlayer, UpdateBlackListInfo)

    REGISTER_LUA_FUNC(KPlayer, GetFellowshipGroupName)
    REGISTER_LUA_FUNC(KPlayer, GetFellowshipData)
    REGISTER_LUA_FUNC(KPlayer, GetFellowshipInfo)
    REGISTER_LUA_FUNC(KPlayer, GetFoeInfo)
    REGISTER_LUA_FUNC(KPlayer, GetBlackListInfo)
    REGISTER_LUA_FUNC(KPlayer, GetFellowshipGroupInfo)
    REGISTER_LUA_FUNC(KPlayer, GetFellowshipNameList)
    REGISTER_LUA_FUNC(KPlayer, IsExistFellowshipList)
#endif
    //-----------------------------------------

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, GetPartyMemberList)

    REGISTER_LUA_FUNC(KPlayer, SendSystemMessage)
    REGISTER_LUA_FUNC(KPlayer, GmSendMessage)
#endif
    //-----------------------------------------
#if defined(_SERVER)
    REGISTER_LUA_FUNC(KPlayer, AbradeEquipmentOnDeath)
#endif

#if defined(_CLIENT)
    REGISTER_LUA_FUNC(KPlayer, TradingInviteRequest)
    REGISTER_LUA_FUNC(KPlayer, TradingInviteRespond)
    REGISTER_LUA_FUNC(KPlayer, TradingAddItem)
    REGISTER_LUA_FUNC(KPlayer, TradingDeleteItem)
    REGISTER_LUA_FUNC(KPlayer, TradingSetMoney)
    REGISTER_LUA_FUNC(KPlayer, TradingConfirm)
    REGISTER_LUA_FUNC(KPlayer, GetTradingItem)
    REGISTER_LUA_FUNC(KPlayer, GetTradingMoney)
#endif

#ifdef _SERVER
	REGISTER_LUA_FUNC(KPlayer, GetSelectCharacter)
    REGISTER_LUA_FUNC(KPlayer, GetSelectDoodad)
	REGISTER_LUA_FUNC(KPlayer, AddMapMark)
	REGISTER_LUA_FUNC(KPlayer, Revive)
	REGISTER_LUA_FUNC(KPlayer, PlaySound)
	REGISTER_LUA_FUNC(KPlayer, PlayMiniGame)
	REGISTER_LUA_FUNC(KPlayer, SendMessageNotify)
#endif

	REGISTER_LUA_FUNC(KPlayer, SyncMidMapMark)

#ifdef _CLIENT
	REGISTER_LUA_FUNC(KPlayer, SetMinimapRadar)
	REGISTER_LUA_FUNC(KPlayer, GetMiniMapRadar)
#endif

	REGISTER_LUA_FUNC(KPlayer, CanApplyDuel)
	REGISTER_LUA_FUNC(KPlayer, CanApplySlay)
	
	REGISTER_LUA_FUNC(KPlayer, ApplyDuel)
	REGISTER_LUA_FUNC(KPlayer, RefuseDuel)
	REGISTER_LUA_FUNC(KPlayer, AcceptDuel)
	REGISTER_LUA_FUNC(KPlayer, LossDuel)
	REGISTER_LUA_FUNC(KPlayer, ApplySlay)
    REGISTER_LUA_FUNC(KPlayer, CanCloseSlay)
	REGISTER_LUA_FUNC(KPlayer, CloseSlay)

	REGISTER_LUA_FUNC(KPlayer, GetPKState)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, RideHorse)
    REGISTER_LUA_FUNC(KPlayer, DownHorse)
    REGISTER_LUA_FUNC(KPlayer, AddEnchant)
    REGISTER_LUA_FUNC(KPlayer, RemoveEnchant)
	REGISTER_LUA_FUNC(KPlayer, GetEnchantDestItemSubType)
    REGISTER_LUA_FUNC(KPlayer, SetEquipColor)
    REGISTER_LUA_FUNC(KPlayer, OpenRouteNode)
#endif
    REGISTER_LUA_FUNC(KPlayer, IsOpenRouteNode)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, AddTrain)
    REGISTER_LUA_FUNC(KPlayer, AddTrainNoLimit)
#endif

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KPlayer, OpenVenation)
#endif

#ifdef _SERVER
    REGISTER_LUA_FUNC(KCharacter, Attach)
    REGISTER_LUA_FUNC(KCharacter, Detach)
    REGISTER_LUA_FUNC(KCharacter, DetachAll)
#endif

    REGISTER_LUA_FUNC(KPlayer, SetTargetAStar)

    REGISTER_LUA_FUNC(KPlayer, GetAroundPlayerID)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KCharacter, SetCustomInteger4)
    REGISTER_LUA_FUNC(KCharacter, GetCustomInteger4)
    REGISTER_LUA_FUNC(KCharacter, SetCustomInteger2)
    REGISTER_LUA_FUNC(KCharacter, GetCustomInteger2)
    REGISTER_LUA_FUNC(KCharacter, SetCustomInteger1)
    REGISTER_LUA_FUNC(KCharacter, GetCustomInteger1)
    REGISTER_LUA_FUNC(KCharacter, SetCustomUnsigned4)
    REGISTER_LUA_FUNC(KCharacter, GetCustomUnsigned4)
    REGISTER_LUA_FUNC(KCharacter, SetCustomUnsigned2)
    REGISTER_LUA_FUNC(KCharacter, GetCustomUnsigned2)
    REGISTER_LUA_FUNC(KCharacter, SetCustomUnsigned1)
    REGISTER_LUA_FUNC(KCharacter, GetCustomUnsigned1)
    REGISTER_LUA_FUNC(KCharacter, SetCustomBoolean)
    REGISTER_LUA_FUNC(KCharacter, GetCustomBoolean)
    REGISTER_LUA_FUNC(KCharacter, SetCustomBitValue)
    REGISTER_LUA_FUNC(KCharacter, GetCustomBitValue)
    REGISTER_LUA_FUNC(KCharacter, SetCustomString)
    REGISTER_LUA_FUNC(KCharacter, GetCustomString)
#endif

    REGISTER_LUA_FUNC(KCharacter, GetSkillPrepareState)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KCharacter, TraceSelectTargetThreat)
#endif
    
    REGISTER_LUA_FUNC(KCharacter, SetAIOriginPos)
    REGISTER_LUA_FUNC(KCharacter, GetAIOriginPos)

    REGISTER_LUA_FUNC(KPlayer, IsOnSlay)

    REGISTER_LUA_FUNC(KPlayer, SetMapVisitFlag)
    REGISTER_LUA_FUNC(KPlayer, GetMapVisitFlag)

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KPlayer, GetReviveLeftFrame)
    REGISTER_LUA_FUNC(KPlayer, IsPartyMemberInSameScene)
	REGISTER_LUA_FUNC(KPlayer, Open)
    REGISTER_LUA_FUNC(KPlayer, GetQuestDiffcultyLevel)
#endif

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, Kickout)
    REGISTER_LUA_FUNC(KPlayer, Ban)
    REGISTER_LUA_FUNC(KPlayer, GetLastKillPointReduceTime)
    REGISTER_LUA_FUNC(KPlayer, SetLastKillPointReduceTime)
    REGISTER_LUA_FUNC(KPlayer, SetCurrentKillPoint)
    REGISTER_LUA_FUNC(KPlayer, SetCannotDialogWithNPC)
    REGISTER_LUA_FUNC(KPlayer, SetRedName)
    REGISTER_LUA_FUNC(KPlayer, SetOnlyReviveInSitu)
    REGISTER_LUA_FUNC(KPlayer, AddPrestige)

    REGISTER_LUA_FUNC(KPlayer, AddPQ)
    REGISTER_LUA_FUNC(KPlayer, DelPQ)
    REGISTER_LUA_FUNC(KPlayer, GetPQIDTable)

    REGISTER_LUA_FUNC(KPlayer, GetHeroMapCopyOpenFlag)
    REGISTER_LUA_FUNC(KPlayer, SetHeroMapCopyOpenFlag)
#endif

    REGISTER_LUA_FUNC(KPlayer, SetCamp)

#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, GetReviveCtrl)
    REGISTER_LUA_FUNC(KPlayer, SetReviveCtrl)
    REGISTER_LUA_FUNC(KPlayer, SetMaxLevel)
#endif

#ifdef _CLIENT
    REGISTER_LUA_FUNC(KPlayer, HideHat)
#endif

    REGISTER_LUA_FUNC(KPlayer, IsAchievementAcquired)
#ifdef _SERVER
    REGISTER_LUA_FUNC(KPlayer, AcquireAchievement)
#endif
    REGISTER_LUA_FUNC(KPlayer, GetAchievementPoint)
    REGISTER_LUA_FUNC(KPlayer, GetAchievementRecord)
DEFINE_LUA_CLASS_END(KPlayer)

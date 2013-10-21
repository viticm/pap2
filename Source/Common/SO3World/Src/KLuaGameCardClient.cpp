#include "stdafx.h"
#include "KGameCardClient.h"
#include "KPlayerClient.h"

#ifdef _CLIENT

int KGameCardClient::LuaSell(Lua_State* L)
{
    int     nTopIndex   = 0;
    DWORD   dwNpcID     = ERROR_ID;
    int     nTypeIndex  = 0;
    int     nPrice   = 0;
    int     nLeftHour   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 4);

    dwNpcID     = (DWORD)Lua_ValueToNumber(L, 1);
    nTypeIndex  = (int)Lua_ValueToNumber(L, 2);
    nPrice      = (int)Lua_ValueToNumber(L, 3);
    nLeftHour   = (int)Lua_ValueToNumber(L, 4);

    KGLOG_PROCESS_ERROR(nLeftHour == 12 || nLeftHour == 24 || nLeftHour == 48);

    g_PlayerClient.DoGameCardSellRequest(dwNpcID, nTypeIndex, nPrice, nLeftHour * 3600);

Exit0:
    return 0;
}

int KGameCardClient::LuaBuy(Lua_State* L)
{
    int     nTopIndex   = 0;
    DWORD   dwNpcID     = ERROR_ID;
    DWORD   dwID        = 0;
    int     nBuyPrice   = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 3);

    dwNpcID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwID        = (DWORD)Lua_ValueToNumber(L, 2);
    nBuyPrice   = (int)Lua_ValueToNumber(L, 3);

    g_PlayerClient.DoGameCardBuyRequest(dwNpcID, dwID, nBuyPrice);

Exit0:
    return 0;
}

int KGameCardClient::LuaApplyLookup(Lua_State* L)
{
    int     nTopIndex   = 0;
    DWORD   dwNpcID     = ERROR_ID;
    BYTE    byCardType  = 0;
    int     nStartIndex = 0;
    BYTE    byOrderType = 0;
    BOOL    bDesc       = false;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 5);

    dwNpcID     = (DWORD)Lua_ValueToNumber(L, 1);
    byCardType  = (BYTE)Lua_ValueToNumber(L, 2);
    nStartIndex = (int)Lua_ValueToNumber(L, 3);
    byOrderType = (BYTE)Lua_ValueToNumber(L, 4);
    bDesc       = (BOOL)Lua_ValueToBoolean(L, 5);

    g_PlayerClient.DoGameCardLookupRequest(dwNpcID, byCardType, nStartIndex, byOrderType, bDesc);

Exit0:
    return 0;
}

int KGameCardClient::LuaGetLookupResult(Lua_State* L)
{
    int                 nResult     = 0;
    int                 nTopIndex   = 0;
    int                 nIndex      = 0;
    int                 nCount      = 0;
    int                 nID         = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 0);

    Lua_PushNumber(L, m_nTotalCount);

    Lua_NewTable(L);

    for (KGAME_CARD_VECTOR::iterator it = m_CardList.begin(); it != m_CardList.end(); ++it)
    {
        Lua_PushNumber(L, ++nCount);
        Lua_NewTable(L);

        nID = (int)it->dwID;
        Lua_PushString(L, "ID");
        Lua_PushNumber(L, nID);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "SellerName");
        Lua_PushString(L, it->szSellerName);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "GameTime");
        Lua_PushNumber(L, it->nGameTime);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "Type");
        Lua_PushNumber(L, (int)it->byType);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "LeftTime");
        Lua_PushNumber(L, it->nLeftTime);
        Lua_SetTable(L, -3);

        Lua_PushString(L, "Price");
        Lua_PushNumber(L, it->nPrice);
        Lua_SetTable(L, -3);

        Lua_SetTable(L, -3);
    }

    nResult = 2;
Exit0:
    return nResult;
}

int KGameCardClient::LuaCancel(Lua_State* L)
{
    int     nTopIndex   = 0;
    DWORD   dwNpcID     = ERROR_ID;
    DWORD   dwID        = 0;

    nTopIndex = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTopIndex == 2);

    dwNpcID     = (DWORD)Lua_ValueToNumber(L, 1);
    dwID        = (DWORD)Lua_ValueToNumber(L, 2);

    g_PlayerClient.DoGameCardCancelRequest(dwNpcID, dwID);

Exit0:
    return 0;
}

DEFINE_LUA_CLASS_BEGIN(KGameCardClient)

    REGISTER_LUA_FUNC(KGameCardClient, Sell)
    REGISTER_LUA_FUNC(KGameCardClient, Buy)
    REGISTER_LUA_FUNC(KGameCardClient, ApplyLookup)
    REGISTER_LUA_FUNC(KGameCardClient, GetLookupResult)
    REGISTER_LUA_FUNC(KGameCardClient, Cancel)

DEFINE_LUA_CLASS_END(KGameCardClient)

#endif // _CLIENT

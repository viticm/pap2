#ifndef _KGAME_CARD_CLIENT_H_
#define _KGAME_CARD_CLIENT_H_

#ifdef _CLIENT

#include "KGameCardDef.h"
#include "Luna.h"

typedef std::vector<KGAME_CARD> KGAME_CARD_VECTOR;

class KGameCardClient
{
public:
    KGameCardClient();
    ~KGameCardClient();

    BOOL UpdateList(int nTotalCount, int nCount, KGAME_CARD* pList);
    BOOL Clear();

    int LuaSell(Lua_State* L);
    int LuaBuy(Lua_State* L);
    int LuaApplyLookup(Lua_State* L);
    int LuaGetLookupResult(Lua_State* L);
    int LuaCancel(Lua_State* L);

private:
    KGAME_CARD_VECTOR   m_CardList;
    int                 m_nTotalCount;

public:
    DECLARE_LUA_CLASS(KGameCardClient);
};

#endif // _CLIENT

#endif // _KGAME_CARD_CLIENT_H_

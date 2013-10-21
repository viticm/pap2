#include "stdafx.h"
#include "KTarget.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"
#include "KItem.h"

int KTarget::LuaGetTargetType(Lua_State* L)
{
    int         nResult = 0;
    TARGET_TYPE eType   = ttInvalid;

    eType = GetTargetType();

    Lua_PushNumber(L, eType);

    nResult = 1;
Exit0:
    return nResult;
}

int KTarget::LuaGetPlayer(Lua_State* L)
{
    int         nResult     = 0;
    BOOL        bRetCode    = false;
    TARGET_TYPE eType       = ttInvalid;
    KPlayer*    pPlayer     = NULL;

    eType = GetTargetType();
    KGLOG_PROCESS_ERROR(eType == ttPlayer);
    
    bRetCode = GetTarget(&pPlayer);
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = pPlayer->LuaGetObj(L);
Exit0:
    return nResult;
}

int KTarget::LuaGetNpc(Lua_State* L)
{
    int         nResult     = 0;
    BOOL        bRetCode    = false;
    TARGET_TYPE eType       = ttInvalid;
    KNpc*       pNpc        = NULL;

    eType = GetTargetType();
    KGLOG_PROCESS_ERROR(eType == ttNpc);
    
    bRetCode = GetTarget(&pNpc);
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = pNpc->LuaGetObj(L);
Exit0:
    return nResult;
}

int KTarget::LuaGetDoodad(Lua_State* L)
{
    int         nResult     = 0;
    BOOL        bRetCode    = false;
    TARGET_TYPE eType       = ttInvalid;
    KDoodad*    pDoodad     = NULL;

    eType = GetTargetType();
    KGLOG_PROCESS_ERROR(eType == ttDoodad);
    
    bRetCode = GetTarget(&pDoodad);
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = pDoodad->LuaGetObj(L);
Exit0:
    return nResult;
}

int KTarget::LuaGetItem(Lua_State* L)
{
    int         nResult     = 0;
    BOOL        bRetCode    = false;
    TARGET_TYPE eType       = ttInvalid;
    KPlayer*    pPlayer     = NULL;
    KItem*      pItem       = NULL;

    eType = GetTargetType();
    KGLOG_PROCESS_ERROR(eType == ttItemPos);
    
    bRetCode = GetTarget(&pPlayer, &pItem);
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = pItem->LuaGetObj(L);
Exit0:
    return nResult;
}

int KTarget::LuaGetCoordinates(Lua_State* L)
{
    int         nResult     = 0;
    BOOL        bRetCode    = false;
    TARGET_TYPE eType       = ttInvalid;
    int         nX          = 0;
    int         nY          = 0;
    int         nZ          = 0;

    eType = GetTargetType();
    KGLOG_PROCESS_ERROR(eType == ttCoordination);
    
    bRetCode = GetTarget(&nX, &nY, &nZ);
    KGLOG_PROCESS_ERROR(bRetCode);

    Lua_PushNumber(L, nX);
    Lua_PushNumber(L, nY);
    Lua_PushNumber(L, nZ);
    
    nResult = 3;
Exit0:
    return nResult;
}

DEFINE_LUA_CLASS_BEGIN(KTarget)
    REGISTER_LUA_FUNC(KTarget, GetTargetType)
    REGISTER_LUA_FUNC(KTarget, GetPlayer)
    REGISTER_LUA_FUNC(KTarget, GetNpc)
    REGISTER_LUA_FUNC(KTarget, GetDoodad)
    REGISTER_LUA_FUNC(KTarget, GetItem)
    REGISTER_LUA_FUNC(KTarget, GetCoordinates)
DEFINE_LUA_CLASS_END(KTarget)

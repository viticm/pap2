/************************************************************************/
/* Copyright : Kingsoft 2007										    */
/* Author	 : Zhao Chunfeng										    */
/* History	 :															*/
/*		2007.10.12	Create												*/
/*      拦截Lua对一些无效对象的引用                                     */
/************************************************************************/

#include "stdafx.h"
#include "Luna.h"
#include "KScriptFuncList.h"
#include "Global.h"
#include "KRegion.h"
#include "KScene.h"
#include "KSO3World.h"
#include "KRecipe.h"
#include "KProfession.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"

template <>
int Dispatcher<KPlayer>(lua_State* L)
{
    Luna<KPlayer>::KObjData* pObjData = (Luna<KPlayer>::KObjData*)lua_touserdata(L, lua_upvalueindex(1));
    Luna<KPlayer>::KLuaData* pLuaData = (Luna<KPlayer>::KLuaData*)lua_touserdata(L, lua_upvalueindex(2));
    KPlayer* pPlayer = pObjData->pObj;

    if (pPlayer->m_pScene == NULL)
    {
        KGLogPrintf(KGLOG_WARNING, "[Lua] index to a invalid player !\n");
        return 0;
    }
	
    return (pPlayer->*(Luna<KPlayer>::LuaFunc)(pLuaData->pGetFunc))(L);
}

template <>
int Dispatcher<KNpc>(lua_State* L)
{
    Luna<KNpc>::KObjData* pObjData = (Luna<KNpc>::KObjData*)lua_touserdata(L, lua_upvalueindex(1));
    Luna<KNpc>::KLuaData* pLuaData = (Luna<KNpc>::KLuaData*)lua_touserdata(L, lua_upvalueindex(2));
    KNpc* pNpc = pObjData->pObj;

    if (pNpc->m_pScene == NULL)
    {
        KGLogPrintf(KGLOG_WARNING, "[Lua] index to a invalid npc !\n");
        return 0;
    }
	
    return (pNpc->*(Luna<KNpc>::LuaFunc)(pLuaData->pGetFunc))(L);
}

template <>
int Dispatcher<KDoodad>(lua_State* L)
{
    Luna<KDoodad>::KObjData* pObjData = (Luna<KDoodad>::KObjData*)lua_touserdata(L, lua_upvalueindex(1));
    Luna<KDoodad>::KLuaData* pLuaData = (Luna<KDoodad>::KLuaData*)lua_touserdata(L, lua_upvalueindex(2));
    KDoodad* pDoodad = pObjData->pObj;

    if (pDoodad->m_pScene == NULL)
    {
        KGLogPrintf(KGLOG_WARNING, "[Lua] index to a invalid doodad !\n");
        return 0;
    }
	
    return (pDoodad->*(Luna<KDoodad>::LuaFunc)(pLuaData->pGetFunc))(L);
}

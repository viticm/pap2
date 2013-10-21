#include "stdafx.h"
#include "KSceneObject.h"
#include "KScene.h"

int KSceneObject::LuaGetScene(Lua_State* L)
{
	if (m_pScene)
    {
	    m_pScene->LuaGetObj(L);
        return 1;
    }

	return 0;
}

int KSceneObject::LuaGetCell(Lua_State* L)
{
	if (m_pCell)
    {
	    m_pCell->LuaGetObj(L);
        return 1;
    }

	return 0;
}

int KSceneObject::LuaGetAbsoluteCoordinate(Lua_State* L)
{
    int nResult = 0;

    Lua_PushNumber(L, m_nX);
    Lua_PushNumber(L, m_nY);
    Lua_PushNumber(L, m_nZ);

    nResult = 3;
Exit0:
    return nResult;
}

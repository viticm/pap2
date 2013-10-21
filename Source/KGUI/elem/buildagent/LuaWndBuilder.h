#ifdef KGUI_LUA_LOADER
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : WndBuilderForLua.h
//  Version     : 1.0
//  Creator     : tongxuehu
//  Create Date : 11/29/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_ELEM_BUILD_AGENT_K_LUA_WND_BUILDER_H
#define UI_ELEM_BUILD_AGENT_K_LUA_WND_BUILDER_H

namespace UI 
{

    class IKUIObject;
    class KWndFrame;

    typedef int (* KUIObjCreator)(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);

    typedef std::map<std::string, KUIObjCreator> KOBJ_MAP;

    class KLuaWndBuilder
    {   
    public:
        KLuaWndBuilder();
        virtual ~KLuaWndBuilder();

    public:
        int OpenFrame(LPCSTR pcszFrame, LPCSTR pcszName, KWndFrame **ppRetFrame);
        int CreateFrame(Lua_State *pLua, int nTableIndex, IKUIObject **ppRetObj);

    private:
        int CreateObject(Lua_State *pLua, int nTableIndex, IKUIObject **ppRetObj);

    private:
        KOBJ_MAP m_ObjCreatorMap;
    };
}

#endif // UI_ELEM_BUILD_AGENT_K_LUA_WND_BUILDER_H

#endif
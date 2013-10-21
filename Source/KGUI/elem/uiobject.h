#ifdef KGUI_LUA_LOADER
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : uiobject.h
//  Version     : 1.0
//  Creator     : tongxuehu
//  Create Date : 12/04/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef UI_ELEM_UI_OBJECT_H
#define UI_ELEM_UI_OBJECT_H

namespace UI
{

#define KD_SMALL_BUFFER_SIZE  64  
#define KD_LESS_BUFFER_SIZE   128
#define KD_NORMAL_BUFFER_SIZE 512

    enum KEM_OBJ_TYPE {
        em_OBJ_TYPE_WND,
        em_OBJ_TYPE_FRAME,
        em_OBJ_TYPE_ITEM,
        em_OBJ_TYPE_HANDLE,
    };

    class IKUIObject
    {
    public:
        static  int  Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);
        virtual void Release() = 0;

        virtual int  Init(Lua_State *pLua, int nTableIndex) = 0;
        virtual int  AddChild(IKUIObject *pChild) = 0;

    public:
        KEM_OBJ_TYPE m_emObjType;
    };

} // namespace UI

#endif // UI_ELEM_UI_OBJECT_H

#endif
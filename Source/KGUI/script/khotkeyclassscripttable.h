////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KHotkeyClassScriptTable.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-6-23 9:03:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KHotkeyClassScriptTable_H_
#define _INCLUDE_KHotkeyClassScriptTable_H_


namespace UI
{

	class KHotkeyClassScriptTable
	{
	public:
		static void Load(lua_State* L);

    private:
		static int Lua_Load(Lua_State *L);
		static int Lua_LoadAsAdd(Lua_State *L);
		static int Lua_LoadDefault(Lua_State *L);
		static int Lua_LoadFromLuaAsAdd(Lua_State *L);
		static int Lua_GetBinding(Lua_State *L);
		static int Lua_Save(Lua_State *L);
		static int Lua_SaveAsAdd(Lua_State *L);
		static int Lua_SaveToLuaAsAdd(Lua_State *L);

		static int Lua_Clear(Lua_State *L);

		static int Lua_Set(Lua_State *L);
		static int Lua_Get(Lua_State *L);
		static int Lua_IsUsed(Lua_State *L);

		static int Lua_IsKeyDown(Lua_State *L);

		static int Lua_SetCapture(Lua_State *L);
		static int Lua_AddBinding(Lua_State *L);

		static int Lua_Enable(Lua_State *L);
		static int Lua_IsEnable(Lua_State *L);

		static int Lua_GetCaptureKey(Lua_State *L);
	};

}


#endif //_INCLUDE_KHotkeyClassScriptTable_H_

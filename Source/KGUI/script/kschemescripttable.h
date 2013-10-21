#ifndef UI_SCRIPT_K_SCHEME_SCRIPT_TABLE_H
#define UI_SCRIPT_K_SCHEME_SCRIPT_TABLE_H


namespace UI
{

	class KSchemeScriptTable
	{
	public:
		static void Load(lua_State* L);

	private:
		static int LuaFont_SetFontOffset(Lua_State* L);
		static int LuaFont_GetFontOffset(Lua_State* L);

		static int LuaFont_GetChatFontID(Lua_State* L);
		static int LuaFont_LoadDefaultFontList(Lua_State* L);
		static int LuaFont_LoadFontList(Lua_State* L);
		static int LuaFont_SetFont(Lua_State* L);
		static int LuaFont_GetFont(Lua_State* L);

		static int LuaGetTextExtent(Lua_State *L);
		static int LuaEncodeComponentsString(Lua_State *L);

		static int LuaGetPureText(Lua_State *L);
	private:
		static KLuaFunc m_aScriptFuncTable[];

		static KSchemeScriptTable* ms_pSelf;
	};

}

#endif // UI_SCRIPT_K_SCHEME_SCRIPT_TABLE_H


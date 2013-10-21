#ifndef UI_SCRIPT_K_FILE_CLASS_SCRIPT_TABLE_H
#define UI_SCRIPT_K_FILE_CLASS_SCRIPT_TABLE_H


namespace UI
{
    struct KG_Table;

	class KFileClassScriptTable
	{
	public:
		static void Load(lua_State* L);

	private:
		static bool NewUserData(lua_State* L, IIniFile* pUserData);
		static bool NewUserData(lua_State* L, ITabFile* pUserData);


		static int LuaIni_Create(Lua_State* L);
		static int LuaIni_Open(Lua_State* L);
		static int LuaIni_Save(Lua_State* L);
		static int LuaIni_Close(Lua_State* L);
		static int LuaIni_Clear(Lua_State* L);
		static int LuaIni_IsSectionExist(Lua_State* L);
		static int LuaIni_GetNextSection(Lua_State* L);
		static int LuaIni_GetNextKey(Lua_State* L);
		static int LuaIni_GetSectionCount(Lua_State* L);
		static int LuaIni_EraseSection(Lua_State* L);
		static int LuaIni_EraseKey(Lua_State* L);
		static int LuaIni_ReadString(Lua_State* L);
		static int LuaIni_ReadInteger(Lua_State* L);
		static int LuaIni_ReadMultiInteger(Lua_State* L);
		static int LuaIni_ReadMultiLong(Lua_State* L);
		static int LuaIni_ReadFloat(Lua_State* L);
		static int LuaIni_ReadMultiFloat(Lua_State* L);
		static int LuaIni_WriteString(Lua_State* L);
		static int LuaIni_WriteInteger(Lua_State* L);
		static int LuaIni_WriteMultiInteger(Lua_State* L);
		static int LuaIni_WriteMultiLong(Lua_State* L);
		static int LuaIni_WriteFloat(Lua_State* L);
		static int LuaIni_WriteMultiFloat(Lua_State* L);

		static int LuaTab_Create(Lua_State* L);
		static int LuaTab_Open(Lua_State* L);
		static int LuaTab_Close(Lua_State* L);
		static int LuaTab_FindRow(Lua_State* L);
		static int LuaTab_FindColumn(Lua_State* L);
		static int LuaTab_GetWidth(Lua_State* L);
		static int LuaTab_GetHeight(Lua_State* L);
		static int LuaTab_GetString(Lua_State* L);
		static int LuaTab_GetInteger(Lua_State* L);
		static int LuaTab_GetFloat(Lua_State* L);

        static int LuaTable_Load(lua_State *pLua);
        static int LuaTable_GetRow(lua_State *pLua);
        static int LuaTable_Release(lua_State *pLua);
        static int LuaTable_Search(lua_State *pLua);
        static int LuaTable_GetRowCount(lua_State *pLua);
        static int LuaTable_PushRow(lua_State *pLua, KG_Table *pTable, BYTE *pbyRow);
	};

}

#endif // UI_SCRIPT_K_FILE_CLASS_SCRIPT_TABLE_H


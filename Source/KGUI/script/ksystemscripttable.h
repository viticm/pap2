#ifndef UI_SCRIPT_K_SYSTEM_SCRIPT_TABLE_H
#define UI_SCRIPT_K_SYSTEM_SCRIPT_TABLE_H


namespace UI
{
	class KSystemScriptTable
	{
	public:
		static void Load(lua_State* L);

	private:
        static int LuaFormatTime(Lua_State* L);
		static int LuaGetTime(Lua_State* L);
		static int LuaGetTickCount(Lua_State* L);
		static int LuaGetFPS(Lua_State* L);
		static int LuaGetVideoMemeryInfo(Lua_State* L);
		static int LuaGetUITextureUsage(Lua_State* L);

		static int LuaOpenURL(Lua_State* L);

		static int LuaRemoveUserdata(Lua_State* L);
		static int LuaSetUserAccount(Lua_State* L);
		static int LuaSetUserServer(Lua_State* L);
		static int LuaSetUserRoleName(Lua_State* L);
		static int LuaGetUserAccount(Lua_State* L);
		static int LuaGetUserServer(Lua_State* L);
		static int LuaGetUserRoleName(Lua_State* L);
		static int LuaGetUserDataFloder(Lua_State* L);
		static int LuaGetUserDataPath(Lua_State* L);
		static int LuaSaveUserData(Lua_State* L);
		static int LuaLoadUserData(Lua_State* L);
		static int LuaIsUserDataExist(Lua_State* L);

		static int LuaLoadDataFromFile(Lua_State* L);
		static int LuaSaveDataToFile(Lua_State* L);

		static int LuaMovieRecord(Lua_State* L);
		static int LuaMovieStop(Lua_State* L);
		static int LuaIsMovieRecord(Lua_State* L);

        static int LuaCanRecord(Lua_State* L);
        static int LuaIsRecording(Lua_State* L);
        static int LuaStartRecord(Lua_State* L);
        static int LuaStopRecord(Lua_State* L);

		static int LuaGetAllowableAdapterModes(Lua_State* L);
		static int LuaGetCurrentAdapterMode(Lua_State* L);

		static int LuaEatRAM(Lua_State* L);
		static int LuaEatVRAM(Lua_State* L);

		static int LuaIsMouseButtonSwaped(Lua_State* L);
		static int LuaSwapMouseButton(Lua_State* L);

		static int LuaGetOpenFileName(Lua_State* L);
		static int LuaGetUnpakFileSize(Lua_State* L);
		static int LuaIsUnpakFileExist(Lua_State* L);

        static int LuaAllocConsole(Lua_State* L);
        static int LuaFreeConsole(Lua_State* L);

        static int LuaClearGlobalString(lua_State *pLua);
        static int LuaRegisterGlobalString(lua_State *pLua);
	};
}

#endif // UI_SCRIPT_K_SYSTEM_SCRIPT_TABLE_H


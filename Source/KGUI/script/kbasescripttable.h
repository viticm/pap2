#ifndef UI_SCRIPT_K_BASE_SCRIPT_TABLE_H
#define UI_SCRIPT_K_BASE_SCRIPT_TABLE_H

namespace UI
{
	class KBaseScriptTable
	{
	public:
		static void Load(lua_State* L);

		static bool HostDebug(LPCSTR szMode);
		static bool HostGetDebugModeState(LPCSTR szMode, DWORD& dwState);

		static int Host_ScreenShot(LPCSTR szFilePath, int nQuality);
		static int Host_ScreenShotBySuffix(LPCSTR szFix, int nQuality);

	private:
		static int LuaRegisterString(Lua_State* L);
		static int LuaGetString(Lua_State *L);

		static int LuaDebug(Lua_State* L);
		static int LuaGetDebugModeState(Lua_State* L);

        static int Lua_Trace(Lua_State* L);
        static int Lua_Break(Lua_State* L);
        static int Lua_Log(Lua_State* L);
		static int Lua_CheckMemory(Lua_State* L);

		static int Lua_ScreenShot(Lua_State* L);

        static int Lua_LoadImage(Lua_State* L);
        static int Lua_UnloadImage(Lua_State* L);
        static int Lua_MaskImage(Lua_State* L);

		static int Lua_NumberToChinese(Lua_State* L);

		static int Lua_LoadDefaultScriptLib(Lua_State* L);
		static int Lua_LoadLoginScriptLib(Lua_State* L);
		static int Lua_LoadScriptFile(Lua_State* L);
		static int Lua_LoadScriptLib(Lua_State* L);

		static int Lua_InitAddOn(Lua_State* L);
		static int Lua_GetAddOnCount(Lua_State* L);
		static int Lua_GetAddOnInfo(Lua_State* L);
		static int Lua_EnableAddOn(Lua_State* L);
		static int Lua_DisableAddOn(Lua_State* L);
		static int Lua_LoadAddOn(Lua_State* L);
		static int Lua_GetAddOnIndexByID(Lua_State* L);
		static int Lua_EnableOverdueAddOn(Lua_State* L);
		static int Lua_IsEnableOverdueAddOn(Lua_State* L);
		static int Lua_SetCurrentAddOnVersion(Lua_State* L);


		static int Lua_MySetGlobal(Lua_State* L);
		static int Lua_MyGetGlobal(Lua_State* L);

		static int Lua_GetRootPath(Lua_State* L);

		static int LuaExitGame(Lua_State* L);

		static int LuaReInitUI(Lua_State* L);

        static int LuaIsValidRoleName(Lua_State* L);

		static int LuaRegisterEvent(Lua_State* L);
		static int LuaUnRegisterEvent(Lua_State* L);
		static int LuaFireEvent(Lua_State* L);

        static int LuaIsActivated(Lua_State* L);

		static int LuaGetLoadLoginReason(Lua_State* L);

		static int LuaGetAutoTipInfo(Lua_State* L);

		static int LuaOpenUIEditor(Lua_State* L);
		static int LuaCloseOpenUIEditor(Lua_State* L);

		static int LuaGetVideoSettings(Lua_State* L);
		static int LuaSetVideoSettings(Lua_State* L);

        static int LuaIsGB2312String(Lua_State* L);
        static int LuaIsGBKString(Lua_State* L);
        static int LuaIsSimpleChineseString(Lua_State* L);

        static int LuaIsDebug(Lua_State* L);

        static int LuaGetUserEnv(Lua_State* L);

		static int LuaGetLastError(Lua_State* L);

		static int LuaIsFileExist(Lua_State* L);

        static int LuaStringFindW(lua_State *pLua);
        static int LuaStringReplaceW(lua_State *pLua);
		static int LuaStringLowerW(lua_State *pLua);

        static int LuaGetNumberBit(lua_State *pLua);
        static int LuaSetNumberBit(lua_State *pLua);

		static int LuaMoneyToGoldSilverAndCopper(Lua_State* L);

        static int LuaFormatString(lua_State *pLua);

    private:
		static bool ms_bLoadLoginUI;
	};

}

#endif // UI_SCRIPT_K_BASE_SCRIPT_TABLE_H


#ifndef UI_SCRIPT_K_SCRIPT_LOADER_H
#define UI_SCRIPT_K_SCRIPT_LOADER_H

#include "engine.h"

namespace UI
{
	class KScriptLoader
	{
	public:
		static bool LoadFile(lua_State* L, LPCSTR szFilePath, LPCSTR pcwszEnv);

		static void RegisterMetaTable(lua_State* L, LPCSTR szTableName, luaL_reg const* pMetaFunc);
		static void RegisterLibTable(lua_State* L, LPCSTR szLibName, luaL_reg const* pLibFunc);
		static void RegisterFunctions(lua_State* L, KLuaFunc const aFuncs[], int nCount);
		static void RegisterFunctions(lua_State* L, luaL_reg const* pFunc);
		static void RegisterConstList(lua_State* L, KLuaConstList const* pLuaConstList);
		static int NewIndexForConstList(lua_State* L);

        static void LogStackDebugInfo(lua_State* L, LPCSTR szMsg);

	private:
		static bool LoadFromBuffer(lua_State* L, LPCSTR szScriptName, LPBYTE pBuffer, DWORD dwSize, LPCSTR pcwszEnv);
    };

}


#endif // UI_SCRIPT_K_SCRIPT_LOADER_H


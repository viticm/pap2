#include "stdafx.h"
#include "./kscriptloader.h"
#include "../common/tblock.h"
#include "../common/KG_I18N.h"
#include "SO3UI/KJX3UIDefine.h"

#define ENABLE_DEBUG_LUA_SCRIPT 0

namespace UI
{

	bool KScriptLoader::LoadFile(lua_State* L, LPCSTR szFilePath, LPCSTR pcwszEnv)
	{
        int nFileSize = 0;
#if ENABLE_DEBUG_LUA_SCRIPT
		bool nRetCode = false;

		ASSERT(L);

#if 0
		KGLogPrintf(KGLOG_ERR, "KScriptLoader::LoadFile(%s)\n", szFilePath);
#endif

		KGLOG_PROCESS_ERROR(szFilePath && szFilePath[0] != '\0');

		ASSERT(L);

		if (lua_gc(L, LUA_GCCOUNT, 0) > 2048) 
			lua_gc(L, LUA_GCCOLLECT, 0);

		if (luaL_loadfile(L, szFilePath) != 0)
			KGLogPrintf(KGLOG_ERR, "KGUI KScriptLoader::LoadFile(%.8x, %s) failed [%s]\n", L, szFilePath, lua_tostring(L, -1));

		nRetCode = lua_pcall(L, 0, 0, 0) == 0;
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "KGUI KScriptLoader::LoadFile() pcall failed [%s]\n", lua_tostring(L, -1));
			LogStackDebugInfo(L, NULL);

			lua_pop(L, 1);
		}

		return nRetCode;
Exit0:
		return false;
#else // ENABLE_DEBUG_LUA_SCRIPT
		int nRetCode = false;
		KSmartFile File;
        TBlock<BYTE> Buffer;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(szFilePath && szFilePath[0] != '\0');
#ifdef KG_PUBLISH
		if (!pcwszEnv || *pcwszEnv == '\0')
		{
			File = g_OpenFileInPak(szFilePath);
			if (!File)
				File = g_OpenFile(szFilePath);
		}
		else
			File = g_OpenFile(szFilePath);
		KGLOG_PROCESS_ERROR(File);
#else
		File = g_OpenFile(szFilePath);
		KGLOG_PROCESS_ERROR(File);
#endif
        nFileSize = (int)File->Size();
        KG_PROCESS_SUCCESS(nFileSize == 0);
        KGLOG_PROCESS_ERROR(nFileSize > 0);

        nRetCode = Buffer.Alloc(nFileSize);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = File->Read(&Buffer[0], nFileSize);
        KGLOG_PROCESS_ERROR(nRetCode == nFileSize);
        
        if (GetCodePage() == CP_UTF8)
        {
            TBlock<BYTE> UTFBuffer;
            int nPrefixLength = sizeof(FILE_PREFIX_UTF8);

            nRetCode = strncmp((const char *)&Buffer[0], FILE_PREFIX_UTF8, nPrefixLength);
            if (nFileSize >= nPrefixLength && nRetCode == 0)
            {
                nRetCode = LoadFromBuffer(L, szFilePath, &Buffer[nPrefixLength], nFileSize - nPrefixLength, pcwszEnv);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                int nRealLength = 0;
                nRetCode = UTFBuffer.Alloc(nFileSize * 2);
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = ConvertToUTF8(CODE_PAGE_GBK, (char *)&Buffer[0], nFileSize, (char *)&UTFBuffer[0], nFileSize * 2, &nRealLength);
                KGLOG_PROCESS_ERROR(nRetCode);
                ASSERT(nRealLength <= nFileSize * 2);

                nRetCode = LoadFromBuffer(L, szFilePath, &UTFBuffer[0], nRealLength, pcwszEnv);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
        }
        else
        {
            nRetCode = LoadFromBuffer(L, szFilePath, &Buffer[0], nFileSize, pcwszEnv);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

Exit1:
		return true;
Exit0:
		return false;

#endif // ENABLE_DEBUG_LUA_SCRIPT
	}


	bool KScriptLoader::LoadFromBuffer(lua_State* L, LPCSTR szScriptName, LPBYTE pBuffer, DWORD dwSize, LPCSTR pcwszEnv)
	{
		bool bRetCode = false;

		ASSERT(L);
		ASSERT(szScriptName);
		ASSERT(pBuffer);

		if (lua_gc(L, LUA_GCCOUNT, 0) > 2048) 
			lua_gc(L, LUA_GCCOLLECT, 0);

		bRetCode = luaL_loadbuffer(L, (char const*)pBuffer, dwSize, szScriptName) == 0;
		if (bRetCode)
		{
			if (pcwszEnv)
			{
				int nTop = lua_gettop(L);
				lua_pushstring(L, "_G");
				lua_rawget(L, LUA_GLOBALSINDEX);
				lua_pushstring(L, pcwszEnv);
				lua_rawget(L, -2);
				if (lua_istable(L, -1))
					lua_setfenv(L, -3);
				lua_settop(L, nTop);
			}
			bRetCode = lua_pcall(L, 0, 0, 0) == 0;
			if (!bRetCode)
			{
				KGLogPrintf(KGLOG_ERR, "KGUI KScriptLoader::LoadFromBuffer() pcall failed [%s]\n", lua_tostring(L, -1));
				LogStackDebugInfo(L, NULL);

				lua_pop(L, 1);
			}
		}
		else
		{
			KGLogPrintf(KGLOG_ERR, "KGUI KScriptLoader::LoadFromBuffer(%.8x, %s) failed [%s]\n", L, szScriptName, lua_tostring(L, -1));
		}

		return bRetCode;
	}


	void KScriptLoader::LogStackDebugInfo(lua_State* L, LPCSTR szMsg)
	{
		KGLogPrintf(KGLOG_ERR, "KGUI KScriptLoader::LogStackDebugInfo(%s)\n", szMsg);

		ASSERT(L);

		lua_Debug ar = { 0 };

		for (int nLevel = 0; lua_getstack(L, nLevel, &ar); ++nLevel)
		{
			if (lua_getinfo(L,"Snluf", &ar))
			{
				KGLogPrintf(KGLOG_ERR, "\t%d %d %d %d %d %d %s %s %s %s %s\n",
					ar.event, ar.currentline, ar.nups, ar.linedefined, ar.lastlinedefined, ar.i_ci,
					ar.name ? ar.name : "", 
					ar.namewhat ? ar.namewhat : "", 
					ar.what ? ar.what : "", 
					ar.source ? ar.source : "", 
					ar.short_src);
			}
		}
	}

	void KScriptLoader::RegisterMetaTable(lua_State* L, LPCSTR szTableName, luaL_reg const* pMetaFunc) 
	{
		ASSERT(L);
        ASSERT(szTableName);
		ASSERT(pMetaFunc);

		luaL_newmetatable(L, szTableName);  /* create new metatable */
		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);				/* push metatable */
		lua_rawset(L, -3);					/* metatable.__index = metatable */
		luaL_openlib(L, NULL, pMetaFunc, 0);
		lua_pop(L, 1);
	}

	void KScriptLoader::RegisterLibTable(lua_State *L, LPCSTR szTableName, luaL_reg const* pLibFunc)
	{
		ASSERT(L);
        ASSERT(szTableName);
        ASSERT(pLibFunc);

		luaL_openlib(L, szTableName, pLibFunc, 0);
		lua_pop(L, 1);
	}

	void KScriptLoader::RegisterFunctions(Lua_State* L, KLuaFunc const aFuncs[], int nCount)
	{
		ASSERT(L);
        ASSERT(aFuncs);

		for (int nIndex = 0; nIndex < nCount; ++nIndex)
		{
			if (aFuncs[nIndex].pszFuncName && aFuncs[nIndex].pFunc) 
			{
				Lua_Register(L, aFuncs[nIndex].pszFuncName, aFuncs[nIndex].pFunc);
			}
		}
	}

	void KScriptLoader::RegisterFunctions(Lua_State* L, luaL_reg const* pFunc)
	{
		ASSERT(L);
		ASSERT(pFunc);

		for (; pFunc->name && pFunc->func; ++pFunc)
		{
			Lua_Register(L, pFunc->name, pFunc->func);
		}
	}

	void KScriptLoader::RegisterConstList(lua_State* L, KLuaConstList const* pLuaConstList)
	{
		bool bRetCode = false;
		int nIndex = 0;
		int nOriginalIndex = 0;

		KG_PROCESS_ERROR(pLuaConstList);

		while (pLuaConstList[nIndex].pConstList && pLuaConstList[nIndex].pszListName)
		{
			KLuaConst* pConstList = pLuaConstList[nIndex].pConstList;

			nOriginalIndex = lua_gettop(L);

			lua_newtable(L);
			int nConstTable = lua_gettop(L);

			lua_newtable(L);
			int nRealTable = lua_gettop(L);

			//加入全局表中
			lua_pushstring(L, pLuaConstList[nIndex].pszListName);
			lua_pushvalue(L, nConstTable);
			lua_settable(L, LUA_GLOBALSINDEX);

			int nConstIndex = 0;
			while (pConstList[nConstIndex].pszConstName)
			{
				lua_pushstring(L, pConstList[nConstIndex].pszConstName);
				lua_pushnumber(L, pConstList[nConstIndex].nValue);
				lua_settable(L, nRealTable);

				nConstIndex++;
			}

			//设置常量表的metatable,防止常量表在脚本中被修改
			lua_newtable(L);
			int nMetaTable = lua_gettop(L);

			lua_pushstring(L, "__index");
			lua_pushvalue(L, nRealTable);
			lua_settable(L, nMetaTable);

			lua_pushstring(L, "__newindex");
			lua_pushcfunction(L, KScriptLoader::NewIndexForConstList);
			lua_settable(L, nMetaTable);

			lua_setmetatable(L, nConstTable);

			lua_settop(L, nOriginalIndex);

			nIndex++;
		}

Exit0:
		return;
	}

	int KScriptLoader::NewIndexForConstList(lua_State* L)
	{
		const char* pszKey = NULL;

		pszKey = lua_tostring(L, 2);
		KG_PROCESS_ERROR(pszKey);

		KGLogPrintf(KGLOG_ERR, "KGUI [LUA] Cannot change a constant value %s.\n", pszKey);
		return 0;

Exit0:
		KGLogPrintf(KGLOG_ERR, "[KGUI LUA] Nil key value.\n");
		return 0;
	}

}


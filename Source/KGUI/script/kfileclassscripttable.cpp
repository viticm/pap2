#include "stdafx.h"
#include "./kfileclassscripttable.h"
#include "./kscriptmgr.h"
#include "./kscriptloader.h"
#include "../common/tblock.h"
#include "../KGUI.h"
#include "../common/kbintab.h"
#include "../common/IKG_TabFile.h"

static const int CELL_SIZE = 4;
static const int MAX_ROW_SIZE = 1024 * CELL_SIZE;

namespace UI
{

    typedef pair<std::string, char> KG_Pair;

    struct KG_Table
    {
        size_t uTotalColumn;
        size_t uTotalRow;
        IKG_TabFile *piTabFile;
        vector<KG_Pair > TitleVector;
    };
}

int Compare(unsigned uLValue, unsigned uRValue)
{
    if (uLValue == uRValue)
        return 0;

    if (uLValue > uRValue)
        return 1;

    return -1;
}

int Compare1Key(BYTE const *pbyLValue, BYTE const *pbyRValue)
{
    return Compare(*((unsigned *)pbyLValue), *((unsigned *)pbyRValue));
}

int Compare2Key(BYTE const *pbyLValue, BYTE const *pbyRValue)
{
    if (*((unsigned *)pbyLValue) == *((unsigned *)pbyRValue))
    {
        return Compare(*((unsigned *)(pbyLValue + CELL_SIZE)), *((unsigned *)(pbyRValue + CELL_SIZE)));
    }
    return Compare(*((unsigned *)pbyLValue), *((unsigned *)pbyRValue));
}

int Compare3Key(BYTE const *pbyLValue, BYTE const *pbyRValue)
{
    if (*((unsigned *)pbyLValue) == *((unsigned *)pbyRValue))
    {
        if (*((unsigned *)(pbyLValue + CELL_SIZE)) == *((unsigned*)(pbyRValue + CELL_SIZE)))
        {
            return Compare(*((unsigned *)(pbyLValue + 2 * CELL_SIZE)), *((unsigned *)(pbyRValue + 2 * CELL_SIZE)));
        }
        return Compare(*((unsigned *)(pbyLValue + CELL_SIZE)), *((unsigned *)(pbyRValue + CELL_SIZE)));
    }
    return Compare(*((unsigned *)pbyLValue), *((unsigned *)pbyRValue));
}

namespace UI
{
	void KFileClassScriptTable::Load(lua_State* L)
	{
		static luaL_reg const s_aIniLibTable[] =
		{
			{ "Create",								LuaIni_Create },
			{ "Open",								LuaIni_Open },

			{ NULL,									NULL },
		};

		static luaL_reg const s_aTabLibTable[] =
		{
			{ "Create",								LuaTab_Create },
			{ "Open",								LuaTab_Open },
			{ NULL,									NULL },
		};

		static luaL_reg const s_aIniMetaTable[] =
		{
			{ "Save",								LuaIni_Save },
			{ "Close",								LuaIni_Close },
			{ "Clear",								LuaIni_Clear },
			{ "IsSectionExist",						LuaIni_IsSectionExist },
			{ "GetNextSection",						LuaIni_GetNextSection },
			{ "GetNextKey",							LuaIni_GetNextKey },
			{ "GetSectionCount",					LuaIni_GetSectionCount },
			{ "EraseSection",						LuaIni_EraseSection },
			{ "EraseKey",							LuaIni_EraseKey },
			{ "ReadString",							LuaIni_ReadString },
			{ "ReadInteger",						LuaIni_ReadInteger },
			{ "ReadMultiInteger",					LuaIni_ReadMultiInteger },
			{ "ReadMultiLong",						LuaIni_ReadMultiLong },
			{ "ReadFloat",							LuaIni_ReadFloat },
			{ "ReadMultiFloat",						LuaIni_ReadMultiFloat },
			{ "WriteString",						LuaIni_WriteString },
			{ "WriteInteger",						LuaIni_WriteInteger },
			{ "WriteMultiInteger",					LuaIni_WriteMultiInteger },
			{ "WriteMultiLong",						LuaIni_WriteMultiLong },
			{ "WriteFloat",							LuaIni_WriteFloat },
			{ "WriteMultiFloat",					LuaIni_WriteMultiFloat },
			{ NULL,									NULL },
		};


		static luaL_reg const s_aTabMetaTable[] =
		{
			{ "Close",								LuaTab_Close },
			{ "FindRow",							LuaTab_FindRow },
			{ "FindColumn",							LuaTab_FindColumn },
			{ "GetWidth",							LuaTab_GetWidth },
			{ "GetHeight",							LuaTab_GetHeight },
			{ "GetString",							LuaTab_GetString },
			{ "GetInteger",							LuaTab_GetInteger },
			{ "GetFloat",							LuaTab_GetFloat },
			{ NULL,									NULL },
		};

        static luaL_reg const s_aTablelibTable[] = 
        {
            { "Load", LuaTable_Load },

            { NULL, NULL },
        };

        static luaL_reg const s_aTableMetaTable[] = 
        {
            { "GetRow",         LuaTable_GetRow },
            { "Search",         LuaTable_Search },
            { "GetRowCount",    LuaTable_GetRowCount },
            { "TableRealse",    LuaTable_Release },
            { "__gc",           LuaTable_Release },

            { NULL, NULL },
        };

        KScriptLoader::RegisterLibTable(L, "Ini", s_aIniLibTable);
		KScriptLoader::RegisterLibTable(L, "Tab", s_aTabLibTable);
        KScriptLoader::RegisterLibTable(L, "KG_Table", s_aTablelibTable);
		KScriptLoader::RegisterMetaTable(L, "IIniFile", s_aIniMetaTable);
		KScriptLoader::RegisterMetaTable(L, "ITabFile", s_aTabMetaTable);
        KScriptLoader::RegisterMetaTable(L, "TableFile", s_aTableMetaTable);
    }


	bool KFileClassScriptTable::NewUserData(lua_State* L, IIniFile* pUserData)
	{
		return TNewUserData(L, pUserData, "IIniFile");
	}

	bool KFileClassScriptTable::NewUserData(lua_State* L, ITabFile* pUserData)
	{
		return TNewUserData(L, pUserData, "ITabFile");
	}

	int KFileClassScriptTable::LuaIni_Create(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile* pIniFile = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 0;
		KGLOG_PROCESS_ERROR(bRetCode);

		pIniFile = g_CreateIniFile();
		KGLOG_PROCESS_ERROR(pIniFile);

		bRetCode = NewUserData(L, pIniFile);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 1;
Exit0:
		KG_COM_RELEASE(pIniFile);
		return 0;
	}

	int KFileClassScriptTable::LuaIni_Open(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szFileName = NULL;
		IIniFile* pIniFile = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		szFileName = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(szFileName);

		szFileName = FormatFilePath(szFileName);
		KGLOG_PROCESS_ERROR(szFileName);

		bRetCode = g_IsFileExist(szFileName);
		KG_PROCESS_ERROR(bRetCode);

		pIniFile = g_OpenIniFile(szFileName);
		KGLOG_PROCESS_ERROR(pIniFile);

		bRetCode = NewUserData(L, pIniFile);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 1;

Exit0:
		KG_COM_RELEASE(pIniFile);
		return 0;
	}

	int KFileClassScriptTable::LuaIni_Save(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szFileName = NULL;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szFileName = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szFileName);

		szFileName = FormatFilePath(szFileName);
		KGLOG_PROCESS_ERROR(szFileName);

		bRetCode = pIniFile->Save(szFileName);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_Close(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		pIniFile->Release();

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_Clear(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		pIniFile->Clear();

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_IsSectionExist(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		BOOL bIsExist = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		bIsExist = pIniFile->IsSectionExist(szSection);

		lua_pushboolean(L, bIsExist);
		return 1;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_GetNextSection(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		BOOL bIsExist = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		char szNextSection[128];

		ASSERT(L);

		bRetCode = lua_gettop(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		bIsExist = pIniFile->GetNextSection(szSection, szNextSection);
		if (bIsExist)
		{
			lua_pushstring(L, szNextSection);
			return 1;
		}
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_GetNextKey(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		BOOL bIsExist = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		char szNextKey[128];

		ASSERT(L);

		bRetCode = lua_gettop(L) == 3;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		bIsExist = pIniFile->GetNextKey(szSection, szKey, szNextKey);
		if (bIsExist)
		{
			lua_pushstring(L, szNextKey);
			return 1;
		}
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_GetSectionCount(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		int nCount = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		nCount = pIniFile->GetSectionCount();

		lua_pushnumber(L, nCount);
		return 1;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_EraseSection(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		pIniFile->EraseSection(szSection);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_EraseKey(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 3;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		pIniFile->EraseKey(szSection, szKey);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_ReadString(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		LPCSTR szDefault = NULL;
		char szBuffer[512];
		szBuffer[0] = '\0';

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		szDefault = lua_tostring(L, 4);
		KGLOG_PROCESS_ERROR(szDefault);

		pIniFile->GetString(szSection, szKey, szDefault, szBuffer, _countof(szBuffer));

		lua_pushstring(L, szBuffer);
		return 1;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_ReadInteger(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		int nDefault = 0;
		int nResult = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		nDefault = (int)lua_tonumber(L, 4);

		pIniFile->GetInteger(szSection, szKey, nDefault, &nResult);

		lua_pushnumber(L, nResult);
		return 1;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_ReadMultiInteger(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		TBlock<int> aBuffer;
		int nCount = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		nCount = (int)lua_tonumber(L, 4);
		KGLOG_PROCESS_ERROR(nCount > 0);

		bRetCode = aBuffer.Alloc(nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pIniFile->GetMultiInteger(szSection, szKey, &aBuffer[0], nCount);
		KG_PROCESS_ERROR(bRetCode);

		for (int nIndex = 0; nIndex < nCount; ++nIndex)
			lua_pushnumber(L, aBuffer[nIndex]);

		return nCount;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_ReadMultiLong(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		TBlock<long> aBuffer;
		int nCount = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		nCount = (int)lua_tonumber(L, 4);
		KGLOG_PROCESS_ERROR(nCount > 0);

		bRetCode = aBuffer.Alloc(nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pIniFile->GetMultiLong(szSection, szKey, &aBuffer[0], nCount);
		KG_PROCESS_ERROR(bRetCode);

		for (int nIndex = 0; nIndex < nCount; ++nIndex)
			lua_pushnumber(L, aBuffer[nIndex]);

		return nCount;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_ReadFloat(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		float fDefault = 0.0f;
		float fResult = 0.0f;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		fDefault = (float)lua_tonumber(L, 4);

		pIniFile->GetFloat(szSection, szKey, fDefault, &fResult);

		lua_pushnumber(L, fResult);
		return 1;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_ReadMultiFloat(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		TBlock<float> aBuffer;
		int nCount = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		nCount = (int)lua_tonumber(L, 4);
		KGLOG_PROCESS_ERROR(nCount > 0);

		bRetCode = aBuffer.Alloc(nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = pIniFile->GetMultiFloat(szSection, szKey, &aBuffer[0], nCount);
		KG_PROCESS_ERROR(bRetCode);

		for (int nIndex = 0; nIndex < nCount; ++nIndex)
			lua_pushnumber(L, aBuffer[nIndex]);

		return nCount;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_WriteString(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		LPCSTR szValue = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		szValue = lua_tostring(L, 4);
		KGLOG_PROCESS_ERROR(szValue);

		bRetCode = pIniFile->WriteString(szSection, szKey, szValue);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_WriteInteger(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		int nValue = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

        if (lua_isboolean(L, 4))
            nValue = (int)lua_toboolean(L, 4);
        else
		    nValue = (int)lua_tonumber(L, 4);

		bRetCode = pIniFile->WriteInteger(szSection, szKey, nValue);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_WriteMultiInteger(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		TBlock<int> aBuffer;
		int nCount = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) > 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		nCount = (int)lua_tonumber(L, 4);
		KGLOG_PROCESS_ERROR(nCount > 0);

		bRetCode = aBuffer.Alloc(nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		for (int nIndex = 0; nIndex < nCount; ++nIndex)
			aBuffer[nIndex] = (int)lua_tonumber(L, nIndex + 5);
		
		bRetCode = pIniFile->WriteMultiInteger(szSection, szKey, &aBuffer[0], nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_WriteMultiLong(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		TBlock<long> aBuffer;
		int nCount = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) > 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		nCount = (int)lua_tonumber(L, 4);
		KGLOG_PROCESS_ERROR(nCount > 0);

		bRetCode = aBuffer.Alloc(nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		for (int nIndex = 0; nIndex < nCount; ++nIndex)
			aBuffer[nIndex] = (long)lua_tonumber(L, nIndex + 5);
		
		bRetCode = pIniFile->WriteMultiLong(szSection, szKey, &aBuffer[0], nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_WriteFloat(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		float fValue = 0.0f;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		fValue = (float)lua_tonumber(L, 4);

		bRetCode = pIniFile->WriteFloat(szSection, szKey, fValue);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaIni_WriteMultiFloat(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		IIniFile** ppIniFile = NULL;
		IIniFile* pIniFile = NULL;
		LPCSTR szSection = NULL;
		LPCSTR szKey = NULL;
		TBlock<float> aBuffer;
		int nCount = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) > 4;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppIniFile = (IIniFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppIniFile);

		pIniFile = *ppIniFile;
		KGLOG_PROCESS_ERROR(pIniFile);

		szSection = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szSection);

		szKey = lua_tostring(L, 3);
		KGLOG_PROCESS_ERROR(szKey);

		nCount = (int)lua_tonumber(L, 4);
		KGLOG_PROCESS_ERROR(nCount > 0);

		bRetCode = aBuffer.Alloc(nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		for (int nIndex = 0; nIndex < nCount; ++nIndex)
			aBuffer[nIndex] = (float)lua_tonumber(L, nIndex + 5);
		
		bRetCode = pIniFile->WriteMultiFloat(szSection, szKey, &aBuffer[0], nCount);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaTab_Create(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile* pTabFile = NULL;
		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		pTabFile = g_CreateTabFile();
		KGLOG_PROCESS_ERROR(pTabFile);

		bRetCode = NewUserData(L, pTabFile);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 1;
Exit0:
		KG_COM_RELEASE(pTabFile);
		return 0;
	}

	int KFileClassScriptTable::LuaTab_Open(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szFileName = NULL;
		ITabFile* pTabFile = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		szFileName = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(szFileName);

		szFileName = FormatFilePath(szFileName);
		KGLOG_PROCESS_ERROR(szFileName);

		pTabFile = g_OpenTabFile(szFileName);
		KG_PROCESS_ERROR(pTabFile);

		bRetCode = NewUserData(L, pTabFile);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 1;
Exit0:
		KG_COM_RELEASE(pTabFile);

		KGLogPrintf(KGLOG_ERR, "KGUI KFileClassScriptTable::LuaTab_Open(%s)\n", szFileName);
		return 0;
	}


	int KFileClassScriptTable::LuaTab_Close(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile** ppTabFile = NULL;
		ITabFile* pTabFile = NULL;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppTabFile = (ITabFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppTabFile);

		pTabFile = *ppTabFile;
		KGLOG_PROCESS_ERROR(pTabFile);

		pTabFile->Release();

		return 0;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaTab_FindRow(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile** ppTabFile = NULL;
		ITabFile* pTabFile = NULL;
		LPCSTR szRow = NULL;
		int nRow = -1;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppTabFile = (ITabFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppTabFile);

		pTabFile = *ppTabFile;
		KGLOG_PROCESS_ERROR(pTabFile);

		szRow = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szRow);

		nRow = pTabFile->FindRow(szRow);
		KGLOG_PROCESS_ERROR(nRow != -1);

		lua_pushnumber(L, nRow);
		return 1;
Exit0:
		return 0;
	}


	int KFileClassScriptTable::LuaTab_FindColumn(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile** ppTabFile = NULL;
		ITabFile* pTabFile = NULL;
		LPCSTR szColumn = NULL;
		int nColumn = -1;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppTabFile = (ITabFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppTabFile);

		pTabFile = *ppTabFile;
		KGLOG_PROCESS_ERROR(pTabFile);

		szColumn = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(szColumn);

		nColumn = pTabFile->FindColumn(szColumn);
		KGLOG_PROCESS_ERROR(nColumn != -1);

		lua_pushnumber(L, nColumn);
		return 1;
Exit0:
		return 0;
	}


	int KFileClassScriptTable::LuaTab_GetWidth(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile** ppTabFile = NULL;
		ITabFile* pTabFile = NULL;
		int nWidth = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppTabFile = (ITabFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppTabFile);

		pTabFile = *ppTabFile;
		KGLOG_PROCESS_ERROR(pTabFile);

		nWidth = pTabFile->GetWidth();

		lua_pushnumber(L, nWidth);
		return 1;
Exit0:
		return 0;
	}


	int KFileClassScriptTable::LuaTab_GetHeight(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile** ppTabFile = NULL;
		ITabFile* pTabFile = NULL;
		int nHeight = 0;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppTabFile = (ITabFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppTabFile);

		pTabFile = *ppTabFile;
		KGLOG_PROCESS_ERROR(pTabFile);

		nHeight = pTabFile->GetHeight();

		lua_pushnumber(L, nHeight);
		return 1;
Exit0:
		return 0;
	}


	int KFileClassScriptTable::LuaTab_GetString(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile** ppTabFile = NULL;
		ITabFile* pTabFile = NULL;
		char szResult[1024];
		int nParamCount = 0;

		ASSERT(L);

		szResult[0] = '\0';

        nParamCount = lua_gettop(L);

		bRetCode = nParamCount == 4 || nParamCount == 5;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppTabFile = (ITabFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppTabFile);

		pTabFile = *ppTabFile;
		KGLOG_PROCESS_ERROR(pTabFile);

		if (lua_type(L, 2) == LUA_TNUMBER && lua_type(L, 3) == LUA_TNUMBER)
		{
			int nRow = -1;
			int nColumn = -1;
			LPCSTR szDefault = NULL;

			bRetCode = nParamCount == 4;
			KGLOG_PROCESS_ERROR(bRetCode);

			nRow = (int)lua_tonumber(L, 2);
			nColumn = (int)lua_tonumber(L, 3);

			szDefault = lua_tostring(L, 4);
			KGLOG_PROCESS_ERROR(szDefault);

			pTabFile->GetString(nRow, nColumn, szDefault, szResult, _countof(szResult));
		}
		else if (lua_type(L, 2) == LUA_TNUMBER && lua_type(L, 3) ==LUA_TSTRING)
		{
			bool bColumnLab = true;
			int nRow = -1;
			LPCSTR szColumn = NULL;
			LPCSTR szDefault = NULL;

			nRow = (int)lua_tonumber(L, 2);

			szColumn = lua_tostring(L, 3);
			KGLOG_PROCESS_ERROR(szColumn);

			szDefault = lua_tostring(L, 4);
			KGLOG_PROCESS_ERROR(szDefault);

			if (nParamCount == 5)
				bColumnLab = (int)lua_toboolean(L, 5) != 0;

			pTabFile->GetString(nRow, szColumn, szDefault, szResult, _countof(szResult), bColumnLab);
		}
		else if (lua_type(L, 2) ==LUA_TSTRING && lua_type(L, 3) ==LUA_TSTRING)
		{
			LPCSTR szRow = NULL;
			LPCSTR szColumn = NULL;
			LPCSTR szDefault = NULL;

			bRetCode = nParamCount == 4;
			KGLOG_PROCESS_ERROR(bRetCode);

			szRow = lua_tostring(L, 2);
			KGLOG_PROCESS_ERROR(szRow);

			szColumn = lua_tostring(L, 3);
			KGLOG_PROCESS_ERROR(szColumn);

			szDefault = lua_tostring(L, 4);
			KGLOG_PROCESS_ERROR(szDefault);

			pTabFile->GetString(szRow, szColumn, szDefault, szResult, _countof(szResult));
		}
		else
		{
			KGLOG_PROCESS_ERROR(0);
		}

		lua_pushstring(L, szResult);
		return 1;
Exit0:
		return 0;
	}


	int KFileClassScriptTable::LuaTab_GetInteger(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile** ppTabFile = NULL;
		ITabFile* pTabFile = NULL;
		int nResult = 0;
		int nParamCount = 0;

		ASSERT(L);

		nParamCount = lua_gettop(L);

		bRetCode = nParamCount == 4 || nParamCount == 5;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppTabFile = (ITabFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppTabFile);

		pTabFile = *ppTabFile;
		KGLOG_PROCESS_ERROR(pTabFile);

		if (lua_type(L, 2) == LUA_TNUMBER && lua_type(L, 3) == LUA_TNUMBER)
		{
			int nRow = -1;
			int nColumn = -1;
			int nDefault = 0;

			bRetCode = nParamCount == 4;
			KGLOG_PROCESS_ERROR(bRetCode);

			nRow = static_cast<int>(lua_tonumber(L, 2));
			nColumn = static_cast<int>(lua_tonumber(L, 3));

			nDefault = static_cast<int>(lua_tonumber(L, 4));

			bRetCode = pTabFile->GetInteger(nRow, nColumn, nDefault, &nResult);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else if (lua_type(L, 2) == LUA_TNUMBER && lua_type(L, 3) ==LUA_TSTRING)
		{
			BOOL bColumnLab = TRUE;
			int nRow = -1;
			int nDefault = 0;
			LPCSTR szColumn = NULL;

			nRow = (int)lua_tonumber(L, 2);

			szColumn = lua_tostring(L, 3);
			KGLOG_PROCESS_ERROR(szColumn);

			nDefault = (int)lua_tonumber(L, 4);

			if (nParamCount == 5)
				bColumnLab = (int)lua_toboolean(L, 5) != 0;

			bRetCode = pTabFile->GetInteger(nRow, szColumn, nDefault, &nResult, bColumnLab);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else if (lua_type(L, 2) ==LUA_TSTRING && lua_type(L, 3) ==LUA_TSTRING)
		{
			LPCSTR szRow = NULL;
			LPCSTR szColumn = NULL;
			int nDefault = 0;

			bRetCode = nParamCount == 4;
			KGLOG_PROCESS_ERROR(bRetCode);

			szRow = lua_tostring(L, 2);
			KGLOG_PROCESS_ERROR(szRow);

			szColumn = lua_tostring(L, 3);
			KGLOG_PROCESS_ERROR(szColumn);

			nDefault = (int)lua_tonumber(L, 4);

			bRetCode = pTabFile->GetInteger(szRow, szColumn, nDefault, &nResult);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else
		{
			KGLOG_PROCESS_ERROR(0);
		}

		lua_pushnumber(L, nResult);
		return 1;
Exit0:
		return 0;
	}

	int KFileClassScriptTable::LuaTab_GetFloat(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		ITabFile** ppTabFile = NULL;
		ITabFile* pTabFile = NULL;
		float fResult = 0.0f;
		int nParamCount = 0;

		ASSERT(L);

		nParamCount = lua_gettop(L);

		bRetCode = nParamCount == 4 || nParamCount == 5;
		KGLOG_PROCESS_ERROR(bRetCode);

		ppTabFile = (ITabFile**)lua_touserdata(L, 1);
		KGLOG_PROCESS_ERROR(ppTabFile);

		pTabFile = *ppTabFile;
		KGLOG_PROCESS_ERROR(pTabFile);

		if (lua_type(L, 2) == LUA_TNUMBER && lua_type(L, 3) == LUA_TNUMBER)
		{
			int nRow = -1;
			int nColumn = -1;
			float fDefault = 0.0f;

			bRetCode = nParamCount == 4;
			KGLOG_PROCESS_ERROR(bRetCode);

			nRow = (int)lua_tonumber(L, 2);
			nColumn = (int)lua_tonumber(L, 3);

			fDefault = (float)lua_tonumber(L, 4);

			bRetCode = pTabFile->GetFloat(nRow, nColumn, fDefault, &fResult);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else if (lua_type(L, 2) == LUA_TNUMBER && lua_type(L, 3) ==LUA_TSTRING)
		{
			BOOL bColumnLab = TRUE;
			int nRow = -1;
			float fDefault = 0.0f;
			LPCSTR szColumn = NULL;

			nRow = (int)lua_tonumber(L, 2);

			szColumn = lua_tostring(L, 3);
			KGLOG_PROCESS_ERROR(szColumn);

			fDefault = (float)lua_tonumber(L, 4);

			if (nParamCount == 5)
				bColumnLab = (int)lua_toboolean(L, 5) != 0;

			bRetCode = pTabFile->GetFloat(nRow, szColumn, fDefault, &fResult, bColumnLab);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else if (lua_type(L, 2) ==LUA_TSTRING && lua_type(L, 3) ==LUA_TSTRING)
		{
			LPCSTR szRow = NULL;
			LPCSTR szColumn = NULL;
			float fDefault = 0.0f;

			bRetCode = nParamCount == 4;
			KGLOG_PROCESS_ERROR(bRetCode);

			szRow = lua_tostring(L, 2);
			KGLOG_PROCESS_ERROR(szRow);

			szColumn = lua_tostring(L, 3);
			KGLOG_PROCESS_ERROR(szColumn);

			fDefault = (float)lua_tonumber(L, 4);

			bRetCode = pTabFile->GetFloat(szRow, szColumn, fDefault, &fResult);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else
		{
			KGLOG_PROCESS_ERROR(0);
		}

		lua_pushnumber(L, fResult);
		return 1;
Exit0:
		return 0;
	}

    int KFileClassScriptTable::LuaTable_Load(lua_State *pLua)
    {
        int nRetCode = false;
        int nResult = false;

        int nRetParamCount = 0;

        LPCSTR pcszFilePath = NULL;
        LPCSTR pcszKey = NULL;
        KG_Table **ppTable = NULL;
        KG_Table *pTable = NULL;
        KG_FILE_OPEN_MODE emFileOpemMode = KG_FILE_OPEN_NORMAL;
        int nMode = 0;

        KG_Pair Key;
        std::string szFormat;
        int nIndex = 0;
        int nParamCount = 0;

        KGLOG_PROCESS_ERROR(pLua);

        nParamCount = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 3);

        pTable = new KG_Table();
        KGLOG_PROCESS_ERROR(pTable);

        pTable->TitleVector.clear();
        pTable->piTabFile = NULL;

        pcszFilePath = lua_tostring(pLua, 1);
        KGLOG_PROCESS_ERROR(pcszFilePath);
        KGLOG_PROCESS_ERROR(pcszFilePath[0]);

        pcszFilePath = FormatFilePath(pcszFilePath);
        KGLOG_PROCESS_ERROR(pcszFilePath);
        KGLOG_PROCESS_ERROR(pcszFilePath[0]);

        nRetCode = lua_istable(pLua, 2);
        KGLOG_PROCESS_ERROR(nRetCode);

        nIndex = 1;
        szFormat.clear();

        while (true)
        {
            lua_pushnumber(pLua, nIndex);
            lua_gettable(pLua, 2);

            nRetCode = lua_isnil(pLua, -1);
            if (nRetCode)
            {
                lua_pop(pLua, 1);
                break;
            }

            nRetCode = lua_istable(pLua, -1);
            KGLOG_PROCESS_ERROR(nRetCode);

            lua_getfield(pLua, -1, "t");
            pcszKey = lua_tostring(pLua, -1);
            KGLOG_PROCESS_ERROR(pcszKey);
            KGLOG_PROCESS_ERROR(pcszKey[0]);

            Key.first = pcszKey;
            lua_pop(pLua, 1);   // table[t]

            lua_getfield(pLua, -1, "f");
            pcszKey = lua_tostring(pLua, -1);
            KGLOG_PROCESS_ERROR(pcszKey);
            KGLOG_PROCESS_ERROR(pcszKey[0]);

            Key.second = pcszKey[0];
            lua_pop(pLua, 1);   // table[f]

            if (Key.second == 'b')
            {
                szFormat += 'i';
            }
            else
            {
                szFormat += Key.second;
            }

            lua_pop(pLua, 1);   // table

            pTable->TitleVector.push_back(Key);
            nIndex++;
        }

        if (nParamCount == 3)
        {
            nRetCode = lua_isnumber(pLua, 3);
            KGLOG_PROCESS_ERROR(nRetCode);

            nMode = (int)lua_tonumber(pLua, 3);
            emFileOpemMode = (KG_FILE_OPEN_MODE)nMode;
            KGLOG_PROCESS_ERROR(emFileOpemMode >= KG_FILE_OPEN_MODE_BEGIN);
            KGLOG_PROCESS_ERROR(emFileOpemMode < KG_FILE_OPEN_MODE_COUNT);
        }

        pTable->piTabFile = OpenTabFile(pcszFilePath, szFormat.c_str(), emFileOpemMode);
        KGLOG_PROCESS_ERROR(pTable->piTabFile);

        pTable->uTotalRow = pTable->piTabFile->GetRowCount();

        pTable->uTotalColumn = pTable->TitleVector.size();
        KGLOG_PROCESS_ERROR(pTable->uTotalColumn > 0);

        ppTable = (KG_Table **)lua_newuserdata(pLua, sizeof(KG_Table *));
        KGLOG_PROCESS_ERROR(ppTable);
        *ppTable = pTable;

        luaL_getmetatable(pLua, "TableFile");
        nRetCode = lua_isnil(pLua, -1);
        KGLOG_PROCESS_ERROR(!nRetCode);

        lua_setmetatable(pLua, -2);

        nRetParamCount++;
        pTable = NULL;
        nResult = true;
Exit0:
        if (!nResult)
        {
            nRetParamCount = 0;
            if (pTable && pTable->piTabFile)
            {
                SAFE_RELEASE(pTable->piTabFile);
            }
            SAFE_DELETE(pTable);
            if (ppTable)
            {
                *ppTable = NULL;
            }
            KGLogPrintf(KGLOG_ERR, "KFileClassScriptTable::LuaTable_Load(%s) Failed\n", pcszFilePath);
        }
        return nRetParamCount;
    }

    int KFileClassScriptTable::LuaTable_Release(lua_State *pLua)
    {
        int nRetCode = false;

        KG_Table **ppTable = NULL;
        KG_Table *pTable = NULL;

        nRetCode = lua_gettop(pLua);
        ASSERT(nRetCode == 1);

        ppTable = (KG_Table **)lua_touserdata(pLua, 1);
        if (ppTable && *ppTable)
        {
            pTable = *ppTable;
            pTable->TitleVector.clear();
            SAFE_RELEASE(pTable->piTabFile);
            SAFE_DELETE(pTable);
        }
        return 1;
    }

    int KFileClassScriptTable::LuaTable_GetRow(lua_State *pLua)
    {
        int nRetCode = false;
        int nResult = false;
        int nRetParamCount = 0;

        KG_Table **ppTable = NULL;
        KG_Table *pTable = NULL;
        BYTE byRow[MAX_ROW_SIZE];
        size_t uRow = 0;

        KGLOG_PROCESS_ERROR(pLua);

        nRetCode = lua_gettop(pLua) == 2;
        KGLOG_PROCESS_ERROR(nRetCode);

        ppTable = (KG_Table **)lua_touserdata(pLua, 1);
        KGLOG_PROCESS_ERROR(ppTable);
        pTable = *ppTable;
        KGLOG_PROCESS_ERROR(pTable);
        KGLOG_PROCESS_ERROR(pTable->piTabFile);

        nRetCode = lua_isnumber(pLua, 2);
        KGLOG_PROCESS_ERROR(nRetCode);
        uRow = (int)lua_tonumber(pLua, 2);
        KGLOG_PROCESS_ERROR(uRow <= pTable->uTotalRow);
        KGLOG_PROCESS_ERROR(uRow >= 1);
        KGLOG_PROCESS_ERROR(pTable->uTotalColumn < sizeof(byRow) / CELL_SIZE);

        // lua Index from 1 then C++ Index from 0
        uRow--;
        nRetCode = pTable->piTabFile->GetRow((unsigned)uRow, (void *)byRow, sizeof(byRow));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = LuaTable_PushRow(pLua, pTable, byRow);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetParamCount++;
Exit0:
        return nRetParamCount;
    }


    int KFileClassScriptTable::LuaTable_Search(lua_State *pLua)
    {
        int nRetCode  = false;
        int nRetParamCount = 0;

        KG_Table **ppTable = NULL;
        KG_Table *pTable = NULL;
        IKG_TabFile *piFile = NULL;
        BYTE byRow[MAX_ROW_SIZE];
        int nTopIndex = 0;
        int aKey[4];

        KGLOG_PROCESS_ERROR(pLua);

        nTopIndex = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nTopIndex >= 2);
        KGLOG_PROCESS_ERROR(nTopIndex <= 4);

        ppTable = (KG_Table **)lua_touserdata(pLua, 1);
        KGLOG_PROCESS_ERROR(ppTable);
        pTable = *ppTable;
        KGLOG_PROCESS_ERROR(pTable);
        KGLOG_PROCESS_ERROR(pTable->piTabFile);
        KGLOG_PROCESS_ERROR(pTable->uTotalColumn >= unsigned(nTopIndex - 1));
        KGLOG_PROCESS_ERROR(pTable->uTotalColumn < sizeof(byRow) / CELL_SIZE);

        piFile = pTable->piTabFile;
        switch (nTopIndex)
        {
        case 2:
            nRetCode = lua_isnumber(pLua, 2);
            KGLOG_PROCESS_ERROR(nRetCode);
            aKey[0] = (unsigned)lua_tonumber(pLua, 2);

            nRetCode = piFile->Search(
                (void *)aKey,
                (IKG_TabFile::FUNC_COMPARE)Compare1Key,
                byRow,
                sizeof(byRow)
            );
            KG_PROCESS_ERROR(nRetCode);
            break;
        case 3:
            nRetCode = lua_isnumber(pLua,2);
            KGLOG_PROCESS_ERROR(nRetCode);
            aKey[0] = (unsigned)lua_tonumber(pLua, 2);

            nRetCode = lua_isnumber(pLua, 3);
            KGLOG_PROCESS_ERROR(nRetCode);
            aKey[1] = (unsigned)lua_tonumber(pLua, 3);

            nRetCode = piFile->Search(
                (void *)aKey,
                (IKG_TabFile::FUNC_COMPARE)Compare2Key,
                byRow,
                sizeof(byRow)
            );
            KG_PROCESS_ERROR(nRetCode);
            break;
        case 4:
            nRetCode = lua_isnumber(pLua,2);
            KGLOG_PROCESS_ERROR(nRetCode);
            aKey[0] = (unsigned)lua_tonumber(pLua, 2);

            nRetCode = lua_isnumber(pLua, 3);
            KGLOG_PROCESS_ERROR(nRetCode);
            aKey[1] = (unsigned)lua_tonumber(pLua, 3);

            nRetCode = lua_isnumber(pLua, 4);
            KGLOG_PROCESS_ERROR(nRetCode);
            aKey[2] = (unsigned)lua_tonumber(pLua, 4);

            nRetCode = piFile->Search(
                (void *)aKey,
                (IKG_TabFile::FUNC_COMPARE)Compare3Key,
                byRow,
                sizeof(byRow)
            );
            KG_PROCESS_ERROR(nRetCode);
            break;
        default:
            KGLOG_PROCESS_ERROR(false);
        }

        nRetCode = LuaTable_PushRow(pLua, pTable, byRow);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetParamCount++;
Exit0:
        return nRetParamCount;
    }

    int KFileClassScriptTable::LuaTable_GetRowCount(lua_State *pLua)
    {
        int nRetCode = false;
        int nRetParamCount = 0;

        KG_Table **ppTable = NULL;
        KG_Table *pTable = NULL;

        KGLOG_PROCESS_ERROR(pLua);

        nRetCode = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        ppTable = (KG_Table **)lua_touserdata(pLua, 1);
        KGLOG_PROCESS_ERROR(ppTable);
        pTable = *ppTable;
        KGLOG_PROCESS_ERROR(pTable);

        lua_pushnumber(pLua, pTable->uTotalRow);

        nRetParamCount++;
Exit0:
        return nRetParamCount;
    }

    // if success return a table on top of the stack
    int KFileClassScriptTable::LuaTable_PushRow(
        lua_State *pLua, 
        KG_Table *pTable, 
        BYTE *pbyRow
    )
    {
        int nResult = false;
        int nRetCode = false;

        int nTopIndex = 0;
        size_t uColumn = 0;
        BYTE *pbyCell = NULL;
        int nKey = 0;
        float fKey = 0.0f;
        char *pszKey = NULL;
        const char *pcszTitle = NULL;

        KGLOG_PROCESS_ERROR(pLua);
        KGLOG_PROCESS_ERROR(pTable);
        KGLOG_PROCESS_ERROR(pbyRow);

        nTopIndex = lua_gettop(pLua);

        lua_newtable(pLua); // table for Row

        for (uColumn = 0; uColumn < pTable->uTotalColumn; uColumn++)
        {
            pbyCell = pbyRow + uColumn * CELL_SIZE;

            pcszTitle = pTable->TitleVector[uColumn].first.c_str();
            KGLOG_PROCESS_ERROR(pcszTitle);
            KGLOG_PROCESS_ERROR(pcszTitle[0]);

            lua_pushstring(pLua, pcszTitle);

            switch (pTable->TitleVector[uColumn].second)
            {
            case 'i':
                nKey = *((int *)pbyCell);
                lua_pushnumber(pLua, nKey);
                break;
            case 'f':
                fKey = *((float *)pbyCell);
                lua_pushnumber(pLua, fKey);
                break;
            case 'b':
                nKey = *((int *)pbyCell);
                lua_pushboolean(pLua, nKey);
                break;
            case 'p':
            case 's':
            case 'S':
                pszKey = *(char **)pbyCell;
                KGLOG_PROCESS_ERROR(pszKey);
                lua_pushstring(pLua, pszKey);
                break;
            default:
                KGLOG_PROCESS_ERROR(false);
                break;
            }
            lua_settable(pLua, -3);
        }

        nResult = true;
Exit0:
        if (!nResult)
        {
            lua_settop(pLua, nTopIndex);
        }
        return nResult;
    }

}





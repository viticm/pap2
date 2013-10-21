#include "stdafx.h"
#include "./kschemescripttable.h"
#include "./kscriptloader.h"
#include "../driver/kcolor.h"
#include "../elem/Components/pickdropboxmgr.h"
#include "../elem/Components/decoder.h"
#include "../KGUI.h"

namespace UI
{

	void KSchemeScriptTable::Load(lua_State* L)
	{
		static KLuaFunc s_aGlobalFunctionTable[] =
		{
			{ "GetTextExtent",						LuaGetTextExtent },
			{ "EncodeComponentsString",				LuaEncodeComponentsString },
			{ "GetPureText",						LuaGetPureText },
		};

		static luaL_reg s_aFontLibTable[] =
		{
			{ "SetOffset",							LuaFont_SetFontOffset },
			{ "GetOffset",							LuaFont_GetFontOffset },
			{ "GetChatFontID",						LuaFont_GetChatFontID },
			{ "LoadDefaultFontList",				LuaFont_LoadDefaultFontList },
			{ "LoadFontList",						LuaFont_LoadFontList },
			{ "SetFont",							LuaFont_SetFont },
			{ "GetFont",							LuaFont_GetFont },

			{ NULL,									NULL },
		};

		KScriptLoader::RegisterLibTable(L, "Font", s_aFontLibTable);

        KScriptLoader::RegisterFunctions(L, s_aGlobalFunctionTable, _countof(s_aGlobalFunctionTable));
	}


	int KSchemeScriptTable::LuaFont_SetFontOffset(Lua_State* L)
	{
		ASSERT(L);

		int nOffset = 0;
		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		nOffset = (int)Lua_ValueToNumber(L, 1);

		g_pUI->m_FontSchemeMgr.SetOffset(nOffset);
Exit0:
		return 0;

	}

	int KSchemeScriptTable::LuaFont_GetFontOffset(Lua_State* L)
	{
		ASSERT(L);

		int nOffset = 0;
		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

		nOffset = g_pUI->m_FontSchemeMgr.GetOffset();

Exit0:
		Lua_PushNumber(L, nOffset);
		return 1;
	}

	int KSchemeScriptTable::LuaFont_LoadDefaultFontList(Lua_State* L)
	{
		g_pUI->m_FontSchemeMgr.LoadDefaultFontList();
		return 0;
	}

	int KSchemeScriptTable::LuaFont_LoadFontList(Lua_State* L)
	{
		int nResult = false;
		int nRetCode = false;
		LPCSTR pcszPath = NULL;
		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszPath = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszPath);

		pcszPath = FormatFilePath(pcszPath);
		KGLOG_PROCESS_ERROR(pcszPath);

		nRetCode = g_pUI->m_FontSchemeMgr.LoadFontList(pcszPath);
		KGLOG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
		Lua_PushBoolean(L, nResult);
		return 1;
	}

	int KSchemeScriptTable::LuaFont_SetFont(Lua_State* L)
	{
		int nResult = false;
		int nRetCode = false;
		DWORD dwID = 0;
		LPCSTR pcszName= NULL;
		LPCSTR pcszPath = NULL;
		int nSize = 20;
		DWORD dwStyle = KG3DUI_TEXT_STYLE_NONE;
		int nParamCount = Lua_GetTopIndex(L);
		KG_PROCESS_ERROR(nParamCount > 3);

		dwID = (DWORD)(int)Lua_ValueToNumber(L, 1);
		pcszName = Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(pcszName);
		pcszPath = Lua_ValueToString(L, 3);
		KGLOG_PROCESS_ERROR(pcszPath);
		nSize = (int)Lua_ValueToNumber(L, 4);
		if (nParamCount > 4 && Lua_IsTable(L, 5))
		{
			Lua_PushString(L, "vertical");
			lua_gettable(L, 5);
			if (lua_isboolean(L, -1) && Lua_ValueToBoolean(L, -1))
				dwStyle |= KG3DUI_TEXT_STYLE_VERTICAL;				
			Lua_Pop(L, 1);

			Lua_PushString(L, "border");
			lua_gettable(L, 5);
			if (lua_isboolean(L, -1) && Lua_ValueToBoolean(L, -1))
				dwStyle |= KG3DUI_TEXT_STYLE_BORDER;				
			Lua_Pop(L, 1);

			Lua_PushString(L, "shadow");
			lua_gettable(L, 5);
			if (lua_isboolean(L, -1) && Lua_ValueToBoolean(L, -1))
				dwStyle |= KG3DUI_TEXT_STYLE_SHADOW;				
			Lua_Pop(L, 1);

			Lua_PushString(L, "mono");
			lua_gettable(L, 5);
			if (lua_isboolean(L, -1) && Lua_ValueToBoolean(L, -1))
				dwStyle |= KG3DUI_TEXT_STYLE_MONO;				
			Lua_Pop(L, 1);

			Lua_PushString(L, "mipmap");
			lua_gettable(L, 5);
			if (lua_isboolean(L, -1) && Lua_ValueToBoolean(L, -1))
				dwStyle |= KG3DUI_TEXT_STYLE_MIPMAP;				
			Lua_Pop(L, 1);
		}


		nRetCode = g_pUI->m_FontSchemeMgr.SetFont(dwID, pcszName, pcszPath, nSize, dwStyle);
		KGLOG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
		Lua_PushBoolean(L, nResult);
		return 1;
	}

	int KSchemeScriptTable::LuaFont_GetFont(Lua_State* L)
	{
		int nResult = false;
		int nRetCode = false;
		DWORD dwID = 0;
		const KFontSchemeMgr::KFontInfo * pInfo = NULL;
		int nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount == 1);

		dwID = (DWORD)(int)Lua_ValueToNumber(L, 1);

		pInfo = g_pUI->m_FontSchemeMgr.GetFont(dwID);
		KGLOG_PROCESS_ERROR(pInfo);

		Lua_PushString(L, pInfo->szName);
		Lua_PushString(L, pInfo->szFile);
		Lua_PushNumber(L, pInfo->nSize);

		lua_newtable(L);

		if (pInfo->dwStyle & KG3DUI_TEXT_STYLE_VERTICAL)
		{
			Lua_PushString(L, "vertical");
			Lua_PushBoolean(L, TRUE);
			lua_settable(L, -3);
		}

		if (pInfo->dwStyle & KG3DUI_TEXT_STYLE_BORDER)
		{
			Lua_PushString(L, "border");
			Lua_PushBoolean(L, TRUE);
			lua_settable(L, -3);
		}

		if (pInfo->dwStyle & KG3DUI_TEXT_STYLE_SHADOW)
		{
			Lua_PushString(L, "shadow");
			Lua_PushBoolean(L, TRUE);
			lua_settable(L, -3);
		}

		if (pInfo->dwStyle & KG3DUI_TEXT_STYLE_MONO)
		{
			Lua_PushString(L, "mono");
			Lua_PushBoolean(L, TRUE);
			lua_settable(L, -3);
		}

		if (pInfo->dwStyle & KG3DUI_TEXT_STYLE_MIPMAP)
		{
			Lua_PushString(L, "mipmap");
			Lua_PushBoolean(L, TRUE);
			lua_settable(L, -3);
		}

		return 4;
Exit0:
		return 0;
	}

	int KSchemeScriptTable::LuaFont_GetChatFontID(Lua_State* L)
	{
		lua_pushnumber(L, g_pUI->m_FontSchemeMgr.GetChatFontID());
		return 1;
	}

	int KSchemeScriptTable::LuaGetTextExtent(Lua_State* L)
	{
		int     nResult     = false;
		int     nRetCode    = false;
        HRESULT hr          = E_FAIL;
		LPCSTR  szText      = NULL;
		DWORD   dwFontID    = 0;
		int     nTextLength = 0;
		FLOAT   fSpace      = 0;
		FLOAT   fScale      = 0;
		FLOAT   fWidth      = 0;
		FLOAT   fHeight     = 0;

        nRetCode = (int)Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nRetCode == 4);

		szText      = Lua_ValueToString(L, 1);
		dwFontID    = (DWORD)Lua_ValueToNumber(L, 2);
		fSpace      = (FLOAT)Lua_ValueToNumber(L, 3);
		fScale      = (FLOAT)Lua_ValueToNumber(L, 4);

        hr = g_pUI->m_p3DUI->GetTextExtent(dwFontID, ATL::CA2W(szText, GetCodePage()), -1, fWidth, fHeight, fSpace, fScale, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);

		nResult = true;
Exit0:
		Lua_PushNumber(L, nResult);
		Lua_PushNumber(L, fWidth);
		Lua_PushNumber(L, fHeight);
		return 3;
	}

	int KSchemeScriptTable::LuaEncodeComponentsString(Lua_State *L)
	{
		ASSERT(L);
		LPCSTR pcszString = NULL;
		LPCWSTR wszEncodedString = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszString = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(pcszString);

        wszEncodedString = KUiComponentsDecoder::GetSelf().EncodeString(ATL::CA2W(pcszString, GetCodePage()));
		KG_PROCESS_ERROR(wszEncodedString);

Exit0:
        Lua_PushString(L, wszEncodedString ? ATL::CW2A(wszEncodedString, GetCodePage()) : "");
		return 1;
	}

	int KSchemeScriptTable::LuaGetPureText(Lua_State *L)
	{
		ASSERT(L);
		LPCSTR pcszString = NULL;
		LPCWSTR wszEncodedString = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszString = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(pcszString);

		wszEncodedString = KUiComponentsDecoder::GetSelf().GetPureText(ATL::CA2W(pcszString, GetCodePage()));
		KG_PROCESS_ERROR(wszEncodedString);

Exit0:
		Lua_PushString(L, wszEncodedString ? ATL::CW2A(wszEncodedString, GetCodePage()) : "");
		return 1;
	}
	
}

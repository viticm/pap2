#include "stdafx.h"
#include "./kmusicscripttsble.h"
#include "../script/kscriptmgr.h"
#include "./kscriptloader.h"
#include "../common/tblock.h"
#include "../KGUI.h"
#include "IKG3DSoundShell.h"

namespace UI
{

	void KMusicScriptTable::Load(lua_State* L)
	{
		static KLuaFunc s_aGlobalFunctionTable[] =
		{
			{ "PlayBgMusic",								LuaPlayBgMusic },
			{ "StopBgMusic",								LuaStopBgMusic },
			{ "PlaySound",									LuaPlaySound },
		};

		KScriptLoader::RegisterFunctions(L, s_aGlobalFunctionTable, _countof(s_aGlobalFunctionTable));
	}


    int KMusicScriptTable::LuaPlayBgMusic(Lua_State* L)
    {
		LPCSTR pcszFileName = NULL;

        ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 0);

		pcszFileName = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszFileName);

		pcszFileName = FormatFilePath(pcszFileName);
		KGLOG_PROCESS_ERROR(pcszFileName);

		g_pUI->m_MusicMgr.PlayMusic(pcszFileName);

Exit0:
		return 0;
    }


    int KMusicScriptTable::LuaStopBgMusic(Lua_State* L)
    {
		ASSERT(L);
		BOOL bImmediately = FALSE;
		if (Lua_GetTopIndex(L) > 0)
		{
			if (Lua_IsBoolean(L, 1))
				bImmediately = (BOOL)Lua_ValueToBoolean(L, 1);
			else
				bImmediately = (BOOL)Lua_ValueToNumber(L, 1);
		}

		g_pUI->m_MusicMgr.StopMusic(bImmediately);
		return 0;
    }

	int KMusicScriptTable::LuaPlaySound(Lua_State* L)
	{
		int nType = UI_SOUND;
		LPCSTR pcszFilePath = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) > 1);

		nType = (int)Lua_ValueToNumber(L, 1);
		pcszFilePath = Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(pcszFilePath);

		pcszFilePath = FormatFilePath(pcszFilePath);
		KGLOG_PROCESS_ERROR(pcszFilePath);

		g_pUI->m_MusicMgr.PlaySound(nType, pcszFilePath);

Exit0:
		return 0;
	}

}


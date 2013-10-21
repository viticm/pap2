////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KHotkeyClassScriptTable.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-6-23 9:03:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "./khotkeyclassscripttable.h"
#include "./kscriptloader.h"
#include "./kscriptmgr.h"
#include "../elem/tipcenter.h"
#include "../KGUI.h"


namespace UI
{


	void KHotkeyClassScriptTable::Load(lua_State* L)
	{
		static luaL_reg const s_aHotkeyLibTable[] =
		{
			{ "Load",					Lua_Load },
			{ "LoadAsAdd",				Lua_LoadAsAdd },
			{ "LoadDefault",			Lua_LoadDefault },
			{ "LoadFromLuaAsAdd",		Lua_LoadFromLuaAsAdd },
			{ "GetBinding",				Lua_GetBinding },
			{ "Save",					Lua_Save },
			{ "SaveAsAdd",				Lua_SaveAsAdd },
			{ "SaveToLuaAsAdd",			Lua_SaveToLuaAsAdd },
			{ "Clear",					Lua_Clear },
			{ "Set",					Lua_Set },
			{ "Get",					Lua_Get },
			{ "IsUsed",					Lua_IsUsed },
			{ "IsKeyDown",				Lua_IsKeyDown },
			{ "SetCapture",				Lua_SetCapture },
			{ "AddBinding",				Lua_AddBinding },
			{ "Enable",					Lua_Enable },
			{ "IsEnable",				Lua_IsEnable },
			{ "GetCaptureKey",			Lua_GetCaptureKey },

			{ NULL,						NULL },
		};

		KScriptLoader::RegisterLibTable(L, "Hotkey", s_aHotkeyLibTable);
	}


	int KHotkeyClassScriptTable::Lua_Load(Lua_State *L)
	{
		int nRetCode = false;
        int nParamCount = 0;
		LPCSTR pcszFile = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszFile = Lua_ValueToString(L, 1);
		pcszFile = FormatFilePath(pcszFile);
		KGLOG_PROCESS_ERROR(pcszFile);

		nRetCode = g_pUI->m_HotkeyMgr.Load(pcszFile);
		KG_PROCESS_ERROR(nRetCode);

		g_pUI->m_Script.FireEvent("HOT_KEY_RELOADED", 0);

Exit0:
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_LoadAsAdd(Lua_State *L)
	{
		int nRetCode = false;
		int nParamCount = 0;
		LPCSTR pcszFile = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszFile = Lua_ValueToString(L, 1);
		pcszFile = FormatFilePath(pcszFile);
		KGLOG_PROCESS_ERROR(pcszFile);

		nRetCode = g_pUI->m_HotkeyMgr.LoadAsAdd(pcszFile);
		KG_PROCESS_ERROR(nRetCode);

		g_pUI->m_Script.FireEvent("HOT_KEY_RELOADED", 0);

Exit0:
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_LoadFromLuaAsAdd(Lua_State *L)
	{
		int nResult = g_pUI->m_HotkeyMgr.LoadAsAdd(L);

		g_pUI->m_Script.FireEvent("HOT_KEY_RELOADED", 0);

		return nResult;
	}

	int KHotkeyClassScriptTable::Lua_Save(Lua_State *L)
	{
		int nRetCode = false;
		LPCSTR pcszFile = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszFile = Lua_ValueToString(L, 1);
		pcszFile = FormatFilePath(pcszFile);
		KGLOG_PROCESS_ERROR(pcszFile);

		nRetCode = g_pUI->m_HotkeyMgr.Save(pcszFile);
		KG_PROCESS_ERROR(nRetCode);

		g_pUI->m_Script.FireEvent("HOT_KEY_RELOADED", 0);

Exit0:
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_SaveAsAdd(Lua_State *L)
	{
		int nRetCode = false;
		LPCSTR pcszFile = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszFile = Lua_ValueToString(L, 1);
		pcszFile = FormatFilePath(pcszFile);
		KGLOG_PROCESS_ERROR(pcszFile);

		nRetCode = g_pUI->m_HotkeyMgr.SaveAsAdd(pcszFile);
		KG_PROCESS_ERROR(nRetCode);

		g_pUI->m_Script.FireEvent("HOT_KEY_RELOADED", 0);

Exit0:
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_SaveToLuaAsAdd(Lua_State *L)
	{
		int nResult = g_pUI->m_HotkeyMgr.SaveAsAdd(L);

		g_pUI->m_Script.FireEvent("HOT_KEY_RELOADED", 0);
 
		return nResult;
	}

	int KHotkeyClassScriptTable::Lua_LoadDefault(Lua_State *L)
	{
		ASSERT(L);

		g_pUI->m_HotkeyMgr.LoadDefault();
		g_pUI->m_Script.FireEvent("HOT_KEY_RELOADED", 0);
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_GetBinding(Lua_State *L)
	{
		int nDefault = false;

		ASSERT(L);

        if (Lua_GetTopIndex(L) > 0)
		{
			if (Lua_IsBoolean(L, 1))
				nDefault = (int)Lua_ValueToBoolean(L, 1);
			else
				nDefault = (int)Lua_ValueToNumber(L, 1);
		}

		struct KHOTKEYTWO
		{
			KHotkeyMgr::KHOTKEY Hotkey[2];
		};

        typedef std::map<std::string, KHOTKEYTWO> KCOMMANDTOHOTKEY;
		KCOMMANDTOHOTKEY CmdToHotkey;

		if (nDefault)
		{
			LPCSTR pcszFile = g_pUI->m_FilePathMgr.GetFilePath("HotkeyBindingSetPath");
			if (pcszFile && g_IsFileExist(pcszFile))
			{
				ITabFile *pTab = g_OpenTabFile(pcszFile);
				if (pTab)
				{
					char szCommand[32];
					int nRowCount = pTab->GetHeight();
					for (int i = 2; i <= nRowCount; ++i)
					{
						if(pTab->GetString(i, "name", "", szCommand, _countof(szCommand)))
						{
							KHOTKEYTWO Hotkey;
							int nKey = 0;
							int nAdd = false;
							if (pTab->GetInteger(i, "key1", 0, &nKey) && nKey)
							{
								Hotkey.Hotkey[0] = DWORD_TO_KHOTKEY(nKey);
								nAdd = true;
							}
							if (pTab->GetInteger(i, "key2", 0, &nKey) && nKey)
							{
								Hotkey.Hotkey[1] = DWORD_TO_KHOTKEY(nKey);
								nAdd = true;
							}
							if (nAdd)
                            {
								CmdToHotkey[szCommand] = Hotkey;
                            }
						}
					}
				}
				KG_COM_RELEASE(pTab);
			}
		}

		KCOMMANDTOHOTKEY::iterator it;
		int nCount = g_pUI->m_HotkeyMgr.GetBindingCount();
		const KHotkeyMgr::KHOTKEYBINDING *pBinding = NULL;
		Lua_NewTable(L);
		int nTable = Lua_GetTopIndex(L);
		int nIndex = 1;
		for (int i = 0; i < nCount; ++i)
		{
			pBinding = g_pUI->m_HotkeyMgr.GetBinding(i);
			if (pBinding)
			{
				Lua_PushNumber(L, nIndex);
				Lua_NewTable(L);
				int nBinding = Lua_GetTopIndex(L);

				Lua_PushString(L, "szCommand");
				Lua_PushString(L, pBinding->szName);
				Lua_SetTable(L, nBinding);

				Lua_PushString(L, "szDesc");
				Lua_PushString(L, pBinding->szDesc);
				Lua_SetTable(L, nBinding);

				Lua_PushString(L, "szHeader");
				Lua_PushString(L, pBinding->szHeader);
				Lua_SetTable(L, nBinding);

				Lua_PushString(L, "bUnchangeable");
				Lua_PushBoolean(L, pBinding->nUnchangeable);
				Lua_SetTable(L, nBinding);

				Lua_PushString(L, "szTip");
				Lua_PushString(L, pBinding->szTip);
				Lua_SetTable(L, nBinding);

				if (nDefault)
					it = CmdToHotkey.find(pBinding->szName);
				for (int j = 0; j < 2; ++j)
				{
					const KHotkeyMgr::KHOTKEY *pHotkey = NULL;
					if (nDefault)
					{
						if (it != CmdToHotkey.end())
							pHotkey = &(it->second.Hotkey[j]);
					}
					else
						pHotkey = &(pBinding->Hotkey[j]);

					if (pHotkey && pHotkey->uKey)
					{
						if (j == 0)
							Lua_PushString(L, "Hotkey1");
						else
							Lua_PushString(L, "Hotkey2");
						Lua_NewTable(L);
						int nHotkey = Lua_GetTopIndex(L);

						Lua_PushString(L, "nKey");
						Lua_PushNumber(L, pHotkey->uKey);
						Lua_SetTable(L, nHotkey);

						int nValue = false;
						Lua_PushString(L, "bShift");
						if (pHotkey->uModify & UI_HOTKEY_MODIFY_SHIFT)
							nValue = true;
						Lua_PushBoolean(L, nValue);
						Lua_SetTable(L, nHotkey);

						nValue = false;
						Lua_PushString(L, "bCtrl");
						if (pHotkey->uModify & UI_HOTKEY_MODIFY_CTRL)
							nValue = true;
						Lua_PushBoolean(L, nValue);
						Lua_SetTable(L, nHotkey);

						nValue = false;
						Lua_PushString(L, "bAlt");
						if (pHotkey->uModify & UI_HOTKEY_MODIFY_ALT)
							nValue = true;
						Lua_PushBoolean(L, nValue);
						Lua_SetTable(L, nHotkey);

						Lua_SetTable(L, nBinding);
					}
				}
				Lua_SetTable(L, nTable);

				nIndex++;
			}
		}

		return 1;
	}

	int KHotkeyClassScriptTable::Lua_Clear(Lua_State *L)
	{
		ASSERT(L);
		g_pUI->m_HotkeyMgr.Clear();
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_Set(Lua_State *L)
	{
		int nPram = Lua_GetTopIndex(L);
		LPCSTR pcszCommand = NULL;
		int nIndex = 1;
		KHotkeyMgr::KHOTKEY Hotkey;

		ASSERT(L);

        KGLOG_PROCESS_ERROR(nPram > 2);

		pcszCommand = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszCommand);

		nIndex = (int)Lua_ValueToNumber(L, 2);

		Hotkey.uKey = (unsigned short)Lua_ValueToNumber(L, 3);
		KGLOG_PROCESS_ERROR(Hotkey.uKey);

		if (nPram > 3)
		{
			int nShift = false;
			if (Lua_IsBoolean(L, 4))
				nShift = (int)Lua_ValueToBoolean(L, 4);
			else
				nShift = (int)Lua_ValueToNumber(L, 4);
			if (nShift)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_SHIFT;
		}
		if (nPram > 4)
		{
			int nCtrl = false;
			if (Lua_IsBoolean(L, 5))
				nCtrl = (int)Lua_ValueToBoolean(L, 5);
			else
				nCtrl = (int)Lua_ValueToNumber(L, 5);
			if (nCtrl)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_CTRL;
		}
		if (nPram > 5)
		{
			int nAlt = false;
			if (Lua_IsBoolean(L, 6))
				nAlt = (int)Lua_ValueToBoolean(L, 6);
			else
				nAlt = (int)Lua_ValueToNumber(L, 6);
			if (nAlt)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_ALT;
		}

		g_pUI->m_HotkeyMgr.Set(Hotkey, pcszCommand, nIndex);

Exit0:
		return 0;

	}

	int KHotkeyClassScriptTable::Lua_Get(Lua_State *L)
	{
		int nPram = Lua_GetTopIndex(L);
		int nIndex = 1;
		int nKey = 0;
		int nAlt = 0;
		int nShift = 0;
		int nCtrl = 0;
		LPCSTR pcszCommand = NULL;
		KHotkeyMgr::KHOTKEY Hotkey;

		ASSERT(L);

        KGLOG_PROCESS_ERROR(nPram > 0);

		pcszCommand = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszCommand);

		if (nPram > 1)
			nIndex = (int)Lua_ValueToNumber(L, 2);

		Hotkey = g_pUI->m_HotkeyMgr.Get(pcszCommand, nIndex);
		nKey = Hotkey.uKey;
		if (Hotkey.uModify & UI_HOTKEY_MODIFY_SHIFT)
			nShift = true;
		if (Hotkey.uModify & UI_HOTKEY_MODIFY_ALT)
			nAlt = true;
		if (Hotkey.uModify & UI_HOTKEY_MODIFY_CTRL)
			nCtrl = true;
Exit0:
		Lua_PushNumber(L, nKey);
		Lua_PushBoolean(L, nShift);
		Lua_PushBoolean(L, nCtrl);
		Lua_PushBoolean(L, nAlt);
		return 4;
	}

	int KHotkeyClassScriptTable::Lua_IsUsed(Lua_State *L)
	{
		int nPram = Lua_GetTopIndex(L);
		int nUsed = false;
		KHotkeyMgr::KHOTKEY Hotkey;

		ASSERT(L);

        KGLOG_PROCESS_ERROR(nPram > 0);

		Hotkey.uKey = (unsigned short)Lua_ValueToNumber(L, 1);
		KGLOG_PROCESS_ERROR(Hotkey.uKey);

		if (nPram > 1)
		{
			int nShift = false;
			if (Lua_IsBoolean(L, 2))
				nShift = (int)Lua_ValueToBoolean(L, 2);
			else
				nShift =(int)Lua_ValueToNumber(L, 2);
			if (nShift)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_SHIFT;
		}
		if (nPram > 2)
		{
			int nCtrl = false;
			if (Lua_IsBoolean(L, 3))
				nCtrl = (int)Lua_ValueToBoolean(L, 3);
			else
				nCtrl = (int)Lua_ValueToNumber(L, 3);
			if (nCtrl)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_CTRL;
		}
		if (nPram > 3)
		{
			int nAlt = false;
			if (Lua_IsBoolean(L, 4))
				nAlt = (int)Lua_ValueToBoolean(L, 4);
			else
				nAlt = (int)Lua_ValueToNumber(L, 4);
			if (nAlt)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_ALT;
		}

		nUsed = g_pUI->m_HotkeyMgr.IsUsed(Hotkey);

Exit0:
		Lua_PushBoolean(L, nUsed);
		return 1;
	}

	int KHotkeyClassScriptTable::Lua_IsKeyDown(Lua_State *L)
	{
		int nPram = Lua_GetTopIndex(L);
		int nDown = false;
		KHotkeyMgr::KHOTKEY Hotkey;

		ASSERT(L);

        KGLOG_PROCESS_ERROR(nPram > 0);

		Hotkey.uKey = (unsigned short)Lua_ValueToNumber(L, 1);
		KGLOG_PROCESS_ERROR(Hotkey.uKey);

		if (nPram > 1)
		{
			int nShift = false;
			if (Lua_IsBoolean(L, 2))
				nShift = (int)Lua_ValueToBoolean(L, 2);
			else
				nShift = (int)Lua_ValueToNumber(L, 2);
			if (nShift)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_SHIFT;
		}
		if (nPram > 2)
		{
			int nCtrl = false;
			if (Lua_IsBoolean(L, 3))
				nCtrl = (int)Lua_ValueToBoolean(L, 3);
			else
				nCtrl = (int)Lua_ValueToNumber(L, 3);
			if (nCtrl)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_CTRL;
		}
		if (nPram > 3)
		{
			int nAlt = false;
			if (Lua_IsBoolean(L, 4))
				nAlt = (int)Lua_ValueToBoolean(L, 4);
			else
				nAlt = (int)Lua_ValueToNumber(L, 4);
			if (nAlt)
				Hotkey.uModify |= UI_HOTKEY_MODIFY_ALT;
		}

		nDown = g_pUI->m_HotkeyMgr.IsKeyDown(Hotkey);

Exit0:
		Lua_PushBoolean(L, nDown);
		return 1;
	}

	int KHotkeyClassScriptTable::Lua_SetCapture(Lua_State *L)
	{
		ASSERT(L);

		int nCap = false;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		if (Lua_IsBoolean(L, 1))
			nCap = (int)Lua_ValueToBoolean(L, 1);
		else
			nCap = (int)Lua_ValueToNumber(L, 1);

		g_pUI->m_HotkeyMgr.SetCaptureAllHotkey(nCap);

Exit0:
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_AddBinding(Lua_State *L)
	{
		KHotkeyMgr::KHOTKEYBINDING Binding;
		LPCSTR pcszValue = NULL;

        ASSERT(L);

        int nPram = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nPram > 4);

		pcszValue = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszValue && pcszValue[0] != '\0');
		KGLOG_PROCESS_ERROR(strlen(pcszValue) < 32);

		strncpy(Binding.szName, pcszValue, _countof(Binding.szName));
		Binding.szName[_countof(Binding.szName) - 1] = '\0';

		Binding.uNameHash = g_StringHash(Binding.szName);

		pcszValue = Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(pcszValue && pcszValue[0] != '\0');

		strncpy(Binding.szDesc, pcszValue, _countof(Binding.szDesc));
		Binding.szDesc[_countof(Binding.szDesc) - 1] = '\0';

		pcszValue = Lua_ValueToString(L, 3);
		if (pcszValue)
		{
			strncpy(Binding.szHeader, pcszValue, _countof(Binding.szHeader));
		}
		Binding.szHeader[_countof(Binding.szHeader) - 1] = '\0';

		if (lua_isfunction(L, 4))
		{
			lua_pushvalue(L, 4);
			Binding.nScriptDown = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		if (lua_isfunction(L, 5))
		{
			lua_pushvalue(L, 5);
			Binding.nScriptUp = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		if (nPram > 5)
		{
			if (Lua_IsBoolean(L, 6))
				Binding.nUnchangeable = (int)(Lua_ValueToBoolean(L, 6));
			else if (Lua_IsNumber(L, 6))
				Binding.nUnchangeable = (int)(Lua_ValueToNumber(L, 6));
			else
			{
				pcszValue = Lua_ValueToString(L, 6);
				if(pcszValue && pcszValue[0] != '\0')
				{
					strncpy(Binding.szTip, pcszValue, _countof(Binding.szTip));
					Binding.szTip[_countof(Binding.szTip) - 1] = '\0';
				}

				if (nPram > 6)
				{
					if (Lua_IsBoolean(L, 7))
						Binding.nUnchangeable = (int)(Lua_ValueToBoolean(L, 7));
					else
						Binding.nUnchangeable = (int)(Lua_ValueToNumber(L, 7));
				}
			}
		}

		if (!g_pUI->m_HotkeyMgr.AddBinding(Binding))
		{
			g_pUI->m_Script.UnRef(Binding.nScriptDown);
			g_pUI->m_Script.UnRef(Binding.nScriptUp);
		}

Exit0:
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_Enable(Lua_State *L)
	{
		int nEnable = true;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		if (Lua_IsBoolean(L, 1))
			nEnable = (int)Lua_ValueToBoolean(L, 1);
		else
			nEnable = (int)Lua_ValueToNumber(L, 1);

		g_pUI->m_HotkeyMgr.Enable(nEnable);

Exit0:
		return 0;
	}

	int KHotkeyClassScriptTable::Lua_IsEnable(Lua_State *L)
	{
		int nEnable = true;

		ASSERT(L);

		nEnable = g_pUI->m_HotkeyMgr.IsEnable();

		Lua_PushBoolean(L, nEnable);
		return 1;
	}

	int KHotkeyClassScriptTable::Lua_GetCaptureKey(Lua_State *L)
	{
		ASSERT(L);
		int nKey, nShift, nCtrl, nAlt;
		g_pUI->m_HotkeyMgr.GetCaptureKey(nKey, nShift, nCtrl, nAlt);

		Lua_PushNumber(L, nKey);
		Lua_PushBoolean(L, nShift);
		Lua_PushBoolean(L, nCtrl);
		Lua_PushBoolean(L, nAlt);
		return 4;
	}


}

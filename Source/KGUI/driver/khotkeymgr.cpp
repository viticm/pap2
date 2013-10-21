////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : khotkeymgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-8-2 14:19:03
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "khotkeymgr.h"
#include "../elem/wndmessage.h"
#include "../elem/tipcenter.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"


////////////////////////////////////////////////////////////////////////////////

namespace UI
{

KHotkeyMgr::KHotkeyMgr()
{
	m_nHandleAllHotKey = false;
	m_nEnable = true;
}

KHotkeyMgr::~KHotkeyMgr()
{

}

bool KHotkeyMgr::Init()
{
	int nRetCode = false;

	nRetCode = LoadBinding();
	KGLOG_PROCESS_ERROR(nRetCode);

	return true;
Exit0:
	Exit();
	return false;
}

bool KHotkeyMgr::ReInit()
{
	int nRetCode = false;
    
    Clear();

    nRetCode = LoadBinding();
    KGLOG_PROCESS_ERROR(nRetCode);

	return true;
Exit0:
	return false;
}

void KHotkeyMgr::Exit()
{
	Clear();
	UnLoadBinding();
}

int KHotkeyMgr::HandleHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nResult = false;
	int nRetCode = false;
	int nUp = false;
	int nDelta = 0;
	int nUseModify = false;
	int nClick = false;
	KHotkeyMap::iterator it;
	KHOTKEY Hotkey;
	LPCSTR pcszCommand = NULL;
	
	KG_PROCESS_ERROR(m_nEnable);

	switch (uMsg)
	{
	case WM_KEYUP:
	case WM_SYSKEYUP:
		nUp = true;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch (wParam)	//忽略ctrl, shift, alt按键
		{
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU:
		case VK_LCONTROL:
		case VK_RCONTROL:
		case VK_LSHIFT:
		case VK_RSHIFT:
		case VK_LMENU:
		case VK_RMENU:
			goto Exit1;
			break;
		case VK_SNAPSHOT:
			if (nUp)
				HandleHotkey(WM_KEYDOWN, wParam, 0);
			break;
		}
		if (!nUp && (lParam & 0x40000000))
			goto Exit1;
		Hotkey.uKey = (BYTE)(wParam);
		nUseModify = true;
		break;
	case WM_LBUTTONDOWN:
		Hotkey.uKey = VK_LBUTTON;
		break;
	case WM_LBUTTONUP:
		Hotkey.uKey = VK_LBUTTON;
		nUp = true;
		break;
	case WM_LBUTTONDBLCLK: //拆成一个DOWM和UP
		Hotkey.uKey = VK_LBUTTON;
		nClick = true;
		break;
	case WM_RBUTTONDOWN:
		Hotkey.uKey = VK_RBUTTON;
		break;
	case WM_RBUTTONUP:
		Hotkey.uKey = VK_RBUTTON;
		nUp = true;
		break;
	case WM_RBUTTONDBLCLK:	//拆成一个DOWM和UP
		Hotkey.uKey = VK_RBUTTON;
		nClick = true;
		break;
	case WM_MBUTTONDOWN:
		Hotkey.uKey = VK_MBUTTON;
		nUseModify = true;
		break;
	case WM_MBUTTONUP:
		Hotkey.uKey = VK_MBUTTON;
		nUp = true;
		break;
	case WM_MBUTTONDBLCLK: //拆成一个DOWM和UP
		Hotkey.uKey = VK_MBUTTON;
		nClick = true;
		break;
	case WM_XBUTTONDOWN:
		switch (GET_XBUTTON_WPARAM(wParam))
		{
		case XBUTTON1:
			Hotkey.uKey = VK_XBUTTON1;
			nUseModify = true;
			break;
		case XBUTTON2:
			Hotkey.uKey = VK_XBUTTON2;
			nUseModify = true;
			break;
		default:
			goto Exit1;
		}
		break;
	case WM_XBUTTONUP:
		switch (GET_XBUTTON_WPARAM(wParam))
		{
		case XBUTTON1:
			Hotkey.uKey = VK_XBUTTON1;
			nUp = true;
			break;
		case XBUTTON2:
			Hotkey.uKey = VK_XBUTTON2;
			nUp = true;
			break;
		default:
			goto Exit1;
		}
		break;
	case WM_XBUTTONDBLCLK:
		switch (GET_XBUTTON_WPARAM(wParam))
		{
		case XBUTTON1:
			Hotkey.uKey = VK_XBUTTON1;
			nClick = true;
			break;
		case XBUTTON2:
			Hotkey.uKey = VK_XBUTTON2;
			nClick = true;
			break;
		default:
			goto Exit1;
		}
		break;
	case WM_MOUSEWHEEL:	//使用自己扩展的虚拟键值
		nDelta = GET_Y_LPARAM(wParam);
		if (!nDelta)
			goto Exit0;
		if (nDelta > 0)
		{
			Hotkey.uKey = VK_MOUSEWHEELUP;
			nClick = true;
			nUseModify = true;
			break;
		}
		Hotkey.uKey = VK_MOUSEWHEELDOWN;
		nClick = true;
		nUseModify = true;
		break;
	case WM_MOUSEHOVER: //使用自己扩展的虚拟键值
		Hotkey.uKey = VK_MOUSEHOVER;
		break;
	default:
		goto Exit1;
	}

	if (nUp)
	{
		//松开的时候不需要检查ctrl, shift, alt按键，只需要把所有把它作为key的弹起来
		it = m_Hotkey.lower_bound(Hotkey);
		while (it != m_Hotkey.end())
		{
			if ((*it).first.uKey != Hotkey.uKey)
				break;
			
			if ((*it).second.nDown)
			{
				(*it).second.nDown = false;
				ASSERT((*it).second.nCommand >= 0 && (*it).second.nCommand < (int)m_aHotKeyBinding.size());
				g_pUI->m_Script.ExecuteScript(m_aHotKeyBinding[(*it).second.nCommand].nScriptUp);
			}
			++it;
		}

		goto Exit1;
	}

	if (nUseModify)
	{
		if (::GetKeyState((VK_CONTROL)) & 0x8000)
			Hotkey.uModify |= UI_HOTKEY_MODIFY_CTRL;
		if (::GetKeyState((VK_SHIFT)) & 0x8000)
			Hotkey.uModify |= UI_HOTKEY_MODIFY_SHIFT;
		if (::GetKeyState((VK_MENU)) & 0x8000)
			Hotkey.uModify |= UI_HOTKEY_MODIFY_ALT;
	}

	it = m_Hotkey.find(Hotkey);
	KG_PROCESS_ERROR(it != m_Hotkey.end());

	ASSERT((*it).second.nCommand >= 0 && (*it).second.nCommand < (int)m_aHotKeyBinding.size());

	if (nClick)
	{
		(*it).second.nDown = false;
		g_pUI->m_Script.ExecuteScript(m_aHotKeyBinding[(*it).second.nCommand].nScriptDown);
		g_pUI->m_Script.ExecuteScript(m_aHotKeyBinding[(*it).second.nCommand].nScriptUp);
		goto Exit1;
	}

	(*it).second.nDown = true;

	nRetCode = g_pUI->m_Script.ExecuteScript(m_aHotKeyBinding[(*it).second.nCommand].nScriptDown);
	KG_PROCESS_ERROR(nRetCode);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KHotkeyMgr::FlipAllDownHotkey()
{
	KHotkeyMap::iterator it = m_Hotkey.begin();
	KHotkeyMap::iterator ie = m_Hotkey.end();
	for (; it != ie; ++it)
	{
		ASSERT((*it).second.nCommand >= 0 && (*it).second.nCommand < (int)m_aHotKeyBinding.size());
		if ((*it).second.nDown)
		{
			(*it).second.nDown = false;
			g_pUI->m_Script.ExecuteScript(m_aHotKeyBinding[(*it).second.nCommand].nScriptUp);
		}
	}
	return true;
}

int KHotkeyMgr::SetCaptureAllHotkey(int nCapture)
{
	m_nHandleAllHotKey = false;
	if (nCapture)
		m_nHandleAllHotKey = true;
	return true;
}

int KHotkeyMgr::CaptureAllHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nResult = false;
	int nRetCode = false;
	int nCancleSetHotkey = false;
	int nKey = 0;
	int nShift = 0;
	int nAlt = 0;
	int nCtrl = 0;

	KG_PROCESS_ERROR(m_nHandleAllHotKey);

	switch (uMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch (wParam)	//忽略ctrl, shift, alt按键
		{
		case VK_CONTROL:
		case VK_SHIFT:
		case VK_MENU:
		case VK_LCONTROL:
		case VK_RCONTROL:
		case VK_LSHIFT:
		case VK_RSHIFT:
		case VK_LMENU:
		case VK_RMENU:
			goto Exit0;
			break;
		}
		if (lParam & 0x40000000)
			goto Exit0;
		nKey = (BYTE)(wParam);
		break;
	case WM_KEYUP:
		if (wParam == VK_SNAPSHOT)
		{
			CaptureAllHotkey(WM_KEYDOWN, wParam, 0);
			goto Exit0;
		}
		goto Exit0;
	case WM_MBUTTONDOWN:
		nKey = VK_MBUTTON;
		break;
	case WM_XBUTTONDOWN:
		switch (GET_XBUTTON_WPARAM(wParam))
		{
		case XBUTTON1:
			nKey = VK_XBUTTON1;
			break;
		case XBUTTON2:
			nKey = VK_XBUTTON2;
			break;
		default:
			goto Exit0;
		}
		break;
	case WM_MOUSEWHEEL:	
		if (GET_Y_LPARAM(wParam) == 0)
			goto Exit0;
		if (GET_Y_LPARAM(wParam) > 0)
		{
			nKey = VK_MOUSEWHEELUP;
			break;
		}
		nKey = VK_MOUSEWHEELDOWN;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		nCancleSetHotkey = true;
		goto Exit1;
		break;
	default:
		goto Exit0;
	}

	if (::GetKeyState((VK_CONTROL)) & 0x8000)
		nCtrl = 1;
	if (::GetKeyState((VK_SHIFT)) & 0x8000)
		nShift = 1;
	if (::GetKeyState((VK_MENU)) & 0x8000)
		nAlt = 1;

	KG_PROCESS_ERROR(nKey);

Exit1:
	if (nCancleSetHotkey)
	{
		nRetCode = g_pUI->m_Script.FireGlobalEvent("OnCancelHotkeySetting");
		KG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		m_nCaptureKey = nKey;
		m_nCaptureKeyShift = nShift;
		m_nCaptureKeyCtrl = nCtrl;
		m_nCaptureKeyAlt = nAlt;

		nRetCode = g_pUI->m_Script.FireGlobalEvent("OnCaptureHotkey");
		KG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KHotkeyMgr::LoadBinding()
{
	int nResult = false;
	LPCSTR pcszFile = NULL;
	IIniFile *pIni = NULL;
	char szSection[128];
	char szNextSection[128];
    szSection[0] = '\0';
	szNextSection[0] = '\0';

	pcszFile = g_pUI->m_FilePathMgr.GetFilePath("HotkeyBindingsFile");
	KGLOG_PROCESS_ERROR(pcszFile);

	pIni = g_OpenIniFile(pcszFile);
	KGLOG_PROCESS_ERROR(pIni);

	UnLoadBinding();

	while (pIni->GetNextSection(szSection, szNextSection))
	{
		char szScripDown[128];
		char szScriptUp[128];
		szScripDown[0] = '\0';
		szScriptUp[0] = '\0';
		KHOTKEYBINDING item;
		strncpy(item.szName, szNextSection, _countof(item.szName));
		item.szName[_countof(item.szName) - 1] = '\0';
		item.uNameHash = g_StringHash(item.szName);
		pIni->GetString(szNextSection, "desc", "", item.szDesc, _countof(item.szDesc));
		pIni->GetString(szNextSection, "header", "", item.szHeader, _countof(item.szHeader));
		pIni->GetString(szNextSection, "down", "", szScripDown, _countof(szScripDown));
		pIni->GetString(szNextSection, "up", "", szScriptUp, _countof(szScriptUp));
		if (szScripDown[0])
			item.nScriptDown = g_pUI->m_Script.RefBuffer(szScripDown);
		if (szScriptUp[0])
			item.nScriptUp = g_pUI->m_Script.RefBuffer(szScriptUp);

		pIni->GetInteger(szNextSection, "unchangeable", 0, &item.nUnchangeable);
		pIni->GetString(szNextSection, "tip", "", item.szTip, _countof(item.szTip));

		if (!AddBinding(item))
		{
			g_pUI->m_Script.UnRef(item.nScriptDown);
			g_pUI->m_Script.UnRef(item.nScriptUp);
		}

		strcpy(szSection, szNextSection);
	}

	nResult = true;
Exit0:
	KG_COM_RELEASE(pIni);

	return nResult;
}

int KHotkeyMgr::UnLoadBinding()
{
	for (KHotKeyBindingArray::iterator it = m_aHotKeyBinding.begin(); it != m_aHotKeyBinding.end(); ++it)
	{
		g_pUI->m_Script.UnRef(it->nScriptDown);
		g_pUI->m_Script.UnRef(it->nScriptUp);
	}
	m_aHotKeyBinding.clear();
	return TRUE;
}

int KHotkeyMgr::Load(LPCSTR pcszFileName)
{
	int nResult = false;
	int nRowCount = 0;
	ITabFile *pTab = NULL;

	KGLOG_PROCESS_ERROR(pcszFileName);

	nResult = !g_IsFileExist(pcszFileName);
	KG_PROCESS_SUCCESS(nResult);

	pTab = g_OpenTabFile(pcszFileName);
	KG_PROCESS_ERROR(pTab);

	Clear();

	nRowCount = pTab->GetHeight();
	KG_PROCESS_ERROR(nRowCount > 1);

	for (int i = 2; i <= nRowCount; ++i)
	{
		char szCommand[32];
		if (pTab->GetString(i, "name", "", szCommand, _countof(szCommand)))
		{
			int nKey = 0;
			if (pTab->GetInteger(i, "key1", 0, &nKey) && nKey)
				Set(DWORD_TO_KHOTKEY(nKey), szCommand, 1);
			if (pTab->GetInteger(i, "key2", 0, &nKey) && nKey)
				Set(DWORD_TO_KHOTKEY(nKey), szCommand, 2);
		}
	}

Exit1:
	nResult = true;
Exit0:
	KG_COM_RELEASE(pTab);

	return nResult;
}

int KHotkeyMgr::Save(LPCSTR pcszFileName)
{
	int nResult = false;
	int nRow = 2;
	ITabFile *pTab = NULL;

	KGLOG_PROCESS_ERROR(pcszFileName);

	pTab = g_CreateTabFile();
	KG_PROCESS_ERROR(pTab);

	pTab->InsertNewCol("name");
	pTab->InsertNewCol("key1");
	pTab->InsertNewCol("key2");
	for (KHotKeyBindingArray::iterator it = m_aHotKeyBinding.begin(); it != m_aHotKeyBinding.end(); ++it)
	{
		pTab->WriteString(nRow, "name", it->szName);
		if (it->Hotkey[0].uKey)
			pTab->WriteInteger(nRow, "key1", KHOTKEY_TO_DWORD(it->Hotkey[0]));
		else
			pTab->WriteString(nRow, "key1", "");
		if (it->Hotkey[1].uKey)
			pTab->WriteInteger(nRow, "key2", KHOTKEY_TO_DWORD(it->Hotkey[1]));
		else
			pTab->WriteString(nRow, "key2", "");
		nRow++;
	}

	pTab->Save(pcszFileName);

	nResult = true;
Exit0:
	KG_COM_RELEASE(pTab);

	return nResult;
}

int KHotkeyMgr::LoadAsAdd(LPCSTR pcszFileName)
{
	int nResult = false;
	int nRowCount = 0;
	ITabFile *pTab = NULL;

	KGLOG_PROCESS_ERROR(pcszFileName);

	nResult = !g_IsFileExist(pcszFileName);
	KG_PROCESS_SUCCESS(nResult);

	pTab = g_OpenTabFile(pcszFileName);
	KG_PROCESS_ERROR(pTab);

	nRowCount = pTab->GetHeight();
	KG_PROCESS_ERROR(nRowCount > 1);

	for (int i = 2; i <= nRowCount; ++i)
	{
		int nIndex = -1;
		char szCommand[32];
		if (pTab->GetString(i, "name", "", szCommand, _countof(szCommand)) && pTab->GetInteger(i, "index", -1, &nIndex) && nIndex > 0 && nIndex <= 2)
		{
			int nKey = 0;
			pTab->GetInteger(i, "key", 0, &nKey);
			Set(DWORD_TO_KHOTKEY(nKey), szCommand, nIndex);
		}
	}

Exit1:
	nResult = true;
Exit0:
	KG_COM_RELEASE(pTab);

	return nResult;
}

int KHotkeyMgr::SaveAsAdd(LPCSTR pcszFileName)
{
	int nResult = false;
	int nRow = 2;
	ITabFile * pTab = NULL;
	ITabFile * pDefaultTab = NULL;
	const char * pcszDefault = g_pUI->m_FilePathMgr.GetFilePath("HotkeyBindingSetPath");

	KGLOG_PROCESS_ERROR(pcszFileName);
	KGLOG_PROCESS_ERROR(pcszDefault);

	if (pcszDefault && g_IsFileExist(pcszDefault))
		pDefaultTab = g_OpenTabFile(pcszDefault);
	if (!pDefaultTab)
		pDefaultTab = g_CreateTabFile();
	KGLOG_PROCESS_ERROR(pDefaultTab);

	pTab = g_CreateTabFile();
	KG_PROCESS_ERROR(pTab);

	pTab->InsertNewCol("name");
	pTab->InsertNewCol("index");
	pTab->InsertNewCol("key");
	for (KHotKeyBindingArray::iterator it = m_aHotKeyBinding.begin(); it != m_aHotKeyBinding.end(); ++it)
	{
		int nKey = 0;
		if (!(pDefaultTab->GetInteger(it->szName, "key1", 0, &nKey) && DWORD_TO_KHOTKEY(nKey) == it->Hotkey[0]))
		{
			pTab->WriteString(nRow, "name", it->szName);
			pTab->WriteInteger(nRow, "index", 1);
			if (it->Hotkey[0].uKey)
				pTab->WriteInteger(nRow, "key", KHOTKEY_TO_DWORD(it->Hotkey[0]));
			else
				pTab->WriteString(nRow, "key", "");
			nRow++;
		}

		nKey = 0;
		if (!(pDefaultTab->GetInteger(it->szName, "key2", 0, &nKey) && DWORD_TO_KHOTKEY(nKey) == it->Hotkey[1]))
		{
			pTab->WriteString(nRow, "name", it->szName);
			pTab->WriteInteger(nRow, "index", 2);
			if (it->Hotkey[1].uKey)
				pTab->WriteInteger(nRow, "key", KHOTKEY_TO_DWORD(it->Hotkey[1]));
			else
				pTab->WriteString(nRow, "key", "");
			nRow++;
		}
	}

	pTab->Save(pcszFileName);

	nResult = true;
Exit0:
	KG_COM_RELEASE(pTab);
	KG_COM_RELEASE(pDefaultTab);

	return nResult;
}

int KHotkeyMgr::LoadAsAdd(Lua_State* L)
{
	int nParam = lua_gettop(L);
	KG_PROCESS_ERROR(nParam == 1);

	KG_PROCESS_ERROR(lua_istable(L, 1));
	
	lua_pushnil(L);
	while (lua_next(L, -2))
	{
		if (lua_istable(L, -1))
		{
			lua_pushnumber(L, 1);
			lua_gettable(L, -2);
			DWORD dwCommand = (DWORD)(int)lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_pushnumber(L, 2);
			lua_gettable(L, -2);
			DWORD dwValue = (DWORD)(int)lua_tonumber(L, -1);
			lua_pop(L, 1);

			int nIndex = (dwValue & 0xFF000000) >> 24;
			dwValue = dwValue & 0x00FFFFFF;
			if (nIndex >= 1 && nIndex <= 2)
			{
				Set(DWORD_TO_KHOTKEY((int)dwValue), (unsigned int)dwCommand, nIndex);
			}
		}
		lua_pop(L, 1);
	}

Exit0:
	lua_settop(L, nParam);
	return 0;
}

int KHotkeyMgr::SaveAsAdd(Lua_State* L)
{
	ITabFile * pDefaultTab = NULL;
	int nIndex = 1;
	const char * pcszDefault = g_pUI->m_FilePathMgr.GetFilePath("HotkeyBindingSetPath");

	lua_newtable(L);

	KGLOG_PROCESS_ERROR(pcszDefault);

	pDefaultTab = g_OpenTabFile(pcszDefault);
	KGLOG_PROCESS_ERROR(pDefaultTab);

	for (KHotKeyBindingArray::iterator it = m_aHotKeyBinding.begin(); it != m_aHotKeyBinding.end(); ++it)
	{
		int nKey = 0;
		if (!(pDefaultTab->GetInteger(it->szName, "key1", 0, &nKey) && DWORD_TO_KHOTKEY(nKey) == it->Hotkey[0]))
		{
			lua_pushnumber(L, nIndex);
			lua_newtable(L);

			lua_pushnumber(L, 1);
			lua_pushnumber(L, (int)it->uNameHash);
			lua_settable(L, -3);

			lua_pushnumber(L, 2);
			lua_pushnumber(L, (int)(0x01000000 | KHOTKEY_TO_DWORD(it->Hotkey[0])));
			lua_settable(L, -3);

			lua_settable(L, -3);
			++nIndex;
		}

		nKey = 0;
		if (!(pDefaultTab->GetInteger(it->szName, "key2", 0, &nKey) && DWORD_TO_KHOTKEY(nKey) == it->Hotkey[1]))
		{
			lua_pushnumber(L, nIndex);
			lua_newtable(L);

			lua_pushnumber(L, 1);
			lua_pushnumber(L, (int)it->uNameHash);
			lua_settable(L, -3);

			lua_pushnumber(L, 2);
			lua_pushnumber(L, (int)(0x02000000 | KHOTKEY_TO_DWORD(it->Hotkey[1])));
			lua_settable(L, -3);

			lua_settable(L, -3);
			++nIndex;
		}
	}

Exit0:
	KG_COM_RELEASE(pDefaultTab);
	return 1;
}

int KHotkeyMgr::Clear()
{
	FlipAllDownHotkey();
	m_Hotkey.clear();
	for (KHotKeyBindingArray::iterator it = m_aHotKeyBinding.begin(); it != m_aHotKeyBinding.end(); ++it)
	{
		it->Hotkey[0].uKey = 0;
		it->Hotkey[0].uModify = 0;
		it->Hotkey[1].uKey = 0;
		it->Hotkey[1].uModify = 0;
	}
	return true;
}

int KHotkeyMgr::LoadDefault()
{
	int nResult = false;
	int nRetCode = false;
	LPCSTR pcszFile = NULL;
	
	pcszFile = g_pUI->m_FilePathMgr.GetFilePath("HotkeyBindingSetPath");
	KGLOG_PROCESS_ERROR(pcszFile);

	nRetCode = Load(pcszFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KHotkeyMgr::Set(KHOTKEY Hotkey, LPCSTR pcszCommand, int nIndex)
{
	int nResult = false;
	KHOTKEYINFO HotkeyInfo;
	KHotKeyBindingArray::iterator itBind;

	--nIndex;

	KG_PROCESS_ERROR(pcszCommand && pcszCommand[0] != '\0');

	itBind = std::find(m_aHotKeyBinding.begin(), m_aHotKeyBinding.end(), pcszCommand);
	if (itBind != m_aHotKeyBinding.end())
	{
		if (nIndex >= 0 && nIndex < 2)
		{
			m_Hotkey.erase(itBind->Hotkey[nIndex]);
		}

		HotkeyInfo.nCommand = (int)std::distance(m_aHotKeyBinding.begin(), itBind);
		KHotkeyMap::iterator it = m_Hotkey.find(Hotkey);
		if (it == m_Hotkey.end())
		{
			m_Hotkey.insert(make_pair(Hotkey, HotkeyInfo));
		}
		else
		{
			if (it->second.nDown)
			{
				it->second.nDown = false;
				g_pUI->m_Script.ExecuteScript(m_aHotKeyBinding[(*it).second.nCommand].nScriptUp);
			}
			if (m_aHotKeyBinding[it->second.nCommand].Hotkey[0] == Hotkey)
			{
				m_aHotKeyBinding[it->second.nCommand].Hotkey[0].uKey = 0;
				m_aHotKeyBinding[it->second.nCommand].Hotkey[0].uModify= 0;
			}
			if (m_aHotKeyBinding[it->second.nCommand].Hotkey[1] == Hotkey)
			{
				m_aHotKeyBinding[it->second.nCommand].Hotkey[1].uKey = 0;
				m_aHotKeyBinding[it->second.nCommand].Hotkey[1].uModify= 0;
			}
			it->second = HotkeyInfo;
		}

		itBind->Hotkey[nIndex].uKey = Hotkey.uKey;
		itBind->Hotkey[nIndex].uModify = Hotkey.uModify;

	}
	else
	{
		//KGLogPrintf(KGLOG_DEBUG, "KGUI HotKey command [%s] was not exist!\n", pcszCommand);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KHotkeyMgr::Set(KHOTKEY Hotkey, unsigned int uCommand, int nIndex)
{
	KHOTKEYINFO HotkeyInfo;
	KHotKeyBindingArray::iterator itBind;

	--nIndex;

	itBind = std::find(m_aHotKeyBinding.begin(), m_aHotKeyBinding.end(), uCommand);
	if (itBind != m_aHotKeyBinding.end())
	{
		if (nIndex >= 0 && nIndex < 2)
		{
			m_Hotkey.erase(itBind->Hotkey[nIndex]);
		}

		HotkeyInfo.nCommand = (int)std::distance(m_aHotKeyBinding.begin(), itBind);
		KHotkeyMap::iterator it = m_Hotkey.find(Hotkey);
		if (it == m_Hotkey.end())
		{
			m_Hotkey.insert(make_pair(Hotkey, HotkeyInfo));
		}
		else
		{
			if (it->second.nDown)
			{
				it->second.nDown = false;
				g_pUI->m_Script.ExecuteScript(m_aHotKeyBinding[(*it).second.nCommand].nScriptUp);
			}
			if (m_aHotKeyBinding[it->second.nCommand].Hotkey[0] == Hotkey)
			{
				m_aHotKeyBinding[it->second.nCommand].Hotkey[0].uKey = 0;
				m_aHotKeyBinding[it->second.nCommand].Hotkey[0].uModify= 0;
			}
			if (m_aHotKeyBinding[it->second.nCommand].Hotkey[1] == Hotkey)
			{
				m_aHotKeyBinding[it->second.nCommand].Hotkey[1].uKey = 0;
				m_aHotKeyBinding[it->second.nCommand].Hotkey[1].uModify= 0;
			}
			it->second = HotkeyInfo;
		}

		itBind->Hotkey[nIndex].uKey = Hotkey.uKey;
		itBind->Hotkey[nIndex].uModify = Hotkey.uModify;

	}

	return true;
}

KHotkeyMgr::KHOTKEY KHotkeyMgr::Get(LPCSTR pcszCommand, int nIndex)
{
	KHOTKEY Hotkey;
	--nIndex;
	KHotKeyBindingArray::iterator it = std::find(m_aHotKeyBinding.begin(), m_aHotKeyBinding.end(), pcszCommand);
	if (it != m_aHotKeyBinding.end() && nIndex >= 0 && nIndex < 2)
	{
		Hotkey.uKey = it->Hotkey[nIndex].uKey;
		Hotkey.uModify = it->Hotkey[nIndex].uModify;
	}
	return Hotkey;
}

int KHotkeyMgr::IsUsed(KHOTKEY Hotkey)
{
	if (m_Hotkey.find(Hotkey) == m_Hotkey.end())
		return false;
	return true;
}

int KHotkeyMgr::Remove(KHOTKEY Hotkey)
{
	int nResult = false;
	KHotkeyMap::iterator it = m_Hotkey.find(Hotkey);
	KG_PROCESS_ERROR(it != m_Hotkey.end());

	if ((*it).second.nDown)
	{
		(*it).second.nDown = false;
		g_pUI->m_Script.ExecuteScript(m_aHotKeyBinding[(*it).second.nCommand].nScriptUp);
	}

	m_Hotkey.erase(it);

	nResult = true;
Exit0:
	return nResult;
}

int KHotkeyMgr::IsKeyDown(KHOTKEY Hotkey)
{
	int nResult = false;

	if (Hotkey.uModify & UI_HOTKEY_MODIFY_CTRL)
		KG_PROCESS_ERROR(::GetKeyState(VK_CONTROL) & 0x8000);
	if (Hotkey.uModify & UI_HOTKEY_MODIFY_SHIFT)
		KG_PROCESS_ERROR(::GetKeyState(VK_SHIFT) & 0x8000);
	if (Hotkey.uModify & UI_HOTKEY_MODIFY_ALT)
		KG_PROCESS_ERROR(::GetKeyState(VK_MENU) & 0x8000);
	if (Hotkey.uKey)
		KG_PROCESS_ERROR(::GetKeyState(Hotkey.uKey) & 0x8000);

	nResult = true;
Exit0:
	return nResult;
}

int KHotkeyMgr::AddBinding(const KHOTKEYBINDING &Binding)
{
	if (std::find(m_aHotKeyBinding.begin(), m_aHotKeyBinding.end(), Binding.uNameHash) != m_aHotKeyBinding.end())
	{
		KGLogPrintf(KGLOG_DEBUG, "KGUI %s%s%s\n", "HotKeyBinding:", Binding.szName, " name hash is conflict or name is aleady used, please changed a name!");
		return false;
	}

	m_aHotKeyBinding.push_back(Binding);
	return true;
}

int KHotkeyMgr::GetBindingCount()
{
	return (int)(m_aHotKeyBinding.size());
}

const KHotkeyMgr::KHOTKEYBINDING * KHotkeyMgr::GetBinding(int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)(m_aHotKeyBinding.size()))
	{
		return &m_aHotKeyBinding[nIndex];
	}
	return NULL;
		
}

int KHotkeyMgr::Enable(int nEnable)
{
	if ((m_nEnable && !nEnable) || (!m_nEnable && nEnable))
	{
		FlipAllDownHotkey();
	}
	m_nEnable = nEnable;
	return true;
}

int KHotkeyMgr::IsEnable()
{
	return m_nEnable;
}

int KHotkeyMgr::GetCaptureKey(int &nKey, int &nShift, int &nCtrl, int &nAlt)
{
	nKey = m_nCaptureKey;
	nShift = m_nCaptureKeyShift;
	nCtrl = m_nCaptureKeyCtrl;
	nAlt = m_nCaptureKeyAlt;

	return true;
}



}
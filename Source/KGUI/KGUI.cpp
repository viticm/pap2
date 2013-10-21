////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGUI.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-11-29 10:09:12
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SO3UI/IKGUI.h"
#include "SO3World/KScriptFuncList.h"
#include "./KGUI.h"
#include "./script/kscriptmgr.h"
#include "./script/kscriptloader.h"
#include "./config/kdebugconfig.h"
#include "./script/kbasescripttable.h"
#include "./script/khotkeyclassscripttable.h"
#include "./script/kschemescripttable.h"
#include "./script/kwndclassscripttable.h"
#include "./script/ksystemscripttable.h"
#include "./script/kconstmgr.h"
#include "./script/kscriptloader.h"
#include "./script/kmusicscripttsble.h"
#include "./script/kfileclassscripttable.h"
#include "./script/kitemhandleclassscripttable.h"
#include "./elem/wndedit.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

	KGUI* g_pUI = NULL;

	LRESULT KGUI::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return m_Input.HandleInput(uMsg, wParam, lParam);
	}

    void KGUI::Active()
    {
        m_AnimateImageMgr.Activate();
        m_MusicMgr.Activate();

        m_WndStation.Breathe();

        m_Script.Activate();
    }

	void KGUI::Paint()
	{
		KG_PROCESS_SUCCESS(!KDebugConfig::ms_aBoolFlag[KDebugConfig::BOOL_ENABLE_RENDER_UI]);
        
        g_pUI->m_WndStation.Paint();

Exit1:
        return;
	}

	int KGUI::Init(const KGUI_INIT_PARAM *pParam)
	{
		int nResult = false;
		int nRetCode = false;
		int nValue = 0;
		int nWidth = 1280;
		int nHeight = 960;
		int nFullScreen = true;
		int nMaximize = false;
		int nPanauision = false;
		int nRefreshRate = 0;
		int nExclusiveMode = false;
        int nTripleBuffering = false;
        int nLockableBackBuffer = false;
        int nVSync = false;
        int nMultiSampleType = 0;
        int nMultiSampleQuality = 0;
		int nX = 0;
		int nY = 0;
		HRESULT hr = E_FAIL;
		IIniFile* pIni = NULL;
		HWND hWnd = NULL;
		LPCSTR szFilePath = NULL;
        KG3DDeviceSettings WndVideoSettings;

		m_bReInit = false;
		m_nInitReason = START_GAME_LOGIN;

		KGLOG_PROCESS_ERROR(pParam && "KGUI::Init error: pParam is null!");
		KGLOG_PROCESS_ERROR(pParam->p3DUI);
		KGLOG_PROCESS_ERROR(pParam->p3DEngineManager);
		KGLOG_PROCESS_ERROR(pParam->p3DModelManager);
		KGLOG_PROCESS_ERROR(pParam->hInst);
		KGLOG_PROCESS_ERROR(pParam->hWnd);

		m_p3DUI = pParam->p3DUI;
		m_p3DEngineManager = pParam->p3DEngineManager;
		m_p3DModelManager = pParam->p3DModelManager;
		m_hInst = pParam->hInst;
		m_hWnd = pParam->hWnd;
        m_phwndEmbedWebPage = pParam->phwndEmbedWebPage;
		m_pSoundShellHolder = pParam->pSoundShellHolder;

        nRetCode = SetCodePage(pParam->uCodePage);
        KGLOG_PROCESS_ERROR(nRetCode);

        strncpy_s(m_szKLVideoDllPath, _countof(m_szKLVideoDllPath), pParam->szKLVideoDllPath, _TRUNCATE);
        strncpy_s(m_szConfigPath, _countof(m_szConfigPath), pParam->szConfigPath, _TRUNCATE);
        strncpy_s(m_szVersionPath, _countof(m_szVersionPath), pParam->szVersionPath, _TRUNCATE);

        pIni = g_OpenIniFile(m_szConfigPath);
        if(pIni)
        {
            pIni->GetInteger("Main", "CanvasWidth", nWidth, &nWidth);
            pIni->GetInteger("Main", "CanvasHeight", nHeight, &nHeight);
            pIni->GetInteger("Main", "Maximize", nMaximize, &nMaximize);
            pIni->GetInteger("Main", "FullScreen", nFullScreen, &nFullScreen);
            pIni->GetInteger("Main", "Panauision", nPanauision, &nPanauision);
            pIni->GetInteger("Main", "RefreshRate", nRefreshRate, &nRefreshRate);
            pIni->GetInteger("Main", "ExclusiveMode", nExclusiveMode, &nExclusiveMode);
            pIni->GetInteger("KG3DENGINE", "TripleBuffering", nTripleBuffering, &nTripleBuffering);
            pIni->GetInteger("KG3DENGINE", "LockableBackBuffer", nLockableBackBuffer, &nLockableBackBuffer);
            pIni->GetInteger("KG3DENGINE", "VSync", nVSync, &nVSync);
            pIni->GetInteger("KG3DENGINE", "MultiSampleType", nMultiSampleType, &nMultiSampleType);
            pIni->GetInteger("KG3DENGINE", "MultiSampleQuality", nMultiSampleQuality, &nMultiSampleQuality);
            pIni->GetInteger("Main", "X", 0, &nX);
            pIni->GetInteger("Main", "Y", 0, &nY);

            if (pIni->GetInteger("Log", "Level", 0, &nValue) && nValue < KGLOG_PRIORITY_MAX)
                KGLogSetPriorityMask(nValue);

            SAFE_RELEASE(pIni);
        }

		hr = m_p3DUI->RegisterSliceImageSize(48, 48);
		KGLOG_COM_PROCESS_ERROR(hr);

		nRetCode = KBlockPool::Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_Config.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_FilePathMgr.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_ImageInfoMgr.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_AnimateImageMgr.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_Input.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_Pointer.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_ColorSchemeMgr.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_FontSchemeMgr.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		// Icon
		{
			szFilePath = m_FilePathMgr.GetFilePath("IconImageTable");
			KGLOG_PROCESS_ERROR(szFilePath);

			nRetCode = m_IconModelTab.Load(szFilePath, "isissss");
			KGLOG_PROCESS_ERROR(nRetCode);

			m_pIconModel = (KIconModel*)m_IconModelTab.Data(m_nIconModelCount);
			KGLOG_PROCESS_ERROR(m_pIconModel);
		}

		nRetCode = m_Script.Init();
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = LoadAllScriptTable();
        KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_WndStation.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_HotkeyMgr.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = KUiConfig::Init();
		KGLOG_PROCESS_ERROR(nRetCode);

        if (nWidth < 640)
            nWidth = 640;
        if (nHeight < 480)
            nHeight = 480;

		if (!nExclusiveMode)
		{
			MONITORINFO Info;
			POINT Pos;
			int nScreenX = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
			int nScreenY = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
			int nScreenW = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
			int nScreenH = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

			if (nX + nWidth > nScreenX + nScreenW)
				nX = nScreenX + nScreenW - nWidth;
			if (nY + nHeight > nScreenY + nScreenH)
				nY = nScreenY + nScreenH - nHeight;
			if (nX < nScreenX)
				nX = nScreenX;
			if (nY < nScreenY)
				nY = nScreenY;
			Pos.x = nX;
			Pos.y = nY;
			Info.cbSize = sizeof(MONITORINFO);
			HMONITOR hM = ::MonitorFromPoint(Pos, MONITOR_DEFAULTTONEAREST);
			if (hM && ::GetMonitorInfo(hM, &Info))
			{
				if (nX < Info.rcMonitor.left)
					nX = Info.rcMonitor.left;
				if (nY < Info.rcMonitor.top)
					nY = Info.rcMonitor.top;
			}
            nRetCode = ::SetWindowPos(m_hWnd, NULL, nX, nY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			KGLOG_CHECK_ERROR(nRetCode);
        }

        WndVideoSettings.uWidth = (UINT)nWidth;
        WndVideoSettings.uHeight = (UINT)nHeight;
        WndVideoSettings.bFullScreen = nFullScreen;
        WndVideoSettings.bPanauision = nPanauision;
		WndVideoSettings.bMaximize = nMaximize;
        WndVideoSettings.bExclusiveMode = nExclusiveMode;
        WndVideoSettings.bTripleBuffering  = nTripleBuffering;
        WndVideoSettings.bLockableBackBuffer = nLockableBackBuffer;
        WndVideoSettings.bVSync = nVSync;
        WndVideoSettings.uRefreshRate = (UINT)nRefreshRate;
        WndVideoSettings.uColorDepth = 32;
        WndVideoSettings.nMultiSampleType = nMultiSampleType;
        WndVideoSettings.dwMultiSampleQuality = nMultiSampleQuality;

		nRetCode = m_WndStation.SetVideoSettings(&WndVideoSettings);
        KGLOG_PROCESS_ERROR(nRetCode);

		m_WndStation.Enable(true);

		m_EditorMgr.OpenUIEditor(false);

		nResult = true;
Exit0:
		SAFE_RELEASE(pIni);
		return nResult;
	}

	int KGUI::NeedReset()
	{
		return m_bReInit;
	}

	void KGUI::Reset()
	{
		int nRetCode = false;

        m_Script.FireEvent("PLAYER_EXIT_GAME", 0);
        m_Script.FireEvent("UI_LUA_RESET", 0);

		m_Pointer.SwitchCursor(CURSOR_NORMAL);
		m_Pointer.ShowCursor(true);

		m_FontSchemeMgr.SetOffset(0);

		m_HotkeyMgr.Exit();

		nRetCode = m_MusicMgr.ReInit();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_WndStation.ReInit();
		KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_Script.ReInit();
        KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = m_HotkeyMgr.Init();
		KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = LoadAllScriptTable();
        KGLOG_PROCESS_ERROR(nRetCode);

		return;
Exit0:
		return;
	}

	void KGUI::SetSoundShell(IKG3DSoundShell *pShell)
	{
		m_MusicMgr.SetSoundShell(pShell);
	}

	void KGUI::Run()
	{
		m_Script.LoadLoginScriptLib();
		m_Script.FireEvent("GAME_START", 0);
		m_Script.FireGlobalEvent("LoadLoginUI");

		m_bReInit = false;
	}

	void KGUI::Exit()
	{
		m_Script.FireEvent("GAME_EXIT", 0);

		m_EditorMgr.CLoseUIEditor();

		KUiConfig::Exit();

		m_HotkeyMgr.Exit();
		m_WndStation.Exit();
        m_Script.Exit();
		m_MusicMgr.Exit();
		m_FontSchemeMgr.Exit();
		m_ColorSchemeMgr.Exit();
		m_Pointer.Exit();
		m_Input.Exit();
		m_AnimateImageMgr.Exit();
		m_ImageInfoMgr.Exit();
		m_FilePathMgr.Exit();
		m_Config.Exit();
		KBlockPool::Exit();

		m_p3DUI = NULL;
		m_p3DEngineManager = NULL;
		m_p3DModelManager = NULL;
		m_hInst = NULL;
		m_hWnd = NULL;	

		delete this;
	}

	lua_State* KGUI::GetLuaState()
	{
		return m_Script.GetLuaState();
	}

	void KGUI::RegisterMetaTable(LPCSTR pszTable, luaL_reg const* pMetaFunc)
	{
		KGLOG_PROCESS_ERROR(pszTable);
		KGLOG_PROCESS_ERROR(pMetaFunc);
		KScriptLoader::RegisterMetaTable(GetLuaState(), pszTable, pMetaFunc);
Exit0:
		return;
	}

	void KGUI::RegisterLibTable(LPCSTR pszTable, luaL_reg const* pLibFunc)
	{
		KGLOG_PROCESS_ERROR(pszTable);
		KGLOG_PROCESS_ERROR(pLibFunc);
		KScriptLoader::RegisterLibTable(GetLuaState(), pszTable, pLibFunc);
Exit0:
		return;
	}

	void KGUI::RegisterFunctions(luaL_reg const* pFunc)
	{
		KGLOG_PROCESS_ERROR(pFunc);
		KScriptLoader::RegisterFunctions(GetLuaState(), pFunc);
Exit0:
		return;
	}

	void KGUI::RegisterFunctions(KLuaFunc const* pFunc, int nCount)
	{
		KGLOG_PROCESS_ERROR(pFunc);
		KScriptLoader::RegisterFunctions(GetLuaState(), pFunc, nCount);
Exit0:
		return;
	}

	void KGUI::RegisterConstList(KLuaConstList const* pConstList)
	{
		KGLOG_PROCESS_ERROR(pConstList);
		KScriptLoader::RegisterConstList(GetLuaState(), pConstList);
Exit0:
		return;
	}

	BOOL KGUI::GetPassword(LPCSTR szTreePath, char szDestMD5String[64])
	{
		BOOL bReturn = FALSE;
		int nRetCode = FALSE;
		KWndWindow * pWnd = NULL;
		KWndEdit * pWndEdit = NULL;

		KG_PROCESS_ERROR(szTreePath);
		KG_PROCESS_ERROR(szDestMD5String);

		pWnd = g_pUI->m_WndStation.SearchWindow(szTreePath);
		KG_PROCESS_ERROR(pWnd);

		pWndEdit = (KWndEdit*)CastWndWindow(pWnd, "WndEdit");
		KG_PROCESS_ERROR(pWndEdit);

		nRetCode = pWndEdit->GetPassword(szDestMD5String);
		KG_PROCESS_ERROR(nRetCode);

		bReturn = TRUE;
Exit0:
		return bReturn;
	}

	void KGUI::PushArg(LPCSTR szName, bool bParam)
	{
		lua_State* L = m_Script.GetLuaState();

		lua_pushstring(L, szName);
		lua_pushboolean(L, bParam);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	void KGUI::PushArg(LPCSTR szName, int nParam)
	{
		PushArg(szName, (double)nParam);
	}
	void KGUI::PushArg(LPCSTR szName, DWORD dwParam)
	{
		PushArg(szName, (double)(int)dwParam);
	}

	void KGUI::PushArg(LPCSTR szName, float fParam)
	{
		PushArg(szName, (double)fParam);
	}

	void KGUI::PushArg(LPCSTR szName, double dblParam)
	{
		lua_State* L = m_Script.GetLuaState();

		lua_pushstring(L, szName);
		lua_pushnumber(L, dblParam);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	void KGUI::PushArg(LPCSTR szName, LPCSTR szParam)
	{
		lua_State* L = m_Script.GetLuaState();

		lua_pushstring(L, szName);
		lua_pushstring(L, szParam);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	void KGUI::FireEvent(LPCSTR szEvent, DWORD dwCookie)
	{
		m_Script.FireEvent(szEvent, dwCookie);
	}

    int KGUI::SetCodePage(unsigned int uCodePage)
    {
        return ::SetCodePage(uCodePage);
    }

    unsigned int KGUI::GetCodePage()
    {
        return ::GetCodePage();
    }

	void KGUI::ReInit(int nReason)
	{
		m_bReInit = true;
		m_nInitReason = nReason;
	}

	int KGUI::GetInitReason()
	{
		return m_nInitReason;
	}

    BOOL KGUI::LoadAllScriptTable()
    {
        BOOL bRetCode = FALSE;
        lua_State* L = NULL;

        L = m_Script.GetLuaState();
        KGLOG_PROCESS_ERROR(L);

        KBaseScriptTable::Load(L);
        KHotkeyClassScriptTable::Load(L);
        KSchemeScriptTable::Load(L);
        KItemHandleClassScriptTable::Load(L);
        KWndClassScriptTable::Load(L);
        KSystemScriptTable::Load(L);
        KMusicScriptTable::Load(L);
        KFileClassScriptTable::Load(L);

        return TRUE;
Exit0:
        return FALSE;
    }

	KIconModel const* KGUI::GetIconModel(DWORD dwID) const
	{
		KIconModel Found = { dwID };

		KIconModel const* pResult = std::lower_bound(m_pIconModel, m_pIconModel + m_nIconModelCount, Found);
		if (pResult != m_pIconModel + m_nIconModelCount && pResult->dwID == dwID)
			return pResult;

		m_FilePathMgr.TabError("GetIconModel(%d) [%s]\n", dwID, m_FilePathMgr.GetFilePath("IconImageTable"));
		return NULL;
	}


	LPCSTR GetIconFullPath(LPCSTR szFilePath)
	{
		static char s_szFilePath[MAX_PATH];

		ASSERT(szFilePath);

		s_szFilePath[0] = '\0';

		LPCSTR szFilePathIconImageDir = g_pUI->m_FilePathMgr.GetFilePath("IconImageDir");
		if (szFilePathIconImageDir)
		{
			_snprintf(s_szFilePath, _countof(s_szFilePath), "%s\\%s", szFilePathIconImageDir, szFilePath);
			s_szFilePath[_countof(s_szFilePath) - 1] = '\0';
		}

		return s_szFilePath;
	}

}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    int nRetCode = false;
    int nResult = false;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
        break;
	case DLL_PROCESS_DETACH:
		break;
	}

	nResult = true;
    return nResult;
}

extern "C" __declspec(dllexport) IKGUI* CreateKGUI()
{
	UI::g_pUI = new(std::nothrow) UI::KGUI;
	return UI::g_pUI;
}

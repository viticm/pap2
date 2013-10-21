#include "stdafx.h"
#include "./ksystemscripttable.h"
#include "./kscriptloader.h"
#include "../config/kdebugconfig.h"
#include "../KGUI.h"

namespace UI
{
	void KSystemScriptTable::Load(lua_State* L)
	{
		static KLuaFunc s_aGlobalFunctionTable[] =
		{
            { "FormatTime",             LuaFormatTime },
			{ "GetTime",				LuaGetTime },
			{ "GetTickCount",			LuaGetTickCount },
			{ "GetFPS",					LuaGetFPS },
			{ "GetVideoMemeryInfo",		LuaGetVideoMemeryInfo },
			{ "GetUITextureUsage",		LuaGetUITextureUsage },
			{ "OpenURL",				LuaOpenURL },

			{ "RemoveUserdata",		LuaRemoveUserdata },
			{ "SetUserAccount",		LuaSetUserAccount },
			{ "SetUserServer",		LuaSetUserServer },
			{ "SetUserRoleName",	LuaSetUserRoleName },
			{ "GetUserAccount",		LuaGetUserAccount },
			{ "GetUserServer",		LuaGetUserServer },
			{ "GetUserRoleName",	LuaGetUserRoleName },
			{ "GetUserDataFloder",	LuaGetUserDataFloder },
			{ "GetUserDataPath",	LuaGetUserDataPath },
			{ "SaveUserData",		LuaSaveUserData },
			{ "LoadUserData",		LuaLoadUserData },
			{ "IsUserDataExist",	LuaIsUserDataExist },

			{ "LoadDataFromFile",   LuaLoadDataFromFile },
			{ "SaveDataToFile",     LuaSaveDataToFile },

			{ "MovieRecord",		LuaMovieRecord },
			{ "MovieStop",			LuaMovieStop },
			{ "IsMovieRecord",		LuaIsMovieRecord },
			
            { "CanRecord",          LuaCanRecord },
            { "IsRecording",        LuaIsRecording },
            { "StartRecord",        LuaStartRecord },
            { "StopRecord",         LuaStopRecord },

			{ "GetAllowableAdapterModes",		LuaGetAllowableAdapterModes },
			{ "GetCurrentAdapterMode",			LuaGetCurrentAdapterMode },
			
			{ "EatRAM",							LuaEatRAM },
			{ "EatVRAM",						LuaEatVRAM },
			{ "IsMouseButtonSwaped",			LuaIsMouseButtonSwaped },
			{ "SwapMouseButton",				LuaSwapMouseButton },

			{ "GetOpenFileName",				LuaGetOpenFileName },

			{ "GetUnpakFileSize",				LuaGetUnpakFileSize },
			{ "IsUnpakFileExist",				LuaIsUnpakFileExist },

            { "AllocConsole", LuaAllocConsole },
            { "FreeConsole", LuaFreeConsole },

            { "ClearGlobalString", LuaClearGlobalString },
            { "RegisterGlobalString", LuaRegisterGlobalString },
		};

		KScriptLoader::RegisterFunctions(L, s_aGlobalFunctionTable, _countof(s_aGlobalFunctionTable));
	}

    int KSystemScriptTable::LuaFormatTime(Lua_State* L)
    {
        size_t nRetCode = 0;

        char szDest[128];
        time_t nTime = 0;
        LPCSTR pcszFormat = NULL;
        struct tm *ptmTime = NULL;

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

        pcszFormat = lua_tostring(L, 1);
        KGLOG_PROCESS_ERROR(pcszFormat);
		KGLOG_PROCESS_ERROR(pcszFormat[0]);

        nTime = (time_t)lua_tonumber(L, 2);

        ptmTime = localtime(&nTime);
		KGLOG_PROCESS_ERROR(ptmTime);

        nRetCode = strftime(szDest, sizeof(szDest), pcszFormat, ptmTime);
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        Lua_PushString(L, szDest);
        return 1;
Exit0:
        return 0;
    }

	int KSystemScriptTable::LuaGetTime(Lua_State* L)
	{
		Lua_PushNumber(L, KG_GetTickCount());
		return 1;
	}

	int KSystemScriptTable::LuaGetTickCount(Lua_State* L)
	{
		Lua_PushNumber(L, KG_GetTickCount());
		return 1;
	}

	int KSystemScriptTable::LuaGetFPS(Lua_State* L)
	{
		Lua_PushNumber(L, g_pUI->m_WndStation.GetFPS());
		return 1;
	}

	int KSystemScriptTable::LuaGetVideoMemeryInfo(Lua_State* L)
	{
		ASSERT(L);

		VideoMemoryInfo Info = g_pUI->m_p3DUI->GetVideoMemeryInfo();

		lua_pushnumber(L, Info.dwLocalTatal);
		lua_pushnumber(L, Info.dwLocalFree);
		lua_pushnumber(L, Info.dwNonLocalTatal);
		lua_pushnumber(L, Info.dwNonLocalFree);
		lua_pushnumber(L, Info.dwTextureTatal);
		lua_pushnumber(L, Info.dwTexturnFree);
		lua_pushnumber(L, Info.dwD3DTotal);
		lua_pushnumber(L, Info.dwD3DFree);
		return 8;
	}

	int KSystemScriptTable::LuaGetUITextureUsage(Lua_State* L)
	{
		ASSERT(L);

		KG3DUITextureUsage Usage;

		g_pUI->m_p3DUI->GetUITextureUsage(Usage);

		lua_pushnumber(L, Usage.uMemoryUsage);
		lua_pushnumber(L, Usage.uTextureUsage);
		lua_pushnumber(L, Usage.uSliceUsage);
		return 3;
	}


	int KSystemScriptTable::LuaOpenURL(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szURL = NULL;
		KUrl url;

		ASSERT(L);

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		szURL = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(szURL);

		url.Open(szURL);

		return 0;
Exit0:
		return 0;
	}

	int KSystemScriptTable::LuaRemoveUserdata(Lua_State* L)
	{
		KGLOG_PROCESS_ERROR(lua_gettop(L) == 0);

		KUiConfig::GetSelf().RemoveUserdata();
Exit0:
		return 0;
	}

	int KSystemScriptTable::LuaSetUserAccount(Lua_State* L)
	{
		LPCSTR pcszAccount = NULL;

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszAccount = lua_tostring(L, 1);
		KG_PROCESS_ERROR(pcszAccount && pcszAccount[0]);

		KUiConfig::GetSelf().SetUserAccount(pcszAccount);
Exit0:
		return 0;
	}

	int KSystemScriptTable::LuaSetUserServer(Lua_State* L)
	{
		LPCSTR pcszRegion = NULL;
		LPCSTR pcszServer = NULL;

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pcszRegion = lua_tostring(L, 1);
		KG_PROCESS_ERROR(pcszRegion && pcszRegion[0]);

		pcszServer = lua_tostring(L, 2);
		KG_PROCESS_ERROR(pcszServer && pcszServer[0]);

		KUiConfig::GetSelf().SetUserServer(pcszRegion, pcszServer);
Exit0:
		return 0;
	}

	int KSystemScriptTable::LuaSetUserRoleName(Lua_State* L)
	{
		LPCSTR pcszRoleName = NULL;

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszRoleName = lua_tostring(L, 1);
		KG_PROCESS_ERROR(pcszRoleName && pcszRoleName[0]);

		KUiConfig::GetSelf().SetUserRoleName(pcszRoleName);
Exit0:
		return 0;
	}

	int KSystemScriptTable::LuaGetUserAccount(Lua_State* L)
	{
		Lua_PushString(L, KUiConfig::GetSelf().GetUserAccount());
		return 1;
	}

	int KSystemScriptTable::LuaGetUserServer(Lua_State* L)
	{
		Lua_PushString(L, KUiConfig::GetSelf().GetUserRegion());
		Lua_PushString(L, KUiConfig::GetSelf().GetUserServer());
		return 2;
	}

	int KSystemScriptTable::LuaGetUserRoleName(Lua_State* L)
	{
		Lua_PushString(L, KUiConfig::GetSelf().GetUserRoleName());
		return 1;
	}

	int KSystemScriptTable::LuaGetUserDataFloder(Lua_State* L)
	{
		Lua_PushString(L, F_UI_USER_DATA_FOLDER);
		return 1;
	}

	int KSystemScriptTable::LuaGetUserDataPath(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		char szFilePath[MAX_PATH];
		szFilePath[0] = '\0';
		LPCSTR pcszRole = NULL;

		if (Lua_GetTopIndex(L) > 0)
			pcszRole = Lua_ValueToString(L, 1);

		bRetCode = KUiConfig::GetSelf().GetUserDataPath(szFilePath, pcszRole);
		KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
		Lua_PushString(L, szFilePath);
		return 1;
	}

	int KSystemScriptTable::LuaSaveUserData(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szAccountName = NULL;

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);
	
		szAccountName = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(szAccountName);

		bRetCode = KUiConfig::GetSelf().Save(szAccountName);
		KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
		return 0;
	}

	int KSystemScriptTable::LuaLoadUserData(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szAccountName = NULL;

		bRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);
	
		szAccountName = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(szAccountName);

		bRetCode = KUiConfig::GetSelf().Load(szAccountName);
		KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
		return 0;
	}

	int KSystemScriptTable::LuaIsUserDataExist(Lua_State* L)
	{
		int nRetCode = false;
        int nResult = false;
        LPCSTR pcszAccountName = NULL;

		nRetCode = lua_gettop(L) == 1;
		KGLOG_PROCESS_ERROR(nRetCode);
	
        pcszAccountName = lua_tostring(L, 1);
        KGLOG_PROCESS_ERROR(pcszAccountName);

        nRetCode = KUiConfig::GetSelf().IsUserdataExist(pcszAccountName);
		KG_PROCESS_ERROR(nRetCode);

        nResult = true;
Exit0:
        lua_pushboolean(L, nRetCode);
		return 1;
	}
	
	int KSystemScriptTable::LuaLoadDataFromFile(Lua_State* L)
	{
		int nRetCode = false;

		IFile *piFile = NULL;
		const char *pcszFile = NULL;
		const char *pcsBuff = NULL;
		size_t uBuffSize = 0;
		int nParamCount = 0;
		char szFilePath[MAX_PATH];

		KGLOG_PROCESS_ERROR(L);

		nRetCode = lua_gettop(L);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		pcszFile = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(pcszFile);

		nRetCode = snprintf(
			szFilePath, 
			sizeof(szFilePath), 
			"%s\\%s", 
			F_UI_USER_DATA_FOLDER, 
			pcszFile
		);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		szFilePath[sizeof(szFilePath) - 1] = '\0';
		FormatFilePath(szFilePath);

		nRetCode = KUiConfig::IsFilePathExist(szFilePath);
		KG_PROCESS_SUCCESS(!nRetCode);

		piFile = g_OpenFile(szFilePath, true);
		KGLOG_PROCESS_ERROR(piFile);

		uBuffSize = piFile->Size();
		KGLOG_PROCESS_ERROR(uBuffSize > 0);

		pcsBuff = (const char *)piFile->GetBuffer();
		KGLOG_PROCESS_ERROR(pcsBuff);

		lua_pushlstring(L, pcsBuff, uBuffSize);
		nParamCount++;

Exit1:
Exit0:
		SAFE_RELEASE(piFile);
		return nParamCount;
	}

	int KSystemScriptTable::LuaSaveDataToFile(Lua_State* L)
	{
		int nRetCode = false;

		IFile *piFile = NULL;
		const char *pcszFile = NULL;
		const char *pcsBuff = NULL;
		unsigned uFilePathLength = 0;
		size_t uBuffSize = 0;
		int i = 0;
		char szFile[MAX_PATH];
		char szPath[MAX_PATH];

		KGLOG_PROCESS_ERROR(L);

		nRetCode = lua_gettop(L);
		KGLOG_PROCESS_ERROR(nRetCode == 2);

		pcsBuff = lua_tolstring(L, 1, &uBuffSize);
		KGLOG_PROCESS_ERROR(pcsBuff);
		KGLOG_PROCESS_ERROR(uBuffSize > 0);

		pcszFile = lua_tostring(L, 2);
		KGLOG_PROCESS_ERROR(pcszFile);
		KGLOG_PROCESS_ERROR(pcszFile[0]);

		uFilePathLength = snprintf(
			szFile, 
			sizeof(szFile), 
			"%s\\%s", 
			F_UI_USER_DATA_FOLDER, 
			pcszFile
		);
		KGLOG_PROCESS_ERROR(uFilePathLength > 0);
		szFile[sizeof(szFile) - 1] = '\0';
		FormatFilePath(szFile);

		szPath[0] = '\0';
		for (i = uFilePathLength; i >= 0; i--)
		{
			if (szFile[i] != '/' && szFile[i] != '\\')
				continue;

			ASSERT(i < sizeof(szPath));
			strncpy(szPath, szFile, i);
			szPath[i] = '\0';
			break;
		}

		nRetCode = KUiConfig::IsFilePathExist(szPath);
		if (!nRetCode)
		{
			nRetCode = KUiConfig::CreatePath(szPath);
			KGLOG_PROCESS_ERROR(nRetCode);
		}

		piFile = g_CreateFile(szFile);
		KGLOG_PROCESS_ERROR(piFile);

		nRetCode = piFile->Write(pcsBuff, (unsigned long)uBuffSize);
		KGLOG_PROCESS_ERROR(nRetCode == uBuffSize);

Exit0:
		SAFE_RELEASE(piFile);
		return 0;
	}

	int KSystemScriptTable::LuaMovieRecord(Lua_State* L)
	{
		int nSizeScale = 1;
		BOOL bTwoSize = TRUE;
		int nParamCount = 0;
		int nFilerType = IPPSPEC_FILTER_LINEAR;
		int nQuality = IPPSPEC_QUALITY_CINEMATIC;
		int nEncoder = IPPSPEC_MPEG2;
		int nFps = IPPSPEC_FPS_25;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);

		if (nParamCount > 0)
		{
			nSizeScale = (int)Lua_ValueToNumber(L, 1);
			if (nSizeScale < 1)
			{
				nSizeScale = 1;
			}
		}
		if (nParamCount > 1)
			nFilerType = (int)Lua_ValueToNumber(L, 2);
		if (nParamCount > 2)
			nQuality = (int)Lua_ValueToNumber(L, 3);
		if (nParamCount > 3)
			nEncoder = (int)Lua_ValueToNumber(L, 4);
		if (nParamCount > 4)
			nFps = (int)Lua_ValueToNumber(L, 5);

		if (!g_pUI->m_p3DEngineManager->MovieReady() )
		{
            LPCSTR pcszPath = g_pUI->m_FilePathMgr.GetFilePath("ScreenShotDir");
			if (pcszPath)
			{
				g_pUI->m_p3DEngineManager->MovieRecord(pcszPath, nSizeScale, nFilerType, nQuality, nEncoder, nFps);
			}
		}

		return 0;
	}

	int KSystemScriptTable::LuaMovieStop(Lua_State* L)
	{
		if ( g_pUI->m_p3DEngineManager->MovieReady() )
		{
			g_pUI->m_p3DEngineManager->MovieStop( );
		}

		return 0;
	}

	int KSystemScriptTable::LuaIsMovieRecord(Lua_State* L)
	{
		Lua_PushBoolean(L,  g_pUI->m_p3DEngineManager->MovieReady());
		return 1;
	}

    int KSystemScriptTable::LuaCanRecord(Lua_State* L)
    {
		Lua_PushBoolean(L,  g_pUI->m_p3DEngineManager->CanRecord());
		return 1;
    }

    int KSystemScriptTable::LuaIsRecording(Lua_State* L)
    {
		Lua_PushBoolean(L,  g_pUI->m_p3DEngineManager->IsRecording());
		return 1;
    }

    int KSystemScriptTable::LuaStartRecord(Lua_State* L)
    {
        unsigned int uMode = 0;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

        uMode = (unsigned int)lua_tonumber(L, 1);

		if (g_pUI->m_p3DEngineManager->CanRecord() && !g_pUI->m_p3DEngineManager->IsRecording() )
		{
            LPCSTR pcszPath = g_pUI->m_FilePathMgr.GetFilePath("ScreenShotDir");
			if (pcszPath)
			{
				g_pUI->m_p3DEngineManager->StartRecord(pcszPath, uMode);
			}
		}

Exit0:
		return 0;
    }

    int KSystemScriptTable::LuaStopRecord(Lua_State* L)
    {
		if (g_pUI->m_p3DEngineManager->CanRecord() && g_pUI->m_p3DEngineManager->IsRecording() )
            g_pUI->m_p3DEngineManager->StopRecord();

        return 0;
    }

	int KSystemScriptTable::LuaGetAllowableAdapterModes(Lua_State* L)
	{
		struct KADAPTERMODE
		{
			UINT Width;
			UINT Height;
			UINT RefreshRates[32];   // 超出忽略后面的
		};

        HRESULT hr = E_FAIL;
        UINT uCount = 0;
        std::vector<KADAPTERMODE> AdapterModes;
        std::vector<D3DDISPLAYMODE> DisplayModes;

		ASSERT(L);

        uCount = g_pUI->m_p3DUI->GetAllowableAdapterModeCount();
        KGLOG_PROCESS_ERROR(uCount != 0);

        DisplayModes.resize(uCount);

        hr = g_pUI->m_p3DUI->GetAllowableAdapterMode(&DisplayModes[0], &uCount);
        KGLOG_COM_PROCESS_ERROR(hr);

        for (UINT uIndex = 0; uIndex < uCount; ++uIndex)
		{
			int nFind = false;
            D3DDISPLAYMODE& dm = DisplayModes[uIndex];

            for (UINT uAdapter = 0; uAdapter != AdapterModes.size(); ++uAdapter)
			{
				KADAPTERMODE& am = AdapterModes[uAdapter];

				if (dm.Width == am.Width && dm.Height == am.Height)
				{
                    for (UINT uRefreshRateIndex = 0; uRefreshRateIndex != _countof(am.RefreshRates); ++uRefreshRateIndex)
                    {
                        if (am.RefreshRates[uRefreshRateIndex] == 0)
                        {
                            am.RefreshRates[uRefreshRateIndex] = dm.RefreshRate;
                            break;
                        }
                    }
                    nFind = true;
					break;
				}
			}
			if (!nFind)
			{
				KADAPTERMODE Mode;

                memset(&Mode, 0, sizeof(Mode));

				Mode.Width = dm.Width;
				Mode.Height = dm.Height;
				Mode.RefreshRates[0] = dm.RefreshRate;

				AdapterModes.push_back(Mode);
			}
		}

		Lua_NewTable(L);

		for (size_t nIndex = 0; nIndex < AdapterModes.size(); ++nIndex)
		{
            KADAPTERMODE& am = AdapterModes[nIndex];

			Lua_PushNumber(L, nIndex + 1);

			Lua_NewTable(L);

			Lua_PushString(L, "nWidth");
			Lua_PushNumber(L, am.Width);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "nHeight");
			Lua_PushNumber(L, am.Height);
			Lua_SetTable(L, -3);

			Lua_PushString(L, "aRefreshRate");
			Lua_NewTable(L);

            int nUsedRefreshRateIndex = 1;

            for (size_t nRefreshRateIndex = 0; nRefreshRateIndex < _countof(am.RefreshRates); ++nRefreshRateIndex)
			{
                UINT nRefreshRate = am.RefreshRates[nRefreshRateIndex];
                if (nRefreshRate != 0)
                {
                    Lua_PushNumber(L, nUsedRefreshRateIndex++);
                    Lua_PushNumber(L, nRefreshRate);
                    Lua_SetTable(L, -3);
                }
			}
			Lua_SetTable(L, -3);

			Lua_SetTable(L, -3);
		}

		return 1;
Exit0:
        return 0;
	}


	int KSystemScriptTable::LuaGetCurrentAdapterMode(Lua_State* L)
	{
		DEVMODE devMode;

		ASSERT(L);

		::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);

		Lua_PushNumber(L, devMode.dmPelsWidth);
		Lua_PushNumber(L, devMode.dmPelsHeight);
		Lua_PushNumber(L, devMode.dmDisplayFrequency);
		return 3;
	}

	
	int KSystemScriptTable::LuaEatRAM(Lua_State* L)
	{
		static void* s_pBuffer = NULL;
		static size_t s_nEatByte = 0;

		size_t nMega = 32;

		ASSERT(L);

		if (lua_gettop(L) == 1)
			nMega = (size_t)lua_tonumber(L, 1);
		
		s_nEatByte += nMega * 1024 * 1024;

		s_pBuffer = realloc(s_pBuffer, s_nEatByte);

		return 0;
	}


	int KSystemScriptTable::LuaEatVRAM(Lua_State* L)
	{
		size_t nMega = 32;

		ASSERT(L);

		if (lua_gettop(L) == 1)
			nMega = (size_t)lua_tonumber(L, 1);

		size_t nByte = nMega * 1024 * 1024;
		size_t nCount = nByte / (256 * 256 * 4);

		DWORD dwImageID = 0;

		for (size_t nIndex = 0; nIndex < nCount; ++nIndex)
		{
			if (FAILED(g_pUI->m_p3DUI->CreateImage(256, 256, &dwImageID, FALSE)))
			{
				break;
			}
		}

		return 0;
	}

	int KSystemScriptTable::LuaSwapMouseButton(Lua_State* L)
	{
		ASSERT(L);
		int nSwap = FALSE;

		KG_PROCESS_ERROR(lua_gettop(L) == 1);

		if (lua_isboolean(L, 1))
			nSwap = (int)Lua_ValueToBoolean(L, 1);
		else
			nSwap = (int)Lua_ValueToNumber(L, 1);

		::SwapMouseButton(nSwap);

Exit0:
		return 0;
	}

	int KSystemScriptTable::LuaIsMouseButtonSwaped(Lua_State* L)
	{
		ASSERT(L);
		BOOL bSwapped = ::GetSystemMetrics(SM_SWAPBUTTON);

		Lua_PushBoolean(L, bSwapped);
		return 1;
	}

	int KSystemScriptTable::LuaGetOpenFileName(Lua_State* L)
	{
		int nRetCode = false;
		OPENFILENAME OpenFileName;
		char szFile[MAX_PATH];
		szFile[0] = '\0';
		const char * pcszTitle = "Please select a file!";
		int nParamCount = lua_gettop(L);

		if (nParamCount > 0)
			pcszTitle = Lua_ValueToString(L, 1);

		memset(&OpenFileName, 0, sizeof(OPENFILENAME));
		OpenFileName.lStructSize = sizeof(OPENFILENAME);
		OpenFileName.hwndOwner = g_pUI->m_hWnd;
		OpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR |
			OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
		OpenFileName.lpstrTitle = pcszTitle;
		OpenFileName.nMaxFile = sizeof(szFile);
		OpenFileName.lpstrFile = szFile;
		nRetCode = ::GetOpenFileNameA(&OpenFileName);
		KG_PROCESS_ERROR(nRetCode);

Exit0:
		Lua_PushString(L, szFile);
		return 1;
	}

	int KSystemScriptTable::LuaGetUnpakFileSize(Lua_State* L)
	{
		int nRetCode = false;
		int nSize = 0;
		const char * pcszFile = NULL;
		FILE * pFile = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszFile = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszFile);

		pFile = fopen(pcszFile, "rb");
		KG_PROCESS_ERROR(pFile);
		
		fseek(pFile, 0, SEEK_END);
		nSize = (int)ftell(pFile);

Exit0:
		if (pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
		Lua_PushNumber(L, nSize);
		return 1;
	}

	int KSystemScriptTable::LuaIsUnpakFileExist(Lua_State* L)
	{
		int nRetCode = false;
		const char * pcszFile = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszFile = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszFile);

		nRetCode = ::PathFileExistsA(pcszFile);
Exit0:
		Lua_PushBoolean(L, nRetCode);
		return 1;
	}

    int KSystemScriptTable::LuaAllocConsole(Lua_State* L)
    {
        int nRetCode = false;

        nRetCode = AllocConsole();
        KGLOG_PROCESS_ERROR(nRetCode);

        freopen("CONOUT$", "w+t", stdout);
        printf("open console success.\n");
      
Exit0:
        return 0;
    }

    int KSystemScriptTable::LuaFreeConsole(Lua_State* L)
    {
        int nRetCode = false;

        nRetCode = FreeConsole();
        KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
        return 0;
    }

    int KSystemScriptTable::LuaClearGlobalString(lua_State *pLua)
    {
        int nRetCode = false;

        ASSERT(pLua);
        ASSERT(g_pUI);
        nRetCode = g_pUI->m_GlobalStrValuableMgr.Clear();
        KGLOG_PROCESS_ERROR(nRetCode);
Exit0:
        return 0;
    }

    int KSystemScriptTable::LuaRegisterGlobalString(lua_State *pLua)
    {
        int nRetCode = false;

        const char *pcszKey = NULL;
        const char *pcszValue = NULL;

        ASSERT(pLua);

        nRetCode = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nRetCode == 2);

        pcszKey = lua_tostring(pLua, 1);
        KGLOG_PROCESS_ERROR(pcszKey);
        KGLOG_PROCESS_ERROR(pcszKey[0]);

        pcszValue = lua_tostring(pLua, 2);
        KGLOG_PROCESS_ERROR(pcszValue);
        KGLOG_PROCESS_ERROR(pcszValue[0]);

        ASSERT(g_pUI);
        nRetCode = g_pUI->m_GlobalStrValuableMgr.Append(pcszKey, pcszValue);
        KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
        return 0;
    }

}



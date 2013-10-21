#include "stdafx.h"
#include "./kbasescripttable.h"
#include "./kscriptmgr.h"
#include "./kscriptloader.h"
#include "../config/kdebugconfig.h"
#include "../common/tblock.h"
#include "../scheme/kaddonmgr.h"
#include "../elem/tipcenter.h"
#include "../elem/wndstation.h"
#include "../KGUI.h"
#include "../Common/KG_Calculate.h"
#include "../Common/KG_I18N.h"

static const int MAX_WIDE_STRING_SIZE = 1024 * 8;
static const int SCREENSHOT_QUALITY_MIN_NUMBER = 0;
static const int SCREENSHOT_QUALITY_MAX_NUMBER = 100;
static const char SCREENSHOT_SUFFIX[] = "jpg";
static const char DEFAULT_SCREENSHOT_QUALITY = 70;

namespace
{
	struct KFlagNameIndexPair
	{
		LPCSTR szName;
		int	nIndex;
	};

	KFlagNameIndexPair const g_aFlagNameIndexPair[] = 
	{
		{ "ShowMouseWnd",				UI::KDebugConfig::BOOL_SHOW_MOUSE_OVER_WND },
		{ "ShowFrame",					UI::KDebugConfig::BOOL_SHOW_FRAME },
		{ "ShowName",					UI::KDebugConfig::BOOL_SHOW_NAME },
		{ "ShowFrameDragAreaBoder",		UI::KDebugConfig::BOOL_SHOW_FRAME_DRAG_AREA_BODER },
		{ "ShowTextAtomBoundary",		UI::KDebugConfig::BOOL_SHOW_TEXT_ATOM_BOUNDARY },
		{ "EnableRenderUI",				UI::KDebugConfig::BOOL_ENABLE_RENDER_UI },
		{ "AutoScreenShot",				UI::KDebugConfig::BOOL_AUTO_SCREEN_SHOT },
		{ "EnableLog3DDebugInfo",		UI::KDebugConfig::BOOL_ENABLE_LOG_3D_DEBUG_INFO	},
		{ "DragWndAndItemMode",			UI::KDebugConfig::BOOL_DRAG_WND_AND_ITEM_MODE },
		{ "ResizeWndAndItemMode",		UI::KDebugConfig::BOOL_RESIZE_WND_AND_ITEM_MODE },
		{ "SelWndInClient",				UI::KDebugConfig::BOOL_SELECT_WINDOW_IN_CLIENT },
		{ "EditMode",					UI::KDebugConfig::BOOL_EDIT_MODE },

	};

}

namespace UI
{

	bool KBaseScriptTable::ms_bLoadLoginUI = false;

	void KBaseScriptTable::Load(lua_State* L)
	{
		static KLuaFunc s_aGlobalFunctionTable[] =
		{
			{ "RegisterString",						LuaRegisterString },
			{ "GetString",							LuaGetString },
			{ "Debug",								LuaDebug },
			{ "GetDebugModeState",					LuaGetDebugModeState },

            { "Trace",                              Lua_Trace },
            { "Break",                              Lua_Break },
            { "Log",                                Lua_Log },
			{ "CheckMemory",						Lua_CheckMemory },

			{ "ScreenShot",							Lua_ScreenShot },

            { "LoadImage",                          Lua_LoadImage },
            { "UnloadImage",                        Lua_UnloadImage },
            { "MaskImage",                          Lua_MaskImage },

			{ "NumberToChinese",					Lua_NumberToChinese },
			
			{ "LoadDefaultScriptLib",				Lua_LoadDefaultScriptLib },
			{ "LoadLoginScriptLib",					Lua_LoadLoginScriptLib },
			{ "LoadScriptFile",						Lua_LoadScriptFile },
			{ "LoadScriptLib",						Lua_LoadScriptLib },
			{ "InitAddOn",							Lua_InitAddOn },
			{ "GetAddOnCount",						Lua_GetAddOnCount },
			{ "GetAddOnInfo",						Lua_GetAddOnInfo },
			{ "EnableAddOn",						Lua_EnableAddOn },
			{ "DisableAddOn",						Lua_DisableAddOn },
			{ "LoadAddOn",							Lua_LoadAddOn },
			{ "GetAddOnIndexByID",					Lua_GetAddOnIndexByID },
			{ "EnableOverdueAddOn",					Lua_EnableOverdueAddOn },
			{ "IsEnableOverdueAddOn",				Lua_IsEnableOverdueAddOn },
			{ "SetCurrentAddOnVersion",				Lua_SetCurrentAddOnVersion },

			{ "SetGlobal",							Lua_MySetGlobal },
			{ "GetGlobal",							Lua_MyGetGlobal },

			{ "GetRootPath",						Lua_GetRootPath },

			{ "ExitGame",							LuaExitGame },

			{ "ReInitUI",							LuaReInitUI },
        
            { "IsValidRoleName",                    LuaIsValidRoleName },

			{ "RegisterEvent",			            LuaRegisterEvent },
			{ "UnRegisterEvent",			        LuaUnRegisterEvent },
            { "FireEvent",			                LuaFireEvent },

            { "IsActivated",                        LuaIsActivated },

			{ "GetLoadLoginReason",					LuaGetLoadLoginReason },

			{ "GetAutoTipInfo",						LuaGetAutoTipInfo },

			{ "OpenUIEditor",						LuaOpenUIEditor },
			{ "CloseUIEditor",						LuaCloseOpenUIEditor },

			{ "GetVideoSettings",				    LuaGetVideoSettings },
			{ "SetVideoSettings",				    LuaSetVideoSettings },

            { "IsGB2312String",                     LuaIsGB2312String },
            { "IsGBKString",                        LuaIsGBKString },
            { "IsSimpleChineseString",              LuaIsSimpleChineseString },

            { "IsDebug",                            LuaIsDebug },

            { "GetUserEnv",                         LuaGetUserEnv },

			{ "GetLastLuaError",                    LuaGetLastError },

			{ "IsFileExist",						LuaIsFileExist },

            { "StringFindW",                        LuaStringFindW },
            { "StringReplaceW",                     LuaStringReplaceW },
			{ "StringLowerW",						LuaStringLowerW },

            { "GetNumberBit",                       LuaGetNumberBit},
            { "SetNumberBit",                       LuaSetNumberBit},
			{ "MoneyToGoldSilverAndCopper",         LuaMoneyToGoldSilverAndCopper},
            { "FormatString",                       LuaFormatString },
			
        };

        KScriptLoader::RegisterFunctions(L, s_aGlobalFunctionTable, _countof(s_aGlobalFunctionTable));
    }


	bool KBaseScriptTable::HostDebug(LPCSTR szMode)
	{
		HRESULT hr = E_FAIL;

		KGLOG_PROCESS_ERROR(szMode && szMode[0] != '\0');

        for (size_t nIndex = 0; nIndex < _countof(g_aFlagNameIndexPair); ++nIndex)
        {
            if (stricmp(szMode, g_aFlagNameIndexPair[nIndex].szName) == 0)
            {
                size_t const nFlagIndex = g_aFlagNameIndexPair[nIndex].nIndex;

                KDebugConfig::ms_aBoolFlag[nFlagIndex] = !KDebugConfig::ms_aBoolFlag[nFlagIndex];

                return true;
            }
        }

Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KBaseScriptTable::HostDebug(%s)\n", szMode ? szMode : "");
		return false;
	}


	bool KBaseScriptTable::HostGetDebugModeState(LPCSTR szMode, DWORD& dwState)
	{
		KGLOG_PROCESS_ERROR(szMode && szMode[0] != '\0');

		for (size_t nIndex = 0; nIndex < _countof(g_aFlagNameIndexPair); ++nIndex)
		{
			if (stricmp(szMode, g_aFlagNameIndexPair[nIndex].szName) == 0)
			{
				size_t const nFlagIndex = g_aFlagNameIndexPair[nIndex].nIndex;
				dwState = KDebugConfig::ms_aBoolFlag[nFlagIndex] ? 1 : 0;
				return true;
			}
		}

Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KBaseScriptTable::HostGetDebugModeState(%s)\n", szMode ? szMode : "");
		return false;
	}


    int KBaseScriptTable::Host_ScreenShot(LPCSTR szFilePath, int nQuality)
    {
        int nResult = false;
        int nRetCode = false;
        HRESULT hr = E_FAIL;
		D3DXIMAGE_FILEFORMAT eType = D3DXIFF_BMP;
		const char * pcszFix = NULL;

        KGLOG_PROCESS_ERROR(szFilePath);
        KGLOG_PROCESS_ERROR(g_pUI);
        KGLOG_PROCESS_ERROR(g_pUI->m_p3DUI);

		pcszFix = strrchr(szFilePath, '.');
		if (pcszFix)
		{
			++pcszFix;
			if (stricmp(pcszFix, "bmp") == 0)
				eType = D3DXIFF_BMP;
			else if (stricmp(pcszFix, "jpg") == 0)
				eType = D3DXIFF_JPG;
			else if (stricmp(pcszFix, "jpeg") == 0)
				eType = D3DXIFF_JPG;
			else if (stricmp(pcszFix, "tga") == 0)
				eType = D3DXIFF_TGA;
			else if (stricmp(pcszFix, "png") == 0)
				eType = D3DXIFF_PNG;
			else if (stricmp(pcszFix, "dds") == 0)
				eType = D3DXIFF_DDS;
			else if (stricmp(pcszFix, "ppm") == 0)
				eType = D3DXIFF_PPM;
			else if (stricmp(pcszFix, "dib") == 0)
				eType = D3DXIFF_DIB;
			else if (stricmp(pcszFix, "hdr") == 0)
				eType = D3DXIFF_HDR;
			else if (stricmp(pcszFix, "pfm") == 0)
				eType = D3DXIFF_PFM;
		}

        hr = g_pUI->m_p3DUI->ScreenShot(szFilePath, eType, nQuality);
        KGLOG_COM_PROCESS_ERROR(hr);

        g_pUI->Paint();

        nRetCode = g_IsFileExist(szFilePath);
        KGLOG_PROCESS_ERROR(nRetCode);

        nResult = true;
Exit0:
        return nResult;
    }

	int KBaseScriptTable::Host_ScreenShotBySuffix(LPCSTR szFix, int nQuality)
	{
		int nRetCode = false;
        int nResult = false;
		char szFilePath[128];
		char szFileName[64];
		char szIndex[32];
		time_t tmNow;
		static time_t s_tmLast;
		static int nIndex = 0;
		LPCSTR szFilePathScreenShotDir = NULL;
		if (!szFix)
			szFix = SCREENSHOT_SUFFIX;

		time(&tmNow);

		if (s_tmLast != tmNow)
			nIndex = 0;

		s_tmLast = tmNow;

		strftime(szFileName, _countof(szFileName), "%Y-%m-%d_%H-%M-%S-", localtime(&tmNow));

		_snprintf(szIndex, _countof(szIndex), "%.3d", nIndex++);
		szIndex[_countof(szIndex) - 1] = '\0';

		szFilePathScreenShotDir = g_pUI->m_FilePathMgr.GetFilePath("ScreenShotDir");
		KGLOG_PROCESS_ERROR(szFilePathScreenShotDir);

		_snprintf(szFilePath, _countof(szFilePath), "%s\\%s%s.%s", szFilePathScreenShotDir, szFileName, szIndex, szFix);
		szFilePath[_countof(szFilePath) - 1] = '\0';

		nRetCode = Host_ScreenShot(szFilePath, nQuality);
		KGLOG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
		return nResult;
	}

	int KBaseScriptTable::LuaRegisterString(Lua_State* L)
	{
		BOOL bRetCode = FALSE;

		ASSERT(L);

		int const nValueCount = Lua_GetTopIndex(L);
		for (int i = 1; i <= nValueCount; ++i)
		{
			LPCSTR pcszValueName = NULL;
            LPCSTR pcszValue = NULL;
            
            pcszValueName = Lua_ValueToString(L, i);
			KGLOG_PROCESS_ERROR(pcszValueName);

            pcszValue = g_pUI->m_GlobalStrValuableMgr.GetAt(pcszValueName);
            KGLOG_PROCESS_ERROR(pcszValue);

            Lua_PushString(L, pcszValue);
            Lua_SetGlobal(L, pcszValueName);
        }

		return 0;
Exit0:
		return 0;
	}

	int KBaseScriptTable::LuaGetString(Lua_State *L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR pcszValue = NULL;
		LPCSTR pcszValueName = NULL;

		ASSERT(L);
		
		bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		pcszValueName = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszValueName);

		pcszValue = g_pUI->m_GlobalStrValuableMgr.GetAt(pcszValueName);
		KGLOG_PROCESS_ERROR(pcszValue);

		Lua_PushString(L, pcszValue);
		return 1;
Exit0:
		return 0;
	}

	int KBaseScriptTable::LuaDebug(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szMode = NULL;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		szMode = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(szMode);

		bRetCode = HostDebug(szMode);
		KGLOG_PROCESS_ERROR(bRetCode);

		return 0;
Exit0:
		return 0;
	}

	int KBaseScriptTable::LuaGetDebugModeState(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szMode = NULL;
		DWORD dwState = 0;

		ASSERT(L);

		bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		szMode = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(szMode);

		bRetCode = HostGetDebugModeState(szMode, dwState);
		KGLOG_PROCESS_ERROR(bRetCode);

		Lua_PushNumber(L, dwState);
		return 1;
Exit0:
		return 0;
	}

    int KBaseScriptTable::Lua_Break(Lua_State* L)
    {
		BOOL bRetCode = FALSE;
		LPCSTR pcszMessage = NULL;

        ASSERT(L);

        bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

        pcszMessage = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszMessage);

        ASSERT(0 && pcszMessage);

        return 0;
Exit0:
		return 0;
    }

    int KBaseScriptTable::Lua_Trace(Lua_State* L)
    {
		BOOL bRetCode = FALSE;
		LPCSTR pcszMessage = NULL;

        ASSERT(L);

        bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

        pcszMessage = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszMessage);

        ATLTRACE(pcszMessage);

		KGLogPrintf(KGLOG_DEBUG, "%s", pcszMessage);

        return 0;
Exit0:
		return 0;
    }


    int KBaseScriptTable::Lua_Log(Lua_State* L)
    {
		BOOL bRetCode = FALSE;
		LPCSTR pcszMessage = NULL;

        ASSERT(L);

        bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

        pcszMessage = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszMessage);

		KGLogPrintf(KGLOG_DEBUG, "%s", pcszMessage);

        return 0;
Exit0:
		return 0;
    }


    int KBaseScriptTable::Lua_CheckMemory(Lua_State* L)
    {
#ifdef _DEBUG

		BOOL bRetCode = FALSE;
		LPCSTR pcszMessage = NULL;

        ASSERT(L);

		bRetCode = _CrtCheckMemory();
		KG_PROCESS_SUCCESS(bRetCode);

        bRetCode = Lua_GetTopIndex(L) == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

        pcszMessage = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszMessage);

		KGLogPrintf(KGLOG_DEBUG, "KGUI %s", pcszMessage);

		ASSERT(_CrtCheckMemory());

Exit1:	
		return 0;
Exit0:
		return 0;
#else
		return 0;
#endif
    }


	int KBaseScriptTable::Lua_ScreenShot(Lua_State* L)
	{
		int nRetCode = false;
		int nParamCount = 0;
        int nRetParamCount = 0;
		BOOL bFullPath = FALSE;
        LPCSTR pcszFilePath = NULL;
        LPCSTR pcszScreenShotDir = NULL;
        char szScreenShotPath[MAX_PATH];
        int nLength = 0;
        int nQuality = 0;

        ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);
        nRetCode = nParamCount == 0 || nParamCount == 2 || nParamCount == 3;
		KGLOG_PROCESS_ERROR(nRetCode);

		if (nParamCount == 0)
		{
			nRetCode = Host_ScreenShotBySuffix(NULL, DEFAULT_SCREENSHOT_QUALITY);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		else
		{
            pcszFilePath = lua_tostring(L, 1);
            KGLOG_PROCESS_ERROR(pcszFilePath);
            KGLOG_PROCESS_ERROR(pcszFilePath[0]);

			pcszFilePath = FormatFilePath(pcszFilePath);

            nRetCode = lua_isnumber(L, 2);
            KGLOG_PROCESS_ERROR(nRetCode);
            nQuality = (int)lua_tonumber(L, 2);
            KGLOG_PROCESS_ERROR(nQuality >= SCREENSHOT_QUALITY_MIN_NUMBER);
            KGLOG_PROCESS_ERROR(nQuality <= SCREENSHOT_QUALITY_MAX_NUMBER);

			if (nParamCount == 3)
			{
                nRetCode = Lua_IsBoolean(L, 3);
				if (nRetCode)
					bFullPath = Lua_ValueToBoolean(L, 3);
				else
					bFullPath = (BOOL)Lua_ValueToNumber(L, 3);
			}

			if (bFullPath)
			{
				nRetCode = Host_ScreenShot(pcszFilePath, nQuality);
				KGLOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
  				nRetCode = Host_ScreenShotBySuffix(pcszFilePath, nQuality);
				KGLOG_PROCESS_ERROR(nRetCode);
			}

		}

        if (!bFullPath)
        {
            g_GetRootPath(szScreenShotPath);
            KGLOG_PROCESS_ERROR(szScreenShotPath);

            pcszScreenShotDir = g_pUI->m_FilePathMgr.GetFilePath("ScreenShotDir");
            KGLOG_PROCESS_ERROR(pcszScreenShotDir);

            nRetCode = snprintf(
                szScreenShotPath, 
                sizeof(szScreenShotPath),
                "%s\\%s", 
                szScreenShotPath, 
                pcszScreenShotDir
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0);
            szScreenShotPath[sizeof(szScreenShotPath) - 1] = '\0';
            
            pcszFilePath = szScreenShotPath;
        }

		lua_pushstring(L, pcszFilePath);
        nRetParamCount++;
Exit0:
		return nRetParamCount;
	}

    int KBaseScriptTable::Lua_LoadImage(Lua_State* L)
    {
        HRESULT hr = E_FAIL;
        LPCSTR szFilePath = NULL;
        DWORD dwImageID = 0;
        char szFormattedFilePath[MAX_PATH];

        ASSERT(L);

        KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

        szFilePath = lua_tostring(L, 1);
        KGLOG_PROCESS_ERROR(szFilePath);

        strncpy(szFormattedFilePath, szFilePath, _countof(szFormattedFilePath));
        szFormattedFilePath[_countof(szFormattedFilePath) - 1];

        FormatFilePath(szFormattedFilePath);

        hr = g_pUI->m_p3DUI->LoadImage(szFormattedFilePath, &dwImageID);
        KGLOG_COM_PROCESS_ERROR(hr);

        lua_pushnumber(L, dwImageID);
        return 1;
Exit0:
        return 0;
    }

    int KBaseScriptTable::Lua_UnloadImage(Lua_State* L)
    {
        HRESULT hr = E_FAIL;
        DWORD dwImageID = 0;

        KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

        dwImageID = (DWORD)lua_tonumber(L, 1);
        
        hr = g_pUI->m_p3DUI->UnloadImage(dwImageID);
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        return 0;
    }

    int KBaseScriptTable::Lua_MaskImage(Lua_State* L)
    {
        HRESULT hr = E_FAIL;
        DWORD dwImageID = 0;
        DWORD dwMaskImageID = 0;

        KGLOG_PROCESS_ERROR(lua_gettop(L) == 2);

        dwImageID = (DWORD)lua_tonumber(L, 1);
        dwMaskImageID = (DWORD)lua_tonumber(L, 2);
        
        hr = g_pUI->m_p3DUI->MaskImage(dwImageID, dwMaskImageID);
        KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
        return 0;
    }

	int KBaseScriptTable::Lua_NumberToChinese(Lua_State* L)
	{
		ASSERT(L);

		char szResult[128];
		LPCSTR pcszString = NULL;
		LPCSTR pcszClip = NULL;
		int nIntCount = 0;
		int nIsFloat = false;
		int nIgnoreZero = true;
		int nLength = 0;
		int nNoZero = 0;
		int nNegative = false; //改变量记录是否为负数
		std::vector<LPCSTR> aEncodedArray;
		static LPCSTR s_szAfters[7] = { NULL, "万", "亿", "万亿", "亿亿", "万亿亿", "亿亿亿" };
		LPCSTR pcszTemp = NULL;

        szResult[0] = '\0';

		KG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszString = Lua_ValueToString(L, 1);
		KG_PROCESS_ERROR(pcszString);

		pcszClip = strchr(pcszString, '.');
		if (pcszClip)
		{
			nIsFloat = true;
			nLength = (int)(pcszClip - pcszString); 
		}
		else
        {
			nLength = (int)strlen(pcszString);
        }
		pcszClip = pcszString + strlen(pcszString);

		//过滤前面的0
		if (pcszString[0] == '-')
			nNegative = true; 
		nNoZero = nNegative;
		while (nNoZero < nLength && pcszString[nNoZero] == '0')
			nNoZero++;
		if (nNoZero == nLength)
			--nNoZero;


		while ( pcszClip-- != pcszString + nNoZero)
		{
			LPCSTR pcszNumber = NULL;
			LPCSTR pcszAddOn = NULL;
			switch (*pcszClip)
			{
			case '0':
				if (!nIgnoreZero || pcszClip == pcszString + nNoZero)
				{
					pcszNumber = "零";
					if (!nIsFloat)
						nIgnoreZero = true;
				}
				break;
			case '1':
				pcszNumber = "一";
				nIgnoreZero = false;
				break;
			case '2':
				pcszNumber = "二";
				nIgnoreZero = false;
				break;
			case '3':
				pcszNumber = "三";
				nIgnoreZero = false;
				break;
			case '4':
				pcszNumber = "四";
				nIgnoreZero = false;
				break;
			case '5':
				pcszNumber = "五";
				nIgnoreZero = false;
				break;
			case '6':
				pcszNumber = "六";
				nIgnoreZero = false;
				break;
			case '7':
				pcszNumber = "七";
				nIgnoreZero = false;
				break;
			case '8':
				pcszNumber = "八";
				nIgnoreZero = false;
				break;
			case '9':
				pcszNumber = "九";
				nIgnoreZero = false;
				break;
			case '.':
				if (aEncodedArray.size())
					pcszNumber = "点";
				nIgnoreZero = true;
				nIsFloat = false;
				break;
			case '+':
				continue;
				break;
			case 'e':
				if (pcszString[0] == '-')
					strcpy(szResult, "负无穷大");
				else
					strcpy(szResult, "无穷大");
				goto Exit0;
				break;
			default:
				strcpy(szResult, "数字转换为汉字出错");
				goto Exit0;
				break;
			}

			if (*pcszClip == '.')
			{
				if (pcszNumber)
				{
					aEncodedArray.push_back(pcszNumber);
				}
				continue;
			}

			if (!nIsFloat)
			{
				int nIndex = 0;
				switch (nIntCount % 4)
				{
				case 0:
					nIndex = nIntCount / 4;
					if (nIndex> 0 && nIndex < 7)
					{
						if (nLength > 4 * (nIndex + 1))
						{
							pcszTemp =strstr(pcszClip - 3, "0000");
							if (pcszTemp && pcszTemp + 3 == pcszClip)
							{
								pcszAddOn = NULL;
								break;
							}
						}

						if (pcszNumber && stricmp(pcszNumber, "零") == 0)
						{
							pcszAddOn = "零";
							pcszNumber = s_szAfters[nIndex];
						}
						else
						{
							pcszAddOn = s_szAfters[nIndex];
						}
						break;
					} 
					else if (!nIndex)
						break;
					else
						strcpy(szResult, "无穷大");
					goto Exit0;
					break;
				case 1:
					if (pcszNumber)
					{
						if (*pcszClip == '1' && pcszClip == pcszString + nNoZero)
							pcszNumber = NULL;
						if (*pcszClip != '0')
							pcszAddOn = "十";
					}
					break;
				case 2:
					if (pcszNumber && *pcszClip != '0')
						pcszAddOn = "百";
					break;
				case 3:
					if (pcszNumber && *pcszClip != '0')
						pcszAddOn = "千";
					break;
				}
				++nIntCount;
			}

			if (pcszAddOn)
				aEncodedArray.push_back(pcszAddOn);

			if (pcszNumber)
				aEncodedArray.push_back(pcszNumber);
		}


		if (nNegative)
		{
			LPCSTR pcszNumber = "负";
			aEncodedArray.push_back(pcszNumber);
		}
		for(std::vector<LPCSTR>::reverse_iterator it = aEncodedArray.rbegin(); it != aEncodedArray.rend(); ++it)
		{
			ASSERT(*it);
			strcat(szResult, (*it));
		}

Exit0:
		Lua_PushString(L, szResult);
		return 1;
	}


	int KBaseScriptTable::Lua_LoadDefaultScriptLib(Lua_State* L)
	{
		g_pUI->m_Script.LoadDefaultScriptLib();
		return 0;
	}

	int KBaseScriptTable::Lua_LoadLoginScriptLib(Lua_State* L)
	{
		g_pUI->m_Script.LoadLoginScriptLib();
		return 0;
	}

	int KBaseScriptTable::Lua_LoadScriptFile(Lua_State* L)
	{
        LPCSTR pcszFile = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pcszFile = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(pcszFile);

		g_pUI->m_Script.LoadScriptFile(pcszFile);
Exit0:
		return 0;
	}

	int KBaseScriptTable::Lua_LoadScriptLib(Lua_State* L)
	{
        LPCSTR pcszFile = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszFile = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszFile);

		g_pUI->m_Script.LoadScriptLib(pcszFile);
Exit0:
		return 0;
	}

	int KBaseScriptTable::Lua_InitAddOn(Lua_State* L)
	{
		g_AddOnMgr.Init();
		return 0;
	}
	int KBaseScriptTable::Lua_GetAddOnCount(Lua_State* L)
	{
		ASSERT(L);

		int nCount = g_AddOnMgr.GetCount();
		Lua_PushNumber(L, nCount);
		return 1;
	}

	int KBaseScriptTable::Lua_GetAddOnInfo(Lua_State* L)
	{
		LPCSTR pcszName = "";
		LPCSTR pcszDesc = "";
		LPCSTR pcszID = "";
		int nEnable = false;
		int nIndex = 0;
		KAddOnMgr::KADDONINFO * pInfo = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
		nIndex = (int)Lua_ValueToNumber(L, 1);

		pInfo = g_AddOnMgr.GetAddOnInfo(nIndex);
		KGLOG_PROCESS_ERROR(pInfo);

		lua_newtable(L);

		Lua_PushString(L, "szID");
		lua_pushstring(L, pInfo->strID.c_str());
		Lua_SetTable(L, -3);

		Lua_PushString(L, "szName");
		lua_pushstring(L, pInfo->strName.c_str());
		Lua_SetTable(L, -3);

		Lua_PushString(L, "szDesc");
		lua_pushstring(L, pInfo->strDesc.c_str());
		Lua_SetTable(L, -3);

		Lua_PushString(L, "szDependence");
		lua_pushstring(L, pInfo->strDependence.c_str());
		Lua_SetTable(L, -3);

		Lua_PushString(L, "szWeakDependence");
		lua_pushstring(L, pInfo->strWeakDependence.c_str());
		Lua_SetTable(L, -3);

		Lua_PushString(L, "szVersion");
		lua_pushstring(L, pInfo->strVersion.c_str());
		Lua_SetTable(L, -3);

		Lua_PushString(L, "bDefault");
		Lua_PushBoolean(L, pInfo->nDefault);
		Lua_SetTable(L, -3);

		Lua_PushString(L, "bEnable");
		Lua_PushBoolean(L, pInfo->nLoad);
		Lua_SetTable(L, -3);

		Lua_PushString(L, "bDisable");
		Lua_PushBoolean(L, pInfo->nDisable);
		Lua_SetTable(L, -3);

		return 1;
Exit0:
		return 0;
	}

	int KBaseScriptTable::Lua_EnableAddOn(Lua_State* L)
	{
		ASSERT(L);
		int nEnable = false;
		int nIndex = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);
		nIndex = (int)Lua_ValueToNumber(L, 1);
		if (Lua_IsBoolean(L, 2))
			nEnable = (int)Lua_ValueToBoolean(L, 2);
		else
			nEnable = (int)Lua_ValueToNumber(L, 2);

		g_AddOnMgr.Enable(nIndex, nEnable);

Exit0:
		return 0;
	}

	int KBaseScriptTable::Lua_DisableAddOn(Lua_State* L)
	{
		ASSERT(L);
		int nDisable = false;
		int nIndex = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);
		nIndex = (int)Lua_ValueToNumber(L, 1);
		if (Lua_IsBoolean(L, 2))
			nDisable = (int)Lua_ValueToBoolean(L, 2);
		else
			nDisable = (int)Lua_ValueToNumber(L, 2);

		g_AddOnMgr.Disable(nIndex, nDisable);

Exit0:
		return 0;
	}

	int KBaseScriptTable::Lua_LoadAddOn(Lua_State* L)
	{
		g_AddOnMgr.LoadAddOn();
		return 0;
	}

	int KBaseScriptTable::Lua_EnableOverdueAddOn(Lua_State* L)
	{
		ASSERT(L);
		int nEnable = false;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
		if (Lua_IsBoolean(L, 1))
			nEnable = (int)Lua_ValueToBoolean(L, 1);
		else
			nEnable = (int)Lua_ValueToNumber(L, 1);

		g_AddOnMgr.EnableOverdueAddOn(nEnable);

Exit0:
		return 0;	
	}

	int KBaseScriptTable::Lua_IsEnableOverdueAddOn(Lua_State* L)
	{
		Lua_PushBoolean(L, g_AddOnMgr.IsEnableOverdueAddOn());
		return 1;
	}


	int KBaseScriptTable::Lua_SetCurrentAddOnVersion(Lua_State* L)
	{
		ASSERT(L);
		int nEnable = false;
		const char * pcszVersion = NULL;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);
		pcszVersion = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszVersion);
		g_AddOnMgr.SetCurrentAddOnVersion(pcszVersion);

Exit0:
		return 0;
	}


	int KBaseScriptTable::Lua_GetAddOnIndexByID(Lua_State* L)
	{
		LPCSTR pcszID = NULL;
		int nIndex = -1;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszID = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszID);

		nIndex = g_AddOnMgr.GetIndexByID(pcszID);

Exit0:
		Lua_PushNumber(L, nIndex);
		return 1;
	}

	int KBaseScriptTable::Lua_MySetGlobal(Lua_State* L)
	{
		LPCSTR pcszKey = NULL;

		ASSERT(L);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		pcszKey = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszKey);

		lua_setglobal(L, pcszKey);
Exit0:
		return 0;
	}

	int KBaseScriptTable::Lua_MyGetGlobal(Lua_State* L)
	{
		int nResult = 0;
		LPCSTR pcszKey = NULL;

		ASSERT(L);

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		pcszKey = Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pcszKey);

		lua_getglobal(L, pcszKey);
		nResult = 1;
Exit0:
		return nResult;
	}

	int KBaseScriptTable::Lua_GetRootPath(Lua_State* L)
	{
        char szRootPath[MAX_PATH];

		ASSERT(L);

        g_GetRootPath(szRootPath);

        lua_pushstring(L, szRootPath);
		return 1;
	}


	int KBaseScriptTable::LuaExitGame(Lua_State* L)
	{
        KGLOG_PROCESS_ERROR(g_pUI);

        PostMessage(g_pUI->m_hWnd, WM_QUIT, 0, 0);

Exit0:
        return 0;
	}

	int KBaseScriptTable::LuaReInitUI(Lua_State* L)
	{
		int nReason = 0;

		ASSERT(L);
		ASSERT(g_pUI);

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		nReason = (int)Lua_ValueToNumber(L, 1);

		g_pUI->ReInit(nReason);

Exit0:
		return 0;
	}

    int KBaseScriptTable::LuaIsValidRoleName(Lua_State* L)
    {
		BOOL bRetCode = FALSE;
        LPCSTR szText = NULL;
        size_t nLength = 0;

        ASSERT(L);

        bRetCode = Lua_GetTopIndex(L) == 1;
        KGLOG_PROCESS_ERROR(bRetCode);
    
        szText = Lua_ValueToString(L, 1);
        KG_PROCESS_ERROR(szText);

        nLength = strlen(szText);
        for (size_t nIndex = 0; nIndex < nLength; ++nIndex)
        {
            int nText = szText[nIndex];
            KG_PROCESS_ERROR(nText < 0 || nText > 0x7F);
        }

        Lua_PushBoolean(L, TRUE);
        return 1;
Exit0:
        Lua_PushBoolean(L, FALSE);
        return 1;
    }


    int KBaseScriptTable::LuaFireEvent(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szEvent = NULL;
        int nParamCount = 0;
        DWORD dwCookie = 0;

		ASSERT(L);

        nParamCount = Lua_GetTopIndex(L);

		bRetCode = nParamCount == 1 || nParamCount == 2;
		KGLOG_PROCESS_ERROR(bRetCode);

		szEvent = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(szEvent);

        if (nParamCount == 2)
            dwCookie = (DWORD)lua_tonumber(L, 2);

		g_pUI->m_Script.FireEvent(szEvent, dwCookie);

Exit0:
		return 0;
	}

	int KBaseScriptTable::LuaRegisterEvent(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		LPCSTR szEvent = NULL;
		int nParamCount = 0;
		DWORD dwCookie = 0;
		int nFunctionRef = LUA_NOREF;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);

		bRetCode = nParamCount == 2 || nParamCount == 3;
		KGLOG_PROCESS_ERROR(bRetCode);

		szEvent = lua_tostring(L, 1);
		KGLOG_PROCESS_ERROR(szEvent);

		if (nParamCount == 3)
			dwCookie = (DWORD)lua_tonumber(L, 2);

		nFunctionRef = luaL_ref(L, LUA_REGISTRYINDEX);
		KGLOG_PROCESS_ERROR(nFunctionRef != LUA_NOREF);
		
		g_pUI->m_Script.SubscribeEvent(nFunctionRef, szEvent, dwCookie);

Exit0:
		lua_settop(L, nParamCount);
		return 0;
	}

	int KBaseScriptTable::LuaUnRegisterEvent(Lua_State* L)
	{
		BOOL bRetCode = FALSE;
		BOOL bSuccess = FALSE;
		LPCSTR szEvent = NULL;
		int nParamCount = 0;
		DWORD dwCookie = 0;
		int nFunctionRef = LUA_NOREF;

		ASSERT(L);

		nParamCount = Lua_GetTopIndex(L);

		bRetCode = nParamCount == 1;
		KGLOG_PROCESS_ERROR(bRetCode);

		nFunctionRef = luaL_ref(L, LUA_REGISTRYINDEX);
		KGLOG_PROCESS_ERROR(nFunctionRef != LUA_NOREF);

		g_pUI->m_Script.UnsubscribeEvent(nFunctionRef);

Exit0:
		if (nFunctionRef != LUA_NOREF)
			luaL_unref(L, LUA_REGISTRYINDEX, nFunctionRef);
		lua_settop(L, nParamCount);
		return 0;
	}

    int KBaseScriptTable::LuaIsActivated(Lua_State* L)
    {
        ASSERT(L);

        BOOL bIsActivated = g_pUI->m_Input.IsActivated();

        Lua_PushBoolean(L, bIsActivated);

        return 1;
    }

	int KBaseScriptTable::LuaGetLoadLoginReason(Lua_State* L)
	{
		ASSERT(L);
		ASSERT(g_pUI);

		int nReason = g_pUI->GetInitReason();
		Lua_PushNumber(L, nReason);
		return 1;
	}

	int KBaseScriptTable::LuaGetAutoTipInfo(Lua_State* L)
	{
		ASSERT(L);
		KTipCenter::KTIPINFO &Info = KTipCenter::GetSelf().GetTipInfo();

		Lua_PushString(L, Info.pcszTip);
		Lua_PushNumber(L, Info.nShowTipType);
		Lua_PushBoolean(L, Info.nRichText);
		Lua_PushNumber(L, Info.fX);
		Lua_PushNumber(L, Info.fY);
		Lua_PushNumber(L, Info.fW);
		Lua_PushNumber(L, Info.fH);
		return 7;
	}

	int KBaseScriptTable::LuaOpenUIEditor(Lua_State* L)
	{
		g_pUI->m_EditorMgr.OpenUIEditor(true);
		return 0;
	}

	int KBaseScriptTable::LuaCloseOpenUIEditor(Lua_State* L)
	{
		g_pUI->m_EditorMgr.CLoseUIEditor();
		return 0;
	}


    static int SetLuaTableField(lua_State* L, int nType, char const* szName, void* pValue)
    {
        int nResult = true;

        ASSERT(lua_istable(L, -1));

        lua_pushstring(L, szName);

        switch (nType)
        {
        case 'f':
		    lua_pushnumber(L, *(float*)pValue);
            break;
        case 'i':
            lua_pushnumber(L, *(int*)pValue);
            break;
        case 'b':
			lua_pushboolean(L, *(int*)pValue);
            break;
        case 's':
			lua_pushstring(L, (char const*)pValue);
            break;
        default:
            nResult = false;
            break;
        }

        lua_settable(L, -3);

        return nResult;
    }

    static int GetLuaTableField(lua_State* L, int nType, char const* szName, void* pValue)
    {
        int nResult = true;

        lua_pushstring(L, szName);
        lua_gettable(L, -2);

        switch (nType)
        {
        case 'f':
		    *(float*)pValue = (float)lua_tonumber(L, -1);
            break;
        case 'i':
            *(int*)pValue = (int)lua_tonumber(L, -1);
            break;
        case 'b':
			if (lua_isboolean(L, -1))
				*(int*)pValue = lua_toboolean(L, -1);
			else
				*(int*)pValue = (int)lua_tonumber(L, -1);
            break;
        default:
            nResult = false;
            break;
        }

        lua_pop(L, 1);

        return nResult;
    }


    int KBaseScriptTable::LuaGetVideoSettings(Lua_State* L)
	{
        HRESULT hr = E_FAIL;
        KG3DDeviceSettings Settings;
		float fWidth = 1280.0f;
		float fHeight = 960.0f;

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 0);

        hr = g_pUI->m_p3DUI->GetDeviceSettings(&Settings);
        KGLOG_COM_PROCESS_ERROR(hr);

        fWidth = g_pUI->m_WndStation.GetClientWidth();
        fHeight = g_pUI->m_WndStation.GetClientHeight();

        Settings.bFullScreen = g_pUI->m_WndStation.IsFullScreen();
        Settings.bPanauision = g_pUI->m_WndStation.IsPanauision();
		Settings.bMaximize = ::GetWindowLong(g_pUI->m_hWnd, GWL_STYLE) & WS_MAXIMIZE;

        lua_newtable(L);

        SetLuaTableField(L, 'f', "Width", &fWidth);
        SetLuaTableField(L, 'f', "Height", &fHeight);
        SetLuaTableField(L, 'b', "FullScreen", &Settings.bFullScreen);
        SetLuaTableField(L, 'b', "Panauision", &Settings.bPanauision);
        SetLuaTableField(L, 'b', "ExclusiveMode", &Settings.bExclusiveMode);
        SetLuaTableField(L, 'b', "TripleBuffering", &Settings.bTripleBuffering);
        SetLuaTableField(L, 'b', "LockableBackBuffer", &Settings.bLockableBackBuffer);
        SetLuaTableField(L, 'b', "VSync", &Settings.bVSync);
		SetLuaTableField(L, 'b', "Maximize", &Settings.bMaximize);
        SetLuaTableField(L, 'i', "RefreshRate", &Settings.uRefreshRate);
        SetLuaTableField(L, 'i', "ColorDepth", &Settings.uColorDepth);
        SetLuaTableField(L, 'i', "MultiSampleType", &Settings.nMultiSampleType);
        SetLuaTableField(L, 'i', "MultiSampleQuality", &Settings.dwMultiSampleQuality);

        return 1;
Exit0:
		return 0;
	}

	int KBaseScriptTable::LuaSetVideoSettings(Lua_State* L)
	{
        int nRetCode = false;
        KG3DDeviceSettings Settings;

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);
        KGLOG_PROCESS_ERROR(lua_istable(L, -1));

        GetLuaTableField(L, 'i', "Width", &Settings.uWidth);
        GetLuaTableField(L, 'i', "Height", &Settings.uHeight);
        GetLuaTableField(L, 'b', "FullScreen", &Settings.bFullScreen);
        GetLuaTableField(L, 'b', "Panauision", &Settings.bPanauision);
        GetLuaTableField(L, 'b', "ExclusiveMode", &Settings.bExclusiveMode);
        GetLuaTableField(L, 'b', "TripleBuffering", &Settings.bTripleBuffering);
        GetLuaTableField(L, 'b', "LockableBackBuffer", &Settings.bLockableBackBuffer);
		GetLuaTableField(L, 'b', "Maximize", &Settings.bMaximize);
        GetLuaTableField(L, 'b', "VSync", &Settings.bVSync);
        GetLuaTableField(L, 'i', "RefreshRate", &Settings.uRefreshRate);
        GetLuaTableField(L, 'i', "ColorDepth", &Settings.uColorDepth);
        GetLuaTableField(L, 'i', "MultiSampleType", &Settings.nMultiSampleType);
        GetLuaTableField(L, 'i', "MultiSampleQuality", &Settings.dwMultiSampleQuality);
		
        g_pUI->m_WndStation.SetVideoSettings(&Settings);

Exit0:
		return 0;
	}

    static bool IsGB2312Char(char const* sz)
    {
        size_t len = strlen(sz);

        if (len < 2)
            return false;

        unsigned char ch1 = (unsigned char)sz[0];
        unsigned char ch2 = (unsigned char)sz[1];

        return ch1>= 176 && ch1 <= 247 && ch2 >= 160 && ch2 <= 254;
    }

    static bool IsGBKChar(char const* sz)
    {
        size_t len = strlen(sz);

        if (len < 2)
            return false;

        unsigned char ch1 = (unsigned char)sz[0];
        unsigned char ch2 = (unsigned char)sz[1];

        return ch1>= 129 && ch1 <= 254 && ch2 >= 64 && ch2 <= 254;
    }

    static bool IsGB2312String(char const* sz)
    {
        size_t len = strlen(sz);

        for (size_t n = 0; n < len; n += 2)
        {
            if (!IsGB2312Char(sz + n))
            {
                return false;
            }
        }

        return true;
    }

    static bool IsGBKString(char const* sz)
    {
        size_t len = strlen(sz);

        for (size_t n = 0; n < len; n += 2)
        {
            if (!IsGBKChar(sz + n))
            {
                return false;
            }
        }

        return true;
    }

    int KBaseScriptTable::LuaIsGB2312String(Lua_State* L)
    {
        int nResult = false;
        LPCSTR szString = NULL;

        KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

        szString = lua_tostring(L, 1);
        KGLOG_PROCESS_ERROR(szString);

        nResult = IsGB2312String(szString);

Exit0:
        lua_pushboolean(L, nResult);
        return 1;
    }

    int KBaseScriptTable::LuaIsGBKString(Lua_State* L)
    {
        int nResult = false;
        LPCSTR szString = NULL;

        KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

        szString = lua_tostring(L, 1);
        KGLOG_PROCESS_ERROR(szString);

        nResult = IsGBKString(szString);

Exit0:
        lua_pushboolean(L, nResult);
        return 1;
    }

    int KBaseScriptTable::LuaIsSimpleChineseString(Lua_State* L)
    {
        int nResult = false;
        LPCSTR szString = NULL;

        KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

        szString = lua_tostring(L, 1);
        KGLOG_PROCESS_ERROR(szString);

        nResult = IsGB2312String(szString) || IsGBKString(szString);

Exit0:
        lua_pushboolean(L, nResult);
        return 1;
    }

    int KBaseScriptTable::LuaIsDebug(Lua_State* L)
    {
        ASSERT(L);
#ifdef _DEBUG
        lua_pushboolean(L, true);
#else
        lua_pushboolean(L, false);
#endif
        return 1;
    }

    int KBaseScriptTable::LuaGetUserEnv(Lua_State* L)
    {
        int nResult = false;
        int nRetCount = 0;
        LONG lRetCode = ERROR_SUCCESS;
        CRegKey RegKey;
        TCHAR szUUID[256];
        ULONG ulUUID = _countof(szUUID);
        int nWidth = 0;
        int nHeight = 0;

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 0);

        lRetCode = RegKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Kingsoft\\JX3"), KEY_READ);
        KG_PROCESS_SUCCESS(lRetCode == ERROR_FILE_NOT_FOUND);
        KGLOG_PROCESS_ERROR(lRetCode == ERROR_SUCCESS);

        lRetCode = RegKey.QueryStringValue(_T("UUID"), szUUID, &ulUUID);
        KGLOG_PROCESS_ERROR(lRetCode == ERROR_SUCCESS);

        lua_newtable(L);

        SetLuaTableField(L, 's', "UUID", szUUID);

        nWidth = (int)g_pUI->m_WndStation.GetClientWidth();
        nHeight = (int)g_pUI->m_WndStation.GetClientHeight();

        SetLuaTableField(L, 'i', "Width", &nWidth);
        SetLuaTableField(L, 'i', "Height", &nHeight);

        nRetCount = 1;
Exit1:
        nResult = true;
Exit0:
        if (!nResult)
        {
        }
		return nRetCount;
    }


	int KBaseScriptTable::LuaGetLastError(Lua_State* L)
	{
		lua_pushstring(L, g_pUI->m_Script.GetLastError());
		return 1;
	}

	int KBaseScriptTable::LuaIsFileExist(Lua_State* L)
	{
		BOOL bExist = FALSE;
		const char * pcszFile = NULL;

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		pcszFile = Lua_ValueToString(L, 1);
		KG_PROCESS_ERROR(pcszFile);

		bExist = g_IsFileExist(pcszFile);
Exit0:
		lua_pushboolean(L, bExist);
		return 1;
	}

    int KBaseScriptTable::LuaStringFindW(lua_State *pLua)
    {
        int nRetCode = false;
        int nRetParamCount = 0;

        LPCSTR  pcszText    = NULL;
        LPCSTR  pcszPattern = NULL;
        LPCWSTR wpszResult   = NULL;

        int nParamCount = 0;
        size_t uStart = 0;
        size_t uTextLength = 0;
        size_t uResultLength = 0;
        size_t uPatternLength = 0;
        size_t uRetStart = 0;
        size_t uRetEnd = 0;

        KGLOG_PROCESS_ERROR(pLua);

        nParamCount = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 3);
       
        pcszText = lua_tolstring(pLua, 1, &uTextLength);
        KGLOG_PROCESS_ERROR(pcszText);

        pcszPattern = lua_tolstring(pLua, 2, &uPatternLength);
        KGLOG_PROCESS_ERROR(pcszPattern);

        if (nParamCount == 3)
        {
            nRetCode = lua_isnumber(pLua, 3);
            KGLOG_PROCESS_ERROR(nRetCode);
            uStart = (size_t)lua_tonumber(pLua, 3);
            KGLOG_PROCESS_ERROR(uStart >= 1);
            uStart--;
            KG_PROCESS_ERROR(uStart < uTextLength);
        }

        {
            ATL::CA2W wszText(pcszText + uStart, GetCodePage());
            ATL::CA2W wszPattern(pcszPattern, GetCodePage());
            wpszResult = wcsstr(wszText, wszPattern);
            KG_PROCESS_ERROR(wpszResult);

            {
                ATL::CW2A pcszResult(wpszResult, GetCodePage());
                uResultLength = strlen(pcszResult);
            }
        }

        uRetStart = uTextLength - uResultLength;
        uRetEnd = uRetStart + uPatternLength - 1;

        // C Index from 0, but lua Index from 1
        uRetStart++;
        uRetEnd++;

        lua_pushnumber(pLua, uRetStart);
        lua_pushnumber(pLua, uRetEnd);
        nRetParamCount += 2;
        
Exit0:
        return nRetParamCount;
    }

    int KBaseScriptTable::LuaStringReplaceW(lua_State *pLua)
    {
        int nRetCode = false;
        int nRetParamCount = 0;

        LPCSTR pcszText    = NULL;
        LPCSTR pcszPattern = NULL;
        LPCSTR pcszReplace = NULL;
        LPWSTR wpszFind     = NULL;
        LPWSTR wpszCurText  = NULL;

        WCHAR wcTemp;
        WCHAR wszResult[MAX_WIDE_STRING_SIZE];

        size_t uResultLength = 0;
        size_t uReplaceLength = 0;
        size_t uCurResultLength = 0;
        size_t uPatternLength = 0;
        size_t uLength = 0;

        KGLOG_PROCESS_ERROR(pLua);

        nRetCode = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nRetCode == 3);

        pcszText = lua_tostring(pLua, 1);
        KGLOG_PROCESS_ERROR(pcszText);

        pcszPattern = lua_tostring(pLua, 2);
        KGLOG_PROCESS_ERROR(pcszPattern);

        pcszReplace = lua_tostring(pLua, 3);
        KGLOG_PROCESS_ERROR(pcszReplace);

        {
            ATL::CA2W wszText(pcszText, GetCodePage());
            wpszCurText = wszText.m_psz;

            ATL::CA2W wszPattern(pcszPattern, GetCodePage());
            uPatternLength = wcslen(wszPattern);

            ATL::CA2W wszReplace(pcszReplace, GetCodePage());
            uReplaceLength = wcslen(wszReplace);

            uCurResultLength = 0;
            while (true)
            {
                wpszFind = (LPWSTR)wcsstr(wpszCurText, wszPattern);
                if (!wpszFind)
                    break;
                wcTemp = *wpszFind;
                *wpszFind = L'\0';

                uLength = (int)wcslen(wpszCurText);

                KGLOG_PROCESS_ERROR(uCurResultLength + uLength + uReplaceLength < _countof(wszResult));
                nRetCode = _snwprintf(
                    &wszResult[uCurResultLength], 
                    _countof(wszResult) - uCurResultLength, 
                    L"%s%s", 
                    wpszCurText, 
                    wszReplace.m_psz
                );
                KGLOG_PROCESS_ERROR(nRetCode == uLength + uReplaceLength);

                uCurResultLength += uLength + uReplaceLength;

                *wpszFind = wcTemp;

                wpszCurText = wpszFind + uPatternLength;
            }

            if (wpszCurText && wpszCurText[0])
            {
                uLength = (int)wcslen(wpszCurText);

                KGLOG_PROCESS_ERROR(uCurResultLength + uLength < _countof(wszResult));
                nRetCode = _snwprintf(
                    &wszResult[uCurResultLength], 
                    _countof(wszResult) - uCurResultLength, 
                    L"%s", 
                    wpszCurText
                );
                KGLOG_PROCESS_ERROR(nRetCode == uLength);
            }

            wszResult[_countof(wszResult) - 1] = L'\0';

            {
                ATL::CW2A pcszResult(wszResult, GetCodePage());
                lua_pushstring(pLua, pcszResult);
                nRetParamCount++;
            }
        }
       
Exit0:
        return nRetParamCount;
    }

	int KBaseScriptTable::LuaStringLowerW(lua_State *pLua)
	{
		const char * pcszString = NULL;

		KGLOG_PROCESS_ERROR(lua_gettop(pLua) == 1);

		pcszString = Lua_ValueToString(pLua, 1);
		KG_PROCESS_ERROR(pcszString);

		{
			ATL::CA2W wszResult(pcszString, GetCodePage());
			wcslwr(wszResult);

			ATL::CW2A szResult(wszResult, GetCodePage());

			lua_pushstring(pLua, szResult);
			return 1;
		}

Exit0:
		return 0;
	}

    int KBaseScriptTable::LuaGetNumberBit(lua_State *pLua)
    {
        int nRetCode = false;
        int nRetParamCount = 0;

        int nNumber = 0;
        int nBit = 0;
        int nValue = 0;

        KGLOG_PROCESS_ERROR(pLua);

        nRetCode = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nRetCode == 2);

        nRetCode = lua_isnumber(pLua, 1);
        KGLOG_PROCESS_ERROR(nRetCode);
        nNumber = (int)lua_tonumber(pLua, 1);

        nRetCode = lua_isnumber(pLua, 2);
        KGLOG_PROCESS_ERROR(nRetCode);
        nBit = (int)lua_tonumber(pLua, 2);

        nValue = nNumber & (1 << (nBit - 1));

        lua_pushboolean(pLua, nValue);
        nRetParamCount++;
Exit0:
        return nRetParamCount;

    }
	
    int KBaseScriptTable::LuaSetNumberBit(lua_State *pLua)
    {
        int nRetCode = false;
        int nRetParamCount = 0;

        int nNumber = 0;
        int nBit = 0;
        int nValue = false;

        KGLOG_PROCESS_ERROR(pLua);

        nRetCode = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nRetCode == 3);

        nRetCode = lua_isnumber(pLua, 1);
        KGLOG_PROCESS_ERROR(nRetCode);
        nNumber = (int)lua_tonumber(pLua, 1);

        nRetCode = lua_isnumber(pLua, 2);
        KGLOG_PROCESS_ERROR(nRetCode);
        nBit = (int)lua_tonumber(pLua, 2);

        nRetCode = lua_isboolean(pLua, 3);
        KGLOG_PROCESS_ERROR(nRetCode);
        nValue = (int)lua_toboolean(pLua,3);
        
        nNumber = nNumber | (nValue << (nBit - 1));

        lua_pushnumber(pLua, nNumber);
        nRetParamCount++;
Exit0:
        return nRetParamCount;

    }

	int KBaseScriptTable::LuaMoneyToGoldSilverAndCopper(Lua_State* L)
	{
		int nMoney = 0;
		int nGold = 0;
		int nSilver = 0;
		int nCopper = 0;

		KGLOG_PROCESS_ERROR(lua_gettop(L) == 1);

		nMoney = (int)Lua_ValueToNumber(L, 1);
		
		if (nMoney >= 0)
		{
			nGold = nMoney / 10000;
			nSilver = (nMoney % 10000) / 100;
			nCopper = nMoney % 100;
		}
		else
		{
			nMoney = -nMoney;
			nGold = nMoney / 10000;
			nSilver = (nMoney % 10000) / 100;
			nCopper = nMoney % 100;
			if (nGold)
				nGold = -nGold;
			else if (nSilver)
				nSilver = -nSilver;
			else
				nCopper = -nCopper;
		}
Exit0:
		lua_pushnumber(L, nGold);
		lua_pushnumber(L, nSilver);
		lua_pushnumber(L, nCopper);
		return 3;
	}

    int KBaseScriptTable::LuaFormatString(lua_State *pLua)
    {
        int nResult = false;
        int nRetCode = false;

        const char *pcszFormat = NULL;
        int nParemCount = 0;
        int nRetParamCount = 0;

        KG_EXPRESSION_PARAM aLuaParam[20];
        char szBuff[1024];
        int nCurLengh = 0;
        int nResultLengh = 0;
        int nExpBeginIndex = -1;
        int nEscapingFlag = false;
        int nEscapingCount = 0;
        int i = 0;

        KGLOG_PROCESS_ERROR(pLua);

        szBuff[0] = '\0';

        nParemCount = lua_gettop(pLua);
        KGLOG_PROCESS_ERROR(nParemCount > 1);	// topindex = format + param_count
        KGLOG_PROCESS_ERROR(nParemCount <= _countof(aLuaParam) + 1);

        pcszFormat = lua_tostring(pLua, 1);
        KGLOG_PROCESS_ERROR(pcszFormat);

        for (i = 0; i < nParemCount - 1; i++)
        {
            int nLuaParamType = lua_type(pLua, i + 2);
            switch (nLuaParamType)
            {
            case LUA_TNUMBER:
                aLuaParam[i].eParamType = EXPRESSION_PARAM_TYPE_FLOAT;
                aLuaParam[i].fParam = (float)lua_tonumber(pLua, i + 2);
                break;
            case LUA_TSTRING:
                aLuaParam[i].eParamType = EXPRESSION_PARAM_TYPE_STRING;
                aLuaParam[i].pcszParam = lua_tostring(pLua, i + 2);
                KGLOG_PROCESS_ERROR(aLuaParam[i].pcszParam);
                break;
            case LUA_TNIL:
                aLuaParam[i].eParamType = EXPRESSION_PARAM_TYPE_STRING;
                aLuaParam[i].pcszParam = "";
                break;
            default:
                aLuaParam[i].eParamType = EXPRESSION_PARAM_TYPE_ERROR;
                KGLogPrintf(KGLOG_ERR, "error param type: index=%d, type=%d", i + 1, nLuaParamType);
                KGLOG_PROCESS_ERROR(false);
                break;
            }
        }

        nExpBeginIndex = -1;
        for (i = 0; pcszFormat[i] != '\0'; i++)
        {
            switch (pcszFormat[i])
            {
            case '<':
                if (
                    pcszFormat[i + 1] == 'D' && 
                    pcszFormat[i + 2] >= '0' && 
                    pcszFormat[i + 2] <= '9' && 
                    pcszFormat[i + 3] == '>'
                )
                {
                    const char *pcszData = lua_tostring(pLua, pcszFormat[i + 2] - '0' + 2);
                    if (pcszData && pcszData[0])
                    {
                        int nDataLength = (int)strlen(pcszData);
                        KGLOG_PROCESS_ERROR(nCurLengh + nDataLength < sizeof(szBuff));
                        memcpy(szBuff + nCurLengh, pcszData, nDataLength);
                        nCurLengh += nDataLength;
                    }

                    i += 3;
                    continue;
                }
                break;
            case '\\':
                if (
                    nExpBeginIndex == -1 &&
                    (pcszFormat[i + 1] == '{' || pcszFormat[i + 1] == '}' || pcszFormat[i + 1] == '<' || pcszFormat[i + 1] == '>')
                )
                {
                    i++;
                }
                break;
            case '{':
                if (i == 0 || pcszFormat[i - 1] != '\\')
                {
                    KGLOG_PROCESS_ERROR(nExpBeginIndex == -1);
                    nExpBeginIndex = i;
                    continue;
                }
                break;
            case '}':
                if (nExpBeginIndex == -1)
                    break;

                ASSERT(i > 0);
                if (pcszFormat[i - 1] == '\\')
                    continue;

                nRetCode = CalculateExpression(
                    pcszFormat + nExpBeginIndex + 1, i - nExpBeginIndex -1,
                    aLuaParam, nParemCount - 1,
                    szBuff + nCurLengh, sizeof(szBuff) - nCurLengh,
                    &nResultLengh
                );
                KGLOG_PROCESS_ERROR(nRetCode);
                nCurLengh += nResultLengh;
                nExpBeginIndex = -1;
                continue;
            }

            if (nExpBeginIndex == -1)
            {
                KGLOG_PROCESS_ERROR(nCurLengh + 1 < (int)sizeof(szBuff));
                szBuff[nCurLengh] = pcszFormat[i];
                nCurLengh++;
            }
        }

// Exit1:
        nEscapingCount = 0;
        for (i = 0; i < nCurLengh; i++)
        {
            if (!nEscapingFlag && szBuff[i] == '\\')
            {
                nEscapingCount++;
                nEscapingFlag = true;
                continue;
            }
            szBuff[i - nEscapingCount] = szBuff[i];
            nEscapingFlag = false;
        }
        szBuff[nCurLengh - nEscapingCount] = '\0';

        szBuff[nCurLengh] = '\0';
        lua_pushstring(pLua, szBuff);
        nRetParamCount++;

        nResult = true;
Exit0:
        if (!nResult)
        {
            if (nParemCount > 0)
                pcszFormat = lua_tostring(pLua, 1);

            if (!pcszFormat)
                pcszFormat = "";

            szBuff[nCurLengh] = '\0';
            KGLogPrintf(
                KGLOG_ERR, 
                "Format: %s, TopIndex: %d, szResult : %s\n", 
                pcszFormat, 
                nParemCount, 
                szBuff
            );
        }
        return nRetParamCount;
    }


}


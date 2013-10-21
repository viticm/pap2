#include "stdafx.h"
#include "UpdatePakSfx.h"
#include <Windows.h>
#include <locale.h>
#include <io.h>
#include <time.h>
#include "../Common/unzip.h"
#include "../Common/KRegInfo.h"
#include "../Common/KProcess.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	int nRetCode = false;
	int nFindNum = 0;

	_tsetlocale(LC_ALL, _T(""));
	
	if (lpCmdLine[0] != _T('\0'))
	{
		nRetCode = _tcsicmp(lpCmdLine, _T("-v"));
		if (nRetCode == 0)
			ShowPackInfo();
		goto Exit0;
	}		

	nRetCode = GetPackVersionInfo();
	if (!nRetCode)
	{
		MessageBox(NULL, _T("更新出错！无法确定更新包版本信息，可能更新包有错！"), _T("提示信息"), MB_OK | MB_ICONERROR);
		goto Exit0;
	}

	nRetCode = LocateInstallPath();
	if (!nRetCode)
	{
		MessageBox(NULL, _T("请将更新包复制到游戏安装目录中！"), _T("提示信息"), MB_OK | MB_ICONINFORMATION);
		goto Exit0;
	}

	nRetCode = CheckVersion();
	if (!nRetCode)
	{
		MessageBox(NULL, _T("更新出错！本更新包不适合您当前版本！"), _T("提示信息"), MB_OK | MB_ICONERROR);
		goto Exit0;
	}

	nRetCode = KProcess::FindProcess(g_szGameupdaterFile, &nFindNum);
	KG_PROCESS_ERROR(nRetCode);
	if (nFindNum > 0)
	{
		MessageBox(NULL, _T("更新出错！请检查游戏客户端或更新程序是否正在运行！"), _T("提示信息"), MB_OK | MB_ICONERROR);
		goto Exit0;
	}

	nRetCode = ExtractGameupdater();
	if (!nRetCode)
	{
		MessageBox(NULL, _T("更新出错！请检查游戏客户端或更新程序是否正在运行！"), _T("提示信息"), MB_OK | MB_ICONERROR);
		goto Exit0;
	}

	nRetCode = Update();
	KG_PROCESS_ERROR(nRetCode);

Exit0:
	return 0;
}

int GetPackVersionInfo()
{
	int nResult  = FALSE;
	int nRetCode = FALSE;

	int nItemIndex = -1;
	HZIP hZip = NULL;
	ZRESULT zipRet = ZR_OK;
	ZIPENTRY ze;
	TCHAR szVerionFile[MAX_PATH];
	TCHAR szTargetFile[MAX_PATH];
	TCHAR szTmpPath[MAX_PATH];

	nRetCode = GetModuleFileName(NULL, g_szUpdatePakFile, MAX_PATH);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = GetTempPath(MAX_PATH, szTmpPath);
	KG_PROCESS_ERROR(nRetCode);

	_sntprintf(szTargetFile, MAX_PATH, _T("%s\\version_%d.cfg"), szTmpPath, time(NULL));
	szTargetFile[MAX_PATH - 1] = _T('\0');

	_sntprintf(szVerionFile, MAX_PATH, _T("%s/%s"), PACK_UPDATE_FILE_PATH, GAME_VERSION_FILE);
	szVerionFile[MAX_PATH - 1] = _T('\0');

	hZip = OpenZip(g_szUpdatePakFile, "");
	KG_PROCESS_ERROR(hZip);
	
	FindZipItem(hZip, szVerionFile, true, &nItemIndex, &ze);
	if (nItemIndex != -1)
	{
		zipRet = UnzipItem(hZip, nItemIndex, szTargetFile);
		if (zipRet != ZR_OK)
		{			
			goto Exit0;
		}
	}

	nRetCode = ParseVersionInfo(szTargetFile, g_szPackVersionLineName, g_szPackVersionEx);
	KG_PROCESS_ERROR(nRetCode);
	KG_PROCESS_ERROR(g_szPackVersionLineName[0] != _T('\0'));

	nResult = TRUE;
Exit0:
	if (hZip)
	{
		CloseZip(hZip);
		hZip = NULL;
	}
	_tremove(szTargetFile);
	return nResult;
}

int LocateInstallPath()
{
	int nResult  = FALSE;
	int nRetCode = FALSE;
	TCHAR szRegPath[MAX_PATH];

	nRetCode = GetModuleFileName(NULL, g_szUpdatePakFile, MAX_PATH);
	KG_PROCESS_ERROR(nRetCode);

	if (g_szPackVersionEx[0] == _T('\0'))
	{
		_tcscpy(szRegPath, g_szPackVersionLineName);
	}
	else
	{
		_sntprintf(szRegPath, MAX_PATH, _T("%s_%s"), g_szPackVersionLineName, g_szPackVersionEx);
		szRegPath[MAX_PATH - 1] = _T('\0');
	}	

	nRetCode = KRegInfo::ReadInstallPath(szRegPath, g_szGameInstallPath, MAX_PATH);
	if (!nRetCode)
	{
		_tcscpy(g_szGameInstallPath, g_szUpdatePakFile);
		TCHAR *pSlash = _tcsrchr(g_szGameInstallPath, _T('\\'));
		if (pSlash)
		{
			*(pSlash + 1) = _T('\0');
		}

		TCHAR szVersionFile[MAX_PATH];
		_sntprintf(szVersionFile, MAX_PATH, _T("%s%s"), g_szGameInstallPath, GAME_VERSION_FILE);
		szVersionFile[MAX_PATH - 1] = _T('\0');
		nRetCode = _taccess(szVersionFile, 0);
		if (nRetCode == -1)
		{			
			goto Exit0;
		}
	}

	if (g_szGameInstallPath[_tcslen(g_szGameInstallPath) - 1] != _T('\\'))
	{
		g_szGameInstallPath[_tcslen(g_szGameInstallPath)] = _T('\\');
		g_szGameInstallPath[_tcslen(g_szGameInstallPath) + 1] = _T('\0');
	}


	_sntprintf(g_szGameupdaterFile, MAX_PATH, _T("%s%s"), g_szGameInstallPath, GAMEUPDATER_PROCESS);
	g_szGameupdaterFile[MAX_PATH - 1] = _T('\0');

	nResult = TRUE;
Exit0:
	return nResult;
}

int CheckVersion()
{
	int nResult  = FALSE;
	int nRetCode = FALSE;
	TCHAR szCurrentVersionFile[MAX_PATH];
	TCHAR szCurrentVersionLineName[MAX_PATH];
	TCHAR szCurrentVersonEx[MAX_PATH];

	_sntprintf(szCurrentVersionFile, MAX_PATH, _T("%s%s"), g_szGameInstallPath, GAME_VERSION_FILE);
	szCurrentVersionFile[MAX_PATH - 1] = _T('\0');

	nRetCode = ParseVersionInfo(szCurrentVersionFile, szCurrentVersionLineName, szCurrentVersonEx);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = _tcsncicmp(szCurrentVersionLineName, g_szPackVersionLineName, MAX_PATH);
	KG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _tcsncicmp(szCurrentVersonEx, g_szPackVersionEx, MAX_PATH);
	KG_PROCESS_ERROR(nRetCode == 0);

	nResult = TRUE;
Exit0:	
	return nResult;
}

int ExtractGameupdater()
{
	int nResult  = FALSE;
	int nRetCode = FALSE;
	
	int nExFileNum = sizeof(GAMEUPDATER_FILE_LIST) / sizeof(GAMEUPDATER_FILE_LIST[0]);
	
	HZIP hZip = NULL;
	ZRESULT zipRet = ZR_OK;
	ZIPENTRY ze;
	TCHAR szTargetFile[MAX_PATH];

	hZip = OpenZip(g_szUpdatePakFile, "");
	KG_PROCESS_ERROR(hZip);

	for (int i = 0; i < nExFileNum; ++i)
	{
		int nItemIndex = -1;
		FindZipItem(hZip, GAMEUPDATER_FILE_LIST[i], true, &nItemIndex, &ze);
		if (nItemIndex != -1)
		{
			_sntprintf(szTargetFile, MAX_PATH, _T("%s%s"), g_szGameInstallPath, ze.name);
			szTargetFile[MAX_PATH - 1] = _T('\0');
			zipRet = UnzipItem(hZip, nItemIndex, szTargetFile);
			if (zipRet != ZR_OK)
			{				
				goto Exit0;
			}
		}
	}

	nResult = TRUE;
Exit0:
	if (hZip)
	{
		CloseZip(hZip);
		hZip = NULL;
	}
	return nResult;
}

int Update()
{
	int nRetCode = FALSE;
	int nResult  = FALSE;
	TCHAR szLauncher[MAX_PATH];
	
	STARTUPINFO StartInfo = {sizeof(STARTUPINFO)};
	PROCESS_INFORMATION ProcessInfo;

	nRetCode = _taccess(g_szGameupdaterFile, 0);
	if (nRetCode == -1) //文件不存在
	{
		TCHAR szInfo[512];
		_sntprintf(szInfo, 512, _T("无法启动更新！\n\n未找到更新程序\n%s"), g_szGameupdaterFile);
		MessageBox(NULL, szInfo, _T("提示信息"), MB_OK | MB_ICONERROR);
		goto Exit0;
	}

	_sntprintf(szLauncher, MAX_PATH, _T("\"%s\" \"%s\""), g_szGameupdaterFile, g_szUpdatePakFile);
	szLauncher[MAX_PATH - 1] = _T('\0');

	try
	{
		GetStartupInfo(&StartInfo);
	}
	catch (...)
	{
		goto Exit0;
	}

	nRetCode = CreateProcess(
		NULL,
		(LPTSTR)szLauncher,
		NULL, 
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL, 
		&StartInfo, 
		&ProcessInfo
	);
	KG_PROCESS_ERROR(nRetCode);

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);

	nResult = TRUE;
Exit0:
	if (!nResult)
	{
		MessageBox(NULL, _T("更新出错！"), _T("提示信息"), MB_OK | MB_ICONERROR);
	}
	return nResult;
}

int ShowPackInfo()
{
	TCHAR szPackInfo[512];
	int nExFileNum = sizeof(GAMEUPDATER_FILE_LIST) / sizeof(GAMEUPDATER_FILE_LIST[0]);
	
	_sntprintf(
		szPackInfo, 512,
		_T("CompileDate:   %s %s\n\nExtractList:\n"),
		_T(__DATE__),
		_T(__TIME__)
	);

	for (int i = 0; i < nExFileNum; ++i)
	{
		if (_tcslen(szPackInfo) + _tcslen(GAMEUPDATER_FILE_LIST[i]) < 512)
		{
			_tcscat(szPackInfo, GAMEUPDATER_FILE_LIST[i]);
			_tcscat(szPackInfo, _T("\n"));
		}
	}	
	szPackInfo[sizeof(szPackInfo) / sizeof(szPackInfo[0]) - 1] = _T('\0');
	
	MessageBox(NULL, szPackInfo, _T("PackInfo"), MB_OK | MB_ICONINFORMATION);

	return TRUE;
}

int ParseVersionInfo(const TCHAR szVersionFile[], TCHAR *pszVersionLineName, TCHAR *pszVersionEx)
{
	int nRetCode = false;
	int nResult  = false;
	FILE* pVerFile = NULL; 
	TCHAR szBuffer[256];   

	pszVersionLineName[0] = _T('\0');
	pszVersionEx[0] = _T('\0');

	pVerFile = _tfopen(szVersionFile, _T("r"));
	KG_PROCESS_ERROR(pVerFile);	

	while (TRUE)
	{
		size_t uLength = 0;
		TCHAR *pRet = _fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), pVerFile);
		if (!pRet)
		{
			if (ferror(pVerFile))
				goto Exit0;
			else
				goto Exit1;
		}
		szBuffer[sizeof(szBuffer) / sizeof(TCHAR) - 1] = _T('\0');

		StringTrim(szBuffer);

		if (szBuffer[0] == _T('#'))
			continue;

		uLength = _tcslen(PRODUCT_VERSION_LINE_NAME_KEY);
		nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_LINE_NAME_KEY, uLength);
		if (!nRetCode && szBuffer[uLength] == _T('='))
		{
			_tcsncpy(pszVersionLineName, szBuffer + uLength + 1, MAX_PATH);
			pszVersionLineName[MAX_PATH - 1] = _T('\0');   
		}

		uLength = _tcslen(PRODUCT_VERSION_EX_KEY);
		nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_EX_KEY, uLength);
		if (!nRetCode && szBuffer[uLength] == _T('='))
		{
			_tcsncpy(pszVersionEx, szBuffer + uLength + 1, MAX_PATH);
			pszVersionEx[MAX_PATH - 1] = _T('\0');
		}
	}

Exit1:
	nResult = true;
Exit0:
	if (pVerFile)
	{
		fclose(pVerFile);
		pVerFile = NULL;
	}
	return nResult;
}

void StringTrim(TCHAR szString[])
{
	TCHAR *pSrc = szString;
	TCHAR *pDes = szString;

	while (pSrc && *pSrc)
	{
		if (
			(*pSrc != _T(' '))  &&
			(*pSrc != _T('\n')) &&
			(*pSrc != _T('\r')) &&
			(*pSrc != _T('\t'))
			)
		{
			*pDes = *pSrc;
			++pDes;
		}
		++pSrc;
	}
	*pDes = _T('\0');
}

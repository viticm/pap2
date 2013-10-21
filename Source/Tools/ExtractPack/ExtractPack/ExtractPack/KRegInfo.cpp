#include "StdAfx.h"
#include "KRegInfo.h"

const HKEY  REG_KEY = HKEY_LOCAL_MACHINE;
const TCHAR REG_SUB_KEY[] = _T("SOFTWARE\\Kingsoft\\JX3");
const TCHAR REG_INSTALL_PATH_NAME[] = _T("installPath");
const TCHAR REG_CURRENT_VERSION_NAME[] = _T("version");
const TCHAR REG_VERSION_LINE_NAME[] = _T("VersionLineName");


int KRegInfo::ReadInstallPath(const TCHAR szLang[], TCHAR szPath[], size_t nCount)
{
	int nResult    = FALSE;
	int nRetCode    = FALSE;
	HKEY hKey       = NULL;
	DWORD dwSizeReaded = (DWORD)(nCount * sizeof(TCHAR));
	TCHAR szSubKey[MAX_PATH];

	KG_PROCESS_ERROR(szLang && szLang[0] && szPath);

	_sntprintf(szSubKey, MAX_PATH, _T("%s\\%s"), REG_SUB_KEY, szLang);
	szSubKey[MAX_PATH - 1] = _T('\0');

	nRetCode = ::RegOpenKeyEx(REG_KEY, szSubKey, 0, KEY_READ, &hKey);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nRetCode = ::RegQueryValueEx(hKey, REG_INSTALL_PATH_NAME, NULL, NULL, (LPBYTE)szPath, &dwSizeReaded);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nResult = TRUE;
Exit0:
	if (hKey)
	{
		::RegCloseKey(hKey);
		hKey = NULL;
	}
	return nResult;
}

int KRegInfo::WriteInstallPath(const TCHAR szLang[], const TCHAR szInstallPath[])
{
	int nResult     = FALSE;
	int nRetCode    = FALSE;
	HKEY hKey       = NULL;
	DWORD dwDisp    = 0;
	TCHAR szSubKey[MAX_PATH];

	KG_PROCESS_ERROR(szInstallPath && szInstallPath[0]);
	KG_PROCESS_ERROR(szLang && szLang[0]);

	_sntprintf(szSubKey, MAX_PATH, _T("%s\\%s"), REG_SUB_KEY, szLang);
	szSubKey[MAX_PATH - 1] = _T('\0');

	nRetCode = ::RegCreateKeyEx(
		REG_KEY, 
		szSubKey, 
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, NULL, &hKey, &dwDisp
	);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nRetCode = ::RegSetValueEx(
		hKey, REG_INSTALL_PATH_NAME, 0, REG_SZ, (BYTE *)szInstallPath, 
		(DWORD)(_tcslen(szInstallPath) * sizeof(szInstallPath[0]))
    );
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nResult = true;
Exit0:
	if (hKey)
	{
		::RegCloseKey(hKey);
		hKey = NULL;
	}
	return nResult;
}

int KRegInfo::ReadCurrentVersion(const TCHAR szLang[], TCHAR szVersion[], size_t nCount)
{
	int nResult  = FALSE;
	int nRetCode    = FALSE;
	HKEY hKey       = NULL;
	DWORD dwSizeReaded = (DWORD)(nCount * sizeof(TCHAR));
	TCHAR szSubKey[MAX_PATH];

	KG_PROCESS_ERROR(szLang && szLang[0] && szVersion);

	_sntprintf(szSubKey, MAX_PATH, _T("%s\\%s"), REG_SUB_KEY, szLang);
	szSubKey[MAX_PATH - 1] = _T('\0');

	nRetCode = ::RegOpenKeyEx(REG_KEY, szSubKey, 0, KEY_READ, &hKey);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nRetCode = ::RegQueryValueEx(hKey, REG_CURRENT_VERSION_NAME, NULL, NULL, (LPBYTE)szVersion, &dwSizeReaded);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nResult = TRUE;
Exit0:
	if (hKey)
	{
		::RegCloseKey(hKey);
		hKey = NULL;
	}
	return nResult;
}

int KRegInfo::WriteCurrentVersion(const TCHAR szLang[], const TCHAR szCurrentVersion[])
{
	int nResult     = FALSE;
	int nRetCode    = FALSE;
	HKEY hKey       = NULL;
	DWORD dwDisp    = 0;
	TCHAR szSubKey[MAX_PATH];

	KG_PROCESS_ERROR(szLang && szLang[0]);
	KG_PROCESS_ERROR(szCurrentVersion && szCurrentVersion[0]);	

	_sntprintf(szSubKey, MAX_PATH, _T("%s\\%s"), REG_SUB_KEY, szLang);
	szSubKey[MAX_PATH - 1] = _T('\0');

	nRetCode = ::RegCreateKeyEx(
		REG_KEY, 
		szSubKey, 
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, NULL, &hKey, &dwDisp
	);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nRetCode = ::RegSetValueEx(
		hKey, REG_CURRENT_VERSION_NAME, 0, REG_SZ, (BYTE *)szCurrentVersion, 
		(DWORD)(_tcslen(szCurrentVersion) * sizeof(szCurrentVersion[0]))
	);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	nResult = true;
Exit0:
	if (hKey)
	{
		::RegCloseKey(hKey);
		hKey = NULL;
	}
	return nResult;
}
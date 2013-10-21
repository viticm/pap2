
#include "KTestCommon.h"

#include <Tlhelp32.h>
#include <io.h>

TCHAR g_szAppPath[MAX_PATH];
TCHAR g_szTestingResDir[MAX_PATH];
TCHAR g_szTestingSpaceDir[MAX_PATH];
TCHAR g_szRegInstallPath[MAX_PATH];
TCHAR g_szRegVersion[MAX_VERSION_LENGTH];

int KTestCommon::PrepareTestResoure(const TCHAR cszResName[], const TCHAR cszTestName[])
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTestResFile[MAX_PATH];	//资源文件 
	TCHAR szFilePath[MAX_PATH];		//复制后的资源文件

	ASSERT(cszResName);
	ASSERT(cszTestName);

	//构造资源文件名
	nRetCode = _sntprintf(szTestResFile, MAX_PATH, _T("%s\\%s"), g_szTestingResDir, cszResName);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestResFile[MAX_PATH - 1] = _T('\0');

	//szTestName为空串时，以原资源名为测试目标名称，否则以szTestName为测试目标名称
	if (cszTestName[0] == _T('\0'))
	{
		nRetCode = _sntprintf(szFilePath, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, cszResName);
	}
	else
	{
		nRetCode = _sntprintf(szFilePath, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, cszTestName);
	}
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szFilePath[MAX_PATH - 1] = _T('\0');

	nRetCode = ::CopyFile(szTestResFile, szFilePath, true);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestCommon::CleanPath(const TCHAR cszPath[])
{
	int nRetCode = false;
	int nResult	 = false;
	TCHAR szPathName[MAX_PATH];
	TCHAR szFullPathFile[MAX_PATH];
	TCHAR* pszFile	 = NULL;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;

	ASSERT(cszPath);

	nRetCode = _sntprintf(szPathName, MAX_PATH, _T("%s\\%s"), cszPath, _T("*.*"));
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szPathName[MAX_PATH - 1] = _T('\0');

	hFindFile = ::FindFirstFile(szPathName, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		pszFile = FindFileData.cFileName;
		if (_tcsncmp(pszFile, _T("."), MAX_PATH) != 0		&&
			_tcsncmp(pszFile, _T("logs"), MAX_PATH) != 0	&&
			_tcsncmp(pszFile, _T(".."), MAX_PATH) != 0)
		{
			nRetCode = _sntprintf(szFullPathFile, MAX_PATH, _T("%s\\%s"), cszPath, pszFile);
			KGLOG_PROCESS_ERROR(nRetCode != -1);
			szFullPathFile[MAX_PATH - 1] = _T('\0');
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = _tremove(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode == 0);
			}
			else
			{
				nRetCode = CleanPath(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);
				nRetCode = ::RemoveDirectory(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);
			}
		}
		nRetCode = ::FindNextFile(hFindFile, &FindFileData);
		if (!nRetCode)
		{
			nRetCode = ::GetLastError();
			KG_PROCESS_SUCCESS(nRetCode == ERROR_NO_MORE_FILES);
		}
	}

Exit1:
	nResult = true;
Exit0:
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KTestCommon::CleanPathExceptLogAndPak(const TCHAR cszPath[])
{
	int nRetCode = false;
	int nResult	 = false;
	TCHAR szPathName[MAX_PATH];
	TCHAR szFullPathFile[MAX_PATH];
	TCHAR* pszFile	 = NULL;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;

	ASSERT(cszPath);

	nRetCode = _sntprintf(szPathName, MAX_PATH, _T("%s\\%s"), cszPath, _T("*.*"));
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szPathName[MAX_PATH - 1] = _T('\0');

	hFindFile = ::FindFirstFile(szPathName, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		pszFile = FindFileData.cFileName;
		if (_tcsncmp(pszFile, _T("."), MAX_PATH)   != 0  &&
			_tcsncmp(pszFile, _T(".."), MAX_PATH)  != 0	 &&
			_tcsncmp(pszFile, _T("logs"), MAX_PATH) != 0	 &&
			_tcsncmp(pszFile, _T("pak"), MAX_PATH) != 0)
		{
			nRetCode = _sntprintf(szFullPathFile, MAX_PATH, _T("%s\\%s"), cszPath, pszFile);
			KGLOG_PROCESS_ERROR(nRetCode != -1);
			szFullPathFile[MAX_PATH - 1] = _T('\0');
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = _tremove(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode == 0);
			}
			else
			{
				nRetCode = CleanPath(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);
				nRetCode = ::RemoveDirectory(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);
			}
		}

		if (_tcsncmp(pszFile, _T("pak"), MAX_PATH) == 0)
		{
			nRetCode = _sntprintf(szFullPathFile, MAX_PATH, _T("%s\\pak\\update_2.pak"), cszPath);
			KGLOG_PROCESS_ERROR(nRetCode != -1);
			szFullPathFile[MAX_PATH - 1] = _T('\0');
			
			nRetCode = _tremove(szFullPathFile);
			//KGLOG_PROCESS_ERROR(nRetCode == 0);
		}

		nRetCode = ::FindNextFile(hFindFile, &FindFileData);
		if (!nRetCode)
		{
			nRetCode = ::GetLastError();
			KG_PROCESS_SUCCESS(nRetCode == ERROR_NO_MORE_FILES);
		}
	}

Exit1:
	nResult = true;
Exit0:
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KTestCommon::SetAllFilesAttributes(const TCHAR cszPath[], DWORD dwFileAttributes)
{
	int nRetCode = false;
	int nResult	 = false;
	TCHAR szPathName[MAX_PATH];
	TCHAR szFullPathFile[MAX_PATH];
	TCHAR* pszFile	 = NULL;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;

	ASSERT(cszPath);

	nRetCode = _sntprintf(szPathName, MAX_PATH, _T("%s\\%s"), cszPath, _T("*.*"));
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szPathName[MAX_PATH - 1] = _T('\0');

	hFindFile = ::FindFirstFile(szPathName, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		pszFile = FindFileData.cFileName;
		if (_tcsncmp(pszFile, _T("."), MAX_PATH) != 0 &&
			_tcsncmp(pszFile, _T(".."), MAX_PATH) != 0)
		{
			nRetCode = _sntprintf(szFullPathFile, MAX_PATH, _T("%s\\%s"), cszPath, pszFile);
			KGLOG_PROCESS_ERROR(nRetCode != -1);
			szFullPathFile[MAX_PATH - 1] = _T('\0');
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				//nRetCode = _tremove(szFullPathFile);
				nRetCode = SetFileAttributes(szFullPathFile, dwFileAttributes);
				KGLOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
				nRetCode = SetAllFilesAttributes(szFullPathFile, dwFileAttributes);
				KGLOG_PROCESS_ERROR(nRetCode);
			}
		}
		nRetCode = ::FindNextFile(hFindFile, &FindFileData);
		if (!nRetCode)
		{
			nRetCode = ::GetLastError();
			KG_PROCESS_SUCCESS(nRetCode == ERROR_NO_MORE_FILES);
		}
	}

Exit1:
	nResult = true;
Exit0:
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KTestCommon::SaveRegData()
{
 	int nResult  = false;
 	int nRetCode = false;
 	HKEY hKey = NULL;
 	DWORD dwPathSize = (DWORD)(MAX_PATH * sizeof(TCHAR));
	DWORD dwVersionSize = (DWORD)(MAX_VERSION_LENGTH * sizeof(TCHAR));

 	//打开注册表键
 	nRetCode = ::RegOpenKeyEx(REG_ROOT_KEY, REG_SUB_KEY, 0, KEY_READ, &hKey);
 	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

 	//查询并记录安装目录的注册表项的值
 	nRetCode = ::RegQueryValueEx(hKey, REG_INSTALL_PATH_NAME, NULL, NULL, (LPBYTE)g_szRegInstallPath, &dwPathSize);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

	//查询并记录版本信息的注册表项的值
	nRetCode = ::RegQueryValueEx(hKey, REG_CURRENT_VERSION_NAME, NULL, NULL, (LPBYTE)g_szRegVersion, &dwVersionSize);
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

 int KTestCommon::RestoreRegData()
 {
 	int nResult  = false;
 	int nRetCode = false;
 	HKEY hKey = NULL;
 
 	//打开注册表键
 	nRetCode = ::RegOpenKeyEx(REG_ROOT_KEY, REG_SUB_KEY, 0, KEY_SET_VALUE, &hKey);
 	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);
 
	//恢复安装目录注册表数据
 	nRetCode = ::RegSetValueEx(
					hKey,
					REG_INSTALL_PATH_NAME,
					0,
					REG_SZ,
					(LPBYTE)g_szRegInstallPath, 
					MAX_PATH
				 );
 	if(nRetCode != ERROR_SUCCESS)
	{
		KGLogPrintf(KGLOG_ERR, "Restore Reg Install Path Failed");
	}
	//恢复版本信息注册表数据
	nRetCode = ::RegSetValueEx(
					hKey,
					REG_CURRENT_VERSION_NAME,
					0,
					REG_SZ,
					(LPBYTE)g_szRegVersion, 
					MAX_VERSION_LENGTH
				 );
	if(nRetCode != ERROR_SUCCESS)
	{
		KGLogPrintf(KGLOG_ERR, "Restore Reg Version Failed");
	}
 
 	nResult = true;
 Exit0:
 	if (hKey)
 	{
 		::RegCloseKey(hKey);
 		hKey = NULL;
 	}
 	return nResult;
}

int KTestCommon::ModifyRegKey(const TCHAR cszValueName[], const TCHAR cszValue[])
{
	int nResult  = false;
	int nRetCode = false;
	HKEY hKey = NULL;

	//打开注册表键
	nRetCode = ::RegOpenKeyEx(REG_ROOT_KEY, REG_SUB_KEY, 0, KEY_SET_VALUE, &hKey);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);
	//修改指定注册表值项
	nRetCode = ::RegSetValueEx(
					 hKey,
					 cszValueName,
					 0,
					 REG_SZ,
					 (LPBYTE)cszValue, 
					 (DWORD)(_tcslen(cszValue) * sizeof(cszValue[0]))
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

int KTestCommon::DelRegValue(const TCHAR cszValueName[])
{
	int nResult  = false;
	int nRetCode = false;
	HKEY hKey = NULL;

	ASSERT(cszValueName);

	//打开注册表键
	nRetCode = ::RegOpenKeyEx(REG_ROOT_KEY, REG_SUB_KEY, 0, KEY_SET_VALUE, &hKey);
	KG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);
	//删除该值项
	nRetCode = ::RegDeleteValue(hKey, cszValueName);
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

int KTestCommon::ModifyVersionFile(const TCHAR cszFilePath[], const TCHAR cszVersion[])
{
	int nResult  = false;
	int nRetCode = false;
	FILE*  pVerFile = NULL;
	TCHAR* pszRet   = NULL;
	size_t uLength = 0;
	long lOffset = 0L;
	TCHAR szBuffer[MAX_PATH];

	ASSERT(cszFilePath);
	ASSERT(cszVersion);

	//修改版本信息文件中的版本号，用于测试需要
	nRetCode = _tfopen_s(&pVerFile, cszFilePath, _T("r+"));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	uLength = _tcslen(PRODUCT_VERSION_KEY);
	while (TRUE)
	{
		pszRet = _fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), pVerFile);
		if (!pszRet)
		{
			nRetCode = ferror(pVerFile);
			KGLOG_PROCESS_ERROR(nRetCode == 0);
			KG_PROCESS_SUCCESS(nRetCode == 0);
		}
		szBuffer[sizeof(szBuffer) / sizeof(TCHAR) - 1] = _T('\0');
		if (szBuffer[0] != _T('#'))
		{
			nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_KEY, uLength);
			if (!nRetCode && szBuffer[uLength] == _T('='))
			{
				lOffset += (long)uLength + 1;
				nRetCode = fseek(pVerFile, lOffset, SEEK_SET); 
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				nRetCode = _fputts(cszVersion, pVerFile);
				KGLOG_PROCESS_ERROR(nRetCode != EOF && nRetCode != WEOF);
				KG_PROCESS_SUCCESS(true);
			}
		}
		lOffset += (long)_tcslen(szBuffer) + 1;
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

int KTestCommon::GetCurVersion(const TCHAR cszFilePath[], OUT TCHAR* pRetCurVersion)
{
	int nResult  = false;
	int nRetCode = false;
	FILE*  pVerFile = NULL;
	TCHAR* pszRet   = NULL;
	size_t uLength = 0;
	long lOffset = 0L;
	TCHAR szBuffer[MAX_PATH] = {0};
	static const int sc_nVersionLen = (int)_tcslen(_T("1-0-8-1505")) + 1;

	ASSERT(cszFilePath);
	ASSERT(pRetCurVersion);

	nRetCode = _tfopen_s(&pVerFile, cszFilePath, _T("r"));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	uLength = _tcslen(PRODUCT_VERSION_KEY);
	while (TRUE)
	{
		pszRet = _fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), pVerFile);
		if (!pszRet)
		{
			nRetCode = ferror(pVerFile);
			KGLOG_PROCESS_ERROR(nRetCode == 0);
			KG_PROCESS_SUCCESS(nRetCode == 0);
		}
		szBuffer[sizeof(szBuffer) / sizeof(TCHAR) - 1] = _T('\0');
		if (szBuffer[0] != _T('#'))
		{
			nRetCode = _tcsncicmp(szBuffer, PRODUCT_VERSION_KEY, uLength);
			if (!nRetCode && szBuffer[uLength] == _T('='))
			{
				lOffset += (long)uLength + 1;
				nRetCode = fseek(pVerFile, lOffset, SEEK_SET); 
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				pszRet = _fgetts(pRetCurVersion, sc_nVersionLen * sizeof(TCHAR), pVerFile);
				KGLOG_PROCESS_ERROR(pszRet);
				KG_PROCESS_SUCCESS(true);
			}
		}
		lOffset += (long)_tcslen(szBuffer) + 1;
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

int KTestCommon::ValidateFileMD5(const TCHAR cszFilePath[], const TCHAR cszMD5[MD5_LENGTH])
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szTempMD5[MD5_LENGTH];

	ASSERT(cszFilePath);
	ASSERT(cszMD5);

	nRetCode = MakdFileMD5(szTempMD5, cszFilePath);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _tcsncmp(szTempMD5, cszMD5, MD5_LENGTH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}

int KTestCommon::ValidatePackageFile()
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szPackageFileName[MAX_PATH];
	TCHAR szTempFileName[MAX_PATH];

	nRetCode = _sntprintf(szPackageFileName, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, PACKAGE_CONFIG);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szPackageFileName[MAX_PATH - 1] = _T('\0');
	//package.ini修改验证
	//0=update_2.pak
	nRetCode = ::GetPrivateProfileString(
					PACKAGE_CONFIG_SECTION, 
					PACKAGE_CONFIG_KEY0,
					NULL,
					szTempFileName,
					MAX_PATH,
					szPackageFileName
				);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _tcsncmp(szTempFileName, TEST_PACK_2, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	//1=update_1.pak
	nRetCode = ::GetPrivateProfileString(
					PACKAGE_CONFIG_SECTION, 
					PACKAGE_CONFIG_KEY1,
					NULL,
					szTempFileName,
					MAX_PATH,
					szPackageFileName
				);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _tcsncmp(szTempFileName, TEST_PACK_1, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}

int KTestCommon::IsFileExists(const TCHAR* cpszFileName)
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = _taccess(cpszFileName, 0); // 第二个参数0表示文件是否存在
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestCommon::WaitForProcessStarted(HANDLE hProcess)
{
	int nResult  = false;
	int nRetCode = false;
	int nSleepTimes  = 0;
	DWORD dwExitCode = 0;

	//每1秒查看一次进程状态，30分钟后如果还是未激活状态，就认为启动失败
	nSleepTimes = MAX_TIMEOUT / 1000;
	for (int i = 0; i < nSleepTimes; i++)
	{
		nRetCode = ::GetExitCodeProcess(hProcess, &dwExitCode);
		KGLOG_PROCESS_ERROR(nRetCode);
		KG_PROCESS_SUCCESS(dwExitCode == STILL_ACTIVE);
		::Sleep(1000);
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KTestCommon::CloseAndWaitForProcess(HANDLE hProcess)
{
	int nResult  = false;
	int nRetCode = false;
	int nSleepTimes  = 0;
	DWORD dwExitCode = 0;

	nRetCode = ::TerminateProcess(hProcess, 4);
	if (!nRetCode)
	{
		KGLogPrintf(KGLOG_ERR, "Terminate Process Failed");
		KG_PROCESS_ERROR(false);
	}

	//每1秒查看一次进程状态，MAX_TIMEOUT毫秒后如果还是激活状态，就认为关闭失败
	nSleepTimes = MAX_TIMEOUT / 1000;
	for (int i = 0; i < nSleepTimes; i++)
	{
		nRetCode = ::GetExitCodeProcess(hProcess, &dwExitCode);
		KGLOG_PROCESS_ERROR(nRetCode);
		KG_PROCESS_SUCCESS(dwExitCode != STILL_ACTIVE);
		::Sleep(1000);
	}

Exit1:
	nResult = true;
Exit0:
	::CloseHandle(hProcess);
	return nResult;
}




bool KTestCommon::_GetPidByName(IN const TCHAR* lpName, OUT DWORD& dwPid)
{
	HANDLE hProcessSnap		= NULL; 
	bool bResult			= false; 
	PROCESSENTRY32 pe32     = {0}; 
	HANDLE hProcess			= INVALID_HANDLE_VALUE;

	if (lpName == NULL)
	{
		return false;
	}

	hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		goto Exit0;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32); 

	if (::Process32First(hProcessSnap, &pe32)) 
	{  
		do 
		{
			if(0 == _tcsicmp(pe32.szExeFile, lpName))
			{
				dwPid = pe32.th32ProcessID;

				bResult = true;
				goto Exit0;
			}
		} 
		while (::Process32Next(hProcessSnap, &pe32)); 
	} 

Exit0:
	if (hProcessSnap != INVALID_HANDLE_VALUE )
	{
		::CloseHandle (hProcessSnap); 
	}

	return bResult;
}

bool KTestCommon::_ExitOtherProcess(IN DWORD dwPid)
{
	BOOL bResult	= FALSE;
	HANDLE hProcess = NULL;
	HANDLE hOriToken = NULL;

	hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, dwPid);
	if (hProcess == NULL)
	{
		return FALSE;
	}

	if (!TerminateProcess(hProcess, 0))
	{
		return FALSE;
	}

	return TRUE;
}

int KTestCommon::CloseProcess(const TCHAR* lpProcName)
{
	if (lpProcName == NULL)
	{
		return enumProcessNameIsEmpty;
	}

	DWORD dwProcessID = 0;
	if (!_GetPidByName(lpProcName, dwProcessID))
	{
		return enumGetProccessIDByNameFail;
	}

	if (!_ExitOtherProcess(dwProcessID))
	{
		return enumTerminateProcessFail;
	}

	return enumSuccessful;
}
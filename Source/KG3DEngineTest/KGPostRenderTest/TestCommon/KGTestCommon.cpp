#include "KGTestCommon.h"
#include <time.h>

TCHAR g_szApplicationPath[MAX_PATH];
TCHAR g_szLogPath[MAX_PATH];

std::string KGTestCommon::IntToString(const int nValue)
{
	TCHAR szString[MAX_PATH] = {0};
	_itot(nValue, szString, 10);
	return szString;
}

int KGTestCommon::FormatPath(TCHAR* pszPath, const size_t cuSize)
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR* pRetCode = NULL;
	size_t uPathLength = 0;

	KG_ASSERT_EXIT(pszPath);
	pszPath[cuSize - 1] = _T('\0');

	pRetCode = strchr(pszPath, _T('/'));
	KGLOG_PROCESS_ERROR(pRetCode == NULL);

	uPathLength = strnlen(pszPath, cuSize);
	KGLOG_PROCESS_ERROR(uPathLength > 0);
	KGLOG_PROCESS_ERROR(uPathLength <= cuSize - 1);
	KGLOG_PROCESS_ERROR(uPathLength <= _MAX_DIR - 1);

	if (pszPath[uPathLength - 1] != _T('\\'))
	{
		pszPath[uPathLength] = _T('\\');
		pszPath[uPathLength + 1] = _T('\0');
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestCommon::FindLogFile(const TCHAR cszLogPath[], TCHAR* pszLogName, const size_t cuSize)
{
	int nRetCode = false;
	int nResult	 = false;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	TCHAR szAllFile[MAX_PATH]  = {0};
	TCHAR szLogPath[MAX_PATH]  = {0};
	TCHAR szFileName[MAX_PATH] = {0};
	TCHAR szExt[MAX_PATH]	  = {0};
	WIN32_FIND_DATA FindFileData;
	WIN32_FIND_DATA LatestLogFile;

	_ASSERTE(cszLogPath);
	KG_ASSERT_EXIT(pszLogName);
	KG_ASSERT_EXIT(cuSize != 0);

	nRetCode = _tcsncpy_s(szLogPath, sizeof(szLogPath), cszLogPath, _tcslen(cszLogPath));
	KG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = KGTestCommon::FormatPath(szLogPath, sizeof(szLogPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _sntprintf_s(szAllFile,
		sizeof(szAllFile),
		sizeof(szAllFile) - 1,
		_T("%s%s"),
		szLogPath,
		_T("*.*"));
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	hFindFile = ::FindFirstFile(szAllFile, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);

	//将最新日志的创建时间初始化为日志文件夹创建时间，用于比较
	memcpy(&LatestLogFile.ftCreationTime, &FindFileData.ftCreationTime, sizeof(FILETIME));

	while (true)
	{
		nRetCode = _tcsncpy_s(szFileName,
							 sizeof(szFileName),
							 FindFileData.cFileName,
							 _tcslen(FindFileData.cFileName)
					);
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		//过滤"."、".."
		if (szFileName[0] != _T('.'))
		{
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = _splitpath_s(szFileName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				nRetCode = _strlwr_s(szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);

				nRetCode = strncmp(szExt, _T(".log"), sizeof(_T(".log")));
				if (nRetCode == 0)
				{
					nRetCode = ::CompareFileTime(&LatestLogFile.ftCreationTime, &FindFileData.ftCreationTime);
					if (nRetCode == -1 || nRetCode == 0)//=0:第一个Log文件的创建时间与目录创建时间相同
					{
						memcpy(&LatestLogFile, &FindFileData, sizeof(WIN32_FIND_DATA));
					}
				}
			}
		}
		nRetCode = ::FindNextFile(hFindFile, &FindFileData);
		if (!nRetCode)
		{
			nRetCode = ::GetLastError();
			KG_PROCESS_SUCCESS(nRetCode == ERROR_NO_MORE_FILES);
			KGLOG_PROCESS_ERROR(false);
		}
	}

Exit1:
	nRetCode = _sntprintf_s(pszLogName,
		cuSize,
		cuSize - 1,
		_T("%s%s"),
		szLogPath,
		LatestLogFile.cFileName
		);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	nResult = true;
Exit0:
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KGTestCommon::MakeResultPath(const TCHAR cszProjectName[], TCHAR* pszResultPath, const size_t cuSize)
{
	int nResult  = false;
	int nRetCode = false;
	TCHAR szLogName[MAX_PATH] = {0};
	TCHAR* pszResult = NULL;

	_ASSERTE(cszProjectName);
	KG_ASSERT_EXIT(pszResultPath);
	KG_ASSERT_EXIT(cuSize != 0);

	nRetCode = FindLogFile(g_szLogPath, szLogName, sizeof(szLogName));
	KGLOG_PROCESS_ERROR(nRetCode);

	pszResult = _tcsrchr(szLogName, _T('.'));
	KGLOG_PROCESS_ERROR(pszResult);
	*pszResult = _T('\0');
	pszResult = NULL;

	nRetCode = KGTestCommon::FormatPath(szLogName, sizeof(szLogName));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _tmkdir(szLogName);
	KGLOG_PROCESS_ERROR(!nRetCode && _T("_tmkdir() error!"));

	nRetCode = _tcsncpy_s(pszResultPath, cuSize, szLogName, _tcslen(szLogName));
	KG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}

int KGTestCommon::TestingInit(const TCHAR cszProjectName[])
{
	int nResult  = false;
	int nRetCode = false;
	int nLogInited = false;
	TCHAR* pszResult = NULL;
	struct tm tmNow;
	time_t tmtNow = 0;
	KGLOG_PARAM LogParam = {_T(""), _T(""), KGLOG_OPTION_FILE, 65535 * 2};

	//获取应用程序路径
	nRetCode = ::GetModuleFileName(NULL, g_szApplicationPath, sizeof(g_szApplicationPath));
	KGLOG_PROCESS_ERROR(nRetCode);
	pszResult = _tcsrchr(g_szApplicationPath, _T('\\'));
	KGLOG_PROCESS_ERROR(pszResult);
	*(pszResult + 1) = _T('\0');
	pszResult = NULL;

	//设置当前工作目录
	nRetCode = ::SetCurrentDirectory(g_szApplicationPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	//初始化日志
	nRetCode = _tcsncpy_s(LogParam.szPath, sizeof(LogParam.szPath), LOG_NAME, _tcslen(LOG_NAME));
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _tcsncpy_s(LogParam.szIdent, sizeof(LogParam.szIdent), cszProjectName, _tcslen(cszProjectName));
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = KGLogInit(LogParam, (PVOID)cszProjectName);
	KG_PROCESS_ERROR(nRetCode);
	nLogInited = true;
	KGLogPrintf(KGLOG_INFO, _T("Testing Log Begin"));

	tmtNow = time(NULL);
	localtime_r(&tmtNow, &tmNow);

	//获得日志文件目录
	//风险：假设今天是2009-06-18，在此代码之前的Log初始化的时刻为2009-06-18-23-59-59，
	//程序执行到这里如果超过1秒，则当前日期会变成2009-06-19，此时根据日期所构造的日志目录就不正确了
	nRetCode = _sntprintf_s(
		g_szLogPath,
		sizeof(g_szLogPath),
		sizeof(g_szLogPath) - 1,
		_T("%s%s\\%s\\%d_%2.2d_%2.2d\\"),
		g_szApplicationPath,
		LOG_NAME,
		cszProjectName, 
		tmNow.tm_year + 1900,
		tmNow.tm_mon + 1,
		tmNow.tm_mday
		);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (nLogInited)
		{
			KGLogUnInit((PVOID)cszProjectName);
		}
	}
	return nResult;
}

int KGTestCommon::TestingUnInit(const TCHAR cszProjectName[])
{
	int nResult  = false;
	int nRetCode = false;

	KGLogPrintf(KGLOG_INFO, _T("Testing Log End"));
	nRetCode = KGLogUnInit((PVOID)cszProjectName);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}
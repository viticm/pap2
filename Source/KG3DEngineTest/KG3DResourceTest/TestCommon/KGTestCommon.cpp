#include "KGTestCommon.h"

char g_szApplicationPath[MAX_PATH];
char g_szLogPath[MAX_PATH];

std::string KGTestCommon::IntToString(const int nValue)
{
	char szString[MAX_PATH] = {0};
	_itoa(nValue, szString, 10);
	return szString;
}

float KGTestCommon::FastFabs(const float& pfIn)
{
	int* temp = (int*)&pfIn;
	int out = *temp & INV_SIGN_BIT;
	return *((float*)&out);
}

BOOL KGTestCommon::FloatEqual(float fNum1, float fNum2)
{
	if (KGTestCommon::FastFabs(fNum1 - fNum2) < EPSILON)
		return TRUE;
	else 
		return FALSE;
}

BOOL KGTestCommon::D3DCOLORVALUEqual(D3DCOLORVALUE pCo1, D3DCOLORVALUE pCo2)
{
	int nResult = false;

	KG_PROCESS_ERROR(FloatEqual(pCo1.r, pCo2.r));
	KG_PROCESS_ERROR(FloatEqual(pCo1.g, pCo2.g));
	KG_PROCESS_ERROR(FloatEqual(pCo1.b, pCo2.b));
	KG_PROCESS_ERROR(FloatEqual(pCo1.a, pCo2.a));

	nResult = true;
Exit0:
	return nResult;

}

BOOL KGTestCommon::D3DMATERIAL9Equal(D3DMATERIAL9 pMat1, D3DMATERIAL9 pMat2)
{
	int nResult = false;

	KG_PROCESS_ERROR(D3DCOLORVALUEqual(pMat1.Diffuse, pMat2.Diffuse));
	KG_PROCESS_ERROR(D3DCOLORVALUEqual(pMat1.Ambient, pMat2.Ambient));
	KG_PROCESS_ERROR(D3DCOLORVALUEqual(pMat1.Specular, pMat2.Specular));
	KG_PROCESS_ERROR(D3DCOLORVALUEqual(pMat1.Emissive, pMat1.Emissive));
	KG_PROCESS_ERROR(FloatEqual(pMat1.Power, pMat2.Power));

	nResult = true;
Exit0:
	return nResult;
}
int KGTestCommon::FormatPath(char* pszPath, const size_t cuSize)
{
	int nResult  = false;
	int nRetCode = false;
	char* pRetCode = NULL;
	size_t uPathLength = 0;

	KG_ASSERT_EXIT(pszPath);
	pszPath[cuSize - 1] = '\0';

	pRetCode = strchr(pszPath, '/');
	KGLOG_PROCESS_ERROR(pRetCode == NULL);

	uPathLength = strnlen(pszPath, cuSize);
	KGLOG_PROCESS_ERROR(uPathLength > 0);
	KGLOG_PROCESS_ERROR(uPathLength <= cuSize - 1);
	KGLOG_PROCESS_ERROR(uPathLength <= _MAX_DIR - 1);

	if (pszPath[uPathLength - 1] != '\\')
	{
		pszPath[uPathLength] = '\\';
		pszPath[uPathLength + 1] = '\0';
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestCommon::FindLogFile(const char cszLogPath[], char* pszLogName, const size_t cuSize)
{
	int nRetCode = false;
	int nResult	 = false;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	char szAllFile[MAX_PATH]  = {0};
	char szLogPath[MAX_PATH]  = {0};
	char szFileName[MAX_PATH] = {0};
	char szExt[MAX_PATH]	  = {0};
	WIN32_FIND_DATA FindFileData;
	WIN32_FIND_DATA LatestLogFile;

	_ASSERTE(cszLogPath);
	KG_ASSERT_EXIT(pszLogName);
	KG_ASSERT_EXIT(cuSize != 0);

	nRetCode = strncpy_s(szLogPath, sizeof(szLogPath) - 1, cszLogPath, strlen(cszLogPath));
	KG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = KGTestCommon::FormatPath(szLogPath, sizeof(szLogPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(szAllFile,
		sizeof(szAllFile),
		sizeof(szAllFile) - 1,
		"%s%s",
		szLogPath,
		"*.*");
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	hFindFile = ::FindFirstFile(szAllFile, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);

	//将最新日志的创建时间初始化为日志文件夹创建时间，用于比较
	memcpy(&LatestLogFile.ftCreationTime, &FindFileData.ftCreationTime, sizeof(FILETIME));

	while (true)
	{
		nRetCode = strncpy_s(szFileName,
							 sizeof(szFileName) - 1,
							 FindFileData.cFileName,
							 strlen(FindFileData.cFileName)
					);
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		//过滤"."、".."
		if (szFileName[0] != '.')
		{
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = _splitpath_s(szFileName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				nRetCode = _strlwr_s(szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);

				nRetCode = strncmp(szExt, ".log", sizeof(".log"));
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
	nRetCode = _snprintf_s(pszLogName,
		cuSize,
		cuSize - 1,
		"%s%s",
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

int KGTestCommon::MakeResultPath(const char cszProjectName[], char* pszResultPath, const size_t cuSize)
{
	int nResult  = false;
	int nRetCode = false;
	char szLogName[MAX_PATH] = {0};
	char* pszResult = NULL;

	_ASSERTE(cszProjectName);
	KG_ASSERT_EXIT(pszResultPath);
	KG_ASSERT_EXIT(cuSize != 0);

	nRetCode = FindLogFile(g_szLogPath, szLogName, sizeof(szLogName));
	KGLOG_PROCESS_ERROR(nRetCode);

	pszResult = strrchr(szLogName, '.');
	KGLOG_PROCESS_ERROR(pszResult);
	*pszResult = '\0';
	pszResult = NULL;

	nRetCode = KGTestCommon::FormatPath(szLogName, sizeof(szLogName));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _mkdir(szLogName);
	KGLOG_PROCESS_ERROR(!nRetCode && "_mkdir() error!");

	nRetCode = strncpy_s(pszResultPath, cuSize - 1, szLogName, strlen(szLogName));
	KG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}

int KGTestCommon::TestingInit(const char cszProjectName[])
{
	int nResult  = false;
	int nRetCode = false;
	int nLogInited = false;
	char* pszResult = NULL;
	struct tm tmNow;
	time_t tmtNow = 0;
	KGLOG_PARAM LogParam = {"", "", KGLOG_OPTION_FILE, 65535 * 2};

	//获取应用程序路径
	nRetCode = ::GetModuleFileName(NULL, g_szApplicationPath, sizeof(g_szApplicationPath));
	KGLOG_PROCESS_ERROR(nRetCode);
	pszResult = strrchr(g_szApplicationPath, '\\');
	KGLOG_PROCESS_ERROR(pszResult);
	*(pszResult + 1) = '\0';
	pszResult = NULL;

	//设置当前工作目录
	nRetCode = ::SetCurrentDirectory(g_szApplicationPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	//日志初始化
	nRetCode = strncpy_s(LogParam.szPath, sizeof(LogParam.szPath) - 1, LOG_NAME, strlen(LOG_NAME));
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = strncpy_s(LogParam.szIdent, sizeof(LogParam.szIdent) - 1, cszProjectName, strlen(cszProjectName));
	KG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = KGLogInit(LogParam, (PVOID)cszProjectName);
	KG_PROCESS_ERROR(nRetCode);
	nLogInited = true;
	KGLogPrintf(KGLOG_INFO, "Testing Log Begin");

	//获得日志文件目录
	//风险：假设今天是2009-06-18，在此代码之前的Log初始化的时刻为2009-06-18-23-59-59，
	//程序执行到这里如果超过1秒，则当前日期会变成2009-06-19，此时根据日期所构造的日志目录就不正确了
	tmtNow = time(NULL);
	localtime_r(&tmtNow, &tmNow);
	nRetCode = _snprintf_s(
		g_szLogPath,
		sizeof(g_szLogPath),
		sizeof(g_szLogPath) - 1,
		"%s%s\\%s\\%d_%2.2d_%2.2d\\",
		g_szApplicationPath,
		LOG_NAME,
		cszProjectName, 
		tmNow.tm_year + 1900,
		tmNow.tm_mon + 1,
		tmNow.tm_mday
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

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

int KGTestCommon::TestingUnit(const char cszProjectName[])
{
	int nResult  = false;
	int nRetCode = false;

	KGLogPrintf(KGLOG_INFO, "Testing Log End");
	nRetCode = KGLogUnInit((PVOID)cszProjectName);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KGTestCommon::GetSubStr(const char cszSource[],
								  const char cszStart[],
								  const char cszEnd[],
								  char* pszSub,
								  size_t uSize)
{
	int nRetCode = false;
	int nResult  = false;
	char* pszResult = NULL;
	int nStart = 0;
	int nEnd   = 0;
	char szLowerSource[MAX_LOG_LINE] = {0};
	char szLowerStart[MAX_PATH]  = {0};
	char szLowerEnd[MAX_PATH]    = {0};

	KG_ASSERT_EXIT(pszSub);
	_ASSERTE(cszSource);
	_ASSERTE(cszStart);
	_ASSERTE(cszEnd);

	nRetCode = strncpy_s(szLowerSource, sizeof(szLowerSource) - 1, cszSource, strlen(cszSource));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = strncpy_s(szLowerStart, sizeof(szLowerStart) - 1, cszStart, strlen(cszStart));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = strncpy_s(szLowerEnd, sizeof(szLowerEnd) - 1, cszEnd, strlen(cszEnd));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _strlwr_s(szLowerSource, sizeof(szLowerSource));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _strlwr_s(szLowerStart, sizeof(szLowerStart));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _strlwr_s(szLowerEnd, sizeof(szLowerEnd));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	pszResult = strstr(szLowerSource, szLowerStart);
	KG_PROCESS_ERROR(pszResult);
	nStart = (int)(pszResult - szLowerSource);
	nStart += (int)strlen(szLowerStart);

	pszResult = strstr(szLowerSource, szLowerEnd);
	KG_PROCESS_ERROR(pszResult);
	nEnd = (int)(pszResult - szLowerSource);

	nRetCode = strncpy_s(pszSub, uSize - 1, cszSource + nStart, nEnd - nStart);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;	
}

int KGTestCommon::CleanPath(const TCHAR cszPath[])
{
	int nRetCode = false;
	int nResult	 = false;
	char szPathName[MAX_PATH];
	char szFullPathFile[MAX_PATH];
	char* pszFile	 = NULL;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;

	ASSERT(cszPath);

	nRetCode = _sntprintf(szPathName, MAX_PATH, "%s\\%s", cszPath, "*.*");
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szPathName[MAX_PATH - 1] = '\0';

	hFindFile = ::FindFirstFile(szPathName, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		pszFile = FindFileData.cFileName;
		if (_tcsncmp(pszFile, ".", MAX_PATH) != 0 &&
			_tcsncmp(pszFile, "..", MAX_PATH) != 0)
		{
			nRetCode = _sntprintf(szFullPathFile, MAX_PATH, "%s\\%s", cszPath, pszFile);
			KGLOG_PROCESS_ERROR(nRetCode != -1);
			szFullPathFile[MAX_PATH - 1] = '\0';
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

int KGTestCommon::OutputInfo(const char cszInfo[])
{
	KGLogPrintf(KGLOG_INFO, cszInfo);
	printf("%s\n", cszInfo);
	return true;
}

int KGTestCommon::GetRandom(int nMin, int nMax)
{
	srand((unsigned)time(NULL));   
	return rand() % (nMax - nMin) + nMin;
}
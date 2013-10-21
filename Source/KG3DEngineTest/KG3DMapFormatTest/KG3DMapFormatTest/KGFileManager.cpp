#include "KGFileManager.h"

KGFileManager KGFileManager::m_Instance;
const int SIZE_READ_BUFFER = 512;     //读文件时的缓存

KGFileManager::KGFileManager(void)
{
}

KGFileManager::~KGFileManager(void)
{
}

KGFileManager* KGFileManager::Instance()
{
	return &m_Instance;
}

int KGFileManager::SearchFiles(const char cszPath[], vector<string>* pvecFileName, const char cszExt[])
{
	int nResult  = false;
	int nRetCode = false;
	char szSep[] = "|";
	char* pszToken = NULL;
	char szFileExt[MAX_PATH] = {0};

	KG_ASSERT_EXIT(cszPath);
	KG_ASSERT_EXIT(cszPath[0] != '\0');
	KG_ASSERT_EXIT(pvecFileName);

	m_vecFileExt.clear();
	m_vecFoundFilesList.clear();
	if (cszExt != NULL)
	{
		nRetCode = strncpy_s(szFileExt, sizeof(szFileExt) - 1, cszExt, strlen(cszExt));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		pszToken = strtok(szFileExt, szSep);   
		while(pszToken != NULL)   
		{
			m_vecFileExt.push_back(pszToken);   
			pszToken = strtok(NULL, szSep);
		}
	}

	nRetCode = SearchFilesFromPath(cszPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	(*pvecFileName) = m_vecFoundFilesList;
	m_vecFoundFilesList.clear();

	nResult = true;
Exit0:
	return nResult;
}

int KGFileManager::SearchFilesFromPath(const char cszPath[])
{
	int nRetCode = false;
	int nResult	 = false;				
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	char szFileName[MAX_PATH]	  = {0};
	char szAllFile[MAX_PATH]	  = {0};
	char szFullPathFile[MAX_PATH] = {0};
	char szPath[MAX_PATH]		  = {0};
	char szExt[MAX_PATH]		  = {0};
	WIN32_FIND_DATA FindFileData;

	KG_ASSERT_EXIT(cszPath);
	KGLOG_PROCESS_ERROR(cszPath[0] != '\0');

	nRetCode = strncpy_s(szPath, sizeof(szPath) - 1, cszPath, strlen(cszPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = KGTestCommon::FormatPath(szPath, sizeof(szPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(
		szAllFile,
		sizeof(szAllFile),
		sizeof(szAllFile) - 1,
		"%s%s",
		szPath,
		"*.*");
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	hFindFile = ::FindFirstFile(szAllFile, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		nRetCode = strncpy_s(szFileName,
			sizeof(szFileName) - 1,
			FindFileData.cFileName,
			strlen(FindFileData.cFileName)
			);
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		//过滤"."、".."、".svn"
		if (szFileName[0] != '.')
		{
			nRetCode = _snprintf_s(
				szFullPathFile,
				sizeof(szFullPathFile),
				sizeof(szFullPathFile) - 1,
				"%s%s",
				szPath,
				szFileName
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = SearchFilesFromPath(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
				vector<string>::iterator iter;
				nRetCode = _splitpath_s(szFileName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				nRetCode = _strlwr_s(szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);

 				iter = find(m_vecFileExt.begin(), m_vecFileExt.end(), szExt);
 				if (m_vecFileExt.size() == 0 || iter != m_vecFileExt.end())
 				{	
 					m_vecFoundFilesList.push_back(szFullPathFile);
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
	nResult = true;
Exit0:
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KGFileManager::ComparePathFiles(const char cszPath_1[], const char cszPath_2[], const char cszExt[])
{
	int nComparedFilesNum = 0;
	int nRetCode = false;
	char szResult[2 * MAX_PATH] = {0};
	vector<string> vecFileList_1;
	vector<string> vecFileList_2;

	KG_ASSERT_EXIT(cszPath_1);
	KG_ASSERT_EXIT(cszPath_1[0] != '\0');
	KG_ASSERT_EXIT(cszPath_2);
	KG_ASSERT_EXIT(cszPath_2[0] != '\0');

	nRetCode = SearchFiles(cszPath_1, &vecFileList_1, cszExt);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = SearchFiles(cszPath_2, &vecFileList_2, cszExt);
	KGLOG_PROCESS_ERROR(nRetCode);

	if (vecFileList_1.size() != vecFileList_2.size())
	{
		nRetCode = _snprintf_s(
			szResult, 
			sizeof(szResult),
			sizeof(szResult) - 1,
			"The number of files in path is different. Path_1=%s Path_2=%s",
			cszPath_1,
			cszPath_2
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		KGLogPrintf(KGLOG_ERR, szResult);
		KGLOG_PROCESS_ERROR(false);
	}

	for (size_t i = 0; i < vecFileList_1.size(); i++)
	{
		nRetCode = CompareFileByDataStream(vecFileList_1[i].c_str(), vecFileList_2[i].c_str());
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nComparedFilesNum =  (int)vecFileList_1.size();
Exit0:
	return nComparedFilesNum;
}

int KGFileManager::CompareFileByDataStream(const char cszFileName_1[], const char cszFileName_2[])
{
	int nResult  = false;
	int nRetCode = false;
	IFile* pFile_1 = NULL;
	IFile* pFile_2 = NULL;
	BYTE byData_1[SIZE_READ_BUFFER] = {0};
	BYTE byData_2[SIZE_READ_BUFFER] = {0};
	char szResult[2 * MAX_PATH] = {0};

	pFile_1 = g_OpenFile(cszFileName_1);
	KGLOG_PROCESS_ERROR(pFile_1);
	pFile_2 = g_OpenFile(cszFileName_2);
	KGLOG_PROCESS_ERROR(pFile_2);

	if (pFile_1->Size() != pFile_2->Size())
	{
		nRetCode = _snprintf_s(
			szResult, 
			sizeof(szResult),
			sizeof(szResult) - 1,
			"Size Differ. File_1=%s File_2=%s",
			cszFileName_1,
			cszFileName_2
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		KGLogPrintf(KGLOG_ERR, szResult);
		KGLOG_PROCESS_ERROR(false);
	}

	for (ULONG i = 0; i < pFile_1->Size(); i += SIZE_READ_BUFFER)
	{
		pFile_1->Read(byData_1, SIZE_READ_BUFFER);
		pFile_2->Read(byData_2, SIZE_READ_BUFFER);
		nRetCode = memcmp(byData_1, byData_2, SIZE_READ_BUFFER);
		if (nRetCode != 0)
		{
			nRetCode = _snprintf_s(
				szResult, 
				sizeof(szResult),
				sizeof(szResult) - 1,
				"File Data Differ. File_1=%s File_2=%s",
				cszFileName_1,
				cszFileName_2
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			KGLogPrintf(KGLOG_ERR, szResult);
			KGLOG_PROCESS_ERROR(false);
		}
	}

	nResult = true;
Exit0:
	KG_COM_RELEASE(pFile_1);
	KG_COM_RELEASE(pFile_2);
	return nResult;
}

int KGFileManager::CopyPathFiles(const char cszSourcePath[], const char cszDestPath[])
{
	int nResult  = false;
	int nRetCode = false;
	char szSourcePath[MAX_PATH] = {0};
	char szDestPath[MAX_PATH] = {0};
	char szCmdLine[2 * MAX_PATH] = {0};
	char* pszResult = NULL;

	nRetCode = strncpy_s(szSourcePath, sizeof(szSourcePath) - 1, cszSourcePath, strlen(cszSourcePath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	pszResult = strrchr(szSourcePath, '\\');
	if (pszResult)
	{
		(*pszResult) = '\0';
	}
	
	nRetCode = strncpy_s(szDestPath, sizeof(szDestPath) - 1, cszDestPath, strlen(cszDestPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	pszResult = strrchr(szDestPath, '\\');
	if (pszResult)
	{
		(*pszResult) = '\0';
	}

	nRetCode = _snprintf_s(
		szCmdLine, 
		sizeof(szCmdLine),
		sizeof(szCmdLine) - 1,
		"xcopy %s %s /e /s /k /i /q",
		szSourcePath,
		szDestPath
		);
	system(szCmdLine);

	nResult = true;
Exit0:
	return nResult;
}
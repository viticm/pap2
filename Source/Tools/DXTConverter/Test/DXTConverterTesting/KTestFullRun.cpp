#include "KTestFullRun.h"
#include "d3dx9tex.h"
#include "Engine/filetype.h"
#include "Engine/file.h"

const char DXTCONVERTER_EXE[]		= "DXTConverter.exe";
const char CONFIG_FILE_NAME[]		= "DXTConvertor.ini";
const char SECTION_BASE[]			= "Base";
const char SECTION_CHECKPSTH[]		= "CheckPath";
const char SECTION_FILTERPATH[]		= "FilterPath";
const char SECTION_FILTERFILE[]		= "FilterFile";
const char SECTION_CONVERTORFILE[]	= "ConvertorFile";
const char KEY_MAPLISTPATH[]		= "MaplistPath";
const char KEY_PATH_LIST[]			= "path_";
const char KEY_FILEPATH[]			= "filepath";
const char MAPLIST_FILENAME[]		= "MapList.tab";
const int MAX_PATH_NUM = 99;
const int MAX_TIMEOUT  = 1000 * 60 * 60 * 5;			//等待转换超时时间

void KTestFullRun::setUp()
{
	int nResult  = false;
	int nRetCode = false;
	char* pSlash = NULL;

	m_szSpecialConvertTableName[0]  = '\0';
	m_szFilterTableName[0]			= '\0';
	m_szApplicationPath[0]			= '\0';
	m_szMapListPath[0]				= '\0';
	m_szConfigFile[0]				= '\0';

	m_vCheckPathList.clear();
	m_vAllFileList.clear();
	m_vMapName.clear();
	m_vFilterFileAndPath.clear();
	m_vSpecialConvertFiles.clear();

	m_vSpecialConvertFilesForValidate.clear();
	m_vDefaultConvertFileList.clear();
	m_vFilterFileList.clear();
	m_vErrorFileList.clear();
	m_vTGAFileList.clear();
	m_vDDSFileList.clear();

	//获取应用程序路径，如：D:\DXTConverterTesting
	nRetCode = ::GetModuleFileName(NULL, m_szApplicationPath, MAX_PATH);
	KGLOG_PROCESS_ERROR(nRetCode);
	pSlash = strrchr(m_szApplicationPath, '\\');
	KGLOG_PROCESS_ERROR(pSlash);
	*(pSlash + 1) = '\0';
	pSlash = NULL;
	//设置当前工作目录
	nRetCode = ::SetCurrentDirectory(m_szApplicationPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestFullRun::tearDown()
{
	int nResult  = false;
	int nRetCode = false;

	if (!m_vCheckPathList.empty())
	{
		for (int i = 0; i < m_vCheckPathList.size(); i++)
		{
			KG_DELETE(m_vCheckPathList[i]);
		}
		m_vCheckPathList.clear();
	}
	if (!m_vFilterFileAndPath.empty())
	{
		for (int i = 0; i < m_vFilterFileAndPath.size(); i++)
		{
			KG_DELETE(m_vFilterFileAndPath[i]);
		}
		m_vFilterFileAndPath.clear();
	}
	if (!m_vMapName.empty())
	{
		for (int i = 0; i < m_vMapName.size(); i++)
		{
			KG_DELETE(m_vMapName[i]);
		}
		m_vMapName.clear();
	}
	if (!m_vAllFileList.empty())
	{
		for (int i = 0; i < m_vAllFileList.size(); i++)
		{
			KG_DELETE(m_vAllFileList[i]);
		}
		m_vAllFileList.clear();
	}

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestFullRun::TestConvertedFiles()
{
	int nResult  = false;
	int nRetCode = false;
	
	nRetCode = LoadConfigFile();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = LoadFileData();
	KGLOG_PROCESS_ERROR(nRetCode);

 	nRetCode = RunDXTConverter();
 	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = ValidateFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

int KTestFullRun::LoadConfigFile()
{
	int nResult  = false;
	int nRetCode = false;
	size_t uPathLen = 0;
	char szPathKeyName[MAX_PATH];
	char szPath[MAX_PATH];
	char* pszPath = NULL;

	nRetCode = _snprintf_s(m_szConfigFile, MAX_PATH, MAX_PATH - 1, "%s%s", m_szApplicationPath, CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	nRetCode = ::GetPrivateProfileString(SECTION_BASE, KEY_MAPLISTPATH, "", m_szMapListPath, MAX_PATH, m_szConfigFile);
	KGLOG_PROCESS_ERROR((nRetCode != MAX_PATH - 1) && (nRetCode != MAX_PATH - 2));

	nRetCode = FormatPath(m_szMapListPath, sizeof(m_szMapListPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = strcat_s(m_szMapListPath, MAX_PATH, MAPLIST_FILENAME);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _access(m_szMapListPath, 0);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	for (int nIndex = 1; nIndex <= MAX_PATH_NUM; nIndex++)
	{
		nRetCode = _snprintf_s(szPathKeyName, MAX_PATH, MAX_PATH - 1, "%s%.2d", KEY_PATH_LIST, nIndex);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		nRetCode = ::GetPrivateProfileString(SECTION_CHECKPSTH, szPathKeyName, "", szPath, MAX_PATH, m_szConfigFile);
		KGLOG_PROCESS_ERROR((nRetCode != MAX_PATH - 1) && (nRetCode != MAX_PATH - 2));
		if (szPath[0] == '\0')
		{
			break;
		}
		nRetCode = FormatPath(szPath, sizeof(szPath));
		KGLOG_PROCESS_ERROR(nRetCode);
		uPathLen = strnlen(szPath, sizeof(szPath));
		pszPath = new char[uPathLen + 1];
		KGLOG_PROCESS_ERROR(pszPath);
		strncpy(pszPath, szPath, uPathLen);
		pszPath[uPathLen] = '\0';
		m_vCheckPathList.push_back(pszPath);
	}
	for (int nIndex = 1; nIndex <= MAX_PATH_NUM; nIndex++)
	{
		nRetCode = _snprintf_s(szPathKeyName, MAX_PATH, MAX_PATH - 1, "%s%.2d", KEY_PATH_LIST, nIndex);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		nRetCode = ::GetPrivateProfileString(SECTION_FILTERPATH, szPathKeyName, "", szPath, MAX_PATH, m_szConfigFile);
		KGLOG_PROCESS_ERROR((nRetCode != MAX_PATH - 1) && (nRetCode != MAX_PATH - 2));
		if (szPath[0] == '\0')
		{
			break;
		}
		nRetCode = FormatPath(szPath, sizeof(szPath));
		KGLOG_PROCESS_ERROR(nRetCode);
		uPathLen = strnlen(szPath, sizeof(szPath));
		pszPath = new char[uPathLen + 1];
		KGLOG_PROCESS_ERROR(pszPath);
		strncpy(pszPath, szPath, uPathLen);
		pszPath[uPathLen] = '\0';
		m_vFilterFileAndPath.push_back(pszPath);
	}

	nRetCode = ::GetPrivateProfileString(SECTION_FILTERFILE, KEY_FILEPATH, "", m_szFilterTableName, MAX_PATH, m_szConfigFile);
	KGLOG_PROCESS_ERROR((nRetCode != MAX_PATH - 1) && (nRetCode != MAX_PATH - 2));
	nRetCode = ::GetPrivateProfileString(SECTION_CONVERTORFILE, KEY_FILEPATH, "", m_szSpecialConvertTableName, MAX_PATH, m_szConfigFile);
	KGLOG_PROCESS_ERROR((nRetCode != MAX_PATH - 1) && (nRetCode != MAX_PATH - 2));

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (!m_vCheckPathList.empty())
		{
			for (int i = 0; i < m_vCheckPathList.size(); i++)
			{
				KG_DELETE(m_vCheckPathList[i]);
			}
			m_vCheckPathList.clear();
		}
		if (!m_vFilterFileAndPath.empty())
		{
			for (int i = 0; i < m_vFilterFileAndPath.size(); i++)
			{
				KG_DELETE(m_vFilterFileAndPath[i]);
			}
			m_vFilterFileAndPath.clear();
		}
	}
	return nResult;
}
int KTestFullRun::LoadFileData()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = LoadMapNames();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = LoadSpecialConvertorFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = LoadFilterFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = FindCheckFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::LoadMapNames()
{
	int nResult  = false; 
	int nRetCode = false;
	errno_t nErron  = 0;
	size_t uPathLen = 0;
	int nHeight     = 0;
	ITabFile *piTableFile = NULL;
	char* pszFileName     = NULL;
	char szMapListFile[MAX_PATH];
	char szName[MAX_PATH];

	piTableFile = g_OpenTabFile(m_szMapListPath);
	KGLOG_PROCESS_ERROR(piTableFile);

	nHeight = piTableFile->GetHeight();
	KGLOG_PROCESS_ERROR(nHeight >= 2);

	for (int i = 2; i <= nHeight; i++)
	{
		nRetCode = piTableFile->GetString(i, "Name", "", szName, sizeof(szName));
		KGLOG_PROCESS_ERROR(nRetCode == 1);
		KGLOG_PROCESS_ERROR(szName[0]);
		
		uPathLen = strnlen(szName, sizeof(szName));
		pszFileName = new char[uPathLen + 1];
		KGLOG_PROCESS_ERROR(pszFileName);
		strncpy(pszFileName, szName, uPathLen);
		pszFileName[uPathLen] = '\0';
		m_vMapName.push_back(pszFileName);
	}

	nResult = true;
Exit0:
	if (piTableFile)
	{
		piTableFile->Release();
		piTableFile = NULL;
	}
	if (!nResult)
	{
		if (!m_vMapName.empty())
		{
			for (int i = 0; i < m_vMapName.size(); i++)
			{
				KG_DELETE(m_vMapName[i]);
			}
			m_vMapName.clear();
		}
		KGLogPrintf(KGLOG_ERR, "MapList.tab:%s load failed!", szMapListFile);
	}    
	return nResult;
}


int KTestFullRun::FindCheckFiles()
{
	int nResult  = false;
	int nRetCode = false;
	for (UINT i = 0; i < m_vCheckPathList.size(); i++)
	{
		nRetCode = FindTextureFile(m_vCheckPathList[i]);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	return nResult;

}

int KTestFullRun::LoadSpecialConvertorFiles()
{
	int nResult  = false; 
	int nRetCode = false;
	char szName[MAX_PATH];
	char szPath[MAX_PATH];
	int nDxt    = 0;
	int nHeight = 0;
	ITabFile *piTableFile = NULL;
	SpecialConvertFile ConvertFile;

	piTableFile = g_OpenTabFile(m_szSpecialConvertTableName);
	KGLOG_PROCESS_ERROR(piTableFile);

	nHeight = piTableFile->GetHeight();
	KG_PROCESS_SUCCESS(nHeight == 1);

	for (int i = 2; i <= nHeight; i++)
	{
		nRetCode = piTableFile->GetString(i, "name", "", szName, sizeof(szName));
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		nRetCode = piTableFile->GetString(i, "path", "", szPath, sizeof(szPath));
		KGLOG_PROCESS_ERROR(nRetCode != 0);
		if (nRetCode == 1)
		{
			nRetCode = FormatPath(szPath, sizeof(szPath));
			KGLOG_PROCESS_ERROR(nRetCode);
		}

		nRetCode = piTableFile->GetInteger(i, "DXT", 0, &nDxt, sizeof(nDxt));
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		nRetCode = _snprintf_s(ConvertFile.szFileName, MAX_PATH, MAX_PATH - 1, "%s%s", szPath, szName);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		ConvertFile.nDxt = nDxt;
		m_vSpecialConvertFiles.push_back(ConvertFile);
	}

Exit1:
	nResult = true;
Exit0:
	if (piTableFile)
	{
		piTableFile->Release();
		piTableFile = NULL;
	}
	if (!nResult)
	{
		KGLogPrintf(KGLOG_ERR, "ConvertorFiles.tab:%s load failed!", m_szSpecialConvertTableName);
	}    
	return nResult;
}

int KTestFullRun::LoadFilterFiles()
{
	int nResult  = false; 
	int nRetCode = false;
	char szName[MAX_PATH];
	char szPath[MAX_PATH];
	char szFullPathName[MAX_PATH];
	size_t uPathLen = 0;
	int nHeight		= 0;
	ITabFile *piTableFile = NULL;
	char* pszFileName     = NULL;

	piTableFile = g_OpenTabFile(m_szFilterTableName);
	KGLOG_PROCESS_ERROR(piTableFile);

	nHeight = piTableFile->GetHeight();
	KG_PROCESS_SUCCESS(nHeight == 1);

	for (int i = 2; i <= nHeight; i++)
	{
		nRetCode = piTableFile->GetString(i, "name", "", szName, sizeof(szName));
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		nRetCode = piTableFile->GetString(i, "path", "", szPath, sizeof(szPath));
		KGLOG_PROCESS_ERROR(nRetCode != 0);
		if (nRetCode == 1)
		{
			nRetCode = FormatPath(szPath, sizeof(szPath));
			KGLOG_PROCESS_ERROR(nRetCode);
		}

		nRetCode = _snprintf_s(szFullPathName, MAX_PATH, MAX_PATH - 1, "%s%s", szPath, szName);
		KGLOG_PROCESS_ERROR(nRetCode != -1);

		uPathLen = strnlen(szFullPathName, sizeof(szFullPathName));
	 	pszFileName = new char[uPathLen + 1];
	 	KGLOG_PROCESS_ERROR(pszFileName);
	 	strncpy(pszFileName, szFullPathName, uPathLen);
	 	pszFileName[uPathLen] = '\0';
		m_vFilterFileAndPath.push_back(pszFileName);
	}

Exit1:
	nResult = true;
Exit0:
	if (piTableFile)
	{
		piTableFile->Release();
		piTableFile = NULL;
	}
	if (!nResult)
	{
		if (!m_vFilterFileAndPath.empty())
		{
			for (int i = 0; i < m_vFilterFileAndPath.size(); i++)
			{
				KG_DELETE(m_vFilterFileAndPath[i]);
			}
			m_vFilterFileAndPath.clear();
		}
		KGLogPrintf(KGLOG_ERR, "FilterFiles.tab:%s load failed!", m_szSpecialConvertTableName);
	}    
	return nResult;
	nResult = true;
}
int KTestFullRun::AnalyseTGAFile(const char cszFilePath[], size_t uLen)
{
	int nResult  = false;
	int nRetCode = false;
	UINT uIndex = 0;
	char* pszFileNamePath = NULL;
	
	pszFileNamePath = new char[uLen + 1];
	KGLOG_PROCESS_ERROR(pszFileNamePath);
	strncpy(pszFileNamePath, cszFilePath, uLen);
	pszFileNamePath[uLen] = '\0';

	m_vAllFileList.push_back(pszFileNamePath);
	uIndex = UINT(m_vAllFileList.size() - 1);
	m_vTGAFileList.push_back(uIndex);

	nRetCode = IsFilterMapFile(uIndex);
	KG_PROCESS_SUCCESS(nRetCode);

	nRetCode = IsFilterFile(uIndex);
	KG_PROCESS_SUCCESS(nRetCode);

	nRetCode = IsTexture(uIndex);
	if(!nRetCode)
	{
		m_vErrorFileList.push_back(uIndex);
		KG_PROCESS_SUCCESS(true);
	}

	nRetCode = IsSpecialConvertFile(uIndex);
	KG_PROCESS_SUCCESS(nRetCode);

	m_vDefaultConvertFileList.push_back(uIndex);

Exit1:
	nResult = true;
Exit0:
	if (!nResult && pszFileNamePath)
	{
		KG_DELETE(pszFileNamePath);
	}
	return nResult;
}
int KTestFullRun::AnalyseDDSFile(const char cszFilePath[], size_t uLen)
{
	int nResult  = false;
	int nRetCode = false;
	UINT uIndex = 0;
	char* pszFileNamePath = NULL;

	pszFileNamePath = new char[uLen + 1];
	KGLOG_PROCESS_ERROR(pszFileNamePath);
	strncpy(pszFileNamePath, cszFilePath, uLen);
	pszFileNamePath[uLen] = '\0';

	m_vAllFileList.push_back(pszFileNamePath);
	uIndex = UINT(m_vAllFileList.size() - 1);

	nRetCode = IsTexture(uIndex);
	if(!nRetCode)
	{
		m_vErrorFileList.push_back(uIndex);
		KG_PROCESS_SUCCESS(true);
	}
	m_vDDSFileList.push_back(uIndex);

Exit1:
	nResult = true;
Exit0:
	if (!nResult && pszFileNamePath)
	{
		KG_DELETE(pszFileNamePath);
	}
	return nResult;
}
int KTestFullRun::RunDXTConverter()
{
	int nResult  = false;
	int nRetCode = false;
	bool bStated = false;
	int nSleepTimes  = 0;
	DWORD dwExitCode = 0;
	STARTUPINFO DXTConverterStartupinfo;
	PROCESS_INFORMATION	DXTConverterProcessInfo;
	char szDXTConverter[MAX_PATH];

	ZeroMemory(&DXTConverterStartupinfo, sizeof(DXTConverterStartupinfo));
	DXTConverterStartupinfo.cb = sizeof(DXTConverterStartupinfo);
	ZeroMemory(&DXTConverterProcessInfo, sizeof(DXTConverterProcessInfo));

	nRetCode = _snprintf(szDXTConverter, MAX_PATH, "%s%s", m_szApplicationPath, DXTCONVERTER_EXE);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	nRetCode = ::CreateProcess(
		NULL,
		szDXTConverter,
		NULL,
		NULL,
		NORMAL_PRIORITY_CLASS,
		0,
		NULL,
		NULL,
		&DXTConverterStartupinfo,
		&DXTConverterProcessInfo
		);
	KGLOG_PROCESS_ERROR(nRetCode);
	bStated = true;

	//等待转换完毕
	nSleepTimes = MAX_TIMEOUT / 1000;
	for (int i = 0; i < nSleepTimes; i++)
	{
		nRetCode = ::GetExitCodeProcess(DXTConverterProcessInfo.hProcess, &dwExitCode);
		KGLOG_PROCESS_ERROR(nRetCode);
		KG_PROCESS_SUCCESS(dwExitCode != STILL_ACTIVE);
		::Sleep(1000);
	}

Exit1:
	nResult = true;
Exit0:
	if (bStated)
	{
		::CloseHandle(DXTConverterProcessInfo.hProcess);
		::CloseHandle(DXTConverterProcessInfo.hThread);
	}
	return nResult;
}

int KTestFullRun::ValidateFiles()
{
	int nResult  = false;
	int nRetCode = false;

	printf("工具转换结果与验证程序不一致的数据：\n");
	KGLogPrintf(KGLOG_INFO, "工具转换结果与验证程序不一致的数据：\n");

	nRetCode = ValidateNoConvertedFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = ValidateConvertedFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = ValidateDDSFiles();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::ValidateNoConvertedFiles()
{
	int nResult  = false;
	int nRetCode = false;
	char* pszFileName = NULL;

	printf("错误的贴图文件(验证文件数 %d)：\n", m_vErrorFileList.size());
	KGLogPrintf(KGLOG_INFO, "错误的贴图文件(验证文件数 %d)：\n", m_vErrorFileList.size());
	for (UINT i = 0; i < m_vErrorFileList.size(); i++)
	{
		pszFileName = m_vAllFileList[m_vErrorFileList[i]];
		nRetCode = _access(pszFileName, 0);
		if (nRetCode != 0)
		{
			printf("[文件不存在] 文件名：%s\n", pszFileName);
			KGLogPrintf(KGLOG_INFO, "[文件不存在] 文件名：%s\n", pszFileName);
		}
	}
	printf("被过滤的贴图文件(验证文件数 %d)：\n", m_vFilterFileList.size());
	KGLogPrintf(KGLOG_INFO, "被过滤的贴图文件(验证文件数 %d)：\n", m_vFilterFileList.size());
	for (UINT i = 0; i < m_vFilterFileList.size(); i++)
	{
		pszFileName = m_vAllFileList[m_vFilterFileList[i]];
		nRetCode = _access(pszFileName, 0);
		if (nRetCode != 0)
		{
			printf("[文件不存在] 文件名：%s\n", pszFileName);
			KGLogPrintf(KGLOG_INFO, "[文件不存在] 文件名：%s\n", pszFileName);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::ValidateConvertedFiles()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hRetCode = E_FAIL;
	errno_t nErrno = 0;
	char* pszFileName = NULL;
	char szConvertedFile[MAX_PATH];
	int nDxtType = 0;
	D3DXIMAGE_INFO Info;

	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFileName[_MAX_FNAME];

	printf("特殊转换的贴图文件(验证文件数 %d)：\n", m_vSpecialConvertFilesForValidate.size());
	KGLogPrintf(KGLOG_INFO, "特殊转换的贴图文件(验证文件数 %d)：\n", m_vSpecialConvertFilesForValidate.size());
	for (UINT i = 0; i < m_vSpecialConvertFilesForValidate.size(); i++)
	{
		pszFileName = m_vAllFileList[m_vSpecialConvertFilesForValidate[i].nFileIndex];
		//确认转换后的TGA文件被删除
		nRetCode = _access(pszFileName, 0);
		if (nRetCode == 0)
		{
			printf("[TGA文件存在，可能是转换后删除失败] 文件名：%s\n", pszFileName);
			KGLogPrintf(KGLOG_INFO, "[TGA文件存在，可能是转换后删除失败] 文件名：%s\n", pszFileName);
			continue;
		}
		nErrno = _splitpath_s(
					pszFileName,
					szDrive,
					_MAX_DRIVE,
					szDir,
					_MAX_DIR,
					szFileName,
					_MAX_FNAME,
					NULL,
					0
				);
		KG_PROCESS_ERROR(szFileName[0] != '\0');
		KG_PROCESS_ERROR(nErrno == 0);

		nRetCode = _snprintf_s(szConvertedFile, MAX_PATH, MAX_PATH - 1, "%s%s%s.DDS", szDrive, szDir, szFileName);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		//确认转换后的DDS存在且格式正确
		nRetCode = _access(szConvertedFile, 0);
		if (nRetCode != 0)
		{
			printf("[DDS文件不存在] 文件名：%s\n", szConvertedFile);
			KGLogPrintf(KGLOG_INFO, "[DDS文件不存在] 文件名：%s\n", szConvertedFile);
			continue;
		}
		nDxtType = m_vSpecialConvertFilesForValidate[i].nDxt;
		ZeroMemory(&Info, sizeof(D3DXIMAGE_INFO));
		hRetCode = D3DXGetImageInfoFromFile(szConvertedFile, &Info);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		
		switch (nDxtType)
		{
		case 1:
			if(Info.Format != D3DFMT_DXT1)
			{
				printf("[贴图格式错误] 文件名：%s DXT%d\n", szConvertedFile, nDxtType);
				KGLogPrintf(KGLOG_INFO, "[贴图格式错误] 文件名：%s DXT%d\n", szConvertedFile, nDxtType);
			}
			break;
		case 3:
			if(Info.Format != D3DFMT_DXT3)
			{
				printf("[贴图格式错误] 文件名：%s DXT%d\n", szConvertedFile, nDxtType);
				KGLogPrintf(KGLOG_INFO, "[贴图格式错误] 文件名：%s DXT%d\n", szConvertedFile, nDxtType);
			}
			break;
		case 5:
			if(Info.Format != D3DFMT_DXT5)
			{
				printf("[贴图格式错误] 文件名：%s DXT%d\n", szConvertedFile, nDxtType);
				KGLogPrintf(KGLOG_INFO, "[贴图格式错误] 文件名：%s DXT%d\n", szConvertedFile, nDxtType);
			}
			break;
		default:
			printf("[非法贴图格式] 文件名：%s DXT%d\n", szConvertedFile, nDxtType);
			KGLogPrintf(KGLOG_INFO, "[非法贴图格式] 文件名：%s DXT%d\n", szConvertedFile, nDxtType);
			break;
		}
	}
	printf("默认转换为DXT5的贴图文件(验证文件数 %d)：\n", m_vDefaultConvertFileList.size());
	KGLogPrintf(KGLOG_INFO, "默认转换为DXT5的贴图文件(验证文件数 %d)：\n", m_vDefaultConvertFileList.size());
	for (UINT i = 0; i < m_vDefaultConvertFileList.size(); i++)
	{
		pszFileName = m_vAllFileList[m_vDefaultConvertFileList[i]];
		//确认转换后的TGA文件被删除
		nRetCode = _access(pszFileName, 0);
		if (nRetCode == 0)
		{
			printf("[TGA文件存在，可能是转换后删除失败] 文件名：%s\n", pszFileName);
			KGLogPrintf(KGLOG_INFO, "[TGA文件存在，可能是转换后删除失败] 文件名：%s\n", pszFileName);
			continue;
		}
		nErrno = _splitpath_s(
			pszFileName,
			szDrive,
			_MAX_DRIVE,
			szDir,
			_MAX_DIR,
			szFileName,
			_MAX_FNAME,
			NULL,
			0
			);
		KG_PROCESS_ERROR(szFileName[0] != '\0');
		KG_PROCESS_ERROR(nErrno == 0);

		nRetCode = _snprintf_s(szConvertedFile, MAX_PATH, MAX_PATH - 1, "%s%s%s.DDS", szDrive, szDir, szFileName);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		//确认转换后的DDS存在且格式正确
		nRetCode = _access(szConvertedFile, 0);
		if (nRetCode != 0)
		{
			printf("[DDS文件不存在] 文件名：%s\n", szConvertedFile);
			KGLogPrintf(KGLOG_INFO, "[DDS文件不存在] 文件名：%s\n", szConvertedFile);
			continue;
		}
		ZeroMemory(&Info, sizeof(D3DXIMAGE_INFO));
		hRetCode = D3DXGetImageInfoFromFile(szConvertedFile, &Info);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		if(Info.Format != D3DFMT_DXT5)
		{
			printf("[转换后贴图格式错误] 文件名：%s\n", szConvertedFile);
			KGLogPrintf(KGLOG_INFO, "[转换后贴图格式错误] 文件名：%s\n", szConvertedFile);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::FindTextureFile(const char cszFilePath[])
{
	int nRetCode = false;
	int nResult	 = false;
	size_t uPathLen = 0;
	errno_t nErrno  = 0;
	char* pszFileName = NULL;
	HANDLE hFindFile  = INVALID_HANDLE_VALUE;
	static char szAllFile[MAX_PATH];
	char szFullPathFile[MAX_PATH];
	char szPath[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	ASSERT(cszFilePath);

	nErrno = strncpy_s(szPath, MAX_PATH, cszFilePath, MAX_PATH);
	KGLOG_PROCESS_ERROR(nErrno == 0);
	nRetCode = FormatPath(szPath, sizeof(szPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(szAllFile, MAX_PATH, MAX_PATH - 1, "%s%s", szPath, "*.*");
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	hFindFile = ::FindFirstFile(szAllFile, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		pszFileName = FindFileData.cFileName;
		//过滤"."、".."、".svn"
		if (pszFileName[0] != '.')
		{
			nRetCode = _snprintf_s(szFullPathFile, MAX_PATH, MAX_PATH - 1, "%s%s", szPath, pszFileName);
			KGLOG_PROCESS_ERROR(nRetCode != -1);
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = _strlwr_s(pszFileName, MAX_PATH);
				KGLOG_PROCESS_ERROR(nRetCode == 0);

				uPathLen = strnlen(szFullPathFile, sizeof(szFullPathFile));

				if (strstr(pszFileName, ".tga"))
				{
					nRetCode = AnalyseTGAFile(szFullPathFile, uPathLen);
					KGLOG_PROCESS_ERROR(nRetCode);
				}
				else if (strstr(pszFileName, ".dds"))
				{
					nRetCode = AnalyseDDSFile(szFullPathFile, uPathLen);
					KGLOG_PROCESS_ERROR(nRetCode);
				}
			}
			else
			{
				nRetCode = FindTextureFile(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);	
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

int KTestFullRun::IsTexture(const UINT uIndex)
{
	int nResult  = false;
	int nRetCode = false;
	UINT uWidth  = 0;
	UINT uHeight = 0;
	HRESULT hRetCode = E_FAIL;
	D3DXIMAGE_INFO Info;

	ZeroMemory(&Info, sizeof(D3DXIMAGE_INFO));
	hRetCode = D3DXGetImageInfoFromFile(m_vAllFileList[uIndex], &Info);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

	ASSERT(Info.Width && Info.Height);

	uWidth = Info.Width;
	uHeight = Info.Height;

	nRetCode = uWidth < 4 || uHeight < 4;
	KG_PROCESS_ERROR(!nRetCode);

	nRetCode = ((uWidth & (uWidth - 1)) == 0);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = ((uHeight & (uHeight - 1)) == 0);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = ((uWidth / uHeight) > 4) || 
		((uHeight / uWidth) > 4) ||
		(((uWidth / uHeight) == 4) && ((uWidth % uHeight) != 0)) ||
		(((uHeight / uWidth) == 4) && ((uHeight % uWidth) != 0));
	KG_PROCESS_ERROR(!nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::IsFilterMapFile(const UINT uIndex)
{
	int nResult  = false;
	int nRetCode = false;
	errno_t nErrno = 0;
	size_t uLen    = 0;
	int nLineNum   = 0;
	char szMapDir[] = "data\\source\\maps\\";
	char szFileName[_MAX_FNAME];
	char* pszFileName = NULL;

	pszFileName = m_vAllFileList[uIndex];

 	nRetCode = SearchSubStr(
		pszFileName, strlen(pszFileName),
		szMapDir, strnlen(szMapDir, sizeof(szMapDir)));
	KG_PROCESS_ERROR(nRetCode);

	nErrno = _splitpath_s(pszFileName, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, NULL, 0);
	KG_PROCESS_ERROR(szFileName[0] != '\0');
	KG_PROCESS_ERROR(nErrno == 0);

	uLen = strnlen(szFileName, sizeof(szFileName));
	for (UINT i = 0; i < uLen; i++)
	{
		if (szFileName[i] == '_')
		{
			nLineNum++;
		}
	}
	KG_PROCESS_SUCCESS(nLineNum == 1);
	KG_PROCESS_ERROR(nLineNum != 2);

	for (UINT i = 0; i < m_vMapName.size(); i++)
	{
		nRetCode = strncmp(szFileName, m_vMapName[i], _MAX_FNAME);
		KG_PROCESS_SUCCESS(nRetCode == 0);
	}

	nRetCode = strncmp(szFileName, "middlemap", _MAX_FNAME);
	KG_PROCESS_SUCCESS(nRetCode == 0);

	nRetCode = strncmp(szFileName, "citymiddlemap", _MAX_FNAME);
	KG_PROCESS_SUCCESS(nRetCode == 0);

	KG_PROCESS_ERROR(false);
Exit1:
	m_vFilterFileList.push_back(uIndex);
	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::IsFilterFile(const UINT uIndex)
{
	int nResult  = false;
	int nRetCode = false;
	char* pszFileName    = NULL;
	char* pcszFilterFile = NULL;

	pszFileName = m_vAllFileList[uIndex];

	for (UINT i = 0; i < m_vFilterFileAndPath.size(); i++)
	{
		pcszFilterFile = m_vFilterFileAndPath[i];
		nRetCode = SearchSubStr(
			pszFileName, strlen(pszFileName),
			pcszFilterFile, strlen(pcszFilterFile)
			);
		if (nRetCode)
		{
			m_vFilterFileList.push_back(uIndex);
			KG_PROCESS_SUCCESS(true);
		}
	}
	
	KG_PROCESS_ERROR(false);
Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::IsSpecialConvertFile(const UINT uIndex)
{
	int nResult  = false;
	int nRetCode = false;
	char* pszFileName        = NULL;
	char* pszConvertFileName = NULL;
	SpecialConvertFileForValidate ConvertFile;

	pszFileName = m_vAllFileList[uIndex];

	for (UINT i = 0; i < m_vSpecialConvertFiles.size(); i++)
	{
		pszConvertFileName = m_vSpecialConvertFiles[i].szFileName;
		nRetCode = SearchSubStr(
			pszFileName, strnlen(pszFileName, MAX_PATH),
			pszConvertFileName, strnlen(pszConvertFileName, MAX_PATH));
		if (nRetCode)
		{
			ConvertFile.nFileIndex = uIndex;
			ConvertFile.nDxt = m_vSpecialConvertFiles[i].nDxt;
			m_vSpecialConvertFilesForValidate.push_back(ConvertFile);
			KG_PROCESS_SUCCESS(true);
		}
	}

	KG_PROCESS_ERROR(false);
Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::FormatPath(char* pszPath, size_t uSize)
{
	int nResult  = false;
	int nRetCode = false;
	char* pRetCode = NULL;
	size_t uPathLength = 0;

	KGLOG_PROCESS_ERROR(pszPath);
	pszPath[uSize - 1] = '\0';

	pRetCode = strchr(pszPath, '/');
	KGLOG_PROCESS_ERROR(pRetCode == NULL);

	uPathLength = strnlen(pszPath, uSize);
	KGLOG_PROCESS_ERROR(uPathLength > 0);
	KGLOG_PROCESS_ERROR(uPathLength < uSize - 1);
	KGLOG_PROCESS_ERROR(uPathLength < _MAX_DIR - 1);

	if (pszPath[uPathLength - 1] != '\\')
	{
		pszPath[uPathLength] = '\\';
		pszPath[uPathLength + 1] = '\0';
	}

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::Output()
{
	int nResult  = false;
	int nRetCode = false;
	printf("\nTotalFileNum: %d\n", m_vAllFileList.size());
	printf("TotalTGAFileNum: %d\n", m_vTGAFileList.size());
	printf("TotalDDSFileNum: %d\n", m_vDDSFileList.size());
	printf("TotalDefaultConvertedFileNum: %d\n", m_vDefaultConvertFileList.size());

	printf("TotalFilterFileNum: %d\n", m_vFilterFileList.size());
	for (int i = 0; i < m_vFilterFileList.size(); i++)
	{
		printf("\tFile%.2d: %s\n", i, m_vAllFileList[m_vFilterFileList[i]]);
	}

	printf("TotalErrorFileNum: %d\n", m_vErrorFileList.size());
	for (int i = 0; i < m_vErrorFileList.size(); i++)
	{
		printf("\tFile%.2d: %s\n", i, m_vAllFileList[m_vErrorFileList[i]]);
	}

	printf("TotalSpecialConvertedFileNum: %d\n", m_vSpecialConvertFilesForValidate.size());
	for (int i = 0; i < m_vSpecialConvertFilesForValidate.size(); i++)
	{
		printf("\tFile%.2d: %s\n", i, m_vAllFileList[m_vSpecialConvertFilesForValidate[i].nFileIndex]);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KTestFullRun::SearchSubStr(
	const char cszSource[], size_t uSourceLen,
	const char cszSearch[], size_t uSearchLen
)
{
	int nResult  = false;
	int nRetCode = false;
	char* pszSrc   = NULL;
	char* pszDst   = NULL;
	char* pRetCode = NULL;

	KGLOG_PROCESS_ERROR(cszSource && cszSource[0]);
	KGLOG_PROCESS_ERROR(cszSearch && cszSearch[0]);

	pszSrc = new char[uSourceLen + 1];
	KGLOG_PROCESS_ERROR(pszSrc);

	pszDst = new char[uSearchLen + 1];
	KGLOG_PROCESS_ERROR(pszDst);

	strncpy(pszSrc, cszSource, uSourceLen);
	pszSrc[uSourceLen] = '\0';

	strncpy(pszDst, cszSearch, uSearchLen);
	pszDst[uSearchLen] = '\0';

	nRetCode = _strlwr_s(pszSrc, uSourceLen + 1);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _strlwr_s(pszDst, uSearchLen + 1);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	pRetCode = strstr(pszSrc, pszDst);
	KG_PROCESS_ERROR(pRetCode);

	nResult = true;
Exit0:
	KG_DELETE(pszDst);
	KG_DELETE(pszSrc);
	return nResult;
}

int KTestFullRun::ValidateDDSFiles()
{
	int nResult  = false;
	int nRetCode = false;
	char* pszFileName = NULL;

	printf("不会被改变的DDS文件(验证文件数 %d)：\n", m_vDDSFileList.size());
	KGLogPrintf(KGLOG_INFO, "不会被改变的DDS文件(验证文件数 %d)：\n", m_vDDSFileList.size());
	for (UINT i = 0; i < m_vDDSFileList.size(); i++)
	{
		pszFileName = m_vAllFileList[m_vDDSFileList[i]];
		nRetCode = _access(pszFileName, 0);
		if (nRetCode != 0)
		{
			printf("[文件不存在] 文件名：%s\n", pszFileName);
			KGLogPrintf(KGLOG_INFO, "[文件不存在] 文件名：%s\n", pszFileName);
		}
	}
	nResult = true;
Exit0:
	return nResult;
}
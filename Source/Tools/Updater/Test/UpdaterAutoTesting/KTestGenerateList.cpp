#include "stdafx.h"
#include "KTestGenerateList.h"
#include <io.h>
#include <errno.h>

//测试数据
const TCHAR WRONG_VERSION[]  = _T("1.0.8.1200");		//不能生成下载列表的输入版本号
const TCHAR LATEST_VERSION[] = _T("1.0.8.1505");		//更新列表中最新版本号
const TCHAR AUTOUPDATE_ENTRY_1[] = _T("autoupdateentry_1.txt");		//正常的更新列表
const TCHAR AUTOUPDATE_ENTRY_2[] = _T("autoupdateentry_2.txt");		//记录有跨多版本的更新列表
const TCHAR AUTOUPDATE_ENTRY_3[] = _T("autoupdateentry_3.txt");		//记录有缺失的更新列表
const TCHAR AUTOUPDATE_ENTRY_4[] = _T("autoupdateentry_4.txt");		//记录有交集的更新列表，输入交集版本"内"的测试版本号
const TCHAR AUTOUPDATE_ENTRY_5[] = _T("autoupdateentry_5.txt");		//记录有交集的更新列表，输入交集版本"外"的测试版本号
const TCHAR AUTOUPDATE_ENTRY_6[] = _T("autoupdateentry_6.txt");		//记录有子集的更新列表
//用于验证正常的更新列表所生成的下载列表中升级包的名称和MD5值
const int ZIP_COUNT_1 = 3;
const TCHAR* ZIP_FILE_1[] =
{
	//_T("jx3_c_1.0.8.1490-to-1.0.8.1492_zhcn_patch.exe"), _T("27d4de01ab6e11c4e0dbfe254c129083"),
	//_T("jx3_c_1.0.8.1492-to-1.0.8.1493_zhcn_patch.exe"), _T("41adcafcc2a0d0df13a6015abb92960e"),
	_T("jx3_c_1.0.8.1493-to-1.0.8.1498_zhcn_patch.exe"), _T("b9a387a916d8abdfdb04eefa8ea4282f"),
	_T("jx3_c_1.0.8.1498-to-1.0.8.1502_zhcn_patch.exe"), _T("87012d2531413733039b1b278f6f4998"),
	_T("jx3_c_1.0.8.1502-to-1.0.8.1505_zhcn_patch.exe"), _T("3758cb90a824e559476004b037511271")
};
//用于验证记录有跨多版本的更新列表所生成的下载列表中升级包的名称和MD5值(跨版本记录1.0.8.1347-to-1.0.8.1351)
const int ZIP_COUNT_2 = 3;
const TCHAR* ZIP_FILE_2[] =	//input 1495
{
	_T("jx3_c_1.0.8.1493-to-1.0.8.1498_zhcn_patch.exe"), _T("b9a387a916d8abdfdb04eefa8ea4282f"),
	_T("jx3_c_1.0.8.1498-to-1.0.8.1502_zhcn_patch.exe"), _T("87012d2531413733039b1b278f6f4998"),
	_T("jx3_c_1.0.8.1502-to-1.0.8.1505_zhcn_patch.exe"), _T("3758cb90a824e559476004b037511271")
};

//用于验证记录有缺失的更新列表所生成的下载列表中升级包的名称和MD5值
const int ZIP_COUNT_3 = 1;
const TCHAR* ZIP_FILE_3[] =	//input 1494
{
	_T("jx3_c_1.0.8.1493-to-1.0.8.1498_zhcn_patch.exe"), _T("b9a387a916d8abdfdb04eefa8ea4282f"),
	//_T("jx3_c_1.0.8.1502-to-1.0.8.1505_zhcn_patch.exe"), _T("3758cb90a824e559476004b037511271")
};
//用于验证记录有交集的更新列表所生成的下载列表中升级包的名称和MD5值
const int ZIP_COUNT_4 = 1;
const TCHAR* ZIP_FILE_4[] =	//input 1495
{
	//_T("sword3_c_1.0.8.1492-to-1.0.8.1498_zhcn_patch.exe"), _T("a1fcbdab2d4ff535ca458272f01f9c76"),
	_T("sword3_c_1.0.8.1493-to-1.0.8.1505_zhcn_patch.exe"), _T("1b09458bd831fe224852a89cca3980e6")
};
//用于验证记录有交集的更新列表所生成的下载列表中升级包的名称和MD5值
const int ZIP_COUNT_5 = 1;
const TCHAR* ZIP_FILE_5[] =	//input 1499
{
	//_T("sword3_c_1.0.8.1492-to-1.0.8.1498_zhcn_patch.exe"), _T("a1fcbdab2d4ff535ca458272f01f9c76"),
	_T("sword3_c_1.0.8.1493-to-1.0.8.1505_zhcn_patch.exe"), _T("1b09458bd831fe224852a89cca3980e6")
};

//用于验证记录有子集的更新列表所生成的下载列表中升级包的名称和MD5值
const int ZIP_COUNT_6 = 1;
const TCHAR* ZIP_FILE_6[] =	//input 1503
{
	//_T("jx3_c_1.0.8.1503-to-1.0.8.1504_zhcn_patch.exe"), _T("3758cb90a824e559476004b037511271");
	_T("jx3_c_1.0.8.1502-to-1.0.8.1505_zhcn_patch.exe"), _T("3758cb90a824e559476004b037511271")
};


void KTestGenerateList::setUp()
{
}
void KTestGenerateList::tearDown()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = KTestCommon::CleanPath(g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestGenerateList::TestGenerateSuccess()
{
	int nResult  = false;
	int nRetCode = false;
	KVersionParser Parser;
	TCHAR szTestRes[MAX_PATH];
	TCHAR szCurVersion[] = _T("1.0.8.1493");

	nRetCode = KTestCommon::PrepareTestResoure(AUTOUPDATE_ENTRY_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	//输入更新列表和在列表范围内的版本号，生成下载列表
	nRetCode = _sntprintf(szTestRes, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, AUTOUPDATE_ENTRY_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestRes[MAX_PATH - 1] = _T('\0');
	nRetCode = Parser.Parse(szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证生成的下载列表
	nRetCode = ValidateData(&Parser, ZIP_FILE_1, ZIP_COUNT_1, szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	Parser.CleanUp();
	CPPUNIT_ASSERT(nResult);
}

void KTestGenerateList::TestLostRecord()
{
	int nResult  = false;
	int nRetCode = false;
	KVersionParser Parser;
	TCHAR szTestRes[MAX_PATH];
	TCHAR szCurVersion[] = _T("1.0.8.1494");

	nRetCode = KTestCommon::PrepareTestResoure(AUTOUPDATE_ENTRY_3);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	//输入更新列表和在列表范围内的版本号，生成下载列表
	nRetCode = _sntprintf(szTestRes, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, AUTOUPDATE_ENTRY_3);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestRes[MAX_PATH - 1] = _T('\0');
	nRetCode = Parser.Parse(szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证生成的下载列表
	nRetCode = ValidateData(&Parser, ZIP_FILE_3, ZIP_COUNT_3, szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	Parser.CleanUp();
	CPPUNIT_ASSERT(nResult);
}
void KTestGenerateList::TestSeveralVersionInOnePack()
{
	int nResult  = false;
	int nRetCode = false;
	KVersionParser Parser;
	TCHAR szTestRes[MAX_PATH];
	TCHAR szCurVersion[] = _T("1.0.8.1495");

	nRetCode = KTestCommon::PrepareTestResoure(AUTOUPDATE_ENTRY_2);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	//输入更新列表和在列表范围内的版本号，生成下载列表
	nRetCode = _sntprintf(szTestRes, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, AUTOUPDATE_ENTRY_2);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestRes[MAX_PATH - 1] = _T('\0');
	nRetCode = Parser.Parse(szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证生成的下载列表
	nRetCode = ValidateData(&Parser, ZIP_FILE_2, ZIP_COUNT_2, szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	Parser.CleanUp();
	CPPUNIT_ASSERT(nResult);
}

void KTestGenerateList::TestIntersectionVersionInside()
{
	int nResult  = false;
	int nRetCode = false;
	KVersionParser Parser;
	TCHAR szTestRes[MAX_PATH];
	TCHAR szCurVersion[] = _T("1.0.8.1495");

	nRetCode = KTestCommon::PrepareTestResoure(AUTOUPDATE_ENTRY_4);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	//输入更新列表和在列表范围内的版本号，生成下载列表
	nRetCode = _sntprintf(szTestRes, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, AUTOUPDATE_ENTRY_4);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestRes[MAX_PATH - 1] = _T('\0');
	nRetCode = Parser.Parse(szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证生成的下载列表
	nRetCode = ValidateData(&Parser, ZIP_FILE_4, ZIP_COUNT_4, szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	Parser.CleanUp();
	CPPUNIT_ASSERT(nResult);
}

void KTestGenerateList::TestIntersectionVersionOutside()
{
	int nResult  = false;
	int nRetCode = false;
	KVersionParser Parser;
	TCHAR szTestRes[MAX_PATH];
	TCHAR szCurVersion[] = _T("1.0.8.1499");

	nRetCode = KTestCommon::PrepareTestResoure(AUTOUPDATE_ENTRY_5);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	//输入更新列表和在列表范围内的版本号，生成下载列表
	nRetCode = _sntprintf(szTestRes, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, AUTOUPDATE_ENTRY_5);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestRes[MAX_PATH - 1] = _T('\0');
	nRetCode = Parser.Parse(szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证生成的下载列表
	nRetCode = ValidateData(&Parser, ZIP_FILE_5, ZIP_COUNT_5, szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	Parser.CleanUp();
	CPPUNIT_ASSERT(nResult);
}


void KTestGenerateList::TestSubsetVersion()
{
	int nResult  = false;
	int nRetCode = false;
	KVersionParser Parser;
	TCHAR szTestRes[MAX_PATH];
	TCHAR szCurVersion[] = _T("1.0.8.1503");

	nRetCode = KTestCommon::PrepareTestResoure(AUTOUPDATE_ENTRY_6);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	//输入更新列表和在列表范围内的版本号，生成下载列表
	nRetCode = _sntprintf(szTestRes, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, AUTOUPDATE_ENTRY_6);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestRes[MAX_PATH - 1] = _T('\0');
	nRetCode = Parser.Parse(szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证生成的下载列表
	nRetCode = ValidateData(&Parser, ZIP_FILE_6, ZIP_COUNT_6, szTestRes, szCurVersion);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	Parser.CleanUp();
	CPPUNIT_ASSERT(nResult);
}

void KTestGenerateList::TestGenerateFail()
{
	int nResult  = false;
	int nRetCode = false;
	int nCompareResult = 0;
	errno_t nErr = 0;
	KZipFileInfo** ppZipFileList = NULL;
	KVersionParser Parser;
	TCHAR szTestRes[MAX_PATH];

	nRetCode = KTestCommon::PrepareTestResoure(AUTOUPDATE_ENTRY_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	//输入更新列表和不在列表范围内的版本号，无法生成下载列表
	nRetCode = _sntprintf(szTestRes, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, AUTOUPDATE_ENTRY_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szTestRes[MAX_PATH - 1] = _T('\0');
	nRetCode = Parser.Parse(szTestRes, WRONG_VERSION);//1.0.8.1200
	KGLOG_PROCESS_ERROR(nRetCode == false);
	KGLogPrintf(KGLOG_INFO, "成功验证\"无法生成下载列表\"");
	ppZipFileList = Parser.GetPackList();
	KGLOG_PROCESS_ERROR(ppZipFileList[0] == NULL);
	//验证其它数据
	KGLOG_PROCESS_ERROR(Parser.NeedUpdate() == TRUE);
	KGLOG_PROCESS_ERROR(Parser.CanDownloadPack() == FALSE);
	nCompareResult = _tcsncmp(Parser.GetBeginVersion().c_str(), WRONG_VERSION, MAX_PATH);//1.0.8.1200
	KGLOG_PROCESS_ERROR(nCompareResult == 0);
	nCompareResult = _tcsncmp(Parser.GetLatestVersion().c_str(), LATEST_VERSION, MAX_PATH);//1.0.8.1353
	KGLOG_PROCESS_ERROR(nCompareResult == 0);
	nErr = ::_taccess_s(szTestRes, 0);
	KGLOG_PROCESS_ERROR(nErr == ENOENT);

	nResult = true;
Exit0:
	Parser.CleanUp();
	CPPUNIT_ASSERT(nResult);
}


int KTestGenerateList::ValidateData(
	KVersionParser* pParser,
	const TCHAR** cppZipList,
	const int cnListCount,
	const TCHAR cszEntryFile[],
	const TCHAR szCurVersion[]
)
{
	int nResult  = false;
	int nRetCode = false;
	int nCompareResult = 0;
	errno_t nErr = 0;
	KZipFileInfo** ppZipFileList = NULL;
	int nParserListCount = 0;

	KGLOG_PROCESS_ERROR(pParser);
	KGLOG_PROCESS_ERROR(cppZipList);
	KGLOG_PROCESS_ERROR(*cppZipList);
	ASSERT(cszEntryFile);

	ppZipFileList = pParser->GetPackList();
	nParserListCount = pParser->GetPackFileListCount();
	KGLOG_PROCESS_ERROR(cnListCount == nParserListCount);
	//验证下载列表每一个包信息(版本号和MD5值)
	for (int nIndex = 0; nIndex < cnListCount; nIndex++)
	{
		nCompareResult = _tcsncmp(ppZipFileList[nIndex]->szZipFileName, cppZipList[2 * nIndex], MAX_PATH);
		KGLOG_PROCESS_ERROR(nCompareResult == 0);
		nCompareResult = _tcsncmp(ppZipFileList[nIndex]->szZipFileMD5, cppZipList[2 * nIndex + 1], MAX_PATH);
		KGLOG_PROCESS_ERROR(nCompareResult == 0);
	}
	//验证其它数据
	KGLOG_PROCESS_ERROR(pParser->NeedUpdate() == TRUE);
	KGLOG_PROCESS_ERROR(pParser->CanDownloadPack() == TRUE);
	nCompareResult = _tcsncmp((pParser->GetBeginVersion()).c_str(), szCurVersion, MAX_PATH);
	KGLOG_PROCESS_ERROR(nCompareResult == 0);
	nCompareResult = _tcsncmp((pParser->GetLatestVersion()).c_str(), LATEST_VERSION, MAX_PATH);//1.0.8.1353
	KGLOG_PROCESS_ERROR(nCompareResult == 0);
	nErr = ::_taccess_s(cszEntryFile, 0);
	KGLOG_PROCESS_ERROR(nErr == ENOENT);

	nResult = true;
Exit0:
	return nResult;
}


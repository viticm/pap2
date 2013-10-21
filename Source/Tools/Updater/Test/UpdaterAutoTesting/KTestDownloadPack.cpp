#include "stdafx.h"
#include "KTestDownloadPack.h"
#include "KFileMD5Maker.h"
//#include "KUtil.h"

//测试下载的升级包名称列表
const TCHAR* ZIP_FILE_LIST[] = 
{
	_T("jx3_c_1.0.8.1464-to-1.0.8.1470_zhcn_patch.exe"),
	_T("jx3_c_1.0.8.1470-to-1.0.8.1473_zhcn_patch.exe"),
	_T("jx3_c_1.0.8.1473-to-1.0.8.1480_zhcn_patch.exe"),
	_T("jx3_c_1.0.8.1480-to-1.0.8.1482_zhcn_patch.exe"),
	_T("jx3_c_1.0.8.1482-to-1.0.8.1487_zhcn_patch.exe"),
	_T("jx3_c_1.0.8.1487-to-1.0.8.1490_zhcn_patch.exe"),
	_T("jx3_c_1.0.8.1490-to-1.0.8.1492_zhcn_patch.exe"), 
	_T("jx3_c_1.0.8.1492-to-1.0.8.1493_zhcn_patch.exe"), 
	_T("jx3_c_1.0.8.1493-to-1.0.8.1498_zhcn_patch.exe"), 
	_T("jx3_c_1.0.8.1498-to-1.0.8.1502_zhcn_patch.exe"), 
	_T("jx3_c_1.0.8.1502-to-1.0.8.1505_zhcn_patch.exe")
};
//测试下载的升级包对应的MD5值列表
const TCHAR* ZIP_MD5_LIST[] = 
{
	_T("b89a259dda76f9b347cd3e0f11c9aac4"),
	_T("e62b73009645a29d6449e31f0344010a"),
	_T("0b8a3842cf85b7abe22433b08f0b34d8"),
	_T("d5668a2298e88d19076bdec69db116f7"),
	_T("addedda80546453f5d84c937ba651913"),
	_T("4a1b03e0b3d6e3ffbcbdb46acf6569b1"),
	_T("27d4de01ab6e11c4e0dbfe254c129083"),
	_T("41adcafcc2a0d0df13a6015abb92960e"),
	_T("b9a387a916d8abdfdb04eefa8ea4282f"),
	_T("87012d2531413733039b1b278f6f4998"),
	_T("3758cb90a824e559476004b037511271")
};

void KTestDownloadPack::setUp()
{
	int nResult  = false;
	int nRetCode = false;
	BOOL bServerStarted = false;
	TCHAR szServerName[MAX_PATH];
 
 	ZeroMemory(&m_ServerStartupInfo, sizeof(m_ServerStartupInfo));
 	m_ServerStartupInfo.cb = sizeof(m_ServerStartupInfo);
 	ZeroMemory(&m_ServerProcessInfo, sizeof(m_ServerProcessInfo));
	//创建http服务器进程
	nRetCode = _sntprintf(szServerName, MAX_PATH, _T("%s\\%s"), g_szAppPath, SERVER_NAME);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	szServerName[MAX_PATH - 1] = _T('\0');
 	nRetCode = ::CreateProcess(
					NULL,
					szServerName,
					NULL,
					NULL,
					FALSE,
					0,
					NULL,
					NULL,
					&m_ServerStartupInfo,
					&m_ServerProcessInfo
				);
 	KGLOG_PROCESS_ERROR(nRetCode);
	bServerStarted = true;
	//等待服务器进程启动完毕
	nRetCode = KTestCommon::WaitForProcessStarted(m_ServerProcessInfo.hProcess);
	KGLOG_PROCESS_ERROR(nRetCode);
	
	nResult = true;
Exit0:
	if (!nResult && bServerStarted)
	{
		KTestCommon::CloseAndWaitForProcess(m_ServerProcessInfo.hProcess);
		m_ServerProcessInfo.hProcess = NULL;
	}
	CPPUNIT_ASSERT(nResult);
}

void KTestDownloadPack::tearDown()
{
	int nResult  = false;
	int nRetCode = false;

	//关闭服务器进程
	if (m_ServerProcessInfo.hProcess != NULL)
	{
		nRetCode = KTestCommon::CloseAndWaitForProcess(m_ServerProcessInfo.hProcess);
		Sleep(100);//临时解决方案。确保服务器已经关闭监听端口
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "CloseAndWait Server Process Failed");
		}
		m_ServerProcessInfo.hProcess = NULL;
	}

	nRetCode = KTestCommon::CleanPath(g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestDownloadPack::TestDownloadOnePack()
{
	int nResult  = false;
	int nRetCode = false;
	KHttpDownloader Downloader;

	//设置同时下载的文件数，从1到10共10种情况，每次进行下载1个更新包的测试
	for (int nCount = 1; nCount <= 10; nCount++)
	{
		// Downloader.SetThreadCount(nCount);
		nRetCode = Download(&Downloader, 1);
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = CheckAndDelZip(1);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestDownloadPack::TestDownloadSeveralPack()
{
	int nResult  = false;
	int nRetCode = false;
	KHttpDownloader Downloader;

	//设置同时下载的文件数，从1到10共10种情况，每次进行下载11个更新包的测试
	for (int nCount = 1; nCount <= 10; nCount++)
	{
		Downloader.SetThreadCount(nCount);
		nRetCode = Download(&Downloader, 11);
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = CheckAndDelZip(11);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestDownloadPack::TestDownloadBoundary()
{
	int nResult  = false;
	int nRetCode = false;
	KHttpDownloader Downloader;

	//设置同时下载的文件数为9，分别下载并验证8个、9个、10个更新包
	Downloader.SetThreadCount(8);
	nRetCode = Download(&Downloader, 8);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = CheckAndDelZip(8);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = Download(&Downloader, 9);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = CheckAndDelZip(9);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = Download(&Downloader, 10);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = CheckAndDelZip(10);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

int KTestDownloadPack::Download(KHttpDownloader* pDownloader, const int cnPackNum)
{
	int nRetCode = false;
	int nResult  = false;
	TCHAR szPackUrl[MAX_URL_LANGTH];
	TCHAR szTempDir[MAX_PATH];
	IHttpFile* pIHttpFile = NULL;

	KGLOG_PROCESS_ERROR(pDownloader);

 	//构造下载目录为测试空间根目录
 	nRetCode = _sntprintf(szTempDir, MAX_PATH, _T("%s\\"), g_szTestingSpaceDir);
 	KGLOG_PROCESS_ERROR(nRetCode != -1);
 	szTempDir[MAX_PATH - 1] = _T('\0');
	//添加nPack个文件信息到下载队列
	for (int nIndex = 0; nIndex < cnPackNum; nIndex++)
	{
		nRetCode = _sntprintf(szPackUrl, MAX_URL_LANGTH, _T("%s%s/%s"), REMOTE_PATH, VERSION_LINE_NAME, ZIP_FILE_LIST[nIndex]);
		//nRetCode = _sntprintf(szPackUrl, MAX_URL_LANGTH, _T("http://192.168.27.50/jx3/zhcn/jx3_c_1.0.8.1410-to-1.0.8.1412_zhcn_patch.exe"));
		KGLOG_PROCESS_ERROR(nRetCode != 1);
		szPackUrl[MAX_URL_LANGTH - 1] = _T('\0');
		nRetCode = pDownloader->AddFile(szPackUrl, szTempDir);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	//下载
	for (int nIndex = 0; nIndex < cnPackNum; nIndex++)
	{
		pIHttpFile = pDownloader->StartAndWait();
		KGLOG_PROCESS_ERROR(pIHttpFile);
	}
	//正确下载完毕
	pIHttpFile = pDownloader->StartAndWait();
	KGLOG_PROCESS_ERROR(pIHttpFile == NULL);

	nResult = true;
Exit0:
	return nResult;
}

int KTestDownloadPack::CheckAndDelZip(const int cnPackNum)
{
	int nRetCode = false;
	int nResult  = false;
	TCHAR szPackName[MAX_PATH];
	//验证每个包的MD5
	for (int nIndex = 0; nIndex < cnPackNum; nIndex++)
	{
		nRetCode = _sntprintf(szPackName, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, ZIP_FILE_LIST[nIndex]);
		KGLOG_PROCESS_ERROR(nRetCode != 1);
		szPackName[MAX_PATH - 1] = _T('\0');
		nRetCode = KTestCommon::ValidateFileMD5(szPackName, ZIP_MD5_LIST[nIndex]);
		KGLOG_PROCESS_ERROR(nRetCode);
		nRetCode = ::DeleteFile(szPackName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		KGLogPrintf(KGLOG_ERR, "ZIP NAME: %s", szPackName);
	}
	return nResult;
}
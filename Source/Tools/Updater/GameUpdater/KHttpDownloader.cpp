///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KHttpDownloader.cpp
// Creator  : chengzhaohui
// Date     : 2009-1-6
// Comment  : 
//
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <process.h>
#include "KHttpFile.h"
#include "KGPublic.h"
#include "KHttpDownloader.h"

KHttpDownloader::KHttpDownloader(void)
{
	m_nThreadCount = DEFAULT_THREAD;
	m_nNextFile = 0;
	m_pNotify = NULL;
	m_HttpFileCallback.m_pDownloader = this;
	m_HttpFileCallback.m_pNotify = NULL;
}

KHttpDownloader::~KHttpDownloader(void)
{
	for (unsigned i = 0; i < m_ReadyFiles.size(); ++i)
	{
		KHttpFile *pDownloader = (KHttpFile *)m_ReadyFiles[i];
		if (pDownloader)
		{
			pDownloader->UnInitDownload();

			delete pDownloader;
			pDownloader = NULL;
		}
	}
	m_ReadyFiles.clear();
}

void KHttpDownloader::SetThreadCount(int nThreadCount)
{
	m_nThreadCount = nThreadCount;
	if (nThreadCount < MIN_THREAD)
		m_nThreadCount = MIN_THREAD;
	else if (m_nThreadCount > MAX_THREAD)
		m_nThreadCount = MAX_THREAD;
}

void KHttpDownloader::SetNotification(IDownloaderNotification *pNotify)
{
	m_HttpFileCallback.m_pNotify = pNotify;
	m_HttpFileCallback.m_pDownloader = this;
}


int KHttpDownloader::AddFile(const TCHAR szFileUrl[], const TCHAR szSavePath[], int nResumeFlag)
{
	int nRetCode = false;
	int nRetsult = false;
	KHttpFile *pDownloader = NULL;

	KGLOG_PROCESS_ERROR(szFileUrl && szFileUrl[0]);
	KGLOG_PROCESS_ERROR(szSavePath && szSavePath[0]);

	pDownloader = new KHttpFile;
	KGLOG_PROCESS_ERROR(pDownloader);

	nRetCode = pDownloader->InitDownload(szFileUrl, szSavePath, nResumeFlag, &m_HttpFileCallback);
	KGLOG_PROCESS_ERROR(nRetCode);

	m_ReadyFiles.push_back(pDownloader);
	
	nRetsult = true;
Exit0:

	if (!nRetsult)
	{
		KG_DELETE(pDownloader);
	}
	return nRetsult;
}

__int64 KHttpDownloader::GetTotalFileSize()
{
	__int64 nTotalSize = 0;
	for (unsigned i = 0; i < m_ReadyFiles.size(); ++i)
	{
		nTotalSize += m_ReadyFiles[i]->FileSize();
	}
	return nTotalSize;
}

__int64 KHttpDownloader::GetTotalDownloadedSize()
{
	__int64 nTotalSize = 0;
	for (unsigned i = 0; i < m_ReadyFiles.size(); ++i)
	{
		nTotalSize += m_ReadyFiles[i]->DownloadedSize();
	}
	return nTotalSize;
}

IHttpFile *KHttpDownloader::StartAndWait()
{
	int nRetCode = false;	
	IHttpFile *pDownloader = NULL;
	HANDLE hThreads[MAX_THREAD] = {0};
	unsigned int nIndex = 0;
	RUN_DOWNLOADER_MAP::iterator it;

	GetTotalFileSize();

	BatchDownload();

	KG_PROCESS_ERROR(m_DownloadingFiles.size() > 0);

	for (it = m_DownloadingFiles.begin(); it != m_DownloadingFiles.end(); ++it)
	{
		if (nIndex < MAX_THREAD)
			hThreads[nIndex++] = it->first;
	}

	nRetCode = WaitForMultipleObjects(m_DownloadingFiles.size(), hThreads, false, INFINITE);
	if (nRetCode != WAIT_FAILED && nRetCode != WAIT_TIMEOUT)
	{
		nIndex = nRetCode - WAIT_OBJECT_0;
		KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_DownloadingFiles.size());
			
		it = m_DownloadingFiles.find(hThreads[nIndex]);
		KGLOG_PROCESS_ERROR(it != m_DownloadingFiles.end());

		pDownloader = it->second;

		//清楚已下载完的
		CloseHandle(it->first);
		m_DownloadingFiles.erase(it);
	}

	//下载下个文件
	BatchDownload();

	return pDownloader;
Exit0:
	return NULL;
}

void KHttpDownloader::Abort()
{
	for (unsigned i = 0; i < m_ReadyFiles.size(); ++i)
	{
		KHttpFile *pDownloader = (KHttpFile *)m_ReadyFiles[i];
		pDownloader->Stop();
	}

	HANDLE hThreads[MAX_THREAD] = {0};
	unsigned int nIndex = 0;

	for (RUN_DOWNLOADER_MAP::iterator it = m_DownloadingFiles.begin(); it != m_DownloadingFiles.end(); ++it)
	{
		if (nIndex < MAX_THREAD)
			hThreads[nIndex++] = it->first;
	}
	WaitForMultipleObjects(m_DownloadingFiles.size(), hThreads, true, INFINITE);
}

int KHttpDownloader::BatchDownload()
{
	HANDLE hThread = NULL;
	IHttpFile *pDownloader = NULL;

	while (true)
	{
		if (
			(m_DownloadingFiles.size() >= m_nThreadCount) ||
			(m_nNextFile >= m_ReadyFiles.size())
		)
		{
			break;
		}

		pDownloader = m_ReadyFiles[m_nNextFile++];
		KGLOG_PROCESS_ERROR(pDownloader);

		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction, (void *)pDownloader, 0, NULL);	
		if (hThread)
		{
			m_DownloadingFiles.insert(std::make_pair(hThread, pDownloader));
		}
	}	

Exit0:
	return true;
}

unsigned KHttpDownloader::ThreadFunction(void *pvParam)
{
	KHttpFile *pDownloader = (KHttpFile *)pvParam;
	if (pDownloader)
		return pDownloader->Download();
	return false;
}

void KHttpDownloader::KHttpFileCallback::OnProgress(IHttpFile *pHttpFile, int nRecvdSize)
{
	if (m_pNotify)
	{
		m_Lock.Lock();
		m_pNotify->OnProgress(m_pDownloader, pHttpFile, nRecvdSize);
		m_Lock.Unlock();
	}
}
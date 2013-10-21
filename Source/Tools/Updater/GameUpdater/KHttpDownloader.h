///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KHttpDownloader.h
// Creator  : chengzhaohui
// Date     : 2009-1-6
// Comment  : 批量文件下载
//
///////////////////////////////////////////////////////////////
#ifndef _KHTTP_DOWNLOADER_H_
#define _KHTTP_DOWNLOADER_H_

#include "IDownloader.h"
#include <vector>
#include <map>
#include "KHttpFile.h"

class KHttpDownloader : public IDownloader
{
public:
	KHttpDownloader(void);
	virtual ~KHttpDownloader(void);

	//设置线程数量，默认为5，最小1，最大10
	void SetThreadCount(int nThreadCount);

	void SetNotification(IDownloaderNotification *pNotify);

	//************************************
	// Method:    添加需要批量下载的文件地址，一次添加一个地址
	// Parameter: const TCHAR szFileUrl[] 文件地址
	// Parameter: const TCHAR szSavePath[] 文件保存路径
	// Parameter: int nResumeFlag 是否需要断点续传
	//************************************
	int AddFile(
		const TCHAR szFileUrl[], 
		const TCHAR szSavePath[],
		int nResumeFlag = true
	);

	//************************************
	// 获取所有文件的总大小
	//************************************
	__int64 GetTotalFileSize();
	//************************************
	// 获取所有文件已经下载的总大小
	//************************************
	__int64 GetTotalDownloadedSize();
	
	//************************************	
	// 开始下载并等待一个或多个文件下载完成。
	// 当某文件下载完成，函数返回此文件信息指针，如果返回NULL，表示所有文件下载完成、被中止或出错
	// 因此需要循环调用直到返回NULL。
	//************************************
	IHttpFile *StartAndWait();

	void Abort();
	
private:
	int BatchDownload();
	static unsigned __stdcall ThreadFunction(void *pvParam);

private:
	IDownloaderNotification *m_pNotify;	

	enum{MIN_THREAD = 1, MAX_THREAD = 10, DEFAULT_THREAD = 5};
	int m_nThreadCount; //开启的下载线程数量，一个线程下载一个文件

	typedef std::vector<IHttpFile *> READY_DOWNLOADER_VECT;
	READY_DOWNLOADER_VECT m_ReadyFiles; //需要下载的文件列表
	int m_nNextFile;//指向下个需要下载的文件

	typedef std::map<HANDLE, IHttpFile *> RUN_DOWNLOADER_MAP;
	RUN_DOWNLOADER_MAP m_DownloadingFiles; //下载队列

private:
	class KHttpFileCallback : public IHttpFileAsyncCallback
	{
	public:	
		KHttpFileCallback(){m_pDownloader = NULL; m_pNotify = NULL;}
		void OnProgress(IHttpFile *pHttpFile, int nRecvdSize);
		IDownloader *m_pDownloader;
		IDownloaderNotification *m_pNotify;
	private:
		KSyncLock m_Lock;
	}m_HttpFileCallback;
};

#endif
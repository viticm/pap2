///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KHttpDownloaderImp.h
// Creator  : chengzhaohui
// Date     : 2009-1-4
// Comment  : 多线程单文件下载
//
///////////////////////////////////////////////////////////////

#ifndef _KHTTP_FILE_H_
#define _KHTTP_FILE_H_

#include <map>
#include <vector>
#include "IDownloader.h"
#include "httpdownload.h"

class KHttpFile;

#if defined _UNICODE || defined UNICODE
	typedef std::wstring _tstring;
#else
	typedef std::string _tstring;
#endif

#pragma pack(1)
struct FILE_BLOCK_DESCRIPTOR
{
	struct FILE_BLOCK_INFO
	{
		__int64 nRangeFrom;
		__int64 nRangeTo;
		__int64 nRecvdSize;
	}BlockInfo;
	KHttpFile *m_pFile;
};
#pragma pack()

class IHttpFileAsyncCallback
{
public:
	virtual void OnProgress(IHttpFile *pHttpFile, int nRecvdSize) = 0;
};

class KHttpFile : public IHttpFile
{
public:
	KHttpFile();
	virtual ~KHttpFile(void);

	int    InitDownload(		
		const TCHAR szFileUrl[], 
		const TCHAR szSavePath[], 
		int nResumeFlag = true, 		
		IHttpFileAsyncCallback *pCallback = NULL,
		int nThreadCount = 1);
	void   UnInitDownload();

	const TCHAR *FileName(){GetFileInfo(); return m_strFile.c_str();};
	__int64 FileSize(){GetFileInfo(); return m_nFileSize;};
	__int64 DownloadedSize(){GetFileInfo(); return m_nDownloadedSize;};
	int    Error(){return m_nErrorCode;}

	int	   Download();
	void   Stop();

	int OnProgress(int nFileSize, int nDownloadedSize, int nRecved);

private:
	int GetFileInfo();
	int CheckDiskSpace();

	void Lock(){m_Lock.Lock();};
	void UnLock(){m_Lock.Unlock();};	
	

private:
	KSyncLock m_Lock;
	int m_nFile;
	int m_nErrorCode;

	_tstring m_strUrl;
	_tstring m_strFile;
	_tstring m_strSavePath;

	int m_nSupportResume; //服务器是否支持断点续传
	int m_nResume;//是否断点续传

	__int64 m_nFileSize;//文件总大小
	__int64 m_nDownloadedSize;//已下载的大小

	IHttpFileAsyncCallback *m_pCallback;

	CHttpDownload m_Downloader;

	int m_nCheckDiskFlag;
};


#endif
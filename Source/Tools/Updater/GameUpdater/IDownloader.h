#ifndef _IHTTPDOWNLOADER_H_
#define _IHTTPDOWNLOADER_H_

#include <Windows.h>

enum DOWNLOADER_ERROR_CODE
{	
	dec_err_success,		   //成功
	dec_err_notstart,		   //未开始下载
	dec_err_stop,			   //用户停止下载
	dec_err_url,			   //文件下载地址错误
	dec_err_cannotconnect,     //无法链接服务器
	dec_err_disconnection,     //网络断线
	dec_err_noenoughdiskspace, //磁盘空间不足
	dec_err_writefileerror,    //写文件出错
	dec_err_cannotdownload     //不能下载文件，有可能文件不存在		
};

class IDownloader
{
public:
	virtual __int64 GetTotalFileSize() = 0;
	virtual __int64 GetTotalDownloadedSize() = 0;
};

class IHttpFile
{
public:
	virtual const TCHAR *FileName() = 0;
	virtual __int64		 FileSize() = 0;
	virtual __int64		 DownloadedSize() = 0;
	virtual int			 Error() = 0;
};

class IDownloaderNotification
{
public:
	virtual void OnProgress(IDownloader *pDownloader, IHttpFile *pHttpFile, int nRecvdSize) = 0;
};

 class KSyncLock
 {
 public:
 	KSyncLock()    { InitializeCriticalSection(&m_CriticalSection);            }
 	~KSyncLock()   { DeleteCriticalSection(&m_CriticalSection);                }
 	int Lock()      { EnterCriticalSection(&m_CriticalSection); return true;    }
 	int Unlock()    { LeaveCriticalSection(&m_CriticalSection); return true;    }
 private:
 	CRITICAL_SECTION m_CriticalSection;
 };

#endif 
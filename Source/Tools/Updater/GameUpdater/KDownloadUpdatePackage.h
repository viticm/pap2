/*****************************************************************************************
//	FileName:   KDownloadUpdatePackage.h
//	Author	:   Zhong Guang Ze
//	CreateTime:	2009/01/14-10:46
//  Comment :
//  Copyright : Kingsoft corp
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef __KDOWNLOAD_UPDATE_PACKAGE_H__
#define __KDOWNLOAD_UPDATE_PACKAGE_H__

#include "KVersionParser.h"
#include "IDownloader.h"
#include "KHttpDownloader.h"

class IDownloaderNotification;

class KDownloadUpdatePackage
{	
public:
	KDownloadUpdatePackage(void);
	~KDownloadUpdatePackage(void);

	int GetLastVersion(
		const TCHAR cpszRemotePath[], 
		const TCHAR cpszUpdatePath[], 
		const TCHAR cpszClientVersion[], 
		TCHAR *pszLastVersion
	);

	int DownloadPacks(
		IDownloaderNotification* pNotify, 
		const TCHAR cpszRemotePath[], 
		const TCHAR cpszUpdatePath[], 
		const TCHAR cpszClientVersion[],
		const TCHAR cpszCurrentVersion[]
	);
	void Abort() {m_HttpClient.Abort(); m_nStop = true; }
	const TCHAR* GetNextPackName();
	
	int NeedUpdate(){return m_VersionParser.NeedUpdate();}
	
private:	
	int DownLoadVersion();
	int ParseVersion();
	int DownloadUpdatePackage();
	int CompareMD5(const TCHAR* cpszFileName);
	void SetDownloadErrorCode(int nErrCode); 
	void CleanUp();
	void ProcessDownloadError(const TCHAR szFile[], int nError);
	
private:

	KHttpDownloader   m_HttpClient;
	KVersionParser    m_VersionParser;
	IDownloaderNotification* m_pNotify;
	tstring           m_strUpdateEntryFile;
	int               m_PackCount;
	KZipFileInfo*     m_pZipFileList[MAX_PATH];
	TCHAR             m_szRemotePath[MAX_PATH];
	TCHAR             m_szUpdatePath[MAX_PATH];
	TCHAR             m_szClientLang[20];
	TCHAR             m_szCurrentVersion[MAX_PATH];
	int               m_nStop;
	int				  m_nDownloadVersionFlag;
};
#endif // __KDOWNLOAD_UPDATE_PACKAGE_H__
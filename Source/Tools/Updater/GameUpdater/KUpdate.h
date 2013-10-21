/*****************************************************************************************
//	FileName:   KUpdateManager.h
//	Author	:   Zhong Guang Ze
//	CreateTime:	2008/12/19-10:16
//  Comment :
//  Copyright : Kingsoft corp
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef _KUPDATEMANAGE_H__
#define _KUPDATEMANAGE_H__

#include "KConfig.h"
#include "KHttpDownloader.h"
#include "KThread.h"
#include "KSelfUpdater.h"
#include "KDownloadUpdatePackage.h"
#include "KExtractUpdatePack.h"
#include "IUpdateEvent.h"

class KUpdate
{
public:
	KUpdate(void);
	virtual ~KUpdate(void);

public:
	int GetLastVersion(
		const TCHAR szUrl[], 
		const TCHAR szUpdatePath[],
		const TCHAR szLang[], 
		IUpdateEvent* pEvent,
        HANDLE hSuccessSelfUpdateEvent,
        HANDLE hFailedSelfUpdateEvent
		);
	int AutoUpdate(
		const TCHAR szUrl[], 
		const TCHAR szCurrentVersion[], 
		const TCHAR szLang[],
		const TCHAR szAppPath[], 
		IUpdateEvent* pEvent
		);
	int ManualUpdate(
		const TCHAR szPackName[], 
		const TCHAR szCurrentVersion[],
		const TCHAR szAppPath[],
		IUpdateEvent* pEvent);
	int Abort();

protected:
	IUpdateEvent*		   m_pEvent;
	KThread                m_Thread;
	KThread				   m_GetLastVerionThread;
	KDownloadUpdatePackage m_DownloadPack;	

private:
	int RunUpdate();

	static void UpdateWorkThreadFunction(void *pParam);	
	void UpdateThreadFunction();
	
	static void GetVersionThread(void *pParam);
	void GetLastVersionWorkFunction();

	int _AutoUpdate();
	int  _ManualUpdate();
	int  _ExtractUpdatePackage(const TCHAR szPack[]);	
	int  _Cleanup(const TCHAR szUpdatePath[]);
	void _SetStatus(UPDATE_PROGRESS_STATUS eStatusID);
	int  _WaitForSelfUpdate();
	int  _StartupSelfupdate();

private:
	int    m_nUpdating;
	int    m_nStop;
	TCHAR  m_szAppPath[MAX_PATH];
	TCHAR  m_szUpdatePath[MAX_PATH];
	TCHAR  m_szCurrVersion[STRING_LENGTH];
	TCHAR  m_szUrl[MAX_PATH];
	TCHAR  m_szLang[STRING_LENGTH];

	int    m_nIsManualUpdate;
	TCHAR  m_szManualPackName[MAX_PATH];
    HANDLE m_hSelfUpdateEvent[2];
private:
	class KNotification : public IDownloaderNotification
	{
	public:
		void SetNotfiy(IUpdateEvent* pControl);
		void OnProgress(IDownloader *pDownloader, IHttpFile *pHttpFile, int nRecvdSize);
	private:
		IUpdateEvent* m_pControl;
		float	m_fLastPrecent;
	};
	KNotification m_Notify;
};


#endif // _KUPDATEMANAGE_H__
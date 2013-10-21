/*****************************************************************************************
//	FileName:   KUpdateControl.h
//	Author	:   Zhong Guang Ze
//	CreateTime:	2008/12/16-18:11
//  Comment :   对FLASH传递进来的命令进行处理，开始下载，合并包，等等
//  Copyright : Kingsoft corp
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef _K_UPDATE_CONTROL_H__
#define _K_UPDATE_CONTROL_H__

#include "KFlashWindow.h"
#include "IUpdateEvent.h"
#include "UpdateDefine.h"
#include "KClientReport.h"
#include "KConfig.h"
#include "KUpdate.h"
	
#include <string.h>
#include <vector>
#include <map>

class IUpdate;

class KGameUpdater : public IFlashCallBack, public IUpdateEvent
{
public:
	KGameUpdater(void);
	virtual ~KGameUpdater(void);

public:
	//IFlashCallBack
	int FlashCallback(const tstring &strFunName, const PARAM_LIST &ParamList);
	//IUpdateEvent
	int OnDownlodProgress(float fDownloadPercent);
	int OnError(UPDATE_FLASH_SHOW_ERROR_MESSAGES eErrorCode);
	int OnStateChange(UPDATE_PROGRESS_STATUS eStatusID);
	int OnLastVersion(const TCHAR szLastVersion[]);

public:
    int Init(const TCHAR cszSwfFileName[]);	
    int UnInit();
    int AutoUpdate(HANDLE hSucessSelfUpdateEvent, HANDLE hFailedSelfUpdateEvent);
    int ManualUpdate(const TCHAR cszManualUpdateFileName[]);

private:
	typedef int (KGameUpdater::*fnOnFlashCall)();
	typedef std::map<std::string, fnOnFlashCall> FLASH_CALL_MAP;
	FLASH_CALL_MAP m_FlashCallFunctions;

	int _OnBeginUpdate();
	int _OnQuitUpdate();
	int _OnStartGame(); 
	int _OnConfigGame();

	int _CallFlash(const TCHAR cpszCallBackName[], int nReturnCode);
	int _CallFlash(const TCHAR cpszCallBackName[], const TCHAR pszRetString[]);
private:
	KConfig                m_Config;
	KFlashWindow		   m_FlashWnd;	
	IUpdate*               m_pUpdate;
	KUpdate                m_Update;
	UPDATE_PROGRESS_STATUS m_eUpdateStatus;
	KClientReport		   m_ClientReport;

	HBITMAP m_bmp16BPPBack;
	//int m_nSelfUpdateFlag;
};

#endif // _K_UPDATE_CONTROL_H__
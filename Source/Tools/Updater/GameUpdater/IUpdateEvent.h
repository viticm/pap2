/*****************************************************************************************
//	FileName:   IUpdateControl.h
//	Author	:   Zhong Guang Ze
//	CreateTime:	2009/01/09-17:09
//  Comment :
//  Copyright : Kingsoft corp
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef _IUpdateControl_h__
#define _IUpdateControl_h__
#include "UpdateDefine.h"

class IUpdateEvent
{
public:
	virtual int OnDownlodProgress(float fDownloadPercent) = 0;
	virtual int OnError(UPDATE_FLASH_SHOW_ERROR_MESSAGES eErrorCode) = 0;
	virtual int OnStateChange(UPDATE_PROGRESS_STATUS eStatusID) = 0;
	virtual int OnLastVersion(const TCHAR szLastVersion[]) = 0;
};


#endif // _IUpdateControl_h__
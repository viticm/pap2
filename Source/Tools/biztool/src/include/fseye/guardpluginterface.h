//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-02 17:00
//      File_base        : guardpluginterface.h
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#ifndef _PLUG_INTERFACE_H_
#define _PLUG_INTERFACE_H_

#define CREATEFUNCTIONNAME "CreatePlugInterface"

struct ISender
{
	virtual int SendPackToServer(unsigned char* pData, unsigned int nLen) = 0;
};

struct IGuard
{
	virtual void PutMessage(void* pData, int nLen) = 0;
	virtual void GetRootPath(char* pszPath) = 0;
	virtual void Log(const char* pszFormat, ...) = 0;
};

struct IGuardPlugin
{
	virtual int LoadPlug(ISender* pSender, IGuard* pGuard) = 0;	
	virtual int Release() = 0;
	virtual int ProcessNetMessage(unsigned char* pData, unsigned int nLen) = 0;
	virtual int Disconnect() = 0;
	virtual int GetDescribe(char* pszDes, int nSize) = 0;
	virtual int GetGUID(char* pszGUID, int nSize) = 0;
	virtual int GetVersion(int& nMaxVer, int& nMinVer) = 0;
	virtual int GetAuthor(char* pszAuthor, int nSize) = 0;
	virtual int Breathe() = 0;
	// 检查该插件是否能够处理某个协议
	virtual bool CheckProtocol(int nProtocol) = 0;
};

typedef void (*PCREATEPLUGINTERFACE)(IGuardPlugin** ppPlug);
#endif


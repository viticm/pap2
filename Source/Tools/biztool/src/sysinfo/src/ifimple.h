//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : sysinfo
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _IFIMPLE_H_
#define _IFIMPLE_H_

#include "def.h"
#include "cpu.h"
#include "mem.h"
#include "disk.h"
#include "net.h"
#include "proc.h"
//////////////////////////////////////////////////////////////////////
class KSysPlugin : public IGuardPlugin
{
public:
	typedef void (KSysPlugin::*fnProtocolProcess)(const void *, unsigned int);
	KSysPlugin( );
	~KSysPlugin( );

	int LoadPlug(ISender*	pSender, IGuard* pGuard);
	
	int Release( );
	
	int ProcessNetMessage(
		unsigned char* pData,
		unsigned int datasize );
	
	int Disconnect( );
	
	int GetDescribe( 
		char* szDes,
		int nSize );
	
	int GetGUID( 
		char* szGUID,
		int nSize );
	
	int GetVersion( 
		int& nMaxVer, 
		int& nMinVer );
	
	int GetAuthor(
		char* szAuthor,
		int nSize  );

	int Breathe( );
	virtual bool CheckProtocol(int nProtocol);
	void Process_GetCpuInfo(const void* pData, unsigned int nLen);
	void Process_GetCpuLoad(const void* pData, unsigned int nLen);
	void Process_GetMemInfo(const void* pData, unsigned int nLen);
	void Process_GetDiskInfo(const void* pData, unsigned int nLen);
	void Process_GetNetInfo(const void* pData, unsigned int nLen);
	void Process_GetNetLoad(const void* pData, unsigned int nLen);
	void Process_GetProcInfo(const void* pData, unsigned int nLen);
private:
	ISender*			m_pSender;
	_CPUInfo			m_CI;
	_MEMInfo			m_MI;
	_DISKInfo			m_DI;
	_NETInfo			m_NI;
	_PROCInfo			m_PI;
	fnProtocolProcess	m_aryProtoProcess[fseye_protocol_count];
};

#endif

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : net
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _SI_NET_H_
#define _SI_NET_H_

#include "def.h"
//////////////////////////////////////////////////////////////////////

#define MAX_NETCARD_COUNT 10
#define MAX_MAC			8
#define MAX_IPSIZE		20

struct _NCRTXInfo
{
	unsigned long ulRXPackage;
	unsigned long ulTXPackage;
	unsigned long ulRXByte;
	unsigned long ulTXByte;
	unsigned long ulRXRate;
	unsigned long ulTXRate;
};

struct _NetCardInfo
{
	char szIP[MAX_IPSIZE];
	unsigned long ulIP;
	unsigned long ulMacLen;
	unsigned char  Mac[MAX_MAC];
	char szMac[MAX_IPSIZE];
	char szNetMask[MAX_IPSIZE];
	unsigned long ulNetMask;
	unsigned long ulMTU;
	char szDesc[MAX_NAME];
	unsigned long ulDescLen;
	_NCRTXInfo	CRTXI;
};

typedef	struct _tagNETInfo
{
	char			szHostName[MAX_NAME];
	char			szSystemName[MAX_NAME];
	_NetCardInfo	Card[MAX_NETCARD_COUNT];
	unsigned long	nNETCardCount;

}NETINFO,*PNETINFO;

class _NETInfo
{
public:
	
	_NETInfo( );
	~_NETInfo( );
	
public:
	
	int Init( );
	int GetNETInfo( NETINFO& NI );
	
private:
	
	void _UpdateNI( );
	void _UpdateNL( );
	unsigned long GetTimetoTickCount( );
	void GetSystemName( );
	
	
private:
	
	NETINFO			m_NI;
	unsigned long	m_ulLoadStart;

};

#endif

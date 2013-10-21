//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-06-28 17:24
//      File_base        : getnccfg.h
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : Create New Player Function
//
//////////////////////////////////////////////////////////////////////
#ifndef _LORD_NCCFG_H_
#define _LORD_NCCFG_H_
#include "def.h"

typedef struct _servernetcard 
{
	char szHostName[MAXHOSTNAME];
	char szInternetIP[MAX_IPSIZE];
	char szIntranetIP[MAX_IPSIZE];
	unsigned long ulInternetIP;
	unsigned long ulIntranetIP;
	unsigned char  InternetMac[MAX_MAC];
	unsigned char  IntranetMac[MAX_MAC];
	unsigned long ulInternetMask;
	unsigned long ulIntranetMask;
	
}SERVERNETCFG,*PSERVERNETCFG;


int GetServerNetCfg( SERVERNETCFG& ServerNetCfg );

#endif

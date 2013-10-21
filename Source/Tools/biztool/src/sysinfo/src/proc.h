//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : proc
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _SI_PROC_H_
#define _SI_PROC_H_

#include "def.h"
//////////////////////////////////////////////////////////////////////

#define MAX_PROCESS_COUNT		50

typedef	struct _tagProcess
{
	char szName[MAX_NAME/6];
	unsigned int nPid;
	unsigned int nMemUseSize;
	unsigned int nVmSize;
	unsigned long ulCPUTime;
	unsigned int nThreadCount;
	
}PROCESS,*PPROCESS;

typedef	struct _tagPROCInfo
{
	PROCESS		Process[MAX_PROCESS_COUNT];
	int			nProcessCount;

}PROCINFO,*PPROCINFO;

class _PROCInfo
{
public:
	
	_PROCInfo( );
	~_PROCInfo( );
	
public:
	
	int Init( );
	int GetPROCInfo( PROCINFO& PI );
	
private:
	
	void _UpdatePI( );
	
private:
	
	PROCINFO	m_PI;
	unsigned long	m_ulLoadStart;
};

#endif

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : cpu
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _SI_CPU_H_
#define _SI_CPU_H_

#include "def.h"
//////////////////////////////////////////////////////////////////////

#define MAX_PROCESSOR_COUNT		10

typedef	struct _tagLogicProcessor
{
	char szName[MAX_NAME];
	char szVendor[MAX_NAME];
	char szDesc[MAX_NAME];
	bool bMMX;
	bool bMMXEXT;
	bool bSSE;
	bool bSSE2;
	bool b3DNow;
	bool b3DNowEXT;
	
	int nSteppingID;
	int nType;
	int nFamily;
	int nModel;
	int nBrandID;
	INT64 CIdleTime;
	int nLoad;
}LOGICPROCESSOR,*PLOGICPROCESSOR;

typedef	struct _tagCPUInfo
{
	LOGICPROCESSOR	Processor[MAX_PROCESSOR_COUNT];
	int				nProcessorCount;
	int				nTotalLoad;
}CPUINFO,*PCPUINFO;

class _CPUInfo
{
public:

	_CPUInfo( );
	~_CPUInfo( );

public:
	
	int Init( );
	int GetCPUInfo( CPUINFO& CI );

private:
	
	void _UpdateCI( );
	void _UpdateCL( );
	unsigned long GetTimetoTickCount( );

private:

	CPUINFO			m_CI;
	unsigned long	m_ulLoadStart;
};

#endif

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : mem
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _SI_MEM_H_
#define _SI_MEM_H_

#include "def.h"
//////////////////////////////////////////////////////////////////////
typedef	struct _tagMEMInfo
{
	unsigned int nTotalMem;
	unsigned int nFreeMem;
	
}MEMINFO,*PMEMINFO;

class _MEMInfo
{
public:
	
	_MEMInfo( );
	~_MEMInfo( );
	
public:
	
	int Init( );
	int GetMEMInfo( MEMINFO& MI );
	
private:
	
	void _UpdateMI( );
	
private:

	MEMINFO	m_MI;
};

#endif

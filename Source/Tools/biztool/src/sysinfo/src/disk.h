//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : disk
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _SI_DISK_H_
#define _SI_DISK_H_

#include "def.h"
//////////////////////////////////////////////////////////////////////
#define _PATH_MOUNTED   "/etc/mtab"
#define MAX_LOGICDISK_COUNT	26

typedef	struct _tagLogicDisk
{
	char szDesc[MAX_NAME];
	unsigned int nTotalSize;
	unsigned int nFreeSize;
	
}LOGICDISK,*PLOGICDISK;

typedef	struct _tagDISKInfo
{
	LOGICDISK	LogicDisk[MAX_LOGICDISK_COUNT];
	int			m_nMountLDCount;

}DISKINFO,*PDISKINFO;

class _DISKInfo
{
public:
	
	_DISKInfo( );
	~_DISKInfo( );
	
public:
	
	int Init( );
	int GetDISKInfo( DISKINFO& DI );
	
private:
	
	void _UpdateDI( );
	
private:
	
	DISKINFO	m_DI;
};

#endif

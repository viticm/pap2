//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 17:00
//      File_base        : disk
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "disk.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

_DISKInfo::_DISKInfo( )
{
	memset( &m_DI, 0, sizeof( DISKINFO ) );
}

_DISKInfo::~_DISKInfo( )
{
}

int _DISKInfo::Init( )
{
	_UpdateDI( );
	return fseye_success;
}

int _DISKInfo::GetDISKInfo( DISKINFO& DI )
{
	_UpdateDI( );

	DI = m_DI;
	
	return fseye_success;
}

void _DISKInfo::_UpdateDI( )
{
	int nUseCount = 0;

#ifdef WIN32
	char szPath[MAX_NAME];

	INT64	FreeSpace;
	INT64	TotalSpace;
	INT64	FreeToCaller;

	strcpy( szPath, "A:\\" );

	for( int nLoopCount = 0 ; nLoopCount < MAX_LOGICDISK_COUNT; nLoopCount++ )
	{
		szPath[0] = 'C' + nLoopCount;

		BOOL nRet =
		GetDiskFreeSpaceEx (
			szPath,
			(PULARGE_INTEGER)&FreeToCaller,
			(PULARGE_INTEGER)&TotalSpace,
			(PULARGE_INTEGER)&FreeSpace);

		if( nRet == TRUE )
		{
			strcpy( m_DI.LogicDisk[nUseCount].szDesc, szPath );
			m_DI.LogicDisk[nUseCount].nTotalSize =	(unsigned int)(TotalSpace / (1024 * 1024));
			m_DI.LogicDisk[nUseCount].nFreeSize	 =	(unsigned int)(FreeSpace / (1024 * 1024));
			nUseCount++;
		}
	}

#else

	char szDir[MAX_NAME];
	char szFS[MAX_NAME];
	char szTemp[MAX_NAME];

	INT64	FreeSpace;
	INT64	TotalSpace;
	
	FILE* dev = fopen("/proc/mounts", "r");
	do
	{
		fscanf( 
			dev, "%s%s%s%[^\n]", 
			szDir, 
			szDir, 
			szFS,
			szTemp);
		
		if( !strcmp( szFS, "ext3" ) || 
			!strcmp( szFS, "ext2" ) ||
			!strcmp( szFS, "vfs" ) )
		{
			struct statfs sfs;
			statfs( szDir, &sfs );

			strncpy( 
				m_DI.LogicDisk[nUseCount].szDesc, 
				szDir,
				MAX_NAME );

			m_DI.LogicDisk[nUseCount].szDesc[MAX_NAME-1] = 0;

			FreeSpace = sfs.f_bfree;
			FreeSpace *= sfs.f_bsize;
			m_DI.LogicDisk[nUseCount].nFreeSize		=	FreeSpace / ( 1024 * 1024 ) ;

			TotalSpace = sfs.f_blocks;
			TotalSpace *= sfs.f_bsize;
			m_DI.LogicDisk[nUseCount].nTotalSize	=	TotalSpace / ( 1024 * 1024 );
			
			nUseCount++;

			if( nUseCount >= MAX_LOGICDISK_COUNT )
				break;
		}
	}while( !feof( dev ) );
	
	fclose( dev );
	
#endif

	m_DI.m_nMountLDCount = nUseCount;
}
	

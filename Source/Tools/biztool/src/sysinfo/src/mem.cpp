//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 17:00
//      File_base        : mem
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "mem.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

_MEMInfo::_MEMInfo( )
{
	memset( &m_MI, 0, sizeof(MEMINFO) );

}

_MEMInfo::~_MEMInfo( )
{
}

int _MEMInfo::Init( )
{
	_UpdateMI( );
	return fseye_success;
}

int _MEMInfo::GetMEMInfo( MEMINFO& MI )
{
	_UpdateMI( );
	MI = m_MI;
	return fseye_success;
}

void _MEMInfo::_UpdateMI( )
{

#ifdef WIN32
	MEMORYSTATUS	MS;
	GlobalMemoryStatus( &MS );

	m_MI.nTotalMem	= MS.dwTotalPhys / 1024;
	m_MI.nFreeMem	= MS.dwAvailPhys / 1024;
#else

	char* szfile = ftostr( "/proc/meminfo" );
	char* szfind = NULL;

	if( szfile )
	{
		szfind = strstr( szfile, "MemTotal" );

		if( szfind )
			sscanf( szfind, "MemTotal:\t%u kB", 
			&m_MI.nTotalMem );


		szfind = strstr( szfile, "MemFree" );
		
		if( szfind )
			sscanf( szfind, "MemFree:\t%u kB", 
			&m_MI.nFreeMem );
	}
	
#endif
	
}

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 17:00
//      File_base        : proc
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "proc.h"
#include "ntqsi.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////
_PROCInfo::_PROCInfo( )
{
	memset( &m_PI, 0, sizeof(PROCINFO) );
}

_PROCInfo::~_PROCInfo( )
{
}

int _PROCInfo::Init( )
{
	_UpdatePI( );
	return fseye_success;
}

int _PROCInfo::GetPROCInfo( PROCINFO& PI )
{
	_UpdatePI( );

	PI = m_PI;
	return fseye_success;
}

void _PROCInfo::_UpdatePI( )
{
	int nUseCount = 0;

#ifdef WIN32
	HINSTANCE hLib = LoadLibrary("ntdll.dll");
	if( hLib == NULL )
		return;
	
	NtQuerySystemInformation NTQSI = (NtQuerySystemInformation)
		GetProcAddress(hLib,"NtQuerySystemInformation");
	
	if( NTQSI == NULL )
	{
		FreeLibrary( hLib );
		return;
	}
	
	char szBuf[MAX_TEMPBUF * 20];
	ULONG	ulRetLen;
	
	unsigned long ulRet = 
		NTQSI( 
		SystemProcessInformation,
		szBuf,
		MAX_TEMPBUF * 20,
		&ulRetLen );

	ULONG ulTotalOffset = 0;

	PSYSTEM_PROCESSES pSystemProc = (PSYSTEM_PROCESSES)(szBuf);

		
	while( pSystemProc )
	{	
		if( pSystemProc->ProcessId == 0 )
		{
			strcpy( m_PI.Process[nUseCount].szName, "System Idle Process" );
		}
		else
		{
			WideCharToMultiByte( 
				CP_ACP, 
				0, 
				pSystemProc->ProcessName.Buffer, 
				pSystemProc->ProcessName.Length,
				m_PI.Process[nUseCount].szName,
				MAX_NAME,
				NULL,
				NULL);
		}

		m_PI.Process[nUseCount].nPid = pSystemProc->ProcessId;

		m_PI.Process[nUseCount].nMemUseSize	=
		pSystemProc->VmCounters.WorkingSetSize / 1024;
		m_PI.Process[nUseCount].nVmSize	=
		pSystemProc->VmCounters.PagefileUsage / 1024;
		m_PI.Process[nUseCount].nThreadCount = pSystemProc->ThreadCount;

		INT64 KnrlTime = 0;
		INT64 UserTime = 0;
		INT64 CPUTime = 0;

		memcpy( &KnrlTime, &pSystemProc->KernelTime, sizeof(INT64) );
		memcpy( &UserTime, &pSystemProc->UserTime, sizeof(INT64) );

		CPUTime = KnrlTime + UserTime;

		CPUTime /= 10000;
		CPUTime /= 1000;

		m_PI.Process[nUseCount].ulCPUTime = (unsigned long)CPUTime;

		nUseCount++;
		
		if( !pSystemProc->NextEntryDelta || 
			nUseCount >= MAX_PROCESS_COUNT )
			break;

		ulTotalOffset += pSystemProc->NextEntryDelta;
		pSystemProc = (PSYSTEM_PROCESSES)( szBuf + ulTotalOffset );
		

	}

	

	FreeLibrary( hLib );
#else
	   DIR*        dir;
	   dirent*     ent;
	   
	   dir = opendir( "/proc" );
	   
	   while( ent = readdir( dir ) )
	   {
		   if( ent->d_name[0] > '0' && ent->d_name[0] < '9' )
		   {
			   int nPID = atoi( ent->d_name );
			   char szFileName[MAX_NAME] = {0};
			   char* szfile = NULL;
			   char* szfind = NULL;
			   unsigned long ulTemp;
			   INT64 KnrlTime;
			   INT64 UserTime;

			   sprintf( szFileName, "/proc/%d/statm", nPID );
			   szfile = ftostr( szFileName );
			   if( szfile )
			   sscanf( szfile, "%u", 
					&m_PI.Process[nUseCount].nMemUseSize );


			   sprintf( szFileName, "/proc/%d/status", nPID );
			   szfile = ftostr( szFileName );

			   if( szfile )
			   {
					szfind = strstr( szfile, "Name" );
					if( szfind )
						sscanf( 
						szfind, 
						"Name:\t%s", 
						m_PI.Process[nUseCount].szName );
					m_PI.Process[nUseCount].szName[MAX_NAME/6-1] = 0;

					szfind = strstr( szfile, "VmSize" );
					if( szfind )
						sscanf( 
						szfind, 
						"VmSize:\t%u kB", 
						&m_PI.Process[nUseCount].nVmSize );

					szfind = strstr( szfile, "Threads" );
						if( szfind )
						sscanf( 
						szfind, 
						"Threads:\t%d", 
						&m_PI.Process[nUseCount].nThreadCount );
			   }

			   m_PI.Process[nUseCount].nPid = nPID;

			   sprintf( szFileName, "/proc/%d/stat", nPID );
			   
			   szfile = ftostr( szFileName );
			   if( szfile )
				  sscanf(
					szfile,
					"%d "
					"%s "
					"%c "
					"%d %d %d %d %d "
					"%lu %lu %lu %lu %lu %Lu %Lu ",
					&ulTemp,
					szFileName,
					&szFileName[0],
					&ulTemp, &ulTemp, &ulTemp, &ulTemp, &ulTemp,
					&ulTemp, &ulTemp, &ulTemp, &ulTemp, &ulTemp, &KnrlTime, &UserTime);

			   m_PI.Process[nUseCount].ulCPUTime = ( KnrlTime + UserTime ) / 100;

			   nUseCount++;
		   }

		   if( nUseCount >= MAX_PROCESS_COUNT )
			   break;
	   }
	   
	   closedir( dir );
	   
#endif
	   m_PI.nProcessCount = nUseCount;
}

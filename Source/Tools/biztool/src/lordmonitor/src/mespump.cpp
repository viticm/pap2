//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-03-12 17:00
//      File_base        : sysinfo.cpp
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "mespump.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

mespump::mespump( )
{
	m_pShareMemToLord = NULL;
	m_ToLord		=	NULL;
	m_FromLord		=	NULL;
}

mespump::~mespump( )
{

}

int mespump::Init( )
{
	//=======================================================================
	
	m_pShareMemToLord = GetShareMemory( TOLORD );
	
	if( m_pShareMemToLord == NULL )
		return FALSE;
	
	m_ToLord = new ( m_pShareMemToLord ) IPCStream( KB, MB, RB_MAXPACK );
	
	//=======================================================================
	
	m_pShareMemFromLord = GetShareMemory( FROMLORD );

	if( m_pShareMemFromLord == NULL )
		return FALSE;

	m_FromLord = (IPCStream*)m_pShareMemFromLord;
	
	//=======================================================================
	
	return TRUE;
}

int mespump::SendPack( const void* pData, unsigned int datasize )
{
	return m_ToLord ? m_ToLord->SndPack( pData, datasize ) : 0;
}

const void* mespump::RcvPack( unsigned int& datasize )
{
	return m_FromLord ? m_FromLord->RcvPack( datasize ) : NULL;
}

void* mespump::GetShareMemory( 
	const char* pKeyName )
{
#ifdef WIN32

	HANDLE hMapFile;
	LPVOID lpMapAddress;
	
	hMapFile = 
	OpenFileMapping(
		FILE_MAP_ALL_ACCESS,		// read/write permission 
		FALSE,						// Do not inherit the name
		pKeyName );					// of the mapping object. 
	
	if (hMapFile == NULL) 
	{
		hMapFile = 
		CreateFileMapping(
			INVALID_HANDLE_VALUE,				// current file handle 
			NULL,								// default security 
			PAGE_READWRITE,						// read/write permission 
			0,									// max. object size 
			MB + KB,							// size of hFile 
			pKeyName );							// name of mapping object 

		if (hMapFile == NULL) 
			return NULL;
	} 
	
	lpMapAddress = 
	MapViewOfFile(
		hMapFile,							// handle to mapping object 
		FILE_MAP_ALL_ACCESS,				// read/write permission 
		0,									// max. object size 
		0,									// size of hFile 
		0);									// map entire file 

	return lpMapAddress;

#else

#define SHM_MOD (SHM_R | SHM_W | SHM_R >> 3 | SHM_R >> 6 )

	int shm;
	int nLoopCount = 0;
	key_t key = 0;

	while( pKeyName[nLoopCount] )
	{
		key += pKeyName[nLoopCount];
		nLoopCount++;
	}

	if( ( shm = shmget( key, MB + KB, SHM_MOD | IPC_CREAT | IPC_EXCL ) ) < 0 )
	{
		if( errno == EEXIST )
		{
			shm = shmget( key, MB + KB, SHM_MOD );
		}
	}

	if( shm < 0 )
		return NULL;

	return shmat( shm, NULL, 0 );

#endif

	return NULL;
}

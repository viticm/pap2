//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 17:00
//      File_base        : cpu
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "cpu.h"
#include "ntqsi.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

#define MASK_STEPPINGID 0x0000000F
#define MASK_PTYE		0x00003000

char	VENDOR_INTEL[] =	"GenuineIntel";
char	VENDOR_AMD[] =		"AuthenticAMD";
char	VENDOR_SIS[] =		"SiS SiS SiS";
char	VENDOR_CYRIX[] =	"CyrixInstead";

_CPUInfo::_CPUInfo( )
{
	memset( &m_CI, 0, sizeof( CPUINFO ) );
	m_ulLoadStart	= 0;
}

_CPUInfo::~_CPUInfo( )
{
}


int _CPUInfo::Init( )
{
	_UpdateCI( );
	_UpdateCL( );

	return fseye_success;
}

int _CPUInfo::GetCPUInfo( CPUINFO& CI )
{
	_UpdateCL( );

	CI = m_CI;

	return fseye_success;
}

unsigned long _CPUInfo::GetTimetoTickCount( )
{
	unsigned long ulTickCount = 0;
#ifdef WIN32
	ulTickCount = GetTickCount( );
#else
	static timeval start = { 0, 0 };
	if( start.tv_sec == 0 )
		gettimeofday( &start, NULL );
	
	timeval now = {0,0};
	gettimeofday( &now, NULL );
	
	ulTickCount = 
		( ( now.tv_sec - start.tv_sec ) * 1000 ) + 
		( now.tv_usec / 1000 ) - 
		( start.tv_usec / 1000 );
	
#endif

	return ulTickCount;
}

void _CPUInfo::_UpdateCI( )
{
	
#ifdef WIN32
	
	LOGICPROCESSOR lp = {0};

	char* pVendor = lp.szVendor;
	char* pName = lp.szName;
	void* pFun = strcmp;
	_asm
	{
		mov esi, pVendor
		xor edi,edi

		xor eax,eax
		CPUID

		mov [esi + 0],ebx
		mov [esi + 4], edx
		mov [esi + 8], ecx
		mov [esi + 12], 0

		mov eax,1
		CPUID
		
		//stepping id
		mov edi,MASK_STEPPINGID
		mov esi,eax
		and esi,edi
		mov lp.nSteppingID ,esi

		//model
		shl edi,4
		mov esi,eax
		and esi,edi
		shr	esi,4
		mov lp.nModel ,esi
		
		//family
		shl edi,4
		mov esi,eax
		and esi,edi
		shr	esi,8
		mov lp.nFamily ,esi

		//process type
		mov edi,MASK_PTYE
		mov esi,eax
		and esi,edi
		shr esi,12
		mov lp.nType, esi


		//
		//test edx,0x04000000
		//jz _NEXT;
		//mov lp.bSSE2,1
		
		push OFFSET VENDOR_INTEL
		push pVendor
		call pFun
		add esp,8
		test eax,eax
		
		jne	__AMD

		mov eax,1
		CPUID

		//sse2
		bt edx,26
		jnc _SSE
		mov lp.bSSE2,1

_SSE:
		//sse
		bt edx,25
		jnc _MMX
		mov lp.bSSE,1

_MMX:

		//mmx
		bt edx,23
		jnc _CPUNAME
		mov lp.bMMX,1

		jmp _CPUNAME

__AMD:
		push OFFSET VENDOR_AMD
		push pVendor
		call pFun
		add esp,8
		test eax,eax
		jne _CPUNAME


		mov eax,0x80000001
		CPUID

		//mmx
		bt edx,23
		jnc _3DNOW
		mov lp.bMMX,1
			
_3DNOW:
		bt edx,31
		jnc _3DNOWEXT
		mov lp.b3DNow,1

_3DNOWEXT:
		bt edx,30
		jnc _MMXEXT
		mov lp.b3DNowEXT,1

_MMXEXT:
		bt edx,22
		jnc _CPUNAME
		mov lp.bMMXEXT,1

_CPUNAME:
		//name 
		mov eax,0x80000002
		CPUID

		mov esi,pName
		mov [esi + 0],	eax
		mov [esi + 4],	ebx
		mov [esi + 8],	ecx
		mov [esi + 12],	edx


		mov eax,0x80000003
		CPUID
		
		mov esi,pName
		mov [esi + 16],	eax
		mov [esi + 20],	ebx
		mov [esi + 24],	ecx
		mov [esi + 28],	edx


		mov eax,0x80000004
		CPUID
		
		mov esi,pName
		mov [esi + 32],	eax
		mov [esi + 36],	ebx
		mov [esi + 40],	ecx
		mov [esi + 44],	edx

		//
	}

	if( lp.bMMX )
		strcat( lp.szDesc, "MMX " );
	
	if( lp.bMMXEXT )
		strcat( lp.szDesc, "MMX Ext " );

	if( lp.bSSE )
		strcat( lp.szDesc, "SSE " );
	
	if( lp.bSSE2 )
		strcat( lp.szDesc, "SSE2 " );

	if( lp.b3DNow )
		strcat( lp.szDesc, "3DNow " );

	if( lp.b3DNowEXT )
		strcat( lp.szDesc, "3DNow Ext " );

	lp.szName[MAX_NAME-1]	= 0;
	lp.szVendor[MAX_NAME-1]	= 0;
	lp.szDesc[MAX_NAME-1]	= 0;

	SYSTEM_INFO	si = {0};
	GetSystemInfo( &si );

	m_CI.nProcessorCount = 
	si.dwNumberOfProcessors > MAX_PROCESSOR_COUNT ? MAX_PROCESSOR_COUNT : si.dwNumberOfProcessors;

	memcpy( 
		&m_CI.Processor,
		&lp, 
		sizeof( LOGICPROCESSOR ) * m_CI.nProcessorCount );

#else

	char szContent[MAX_NAME];
	int nCPUCount;

	FILE* dev = fopen("/proc/cpuinfo", "r");
	
	if( dev == NULL )
		return;

	do {
		
		fscanf( dev, "%s", szContent );
		
		if( !strncmp( "processor", szContent, 9 ) )
		{
			fscanf( dev, "%[^:]:%u", szContent, &nCPUCount );		
			fscanf( dev, " %[^:]:%[^\n]", szContent, m_CI.Processor[nCPUCount].szVendor );
			fscanf( dev, " %[^:]:%u", szContent, &m_CI.Processor[nCPUCount].nFamily );
			fscanf( dev, " %[^:]:%u", szContent, &m_CI.Processor[nCPUCount].nModel );
			fscanf( dev, " %[^:]:%[^\n]", szContent, m_CI.Processor[nCPUCount].szName );
			fscanf( dev, " %[^:]:%u", szContent, &m_CI.Processor[nCPUCount].nSteppingID );
			
			while( true )
			{
				fscanf( dev, " %[^:]:%[^\n]", szContent, &m_CI.Processor[nCPUCount].szDesc );
				if( strncmp( "flags", szContent, 5 ) == 0 )
					break;
			}

			m_CI.Processor[nCPUCount].szName[MAX_NAME-1]	= 0;
			m_CI.Processor[nCPUCount].szVendor[MAX_NAME-1]	= 0;
			m_CI.Processor[nCPUCount].szDesc[MAX_NAME-1]	= 0;
		}
		
	} while( !feof( dev ) && nCPUCount < MAX_PROCESSOR_COUNT );
	
	fclose( dev );

	m_CI.nProcessorCount = nCPUCount + 1;
	
#endif
}


void _CPUInfo::_UpdateCL( )
{
#ifdef WIN32
	
	unsigned long ulElapsedTime = 
	GetTimetoTickCount( ) - m_ulLoadStart;
	
	if( ulElapsedTime == 0 )
		return;
	
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

	char	szBuf[MAX_TEMPBUF];
	ULONG	ulRetLen;

	unsigned long ulRet = 
	NTQSI( 
		SystemProcessorPerformanceInformation,
		szBuf,
		MAX_TEMPBUF,
		&ulRetLen );

	if( ulRet != fseye_success )
		return;

	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* pSI = 
	(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*)szBuf;

	int nAllCPULoad = 0;

	for( int nLoopCount = 0; nLoopCount < m_CI.nProcessorCount; nLoopCount++ )
	{
		INT64 IdleTime = 0;

		memcpy( &IdleTime, &pSI[nLoopCount].IdleTime, sizeof(INT64) );

		unsigned int nCPUIdle = (unsigned int)((IdleTime - m_CI.Processor[nLoopCount].CIdleTime) * 100 / (ulElapsedTime * 10000));
		
		nCPUIdle = nCPUIdle > 100 ? 100 : nCPUIdle;
		m_CI.Processor[nLoopCount].CIdleTime = IdleTime;
		m_CI.Processor[nLoopCount].nLoad = 100 - nCPUIdle;

		nAllCPULoad += m_CI.Processor[nLoopCount].nLoad;
	}


	m_CI.nTotalLoad = nAllCPULoad / m_CI.nProcessorCount;

	FreeLibrary( hLib );

	
#else
	
	char szContent[MAX_NAME];
	char szTemp[MAX_NAME];
	INT64 nTemp;
	INT64 IdleTick = 0;

	unsigned long ulElapsedTime = 
	GetTimetoTickCount( ) - m_ulLoadStart;

	if( ulElapsedTime == 0 )
		return;

	char* szfile = ftostr( "/proc/stat" );
	int nAllCPULoad = 0;

	for( int nLoopCount = 0; nLoopCount < m_CI.nProcessorCount; nLoopCount++ )
	{
		INT64 IdleTime = 0;

		sprintf( szTemp, "cpu%d", nLoopCount );
		char* szfind = strstr( szfile, szTemp );

		sscanf(
			szfind, 
			"%s %Lu %Lu %Lu %Lu %[^\n]", 
			szContent, 
			&nTemp, 
			&nTemp, 
			&nTemp, 
			&IdleTime,
			szTemp );
		
		unsigned int nCPUIdle = 
		( ( IdleTime - m_CI.Processor[nLoopCount].CIdleTime ) * 100 ) / ( ulElapsedTime / 10 + 1 );
		
		m_CI.Processor[nLoopCount].CIdleTime = IdleTime;
		nCPUIdle = nCPUIdle > 100 ? 100 : nCPUIdle;
		m_CI.Processor[nLoopCount].nLoad = 100 - nCPUIdle;

		nAllCPULoad += m_CI.Processor[nLoopCount].nLoad;
	}

	m_CI.nTotalLoad = nAllCPULoad / m_CI.nProcessorCount;

#endif

	m_ulLoadStart = GetTimetoTickCount( );
}

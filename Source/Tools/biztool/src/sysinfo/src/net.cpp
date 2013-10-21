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

#include "net.h"
#include "iphelp.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

_NETInfo::_NETInfo( )
{
	memset( &m_NI, 0, sizeof( NETINFO ) );
	m_ulLoadStart = 0;
}

_NETInfo::~_NETInfo( )
{

}

int _NETInfo::Init( )
{
	_UpdateNI( );
	_UpdateNL( );
	return fseye_success;
}

int _NETInfo::GetNETInfo( NETINFO& NI )
{
	_UpdateNL( );

	NI = m_NI;

	return fseye_success;
}

void _NETInfo::_UpdateNI( )
{
	
	int nUseCount = 0;

	GetSystemName( );
	
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int nRet;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	nRet = WSAStartup( wVersionRequested, &wsaData );
	if ( nRet != 0 )
		return;

	nRet = gethostname( m_NI.szHostName, MAX_NAME );

	if( nRet != 0 )
		return;

	unsigned char szBuf[MAX_TEMPBUF];
	

	
    HINSTANCE hLib = LoadLibrary("iphlpapi.dll");

    if( hLib == NULL )
		return;

    PGETADAPTERSINFO	GetAdapterInfo = (PGETADAPTERSINFO)GetProcAddress(hLib,"GetAdaptersInfo");
	PGETIFENTRY			GetIfEntry = (PGETIFENTRY)GetProcAddress(hLib,"GetIfEntry");

	if ( GetAdapterInfo == NULL ||
		GetIfEntry == NULL )
	{
		FreeLibrary( hLib );
		return;
	}

	
	int nLoopCount = 0;
	for( nLoopCount = 0; nLoopCount < MAX_NETCARD_COUNT; nLoopCount++ )
	{
		MIB_IFROW	MIFROW;
		MIFROW.dwIndex = nLoopCount;

		if( GetIfEntry( &MIFROW ) == fseye_success &&
			MIFROW.dwType == MIB_IF_TYPE_ETHERNET )
		{
			m_NI.Card[nUseCount].CRTXI.ulRXByte		=	MIFROW.dwInOctets;
			m_NI.Card[nUseCount].CRTXI.ulRXPackage	=	MIFROW.dwInNUcastPkts;
			m_NI.Card[nUseCount].CRTXI.ulTXByte		=	MIFROW.dwOutOctets;
			m_NI.Card[nUseCount].CRTXI.ulTXPackage	=	MIFROW.dwOutUcastPkts;
			m_NI.Card[nUseCount].ulMTU				=	MIFROW.dwMtu;
			m_NI.Card[nUseCount].ulMacLen			=	MIFROW.dwPhysAddrLen;
			m_NI.Card[nUseCount].ulDescLen			=	MIFROW.dwDescrLen;

			memcpy( 
				m_NI.Card[nUseCount].Mac, 
				MIFROW.bPhysAddr, 
				MIFROW.dwPhysAddrLen );

			sprintf( 
				m_NI.Card[nUseCount].szMac, 
				"%02X-%02X-%02X-%02X-%02X-%02X", 
				m_NI.Card[nUseCount].Mac[0],
				m_NI.Card[nUseCount].Mac[1],
				m_NI.Card[nUseCount].Mac[2],
				m_NI.Card[nUseCount].Mac[3],
				m_NI.Card[nUseCount].Mac[4],
				m_NI.Card[nUseCount].Mac[5]);

			memcpy( 
				m_NI.Card[nUseCount].szDesc, 
				MIFROW.bDescr, 
				MAX_NAME );

			m_NI.Card[nUseCount].szDesc[MAX_NAME-1] = 0;
			
			nUseCount++;
		}
	}


	K_PIP_ADAPTER_INFO pInfo = (K_PIP_ADAPTER_INFO)szBuf;
	unsigned long ulSize = 0;

	GetAdapterInfo( NULL, &ulSize );

	if( ulSize > MAX_TEMPBUF )
	{
		FreeLibrary( hLib );
		return;
	}

	GetAdapterInfo( pInfo, &ulSize );

	while ( pInfo )
	{
		//card
		K_PIP_ADDR_STRING pAddrList = &pInfo->IpAddressList;

		while( pAddrList )
		{
			unsigned long ulAddress = inet_addr( pAddrList->IpAddress.String );
			unsigned long ulNetMask = inet_addr( pAddrList->IpMask.String );

			for( nLoopCount = 0; nLoopCount < nUseCount; nLoopCount++ )
			{
				if( memcmp( 
					m_NI.Card[nLoopCount].Mac, 
					pInfo->Address, 
					m_NI.Card[nLoopCount].ulMacLen ) == fseye_success )
				{
					m_NI.Card[nLoopCount].ulIP = ulAddress;
					m_NI.Card[nLoopCount].ulNetMask	= ulNetMask;

					strcpy( m_NI.Card[nLoopCount].szIP, pAddrList->IpAddress.String );
					strcpy( m_NI.Card[nLoopCount].szNetMask, pAddrList->IpMask.String );

					break;
				}
			}
			
			pAddrList = pAddrList->Next;
		}

		//
		pInfo = pInfo->Next;
	}
	
	
#else
	int sock;
	struct ifreq ifrIP;
	struct ifreq ifrMAC;
	struct ifreq ifrMASK;
	struct ifreq ifrMTU;

	gethostname( m_NI.szHostName, MAX_NAME );
	

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	if ( sock >= 0 )
	{
		strcpy(ifrIP.ifr_name, "eth0");

		for( int nLoopCount = 0; nLoopCount < MAX_NETCARD_COUNT; nLoopCount ++)
		{
			ifrIP.ifr_name[3] = '0' + nLoopCount;
			ifrIP.ifr_addr.sa_family = AF_INET;
			ifrMAC = ifrIP;
			ifrMASK = ifrIP;
			ifrMTU	= ifrIP;
			
			if( ioctl(sock, SIOCGIFADDR, &ifrIP) >= 0 &&
				ioctl(sock, SIOCGIFHWADDR, &ifrMAC) >= 0 &&
				ioctl(sock, SIOCGIFNETMASK, &ifrMASK) >= 0 &&
				ioctl(sock, SIOCGIFMTU, &ifrMTU) >= 0 )
			{
				unsigned long ulAddress = (((sockaddr_in*) &(ifrIP.ifr_addr))->sin_addr).s_addr;
				unsigned long ulNetMask = (((sockaddr_in*) &(ifrMASK.ifr_netmask))->sin_addr).s_addr;
				in_addr addr;
				addr.s_addr = ulAddress;

				m_NI.Card[nLoopCount].ulMacLen			=	MAX_MAC;
				m_NI.Card[nLoopCount].ulDescLen			=	MAX_NAME;
				
				memcpy( 
					m_NI.Card[nLoopCount].Mac, 
					ifrMAC.ifr_hwaddr.sa_data, 
					MAX_MAC );

				sprintf( 
					m_NI.Card[nUseCount].szMac, 
					"%02X-%02X-%02X-%02X-%02X-%02X", 
					m_NI.Card[nUseCount].Mac[0],
					m_NI.Card[nUseCount].Mac[1],
					m_NI.Card[nUseCount].Mac[2],
					m_NI.Card[nUseCount].Mac[3],
					m_NI.Card[nUseCount].Mac[4],
					m_NI.Card[nUseCount].Mac[5]);
				
				memcpy( 
					m_NI.Card[nLoopCount].szDesc, 
					ifrIP.ifr_name, 
					MAX_NAME );

				m_NI.Card[nLoopCount].szDesc[MAX_NAME-1] = 0;
				
				m_NI.Card[nLoopCount].ulIP = ulAddress;
				m_NI.Card[nLoopCount].ulNetMask	= ulNetMask;
				
				strcpy( m_NI.Card[nLoopCount].szIP, inet_ntoa( addr ) );
				
				addr.s_addr = ulNetMask;
				strcpy( m_NI.Card[nLoopCount].szNetMask, inet_ntoa( addr ) );

				m_NI.Card[nLoopCount].ulMTU				=	ifrMTU.ifr_mtu;

				nUseCount++;
				//////////////////////////////////////////////////////////////////////////
			}
		}
		close( sock );
	}

#endif

	m_NI.nNETCardCount = nUseCount;
	
}

void _NETInfo::_UpdateNL( )
{
#ifdef WIN32

	time_t tNow = time(NULL);
	unsigned long ulElapsed = (unsigned long)(tNow - m_ulLoadStart);
	
	if( !ulElapsed )
		return;

	typedef unsigned long (__stdcall * PGETIFENTRY)( PMIB_IFROW );

    HINSTANCE hLib = LoadLibrary("iphlpapi.dll");
    if( hLib == NULL )
		return;
	
	PGETIFENTRY	GetIfEntry = (PGETIFENTRY)GetProcAddress(hLib,"GetIfEntry");
	
	if ( GetIfEntry == NULL )
	{
		FreeLibrary( hLib );
		return;
	}

	for( int nLoopCount = 0; nLoopCount < MAX_NETCARD_COUNT; nLoopCount++ )
	{
		MIB_IFROW	MIFROW;
		MIFROW.dwIndex = nLoopCount;
		
		if( GetIfEntry( &MIFROW ) == fseye_success &&
			MIFROW.dwType == MIB_IF_TYPE_ETHERNET )
		{
			for (unsigned long i = 0; i < m_NI.nNETCardCount; ++i)
			{
				if( !memcmp( 
					m_NI.Card[i].Mac, 
					MIFROW.bPhysAddr,
					m_NI.Card[i].ulMacLen ) )
				{
					m_NI.Card[i].CRTXI.ulRXRate	=	
					( MIFROW.dwInOctets - m_NI.Card[i].CRTXI.ulRXByte ) / ulElapsed;

					m_NI.Card[i].CRTXI.ulTXRate	=	
					( MIFROW.dwOutOctets - m_NI.Card[i].CRTXI.ulTXByte ) / ulElapsed;

					m_NI.Card[i].CRTXI.ulRXByte		=	MIFROW.dwInOctets;
					m_NI.Card[i].CRTXI.ulRXPackage	=	MIFROW.dwInNUcastPkts;
					m_NI.Card[i].CRTXI.ulTXByte		=	MIFROW.dwOutOctets;
					m_NI.Card[i].CRTXI.ulTXPackage	=	MIFROW.dwOutUcastPkts;
					break;
				}
			}
		}
	}

	m_ulLoadStart = (unsigned long)tNow;
#else

	unsigned long ulElapsed = time( NULL ) - m_ulLoadStart;

	if( !ulElapsed )
		return;

	char* szfile = ftostr( "/proc/net/dev" );
	
	if( !szfile )
		return;

	for( int nLoopCount = 0; nLoopCount < m_NI.nNETCardCount; nLoopCount ++)
	{
		unsigned long ulTemp;
		unsigned long ulRXPackage;
		unsigned long ulTXPackage;
		unsigned long ulRXByte;
		unsigned long ulTXByte;
		char szDevName[MAX_NAME];
		char* szfind = strstr( szfile, m_NI.Card[nLoopCount].szDesc );

		if( szfind )
		{
			sscanf( szfind, " %6[^:]:%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",
				szDevName,
				&ulRXByte, 
				&ulRXPackage, 
				&ulTemp, &ulTemp,
				&ulTemp, &ulTemp, &ulTemp, &ulTemp,
				&ulTXByte,
				&ulTXPackage, 
				&ulTemp, &ulTemp,
				&ulTemp, &ulTemp, &ulTemp, &ulTemp);

			m_NI.Card[nLoopCount].CRTXI.ulRXRate	=	
				( ulRXByte - m_NI.Card[nLoopCount].CRTXI.ulRXByte ) / ulElapsed;
			
			m_NI.Card[nLoopCount].CRTXI.ulTXRate	=	
				( ulTXByte - m_NI.Card[nLoopCount].CRTXI.ulTXByte ) / ulElapsed;

			m_NI.Card[nLoopCount].CRTXI.ulRXByte	=	ulRXByte;
			m_NI.Card[nLoopCount].CRTXI.ulTXByte	=	ulTXByte;
			m_NI.Card[nLoopCount].CRTXI.ulRXPackage	=	ulRXPackage;
			m_NI.Card[nLoopCount].CRTXI.ulTXPackage	=	ulTXPackage;
		}
	}
	
	m_ulLoadStart = time( NULL );
	
#endif

}

unsigned long _NETInfo::GetTimetoTickCount( )
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

void _NETInfo::GetSystemName( )
{
#ifdef WIN32
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return;
	}

	switch (osvi.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_NT:
		{
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
				strcat( m_NI.szSystemName, "Microsoft Windows Server 2003 family " );

			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
				strcat( m_NI.szSystemName, "Microsoft Windows XP " );

			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				strcat( m_NI.szSystemName, "Microsoft Windows 2000 " );

			if ( osvi.dwMajorVersion <= 4 )
				strcat( m_NI.szSystemName, "Microsoft Windows NT " );

			break;
		}

      case VER_PLATFORM_WIN32_WINDOWS:
	  {
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
				strcat( m_NI.szSystemName, "Microsoft Windows 95 " );

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				strcat( m_NI.szSystemName, "Microsoft Windows 98 " );

			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				strcat( m_NI.szSystemName, "Microsoft Windows Millennium Edition" );
			break;
	  }

      case VER_PLATFORM_WIN32s:
		  strcat( m_NI.szSystemName, "Microsoft Win32s" );
         break;
   }
#else

	struct utsname un;

	uname( &un );

	sprintf( 
		m_NI.szSystemName, 
		"%s %s", 
		un.sysname, 
		un.version );
	
#endif
}

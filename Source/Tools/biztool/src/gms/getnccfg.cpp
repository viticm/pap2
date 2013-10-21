//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-06-28 17:24
//      File_base        : getnccfg.cpp
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : Create New Player Function
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "getnccfg.h"


/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
//copy from inoutmac.h

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.

typedef struct {
    char String[4 * 4];
} K_IP_ADDRESS_STRING, *K_PIP_ADDRESS_STRING, K_IP_MASK_STRING, *K_PIP_MASK_STRING;

typedef struct _K_IP_ADDR_STRING {
    struct _K_IP_ADDR_STRING* Next;
    K_IP_ADDRESS_STRING IpAddress;
    K_IP_MASK_STRING IpMask;
    DWORD Context;
} K_IP_ADDR_STRING, *K_PIP_ADDR_STRING;

typedef struct _K_IP_ADAPTER_INFO {
    struct _K_IP_ADAPTER_INFO* Next;
    DWORD ComboIndex;
    char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    UINT AddressLength;
    BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD Index;
    UINT Type;
    UINT DhcpEnabled;
    K_PIP_ADDR_STRING CurrentIpAddress;
    K_IP_ADDR_STRING IpAddressList;
    K_IP_ADDR_STRING GatewayList;
    K_IP_ADDR_STRING DhcpServer;
    BOOL HaveWins;
    K_IP_ADDR_STRING PrimaryWinsServer;
    K_IP_ADDR_STRING SecondaryWinsServer;
    time_t LeaseObtained;
    time_t LeaseExpires;
} K_IP_ADAPTER_INFO, *K_PIP_ADAPTER_INFO;

#endif

/////////////////////////////////////////////////////////////////////////////

int GetServerNetCfg( SERVERNETCFG& ServerNetCfg )
{
#ifdef WIN32

	WORD wVersionRequested;
	WSADATA wsaData;
	int nRet;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	nRet = WSAStartup( wVersionRequested, &wsaData );
	if ( nRet != 0 )
		return INVALID_VALUE;

	nRet = gethostname( ServerNetCfg.szHostName, MAXHOSTNAME );
	if( nRet != 0 )
		return INVALID_VALUE;

	unsigned char szBuf[MAX_BUFFSIZE];
	
	typedef unsigned long (__stdcall * PGETADAPTERSINFO)(K_PIP_ADAPTER_INFO, PULONG);
    HINSTANCE hLib = LoadLibrary("iphlpapi.dll");

    if( hLib == NULL )
		return INVALID_VALUE;

    PGETADAPTERSINFO GetAdapterInfo = (PGETADAPTERSINFO)GetProcAddress(hLib,"GetAdaptersInfo");

	if ( GetAdapterInfo == NULL )
		return INVALID_VALUE;

	K_PIP_ADAPTER_INFO pInfo = (K_PIP_ADAPTER_INFO)szBuf;
	unsigned long ulSize = 0;

	GetAdapterInfo( NULL, &ulSize );

	if( ulSize > MAX_BUFFSIZE )
		return INVALID_VALUE;

	GetAdapterInfo( pInfo, &ulSize );

	while ( pInfo )
	{
		//card
		K_PIP_ADDR_STRING pAddrList = &pInfo->IpAddressList;

		while( pAddrList )
		{
			unsigned long ulAddress = inet_addr( pAddrList->IpAddress.String );
			unsigned long ulNetMask = inet_addr( pAddrList->IpMask.String );

			//////////////////////////////////////////////////////////////////////////

			if( ( ulAddress & ulNetMask ) ==  ( ServerNetCfg.ulIntranetMask & ulNetMask ) &&
				ServerNetCfg.ulIntranetIP == 0 )
			{
				strncpy( ServerNetCfg.szIntranetIP, pAddrList->IpAddress.String, MAX_IPSIZE );
				ServerNetCfg.ulIntranetIP = ulAddress;
				memcpy( ServerNetCfg.IntranetMac, pInfo->Address, MAX_MAC );
			}
			else
			{
				if( ServerNetCfg.ulInternetIP == 0 )
				{
					if( ServerNetCfg.ulInternetMask == 0 )
					{
						strncpy( ServerNetCfg.szInternetIP, pAddrList->IpAddress.String, MAX_IPSIZE );
						ServerNetCfg.ulInternetIP = ulAddress;
						memcpy( ServerNetCfg.InternetMac, pInfo->Address, MAX_MAC );
					}
					else
					{
						if( ( ulAddress & ulNetMask ) ==  ( ServerNetCfg.ulInternetMask & ulNetMask ) )
						{
							strncpy( ServerNetCfg.szInternetIP, pAddrList->IpAddress.String, MAX_IPSIZE );
							ServerNetCfg.ulInternetIP = ulAddress;
							memcpy( ServerNetCfg.InternetMac, pInfo->Address, MAX_MAC );
						}
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////

			pAddrList = pAddrList->Next;
		}

		//
		pInfo = pInfo->Next;
	}

	FreeLibrary( hLib );
#else
	int sock;
	struct ifreq ifrIP;
	struct ifreq ifrMAC;
	struct ifreq ifrMASK;

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	if ( sock >= 0 )
	{
		strcpy(ifrIP.ifr_name, "eth0");

		for( int nLoopCount = 0; nLoopCount < MAX_MAC; nLoopCount ++)
		{
			ifrIP.ifr_name[3] = '0' + nLoopCount;
			ifrIP.ifr_addr.sa_family = AF_INET;
			ifrMAC = ifrIP;
			ifrMASK = ifrIP;
			
			if( ioctl(sock, SIOCGIFADDR, &ifrIP) >= 0 &&
				ioctl(sock, SIOCGIFHWADDR, &ifrMAC) >= 0 &&
				ioctl(sock, SIOCGIFNETMASK, &ifrMASK) >= 0 )
			{
				unsigned long ulAddress = (((sockaddr_in*) &(ifrIP.ifr_addr))->sin_addr).s_addr;
				unsigned long ulNetMask = (((sockaddr_in*) &(ifrMASK.ifr_netmask))->sin_addr).s_addr;
				in_addr addr;
				addr.s_addr = ulAddress;
				//////////////////////////////////////////////////////////////////////////
				
				if( ( ulAddress & ulNetMask ) ==  ( ServerNetCfg.ulIntranetMask & ulNetMask ) &&
					ServerNetCfg.ulIntranetIP == 0 )
				{
					strncpy( ServerNetCfg.szIntranetIP, inet_ntoa( addr ), MAX_IPSIZE );
					ServerNetCfg.ulIntranetIP = ulAddress;
					memcpy( ServerNetCfg.IntranetMac, ifrMAC.ifr_hwaddr.sa_data, MAX_MAC );
				}
				else
				{
					if( ServerNetCfg.ulInternetIP == 0 )
					{
						if( ServerNetCfg.ulInternetMask == 0 )
						{
							strncpy( ServerNetCfg.szInternetIP, inet_ntoa( addr ), MAX_IPSIZE );
							ServerNetCfg.ulInternetIP = ulAddress;
							memcpy( ServerNetCfg.InternetMac, ifrMAC.ifr_hwaddr.sa_data, MAX_MAC );
						}
						else
						{
							if( ( ulAddress & ulNetMask ) ==  ( ServerNetCfg.ulInternetMask & ulNetMask ) )
							{
								strncpy( ServerNetCfg.szInternetIP, inet_ntoa( addr ), MAX_IPSIZE );
								ServerNetCfg.ulInternetIP = ulAddress;
								memcpy( ServerNetCfg.InternetMac, ifrMAC.ifr_hwaddr.sa_data, MAX_MAC );
							}
						}
					}
				}
				
				//////////////////////////////////////////////////////////////////////////
			}
		}
		close( sock );
	}
#endif
	return fseye_success;
}

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 11:00
//      File_base        : getsi
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _IPHELP_H_
#define _IPHELP_H_

#ifdef WIN32
#include "windows.h"

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_INTERFACE_NAME_LEN			256
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.
#define MAXLEN_IFDESCR 256
#define MAXLEN_PHYSADDR 8

#define MIB_IF_TYPE_OTHER               1
#define MIB_IF_TYPE_ETHERNET            6
#define MIB_IF_TYPE_TOKENRING           9
#define MIB_IF_TYPE_FDDI                15
#define MIB_IF_TYPE_PPP                 23
#define MIB_IF_TYPE_LOOPBACK            24
#define MIB_IF_TYPE_SLIP                28

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

typedef struct _MIB_IFROW
{
    WCHAR   wszName[MAX_INTERFACE_NAME_LEN];
    DWORD	dwIndex;
    DWORD	dwType;
    DWORD	dwMtu;
    DWORD	dwSpeed;
    DWORD	dwPhysAddrLen;
    BYTE	bPhysAddr[MAXLEN_PHYSADDR];
    DWORD	dwAdminStatus;
    DWORD	dwOperStatus;
    DWORD	dwLastChange;
    DWORD	dwInOctets;
    DWORD	dwInUcastPkts;
    DWORD	dwInNUcastPkts;
    DWORD	dwInDiscards;
    DWORD	dwInErrors;
    DWORD	dwInUnknownProtos;
    DWORD	dwOutOctets;
    DWORD	dwOutUcastPkts;
    DWORD	dwOutNUcastPkts;
    DWORD	dwOutDiscards;
    DWORD	dwOutErrors;
    DWORD	dwOutQLen;
    DWORD	dwDescrLen;
    BYTE	bDescr[MAXLEN_IFDESCR];
} MIB_IFROW,*PMIB_IFROW;

typedef unsigned long (__stdcall * PGETADAPTERSINFO)(K_PIP_ADAPTER_INFO, PULONG);
typedef unsigned long (__stdcall * PGETIFENTRY)( PMIB_IFROW );

#endif

#endif

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-16 17:00
//      File_base        : sysinfo.cpp
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#include "def.h"
#include "Ksdef.h"
#include "ifimple.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////


KSysPlugin::KSysPlugin( )
{
	m_pSender = NULL;
	for (int i = 0; i < fseye_protocol_count; ++i)
	{
		m_aryProtoProcess[i] = NULL;
	}
	m_aryProtoProcess[e2g_getcpubaseinfo_def] = &KSysPlugin::Process_GetCpuInfo;
	m_aryProtoProcess[e2g_getcpuload_def	] = &KSysPlugin::Process_GetCpuLoad;
	m_aryProtoProcess[e2g_getmeminfo_def	] = &KSysPlugin::Process_GetMemInfo;
	m_aryProtoProcess[e2g_getdiskinfo_def	] = &KSysPlugin::Process_GetDiskInfo;
	m_aryProtoProcess[e2g_getnetbaseinfo_def] = &KSysPlugin::Process_GetNetInfo;
	m_aryProtoProcess[e2g_getcardload_def	] = &KSysPlugin::Process_GetNetLoad;
	m_aryProtoProcess[e2g_getprocinfo_def	] = &KSysPlugin::Process_GetProcInfo;
}

KSysPlugin::~KSysPlugin( )
{
}

int KSysPlugin::LoadPlug(ISender* pSender, IGuard* pGuard)
{
	m_pSender = pSender;

	m_CI.Init( );
	m_MI.Init( );
	m_DI.Init( );
	m_NI.Init( );
	m_PI.Init( );
	
	return fseye_success;
}

int KSysPlugin::Release( )
{
	return fseye_success;
}

bool KSysPlugin::CheckProtocol(int nProtocol)
{
	return
		nProtocol >= 0 &&
		nProtocol < fseye_protocol_count &&
		m_aryProtoProcess[nProtocol] != NULL;
}

void KSysPlugin::Process_GetCpuInfo(const void* pData, unsigned int nLen)
{
	if (nLen != sizeof(KProtocolHead))
		return;

	g2e_getcpubaseinfo cbi;
	memset(&cbi, 0, sizeof(cbi));

	CPUINFO CI;
	m_CI.GetCPUInfo( CI );

	cbi.wProtocol = g2e_getcpubaseinfo_def;
	cbi.wServer = 0;
	cbi.nCPUCount = CI.nProcessorCount;
	for (int i = 0; i < CI.nProcessorCount; ++i)
	{
		int nSize = sizeof(cbi.Processor[i].szCPUName);

		strncpy( 
			cbi.Processor[i].szCPUName,
			CI.Processor[i].szName,
			nSize );
		cbi.Processor[i].szCPUName[nSize - 1] = 0;

		strncpy( 
			cbi.Processor[i].szCPUVendor,
			CI.Processor[i].szVendor,
			nSize );
		cbi.Processor[i].szCPUVendor[nSize - 1] = 0;

		strncpy( 
			cbi.Processor[i].szCPUDesc,
			CI.Processor[i].szDesc,
			nSize );
		cbi.Processor[i].szCPUDesc[nSize - 1] = 0;

		cbi.Processor[i].nCPULoad = CI.nTotalLoad;
	}

	m_pSender->SendPackToServer(
		(unsigned char*)&cbi, 
		sizeof(g2e_getcpubaseinfo));
}

void KSysPlugin::Process_GetCpuLoad(const void* pData, unsigned int nLen)
{
	if (nLen != sizeof(KProtocolHead))
		return;

	g2e_getcpuload cl;
	memset(&cl, 0, sizeof(cl));

	CPUINFO CI;
	m_CI.GetCPUInfo( CI );

	cl.wProtocol = g2e_getcpuload_def;
	cl.wServer	 = 0;
	cl.nCPUCount = CI.nProcessorCount;
	for( int nLoopCount = 0; nLoopCount < CI.nProcessorCount; nLoopCount++ )
		cl.nLoad[nLoopCount] = CI.Processor[nLoopCount].nLoad;

	m_pSender->SendPackToServer( 
		(unsigned char*)&cl, 
		sizeof(g2e_getcpuload) );
}

void KSysPlugin::Process_GetMemInfo(const void* pData, unsigned int nLen)
{
	if (nLen != sizeof(KProtocolHead))
		return;

	MEMINFO MI;
	m_MI.GetMEMInfo( MI );

	g2e_getmeminfo gmi;
	memset(&gmi, 0, sizeof(gmi));

	gmi.wProtocol	= g2e_getmeminfo_def;
	gmi.wServer		= 0;
	gmi.nTotalMem	= MI.nTotalMem;
	gmi.nFreeMem	= MI.nFreeMem;

	m_pSender->SendPackToServer( 
		(unsigned char*)&gmi, 
		sizeof(g2e_getmeminfo) );
}

void KSysPlugin::Process_GetDiskInfo(const void* pData, unsigned int nLen)
{
	if (nLen != sizeof(KProtocolHead))
		return;

	DISKINFO DI;
	m_DI.GetDISKInfo( DI );


	g2e_getdiskinfo	gdi;
	memset(&gdi, 0, sizeof(gdi));

	gdi.wProtocol	= g2e_getdiskinfo_def;
	gdi.wServer		= 0;
	gdi.nDiskCount	= DI.m_nMountLDCount;

	for( int nLoopCount = 0; nLoopCount < DI.m_nMountLDCount; nLoopCount++ )
	{
		int nSize = sizeof( gdi.Disk[nLoopCount].szDiskDesc );

		strncpy( 
			gdi.Disk[nLoopCount].szDiskDesc,
			DI.LogicDisk[nLoopCount].szDesc,
			nSize );
		gdi.Disk[nLoopCount].szDiskDesc[nSize-1] = 0;

		gdi.Disk[nLoopCount].nTotalSize = DI.LogicDisk[nLoopCount].nTotalSize;
		gdi.Disk[nLoopCount].nFreeSize	= DI.LogicDisk[nLoopCount].nFreeSize;
	}

	m_pSender->SendPackToServer( 
		(unsigned char*)&gdi, 
		sizeof(g2e_getdiskinfo) );
}

void KSysPlugin::Process_GetNetInfo(const void* pData, unsigned int nLen)
{
	if (nLen != sizeof(KProtocolHead))
		return;

	NETINFO	NI;
	m_NI.GetNETInfo( NI );

	g2e_getnetinfo	gni;
	memset(&gni, 0, sizeof(gni));
	gni.wProtocol	= g2e_getnetinfo_def;
	gni.wServer		= 0;
	gni.nCardCount	= (unsigned short)NI.nNETCardCount;

	strcpy( gni.szSystemName, NI.szSystemName );
	strcpy( gni.szHostName, NI.szHostName );

	for (unsigned long nLoopCount = 0; nLoopCount < NI.nNETCardCount; nLoopCount++)
	{
		int nSize = sizeof( gni.Card[nLoopCount].szCardDesc );
		strncpy(  
			gni.Card[nLoopCount].szCardDesc, 
			NI.Card[nLoopCount].szDesc, 
			nSize );
		gni.Card[nLoopCount].szCardDesc[nSize-1] = 0;

		nSize = sizeof( gni.Card[nLoopCount].szIP );
		strncpy(  
			gni.Card[nLoopCount].szIP, 
			NI.Card[nLoopCount].szIP, 
			nSize );
		gni.Card[nLoopCount].szIP[nSize-1] = 0;

		strncpy(  
			gni.Card[nLoopCount].szMask, 
			NI.Card[nLoopCount].szNetMask, 
			nSize );
		gni.Card[nLoopCount].szMask[nSize-1] = 0;

		strncpy(  
			gni.Card[nLoopCount].szMac, 
			NI.Card[nLoopCount].szMac, 
			nSize );
		gni.Card[nLoopCount].szMac[nSize-1] = 0;

	}

	m_pSender->SendPackToServer( 
		(unsigned char*)&gni, 
		sizeof(g2e_getnetinfo) );
}

void KSysPlugin::Process_GetNetLoad(const void* pData, unsigned int nLen)
{
	if (nLen != sizeof(KProtocolHead))
		return;

	g2e_getcardload gncl;
	memset(&gncl, 0, sizeof(gncl));

	NETINFO	NI;
	m_NI.GetNETInfo( NI );

	gncl.wProtocol	= g2e_getcardload_def;
	gncl.wServer	= 0;
	gncl.nCardCount	= (unsigned short)NI.nNETCardCount;

	for (unsigned long nLoopCount = 0; nLoopCount < NI.nNETCardCount; nLoopCount++)
	{
		gncl.Card[nLoopCount].nTXSize = NI.Card[nLoopCount].CRTXI.ulTXByte;
		gncl.Card[nLoopCount].nRXSize = NI.Card[nLoopCount].CRTXI.ulRXByte;
		gncl.Card[nLoopCount].nTXRate = NI.Card[nLoopCount].CRTXI.ulTXRate;
		gncl.Card[nLoopCount].nRXRate = NI.Card[nLoopCount].CRTXI.ulRXRate;
	}

	m_pSender->SendPackToServer( 
		(unsigned char*)&gncl, 
		sizeof(g2e_getcardload) );
}

void KSysPlugin::Process_GetProcInfo(const void* pData, unsigned int nLen)
{
	if (nLen != sizeof(KProtocolHead))
		return;

	g2e_getprocinfo	gpi;
	memset(&gpi, 0, sizeof(gpi));

	PROCINFO PI;
	m_PI.GetPROCInfo( PI );

	gpi.wProtocol	= g2e_getprocinfo_def;
	gpi.wServer		= 0;
	gpi.nProcCount	= PI.nProcessCount;

	for( int nLoopCount = 0; nLoopCount < PI.nProcessCount; nLoopCount++ )
	{
		gpi.Proc[nLoopCount].nCPUTime	=	PI.Process[nLoopCount].ulCPUTime;
		int nSize = sizeof( gpi.Proc[nLoopCount].szProcName );
		strncpy( 
			gpi.Proc[nLoopCount].szProcName	, 
			PI.Process[nLoopCount].szName,
			nSize );
		gpi.Proc[nLoopCount].szProcName[nSize-1] = 0;

		gpi.Proc[nLoopCount].nPID		=	PI.Process[nLoopCount].nPid;
		gpi.Proc[nLoopCount].nMemUse	=	PI.Process[nLoopCount].nMemUseSize;
		gpi.Proc[nLoopCount].nVMSize	=	PI.Process[nLoopCount].nVmSize;
		gpi.Proc[nLoopCount].nThreadCount = PI.Process[nLoopCount].nThreadCount;
	}

	m_pSender->SendPackToServer( 
		(unsigned char*)&gpi, 
		sizeof(g2e_getprocinfo) );
}

int KSysPlugin::ProcessNetMessage(unsigned char* pData, unsigned int nLen)
{
	BYTE byProtocol = *(BYTE*)pData;
	if (byProtocol >= fseye_protocol_count)
		return guard_err;
	fnProtocolProcess pfnProcess = m_aryProtoProcess[byProtocol];
	if (!pfnProcess)
		return guard_err;
	(this->*pfnProcess)(pData, nLen);
	return fseye_success;
}

int KSysPlugin::Disconnect( )
{
	return fseye_success;
}

int KSysPlugin::GetDescribe( 
			char* szDes,
			int nSize )
{
	strcpy( szDes, "Get System Information , cpu , ram etc.." );

	return fseye_success;
	
	//////////////////////////////////////
	CPUINFO CI;

	m_CI.GetCPUInfo( CI );

	printf( "*******************CPU******************\n" );
	printf( 
		"Logic Processor Count : %d \n"
		"Average Load : %d\n", 
		CI.nProcessorCount, CI.nTotalLoad );

	for (int i = 0; i < CI.nProcessorCount; ++i)
	{
		printf( 
			"Logic Processor %d : \n"
			"Name : %s\n"
			"Desc : %s\n"
			"Load : %d\n", 
			i, 
			CI.Processor[i].szName,
			CI.Processor[i].szDesc,
			CI.Processor[i].nLoad);
	}
	printf( "*******************CPU******************\n" );
	//////////////////////////////////////
	MEMINFO	MI;

	m_MI.GetMEMInfo( MI );

	printf( "*******************Mem******************\n" );
	printf( 
		"Memory Total : %d\n"
		"Memory Free : %d \n",
		MI.nTotalMem,
		MI.nFreeMem );
	printf( "*******************Mem******************\n" );
	
	//////////////////////////////////////

	//////////////////////////////////////
	DISKINFO DI;
	
	m_DI.GetDISKInfo( DI );
	
	printf( "*******************Disk******************\n" );
	printf( 
		"Logic Disk Count : %d \n",
		DI.m_nMountLDCount );
	
	for (int i = 0; i < DI.m_nMountLDCount; ++i)
	{
		printf( 
			"Logic Disk %s : \n"
			"Total : %u M\n"
			"Free : %u M\n", 
			DI.LogicDisk[i].szDesc,
			DI.LogicDisk[i].nTotalSize,
			DI.LogicDisk[i].nFreeSize);
	}
	printf( "*******************Disk******************\n" );
	//////////////////////////////////////
	NETINFO	NI;

	m_NI.GetNETInfo( NI );

	printf( "*******************Net******************\n" );
	printf(
		"System Name : %s \n"
		"Host Name : %s \n"
		"Net Card Count : %d \n",
		NI.szSystemName,
		NI.szHostName,
		NI.nNETCardCount );
	
	for (unsigned long i = 0; i < NI.nNETCardCount; ++i)
	{
		printf( 
			"Net Card %d : \n"
			"IP : %s\n"
			"Mac : %x-%x-%x-%x-%x-%x\n"
			"Mask : %s\n"
			"TXBytes : %u\n"
			"RXBytes : %u\n", 
			i,
			NI.Card[i].szIP,
			NI.Card[i].Mac[0],
			NI.Card[i].Mac[1],
			NI.Card[i].Mac[2],
			NI.Card[i].Mac[3],
			NI.Card[i].Mac[4],
			NI.Card[i].Mac[5],
			NI.Card[i].szNetMask,
			NI.Card[i].CRTXI.ulTXByte,
			NI.Card[i].CRTXI.ulRXByte);
	}
	printf( "*******************Net******************\n" );


	printf( "*******************Proc******************\n" );

	PROCINFO PI;

	m_PI.GetPROCInfo( PI );

	for (int i = 0; i < PI.nProcessCount; ++i)
		printf( "Process : %s PID : %d CPU Time : %d Thread Count : %d Phy Mem : %u Vir Mem : %u\n",
				PI.Process[i].szName,
				PI.Process[i].nPid,
				PI.Process[i].ulCPUTime,
				PI.Process[i].nThreadCount,
				PI.Process[i].nMemUseSize,
				PI.Process[i].nVmSize );

	printf( "*******************Proc******************\n" );

	while( true )
	{
		m_CI.GetCPUInfo( CI );
		printf( "CPU Load : %d\n", CI.nTotalLoad );

		for (int i = 0; i < PI.nProcessCount; ++i)
		{
			printf( "CPU %d : %d\n", i, CI.Processor[i].nLoad );
		}

#ifdef WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}

	//////////////////////////////////////
	return fseye_success;
}

int KSysPlugin::GetGUID( 
			char* szGUID,
			int nSize )
{
	strcpy( szGUID, "{BCCC1A2A-8709-4745-86CF-32A47B3098EE}" );
	return fseye_success;
}

int KSysPlugin::Breathe( )
{
	return fseye_success;
}

int KSysPlugin::GetVersion( 
	int& nMaxVer, 
	int& nMinVer )
{
	nMaxVer = SYSPLUGMAXVER;
	nMinVer = SYSPLUGMINVER;
	
	return fseye_success;
}

int KSysPlugin::GetAuthor(
	char* szAuthor,
	int nSize  )
{
	strcpy( szAuthor, "zuolizhi(zola)" );
	return fseye_success;
}

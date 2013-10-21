//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-03 11:00
//      File_base        : client
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "client.h"
#include "Engine.h"
#include "Engine/KSG_MD5_String.h"
#include "fsencrypt.h"
#include "kclient.h"
#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

#define MAX_DEBUFF_LEN				(1024*1024)
#define PYTHON_UPDATESERVER			"updateserver.py"		// 执行服务器更新的python脚本
#define PYTHON_DOWNLOADSERVER		"ftpdownload.py"		// 下载服务器更新包的python脚本
#define FILE_GUARDPID				"guard.pid"
#define	PORT_UPDATESERVER			(9990)					// guard和python脚本进行通信的UDP端口

unsigned char EncryptBuf[MAX_DEBUFF_LEN];

KGuardClient::KGuardClient() : m_bEncrypt(FALSE), m_bRunning(TRUE), m_nUpdateSock(0), m_bDaemon(FALSE), m_pLog(NULL)
{
	for (int i = 0; i < fseye_protocol_maxcount; ++i)
	{
		m_aryProtoProcess[i].pfnHandler = NULL;
		m_aryProtoProcess[i].bImportant = TRUE;
	}
	m_aryProtoProcess[e2g_DeliverKey_def	].pfnHandler = &KGuardClient::Process_DeliverKey;
	m_aryProtoProcess[e2g_ping_def			].pfnHandler = &KGuardClient::Process_Ping;
	m_aryProtoProcess[e2g_Authentication_def].pfnHandler = &KGuardClient::Process_Authentication;
	m_aryProtoProcess[e2g_switchmode_def	].pfnHandler = &KGuardClient::Process_SwitchMode;
	m_aryProtoProcess[e2g_config_def		].pfnHandler = &KGuardClient::Process_Config;
	m_aryProtoProcess[e2g_file_def			].pfnHandler = &KGuardClient::Process_File;
	m_aryProtoProcess[e2g_UpdateGuard_def	].pfnHandler = &KGuardClient::Process_Exit;
	m_aryProtoProcess[e2g_updateserver_def	].pfnHandler = &KGuardClient::Process_UpdateServer;
	m_aryProtoProcess[e2g_ftpdownload_def	].pfnHandler = &KGuardClient::Process_FtpDownload;

	m_aryProtoProcess[e2g_ping_def			].bImportant = FALSE;
	m_aryProtoProcess[e2g_file_def			].bImportant = FALSE;

	Clear();
}

KGuardClient::~KGuardClient( )
{
}

void KGuardClient::Clear( )
{
	m_pClient	= NULL;
	m_nClientID = INVALID_VALUE;
	
	m_nPort		= INVALID_VALUE;
	m_szIP[0]	= 0;
	
	m_ulLastRcvPingTime	= time(NULL);
	
	m_UDPSock = INVALID_VALUE;

	m_nEnableCfgMod = TRUE;
	m_bEncrypt = FALSE;

	m_szBindIp[0] = '\0';
	memset(m_szRC4Key, 0, sizeof(m_szRC4Key));

#ifndef WIN32
	m_nLockFile = -1;
#endif
}

extern KGuardClient g_cGuardClient;
void exithandle(int sig)
{
#ifndef WIN32
	wait(0);
	g_cGuardClient.CompleteExeCMD();
#endif
}

VOID KGuardClient::BecomeDaemon()
{
#ifdef __linux
	if (m_bDaemon)
	{
		pid_t pid = fork();	
		if (pid < 0)
			return;
		if (pid)
		{
			// parent process
			exit(0);
		}
		setsid();

		INT fd = open("/dev/null", O_RDWR);
		if (fd >= 0)
		{
			dup2(fd, 0);
			dup2(fd, 1);
			dup2(fd, 2);
			close(fd);
		}
	}
	signal(SIGCHLD, exithandle);
	signal(SIGPIPE, SIG_IGN);
#endif
}

BOOL KGuardClient::Initialize()
{
#ifndef WIN32
	// 设置创建的新文件权限为(rw.r.r)
	umask(022);
#endif

	// Open log file
	ASSERT(m_pLog == NULL);
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	CHAR szFile[MAX_PATH];
#ifdef WIN32
	mkdir("log");
#else
	mkdir("log", 0755);
	chmod("log", 0755);
#endif
	sprintf(szFile, "log/guard_%04d%02d%02d_%02d%02d%02d.log",
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec);
	m_pLog = fopen(szFile, "w");
	if (!m_pLog)
	{
		printf("Failed to open log[%s]!\n", szFile);
		return FALSE;
	}
	Log("Guard start ......");
	return TRUE;
}

VOID KGuardClient::UnInitialize()
{
	Log("Guard close ......");
	if (m_pLog)
	{
		fclose(m_pLog);
		m_pLog = NULL;
	}
}

void KGuardClient::Action( )
{
	CHAR szPath[MAX_PATH];
	getcwd(szPath, MAX_PATH);
	g_SetRootPath(szPath);
	LoadConfig( );
	GetServerNetCfg( m_ServerNetCfg );

	//
	//do not care result must be successful
	//
	m_FileTransfer.Init( this );
	m_PlugMan.Init(this, szPath);
	m_PlugMan.LoadPlug( );
	//
	// init net
	//
	if( InitNet( ) != fseye_success )
	{
		m_PlugMan.UnloadPlug( );
		return;
	}

	BecomeDaemon();

	//
	//work
	//

	MainLoop( );
	m_PlugMan.UnInit();
}

void KGuardClient::CalcSignMD5( int nX, char szMD5[64] )
{
	char szSign[MAX_DESC];
	int nY = ( A * nX * nX ) + ( B * nX ) + ( C );
	sprintf( szSign, "%d", nY );
	KG_EDStringToMD5String( szMD5, szSign );
}

int KGuardClient::LoadConfig( )
{
// 	KIniFile IniFile;
// 	if (IniFile.Load(CFGFILENAME))
// 	{
// 		IniFile.GetString( SERVERSEC, IPKEY, "", m_szIP, sizeof(m_szIP) );
// 		IniFile.GetInteger( SERVERSEC, PORTKEY, 0, &m_nPort );
// 		IniFile.GetString( SERVERSEC, LOGINPWD, "", m_szLoginPwd, sizeof(m_szLoginPwd) );
// 		IniFile.GetInteger( SERVERSEC,  ENABLECMOD, TRUE, &m_nEnableCfgMod );
// 		IniFile.GetString(SERVERSEC, "BindIp", "", m_szBindIp, sizeof(m_szBindIp));
// 		IniFile.GetInteger("System", "Daemon", TRUE, &m_bDaemon);
// 	}
	IIniFile* IniFile = NULL;
	IniFile = g_OpenIniFile(CFGFILENAME);
	if (IniFile)
	{
		IniFile->GetString(  SERVERSEC, IPKEY,      "",   m_szIP,       sizeof(m_szIP) );
		IniFile->GetInteger( SERVERSEC, PORTKEY,    0,    &m_nPort );
		IniFile->GetString(  SERVERSEC, LOGINPWD,   "",   m_szLoginPwd, sizeof(m_szLoginPwd) );
		IniFile->GetInteger( SERVERSEC, ENABLECMOD, TRUE, &m_nEnableCfgMod );
		IniFile->GetString(  SERVERSEC, "BindIp",   "",   m_szBindIp,   sizeof(m_szBindIp));
		IniFile->GetInteger( "System",  "Daemon",   TRUE, &m_bDaemon);	
	}

	return fseye_success;
}

int KGuardClient::InitNet( )
{
	if( m_pClient != NULL )
		return guard_err;

	//CreateClient( m_pClient, MAXCLIENT, NULL, MAXNETBUF, MAXNETBUF );
	if (!KClient::CreateClient(MAXCLIENT, &m_pClient))
		return guard_create_client_err;

	ASSERT(m_pClient != NULL);
	m_pClient->RegisterMsgFilter( 
		reinterpret_cast< void * >( this ), 
		ClientEventNotify );

	if( m_pClient->Startup( ) == INVALID_VALUE )
	{
		m_pClient->Cleanup( );
		m_pClient->Release( );
		m_pClient = NULL;
		return guard_startup_client_err;
	}

	if( m_nEnableCfgMod )
		InitUDPMod( );

	m_nUpdateSock = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef WIN32
	if (m_nUpdateSock == INVALID_SOCKET)
		return guard_err;
#else
	if (m_nUpdateSock == -1)
		return guard_err;
#endif

	sockaddr_in sAddress;
	memset(&sAddress, 0, sizeof(sAddress));
	sAddress.sin_family = AF_INET;
	sAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	sAddress.sin_port = htons(PORT_UPDATESERVER);
	bind(m_nUpdateSock, (sockaddr*)&sAddress, sizeof(sAddress));
	// 设置为非阻塞
#ifdef WIN32
	u_long nFlag = 1;
	ioctlsocket(m_nUpdateSock, FIONBIO, &nFlag);
#else
	int nFlag = fcntl(m_nUpdateSock, F_GETFL, 0);
	fcntl(m_nUpdateSock, F_SETFL, nFlag | O_NONBLOCK);
#endif

	return fseye_success;
}

void KGuardClient::UninitNet( )
{
	if( m_pClient != NULL )
	{
		m_pClient->Disconnect( m_nClientID );
		m_pClient->Cleanup( );
		m_pClient->Release( );
		m_pClient = NULL;
	}

	if( m_nEnableCfgMod )
		UninitUDPMod( );
#ifdef WIN32
	closesocket(m_nUpdateSock);
#else
	close(m_nUpdateSock);
#endif
	m_nUpdateSock = 0;
}

int KGuardClient::InitUDPMod( )
{
	if( m_UDPSock != INVALID_VALUE )
		return guard_err;

	m_UDPSock = socket( AF_INET, SOCK_DGRAM, 0 );
	
	if( m_UDPSock == INVALID_VALUE )
		return guard_startup_client_err;

	unsigned long ul;
#ifdef WIN32
	ioctlsocket( m_UDPSock, FIONBIO, &ul );
#else
	int flags;
	flags = fcntl(m_UDPSock, F_GETFL, 0);
	fcntl(m_UDPSock, F_SETFL, flags | O_NONBLOCK | O_ASYNC );
#endif
	
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons( G_UDP_PORT );
	addr.sin_addr.s_addr = INADDR_ANY;

	if( bind( m_UDPSock , (const sockaddr *)&addr, sizeof( sockaddr_in ) ) == INVALID_VALUE ) 
	{
		CloseSock( m_UDPSock );
		return guard_startup_client_err;
	}
	
	return fseye_success;
}

void KGuardClient::UninitUDPMod( )
{
	if( m_UDPSock != INVALID_VALUE )
	{
		CloseSock( m_UDPSock );
		m_UDPSock = INVALID_VALUE;
	}
}

int KGuardClient::ConnectTo( )
{
	#define MAXTIME 10

	static bool bNotify = false;
	static unsigned long ulLastReconnectTime = 0;

	if( m_nClientID == INVALID_VALUE )
	{
		unsigned long ulTime = time( NULL );

		if( !bNotify )
		{
			NotifyDisconnect( );
			bNotify = true;
		}
		
		if (ulTime - ulLastReconnectTime > MAXTIME && m_szIP[0] != '\0')
		{
			// 绑定IP
			if (m_szBindIp[0] != '\0')
			{
				m_pClient->Bind(m_szBindIp);
				Log("Bind to ip[%s]!", m_szBindIp);
			}
			else
			{
				Log("Bind none ip!");
			}

			m_nClientID = m_pClient->ConnectTo( m_szIP, m_nPort );
			ulLastReconnectTime = ulTime;

			if( m_nClientID !=  INVALID_VALUE )
			{
				bNotify = false;
				m_ulLastRcvPingTime	= ulTime;
			}
		}
	}

	return fseye_success;
}

void KGuardClient::Ping( )
{
	static unsigned long ulSendTime = 0;
	time_t now = time(NULL);
	if(now - ulSendTime > MAX_PING_TIME )
	{
		KProtocolHead sPing;
		sPing.wProtocol = g2e_ping_def;
		sPing.wServer = 0;

		printf("ping server!\n");
		SendPackToServer(
			(unsigned char*)&sPing, 
			sizeof(sPing) );

		ulSendTime = now;
	}

	if( m_nClientID !=  INVALID_VALUE && 
		now - m_ulLastRcvPingTime > MAX_ALIVE_TMIE )
	{
		printf("Ping timeout, disconnect connection[%u]", m_nClientID);
		m_pClient->Disconnect( m_nClientID );
		m_nClientID = INVALID_VALUE;
	}
}

void KGuardClient::SendSign( int nX )
{
	g2e_Authentication auth;
	memset(&auth, 0, sizeof(auth));
	auth.wProtocol = g2e_Authentication_def;
	auth.wServer = 0;
	CalcSignMD5( nX, auth.Y );

	SendPackToServer( 
		(unsigned char*)&auth, 
		sizeof(auth) );

}

int KGuardClient::ProcessUDPCmd( )
{
	if( m_UDPSock != INVALID_VALUE )
	{
		sockaddr_in addr;
		char szBuf[MAX_BUFFSIZE];
		
		int nRecvLen = 0;
		#ifdef WIN32
		int nFromLen = sizeof(addr);
		#else
		socklen_t nFromLen = sizeof(addr);
		#endif
		//sockaddr_in addr = {0};
		nRecvLen = 
		recvfrom( 
			m_UDPSock, 
			szBuf, 
			sizeof(szBuf), 
			0, 
			(sockaddr*)&addr,
			&nFromLen );
		
		if( nRecvLen == sizeof(e2g_config) )
		{
			e2g_config* config = (e2g_config*)szBuf;

			//-----------------------------------------------------
			/*
			int nRawSize;
#ifdef WIN32
			nRawSize = RC4_KEY_LEN;
#else
			RSADencrypt( 
				config->Key.Data, 
				sizeof(config->Key.Data), 
				&nRawSize );
#endif
			if( nRawSize == RC4_KEY_LEN )
			*/
			{

#ifndef  WIN32
				/*
				RC4Encrypt( 
					config->ConfigInfo, 
					sizeof(config->ConfigInfo), 
					config->Key.Data );
				*/
#endif

				e2g_ConfigInfo* config_info = (e2g_ConfigInfo*)config->ConfigInfo;
				//----------------------------------------

				char szYMD5[64];
				CalcSignMD5( config_info->X, szYMD5 );
				
				if( !strcmp( szYMD5, config_info->Y ) )
				{
					{{
						std::fstream configfile( 
							CFGFILENAME, 
							std::ios::out );
						
						configfile.write( 
							(const char *)config_info->ConfigFileData, 
							config_info->ConfigFileDataLength );
						configfile.close();
					}}
					
					LoadConfig( );
					
					if( m_nClientID !=  INVALID_VALUE )
					{
						Log("Reconfig, disconnect with connection[%u]", m_nClientID);
						m_pClient->Disconnect( m_nClientID );
						m_nClientID = INVALID_VALUE;
					}
					
					m_PlugMan.UnloadPlug( );
					m_PlugMan.LoadPlug( );
				}

				//------------------------------------------
			}

			//-----------------------------------------------------
		}
	}

	return fseye_success;
}

void KGuardClient::Process_DeliverKey(LPCVOID pData, UINT nLen)
{
	if (nLen != sizeof(e2g_DeliverKey))
		return;

	e2g_DeliverKey* pDK = (e2g_DeliverKey*)pData;
#ifndef  WIN32
	/*
	int nRawSize;
	RSADencrypt( 
		pDK->Key.Data, 
		sizeof(pDK->Key.Data), 
		&nRawSize );
	if( nRawSize == RC4_KEY_LEN )
		memcpy( m_szRC4Key, pDK->Key.Data, RC4_KEY_LEN );
	m_bEncrypt = TRUE;
	*/
#endif
	return;
}

void KGuardClient::Process_Ping(LPCVOID pData, UINT nLen)
{
	m_ulLastRcvPingTime = time(NULL);
}

void KGuardClient::Process_Authentication(LPCVOID pData, UINT nLen)
{
	if (nLen == sizeof(e2g_Authentication))
	{
		e2g_Authentication* pAuth = (e2g_Authentication*)pData;
		SendSign(pAuth->X);
	}
}

void KGuardClient::Process_SwitchMode(LPCVOID pData, UINT nLen)
{
	if (nLen == sizeof(e2g_switchmode))
	{
		e2g_switchmode* switchmode = (e2g_switchmode*)pData;
		m_nEnableCfgMod = switchmode->Mode;

		IIniFile* IniFile = NULL;
		IniFile = g_OpenIniFile(CFGFILENAME);
		if (IniFile)
		{
			IniFile->WriteInteger(SERVERSEC, ENABLECMOD, m_nEnableCfgMod);
			IniFile->Save(CFGFILENAME);
		}

		if (switchmode->Mode)
			InitUDPMod();
		else
			UninitUDPMod();

		KProtocolHead retswitch;
		retswitch.wProtocol = g2e_switchmode_def;
		retswitch.wServer = switchmode->wServer;
		SendPackToServer( (unsigned char*)&retswitch, sizeof(retswitch));
	}
}

void KGuardClient::Process_Config(LPCVOID pData, UINT nLen)
{
	if (nLen == sizeof(e2g_config))
	{
		e2g_config* config = (e2g_config*)pData;
		e2g_ConfigInfo* configinfo = (e2g_ConfigInfo*)config->ConfigInfo;

		std::fstream configfile( 
			CFGFILENAME, 
			std::ios::out );

		configfile.write( 
			(const char *)configinfo->ConfigFileData,
			configinfo->ConfigFileDataLength );

		IIniFile* IniFile = NULL;
		IniFile = g_OpenIniFile(CFGFILENAME);
		if (IniFile)
		{
			IniFile->WriteInteger( SERVERSEC, ENABLECMOD, m_nEnableCfgMod );
			IniFile->Save( CFGFILENAME );
		}

#ifndef WIN32
		chmod(CFGFILENAME, S_IRUSR | S_IWUSR);
#endif

		KProtocolHead retconfig;
		retconfig.wProtocol = g2e_config_def;
		retconfig.wServer = 0;
		SendPackToServer( (unsigned char*)&retconfig, sizeof(retconfig));
	}
}

void KGuardClient::Process_File(LPCVOID pData, UINT nLen)
{
	if (nLen == sizeof(e2g_file))
	{
		TransferFile((e2g_file*)pData);
	}
}

void KGuardClient::Process_Exit(LPCVOID pData, UINT nLen)
{
	m_bRunning = FALSE;
}

void KGuardClient::Process_FtpDownload(LPCVOID pData, UINT nLen)
{
	do 
	{
		if (nLen != sizeof(e2g_ftpdownload))
		{
			ASSERT(FALSE);
			break;
		}
		e2g_ftpdownload* pMsg = (e2g_ftpdownload*)pData;
		pMsg->szFtpHost[sizeof(pMsg->szFtpHost) - 1] = '\0';
		pMsg->szFtpPath[sizeof(pMsg->szFtpPath) - 1] = '\0';
		pMsg->szFtpUser[sizeof(pMsg->szFtpUser) - 1] = '\0';
		pMsg->szFtpPass[sizeof(pMsg->szFtpPass) - 1] = '\0';

		CHAR szCmd[4096];
		sprintf(szCmd, "python %s %d %s %s %s %s",
			PYTHON_DOWNLOADSERVER,
			PORT_UPDATESERVER,
			pMsg->szFtpHost,
			pMsg->szFtpPath,
			pMsg->szFtpUser,
			pMsg->szFtpPass);
#ifdef WIN32
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		memset(&pi, 0, sizeof(pi));
		if (!CreateProcess( 
			NULL,
			szCmd,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&si,
			&pi))
		{
			CHAR szError[MAX_PATH];
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				szError,
				sizeof(szError) - 1,
				NULL);
			szError[sizeof(szError) - 1] = '\0';
			Log("FtpDownload: Failed to create process[%s]: %s", szCmd, szError);
		}
		else
		{
			Log("FtpDownload: Create process[%s]", szCmd);
		}
#else
		if (fork() == 0)
		{
			CHAR szPort[32];
			sprintf(szPort, "%d", PORT_UPDATESERVER);
			if (execlp("python",
				"python",
				PYTHON_DOWNLOADSERVER,
				szPort,
				pMsg->szFtpHost,
				pMsg->szFtpPath,
				pMsg->szFtpUser,
				pMsg->szFtpPass,
				NULL) == -1)
			{
				Log("FtpDownload: Failed to create process[%s]: %s",
					szCmd,
					strerror(errno));
				exit(0);
			}
		}
		Log("FtpDownload: Create process[%s]", szCmd);
#endif
	}
	while(0);
}

void KGuardClient::Process_UpdateServer(LPCVOID pData, UINT nLen)
{
	do 
	{
		e2g_updateserver* pCmd = (e2g_updateserver*)pData;
		if (nLen != sizeof(e2g_updateserver))
		{
			ASSERT(FALSE);
			break;
		}
		pCmd->szFile[sizeof(pCmd->szFile) - 1] = '\0';
		pCmd->szCode[sizeof(pCmd->szCode) - 1] = '\0';

		CHAR szCmd[4096];
		sprintf(szCmd, "python %s %d %s %s %u",
			PYTHON_UPDATESERVER,
			PORT_UPDATESERVER,
			pCmd->szFile,
			pCmd->szCode,
			pCmd->byMode);
#ifdef WIN32
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		memset(&pi, 0, sizeof(pi));
		if (!CreateProcess( 
			NULL,
			szCmd,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&si,
			&pi))
		{
			CHAR szError[MAX_PATH];
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				szError,
				sizeof(szError) - 1,
				NULL);
			szError[sizeof(szError) - 1] = '\0';
			Log("UpdateServer: Failed to create process[%s]: %s", szCmd, szError);
		}
		else
		{
			Log("Create process[%s]: %s", szCmd);
		}
#else
		if (fork() == 0)
		{
			CHAR szPort[32];
			CHAR szMode[32];
			sprintf(szPort, "%d", PORT_UPDATESERVER);
			sprintf(szMode, "%u", pCmd->byMode);
			if (execlp(
				"python",
				"python",
				PYTHON_UPDATESERVER,
				szPort,
				pCmd->szFile,
				pCmd->szCode,
				szMode,
				NULL) == -1)
			{
				Log("UpdateServer: Failed to create process[%s]: %s",
					szCmd,
					strerror(errno));
				exit(0);
			}
			Log("UpdateServer: Create process[%s]", szCmd);
		}
#endif
	}
	while(0);
}

int KGuardClient::ProcessMessages( )
{
	if( m_nClientID == INVALID_VALUE )
		return fseye_success;

	unsigned int nLen = 0;
	const void *pData = m_pClient->GetPackFromServer(m_nClientID, nLen);
	if (pData != NULL && nLen != 0)
	{
		BYTE byProtocol = *(BYTE*)pData;
		if (!m_bDaemon)
		{
			printf("Recv %d %d\n", PH(pData), nLen);
		}
		if (byProtocol >= fseye_protocol_maxcount)
		{
			Log("Unknown protocol[%u:%u]!", byProtocol, nLen);
			return fseye_success;
		}
#ifndef  WIN32
		/*
		if (m_bEncrypt)
		{
			RC4Encrypt( 
				(unsigned char*)pData, 
				nLen, 
				(unsigned char*)m_szRC4Key );
		}
		*/
#endif
		KPROCESS sProcess = m_aryProtoProcess[byProtocol];
		if (sProcess.pfnHandler)
		{
			if (sProcess.bImportant)
				Log("Guard process protocol[%u:%d]", byProtocol, nLen);
			(this->*sProcess.pfnHandler)(pData, nLen);
		}
		else if (m_FileTransfer.ProcessNetMessage((unsigned char*)pData, nLen) == fseye_success)
		{
			return fseye_success;
		}
		else
		{
			m_PlugMan.ProcessNetMessage((unsigned char*)pData, nLen);
		}
	}

	m_FileTransfer.checkcmdres();

	return fseye_success;
}

int KGuardClient::MainLoop()
{
	while (m_bRunning)
	{

		if( m_nEnableCfgMod )
			ProcessUDPCmd();

		ConnectTo(); 
		ProcessMessages();
		m_PlugMan.Breathe();
		Ping();
		Test();
		ProcessPythonMessages();
#ifdef WIN32
		Sleep(1);
#else
		usleep(1);
#endif
	}

	return fseye_success;
}



int KGuardClient::SendPackToServer( 
		unsigned char* pData, 
		unsigned int datasize  )
{
	if( m_nClientID == INVALID_VALUE )
		return guard_client_invalidhadle;
	else
	{
		if( datasize > MAX_DEBUFF_LEN )
			return guard_client_send_err;

		memcpy( EncryptBuf, pData, datasize );
#ifndef WIN32
		/*
		RC4Encrypt( 
			EncryptBuf, 
			datasize, 
			(unsigned char*)m_szRC4Key );
		*/
#endif

		if( m_pClient->SendPackToServer( 
			m_nClientID, EncryptBuf, datasize ) != fseye_success )
			return guard_client_send_err;
	}

	return fseye_success;
}

/*
KGuardClient& KGuardClient::Singleton( )
{
	static KGuardClient GC;

	return GC;
}
*/

void KGuardClient::ClientEventNotify(
		void* lpParam,
		const unsigned long ulnID,
		const unsigned long ulnEventType )
{
	switch( ulnEventType ) 
	{
	case enumClientConnectCreate:
		{
		}
		break;
	case enumClientConnectClose:
		{
			KGuardClient* pGC = (KGuardClient*)lpParam;
			pGC->Disconnect( );
		}
		break;
	};
}

void KGuardClient::Disconnect( )
{
	m_nClientID = INVALID_VALUE;
}

void KGuardClient::NotifyDisconnect( )
{
	m_FileTransfer.Disconnect( );

	m_PlugMan.Disconnect( );
}

BOOL KGuardClient::MakeDirectory(LPCSTR pszPath)
{
	ASSERT(pszPath);
#ifdef WIN32
	if (!CreateDirectory(pszPath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return FALSE;
#else
	if (mkdir(pszPath, 0755) == -1 && errno != EEXIST)
		return FALSE;
	chmod(pszPath, 0755);
#endif
	return TRUE;
}

BOOL KGuardClient::MakePath(LPCSTR pszPath)
{
	ASSERT(pszPath);
	char szPath[4096];
	char* pszDes = szPath;
	const char* pszSrc = pszPath;
	const char* pszEnd = &szPath[sizeof(szPath) - 1];
	while (*pszSrc != '\0' && pszDes < pszEnd)
	{
		char ch = *pszSrc;
#ifdef WIN32
		if ((ch != '\\' && ch != '/') ||
			(pszSrc > pszPath && *(pszSrc - 1) == ':') ||
			(pszSrc == pszPath))
#else
		if (ch != '/' || pszSrc == pszPath)
#endif
		{
			*pszDes++ = ch;
			++pszSrc;
		}
		else
		{
			*pszDes = '\0';
			if (!MakeDirectory(szPath))
				return FALSE;
			*pszDes++ = *pszSrc++;
		}
	}
	*pszDes = '\0';
	if (!MakeDirectory(szPath))
		return FALSE;
	return TRUE;
}

BOOL KGuardClient::MakeFilePath(LPCSTR pszFile)
{
	ASSERT(pszFile);
	INT nLastSepPos = 0;
	INT nIndex = 0;
	CHAR ch = *pszFile;
	CHAR szPath[1024];
	while (ch != '\0' && nIndex < sizeof(szPath))
	{
		szPath[nIndex] = ch;
		if (nIndex > 0 &&
#ifdef WIN32
			pszFile[nIndex - 1] != ':' &&
			(ch == '\\' || ch == '/'))
#else
			ch == '/')
#endif
		{
			nLastSepPos = nIndex;
		}
		ch = pszFile[++nIndex];
	}
	szPath[nLastSepPos] = '\0';
	return szPath[0] == '\0' || MakePath(szPath);
}

void KGuardClient::TransferFile(e2g_file* pMsg)
{
	ASSERT(pMsg && pMsg->wFileLen > 0);
	pMsg->szPath[sizeof(pMsg->szPath) - 1] = '\0';
	if (MakeFilePath(pMsg->szPath))
	{
		FILE* pFile = fopen(pMsg->szPath, "w+");
		if (pFile)
		{
			fwrite(pMsg->szData, 1, pMsg->wFileLen, pFile);
			fclose(pFile);
			pFile = NULL;

#ifndef WIN32
			chmod(pMsg->szPath, S_IRUSR | S_IWUSR);
#endif

			Log("Transfer file[%s]", pMsg->szPath);
		}
		else
		{
			CHAR szError[MAX_PATH];
#ifdef WIN32
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				szError,
				sizeof(szError) - 1,
				NULL);
#else
			strncpy(szError, strerror(errno), sizeof(szError) - 1);
#endif
			szError[sizeof(szError) - 1] = '\0';
			Log("Failed to create file[%s]: %s", pMsg->szPath, szError);
		}
	}
}

void KGuardClient::ProcessPythonMessages()
{
	sockaddr_in sAddress;
	CHAR szBuff[8192];
#ifdef WIN32
	int nAddrLen = sizeof(sAddress);
	int nLen = 0;
#else
	socklen_t nAddrLen = sizeof(sAddress);
	ssize_t nLen = 0;
#endif
	do 
	{
		nLen = ::recvfrom(m_nUpdateSock,
			szBuff,
			sizeof(szBuff),
			0,
			(sockaddr*)&sAddress,
			&nAddrLen);
		if (nLen > 0)
		{
			if (szBuff[0] == p2g_log_def)
			{
				p2g_log* pLog = (p2g_log*)szBuff;
				if (nLen > sizeof(p2g_log) &&
					nLen == sizeof(p2g_log) + pLog->wLen)
				{
					LPSTR pszLog = (LPSTR)(pLog + 1);
					pszLog[pLog->wLen - 1] = '\0';
					Log("Python script log: %s", pszLog);
				}
				else
				{
					Log("Invalid log protocol[%u:%d] from python script", szBuff[0], nLen);
				}
			}
			else
			{
				SendPackToServer((UCHAR*)szBuff, nLen);
			}
		}
	}
	while (nLen > 0);
}

VOID KGuardClient::LogV(LPCSTR pszFormat, va_list vl)
{
	ASSERT(pszFormat);
	if (m_pLog)
	{
		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		fprintf(m_pLog,
			"[%04d-%02d-%02d %02d:%02d:%02d]",
			t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);
		vfprintf(m_pLog, pszFormat, vl);
		fprintf(m_pLog, "\r\n");
		vprintf(pszFormat, vl);
		printf("\n");
		fflush(m_pLog);
	}
}

VOID KGuardClient::Log(LPCSTR pszFormat, ...)
{
	ASSERT(pszFormat);
	if (m_pLog)
	{
		va_list vl;
		va_start(vl, pszFormat);
		LogV(pszFormat, vl);
		va_end(vl);
	}
}

void KGuardClient::Test()
{
	/*
	e2g_file sMsg;
	sMsg.sHead.wProtocol = 0;
	sMsg.sHead.wServer = 0;
	strcpy((CHAR*)sMsg.szPath, "cav.ini");
	strcpy((CHAR*)sMsg.szData, "This is a test!");
	sMsg.wFileLen = strlen((CHAR*)sMsg.szData);
	TransferFile(&sMsg);
	*/
}

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-03 11:00
//      File_base        : client
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _GUARD_CLIENT_H_
#define _GUARD_CLIENT_H_

#include "networkinterface.h"
#include "def.h"
#include "plugin.h"
#include "filedl.h"
#include "getnccfg.h"
#include "IEncrypt.h"

#define CFGFILENAME	"guard.cfg"
#define SERVERSEC	"FSEye"
#define IPKEY		"IP"
#define PORTKEY		"Port"
#define LOGINPWD	"LoginPwd"
#define ENABLECMOD	"EABCM"

#define A			(101)
#define B			(43)
#define C			(72)
#define RC4_KEY_LEN	(8)
#define MAX_RSA_LEN (1024)
#define G_UDP_PORT	(9999)

class KGuardClient : public ISender
{
public:

	KGuardClient( );
	~KGuardClient( );
	BOOL Initialize();
	VOID UnInitialize();
	void Action( );

	void Disconnect( );

	int SendPackToServer( 
		unsigned char* pData, 
		unsigned int datasize  );

	void CompleteExeCMD( ){ m_FileTransfer.CompleteExeCMD( ); }
	
	//static KGuardClient& Singleton( );
	typedef void (KGuardClient::*fnProtocolProcess)(const void *, unsigned int);
	VOID Log(LPCSTR pszFormat, ...);
	VOID LogV(LPCSTR pszFormat, va_list vl);
private:
	static void ClientEventNotify(
		void* lpParam,
		const unsigned long ulnID,
		const unsigned long ulnEventType );

	VOID BecomeDaemon();
	int InitNet( );
	void UninitNet( );
	int ConnectTo( );
	int MainLoop( );
	int ProcessMessages( );
	int ProcessUDPCmd( );
	void NotifyDisconnect( );
	int LoadConfig( );
	int InitUDPMod( );
	void UninitUDPMod( );
	void Clear( );
	void CloseSock( int& nSock )
	{
		#ifdef WIN32
		closesocket( nSock );
		#else
		close( nSock );
		#endif
		nSock = INVALID_VALUE;
	}
	void Ping( );
	void SendSign( int nX );
	void CalcSignMD5( int nX, char szMD5[64] );
	void TransferFile(e2g_file* pMsg);
	BOOL MakeDirectory(LPCSTR pszPath);
	BOOL MakePath(LPCSTR pszPath);
	BOOL MakeFilePath(LPCSTR pszFile);
	void ProcessPythonMessages();
	void Test();
	void Process_DeliverKey		(LPCVOID pData, UINT nLen);
	void Process_Ping			(LPCVOID pData, UINT nLen);
	void Process_Authentication	(LPCVOID pData, UINT nLen);
	void Process_SwitchMode		(LPCVOID pData, UINT nLen);
	void Process_Config			(LPCVOID pData, UINT nLen);
	void Process_File			(LPCVOID pData, UINT nLen);
	void Process_Exit			(LPCVOID pData, UINT nLen);
	void Process_UpdateServer	(LPCVOID pData, UINT nLen);
	void Process_FtpDownload	(LPCVOID pData, UINT nLen);
private:
	struct KPROCESS 
	{
		fnProtocolProcess	pfnHandler;
		BOOL				bImportant;	// 是否重要协议
	};
	IClient*			m_pClient;
	int					m_nClientID;
	int					m_UDPSock;
	int					m_nPort;
	char				m_szIP[MAX_NAME];
	char				m_szLoginPwd[MAX_NAME*4];
	char				m_szRC4Key[RC4_KEY_LEN];
	char				m_szBindIp[MAX_NAME];		// 绑定IP
	int					m_nEnableCfgMod;
	BOOL				m_bDaemon;					// 是否以守护进程模式运行
	_FileTrans			m_FileTransfer;
	PlugInMan			m_PlugMan;
	SERVERNETCFG		m_ServerNetCfg;
	
	unsigned long		m_ulLastRcvPingTime;
	BOOL				m_bEncrypt;
	KPROCESS			m_aryProtoProcess[fseye_protocol_maxcount];
	BOOL				m_bRunning;
#ifdef WIN32
	SOCKET				m_nUpdateSock;
#else
	int					m_nUpdateSock;
	int					m_nLockFile;
#endif
	FILE*				m_pLog;
};

#endif

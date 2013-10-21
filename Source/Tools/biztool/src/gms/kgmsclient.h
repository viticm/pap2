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
#ifndef _GMS_CLIENT_H_
#define _GMS_CLIENT_H_

#include "networkinterface.h"
#include "def.h"
#include "getnccfg.h"
#include "IEncrypt.h"
#include "GameMasterServiceDef.h"
#include "fseye_protocol.h"
#include <map>
#include <string>

#define CFGFILENAME	"guard.cfg"
#define SCRIPTFILENAME "j3gmScript.txt"
#define PARAMETERREGULARFILENAME "j3ParamRegular.txt"
#define SERVER_GMC	"GMC"
#define IPKEY		"IP"
#define PORTKEY		"Port"
#define LOGINPWD	"Password"
#define LOGINACC	"Account"
#define AREAINDEX	"AreaIndex"
#define	GROUPINDEX	"GroupIndex"
#define GC_SERVERID	"GCServer"
#define ENABLECMOD	"EABCM"

#define A			(101)
#define B			(43)
#define C			(72)
#define RC4_KEY_LEN	(8)
#define MAX_RSA_LEN (1024)
#define G_UDP_PORT	(9999)
#define MAX_FILE_LEN (102400)

class KGMSPlug;
class KGMSClient : public ISender
{
public:

	KGMSClient( );
	~KGMSClient( );

	void Disconnect( );

	int SendPackToServer( 
		unsigned char* pData, 
		unsigned int datasize  );

	static KGMSClient& Singleton( );
	typedef void (KGMSClient::*fnProtocolProcess)(const void *, unsigned int);
	int ConnectTo( );
	int MainLoop( );
private:
	void SendLoginMsg(const char * szAccountName, const char * szPassword, int nAreaIndex, int nGroupIndex);
	VOID ExecuteCmd(LPCSTR pszCmd, LPSTR pszData);
	VOID ExecuteCmdOnRole(LPCSTR pszRole, LPCSTR pszCmd, LPCSTR pszData, GAME_MASTER_PROTOCOL_INFO* pSession);
	VOID ExecuteCmdOnAccount(LPCSTR pszAccount, LPCSTR pszCmd, LPCSTR pszData, GAME_MASTER_PROTOCOL_INFO* pSession);
	void ProcessMessasge(CONST VOID* pData, UINT nLen);
public:
	static void ClientEventNotify(
		void* lpParam,
		const unsigned long ulnID,
		const unsigned long ulnEventType );

	int InitNet( );
	void UninitNet( );
	void PostRoleList(VOID* pData, UINT nLen);
	void ProecssGmResult(CONST VOID* pData, UINT nLen);	
	void PostGmChannelMessage(VOID* pData, UINT nLen);
	int ProcessMessages( );
	void NotifyDisconnect( );
	int LoadConfig(IGuard* pGuard);
	bool LoadGMScriptTable();
	bool LoadParamRegular();
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
	void Ping(time_t tNow);
	void SendSign( int nX );
	void CalcSignMD5( int nX, char szMD5[64] );
private:
	IClient*			m_pClient;
	int					m_nClientID;
	int					m_UDPSock;
	int					m_nPort;
	int					m_nAreaIndex;
	int					m_nGroupIndex;
	char				m_szIP[MAX_NAME];
	char				m_szAccountName[MAX_NAME];
	char				m_szLoginPwd[MAX_NAME*4];
	char				m_szRC4Key[RC4_KEY_LEN];
	int					m_nEnableCfgMod;
	char				m_szBindIp[MAX_NAME];
	int					m_nGCServerId;
	unsigned long		m_ulLastRcvPingTime;
	BOOL				m_bEncrypt;
	fnProtocolProcess	m_aryProtoProcess[fseye_protocol_maxcount];
	BOOL				m_bRunning;
	KGMSPlug			*m_pGuardPlugin;
	INT					m_nLastSession;
	INT					m_nLastSerialNo;
	LPCSTR				m_aryResult[emKGMCMD_RESULT_COUNT];
	IGuard*				m_pGuard;
	char				m_szGMScriptFileContent[MAX_FILE_LEN];//gm脚本文件内容
	char				m_szParamRegularFileContent[MAX_FILE_LEN];//参数规则配置文件内容

	typedef BOOL (KGMSClient::*fnProcessRole)(LPCSTR, LPCSTR, LPCSTR);
	typedef BOOL (KGMSClient::*fnProcessAccount)(LPCSTR, LPCSTR, LPCSTR);
	typedef BOOL (KGMSClient::*fnProcess)(LPCSTR, LPCSTR,LPCSTR);

	std::map<std::string, fnProcess>			m_mapProcess;
	std::map<std::string, fnProcessRole>		m_mapRoleProcess;
	std::map<int, int>	m_mapSessionEYE;

	std::map<std::string, fnProcessAccount>		m_mapAccountProcess;
	std::map<int, GAME_MASTER_PROTOCOL_INFO>	m_mapSession;
private:
	VOID SplitString(LPSTR pszCmd, LPSTR* ppCmdList, INT nMaxCount, INT& nRetCount, CHAR chSep = ' ');
	VOID Process_Chat(LPSTR pszCmd);
	BOOL Kick(LPCSTR pszName, LPCSTR pszData, BOOL bIsRoleName, LPCSTR pszCmd);

	BOOL ProcessAccount_Kick(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);
	BOOL ProcessAccount_Chat(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);
	BOOL ProcessAccount_RoleList(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);

	BOOL ProcessRole_Kick(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);
	BOOL ProcessRole_Chat(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);
	BOOL ProcessRole_Fly (LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);
	BOOL ProcessRole_Mail(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);
	BOOL ProcessZone_Announce(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);
	BOOL ProcessZone_Message(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd);
	// 转义字符串，把"替换成\"
	VOID Translate(LPCSTR pszMsg, LPSTR pszBuf, INT nBufLen);

	//add by SunYong
	BOOL GenerateGMScript(LPSTR pszRtScript, LPCSTR pszCmd, LPSTR* ppszParameterlist);
	BOOL GenerateGMParams(LPCSTR pszCmd, LPCSTR pszData, LPSTR* ppszParameterlist, CONST INT nYetHaveCnt);
	BOOL IsBalank(char cTest);
	BOOL IsParamEnd(char cPost, char cTest);
	BOOL GetOneKeyWord(LPCSTR pcWalker, int& iWalker, LPSTR pszKeyWord, CONST int nLength);
	INT  GetParamCnt_OneKeyWord(LPCSTR pcWalker, int iWalker, CONST int nLen);
	BOOL GetPrePost_fix(char& cPre, char& cPost, LPCSTR pcWalker, int& iWalker, CONST int nLength);
	BOOL ProcessGmcmd_gs(LPCSTR pszData, LPCSTR pszCmd, LPCSTR pszName);
	BOOL ProcessGmcmd_gc(LPCSTR pszData, LPCSTR pszCmd, LPCSTR pszAccount = "");
	BOOL RegulateOperationKeyWord(LPCSTR pszData, LPCSTR pszOldCmd, BYTE byProtocol, LPSTR pszNewCmd);
	
public:
	void SetOwner(KGMSPlug * pGuardPlugin)
	{
		m_pGuardPlugin = pGuardPlugin;
	}
	BOOL SendGMExcuteCmd(e2l_ExeGMCmd * pProtocl);
};

#endif

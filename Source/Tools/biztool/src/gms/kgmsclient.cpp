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
#include "kclient.h"
#include "kgmsclient.h"
#include "Engine.h"
#include "Engine/KSG_MD5_String.h"
#include "fsencrypt.h"
#include "GameMasterProtocol.h"
#include "fseye_protocol.h"
#include "gmsplug.h"
#include "Engine/FileType.h"

#define MAX_DEBUFF_LEN	(1024*1024)
#define GMMAIL_CAPTION	"客服系统"

unsigned char EncryptBuf[MAX_DEBUFF_LEN];

KGMSClient::KGMSClient() : m_bEncrypt(FALSE), m_bRunning(TRUE), m_nLastSession(0), m_nLastSerialNo(0), m_pGuard(NULL)
{
	m_mapRoleProcess["KICKROLE"		] = &KGMSClient::ProcessGmcmd_gs;
	m_mapRoleProcess["ROLECHAT"		] = &KGMSClient::ProcessGmcmd_gs;
	m_mapRoleProcess["FLY2TARGET"	] = &KGMSClient::ProcessGmcmd_gs;
	m_mapRoleProcess["FLY2DEST"	    ] = &KGMSClient::ProcessGmcmd_gs;
	m_mapRoleProcess["MAIL"			] = &KGMSClient::ProcessGmcmd_gs;
	m_mapRoleProcess["ANNOUNCE"		] = &KGMSClient::ProcessGmcmd_gs;
	m_mapRoleProcess["MESSAGE"		] = &KGMSClient::ProcessGmcmd_gs;
	m_mapRoleProcess["BEHAVE"		] = &KGMSClient::ProcessGmcmd_gc;

	m_mapAccountProcess["KICKACCOUNT"   ] = &KGMSClient::ProcessGmcmd_gc;
	m_mapAccountProcess["ACCOUNTCHAT"	] = &KGMSClient::ProcessGmcmd_gc;
	m_mapAccountProcess["ROLELIST"	    ] = &KGMSClient::ProcessGmcmd_gc;

	m_mapProcess["ZONECHAT"] = &KGMSClient::ProcessGmcmd_gc; //gonggao

	m_aryResult[emKGMCMD_RESULT_SUCCESS	] = "RESULT:SUCCESS";		// 成功
	m_aryResult[emKGMCMD_RESULT_FAILURE	] = "RESULT:FAIL";			// 失败
	m_aryResult[emKGMCMD_RESULT_DISPFAIL] = "RESULT:DISPATCH_FAIL";	// 请求无法被发送，可能的原因例如目标区服不存在/未连上
	m_aryResult[emKGMCMD_RESULT_TIMEOUT	] = "RESULT:TIMEOUT";		// 超时
	m_aryResult[emKGMCMD_RESULT_NONE	] = "RESULT:NONE";			// （查询的）目标不存在
	m_aryResult[emKGMCMD_RESULT_NOTALLOW] = "RESULT:NOT_ALLOW";		// 操作不被许可
	m_aryResult[emKGMCMD_RESULT_WRONGCMD] = "RESULT:CMD_ERR";		// 命令错误

	for (int i = 0; i < fseye_protocol_maxcount; ++i)
	{
		m_aryProtoProcess[i] = NULL;
	}
	Clear( );
}

KGMSClient::~KGMSClient( )
{
}

void KGMSClient::Clear( )
{
	m_pClient	= NULL;
	m_nClientID = INVALID_VALUE;

	m_nPort		= INVALID_VALUE;
	m_szIP[0]	= 0;

	m_ulLastRcvPingTime	=	(unsigned long)time(NULL);

	m_UDPSock = INVALID_VALUE;

	m_nEnableCfgMod = TRUE;
	m_bEncrypt = FALSE;

	m_szBindIp[0] = '\0';
	memset( m_szRC4Key, 0, sizeof(m_szRC4Key) );
}

void KGMSClient::CalcSignMD5( int nX, char szMD5[64] )
{
	char szSign[MAX_DESC];
	int nY = ( A * nX * nX ) + ( B * nX ) + ( C );
	sprintf( szSign, "%d", nY );
	KG_EDStringToMD5String( szMD5, szSign );
}

int KGMSClient::LoadConfig(IGuard* pGuard)
{
	ASSERT(pGuard);
	//KIniFile IniFile;
	CHAR szPath[MAX_PATH];
	getcwd(szPath, MAX_PATH);
	g_SetRootPath(szPath);

	IIniFile* IniFile = g_OpenIniFile(CFGFILENAME);
	//if (IniFile.Load(CFGFILENAME))
	if (IniFile)
	{
		IniFile->GetString( SERVER_GMC, IPKEY, "", m_szIP, sizeof(m_szIP) );
		IniFile->GetInteger( SERVER_GMC, PORTKEY, 0, &m_nPort );
		IniFile->GetString( SERVER_GMC, LOGINACC, "gateway011", m_szAccountName, sizeof(m_szAccountName) );
		IniFile->GetString( SERVER_GMC, LOGINPWD, "gateway011", m_szLoginPwd, sizeof(m_szLoginPwd) );
		IniFile->GetInteger( SERVER_GMC, AREAINDEX, 1, &m_nAreaIndex );
		IniFile->GetInteger( SERVER_GMC, GROUPINDEX, 1, &m_nGroupIndex);
		IniFile->GetInteger(SERVER_GMC,	GC_SERVERID, 0, &m_nGCServerId);
		IniFile->GetInteger( SERVER_GMC,  ENABLECMOD, TRUE, &m_nEnableCfgMod );
		IniFile->GetString(SERVER_GMC, "BindIp", "", m_szBindIp, sizeof(m_szBindIp));
	}
	m_pGuard = pGuard;
	KG_COM_RELEASE(IniFile);
	return fseye_success;
}

int KGMSClient::InitNet( )
{
	if( m_pClient != NULL )
		return guard_err;

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
	return fseye_success;
}

void KGMSClient::UninitNet( )
{
	if( m_pClient != NULL )
	{
		if (m_nClientID != INVALID_VALUE)
		{
			m_pClient->Disconnect(m_nClientID);
			m_nClientID = INVALID_VALUE;
		}
		m_pClient->Cleanup( );
		m_pClient->Release( );
		m_pClient = NULL;
	}
}

int KGMSClient::ConnectTo( )
{
#define MAXTIME 10

	static bool bNotify = false;
	static unsigned long ulLastReconnectTime = 0;

	if( m_nClientID == INVALID_VALUE )
	{
		unsigned long ulTime = (unsigned long)time( NULL );

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
				printf("Bind to ip[%s]!\n", m_szBindIp);
			}
			else
			{
				printf("Bind none ip!\n");
			}
			m_nClientID = m_pClient->ConnectTo( m_szIP, m_nPort );
			ulLastReconnectTime = ulTime;

			if( m_nClientID !=  INVALID_VALUE )
			{
				bNotify = false;
				SendLoginMsg(m_szAccountName, m_szLoginPwd,m_nAreaIndex, m_nGroupIndex  );
				m_ulLastRcvPingTime	= ulTime;
			}
		}
	}

	return fseye_success;
}

void KGMSClient::SendLoginMsg(const char * szAccountName, const char * szPassword, int nAreaIndex, int nGroupIndex)
{
	if (!szAccountName || !szPassword) 
	{
		printf("Error! szAccountName or szPassword is null");
		return ;
	}
	GAME_MASTER_SERVICE_INFO info;
	info.cProtocol = GM_P_NORMAL;
	info.cSubProtocol = GMN_P_SERVICE_INFO;
	info.uServiceType = MST_GAME_ONLINE_OBJECTIVE;
	strcpy(info.szOperatorAccount, szAccountName);
	strcpy(info.szOperatorPasswd, szPassword);
	info.nAreaIndex = nAreaIndex;
	info.nGroupIndex = nGroupIndex;
	SendPackToServer((unsigned char *)&info, sizeof(info));
	
}
void KGMSClient::Ping(time_t tNow)
{
	//static unsigned long ulSendTime = 0;

	GAME_MASTER_SERVICE_PING sPing;
	sPing.cProtocol = GM_P_NORMAL;
	sPing.cSubProtocol = GMN_P_SERVICE_PING;
	sPing.uTime = (unsigned int)tNow;

	SendPackToServer(
		(unsigned char*)&sPing, 
		sizeof(sPing) );

	/*
		ulSendTime = (unsigned long)now;
	}

	if( m_nClientID !=  INVALID_VALUE && 
		now - m_ulLastRcvPingTime > MAX_ALIVE_TMIE )
	{
		m_pClient->Disconnect( m_nClientID );
		m_nClientID = INVALID_VALUE;
	}
	*/
}

void KGMSClient::SendSign( int nX )
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
#define  MAX_BUFFER_LEN 7000
//处理GMS的协议
int KGMSClient::ProcessMessages( )
{
	if( m_nClientID == INVALID_VALUE )
		return fseye_success;

	unsigned int nLen = 0;
	const void *pData = m_pClient->GetPackFromServer(m_nClientID, nLen);
	if (pData != NULL && nLen != 0)
	{
		ProcessMessasge(pData, nLen);
	}
	return fseye_success;
}

void KGMSClient::ProcessMessasge(CONST VOID* pData, UINT nLen)
{
	BYTE byProtocol = *(BYTE*)pData;
	printf("[GMC]Recv %d %d\n", byProtocol, nLen);
	char szCmd[MAX_BUFFER_LEN];
	if (nLen >= MAX_BUFFER_LEN - sizeof(c2r_gmcmd))
	{
		printf("GMC Data is Too Long , igr it !\n");
		return;
	}
	c2r_gmcmd *pCmd = (c2r_gmcmd *)szCmd;
	pCmd->wProtocol = c2r_gmcmd_def;
	pCmd->wServer	= m_nGCServerId;
	pCmd->nCmdLen	= (unsigned short)nLen;
	memcpy(pCmd + 1, pData, nLen);
	//m_pGuardPlugin->GetGuard()->PutMessage(pCmd, pCmd->nCmdLen + sizeof(c2r_gmcmd));
	switch(byProtocol)
	{
	case GM_P_OPERATION_REQUEST:
		{
			GAME_MASTER_PROTOCOL_WITH_DATA * pProt = (GAME_MASTER_PROTOCOL_WITH_DATA *) pData;
			if (nLen > sizeof(GAME_MASTER_PROTOCOL_HEADER))
			{
				memcpy(szCmd, &pProt->Data.Content[0], pProt->Data.uContentLength);
				szCmd[pProt->Data.uContentLength] = 0;
				ExecuteCmd(pProt->ProtInfo.szOperationKeyWord, szCmd);
				
			}
			else
			{
				printf("REQUEST %s \n", pProt->ProtInfo.szOperationKeyWord);
			}

		}
		break;
	case GM_P_OPERATION_REQUEST_ON_ACCOUNT:
		{
			GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA* pProto = (GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA*)pData;
			LPSTR pszName = pProto->Obj.szName;
			pszName[sizeof(pProto->Obj.szName) - 1] = '\0';
			LPSTR pszData = pProto->Data.Content;
			pszData[pProto->Data.uContentLength] = '\0';
			pProto->ProtInfo.szOperationKeyWord[sizeof(pProto->ProtInfo.szOperationKeyWord) - 1] = '\0';
			ExecuteCmdOnAccount(pszName,
				pProto->ProtInfo.szOperationKeyWord,
				pszData,
				&pProto->ProtInfo);
		}
		break;
	case GM_P_OPERATION_REQUEST_ON_ROLE:
		{
			GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA* pProto = (GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA*)pData;
			LPSTR pszName = pProto->Obj.szName;
			pszName[sizeof(pProto->Obj.szName) - 1] = '\0';
			LPSTR pszData = pProto->Data.Content;
			pszData[pProto->Data.uContentLength] = '\0';
			pProto->ProtInfo.szOperationKeyWord[sizeof(pProto->ProtInfo.szOperationKeyWord) - 1] = '\0';
			ExecuteCmdOnRole(pszName,
				pProto->ProtInfo.szOperationKeyWord,
				pszData,
				&pProto->ProtInfo);
		}
		break;
	}
	/*
	if (byProtocol >= fseye_protocol_maxcount)
	{
		printf("Unknown protocol[%u]!\n", byProtocol);
		return;
	}
	fnProtocolProcess pfnProcess = m_aryProtoProcess[byProtocol];
	if (pfnProcess)
	{
		(this->*pfnProcess)(pData, nLen);
	}
	*/
}

int KGMSClient::MainLoop()
{
	ConnectTo(); 
	ProcessMessages();
	//Ping();
	return fseye_success;
}

int KGMSClient::SendPackToServer(unsigned char* pData, unsigned int datasize)
{
	l2e_send_gm_message* pGm = (l2e_send_gm_message*)pData;

	if( m_nClientID == INVALID_VALUE )
		return guard_client_invalidhadle;
	else
	{
		if( datasize > MAX_DEBUFF_LEN )
			return guard_client_send_err;

		memcpy( EncryptBuf, pData, datasize );
		if( m_pClient->SendPackToServer( 
			m_nClientID, EncryptBuf, datasize ) != fseye_success )
			return guard_client_send_err;
	}
	return fseye_success;
}

KGMSClient& KGMSClient::Singleton( )
{
	static KGMSClient GC;

	return GC;
}

void KGMSClient::ClientEventNotify(
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
			KGMSClient* pGC = (KGMSClient*)lpParam;
			pGC->Disconnect( );
		}
		break;
	};
}

void KGMSClient::Disconnect( )
{
	m_nClientID = INVALID_VALUE;
}

void KGMSClient::NotifyDisconnect( )
{
}

VOID KGMSClient::ExecuteCmd(LPCSTR pszCmd, LPSTR pszData)
{
	ASSERT(pszCmd && pszData);
	CONST int MAX_CMD_LEN = 64;
	char szNewCmd[MAX_CMD_LEN];
	if (RegulateOperationKeyWord(pszData, pszCmd, GM_P_OPERATION_REQUEST, szNewCmd))
	{
		pszCmd = szNewCmd;
	}
	std::map<std::string, fnProcess>::iterator it = m_mapProcess.find(pszCmd);
	if (it != m_mapProcess.end())
	{
		fnProcess pfnProcess = it->second;
		
		(this->*pfnProcess)(pszData, pszCmd, "");
		m_pGuard->Log("Execute command cmd:%s, data:%s", pszCmd, pszData);
	}
}

VOID KGMSClient::ExecuteCmdOnRole(LPCSTR pszRole, LPCSTR pszCmd, LPCSTR pszData, GAME_MASTER_PROTOCOL_INFO* pSession)
{	
	CONST int MAX_CMD_LEN = 64;
	char szNewCmd[MAX_CMD_LEN];
	if (RegulateOperationKeyWord(pszData, pszCmd, GM_P_OPERATION_REQUEST_ON_ROLE, szNewCmd))
	{
		pszCmd = szNewCmd;
	}
	std::map<std::string, fnProcessRole>::iterator it = m_mapRoleProcess.find(pszCmd);
	if (it != m_mapRoleProcess.end())
	{
		fnProcessRole pfnProcess = it->second;		
		BOOL bResult = (this->*pfnProcess)(pszData, pszCmd, pszRole);
		if (bResult)
		{
			m_mapSession[m_nLastSession++] = *pSession;
		}
		m_pGuard->Log("Execute command on role:%s, cmd:%s, data:%s, result:%s",
			pszRole,
			pszCmd,
			pszData,
			bResult ? "success" : "failure");
	}
}

VOID KGMSClient::ExecuteCmdOnAccount(LPCSTR pszAccount, LPCSTR pszCmd, LPCSTR pszData, GAME_MASTER_PROTOCOL_INFO* pSession)
{
	CONST int MAX_CMD_LEN = 64;
	char szNewCmd[MAX_CMD_LEN];
	if (RegulateOperationKeyWord(pszData, pszCmd, GM_P_OPERATION_REQUEST_ON_ACCOUNT, szNewCmd))
	{
		pszCmd = szNewCmd;
	}
	std::map<std::string, fnProcessAccount>::iterator it = m_mapAccountProcess.find(pszCmd);
	if (it != m_mapAccountProcess.end())
	{
		fnProcessAccount pfnProcess = it->second;
		BOOL bResult = (this->*pfnProcess)( pszData, pszCmd, pszAccount);
		if (bResult)
		{
			m_mapSession[m_nLastSession++] = *pSession;
		}
		m_pGuard->Log("Execute command on account:%s, cmd:%s, data:%s, result:%s",
			pszAccount,
			pszCmd,
			pszData,
			bResult ? "success" : "failure");
	}
}

VOID KGMSClient::Translate(LPCSTR pszMsg, LPSTR pszBuf, INT nBufLen)
{
	ASSERT(pszMsg && pszBuf && nBufLen > 0);
	INT nLen = nBufLen - 1;
	LPCSTR pszSrc = pszMsg;
	LPSTR pszDst = pszBuf;
	while (*pszSrc != '\0')
	{
		if (nLen <= 0)
			break;
		CHAR ch = *pszSrc++;
		if (ch != '"')
		{
			*pszDst++ = ch;
			--nLen;
		}
		else
		{
			*pszDst++ = '\\';
			if (nLen - 1 <= 0)
				break;
			*pszDst++ = ch;
			nLen -= 2;
		}
	}
	*pszDst = '\0';
}

VOID KGMSClient::ProecssGmResult(CONST VOID* pData, UINT nLen)
{
	size_t                  uStrLen     = 0;	

	l2e_send_gm_message* pResult = (l2e_send_gm_message*)pData;
	uStrLen = strlen(pResult->szMessage);
	int iMsgLen = sizeof(char) * (uStrLen + 1);
	if (nLen < sizeof(l2e_send_gm_message) ||
		nLen != sizeof(l2e_send_gm_message) + iMsgLen)
	{
		ASSERT(FALSE);
		return;
	}
	int nSession = 0;
	nSession = atoi(pResult->szGmName);


	std::map<int, GAME_MASTER_PROTOCOL_INFO>::iterator it = m_mapSession.find(nSession);
	if (it != m_mapSession.end())
	{
		GAME_MASTER_PROTOCOL_INFO* pSession = &it->second;
		CONST INT MAX_MSGLEN = 4096;
		CHAR szBuf[sizeof(GAME_MASTER_PROTOCOL_WITH_DATA) + MAX_MSGLEN];
		GAME_MASTER_PROTOCOL_WITH_DATA* pRet = (GAME_MASTER_PROTOCOL_WITH_DATA*)szBuf;
		pRet->cProtocol = GM_P_OPERATION_RESPONSE;
		pRet->ProtInfo	= *pSession;
		pRet->ProtInfo.OperationFamily = GMOF_RET;

	/*	LPCSTR pszFormat = (pResult->byAction == 0 || pResult->byResult != emKGMCMD_RESULT_SUCCESS) ?
			m_aryResult[pResult->byResult] :
		"T_DATA:%s";*/

		LPCSTR pszFormat = "T_DATA:%s";
		printf("receive gm result from gc sucessfully\n");
		
		INT nLen = snprintf(pRet->Data.Content, MAX_MSGLEN - 1, pszFormat, pResult->szMessage) + 1;
		pRet->Data.Content[nLen - 1] = '\0';
		pRet->Data.uContentLength = nLen;
		SendPackToServer((unsigned char*)pRet, sizeof(GAME_MASTER_PROTOCOL_WITH_DATA) - 1 + nLen);
		m_mapSession.erase(it);
		//printf("gm result:%s", pRet->Data);
		return ;
	}

}

BOOL KGMSClient::ProcessAccount_Kick(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd)
{
	ASSERT(pszName && pszData);
	return Kick(pszName, pszData, FALSE, pszCmd);
}

BOOL KGMSClient::ProcessRole_Kick(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd)
{
	return Kick(pszName, pszData, TRUE, pszCmd);
}

BOOL KGMSClient::Kick(LPCSTR pszName, LPCSTR pszData, BOOL bIsRoleName, LPCSTR pszCmd)
{
	ASSERT(pszName && pszData);
	BOOL bResult = FALSE;
	//do
	//{
	//	CONST INT MAX_MSGLEN = 1024;
	//	CHAR szBuf[sizeof(e2l_gmcmd_gs) + MAX_MSGLEN];
	//	e2l_gmcmd_gs* pCmd = (e2l_gmcmd_gs*)szBuf;
	//	pCmd->wProtocol		= e2l_gmcmd_gs_def;
	//	pCmd->wSubProtocol	= e2l_gmcmd_gs_def;
	//	pCmd->wServer		= m_nGCServerId;
	//	strncpy(pCmd->szRoleName, pszName, sizeof(pCmd->szRoleName) - 1);
	//	pCmd->szRoleName[sizeof(pCmd->szRoleName) - 1] = '\0';

	//	CHAR szName[128];
	//	Translate(pszName, szName, sizeof(szName));
	//	LPSTR pszCmdBuf = (LPSTR)(pCmd + 1);

	//	CONST INT MAX_COUNT = 8;
	//	CONST INT MAX_PARAMENT_LEN = 32;
	//	LPSTR ppszParameterlist[MAX_COUNT];
	//	char szParameters[MAX_COUNT][MAX_PARAMENT_LEN];	
	//	for (int i = 0; i < MAX_COUNT; i++)
	//	{
	//		ppszParameterlist[i] = szParameters[i];
	//	}

	//	sprintf(szParameters[0], "%s\0", pszName);
	//	sprintf(szParameters[1], "%d\0", m_nLastSession);
	//	
	//	
	//	
	//	GenerateGMParams(pszCmd, pszData, ppszParameterlist, 2);

	//	GenerateGMScript(pCmd->szCommand, pszCmd, ppszParameterlist);
	//	
	//	m_pGuardPlugin->GetGuard()->PutMessage(pCmd, sizeof(e2l_gmcmd_gs) + strlen(pCmd->szCommand));
	//	bResult = TRUE;
	//	
	//}
	//while (0);
	return bResult;
}

BOOL KGMSClient::ProcessAccount_RoleList(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd)
{	
	return TRUE;
}

BOOL KGMSClient::ProcessAccount_Chat(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd)
{	

	BOOL bResult = FALSE;
	
	return bResult;
}

BOOL KGMSClient::ProcessRole_Chat(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd)
{

	BOOL bResult = FALSE;	
	
	return bResult;
}

//向全区发送chat信息
BOOL KGMSClient::ProcessZone_Message(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd)
{
	

	BOOL bResult = FALSE;
	
	return bResult;
}


BOOL KGMSClient::ProcessZone_Announce(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd)
{
	
	BOOL bResult = FALSE;
	
	return bResult;
}
BOOL KGMSClient::ProcessRole_Fly(LPCSTR pszName, LPCSTR pszData, LPCSTR pcszCmd)
{
	
	return TRUE;
}

BOOL KGMSClient::ProcessRole_Mail(LPCSTR pszName, LPCSTR pszData, LPCSTR pszCmd)
{
	

	BOOL bResult = FALSE;
	
	return bResult;
}

void KGMSClient::PostGmChannelMessage(VOID* pData, UINT nLen)
{
	ASSERT(pData && nLen > 0);
	do 
	{
		l2e_gmchn* pMsg = (l2e_gmchn*)pData;
		if (nLen <= sizeof(l2e_gmchn) ||
			nLen != sizeof(l2e_gmchn) + pMsg->nMsgLen ||
			pMsg->nMsgLen <= 0)
		{
			ASSERT(FALSE);
			break;
		}
		pMsg->szAccount[sizeof(pMsg->szAccount) - 1] = '\0';
		pMsg->szRole[sizeof(pMsg->szRole) - 1] = '\0';

		CHAR szPrefix[128];
		CHAR szBuf[8192];
		INT	nPrefixLen = sprintf(szPrefix, "ROLE:%s TYPE:bug-none T_DATA:%d-", pMsg->szRole, pMsg->nMsgLen);
		INT nPackLen = sizeof(GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA) - 1 + nPrefixLen + pMsg->nMsgLen;
		if (nPackLen > sizeof(szBuf))
		{
			ASSERT(FALSE);
			break;
		}

		GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA* pChat = (GAME_MASTER_PROTOCOL_WITH_OBJNAME_AND_DATA*)szBuf;
		pChat->cProtocol = GM_P_OPERATION_REQUEST_ON_ACCOUNT;
		pChat->Length = (USHORT)nPackLen - PROTOCOL_MSG_SIZE;
		strncpy(pChat->Obj.szName, pMsg->szAccount, sizeof(pChat->Obj.szName));
		pChat->Obj.szName[sizeof(pChat->Obj.szName) - 1] = 0;
		pChat->ProtInfo.OperationFamily = GMOF_GSINFO;
		pChat->ProtInfo.SerialNo.ByRefNo.uRefNo[0] = (++m_nLastSerialNo);
		pChat->ProtInfo.SerialNo.ByRefNo.uRefNo[1] = 0;
		strcpy(pChat->ProtInfo.szOperationKeyWord, "CHAT");
		pChat->Data.uContentLength = nPrefixLen + pMsg->nMsgLen;
		memcpy(pChat->Data.Content, szPrefix, nPrefixLen);
		LPSTR pszMsg = (LPSTR)(pMsg + 1);
		//pszMsg[pMsg->nMsgLen] = '\0';
		memcpy(pChat->Data.Content + nPrefixLen, pszMsg, pMsg->nMsgLen);
		
		SendPackToServer((unsigned char*)szBuf, nPackLen);
	}
	while(0);
}
BOOL KGMSClient::SendGMExcuteCmd(e2l_ExeGMCmd * pProtocl)
{
	return TRUE;
}

VOID KGMSClient::SplitString(LPSTR pszCmd, LPSTR* ppszCmdList, INT nMaxCount, INT& nRetCount, CHAR chSep)
{
	ASSERT(pszCmd && ppszCmdList && nMaxCount > 1);
	INT nCount = 0;
	INT nIndex = 0;
	ppszCmdList[nCount++] = pszCmd;
	while (pszCmd[nIndex] != '\0' && nCount < nMaxCount)
	{
		if (pszCmd[nIndex] == chSep)
		{
			pszCmd[nIndex] = '\0';
			if (pszCmd[nIndex + 1] != '\0')
			{
				ppszCmdList[nCount++] = &pszCmd[nIndex + 1];
			}
		}
		nIndex++;
	}
	nRetCount = nCount;
}

VOID KGMSClient::Process_Chat(LPSTR pszCmd)
{
	
}

void KGMSClient::PostRoleList(VOID* pData, UINT nLen)
{
	ASSERT(pData && nLen);
	do 
	{
		l2e_rolelist* pResult = (l2e_rolelist*)pData;
		if (nLen < sizeof(l2e_rolelist) ||
			nLen != sizeof(l2e_rolelist) + pResult->byRoleCount * emKEYE_NAMELEN)
		{
			ASSERT(FALSE);
			break;
		}

		CHAR szBuf[sizeof(GAME_MASTER_PROTOCOL_WITH_DATA) + 255 * 100];
		GAME_MASTER_PROTOCOL_WITH_DATA* pRet = (GAME_MASTER_PROTOCOL_WITH_DATA*)szBuf;
		std::map<int, GAME_MASTER_PROTOCOL_INFO>::iterator it = m_mapSession.find(pResult->nSession);
		if (it == m_mapSession.end())
		{
			ASSERT(FALSE);
			break;
		}
		pRet->ProtInfo = it->second;
		m_mapSession.erase(it);
		pRet->ProtInfo.OperationFamily = GMOF_RET;
		pRet->cProtocol = GM_P_OPERATION_RESPONSE;
		strcpy(pRet->ProtInfo.szOperationKeyWord, "ROLELIST");

		INT nPackLen = 0;
		if (pResult->byRoleCount == 0)
		{
			pRet->Data.uContentLength = sprintf(pRet->Data.Content, "RESULT:NONE");
			nPackLen = sizeof(GAME_MASTER_PROTOCOL_WITH_DATA) - 1 + pRet->Data.uContentLength;
			pRet->Length = (USHORT)nPackLen - PROTOCOL_MSG_SIZE;
			SendPackToServer((unsigned char*)szBuf, nPackLen);
			break;
		}

		LPSTR pszRoleName = (LPSTR)(pResult + 1);
		CHAR szRoleList[255 * 100];
		INT nRoleListLen = 0;
		for (INT i = 0; i < pResult->byRoleCount; ++i)
		{
			nRoleListLen += sprintf(&szRoleList[nRoleListLen], "Role=%s\n", pszRoleName);
			nRoleListLen += sprintf(&szRoleList[nRoleListLen], "CreateTime=1900-1-1 0:0\n");
			nRoleListLen += sprintf(&szRoleList[nRoleListLen], "nLevel=0\n");
			nRoleListLen += sprintf(&szRoleList[nRoleListLen], "nStat=0\n");
			pszRoleName += emKEYE_NAMELEN;
		}
		--nRoleListLen;
		szRoleList[nRoleListLen] = '\0';
		pRet->Data.uContentLength = sprintf(pRet->Data.Content, "T_DATA:%d-%s", nRoleListLen, szRoleList);
		nPackLen = sizeof(GAME_MASTER_PROTOCOL_WITH_DATA) - 1 + pRet->Data.uContentLength;
		pRet->Length = (USHORT)nPackLen - PROTOCOL_MSG_SIZE;
		SendPackToServer((unsigned char*)szBuf, nPackLen);
	}
	while (0);
}
//add by SunYong
//查找gm脚本表，得到与功能pszGM对应的脚本，并替换其中的参数，形成发给GameCenter的指令保存到pszRtScritp里返回
BOOL KGMSClient::GenerateGMScript(LPSTR pszRtScript, LPCSTR pszCmd, LPSTR* ppszParameterlist)
{
	char pszGMName[64];
	sprintf(pszGMName, "[%s]", pszCmd);//对应与功能pszGM,在脚本文件里保存的是[pszGM].
	char* pcBegin = NULL;
	pcBegin = strstr(m_szGMScriptFileContent, pszGMName);	
	if (!pcBegin)
	{
		return false;
	}
	pcBegin += (int)strlen(pszGMName) + 1;

	char* pcEnd = NULL;
	pcEnd = strstr(pcBegin, pszGMName);
	char* pcWalker = pcBegin;
	INT nLen = pcEnd - pcBegin;
	INT j = 0;
	INT i = 0;
	while (i < nLen)
	{
		switch(pcWalker[i])
		{
		case '{':
			if (pcWalker[i+1] == '{')//{转义，两个{表示一个
			{
				pszRtScript[j++] = '{';
				i += 2;
			}		
			else
			{
				INT n = atoi(&pcWalker[i+1]);
				for (char* pcParameter = ppszParameterlist[n]; *pcParameter != '\0'; pcParameter++)
				{
					pszRtScript[j++] = *pcParameter;
				}
				i += 3;
			}
			break;
		case '}':
			pszRtScript[j++] = pcWalker[i];
			i += 2;
			break;
		case '-':
			if (pcWalker[i+1] == '-')
			{
				while (pcWalker[i++] != '\n')
				{
					NULL;
				}
			}
			else
			{
				pszRtScript[j++] = pcWalker[i];
				i += 1;
			}
			break;
		default:
			pszRtScript[j++] = pcWalker[i];
			i +=1;
		}		
	}
	pszRtScript[j] = '\0';

	return TRUE;
}
BOOL KGMSClient::GenerateGMParams(LPCSTR pszCmd, LPCSTR pszData, LPSTR* ppszParameterlist, CONST INT nYetHaveCnt)
{
	CONST int WordLen = 64;
	char szCmdName[WordLen];
	sprintf(szCmdName, "[%s]\0", pszCmd);
	char* pcBegin     = NULL;
	pcBegin = strstr(m_szParamRegularFileContent, szCmdName);
	if (pcBegin == NULL)
	{
		return false;
	}
	pcBegin += strlen(szCmdName) + 1;
	char * pcEnd = NULL;
	pcEnd = strstr(pcBegin, szCmdName);
	if (pcEnd == NULL)
	{
		return false;
	}
	int nRegulationLen = pcEnd - pcBegin;
	int iRegulation = 0;
	int iParamCnt = nYetHaveCnt;
	
	char cPre  = '\0';
	char cPost = '\0';
	char* pcRegulationWalker = pcBegin;
	
	char szKeyWord[WordLen];
	int nParam_OneKeyWordCnt = 0;

	while (iRegulation < nRegulationLen)
	{
		if (!GetOneKeyWord(pcRegulationWalker, iRegulation, szKeyWord, nRegulationLen))
		{
			return iRegulation == nRegulationLen;
		}

		nParam_OneKeyWordCnt = GetParamCnt_OneKeyWord(pcRegulationWalker, iRegulation, nRegulationLen);

		pcBegin = (char*)strstr(pszData, szKeyWord);
		if (!pcBegin)
		{
			printf("%s in %s is not exits in received data\r\n", szKeyWord, PARAMETERREGULARFILENAME);
			return false;
		}
		for (int i = 0; i < nParam_OneKeyWordCnt; i++)
		{
			if (!GetPrePost_fix(cPre, cPost, pcRegulationWalker, iRegulation, nRegulationLen))
			{
				return iRegulation == nRegulationLen;
			}			
			pcBegin = strchr(pcBegin, cPre);
			if (!pcBegin)
			{
				printf("prefix %c in %s is not exits in received data\r\n", cPre, PARAMETERREGULARFILENAME);
				m_pGuard->Log("prefix %c in %s is not exits in received data\r\n", cPre, PARAMETERREGULARFILENAME);
				return false;
			}
			++pcBegin;
			pcEnd = pcBegin;
			while (*pcEnd != '\0' && !IsParamEnd(cPost, *pcEnd))
			{
				++pcEnd;
			}
			if (*pcEnd == '\0' && !IsParamEnd(cPost, *pcEnd))
			{
				printf("postfix %c in %s is not exits in received data\r\n", cPost, PARAMETERREGULARFILENAME);
				return false;
			}
			int nParamLen = pcEnd - pcBegin;
			strncpy(ppszParameterlist[iParamCnt], pcBegin, nParamLen);
			ppszParameterlist[iParamCnt][nParamLen] = '\0';
			++iParamCnt;
		}		
	}	
	return TRUE;
}
INT KGMSClient::GetParamCnt_OneKeyWord(LPCSTR pcWalker, int iWalker, CONST int nLen)
{
	int nCnt = 0;
	while (iWalker < nLen && pcWalker[iWalker] != '{')
	{
		++iWalker;
	}
	while (iWalker < nLen && pcWalker[iWalker] != ':')
	{
		if (pcWalker[iWalker] == '{')
		{
			++nCnt;			
		}
		++iWalker;
	}
	return nCnt;
}
BOOL KGMSClient::GetPrePost_fix(char& cPre, char& cPost, LPCSTR pcWalker, int& iWalker, CONST int nLength)
{	
	while (pcWalker[iWalker] != '{' && iWalker < nLength)
	{
		++iWalker;		
	}
	if (iWalker < nLength)
	{
		cPre = pcWalker[iWalker - 1];
		while (pcWalker[iWalker] != '}' && iWalker < nLength)
		{
			++iWalker;
		}
		if (iWalker < nLength)
		{
			if (iWalker + 1 < nLength)
			{
				++iWalker;
				cPost = pcWalker[iWalker];
			}
			else
			{
				cPost = '\0';
			}
		}
	}
	return iWalker < nLength;
}
BOOL KGMSClient::GetOneKeyWord(LPCSTR pcWalker, int& iWalker, LPSTR pszKeyWord, CONST int nLength)
{
	if (!pcWalker || iWalker < 0)
	{
		return false;
	}
	while (IsBalank(pcWalker[iWalker]) && iWalker < nLength)
	{
		++iWalker;
	}
	int i = 0;
	if (iWalker < nLength)
	{
		while (pcWalker[iWalker] != ':' && iWalker < nLength)
		{
			pszKeyWord[i++] = pcWalker[iWalker++];
		}
		pszKeyWord[i] = '\0';

	}
	return iWalker < nLength;

}
BOOL KGMSClient::IsParamEnd(char cPost, char cTest)
{
	if (IsBalank(cPost))
	{
		return IsBalank(cTest);
	}
	else
	{
		return cPost == cTest;
	}
}
BOOL KGMSClient::IsBalank(char cTest)
{
	return cTest == ' ' || cTest == '\t' || cTest == '\n' || cTest == '\0';
}
bool KGMSClient::LoadGMScriptTable()
{
	FILE* fp = NULL;

	fp = fopen(SCRIPTFILENAME, "r");
	if (fp == NULL)
	{
		printf("open file failed! : j3gmScript.txt File does not exits...\r\n");
		return FALSE;
	}
	fread(m_szGMScriptFileContent, sizeof(char), sizeof(m_szGMScriptFileContent)/sizeof(char), fp);
	fclose(fp);
	return TRUE;
}
bool KGMSClient::LoadParamRegular()
{
	FILE* fp = NULL;

	fp = fopen(PARAMETERREGULARFILENAME, "r");
	if (fp == NULL)
	{
		printf("open file failed! : j3ParamRegular.txt File does not exits...\r\n");
		return FALSE;
	}
	fread(m_szParamRegularFileContent, sizeof(char), sizeof(m_szParamRegularFileContent)/sizeof(char), fp);
	fclose(fp);
	return TRUE;

}
BOOL KGMSClient::RegulateOperationKeyWord(LPCSTR pszData, LPCSTR pszOldCmd, BYTE byProtocol, LPSTR pszNewCmd)
{
	BOOL bChanged = false;
	if (strcmp(pszOldCmd, "FLY") == 0)
	{
		LPSTR pszKeyWord = "TARGET";
		CONST CHAR* pcStr = strstr(pszData, pszKeyWord);
		if (pcStr)
		{
			sprintf(pszNewCmd, "%s2TARGET\0", pszOldCmd);
		}
		else
		{
			sprintf(pszNewCmd, "%s2DEST\0", pszOldCmd);
		}
		bChanged = true;
	}
	else if (strcmp(pszOldCmd, "CHAT") == 0)
	{
		if (byProtocol == GM_P_OPERATION_REQUEST)
		{
			sprintf(pszNewCmd, "ZONE%s\0",pszOldCmd);
		}
		else if (byProtocol == GM_P_OPERATION_REQUEST_ON_ROLE)
		{
			sprintf(pszNewCmd, "ROLE%s\0", pszOldCmd);
		}
		else if (byProtocol == GM_P_OPERATION_REQUEST_ON_ACCOUNT)
		{
			sprintf(pszNewCmd, "ACCOUNT%s\0", pszOldCmd);
		}
		bChanged = true;
	}
	else if (strcmp(pszOldCmd, "KICK") == 0)
	{
		if (byProtocol == GM_P_OPERATION_REQUEST_ON_ACCOUNT)
		{
			sprintf(pszNewCmd, "%sACCOUNT",pszOldCmd);
		}
		else if (byProtocol == GM_P_OPERATION_REQUEST_ON_ROLE)
		{
			sprintf(pszNewCmd, "%sROLE\0", pszOldCmd);
		}
		bChanged = true;

	}
	/*else if (strcmp(pszOldCmd, "BEHAVE") == 0)
	{
		CONST INT MAX_COUNT = 2;
		CONST INT MAX_PARAMENT_LEN = 32;
		LPSTR ppszParameterlist[MAX_COUNT];
		char szParameters[MAX_COUNT][MAX_PARAMENT_LEN];	

		for (int i = 0; i < MAX_COUNT; i++)
		{
			ppszParameterlist[i] = szParameters[i];
			szParameters[i][0] = '\0';
		}
		GenerateGMParams("BEHAVE", pszData, ppszParameterlist, 0);
		sprintf(pszNewCmd, "%s%s", szParameters[0], szParameters[1]);
	}*/
	return bChanged;	

}
BOOL KGMSClient::ProcessGmcmd_gs(LPCSTR pszData, LPCSTR pszCmd, LPCSTR pszName)
{
	ASSERT(pszName && pszData);
	BOOL bResult = FALSE;
	do
	{
		CONST INT MAX_MSGLEN = 10240;
		CHAR szBuf[sizeof(e2l_gmcmd_gs) + MAX_MSGLEN];
		e2l_gmcmd_gs* pCmd = (e2l_gmcmd_gs*)szBuf;
		pCmd->wProtocol		= e2l_header_def;
		pCmd->wSubProtocol	= e2l_gmcmd_gs_def;
		pCmd->wServer		= m_nGCServerId;
		sprintf(pCmd->szGmName, "%d\0", m_nLastSession);

		strncpy(pCmd->szRoleName, pszName, sizeof(pCmd->szRoleName) - 1);
		pCmd->szRoleName[sizeof(pCmd->szRoleName) - 1] = '\0';

		CHAR szName[128];
		Translate(pszName, szName, sizeof(szName));
		LPSTR pszCmdBuf = (LPSTR)(pCmd + 1);

		CONST INT MAX_COUNT = 8;
		CONST INT MAX_PARAMENT_LEN = 32;
		LPSTR ppszParameterlist[MAX_COUNT];
		char szParameters[MAX_COUNT][MAX_PARAMENT_LEN];	

		for (int i = 0; i < MAX_COUNT; i++)
		{
			ppszParameterlist[i] = szParameters[i];
			szParameters[i][0] = '\0';
		}

		sprintf(szParameters[0], "%d", m_nLastSession);
		sprintf(szParameters[1], "%s", pszName);

		GenerateGMParams(pszCmd, pszData, ppszParameterlist, 2);
		GenerateGMScript(pCmd->szCommand, pszCmd, ppszParameterlist);
		
		m_pGuardPlugin->GetGuard()->PutMessage(pCmd, sizeof(e2l_gmcmd_gs) + strlen(pCmd->szCommand));
		bResult = TRUE;

	}
	while (0);

	return bResult;
}

BOOL KGMSClient::ProcessGmcmd_gc(LPCSTR pszData, LPCSTR pszCmd, LPCSTR pszAccount)
{
	ASSERT(pszData);
	BOOL bResult = FALSE;
	do
	{
		CONST INT MAX_MSGLEN = 10240;
		CHAR szBuf[sizeof(e2l_gmcmd_gc) + MAX_MSGLEN];
		e2l_gmcmd_gc* pCmd = (e2l_gmcmd_gc*)szBuf;
		pCmd->wProtocol		= e2l_header_def;
		pCmd->wSubProtocol	= e2l_gmcmd_gc_def;
		pCmd->wServer		= m_nGCServerId;
		sprintf(pCmd->szGmName, "%d\0", m_nLastSession);

		CONST INT MAX_COUNT = 8;
		CONST INT MAX_PARAMENT_LEN = 32;
		LPSTR ppszParameterlist[MAX_COUNT];
		char szParameters[MAX_COUNT][MAX_PARAMENT_LEN];	

		for (int i = 0; i < MAX_COUNT; i++)
		{
			ppszParameterlist[i] = szParameters[i];
			szParameters[i][0] = '\0';
		}
		sprintf(szParameters[0], "%d", m_nLastSession);
		sprintf(szParameters[1], "%s", pszAccount);

		GenerateGMParams(pszCmd, pszData, ppszParameterlist, 2);		
		GenerateGMScript(pCmd->szCommand, pszCmd, ppszParameterlist);
		m_pGuardPlugin->GetGuard()->PutMessage(pCmd, sizeof(e2l_gmcmd_gc) + strlen(pCmd->szCommand));
		bResult = TRUE;
	}while (0);
	
	return bResult;

}

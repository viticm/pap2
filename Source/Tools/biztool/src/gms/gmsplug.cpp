/* -------------------------------------------------------------------------
//	文件名		：	kdatabaseplugin.cpp
//	创建者		：	wangbin
//	创建时间	：	2007-12-19 10:44:00
//	功能描述	：	数据库插件实现：执行指定的SQL语句然后将结果返回
// -----------------------------------------------------------------------*/
#include "stdafx.h"
#include "gmsplug.h"
#include "Engine/KGLog.h"
#include "def.h"
//#include "engine/kinifile.h"
#include "fsencrypt.h"
#include "kclient.h"
#include "kgmsclient.h"
#include "fseye_protocol.h"

#define DB_CFG					"guard.cfg"
#define SECTION_GMS				"GMS"
#define SQL_SETUTF8				"SET names utf8"
#define SQL_SETGBK				"SET character_set_client=gbk"

KGMSPlug::KGMSPlug() : m_pSender(NULL), m_pGuard(NULL), m_tLastPing(0)
{
	m_pGMSClient = new KGMSClient;
	m_pGMSClient->SetOwner(this);
}

KGMSPlug::~KGMSPlug()
{
	delete m_pGMSClient;
	m_pGMSClient = NULL;
	
}

int KGMSPlug::Disconnect()
{
	return fseye_success;
}

int KGMSPlug::GetDescribe(char* pszDesc, int nLen)
{
	ASSERT(pszDesc && nLen > 0);
	strncpy(pszDesc, "GameCenter Service Plugin", nLen - 1);
	pszDesc[nLen - 1] = '\0';
	return fseye_success;
}

int KGMSPlug::GetGUID(char* pszGuid, int nLen)
{
	ASSERT(pszGuid && nLen > 0);
	strncpy(pszGuid, "{244627F1-5FD0-43eb-8B2D-12E90B853132}", nLen - 1);
	pszGuid[nLen - 1] = '\0';
	return fseye_success;
}

int KGMSPlug::GetVersion(int& nMaxVersion, int& nMinVersion)
{
	nMaxVersion = 1;
	nMinVersion = 1;
	return fseye_success;
}

int KGMSPlug::GetAuthor(char* pszAuthor, int nLen)
{
	ASSERT(pszAuthor && nLen > 0);
	strncpy(pszAuthor, "Romandou", nLen - 1);
	pszAuthor[nLen - 1] = '\0';
	return fseye_success;
}

int KGMSPlug::Breathe()
{
	time_t tNow = time(NULL);
	if (m_tLastPing == 0)
	{
		m_tLastPing = tNow;
	}
	else if (tNow - m_tLastPing >= emPING_INTERVAL)
	{
		m_pGMSClient->Ping(tNow);
		m_tLastPing = tNow;
	}
	m_pGMSClient->MainLoop();
	return fseye_success;
}

int KGMSPlug::LoadPlug(ISender* pSender, IGuard* pGuard)
{
	ASSERT(pSender && pGuard);
	int nRet = mydb_err_opendb;
	do 
	{
		CHAR szPath[MAX_PATH];
		pGuard->GetRootPath(szPath);
	//	g_SetRootPath(szPath);

		if (!Initialize(pGuard))
			break;
		m_pSender = pSender;
		m_pGuard = pGuard;
		nRet = fseye_success;
	}
	while(0);
	return nRet;
}

int KGMSPlug::Release()
{
	UnInitialize();
	return fseye_success;
}

bool KGMSPlug::CheckProtocol(int nProtocol)
{
	return nProtocol == r2c_gmcmd_def ||
		nProtocol == l2e_gmcmd_def ||		
		nProtocol == l2e_send_gm_message_def ||
		nProtocol == l2e_gmchn_def ||		
		nProtocol == e2l_ExeGMCmd_def ||
		nProtocol == l2e_rolelist_def;
}

//处理来自eyes或GUARD内部plug之间的消息
int KGMSPlug::ProcessNetMessage(unsigned char* pData, unsigned int nLen)
{
	ASSERT(pData && nLen > 0);
	int nRet = fseye_success;
	BYTE byProtocol = *(BYTE*)pData;
	switch (byProtocol)
	{
		//网站要求执行GM指令
	case e2l_ExeGMCmd_def:
		{
			m_pGMSClient->SendGMExcuteCmd((e2l_ExeGMCmd *)pData);
		}
		break;
		//gamecenter向gmc询问数据，请求
	case r2c_gmcmd_def:
		{
			r2c_gmcmd * pCmd = (r2c_gmcmd *)pData; 
			m_pGMSClient->SendPackToServer((unsigned char *)&pCmd->Command[0], nLen - sizeof(KProtocolHead));
		}break;
	case l2e_gmcmd_def:
		m_pGMSClient->ProecssGmResult(pData, nLen);
		break;
	case l2e_send_gm_message_def: // 
		m_pGMSClient->ProecssGmResult(pData, nLen);
		break;
	case l2e_gmchn_def:
		m_pGMSClient->PostGmChannelMessage(pData, nLen);
		break;
	case l2e_rolelist_def:
		m_pGMSClient->PostRoleList(pData, nLen);
		break;
	default:
		ASSERT(FALSE);
		nRet = fseye_success;
		break;
	}
	
	return nRet;
}

BOOL KGMSPlug::Initialize(IGuard* pGuard)
{
	ASSERT(pGuard);
	return 
		m_pGMSClient->LoadConfig(pGuard) == fseye_success &&
		m_pGMSClient->InitNet() == fseye_success &&
		m_pGMSClient->LoadGMScriptTable()&&
		m_pGMSClient->LoadParamRegular();
}

VOID KGMSPlug::UnInitialize()
{
	m_pGMSClient->UninitNet();
}

VOID KGMSPlug::SendToServer(VOID * pData, size_t ulLen)
{
	m_pSender->SendPackToServer((unsigned char *)pData, (unsigned int)ulLen);
}

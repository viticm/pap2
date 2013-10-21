/* -------------------------------------------------------------------------
//	文件名		：	kmonitor.cpp
//	创建者		：	wangbin
//	创建时间	：	2007-12-19 15:29:00
//	功能描述	：	协议中转
// -----------------------------------------------------------------------*/
#pragma warning(disable:4005)

#include "kmonitor.h"
#include "Engine.h"
#include "fseye_protocol.h"


#define INI_GUARDCFG			"guard.cfg"
#define SEC_SERVER				"Server"
#define KEY_COUNT				"Count"
#define KEY_SERVER				"Server"

KMonitor::KMonitor() : m_pSender(NULL), m_pServer(NULL), m_pGuard(NULL)
{
}

KMonitor::~KMonitor()
{
}

int KMonitor::LoadPlug(ISender* pSender, IGuard* pGuard)
{
	ASSERT(pSender && pGuard);
	INT nRet = guard_err;
	do
	{
		m_pServer = new KSERVER[emKGUARDSERVER_COUNT];
		if (!m_pServer)
		{
			ASSERT(FALSE);
			break;
		}
		for (INT i = 0; i < emKGUARDSERVER_COUNT; ++i)
		{
			m_pServer[i].wServer = 0;
			m_pServer[i].nPid = 0;
		}
		m_pSender = pSender;
		m_pGuard = pGuard;
		nRet = fseye_success;
	}
	while (0);
	return nRet;
}

int KMonitor::Release()
{
	ASSERT(m_pServer);
	for (INT i = 0; i < emKGUARDSERVER_COUNT; ++i)
	{
		m_pServer[i].cStream.UnInitialize();
		m_pServer[i].wServer = 0;
	}
	delete[] m_pServer;
	m_pServer = NULL;
	return fseye_success;
}

KMonitor::KSERVER* KMonitor::FindServer(WORD wServer)
{
	for (int i = 0; i < emKGUARDSERVER_COUNT; ++i)
	{
		if (m_pServer[i].wServer == wServer)
			return &m_pServer[i];
	}
	return NULL;
}

void KMonitor::Process_ServerId(char* pData, size_t nLen)
{
	if (nLen != sizeof(e2g_serverid))
	{
		m_pGuard->Log("Receive invalid e2g_serverid_def protocol!");
		return;
	}
	e2g_serverid* pProtocol = (e2g_serverid*)pData;
	INT nCount = sizeof(pProtocol->aryServerId) / sizeof(pProtocol->aryServerId[0]);
	for (INT i = 0; i < emKGUARDSERVER_COUNT && i < nCount; ++i)
	{
		INT nServerId = pProtocol->aryServerId[i];
		if (nServerId <= 0 || m_pServer[i].wServer != 0)
			continue;
		KSERVER* pServer = &m_pServer[i];
		INT nShmId = i + emKGUARDSERVER_FIRSTID;
		if (!pServer->cStream.Initialize(nShmId, 200 * 1024, TRUE))
		{
			ASSERT(FALSE);
			break;
		}

		m_pGuard->Log("Initialize shmid:%d, serverid:%d OK", nShmId, nServerId);
		pServer->wServer = (WORD)nServerId;

		// 询问进程ID
		QueryProcessId(pServer);
	}
}

void KMonitor::Process_ServerPid(char* pData, size_t nLen)
{
	ASSERT(pData && nLen > 0);
	KProtocolHead* pHead = (KProtocolHead*)pData;
	if (nLen != sizeof(KProtocolHead) ||
		pHead->wServer == 0)
	{
		m_pGuard->Log("Receive invalid e2g_serverpid_def protocol!");
		return;
	}
	KSERVER* pServer = FindServer(pHead->wServer);
	if (!pServer)
	{
		m_pGuard->Log(
			"Failed to process e2g_serverpid_def protocol: server[%u] is not found!",
			pHead->wServer);
		return;
	}
	ASSERT(pServer->wServer == pHead->wServer);
	if (pServer->nPid > 0)
	{
		// 验证进程是否存在
		if (!FindProcess(pServer->nPid))
		{
			m_pGuard->Log("Process[%d] is not found", pServer->nPid);
			pServer->nPid = -1;
		}
	}
	g2e_serverpid sRet;
	sRet.wProtocol = g2e_serverpid_def;
	sRet.wServer   = pServer->wServer;
	sRet.nPid	   = pServer->nPid;
	m_pSender->SendPackToServer((unsigned char *)&sRet, sizeof(sRet));
	m_pGuard->Log("Report pid of server[%u]: %d", pServer->wServer, pServer->nPid);
}

int KMonitor::ProcessNetMessage(unsigned char* pData, unsigned int nLen)
{
	ASSERT(pData && nLen > 0);
	int nRet = guard_err;
	do 
	{
		if (nLen < sizeof(KProtocolHead))
		{
			m_pGuard->Log("Receive invalid protocol from web, size:%u", nLen);
			break;
		}
		KProtocolHead* pHead = (KProtocolHead*)pData;
		if (pHead->wProtocol == e2g_serverid_def)
		{
			Process_ServerId((char*)pData, nLen);
			break;
		}
		else if (pHead->wProtocol == e2g_serverpid_def)
		{
			Process_ServerPid((char*)pData, nLen);
			break;
		}
		KSERVER* pServer = FindServer(pHead->wServer);
		if (!pServer)
		{
			m_pGuard->Log("Receive protocol[%u:%u] for unknown server[%u]",
				pHead->wProtocol,
				nLen,
				pHead->wServer);
			break;
		}
		if (pHead->wProtocol == e2l_exit_def)
		{
			m_pGuard->Log("Server[%u] is exiting...", pHead->wServer);
		}
		pServer->cStream.PutPack((CHAR*)pData, nLen);
		nRet = fseye_success;
	}
	while(0);
	return nRet;
}

int KMonitor::Disconnect()
{
	return fseye_success;
}

int KMonitor::GetDescribe(char* pszDes, int nSize)
{
	ASSERT(pszDes && nSize > 0);
	strncpy(pszDes, "Monitor Lord etc..", nSize - 1);
	pszDes[nSize - 1] = '\0';
	return fseye_success;
}

int KMonitor::GetGUID(char* pszGuid, int nSize)
{
	ASSERT(pszGuid && nSize > 0);
	strncpy(pszGuid, "{5515BAE7-A3F6-423f-BFE6-B861FCDAA1CD}", nSize - 1);
	pszGuid[nSize - 1] = '\0';
	return fseye_success;
}

void KMonitor::QueryProcessId(KSERVER* pServer)
{
	ASSERT(pServer && pServer->nPid <= 0);
	KSubProtocolHead sReq;
	sReq.wProtocol = g2l_header_def;
	sReq.wServer = pServer->wServer;
	sReq.wSubProtocol = emKPROTOCOL_G2L_PIDREQ;
	pServer->cStream.PutPack(&sReq, sizeof(sReq));
	m_pGuard->Log("Query pid of server[%d]", pServer->wServer);
}

int KMonitor::Breathe()
{
	ASSERT(m_pServer);
	for (INT i = 0; i < emKGUARDSERVER_COUNT; ++i)
	{
		KSERVER* pServer = &m_pServer[i];
		if (pServer->wServer == 0)
			continue;
		size_t nLen = 0;
		CHAR* pData = pServer->cStream.GetPack(nLen);
		while (pData)
		{
			if (nLen < sizeof(KProtocolHead))
			{
				m_pGuard->Log("Receive invalid protocol from server[%u], size:%u", pServer->wServer, nLen);
				ASSERT(FALSE);
				break;
			}
			if (pServer->nPid <= 0)
			{
				// 查询进程ID
				QueryProcessId(pServer);
			}
			KProtocolHead* pHead = (KProtocolHead*)pData;
			if (pHead->wServer == (WORD)(-1))
			{
				// DEBUG
				printf("Server[%u] transfer %u %u\n", pServer->wServer, pHead->wProtocol, nLen);
				// 转发给其他插件
				m_pGuard->PutMessage(pData, nLen);
			}
			else if (pHead->wProtocol == l2g_header_def)
			{
				if (nLen >= sizeof(KSubProtocolHead))
				{
					ProcessServerMessages(pServer, pData, nLen);
				}
				else
				{
					ASSERT(FALSE);
				}
			}
			else if (pHead->wProtocol == l2e_send_gm_message_def)
			{
				// DEBUG
				printf("Server[%u] transfer %u %u\n", pServer->wServer, pHead->wProtocol, nLen);
				// 转发给其他插件
				m_pGuard->PutMessage(pData, nLen);
			}
			else
			{
				// DEBUG
				printf("Server[%u] send %u %u\n", pServer->wServer, pHead->wProtocol, nLen);
				pHead->wServer = pServer->wServer;
				m_pSender->SendPackToServer((unsigned char *)pData, nLen);
			}
			pServer->cStream.PopPack();
			pData = pServer->cStream.GetPack(nLen);
		}
	}
	return fseye_success;
}

int KMonitor::GetVersion(int& nMaxVer, int& nMinVer)
{
	nMaxVer = EXESQLMAXVER;
	nMinVer = EXESQLMINVER;
	return fseye_success;
}

int KMonitor::GetAuthor(char* pszAuthor, int nSize)
{
	ASSERT(pszAuthor && nSize > 0);
	strncpy(pszAuthor, "wangbin", nSize - 1);
	pszAuthor[nSize - 1] = '\0';
	return fseye_success;
}

bool KMonitor::CheckProtocol(int nProtocol)
{
	return
		nProtocol == e2l_header_def ||
		nProtocol == e2l_exit_def ||
		nProtocol == c2r_gmcmd_def||
		nProtocol == e2l_getpid_def ||
		nProtocol == e2g_serverid_def ||
		nProtocol == e2g_serverpid_def;
}

void KMonitor::ProcessServerMessages(KSERVER* pServer, char* pData, size_t nLen)
{
	ASSERT(pData && nLen > 0);
	KSubProtocolHead* pHead = (KSubProtocolHead*)pData;
	switch (pHead->wSubProtocol)
	{
	case emKPROTOCOL_L2G_PIDRET:
		if (nLen == sizeof(KPROTOCOL_L2G_PIDRET))
		{
			KPROTOCOL_L2G_PIDRET* pProto = (KPROTOCOL_L2G_PIDRET*)pHead;
			if (pProto->nPid <= 0)
			{
				ASSERT(FALSE);
				break;
			}
			pServer->nPid = pProto->nPid;
			m_pGuard->Log("Set pid of server[%u]: %d", pServer->wServer, pProto->nPid);
		}
		break;
	}
}

BOOL KMonitor::FindProcess(int nPid)
{
	BOOL bFound = FALSE;
#ifdef WIN32
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, nPid);
	if (hProcess)
	{
		CloseHandle(hProcess);
		bFound = TRUE;
	}
#else
	char szPath[256];
	sprintf(szPath, "/proc/%d", nPid);
	struct stat sInf;
	if (stat(szPath, &sInf) == 0)
	{
		bFound = TRUE;
	}
#endif
	return bFound;
}

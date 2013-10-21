//*********************************************************************
// file		: kclient.cpp
// author	: wangbin
// datetime	: 2007-12-13 16:43
// comment	: IClient接口实现
//*********************************************************************
#include "stdafx.h"
#include "kclient.h"
#include "GameMasterProtocol.h"

KClient::KClient(int nMaxConnectionCount) : m_nMaxConnCount(nMaxConnectionCount), m_pfnCallBack(NULL), m_pCallParam(NULL)
{
	ASSERT(nMaxConnectionCount > 0);
	m_ppConnections = new IKG_SocketStream*[nMaxConnectionCount];
	for (int i = 0; i < nMaxConnectionCount; ++i)
	{
		m_listFreeLink.push_back(i);
		m_ppConnections[i] = NULL;
	}
}

KClient::~KClient()
{
	ASSERT(m_listFreeLink.size() == m_nMaxConnCount);
	delete[] m_ppConnections;
	m_ppConnections = NULL;
}

int KClient::Startup()
{
	return 1;
}

int KClient::Cleanup()
{
	return 1;
}

int KClient::Shutdown()
{
	for (int i = 0; i < m_nMaxConnCount; ++i)
	{
		IKG_SocketStream* pConnection = m_ppConnections[i];
		if (pConnection)
		{
			OnConnectionClose((unsigned int)i);
			pConnection->Release();
			m_ppConnections[i] = NULL;
		}
	}
	return 0;
}

int KClient::Release()
{
	delete this;
	return 0;
}

void KClient::Bind(const char* pszIp)
{
	ASSERT(pszIp);
	m_cConnector.Bind(pszIp);
}

int KClient::ConnectTo(char *pszIp, unsigned short uPort)
{
	int nRet = INVALID_VALUE;
	do 
	{
		if (m_listFreeLink.empty())
		{
			ASSERT(FALSE);
			break;
		}
		IKG_SocketStream* pConnection = m_cConnector.Connect(pszIp, uPort);
		printf("connect to %s:%d...", pszIp, uPort);
		if (!pConnection)
		{
			//ASSERT(FALSE);
			printf("failed! \n");
			break;
		}
		else
		{
			printf("OK\n");
		}
		unsigned int uLinkId = m_listFreeLink.front();
		m_listFreeLink.pop_front();
		ASSERT((int)uLinkId < m_nMaxConnCount && m_ppConnections[uLinkId] == NULL);
		m_ppConnections[uLinkId] = pConnection;
		if (m_pfnCallBack)
		{
			m_pfnCallBack(m_pCallParam, uLinkId, enumClientConnectCreate);
		}
		nRet = uLinkId;
	}
	while (0);
	return nRet;
}


int KClient::Disconnect(unsigned int uLinkId)
{
	int nRet = INVALID_VALUE;
	do 
	{
		if (uLinkId >= (unsigned int)m_nMaxConnCount)
		{
			ASSERT(FALSE);
			break;
		}
		IKG_SocketStream* pConnection = m_ppConnections[uLinkId];
		if (pConnection == NULL)
		{
			//ASSERT(FALSE);
			break;
		}
		OnConnectionClose(uLinkId);
		pConnection->Release();
		nRet = fseye_success;
	}
	while(0);
	return nRet;
}

void KClient::RegisterMsgFilter(void *pParam, CALLBACK_CLIENT_EVENT pfnEventNotify)
{
	m_pCallParam  = pParam;
	m_pfnCallBack = pfnEventNotify;
}

int KClient::SendPackToServer(unsigned int uLinkId, const void *pData, unsigned int uLen)
{
	ASSERT(pData && uLen);
	int nRet = guard_err;
	do 
	{
		if (uLinkId >= (unsigned int)m_nMaxConnCount)
		{
			ASSERT(FALSE);
			break;
		}
		IKG_SocketStream* pConnection = m_ppConnections[uLinkId];
		if (!pConnection)
		{
		//	ASSERT(FALSE);
			break;
		}
		IKG_Buffer *pBuffer = KG_MemoryCreateBuffer(uLen);
		if (!pBuffer)
		{
			ASSERT(FALSE);
			break;
		}
		memcpy(pBuffer->GetData(), pData, uLen);
		INT nResult = pConnection->Send(pBuffer);
		pBuffer->Release();
		if (nResult != 1)
		{
			OnConnectionClose(uLinkId);
			pConnection->Release();
			break;
		}
		nRet = fseye_success;
	}
	while(0);
	return nRet;
}

const void *KClient::GetPackFromServer(unsigned int uLinkId, unsigned int &uLen)
{
	void* pRet = NULL;
	uLen = 0;
	do 
	{
		if (uLinkId >= (unsigned int)m_nMaxConnCount)
		{
			ASSERT(FALSE);
			break;
		}
		IKG_SocketStream* pConnection = m_ppConnections[uLinkId];
		//ASSERT(pConnection);
		if (!pConnection)
			return NULL;
		timeval sTimeOut = {0, 0};
		INT nResult = pConnection->CheckCanRecv(&sTimeOut);
		if (nResult == 0)
		{
			break;
		}
		else if (nResult == -1)
		{
			// 断线
			OnConnectionClose(uLinkId);
			pConnection->Release();
			break;
		}
		IKG_Buffer *pBuffer = NULL;
		nResult = pConnection->Recv(&pBuffer);
		if (nResult <= 0)
		{
			// 断线
			OnConnectionClose(uLinkId);
			pConnection->Release();
			break;
		}
		LPVOID pData = pBuffer->GetData();
		size_t nLen = pBuffer->GetSize();
		if (nLen > sizeof(m_szBuffer))
		{
			ASSERT(FALSE);
			pBuffer->Release();
			break;
		}
		memcpy(m_szBuffer, pData, nLen);
		uLen = (UINT)nLen;
		pBuffer->Release();
		pRet = m_szBuffer;
	}
	while(0);
	return pRet;
}

int KClient::FlushData()
{
	return 0;
}

int KClient::FlushData(unsigned int uLinkId)
{
	return 0;
}

bool KClient::IsAllRecvKey()
{
	return false;
}

bool KClient::IsRecvKey(unsigned int uLinkId)
{
	return false;
}

BOOL KClient::CreateClient(int nMaxConnectionCount, IClient** ppClient)
{
	ASSERT(nMaxConnectionCount > 0 && ppClient != NULL);
	KClient* pClient = new KClient(nMaxConnectionCount);
	if (pClient)
	{
		*ppClient = pClient;
	}
	return pClient != NULL;
}

void KClient::OnConnectionClose(unsigned int uLinkId)
{
	printf("disconnect \n");
	ASSERT(uLinkId < (unsigned int)m_nMaxConnCount);
	m_listFreeLink.push_back(uLinkId);
	m_ppConnections[uLinkId] = NULL;
	if (m_pfnCallBack)
	{
		m_pfnCallBack(m_pCallParam, uLinkId, enumClientConnectClose);
	}
}

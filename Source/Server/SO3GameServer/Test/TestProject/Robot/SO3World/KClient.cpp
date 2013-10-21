#include "stdafx.h"

#include <conio.h>
#include <time.h>
#include "KClient.h"

KClient::KClient(void)
{
	m_piSocketStream = NULL;
	memset(m_ProcessProtocolFuns, 0, sizeof(m_ProcessProtocolFuns));
}

KClient::~KClient(void)
{
	Disconnect();
}

BOOL KClient::Init(const char* pszIPAddr, int nPort)
{
	BOOL bRetCode = FALSE;
	KG_SocketConnector SocketeConnector;

	KGLOG_PROCESS_ERROR(pszIPAddr);

	strncpy(m_szIPAddr, pszIPAddr, 16);
	m_szIPAddr[15] = 0;
	m_nPort = nPort;

	m_piSocketStream = SocketeConnector.Connect(pszIPAddr, nPort);
	KGLOG_PROCESS_ERROR(m_piSocketStream);

	struct timeval TimeVal;
	TimeVal.tv_sec  = 0;
	TimeVal.tv_usec = 0;

	bRetCode = m_piSocketStream->SetTimeout(&TimeVal);
	KGLOG_PROCESS_ERROR(bRetCode);

	OnConnected();

    return TRUE;

Exit0:
    KG_COM_RELEASE(m_piSocketStream);
    
	return FALSE;
}

void KClient::Breathe()
{
	int nRetCode = false;
	const struct timeval TimeVal = {0, 0};

	nRetCode = 1;
	while (nRetCode == 1 && m_piSocketStream)
	{
		nRetCode = m_piSocketStream->CheckCanRecv(&TimeVal);
		if (nRetCode == -1)
		{
			//断线处理
			OnDisconnect();
			KG_COM_RELEASE(m_piSocketStream);
			KGLogPrintf(KGLOG_ERR, "Disconnected to remote server.\n");
			
			return;
		}
		else if (nRetCode == 1)
		{
			IKG_Buffer* pBuffer = NULL;
			nRetCode = m_piSocketStream->Recv(&pBuffer);
			if (nRetCode == -1)
			{
				//断线处理
				OnDisconnect();
				KG_COM_RELEASE(pBuffer);
				KG_COM_RELEASE(m_piSocketStream);
				KGLogPrintf(KGLOG_ERR, "Disconnected to remote server.");

				return;
			}
			else if (nRetCode == 1)
			{
				INTERNAL_PROTOCOL_HEADER* pHeader = (INTERNAL_PROTOCOL_HEADER*)pBuffer->GetData();
				if (pHeader->byProtocolFamily >= MAX_PROTOCOL_FAMILY_NUM ||
					pHeader->wProtocolID >= MAX_PROTOCOL_ID_NUM ||
					m_ProcessProtocolFuns[(pHeader->byProtocolFamily << 8) + pHeader->wProtocolID] == NULL)
				{
					//错误的协议号，内网就不断连接了
					KGLogPrintf(KGLOG_ERR, "Received an error packet (%d - %d)", 
						pHeader->byProtocolFamily, pHeader->wProtocolID);
				}
				else
				{
					//正确的协议号，调用相应的处理函数
					(this->*m_ProcessProtocolFuns[(pHeader->byProtocolFamily << 8) + pHeader->wProtocolID])\
						(pBuffer);
				}

			}
			KG_COM_RELEASE(pBuffer);
		}
	}

	return;
}

BOOL KClient::Send(IKG_Buffer* pBuffer)
{
	BOOL bRetCode = FALSE;

	KGLOG_PROCESS_ERROR(pBuffer);
	KGLOG_PROCESS_ERROR(m_piSocketStream);

	bRetCode = m_piSocketStream->Send(pBuffer);
	KGLOG_PROCESS_ERROR(bRetCode);

	return TRUE;

Exit0:

	return FALSE;
}

BOOL KClient::Send(void* pvData, size_t uSize)
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    IKG_Buffer* piBuffer    = NULL;
    void*       pvBuffer    = NULL;

    piBuffer = KG_MemoryCreateBuffer((unsigned)uSize);
    KGLOG_PROCESS_ERROR(piBuffer);

    pvBuffer = piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pvBuffer);

    memcpy(pvBuffer, pvData, uSize);

    bRetCode = m_piSocketStream->Send(piBuffer);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KClient::Disconnect()
{
	KG_COM_RELEASE(m_piSocketStream);
	OnDisconnect();
	
	return TRUE;
}

//连接事件响应
void KClient::OnConnected()
{

	return;
}

void KClient::OnDisconnect()
{

	return;
}


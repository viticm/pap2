#include "StdAfx.h"
#include "KSimulateSocket.h"
#include "TestDef.h"
#include "Common/KG_Memory.h"
#include "Common/KG_Socket.h"
#include "Utility.h"
#include <algorithm>

KSimulateSocket::KSimulateSocket(void) : m_piSocketStream(NULL), m_nExitFlag(false), m_TimeOut(3), m_dwPingCys(2)
{

}


KSimulateSocket::~KSimulateSocket(void)
{
	UnInit();
}

struct AddRefference
{
	void operator()(IUnknown* pCom)
	{
		pCom->AddRef();
	}
};

BOOL KSimulateSocket::Connect(char szIP[], int nPort, DWORD dwPingTime)
{
	int nResult = false;
	int nRetCode = false;
	timeval              TimeVal		 = {1, 0};
	KG_SocketConnector  Connector;

	KG_PROCESS_ERROR(m_piSocketStream == NULL);

	m_piSocketStream = Connector.Connect(szIP, nPort);
	KG_PROCESS_ERROR(m_piSocketStream);

	m_piSocketStream->SetTimeout(&TimeVal);

	m_dwPingCys = dwPingTime;

	nRetCode = m_Thread.Create(WorkFucntion, this);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

void KSimulateSocket::UnInit()
{
	m_nExitFlag = true;
	m_Thread.Destroy();
	std::map<int, std::list<IKG_Buffer*>>::iterator Iter = m_ListenData.begin();
	for (Iter; Iter != m_ListenData.end(); ++Iter)
	{
		std::list<IKG_Buffer*>::iterator ListIter = Iter->second.begin();
		for (ListIter; ListIter != Iter->second.end(); ++ListIter)
		{
			KG_COM_RELEASE(*ListIter);
		}
	}
	KG_COM_RELEASE(m_piSocketStream);
}

IKG_Buffer* KSimulateSocket::ReceiveDataOnce(DWORD wProtocolID)
{
	std::list<IKG_Buffer*>& List = ReceiveData(wProtocolID);
	if (List.empty())
	{
		return NULL;
	}
	std::list<IKG_Buffer*>::iterator Iter = List.begin();
	++Iter;
	for (Iter; Iter != List.end(); ++Iter)
	{
		KG_COM_RELEASE(*Iter);
	}
	return *List.begin();
}

IKG_Buffer* KSimulateSocket::ReceiveDataOnceTime(DWORD wProtocolID, time_t uTime)
{
	std::list<IKG_Buffer*>& List = ReceiveDataTime(wProtocolID, uTime);
	if (List.empty())
	{
		return NULL;
	}
	std::list<IKG_Buffer*>::iterator Iter = List.begin();
	++Iter;
	for (Iter; Iter != List.end(); ++Iter)
	{
		KG_COM_RELEASE(*Iter);
	}
	return *List.begin();
}

void KSimulateSocket::WorkFucntion(void *pvParam)
{
	KSimulateSocket* pBishopSimulator = (KSimulateSocket*)pvParam;
	pBishopSimulator->ThreadFunction();
}

void KSimulateSocket::ThreadFunction()
{
	BOOL                 bResult         = false;
	int                  nRetCode        = false;
	timeval              TimeoutValue    = {1, 0};
	IKG_Buffer*          piBuffer        = NULL;
	int                  nReceiveCounter = 0;
	size_t               uBufferSize     = 0;
	DWORD dwLastTime = time(NULL);
	DWORD dwTimeNow = dwLastTime;
	int nIndex;

	while (!m_nExitFlag)
	{
		KGLOG_PROCESS_ERROR(m_piSocketStream);
		dwTimeNow = time(NULL);
		if ((dwTimeNow - dwLastTime) > (m_dwPingCys - 20))
		{
			dwLastTime = dwTimeNow;
			if (!DoPingSignal())
				break;
		}

		nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
		KGLOG_PROCESS_ERROR(nRetCode != -1);

		if (nRetCode != 1)
			continue;

		nRetCode = m_piSocketStream->Recv(&piBuffer);
		KGLOG_PROCESS_ERROR(nRetCode == 1);

		ASSERT(piBuffer);
		nIndex  = GetProtocolIndex(piBuffer);
		if (nIndex > 0)
		{
			MutextHelper mh(&m_Mutex);
			if (m_ListenData.find(nIndex) != m_ListenData.end())
			{
				piBuffer->AddRef();
				m_ListenData[nIndex].push_back(piBuffer);
			}
		}
		KG_COM_RELEASE(piBuffer);
	}

Exit0:    
	KG_COM_RELEASE(piBuffer);
	return ;
}


std::list<IKG_Buffer*> KSimulateSocket::ReceiveData(DWORD wProtocolID)
{
	SCOPEPROTOCOL(this, wProtocolID);
	std::list<IKG_Buffer*> List;
	time_t TimeNow = time(NULL);
	while (time(NULL) - TimeNow < m_TimeOut)
	{
		{
			MutextHelper mh(&m_Mutex);
			std::map<int, std::list<IKG_Buffer*>>::iterator Iter = m_ListenData.find(wProtocolID);
			if (Iter != m_ListenData.end())
			{
				if (!Iter->second.empty())
				{
					List = Iter->second;
					for_each(List.begin(), List.end(), AddRefference());
					break;
				}
			}
		}
		KGThread_Sleep(20);
	}
	return List;
}

void KSimulateSocket::AddListenProtocol(DWORD wProtocolID)
{
	MutextHelper mh(&m_Mutex);
	m_ListenData.insert(std::make_pair(wProtocolID, std::list<IKG_Buffer*>()));
}

void KSimulateSocket::RemoveListenProtocol(DWORD wProtocolID)
{
	MutextHelper mh(&m_Mutex);
	std::map<int, std::list<IKG_Buffer*>>::iterator Iter = m_ListenData.find(wProtocolID);
	if (Iter != m_ListenData.end())
	{
		std::list<IKG_Buffer*>::iterator lIter = Iter->second.begin();
		for (lIter; lIter != Iter->second.end(); ++lIter)
		{
			KG_COM_RELEASE(*lIter);
		}
		m_ListenData.erase(Iter);
	}
}
bool KSimulateSocket::Send(void *pData, size_t uLength)
{
	if (uLength > 0 && pData)
	{
		BufferScopedPrt buffer(KG_MemoryCreateBuffer(uLength));
		if (buffer.get())
		{
			memcpy(buffer->GetData(), pData, uLength);
			return Send(buffer.get());
		}
	}
	return false;
}
bool KSimulateSocket::Send(IKG_Buffer* pBuffer)
{
	if (m_piSocketStream)
	{
		int nRetCode  = m_piSocketStream->Send(pBuffer);
		if (nRetCode == -1)
		{
			KGLogPrintf(KGLOG_INFO, "socket error %d", m_piSocketStream->GetLastError());
		}
		return nRetCode == 1;
	}
	return false;
}

std::list<IKG_Buffer*> KSimulateSocket::ReceiveDataTime(DWORD wProtocolID, time_t uTime)
{
	SCOPEPROTOCOL(this, wProtocolID);
	std::list<IKG_Buffer*> List;
	time_t TimeNow = time(NULL);
	while (time(NULL) - TimeNow < uTime)
	{
		KGThread_Sleep(20);
	}
	{
		MutextHelper mh(&m_Mutex);
		std::map<int, std::list<IKG_Buffer*>>::iterator Iter = m_ListenData.find(wProtocolID);
		if (Iter != m_ListenData.end())
		{
			List = Iter->second;
			std::for_each(List.begin(), List.end(), AddRefference());
		}
	}
	return List;
}

std::list<IKG_Buffer*> KSimulateSocket::GetProtocolData(DWORD dwProtocol)
{
	MutextHelper mh(&m_Mutex);
	std::list<IKG_Buffer*> List;
	
	std::map<int, std::list<IKG_Buffer*>>::iterator Iter = m_ListenData.find(dwProtocol);
	if (Iter != m_ListenData.end())
	{
		List = Iter->second;
//		Iter->second.clear();
	}
	return List;
}

IKG_Buffer* KSimulateSocket::GetProtocolDataOnce(DWORD dwProtocol)
{
	MutextHelper mh(&m_Mutex);
	IKG_Buffer* pBuff = NULL;
	std::map<int, std::list<IKG_Buffer*>>::iterator Iter = m_ListenData.find(dwProtocol);
	if (Iter != m_ListenData.end())
	{
		if (!Iter->second.empty())
		{
			pBuff = *Iter->second.begin();
//			Iter->second.pop_front();
		}
	}
	return pBuff;
}
#pragma once
#include <map>
#include <list>

#define CONCAT_TOKEN_(foo, bar) CONCAT_TOKEN_IMPL_(foo, bar)
#define CONCAT_TOKEN_IMPL_(foo, bar) foo##bar

#define  SCOPEPROTOCOL(pSocket, ProtocolID)\
	ScopeRegistProtocol CONCAT_TOKEN_(sp##ProtocolID, __LINE__)(pSocket, ProtocolID)

class IKG_SocketStream;
class IKG_Buffer;

class KSimulateSocket
{
public:
	KSimulateSocket(void);
	~KSimulateSocket(void);
	void UnInit();
	virtual bool DoPingSignal() = 0;
	BOOL Connect(char szIP[], int nPort, DWORD dwPingTime);
	void AddListenProtocol(DWORD wProtocolID);
	std::list<IKG_Buffer*> ReceiveData(DWORD wProtocolID);
	std::list<IKG_Buffer*> ReceiveDataTime(DWORD wProtocolID, time_t uTime);
	IKG_Buffer* ReceiveDataOnceTime(DWORD wProtocolID, time_t uTime);
	IKG_Buffer* ReceiveDataOnce(DWORD wProtocolID);
	void TimeOut(int nTime) {m_TimeOut = nTime;}
	void RemoveListenProtocol(DWORD wProtocolID);
	bool Send(IKG_Buffer* pBuffer);
	bool Send(void* pData, size_t uLength);
	std::list<IKG_Buffer*> GetProtocolData(DWORD dwProtocol);

	IKG_Buffer* GetProtocolDataOnce(DWORD dwProtocol);
	template<class T>
	bool Send(T& Value);
	
	template <class T, int nSize>
	bool Send(T (&Value)[nSize]);

protected:
	virtual int GetProtocolIndex(IKG_Buffer* pBuffer) = 0;
	static void WorkFucntion(void *pvParam);
	void ThreadFunction();

protected:
	IKG_SocketStream*       m_piSocketStream;
	DWORD                   m_dwPingCys;
	KThread                 m_Thread;
	bool                    m_nExitFlag;
	KMutex                  m_Mutex;
	std::map<int, std::list<IKG_Buffer*>> m_ListenData;
	time_t m_TimeOut;
};

struct ScopeRegistProtocol
{
	ScopeRegistProtocol(KSimulateSocket* pSocket, int nProtocol)
	{
		m_pSocket = pSocket;
		m_nProtocol = nProtocol;
		m_pSocket->AddListenProtocol(nProtocol);
	}
	~ScopeRegistProtocol()
	{
		m_pSocket->RemoveListenProtocol(m_nProtocol);
	}
	KSimulateSocket* m_pSocket;
	int m_nProtocol;
};

template<class T>
bool KSimulateSocket::Send(T& Value)
{
	return Send(&Value, sizeof(Value));
}

template <class T, int nSize>
bool KSimulateSocket::Send(T (&Value)[nSize])
{
	return Send((void*)Value, sizeof(T) * nSize);
}
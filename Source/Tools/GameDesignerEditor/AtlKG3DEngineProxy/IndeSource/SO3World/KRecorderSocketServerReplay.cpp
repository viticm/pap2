#include "stdafx.h"
#include "KSO3World.h"
#include "KRecorderSocketStreamReplay.h"
#include "KRecorderSocketServerReplay.h"
#include "KProtocolRecorderBase.h"

HRESULT STDMETHODCALLTYPE KRecorderSocketServerReplay::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSocketServerReplay::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSocketServerReplay::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

int KRecorderSocketServerReplay::Init(
    const char cszIPAddress[], int nPort,
    int nMaxAcceptEachWait,
    int nMaxRecvBufSizePerSocket,
    int nMaxSendBufSizePerSocket,
    ENCODE_DECODE_MODE EncodeDecodeMode, 
    void* pvContext
)
{
    int                 nResult         = false;
    int                 nRetCode        = false;
    KSERVER_INIT        ServerInit;

    assert(m_piReplayer);
    
    nRetCode = m_piReplayer->Pop(&ServerInit, sizeof(ServerInit));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(ServerInit.uType == ertServerInit);
        
    nResult = ServerInit.nOperationResult;
Exit0:
    return nResult;
}

void KRecorderSocketServerReplay::UnInit(void* pvContext)
{
}

int KRecorderSocketServerReplay::Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pnRetEventCount)
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    KG_SOCKET_EVENT*        pSocketEvent        = NULL;
    KG_SOCKET_EVENT*        pSocketEventEnd     = NULL;
    IKG_SocketStream*       piSocketStream      = NULL;
    MAP_SOCKET::iterator    it;
    KSERVER_WAIT            ServerWait;

    assert(m_piReplayer);
    
    nRetCode = m_piReplayer->Pop(&ServerWait, sizeof(ServerWait));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(ServerWait.uType == ertServerWait);
    
    *pnRetEventCount = ServerWait.wRetEventCount;

    KGLOG_PROCESS_ERROR(nEventCount >= ServerWait.wRetEventCount);

    nRetCode = m_piReplayer->Pop(pEvent, sizeof(KG_SOCKET_EVENT) * ServerWait.wRetEventCount);
    KGLOG_PROCESS_ERROR(nRetCode);    

    if (ServerWait.nOperationResult)
    {
        pSocketEventEnd = pEvent + ServerWait.wRetEventCount;
        for (pSocketEvent = pEvent; pSocketEvent < pSocketEventEnd; pSocketEvent++)
        {
            if (pSocketEvent->uEventFlag & KG_SOCKET_EVENT_ACCEPT)
            {
                piSocketStream = new KRecorderAsyncSocketStreamReplay(m_piReplayer);
                KGLOG_PROCESS_ERROR(piSocketStream);

                // 严格的说,需要在"真Socket"被释放的时候来解除映射关系
                m_SocketMap[pSocketEvent->piSocket] = piSocketStream;

                pSocketEvent->piSocket = piSocketStream;
                continue;
            }
            
            it = m_SocketMap.find(pSocketEvent->piSocket);
            KGLOG_PROCESS_ERROR(it != m_SocketMap.end());

            piSocketStream = it->second;

            pSocketEvent->piSocket = piSocketStream;
            pSocketEvent->piSocket->AddRef();
        }
    }

    nResult = ServerWait.nOperationResult;
Exit0:
    return nResult;
}

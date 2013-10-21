#include "stdafx.h"
#include "KSO3World.h"
#include "KProtocolRecorderBase.h"
#include "KRecorderSocketServerRecord.h"
#include "KRecorderSocketStreamRecord.h"

HRESULT STDMETHODCALLTYPE KRecorderSocketServerRecord::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSocketServerRecord::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSocketServerRecord::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

int KRecorderSocketServerRecord::Init(
    const char cszIPAddress[], int nPort,
    int nMaxAcceptEachWait,
    int nMaxRecvBufSizePerSocket,
    int nMaxSendBufSizePerSocket,
    ENCODE_DECODE_MODE EncodeDecodeMode, 
    void *pvContext
)
{
    int                 nRetCode            = false;
    int                 nOperationResult    = false;
    KSERVER_INIT        ServerInit;
    
    assert(m_piRecorder);
    assert(g_pSO3World);

    nOperationResult = m_SocketServer.Init(
        cszIPAddress, nPort, nMaxAcceptEachWait, nMaxRecvBufSizePerSocket, 
        nMaxSendBufSizePerSocket, EncodeDecodeMode, pvContext
    ); // return true or false
    
    ServerInit.uType            = ertServerInit;
    ServerInit.nOperationResult = (int8_t)nOperationResult;

    m_piRecorder->Push(&ServerInit, sizeof(ServerInit));
    
Exit0:
    return nOperationResult;
}

void KRecorderSocketServerRecord::UnInit(void* pvContext)
{
    m_SocketServer.UnInit(pvContext);
}

int KRecorderSocketServerRecord::Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pnRetEventCount)
{
    int                             nOperationResult    = false;
    int                             nRetEventCount      = 0;
    KG_SOCKET_EVENT*                pSocketEvent        = NULL;
    KG_SOCKET_EVENT*                pSocketEventEnd     = NULL;
    IKG_SocketStream*               piSocketStream      = NULL;
    KSERVER_WAIT                    ServerWait;

    assert(pEvent);
    assert(pnRetEventCount);
    assert(g_pSO3World);

    nOperationResult = m_SocketServer.Wait(nEventCount, pEvent, &nRetEventCount);
    // return true or false

    *pnRetEventCount = nRetEventCount;
    
    if (nOperationResult)
    {
        pSocketEventEnd = pEvent + nRetEventCount;
        for (pSocketEvent = pEvent; pSocketEvent < pSocketEventEnd; pSocketEvent++)
        {
            if (pSocketEvent->uEventFlag & KG_SOCKET_EVENT_ACCEPT)
            {
                piSocketStream = new KRecorderAsyncSocketStreamRecord(m_piRecorder, pSocketEvent->piSocket);
                KGLOG_PROCESS_ERROR(piSocketStream);

                pSocketEvent->piSocket->SetUserData(piSocketStream);

                KG_COM_RELEASE(pSocketEvent->piSocket);
                pSocketEvent->piSocket = piSocketStream;
                continue;
            }
            
            piSocketStream = (IKG_SocketStream*)(pSocketEvent->piSocket->GetUserData());
            KG_COM_RELEASE(pSocketEvent->piSocket);
            pSocketEvent->piSocket = piSocketStream;
            pSocketEvent->piSocket->AddRef();
        }
    }
    
    assert(nRetEventCount < USHRT_MAX);

    ServerWait.uType            = ertServerWait;
    ServerWait.nOperationResult = (int8_t)nOperationResult;
    ServerWait.wRetEventCount   = (WORD)nRetEventCount;

    m_piRecorder->Push(&ServerWait, sizeof(ServerWait));
    m_piRecorder->Push(pEvent, sizeof(KG_SOCKET_EVENT) * nRetEventCount);

Exit0:
    return nOperationResult;
}

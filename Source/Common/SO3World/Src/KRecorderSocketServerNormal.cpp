#include "stdafx.h"
#include "KRecorderSocketServerNormal.h"

HRESULT STDMETHODCALLTYPE KRecorderSocketServerNormal::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSocketServerNormal::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSocketServerNormal::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

int KRecorderSocketServerNormal::Init(
    const char cszIPAddress[], int nPort,
    int nMaxAcceptEachWait,
    int nMaxRecvBufSizePerSocket,
    int nMaxSendBufSizePerSocket,
    ENCODE_DECODE_MODE EncodeDecodeMode, 
    void *pvContext
)
{
    int nResult  = false;
    int nRetCode = false;

    nRetCode = m_SocketServer.Init(
        cszIPAddress, nPort, nMaxAcceptEachWait, nMaxRecvBufSizePerSocket, 
        nMaxSendBufSizePerSocket, EncodeDecodeMode, pvContext
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

void KRecorderSocketServerNormal::UnInit(void* pvContext)
{
    m_SocketServer.UnInit(pvContext);
}

int KRecorderSocketServerNormal::Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pnRetEventCount)
{
    int nResult  = false;
    int nRetCode = false;

    nRetCode = m_SocketServer.Wait(nEventCount, pEvent, pnRetEventCount);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

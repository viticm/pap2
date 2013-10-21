#include "stdafx.h"
#include "KSO3World.h"
#include "KRecorderSocketStreamReplay.h"
#include "KProtocolRecorderBase.h"

HRESULT STDMETHODCALLTYPE KRecorderSocketStreamReplay::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSocketStreamReplay::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSocketStreamReplay::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

int KRecorderSocketStreamReplay::SetTimeout(const timeval* pTimeout)
{
    int                 nResult         = false;
    int                 nRetCode        = false;
    KSOCKET_SET_TIMEOUT SocketSetTimeOut;

    assert(m_piReplayer);
    
    nRetCode = m_piReplayer->Pop(&SocketSetTimeOut, sizeof(SocketSetTimeOut));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(SocketSetTimeOut.uType == ertSocketSetTimeout);
        
    nResult = SocketSetTimeOut.nOperationResult;
Exit0:
    return nResult;
}

int KRecorderSocketStreamReplay::CheckCanSend(const timeval* pTimeout)
{
    return false;
}

int KRecorderSocketStreamReplay::Send(IKG_Buffer* piBuffer)
{
    int                 nResult         = false;
    int                 nRetCode        = false;
    KSOCKET_SEND        SocketSend;

    assert(m_piReplayer);
    
    nRetCode = m_piReplayer->Pop(&SocketSend, sizeof(SocketSend));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(SocketSend.uType == ertSocketSend);
        
    nResult = SocketSend.nOperationResult;
Exit0:
    return nResult;
}

int KRecorderSocketStreamReplay::CheckCanRecv(const timeval* pTimeout)
{
    int                     nResult         = false;
    int                     nRetCode        = false;
    KSOCKET_CHECK_CAN_RECV  SocketCheckCanRecv;

    assert(m_piReplayer);
    
    nRetCode = m_piReplayer->Pop(&SocketCheckCanRecv, sizeof(SocketCheckCanRecv));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(SocketCheckCanRecv.uType == ertSocketCheckCanRecv);
        
    nResult = SocketCheckCanRecv.nOperationResult;
Exit0:
    return nResult;
}

int KRecorderSocketStreamReplay::Recv(IKG_Buffer** ppiRetBuffer)
{
    int             nResult         = false;
    int             nRetCode        = false;
    IKG_Buffer*     piRecvBuffer    = NULL;
    void*           pvBuffer        = NULL;
    KSOCKET_RECV    SocketRecv;

    assert(m_piReplayer);
    
    nRetCode = m_piReplayer->Pop(&SocketRecv, sizeof(SocketRecv));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(SocketRecv.uType == ertSocketRecv);
    
    if (SocketRecv.nOperationResult == 1)
    {
        size_t uDataLen = SocketRecv.wRecvDataLen;

        KGLOG_PROCESS_ERROR(uDataLen > 0);

        piRecvBuffer = KG_MemoryCreateBuffer((unsigned)uDataLen);
        KGLOG_PROCESS_ERROR(piRecvBuffer);

        pvBuffer = piRecvBuffer->GetData();
        KGLOG_PROCESS_ERROR(pvBuffer);

        nRetCode = m_piReplayer->Pop(pvBuffer, uDataLen);
        KGLOG_PROCESS_ERROR(nRetCode);

        *ppiRetBuffer = piRecvBuffer;
        (*ppiRetBuffer)->AddRef();
    }
    
    nResult = SocketRecv.nOperationResult;
Exit0:
    KG_COM_RELEASE(piRecvBuffer);
    return nResult;
}

int KRecorderSocketStreamReplay::IsAlive()
{
    int                 nResult         = false;
    int                 nRetCode        = false;
    KSOCKET_IS_ALIVE    SocketIsAlive;

    assert(m_piReplayer);

    nRetCode = m_piReplayer->Pop(&SocketIsAlive, sizeof(SocketIsAlive));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(SocketIsAlive.uType == ertSocketIsAlive);

    nResult = SocketIsAlive.nOperationResult;
Exit0:
    return nResult;
}

int KRecorderSocketStreamReplay::GetRemoteAddress(struct in_addr*pRemoteIP, u_short* pusRemotePort)
{
    assert(false); //未实现
    return false;
}

int KRecorderSocketStreamReplay::SetUserData(void* pvUserData)
{
    assert(false); //未实现
    return false;
}

void* KRecorderSocketStreamReplay::GetUserData()
{
    assert(false); //未实现
    return false;
}

int KRecorderSocketStreamReplay::GetLastError()
{
    assert(false); //未实现
    return 0;
}

//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE KRecorderAsyncSocketStreamReplay::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderAsyncSocketStreamReplay::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderAsyncSocketStreamReplay::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

int KRecorderAsyncSocketStreamReplay::SetTimeout(const timeval* pTimeout)
{
    assert(false); //未实现
    return false;
}

int KRecorderAsyncSocketStreamReplay::CheckCanSend(const timeval* pTimeout)
{
    assert(false); //未实现
    return false;
}

int KRecorderAsyncSocketStreamReplay::Send(IKG_Buffer* piBuffer)
{
    int                 nResult         = false;
    int                 nRetCode        = false;
    unsigned            uSendDataLen    = 0;
    WORD                wSendDataLen    = 0;
    KSERVER_SEND        ServerSend;
    
    assert(piBuffer);
    assert(m_piReplayer);
    
    uSendDataLen = piBuffer->GetSize();

    nRetCode = m_piReplayer->Pop(&ServerSend, sizeof(ServerSend));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(ServerSend.uType == ertServerSend);
    
    assert(uSendDataLen == ServerSend.wSendDataLen); // 检查发送的数据包大小是否一致
    
    nResult = ServerSend.nOperationResult;
Exit0:
    return nResult;
}

int KRecorderAsyncSocketStreamReplay::CheckCanRecv(const timeval* pTimeout)
{
    assert(false); //未实现
    return false;
}

int KRecorderAsyncSocketStreamReplay::Recv(IKG_Buffer** ppiRetBuffer)
{
    int             nResult         = false;
    int             nRetCode        = false;
    void*           pvData          = NULL;
    size_t          uDataLen        = 0;
    IKG_Buffer*     piRecvData      = NULL;
    KSERVER_RECV    ServerRecv;

    assert(ppiRetBuffer);
    assert(m_piReplayer);

    nRetCode = m_piReplayer->Pop(&ServerRecv, sizeof(ServerRecv));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(ServerRecv.uType == ertServerRecv);
    
    if (ServerRecv.nOperationResult == 1)
    {
        assert(ServerRecv.wRecvDataLen);
        piRecvData = KG_MemoryCreateBuffer((unsigned)ServerRecv.wRecvDataLen);
        KGLOG_PROCESS_ERROR(piRecvData);

        pvData = piRecvData->GetData();
        KGLOG_PROCESS_ERROR(pvData);

        nRetCode = m_piReplayer->Pop(pvData, ServerRecv.wRecvDataLen);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        (*ppiRetBuffer) = piRecvData;
        (*ppiRetBuffer)->AddRef();
    }
    
    nResult = ServerRecv.nOperationResult;
Exit0:
    KG_COM_RELEASE(piRecvData);
    return nResult;
}

int KRecorderAsyncSocketStreamReplay::IsAlive()
{
    assert(false); //未实现
    return false;
}

int KRecorderAsyncSocketStreamReplay::GetRemoteAddress(struct in_addr* pRemoteIP, u_short* pusRemotePort)
{
    int                 nResult         = false;
    int                 nRetCode        = false;
    KSERVER_GET_ADDRESS ServerGetAddress;

    assert(pRemoteIP);
    assert(pusRemotePort);
    assert(m_piReplayer);

    nRetCode = m_piReplayer->Pop(&ServerGetAddress, sizeof(ServerGetAddress));
    KGLOG_PROCESS_ERROR(nRetCode);

    assert(ServerGetAddress.uType == ertServerGetAddress);
    
    if (ServerGetAddress.nOperationResult)
    {
        *pRemoteIP = ServerGetAddress.addr;
        *pusRemotePort = ServerGetAddress.uPort;
    }
    
    nResult = ServerGetAddress.nOperationResult;
Exit0:
    return nResult;
}

int KRecorderAsyncSocketStreamReplay::SetUserData(void* pvUserData)
{
    m_pvUserData = pvUserData;
    return true;
}

void* KRecorderAsyncSocketStreamReplay::GetUserData()
{
    return m_pvUserData;
}

int KRecorderAsyncSocketStreamReplay::GetLastError()
{
    assert(false); //未实现
    return 0;
}

#include "stdafx.h"
#include "KRecorderSocketStreamRecord.h"
#include "KProtocolRecorderBase.h"
#include "KSO3World.h"
HRESULT STDMETHODCALLTYPE KRecorderSocketStreamRecord::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSocketStreamRecord::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSocketStreamRecord::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

int KRecorderSocketStreamRecord::SetTimeout(const timeval* pTimeout)
{
    int                 nOperationResult    = false;
    KSOCKET_SET_TIMEOUT SocketSetTimeOut;

    assert(m_piSocketStream);
    assert(m_piRecorder);

    nOperationResult = m_piSocketStream->SetTimeout(pTimeout); // return value is : true
    
    SocketSetTimeOut.uType              = ertSocketSetTimeout;
    SocketSetTimeOut.nOperationResult   = (int8_t)nOperationResult;

    m_piRecorder->Push(&SocketSetTimeOut, sizeof(SocketSetTimeOut));

Exit0:
    return nOperationResult;
}

int KRecorderSocketStreamRecord::CheckCanSend(const timeval* pTimeout)
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->CheckCanSend(pTimeout);

    return nResult;
}

int KRecorderSocketStreamRecord::Send(IKG_Buffer* piBuffer)
{
    int                 nOperationResult    = false;
    KSOCKET_SEND        SocketSend;
    
    assert(piBuffer);
    assert(m_piRecorder);
    assert(m_piSocketStream);
    
    // return -1: error, 0: timeout, 1: success
    nOperationResult = m_piSocketStream->Send(piBuffer); 
    
    SocketSend.uType            = ertSocketSend;
    SocketSend.nOperationResult = (int8_t)nOperationResult;

    m_piRecorder->Push(&SocketSend, sizeof(SocketSend));

Exit0:
    return nOperationResult;
}

int KRecorderSocketStreamRecord::CheckCanRecv(const timeval* pTimeout)
{
    int                     nOperationResult    = false;
    KSOCKET_CHECK_CAN_RECV  SocketCheckCanRecv;
    
    assert(pTimeout);
    assert(m_piRecorder);
    assert(m_piSocketStream);
    
    // return -1: error, 0: timeout, 1: success
    nOperationResult = m_piSocketStream->CheckCanRecv(pTimeout);

    SocketCheckCanRecv.uType            = ertSocketCheckCanRecv;
    SocketCheckCanRecv.nOperationResult = (int8_t)nOperationResult;

    m_piRecorder->Push(&SocketCheckCanRecv, sizeof(SocketCheckCanRecv));

Exit0:
    return nOperationResult;
}

int KRecorderSocketStreamRecord::Recv(IKG_Buffer** ppiRetBuffer)
{
    int             nOperationResult    = false;
    void*           pvData              = NULL;
    size_t          uDataLen            = 0;
    KSOCKET_RECV    SocketRecv;

    assert(ppiRetBuffer);
    assert(m_piSocketStream);
    assert(m_piRecorder);
    
    nOperationResult = m_piSocketStream->Recv(ppiRetBuffer);
    if (nOperationResult == 1)
    {
        assert(*ppiRetBuffer);
        
        pvData = (*ppiRetBuffer)->GetData();
        KGLOG_PROCESS_ERROR(pvData);

        uDataLen = (size_t)(*ppiRetBuffer)->GetSize();
        assert(uDataLen < USHRT_MAX);
    }
        
    SocketRecv.uType            = ertSocketRecv;
    SocketRecv.nOperationResult = (int8_t)nOperationResult;
    SocketRecv.wRecvDataLen     = (WORD)uDataLen;

    m_piRecorder->Push(&SocketRecv, sizeof(SocketRecv));
    m_piRecorder->Push(pvData, uDataLen);

Exit0:
    return nOperationResult;
}

int KRecorderSocketStreamRecord::IsAlive()
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->IsAlive();

    return nResult;
}

int KRecorderSocketStreamRecord::GetRemoteAddress(struct in_addr* pRemoteIP, u_short* pusRemotePort)
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->GetRemoteAddress(pRemoteIP, pusRemotePort);

    return nResult;
}

int KRecorderSocketStreamRecord::SetUserData(void* pvUserData)
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->SetUserData(pvUserData);

    return nResult;
}

void* KRecorderSocketStreamRecord::GetUserData()
{
    void* pvResult  = NULL;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    pvResult = m_piSocketStream->GetUserData();

    return pvResult;
}

int KRecorderSocketStreamRecord::GetLastError()
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->GetLastError();

    return nResult;
}
//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE KRecorderAsyncSocketStreamRecord::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderAsyncSocketStreamRecord::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderAsyncSocketStreamRecord::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

int KRecorderAsyncSocketStreamRecord::SetTimeout(const timeval* pTimeout)
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->SetTimeout(pTimeout);

    return nResult;
}

int KRecorderAsyncSocketStreamRecord::CheckCanSend(const timeval* pTimeout)
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->CheckCanSend(pTimeout);

    return nResult;
}

int KRecorderAsyncSocketStreamRecord::Send(IKG_Buffer* piBuffer)
{
    int                 nOperationResult = false;
    unsigned            uSendDataLen     = 0;
    KSERVER_SEND        ServerSend;
    
    assert(piBuffer);
    assert(m_piSocketStream);
    assert(m_piRecorder);
    
    uSendDataLen = piBuffer->GetSize();
    assert(uSendDataLen < USHRT_MAX);
        
    // return -1: error, 0: timeout, 1: success
    nOperationResult = m_piSocketStream->Send(piBuffer);
    
    ServerSend.uType            = ertServerSend;
    ServerSend.nOperationResult = (int8_t)nOperationResult;
    ServerSend.wSendDataLen     = (WORD)uSendDataLen;
    
    m_piRecorder->Push(&ServerSend, sizeof(ServerSend));
    
Exit0:
    return nOperationResult;
}

int KRecorderAsyncSocketStreamRecord::CheckCanRecv(const timeval* pTimeout)
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->CheckCanRecv(pTimeout);

    return nResult;
}

int KRecorderAsyncSocketStreamRecord::Recv(IKG_Buffer** ppiRetBuffer)
{
    int             nOperationResult    = false;
    void*           pvData              = NULL;
    size_t          uDataLen            = 0;
    KSERVER_RECV    ServerRecv;

    assert(ppiRetBuffer);
    assert(m_piSocketStream);
    assert(m_piRecorder);
    
    //return -2: again, -1: error, 0: timeout, 1: success
    nOperationResult = m_piSocketStream->Recv(ppiRetBuffer);
    if (nOperationResult == 1)
    {
        assert(*ppiRetBuffer);
        
        pvData = (*ppiRetBuffer)->GetData();
        KGLOG_PROCESS_ERROR(pvData);

        uDataLen = (*ppiRetBuffer)->GetSize();
        assert(uDataLen < USHRT_MAX);
    }
    
    ServerRecv.uType            = ertServerRecv;
    ServerRecv.nOperationResult = (int8_t)nOperationResult;
    ServerRecv.wRecvDataLen     = (WORD)uDataLen;

    m_piRecorder->Push(&ServerRecv, sizeof(ServerRecv));
    m_piRecorder->Push(pvData, uDataLen);

Exit0:
    return nOperationResult;
}

int KRecorderAsyncSocketStreamRecord::IsAlive()
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->IsAlive();

    return nResult;
}

int KRecorderAsyncSocketStreamRecord::GetRemoteAddress(struct in_addr* pRemoteIP, u_short* pusRemotePort)
{
    int                 nOperationResult    = false;
    KSERVER_GET_ADDRESS ServerGetAddress;
    
    assert(pRemoteIP);
    assert(pusRemotePort);
    assert(m_piSocketStream);
    assert(m_piRecorder);
    
    // return true or false
    nOperationResult = m_piSocketStream->GetRemoteAddress(pRemoteIP, pusRemotePort);

    ServerGetAddress.uType              = ertServerGetAddress;
    ServerGetAddress.nOperationResult   = (int8_t)nOperationResult;
    ServerGetAddress.addr               = *pRemoteIP;
    ServerGetAddress.uPort              = *pusRemotePort;

    m_piRecorder->Push(&ServerGetAddress, sizeof(ServerGetAddress));
    
Exit0:
    return nOperationResult;
}

int KRecorderAsyncSocketStreamRecord::SetUserData(void* pvUserData)
{
    m_pvUserData = pvUserData;
    return true;
}

void* KRecorderAsyncSocketStreamRecord::GetUserData()
{
    return m_pvUserData;
}

int KRecorderAsyncSocketStreamRecord::GetLastError()
{
    int nResult  = false;

    assert(m_piSocketStream);
    assert(false); // 未实现
    
    nResult = m_piSocketStream->GetLastError();

    return nResult;
}
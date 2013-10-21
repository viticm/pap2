#include "stdafx.h"
#include "KSO3World.h"
#include "KProtocolRecorderBase.h"
#include "KRecorderSocketClientRecord.h"
#include "KRecorderSocketStreamRecord.h"

HRESULT STDMETHODCALLTYPE KRecorderSocketClientRecord::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSocketClientRecord::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSocketClientRecord::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

IKG_SocketStream* KRecorderSocketClientRecord::Connect(const char cszIP[], int nPort)
{
    IKG_SocketStream*   piResult            = NULL;
    IKG_SocketStream*   piSocket            = NULL;
    KG_SocketConnector  Connector;
    KSOCKET_CONNECTOR   SocketConnector;
    
    assert(cszIP);

    piSocket = Connector.Connect(cszIP, nPort);
    
    SocketConnector.uType    = ertSocketConnector;
    SocketConnector.piSocket = piSocket;

    m_piRecorder->Push(&SocketConnector, sizeof(SocketConnector));

    KGLOG_PROCESS_ERROR(piSocket);

    piResult = new KRecorderSocketStreamRecord(m_piRecorder, piSocket);
    
Exit0:
    KG_COM_RELEASE(piSocket);
    return piResult;
}

#include "stdafx.h"
#include "KSO3World.h"
#include "KRecorderSocketClientReplay.h"
#include "KRecorderSocketStreamReplay.h"
#include "KProtocolRecorderBase.h"

HRESULT STDMETHODCALLTYPE KRecorderSocketClientReplay::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSocketClientReplay::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSocketClientReplay::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

IKG_SocketStream* KRecorderSocketClientReplay::Connect(const char cszIP[], int nPort)
{
    IKG_SocketStream*   piResult        = NULL;
    BOOL                bRetCode        = false;
    IKG_SocketStream*   piRecordSocket  = NULL;
    KSOCKET_CONNECTOR   SocketConnector;

    bRetCode = m_piReplayer->Pop(&SocketConnector, sizeof(SocketConnector));
    KGLOG_PROCESS_ERROR(bRetCode);

    assert(SocketConnector.uType == ertSocketConnector);

    KG_PROCESS_ERROR(SocketConnector.piSocket);

    piResult = new KRecorderSocketStreamReplay(m_piReplayer);

Exit0:
    return piResult;
}

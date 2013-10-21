////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSocketServerReplay.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-31 10:28:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KRECORDERSOCKETSERVERREPLAY_H_
#define _INCLUDE_KRECORDERSOCKETSERVERREPLAY_H_
#include "KRecorderFactory.h"
#include "KProtocolReplayer.h"

class KRecorderSocketServerReplay : public ISocketServer
{
public:
    KRecorderSocketServerReplay(IProtocolReplayer* piReplayer) : m_ulRefCount(1)
    {
        assert(piReplayer);
        m_piReplayer = piReplayer;
        m_piReplayer->AddRef();
    }
    ~KRecorderSocketServerReplay()
    {
        KG_COM_RELEASE(m_piReplayer);
    }
    
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

public:
    int Init(
        const char cszIPAddress[], int nPort,
        int nMaxAcceptEachWait,
        int nMaxRecvBufSizePerSocket,
        int nMaxSendBufSizePerSocket,
        ENCODE_DECODE_MODE EncodeDecodeMode, 
        void* pvContext
    );

    void UnInit(void* pvContext);

    int Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pnRetEventCount);

private:
    ULONG               m_ulRefCount;
    IProtocolReplayer*  m_piReplayer;

    typedef std::map<void*, IKG_SocketStream*> MAP_SOCKET;
    MAP_SOCKET m_SocketMap;
};

#endif //_INCLUDE_KRECORDERSOCKETSERVERREPLAY_H_

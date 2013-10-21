////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSocketServerRecord.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-30 14:30:23
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KRECORDERSOCKETSERVERRECORD_H_
#define _INCLUDE_KRECORDERSOCKETSERVERRECORD_H_
#include "KRecorderFactory.h"
#include "Common/KG_Socket.h"
#include "KProtocolRecorder.h"
class KRecorderSocketServerRecord : public ISocketServer
{
public:
    KRecorderSocketServerRecord(IProtocolRecorder* piRecorder) : m_ulRefCount(1)
    {
        assert(piRecorder);
        m_piRecorder = piRecorder;
        m_piRecorder->AddRef();
    }

    ~KRecorderSocketServerRecord()
    {
        KG_COM_RELEASE(m_piRecorder);
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
    ULONG                   m_ulRefCount;
    IProtocolRecorder*      m_piRecorder;
    KG_SocketServerAcceptor m_SocketServer;
};

#endif //_INCLUDE_KRECORDERSOCKETSERVERRECORD_H_

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSocketServerNormal.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-29 17:29:43
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KRECORDERSOCKETSERVERNORMAL_H_
#define _KRECORDERSOCKETSERVERNORMAL_H_
#include "KRecorderFactory.h"
#include "Common/KG_Socket.h"

class KRecorderSocketServerNormal : public ISocketServer
{
public:
    KRecorderSocketServerNormal() : m_ulRefCount(1){}
    ~KRecorderSocketServerNormal(){}
    
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

public:
    virtual int Init(
        const char cszIPAddress[], int nPort,
        int nMaxAcceptEachWait,
        int nMaxRecvBufSizePerSocket,
        int nMaxSendBufSizePerSocket,
        ENCODE_DECODE_MODE EncodeDecodeMode, 
        void *pvContext
    );

    virtual void UnInit(void* pvContext);

    virtual int Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pnRetEventCount);

private:
    ULONG                   m_ulRefCount;
    KG_SocketServerAcceptor m_SocketServer;
};
#endif

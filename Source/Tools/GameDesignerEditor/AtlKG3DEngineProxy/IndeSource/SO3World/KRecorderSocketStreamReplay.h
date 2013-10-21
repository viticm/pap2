////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSocketStreamReplay.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-30 13:51:30
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KRECORDERSOCKETSTREAMREPLAY_H_
#define _INCLUDE_KRECORDERSOCKETSTREAMREPLAY_H_
#include "Common/KG_Socket.h"
#include "KProtocolReplayer.h"
class KRecorderSocketStreamReplay : public IKG_SocketStream
{
public:
    KRecorderSocketStreamReplay(IProtocolReplayer* piReplayer) : m_ulRefCount(1)
    {
        assert(piReplayer);
        
        m_piReplayer = piReplayer;
        m_piReplayer->AddRef();
    }
    ~KRecorderSocketStreamReplay()
    {
        KG_COM_RELEASE(m_piReplayer);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

public:
    int     SetTimeout(const timeval* pTimeout);
    int     CheckCanSend(const timeval* pTimeout);
    int     Send(IKG_Buffer* piBuffer);
    int     CheckCanRecv(const timeval* pTimeout);
    int     Recv(IKG_Buffer** ppiRetBuffer);
    int     IsAlive();
    int     GetRemoteAddress(struct in_addr*pRemoteIP, u_short* pusRemotePort);
    int     SetUserData(void* pvUserData);
    void*   GetUserData();
    int     GetLastError();

private:
    ULONG               m_ulRefCount;
    IProtocolReplayer*  m_piReplayer;
};

//////////////////////////////////////////////////////////////////////////

class KRecorderAsyncSocketStreamReplay : public IKG_SocketStream
{
public:
    KRecorderAsyncSocketStreamReplay(IProtocolReplayer* piReplayer) : m_ulRefCount(1)
    {
        assert(piReplayer);
        
        m_piReplayer = piReplayer;
        m_piReplayer->AddRef();
    }

    ~KRecorderAsyncSocketStreamReplay()
    {
        KG_COM_RELEASE(m_piReplayer);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

public:
    int     SetTimeout(const timeval* pTimeout);
    int     CheckCanSend(const timeval* pTimeout);
    int     Send(IKG_Buffer* piBuffer);
    int     CheckCanRecv(const timeval* pTimeout);
    int     Recv(IKG_Buffer** ppiRetBuffer);
    int     IsAlive();
    int     GetRemoteAddress(struct in_addr*pRemoteIP, u_short* pusRemotePort);
    int     SetUserData(void* pvUserData);
    void*   GetUserData();
    int     GetLastError();

private:
    ULONG               m_ulRefCount;
    IProtocolReplayer*  m_piReplayer;
    void*               m_pvUserData;
};
#endif //_INCLUDE_KRECORDERSOCKETSTREAMREPLAY_H_

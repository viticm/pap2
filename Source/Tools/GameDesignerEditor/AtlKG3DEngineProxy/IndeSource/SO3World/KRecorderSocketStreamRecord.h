////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSocketStreamRecord.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-30 10:35:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KRECORDERSOCKETSTREAMRECORD_H_
#define _INCLUDE_KRECORDERSOCKETSTREAMRECORD_H_
#include "Common/KG_Socket.h"
#include "KProtocolRecorder.h"

class KRecorderSocketStreamRecord : public IKG_SocketStream
{
public:
    KRecorderSocketStreamRecord(IProtocolRecorder* piRecorder, IKG_SocketStream* piSocketStream) : m_ulRefCount(1)
    {
        assert(piRecorder);
        assert(piSocketStream);
        
        m_piRecorder = piRecorder;
        m_piRecorder->AddRef();

        m_piSocketStream = piSocketStream;
        m_piSocketStream->AddRef();
    }
    ~KRecorderSocketStreamRecord()
    {
        KG_COM_RELEASE(m_piSocketStream);
        KG_COM_RELEASE(m_piRecorder);
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
    IKG_SocketStream*   m_piSocketStream;
    IProtocolRecorder*  m_piRecorder;
};

//////////////////////////////////////////////////////////////////////////
class KRecorderAsyncSocketStreamRecord : public IKG_SocketStream
{
public:
    KRecorderAsyncSocketStreamRecord(IProtocolRecorder* piRecorder, IKG_SocketStream* piSocketStream) : m_ulRefCount(1)
    {
        assert(piRecorder);
        assert(piSocketStream);
        
        m_piRecorder = piRecorder;
        m_piRecorder->AddRef();

        m_piSocketStream = piSocketStream;
        m_piSocketStream->AddRef();
    }
    ~KRecorderAsyncSocketStreamRecord()
    {
        KG_COM_RELEASE(m_piSocketStream);
        KG_COM_RELEASE(m_piRecorder);
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
    IKG_SocketStream*   m_piSocketStream;
    IProtocolRecorder*  m_piRecorder;
    void*               m_pvUserData;
};
#endif //_INCLUDE_KRECORDERSOCKETSTREAMRECORD_H_

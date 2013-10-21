////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSocketClientNormal.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-29 17:29:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KRECORDERSOCKETCLIENTNORMAL_H_
#define _KRECORDERSOCKETCLIENTNORMAL_H_
#include "KRecorderFactory.h"
#include "Common/KG_Socket.h"
class KRecorderSocketClientNormal : public ISocketClient
{
public:
    KRecorderSocketClientNormal() : m_ulRefCount(1){}
    ~KRecorderSocketClientNormal(){}
    
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

public:
    IKG_SocketStream* Connect(const char cszIP[], int nPort);

private:
    ULONG                   m_ulRefCount;
};
#endif

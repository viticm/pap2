////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSocketClientRecord.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-30 9:55:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KRECORDERSOCKETCLIENTRECORD_H_
#define _INCLUDE_KRECORDERSOCKETCLIENTRECORD_H_
#include "KRecorderFactory.h"
#include "KProtocolRecorder.h"
#include "Common/KG_Socket.h"

class KRecorderSocketClientRecord : public ISocketClient
{
public:
    KRecorderSocketClientRecord(IProtocolRecorder* piRecorder) : m_ulRefCount(1)
    {
        assert(piRecorder);
        m_piRecorder = piRecorder;
        m_piRecorder->AddRef();
    }
    ~KRecorderSocketClientRecord()
    {
        KG_COM_RELEASE(m_piRecorder);
    }
    
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

public:
    IKG_SocketStream* Connect(const char cszIP[], int nPort);

private:
    ULONG                   m_ulRefCount;
    IProtocolRecorder*      m_piRecorder;
};
#endif //_INCLUDE_KRECORDERSOCKETCLIENTRECORD_H_

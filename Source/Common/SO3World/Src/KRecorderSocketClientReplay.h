////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSocketClientReplay.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-30 13:28:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KRECORDERSOCKETCLIENTREPLAY_H_
#define _INCLUDE_KRECORDERSOCKETCLIENTREPLAY_H_
#include "KRecorderFactory.h"
#include "KProtocolReplayer.h"

class KRecorderSocketClientReplay : public ISocketClient
{
public:
    KRecorderSocketClientReplay(IProtocolReplayer* piReplayer) : m_ulRefCount(1)
    {
        assert(piReplayer);
        m_piReplayer = piReplayer;
        m_piReplayer->AddRef();
    }
    ~KRecorderSocketClientReplay()
    {
        KG_COM_RELEASE(m_piReplayer);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

public:
    IKG_SocketStream* Connect(const char cszIP[], int nPort);

private:
    ULONG               m_ulRefCount;
    IProtocolReplayer*  m_piReplayer;    
};
#endif //_INCLUDE_KRECORDERSOCKETCLIENTREPLAY_H_

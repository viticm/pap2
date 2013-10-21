////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderToolsReplay.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng,XiaYong
//  Create Date : 2008-7-25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KRECORDERTOOLSREPLAY_H_
#define _KRECORDERTOOLSREPLAY_H_
#include "KRecorderFactory.h"
#include "KProtocolReplayer.h"
class KRecorderToolsReplay : public IRecorderTools
{
public:
    KRecorderToolsReplay(IProtocolReplayer* piReplayer) : m_ulRefCount(1)
    {
        assert(piReplayer);
        m_piReplayer = piReplayer;
        m_piReplayer->AddRef();
    }
    ~KRecorderToolsReplay()
    {
        KG_COM_RELEASE(m_piReplayer);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
    {
        KG_USE_ARGUMENT(riid);
        KG_USE_ARGUMENT(ppvObject);
        return KG_E_FAIL;
    }

    ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
    }

    ULONG STDMETHODCALLTYPE Release(void)
    {
        ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
        if (ulCount == 0)
        {
            delete this;
        }
        return ulCount;
    }
    
public:
    DWORD   GetTickCount();
    time_t  GetTime();
    BOOL    CreateGUID(GUID* pGuid);

private:
    ULONG               m_ulRefCount;
    IProtocolReplayer*  m_piReplayer;
};
#endif

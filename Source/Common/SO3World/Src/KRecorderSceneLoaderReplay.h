////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSceneLoaderReplay.h
//  Version     : 1.0
//  Creator     : ZhaoChunFeng,XiaYong
//  Create Date : 2008-7-29 11:02:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KRECORDERSCENELOADERREPLAY_H_
#define _KRECORDERSCENELOADERREPLAY_H_

#include "KProtocolReplayer.h"
#include "KRecorderFactory.h"

class KRecorderSceneLoaderReplay : public IRecorderSceneLoader
{
public:
    KRecorderSceneLoaderReplay(IProtocolReplayer* piReplayer) : m_ulRefCount(1)
    {
        assert(piReplayer);
        m_piReplayer = piReplayer;
        m_piReplayer->AddRef();
    }
    ~KRecorderSceneLoaderReplay()
    {
        KG_COM_RELEASE(m_piReplayer);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    
public:    
    BOOL    PostLoadingScene(DWORD dwSceneID);
    DWORD   GetCompletedScene();

private:
    ULONG                   m_ulRefCount;
    IProtocolReplayer*      m_piReplayer;
};
#endif

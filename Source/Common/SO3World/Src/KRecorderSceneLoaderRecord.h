////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSceneLoaderRecord.h
//  Version     : 1.0
//  Creator     : ZhaoChunFeng, XiaYong
//  Create Date : 2008-7-29 11:03:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KRECORDERSCENELOADERRECORD_H_
#define _KRECORDERSCENELOADERRECORD_H_

#include "KRecorderFactory.h"
#include "KCircleList.h"
#include "KProtocolRecorder.h"

class KRecorderSceneLoaderRecord : public IRecorderSceneLoader
{
public:
    KRecorderSceneLoaderRecord(IProtocolRecorder* piRecorder) : m_ulRefCount(1), m_bQuit(false)
    {
        assert(piRecorder);
        m_piRecorder = piRecorder;
        m_piRecorder->AddRef();
    }
    ~KRecorderSceneLoaderRecord()
    {
        KG_COM_RELEASE(m_piRecorder);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    
public:
    BOOL Init();
    void UnInit();
    
    BOOL PostLoadingScene(DWORD dwSceneID);
    DWORD GetCompletedScene();

private:    
    static void WorkThreadFunction(void *pvParam);
    void ThreadFunction();

private:
    BOOL                    m_bQuit;
    KThread                 m_Thread;
    ULONG                   m_ulRefCount;
    KCircleList<DWORD, 64>	m_LoadingList;
    KCircleList<DWORD, 64>	m_LoadedList;
    IProtocolRecorder*      m_piRecorder;
};
#endif

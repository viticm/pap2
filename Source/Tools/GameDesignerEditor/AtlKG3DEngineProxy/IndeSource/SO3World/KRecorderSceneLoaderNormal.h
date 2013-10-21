////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderSceneLoaderNormal.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng,XiaYong
//  Create Date : 2008-7-28 17:55:58
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KRECORDERSCENELOADERNORMAL_H_
#define _KRECORDERSCENELOADERNORMAL_H_
#include "KRecorderFactory.h"
#include "KCircleList.h"

class KRecorderSceneLoaderNormal : public IRecorderSceneLoader
{
public:
    KRecorderSceneLoaderNormal() : m_ulRefCount(1), m_bQuit(false){}
    ~KRecorderSceneLoaderNormal(){}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    
public:
    BOOL Init();
    void UnInit();
    
    BOOL  PostLoadingScene(DWORD dwSceneID);
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
};
#endif


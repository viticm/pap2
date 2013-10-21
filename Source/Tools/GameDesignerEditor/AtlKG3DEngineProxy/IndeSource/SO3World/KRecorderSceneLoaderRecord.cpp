#include "stdafx.h"
#include "KScene.h"
#include "KSO3World.h"
#include "KRecorderSceneLoaderRecord.h"
#include "KProtocolRecorderBase.h"

HRESULT STDMETHODCALLTYPE KRecorderSceneLoaderRecord::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSceneLoaderRecord::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSceneLoaderRecord::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        UnInit();
        delete this;
    }
    return ulCount;
}

BOOL KRecorderSceneLoaderRecord::Init()
{
    BOOL bResult  = false;
    int  nRetCode = false;

    nRetCode = m_Thread.Create(WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KRecorderSceneLoaderRecord::UnInit()
{
    m_bQuit = true;
    m_Thread.Destroy();
}

BOOL KRecorderSceneLoaderRecord::PostLoadingScene(DWORD dwSceneID)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    bRetCode = m_LoadingList.Push(dwSceneID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

DWORD KRecorderSceneLoaderRecord::GetCompletedScene()
{
    DWORD   dwSceneID  = ERROR_ID;

    m_LoadedList.Pop(&dwSceneID);

    if (dwSceneID == ERROR_ID)
    {
        KGET_LOADED_SCENE_BASE   GetLoadedScene;
        
        GetLoadedScene.uType            = ertGetLoadedScene;
        GetLoadedScene.uSceneLoadedFlag = false;

        m_piRecorder->Push(&GetLoadedScene, sizeof(GetLoadedScene));
    }
    else
    {
        KGET_LOADED_SCENE   GetLoadedScene;
        
        GetLoadedScene.uType            = ertGetLoadedScene;
        GetLoadedScene.uSceneLoadedFlag = true;
        GetLoadedScene.dwSceneID        = dwSceneID;
    
        m_piRecorder->Push(&GetLoadedScene, sizeof(GetLoadedScene));
    }
    
Exit0:
    return dwSceneID;
}

void KRecorderSceneLoaderRecord::WorkThreadFunction(void *pvParam)
{
    KRecorderSceneLoaderRecord* pThis = (KRecorderSceneLoaderRecord*)pvParam;

    assert(pThis);
    pThis->ThreadFunction();
}

void KRecorderSceneLoaderRecord::ThreadFunction()
{
    BOOL    bRetCode    = false;
    DWORD   dwSceneID   = ERROR_ID;
    KScene* pScene      = NULL;

    while(!m_bQuit)
	{
        KGThread_Sleep(100);

		bRetCode = m_LoadingList.Pop(&dwSceneID);
		if (!bRetCode)
			continue;
        
        pScene = g_pSO3World->m_SceneSet.GetObj(dwSceneID);
        KGLOG_PROCESS_ERROR(pScene);
        
        KGLOG_PROCESS_ERROR(pScene->m_eSceneState == ssLoading);

#ifdef _SERVER
        bRetCode = pScene->ServerLoad();
#endif
        
        pScene->m_eSceneState = bRetCode ? ssCompleteLoading : ssFailedLoading;

        m_LoadedList.Push(dwSceneID);
    }

Exit0:
    return;
}

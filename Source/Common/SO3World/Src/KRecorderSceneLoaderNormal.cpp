#include "stdafx.h"
#include "KRecorderSceneLoaderNormal.h"
#include "KScene.h"
#include "KSO3World.h"

HRESULT STDMETHODCALLTYPE KRecorderSceneLoaderNormal::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSceneLoaderNormal::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSceneLoaderNormal::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        UnInit();
        delete this;
    }
    return ulCount;
}

BOOL KRecorderSceneLoaderNormal::Init()
{
    BOOL bResult  = false;
    int  nRetCode = false;

    nRetCode = m_Thread.Create(WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KRecorderSceneLoaderNormal::UnInit()
{
    m_bQuit = true;
    m_Thread.Destroy();
}

BOOL KRecorderSceneLoaderNormal::PostLoadingScene(DWORD dwSceneID)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

    bRetCode = m_LoadingList.Push(dwSceneID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

DWORD KRecorderSceneLoaderNormal::GetCompletedScene()
{
    DWORD   dwSceneID       = ERROR_ID;

    m_LoadedList.Pop(&dwSceneID);

    return dwSceneID;
}

void KRecorderSceneLoaderNormal::WorkThreadFunction(void *pvParam)
{
    KRecorderSceneLoaderNormal* pThis = (KRecorderSceneLoaderNormal*)pvParam;

    assert(pThis);

    pThis->ThreadFunction();
}

void KRecorderSceneLoaderNormal::ThreadFunction()
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

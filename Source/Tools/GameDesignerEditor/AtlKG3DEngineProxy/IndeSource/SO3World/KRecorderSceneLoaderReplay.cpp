#include "stdafx.h"
#include "KScene.h"
#include "KSO3World.h"
#include "KRecorderSceneLoaderReplay.h"
#include "KProtocolRecorderBase.h"

HRESULT STDMETHODCALLTYPE KRecorderSceneLoaderReplay::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderSceneLoaderReplay::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderSceneLoaderReplay::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

BOOL KRecorderSceneLoaderReplay::PostLoadingScene(DWORD dwSceneID)
{
    return true;
}

DWORD KRecorderSceneLoaderReplay::GetCompletedScene()
{
    DWORD                   dwResult   = ERROR_ID;
    BOOL                    bRetCode   = false;
    DWORD                   dwSceneID  = ERROR_ID;
    KScene*                 pScene     = NULL;
    KGET_LOADED_SCENE_BASE  GetLoadedScene;

    bRetCode = m_piReplayer->Pop(&GetLoadedScene, sizeof(GetLoadedScene));
    KGLOG_PROCESS_ERROR(bRetCode);

    assert(GetLoadedScene.uType == ertGetLoadedScene);

    KG_PROCESS_ERROR(GetLoadedScene.uSceneLoadedFlag);
    
    bRetCode = m_piReplayer->Pop(&dwSceneID, sizeof(dwSceneID));
    KGLOG_PROCESS_ERROR(bRetCode);

    pScene = g_pSO3World->m_SceneSet.GetObj(dwSceneID);
    KGLOG_PROCESS_ERROR(pScene);

    KGLOG_PROCESS_ERROR(pScene->m_eSceneState == ssLoading);

#ifdef _SERVER
    bRetCode = pScene->ServerLoad();
#endif
    
    pScene->m_eSceneState = bRetCode ? ssCompleteLoading : ssFailedLoading;
    
    dwResult = dwSceneID;
Exit0:
    return dwResult;
}

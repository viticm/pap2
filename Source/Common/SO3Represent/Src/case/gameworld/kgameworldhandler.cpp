#include "stdafx.h"
#include "KScene.h"
#include "IKG3DSoundShell.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/KSO3World.h"
#include "../../Source/Common/SO3World/Src/KPlayer.h"
#include "./ktabletransform.h"
#include "../actionobject/krlcamera.h"
#include "../actionobject/krlcursor.h"
#include "../actionobject/krltarget.h"
#include "../actionobject/krlscene.h"
#include "../actionobject/krldoodad.h"
#include "../actionobject/krlrides.h"
#include "../actionobject/krlcharacter.h"
#include "../actionobject/krlremotecharacter.h"
#include "../render/kmodel.h"
#include "../render/kmodelgcmgr.h"
#include "../../SO3Represent.h"
#include "../../krlcommand.h"
#include "../../krloptimization.h"
#include "../../krltracemsg.h"

#define ENABLE_REPRESENT_OBJECT_LOAD 0

static KRLCursor* GetCursor(DWORD dwScene)
{
	KRLScene* pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
	return pRLScene ? pRLScene->m_pRLCursor : NULL;
}

static KRLTarget* GetTarget(DWORD dwScene)
{
	KRLScene* pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
	return pRLScene ? pRLScene->m_pRLTarget : NULL;
}

static KRLCamera* GetCamera(DWORD dwScene)
{
	KRLScene* pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
	return pRLScene ? pRLScene->m_pRLCamera : NULL;
}

KGameWorldHandler::~KGameWorldHandler()
{
}

static int ActivateFrame()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;

    KRLScenePtrArray& RLScenePtrArray = g_pRL->m_SceneMgr.m_apScene;

    for (KRLScenePtrArray::iterator itScene = RLScenePtrArray.begin(); itScene != RLScenePtrArray.end(); ++itScene)
    {
        KRLScene* pRLScene = *itScene;

        KRLLocalCharacter& RLLocalCharacter = pRLScene->m_pRLCamera->m_CharacterCamera.m_RLLocalCharacter;

        KRLCharacter& RLCharacter = RLLocalCharacter.m_RLCharacter;

        KCharacter* pCharacter = RLLocalCharacter.m_FrameData.m_pCharacter;
        if (pCharacter == NULL)
            continue;

        KGLOG_PROCESS_ERROR(pCharacter->m_pScene);

        // update region info
        static DWORD const RL_UPDATE_EGION_INTERVAL = GAME_FPS * 2;

        if ((g_pRL->m_dwGameLoop % RL_UPDATE_EGION_INTERVAL) == 0)
        {
            D3DXVECTOR3 vPosition;

            StartProfile(&g_pRL->m_Profile, KRLPROFILE_REGION_INFO);

            RLCharacter.GetPosition(vPosition);

            if (D3DXVec3Length(&(vPosition - g_pRL->m_SceneMgr.m_vClientPlayerRegionPosition)) > 20.0f)
            {
                DWORD dwSceneID = 0;
                DWORD dwRegionInfo = 0;

                dwSceneID = pCharacter->m_pScene->m_dwID;
                dwRegionInfo = g_pRL->m_SceneMgr.GetRegionData(dwSceneID, vPosition);

                if (g_pRL->m_SceneMgr.m_dwClientPlayerRegionInfo != dwRegionInfo)
                {	
                    g_pRL->m_SceneMgr.m_dwClientPlayerRegionInfo = dwRegionInfo;

                    KUIEventUpdateRegionInfo param = { dwRegionInfo };

                    if (g_pRL->m_pGameWorldUIHandler)
                    {
                        g_pRL->m_pGameWorldUIHandler->OnUpdateRegionInfo(param);
                    }
                }
            }

            StopProfile(&g_pRL->m_Profile, KRLPROFILE_REGION_INFO);
        }
    }

    // update time
    static DWORD const RL_UPDATE_TIME_INTERVAL = GAME_FPS * 60;

    if ((g_pRL->m_dwGameLoop % RL_UPDATE_TIME_INTERVAL) == 0)
    {
        IKG3DTimeClock* p3DClock = NULL;

        if (SUCCEEDED(g_pRL->m_p3DEngineManager->GetInterface(KG3DTYPE_TIME_CLOCK, 0, 0, (LPVOID*)&p3DClock)))
        {
            _ASSERTE(NULL != p3DClock);

            time_t time = g_pRL->m_pSO3WorldClient->GetWorldTime();

            struct tm* tmWorld = localtime(&time);
            if (tmWorld)
            {
                DWORD dwMilliSecond = ((tmWorld->tm_hour * 60 + tmWorld->tm_min) * 60 + tmWorld->tm_sec) * 1000;

                hr = p3DClock->AdjustClock(dwMilliSecond);
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }
    }

    nRetCode = UpdateOptimization(&g_pRL->m_Optimization);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateCharacterResourceLimit(&g_pRL->m_ResourceLimit);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KGameWorldHandler::Activate()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    double fTime = 0.0;
    DWORD dwGameLoop = 0;
    BOOL bFrame = FALSE;
	
    StartProfile(&g_pRL->m_Profile, KRLPROFILE_MAIN);

    KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

    dwGameLoop = (DWORD)g_pRL->m_pSO3World->m_nGameLoop;

	fTime = GetPerformanceTickCount();

	if (g_pRL->m_dwGameLoop != dwGameLoop)
	{
        g_pRL->m_fFrameTimeLast = g_pRL->m_fFrameTime;
		g_pRL->m_fFrameTime = fTime;
		g_pRL->m_dwGameLoop = dwGameLoop;

        bFrame = TRUE;
	}

    g_pRL->m_fTimeLast = g_pRL->m_fTime;
    g_pRL->m_fTime = fTime;

    g_pRL->m_SceneMgr.Activate(g_pRL->m_fTime, g_pRL->m_fTimeLast, dwGameLoop, bFrame);

    nRetCode = UpdateFPS(&g_pRL->m_Optimization);
    KGLOG_PROCESS_ERROR(nRetCode);

	if (bFrame)
    {
        nRetCode = ActivateFrame();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

	hrResult = S_OK;
Exit0:
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_MAIN);
    PrintProfile(&g_pRL->m_Profile);
    return hrResult;
}

HRESULT KGameWorldHandler::ResetActiveObject()
{
    // TODO:
    return g_pRL->m_FilePath.Reset() ? S_OK : E_FAIL;
}

HRESULT KGameWorldHandler::GetCurrentSceneID(DWORD& dwScene) const
{
    HRESULT hrResult = E_FAIL;
	KScene* pScene = NULL;
	
	pScene = g_pRL->m_pSO3WorldClient->GetClientScene();
	KG_PROCESS_ERROR(pScene);
	
	dwScene = pScene->m_dwID;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::Attach3DEngine(IKG3DEngineManager* p3DEngineManager, IKG3DResourceManager* p3DModelManager, IKG3DUI* p3DUI)
{
    ASSERT(g_pRL);
	ASSERT(p3DEngineManager);
	ASSERT(g_pRL->m_p3DEngineManager == NULL);

	ASSERT(p3DModelManager);
	ASSERT(g_pRL->m_p3DModelManager == NULL);

	ASSERT(p3DUI);
	ASSERT(g_pRL->m_p3DUI == NULL);

	g_pRL->m_p3DEngineManager = p3DEngineManager;
	g_pRL->m_p3DModelManager = p3DModelManager;
	g_pRL->m_p3DUI = p3DUI;

	return S_OK;
}

HRESULT KGameWorldHandler::AttachGameWorld(KSO3World* pSO3World, IKGSO3WorldClient* pSO3WorldClient)
{
	ASSERT(pSO3World);
	ASSERT(pSO3WorldClient);

	g_pRL->m_pSO3World = pSO3World;
	g_pRL->m_pSO3WorldClient = pSO3WorldClient;

	return S_OK;
}

HRESULT KGameWorldHandler::AttachUI(ISO3UI *pSO3UI, IKSO3GameWorldUIHandler *pGameWorldUIHandler)
{
	ASSERT(pSO3UI);
	ASSERT(pGameWorldUIHandler);

	g_pRL->m_pSO3UI = pSO3UI;
	g_pRL->m_pGameWorldUIHandler = pGameWorldUIHandler;

	return S_OK;
}

HRESULT KGameWorldHandler::InitHWND(HWND hWnd)
{
    int nRetCode = false;
    HRESULT hrResult = E_FAIL;

    ASSERT(g_pRL);

	g_pRL->m_hWnd = hWnd;

	if (g_pRL->m_pSound)
		g_pRL->m_pSound->Init(hWnd);

    nRetCode = InitPerformanceTickCount();
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::AddSceneOutputWindow(DWORD dwSceneID, HWND hWnd)
{
	int nRetCode = false;
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	TCHAR szName[64];
	KRLScene* pRLScene = NULL;
	IKG3DScene* p3DScene = NULL;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
	KGLOG_PROCESS_ERROR(pRLScene);

	_itot(dwSceneID, szName, 10);

	if (hWnd == NULL)
		hWnd = g_pRL->m_hWnd;

	p3DScene = pRLScene->m_p3DScene;
	KGLOG_PROCESS_ERROR(p3DScene);

	hr = p3DScene->AddOutputWindow(szName, hWnd, 0, 0, &pRLScene->m_nOutputWindowID);
	KGLOG_COM_PROCESS_ERROR(hr);

	nRetCode = pRLScene->InitCamera();
	KGLOG_PROCESS_ERROR(nRetCode);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::RemoveSceneOutputWindow(DWORD dwSceneID)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLScene* pRLScene = NULL;
	IKG3DScene* p3DScene = NULL;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
	KGLOG_PROCESS_ERROR(pRLScene);

	pRLScene->ExitCamera();

	p3DScene = pRLScene->m_p3DScene;
	KGLOG_PROCESS_ERROR(p3DScene);

	if (pRLScene->m_nOutputWindowID != -1)
	{
		hr = p3DScene->RemoveOutputWindow(pRLScene->m_nOutputWindowID);
		KGLOG_COM_CHECK_ERROR(hr);

		pRLScene->m_nOutputWindowID = -1;
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KGameWorldHandler::NewScene(DWORD dwSceneID, DWORD dwOption, int nX, int nY, int nZ, LPCTSTR szFilePath)
{
	int nRetCode = false;
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLScene* pRLScene = NULL;
	IKG3DScene* p3DScene = NULL;
	KG3DSCENE_INITSTRUCT SceneInit;
	struct tm* pTm = NULL;
	KRLCamera * pCamera = NULL;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
	KGLOG_PROCESS_ERROR(pRLScene == NULL);

    KGLOG_PROCESS_ERROR(g_pRL->m_p3DEngineManager);

	// TODO: 目前使用场景编辑器的scene
	hr = g_pRL->m_p3DEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR, &p3DScene);
	KGLOG_COM_PROCESS_ERROR(hr);

	ASSERT(p3DScene);

    KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

	pTm = localtime(&g_pRL->m_pSO3World->m_nCurrentTime);

	memset(&SceneInit, 0, sizeof(SceneInit));
	//SceneInit.m_GameTimeInitStruct.m_dwInitTime = pTm ? pTm->tm_hour * 3600 + pTm->tm_min * 60 + pTm->tm_sec : 0;
	//SceneInit.m_GameTimeInitStruct.m_fBoostFactor = 1.0f;
	SceneInit.m_vecGameGPSInitPos.x = (float)nX * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
	SceneInit.m_vecGameGPSInitPos.z = (float)nY * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
	SceneInit.m_vecGameGPSInitPos.y = (float)nZ / POINT_PER_ALTITUDE * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;

	hr = p3DScene->LoadFromFileEx(szFilePath, 0, dwOption, (DWORD_PTR)&SceneInit, 0);// --异步加载
	KGLOG_COM_PROCESS_ERROR(hr);

	pRLScene  = new(std::nothrow) KRLScene;
	KGLOG_PROCESS_ERROR(pRLScene);

	pRLScene->m_p3DScene = p3DScene;

	nRetCode = pRLScene->Init();
	KGLOG_PROCESS_ERROR(nRetCode);

	g_pRL->m_SceneMgr.Append(dwSceneID, pRLScene);
	g_pRL->m_SceneMgr.m_dwClientPlayerRegionInfo = 0;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KGameWorldHandler::NewScene(%u, %u, %d, %d, %d, %s)\n", dwSceneID, dwOption, nX, nY, nZ, szFilePath ? szFilePath : "");

        g_pRL->m_SceneMgr.Remove(dwSceneID);

        if (p3DScene && g_pRL->m_p3DEngineManager)
        {
            hr = g_pRL->m_p3DEngineManager->RemoveScene(p3DScene);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        SAFE_DELETE(pRLScene);
    }
	return hrResult;
}

HRESULT KGameWorldHandler::LoadScene(DWORD dwSceneID, DWORD dwOption, int nX, int nY, int nZ, LPCTSTR szFilePath)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLScene* pRLScene = NULL;
	IKG3DScene* p3DScene = NULL;
	KG3DSCENE_INITSTRUCT SceneInit;
	struct tm* pTm = NULL;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
	KGLOG_PROCESS_ERROR(pRLScene);

	p3DScene = pRLScene->m_p3DScene;
	KGLOG_PROCESS_ERROR(p3DScene);

    KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

	pTm = localtime(&g_pRL->m_pSO3World->m_nCurrentTime);

	memset(&SceneInit, 0, sizeof(SceneInit));
	//SceneInit.m_GameTimeInitStruct.m_dwInitTime = pTm ? pTm->tm_hour * 3600 + pTm->tm_min * 60 + pTm->tm_sec : 0;
	//SceneInit.m_GameTimeInitStruct.m_fBoostFactor = 1.0f;
	SceneInit.m_vecGameGPSInitPos.x = (float)nX * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
	SceneInit.m_vecGameGPSInitPos.z = (float)nY * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
	SceneInit.m_vecGameGPSInitPos.y = (float)nZ / POINT_PER_ALTITUDE * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;

	hr = p3DScene->LoadFromFileEx(szFilePath, 0, dwOption, (DWORD_PTR)&SceneInit, 0);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
	    KGLogPrintf(KGLOG_ERR, "KGameWorldHandler::LoadScene(%u, %u, %d, %d, %d, %s)\n", dwSceneID, dwOption, nX, nY, nZ, szFilePath ? szFilePath : "");
    }
	return hrResult;
}

HRESULT KGameWorldHandler::DeleteScene(DWORD dwSceneID)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLScene* pRLScene = NULL;
	IKG3DScene* p3DScene = NULL;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
	KGLOG_PROCESS_ERROR(pRLScene);

	hr = RemoveSceneOutputWindow(dwSceneID);
	KGLOG_COM_CHECK_ERROR(hr);

	pRLScene->Exit();

	p3DScene = pRLScene->m_p3DScene;
	KGLOG_PROCESS_ERROR(p3DScene);

    if (g_pRL->m_p3DEngineManager)
    {
        hr = g_pRL->m_p3DEngineManager->RemoveScene(p3DScene);
        KGLOG_COM_CHECK_ERROR(hr);
    }

	hrResult = S_OK;
Exit0:
	g_pRL->m_SceneMgr.Remove(dwSceneID);

	SAFE_DELETE(pRLScene);
	return hrResult;
}

HRESULT KGameWorldHandler::ZoomCamera(DWORD dwScene, float fScale)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->Zoom(fScale);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::BeginDragCamera(DWORD dwScene, int& nX, int& nY)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->BeginDrag(nX, nY);
    
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
		nX = 0;
		nY = 0;
    }
    return hrResult;
}

HRESULT KGameWorldHandler::EndDragCamera(DWORD dwScene, int nX, int nY)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->EndDrag(nX, nY);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::HasCameraDragged(
    DWORD dwScene,
    BOOL& bCamera,
    BOOL& bObject,
    float& fMaxMouseX,
    float& fMaxMouseY,
    float& fFinalMouseX,
    float& fFinalMouseY
)
{
	KRLCamera* pCamera = GetCamera(dwScene);
	if (pCamera)
	{
        pCamera->HasDragged(bCamera, bObject, fMaxMouseX, fMaxMouseY, fFinalMouseX, fFinalMouseY);
	}
	else
	{
		bCamera = FALSE;
        bObject = FALSE;
        fMaxMouseX = 0.0f;
        fMaxMouseY = 0.0f;
        fFinalMouseX = 0.0f;
        fFinalMouseY = 0.f;
	}
	return S_OK;
}

HRESULT KGameWorldHandler::SetCameraParams(DWORD dwScene, KRepresentCameraParams Param)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->SetParams(Param);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::GetCameraParams(DWORD dwScene, KRepresentCameraParams& Param)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->GetParams(Param);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::SetCameraRTParams(DWORD dwScene, KRepresentCameraRTParams Param)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->SetRTParams(Param);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::GetCameraRTParams(DWORD dwScene, KRepresentCameraRTParams& Param)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->GetRTParams(Param);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::EnableControlCamera(DWORD dwScene, size_t nIndex, BOOL bEnable)
{
    int nRetCode = false;
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    nRetCode = pCamera->EnableControl((RL_CONTROL)nIndex, bEnable);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::ToggleControlCamera(DWORD dwScene, size_t nIndex)
{
    int nRetCode = false;
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    nRetCode = pCamera->ToggleControl((RL_CONTROL)nIndex);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::SetView(DWORD dwScene, float fAngle)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->SetView(fAngle);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::TurnToFaceDirection(DWORD dwScene)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->TurnToFaceDirection();
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::GetCameraYawPitchRoll(DWORD dwScene, float& fYaw, float& fPitch, float& fRoll)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->GetYawPitchRoll(fYaw, fPitch, fRoll);
    
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
		fPitch = 0.0f;
		fYaw = 0.0f;
		fRoll = 0.0f;
    }
    return hrResult;
}

HRESULT KGameWorldHandler::SwitchCameraMode(DWORD dwScene, int nMode)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KG_PROCESS_ERROR(pCamera);

    pCamera->SwitchMode(nMode);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::IsCharacterMoving(DWORD dwScene, int nMoveStyle, int *pnIsMoving)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    KGLOG_PROCESS_ERROR(pnIsMoving);

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    *pnIsMoving = pCamera->IsMoving((RL_CONTROL)nMoveStyle);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::MouseControlMoveEnable(DWORD dwScene, BOOL bEnable)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    pCamera->MouseControlMoveEnable(bEnable);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::LockControl(DWORD dwScene, DWORD dwFrame)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    pCamera->m_CharacterCamera.m_RLLocalCharacter.m_dwLockControlFrame = dwFrame;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::StartFollow(DWORD dwScene, DWORD dwTargetType, DWORD dwTargetID)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    nRetCode = pCamera->StartFollow(dwTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::StopFollow(DWORD dwScene)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    nRetCode = pCamera->StopFollow();
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::IsFollowing(DWORD dwScene, BOOL *pbIsFollowing, DWORD *pdwTargetType, DWORD *pdwTargetID)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    KRLCamera* pCamera = NULL;

    KGLOG_PROCESS_ERROR(pbIsFollowing);
    KGLOG_PROCESS_ERROR(pdwTargetType);
    KGLOG_PROCESS_ERROR(pdwTargetID);

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    nRetCode = pCamera->IsFollowing(pbIsFollowing, pdwTargetType, pdwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::AutoMoveToPoint(DWORD dwScene, D3DXVECTOR3 vPoint)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    nRetCode = pCamera->AutoMoveToPoint(vPoint);
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::AutoMoveToTarget(DWORD dwScene, DWORD dwTargetType, DWORD dwTargetID)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    nRetCode = pCamera->AutoMoveToTarget(dwTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::IsAutoMoving(DWORD dwScene, BOOL *pbIsAutoMoving)
{
    HRESULT hrResult = E_FAIL;
    KRLCamera* pCamera = NULL;

    pCamera = GetCamera(dwScene);
    KGLOG_PROCESS_ERROR(pCamera);

    *pbIsAutoMoving = pCamera->IsAutoMoving();

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::ShowCursorEffect(DWORD dwScene, DWORD dwCursorEffectID)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    KRLScene* pRLScene = NULL;
    KRLCursorEffectModel const* pcCursorEffectModel = NULL;

    pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
    KGLOG_PROCESS_ERROR(pRLScene);

    pcCursorEffectModel = g_pRL->m_TableList.GetCursorEffectModel(dwCursorEffectID);
    KGLOG_PROCESS_ERROR(pcCursorEffectModel);

    nRetCode = ShowRLCursorEffect(pRLScene, pcCursorEffectModel);
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

BOOL KGameWorldHandler::OnNewScene(KScene* pScene, int nX, int nY, int nZ, BOOL bEditor)
{
	HRESULT hr = E_FAIL;
    BOOL bResult = FALSE;
	DWORD dwSceneID = ULONG_MAX;

	KG_USE_ARGUMENT(nX);
	KG_USE_ARGUMENT(nY);
	KG_USE_ARGUMENT(nZ);

	ASSERT(pScene);

#if ENABLE_REPRESENT_OBJECT_LOAD
    KGLogPrintf(KGLOG_INFO, "0x%.8x OnNewScene(%u, %s, %u, %d, %d, %d)\n", 
        g_pRL->m_dwGameLoop,
        pScene->m_dwID,
        pScene->m_szName, nX, nY, nZ, bEditor);
    DWORD dwTickCount = KG_GetTickCount();
#endif

	dwSceneID = pScene->m_dwID;

	hr = NewScene(dwSceneID, SCENE_LOADOPTION_PRELOAD, nX, nY, nZ, pScene->m_szFilePath);
	KGLOG_COM_PROCESS_ERROR(hr);

	if (!bEditor)
	{
		hr = AddSceneOutputWindow(dwSceneID, g_pRL->m_hWnd);
		KGLOG_COM_CHECK_ERROR(hr);
	}

    g_pRL->m_bRunning = TRUE;

	bResult = TRUE;
Exit0:

#if ENABLE_REPRESENT_OBJECT_LOAD
    KGLogPrintf(KGLOG_INFO, "0x%.8x OnNewScene(%u, %s) end %d\n", 
        g_pRL->m_dwGameLoop, 
        pScene->m_dwID,
        pScene->m_szName, 
        KG_GetTickCount() - dwTickCount);
#endif
	return bResult;
}

void KGameWorldHandler::OnDeleteScene(DWORD dwSceneID)
{
	HRESULT hr = E_FAIL;

#if ENABLE_REPRESENT_OBJECT_LOAD
    KGLogPrintf(KGLOG_INFO, "%u OnDeleteScene(%u)\n", g_pRL->m_dwGameLoop, dwSceneID);
    DWORD dwTickCount = KG_GetTickCount();
#endif

	KRLCamera* pCamera = GetCamera(dwSceneID);
	if (pCamera)
		pCamera->Reset();

	hr = DeleteScene(dwSceneID);
	KGLOG_COM_CHECK_ERROR(hr);

#if ENABLE_REPRESENT_OBJECT_LOAD
    KGLogPrintf(KGLOG_INFO, "%u OnDeleteScene(%u) end %d\n", g_pRL->m_dwGameLoop, dwSceneID, KG_GetTickCount() - dwTickCount);
#endif
}

void KGameWorldHandler::OnResetFrameData(int nGameLoop, DWORD dwStartTime)
{
    g_pRL->m_nStartLoop = nGameLoop;

    g_pRL->m_fFrameTimeStart = (double)dwStartTime - KG_GetTickCount() + GetPerformanceTickCount();
}

void KGameWorldHandler::OnAddCharacter(KCharacter* pCharacter)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLScene* pRLScene = NULL;
    KRLRemoteCharacter* pRLRemoteCharacter = NULL;
    DWORD dwObjectID = 0;
    DWORD dwSceneID = 0;
    DWORD dwType = 0;
    BOOL bCurrentPlayer = FALSE;

    KG_ASSERT_EXIT(pCharacter);
    KG_ASSERT_EXIT(pCharacter->m_pScene);

#if ENABLE_REPRESENT_OBJECT_LOAD
    KGLogPrintf(KGLOG_INFO, "%u OnAddCharacter(%s, %u, %d, %d, %d)\n", 
        g_pRL->m_dwGameLoop, 
        pCharacter->m_szName, pCharacter->m_dwID, pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_nZ);
    DWORD dwTickCount = KG_GetTickCount();
#endif

    KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

    dwObjectID = pCharacter->m_dwID;
    dwSceneID = pCharacter->m_pScene->m_dwID;
    dwType = IS_PLAYER(dwObjectID) ? RL_OBJECT_PLAYER : RL_OBJECT_NPC;

    bCurrentPlayer = g_pRL->m_pSO3World->m_dwClientPlayerID == dwObjectID;

    pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
    KGLOG_PROCESS_ERROR(pRLScene);

    if (bCurrentPlayer)
    {
        if (pRLScene->m_pRLCamera)
        {
            KRLLocalCharacter& RLLocalCharacter = pRLScene->m_pRLCamera->m_CharacterCamera.m_RLLocalCharacter;
            KRLCharacter& RLCharacter = RLLocalCharacter.m_RLCharacter;

            RLLocalCharacter.Init();

            RLCharacter.AttachObject(dwObjectID);
            RLCharacter.SetObjectType(dwType);

            RLLocalCharacter.m_FrameData.Init(pCharacter);

            nRetCode = RLCharacter.Init(pCharacter, pRLScene, &RLLocalCharacter.m_FrameData);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = RLLocalCharacter.UpdateDisplayData(TRUE);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = pRLScene->m_pRLCamera->InitRLCharacter();
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = RLLocalCharacter.m_FrameData.InvalidateFrameData(
                g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop - 1);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = RLLocalCharacter.m_FrameData.UpdateFrameData(
                g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop, TRUE);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = StartOptimization(&g_pRL->m_Optimization);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }
    else
    {
        pRLRemoteCharacter = pRLScene->m_CharacterGCMgr.Lookup(dwObjectID);
        if (pRLRemoteCharacter)
        {
            hr = pRLScene->m_CharacterGCMgr.Remove(dwObjectID);
            KGLOG_COM_PROCESS_ERROR(hr);

	        nRetCode = pRLRemoteCharacter->m_RLCharacter.ResetSelectable();
    	    KGLOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            pRLRemoteCharacter = new KRLRemoteCharacter;
            KGLOG_PROCESS_ERROR(pRLRemoteCharacter);
        }

        hr = pRLScene->m_CharacterMgr.Append(dwObjectID, pRLRemoteCharacter);
        KGLOG_COM_PROCESS_ERROR(hr);

        pRLRemoteCharacter->Init();
        pRLRemoteCharacter->m_RLCharacter.AttachObject(dwObjectID);
        pRLRemoteCharacter->m_RLCharacter.SetObjectType(dwType);

        pRLRemoteCharacter->m_FrameData.Init(pCharacter);
        pRLRemoteCharacter->m_RLCharacter.Init(pCharacter, pRLScene, &pRLRemoteCharacter->m_FrameData);

        pRLRemoteCharacter->m_RLCharacter.SetAlpha(0.0f);
        pRLRemoteCharacter->m_RLCharacter.FadeIn();

        nRetCode = pRLRemoteCharacter->m_FrameData.UpdateFrameData(
            g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop, TRUE);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    ASSERT(pCharacter->m_pRepresentObject);

#if ENABLE_REPRESENT_OBJECT_LOAD
    KGLogPrintf(KGLOG_INFO, "%u OnAddCharacter(%s, %u, %d, %d, %d) end %u %x\n", 
        g_pRL->m_dwGameLoop, 
        pCharacter->m_szName, pCharacter->m_dwID, pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_nZ, 
        KG_GetTickCount() - dwTickCount, pCharacter->m_pRepresentObject);
#endif

    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_DELETE(pRLRemoteCharacter);

        if (pRLScene)
        {
            if (bCurrentPlayer)
            {
                if (pRLScene->m_pRLCamera)
                {
                    pRLScene->m_pRLCamera->UnInitRLCharacter();
                }
            }
            else
            {
                hr = pRLScene->m_CharacterMgr.Remove(dwObjectID);
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }
    }
}

void KGameWorldHandler::OnRemoveCharacter(KCharacter* pCharacter, BOOL bKilled)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    DWORD dwObjectID = 0;
    DWORD dwSceneID = 0;
    KRLCharacter* pRLCharacter = NULL;
    KRLScene* pRLScene = NULL;
    KRLRemoteCharacter* pRLRemoteCharacter = NULL;
    int nAppendCharacterGCMgr = false;

    KG_ASSERT_EXIT(pCharacter);
    KG_ASSERT_EXIT(pCharacter->m_pScene);

#if ENABLE_REPRESENT_OBJECT_LOAD
    KGLogPrintf(KGLOG_INFO, "%u OnRemoveCharacter(%s, %u, %d, %d, %d, %d, %u)\n", 
        g_pRL->m_dwGameLoop, 
        pCharacter->m_szName, pCharacter->m_dwID, pCharacter->m_nX, pCharacter->m_nY, pCharacter->m_nZ,
        pCharacter->m_nFaceDirection,
        pCharacter->m_dwModelID);
#endif

    dwObjectID = pCharacter->m_dwID;
    dwSceneID = pCharacter->m_pScene->m_dwID;

    KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
    KGLOG_PROCESS_ERROR(pRLScene);

    if (pRLCharacter->m_bCurrentPlayer)
    {
        if (pRLScene->m_pRLCamera)
        {
            pRLScene->m_pRLCamera->UnInitRLCharacter();
        }
    }
    else
    {
        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        hr = pRLScene->m_CharacterMgr.Remove(dwObjectID);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = pRLScene->m_CharacterGCMgr.Append(dwObjectID, pRLRemoteCharacter);
        KGLOG_COM_PROCESS_ERROR(hr);

        nAppendCharacterGCMgr = true;

        nRetCode = pRLRemoteCharacter->Update(g_pRL->m_fTime, g_pRL->m_fTimeLast, g_pRL->m_dwGameLoop, TRUE);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (pRLRemoteCharacter->m_FrameData.m_pCharacter->m_eMoveState == cmsOnDeath)
        {
            pRLRemoteCharacter->m_fExpireTime = g_pRL->m_fTime + 30000.0;
        }
        else
        {
            nRetCode = pRLRemoteCharacter->m_RLCharacter.SetSelectable(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);

            pRLRemoteCharacter->m_RLCharacter.FadeOut();
        }

        pRLRemoteCharacter->m_FrameData.m_pCharacter = NULL;
    }

    pCharacter->m_pRepresentObject = NULL;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (nAppendCharacterGCMgr)
        {
            hr = pRLScene->m_CharacterGCMgr.Remove(dwObjectID);
            KGLOG_COM_CHECK_ERROR(hr);

            nAppendCharacterGCMgr = false;

            SAFE_DELETE(pRLRemoteCharacter);
        }
    }
}

void KGameWorldHandler::OnUpdateCharacterFrameData(KCharacter* pCharacter)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;

    KG_ASSERT_EXIT(pCharacter);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    if (pRLCharacter->m_bCurrentPlayer)
    {
        KRLLocalCharacter* pRLLocalCharacter = NULL;
        
        pRLLocalCharacter = CONTAINING_RECORD(pRLCharacter, KRLLocalCharacter, m_RLCharacter);

        nRetCode = pRLLocalCharacter->m_FrameData.UpdateFrameData(
            g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop, 
            true);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;
        
        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        nRetCode = pRLRemoteCharacter->m_FrameData.UpdateFrameData(
            g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop, 
            pRLRemoteCharacter->m_RLCharacter.m_RenderData.IsVisible());
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterPrepareCastSkill(KCharacter* pCharacter, DWORD dwSkillID, DWORD dwSkillLevel)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;

    KG_ASSERT_EXIT(pCharacter);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    if (pRLCharacter->m_bCurrentPlayer)
    {
        KRLLocalCharacter* pRLLocalCharacter = NULL;
        
        pRLLocalCharacter = CONTAINING_RECORD(pRLCharacter, KRLLocalCharacter, m_RLCharacter);

        nRetCode = pRLLocalCharacter->PrepareCastSkill(dwSkillID, dwSkillLevel);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;
        
        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        nRetCode = pRLRemoteCharacter->PrepareCastSkill(dwSkillID, dwSkillLevel);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterCastSkill(KCharacter* pCharacter, KTargetData* pTarget, KSkill* pSkill, DWORD dwBulletID, BOOL bPlayAni)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;
    DWORD dwSkillID = 0;
    DWORD dwSkillLevel = 0;

    KG_ASSERT_EXIT(pCharacter);
    KG_ASSERT_EXIT(pSkill);
    KG_ASSERT_EXIT(pSkill->m_pBaseInfo);

    dwSkillID = pSkill->m_pBaseInfo->dwSkillID;
    dwSkillLevel = pSkill->m_dwLevel;

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);
    
    if (pRLCharacter->m_bCurrentPlayer)
    {
        KRLLocalCharacter* pRLLocalCharacter = NULL;
        
        pRLLocalCharacter = CONTAINING_RECORD(pRLCharacter, KRLLocalCharacter, m_RLCharacter);

        nRetCode = pRLLocalCharacter->CastSkill(dwSkillID, dwSkillLevel, dwBulletID);
        KG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;
        
        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        nRetCode = pRLRemoteCharacter->CastSkill(dwSkillID, dwSkillLevel, dwBulletID);
        KG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterDisplayData(KCharacter* pCharacter)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;
    KRLScene* pRLScene = NULL;
    DWORD dwObjectID = 0;
    BOOL bCurrentPlayer = FALSE;

    KG_ASSERT_EXIT(pCharacter);
    KG_ASSERT_EXIT(pCharacter->m_pScene);

    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pCharacter->m_pScene);

    KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

    dwObjectID = pCharacter->m_dwID;

    bCurrentPlayer = g_pRL->m_pSO3World->m_dwClientPlayerID == dwObjectID;

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    pRLScene = g_pRL->m_SceneMgr.Lookup(pCharacter->m_pScene->m_dwID);
    KGLOG_PROCESS_ERROR(pRLScene);

    if (bCurrentPlayer && pRLScene->m_pRLCamera)
    {
        nRetCode = pRLScene->m_pRLCamera->m_CharacterCamera.m_RLLocalCharacter.UpdateDisplayData(TRUE);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;

        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        nRetCode = pRLRemoteCharacter->UpdateDisplayData(TRUE);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pRLScene)
        {
            if (bCurrentPlayer)
            {
                if (pRLScene->m_pRLCamera)
                {
                    pRLScene->m_pRLCamera->UnInitRLCharacter();
                }
            }
            else
            {
                hr = pRLScene->m_CharacterMgr.Remove(dwObjectID);
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }
    }
    return;
}

void KGameWorldHandler::OnCharacterMountHorse(KCharacter* pCharacter, BOOL bMount)
{
    KG_PROCESS_ERROR(pCharacter);
    KG_PROCESS_ERROR(pCharacter->m_pScene);

    OnCharacterDisplayData(pCharacter);

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterSkillBuffData(KCharacter* pCharacter)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;

    KG_ASSERT_EXIT(pCharacter);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KG_PROCESS_ERROR(pRLCharacter);   // DisplayData前有可能有SkillBuffData的改变
    
    if (pRLCharacter->m_bCurrentPlayer)
    {
        KRLLocalCharacter* pRLLocalCharacter = NULL;
        
        pRLLocalCharacter = CONTAINING_RECORD(pRLCharacter, KRLLocalCharacter, m_RLCharacter);

        nRetCode = pRLLocalCharacter->UpdateSkillBuff();
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;
        
        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        nRetCode = pRLRemoteCharacter->UpdateSkillBuff();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterSkillEffectResult(KCharacter* pTarget, KRepresentSkillEffectResult* pResult)
{
    int nRetCode = false;
    KRLCharacter* pRLTarget = NULL;
    DWORD dwTargetID = 0;

    ASSERT(pTarget);
    ASSERT(pResult);

    nRetCode = KRLSkillEffectResult::GetTargetID(pResult->Target, dwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    pRLTarget = reinterpret_cast<KRLCharacter*>(pTarget->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLTarget);

    if (g_pRL->m_CurrentOptimizationOption.bSkillAnimationUniform)
        pResult->bImmediately = true;

    if (pResult->bImmediately)
    {
        nRetCode = PlaySkillEffectResult(pRLTarget, pResult);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = RegisterSkillEffectResult(pRLTarget, pResult);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterSkillChainDisplay(int nCount, KChainDisplayInfo* pDisplayList)
{
    HRESULT hr = E_FAIL;

    for (int nIndex = 0; nIndex < nCount; ++nIndex)
    {
        KRLCharacter* pSrcCharacter = NULL;
        KRLCharacter* pDstCharacter = NULL;
        KSkillChainModel const* pChain = NULL;

        pSrcCharacter = GetRLCharacter(pDisplayList[nIndex].dwSrcCharacterID);
        KGLOG_CHECK_ERROR(pSrcCharacter);

        pDstCharacter = GetRLCharacter(pDisplayList[nIndex].dwDstCharacterID);
        KGLOG_CHECK_ERROR(pDstCharacter);

        pChain = g_pRL->m_TableList.GetSkillChainModel(pDisplayList[nIndex].dwSkillID, pDisplayList[nIndex].dwSkillLevel);
        KGLOG_CHECK_ERROR(pChain);

        if (pSrcCharacter && pDstCharacter && pChain)
        {
            KRLScene* pRLScene = NULL;
            IKG3DModel* p3DModel = NULL;

            pRLScene = pSrcCharacter->m_RenderData.GetRLScene();
            if (pRLScene)
            {
                p3DModel = pRLScene->m_ModelGCMgr.CreateAutoRemoveModel(pChain->szFilePath, pRLScene);
                if (p3DModel)
                {
                    KRLMonoAnimationParam MonoAnimationParam;

                    D3DXVECTOR3 vPositionSrc(0.0f, 0.0f, 0.0f);
                    D3DXVECTOR3 vPositionDst(0.0f, 0.0f, 0.0f);
                    D3DXVECTOR3 vOffset(0.0f, 0.0f, 0.0f);

                    if (pChain->szSrcBone[0] != _T('\0'))
                        hr = pSrcCharacter->m_RenderData.GetBonePosition(vPositionSrc, vOffset, pChain->szSrcBone);

                    if (FAILED(hr))
                        pSrcCharacter->GetPosition(vPositionSrc);

                    if (pChain->szDstBone[0] != _T('\0'))
                        hr = pDstCharacter->m_RenderData.GetBonePosition(vPositionDst, vOffset, pChain->szDstBone);

                    if (!SUCCEEDED(hr))
                        pDstCharacter->GetPosition(vPositionDst);

                    MonoAnimationParam.dwPlayType = ENU_ANIMATIONPLAY_ADDCURRENT_ONCE;
                    MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
                    MonoAnimationParam.fSpeed = 1.0f;
                    MonoAnimationParam.nOffsetTime = 0;
                    MonoAnimationParam.dwTweenTime = 0;
                    MonoAnimationParam.pUserdata = NULL;
                    MonoAnimationParam.szFilePath = _T("");
                    MonoAnimationParam.fStartTime = g_pRL->m_fTime;

                    hr = KModel::PlayMonoAnimation(p3DModel, MonoAnimationParam);
                    KGLOG_COM_CHECK_ERROR(hr);

                    hr = KModel::SetScale(p3DModel, 1.0f);
                    KGLOG_COM_CHECK_ERROR(hr);

                    hr = p3DModel->UpdateChannelEffectPos(vPositionSrc, vPositionDst);
                    KGLOG_COM_CHECK_ERROR(hr);
                }
            }
        }
    }
}

void KGameWorldHandler::OnCharacterPlayAnimation(KCharacter* pCharacter, DWORD dwAnimationID)
{
    KRLCharacter* pRLCharacter = NULL;

    KG_ASSERT_EXIT(pCharacter);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);
    
    pRLCharacter->BeginPlaySerialAnimation(dwAnimationID);

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterPlayAnimationEnd(KCharacter* pCharacter, DWORD dwAnimationID)
{
    KRLCharacter* pRLCharacter = NULL;

    KG_ASSERT_EXIT(pCharacter);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);
    
    pRLCharacter->EndPlaySerialAnimation(dwAnimationID);

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterFollowTarget(KCharacter* pCharacter, KCharacter* pTarget)
{
    KRLCamera* pCamera = NULL;

	KG_ASSERT_EXIT(pCharacter);
	KG_ASSERT_EXIT(pCharacter->m_pScene);
	
    pCamera = GetCamera(pCharacter->m_pScene->m_dwID);
	KGLOG_PROCESS_ERROR(pCamera);

	pCamera->EnableControl(RL_CONTROL_FOLLOW, pTarget != NULL);

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterUpdateRelation(KCharacter* pCharacter)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;

    KG_ASSERT_EXIT(pCharacter);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->UpdateBalloon();
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

static BOOL KUpdatePlayerRelation(DWORD dwPlayerID, KPlayer* pPlayer)
{
    int nResult = false;
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;
    
    KGLOG_PROCESS_ERROR(pPlayer);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pPlayer->m_pRepresentObject);
    KG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->UpdateBalloon();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

static BOOL KUpdateNpcRelation(DWORD dwNpcID, KNpc* pNpc)
{
    int nResult = false;
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;
    
    KGLOG_PROCESS_ERROR(pNpc);
    
    pRLCharacter = reinterpret_cast<KRLCharacter*>(pNpc->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->UpdateBalloon();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

static BOOL KUpdatePlayerHeadTop(DWORD dwPlayerID, KPlayer* pPlayer)
{
    int nResult = false;
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;
    
    KGLOG_PROCESS_ERROR(pPlayer);
    
    pRLCharacter = reinterpret_cast<KRLCharacter*>(pPlayer->m_pRepresentObject);
    KG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->UpdateBalloon();
    KGLOG_PROCESS_ERROR(nRetCode);

    pRLCharacter->UpdateTitleMarkPosition();

    nResult = true;
Exit0:
    return nResult;
}

static BOOL KUpdateNpcHeadTop(DWORD dwNpcID, KNpc* pNpc)
{
    int nResult = false;
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;
    
    KGLOG_PROCESS_ERROR(pNpc);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pNpc->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->UpdateBalloon();
    KGLOG_PROCESS_ERROR(nRetCode);

    pRLCharacter->UpdateTitleMarkPosition();

    nResult = true;
Exit0:
    return nResult;
}

static BOOL KUpdateDoodadQuestEffect(DWORD dwDoodadID, KDoodad* pDoodad)
{
    int nResult = false;
    KRLDoodad* pRLDoodad = NULL;
    
    KGLOG_PROCESS_ERROR(pDoodad);

    pRLDoodad = reinterpret_cast<KRLDoodad*>(pDoodad->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLDoodad);

    pRLDoodad->UpdateQuestEffect();

    nResult = true;
Exit0:
    return nResult;
}

void KGameWorldHandler::OnCharacterUpdateAllRelation()
{
    IKGSO3WorldClient* pSO3WorldClient = NULL;

    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    pSO3WorldClient->TraversePlayerSet(KUpdatePlayerRelation);
    pSO3WorldClient->TraverseNpcSet(KUpdateNpcRelation);

Exit0:
    return;
}

HRESULT KGameWorldHandler::SetSceneObjectTitleEffect(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, DWORD dwEffectID)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    KSceneObject* pSceneObject = NULL;
    KRLCharacter *pRLCharacter = NULL;

    KG_USE_ARGUMENT(dwScene);
    KGLOG_PROCESS_ERROR(dwEffectID >= RL_GLOBAL_EFFECT_NONE);

    switch (dwObjectType)
    {
    case RL_OBJECT_PLAYER:
    case RL_OBJECT_NPC:
        pRLCharacter = GetRLCharacter(dwObject);
        break;
    default:
        KGLOG_PROCESS_ERROR(false);
        break;
    }
    KGLOG_PROCESS_ERROR(pRLCharacter);

    hrRetCode = pRLCharacter->UnloadTitleMark();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    if (dwEffectID != RL_GLOBAL_EFFECT_NONE)
    {
        hrRetCode = pRLCharacter->LoadTitleMark(dwEffectID);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

void KGameWorldHandler::OnCharacterOpenDoodad(KCharacter* pCharacter, KDoodad* pDoodad)
{
    int nRetCode = false;
    int nResult = false;
	KRLCharacter* pRLCharacter = NULL;
    DWORD dwRepresentID = 0;
    KRLDoodadModel const* pDoodadModel = NULL;

    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pDoodad);

	pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    if (pDoodad->m_eKind == dkCorpse)
        dwRepresentID = pDoodad->m_pTemplate->m_dwRepresentID;
    else
        dwRepresentID = pDoodad->m_dwRepresentID;

    pDoodadModel = g_pRL->m_TableList.GetDoodadModel(dwRepresentID);
    KGLOG_PROCESS_ERROR(pDoodadModel);

    nRetCode = pRLCharacter->PlayAnimation(pDoodadModel->dwPlayerOpenAnimationID, 
        RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_OPEN_DOODAD, RL_ANIMATION_DOWN_UNKNOWN);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return;
}


void KGameWorldHandler::OnCharacterBeginPickDoodad(KCharacter* pCharacter, KDoodad* pDoodad)
{
    int nRetCode = false;
    int nResult = false;
	KRLCharacter* pRLCharacter = NULL;
    DWORD dwRepresentID = 0;
    KRLDoodadModel const* pDoodadModel = NULL;

    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pDoodad); 

	pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    if (pDoodad->m_eKind == dkCorpse)
        dwRepresentID = pDoodad->m_pTemplate->m_dwRepresentID;
    else
        dwRepresentID = pDoodad->m_dwRepresentID;

    pDoodadModel = g_pRL->m_TableList.GetDoodadModel(dwRepresentID);
    KGLOG_PROCESS_ERROR(pDoodadModel);

    nRetCode = pRLCharacter->PlayAnimation(pDoodadModel->dwPlayerBeginPickAnimationID, 
        RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_BEGIN_PICK_DOODAD, RL_ANIMATION_DOWN_UNKNOWN);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return;
}


void KGameWorldHandler::OnCharacterEndPickDoodad(KCharacter* pCharacter, KDoodad* pDoodad)
{
    int nRetCode = false;
    int nResult = false;
	KRLCharacter* pRLCharacter = NULL;
    DWORD dwRepresentID = 0;
    KRLDoodadModel const* pDoodadModel = NULL;

    KGLOG_PROCESS_ERROR(pCharacter);
    KGLOG_PROCESS_ERROR(pDoodad);

	pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    if (pDoodad->m_eKind == dkCorpse)
        dwRepresentID = pDoodad->m_pTemplate->m_dwRepresentID;
    else
        dwRepresentID = pDoodad->m_dwRepresentID;

    pDoodadModel = g_pRL->m_TableList.GetDoodadModel(dwRepresentID);
    KGLOG_PROCESS_ERROR(pDoodadModel);

    nRetCode = pRLCharacter->PlayAnimation(pDoodadModel->dwPlayerEndPickAnimationID, 
        RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_END_PICK_DOODAD, RL_ANIMATION_DOWN_UNKNOWN);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return;
}

void KGameWorldHandler::OnCharacterBeginCraftAnimation(KCharacter* pCharacter, int nCraftID)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;
    KPlayerCraftAnimation const* pCraftModel = NULL;

    KG_ASSERT_EXIT(pCharacter);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    if (pRLCharacter->m_bCurrentPlayer)
    {
        KRLLocalCharacter* pRLLocalCharacter = NULL;

        pRLLocalCharacter = CONTAINING_RECORD(pRLCharacter, KRLLocalCharacter, m_RLCharacter);

        nRetCode = pRLLocalCharacter->m_FrameData.UpdateRepresentFrameData(TRUE,
            pRLLocalCharacter->m_FrameData.GetCurrentPosition());
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;

        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        nRetCode = pRLRemoteCharacter->m_FrameData.UpdateRepresentFrameData(TRUE,
            pRLRemoteCharacter->m_FrameData.GetCurrentPosition());
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    pCraftModel = g_pRL->m_TableList.GetPlayerCraftAnimation(nCraftID);
    KGLOG_PROCESS_ERROR(pCraftModel);

    pRLCharacter->BeginPlaySerialAnimation(pCraftModel->dwAnimationID);

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterEndCraftAnimation(KCharacter* pCharacter, int nCraftID)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;
    KPlayerCraftAnimation const* pCraftModel = NULL;

    KG_ASSERT_EXIT(pCharacter);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    if (pRLCharacter->m_bCurrentPlayer)
    {
        KRLLocalCharacter* pRLLocalCharacter = NULL;

        pRLLocalCharacter = CONTAINING_RECORD(pRLCharacter, KRLLocalCharacter, m_RLCharacter);

        nRetCode = pRLLocalCharacter->m_FrameData.UpdateRepresentFrameData(TRUE,
            pRLLocalCharacter->m_FrameData.GetCurrentPosition());
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;

        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        nRetCode = pRLRemoteCharacter->m_FrameData.UpdateRepresentFrameData(TRUE,
            pRLRemoteCharacter->m_FrameData.GetCurrentPosition());
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    pCraftModel = g_pRL->m_TableList.GetPlayerCraftAnimation(nCraftID);
    KGLOG_PROCESS_ERROR(pCraftModel);

    pRLCharacter->EndPlaySerialAnimation(pCraftModel->dwAnimationID);

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterLevelUp(KCharacter* pCharacter, REPRESENT_SFX nSFX)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;

    KG_ASSERT_EXIT(pCharacter);

	pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
	KG_PROCESS_ERROR(pRLCharacter);
    
    nRetCode = pRLCharacter->PlayLevelUpSFX(nSFX);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterSound(KCharacter* pCharacter, DWORD dwSoundID)
{
    KRLCharacter* pRLCharacter = NULL;

	KG_ASSERT_EXIT(pCharacter);

	pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
	KGLOG_PROCESS_ERROR(pRLCharacter);

	pRLCharacter->PlaySound(dwSoundID);

Exit0:
    return;
}

void KGameWorldHandler::OnCharacterTurnTo(KCharacter* pCharacter, BOOL bTurnImmediately)
{
    int nRetCode = false;
    int nResult = false;
    KRLCharacter* pRLCharacter = NULL;
    KRLScene* pRLScene = NULL;

    KG_ASSERT_EXIT(pCharacter);
    KG_ASSERT_EXIT(pCharacter->m_pScene);

    KGLOG_PROCESS_ERROR(g_pRL->m_pSO3World);

    pRLCharacter = reinterpret_cast<KRLCharacter*>(pCharacter->m_pRepresentObject);
    KGLOG_PROCESS_ERROR(pRLCharacter);

    pRLScene = g_pRL->m_SceneMgr.Lookup(pCharacter->m_pScene->m_dwID);
    KGLOG_PROCESS_ERROR(pRLScene);

    if (pRLCharacter->m_bCurrentPlayer)
    {
        if (pRLScene->m_pRLCamera)
        {
            float fYaw = YawRHToLH(ToSceneAngle(pCharacter->m_nFaceDirection));

            pRLScene->m_pRLCamera->m_CharacterCamera.UpdateYaw(fYaw, bTurnImmediately);
        }
    }
    else
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;
        
        pRLRemoteCharacter = CONTAINING_RECORD(pRLCharacter, KRLRemoteCharacter, m_RLCharacter);

        nRetCode = pRLRemoteCharacter->UpdateYaw(NULL);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return;
}

void KGameWorldHandler::OnCharacterUpdateTitleMark(KCharacter* pCharacter)
{
    return; // tongxuehu
}

void KGameWorldHandler::OnAddDoodad(KDoodad* pDoodad)
{
#if ENABLE_REPRESENT_OBJECT_LOAD
	KGLogPrintf(KGLOG_INFO, "%u OnAddDoodad(%s, %u, %d, %d, %d, %d, %u)\n", 
		g_pRL->m_dwGameLoop,
        pDoodad->m_szName, 
        pDoodad->m_dwID, pDoodad->m_nX, pDoodad->m_nY, pDoodad->m_nZ, 
        pDoodad->m_nFaceDirection, 
        pDoodad->m_dwRepresentID);

	DWORD dwTickCount = KG_GetTickCount();
#endif

	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;
	KRLDoodad* pRLDoodad = NULL;
	KRLScene* pRLScene = NULL;
	DWORD dwDoodad = ULONG_MAX;
	DWORD dwScene = ULONG_MAX;

	ASSERT(pDoodad);
	ASSERT(pDoodad->m_pScene);

	dwDoodad = pDoodad->m_dwID;
	dwScene = pDoodad->m_pScene->m_dwID;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
	KGLOG_PROCESS_ERROR(pRLScene);

    // 查找是否已有doodad还在淡出，又重新进入同步范围
	pRLDoodad = pRLScene->m_DoodadGCMgr.Lookup(dwDoodad);
	if (pRLDoodad)
	{
		hr = pRLScene->m_DoodadGCMgr.Remove(dwDoodad);
		KGLOG_COM_PROCESS_ERROR(hr);

        hr = pRLScene->m_DoodadMgr.Append(dwDoodad, pRLDoodad);
		KGLOG_COM_PROCESS_ERROR(hr);

		if (pRLDoodad->m_FrameData.m_pDoodad == pDoodad)
		{
			pRLDoodad->FadeIn();

            pDoodad->m_pRepresentObject = reinterpret_cast<void*>(pRLDoodad);
		}
		else
		{
			pRLDoodad->Reset();

            pRLDoodad->AttachObject(dwDoodad);
            pRLDoodad->SetObjectType(RL_OBJECT_DOODAD);

            pRLDoodad->Init(pDoodad, pRLScene);
		}

        goto Exit1;
	}

    // 新建一个
    pRLDoodad  = new(std::nothrow) KRLDoodad;
    KGLOG_PROCESS_ERROR(pRLDoodad);

    hr = pRLScene->m_DoodadMgr.Append(dwDoodad, pRLDoodad);
    KGLOG_COM_PROCESS_ERROR(hr);

    pRLDoodad->AttachObject(dwDoodad);
    pRLDoodad->SetObjectType(RL_OBJECT_DOODAD);

    // 查找是否有相同坐标朝向的淡出npc，如果有，就认为是尸体
    if (pDoodad->m_eKind == dkCorpse)
    {
        KRLRemoteCharacter* pRLRemoteCharacter = NULL;

        pRLRemoteCharacter = pRLScene->m_CharacterGCMgr.Lookup(pDoodad->m_dwNpcID);
        if (pRLRemoteCharacter)
        {
            nRetCode = pRLDoodad->InitCorpse(pDoodad, pRLScene, pRLRemoteCharacter);
            KGLOG_PROCESS_ERROR(nRetCode);

            hr = pRLScene->m_CharacterGCMgr.Remove(pDoodad->m_dwNpcID);
            KGLOG_COM_PROCESS_ERROR(hr);

            SAFE_DELETE(pRLRemoteCharacter);

            goto Exit1;
        }
    }

    // 正常的新建初始化
    nRetCode = pRLDoodad->Init(pDoodad, pRLScene);
    KGLOG_PROCESS_ERROR(nRetCode);

    goto Exit1;

Exit1:
	ASSERT(pDoodad->m_pRepresentObject);

#if ENABLE_REPRESENT_OBJECT_LOAD
	KGLogPrintf(KGLOG_INFO, "%u OnAddDoodad(%s, %u, %u, %d, %d, %d) end %u %x\n", 
		g_pRL->m_dwGameLoop, 
		pDoodad->m_szName, pDoodad->m_dwID, pDoodad->m_dwNpcID, pDoodad->m_nX, pDoodad->m_nY, pDoodad->m_nZ, KG_GetTickCount() - dwTickCount, pDoodad->m_pRepresentObject);
#endif

	nResult = true;;
Exit0:
    if (!nResult)
    {
        SAFE_DELETE(pRLDoodad);

        if (pRLScene)
        {
            hr = pRLScene->m_DoodadMgr.Remove(dwDoodad);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
}

void KGameWorldHandler::OnRemoveDoodad(KDoodad* pDoodad)
{
#if ENABLE_REPRESENT_OBJECT_LOAD
	KGLogPrintf(KGLOG_INFO, "%u OnRemoveDoodad(%s, %u)\n", 
		g_pRL->m_dwGameLoop, 
		pDoodad->m_szName, pDoodad->m_dwID);
#endif

    int nRetCode = false;
    HRESULT hr = E_FAIL;

	ASSERT(pDoodad);

	KRLDoodad* pRLDoodad = reinterpret_cast<KRLDoodad*>(pDoodad->m_pRepresentObject);
	if (pRLDoodad)
	{
		KRLScene* pRLScene = pRLDoodad->m_RenderData.GetRLScene();
		if (pRLScene)
        {
            hr = pRLDoodad->m_RenderData.SetMDLSelectable(FALSE);
            KGLOG_COM_CHECK_ERROR(hr);

            pRLScene->m_DoodadMgr.Remove(pDoodad->m_dwID);
			pRLScene->m_DoodadGCMgr.Append(pDoodad->m_dwID, pRLDoodad);
        }

        nRetCode = pRLDoodad->SetState(RL_DOODAD_STATE_LEAVE_SCENE);
        KGLOG_CHECK_ERROR(nRetCode);

		pRLDoodad->FadeOut();
	}

	pDoodad->m_pRepresentObject = NULL;
}

void KGameWorldHandler::OnDoodadLootList(KDoodad* pDoodad)
{
    HRESULT hr = E_FAIL;
    KRLDoodad* pRLDoodad = NULL;

    ASSERT(pDoodad);

    pRLDoodad = reinterpret_cast<KRLDoodad*>(pDoodad->m_pRepresentObject);
	KGLOG_PROCESS_ERROR(pRLDoodad);

    hr = pRLDoodad->UpdateLootList();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pRLDoodad->UpdateQuestEffect();
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return;
}

void KGameWorldHandler::OnDoodadIdle(KDoodad* pDoodad)
{
    int nRetCode = false;
    KRLDoodad* pRLDoodad = NULL;

    ASSERT(pDoodad);

    pRLDoodad = reinterpret_cast<KRLDoodad*>(pDoodad->m_pRepresentObject);
	KGLOG_PROCESS_ERROR(pRLDoodad);

    nRetCode = pRLDoodad->SetState(RL_DOODAD_STATE_IDLE);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KGameWorldHandler::OnDoodadAfterHit(KDoodad* pDoodad)
{
    int nRetCode = false;
    KRLDoodad* pRLDoodad = NULL;

    ASSERT(pDoodad);

    pRLDoodad = reinterpret_cast<KRLDoodad*>(pDoodad->m_pRepresentObject);
	KGLOG_PROCESS_ERROR(pRLDoodad);

    nRetCode = pRLDoodad->SetState(RL_DOODAD_STATE_AFTER_OPERATION);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

void KGameWorldHandler::OnDoodadBeHit(KDoodad* pDoodad)
{
    int nRetCode = false;
    KRLDoodad* pRLDoodad = NULL;

    ASSERT(pDoodad);

    pRLDoodad = reinterpret_cast<KRLDoodad*>(pDoodad->m_pRepresentObject);
	KGLOG_PROCESS_ERROR(pRLDoodad);

    nRetCode = pRLDoodad->SetState(RL_DOODAD_STATE_OPERATION);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

HRESULT KGameWorldHandler::GetScene(DWORD dwSceneID, IKG3DScene** pp3DScene) const
{
    HRESULT hrResult = E_FAIL;
	KRLScene* pRLScene = NULL;
	IKG3DScene* p3DScene = NULL;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
	KGLOG_PROCESS_ERROR(pRLScene);

	p3DScene = pRLScene->m_p3DScene;
	KGLOG_PROCESS_ERROR(p3DScene);

	*pp3DScene = p3DScene;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        DWORD dwCurrentSceneID = 0;
        GetCurrentSceneID(dwCurrentSceneID);
        KGLogPrintf(KGLOG_ERR, "KGameWorldHandler::GetScene(%u, %u)\n", dwSceneID, dwCurrentSceneID);
    }
	return hrResult;
}

size_t KGameWorldHandler::SelectPtInScene(DWORD dwSceneID, D3DXVECTOR2 v2Scale, KRepresentSceneObject* pObject, size_t nObjectCount)
{
	typedef std::vector<IKG3DModel*> K3DModelPtrArray;

    int nResult = false;
	HRESULT hr = E_FAIL;
	KRLScene* pRLScene = NULL;
	IKG3DScene* p3DScene = NULL;
	K3DModelPtrArray aModel;
	int nCount = 0;
	D3DXVECTOR3 vPosition;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwSceneID);
	KGLOG_PROCESS_ERROR(pRLScene);

	p3DScene = pRLScene->m_p3DScene;
	KGLOG_PROCESS_ERROR(p3DScene);

	hr = p3DScene->GetSelectModelList(v2Scale, aModel, nObjectCount == 1);
	KGLOG_COM_PROCESS_ERROR(hr);

	nCount = (int)min(aModel.size(), nObjectCount);
	nObjectCount = 0;
	for (int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		KSceneObject* pSceneObject = NULL;
		DWORD dwSceneObjectID = 0;
		DWORD dwSceneObjectType = 0;

		if (FAILED(pRLScene->m_ModelMgr.Lookup(&pSceneObject, &dwSceneObjectType, &dwSceneObjectID, aModel[nIndex])))
			continue;

        if (dwSceneObjectType == RL_OBJECT_RIDES)
            dwSceneObjectType = RL_OBJECT_PLAYER;

		pObject[nObjectCount].dwSceneObjectType = dwSceneObjectType;
		pObject[nObjectCount].dwSceneObjectID = dwSceneObjectID;

		++nObjectCount;
	}

    nResult = true;
Exit0:
    if (!nResult)
    {
        nObjectCount = 0;
    }
	return nObjectCount;
}

HRESULT KGameWorldHandler::SelectPtInGround(DWORD dwSceneID, D3DXVECTOR2 v2Scale, int& nX, int& nY, int& nZ)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	IKG3DScene*	p3DScene = NULL;
	D3DXVECTOR3 vPosition;
	
	hr = GetScene(dwSceneID, &p3DScene);
	KGLOG_COM_PROCESS_ERROR(hr);

	// Select ground
    hr = p3DScene->GetSelectPosition(v2Scale, vPosition);
    if (FAILED(hr))
    {
        vPosition.x = FLT_MAX;
        vPosition.y = FLT_MAX;
        vPosition.z = FLT_MAX;
    }

    hr = p3DScene->SceneToLogical(&nX, &nY, &nZ, vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::GetCharacterTop(DWORD dwCharacterID, D3DXVECTOR3& vPosition)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	float fBalloonHeight = 0.0f;
	KRLCharacter* pRLCharacter = NULL;
    D3DXVECTOR3 vOffset(0.0f, 0.0f, 0.0f);

    pRLCharacter = GetRLCharacter(dwCharacterID);
    KG_PROCESS_ERROR(pRLCharacter);
   
    if (pRLCharacter->m_pRLVehicle)
    {
        hr = pRLCharacter->m_pRLVehicle->m_RenderData.GetBonePosition(vPosition, vOffset, _T("S_Hs"));
        if (FAILED(hr))
        {
            pRLCharacter->GetPosition(vPosition);
        }
    }
    else
    {
        pRLCharacter->GetPosition(vPosition);
    }

    vPosition.y += pRLCharacter->m_EquipResource.fModelHeight + pRLCharacter->GetRideAdjustHeight();

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
    }
    return hrResult;
}

HRESULT KGameWorldHandler::ScenePointToScreenPoint(DWORD dwSceneID, D3DXVECTOR3 const& vScenePos, float& fScreenX, float& fScreenY)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;
    IKG3DScene* p3DScene = NULL;

	hr = GetScene(dwSceneID, &p3DScene);
	KGLOG_COM_PROCESS_ERROR(hr);
    KG_ASSERT_EXIT(p3DScene);
    
    hr = p3DScene->ScenePosToScreenPos(vScenePos, fScreenX, fScreenY);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = hr;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::GameWorldPositionToScenePosition(D3DXVECTOR3& vPosition, DWORD dwScene, float fX, float fY, float fZ, BOOL bOnFloor)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	IKG3DScene* p3DScene = NULL;

	hr = GetScene(dwScene, &p3DScene);
	KGLOG_COM_PROCESS_ERROR(hr);
	KGLOG_PROCESS_ERROR(p3DScene);

	hr = p3DScene->LogicalToScene(&vPosition, fX, fY, fZ, bOnFloor);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KGameWorldHandler::GameWorldPositionToScenePosition(%d, %f, %f, %f, %d)\n", dwScene, fX, fY, fZ, bOnFloor);
    }
	return hrResult;
}

HRESULT KGameWorldHandler::ScenePositionToGameWorldPosition(DWORD dwScene, float& fX, float& fY, float& fZ, D3DXVECTOR3 const& vPosition)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	IKG3DScene* p3DScene = NULL;
	int nX = 0;
	int nY = 0;
	int nZ = 0;

	GetScene(dwScene, &p3DScene);
	KGLOG_PROCESS_ERROR(p3DScene);

    hr = p3DScene->SceneToLogical(&nX, &nY, &nZ, vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

	fX = (float)nX;
	fY = (float)nY;
	fZ = (float)nZ;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::SetSceneObjectAlpha(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, float fScale)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLScene* pRLScene = NULL;
	KSceneObject* pObject = NULL;
	IKG3DModel* ap3DModel[SCENE_OBJECT_MODEL_MAX_COUNT];
	int nCount = 0;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
	KGLOG_PROCESS_ERROR(pRLScene);

	pObject = GetSceneObject(dwObjectType, dwObject);
	KG_PROCESS_SUCCESS(pObject == NULL);

	hr = pRLScene->m_ModelMgr.Lookup(ap3DModel, nCount, pObject);
	KGLOG_COM_PROCESS_ERROR(hr);

    // TODO: Only have to set mdl
	for (int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		hr = KModel::SetAlpha(ap3DModel[nIndex], fScale);
		KGLOG_COM_CHECK_ERROR(hr);
	}

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::SetSceneObjectBrightness(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, float fScale)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLScene* pRLScene = NULL;
	KSceneObject* pObject = NULL;
	int nCount = 0;
	IKG3DModel* ap3DModel[SCENE_OBJECT_MODEL_MAX_COUNT];

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
	KGLOG_PROCESS_ERROR(pRLScene);

	pObject = GetSceneObject(dwObjectType, dwObject);
	KG_PROCESS_SUCCESS(pObject == NULL);

	if (SUCCEEDED(pRLScene->m_ModelMgr.Lookup(ap3DModel, nCount, pObject)))
    {
        // TODO: Only have to set mdl
        for (int nIndex = 0; nIndex < nCount; ++nIndex)
        {
            hr = KModel::SetBrightness(ap3DModel[nIndex], fScale);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::SetSceneObjectSelectable(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, BOOL bSelectable)
{
    int nRetCode = false;
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLScene* pRLScene = NULL;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
	KGLOG_PROCESS_ERROR(pRLScene);

    switch (dwObjectType)
    {
    case RL_OBJECT_UNKNOWN:
        break;
    case RL_OBJECT_PLAYER:
    case RL_OBJECT_NPC:
        {
            KRLRemoteCharacter* pRLRemoteCharacter = NULL;
            
            pRLRemoteCharacter = pRLScene->m_CharacterMgr.Lookup(dwObject);
            if (pRLRemoteCharacter)
            {
                nRetCode = pRLRemoteCharacter->m_RLCharacter.SetSelectable(bSelectable);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
        }
        break;
    case RL_OBJECT_DOODAD:
        {
            KRLDoodad* pRLDoodad = NULL;

            pRLDoodad = pRLScene->m_DoodadMgr.Lookup(dwObject);
            if (pRLDoodad)
            {
                hr = pRLDoodad->m_RenderData.SetMDLSelectable(bSelectable);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }
        break;
    case RL_OBJECT_RIDES:
        KG_ASSERT_EXIT(0);
        break;
    default:
        KG_ASSERT_EXIT(0);
        break;
    }

	hrResult = S_OK;
Exit0:
	return hrResult;
}

int KGameWorldHandler::SceneObjectTypeToGameWorldObjectType(DWORD dwSceneObjectType)
{
	int nGameWorldObjectType = ttNoTarget;

	switch (dwSceneObjectType)
	{
	case RL_OBJECT_UNKNOWN:
		nGameWorldObjectType = ttNoTarget;
		break;
	case RL_OBJECT_PLAYER:
		nGameWorldObjectType = ttPlayer;
		break;
	case RL_OBJECT_NPC:
		nGameWorldObjectType = ttNpc;
		break;
	case RL_OBJECT_DOODAD:
		nGameWorldObjectType = ttDoodad;
		break;
	default:
		ASSERT(0);
		break;
	}
	return nGameWorldObjectType;
}

DWORD KGameWorldHandler::GameWorldObjectTypeToSceneObjectType(int nGameWorldObjectType)
{
	DWORD dwSceneObjectType = RL_OBJECT_UNKNOWN;

	switch (nGameWorldObjectType)
	{
	case ttInvalid:
		dwSceneObjectType = RL_OBJECT_UNKNOWN;
		break;
	case ttNoTarget:
		dwSceneObjectType = RL_OBJECT_UNKNOWN;
		break;
	case ttCoordination:
		dwSceneObjectType = RL_OBJECT_UNKNOWN;
		break;
	case ttNpc:
		dwSceneObjectType = RL_OBJECT_NPC;
		break;
	case ttPlayer:
		dwSceneObjectType = RL_OBJECT_PLAYER;
		break;
	case ttDoodad:
		dwSceneObjectType = RL_OBJECT_DOODAD;
		break;
	case ttItem:
		dwSceneObjectType = RL_OBJECT_UNKNOWN;
		break;
	default:
		ASSERT(0);
		break;
	}
	return dwSceneObjectType;
}

HRESULT KGameWorldHandler::GetPlayerAnimationResource(ROLE_TYPE nRoleType, DWORD dwAnimationID, KRepresentPlayerAnimationResource* pResource) const
{
	return GetPlayerAnimationModelResource(dwAnimationID, nRoleType, pResource);
}

HRESULT KGameWorldHandler::GetPlayerEquipResource(ROLE_TYPE nRoleType, DWORD dwRepresentIDs[perRepresentCount], KRepresentPlayerEquipResource* pResource) const
{
	return GetPlayerEquipModelResource(dwRepresentIDs, nRoleType, pResource);
}

HRESULT KGameWorldHandler::GetRidesAnimationResource(DWORD dwRidesID, DWORD dwAnimationID, KRepresentRidesAnimationResource* pResource) const
{
    return GetRidesAnimationModelResource(dwRidesID, dwAnimationID, pResource);
}

HRESULT KGameWorldHandler::GetRidesEquipResource(DWORD dwRidesID, DWORD dwRepresentIDs[MODEL_RIDES_EQUIP_AND_SOCKET_COUNT], KRepresentRidesEquipResource* pResource) const
{
    return GetRidesEquipModelResource(dwRidesID, dwRepresentIDs, pResource);
}

HRESULT KGameWorldHandler::GetNPCAnimationResource(DWORD dwRepresentID, DWORD dwAnimationID, KRepresentNPCAnimationResource* pResource) const
{
	return GetNpcAnimationModelResource(dwRepresentID, dwAnimationID, pResource);
}

HRESULT KGameWorldHandler::GetNPCEquipResource(DWORD dwRepresentID, KRepresentNPCEquipResource* pResource) const
{
	return GetNpcEquipModelResource(dwRepresentID, pResource);
}

HRESULT KGameWorldHandler::GetDoodadResource(DWORD dwRepresentID, KRepresentDoodadResource* pResource) const
{
	return GetDoodadModelResource(dwRepresentID, pResource);
}

HRESULT KGameWorldHandler::ShowSelectionSFX(DWORD dwScene, DWORD dwSkillID, DWORD dwSkillLevel)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLScene* pRLScene = NULL;
    KRLCamera* pRLCamera = NULL;
	KRLCursor* pRLCursor = NULL;
	KPlayer* pPlayer = NULL;
    ROLE_TYPE nRoleType = rtInvalid;

	pRLScene = g_pRL->m_SceneMgr.Lookup(dwScene);
    KGLOG_PROCESS_ERROR(pRLScene);

    pRLCamera = pRLScene->m_pRLCamera;
    KGLOG_PROCESS_ERROR(pRLCamera);

    pRLCursor = pRLScene->m_pRLCursor;
    KGLOG_PROCESS_ERROR(pRLCursor);

    nRoleType = pRLCamera->m_CharacterCamera.m_RLLocalCharacter.m_RLCharacter.m_EquipResource.Player.nRoleType;

	hr = pRLCursor->Show(dwSkillID, dwSkillLevel, nRoleType);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::HideSelectionSFX(DWORD dwScene)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLCursor* pCursor = NULL;

	pCursor = GetCursor(dwScene);
	KGLOG_PROCESS_ERROR(pCursor);

	hr = pCursor->Hide();
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::AttachTargetSceneObject(DWORD dwTarget, DWORD dwObject, DWORD dwObjectType)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLTarget* pTarget = NULL;

	pTarget = GetTarget(dwTarget);
	KGLOG_PROCESS_ERROR(pTarget);

	hr = pTarget->AttachSceneObject(dwObject, (RL_OJBECT)dwObjectType);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KGameWorldHandler::AttachTargetSceneObject(%u, %u, %u)\n", dwTarget, dwObject, dwObjectType);
    }
	return hrResult;
}

HRESULT KGameWorldHandler::DetachTargetSceneObject(DWORD dwTarget, DWORD dwObject, DWORD dwObjectType)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLTarget* pTarget = NULL;

	pTarget = GetTarget(dwTarget);
	KGLOG_PROCESS_ERROR(pTarget);

	hr = pTarget->DetachSceneObject(dwObject, (RL_OJBECT)dwObjectType);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::ShowTargetSelectionSFX(DWORD dwTarget, int nForceRelationType)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLTarget* pTarget = NULL;

	pTarget = GetTarget(dwTarget);
	KGLOG_PROCESS_ERROR(pTarget);

	hr = pTarget->Show(nForceRelationType);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KGameWorldHandler::HideTargetSelectionSFX(DWORD dwTarget)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KRLTarget* pTarget = NULL;

	pTarget = GetTarget(dwTarget);
	KGLOG_PROCESS_ERROR(pTarget);

	hr = pTarget->Hide();
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

void KGameWorldHandler::EnableHeadSFX(int nCharacterType, int nIndex, BOOL bEnable)
{
    ASSERT(nCharacterType < CHARACTER_TYPE_END);
    ASSERT(nIndex < CHARACTER_SFX_BBOX_END - CHARACTER_SFX_BBOX_BEGIN);

    g_pRL->m_Option.bHeadSFXEnabled[nCharacterType][nIndex] = bEnable;
}

void KGameWorldHandler::UpdateHeadTopPosition()
{
    IKGSO3WorldClient* pSO3WorldClient = NULL;

    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    pSO3WorldClient->TraversePlayerSet(KUpdatePlayerHeadTop);
    pSO3WorldClient->TraverseNpcSet(KUpdateNpcHeadTop);

Exit0:
    return;
}

void KGameWorldHandler::OnUpdateDoodadQuestEffect()
{
    IKGSO3WorldClient* pSO3WorldClient = NULL;

    pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

    pSO3WorldClient->TraverseDoodadSet(KUpdateDoodadQuestEffect);

Exit0:
    return;
}

void KGameWorldHandler::OnFinishQuest(KPlayer* pPlayer, DWORD dwQuestID, BOOL bAssist)
{
    int nRetCode = false;
    KRLCharacter* pRLCharacter = NULL;

    KG_USE_ARGUMENT(dwQuestID);

    ASSERT(pPlayer);

	pRLCharacter = reinterpret_cast<KRLCharacter*>(pPlayer->m_pRepresentObject);
	KGLOG_PROCESS_ERROR(pRLCharacter);
    
    if (bAssist)
    {
        nRetCode = pRLCharacter->PlayLevelUpSFX(REPRESENT_SFX_FINISH_ASSIST_QUEST);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = pRLCharacter->PlayLevelUpSFX(REPRESENT_SFX_FINISH_QUEST);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return;
}

void KGameWorldHandler::OnReputationLevelUp(KPlayer* pPlayer, BOOL bLevelUp)
{
    if (bLevelUp)
    {
        OnCharacterLevelUp((KCharacter*)pPlayer, REPRESENT_SFX_REPUTATION_LEVEL_UP);
    }
    else
    {
        OnCharacterLevelUp((KCharacter*)pPlayer, REPRESENT_SFX_REPUTATION_LEVEL_DOWN);
    }
}

void KGameWorldHandler::OnDisconnect()
{
    g_pRL->m_bRunning = FALSE;
}

int KGameWorldHandler::GetMinimapLayer(DWORD dwScene, D3DVECTOR const& vPos)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	int nLayer = 0;

	IKG3DScene* p3DScene = NULL;

	hr = GetScene(dwScene, &p3DScene);
	KGLOG_COM_PROCESS_ERROR(hr);
	ASSERT(p3DScene);

	hr = p3DScene->GetMinimapLayer(vPos, nLayer);
	KG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        nLayer = 0;
    }
	return nLayer;
}

DWORD KGameWorldHandler::GetRegionInfo(DWORD dwScene, D3DVECTOR const& vPos)
{
	return g_pRL->m_SceneMgr.GetRegionData(dwScene, vPos);
}

DWORD KGameWorldHandler::GetRegionInfoByMapID(DWORD dwMapID, D3DVECTOR const& vPos)
{
	return g_pRL->m_SceneMgr.GetRegionDataByMapID(dwMapID, vPos);
}

HRESULT KGameWorldHandler::ExecuteCommand(LPCTSTR szCommand, void* pResult)
{
    int nRetCode = false;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(szCommand);

    nRetCode = ExecuteRLCommand(szCommand, pResult);
    KG_PROCESS_SUCCESS(nRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::PlayAnimation(DWORD dwScene, DWORD dwActor, DWORD dwTarget, BOOL bTurnToTarget, DWORD dwAnimationID)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;

    KG_USE_ARGUMENT(dwScene);
    KG_USE_ARGUMENT(dwTarget);
    KG_USE_ARGUMENT(bTurnToTarget);

    // TODO: turn to target

    pRLCharacter = GetRLCharacter(dwActor);
    KG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE,
        RL_ANIMATION_UP_UNKNOWN, RL_ANIMATION_DOWN_UNKNOWN);
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::StopAnimation(DWORD dwScene, DWORD dwActor)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;

    KG_USE_ARGUMENT(dwScene);

    pRLCharacter = GetRLCharacter(dwActor);
    KG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->ResetAnimation();
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::PlaySpeech(DWORD dwScene, DWORD dwActor, DWORD dwTarget, BOOL bTurnToTarget, LPCTSTR pszFilePath)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;

    KG_USE_ARGUMENT(dwScene);
    KG_USE_ARGUMENT(dwTarget);
    KG_USE_ARGUMENT(bTurnToTarget);

    // TODO: turn to target

    pRLCharacter = GetRLCharacter(dwActor);
    KG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->PlaySpeech(dwTarget, bTurnToTarget, pszFilePath);
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::StopSpeech(DWORD dwScene, DWORD dwActor)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLCharacter* pRLCharacter = NULL;

    KG_USE_ARGUMENT(dwScene);

    pRLCharacter = GetRLCharacter(dwActor);
    KG_PROCESS_ERROR(pRLCharacter);

    nRetCode = pRLCharacter->StopSpeech();
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KGameWorldHandler::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int nRetCode = false;
    HRESULT hrResult = E_FAIL;

    switch (uMsg)
    {
    case WM_ACTIVATEAPP:
        g_pRL->m_bActivated = wParam != 0;
        break;
    }

    if (g_pRL->m_CurrentTraceOption.bTraceMsg)
    {
        nRetCode = TraceMsg(uMsg, wParam, lParam);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KGameWorldHandler::GetCameraInitParams(
    int *pnCameraResetMode, float *pfCameraDragSpeed, float *pfCameraMaxDistance, float *pfCameraResetSpeed)
{
    int nResult = false;
    int nCameraResetMode = 0;
    float fCameraDragSpeed = 0.0f;
    float fCameraMaxDistance = 0.0f;
    float fCameraResetSpeed = 0.0f;

    KGLOG_PROCESS_ERROR(pnCameraResetMode);
    KGLOG_PROCESS_ERROR(pfCameraDragSpeed);
    KGLOG_PROCESS_ERROR(pfCameraMaxDistance);
    KGLOG_PROCESS_ERROR(pfCameraResetSpeed);

    nCameraResetMode = (int)g_pRL->m_TableList.GetNumber(RL_NUMBER_CAMERA_RESET_MODE);
    fCameraDragSpeed = g_pRL->m_TableList.GetNumber(RL_NUMBER_CAMERA_DRAG_SPEED);
    fCameraMaxDistance = g_pRL->m_TableList.GetNumber(RL_NUMBER_CAMERA_MAX_DISTANCE);
    fCameraResetSpeed = g_pRL->m_TableList.GetNumber(RL_NUMBER_CAMERA_RESET_SPEED);

    *pnCameraResetMode = nCameraResetMode;
    *pfCameraDragSpeed = fCameraDragSpeed;
    *pfCameraMaxDistance = fCameraMaxDistance;
    *pfCameraResetSpeed = fCameraResetSpeed;

    nResult = true;
Exit0:
    return nResult;
}

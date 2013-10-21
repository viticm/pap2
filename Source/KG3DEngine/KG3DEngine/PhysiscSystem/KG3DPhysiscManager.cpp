
#include "Stdafx.h"
#include "NxPhysics.h"
#include "KG3DPhysiscScene.h"
#include "KG3DPhysiscManager.h"
#include "KG3DCamera.h"
#include "KG3DGraphicsTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void KG3DPhysisErrorReport::reportError(NxErrorCode code, const char *message, const char* file, int line)
{
    //this should be routed to the application specific error handling. If this gets hit
    //then you are in most cases using the SDK wrong and you need to debug your code!
    //however, code may  just be a warning or information.

    ASSERT(message);
    if (strcmp(message, "CUDA not available") != 0)
    {
        KGLogPrintf(KGLOG_ERR, "[Physics] ErrCode=%d, Msg=%s", code, message);
    }
}
NxAssertResponse KG3DPhysisErrorReport::reportAssertViolation(const char *message, const char *file,int line)
{
    //this should not get hit by a properly debugged SDK!
    ASSERT(message);
    KGLogPrintf(KGLOG_ERR, "[Physics] ASSERT Msg=%s", message);
    ASSERT(false && "[Physics] ASSERT");
    return NX_AR_CONTINUE;
}

void KG3DPhysisErrorReport::print(const char *message)
{
    ASSERT(message);
    KGLogPrintf(KGLOG_INFO, "[Physics] %s", message);
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
KG3DPhysiscManager::KG3DPhysiscManager(void)
{
    m_pPhysicsSDK = NULL;
    m_nDebuggerIsConnectedFlag = false;
}

KG3DPhysiscManager::~KG3DPhysiscManager(void)
{

    ASSERT(m_listPhysicsScene.empty());
}

HRESULT KG3DPhysiscManager::Init()
{
    HRESULT hrResult = E_FAIL;
    NxRemoteDebugger *pRemoteDebugger = NULL;
    NxSDKCreateError CreateErrorCode;

    // Create the physics SDK
	m_pPhysicsSDK = NxCreatePhysicsSDK(
        NX_PHYSICS_SDK_VERSION, 
        NULL, 
        &m_ErrorReport, 
        NxPhysicsSDKDesc(),
        &CreateErrorCode
    );

	if (!m_pPhysicsSDK) 
	{
		KGLogPrintf(KGLOG_ERR, "物理引擎初始化失败，ErrorCode=%d", CreateErrorCode);
		KG_PROCESS_ERROR(false);
	}
    // -------- Set remote debugger information
    pRemoteDebugger = m_pPhysicsSDK->getFoundationSDK().getRemoteDebugger();
    ASSERT(pRemoteDebugger);

    pRemoteDebugger->connect("localhost", NX_DBG_DEFAULT_PORT);
    m_nDebuggerIsConnectedFlag = pRemoteDebugger->isConnected();
    if (m_nDebuggerIsConnectedFlag)
    {
        //"this" is just used as an identifier for the camera, could be any value, 
        // as long as it is unique. 
        pRemoteDebugger->createObject(this, NX_DBG_OBJECTTYPE_CAMERA, "KG3DEngine Camera", NX_DBG_EVENTMASK_EVERYTHING);
        pRemoteDebugger->writeParameter(NxVec3(0,0,0), this, true, "Origin", NX_DBG_EVENTMASK_EVERYTHING);
        pRemoteDebugger->writeParameter(NxVec3(1,0,0), this, true, "Target", NX_DBG_EVENTMASK_EVERYTHING);
        pRemoteDebugger->writeParameter(NxVec3(0,1,0), this, true, "Up",     NX_DBG_EVENTMASK_EVERYTHING);
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (m_nDebuggerIsConnectedFlag)
        {
            //Remove the camera object from the VRD
            m_pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->removeObject(this, NX_DBG_EVENTMASK_EVERYTHING);
            
            m_pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->disconnect();
            m_nDebuggerIsConnectedFlag = false;
        }
        if (m_pPhysicsSDK)
        {
            m_pPhysicsSDK->release();
            m_pPhysicsSDK = NULL;
        }
    }
	return hrResult;
}

HRESULT KG3DPhysiscManager::UnInit()
{
    ASSERT(m_listPhysicsScene.empty());

    if (m_nDebuggerIsConnectedFlag)
    {
        //Remove the camera object from the VRD
        m_pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->removeObject(this, NX_DBG_EVENTMASK_EVERYTHING);

        m_pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->disconnect();
        m_nDebuggerIsConnectedFlag = false;
    }
    if (m_pPhysicsSDK)
    {
        m_pPhysicsSDK->release();
        m_pPhysicsSDK = NULL;
    }
	return S_OK;
}

HRESULT KG3DPhysiscManager::NewOnePhysiscScene(KG3DPhysiscScene **ppScene)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	KG3DPhysiscScene *pPhysicsScene = NULL;
    int nPhysicsSceneInitFlag = false;
    
    KGLOG_PROCESS_ERROR(ppScene);
    KGLOG_PROCESS_ERROR(!(*ppScene));

    ASSERT(m_pPhysicsSDK);

    pPhysicsScene = new KG3DPhysiscScene();
	KGLOG_PROCESS_ERROR(pPhysicsScene);

    hrRetCode = _ForceAllSceneFinishSimulate();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pPhysicsScene->Init(m_pPhysicsSDK);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    nPhysicsSceneInitFlag = true;

	m_listPhysicsScene.push_back(pPhysicsScene);

	(*ppScene) = pPhysicsScene;

	hrResult =  S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nPhysicsSceneInitFlag)
        {
            pPhysicsScene->Unint(m_pPhysicsSDK);
            nPhysicsSceneInitFlag = false;
        }
        KG_DELETE(pPhysicsScene);
    }
	return hrResult;
}

HRESULT KG3DPhysiscManager::DeleteOnePhysiscScene(KG3DPhysiscScene *pScene)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    KGLOG_PROCESS_ERROR(pScene);

    hrRetCode = _ForceAllSceneFinishSimulate();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    m_listPhysicsScene.remove(pScene);
    
    pScene->Unint(m_pPhysicsSDK);    
    KG_DELETE(pScene);

	hrResult = S_OK;
Exit0:
	return hrResult;
}
HRESULT KG3DPhysiscManager::UpdateRemoteDebuggerCamera()
{
    HRESULT hrResult = E_FAIL;
    NxRemoteDebugger *pRemoteDebugger = NULL;
    KG3DCamera *pCamera = NULL;

    KG_PROCESS_SUCCESS(!m_nDebuggerIsConnectedFlag);

    pRemoteDebugger = m_pPhysicsSDK->getFoundationSDK().getRemoteDebugger();
    ASSERT(pRemoteDebugger);
  
    pCamera = g_cGraphicsTool.GetCamera();
    KG_PROCESS_SUCCESS(!pCamera);

    {
        D3DXVECTOR3 LockAtPos = pCamera->GetLookAtPosition();
        D3DXVECTOR3 CameraPos = pCamera->GetPosition();
        NxVec3 Origin(CameraPos.x / 100, CameraPos.y / 100, CameraPos.z / 100);// convert from centimeter to meter
        NxVec3 Target(LockAtPos.x / 100, LockAtPos.y / 100, LockAtPos.z / 100);
        pRemoteDebugger->writeParameter(Origin, this, false, "Origin", NX_DBG_EVENTMASK_EVERYTHING);
        pRemoteDebugger->writeParameter(Target, this, false, "Target", NX_DBG_EVENTMASK_EVERYTHING);
    }

Exit1:
    hrResult = S_OK;
//Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscManager::_ForceAllSceneFinishSimulate()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    list<KG3DPhysiscScene *>::iterator it;

    for (it = m_listPhysicsScene.begin(); it != m_listPhysicsScene.end(); ++it)
    {
        KG3DPhysiscScene *pScene = *it;
        hrRetCode = pScene->ForceFinishSimulate();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }
    hrResult = S_OK;
Exit0:
    return hrResult;
}


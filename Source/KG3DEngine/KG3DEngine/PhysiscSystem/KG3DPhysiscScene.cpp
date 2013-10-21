#include "StdAfx.h"
#include "KG3DPhysiscScene.h"
#include "KG3DPhysiscManager.h"
#include "../MathTool.h"
#include "../KG3DEngineManager.h"
#include "../KG3DGraphicsTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PHYSISC_PRARM_FILE_NAME "data\\public\\physics.ini"

////////////////////////////////////////////////////////////////////////////////
// Extract the part that the physical engine's pose needs from the d3d the matrix
HRESULT _ToNxMat34(const D3DXMATRIX &crD3DXMatrix, NxMat34* pRetNxMat34, D3DXMATRIX *pRetRevertMatrix)
{
    static int s_InitRote = false;
    static D3DXMATRIX s_matRot;
    D3DXMATRIX  matConverted;
    D3DXVECTOR3 vTran;
    D3DXVECTOR3 vScal;
 
    ASSERT(pRetNxMat34);

    // rotation
    if (!s_InitRote)
    {
        D3DXMatrixRotationZ(&s_matRot, -0.5f * D3DX_PI); // covert y to x
        s_InitRote = true;
    }
    D3DXMatrixMultiply(&matConverted, &s_matRot, &crD3DXMatrix);  
    
    // remove scale part
    MatrixExract(vScal, vTran, matConverted, &matConverted);
    matConverted._41 = vTran.x;
    matConverted._42 = vTran.y;
    matConverted._43 = vTran.z;

    // normalize
    D3DXQUATERNION Quat;
    NxQuat nxTempForNormalize;
    D3DXQuaternionRotationMatrix(&Quat, &matConverted);
    nxTempForNormalize.setXYZW((float *)&Quat);
    nxTempForNormalize.normalize();

    if (pRetRevertMatrix)
    {
        NxMat34 Temp;
        Temp.M.fromQuat(nxTempForNormalize);
        Temp.t.set((float *)vTran);
        Temp.getColumnMajor44((float *)matConverted);

        D3DXMatrixInverse(pRetRevertMatrix, NULL, &matConverted);
        D3DXMatrixMultiply(pRetRevertMatrix, &crD3DXMatrix, pRetRevertMatrix);  
    }

    // to nxmat34
    pRetNxMat34->M.fromQuat(nxTempForNormalize);
    pRetNxMat34->t.x = vTran.x / 100; // convert from centimeter to meter
    pRetNxMat34->t.y = vTran.y / 100;
    pRetNxMat34->t.z = vTran.z / 100;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT _FromNxMat34(D3DXMATRIX *pRetD3DXMatrix, const NxMat34& nxmat, const D3DXMATRIX *pcRevertMatrix)
{
    ASSERT(pRetD3DXMatrix);

    nxmat.getColumnMajor44((float *)pRetD3DXMatrix);
    pRetD3DXMatrix->_41 *= 100; // convert from meter to centimeter
    pRetD3DXMatrix->_42 *= 100;
    pRetD3DXMatrix->_43 *= 100;

    if (pcRevertMatrix)
    {
        D3DXMatrixMultiply(pRetD3DXMatrix, pcRevertMatrix, pRetD3DXMatrix);        
    }
    
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
KG3DPhysiscScene::KG3DPhysiscScene(void):
    m_vCameraPos(0, 0, 0),
    m_vCamera2LookAt(0, 0, 0)
{
    m_pNxScene = NULL;
    m_nIsSimulating = false;
    m_dwDeltaGameTime = 0;

    m_uGameFrameCount = 0;
    m_uPhysicsFrameCount = 0;
    m_dwGameTimeTotalSimulate = 0;
    m_dwPhysicsTimeTotalSimulate = 0;

    memset(&m_PerformanceAdjust, 0, sizeof(m_PerformanceAdjust));
    m_PerformanceAdjust.fMaxSolverIterationCount = 40.0f;

    memset(&m_DebugInfo, 0, sizeof(m_DebugInfo));
    memset(&m_GlobalConfig, 0, sizeof(m_GlobalConfig));
}

KG3DPhysiscScene::~KG3DPhysiscScene(void)
{
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::Init(NxPhysicsSDK *pPhysicsSDK)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    NxSceneDesc sceneDesc;
    SYSTEM_INFO sysInfo;

    ASSERT(pPhysicsSDK);
    ASSERT(!m_pNxScene);

    hrRetCode = _ReloadPrarm();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    sceneDesc.setToDefault();
    sceneDesc.gravity.y   = -8;

    m_pNxScene = pPhysicsSDK->createScene(sceneDesc);
    KGLOG_PROCESS_ERROR(m_pNxScene);

    m_pNxScene->setGroupCollisionFlag(0, 0, false);

    ::GetSystemInfo(&sysInfo);
    if (sysInfo.dwNumberOfProcessors > 1)
    {
        m_PerformanceAdjust.fNeedAdjustLowLevel  = 0.5f;
        m_PerformanceAdjust.fNeedAdjustHighLevel = 1.0f;
    }
    else
    {
        m_PerformanceAdjust.fNeedAdjustLowLevel  = 0.1f;
        m_PerformanceAdjust.fNeedAdjustHighLevel = 0.4f;
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (m_pNxScene)
        {
            pPhysicsSDK->releaseScene(*m_pNxScene);
            m_pNxScene = NULL;
        }
    }
    return hrResult;
}

HRESULT KG3DPhysiscScene::Unint(NxPhysicsSDK *pPhysicsSDK)
{
    ASSERT(pPhysicsSDK);
    
    ASSERT(!m_nIsSimulating && "should cann KG3DPhysiscManager::_ForceAllSceneFinishSimulate() first"); 

    _ProcessCommandQueue();

    ASSERT(m_FlexibleBodySet.empty());

    if (m_pNxScene)
    {
        pPhysicsSDK->releaseScene(*m_pNxScene);
        m_pNxScene = NULL;
    }
    m_mapPhysicsParam.clear();
    return S_OK;
}
HRESULT KG3DPhysiscScene::ForceFinishSimulate()
{
    if (m_nIsSimulating)
    {
        ASSERT(m_pNxScene);
        m_pNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
        m_nIsSimulating = false;
    }
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::FrameMove(
    DWORD dwDeltaTime, const D3DXVECTOR3 &crvCameraPos, const D3DXVECTOR3 &crvLookAtPos
)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

	DWORD dwStartTime = timeGetTime();
    KG_PROCESS_ERROR(m_pNxScene);

    ++m_uGameFrameCount;
    m_dwDeltaGameTime += dwDeltaTime;

    m_vCameraPos     = crvCameraPos;
    m_vCamera2LookAt = crvLookAtPos - crvCameraPos;
    
    // 这一帔取上一帖的结果，而不是完全异步。
    if (m_nIsSimulating && (m_pNxScene->fetchResults(NX_RIGID_BODY_FINISHED, false)))
    {
        m_nIsSimulating = false;

        const NxProfileData *pcProfileData = m_pNxScene->readProfileData(true);
        ASSERT(pcProfileData);
        const NxProfileZone *pcProfileZone = pcProfileData->getNamedZone(NX_PZ_TOTAL_SIMULATION);
        ASSERT(pcProfileZone);

        ++m_uPhysicsFrameCount;
        m_dwPhysicsTimeTotalSimulate += pcProfileZone->hierTime;
    }

    if (m_GlobalConfig.nShowDebugInfoTime > 0)
    {
        hrRetCode = _ShowDebugInfo(dwDeltaTime);
        //KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }
    
    KG_PROCESS_SUCCESS(m_nIsSimulating);
    
    // -------------------------- update physics system --------------------------
    if (m_GlobalConfig.nAutoRefreshTime > 0)
    {
        hrRetCode = _AutoRefresh();
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    hrRetCode = _ProcessCommandQueue();
    //KGLOG_COM_PROCESS_ERROR(hrRetCode);

    if (m_GlobalConfig.nAutoAdjustPerformance)
    {
        hrRetCode = _AutoAdjustPerformance();
        //KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    if (m_dwDeltaGameTime > 0) // a very high FPS in Model Editor can make this is 0 in some frame
    {
        float fSimulateTimeInSecond = m_dwDeltaGameTime / 1000.0f;
        float fSubstepTime = fSimulateTimeInSecond / m_GlobalConfig.nSubstepPerFrame;
        m_pNxScene->setTiming(fSubstepTime, m_GlobalConfig.nSubstepPerFrame, NX_TIMESTEP_FIXED);

        m_pNxScene->simulate(fSimulateTimeInSecond); 
        m_pNxScene->flushStream();
        m_nIsSimulating = true;

        m_dwGameTimeTotalSimulate += m_dwDeltaGameTime;
        m_dwDeltaGameTime = 0;
    }

    hrRetCode = g_cEngineManager.m_pPhysicsManager->UpdateRemoteDebuggerCamera();
    //KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost >= 20)
	{
		KGLogPrintf(KGLOG_ERR, "TimeOptimize %d KG3DPhysiscScene::FrameMove.",dwCost);
	}
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::Refresh()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    std::set<_FLEXIBLE_BODY *>::iterator it;
    std::set<_FLEXIBLE_BODY *>::iterator itEnd;

    hrRetCode = _ReloadPrarm();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    // recreate flexible body
    itEnd = m_FlexibleBodySet.end();
    for (it = m_FlexibleBodySet.begin(); it != itEnd; ++it)
    {
        _FLEXIBLE_BODY *pFlexibleBody = *it;
        ASSERT(pFlexibleBody);

        if (pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_UPDATE)
        {
            m_CmdQueue.push(COMMAND_QUEUE_NODE(CMD_REFRESH_FLEXIBLE, pFlexibleBody));
        }
    }

    memset(&m_DebugInfo, 0, sizeof(m_DebugInfo));

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_AutoRefresh()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nRetCode = false;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    FILETIME  ftWriteTime;
    static FILETIME s_ftLastWriteTime = {0};
    static DWORD    s_dwLastCheckTime = 0;
    DWORD dwCurTime = ::timeGetTime();

    ASSERT(m_GlobalConfig.nAutoRefreshTime > 0);

    KG_PROCESS_SUCCESS((dwCurTime - s_dwLastCheckTime) < (unsigned)m_GlobalConfig.nAutoRefreshTime * 1000);
    s_dwLastCheckTime = dwCurTime;

    // only support file on dist, not support package
    hFile = ::CreateFile(
        PHYSISC_PRARM_FILE_NAME, 
        FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    KG_PROCESS_SUCCESS(hFile == INVALID_HANDLE_VALUE); // maybe use package, not as error

    nRetCode = ::GetFileTime(hFile, NULL, NULL, &ftWriteTime);
    KGLOG_PROCESS_ERROR(nRetCode);

    if (s_ftLastWriteTime.dwHighDateTime == 0 && s_ftLastWriteTime.dwLowDateTime == 0)
    {
        // is first time run, just record current file time
        s_ftLastWriteTime = ftWriteTime;
        KG_PROCESS_SUCCESS(true);
    }
    else
    {
        nRetCode = ::CompareFileTime(&s_ftLastWriteTime, &ftWriteTime);
        KG_PROCESS_SUCCESS(nRetCode >= 0);
        s_ftLastWriteTime = ftWriteTime;
    }

    // file changed
    hrRetCode = Refresh();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
    if (hFile != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_ReloadPrarm()
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    int nDefaultLoadFlag = false;;
    TCHAR szSection[128];
    KG3DPhysiscParam Param;
    IIniFile *pIniFile = NULL;

    m_mapPhysicsParam.clear();

    pIniFile = g_OpenIniFile(PHYSISC_PRARM_FILE_NAME);
    KGLOG_PROCESS_ERROR(pIniFile);

    nRetCode = pIniFile->GetInteger("Global", "nAutoRefreshTime", 0, &m_GlobalConfig.nAutoRefreshTime);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_GlobalConfig.nAutoRefreshTime >= 0);

    nRetCode = pIniFile->GetBool("Global", "bAutoAdjustPerformance", &m_GlobalConfig.nAutoAdjustPerformance);
    KGLOG_PROCESS_ERROR(nRetCode);
   
    nRetCode = pIniFile->GetInteger("Global", "nShowDebugInfoTime", 0, &m_GlobalConfig.nShowDebugInfoTime);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_GlobalConfig.nShowDebugInfoTime >= 0);

    nRetCode = pIniFile->GetInteger("Global", "nSubstepPerFrame", 4, &m_GlobalConfig.nSubstepPerFrame);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_GlobalConfig.nSubstepPerFrame > 0);

    szSection[0] = '\0';
    while (pIniFile->GetNextSection(szSection, szSection))
    {
        int id = atoi(szSection);
        if ((id <= 0) && (strcmp(szSection, "00") != 0))
            continue;

        nRetCode = pIniFile->GetFloat(szSection, "fRadius", 0.0f, &Param.fRadius);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.fRadius > 0.0f);
        nRetCode = pIniFile->GetFloat(szSection, "fDensity", 0.0f, &Param.fDensity);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.fDensity > 0.0f);
        nRetCode = pIniFile->GetInteger(szSection, "nSolverIterationCount", 0, &Param.nSolverIterationCount);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.nSolverIterationCount > 0);
        nRetCode = pIniFile->GetFloat(szSection, "fLinearDamping", 0.0f, &Param.fLinearDamping);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.fLinearDamping >= 0.0f);
        nRetCode = pIniFile->GetFloat(szSection, "fAngularDamping", 0.0f, &Param.fAngularDamping);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.fAngularDamping >= 0.0f);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableTwistLimit", &Param.SphericalJoint.bEnableTwistLimit);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitLow.fValue", 0.0f, &Param.SphericalJoint.TwistLimitLow.fValue);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitLow.fValue >= -180.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitLow.fValue <=  180.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitLow.fHardness", 0.0f, &Param.SphericalJoint.TwistLimitLow.fHardness);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitLow.fHardness >= 0.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitLow.fHardness <= 1.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitLow.fRestitution", 0.0f, &Param.SphericalJoint.TwistLimitLow.fRestitution);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitLow.fRestitution >= 0.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitLow.fRestitution <= 1.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitHigh.fValue", 0.0f, &Param.SphericalJoint.TwistLimitHigh.fValue);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitHigh.fValue >= -180.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitHigh.fValue <=  180.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitHigh.fHardness", 0.0f, &Param.SphericalJoint.TwistLimitHigh.fHardness);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitHigh.fHardness >= 0.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitHigh.fHardness <= 1.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistLimitHigh.fRestitution", 0.0f, &Param.SphericalJoint.TwistLimitHigh.fRestitution);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitHigh.fRestitution >= 0.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistLimitHigh.fRestitution <= 1.0f);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableSwingLimit", &Param.SphericalJoint.bEnableSwingLimit);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingLimit.fValue", 0.0f, &Param.SphericalJoint.SwingLimit.fValue);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.SwingLimit.fValue >= -180.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.SwingLimit.fValue <=  180.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingLimit.fHardness", 0.0f, &Param.SphericalJoint.SwingLimit.fHardness);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.SwingLimit.fHardness >= 0.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.SwingLimit.fHardness <= 1.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingLimit.fRestitution", 0.0f, &Param.SphericalJoint.SwingLimit.fRestitution);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.SwingLimit.fRestitution >= 0.0f);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.SwingLimit.fRestitution <= 1.0f);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableTwistSpring", &Param.SphericalJoint.bEnableTwistSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistSpring.fSpring", 0.0f, &Param.SphericalJoint.TwistSpring.fSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistSpring.fSpring >= 0.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.TwistSpring.fDamper", 0.0f, &Param.SphericalJoint.TwistSpring.fDamper);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.TwistSpring.fDamper >= 0.0f);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableSwingSpring", &Param.SphericalJoint.bEnableSwingSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingSpring.fSpring", 0.0f, &Param.SphericalJoint.SwingSpring.fSpring);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.SwingSpring.fSpring >= 0.0f);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.SwingSpring.fDamper", 0.0f, &Param.SphericalJoint.SwingSpring.fDamper);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.SwingSpring.fDamper >= 0.0f);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pIniFile->GetBool(szSection, "SphericalJoint.bEnableJointSpring", &Param.SphericalJoint.bEnableJointSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.JointSpring.fSpring", 0.0f, &Param.SphericalJoint.JointSpring.fSpring);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.JointSpring.fSpring >= 0.0f);
        nRetCode = pIniFile->GetFloat(szSection, "SphericalJoint.JointSpring.fDamper", 0.0f, &Param.SphericalJoint.JointSpring.fDamper);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(Param.SphericalJoint.JointSpring.fDamper >= 0.0f);

        // cm to m
        //Param.fRadius  /= 100; // do it in _CreateCapsule        
        Param.fDensity *= 1000; // g/cm^3 to kg/m^3
        Param.SphericalJoint.TwistLimitLow.fValue  = D3DXToRadian(Param.SphericalJoint.TwistLimitLow.fValue);
        Param.SphericalJoint.TwistLimitHigh.fValue = D3DXToRadian(Param.SphericalJoint.TwistLimitHigh.fValue);
        Param.SphericalJoint.SwingLimit.fValue     = D3DXToRadian(Param.SphericalJoint.SwingLimit.fValue);

        m_mapPhysicsParam[id] = Param;
        if (id == 0)
            nDefaultLoadFlag = true;
    }
    KGLOG_PROCESS_ERROR(nDefaultLoadFlag && "can't load default section [00]");

    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(pIniFile);
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
KG3DPhysiscParam* KG3DPhysiscScene::_QueryPhysiscParam(LPCTSTR szBoneName)
{
    KG3DPhysiscParam *pParam  = NULL;
    std::map<int, KG3DPhysiscParam>::iterator it;

    const char *pszID = NULL;
    int nID = 0;
    
    pszID = strrchr(szBoneName, '_');

    if (!pszID || (*(++pszID)) == '\0')
        return &(m_mapPhysicsParam[0]);

    nID = atoi(pszID);

    it = m_mapPhysicsParam.find(nID);

    if (it == m_mapPhysicsParam.end())
    {
        KGLogPrintf(KGLOG_INFO, "[FlexibleBody] Can't found Physics Param, Bone name (%s) error", szBoneName);
        KG_PROCESS_ERROR(false);
    }
    pParam = &(it->second);

Exit0:
    return pParam;
}
////////////////////////////////////////////////////////////////////////////////
NxSphericalJoint* KG3DPhysiscScene::_CreateSphericalJoint(
    NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis,
    KG3DPhysiscParam* pParam
)
{
    ASSERT(pParam);

    if(!m_pNxScene)
        return NULL;
    NxSphericalJointDesc sphericalDesc;
    sphericalDesc.actor[0] = a0;
    sphericalDesc.actor[1] = a1;
    sphericalDesc.setGlobalAnchor(globalAnchor);
    sphericalDesc.setGlobalAxis(globalAxis);

    if(pParam->SphericalJoint.bEnableTwistLimit)
    {
        sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
        sphericalDesc.twistLimit.low.value        = pParam->SphericalJoint.TwistLimitLow.fValue;
        sphericalDesc.twistLimit.low.hardness     = pParam->SphericalJoint.TwistLimitLow.fHardness;
        sphericalDesc.twistLimit.low.restitution  = pParam->SphericalJoint.TwistLimitLow.fRestitution;
        sphericalDesc.twistLimit.high.value       = pParam->SphericalJoint.TwistLimitHigh.fValue;
        sphericalDesc.twistLimit.high.hardness    = pParam->SphericalJoint.TwistLimitHigh.fHardness;
        sphericalDesc.twistLimit.high.restitution = pParam->SphericalJoint.TwistLimitHigh.fRestitution;
    }

    if(pParam->SphericalJoint.bEnableSwingLimit)
    {
        sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
        sphericalDesc.swingLimit.value       = pParam->SphericalJoint.SwingLimit.fValue;
        sphericalDesc.swingLimit.hardness    = pParam->SphericalJoint.SwingLimit.fHardness;
        sphericalDesc.swingLimit.restitution = pParam->SphericalJoint.SwingLimit.fRestitution;
    }

    if(pParam->SphericalJoint.bEnableTwistSpring)
    {
        sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
        sphericalDesc.twistSpring.spring = pParam->SphericalJoint.TwistSpring.fSpring;
        sphericalDesc.twistSpring.damper = pParam->SphericalJoint.TwistSpring.fDamper;
    }

    if(pParam->SphericalJoint.bEnableSwingSpring)
    {
        sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
        sphericalDesc.swingSpring.spring = pParam->SphericalJoint.SwingSpring.fSpring;
        sphericalDesc.swingSpring.damper = pParam->SphericalJoint.SwingSpring.fDamper;
    }

    if(pParam->SphericalJoint.bEnableJointSpring)
    {
        sphericalDesc.flags |= NX_SJF_JOINT_SPRING_ENABLED;
        sphericalDesc.jointSpring.spring = pParam->SphericalJoint.JointSpring.fSpring;
        sphericalDesc.jointSpring.damper = pParam->SphericalJoint.JointSpring.fDamper;
    }

    sphericalDesc.projectionMode     = NX_JPM_POINT_MINDIST;
    sphericalDesc.projectionDistance = 
        (a1->getShapes()[0])->isCapsule()->getRadius() * 0.5f;

    return (NxSphericalJoint*)m_pNxScene->createJoint(sphericalDesc);
}

////////////////////////////////////////////////////////////////////////////////
NxActor* KG3DPhysiscScene::_CreateCapsule(
    const char cszName[], const NxMat34& Pose, NxReal height, const KG3DPhysiscParam &crParam
)
{
    NxActor *pActor = NULL;

    ASSERT(cszName);
    ASSERT(m_pNxScene);
    
    NxReal radius = crParam.fRadius / 100;
    height /= 100;    // convert from centimeter to meter

    // Add a single-shape actor to the scene
    NxActorDesc actorDesc;
    NxBodyDesc bodyDesc;
    bodyDesc.solverIterationCount = crParam.nSolverIterationCount;

    // The actor has one shape, a capsule
    NxCapsuleShapeDesc capsuleDesc;
    capsuleDesc.height      = height;
    capsuleDesc.radius      = radius;
    capsuleDesc.localPose.t = NxVec3(0, (radius + height * 0.5f), 0);
    capsuleDesc.skinWidth   = radius * 0.5f;
    actorDesc.shapes.pushBack(&capsuleDesc);
    actorDesc.body       = &bodyDesc;
    actorDesc.density    = crParam.fDensity;
    actorDesc.globalPose = Pose;
    actorDesc.name       = cszName;
    
    pActor = m_pNxScene->createActor(actorDesc);
    return pActor;    
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_ShowDebugInfo(DWORD dwDeltaTime)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    hrRetCode = _ShowDebugInfo_Scene(dwDeltaTime);
    KG_COM_PROCESS_ERROR(hrRetCode);
    
    hrRetCode = _ShowDebugInfo_Joint();
    KG_COM_PROCESS_ERROR(hrRetCode);
   
    hrRetCode = _ShowDebugInfo_Actor();
    KG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_ShowDebugInfo_Scene(DWORD dwDeltaTime)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    char szMessage[256];

    ASSERT(m_pNxScene);

    if (!m_nIsSimulating)
    {
        float fDeltaGameTime = (float)(m_dwGameTimeTotalSimulate - m_DebugInfo.dwLastGameTime);
        ASSERT(m_GlobalConfig.nShowDebugInfoTime > 0);
        if (fDeltaGameTime >= m_GlobalConfig.nShowDebugInfoTime * 1000)
        {
            float fDeltaGameFrame    = (float)(m_uGameFrameCount - m_DebugInfo.uLastGameFrame);
            float fDeltaPhysicsFrame = (float)(m_uPhysicsFrameCount - m_DebugInfo.uLastPhysicsFrame);
            float fDeltaPhysicsTime  = (float)(m_dwPhysicsTimeTotalSimulate - m_DebugInfo.dwLastPhysicsTime);

            m_DebugInfo.fAvgGameTime = fDeltaGameTime / fDeltaGameFrame;
            m_DebugInfo.fAvgGameFPS  = fDeltaGameFrame * 1000 / fDeltaGameTime; // millisecond to second

            m_DebugInfo.fAvgPhysicsTime = fDeltaPhysicsTime  / 1000 / fDeltaPhysicsFrame; // microsecond to millisecond
            m_DebugInfo.fAvgPhysicsFPS  = fDeltaPhysicsFrame * 1000 / fDeltaGameTime;  // not use fDeltaPhysicsTime

            m_DebugInfo.dwLastPhysicsTime = m_dwPhysicsTimeTotalSimulate;
            m_DebugInfo.dwLastGameTime    = m_dwGameTimeTotalSimulate;
            m_DebugInfo.uLastGameFrame    = m_uGameFrameCount;
            m_DebugInfo.uLastPhysicsFrame = m_uPhysicsFrameCount;
        }
    }
  
    if (m_DebugInfo.fAvgGameTime > 0)
    {
        snprintf(
            szMessage, sizeof(szMessage),
            "PhysicsScene [%p] : GameFPS=%.1f, PhysicsFPS=%.1f, GameTime=%.1f, PhysicsTime=%.3f (1:%.2f)",
            m_pNxScene,
            m_DebugInfo.fAvgGameFPS,
            m_DebugInfo.fAvgPhysicsFPS, 
            m_DebugInfo.fAvgGameTime,
            m_DebugInfo.fAvgPhysicsTime,
            m_DebugInfo.fAvgPhysicsTime / m_DebugInfo.fAvgGameTime
        );
        szMessage[sizeof(szMessage) - 1] = '\0';

        hrRetCode = g_cGraphicsTool.DrawDebugText(szMessage, -1, NULL, D3DXCOLOR(0xFF, 0, 0, 0xFF));
        KG_COM_PROCESS_ERROR(hrRetCode);
    }
    {
        float fMaxTimeStep = 0.0f;
        unsigned uMaxIter = 0;
        NxTimeStepMethod  eMethod = NX_TIMESTEP_FIXED;
        m_pNxScene->getTiming(fMaxTimeStep, uMaxIter, eMethod);

        snprintf(
            szMessage, sizeof(szMessage),
            "MaxTimeStep=1/%u, MaxIterCount = %u, Actor=%d, Join=%d",
            (unsigned)(1.0f / fMaxTimeStep + 0.5f), // round to unsigned integer
            uMaxIter,
            m_pNxScene->getNbActors(),
            m_pNxScene->getNbJoints()
        );
        szMessage[sizeof(szMessage) - 1] = '\0';

        hrRetCode = g_cGraphicsTool.DrawDebugText(szMessage, -1, NULL, D3DXCOLOR(0xFF, 0, 0, 0xFF));
        KG_COM_PROCESS_ERROR(hrRetCode);
    }
    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_ShowDebugInfo_Joint()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    NxSphericalJointDesc JointDesc;
    char szMessage[256];
   
    ASSERT(m_pNxScene);

    if ((m_uPhysicsFrameCount > 0) && (m_DebugInfo.uLastPhysicsFrame == m_uPhysicsFrameCount))
    {
        m_DebugInfo.fAvgJointAxisDeviation = 0.0f;
        m_DebugInfo.fAvgJointAxisDeviationPercent = 0.0f;
        if (m_DebugInfo.uDeltaJointCount > 0)
        {
            m_DebugInfo.fAvgJointAxisDeviation = 
                m_DebugInfo.fDeltaJointAxisDeviation / m_DebugInfo.uDeltaJointCount;
            m_DebugInfo.fAvgJointAxisDeviationPercent = 
                m_DebugInfo.fDeltaJointAxisDeviationPercent / m_DebugInfo.uDeltaJointCount;
        }

        m_DebugInfo.fAvgJointAxisDeviation2 = 0.0f;
        m_DebugInfo.fAvgJointAxisDeviationPercent2 = 0.0f;
        if (m_DebugInfo.uDeltaJointDeviationCount > 0)
        {
            m_DebugInfo.fAvgJointAxisDeviation2 = 
                m_DebugInfo.fDeltaJointAxisDeviation / m_DebugInfo.uDeltaJointDeviationCount;
            m_DebugInfo.fAvgJointAxisDeviationPercent2 = 
                m_DebugInfo.fDeltaJointAxisDeviationPercent / m_DebugInfo.uDeltaJointDeviationCount;
        }
 
        m_DebugInfo.uDeltaJointCount = 0;
        m_DebugInfo.uDeltaJointDeviationCount = 0;
        m_DebugInfo.fDeltaJointAxisDeviation = 0;
        m_DebugInfo.fDeltaJointAxisDeviationPercent = 0;
    }

    m_pNxScene->resetJointIterator();
    while(m_pNxScene->getNextJoint())
    {
        NxJoint *pJoint = m_pNxScene->getNextJoint();
        if (!pJoint)
            break;

        NxSphericalJoint *pSphericalJoint = pJoint->isSphericalJoint();
        if (!pSphericalJoint)
            continue;

        pSphericalJoint->saveToDesc(JointDesc);

        NxMat34 ActorPose  = JointDesc.actor[1]->getGlobalPose();
        NxVec3 JointAnchor = pSphericalJoint->getGlobalAnchor();

        float fAnchorDeviation = JointAnchor.distance(ActorPose.t);
        m_DebugInfo.fMaxAnchorDeviation = max(m_DebugInfo.fMaxAnchorDeviation, fAnchorDeviation);

        // --------------------  --------------------
        if (!(JointDesc.flags | NX_SJF_SWING_LIMIT_ENABLED))
            continue;

        NxVec3 JointAxis = pSphericalJoint->getGlobalAxis();
        NxVec3 ActorAxis;
        ActorPose.M.multiply(NxVec3(0, 1, 0), ActorAxis);

        float fAxisDeviation = 
            acos(fabs(ActorAxis.dot(JointAxis))) - JointDesc.swingLimit.value;
        if (fAxisDeviation > 0) // out of limit
        {
            float fAxisDeviationPercent = (fAxisDeviation / JointDesc.swingLimit.value) * 100.0f;

            m_DebugInfo.fDeltaJointAxisDeviationPercent += fAxisDeviationPercent;
            m_DebugInfo.fDeltaJointAxisDeviation        += fAxisDeviation;
            ++m_DebugInfo.uDeltaJointDeviationCount;

            if (fAxisDeviationPercent > m_DebugInfo.fMaxAxisDeviationPercent)
            {
                m_DebugInfo.fMaxAxisDeviationPercent = fAxisDeviationPercent;
                m_DebugInfo.fMaxAxisDeviation        = fAxisDeviation;
            }
        }
        ++m_DebugInfo.uDeltaJointCount;
    }

    snprintf(
        szMessage, sizeof(szMessage),
        "Joint Anchor: Max Deviation=%.2fcm",
        m_DebugInfo.fMaxAnchorDeviation * 100.0f
    );
    szMessage[sizeof(szMessage) - 1] = '\0';
    
    hrRetCode = g_cGraphicsTool.DrawDebugText(szMessage, -1, NULL, D3DXCOLOR(0xFF, 0, 0, 0xFF));
    KG_COM_PROCESS_ERROR(hrRetCode);

    snprintf(
        szMessage, sizeof(szMessage),
        "Joint Axis: "
        "Avg Deviation%3.0f%%(%4.1fDegree), "
        "Deviation Avg%3.0f%%(%4.1fDegree), "
        "Max Deviation%3.0f%%(%4.1fDegree)",
        m_DebugInfo.fAvgJointAxisDeviationPercent, 
        D3DXToDegree(m_DebugInfo.fAvgJointAxisDeviation),
        m_DebugInfo.fAvgJointAxisDeviationPercent2, 
        D3DXToDegree(m_DebugInfo.fAvgJointAxisDeviation2),
        m_DebugInfo.fMaxAxisDeviationPercent,
        D3DXToDegree(m_DebugInfo.fMaxAxisDeviation)
    );
    szMessage[sizeof(szMessage) - 1] = '\0';
    
    hrRetCode = g_cGraphicsTool.DrawDebugText(szMessage, -1, NULL, D3DXCOLOR(0xFF, 0, 0, 0xFF));
    KG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_ShowDebugInfo_Actor()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    unsigned uActorCount = 0;
    unsigned uSleepActorCount = 0;
    NxActor **ppActor = NULL;
    char szMessage[256];

    ASSERT(m_pNxScene);
  
    if ((m_uPhysicsFrameCount > 0) && (m_DebugInfo.uLastPhysicsFrame == m_uPhysicsFrameCount))
    {
        m_DebugInfo.fAvgIterCount = 0.0f;

        if (m_DebugInfo.uDeltaActorCount > 0)
        {
            m_DebugInfo.fAvgIterCount = (float)m_DebugInfo.uDeltaIterCount / (float)m_DebugInfo.uDeltaActorCount;
        }
        m_DebugInfo.uDeltaActorCount = 0;
        m_DebugInfo.uDeltaIterCount = 0;
        m_DebugInfo.uMaxIterCount = 0;
        m_DebugInfo.uMinIterCount = 0;
    }

    m_pNxScene->resetJointIterator();
    uActorCount = m_pNxScene->getNbActors();
    ppActor     = m_pNxScene->getActors();
    for (unsigned i = 0; i < uActorCount; ++i)
    {
        ASSERT(ppActor[i]);
		if(!ppActor[i]->isDynamic())
			continue;
        unsigned uIterCount = ppActor[i]->getSolverIterationCount();
        if (ppActor[i]->isSleeping())
        {
            ++uSleepActorCount;
        }
        
        if (uIterCount > m_DebugInfo.uMaxIterCount)
        {
            m_DebugInfo.uMaxIterCount = uIterCount;
        }
        if ((uIterCount < m_DebugInfo.uMinIterCount) || (m_DebugInfo.uMinIterCount == 0))
        {
            m_DebugInfo.uMinIterCount = uIterCount;
        }
        m_DebugInfo.uDeltaIterCount += uIterCount;
    }
    m_DebugInfo.uDeltaActorCount += uActorCount;

    snprintf(
        szMessage, sizeof(szMessage),
        "Actor Solver Iteration Count: Max=%u, Min=%u, Avg=%.2f, SleepActor=%u",
        m_DebugInfo.uMaxIterCount,
        m_DebugInfo.uMinIterCount,
        m_DebugInfo.fAvgIterCount,
        uSleepActorCount
    );
    szMessage[sizeof(szMessage) - 1] = '\0';
    
    hrRetCode = g_cGraphicsTool.DrawDebugText(szMessage, -1, NULL, D3DXCOLOR(0xFF, 0, 0, 0xFF));
    KG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_AutoAdjustPerformance()
{
    HRESULT hrResult = E_FAIL;
    float fDeltaGameTime     = 0.0f;
    float fPhysicsTimeRate   = 0.0f;
    NxActor **pActorArray = NULL;
    unsigned uActorCount = 0;
    NxVec3 nvCameraPos;
    NxVec3 nvCamera2LookAt;

    ASSERT(m_pNxScene);
    ASSERT(!m_nIsSimulating);

    fDeltaGameTime = (float)(m_dwGameTimeTotalSimulate - m_PerformanceAdjust.dwLastGameTime);
    KG_PROCESS_SUCCESS(fDeltaGameTime < 1000);

    {
        float fDeltaGameFrame    = (float)(m_uGameFrameCount - m_PerformanceAdjust.uLastGameFrame);
        float fDeltaPhysicsFrame = (float)(m_uPhysicsFrameCount - m_PerformanceAdjust.uLastPhysicsFrame);
        float fDeltaPhysicsTime  = (float)(m_dwPhysicsTimeTotalSimulate - m_PerformanceAdjust.dwLastPhysicsTime);
        float fAvgGameTime    = fDeltaGameTime / fDeltaGameFrame;
        float fAvgPhysicsTime = fDeltaPhysicsTime  / 1000 / fDeltaPhysicsFrame; // microsecond to millisecond
    
        fPhysicsTimeRate = fAvgPhysicsTime / fAvgGameTime;

        m_PerformanceAdjust.dwLastPhysicsTime = m_dwPhysicsTimeTotalSimulate;
        m_PerformanceAdjust.dwLastGameTime    = m_dwGameTimeTotalSimulate;
        m_PerformanceAdjust.uLastGameFrame    = m_uGameFrameCount;
        m_PerformanceAdjust.uLastPhysicsFrame = m_uPhysicsFrameCount;
    }

    // -------------------- adjust MaxTimeStep -------------------- 
    if (fPhysicsTimeRate < m_PerformanceAdjust.fNeedAdjustLowLevel)
    {
        m_PerformanceAdjust.fMaxSolverIterationCount = 40.0f;
    } 
    else if (fPhysicsTimeRate > m_PerformanceAdjust.fNeedAdjustHighLevel)
    {
        if (m_PerformanceAdjust.fMaxSolverIterationCount > 5.0f)
            m_PerformanceAdjust.fMaxSolverIterationCount *= 0.8f; // 5 * 0.8 = 4
    }
    // -------------------- adjust SolverIterationCount -------------------- 
    nvCameraPos = NxVec3(
        m_vCameraPos.x / 100,
        m_vCameraPos.y / 100,
        m_vCameraPos.z / 100
    );
    nvCamera2LookAt = NxVec3(
        m_vCamera2LookAt.x / 100,
        m_vCamera2LookAt.y / 100,
        m_vCamera2LookAt.z / 100
    );
    uActorCount = m_pNxScene->getNbActors();
    pActorArray = m_pNxScene->getActors();
    for (unsigned i = 0; i < uActorCount; ++i)
    {
        NxActor *pActor = pActorArray[i];

		if(!pActor->isDynamic())
			continue;

        NxVec3 ActorPos = pActor->getGlobalPosition();

        NxVec3 nvCamera2Actor = ActorPos - nvCameraPos;
        float fCos = nvCamera2Actor.dot(nvCamera2LookAt);
        if (fCos <= 0)
        {            
            pActor->putToSleep();// not visible
            continue;
        }

        float fDistance = nvCamera2Actor.magnitude(); 
        unsigned uIterationCount = 1;
        if (fDistance < 80.0f)
        {
            float fDamping = 0.0f;
            if (fDistance <= 20)        // [0,20]: 
                fDamping = 0.67f * (fDistance / 20); // Damping 67%
            else if (fDistance <= 30)   // [21,30]
                fDamping = 0.67f + 0.20f * ((fDistance - 20) / 10.0f); // Damping 20%
            else if (fDistance <= 50)   // [31,50]
                fDamping = 0.87f + 0.10f * ((fDistance - 30) / 20.0f); // Damping 10%
            else
                fDamping = 0.97f;

            uIterationCount = (unsigned)(m_PerformanceAdjust.fMaxSolverIterationCount * (1 - fDamping) + 0.5f);
            if (uIterationCount == 0)
                uIterationCount = 1;
        }

        pActor->setSolverIterationCount(uIterationCount);
    }

Exit1:
    hrResult = S_OK;
//Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_UpdateFlexibleBody_FromDriverBone(_FLEXIBLE_BODY *pFlexibleBody)
{
    HRESULT hrResult = E_FAIL;
    D3DXMATRIX matInvers;
    D3DXMATRIX matMovement;
   
    ASSERT(pFlexibleBody);

    // set pose to the init state, and move to current world position
    for (unsigned i = 0; i < pFlexibleBody->uInputCount; ++i)
    {
        _FLEXIBLE_BODY::INPUT *pInput = pFlexibleBody->pInputs + i;

        D3DXMatrixInverse(&matInvers, NULL, &(pInput->pDriverBone->InitPose));
        D3DXMatrixMultiply(&matMovement, &matInvers, &(pInput->pDriverBone->WorldPose));
        for (size_t j = 0; j < pInput->pDriverBone->vecBoneChain.size(); ++j)
        {
            FLEXIBLE_BONE_CHAIN &Chian = pInput->pDriverBone->vecBoneChain[j];
            _FLEXIBLE_BODY::OUTPUT *pOutput = pFlexibleBody->pOutputs + Chian.uFirstBoneIndex;
            for (size_t k = 0; k < Chian.uBoneCount; ++k)
            {
                D3DXMatrixMultiply(
                    &(pOutput->pNormalBone->WorldPose), 
                    &(pOutput->pNormalBone->InitPose), 
                    &matMovement
                );
                ++pOutput;
            }
        }
    }

    hrResult = S_OK;
//Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_ProcessCommandQueue()
{
    while(!(m_CmdQueue.empty()))
    {
        COMMAND_QUEUE_NODE &cmd = m_CmdQueue.front();
        _FLEXIBLE_BODY *pFlexibleBody = (_FLEXIBLE_BODY *)cmd.pvData;

        switch (cmd.eCmd)
        {
        case CMD_CREATE_FLEXIBLE:
            // can be skip by the following release command in the queue
            if (pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_CREATE)
            {
                _DoCreateFlexibleBody(pFlexibleBody);
            }
        	break;

        case CMD_REFRESH_FLEXIBLE:
            // can be skip by the following release command in the queue
            if (pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_UPDATE)
            {
                _ReleaseNxObject(pFlexibleBody);
                pFlexibleBody->ePhisicsStatue = _FLEXIBLE_BODY::WAIT_CREATE;
                _DoCreateFlexibleBody(pFlexibleBody);
            }
        	break;

        case CMD_UPDATE_FLEXIBLE:
            // can be skip by the following release command in the queue
            if (pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_UPDATE)
            {
                _DoUpdateFlexibleBody(pFlexibleBody);
            }
            break;

        case CMD_MOVE_FLEXIBLE:
            // can be skip by the following release command in the queue
            if (pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_UPDATE)
            {
                _DoSetFlexibleBodyPos(pFlexibleBody);
            }
            break;

        case CMD_RELEASE_FLEXIBLE:
            _DoReleaseFlexibleBody(pFlexibleBody);
            pFlexibleBody = NULL;

            break;
        
        default:
            ASSERT(false);
        }

        m_CmdQueue.pop();
    }

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::CreateFlexibleBody(
    unsigned uDriverBoneCount, FLEXIBLE_DRIVER_BONE *aDriverBonesPtr[],
    unsigned uNormalBoneCount, FLEXIBLE_NORMAL_BONE *aNormalBonesPtr[],
    HANDLE *phRetFlexibleBody
)
{
    HRESULT hrResult  = E_FAIL;
    unsigned i = 0;
    _FLEXIBLE_BODY *pFlexibleBody = NULL;
    unsigned uBufferSize = 0;
    std::set<_FLEXIBLE_BODY *>::_Pairib InsertResult;

    KGLOG_PROCESS_ERROR(uDriverBoneCount > 0);
    KGLOG_PROCESS_ERROR(aDriverBonesPtr);
    KGLOG_PROCESS_ERROR(uNormalBoneCount > 0);
    KGLOG_PROCESS_ERROR(aNormalBonesPtr);
    KGLOG_PROCESS_ERROR(phRetFlexibleBody);

    uBufferSize =
        sizeof(_FLEXIBLE_BODY) +
        sizeof(_FLEXIBLE_BODY::INPUT)  * uDriverBoneCount +
        sizeof(_FLEXIBLE_BODY::OUTPUT) * uNormalBoneCount;

    pFlexibleBody = (_FLEXIBLE_BODY *)new char[uBufferSize];
    KGLOG_PROCESS_ERROR(pFlexibleBody);
    memset(pFlexibleBody, 0, uBufferSize);

    pFlexibleBody->ePhisicsStatue = _FLEXIBLE_BODY::WAIT_CREATE;
    pFlexibleBody->nVisiable    = false;
    pFlexibleBody->uInputCount  = uDriverBoneCount;
    pFlexibleBody->pInputs      = (_FLEXIBLE_BODY::INPUT  *)(pFlexibleBody + 1);
    pFlexibleBody->uOutputCount = uNormalBoneCount;
    pFlexibleBody->pOutputs     = (_FLEXIBLE_BODY::OUTPUT *)(pFlexibleBody->pInputs + pFlexibleBody->uInputCount);

    for (i = 0; i < pFlexibleBody->uInputCount; ++i)
    {
        pFlexibleBody->pInputs[i].pDriverBone = aDriverBonesPtr[i];
    }

    for (i = 0; i < pFlexibleBody->uOutputCount; ++i)
    {
        pFlexibleBody->pOutputs[i].pNormalBone = aNormalBonesPtr[i];
    }

    m_CmdQueue.push(COMMAND_QUEUE_NODE(CMD_CREATE_FLEXIBLE, pFlexibleBody));
    
    InsertResult = m_FlexibleBodySet.insert(pFlexibleBody);
    ASSERT(InsertResult.second);

    *phRetFlexibleBody = (HANDLE)pFlexibleBody;
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(pFlexibleBody);
    }

    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_DoCreateFlexibleBody(_FLEXIBLE_BODY *pFlexibleBody)
{
    HRESULT hrResult  = E_FAIL;
    NxMat34 nxmatPose;
    NxActor *pPreActor = NULL;
    float fHeight = 0;

    ASSERT(pFlexibleBody);
    ASSERT(pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_CREATE);

    for (unsigned i = 0; i < pFlexibleBody->uInputCount; ++i)
    {
        _FLEXIBLE_BODY::INPUT *pInput = pFlexibleBody->pInputs + i;
        FLEXIBLE_DRIVER_BONE *pDriverBone = pInput->pDriverBone;

        // -------------------- create driver actor -------------------- 
        KG3DPhysiscParam* pParam = _QueryPhysiscParam(pDriverBone->pcszName);
        KGLOG_PROCESS_ERROR(pParam);

        _ToNxMat34(pInput->pDriverBone->InitPose, &nxmatPose, &(pInput->matRevert)); // don't use pDriverBone->WorldPose, may be changed again when create command still in queue

        pInput->pActor = _CreateCapsule(
            pDriverBone->pcszName, nxmatPose, 1.0f, *pParam
        );
        KGLOG_PROCESS_ERROR(pInput->pActor);

        pInput->pActor->raiseBodyFlag(NX_BF_KINEMATIC);
        //pInput->matInitPose = pDriverBone->WorldPose; already saved at KG3DPhysiscScene::_CreateFlexibleBody

        // -------------------- create chains -------------------- 
        for (size_t j = 0; j < pDriverBone->vecBoneChain.size(); ++j)
        {
            FLEXIBLE_BONE_CHAIN &Chian = pDriverBone->vecBoneChain[j];
            _FLEXIBLE_BODY::OUTPUT *pOutput = pFlexibleBody->pOutputs + Chian.uFirstBoneIndex;
            pPreActor = pInput->pActor;
            for (size_t k = 0; k < Chian.uBoneCount; ++k)
            {
                FLEXIBLE_NORMAL_BONE *pNormalBone = pOutput->pNormalBone;
                // create actor
                KG3DPhysiscParam* pParam = _QueryPhysiscParam(pNormalBone->pcszName);
                KGLOG_PROCESS_ERROR(pParam);

                _ToNxMat34(pOutput->pNormalBone->InitPose, &nxmatPose, &(pOutput->matRevert));

                fHeight = max(1.0f, (pNormalBone->fLength - pParam->fRadius * 2));
                pOutput->pActor = _CreateCapsule(
                    pNormalBone->pcszName, nxmatPose, fHeight, *pParam
                );
                KGLOG_PROCESS_ERROR(pOutput->pActor);

                //pOutput->pActor->raiseBodyFlag(NX_BF_KINEMATIC);
                pOutput->pActor->setLinearDamping(pParam->fLinearDamping);
                pOutput->pActor->setAngularDamping(pParam->fAngularDamping);

                // create join
                NxVec3 globalAnchor = pOutput->pActor->getGlobalPosition();
                NxVec3 localAxis(0, 1, 0);
                NxVec3 globalAxis;
                NxMat33 nxGlobalOrientation = pOutput->pActor->getGlobalOrientation();
                nxGlobalOrientation.multiply(localAxis, globalAxis);
                globalAxis.normalize();

                pOutput->pJoint = _CreateSphericalJoint(
                    pPreActor, pOutput->pActor, globalAnchor, globalAxis, pParam
                );
                KGLOG_PROCESS_ERROR(pOutput->pJoint);

                pPreActor = pOutput->pActor;
                ++pOutput;
            }
        }
    }

    pFlexibleBody->ePhisicsStatue = _FLEXIBLE_BODY::WAIT_UPDATE;
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        _ReleaseNxObject(pFlexibleBody);
        //pFlexibleBody = NULL; // not destory pFlexibleBody
    }
    return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::UpdateFlexibleBody(HANDLE hFlexibleBody)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    _FLEXIBLE_BODY *pFlexibleBody = NULL;
    _UPDATE_TYPE eUpdateType;
    _PHYSX_COMMAND eCmd;


    KGLOG_PROCESS_ERROR((_FLEXIBLE_BODY *)hFlexibleBody);
    pFlexibleBody = (_FLEXIBLE_BODY *)hFlexibleBody;

    hrRetCode = _GetFlexibleBodyUpdateType(pFlexibleBody, &eUpdateType, &eCmd);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    {
        static _UPDATE_TYPE s_eUpdateType = _UPDATE_FROM_DRIVER_BONE;
        static _PHYSX_COMMAND s_eCmd = _PHYSX_MOVE;
        static int s_nDebugForce = false;
        if (s_nDebugForce)
        {
            eUpdateType = s_eUpdateType;
            eCmd = s_eCmd;
        }
    }

    if (eUpdateType == _UPDATE_FROM_DRIVER_BONE)
    {
        hrRetCode = _UpdateFlexibleBody_FromDriverBone(pFlexibleBody);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }
    else
    {
        hrRetCode = _UpdateFlexibleBody_FromNx(pFlexibleBody);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }
    
    switch (eCmd)
    {
    case _PHYSX_NOTHING:
        break;

    case _PHYSX_UPDATE:
        m_CmdQueue.push(COMMAND_QUEUE_NODE(CMD_UPDATE_FLEXIBLE, pFlexibleBody));
        break;

    case _PHYSX_MOVE:
        m_CmdQueue.push(COMMAND_QUEUE_NODE(CMD_MOVE_FLEXIBLE, pFlexibleBody));
        break;
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_GetFlexibleBodyUpdateType(
    _FLEXIBLE_BODY *pFlexibleBody, 
    _UPDATE_TYPE *pnRetUpdateType,
    _PHYSX_COMMAND *pnRetPhysXCmd
)
{
    HRESULT hrResult  = E_FAIL;
    int nVisible = false;
    int nBigMove = false;
    _FLEXIBLE_BODY::INPUT *pInput = NULL;
    D3DXVECTOR3 vFlexibleBody;
    D3DXVECTOR3 vCamera2FlexibleBody;
    float fCos = 0.0f;
    NxVec3 nxOldPostion;

    ASSERT(pFlexibleBody);
    ASSERT(pnRetUpdateType);
    ASSERT(pnRetPhysXCmd);
    //
    KG_PROCESS_SUCCESS(pFlexibleBody->ePhisicsStatue != _FLEXIBLE_BODY::WAIT_UPDATE);

    ASSERT(pFlexibleBody->uInputCount > 0);
    pInput = &(pFlexibleBody->pInputs[0]);
    
    //
    vFlexibleBody = D3DXVECTOR3(
        pInput->pDriverBone->WorldPose._41,
        pInput->pDriverBone->WorldPose._42,
        pInput->pDriverBone->WorldPose._43
    );
    vCamera2FlexibleBody = vFlexibleBody - m_vCameraPos;
    fCos = D3DXVec3Dot(&vCamera2FlexibleBody, &m_vCamera2LookAt);
    nVisible = fCos > 0;
    KG_PROCESS_SUCCESS(!nVisible);
    
    // 物体从0点放到场景中时，由于移动距离过长会导致柔体狂抖
    nxOldPostion = pInput->pActor->getGlobalPosition();
    if (
        (fabs(vFlexibleBody.x - nxOldPostion.x * 100) > 1000) ||
        (fabs(vFlexibleBody.y - nxOldPostion.y * 100) > 1000) ||
        (fabs(vFlexibleBody.z - nxOldPostion.z * 100) > 1000)
    )
    {
        nBigMove  = true;
    }  

Exit1:
    *pnRetUpdateType = _UPDATE_FROM_DRIVER_BONE;
    *pnRetPhysXCmd   = _PHYSX_NOTHING;
    if (nVisible)
    {
        if (!nBigMove && pFlexibleBody->nVisiable)
        {
            *pnRetUpdateType = _UPDATE_FROM_PHYSX;
            *pnRetPhysXCmd   = _PHYSX_UPDATE;
        } 
        else
        {
            *pnRetPhysXCmd   = _PHYSX_MOVE;
        }
    }
    pFlexibleBody->nVisiable = nVisible;

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_UpdateFlexibleBody_FromNx(_FLEXIBLE_BODY *pFlexibleBody)
{
    HRESULT hrResult  = E_FAIL;
    D3DXMATRIX matPhysicsPose;
    NxMat34    nxmatPhysicsPose;
    D3DXMATRIX matInvers;
    D3DXMATRIX matFix;

    ASSERT(pFlexibleBody);
    ASSERT(pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_UPDATE);

    for (unsigned i = 0; i < pFlexibleBody->uInputCount; ++i)
    {
        _FLEXIBLE_BODY::INPUT *pInput = pFlexibleBody->pInputs + i;

        nxmatPhysicsPose = pInput->pActor->getGlobalPose();
        _FromNxMat34(&matPhysicsPose, nxmatPhysicsPose, &(pInput->matRevert));

        //-- because current frame get last frame's input's result,
        //-- so we should fix the position of the output date to match the current model position
        D3DXMatrixInverse(&matInvers, NULL, &matPhysicsPose);
        D3DXMatrixMultiply(&matFix, &matInvers, &(pInput->pDriverBone->WorldPose));

        // get last frame's result
        for (size_t j = 0; j < pInput->pDriverBone->vecBoneChain.size(); ++j)
        {
            FLEXIBLE_BONE_CHAIN &Chian = pInput->pDriverBone->vecBoneChain[j];
            _FLEXIBLE_BODY::OUTPUT *pOutput = pFlexibleBody->pOutputs + Chian.uFirstBoneIndex;
            for (size_t k = 0; k < Chian.uBoneCount; ++k)
            {
                nxmatPhysicsPose = pOutput->pActor->getGlobalPose();
                _FromNxMat34(&matPhysicsPose, nxmatPhysicsPose, &(pOutput->matRevert));
                D3DXMatrixMultiply(&(pOutput->pNormalBone->WorldPose), &matPhysicsPose, &matFix);

                ++pOutput;
            }
        }
    }

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_DoUpdateFlexibleBody(_FLEXIBLE_BODY *pFlexibleBody)
{
    HRESULT hrResult  = E_FAIL;
    unsigned i = 0;
    NxMat34 vNxMat;

    ASSERT(pFlexibleBody);
    ASSERT(pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_UPDATE);

    // update physics system
    for (i = 0; i < pFlexibleBody->uInputCount; ++i)
    {
        _FLEXIBLE_BODY::INPUT *pInput = &(pFlexibleBody->pInputs[i]);

        _ToNxMat34(pInput->pDriverBone->WorldPose, &vNxMat, NULL);
        pInput->pActor->moveGlobalPose(vNxMat);
    }

    hrResult = S_OK;
//Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_DoSetFlexibleBodyPos(_FLEXIBLE_BODY *pFlexibleBody)
{
    HRESULT hrResult  = E_FAIL;
    int nRetCode = false;
    NxMat34    nxNewPose;
    NxMat34    nxOldPose;
    NxMat34    NxMovement;
    NxMat34    NxInvers;

    ASSERT(pFlexibleBody);
    ASSERT(pFlexibleBody->ePhisicsStatue == _FLEXIBLE_BODY::WAIT_UPDATE);

    for (unsigned i = 0; i < pFlexibleBody->uInputCount; ++i)
    {
        _FLEXIBLE_BODY::INPUT *pInput = pFlexibleBody->pInputs + i;

        nxOldPose = pInput->pActor->getGlobalPose();

        _ToNxMat34(pInput->pDriverBone->WorldPose, &nxNewPose, NULL);
        pInput->pActor->setGlobalPose(nxNewPose);

        nRetCode = nxOldPose.getInverse(NxInvers);
        KGLOG_PROCESS_ERROR(nRetCode);
        //NxMovement.multiply(NxInvers, nxNewPose); // 和DirectX的方向不一样，要交换下位置算的结果才对:(
        NxMovement.multiply(nxNewPose, NxInvers);

        for (size_t j = 0; j < pInput->pDriverBone->vecBoneChain.size(); ++j)
        {
            FLEXIBLE_BONE_CHAIN &Chian = pInput->pDriverBone->vecBoneChain[j];
            _FLEXIBLE_BODY::OUTPUT *pOutput = pFlexibleBody->pOutputs + Chian.uFirstBoneIndex;
            for (size_t k = 0; k < Chian.uBoneCount; ++k)
            {

                nxOldPose = pOutput->pActor->getGlobalPose();
                //nxNewPose.multiply(nxOldPose, NxMovement);
                nxNewPose.multiply(NxMovement, nxOldPose);
                pOutput->pActor->setGlobalPose(nxNewPose);

                ++pOutput;
            }
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::ReleaseFlexibleBody(HANDLE hFlexibleBody)
{
    HRESULT hrResult  = E_FAIL;
    _FLEXIBLE_BODY *pFlexibleBody = NULL;

    KGLOG_PROCESS_ERROR((_FLEXIBLE_BODY *)hFlexibleBody);
    pFlexibleBody = (_FLEXIBLE_BODY *)hFlexibleBody;
    
    pFlexibleBody->ePhisicsStatue = _FLEXIBLE_BODY::WAIT_RELEASE;
    m_CmdQueue.push(COMMAND_QUEUE_NODE(CMD_RELEASE_FLEXIBLE, pFlexibleBody));

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_ReleaseNxObject(_FLEXIBLE_BODY *pFlexibleBody)
{
    HRESULT hrResult  = E_FAIL;
    unsigned i = 0;

    ASSERT(pFlexibleBody);

    for (i = 0; i < pFlexibleBody->uOutputCount; ++i)
    {
        _FLEXIBLE_BODY::OUTPUT *pOutput = pFlexibleBody->pOutputs + i;
        if (pOutput->pJoint)
        {
            m_pNxScene->releaseJoint(*(pOutput->pJoint));
            pOutput->pJoint = NULL;
        }
        if (pOutput->pActor)
        {
            m_pNxScene->releaseActor(*(pOutput->pActor));
            pOutput->pActor = NULL;
        }
    }
    for (i = 0; i < pFlexibleBody->uInputCount; ++i)
    {
        _FLEXIBLE_BODY::INPUT *pInput = pFlexibleBody->pInputs + i;
        if (pInput->pActor)
        {
            m_pNxScene->releaseActor(*(pInput->pActor));
            pInput->pActor = NULL;
        }
    }
    //pFlexibleBody->ePhisicsStatue = _FLEXIBLE_BODY::WAIT_RELEASE;

    hrResult = S_OK;
//Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DPhysiscScene::_DoReleaseFlexibleBody(_FLEXIBLE_BODY *pFlexibleBody)
{
    HRESULT hrResult  = E_FAIL;

    ASSERT(pFlexibleBody);

    _ReleaseNxObject(pFlexibleBody);

    m_FlexibleBodySet.erase(pFlexibleBody);
    KG_DELETE_ARRAY(pFlexibleBody);

    hrResult = S_OK;
//Exit0:
    return hrResult;
}

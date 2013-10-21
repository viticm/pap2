#include "stdafx.h"
#include "./krltrackcamera.h"
#include "./ikgrlcamera.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"
#include "KG3DEngine/KG3DInterface.h"

#define RL_SPRING_CONSTANT 3.0f    // 弹性系数
#define RL_DAMPING_COEFFICIENT sqrtf(RL_SPRING_CONSTANT) * 2.0f  // 阻尼系数
#define RL_SPRING_EPSILON 0.1f

KRLTrackCamera::KRLTrackCamera()
{
    m_vTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vExpectTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vPosition = D3DXVECTOR3(0.0f, 500.0f, 1000.0f);
    m_vExpectFront = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vLookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    m_fExpectDistance = 0.0f;
    m_fDistance = 0.0f;

    m_fCurrentVelocity = 0.0f;

    m_fWheelOffset = 0.0f;
       
/*    m_nObstrucFlag = false; */
    m_nCollisionFlag = false;

    m_nUseFlexFlag = false;
    m_nUpdateFlag = true;     
    m_nCollisionFlag = false;
}

KRLTrackCamera::~KRLTrackCamera()
{
}

HRESULT KRLTrackCamera::UpdateCamera(D3DXVECTOR3 vTarget, D3DXVECTOR3 vPosition, D3DXVECTOR3 vFront, 
                                     float fWheelOffset, float fLookAtOffset)
{
    HRESULT hrResult = E_FAIL;

    m_vTarget = vTarget;
    m_vExpectTarget = vPosition + vFront;

    m_fExpectDistance = D3DXVec3Length(&vFront);
    D3DXVec3Normalize(&m_vExpectFront, &vFront);

    m_vLookAt = m_vExpectTarget + m_vExpectFront * fLookAtOffset;

    m_fWheelOffset = fWheelOffset;

    if (m_fExpectDistance - m_fDistance - m_fWheelOffset < RL_SPRING_EPSILON)
    {
        m_nUseFlexFlag = false;
    }

	// 如果不这么做，刚进游戏，动一下镜头会突然后拉，因为下面用了(m_fExpectDistance + m_fDistance) * 0.5f这个优化
    if (abs(m_fDistance) < FLT_MIN)
    {
        m_fDistance = m_fExpectDistance;
    }

    m_nUpdateFlag = true;    // Camera 需要更新                                      

    hrResult = S_OK;
    return hrResult;
}

HRESULT KRLTrackCamera::FrameMove(KRLCameraParam* pResultParam, float fDeltaTime)
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
    float fDeltaDistance = 0.0f;
    KRLSpringParam RLSpringParam;
    KRLSpringResult RLSpringResult;

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_TRACK_CAMERA);

    KGLOG_PROCESS_ERROR(pResultParam);
    KG_PROCESS_SUCCESS(!m_nUpdateFlag);

    // 利用弹簧系统移动Camera到期待的位置
    if (m_nUseFlexFlag)
    {
        fDeltaDistance = m_fExpectDistance - m_fDistance - m_fWheelOffset;     // 要参与弹簧系统的距离
        ASSERT(fDeltaDistance >= 0.0f);
        KGLOG_CHECK_ERROR(fDeltaDistance >= 0.0f);

        fDeltaTime *= 0.001f;
 
        RLSpringParam.fDistance = fDeltaDistance;
        RLSpringParam.fTime = fDeltaTime;
        RLSpringParam.fVelocity = m_fCurrentVelocity;
        RLSpringParam.fEpsilon = RL_SPRING_EPSILON;

        nRetCode = RunSpringSystem(RLSpringParam, &RLSpringResult);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (!RLSpringResult.nFinished)
        {
            m_fDistance += RLSpringResult.fDistance;
            m_fCurrentVelocity = RLSpringResult.fVelocity;
        }
        else
        {
            StopSpringSystem((m_fExpectDistance + m_fDistance) * 0.5f);
        }
    }
    else
    {
        StopSpringSystem((m_fExpectDistance + m_fDistance) * 0.5f);
    }

    // 遮挡检测
    DetectObstruct(m_fExpectDistance);

    m_vPosition = m_vExpectTarget - m_vExpectFront * m_fDistance;

    m_fWheelOffset = 0.0f;

Exit1:
    hrResult = S_OK;
Exit0:
    pResultParam->vPosition = m_vPosition;
    pResultParam->vLookAt = m_vLookAt;
    pResultParam->vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    pResultParam->vTarget = m_vTarget;

    StopProfile(&g_pRL->m_Profile, KRLPROFILE_TRACK_CAMERA);
    return hrResult;
}

void KRLTrackCamera::DetectObstruct(float fExpectDistance)
{
    int nHasObstruct = false;

    if (m_nCollisionFlag)
    {
        m_nUseFlexFlag = true;

        if (fExpectDistance <= m_fDistance)
        {
            nHasObstruct = true;
        }
        else
        {
#if 0
            // 近点的障碍到远点的障碍，或是同一个障碍镜头在做弹性回归，反正前后两帧都有障碍
            float fCmpDis = 50.f;

            if (m_nObstrucFlag)
                fCmpDis = 100.f;

            float fDis = D3DXVec3LengthSq(&(crvPreCameraPos - m_vPerfectPos));

            if (fDis < fCmpDis * fCmpDis)
                nHasObstruct = true;
            else
#endif
                nHasObstruct = false;
        }
    }

    if (nHasObstruct)
    {
        StopSpringSystem(fExpectDistance);
    }
#if 0
    else
    {
        m_nObstrucFlag   = false; 
    }
#endif
}

int KRLTrackCamera::GetCollisionPosition(
    const D3DXVECTOR3& crvCameraPos,
    const D3DXVECTOR3& crvTargetPos0,
    IKG3DCamera* p3DCamera,
    D3DXVECTOR3* pvCrossPos
)
{
    HRESULT hRetCode = E_FAIL;
    int nRetCode = false;

    D3DXVECTOR3 vSrc[5];
    D3DXVECTOR3 vDst[5];
    D3DXVECTOR3 vCrossPos(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vRightDirection(0.0f, 0.0f, 0.0f);    // 当前的位置的右方向
    D3DXVECTOR3 vUpDirection(0.0f, 0.0f, 0.0f);       // 当前的位置的上方向
    D3DXVECTOR3 vLimitY(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vLimitX(0.0f, 0.0f, 0.0f);
    float fHalfHeight = 0.0f;
    float fHalfWidth = 0.0f;
    float fMinDistance = FLT_MAX;
	D3DXVECTOR3 crvTargetPos = crvTargetPos0 + D3DXVECTOR3(0.001F,0.001F,0.001f);
    int nHasCross = false;
    int i = 0;

    float fFovy    = 0.0f;
    float fAspect = 0.0f;
    float fzNear   = 0.0f;
    float fzFar    = 0.0f;
    
    KGLOG_PROCESS_ERROR(p3DCamera);
    KGLOG_PROCESS_ERROR(pvCrossPos);

    hRetCode = p3DCamera->GetPerspective(&fFovy, &fAspect, &fzNear, &fzFar);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    fHalfHeight = tanf(fFovy * 0.5f) * fzNear;      // half height of the near plan of view frustum
    fHalfWidth = fHalfHeight * fAspect;             // half width of the near plan of view frustum

    hRetCode = p3DCamera->GetUpDirection(&vUpDirection);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = p3DCamera->GetRight(&vRightDirection);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    vLimitY = vUpDirection * fHalfHeight;
    vLimitX = vRightDirection * fHalfWidth;

    // 五射线检测
    vSrc[0] = crvCameraPos;
    vSrc[1] = crvCameraPos + vLimitY - vLimitX;
    vSrc[2] = crvCameraPos + vLimitY + vLimitX;
    vSrc[3] = crvCameraPos - vLimitY - vLimitX;
    vSrc[4] = crvCameraPos - vLimitY + vLimitX;

    vDst[0] = crvTargetPos;
    vDst[1] = crvTargetPos + vLimitY - vLimitX;
    vDst[2] = crvTargetPos + vLimitY + vLimitX;
    vDst[3] = crvTargetPos - vLimitY - vLimitX;
    vDst[4] = crvTargetPos - vLimitY + vLimitX;

    fMinDistance = D3DXVec3Length(&(crvCameraPos - crvTargetPos));
	
	ASSERT(fMinDistance > 0.9f);
	
    for (i = 0; i < 5; i++)
    {
        D3DXVECTOR3 vCross(0.0f, 0.0f, 0.0f);
        float fDistance = 0.0f;

        nRetCode = p3DCamera->GetCrossPosition(vDst[i], vSrc[i], &vCross);
        if (!nRetCode)
            continue;

        fDistance = D3DXVec3Length(&(vDst[i] - vCross));

        if (fDistance < fMinDistance)
        {
            fMinDistance = fDistance;
            nHasCross = true;
        }
    }

    m_nCollisionFlag = false;
    if (nHasCross)
    {
        D3DXVECTOR3 vDirection = crvTargetPos - crvCameraPos;
        D3DXVec3Normalize(&vDirection, &vDirection);

        if (fMinDistance > 18.0f)
            fMinDistance -= 18.0f;

        vCrossPos = crvTargetPos - vDirection * fMinDistance;

        m_nCollisionFlag = true;
    }

    *pvCrossPos = vCrossPos;
Exit0:
    return nHasCross;
}

void KRLTrackCamera::SetCollisionFlag(int nFlag)
{
    m_nCollisionFlag = nFlag;
}

int RunSpringSystem(KRLSpringParam Param, KRLSpringResult* pResult)
{
    int nResult = false;

    float fVelocity = 0.0f;
    float fFlexDistance = 0.0f;
    float fFlexIAccel = 0.0f;
    KRLSpringResult Result;

    KGLOG_PROCESS_ERROR(pResult);
    KGLOG_PROCESS_ERROR(Param.fDistance >= 0.0f);

    // 弦加速度 = (弹性系数 * 线偏移值) - (阻尼系数 * 当前速度)
    fFlexIAccel = (RL_SPRING_CONSTANT * Param.fDistance) - (RL_DAMPING_COEFFICIENT * Param.fVelocity);

    fVelocity = Param.fVelocity + fFlexIAccel * Param.fTime;

    fFlexDistance = fVelocity * Param.fTime;

    if (fFlexDistance <= 0.0f || Param.fDistance <= fFlexDistance + Param.fEpsilon)
    {
        Result.nFinished = true;
        Result.fDistance = 0.0f;
        Result.fVelocity = 0.0f;
    }
    else
    {
        Result.nFinished = false;
        Result.fDistance = fFlexDistance;
        Result.fVelocity = fVelocity;
    }

    *pResult = Result;
    nResult = true;
Exit0:
    return nResult;
}

void KRLTrackCamera::StopSpringSystem(float fDistance)
{
    m_fDistance = fDistance;
    m_nUpdateFlag = false;

    m_fCurrentVelocity = 0.0f;
}

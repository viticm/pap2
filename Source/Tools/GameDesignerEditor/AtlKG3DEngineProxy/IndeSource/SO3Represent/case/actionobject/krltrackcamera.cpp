#include "stdafx.h"
#include "./krltrackcamera.h"
#include "./ikgrlcamera.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"

KRLTrackCamera::KRLTrackCamera()
{
    m_vPosition = D3DXVECTOR3(0.0f, 500.0f, 1000.0f);
    m_vExpectPosition = D3DXVECTOR3(0.0f, 500.0f, 1000.0f);
    m_vExpectLookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vLookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vCurrentVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    m_fFlexOffset = 0.0f;
    m_fSpringK = 1.5f;
    m_fDampingK = sqrtf(1.5f) * 2;

    m_nUseFlexFlag = false;
    m_nUpdateFlag = true;         
    m_nObstrucFlag = false; 
    m_nCollisionFlag = false;
}

KRLTrackCamera::~KRLTrackCamera()
{
}

HRESULT KRLTrackCamera::UpdateCamera(D3DXVECTOR3 vPosition, D3DXVECTOR3 vFront, 
                                     float fFlexOffset, float fLookAtOffset)
{
    HRESULT hrResult = E_FAIL;

    D3DXVec3Normalize(&vFront, &vFront);

    m_vExpectPosition = vPosition;
    m_vExpectLookAt = vPosition + vFront * fLookAtOffset;

    m_fFlexOffset = fFlexOffset;

    m_nUpdateFlag = true;    // Camera 需要更新                                      

    hrResult = S_OK;
    return hrResult;
}

HRESULT KRLTrackCamera::FrameMove(KRLCameraParam* pResultParam, float fDelayTime)
{
    HRESULT hrResult = E_FAIL;

    D3DXVECTOR3 vExpectFront = m_vExpectLookAt - m_vExpectPosition;
    D3DXVECTOR3 vPreCameraPos = m_vPosition;
    float fPreDistance = D3DXVec3Length(&(m_vLookAt - vPreCameraPos));

    KGLOG_PROCESS_ERROR(pResultParam);
    KG_PROCESS_SUCCESS(!m_nUpdateFlag);

    D3DXVec3Normalize(&vExpectFront, &vExpectFront);

    m_vPosition = m_vExpectLookAt - vExpectFront * (fPreDistance + m_fFlexOffset);    // 方向调整到和期望位置一样

    m_fFlexOffset = 0.0f;
    fDelayTime *= 0.0001f;

    // 利用弹簧系统移动Camera到期待的位置
    if (m_nUseFlexFlag)
    {
        D3DXVECTOR3 vFlexIAccel(0.0f, 0.0f, 0.0f);  // 弦加速度
        D3DXVECTOR3 vDisplace(0.0f, 0.0f, 0.0f);    // 线偏移值
        D3DXVECTOR3 vUpdatePos(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 vUpdateDisplace(0.0f, 0.0f, 0.0f);

        vDisplace  = m_vPosition - m_vExpectPosition;

        // 弦加速度 = (-弹性系数 * 线偏移值) - (阻尼系数 * 当前速度)
        vFlexIAccel = (-m_fSpringK * vDisplace) - (m_fDampingK * m_vCurrentVelocity);

        m_vCurrentVelocity = m_vCurrentVelocity + vFlexIAccel * fDelayTime;

        vUpdatePos = m_vPosition + m_vCurrentVelocity * fDelayTime;

        vUpdateDisplace = vUpdatePos - m_vExpectPosition;

        if (EqualDirection(vDisplace, vUpdateDisplace, 0.05f))
        {
            m_vPosition += (vUpdatePos - m_vPosition) * 0.5f;
        }
        else
        {
            m_vPosition += (m_vExpectPosition - m_vPosition) * 0.5f;
            m_nUseFlexFlag = false;
            m_nUpdateFlag = false;
        }
    }
    else
    {
		m_vPosition += (m_vExpectPosition - m_vPosition) * 0.5f;
        m_nUpdateFlag = false;
    }

    // 遮挡检测
    DetectObstruct(vPreCameraPos);

    m_vLookAt = m_vExpectLookAt; 

Exit1:
    hrResult = S_OK;
Exit0:
    pResultParam->vPosition = m_vPosition;
    pResultParam->vLookAt = m_vExpectLookAt;
    pResultParam->vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

    return hrResult;
}

void KRLTrackCamera::DetectObstruct(const D3DXVECTOR3& crvPreCameraPos)
{
    int nHasObstruct = false;

    if (m_nCollisionFlag)
    {
        D3DXVECTOR3 vDirectionC2T = m_vExpectLookAt - m_vPosition;
        D3DXVECTOR3 vDirectionP2T = m_vExpectLookAt - m_vExpectPosition;

        if (D3DXVec3LengthSq(&vDirectionP2T) <= D3DXVec3LengthSq(&vDirectionC2T))
        {
            nHasObstruct = true;
        }
        else
        {
            float fCmpDis = 50.f;

            if (m_nObstrucFlag)
                fCmpDis = 100.f;

            float fDis = D3DXVec3LengthSq(&(crvPreCameraPos - m_vExpectPosition));

            if (fDis < fCmpDis * fCmpDis)
                nHasObstruct = true;
            else
                nHasObstruct = false;
        }
    }

    if (nHasObstruct)
    {
        m_vPosition    = m_vExpectPosition;
        m_vCurrentVelocity   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        m_nUseFlexFlag   = true;
        m_nObstrucFlag   = true;
        m_nUpdateFlag    = false;
    }
    else
    {
        m_nObstrucFlag   = false; 
    }
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
	
	ASSERT(fMinDistance > 1);
	
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

void KRLTrackCamera::SetFlexOffset(float fFlexOffset)
{
    m_fFlexOffset = fFlexOffset;
}

void KRLTrackCamera::SetCollisionFlag(int nFlag)
{
    m_nCollisionFlag = nFlag;
}


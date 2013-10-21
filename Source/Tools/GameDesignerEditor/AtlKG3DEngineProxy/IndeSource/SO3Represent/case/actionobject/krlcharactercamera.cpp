#include "stdafx.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "./krlcharactercamera.h"
#include "./krlscene.h"
#include "./krldoodad.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"
#include "./krlcamera.h"

namespace
{
    float const PLAYER_DIVING_PITCH = -D3DX_PI / 4.0f;            //潜水开启
    float const RL_CHARACTER_EYE_Y = 20.0f;
    float const AUTO_RESET_PITCH_VELOCITY = D3DX_PI / 3000.0f;
    float const AUTO_RESET_YAW_VELOCITY   = D3DX_PI / 48.0f;
    float const CAMERA_ADJUST_CRITICAL_LENGTH = 50.0f;              //镜头策略修正临界值
    float const FIRST_PERSON_CAMERA = 50.0f;                        //第一人称视角距离
    float const ALPHA_DISTANCE_MIN = FIRST_PERSON_CAMERA;
    float const ALPHA_DISTANCE_MAX = 100.0f;
    float const ZOOM_DISTANCE = 90.0f;
    float const SMOOTH_SPEED = 600.0f;
    float const SCALE_MIN = 0.001f;
    float const SCALE_MAX = 1.0f;
    float const ANGLE_EPSILON = D3DX_PI * 0.01f;
    float const FLT_DISTANCE_EPSILON_TO_TARGET = 180.0f;
    float const FLT_DISTANCE_EPSILON_TO_POINT = 30.0f;
    int const INT_DISTANCE_EPSILON = 5;
}

HRESULT KRLCharacterCamera::Reset()
{
    m_nPitchState                   = PITCH_NONE;
    m_nObjectAltitudeType           = ALTITUDE_AIR;
    m_nCameraAltitudeType           = ALTITUDE_AIR;
    m_nCameraMovement               = MOVEMENT_FOLLOW;
    m_nPreCameraMovement            = MOVEMENT_FOLLOW;

    m_fWaterLine = 0.0f;
    m_fLastPitch = 0.0f;
    m_fEarthPitch = 0.0f;
    m_fCharacterDeltaYaw = 0.0f;
    m_fObjectEyeHeight = 0.0f;

    m_fCameraToObjectEyeScale       = 1.0f;

    m_fCurrentZoomLength            = 400.0f;
    m_fNeedSmoothObjectHeight       = 0.0f;
    m_fPitchStartTime               = g_pRL->m_fTime;

    m_fMaxMouseX                    = 0.0f;
    m_fMaxMouseY                    = 0.0f;
    m_fFinalMouseX                  = 0.0f;
    m_fFinalMouseY                  = 0.0f;

    m_bWaterArea = FALSE;
    m_bObjectMoving = FALSE;
    m_bIgnoreDragObject = FALSE;
    m_bControlEnabled = FALSE;
    m_bHasDraggedObject = FALSE;
    m_bFirstPerson = FALSE;
    m_bControlLocked = FALSE;
    m_bNeedSmoothObject = FALSE;
    m_bIsRiding = FALSE;

    m_dwObjectMoveState = cmsInvalid;

    m_vCameraPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vObjectPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    m_fFaceFootDelta = 0.0f;
    m_bUsingFaceFootDelta = FALSE;

    m_bMouseControlMoveEnable = TRUE;

    m_bAutoMove = FALSE;

    m_p3DSFXModel = NULL;

    return S_OK;
}

HRESULT KRLCharacterCamera::SetCamera()
{
    m_ObjectController.SetYaw(0.0f);
    m_RLLocalCharacter.SetFaceYaw(0.0f);
    m_RLLocalCharacter.SetFootYaw(0.0f);
    m_ObjectController.SetPitch(0.0f);
    m_ObjectController.SetRoll(0.0f);

    return S_OK;
}

int KRLCharacterCamera::InitRLCharacter(KMovementController* pCameraController)
{
    int nResult = false;
    KCharacter* pCharacter = NULL;
    float fYawLH = 0.0f;
    float fHeight = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraController);

    pCharacter = m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_pCharacter;
    KGLOG_PROCESS_ERROR(pCharacter);

    fHeight = (float)pCharacter->m_nHeight * ((float)LOGICAL_CELL_CM_LENGTH / CELL_LENGTH / 8.0f);

    m_fObjectEyeHeight = fHeight - RL_CHARACTER_EYE_Y;

    m_CharacterController.Attach(pCharacter);

    fYawLH = GetObjectFaceDirection();

    pCameraController->SetYaw(fYawLH);
    UpdateYaw(fYawLH, FALSE);

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacterCamera::UnInitRLCharacter()
{
    m_RLLocalCharacter.Reset();
    m_CharacterController.Attach(NULL);
}

D3DXVECTOR3 KRLCharacterCamera::GetObjectPosition() const
{
    return m_vObjectPosition;
}

float KRLCharacterCamera::GetObjectFaceDirection() const
{
    return YawRHToLH(D3DX_PI * 2.0f / (float)DIRECTION_COUNT * m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Interpolated.fFaceDirection);
}

int KRLCharacterCamera::Zoom(float fScale, float fMaxCameraDistance)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(fMaxCameraDistance > 0.0f);

    if (fScale <= 1.0f)
    {
        if ((m_fCameraToObjectEyeScale * fMaxCameraDistance > FIRST_PERSON_CAMERA * 3) &&
            (m_fCameraToObjectEyeScale * fMaxCameraDistance - ZOOM_DISTANCE < FIRST_PERSON_CAMERA * 3))
        {
            m_bNeedSmoothObject = TRUE;
        }

        m_fCameraToObjectEyeScale -= ZOOM_DISTANCE / fMaxCameraDistance;
    }
    else
    {
        if ((m_fCameraToObjectEyeScale * fMaxCameraDistance < FIRST_PERSON_CAMERA * 3) &&
            (m_fCameraToObjectEyeScale * fMaxCameraDistance + ZOOM_DISTANCE > FIRST_PERSON_CAMERA * 3))
        {
            m_bNeedSmoothObject = TRUE;
        }

        m_fCameraToObjectEyeScale += ZOOM_DISTANCE / fMaxCameraDistance;
    }

    if (m_fCameraToObjectEyeScale < SCALE_MIN)
        m_fCameraToObjectEyeScale = SCALE_MIN;

    if (m_fCameraToObjectEyeScale > SCALE_MAX)
        m_fCameraToObjectEyeScale = SCALE_MAX;

    nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_ERR, "KRLCharacterCamera::Zoom(%f, %f)\n", fScale, fMaxCameraDistance);
    }
    return nResult;
}

static BOOL IsMoving(CHARACTER_MOVE_STATE nMoveState)
{
    switch (nMoveState)
    {
    case cmsInvalid:
    case cmsOnStand:
        return false;
    case cmsOnWalk:
    case cmsOnRun:
    case cmsOnJump:
    case cmsOnSwimJump:
    case cmsOnSwim:
    case cmsOnFloat:
        return true;
    case cmsOnSit:
    case cmsOnKnockedDown:
        return false;
    case cmsOnKnockedBack:
    case cmsOnKnockedOff:
        return true;
    case cmsOnHalt:
    case cmsOnFreeze:
    case cmsOnEntrap:
        return false;
    case cmsOnAutoFly:
        return true;
    case cmsOnDeath:
        return false;
    case cmsOnDash:
    case cmsOnPull:
    case cmsOnRepulsed:
        return true;
    case cmsOnRise:
        return false;
    case cmsOnSkid:
        return true;
    default:
        ASSERT(0);
        return false;
    }
}

static BOOL IgnoreDragObject(CHARACTER_MOVE_STATE nMoveState, BOOL bAutoMove)
{
    if (bAutoMove)
        return true;

    switch (nMoveState)
    {
    case cmsInvalid:
    case cmsOnStand:
    case cmsOnWalk:
    case cmsOnRun:
    case cmsOnJump:
    case cmsOnSwimJump:
    case cmsOnSwim:
    case cmsOnFloat:
        return false;
    case cmsOnSit:
    case cmsOnKnockedDown:
    case cmsOnKnockedBack:
    case cmsOnKnockedOff:
    case cmsOnHalt:
    case cmsOnFreeze:
    case cmsOnEntrap:   
    case cmsOnAutoFly:
    case cmsOnDeath:
    case cmsOnDash:
    case cmsOnPull:
    case cmsOnRepulsed:
        return true;
    case cmsOnRise:
        return false;
    case cmsOnSkid:
        return true;
    default:
        ASSERT(0);
        return false;
    }
}

static BOOL IsControlEnabled(CHARACTER_MOVE_STATE nMoveState)
{
    switch (nMoveState)
    {
    case cmsInvalid:
        return false;
    case cmsOnStand:
    case cmsOnWalk:
    case cmsOnRun:
    case cmsOnJump:
    case cmsOnSwimJump:
    case cmsOnSwim:
    case cmsOnFloat:
    case cmsOnSit:
        return true;
    case cmsOnKnockedDown:
    case cmsOnKnockedBack:
    case cmsOnKnockedOff:
    case cmsOnHalt:
    case cmsOnFreeze:
    case cmsOnEntrap:
    case cmsOnAutoFly:
    case cmsOnDeath:
    case cmsOnDash:
    case cmsOnPull:
    case cmsOnRepulsed:
    case cmsOnRise:
    case cmsOnSkid:
        return false;
    default:
        ASSERT(0);
        return false;
    }
}

int KRLCharacterCamera::UpdateObjectState(float* pfRotationYawSpeed, BOOL bControls[], BOOL bFrame)
{
    int nResult = false;
    int nRetCode = false;
    float fRotationYawSpeed = 0.0;
    BOOL bLockControl = FALSE;
    BOOL bIsRiding = FALSE;
    DWORD dwMoveState = cmsInvalid;

    bLockControl = m_RLLocalCharacter.m_dwLockControlFrame > 0;

    KGLOG_PROCESS_ERROR(pfRotationYawSpeed);
    KGLOG_PROCESS_ERROR(bControls);

    fRotationYawSpeed = *pfRotationYawSpeed;

    if (bFrame)
    {
        dwMoveState = m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Current.nMoveState;
        bIsRiding = m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Current.bOnRide;
        m_bObjectMoving = ::IsMoving(m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Current.nMoveState);
        m_bIgnoreDragObject = ::IgnoreDragObject(m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Current.nMoveState, m_bAutoMove);
        m_bControlEnabled = ::IsControlEnabled(m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Current.nMoveState);

        if (m_dwObjectMoveState != cmsInvalid &&
            (m_dwObjectMoveState == cmsOnSit && dwMoveState != cmsOnSit || m_bIsRiding != bIsRiding))
        {
            m_bNeedSmoothObject = TRUE;
        }

        if (!m_bControlLocked && bLockControl || 
            (m_dwObjectMoveState != cmsOnDeath && dwMoveState == cmsOnDeath))
        {
            fRotationYawSpeed = 0.0f;
            m_CharacterController.Move(0, 0, 0);

            nRetCode = StopFollowAndAutoMove(bControls);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        m_dwObjectMoveState = dwMoveState;
        m_bIsRiding = bIsRiding;
    }

    m_bControlLocked = bLockControl;

    *pfRotationYawSpeed = fRotationYawSpeed;
    nResult = true;

Exit0:
    return nResult;
}

void KRLCharacterCamera::UpdateAutoResetCamera(KMovementController* pCameraController, float fCameraResetSpeed)
{
    float fObjectYaw = m_ObjectController.GetYaw();
    float fCameraYaw = pCameraController->GetYaw();
    float fDeltaYaw = fCameraYaw - fObjectYaw;

    if (abs(fDeltaYaw) > FLT_EPSILON)
    {
        if (fDeltaYaw > D3DX_PI || (fDeltaYaw < 0.0f && fDeltaYaw >= -D3DX_PI))
        {
            if (fDeltaYaw >= 0.0f)
                fDeltaYaw -= 2.0f * D3DX_PI;

            fDeltaYaw += AUTO_RESET_YAW_VELOCITY * fCameraResetSpeed;

            if (fDeltaYaw > 0.0f)
                fDeltaYaw = 0.0f;
        }
        else
        {
            if (fDeltaYaw < 0.0f)
                fDeltaYaw += 2.0f * D3DX_PI;

            fDeltaYaw -= AUTO_RESET_YAW_VELOCITY * fCameraResetSpeed;

            if (fDeltaYaw < 0.0f)
                fDeltaYaw = 0.0f;
        }

        pCameraController->SetYaw(fObjectYaw + fDeltaYaw);
    }
}

int KRLCharacterCamera::UpdateAlpha()
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    float fAlpha = 0.0f;
    float fDistance = 0.0f;
    float fDistanceTemp = 0.0f;
    D3DXVECTOR3 vObjectPosition(0.0f, 0.0f, 0.0f);

    vObjectPosition = m_vObjectPosition;
    fDistance = D3DXVec3Length(&(m_vCameraPosition - vObjectPosition));

    vObjectPosition.y -= 0.5f * m_fObjectEyeHeight;
    fDistanceTemp = D3DXVec3Length(&(m_vCameraPosition - vObjectPosition));
    fDistance = (fDistance < fDistanceTemp) ? fDistance : fDistanceTemp;

    if (fDistance < FIRST_PERSON_CAMERA)
        fAlpha = 0.0f;
    else if (fDistance > ALPHA_DISTANCE_MAX)
        fAlpha = 1.0f;
    else
        fAlpha = (fDistance - ALPHA_DISTANCE_MIN) / (ALPHA_DISTANCE_MAX - ALPHA_DISTANCE_MIN);

    hr = m_RLLocalCharacter.m_RLCharacter.SetAlpha(fAlpha);
    KGLOG_COM_PROCESS_ERROR(hr);    

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLCharacterCamera::Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame, KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
{
    // 考虑下旋转参数speed的设置
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    int nRetCode = false;
    int nForward = 0;
    int nStrafe = 0;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    nRetCode = UpdateObjectState(&pCameraCommonInfo->fRotationYawSpeed, pCameraCommonInfo->bControls, bFrame);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_bAutoMove = FALSE;
    if (!m_bControlLocked && m_bControlEnabled && pCameraCommonInfo->bControls[RL_CONTROL_FOLLOW])
    {
        nRetCode = UpdateFollow(pCameraCommonInfo->bControls, &m_bAutoMove);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (!m_bControlLocked && m_bControlEnabled && pCameraCommonInfo->bControls[RL_CONTROL_AUTO_MOVE_TO_POINT])
    {
        nRetCode = UpdateAutoMoveToPoint(pCameraCommonInfo->bControls, &m_bAutoMove);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (!m_bControlLocked && m_bControlEnabled && pCameraCommonInfo->bControls[RL_CONTROL_AUTO_MOVE_TO_TARGET])
    {
        nRetCode = UpdateAutoMoveToTarget(pCameraCommonInfo->bControls, &m_bAutoMove);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = PrepareUpdateControl(pCameraCommonInfo, &nForward, &nStrafe);
    KGLOG_PROCESS_ERROR(nRetCode);

	m_RLLocalCharacter.Update(fTime, fTimeLast, dwGameLoop, bFrame);

    nRetCode = PostUpdateControl(pCameraCommonInfo, nForward, nStrafe);
    KGLOG_PROCESS_ERROR(nRetCode);

    hRetCode = SetPosition(pCameraCommonInfo);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    nRetCode = UpdateAlpha();
    KGLOG_PROCESS_ERROR(nRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

// 计算出镜头的期望位置和目标点位置
// A:镜头指向人物的方向向量, B:镜头指向目标点的方向向量
// A,B两个向量在镜头上抬和回归的时候不同，此时屏幕的中心点不是人物
// 已知人物的位置，通过A得到镜头的位置，再通过B得到目标点的位置
HRESULT KRLCharacterCamera::SetPosition(KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hr = E_FAIL;
    int nRetCode = false;

    D3DXVECTOR3 vCameraPosition(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vDirectionC2O(0.0f, 0.0f, 0.0f);      // 镜头到人物的方向向量
    D3DXVECTOR3 vDirectionC2T(0.0f, 0.0f, 0.0f);      // 镜头到目标点的方向向量
    float fCameraPitchC2O = 0.0f;
    float fCameraPitchC2T = 0.0f;
    float fDeltaLength = 0.0f;
    KMovementController* pCameraController = NULL;
    KRLCharacterFrameData const& FrameData = m_RLLocalCharacter.m_RLCharacter.m_FrameData;
    D3DXVECTOR3 vObjectPosition = FrameData.GetCurrentPosition();

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(pCameraCommonInfo->p3DCamera);

    pCameraController = &(pCameraCommonInfo->CameraController);
    fCameraPitchC2T = pCameraController->GetPitch();

    // 得到镜头指向人物的方向向量
    if (m_nCameraMovement == MOVEMENT_FOLLOW)
        fCameraPitchC2O = fCameraPitchC2T;
    else
        fCameraPitchC2O = m_fEarthPitch;

    nRetCode = GetDirectionVector(pCameraCommonInfo, fCameraPitchC2O, &vDirectionC2O);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 得到镜头指向目标点的方向向量
    nRetCode = GetDirectionVector(pCameraCommonInfo, fCameraPitchC2T, &vDirectionC2T);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = AdjustObject(
        pCameraCommonInfo->Params.fMaxCameraDistance,
        fCameraPitchC2T,
        pCameraCommonInfo->p3DScene,
        &vObjectPosition
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    // 坐下、起立、上马、下马都需要目标点平滑
    SmoothObject(&vObjectPosition.y);

    vCameraPosition = vObjectPosition - vDirectionC2O + D3DXVECTOR3(0.01F,0.01F,0.01F);

    // 处理滑轮滚动的，要在碰撞之前处理，否则镜头位置已经被改变
    fDeltaLength = SmoothCamera(vObjectPosition, vCameraPosition);  

    // 调整镜头位置，因为碰撞
    nRetCode = AdjustCameraRough(vObjectPosition, vDirectionC2T, pCameraCommonInfo, &vCameraPosition);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 这一帧不用更新，如果镜头位置没变，pitch值也没变（上抬或回归镜头位置是不变的，靠pitch值来调整目标点）
    KG_PROCESS_SUCCESS(vCameraPosition == m_vCameraPosition && m_nPitchState == PITCH_NONE);

    nRetCode = ChangeCameraMovement(vObjectPosition, vCameraPosition, pCameraController);
    KGLOG_PROCESS_ERROR(nRetCode);

    switch (m_nCameraMovement)
    {
    case MOVEMENT_ELEVATE:
        break;
    case MOVEMENT_REGRESS:
        RegressPitch(pCameraController);
        break;
    case MOVEMENT_FOLLOW:  
        m_fEarthPitch = fCameraPitchC2T;
        break;
    }
    m_nPreCameraMovement = m_nCameraMovement;

    nRetCode = AdjustCameraFine(vObjectPosition, &vCameraPosition, &vDirectionC2T, pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 检查镜头除零
    ASSERT(vCameraPosition == vCameraPosition);
    ASSERT(vObjectPosition == vObjectPosition);

    hr = pCameraCommonInfo->RLTrackCamera.UpdateCamera(vCameraPosition, vDirectionC2T, fDeltaLength, 100.0f);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_vCameraPosition = vCameraPosition;
    m_vObjectPosition = vObjectPosition;

Exit1:
    hrResult = S_OK;
Exit0:
    m_fPitchStartTime = g_pRL->m_fTime;
    return hrResult;
}

int KRLCharacterCamera::GetDirectionVector(
    const KG_CAMERA_COMMON_INFO* cpCameraCommonInfo,
    float fCameraPitch,
    D3DXVECTOR3* pvDirectionVector
) const
{
    int nResult = false;
    float fLength = 0.0f;
    float fCameraYaw = 0.0f;

    KGLOG_PROCESS_ERROR(cpCameraCommonInfo);
    KGLOG_PROCESS_ERROR(pvDirectionVector);

    fCameraYaw += cpCameraCommonInfo->CameraController.GetYaw();
    fCameraYaw += m_fCharacterDeltaYaw;

    if (m_bIsRiding)
        fCameraYaw += g_pRL->m_TableList.GetNumber(RL_NUMBER_CAMERA_RIDE_YAW_OFFSET);

    fLength = cpCameraCommonInfo->Params.fMaxCameraDistance * m_fCameraToObjectEyeScale;

    GetParallelLine(*pvDirectionVector, fCameraYaw, fCameraPitch, fLength);

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacterCamera::BeginDrag()
{
    m_fMaxMouseX = 0.0f;
    m_fMaxMouseY = 0.0f;
    m_fFinalMouseX = 0.0f;
    m_fFinalMouseY = 0.0f;

    m_ObjectController.OnBegin();
}

void KRLCharacterCamera::DragMoveObject(float fYaw, float fPitch, float fDeltaYaw, float fDeltaPitch)
{
    m_bHasDraggedObject = TRUE;

    if (!m_bIgnoreDragObject && !m_bControlLocked && m_bControlEnabled)
    {
        UpdateYaw(fYaw, FALSE);
    }
}

void KRLCharacterCamera::UpdateYaw(float fYaw, BOOL bTurnImmediately)
{
    m_ObjectController.SetYaw(fYaw);
    m_CharacterController.SetYaw(ToGameWorldAngle(YawLHToRH(fYaw)));

    if (bTurnImmediately)
        m_RLLocalCharacter.SetFaceYaw(fYaw);
    else
        m_RLLocalCharacter.SetFaceExpectYaw(fYaw);

    if (m_bUsingFaceFootDelta || bTurnImmediately)
        m_RLLocalCharacter.SetFootExpectYaw(fYaw + m_fFaceFootDelta);
}

void KRLCharacterCamera::EndDrag()
{
    m_bHasDraggedObject = FALSE;
    m_ObjectController.OnEnd();
}

void KRLCharacterCamera::HasDragged(
    BOOL& bObject,
    float& fMaxMouseX,
    float& fMaxMouseY,
    float& fFinalMouseX,
    float& fFinalMouseY
) const
{
    bObject = m_bHasDraggedObject;
    fMaxMouseX = m_fMaxMouseX;
    fMaxMouseY = m_fMaxMouseY;
    fFinalMouseX = m_fFinalMouseX;
    fFinalMouseY = m_fFinalMouseY;
}

HRESULT KRLCharacterCamera::OnGetBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_RLLocalCharacter.m_RLCharacter.GetBBox(vMin, vMax);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLCharacterCamera::IsMoving(RL_CONTROL nMoveStyle)
{
    int nRetCode = false;

    int nForward = 0;
    int nStrafeRight = 0;
    int nRotationRight = 0;
    int nIsMoving = false;

    nRetCode = m_CharacterController.GetMoveInfo(&nForward, &nStrafeRight, &nRotationRight);
    KGLOG_PROCESS_ERROR(nRetCode);

    switch (nMoveStyle)
    {
    case RL_CONTROL_FORWARD:
        nIsMoving = nForward > 0;
        break;
    case RL_CONTROL_BACKWARD:
        nIsMoving = nForward < 0;
        break;
    case RL_CONTROL_STRAFE_RIGHT:
        nIsMoving = nStrafeRight > 0;
        break;
    case RL_CONTROL_STRAFE_LEFT:
        nIsMoving = nStrafeRight < 0;
        break;
    case RL_CONTROL_TURN_RIGHT:
        nIsMoving = nRotationRight < 0;
        break;
    case RL_CONTROL_TURN_LEFT:
        nIsMoving = nRotationRight > 0;
        break;
    }

Exit0:
    return nIsMoving;
}

void KRLCharacterCamera::SetRTParams(float fCameraToObjectEyeScale)
{
    m_fCameraToObjectEyeScale = fCameraToObjectEyeScale;

    if (m_fCameraToObjectEyeScale < SCALE_MIN)
        m_fCameraToObjectEyeScale = SCALE_MIN;

    if (m_fCameraToObjectEyeScale > SCALE_MAX)
        m_fCameraToObjectEyeScale = SCALE_MAX;
}

void KRLCharacterCamera::GetRTParams(float& fCameraToObjectEyeScale) const
{
    fCameraToObjectEyeScale = m_fCameraToObjectEyeScale;
}

int KRLCharacterCamera::ToggleControl(RL_CONTROL nIndex, BOOL bEnable)
{
    int nRetCode = false;
    int nResult = false;

    switch (nIndex)
    {
    case RL_CONTROL_FORWARD:
    case RL_CONTROL_BACKWARD:
    case RL_CONTROL_TURN_LEFT:
    case RL_CONTROL_TURN_RIGHT:
    case RL_CONTROL_STRAFE_LEFT:
    case RL_CONTROL_STRAFE_RIGHT:
    case RL_CONTROL_CAMERA:
    case RL_CONTROL_OBJECT_STICK_CAMERA:
        break;
    case RL_CONTROL_WALK:
        if (bEnable)
            m_CharacterController.Walk();
        else
            m_CharacterController.Run();
        break;
    case RL_CONTROL_JUMP:
        break;
    case RL_CONTROL_AUTO_RUN:
        if (!bEnable)
            m_CharacterController.Move(0, 0, 0);
        break;
    case RL_CONTROL_FOLLOW:
        break;
    default:
        KG_ASSERT_EXIT(0);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::AdjustObject(float fMaxCameraDistance, float fCameraPitch, IKG3DScene* p3DScene, D3DXVECTOR3* pvObject)
{   
    //Object点修正
    int nResult = false;
    HRESULT hr = E_FAIL;
    D3DXVECTOR3 vWaterHeight(0.0f, 0.0f, 0.0f);
    KRLCharacterFrameData const& FrameData = m_RLLocalCharacter.m_RLCharacter.m_FrameData;
    D3DXVECTOR3 vObject(0.0f, 0.0f, 0.0f);
    float fObjectHigh = 0.0f;

    KGLOG_PROCESS_ERROR(p3DScene);
    KGLOG_PROCESS_ERROR(pvObject);

    vObject = *pvObject;
    fObjectHigh = vObject.y + m_fObjectEyeHeight;

    float fDeltaPitch = fCameraPitch - m_fLastPitch;

    if (fDeltaPitch > D3DX_PI / 1024.0f)
        m_nPitchState = PITCH_UP;
    else if (fDeltaPitch < -D3DX_PI / 1024.0f)
        m_nPitchState = PITCH_DOWN;
    else
        m_nPitchState = PITCH_NONE;

    m_fLastPitch = fCameraPitch;

    fObjectHigh += m_RLLocalCharacter.m_RLCharacter.GetRideAdjustHeight();

    if (FrameData.m_Previous.bUnderWater != FrameData.m_Current.bUnderWater)
    {
        if (FrameData.m_Current.bUnderWater)
            m_nCameraMovement = MOVEMENT_IN_WATER;
        else
            m_nCameraMovement = MOVEMENT_OUT_WATER;
    }

    if (FrameData.m_Current.bUnderWater)
        m_nObjectAltitudeType = ALTITUDE_WATER;
    else
        m_nObjectAltitudeType = ALTITUDE_AIR;

    int nCanDiving = FrameData.m_Current.bUnderWater || (FrameData.m_Current.nVelocityXY && fCameraPitch <= PLAYER_DIVING_PITCH);
    if (m_bHasDraggedObject && nCanDiving)
    {
        m_CharacterController.SetPitch(ToGameWorldAngle(fCameraPitch));
        m_RLLocalCharacter.SetFacePitch(fCameraPitch);
    }

    if (m_fCameraToObjectEyeScale * fMaxCameraDistance <= FIRST_PERSON_CAMERA * 3 && m_dwObjectMoveState == cmsOnSit)
    {
        D3DXVECTOR3 vEye(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 vOffset(0.0f, 0.0f, 0.0f);

        if (SUCCEEDED(m_RLLocalCharacter.m_RLCharacter.m_RenderData.GetBonePosition(vEye, vOffset, _T("Bip01 Head"))))
        {
            fObjectHigh = vEye.y;
        }
    }

    if (m_bNeedSmoothObject)
    {
        m_fNeedSmoothObjectHeight = m_vObjectPosition.y - fObjectHigh;
        m_bNeedSmoothObject = FALSE;
    }

    vObject.y = fObjectHigh;

    m_bWaterArea = FALSE;
    hr = p3DScene->GetWaterHeight(&vWaterHeight, &vObject);
    if (S_OK == hr)
    {
        m_bWaterArea = TRUE;
        m_fWaterLine = (abs(vWaterHeight.y) <= FLT_EPSILON) ? 0.0f : vWaterHeight.y;
    }

    *pvObject = vObject;
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::AdjustCameraRough(
    D3DXVECTOR3& vObject,
    D3DXVECTOR3& vDirectionC2T,
    KG_CAMERA_COMMON_INFO* pCameraCommonInfo,
    D3DXVECTOR3* pvCamera
)
{   
    // Camera点修正
    int nResult = false;
    HRESULT hr = E_FAIL;

    int nHasCollision = false;
    float fCameraPitch = 0.0f;
    D3DXVECTOR3 vCamera(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vCross(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vWaterHeight(0.0f, 0.0f, 0.0f);

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(pCameraCommonInfo->p3DCamera);
    KGLOG_PROCESS_ERROR(pCameraCommonInfo->p3DScene);
    KGLOG_PROCESS_ERROR(pvCamera);
    vCamera = *pvCamera;

    fCameraPitch = pCameraCommonInfo->CameraController.GetPitch();

    // 入水判断
    m_nCameraAltitudeType = ALTITUDE_AIR;
    hr = pCameraCommonInfo->p3DScene->GetWaterHeight(&vWaterHeight, &vCamera);
    if (hr == S_OK)
    {
        D3DXVECTOR3 vCameraOrigin = vObject - vDirectionC2T;

        if (!m_bWaterArea)
            m_fWaterLine = (abs(vWaterHeight.y) <= FLT_EPSILON) ? 0.0f : vWaterHeight.y;

        m_bWaterArea = TRUE;
        if (vCameraOrigin.y < m_fWaterLine)
        {
            m_nCameraAltitudeType = ALTITUDE_WATER;
            if (m_nPreCameraMovement == MOVEMENT_FOLLOW)
                m_fEarthPitch = fCameraPitch;
        }
    }

    // 与障碍碰撞
    nHasCollision = pCameraCommonInfo->RLTrackCamera.
        GetCollisionPosition(vCamera, vObject, pCameraCommonInfo->p3DCamera, &vCross);

    if (nHasCollision)
        vCamera = vCross;

    // 入地判断
    if (
        nHasCollision &&
        (vCamera.y < vObject.y) &&  // 需要pitch是个仰角
        (fCameraPitch >= m_fEarthPitch) &&  // 上抬条件
        (m_nObjectAltitudeType == m_nCameraAltitudeType)
    )
    {
        if (m_nPreCameraMovement == MOVEMENT_FOLLOW)
            m_fEarthPitch = fCameraPitch;
        m_nCameraAltitudeType = ALTITUDE_EARTH;
    }

    *pvCamera = vCamera;
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::AdjustCameraFine(
    D3DXVECTOR3 vObject,
    D3DXVECTOR3* pvCamera,
    D3DXVECTOR3* pvDirectionC2T, 
    KG_CAMERA_COMMON_INFO* pCameraCommonInfo
)
{
    // 摄像机最终修正
    int nResult = false;
    int nRetCode = false;

    float fDistanceC2O = 0.0f;
    D3DXVECTOR3 vCamera(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vDirectionC2T(0.0f, 0.0f, 0.0f);

    KGLOG_PROCESS_ERROR(pvCamera);
    KGLOG_PROCESS_ERROR(pvDirectionC2T);
    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    vCamera = *pvCamera;
    vDirectionC2T = *pvDirectionC2T;

    // 水面校正, camera必须和人物同时在水里或是在空气中，并且注意3D camera是个椎体
    if (m_bWaterArea)
    {
        float fAdjustHeight = 0.0f;

        nRetCode = GetAdjustHeightForWater(
            pCameraCommonInfo->p3DCamera,
            &fAdjustHeight
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        if (ALTITUDE_WATER == m_nObjectAltitudeType && vCamera.y > m_fWaterLine - fAdjustHeight)
        {
            float fHeight = m_fWaterLine - fAdjustHeight;

            if (vObject.y > fHeight)
            {
                vObject.y = fHeight;
            }
            if (vCamera.y > m_fWaterLine)
            {
                vCamera = vObject - (vObject - vCamera) * (m_fWaterLine - vObject.y) / (vCamera.y - vObject.y);
            }

            vCamera.y = fHeight;
        }

        if (ALTITUDE_AIR == m_nObjectAltitudeType && vCamera.y < m_fWaterLine + fAdjustHeight)
        {
            float fHeight = m_fWaterLine + fAdjustHeight;

            if (vObject.y < fHeight)
            {
                vObject.y = fHeight;
            }
            if (vCamera.y < m_fWaterLine)
            {
                vCamera = vObject - (vObject - vCamera) * (m_fWaterLine - vObject.y) / (vCamera.y - vObject.y);
            }
            
            vCamera.y = fHeight;
        }

        if (m_nObjectAltitudeType != m_nCameraAltitudeType)
            pCameraCommonInfo->RLTrackCamera.SetCollisionFlag(true);
    }

    // 碰撞，鼠标滑轮都会改变镜头到目标点的距离，在这里做最后修正
    fDistanceC2O = D3DXVec3Length(&(vObject - vCamera));
    D3DXVec3Normalize(&vDirectionC2T, &vDirectionC2T);
    vDirectionC2T *= fDistanceC2O;

    *pvCamera = vCamera;
    *pvDirectionC2T = vDirectionC2T;
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::ChangeCameraMovement(D3DXVECTOR3& vObject, D3DXVECTOR3& vCamera, KMovementController* pCameraController)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pCameraController);

    // 摄像机策略选择情况
    if (MOVEMENT_OUT_WATER == m_nCameraMovement)
    {
        m_fEarthPitch  = 0.0f;
        m_CharacterController.SetPitch(0);
        m_RLLocalCharacter.SetFacePitch(0.0f);
        m_nCameraMovement = MOVEMENT_REGRESS;
    }
    else if (MOVEMENT_IN_WATER == m_nCameraMovement)
    {
        m_fEarthPitch  = 0.0f;
        pCameraController->SetPitch(0.0f);
        m_fLastPitch = 0.0f;
        m_nCameraMovement = MOVEMENT_FOLLOW;
    }
    else if (m_nObjectAltitudeType == m_nCameraAltitudeType)
    {
        if (
            m_nCameraMovement == MOVEMENT_FOLLOW ||
            m_bObjectMoving && (m_nPitchState == PITCH_UP) ||
            (m_nCameraMovement != MOVEMENT_REGRESS) && (m_nPitchState == PITCH_DOWN)
        )
            m_nCameraMovement = MOVEMENT_FOLLOW;
        else
            m_nCameraMovement = MOVEMENT_REGRESS;
    }
    else if (m_nObjectAltitudeType == ALTITUDE_WATER)
    {
        m_nCameraMovement = MOVEMENT_FOLLOW;
    }
    else
    {
        if (m_bObjectMoving)
        {
            if (m_nCameraMovement == MOVEMENT_FOLLOW || m_nPitchState == PITCH_UP)
                m_nCameraMovement = MOVEMENT_FOLLOW;
            else
                m_nCameraMovement = MOVEMENT_REGRESS;
        }
        else
        {
            if (m_nPitchState == PITCH_UP)
                m_nCameraMovement = MOVEMENT_ELEVATE;
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::RegressPitch(KMovementController* pCameraController)
{
    // 摄像机回归
    int nResult = false;

    float fDeltaTime = 0.0f;
    float fPitchCurrent = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraController);

    fPitchCurrent = pCameraController->GetPitch();
    fDeltaTime = (float)(g_pRL->m_fTime - m_fPitchStartTime);
    fPitchCurrent -= fDeltaTime * AUTO_RESET_PITCH_VELOCITY;

    if (fPitchCurrent <= m_fEarthPitch)
    {
        fPitchCurrent = m_fEarthPitch;
        m_nCameraMovement = MOVEMENT_FOLLOW;
    }
    pCameraController->SetPitch(fPitchCurrent);

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacterCamera::SmoothObject(float* pfObjectY)
{
    float fObjectY = 0.0f;
    float fDeltaHeight = 0.0f;
    float fDeltaTime = (float)(g_pRL->m_fTime - g_pRL->m_fTimeLast);

    KGLOG_PROCESS_ERROR(pfObjectY);
    fObjectY = *pfObjectY;

    if (m_nCameraMovement == MOVEMENT_FOLLOW && abs(m_fNeedSmoothObjectHeight) > 0.5f)
    {
        // 防止帧数过大或者过小而导致镜头反转
        if (fDeltaTime >= 1000.0f)   
            fDeltaTime = 500.0f;
        else if (fDeltaTime <= 0.0f)
            fDeltaTime = 60.0f;

        fDeltaHeight = SMOOTH_SPEED / 5.0f * fDeltaTime / 1000.0f;

        if (m_fNeedSmoothObjectHeight > 0.0f)
        {
            m_fNeedSmoothObjectHeight -= fDeltaHeight;
            if (m_fNeedSmoothObjectHeight < 0.0f)
                m_fNeedSmoothObjectHeight = 0.0f;
        }
        else
        {
            m_fNeedSmoothObjectHeight += fDeltaHeight;
            if (m_fNeedSmoothObjectHeight > 0.0f)
                m_fNeedSmoothObjectHeight = 0.0f;
        }

        fObjectY += m_fNeedSmoothObjectHeight;
    }
    else
    {
        m_fNeedSmoothObjectHeight = 0.0f;
    }

    *pfObjectY = fObjectY;
Exit0:
    return;
}

float KRLCharacterCamera::SmoothCamera(D3DXVECTOR3& vObject, D3DXVECTOR3& vCamera)
{   
    // 摄像机平滑
    float fDeltaLength = 0.0f;

    if (m_nCameraMovement == MOVEMENT_FOLLOW)
    {
        float fExpectZoomLength = D3DXVec3Length(&(vObject - vCamera));
        float fDeltaTime = (float)(g_pRL->m_fTime - g_pRL->m_fTimeLast);

		KG_PROCESS_SUCCESS(abs(fExpectZoomLength)<0.1F);
        KG_PROCESS_SUCCESS(abs(fExpectZoomLength - m_fCurrentZoomLength) < 0.5f);

        // 防止帧数过大或者过小而导致镜头反转
        if (fDeltaTime >= 1000.0f)   
            fDeltaTime = 500.0f;
        else if (fDeltaTime <= 0.0f)
            fDeltaTime = 60.0f;

        if (fExpectZoomLength > m_fCurrentZoomLength)
        {
            fDeltaLength = SMOOTH_SPEED * fDeltaTime / 1000.0f;
            if (m_fCurrentZoomLength + fDeltaLength > fExpectZoomLength)
                fDeltaLength = fExpectZoomLength - m_fCurrentZoomLength;
        }
        else
        {
            fDeltaLength = -SMOOTH_SPEED * fDeltaTime / 1000.0f;
            if (m_fCurrentZoomLength + fDeltaLength < fExpectZoomLength)
                fDeltaLength = fExpectZoomLength - m_fCurrentZoomLength;
        }

        m_fCurrentZoomLength += fDeltaLength;
		 
        vCamera = vObject - (vObject - vCamera) * m_fCurrentZoomLength / fExpectZoomLength;
    }


Exit1:
    return fDeltaLength;
}

int KRLCharacterCamera::AdjustFaceDirection(RL_CONTROL nIndex, BOOL bEnable)
{
    // 在停止转身的时候修正朝向

    int nRetCode = false;
    int nResult = false;
    BOOL bAdjustDirection = false;
    int nDirection = 0;

    KG_PROCESS_SUCCESS(bEnable);

	ASSERT(m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_pCharacter);
    KGLOG_PROCESS_ERROR(m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_pCharacter);

    if (nIndex == RL_CONTROL_TURN_LEFT)
    {
        nDirection = (int)(m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Interpolated.fFaceDirection + 0.5f);
        nDirection %= DIRECTION_COUNT;

        bAdjustDirection = true;
    }
    else if (nIndex == RL_CONTROL_TURN_RIGHT)
    {
        nDirection = (int)(m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Interpolated.fFaceDirection);
        bAdjustDirection = true;
    }

    if (bAdjustDirection && nDirection != m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_pCharacter->m_nFaceDirection)
    {
        m_CharacterController.SetYaw(nDirection);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateControl(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, RL_CONTROL nIndex, BOOL bEnable)
{
    int nRetCode = false;
    int nResult = false;
    int nForward = 0;
    int nStrafe = 0;

    nRetCode = m_RLLocalCharacter.m_RLCharacter.m_FrameData.InvalidateFrameData(
        g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop - 1);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = PrepareUpdateControl(pCameraCommonInfo, &nForward, &nStrafe);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = AdjustFaceDirection(nIndex, bEnable);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = PostUpdateControl(pCameraCommonInfo, nForward, nStrafe);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::PrepareUpdateControl(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, int* pnForward, int* pnStrafe)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    // 鼠标拖拽
    nRetCode = MouseMove(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 前后左右移动
    nRetCode = UpdateMove(pCameraCommonInfo, pnForward, pnStrafe);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::PostUpdateControl(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, int nForward, int nStrafe)
{
    int nRetCode = false;
    int nResult = false;
    float fDeltaYaw = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    // 旋转
    nRetCode = UpdateRotation(pCameraCommonInfo, &fDeltaYaw);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 镜头追尾
    nRetCode = UpdateAutoReset(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 人物脚的方向的调整
    nRetCode = AdjustFootYaw(nForward, nStrafe, fDeltaYaw, pCameraCommonInfo->bControls);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int ComputeMouseMoveDelta(D3DXVECTOR2* pvMouse, LONG* plWidth, LONG* plHeight)
{
    int nResult = false;
    int nRetCode = false;

    D3DXVECTOR2 vMouse(0.0f, 0.0f);
    RECT rcClient;
    POINT ptCursor;
    POINT ptCenter;
    LONG lWidth = 0;
    LONG lHeight = 0;

    KGLOG_PROCESS_ERROR(pvMouse);
    KGLOG_PROCESS_ERROR(plWidth);
    KGLOG_PROCESS_ERROR(plHeight);

    nRetCode = ::GetClientRect(g_pRL->m_hWnd, &rcClient);
    KGLOG_PROCESS_ERROR(nRetCode);

    lWidth = rcClient.right - rcClient.left;
    lHeight = rcClient.bottom - rcClient.top;

    ptCenter.x = (rcClient.left + rcClient.right) / 2;
    ptCenter.y = (rcClient.top + rcClient.bottom) / 2;

    nRetCode = ::ClientToScreen(g_pRL->m_hWnd, &ptCenter);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::GetCursorPos(&ptCursor);
    KGLOG_PROCESS_ERROR(nRetCode);

    vMouse.x = (float)(ptCenter.x - ptCursor.x);
    vMouse.y = (float)(ptCenter.y - ptCursor.y);

    nRetCode = ::SetCursorPos(ptCenter.x, ptCenter.y);
    KGLOG_PROCESS_ERROR(nRetCode);

    *pvMouse = vMouse;
    *plWidth = lWidth;
    *plHeight = lHeight;

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::GetAdjustHeightForWater(IKG3DCamera *p3DCamera, float *pfHeight)
{
    int nResult = false;
    HRESULT hRetCode = E_FAIL;

    float fFovy = 0.0f;
    float fAspect = 0.0f;
    float fzNear = 0.0f;
    float fzFar = 0.0f;
    float fHeight = 0.0f;

    KGLOG_PROCESS_ERROR(p3DCamera);
    KGLOG_PROCESS_ERROR(pfHeight);

    hRetCode = p3DCamera->GetPerspective(&fFovy, &fAspect, &fzNear, &fzFar);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    KGLOG_PROCESS_ERROR(fFovy > 0 && fFovy < D3DX_PI);

    fHeight = GetHypotenuseByCosine(fzNear, fFovy * 0.5f);

    *pfHeight = fHeight;
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::MouseMove(KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
{
    int nResult = false;
    int nRetCode = false;

    D3DXVECTOR2 vMouse(0.0f, 0.0f);
    LONG lWidth = 0;
    LONG lHeight = 0;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    if (
        pCameraCommonInfo->bControls[RL_CONTROL_CAMERA] || 
        pCameraCommonInfo->bControls[RL_CONTROL_OBJECT_STICK_CAMERA]
    )
    {
        nRetCode = ComputeMouseMoveDelta(&vMouse, &lWidth, &lHeight);
        KGLOG_PROCESS_ERROR(nRetCode);

        m_fFinalMouseX += vMouse.x;
        if (abs(m_fFinalMouseX) > m_fMaxMouseX)
            m_fMaxMouseX = abs(m_fFinalMouseX);

        m_fFinalMouseY += vMouse.y;
        if (abs(m_fFinalMouseY) > m_fMaxMouseY)
            m_fMaxMouseY = abs(m_fFinalMouseY);

        vMouse.x /= (float)lWidth;
        vMouse.y /= (float)lHeight;

        nRetCode = ScaleMouse(pCameraCommonInfo->Params.fDragSpeed, &vMouse.x, &vMouse.y);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = ApplyMouse(pCameraCommonInfo, vMouse);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::ScaleMouse(float fDragSpeed, float* pfMouseX, float* pfMouseY)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pfMouseX);
    KGLOG_PROCESS_ERROR(pfMouseY);

    *pfMouseX *= fDragSpeed;
    *pfMouseY *= fDragSpeed;

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::ApplyMouse(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, const D3DXVECTOR2& crvMouse)
{
    int nResult = false;
    D3DXVECTOR3 vMouse;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);
    KG_PROCESS_SUCCESS(crvMouse == D3DXVECTOR2(0.0f, 0.0f));

    vMouse.x = crvMouse.x;
    vMouse.y = crvMouse.y;
    vMouse.z = 0.0f;

    pCameraCommonInfo->CameraController.OnMove(vMouse);

    if (pCameraCommonInfo->bControls[RL_CONTROL_CAMERA])
    {
        pCameraCommonInfo->bHasDraggedCamera = TRUE;
    }

    if (pCameraCommonInfo->bControls[RL_CONTROL_OBJECT_STICK_CAMERA])
    {
        float fYaw = pCameraCommonInfo->CameraController.GetYaw();
        float fPitch = pCameraCommonInfo->CameraController.GetPitch();

        DragMoveObject(fYaw, fPitch, crvMouse.x * (D3DX_PI * 2.0f), crvMouse.y * (D3DX_PI * 2.0f));
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateMove(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, int* pnForwardMove, int* pnStrafeMove)
{
    int nResult = false;
    int nRetCode = false;

    int nForwardMove = 0;
    int nStrafeMove = 0;
    float fCameraYaw = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(pnForwardMove);
    KGLOG_PROCESS_ERROR(pnStrafeMove);

    fCameraYaw = pCameraCommonInfo->CameraController.GetYaw();
    nRetCode = ComputeForward(pCameraCommonInfo->bControls, fCameraYaw, &nForwardMove);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ComputeStrafe(pCameraCommonInfo->bControls, &nStrafeMove);
    KGLOG_PROCESS_ERROR(nRetCode);

    if (pCameraCommonInfo->bControls[RL_CONTROL_JUMP])
    {
        if (!m_bControlLocked && m_bControlEnabled)
            m_CharacterController.Jump(nStrafeMove, nForwardMove);
        pCameraCommonInfo->bControls[RL_CONTROL_JUMP] = FALSE;
    }
    else
    {
        m_CharacterController.Move(nStrafeMove * MOVE_DEST_RANGE, nForwardMove * MOVE_DEST_RANGE, 0);
    }

    *pnForwardMove = nForwardMove;
    *pnStrafeMove = nStrafeMove;
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::ComputeForward(const BOOL cbControls[], float fCameraYaw, int* pnForwardMove)
{
    int nResult = false;

    int nForwardMove = 0;

    KGLOG_PROCESS_ERROR(cbControls);
    KGLOG_PROCESS_ERROR(pnForwardMove);

    if (!m_bControlLocked && m_bControlEnabled)
    {
        if (m_bMouseControlMoveEnable && cbControls[RL_CONTROL_CAMERA] && cbControls[RL_CONTROL_OBJECT_STICK_CAMERA])
        {   
            // 鼠标左右键同时按住，人物立即转向镜头方向，向前跑 
            UpdateYaw(fCameraYaw, FALSE);
            ++nForwardMove;
        }
        else if (cbControls[RL_CONTROL_FORWARD] || cbControls[RL_CONTROL_AUTO_RUN] || m_bAutoMove)
        {
            ++nForwardMove;
        }

        if (cbControls[RL_CONTROL_BACKWARD])
        {
            --nForwardMove;
        }
    }

    *pnForwardMove = nForwardMove;
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::ComputeStrafe(const BOOL cbControls[], int *pnStrafeMove)
{
    int nResult = false;
    int nStrafeMove = 0;

    KGLOG_PROCESS_ERROR(cbControls);
    KGLOG_PROCESS_ERROR(pnStrafeMove);

    if (!m_bControlLocked && m_bControlEnabled)
    {
        if (cbControls[RL_CONTROL_OBJECT_STICK_CAMERA] && cbControls[RL_CONTROL_TURN_RIGHT] ||  // 鼠标右键+向右转=向右跑
            cbControls[RL_CONTROL_STRAFE_RIGHT])
        {
            ++nStrafeMove;
        }

        if (cbControls[RL_CONTROL_OBJECT_STICK_CAMERA] && cbControls[RL_CONTROL_TURN_LEFT] ||  // 鼠标右键+向左转=向左跑
            cbControls[RL_CONTROL_STRAFE_LEFT])
        {
            --nStrafeMove;
        }
    }

    *pnStrafeMove = nStrafeMove;
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateRotation(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, float* pfCharacterRotationYawSpeed)
{
    int nResult = false;
    int nRetCode = false;
    float fDeltaYaw = 0.0f;
    
    KGLOG_PROCESS_ERROR(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(pfCharacterRotationYawSpeed);

    // 人物旋转
    nRetCode = UpdateCharacterRotation(
        pCameraCommonInfo->bControls,
        &fDeltaYaw);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 镜头旋转
    nRetCode = UpdateCameraRotation(
        &pCameraCommonInfo->CameraController,
        pCameraCommonInfo->bControls,
        fDeltaYaw);
    KGLOG_PROCESS_ERROR(nRetCode);

    *pfCharacterRotationYawSpeed = fDeltaYaw;
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateCharacterRotation(const BOOL cbControls[], float* pfDeltaYaw)
{
    int nResult = false;
    float fObjectYaw = 0.0f;

    KGLOG_PROCESS_ERROR(cbControls);
    KGLOG_PROCESS_ERROR(pfDeltaYaw);

    if (!m_bControlLocked && m_bControlEnabled && !cbControls[RL_CONTROL_OBJECT_STICK_CAMERA] && 
        cbControls[RL_CONTROL_TURN_LEFT] != cbControls[RL_CONTROL_TURN_RIGHT])
    {
        if (cbControls[RL_CONTROL_TURN_LEFT] != cbControls[RL_CONTROL_TURN_RIGHT])
        {
            if (cbControls[RL_CONTROL_TURN_LEFT])
                m_CharacterController.Turn(1);
            else
                m_CharacterController.Turn(-1);
        }

        float fObjectFaceDirection = GetObjectFaceDirection();

        fObjectYaw = m_ObjectController.GetYaw();

        m_ObjectController.SetYaw(fObjectFaceDirection);
        m_RLLocalCharacter.SetFaceYaw(fObjectFaceDirection);

        if (m_bUsingFaceFootDelta)
            m_RLLocalCharacter.SetFootExpectYaw(fObjectFaceDirection + m_fFaceFootDelta);

        *pfDeltaYaw = m_ObjectController.GetYaw() - fObjectYaw;
    }
    else
    {
        *pfDeltaYaw = 0.0f;

        m_CharacterController.Turn(0);

        float fYaw = m_ObjectController.GetYaw();

        m_CharacterController.SetYaw(ToGameWorldAngle(YawLHToRH(fYaw)));
        m_RLLocalCharacter.SetFaceYaw(fYaw);

        if (m_bUsingFaceFootDelta)
            m_RLLocalCharacter.SetFootExpectYaw(fYaw + m_fFaceFootDelta);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateCameraRotation(
    KMovementController* pCameraController,
    const BOOL cbControls[],
    float fDeltaYaw)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pCameraController);
    KGLOG_PROCESS_ERROR(cbControls);

    if (m_bControlEnabled &&
        !m_bControlLocked &&
        !cbControls[RL_CONTROL_OBJECT_STICK_CAMERA] &&
        !cbControls[RL_CONTROL_CAMERA] &&
        fDeltaYaw != 0.0f)
    {
        float fCameraYaw = pCameraController->GetYaw();

        pCameraController->SetYaw(fCameraYaw + fDeltaYaw);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateAutoReset(KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
{
    int nResult = false;
    int nRetCode = false;

    BOOL bIsMoveControl = false;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    if (!pCameraCommonInfo->bControls[RL_CONTROL_OBJECT_STICK_CAMERA] &&
        !pCameraCommonInfo->bControls[RL_CONTROL_CAMERA])
    {
        nRetCode = NeedResetCamera(pCameraCommonInfo->bControls, &bIsMoveControl);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (pCameraCommonInfo->Params.nResetMode == RL_RESET_MODE_SMART && bIsMoveControl ||
            pCameraCommonInfo->Params.nResetMode == RL_RESET_MODE_ALWAYS)
        {
            UpdateAutoResetCamera(&pCameraCommonInfo->CameraController, pCameraCommonInfo->Params.fCameraResetSpeed);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::NeedResetCamera(const BOOL cbControls[], BOOL* pbIsMoveControl)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(cbControls);
    KGLOG_PROCESS_ERROR(pbIsMoveControl);

    *pbIsMoveControl = m_bAutoMove ||
                       cbControls[RL_CONTROL_FORWARD] ||
                       cbControls[RL_CONTROL_BACKWARD] ||
                       cbControls[RL_CONTROL_TURN_LEFT] ||
                       cbControls[RL_CONTROL_TURN_RIGHT] ||
                       cbControls[RL_CONTROL_STRAFE_LEFT] ||
                       cbControls[RL_CONTROL_STRAFE_RIGHT] ||
                       cbControls[RL_CONTROL_AUTO_RUN] ||
                       (cbControls[RL_CONTROL_CAMERA] && cbControls[RL_CONTROL_OBJECT_STICK_CAMERA]);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::AdjustFootYaw(
    int nForwardMove,
    int nStrafeMove,
    float fRotationYawSpeed,
    const BOOL cbControls[]
)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(cbControls);

    if (nStrafeMove != 0 || nForwardMove != 0)
    {
        float fX = (float)nStrafeMove;
        float fY = (float)nForwardMove;
    
        if (nForwardMove < 0)
        {
            fX *= -1.0f;
            fY *= -1.0f;
        }

        m_fFaceFootDelta = atan2f(fX, fY);
        m_RLLocalCharacter.SetFootExpectYaw(m_ObjectController.GetYaw() + m_fFaceFootDelta);
    }
    else
    {
        m_RLLocalCharacter.SetFootExpectYaw(m_ObjectController.GetYaw());
    }

    if (fRotationYawSpeed != 0.0f && (nForwardMove != 0 || nStrafeMove != 0 || cbControls[RL_CONTROL_JUMP]))
    {
        m_bUsingFaceFootDelta = TRUE;
    }
    else
    {
        m_fFaceFootDelta = 0.0f;
        m_bUsingFaceFootDelta = FALSE;
    }

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacterCamera::SetView(float fAngle, KMovementController* pCameraController)
{
    float fYawLH = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraController);

    m_fCharacterDeltaYaw = D3DXToRadian(fAngle);

    fYawLH = GetObjectFaceDirection();
    pCameraController->SetYaw(fYawLH);

    pCameraController->SetPitch(-0.3f);
    pCameraController->SetRoll(0.0f);

    m_fCameraToObjectEyeScale = 1.0f;

Exit0:
    return;
}

void KRLCharacterCamera::TurnToFaceDirection(KMovementController* pCameraController)
{
    float fYawLH = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraController);

    fYawLH = GetObjectFaceDirection();
    pCameraController->SetYaw(fYawLH);

Exit0:
    return;
}

void KRLCharacterCamera::MouseControlMoveEnable(BOOL bEnable)
{
    m_bMouseControlMoveEnable = bEnable;
}

static float GetSelfToTargetYaw(D3DXVECTOR3 vSefToTarget)
{
    float fYaw = 0.0f;

    fYaw = atan2(vSefToTarget.z, vSefToTarget.x);

    fYaw = D3DX_PI * 0.5f - fYaw;

    return fYaw;
}

KRLSceneObject* KRLCharacterCamera::GetTarget(DWORD dwTargetType, DWORD dwTargetID)
{
    KRLSceneObject* pTarget = NULL;

    if (dwTargetType == ttDoodad)
    {
        pTarget = GetRLDoodad(dwTargetID);
        KGLOG_PROCESS_ERROR(pTarget);
    }
    else
    {
        pTarget = GetRLCharacter(dwTargetID);
        KGLOG_PROCESS_ERROR(pTarget);
    }

Exit0:
    return pTarget;
}

int KRLCharacterCamera::StartFollowTarget(DWORD dwTargetType, DWORD dwTargetID)
{
    int nResult = false;

    ASSERT(dwTargetType != ttNoTarget);

    m_FollowTarget.dwType = dwTargetType;
    m_FollowTarget.dwID = dwTargetID;

    m_FollowTarget.Points.clear();

    nResult = true;
//Exit0:
    if (!nResult)
    {
        m_FollowTarget.dwType = ttNoTarget;
        m_FollowTarget.dwID = 0;
        m_FollowTarget.Points.clear();
    }
    return nResult;
}

void KRLCharacterCamera::StopFollowTarget()
{
    m_FollowTarget.dwType = ttNoTarget;
    m_FollowTarget.dwID = 0;
	
    m_FollowTarget.Points.clear();

//Exit0:
    return;
}

int KRLCharacterCamera::UpdateFollowTarget(KRLSceneObject* pTarget, BOOL* pbMove)
{
    int nResult = false;
    int nRetCode = false;

    ASSERT(pTarget);
    ASSERT(pbMove);

    InvalidatePoints();
	
    nRetCode = AddFollowPoint(pTarget);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateFollowPoint(pbMove);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacterCamera::InvalidatePoints()
{
    int nPreviousX = m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Previous.nX;
    int nPreviousY = m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Previous.nY;
    int nCurrentX = m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Current.nX;
    int nCurrentY = m_RLLocalCharacter.m_RLCharacter.m_FrameData.m_Current.nY;

    if (
        abs(nPreviousX - nCurrentX) < INT_DISTANCE_EPSILON &&
        abs(nPreviousY - nCurrentY) < INT_DISTANCE_EPSILON
    )
    {
        m_FollowTarget.Points.clear();
    }
}

int KRLCharacterCamera::AddFollowPoint(KRLSceneObject* pTarget)
{
    int nResult = false;
    int nRetCode = false;
    D3DXVECTOR3 vTargetPoint;

    ASSERT(pTarget);

    pTarget->GetPosition(vTargetPoint);

    size_t uSize = m_FollowTarget.Points.size();
    if (uSize >= 2)
    {
        float fDeltaX1 = vTargetPoint.x - m_FollowTarget.Points.back().x;
        float fDeltaY1 = vTargetPoint.y - m_FollowTarget.Points.back().y;
        float fDeltaX2 = m_FollowTarget.Points.back().x - m_FollowTarget.Points[uSize - 2].x;
        float fDeltaY2 = m_FollowTarget.Points.back().y - m_FollowTarget.Points[uSize - 2].y;

        float fAngle1 = atan2(fDeltaY1, fDeltaX1);
        float fAngle2 = atan2(fDeltaY2, fDeltaX2);

        // replace the last point if in a line
        if (
            abs(fAngle1 - fAngle2) < ANGLE_EPSILON ||                  // same direction
            abs(abs(fAngle1 - fAngle2) - D3DX_PI) < ANGLE_EPSILON      // opposite direction
        )
        {
            m_FollowTarget.Points.pop_back();
        }
    }
    
    m_FollowTarget.Points.push_back(vTargetPoint);

    nResult = true;
//Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateFollowPoint(BOOL* pbMove)
{
    int nResult = false;
    int nRetCode = false;

    ASSERT(pbMove);

    nRetCode = IsReachPoint(m_FollowTarget.Points.front(), FLT_DISTANCE_EPSILON_TO_TARGET);

    if (nRetCode)
    {
        m_FollowTarget.Points.pop_front();
    }

    if (!m_FollowTarget.Points.empty())
    {
        nRetCode = MoveToPoint(m_FollowTarget.Points.front(), FLT_DISTANCE_EPSILON_TO_TARGET, pbMove);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::MoveToPoint(D3DXVECTOR3 vPoint, float fDistanceEpsilon, BOOL* pbMove)
{
    int nResult = false;
    int nRetCode = false;
    float fYaw = 0.0f;
    D3DXVECTOR3 vSelfPoint(0.0f, 0.0f, 0.0f);

    ASSERT(pbMove);

    nRetCode = IsReachPoint(vPoint, fDistanceEpsilon);
    if (!nRetCode)
    {
        m_RLLocalCharacter.m_RLCharacter.GetPosition(vSelfPoint);

        fYaw = GetSelfToTargetYaw(vPoint - vSelfPoint);
        UpdateYaw(fYaw, TRUE);

        *pbMove = TRUE;
    }

    nResult = true;
//Exit0:
    return nResult;
}

int KRLCharacterCamera::IsReachPoint(D3DXVECTOR3 vPoint, float fDistanceEpsilon)
{
    D3DXVECTOR3 vSelfPoint;
    float fDistance = 0.0f;

    m_RLLocalCharacter.m_RLCharacter.GetPosition(vSelfPoint);
    fDistance = (vSelfPoint.x - vPoint.x) * (vSelfPoint.x - vPoint.x) +
                (vSelfPoint.z - vPoint.z) * (vSelfPoint.z - vPoint.z);

    return (fDistance < fDistanceEpsilon * fDistanceEpsilon);
}

void KRLCharacterCamera::AutoMoveToPoint(D3DXVECTOR3 vPoint)
{
    HRESULT hr = E_FAIL;

    m_vTargetPoint = vPoint;

    hr = ShowSFX(vPoint);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return;
}

int KRLCharacterCamera::UpdateAutoMoveToPoint(BOOL bControls[], BOOL* pbMove)
{
    int nResult = false;
    int nRetCode = false;

    ASSERT(bControls);
    ASSERT(pbMove);

    nRetCode = IsReachPoint(m_vTargetPoint, FLT_DISTANCE_EPSILON_TO_POINT);

    if (nRetCode)
    {
        nRetCode = StopAutoMoveToPoint(bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = MoveToPoint(m_vTargetPoint, FLT_DISTANCE_EPSILON_TO_POINT, pbMove);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::StopAutoMoveToPoint(BOOL bControls[])
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    IKSO3GameWorldUIHandler* pGameWorldUIHandler = NULL;
    KUIEventReachPoint Param;

    KGLOG_PROCESS_ERROR(bControls);

    bControls[RL_CONTROL_AUTO_MOVE_TO_POINT] = FALSE;

    hr = HideSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    pGameWorldUIHandler = g_pRL->m_pGameWorldUIHandler;
    KGLOG_PROCESS_ERROR(pGameWorldUIHandler);

    Param.fX = m_vTargetPoint.x;
    Param.fY = m_vTargetPoint.y;
    Param.fZ = m_vTargetPoint.z;

    pGameWorldUIHandler->OnStopAutoMoveToPoint(Param);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::AutoMoveToTarget(DWORD dwTargetType, DWORD dwTargetID)
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(dwTargetType != ttNoTarget);

    nRetCode = StartFollowTarget(dwTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateAutoMoveToTarget(BOOL bControls[], BOOL* pbMove)
{
    int nResult = false;
    int nRetCode = false;
    D3DXVECTOR3 vTargetPoint;
    KRLSceneObject* pTarget = NULL;

    ASSERT(bControls);
    ASSERT(pbMove);

    pTarget = GetTarget(m_FollowTarget.dwType, m_FollowTarget.dwID);
    if (pTarget == NULL)
    {
        StopAutoMoveToTarget("MISS_TARGET", bControls);
        KG_PROCESS_SUCCESS(true);
    }

    pTarget->GetPosition(vTargetPoint);

    nRetCode = IsReachPoint(vTargetPoint, FLT_DISTANCE_EPSILON_TO_TARGET);
    if (nRetCode)
    {
        float fYaw = 0.0f;
        D3DXVECTOR3 vSelfPoint;

        m_RLLocalCharacter.m_RLCharacter.GetPosition(vSelfPoint);
        fYaw = GetSelfToTargetYaw(vTargetPoint - vSelfPoint);
        UpdateYaw(fYaw, TRUE);

        StopAutoMoveToTarget("REACH", bControls);
        KG_PROCESS_SUCCESS(true);
    }

    nRetCode = UpdateFollowTarget(pTarget, pbMove);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::StopAutoMoveToTarget(char szReason[], BOOL bControls[])
{
    int nResult = false;
    IKSO3GameWorldUIHandler* pGameWorldUIHandler = NULL;
    KUIEventStopAutoMove Param;

    KGLOG_PROCESS_ERROR(szReason);
    KGLOG_PROCESS_ERROR(szReason[0]);
    KGLOG_PROCESS_ERROR(bControls);

    bControls[RL_CONTROL_AUTO_MOVE_TO_TARGET] = FALSE;

    pGameWorldUIHandler = g_pRL->m_pGameWorldUIHandler;
    KGLOG_PROCESS_ERROR(pGameWorldUIHandler);

    Param.pszReason = szReason;
    Param.dwTargetType = m_FollowTarget.dwType;
    Param.dwTargetID = m_FollowTarget.dwID;

    pGameWorldUIHandler->OnStopAutoMoveToTarget(Param);

    StopFollowTarget();

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::StartFollow(DWORD dwTargetType, DWORD dwTargetID)
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(dwTargetType != ttNoTarget);

    nRetCode = StartFollowTarget(dwTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::UpdateFollow(BOOL bControls[], BOOL* pbMove)
{
    int nResult = false;
    int nRetCode = false;
    KRLSceneObject* pTarget = NULL;

    ASSERT(bControls);
    ASSERT(pbMove);

    pTarget = GetTarget(m_FollowTarget.dwType, m_FollowTarget.dwID);
    KG_PROCESS_SUCCESS(pTarget == NULL);

    nRetCode = UpdateFollowTarget(pTarget, pbMove);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    if (pTarget == NULL)
    {
        StopFollow("MISS_TARGET", bControls);
    }
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::StopFollow(char szReason[], BOOL bControls[])
{
    int nResult = false;
    IKSO3GameWorldUIHandler* pGameWorldUIHandler = NULL;
    KUIEventStopAutoMove Param;

    KGLOG_PROCESS_ERROR(szReason);
    KGLOG_PROCESS_ERROR(szReason[0]);
    KGLOG_PROCESS_ERROR(bControls);

    bControls[RL_CONTROL_FOLLOW] = FALSE;

    pGameWorldUIHandler = g_pRL->m_pGameWorldUIHandler;
    KGLOG_PROCESS_ERROR(pGameWorldUIHandler);

    Param.pszReason = szReason;
    Param.dwTargetType = m_FollowTarget.dwType;
    Param.dwTargetID = m_FollowTarget.dwID;

    pGameWorldUIHandler->OnStopFollow(Param);

    StopFollowTarget();

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::GetFollowingTarget(DWORD* pdwTargetType, DWORD* pdwTargetID)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pdwTargetType);
    KGLOG_PROCESS_ERROR(pdwTargetID);

    *pdwTargetType = m_FollowTarget.dwType;
    *pdwTargetID = m_FollowTarget.dwID;

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacterCamera::StopFollowAndAutoMove(BOOL bControls[])
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(bControls);

    if (bControls[RL_CONTROL_FOLLOW])
    {
        nRetCode = StopFollow("STOP", bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (bControls[RL_CONTROL_AUTO_MOVE_TO_POINT])
    {
        nRetCode = StopAutoMoveToPoint(bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (bControls[RL_CONTROL_AUTO_MOVE_TO_TARGET])
    {
        nRetCode = StopAutoMoveToTarget("STOP", bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLCharacterCamera::ShowSFX(D3DXVECTOR3 vPoint)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLScene* pRLScene = NULL;
    KRLCursorEffectModel const* pcCursorEffectModel = NULL;

    pRLScene = m_RLLocalCharacter.m_RLCharacter.m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    pcCursorEffectModel = g_pRL->m_TableList.GetCursorEffectModel(0);
    KGLOG_PROCESS_ERROR(pcCursorEffectModel);

    if (m_p3DSFXModel)
    {
        hr = HideSFX();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (pcCursorEffectModel->szFilePath[0] != _T('\0'))
    {
        hr = KModel::LoadModel(pRLScene, &m_p3DSFXModel, NULL, pcCursorEffectModel->szFilePath);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = KModel::SetPosition(m_p3DSFXModel, vPoint);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = KModel::AppendRenderEntity(pRLScene->m_p3DScene, m_p3DSFXModel, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = KModel::SetScale(m_p3DSFXModel, pcCursorEffectModel->fScale);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLCharacterCamera::HideSFX()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLScene* pRLScene = m_RLLocalCharacter.m_RLCharacter.m_RenderData.GetRLScene();

    KG_PROCESS_SUCCESS(m_p3DSFXModel == NULL);

    KGLOG_PROCESS_ERROR(pRLScene);

    hr = KModel::RemoveRenderEntity(pRLScene->m_p3DScene, m_p3DSFXModel, FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::UnloadModel(pRLScene, &m_p3DSFXModel, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

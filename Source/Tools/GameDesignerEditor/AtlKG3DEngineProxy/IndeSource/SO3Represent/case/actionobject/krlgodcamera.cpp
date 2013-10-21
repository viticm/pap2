#include "stdafx.h"
#include "./krlgodcamera.h"
#include "./krlcharactercamera.h"
#include "./krlsceneobject.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"

HRESULT KRLGodCamera::Reset()
{
    m_vCameraPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vPreCameraPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_vPreObjectPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    m_fMaxHeight = 0.0f;
    m_fMoveSpeed = 0.0f;

    m_bTurnCamera = FALSE;

    return S_OK;
}

HRESULT KRLGodCamera::SetCamera()
{
    return S_OK;
}

int KRLGodCamera::InitRLCharacter(KMovementController* pCameraController)
{
    return true;
}

void KRLGodCamera::UnInitRLCharacter()
{
}

int KRLGodCamera::Zoom(float fScale, float fMaxCameraDistance)
{
    return false;
}

int KRLGodCamera::ToggleControl(RL_CONTROL nIndex, BOOL bEnable)
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
    case RL_CONTROL_JUMP:
    case RL_CONTROL_FOLLOW:
    case RL_CONTROL_AUTO_RUN:
        break;
    case RL_CONTROL_WALK:
        if (bEnable)
            m_fMoveSpeed = g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_WALK_SPEED) * g_pRL->m_TableList.GetNumber(RL_NUMBER_CAMERA_SPEED_RATIO);
        else
            m_fMoveSpeed = g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RUN_SPEED) * g_pRL->m_TableList.GetNumber(RL_NUMBER_CAMERA_SPEED_RATIO);
        break;
    default:
        KG_ASSERT_EXIT(0);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

void KRLGodCamera::BeginDrag()
{
}

void KRLGodCamera::EndDrag()
{
}

void KRLGodCamera::HasDragged(
    BOOL& bObject,
    float& fMaxMouseX,
    float& fMaxMouseY,
    float& fFinalMouseX,
    float& fFinalMouseY
) const
{
    bObject = FALSE;
    fMaxMouseX = 0.0f;
    fMaxMouseY = 0.0f;
    fFinalMouseX = 0.0f;
    fFinalMouseY = 0.0f;
}

void KRLGodCamera::SetRTParams(float fCameraToObjectEyeScale)
{
}
void KRLGodCamera::GetRTParams(float& fCameraToObjectEyeScale) const
{
    fCameraToObjectEyeScale = 0.0f;
}

HRESULT KRLGodCamera::Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame, KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    nRetCode = UpdateControl(pCameraCommonInfo, RL_CONTROL_COUNT, false);
    KGLOG_PROCESS_ERROR(nRetCode);

    hRetCode = SetPosition(pCameraCommonInfo);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}


int KRLGodCamera::UpdateControl(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, RL_CONTROL nIndex, BOOL bEnable)
{
    int nResult = false;
    int nRetCode = false;

    int nForwardMove = 0;
    int nStrafeMove = 0;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    // 鼠标拖拽
    nRetCode = MouseMove(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 前后左右移动
    nRetCode = UpdateMove(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    // 旋转
    nRetCode = UpdateRotation(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLGodCamera::MouseMove(KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
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

        vMouse.x /= lWidth;
        vMouse.y /= lHeight;

        nRetCode = ScaleMouse(pCameraCommonInfo->Params.fDragSpeed, &vMouse.x, &vMouse.y);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = ApplyMouse(pCameraCommonInfo, vMouse);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLGodCamera::ScaleMouse(float fDragSpeed, float* pfMouseX, float* pfMouseY)
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

int KRLGodCamera::ApplyMouse(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, const D3DXVECTOR2& crvMouse)
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

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLGodCamera::UpdateMove(KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
{
    int nResult = false;
    int nRetCode = false;

    int nForwardMove = 0;
    int nStrafeMove = 0;
    int nUpMove = 0;
    float fCameraYaw = 0.0f;
    float fCameraPitch = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    nRetCode = ComputeForwardMove(pCameraCommonInfo->bControls, &nForwardMove);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ComputeStrafeMove(pCameraCommonInfo->bControls, &nStrafeMove);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ComputeUpMove(pCameraCommonInfo->bControls, &nUpMove);
    KGLOG_PROCESS_ERROR(nRetCode);

    fCameraYaw = pCameraCommonInfo->CameraController.GetYaw();
    fCameraPitch = pCameraCommonInfo->CameraController.GetPitch();

    MoveCamera(
        nForwardMove * m_fMoveSpeed,
        nStrafeMove * m_fMoveSpeed,
        nUpMove * m_fMoveSpeed,
        fCameraYaw,
        fCameraPitch
    );

    nResult = true;
Exit0:
    return nResult;
}

int KRLGodCamera::ComputeForwardMove(const BOOL cbControls[], int* pnForwardMove)
{
    int nResult = false;

    int nForwardMove = 0;

    KGLOG_PROCESS_ERROR(cbControls);
    KGLOG_PROCESS_ERROR(pnForwardMove);
 
    if (cbControls[RL_CONTROL_FORWARD] || cbControls[RL_CONTROL_AUTO_RUN])
    {
        ++nForwardMove;
    }

    if (cbControls[RL_CONTROL_BACKWARD])
    {
        --nForwardMove;
    }

    *pnForwardMove = nForwardMove;
    nResult = true;
Exit0:
    return nResult;
}

int KRLGodCamera::ComputeStrafeMove(const BOOL cbControls[], int *pnStrafeMove)
{
    int nResult = false;

    int nStrafeMove = 0;

    KGLOG_PROCESS_ERROR(cbControls);
    KGLOG_PROCESS_ERROR(pnStrafeMove);

    if (cbControls[RL_CONTROL_STRAFE_LEFT])
    {
        ++nStrafeMove;
    }

    if (cbControls[RL_CONTROL_STRAFE_RIGHT])
    {
        --nStrafeMove;
    }

    *pnStrafeMove = nStrafeMove;
    nResult = true;
Exit0:
    return nResult;
}

int KRLGodCamera::ComputeUpMove(const BOOL cbControls[], int *pnUpMove)
{
    int nResult = false;

    int nUpMove = 0;

    KGLOG_PROCESS_ERROR(cbControls);
    KGLOG_PROCESS_ERROR(pnUpMove);

    if (cbControls[RL_CONTROL_UP])
    {
        ++nUpMove;
    }

    if (cbControls[RL_CONTROL_DOWN])
    {
        --nUpMove;
    }

    *pnUpMove = nUpMove;
    nResult = true;
Exit0:
    return nResult;
}

int KRLGodCamera::UpdateRotation(KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
{
    int nResult = false;

    float fRotationYawSpeed = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);

    // 算出旋转的速度
    if (pCameraCommonInfo->bControls[RL_CONTROL_TURN_RIGHT])
        fRotationYawSpeed += ((float)GAME_FPS / 1000.0f) * ((float)DIRECTION_TURN_SPEED / (float)DIRECTION_COUNT * 2.0f * D3DX_PI);
    if (pCameraCommonInfo->bControls[RL_CONTROL_TURN_LEFT])
        fRotationYawSpeed -= ((float)GAME_FPS / 1000.0f) * ((float)DIRECTION_TURN_SPEED / (float)DIRECTION_COUNT * 2.0f * D3DX_PI);

    if (
        !pCameraCommonInfo->bControls[RL_CONTROL_OBJECT_STICK_CAMERA] &&
        !pCameraCommonInfo->bControls[RL_CONTROL_CAMERA] &&
        abs(fRotationYawSpeed) > FLT_EPSILON
    )
    {
        float fDeltaTime = (float)(g_pRL->m_fTime - g_pRL->m_fTimeLast);
        float fRotationYaw = fRotationYawSpeed * fDeltaTime;
        float fCameraYaw = pCameraCommonInfo->CameraController.GetYaw();

        pCameraCommonInfo->CameraController.SetYaw(fCameraYaw + fRotationYaw);
    }

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLGodCamera::OnGetBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const
{
    return S_OK;
}

void KRLGodCamera::MoveCamera(float fForward, float fRight, float fUp, float fCameraYaw, float fCameraPitch)
{
    float fX = 0.0f;
    float fY = 0.0f;
    float fZ = 0.0f;

    fY += fForward * sinf(fCameraPitch);
    fX += fForward * cosf(fCameraPitch) * sinf(fCameraYaw);
    fZ += fForward * cosf(fCameraPitch) * cosf(fCameraYaw);

    fX += fRight * sinf(fCameraYaw - D3DX_PI / 2.0f);
    fZ += fRight * cosf(fCameraYaw - D3DX_PI / 2.0f);

    fY += fUp;

    m_vCameraPosition.x += fX;
    m_vCameraPosition.y += fY;
    m_vCameraPosition.z += fZ;

    if (m_vCameraPosition.y > m_fMaxHeight)
        m_vCameraPosition.y = m_fMaxHeight;
}

HRESULT KRLGodCamera::SetPosition(KG_CAMERA_COMMON_INFO* pCameraCommonInfo)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    D3DXVECTOR3 vObjectPosition(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vDirectionC2O(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vCameraConfine(0.0f, 0.0f, 0.0f);
    float fCameraYaw = 0.0f;
    float fCameraPitch = 0.0f;

    KGLOG_PROCESS_ERROR(pCameraCommonInfo);
    KGLOG_PROCESS_ERROR(pCameraCommonInfo->p3DCamera);

    fCameraYaw = pCameraCommonInfo->CameraController.GetYaw();
    fCameraPitch = pCameraCommonInfo->CameraController.GetPitch();
    GetParallelLine(vDirectionC2O, fCameraYaw, fCameraPitch, 50.0f);

    nRetCode = AdjustCamera(pCameraCommonInfo, vDirectionC2O);
    KGLOG_PROCESS_ERROR(nRetCode);

    vObjectPosition = m_vCameraPosition + vDirectionC2O;

    KG_PROCESS_SUCCESS(m_vCameraPosition == m_vPreCameraPosition && vObjectPosition == m_vPreObjectPosition);

    // 检查镜头除零
    ASSERT(m_vCameraPosition == m_vCameraPosition);
    ASSERT(vObjectPosition == vObjectPosition);

    hr = pCameraCommonInfo->RLTrackCamera.UpdateCamera(m_vCameraPosition, vDirectionC2O, 0.0f, 100.0f);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_vPreCameraPosition = m_vCameraPosition;
    m_vPreObjectPosition = vObjectPosition;

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLGodCamera::AdjustCamera(KG_CAMERA_COMMON_INFO* pCommonInfo, const D3DXVECTOR3& crvDirC2O)
{
    int nResult = false;
    int nRetCode = false;

    HRESULT hr = E_FAIL;
    D3DXVECTOR3 vWaterHeight(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vCross(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vDir(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vAdvanceCameraPos(0.0f, 0.0f, 0.0f);
    float fCameraY = 0.0f;

    KGLOG_PROCESS_ERROR(pCommonInfo);
    KGLOG_PROCESS_ERROR(pCommonInfo->p3DScene);

    // 碰撞处理
    D3DXVec3Normalize(&vDir, &crvDirC2O);
    vAdvanceCameraPos = m_vCameraPosition + vDir * BASE_GLACIS;

    nRetCode = pCommonInfo->RLTrackCamera.GetCollisionPosition(
        vAdvanceCameraPos,
        m_vPreCameraPosition,
        pCommonInfo->p3DCamera,
        &vCross
    );
    if (nRetCode)
        vAdvanceCameraPos = vCross;
    m_vCameraPosition = vAdvanceCameraPos - vDir * BASE_GLACIS;

    // 水面处理
    hr = pCommonInfo->p3DScene->GetWaterHeight(&vWaterHeight, &m_vCameraPosition);
    KG_PROCESS_SUCCESS(FAILED(hr));

    fCameraY = m_vCameraPosition.y;
    if (vAdvanceCameraPos.y < vWaterHeight.y && m_vCameraPosition.y > vWaterHeight.y - BASE_GLACIS)
        fCameraY = vWaterHeight.y - BASE_GLACIS;
    if (vAdvanceCameraPos.y > vWaterHeight.y && m_vCameraPosition.y < vWaterHeight.y + BASE_GLACIS)
        fCameraY = vWaterHeight.y + BASE_GLACIS;

    m_vCameraPosition = vAdvanceCameraPos -
        (vAdvanceCameraPos - m_vCameraPosition) *
        (vAdvanceCameraPos.y - fCameraY) / (vAdvanceCameraPos.y - m_vCameraPosition.y);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

void KRLGodCamera::SetCameraPosition(const D3DXVECTOR3& crvPosition)
{
    m_vCameraPosition = crvPosition;
    m_vPreCameraPosition = crvPosition;
}

void KRLGodCamera::SetCameraMaxHeight(float fMaxHeight)
{
    m_fMaxHeight = fMaxHeight;
}

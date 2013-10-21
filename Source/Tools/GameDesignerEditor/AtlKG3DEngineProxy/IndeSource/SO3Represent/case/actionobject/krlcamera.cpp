#include "stdafx.h"
#include "./krlcamera.h"
#include "../../SO3Represent.h"

namespace
{
    float const LOOK_AT_DISTANCE_MIN = 1.0f;
	float const LOOK_AT_DISTANCE_MAX = 2400.0f;

    float const GOD_CAMERA_TO_OBJECT_MAX_HEIGHT = 2000.0f;
}

KRLCamera::KRLCamera()
{
    Reset();
}

KRLCamera::~KRLCamera()
{
}

HRESULT KRLCamera::SwitchMode(int nMode)
{
    HRESULT hResult = E_FAIL;
    HRESULT hr = E_FAIL;
    D3DXVECTOR3 vCameraPosition(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vObjectPosition(0.0f, 0.0f, 0.0f);

    switch(nMode)
    {
    case RL_CAMERA_MODE_CHARACTER:
        m_pCamera = &m_CharacterCamera;
        break;
    case RL_CAMERA_MODE_GOD:
        {
            hr = m_CommonInfo.p3DCamera->GetPosition(&vCameraPosition);
            KGLOG_COM_PROCESS_ERROR(hr);
            m_GodCamera.SetCameraPosition(vCameraPosition);

            vObjectPosition = m_CharacterCamera.GetObjectPosition();
            m_GodCamera.SetCameraMaxHeight(vObjectPosition.y + GOD_CAMERA_TO_OBJECT_MAX_HEIGHT);

            m_pCamera = &m_GodCamera;
            break;
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KRLCamera::SetCamera(IKG3DCamera* p3DCamera, IKG3DScene* p3DScene)
{
    int nRetCode = false;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DCamera);
    KGLOG_PROCESS_ERROR(p3DScene);

    m_CommonInfo.p3DCamera = p3DCamera;
    m_CommonInfo.p3DScene = p3DScene;

    nRetCode = SetPerspective();
    KGLOG_PROCESS_ERROR(nRetCode);

    m_CommonInfo.CameraController.SetYaw(0.0f);
    m_CommonInfo.CameraController.SetPitch(-0.3f);
    m_CommonInfo.CameraController.SetRoll(0.0f);

    m_pCamera->SetCamera();

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLCamera::Reset()
{
    m_CommonInfo.Params.nResetMode             = RL_RESET_MODE_SMART;
    m_CommonInfo.Params.fDragSpeed             = 1.0f;
    m_CommonInfo.Params.fSpringResetSpeed      = 1.0f;
    m_CommonInfo.Params.fCameraResetSpeed      = 1.0f;
    m_CommonInfo.Params.fMaxCameraDistance     = 1200.0f;
    m_CommonInfo.fRotationYawSpeed             = 0.0f;
    m_CommonInfo.p3DCamera                     = NULL;
    m_CommonInfo.p3DScene                      = NULL;

    memset(m_CommonInfo.bControls, 0, sizeof(m_CommonInfo.bControls));

    m_CharacterCamera.Reset();
    m_GodCamera.Reset();

    m_pCamera = &m_CharacterCamera;

    m_bFrameMoveEnable = FALSE;

    m_bBeingDragged = false;

    return S_OK;
}

int KRLCamera::SetPerspective()
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    float fFov    = 0.0f;
    float fAspect = 0.0f;
    float fNear   = 0.0f;
    float fFar    = 0.0f;

    ASSERT(m_CommonInfo.p3DCamera);

    hr = m_CommonInfo.p3DCamera->GetPerspective(&fFov, &fAspect, &fNear, &fFar);
    KGLOG_COM_PROCESS_ERROR(hr);

    fNear = 10.0f;

    hr = m_CommonInfo.p3DCamera->SetPerspective(fFov, fAspect, fNear, fFar);
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCamera::InitRLCharacter()
{
    m_bFrameMoveEnable = TRUE;
    return m_pCamera->InitRLCharacter(&m_CommonInfo.CameraController);
}

void KRLCamera::UnInitRLCharacter()
{
    m_bFrameMoveEnable = FALSE;
    return m_pCamera->UnInitRLCharacter();
}

void KRLCamera::SetView(float fAngle)
{
    m_CharacterCamera.SetView(fAngle, &m_CommonInfo.CameraController);
}

void KRLCamera::TurnToFaceDirection()
{
    m_CharacterCamera.TurnToFaceDirection(&m_CommonInfo.CameraController);
}

void KRLCamera::Zoom(float fScale)
{
    m_pCamera->Zoom(fScale, m_CommonInfo.Params.fMaxCameraDistance);
}

int KRLCamera::EnableControl(RL_CONTROL nIndex, BOOL bEnable)
{
    int nRetCode = false;
    int nResult = false;

    if (m_CommonInfo.bControls[nIndex] != bEnable)
    {
        m_CommonInfo.bControls[nIndex] = bEnable;

        switch (nIndex)
        {
        case RL_CONTROL_FORWARD:
            if (m_CommonInfo.bControls[nIndex])
                m_CommonInfo.bControls[RL_CONTROL_AUTO_RUN] = FALSE;
            break;
        case RL_CONTROL_BACKWARD:
            if (m_CommonInfo.bControls[nIndex])
                m_CommonInfo.bControls[RL_CONTROL_AUTO_RUN] = FALSE;
            break;
        case RL_CONTROL_TURN_LEFT:
        case RL_CONTROL_TURN_RIGHT:
        case RL_CONTROL_STRAFE_LEFT:
        case RL_CONTROL_STRAFE_RIGHT:
        case RL_CONTROL_UP:
        case RL_CONTROL_DOWN:
        case RL_CONTROL_CAMERA:
        case RL_CONTROL_OBJECT_STICK_CAMERA:
        case RL_CONTROL_WALK:
        case RL_CONTROL_JUMP:
        case RL_CONTROL_AUTO_RUN:
        case RL_CONTROL_FOLLOW:
        case RL_CONTROL_AUTO_MOVE_TO_TARGET:
        case RL_CONTROL_AUTO_MOVE_TO_POINT:
            break;
        default:
            ASSERT(0);
            break;
        }

        nRetCode = StopFollowAndAutoMove();
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = m_pCamera->UpdateControl(&m_CommonInfo, nIndex, bEnable);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCamera::ToggleControl(RL_CONTROL nIndex)
{
    int nRetCode = false;
    int nResult = false;

    KG_ASSERT_EXIT(nIndex >= 0 && nIndex < _countof(m_CommonInfo.bControls));

    m_CommonInfo.bControls[nIndex] = !m_CommonInfo.bControls[nIndex];

    nRetCode = StopFollowAndAutoMove();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pCamera->ToggleControl(nIndex, m_CommonInfo.bControls[nIndex]);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

void KRLCamera::GetYawPitchRoll(float& fYaw, float& fPitch, float& fRoll) const
{
    fYaw   = m_CommonInfo.CameraController.GetYaw();
    fPitch = m_CommonInfo.CameraController.GetPitch();
    fRoll  = m_CommonInfo.CameraController.GetRoll();
}

void KRLCamera::BeginDrag(int& nX, int& nY)
{
    RECT rcClient;
    POINT ptCursor;

    m_bBeingDragged = true;

    ::GetClientRect(g_pRL->m_hWnd, &rcClient);
    ::GetCursorPos(&ptCursor);
    ::ScreenToClient(g_pRL->m_hWnd, &ptCursor);

    nX = (int)ptCursor.x;
    nY = (int)ptCursor.y;

    ptCursor.x = (rcClient.left + rcClient.right) / 2;
    ptCursor.y = (rcClient.top + rcClient.bottom) / 2;
    ::ClientToScreen(g_pRL->m_hWnd, &ptCursor);

    ::SetCursorPos(ptCursor.x, ptCursor.y);

    m_CommonInfo.CameraController.OnBegin();
    m_pCamera->BeginDrag();
}

void KRLCamera::EndDrag(int nX, int nY)
{
    POINT ptCursor;
    ptCursor.x = nX;
    ptCursor.y = nY;

    ::ClientToScreen(g_pRL->m_hWnd, &ptCursor);
    ::SetCursorPos(ptCursor.x, ptCursor.y);

    m_CommonInfo.bHasDraggedCamera = FALSE;
    m_CommonInfo.CameraController.OnEnd();

    m_pCamera->EndDrag();

    m_bBeingDragged = false;
}

void KRLCamera::HasDragged(BOOL& bCamera, BOOL& bObject, float& fMaxMouseX, float& fMaxMouseY, float& fFinalMouseX, float& fFinalMouseY) const
{
    bCamera = m_CommonInfo.bHasDraggedCamera;
    m_pCamera->HasDragged(bObject, fMaxMouseX, fMaxMouseY, fFinalMouseX, fFinalMouseY);
}

int KRLCamera::IsBeingDragged() const
{
    return m_bBeingDragged;
}

int KRLCamera::SetParams(KRepresentCameraParams const& Params)
{
    int nRetCode = false;
    int nResult = false;

    m_CommonInfo.Params = Params;

    m_CommonInfo.Params.fMaxCameraDistance = max(LOOK_AT_DISTANCE_MIN, m_CommonInfo.Params.fMaxCameraDistance);
	m_CommonInfo.Params.nResetMode = max(RL_RESET_MODE_BEGIN, m_CommonInfo.Params.nResetMode);
	m_CommonInfo.Params.nResetMode = min(RL_RESET_MODE_END - 1, m_CommonInfo.Params.nResetMode);

    nResult = true;
    return nResult;
}

void KRLCamera::GetParams(KRepresentCameraParams& Params) const
{
    Params = m_CommonInfo.Params;
}

void KRLCamera::SetRTParams(KRepresentCameraRTParams const& Params)
{
    m_CommonInfo.CameraController.SetYaw(Params.fYaw);
    m_CommonInfo.CameraController.SetPitch(Params.fPitch);
    m_pCamera->SetRTParams(Params.fCameraToObjectEyeScale);
}

void KRLCamera::GetRTParams(KRepresentCameraRTParams& Params) const
{
    Params.fYaw = m_CommonInfo.CameraController.GetYaw();
    Params.fPitch = m_CommonInfo.CameraController.GetPitch();
    m_pCamera->GetRTParams(Params.fCameraToObjectEyeScale);
}

int KRLCamera::UpdateLocal(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLCameraParam RLCameraParam;
    float fDelayTime = 0.0f;

    if (m_bFrameMoveEnable)
    {
        fDelayTime = (float)(g_pRL->m_fTime - g_pRL->m_fTimeLast);

        hr = m_pCamera->Update(fTime, fTimeLast, dwGameLoop, bFrame, &m_CommonInfo);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_CommonInfo.RLTrackCamera.FrameMove(&RLCameraParam, fDelayTime);
        KGLOG_COM_PROCESS_ERROR(hr);

        m_CommonInfo.p3DCamera->SetLookAtPosition(RLCameraParam.vLookAt);
        m_CommonInfo.p3DCamera->SetUpDirection(RLCameraParam.vUp);
        m_CommonInfo.p3DCamera->SetPosition(RLCameraParam.vPosition);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCamera::IsMoving(RL_CONTROL nMoveStyle)
{
    int nIsMoving = m_CharacterCamera.IsMoving(nMoveStyle);
    return nIsMoving;
}

void KRLCamera::MouseControlMoveEnable(BOOL bEnable)
{
    m_CharacterCamera.MouseControlMoveEnable(bEnable);
}

int KRLCamera::AutoMoveToPoint(D3DXVECTOR3 vPoint)
{
    int nResult = false;
    int nRetCode = false;

    EnableControl(RL_CONTROL_AUTO_RUN, FALSE);
    EnableControl(RL_CONTROL_AUTO_MOVE_TO_POINT, TRUE);

    if (m_CommonInfo.bControls[RL_CONTROL_FOLLOW])
    {
        nRetCode = m_CharacterCamera.StopFollow("STOP", m_CommonInfo.bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_CommonInfo.bControls[RL_CONTROL_AUTO_MOVE_TO_TARGET])
    {
        nRetCode = m_CharacterCamera.StopAutoMoveToTarget("STOP", m_CommonInfo.bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    m_CharacterCamera.AutoMoveToPoint(vPoint);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCamera::AutoMoveToTarget(DWORD dwTargetType, DWORD dwTargetID)
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(dwTargetType != ttNoTarget);

    EnableControl(RL_CONTROL_AUTO_RUN, FALSE);
    EnableControl(RL_CONTROL_AUTO_MOVE_TO_TARGET, TRUE);

    if (m_CommonInfo.bControls[RL_CONTROL_FOLLOW])
    {
        nRetCode = m_CharacterCamera.StopFollow("STOP", m_CommonInfo.bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_CommonInfo.bControls[RL_CONTROL_AUTO_MOVE_TO_POINT])
    {
        nRetCode = m_CharacterCamera.StopAutoMoveToPoint(m_CommonInfo.bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = m_CharacterCamera.AutoMoveToTarget(dwTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCamera::StartFollow(DWORD dwTargetType, DWORD dwTargetID)
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(dwTargetType != ttNoTarget);

    EnableControl(RL_CONTROL_AUTO_RUN, FALSE);
    EnableControl(RL_CONTROL_FOLLOW, TRUE);

    if (m_CommonInfo.bControls[RL_CONTROL_AUTO_MOVE_TO_POINT])
    {
        nRetCode = m_CharacterCamera.StopAutoMoveToPoint(m_CommonInfo.bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_CommonInfo.bControls[RL_CONTROL_AUTO_MOVE_TO_TARGET])
    {
        nRetCode = m_CharacterCamera.StopAutoMoveToTarget("STOP", m_CommonInfo.bControls);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = m_CharacterCamera.StartFollow(dwTargetType, dwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCamera::StopFollow()
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = m_CharacterCamera.StopFollow("STOP", m_CommonInfo.bControls);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCamera::IsFollowing(BOOL* pbIsFollowing, DWORD* pdwTargetType, DWORD* pdwTargetID)
{
    int nResult = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(pbIsFollowing);
    KGLOG_PROCESS_ERROR(pdwTargetType);
    KGLOG_PROCESS_ERROR(pdwTargetID);

    *pbIsFollowing = m_CommonInfo.bControls[RL_CONTROL_FOLLOW];

    nRetCode = m_CharacterCamera.GetFollowingTarget(pdwTargetType, pdwTargetID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

BOOL KRLCamera::IsAutoMoving()
{
    return m_CommonInfo.bControls[RL_CONTROL_AUTO_MOVE_TO_TARGET] ||
        m_CommonInfo.bControls[RL_CONTROL_AUTO_MOVE_TO_POINT];
}

int KRLCamera::StopFollowAndAutoMove()
{
    int nResult = false;
    int nRetCode = false;

    BOOL bNeedStopFollowAndAutoMove = FALSE;

    bNeedStopFollowAndAutoMove = 
        m_CommonInfo.bControls[RL_CONTROL_FORWARD] ||
        m_CommonInfo.bControls[RL_CONTROL_BACKWARD] ||
        m_CommonInfo.bControls[RL_CONTROL_STRAFE_LEFT] ||
        m_CommonInfo.bControls[RL_CONTROL_STRAFE_RIGHT] ||
        m_CommonInfo.bControls[RL_CONTROL_AUTO_RUN] ||
        (m_CommonInfo.bControls[RL_CONTROL_TURN_LEFT] && m_CommonInfo.bControls[RL_CONTROL_OBJECT_STICK_CAMERA]) ||
        (m_CommonInfo.bControls[RL_CONTROL_TURN_RIGHT] && m_CommonInfo.bControls[RL_CONTROL_OBJECT_STICK_CAMERA]) ||
        (m_CommonInfo.bControls[RL_CONTROL_CAMERA] && m_CommonInfo.bControls[RL_CONTROL_OBJECT_STICK_CAMERA]);

    KG_PROCESS_SUCCESS(!bNeedStopFollowAndAutoMove);

    nRetCode = m_CharacterCamera.StopFollowAndAutoMove(m_CommonInfo.bControls);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

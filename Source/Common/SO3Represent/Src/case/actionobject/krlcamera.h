#ifndef CASE_ACTION_OBJECT_KRLCAMERA_H
#define CASE_ACTION_OBJECT_KRLCAMERA_H

#include "./ikgrlcamera.h"
#include "./krlgodcamera.h"
#include "./krlcharactercamera.h"

class KRLCharacter;

class KRLCamera
{
public:
    KRLCamera();
    ~KRLCamera();

    HRESULT SetCamera(IKG3DCamera* p3DCamera, IKG3DScene* p3DScene);
    HRESULT Reset();

    int InitRLCharacter();
    void UnInitRLCharacter();

    void SetView(float fAngle);
    void Zoom(float fScale);

    void TurnToFaceDirection();

    int EnableControl(RL_CONTROL nIndex, BOOL bEnable);
    int ToggleControl(RL_CONTROL nIndex);

    void GetYawPitchRoll(float& fYaw, float& fPitch, float& fRoll) const;

    void BeginDrag(int& nX, int& nY);
    void EndDrag(int nX, int nY);
    void HasDragged(BOOL& bCamera, BOOL& bObject, float& fMaxMouseX, float& fMaxMouseY, float& fFinalMouseX, float& fFinalMouseY) const;
    int IsBeingDragged() const;

    int SetParams(KRepresentCameraParams const& Params);
    void GetParams(KRepresentCameraParams& Params) const;

    void SetRTParams(KRepresentCameraRTParams const& Params);
    void GetRTParams(KRepresentCameraRTParams& Params) const;

    HRESULT SwitchMode(int nMode);

    int UpdateLocal(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

    int IsMoving(RL_CONTROL nMoveStyle);

    void MouseControlMoveEnable(BOOL bEnable);

    int AutoMoveToPoint(D3DXVECTOR3 vPoint);
    int AutoMoveToTarget(DWORD dwTargetType, DWORD dwTargetID);
    int StartFollow(DWORD dwTargetType, DWORD dwTargetID);
    int StopFollow();
    int IsFollowing(BOOL* pbIsFollowing, DWORD* pdwTargetType, DWORD* pdwTargetID);
    BOOL IsAutoMoving();
    int StopFollowAndAutoMove();

private:
    int SetPerspective();

    int ClearControlsImpluse();

public:
    KRLCharacterCamera m_CharacterCamera;
    KRLGodCamera m_GodCamera;

private:
    IKGRLCamera* m_pCamera;
    KG_CAMERA_COMMON_INFO m_CommonInfo;

    BOOL m_bFrameMoveEnable;

    int m_bBeingDragged;
};

#endif // CASE_ACTION_OBJECT_KRL_CAMERA_H

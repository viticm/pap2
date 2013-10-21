#ifndef CASE_ACTIONOBJECT_KRLGODCAMERA_H
#define CASE_ACTIONOBJECT_KRLGODCAMERA_H

#include "./ikgrlcamera.h"

class KRLGodCamera : public IKGRLCamera
{
public:
    virtual HRESULT SetCamera();
    virtual HRESULT Reset();

    virtual int InitRLCharacter(KMovementController* pCameraController);
    virtual void UnInitRLCharacter();

    virtual int Zoom(float fScale, float fMaxCameraDistance);

    virtual int UpdateControl(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, RL_CONTROL nIndex, BOOL bEnable);
    virtual int ToggleControl(RL_CONTROL nIndex, BOOL bEnable);

    virtual void BeginDrag();
    virtual void EndDrag();
    virtual void HasDragged(
        BOOL& bObject,
        float& fMaxMouseX,
        float& fMaxMouseY,
        float& fFinalMouseX,
        float& fFinalMouseY
    ) const;

    virtual void SetRTParams(float fCameraToObjectEyeScale);
    virtual void GetRTParams(float& fCameraToObjectEyeScale) const;

    virtual HRESULT Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame, KG_CAMERA_COMMON_INFO* pCameraCommonInfo);

    virtual HRESULT OnGetBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const;

    void SetCameraPosition(const D3DXVECTOR3& crvPosition);
    void SetCameraMaxHeight(float fMaxHeight);

private:
    int MouseMove(KG_CAMERA_COMMON_INFO* pCameraCommonInfo);
    int ScaleMouse(float fDragSpeed, float* pfMouseX, float* pfMouseY);
    int ApplyMouse(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, const D3DXVECTOR2& crvMouse);

    int UpdateMove(KG_CAMERA_COMMON_INFO* pCameraCommonInfo);
    int ComputeForwardMove(const BOOL cbControls[], int* pnForwardMove);
    int ComputeStrafeMove(const BOOL cbControls[], int *pnStrafeMove);
    int ComputeUpMove(const BOOL cbControls[], int *pnUpMove);

    int UpdateRotation(KG_CAMERA_COMMON_INFO* pCameraCommonInfo);
    HRESULT SetPosition(KG_CAMERA_COMMON_INFO* pCameraCommonInfo);
    void MoveCamera(float fForward, float fRight, float fUp, float fCameraYaw, float fCameraPitch);
    int AdjustCamera(KG_CAMERA_COMMON_INFO* pCommonInfo, const D3DXVECTOR3& crvDirC2O);

private:
    D3DXVECTOR3 m_vPreObjectPosition;
    D3DXVECTOR3 m_vCameraPosition;
    D3DXVECTOR3 m_vPreCameraPosition;

    float m_fMaxHeight;
    float m_fMoveSpeed;

    BOOL m_bTurnCamera;
};

#endif // CASE_ACTIONOBJECT_KRLGODCAMERA_H
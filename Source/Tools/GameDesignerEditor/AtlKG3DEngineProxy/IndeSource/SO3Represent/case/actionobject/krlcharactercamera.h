#ifndef CASE_ACTIONOBJECT_KRLCHARACTERCAMERA_H
#define CASE_ACTIONOBJECT_KRLCHARACTERCAMERA_H

#include "./ikgrlcamera.h"
#include "./krllocalcharacter.h"
#include "../gameworld/kgameworldcharactercontroller.h"
#include "../../common/kmathtools.h"

struct RL_TARGET_INFO
{
    DWORD dwType;
    DWORD dwID;
    std::deque <D3DXVECTOR3> Points;

    RL_TARGET_INFO()
    {
        dwType = ttNoTarget;
        dwID = 0;
    }
};

class KRLCharacterCamera : public IKGRLCamera
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

    int IsMoving(RL_CONTROL nMoveStyle);

    D3DXVECTOR3 GetObjectPosition() const;

    void DragMoveObject(float fYaw, float fPitch, float fDeltaYaw, float fDeltaPitch);
    void UpdateYaw(float fYaw, BOOL bTurnImmediately);

    void SetView(float fAngle, KMovementController* pCameraController);

    void TurnToFaceDirection(KMovementController* pCameraController);

    void MouseControlMoveEnable(BOOL bEnable);

    void AutoMoveToPoint(D3DXVECTOR3 vPoint);
    int StopAutoMoveToPoint(BOOL bControls[]);
    int AutoMoveToTarget(DWORD dwTargetType, DWORD dwTargetID);
    int StopAutoMoveToTarget(char szReason[], BOOL bControls[]);
    int StartFollow(DWORD dwTargetType, DWORD dwTargetID);
    int StopFollow(char szReason[], BOOL bControls[]);
    int GetFollowingTarget(DWORD* pdwTargetType, DWORD* pdwTargetID);
    int StopFollowAndAutoMove(BOOL bControls[]);

private:
    HRESULT SetPosition(KG_CAMERA_COMMON_INFO* pCameraCommonInfo);
    int ChangeCameraMovement(D3DXVECTOR3& vObject, D3DXVECTOR3& vCamera, KMovementController* pCameraController);
    int AdjustObject(float fMaxCameraDistance, float fCameraPitch, IKG3DScene* p3DScene, D3DXVECTOR3* pvObject);
    int AdjustCameraRough(
        D3DXVECTOR3& vObject,
        D3DXVECTOR3& vDirectionC2T,
        KG_CAMERA_COMMON_INFO* pCameraCommonInfo,
        D3DXVECTOR3* pvCamera
    );
    int AdjustCameraFine(
        D3DXVECTOR3 vObject,
        D3DXVECTOR3* pvCamera,
        D3DXVECTOR3* pvDirectionC2T, 
        KG_CAMERA_COMMON_INFO* pCameraCommonInfo
    );
    int GetDirectionVector(
        const KG_CAMERA_COMMON_INFO* cpCameraCommonInfo,
        float fCameraPitch,
        D3DXVECTOR3* pvDirectionVector
    ) const;
    int RegressPitch(KMovementController* pCameraController);
    void SmoothObject(float* pfObjectY);
    float SmoothCamera(D3DXVECTOR3& vObject, D3DXVECTOR3& vCamera);

    void UpdateAutoResetCamera(KMovementController* pCameraController, float fCameraResetSpeed);
    int UpdateAlpha();
    int UpdateObjectState(float* pfRotationYawSpeed, BOOL bControls[], BOOL bFrame);

    float GetObjectFaceDirection() const;

    int GetAdjustHeightForWater(IKG3DCamera *p3DCamera, float *pfHeight);

    int MouseMove(KG_CAMERA_COMMON_INFO* pCameraCommonInfo);
    int ScaleMouse(float fDragSpeed, float* pfMouseX, float* pfMouseY);
    int ApplyMouse(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, const D3DXVECTOR2& crvMouse);

    int UpdateMove(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, int* pnForwardMove, int* pnStrafeMove);
    int ComputeForward(const BOOL cbControls[], float fCameraYaw, int* pnForwardMove);
    int ComputeStrafe(const BOOL cbControls[], int *pnStrafeMove);
    int UpdateRotation(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, float* pfRotationYawSpeed);
    int UpdateCharacterRotation(const BOOL cbControls[], float* pfDeltaYaw);
    int UpdateCameraRotation(KMovementController* pCameraController, const BOOL cbControls[], float fDeltaYaw);
    int UpdateAutoReset(KG_CAMERA_COMMON_INFO* pCameraCommonInfo);
    int NeedResetCamera(const BOOL cbControls[], BOOL* pbIsMoveControl);
    int AdjustFootYaw(
        int nForwardMove,
        int nStrafeMove,
        float fRotationYawSpeed,
        const BOOL cbControls[]
    );

    KRLSceneObject* GetTarget(DWORD dwTargetType, DWORD dwTargetID);
    int StartFollowTarget(DWORD dwTargetType, DWORD dwTargetID);
    void StopFollowTarget();
    int UpdateFollowTarget(KRLSceneObject* pTarget,  BOOL* pbMove);
    void InvalidatePoints();
    int AddFollowPoint(KRLSceneObject* pTarget);
    int UpdateFollowPoint(BOOL* pbMove);
    int MoveToPoint(D3DXVECTOR3 vPoint, float fDistanceEpsilon, BOOL* pbMove);
    int IsReachPoint(D3DXVECTOR3 vPoint, float fDistanceEpsilon);
    int UpdateAutoMoveToPoint(BOOL bControls[], BOOL* pbMove);
    int UpdateAutoMoveToTarget(BOOL bControls[], BOOL* pbMove);
    int UpdateFollow(BOOL bControls[], BOOL* pbMove);
    HRESULT ShowSFX(D3DXVECTOR3 vPoint);
    HRESULT HideSFX();

    int PrepareUpdateControl(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, int* pnForward, int* pnStrafe);
    int PostUpdateControl(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, int nForward, int nStrafe);

    int AdjustFaceDirection(RL_CONTROL nIndex, BOOL bEnable);

private:
    enum
    {
        ALTITUDE_AIR,
        ALTITUDE_WATER,
        ALTITUDE_EARTH,
    };

    enum
    {
        PITCH_NONE,
        PITCH_UP,
        PITCH_DOWN,
    };

    enum
    {
        MOVEMENT_ELEVATE,
        MOVEMENT_REGRESS,
        MOVEMENT_FOLLOW,
        MOVEMENT_OUT_WATER,
        MOVEMENT_IN_WATER,
    };

public:
    KRLLocalCharacter m_RLLocalCharacter;
    KGameWorldCharacterController m_CharacterController;

private: 
    KMovementController m_ObjectController;

    D3DXVECTOR3 m_vCameraPosition;
    D3DXVECTOR3 m_vObjectPosition;

    DWORD m_dwObjectMoveState;

    double m_fPitchStartTime;

    float m_fCurrentZoomLength;

    float m_fObjectEyeHeight;
    float m_fCameraToObjectEyeScale;

    float m_fEarthPitch;

    float m_fCharacterDeltaYaw;

    float m_fMaxMouseX;
    float m_fMaxMouseY;
    float m_fFinalMouseX;
    float m_fFinalMouseY;

    float m_fLastPitch;
    int m_nPitchState;

    int m_nObjectAltitudeType;
    int m_nCameraAltitudeType;
    int m_nCameraMovement;
    int m_nPreCameraMovement;

    BOOL m_bWaterArea;
    float m_fWaterLine;

    BOOL m_bFirstPerson;

    BOOL m_bHasDraggedObject;

    BOOL m_bObjectMoving;

    BOOL m_bIgnoreDragObject;

    BOOL m_bControlEnabled;
    BOOL m_bControlLocked;

    BOOL m_bNeedSmoothObject;
    float m_fNeedSmoothObjectHeight;

    BOOL m_bIsRiding;

    float m_fFaceFootDelta;
    BOOL m_bUsingFaceFootDelta;

    BOOL m_bMouseControlMoveEnable;

    RL_TARGET_INFO m_FollowTarget;
    D3DXVECTOR3 m_vTargetPoint;
    BOOL m_bAutoMove;
    IKG3DModel* m_p3DSFXModel;
};

int ComputeMouseMoveDelta(D3DXVECTOR2* pvMouse, LONG* plWidth, LONG* plHeight);

#endif // CASE_ACTIONOBJECT_KRLCHARACTERCAMERA_H
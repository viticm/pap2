#ifndef CASE_ACTIONOBJECT_IKGRLCAMERA_H
#define CASE_ACTIONOBJECT_IKGRLCAMERA_H

#include "../gameworld/kgameworldhandler.h"
#include "../../common/kmovementcontroller.h"
#include "./krltrackcamera.h"

#define OBJECT_ROTATION_YAW_VELOCITY D3DX_PI / 2000.0f
#define BASE_GLACIS 12.0f                               // 3D隔离带
#define WATER_UP_GLACIS BASE_GLACIS + 5.0f              // 水上隔离带
#define WATER_DOWN_GLACIS BASE_GLACIS + 10.0f           // 水下隔离带

enum
{
    RL_RESET_MODE_BEGIN,

    RL_RESET_MODE_NONE = RL_RESET_MODE_BEGIN,
    RL_RESET_MODE_SMART,
    RL_RESET_MODE_ALWAYS,

    RL_RESET_MODE_END,
};

enum RL_CONTROL
{
    RL_CONTROL_FORWARD,
    RL_CONTROL_BACKWARD,
    RL_CONTROL_TURN_LEFT,
    RL_CONTROL_TURN_RIGHT,
    RL_CONTROL_STRAFE_LEFT,
    RL_CONTROL_STRAFE_RIGHT,
    RL_CONTROL_CAMERA,
    RL_CONTROL_OBJECT_STICK_CAMERA,
    RL_CONTROL_WALK,
    RL_CONTROL_JUMP,
    RL_CONTROL_AUTO_RUN,
    RL_CONTROL_FOLLOW,
    RL_CONTROL_UP,
    RL_CONTROL_DOWN,
    RL_CONTROL_AUTO_MOVE_TO_POINT,
    RL_CONTROL_AUTO_MOVE_TO_TARGET,

    RL_CONTROL_COUNT,
};

enum
{
    RL_CAMERA_MODE_BEGIN,

    RL_CAMERA_MODE_CHARACTER = RL_CAMERA_MODE_BEGIN,
    RL_CAMERA_MODE_GOD,

    RL_CAMERA_MODE_END,
};

enum { RL_CONTROL_STATE_ENABLE = 0x0000FFFF };
enum { RL_CONTROL_IMPLUSE_ENABLE = 0x00010000 };
enum { RL_CONTROL_IMPLUSE_DISABLE = 0x00020000 };

struct KG_CAMERA_COMMON_INFO 
{
    KRLTrackCamera RLTrackCamera;
    KMovementController CameraController;
    KRepresentCameraParams Params;

    IKG3DCamera* p3DCamera;
    IKG3DScene* p3DScene;

    float fRotationYawSpeed;

    BOOL bHasDraggedCamera;

    int nControls[RL_CONTROL_COUNT];
};

struct KRLCameraParam
{
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vLookAt;
    D3DXVECTOR3 vUp;
    D3DXVECTOR3 vTarget;
};

class KRLCharacter;

class IKGRLCamera
{
public:	
    virtual HRESULT SetCamera() = 0; 
    virtual HRESULT Reset() = 0;

    virtual int InitRLCharacter(KMovementController* pCameraController) = 0;
    virtual void UnInitRLCharacter() = 0;

    virtual int Zoom(float fScale, float fMaxCameraDistance) = 0;

    virtual int UpdateControl(KG_CAMERA_COMMON_INFO* pCameraCommonInfo, RL_CONTROL nIndex, BOOL bEnable) = 0;
    virtual int ToggleControl(RL_CONTROL nIndex, BOOL bEnable) = 0;

    virtual void BeginDrag() = 0;
    virtual void EndDrag() = 0;
    virtual void HasDragged(
        BOOL& bObject,
        float& fMaxMouseX,
        float& fMaxMouseY,
        float& fFinalMouseX,
        float& fFinalMouseY
    ) const = 0;

    virtual void SetRTParams(float fCameraToObjectEyeScale) = 0;
    virtual void GetRTParams(float& fCameraToObjectEyeScale) const = 0;

    virtual HRESULT Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame, KG_CAMERA_COMMON_INFO* pCameraCommonInfo) = 0;
    virtual HRESULT PostUpdate(KRLCameraParam const& RLCameraParam) = 0;

    virtual HRESULT OnGetBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const = 0;
};

#endif // CASE_ACTIONOBJECT_IKGRLCAMERA_H
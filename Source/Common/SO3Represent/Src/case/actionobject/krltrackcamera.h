#ifndef CASE_ACTIONOBJECT_KRLTRACKCAMERA_H
#define CASE_ACTIONOBJECT_KRLTRACKCAMERA_H

struct KRLCameraParam;
class IKG3DCamera;

struct KRLSpringParam
{
    float fDistance;
    float fTime;
    float fVelocity;
    float fEpsilon;
};

struct KRLSpringResult
{
    int nFinished;
    float fDistance;
    float fVelocity;
};

class KRLTrackCamera
{
public:
    KRLTrackCamera();
    ~KRLTrackCamera();

    HRESULT UpdateCamera(D3DXVECTOR3 vTarget, D3DXVECTOR3 vPosition, D3DXVECTOR3 vFront, float fWheelOffset, float fLookAtOffset);

    HRESULT FrameMove(KRLCameraParam* pResultParam, float fDelayTime);


    // 五射线检测，如果有交点，返回离目标点最近的交点
    int GetCollisionPosition(
        const D3DXVECTOR3& crvCameraPos,
        const D3DXVECTOR3& crvTargetPos,
        IKG3DCamera* p3DCamera,
        D3DXVECTOR3* pvCrossPos
    );

    void SetCollisionFlag(int nFlag);

private:
    void DetectObstruct(float fExpectDistance);
    void StopSpringSystem(float fDistance);

private:
    D3DXVECTOR3 m_vTarget;
    D3DXVECTOR3 m_vExpectTarget;

    D3DXVECTOR3 m_vPosition;
    D3DXVECTOR3 m_vExpectFront;
    float m_fExpectDistance;

    D3DXVECTOR3 m_vLookAt;

    float m_fDistance;

    float m_fCurrentVelocity;     // 当前做弹簧运动的速度

    float m_fWheelOffset;          // 相机到轴心的长度通过鼠标滑轮拉伸了的长度

    int m_nUseFlexFlag;         // 是否要做弹簧运动(在出现遮档后，此标志设为True)
    int m_nUpdateFlag;          // 需要更新相机位置标志(UpdateCamera函数被调用后设置为True)
/*    int m_nObstrucFlag;         // 遮档标志(出现遮档后设置为True)*/
    int m_nCollisionFlag;       // 镜头期望点与障碍物发生碰撞
};

int RunSpringSystem(KRLSpringParam RLSpringInParam, KRLSpringResult* pRLSpringOutParam);

#endif // CASE_ACTIONOBJECT_KRLTRACKCAMERA_H
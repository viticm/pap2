#ifndef CASE_ACTIONOBJECT_KRLTRACKCAMERA_H
#define CASE_ACTIONOBJECT_KRLTRACKCAMERA_H

struct KRLCameraParam;

class KRLTrackCamera
{
public:
    KRLTrackCamera();
    ~KRLTrackCamera();

    HRESULT UpdateCamera(D3DXVECTOR3 vPosition, D3DXVECTOR3 vFront, float fFlexOffset, float fLookAtOffset);

    HRESULT FrameMove(KRLCameraParam* pResultParam, float fDelayTime);


    // 五射线检测，如果有交点，返回离目标点最近的交点
    int GetCollisionPosition(
        const D3DXVECTOR3& crvCameraPos,
        const D3DXVECTOR3& crvTargetPos,
        IKG3DCamera* p3DCamera,
        D3DXVECTOR3* pvCrossPos
    );

    void SetFlexOffset(float fFlexOffset);

    void SetCollisionFlag(int nFlag);

private:
    void DetectObstruct(const D3DXVECTOR3& crvPreCameraPos);

private:
    D3DXVECTOR3		m_vPosition;
    D3DXVECTOR3		m_vExpectPosition;

    D3DXVECTOR3		m_vLookAt;
    D3DXVECTOR3		m_vExpectLookAt;

    D3DXVECTOR3     m_vCurrentVelocity;     // 当前做弹簧运动的速度

    float           m_fFlexOffset;          // 相机到轴心的长度通过鼠标滑轮拉伸了的长度

    float           m_fSpringK;             // 弹性系数  
    float           m_fDampingK;            // 阻尼系数  

    int             m_nUseFlexFlag;         // 是否要做弹簧运动(在出现遮档后，此标志设为True)
    int             m_nUpdateFlag;          // 需要更新相机位置标志(UpdateCamera函数被调用后设置为True)
    int				m_nObstrucFlag;         // 遮档标志(出现遮档后设置为True)
    int             m_nCollisionFlag;       // 镜头期望点与障碍物发生碰撞
};

#endif // CASE_ACTIONOBJECT_KRLTRACKCAMERA_H
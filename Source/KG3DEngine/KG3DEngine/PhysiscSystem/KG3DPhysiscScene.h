////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPhysiscScene.h
//  Version     : 
//  Creator     : 
//  Create Date : 
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DPHYSISCSCENE_H_
#define _INCLUDE_KG3DPHYSISCSCENE_H_

////////////////////////////////////////////////////////////////////////////////
struct FLEXIBLE_NORMAL_BONE 
{
    const char *pcszName;
    D3DXMATRIX  InitPose;
    D3DXMATRIX  WorldPose;
    float       fLength;
};

struct FLEXIBLE_BONE_CHAIN 
{
    unsigned uFirstBoneIndex;
    unsigned uBoneCount;
};

struct FLEXIBLE_DRIVER_BONE
{
    const char *pcszName;
    D3DXMATRIX  InitPose;
    D3DXMATRIX  WorldPose;    
    std::vector<FLEXIBLE_BONE_CHAIN> vecBoneChain;
};

////////////////////////////////////////////////////////////////////////////////
class KG3DPhysiscScene
{
public:
    KG3DPhysiscScene(void);
    virtual ~KG3DPhysiscScene(void);
 
    HRESULT Init(NxPhysicsSDK *pPhysicsSDK);
    HRESULT Unint(NxPhysicsSDK *pPhysicsSDK);

    HRESULT ForceFinishSimulate();

    HRESULT FrameMove(
        DWORD dwDeltaTime, 
        const D3DXVECTOR3 &crvCameraPos, 
        const D3DXVECTOR3 &crvLookAtPos
    );
    HRESULT Refresh();

    HRESULT CreateFlexibleBody(
        unsigned uDriverBoneCount, FLEXIBLE_DRIVER_BONE *aDriverBonesPtr[],
        unsigned uNormalBoneCount, FLEXIBLE_NORMAL_BONE *aNormalBonesPtr[],
        HANDLE *phRetFlexibleBody
    );
    HRESULT UpdateFlexibleBody(HANDLE hFlexibleBody);
    HRESULT ReleaseFlexibleBody(HANDLE hFlexibleBody);

private:
    HRESULT _AutoRefresh();
    HRESULT _ReloadPrarm();
    HRESULT _ShowDebugInfo(DWORD dwDeltaTime);
    HRESULT _ShowDebugInfo_Scene(DWORD dwDeltaTime);
    HRESULT _ShowDebugInfo_Joint();
    HRESULT _ShowDebugInfo_Actor();
    HRESULT _AutoAdjustPerformance();

    KG3DPhysiscParam* _QueryPhysiscParam(LPCTSTR szBoneName);

    NxSphericalJoint* _CreateSphericalJoint(
        NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis, KG3DPhysiscParam* pParam
    );
    NxActor* _CreateCapsule(const char cszName[], const NxMat34& Pose, NxReal height, const KG3DPhysiscParam &crParam);
    
    HRESULT _ProcessCommandQueue();

    enum _UPDATE_TYPE
    {
        _UPDATE_FROM_DRIVER_BONE,
        _UPDATE_FROM_PHYSX
    };
    enum _PHYSX_COMMAND
    {
        _PHYSX_NOTHING,
        _PHYSX_UPDATE,
        _PHYSX_MOVE
    };
    struct _FLEXIBLE_BODY;
    HRESULT _DoCreateFlexibleBody(_FLEXIBLE_BODY *pFlexibleBody);
    HRESULT _DoUpdateFlexibleBody(_FLEXIBLE_BODY *pFlexibleBody);
    HRESULT _DoSetFlexibleBodyPos(_FLEXIBLE_BODY *pFlexibleBody);
    HRESULT _DoReleaseFlexibleBody(_FLEXIBLE_BODY *pFlexibleBody);
    HRESULT _ReleaseNxObject(_FLEXIBLE_BODY *pFlexibleBody);
    HRESULT _UpdateFlexibleBody_FromDriverBone(_FLEXIBLE_BODY *pFlexibleBody);
    HRESULT _UpdateFlexibleBody_FromNx(_FLEXIBLE_BODY *pFlexibleBody);
    HRESULT _GetFlexibleBodyUpdateType(
        _FLEXIBLE_BODY *pFlexibleBody, 
        _UPDATE_TYPE *pnRetUpdateType,
        _PHYSX_COMMAND *pnRetPhysXCmd
    );

private:
    std::map<int, KG3DPhysiscParam> m_mapPhysicsParam;

    NxScene *m_pNxScene;
    int      m_nIsSimulating;
    DWORD    m_dwDeltaGameTime;

    unsigned m_uGameFrameCount;
    unsigned m_uPhysicsFrameCount;
    DWORD    m_dwGameTimeTotalSimulate;   // microsecond, Game time need physics system to simulate,
    DWORD    m_dwPhysicsTimeTotalSimulate;// millisecond, time of the physics system used to simulate the game time

    D3DXVECTOR3 m_vCameraPos;
    D3DXVECTOR3 m_vCamera2LookAt;
    struct _PERFORMANCE_ADJUST
    {
        float fNeedAdjustLowLevel;
        float fNeedAdjustHighLevel;

        float fMaxSolverIterationCount;

        DWORD    dwLastGameTime;
        DWORD    dwLastPhysicsTime;
        unsigned uLastGameFrame;
        unsigned uLastPhysicsFrame;
    } m_PerformanceAdjust;

    struct _DEBUG_INFO 
    {
        // scene
        unsigned uLastGameFrame;
        DWORD    dwLastGameTime;
        float    fAvgGameFPS;
        float    fAvgGameTime;
      
        unsigned uLastPhysicsFrame;
        DWORD    dwLastPhysicsTime;
        float    fAvgPhysicsFPS;
        float    fAvgPhysicsTime;
        
        // joint
        float    fMaxAnchorDeviation;
        float    fMaxAxisDeviation;
        float    fMaxAxisDeviationPercent;

        unsigned uDeltaJointCount;
        unsigned uDeltaJointDeviationCount;
        float    fDeltaJointAxisDeviation;
        float    fDeltaJointAxisDeviationPercent;
        float    fAvgJointAxisDeviation;
        float    fAvgJointAxisDeviationPercent;
        float    fAvgJointAxisDeviation2;
        float    fAvgJointAxisDeviationPercent2;

        // actor
        unsigned uDeltaActorCount;
        unsigned uDeltaIterCount;
        float    fAvgIterCount;
        unsigned uMaxIterCount;
        unsigned uMinIterCount;
    } m_DebugInfo;

    struct _GLOBAL_CONFIG 
    {
        int   nAutoRefreshTime; // check config file modify and refresh
        int   nAutoAdjustPerformance;
        int   nShowDebugInfoTime;
        int   nSubstepPerFrame;
    } m_GlobalConfig;
    
    enum COMMAND
    {
        CMD_CREATE_FLEXIBLE,
        CMD_REFRESH_FLEXIBLE,
        CMD_UPDATE_FLEXIBLE,
        CMD_MOVE_FLEXIBLE,
        CMD_RELEASE_FLEXIBLE
    };
    struct COMMAND_QUEUE_NODE
    {
        COMMAND eCmd;
        void   *pvData;
        COMMAND_QUEUE_NODE(COMMAND eCmd, void *pvData)
        {
            this->eCmd   = eCmd;
            this->pvData = pvData;
        }
    };
    std::queue<COMMAND_QUEUE_NODE> m_CmdQueue;

    struct _FLEXIBLE_BODY 
    {
        enum PHYSICS_STATUE
        {
            WAIT_CREATE,
            WAIT_UPDATE,
            WAIT_RELEASE
        } ePhisicsStatue; // if use multi thread, should lock this when modify

        int nVisiable;
        unsigned uInputCount;
        struct INPUT
        {
            FLEXIBLE_DRIVER_BONE *pDriverBone;

            D3DXMATRIX  matRevert;
            NxActor    *pActor;
        } *pInputs;

        unsigned uOutputCount;
        struct OUTPUT
        {
            FLEXIBLE_NORMAL_BONE *pNormalBone;

            // Internal use
            D3DXMATRIX  matRevert;
            NxActor    *pActor;
            NxJoint    *pJoint;
        } *pOutputs;
    };
    std::set<_FLEXIBLE_BODY *> m_FlexibleBodySet;
};

#endif //_INCLUDE_KG3DPHYSISCSCENE_H_

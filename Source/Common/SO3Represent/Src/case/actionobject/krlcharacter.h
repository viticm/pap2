#ifndef CASE_ACTION_OBJECT_KRLCHARACTER_H
#define CASE_ACTION_OBJECT_KRLCHARACTER_H

#include "./krlsceneobject.h"
#include "./krlskillsequence.h"
#include "./krlmissile.h"
#include "../gameworld/ktabletransform.h"
#include "../render/krlresource.h"
#include "../render/kanimationmodel.h"

enum RL_ANIMATION_TYPE
{
    RL_ANIMATION_NONE,

    RL_ANIMATION_MONO,
    RL_ANIMATION_SPLIT,
};

enum RL_ANIMATION_STATE
{
    RL_ANIMATION_STATE_IMPULSE,
    RL_ANIMATION_STATE_HOLD_ONCE,   // 维持状态，并且只播放循环动作或者播放一次动作的最后一帧
    RL_ANIMATION_STATE_HOLD_LOOP,   // 维持状态，但每次播放完一个动作，需要选择另外一个
};

struct KRLMonoAnimationStatus
{
    KRLMonoAnimationParam MonoParam;
};

struct KRLSplitAnimationStatus
{
    KRLSplitAnimationParam UpLowTrackParam;
    KRLSplitAnimationParam UpHighTrackParam;

    KRLSplitAnimationParam DownLowTrackParam;
    KRLSplitAnimationParam DownHighTrackParam;
};

struct KRLAnimationStatus
{
    RL_ANIMATION_TYPE nType;

    RL_ADVANCE nAdvance;

    union
    {
        KRLMonoAnimationStatus MonoStatus;
        KRLSplitAnimationStatus SplitStatus;
    };
};

struct KRLAnimationCommonInfo
{
    DWORD dwAnimationID;
    DWORD dwKindID;
    DWORD dwTrackID;
    RL_ANIMATION_STATE nState;
};

struct KAnimationUpInfo : KRLAnimationCommonInfo
{
    RL_ANIMATION_UP nAnimationUpType;
    RL_SHEATH_SOCKET nSheathSocket;
};

struct KAnimationDownInfo : KRLAnimationCommonInfo
{
    RL_ANIMATION_DOWN nAnimationDownType;

    void Init(BOOL bAll)
    {
        DWORD dwPrevAnimationID = dwAnimationID;
        RL_ANIMATION_STATE nPrevState = nState;

        memset(this, 0, sizeof(KAnimationDownInfo));
        if (!bAll)
        {
            dwAnimationID = dwPrevAnimationID;
            nState = nPrevState;
        }
    }
};

struct KRLAniInfo
{
    DWORD dwAnimationID;
    RL_ANIMATION_STATE nState;
    RL_ANIMATION_UP nUpState;
    RL_ANIMATION_DOWN nDownState;
    int nOffsetTime;
    void* pUserdata;
};

class KRLRides;
class KRLCharacterFrameData;

struct KRLCharacterDisplayData
{
    BOOL bSplitModel;

    DWORD dwModelID;

    KRLRides* pRLHorse;
    KRLRides* pRLVehicle;
};

class KRLCharacter : public KRLSceneObject
{
    // TODO: to be removed
    friend class KRLRides;
    friend class KRLLocalCharacter;
    friend class KRLRemoteCharacter;

private:
    struct KRLCharacterControl
    {
        int (KRLCharacter::*pUpdateWeapon)(BOOL bSheath);
        int (KRLCharacter::*pUpdateSheath)(BOOL bSheath);
        void (KRLCharacter::*pUpdateAnimationSpeed)(float fSpeed);
        void (KRLCharacter::*pPlayAnimation)(KRLAniInfo const& AniInfo);
        void (KRLCharacter::*pTurnFaceBegin)(float fDeltaDirection);
        void (KRLCharacter::*pTurnFaceEnd)();
    };

public:
    KRLCharacter();
    virtual ~KRLCharacter();

    int Init(KCharacter* pCharacter, KRLScene* pRLScene, KRLCharacterFrameData* pFrameData);
    int Reset();

    int ResetSelectable();
    int SetSelectable(BOOL bSelectable);

    int LoadRides(DWORD dwRepresentID, KRLRides** ppRLRides);
    int UnloadRides(KRLRides** ppRLRides);

    int UpdateSkillBuff(KSkillBuffParam const* pSkillBuff, size_t nSkillBuffCount);

    int PlaySpeech(DWORD dwTarget, BOOL bTurnToTarget, LPCTSTR pszFilePath);
    int StopSpeech();

    int PlaySound(DWORD dwSoundID);
    int PlayLevelUpSFX(REPRESENT_SFX nSFX);

    int PlayAnimation(DWORD dwAnimationID, RL_ANIMATION_STATE nState, RL_ANIMATION_UP nUpState, RL_ANIMATION_DOWN nDownState, int nOffsetTime = 0, void* pUserdata = NULL);
    int ResetAnimation();

    int PrepareCastSkill(KSkillCasterParam const& SkillCasterParam);
    int CastSkill(KSkillCasterParam const& SkillCasterParam, DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwBulletID);

    int PlaySkillEffectResultAnimation(RL_ANIMATION_UP nAnimationUp, RL_ANIMATION_TABLE nAnimationTable, KSkillCasterModel const* pSkillCasterModel);
    int PlaySkillEffectResultSFX(DWORD dwEffectIDs[CHARACTER_SFX_SKILL_EFFECT_COUNT], DWORD dwCasterID);

    int BeginPlaySerialAnimation(DWORD dwSerialID);
    int EndPlaySerialAnimation(DWORD dwSerialID);

    void UpdateSheath(BOOL bSheathFlag);
    void UpdateAnimationSpeed(float fSpeed);
    int UpdateBalloon();
    int UpdateTitleMark();
    int UpdateTitleMarkPosition();

    int UpdateModel(KRLCharacterEquipResource const& EquipResource, BOOL bForceUpdate);

    int UpdateAnimationDown(RL_ANIMATION_DOWN nAnimationDown, RL_ADVANCE nAdvance, BOOL bSlip, RL_FIGHT_FLAG nFightFlag, DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT]);
    int UpdateAnimationUp(RL_ANIMATION_UP nAnimationUp);

    void TurnFace(float fDeltaDirection);

    float GetRideAdjustHeight() const;

    HRESULT LoadTitleMark(DWORD dwEffect);
    HRESULT UnloadTitleMark();

    int FillRLAdjustAnimationParam(KRLAdjustAnimationParam* pParam, BOOL bSheathFlag) const;

protected:
    virtual HRESULT OnPositionChange(D3DXVECTOR3 const& vPosition);
    virtual HRESULT OnRotationLHChange(D3DXQUATERNION const& qRotationLH);
    virtual HRESULT OnAlphaChange(float fAlpha);
    virtual HRESULT OnGetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const;
    virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);
    virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);

private:
    KRLCharacter(KRLCharacter const& rhs);
    KRLCharacter& operator=(KRLCharacter& rhs);

    void UpdateWeaponSocketPosition(BOOL bSheath);
    void UpdateWeaponPosition(RL_SHEATH_SOCKET nSheathSocket, BOOL bSheath);

    int UpdateRide();
    int UpdateVehicle(int nVehicleTrack, BOOL bForceUpdate);

    int UnloadModel();
    int LoadModel(KRLCharacterEquipResource const& EquipResource, BOOL bForceUpdate);
    int LoadSocket(int nIndex, KRLSocketResource const SocketParam, int nColorChannelTable);
    int BindSocket(int nSocket, LPCTSTR szSocket);

    DWORD GetAnimationID(RL_ANIMATION_TABLE nAnimationTable, BOOL bSheathFlag) const;

    RL_ANIMATION_TABLE GetAnimationTable(RL_ANIMATION_DOWN nDownType, RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag) const;

    int UpdateAnimation(RL_ANIMATION_DOWN nAnimationDown, RL_ANIMATION_STATE nAnimationState, RL_ADVANCE nAdvance,
        BOOL bSlip, RL_FIGHT_FLAG nFightFlag, DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT]);

    int CanPlaySplitAnimation(enuAnimationJointType nJointType, DWORD dwAnimationID, DWORD dwAnimationTrackID, KRLAnimationParam const& Animation) const;
    int PlaySplitAnimation(enuAnimationJointType nJointType, DWORD dwAnimationID, RL_ANIMATION_STATE nState, int nAnimationState, int nOffsetTime = 0, void* pUserdata = NULL);

    int PauseAnimation(BOOL bPause);

    int PauseAnimationStatus(KRLCharacterDisplayData* pRLCharacterDisplayData);
    int ResumeAnimationStatus(KRLCharacterDisplayData const* pRLCharacterDisplayData);

    int UpBodyHighTrackAnimationFinish();
    int UpBodyLowTrackAnimationFinish();

    int DownBodyHighTrackAnimationFinish();
    int DownBodyLowTrackAnimationFinish();

    int WholeBodyHighTrackAnimationFinish();
    int WholeBodyLowTrackAnimationFinish();
    
    void LoadSkillBuff(size_t nIndex, KSkillBuffParam SkillBuff);
    void UnloadSkillBuff(size_t nIndex);

    int InitRLControl();

    // Player Function
    int UpdatePlayerSheath(BOOL bSheathFlag);
    void UpdatePlayerAnimationSpeed(float fSpeed);
    int UpdatePlayerWeaponSocket(BOOL bSheath);
    void PlayPlayerAnimation(KRLAniInfo const& AniInfo);
    void TurnPlayerFaceBegin(float fDeltaDirection);
    void TurnPlayerFaceEnd();

    // Npc Function
    void UpdateNpcAnimationSpeed(float fSpeed);
    void PlayNpcAnimation(KRLAniInfo const& AniInfo);
    void TurnNpcFaceBegin(float fDeltaDirection);
    void TurnNpcFaceEnd();

    // Npc private function
    void PlayWholeAnimation(KRLAniInfo Info);
    int UpdateNpcModel(DWORD dwAnimationID);

    int LoadSocketByAniamtion(DWORD dwAnimationID);

    int UpdateVehicleVisible();
    int PlaySound(int nType, LPCTSTR pszFilePath, float fMinDistance, float fMaxDistance, PDWORD pdwSoundID);
    int StopSound(DWORD dwSoundID);

    int PlayEquipSFXSequence(DWORD dwEquipType, DWORD dwSequenceID);

public:
    BOOL m_bCurrentPlayer;    // TODO: move to local character 逻辑是否是当前Player 

    KAnimationModel m_RenderData;
    KRLCharacterFrameData* m_pFrameData;

    KRLCharacterEquipResource m_EquipResource;

    KRLRides* m_pRLHorse;
    KRLRides* m_pRLVehicle;

    BOOL m_bAnimationPaused;

private:
    KRLSkillSequence m_SkillSequence;

    DWORD m_dwTitleMarkEffect;

    DWORD m_dwSpeechID;

    BOOL m_bSheath;
    
    float m_fRideAdjustHeight;

    RL_ANIMATION_UP m_nUpBodyState;
    RL_ANIMATION_DOWN m_nDownBodyState;

    DWORD m_dwSerialAnimationID;

    KAnimationUpInfo m_UpBodyHighAnimationTrack;
    KAnimationUpInfo m_UpBodyLowAnimationTrack;

    KAnimationDownInfo m_DownBodyHighAnimationTrack;
    KAnimationDownInfo m_DownBodyLowAnimationTrack;

    KAnimationUpInfo* m_pUpBodyCurrentAnimationTrack;
    KAnimationDownInfo* m_pDownBodyCurrentAnimationTrack;

    KRLAnimationStatus m_AnimationStatus;

    KRLCharacterControl m_RLControl;

    BOOL m_bSlip;
    RL_FIGHT_FLAG m_nFightFlag;
    DWORD m_dwSkillBuffs[SKILL_BUFF_PART_COUNT];
    int m_nVehicleTrack;

    int m_nPlayerEquipSFXSequence[perRepresentCount][EQUIP_SFX_SEQUENCE_COUNT];
    int m_nEquipSFXSequenceModelIndex;
};

#endif // CASE_ACTION_OBJECT_KRLCHARACTER_H
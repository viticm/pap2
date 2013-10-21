#ifndef CASE_ACTION_OBJECT_KRLLOCALCHARACTER_H
#define CASE_ACTION_OBJECT_KRLLOCALCHARACTER_H

#include "./krlcharacter.h"
#include "./krlcharacterturning.h"
#include "../../common/kmathtools.h"

 // controlled by camera

class KRLLocalCharacter
{
public:
    void Init();
    void Reset();

    int Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);
    int UpdateDisplayData(BOOL bForceUpdate);
    int UpdateSkillBuff();

    void SetFaceYaw(float fFaceYaw);
    void SetFaceExpectYaw(float fFaceYaw);
    void SetFacePitch(float fFacePitch);
    void SetFootYaw(float fFootYaw);
    void SetFootExpectYaw(float fFootYaw);

    void Turn(DWORD dwTurnFlags, BOOL bEnable);

    int PrepareCastSkill(DWORD dwSkillID, DWORD dwSkillLevel);
    int CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwBulletID);

private:
    int UpdateDirection();
    int UpdatePosition();

    int UpdateFaceFootDirection(RL_TURNING nTurning);
    
    int SetFaceFootDirection(float fFaceYaw, float fFacePitch, float fFootYaw);

    void UpdateAnimationSpeed(RL_ANIMATION_DOWN nAnimationDown, RL_ADVANCE nAdvance);
    RL_ADVANCE GetAdvance(RL_ANIMATION_DOWN nAnimationDown);

    int GetSlopeRotation(D3DXQUATERNION& qRotationLH, RL_TERRAIN_SLOPE nTerrainSlope);
    
    void UpdateLifeState();

    int UpdateListener();

public:
    KRLCharacter m_RLCharacter;

    DWORD m_dwLockControlFrame;

    RL_TURNING m_nTurning;

    float m_fFaceYaw;
    float m_fFaceExpectYaw;
    float m_fPreviousFaceYaw;

    float m_fFacePitch;

    float m_fFootYaw;
    float m_fFootExpectYaw;
    float m_fPreviousFootYaw;

    float m_fFootPitch;
    float m_fFootExpectPitch;

    float m_fFootRoll;
    float m_fFootExpectRoll;

    BOOL m_bSheath;
    RL_ANIMATION_UP m_nAnimationUp;
};

#endif // CASE_ACTION_OBJECT_KRLLOCALCHARACTER_H
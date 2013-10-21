#ifndef CASE_ACTION_OBJECT_KRLREMOTECHARACTER_H
#define CASE_ACTION_OBJECT_KRLREMOTECHARACTER_H

#include "./krlcharacter.h"
#include "./krlcharacterturning.h"
#include "../../common/kmathtools.h"

// controlled by gameworld

class KRLRemoteCharacter
{
public:
    void* operator new(size_t nSize, std::nothrow_t const&) throw();
    void operator delete(void* pData, std::nothrow_t const&) throw();

    void* operator new(size_t nSize);
    void operator delete(void* pData) throw();

    void Init();

    int Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

    int UpdateDisplayData(BOOL bForceUpdate);
    int UpdateSkillBuff();
    int UpdateDirection(BOOL bForceUpdate);
    int UpdatePosition();

    int PrepareCastSkill(DWORD dwSkillID, DWORD dwSkillLevel);
    int CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwBulletID);
    
	int UpdateYaw(RL_ADVANCE *pnAdvance);

    int IsFinished() const;

private:
    int UpdateFaceFootDirection(RL_TURNING nTurning, BOOL bForceUpdate);
    
    int SetFaceFootDirection(float fFaceYaw, float fFacePitch, float fFootYaw, BOOL bForceUpdate);

    void UpdateLifeState();
    void UpdateAnimationSpeed(RL_ANIMATION_DOWN nAnimationDown, RL_ADVANCE nAdvance);
    RL_ADVANCE GetAdvance();

    int GetSlopeRotation(D3DXQUATERNION& qRotationLH, RL_TERRAIN_SLOPE nTerrainSlope, BOOL bForceUpdate);

public:
    KRLCharacter m_RLCharacter;

    RL_TURNING m_nTurning;

    BOOL m_bVisible;    // 3D ÊÓ×¶²Ã¼ô½á¹û

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

    double m_fExpireTime;

    BOOL m_bSheath;
    RL_ANIMATION_UP m_nAnimationUp;
};

#endif // CASE_ACTION_OBJECT_KRLREMOTECHARACTER_H
#ifndef RL_GAMEWORLD_TABLE_KRLANIMATIONTABLE_H
#define RL_GAMEWORLD_TABLE_KRLANIMATIONTABLE_H

#include "./ktabledefine.h"
#include "../render/krlresource.h"
#include "../actionobject/krlcharacterturning.h"
#include "../../Source/Common/SO3World/Src/KCharacter.h"

enum RL_ADVANCE_TYPE
{
    RL_ADVANCE_TYPE_NORMAL,
    RL_ADVANCE_TYPE_SWIM,

    RL_ADVANCE_TYPE_COUNT,
};

DWORD GetBuffAdjustAnimationIndexID(DWORD dwAnimationIndexID, DWORD const dwSkillBuffs[SKILL_BUFF_PART_COUNT]);

DWORD GetAdjustAnimationIndexID(DWORD dwAnimationIndexID, KRLAdjustAnimationParam const* pParam);

RL_ANIMATION_TABLE GetSwimAnimationTable(RL_ADVANCE nAdvance);
RL_ANIMATION_TABLE GetDashAnimationTable(RL_ADVANCE nAdvance);
RL_ANIMATION_TABLE GetRepulsedAnimationTable(RL_ADVANCE nAdvance);
RL_ANIMATION_TABLE GetSkidAnimationTable(RL_ADVANCE nAdvance);
RL_ANIMATION_TABLE GetDashIdleAnimationTable(RL_ADVANCE nAdvance);
RL_ANIMATION_TABLE GetIdleAnimationTable(RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag, BOOL bSheathFlag, BOOL bSlip);
RL_ANIMATION_TABLE GetWalkAnimationTable(RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag);
RL_ANIMATION_TABLE GetRunAnimationTable(RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag);
RL_ANIMATION_TABLE GetSheathAnimationTable(RL_SHEATH nSheathType, BOOL bSheathFlag, int nPhase);

RL_ANIMATION_TABLE GetAnimationTable(KRLAnimationTableParam const& Param);

RL_ANIMATION_TABLE GetKnowDownIdleBeHitAnimationTable(RL_ANIMATION_TABLE nAnimationTable);

RL_ANIMATION_TABLE GetStateInAnimationTable(RL_ANIMATION_DOWN nCurrentAnimationDown, RL_ANIMATION_DOWN nPreviousAnimationDown, RL_ADVANCE nAdvance);
RL_ANIMATION_TABLE GetStateOutAnimationTable(RL_ANIMATION_DOWN nCurrentAnimationDown, RL_ANIMATION_DOWN nNextAnimationDown, RL_ADVANCE nAdvance);

float GetAnimationSpeed(CHARACTER_MOVE_STATE nMoveState, float fModelScale, int nVelocityXY, BOOL bOnRide, RL_ANIMATION_DOWN nAnimationDown, RL_ADVANCE nAdvance);

float GetYawTurnSpeed(BOOL bRide);
float GetYawTurnResetSpeed(BOOL bRide);

float GetPitchStepSpeed(CHARACTER_MOVE_STATE nMoveState);
float GetRollStepSpeed(CHARACTER_MOVE_STATE nMoveState);

LPCTSTR GetAnimationDownName(RL_ANIMATION_DOWN nAnimationDown);
LPCTSTR GetAnimationUpName(RL_ANIMATION_UP nAnimationUp);

RL_ADVANCE GetAdvance(RL_ADVANCE_TYPE nAdvanceType, float fFootFaceDeltaYaw);

DWORD GetAnimationTrackID(enuAnimationJointType nJointType, DWORD dwAnimationKindID);

RL_ANIMATION_DOWN GetRLAnimationDown(CHARACTER_MOVE_STATE nMoveState, KOT_ACTION_TYPE nOTActionType, int nJumpCount, RL_TURNING nTurning);
RL_ANIMATION_UP GetRLAnimationUp(KOT_ACTION_TYPE nOTActionType);

#endif // RL_GAMEWORLD_TABLE_KRLANIMATIONTABLE_H

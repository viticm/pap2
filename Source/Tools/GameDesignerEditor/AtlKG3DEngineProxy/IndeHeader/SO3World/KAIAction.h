////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAIAction.h
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : 定义所有的AI Action
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KAIACTION_H_
#define _KAIACTION_H_

#include "Luna.h"

#define KAI_ACTION_PARAM_NUM    5
#define KAI_ACTION_BRANCH_NUM   3

class KCharacter;

class KAIAction 
{
public:
    KAIAction();

    int         m_nKey;

    int         m_nParam[KAI_ACTION_PARAM_NUM];
    int         m_nBranch[KAI_ACTION_BRANCH_NUM];

public:
    DECLARE_LUA_CLASS(KAIAction);
    int LuaSetParam(Lua_State* L);
    int LuaGetParam(Lua_State* L);

    int LuaSetBranch(Lua_State* L);
};


enum KAI_ACTION_KEY
{
    eakInvalid = 0,

    eakSetState,
    eakSetPrimaryTimer,    
    eakSetSecondaryTimer,
    eakRandomBiBranch,
    eakRandomTriBranch,
    eakStand,

    eakSearchEnemy,
    eakAddTargetToThreatList,
    eakSelectFirstThreat,
    eakIsInFight,
    eakKeepRange,
    eakCastSkill,
    eakSetSystemShield,
    eakRecordReturnPosition,
    eakReturn,
    eakEnterPatrolPath,
    eakLeavePatrolPath,
    eakPatrol,
    eakRecordOriginPosition,
    eakKeepOriginDirection,
    eakFaceTarget,
    eakWander,
    eakSearchPlayer,
    eakSelectEventSrc,
    eakNpcOpenDialogWindow,
    eakNpcSayToTarget,
    eakCheckLife,
    eakNpcInitSkillCD,
    eakNpcStandardSkillSelector,
    eakNpcKeepSkillCastRange,
    eakNpcCastSelectSkill,
    eakDoAnimation,
    eakGroupRevive,
    eakIsTargetInRange,
    eakCleanBuffByFunction,
    eakNeedEscape,
    eakParamCompare,
    eakSelectState,
    eakRandomRun,
    eakCleanDebuff,
    eakSearchMultiPlayerAddThreatList,
    eakCheckSkillFailedCounter,
    eakNpcSetSecTimerBySkillAni,
    eakNpcSetSecTimerBySkillRest,
    eakIsTargetExist,
    eakIsTargeInWater,
    eakNpcClearSelectSkillCD,
    eakNpcResetSelectSkillCD,
    eakIsNeedKeepRange,
    eakSetTertiaryTimer,
    eakSearchPlayerWithoutAnyAffect,
    eakClearThreatForTarget,

    eakTotal
};

// 用户自定义 ai action key 从 KAI_USER_ACTION 开始
#define KAI_USER_ACTION 1000

#define KAI_ACTION_BRANCH_ERROR      (-1)
#define KAI_ACTION_BRANCH_NONE        0

typedef int (*KAI_ACTION_FUNC)(KCharacter* pCharacter, KAIAction* pActionData);

#endif  // _KAIACTION_H_

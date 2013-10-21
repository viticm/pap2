#ifndef CASE_ACTIONOBJECT_KRLSKILLSEQUENCE_H
#define CASE_ACTIONOBJECT_KRLSKILLSEQUENCE_H

#include "../gameworld/ktabledefine.h"

struct KRLSkillSequnceParam
{
    DWORD dwAnimationIDs[RL_SKILL_SEQUENCE_COUNT];
    DWORD dwSkillID;
    DWORD dwSkillLevel;
};

struct KRLSkillSequence
{
    int   nLastSkillAnimationIndex;
    DWORD dwCastSkillResetPeriod; // 技能动作序列重置时间
    DWORD dwLastSkillID;
    DWORD dwLastSkillLevel;
    DWORD dwLastCastSkillTime;
};

int InitSkillSequence(KRLSkillSequence* pRLSkillSequence);
int GetSkillNextAnimationID(KRLSkillSequence* pRLSkillSequence, KRLSkillSequnceParam const* pParam, PDWORD pdwAnimationID);

#endif // CASE_ACTIONOBJECT_KRLSKILLSEQUENCE_H


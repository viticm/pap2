// 技能选择策略

class KNpc;
class KSkill;

// 所有类型的技能选择函数的返回值都必须是下面三种
#define SKILL_SELECTOR_IGNORE       0   // 不释放
#define SKILL_SELECTOR_CAST         1   // 释放
#define SKILL_SELECTOR_CAST_SPECIAL 2   // 释放,特殊目标(非第一仇恨)
#define SKILL_SELECTOR_READY        3   // 最后根据概率决定 

int StandardTargetPeriodEvaluator(KNpc* pNpc, KTarget* pTarget);
int StandardTargetPeriodRangeEvaluator(KNpc* pNpc, KTarget* pTarget);
int StandardSelfPeriodEvaluator(KNpc* pNpc, KTarget* pTarget);
int StandardPassiveRespond(KNpc* pNpc, KTarget* pTarget);
int StandardSelfHPEvaluator(KNpc* pNpc, KTarget* pTarget, int nLifeRate);
int StandardTargetCountEvaluator(KNpc* pNpc, KTarget* pTarget, int nSkillRate);
int StandardChannelingBreak(KNpc* pNpc, KTarget* pTarget, int nSkillRate);
int StandardNThreat(KNpc* pNpc, KTarget* pTarget, THREAT_TYPE eThreatType, int nThreatIndex);
int StandardRandomTargetByBase(KNpc* pNpc, KTarget* pTarget, BOOL bExceptSelectTarget);
int StandardRandomTargetByRange(KNpc* pNpc, KTarget* pTarget, int nMinRange, int nMaxRange, BOOL bExceptSelectTarget);
int StandardRandomTargetByNearest(KNpc* pNpc, KTarget* pTarget, BOOL bExceptSelectTarget);

// 以下为各种随机选择目标函数(临时数据TargetGroup)
BOOL FilterTargetGroupByBase(KCharacter* pSelf, BOOL bExceptSelectTarget);
BOOL FilterTargetGroupByRange(KCharacter* pSelf, BOOL bExceptSelectTarget, int nMinRange, int nMaxRange);
BOOL FilterTargetGroupByNearest(KCharacter* pSelf, BOOL bExceptSelectTarget);

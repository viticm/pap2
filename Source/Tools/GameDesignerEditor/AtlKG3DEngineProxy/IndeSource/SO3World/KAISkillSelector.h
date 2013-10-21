// 技能选择策略

class KNpc;
class KSkill;

// 所有类型的技能选择函数的返回值都必须是下面三种
#define SKILL_SELECTOR_IGNORE       0   // 不释放
#define SKILL_SELECTOR_CAST         1   // 释放
#define SKILL_SELECTOR_READY        2   // 最后根据概率决定 

int StandardTargetPeriodEvaluator(KNpc* pNpc, KTarget* pTarget);
int StandardTargetPeriodRangeEvaluator(KNpc* pNpc, KTarget* pTarget);
int StandardSelfPeriodEvaluator(KNpc* pNpc, KTarget* pTarget);
int StandardPassiveRespond(KNpc* pNpc, KTarget* pTarget);
int StandardSelfHPEvaluator(KNpc* pNpc, KTarget* pTarget, int nLifeRate);
int StandardTargetCountEvaluator(KNpc* pNpc, KTarget* pTarget, int nSkillRate);
int StandardChannelingBreak(KNpc* pNpc, KTarget* pTarget, int nSkillRate);

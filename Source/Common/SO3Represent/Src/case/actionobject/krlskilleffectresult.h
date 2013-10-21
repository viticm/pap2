#ifndef CASE_ACTIONOBJECT_KRLSKILLEFFECTRESULT_H
#define CASE_ACTIONOBJECT_KRLSKILLEFFECTRESULT_H

#include "SO3Represent/SO3GameWorldRepresentHandler.h"

enum { RL_STUB_TARGET_ID = 0xFFFFFFFF }; 			//存根，还没收到伤害包，数据还没填充，但动作标签已到的情况

class KTargetData;
class KSkill;
class KRLCharacter;

class KRLSkillEffectResult
{
public:
	class KSkillEffectResultData
	{
	public:
		KSkillEffectResultData();

		void InvokeResult(DWORD dwCasterID, DWORD dwTargetID, float fPercent);
		BOOL IsFinished(double fTime) const;

		double m_fTime;
        float m_fPercent;
		int m_aValue[serTotal];
 
		KRepresentSkillEffectResult m_Result;
	};

    typedef std::multimap<unsigned long long, KSkillEffectResultData> KSkillEffectResultDataMap;
	typedef std::pair<KSkillEffectResultDataMap::iterator, KSkillEffectResultDataMap::iterator> KSkillEffectResultDataMapEqualRangeResult;

public:
    KRLSkillEffectResult();
    ~KRLSkillEffectResult();

    BOOL Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

    HRESULT Append(DWORD dwCasterID, DWORD dwTargetID, KRepresentSkillEffectResult const* pResult);

	void InvokeResult(DWORD dwCasterID, DWORD dwBulletID, float fPercent);
	void InvokeResult(DWORD dwCasterID, DWORD dwTargetID, DWORD dwBulletID, float fPercent);

	static BOOL GetTargetID(KTargetData const& target, DWORD& dwTargetID);

private:
    KSkillEffectResultDataMap m_aData;
};

int RegisterSkillEffectResult(KRLCharacter* pRLTarget, KRepresentSkillEffectResult const* pResult);
int PlaySkillEffectResult(KRLCharacter* pRLTarget, KRepresentSkillEffectResult* pResult);

#endif // CASE_ACTIONOBJECT_KRLSKILLEFFECTRESULT_H
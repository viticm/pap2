#ifndef CASE_ACTION_OBJECT_KRLMISSILEMGR_H
#define CASE_ACTION_OBJECT_KRLMISSILEMGR_H

#include "./krlmissile.h"
#include "../gameworld/ktabledefine.h"
#include "../../common/ksmallobjectpool.h"

class KRLCharacter;
class KRLMissile;
class KRepresentMissile;

typedef std::list<KRLMissile*> KRLMissilePtrList;

typedef std::multimap<unsigned long long, KRLMissileParam> KRLMissileParamMap;
typedef std::pair<KRLMissileParamMap::iterator, KRLMissileParamMap::iterator> KRLMissileParamMapEqualRangeResult;

class KRLMissileMgr
{
public:
	KRLMissileMgr();
	~KRLMissileMgr();

	int Init();
	void Exit();
	int Reset();
	int Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

    int CreateRLMissile(KRLMissile** ppMissile);
    int DuplicateRLMissile(KRLMissile* pMissile, KRLMissile** ppNewMissile);
	int DestroyRLMissile(KRLMissile* pMissile);

    int UpdateResult(DWORD dwBulletID, KRepresentSkillEffectResult const* pResult);

    int InsertRLMissileParam(DWORD dwBulletID, DWORD dwTargetID, KRLMissileParam& Param);
    int RemoveRLMissileParam(DWORD dwBulletID, DWORD dwTargetID);
    KRLMissileParam* LookupRLMissileParam(DWORD dwBulletID, DWORD dwTargetID);

public:
    KRLMissileParamMap m_RLMissileParams;

private:
	KRLMissilePtrList m_pRLMissiles;

    KSmallObjectPool m_MissileAllocator;
};

KRLCharacter* GetCasterRLCharacter(KCaster const& Caster);

int PrepareAnimationTagMissileParam(KRLCharacter* pRLCaster, DWORD dwBulletID, DWORD dwMissileID, KRLMissileTagParam const* pTagParam);
int PrepareEffectResultMissileParam(KRLCharacter* pRLCharacter, KRepresentSkillEffectResult const* pResult);

int FireMissile(KRLCharacter* pRLCaster, KRLMissileParam* pRLMissileParam);
int HitTarget(KRLMissileParam* pRLMissileParam);

#endif // CASE_ACTION_OBJECT_KRLMISSILEMGR_H
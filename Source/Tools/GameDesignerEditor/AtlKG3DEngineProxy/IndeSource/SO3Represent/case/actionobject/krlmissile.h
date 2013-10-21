#ifndef CASE_ACTION_OBJECT_KRLMISSILE_H
#define CASE_ACTION_OBJECT_KRLMISSILE_H

#include "../gameworld/ktablelist.h"
#include "../../base/missile/krepresentmissile.h"
#include "../../base/missile/krepresentmissilephase.h"

class KRLScene;
class KRepresentMissile;
class KBaseMissileProcessor;
class KGameWorldMissilePhaseBaseModel;

struct KRLMissileTagParam
{
    D3DXVECTOR3	vInitialWorldPosition;
    D3DXQUATERNION qInitialWorldRotation;

    float fPercent;
};

struct KRLMissileRTParam
{
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vVelocity;

    D3DXQUATERNION qRotation;

    float fScale;
};

struct KRLMissileParam
{
    double fDeadTime;

    DWORD dwMissileID;

    KRepresentSkillEffectResult Result;

    KRLMissileTagParam TagParam;

    KRLMissileRTParam RTParam;
};

class KRLMissile
{
public:
    int Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);
	int IsFinished() const;

	int Init(KRLScene* pRLScene, KRLMissileParam const* pParam);
	int UnInit();
    int Start();

    int UpdateResult();
    int HitTarget();

public:
    KRLMissileParam m_Param;

	KRLScene* m_pRLScene;

	KRepresentMissile m_RepresentMissile;
};

#endif // CASE_ACTION_OBJECT_KRLMISSILE_H
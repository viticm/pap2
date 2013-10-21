#ifndef CASE_ACTION_OBJECT_MISSILE_K_HOMING_MISSILE_PROCESSOR_H
#define CASE_ACTION_OBJECT_MISSILE_K_HOMING_MISSILE_PROCESSOR_H

#include "../../gameworld/ktabledefine.h"

class KCompositeMissileProcessor;

class KHomingMissileProcessor
{
public:
	KHomingMissileProcessor();

	int Init(DWORD dwMissileParamID, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int UnInit(KCompositeMissileProcessor* pCompositeMissileProcessor);
	int Update(double fTime, double fTimeLast, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int IsFinished() const;
	int Start(double fTime, KCompositeMissileProcessor* pCompositeMissileProcessor);

public:
    KRLMissileHomingParam m_Param;

private:
    int m_nFinished;
};

#endif // CASE_ACTION_OBJECT_MISSILE_K_HOMING_MISSILE_PROCESSOR_H
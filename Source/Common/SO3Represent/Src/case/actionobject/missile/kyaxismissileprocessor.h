#ifndef CASE_ACTIONOBJECT_MISSILE_KYAXISMISSILEPROCESSOR_H
#define CASE_ACTIONOBJECT_MISSILE_KYAXISMISSILEPROCESSOR_H

#include "../../gameworld/ktabledefine.h"

class KCompositeMissileProcessor;

class KYAxisMissileProcessor
{
public:
	KYAxisMissileProcessor();

	int Init(DWORD dwMissileParamID, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int UnInit(KCompositeMissileProcessor* pCompositeMissileProcessor);
	int Update(double fTime, double fTimeLast, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int IsFinished() const;
	int Start(double fTime, KCompositeMissileProcessor* pCompositeMissileProcessor);

private:
    KRLMissileYAxisParam m_Param;
};


#endif // CASE_ACTIONOBJECT_MISSILE_KYAXISMISSILEPROCESSOR_H
#ifndef CASE_ACTIONOBJECT_MISSILE_KTIMELINEMISSILEPROCESSOR_H
#define CASE_ACTIONOBJECT_MISSILE_KTIMELINEMISSILEPROCESSOR_H

#include "../../gameworld/ktabledefine.h"

class KCompositeMissileProcessor;

class KTimelineMissileProcessor
{
public:
	KTimelineMissileProcessor();

	int Init(DWORD dwMissileParamID, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int UnInit(KCompositeMissileProcessor* pCompositeMissileProcessor);
	int Update(double fTime, double fTimeLast, KCompositeMissileProcessor* pCompositeMissileProcessor);
	int IsFinished() const;
	int Start(double fTime, KCompositeMissileProcessor* pCompositeMissileProcessor);

public:
    KRLMissileTimelineParam m_Param;

    double m_fExpireTime;
};


#endif // CASE_ACTIONOBJECT_MISSILE_KTIMELINEMISSILEPROCESSOR_H
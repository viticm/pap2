#ifndef K_MISSILE_REPRESENT_MISSILE_H
#define K_MISSILE_REPRESENT_MISSILE_H

#include "./krepresentmissiledefine.h"
#include "./krepresentmissilephase.h"

class KRepresentMissile
{
public:
	KRepresentMissile();
    ~KRepresentMissile();

    int SetEndPhase(RL_MISSILE_PHASE nPhase);

	int Start(double fTime);
	int Update(double fTime, double fTimeLast);
	int IsFinished() const;

public:
    KRepresentMissilePhase m_MissilePhases[RL_MISSILE_PHASE_COUNT];

private:
    RL_MISSILE_PHASE m_nPhase;
    RL_MISSILE_PHASE m_nEndPhase;
};

#endif // K_MISSILE_REPRESENT_MISSILE_H
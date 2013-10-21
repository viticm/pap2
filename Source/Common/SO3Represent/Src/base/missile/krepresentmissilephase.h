#ifndef K_MISSILE_REPRESENT_MISSILE_PHASE_H
#define K_MISSILE_REPRESENT_MISSILE_PHASE_H

class KRepresentMissileProcessor;

class KRepresentMissilePhase;

struct KRepresentMissileSerialPhase
{
	KRepresentMissileProcessor*	pProcessor;
};

typedef std::vector<KRepresentMissileSerialPhase> KRepresentMissileSerialPhaseList;

struct KRepresentMissileSimultaneousPhase
{
    KRepresentMissileSerialPhaseList SerialPhases;
    size_t nSerialPhase;
};

typedef std::vector<KRepresentMissileSimultaneousPhase> KRepresentMissileSimultaneousPhaseList;

class KRepresentMissilePhase
{
	friend class KRepresentMissile;

public:
	KRepresentMissilePhase();
	~KRepresentMissilePhase();

    int Init();
    int UnInit();

	int Start(double fTime);
	int Update(double fTime, double fTimeLast);
	int IsFinished() const;

public: // TODO: 
    KRepresentMissileSimultaneousPhaseList m_SimultaneousPhases;
};

#endif // K_MISSILE_REPRESENT_MISSILE_PHASE_H
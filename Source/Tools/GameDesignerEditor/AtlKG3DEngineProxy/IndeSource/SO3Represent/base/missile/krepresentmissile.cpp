#include "stdafx.h"
#include "./krepresentmissile.h"
#include "./krepresentmissilephase.h"

KRepresentMissile::KRepresentMissile()
: m_nPhase(RL_MISSILE_PHASE_INFLIGHT)
, m_nEndPhase(RL_MISSILE_PHASE_END_HIT)
{
}

KRepresentMissile::~KRepresentMissile()
{
}

int KRepresentMissile::SetEndPhase(RL_MISSILE_PHASE nPhase)
{
    int nResult = false;

    KG_ASSERT_EXIT(nPhase == RL_MISSILE_PHASE_END_HIT || nPhase == RL_MISSILE_PHASE_END_MISS);

    m_nEndPhase = nPhase;

    nResult = true;
Exit0:
    return nResult;
}

int KRepresentMissile::Start(double fTime)
{
    int nRetCode = false;
    int nResult = false;

	nRetCode = m_MissilePhases[m_nPhase].Start(fTime);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRepresentMissile::Update(double fTime, double fTimeLast)
{
	int nRetCode = false;
    int nResult = false;

    if (!m_MissilePhases[m_nPhase].IsFinished())
    {
        nRetCode = m_MissilePhases[m_nPhase].Update(fTime, fTimeLast);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (m_MissilePhases[m_nPhase].IsFinished())
        {
            if (m_nPhase == RL_MISSILE_PHASE_INFLIGHT)
            {
                m_nPhase = m_nEndPhase;

                nRetCode = m_MissilePhases[m_nPhase].Start(fTime);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
        }
    }

    nResult = true;
Exit0:
	return nResult;
}

int KRepresentMissile::IsFinished() const
{
    if (m_nPhase != m_nEndPhase)
        return false;

    ASSERT(m_nEndPhase >= 0 && m_nEndPhase < RL_MISSILE_PHASE_COUNT);

    if (!m_MissilePhases[m_nEndPhase].IsFinished())
        return false;

    return true;
}


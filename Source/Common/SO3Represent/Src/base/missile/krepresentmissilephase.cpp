#include "stdafx.h"
#include "./krepresentmissilephase.h"
#include "./krepresentmissileprocessor.h"

KRepresentMissilePhase::KRepresentMissilePhase()
{
}

KRepresentMissilePhase::~KRepresentMissilePhase()
{
}

int KRepresentMissilePhase::Init()
{
    return true;
}

int KRepresentMissilePhase::UnInit()
{
    int nRetCode = false;
    int nResult = false;

    for (KRepresentMissileSimultaneousPhaseList::iterator itSimultaneous = m_SimultaneousPhases.begin(); 
        itSimultaneous != m_SimultaneousPhases.end(); 
        ++itSimultaneous)
    {
        KRepresentMissileSimultaneousPhase& SimultaneousPhase = *itSimultaneous;
        KRepresentMissileSerialPhaseList& SerialPhases = SimultaneousPhase.SerialPhases;

        for (KRepresentMissileSerialPhaseList::iterator itSerial = SerialPhases.begin();
            itSerial != SerialPhases.end();
            ++itSerial)
        {
            KRepresentMissileSerialPhase& SerialPhase = *itSerial;

            KRepresentMissileProcessor* pProcessor = SerialPhase.pProcessor;

            nRetCode = pProcessor->UnInit();
            KGLOG_PROCESS_ERROR(nRetCode);

            SAFE_RELEASE(pProcessor);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRepresentMissilePhase::Start(double fTime)
{
    int nRetCode = false;
    int nResult = false;

    for (KRepresentMissileSimultaneousPhaseList::iterator itSimultaneous = m_SimultaneousPhases.begin(); 
        itSimultaneous != m_SimultaneousPhases.end(); 
        ++itSimultaneous)
    {
        KRepresentMissileSimultaneousPhase& SimultaneousPhase = *itSimultaneous;

        SimultaneousPhase.nSerialPhase = 0;

        KRepresentMissileSerialPhase& SerialPhase = SimultaneousPhase.SerialPhases[SimultaneousPhase.nSerialPhase];

        KRepresentMissileProcessor* pProcessor = SerialPhase.pProcessor;

        nRetCode = pProcessor->Start(fTime);
        KG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRepresentMissilePhase::Update(double fTime, double fTimeLast)
{
	int nRetCode = false;
    int nResult = false;
    size_t nFinishedCount = 0;

    for (KRepresentMissileSimultaneousPhaseList::iterator itSimultaneous = m_SimultaneousPhases.begin(); 
        itSimultaneous != m_SimultaneousPhases.end(); 
        ++itSimultaneous)
    {
        KRepresentMissileSimultaneousPhase& SimultaneousPhase = *itSimultaneous;

        if (SimultaneousPhase.nSerialPhase == SimultaneousPhase.SerialPhases.size())
            continue;

        KRepresentMissileSerialPhase& SerialPhase = SimultaneousPhase.SerialPhases[SimultaneousPhase.nSerialPhase];

        KRepresentMissileProcessor* pProcessor = SerialPhase.pProcessor;

        nRetCode = pProcessor->Update(fTime, fTimeLast);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (pProcessor->IsFinished())
        {
            ++SimultaneousPhase.nSerialPhase;

            nRetCode = pProcessor->Stop(fTime);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRepresentMissilePhase::IsFinished() const
{
    for (KRepresentMissileSimultaneousPhaseList::const_iterator itSimultaneous = m_SimultaneousPhases.begin(); 
        itSimultaneous != m_SimultaneousPhases.end(); 
        ++itSimultaneous)
    {
        KRepresentMissileSimultaneousPhase const& SimultaneousPhase = *itSimultaneous;

        if (SimultaneousPhase.nSerialPhase != SimultaneousPhase.SerialPhases.size())
        {
            return false;
        }
    }

    return true;
}




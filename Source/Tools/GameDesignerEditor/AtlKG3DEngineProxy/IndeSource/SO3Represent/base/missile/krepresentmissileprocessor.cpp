#include "stdafx.h"
#include "./krepresentmissileprocessor.h"

KRepresentMissileProcessor::~KRepresentMissileProcessor()
{
}

ULONG KRepresentMissileProcessor::Release()
{
	delete this;

	return 0;
}

int KRepresentMissileProcessor::Start(double fTime)
{
	return OnStart(fTime);
}

int KRepresentMissileProcessor::Stop(double fTime)
{
	return OnStop(fTime);
}

int KRepresentMissileProcessor::Init()
{
	return OnInit();
}

int KRepresentMissileProcessor::UnInit()
{
    return OnUnInit();
}

int KRepresentMissileProcessor::Update(double fTime, double fTimeLast)
{
	return OnUpdate(fTime, fTimeLast);
}

int KRepresentMissileProcessor::IsFinished() const
{
	return OnIsFinished();
}

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPerformerce.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-16 10:23:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGPerformence.h"


////////////////////////////////////////////////////////////////////////////////

KGPerformerceSystemInfo::KGPerformerceSystemInfo()
{
	LARGE_INTEGER ifre = {0, 0};
	if(QueryFreq(ifre))
	{
		frequency = static_cast<double>(ifre.QuadPart)/1000.f;
	}
	else
	{
		frequency = 1;
	}
}

BOOL KGPerformerceSystemInfo::QueryCounter( LARGE_INTEGER& counter )
{
	return QueryPerformanceCounter(&counter);
}

BOOL KGPerformerceSystemInfo::QueryFreq( LARGE_INTEGER& freq )
{
	return QueryPerformanceFrequency(&freq);
}

enum{
	em_proper_sample_times = 1000,
};

KGPerformercenRecorder::KGPerformercenRecorder()
:m_bIsEnable(true)
,m_bStarted(false)
{
	m_startCounter.QuadPart = 0;
}
KGPerformercenRecorder::~KGPerformercenRecorder()
{
	if (m_bStarted)
	{
		this->Stop();
	}
}
VOID KGPerformercenRecorder::Sample()
{
	if(! m_bIsEnable)
		return;

	LARGE_INTEGER temp;
	
	m_systemInfo.QueryCounter(temp);
	m_samplers.push_back(temp);
}	

VOID KGPerformercenRecorder::PrintToOut()
{
	if(! m_bIsEnable)
		return;

#if defined(UNICODE) | defined(_UNICODE)
	_RPTW0(_CRT_WARN, _T("Perf:\n"));
#else
	_RPT0(_CRT_WARN, _T("Perf:\n"));
#endif

	double timeUsed = 0;

	for (size_t i = 0; i < m_samplers.size(); ++i)
	{
		timeUsed = static_cast<double>(m_samplers[i].QuadPart - m_startCounter.QuadPart) / m_systemInfo.frequency;
#if defined(UNICODE) | defined(_UNICODE)
		_RPTW1(_CRT_WARN, _T("%f,"), timeUsed);
#else
		_RPT1(_CRT_WARN, _T("%f,"), timeUsed);
#endif
	}

#if defined(UNICODE) | defined(_UNICODE)
	_RPTW0(_CRT_WARN, _T("\n"));
#else
	_RPT0(_CRT_WARN, _T("\n"));
#endif
}
static size_t s_lastSamplerTimes = em_proper_sample_times;
VOID KGPerformercenRecorder::Start()
{
	_ASSERTE(! m_bStarted);

	if(! m_bIsEnable)
		return;

	m_samplers.resize(0);
	m_samplers.reserve(s_lastSamplerTimes);
	m_systemInfo.QueryCounter(m_startCounter);

	m_bStarted = true;
}

VOID KGPerformercenRecorder::Stop()
{
	if(! m_bIsEnable)
		return;

	_ASSERTE(m_bStarted);

	size_t curSize = m_samplers.size();
	s_lastSamplerTimes = min(curSize, s_lastSamplerTimes);
	s_lastSamplerTimes = min(s_lastSamplerTimes, (size_t)em_proper_sample_times);

	m_bStarted = false;
}

VOID KGPerformercenRecorder::Enable(BOOL enable)
{
	m_bIsEnable = enable ? true : false;
}


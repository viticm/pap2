#include "stdafx.h"
#include "./ktimelinemissileprocessor.h"
#include "./kcompositemissileprocessor.h"
#include "../../../SO3Represent.h"

KTimelineMissileProcessor::KTimelineMissileProcessor()
: m_fExpireTime(0.0)
{
}

int KTimelineMissileProcessor::Init(DWORD dwMissileParamID, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    int nRetCode = false;
    int nResult = false;
    KRLMissileTimelineParam const* pParam = NULL;
    
    pParam = g_pRL->m_TableList.GetMissileTimelineParam(dwMissileParamID);
    KGLOG_PROCESS_ERROR(pParam);

    memcpy(&m_Param, pParam, sizeof(m_Param));

    nResult = true;
Exit0:
    if (!nResult)
    {
        memset(&m_Param, 0, sizeof(m_Param));
    }
    return nResult;
}

int KTimelineMissileProcessor::UnInit(KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    return true;
}

int KTimelineMissileProcessor::Update(double fTime, double fTimeLast, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    if (m_fExpireTime > 0.0f && fTime > m_fExpireTime)
    {
        m_fExpireTime = -1.0;
    }

	return true;
}

int KTimelineMissileProcessor::IsFinished() const
{
	return m_fExpireTime < 0.0;
}

int KTimelineMissileProcessor::Start(double fTime, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    m_fExpireTime = fTime + m_Param.fMillisecond;

	return true;
}


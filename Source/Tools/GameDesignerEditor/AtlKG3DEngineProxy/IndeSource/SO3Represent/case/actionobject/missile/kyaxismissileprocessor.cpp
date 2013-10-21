#include "stdafx.h"
#include "./kyaxismissileprocessor.h"
#include "./kcompositemissileprocessor.h"
#include "../krlmissile.h"
#include "../krlscene.h"
#include "../../../SO3Represent.h"

KYAxisMissileProcessor::KYAxisMissileProcessor()
{
}

int KYAxisMissileProcessor::Init(DWORD dwMissileParamID, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    int nRetCode = false;
    int nResult = false;
    KRLMissileYAxisParam const* pParam = NULL;
    
    pParam = g_pRL->m_TableList.GetMissileYAxisParam(dwMissileParamID);
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

int KYAxisMissileProcessor::UnInit(KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    return true;
}

int KYAxisMissileProcessor::Update(double fTime, double fTimeLast, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLMissile* pRLMissile = NULL;
    KRLMissileRTParam* pRTParam = NULL;

    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);

    pRLMissile = pCompositeMissileProcessor->m_pRLMissile;
    KGLOG_PROCESS_ERROR(pRLMissile);

    pRTParam = &pRLMissile->m_Param.RTParam;
	
	hr = pRLMissile->m_pRLScene->m_p3DScene->GetFloor(&pRTParam->vPosition, &pRTParam->vPosition.y);
    KGLOG_COM_PROCESS_ERROR(hr);

    pRTParam->vPosition.y += m_Param.fY;

    nResult = true;
Exit0:
    return nResult;
}

int KYAxisMissileProcessor::IsFinished() const
{
    return true;
}

int KYAxisMissileProcessor::Start(double fTime, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	return true;
}


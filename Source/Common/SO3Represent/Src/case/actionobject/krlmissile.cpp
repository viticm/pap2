#include "stdafx.h"
#include "./krlmissile.h"
#include "./krlscene.h"
#include "./missile/kcompositemissileprocessor.h"
#include "../gameworld/ktabletransform.h"
#include "../../common/ksmallobjectpool.h"
#include "../../SO3Represent.h"

namespace
{
    int InitMissilePhase(KRepresentMissilePhase& Phase, KRLMissileProcessorParam const& Param, KRLMissile* pRLMissile)
    {
        int nRetCode = false;
        int nResult = false;
        KCompositeMissileProcessor* pCompositeMissileProcessor = NULL;
        int nInitProcessor = false;

        Phase.m_SimultaneousPhases.resize(1);
        Phase.m_SimultaneousPhases[0].SerialPhases.resize(1);
        Phase.m_SimultaneousPhases[0].nSerialPhase = 0;

        pCompositeMissileProcessor = new(std::nothrow) KCompositeMissileProcessor;
        KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);

        nRetCode = pCompositeMissileProcessor->SetParam(Param);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pCompositeMissileProcessor->SetMissile(pRLMissile);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pCompositeMissileProcessor->Init();
        KGLOG_PROCESS_ERROR(nRetCode);

        nInitProcessor = true;

        Phase.m_SimultaneousPhases[0].SerialPhases[0].pProcessor = pCompositeMissileProcessor;

        nResult = true;
Exit0:
        if (!nResult)
        {
            if (nInitProcessor)
            {
                pCompositeMissileProcessor->UnInit();
            }

            SAFE_RELEASE(pCompositeMissileProcessor);
            Phase.m_SimultaneousPhases.clear();
        }
        return nResult;
    }

}

int KRLMissile::Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = m_RepresentMissile.Update(fTime, fTimeLast);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLMissile::IsFinished() const
{
    return m_RepresentMissile.IsFinished();
}

int KRLMissile::Init(KRLScene* pRLScene, KRLMissileParam const* pParam)
{
    int nRetCode = false;
    int nResult = false;
    KRLMissileModel const* pMissileModel = NULL;
    int nInitMissilePhases[RL_MISSILE_PHASE_COUNT] = { 0 };

    memcpy(&m_Param, pParam, sizeof(m_Param));

    ASSERT(pRLScene);
    ASSERT(m_pRLScene == NULL);

    m_pRLScene = pRLScene;

    pMissileModel = g_pRL->m_TableList.GetMissileModel(pParam->dwMissileID);
    KGLOG_PROCESS_ERROR(pMissileModel);

    for (int nPhase = 0; nPhase < RL_MISSILE_PHASE_COUNT; ++nPhase)
    {
        nRetCode = InitMissilePhase(m_RepresentMissile.m_MissilePhases[nPhase], pMissileModel->Params[nPhase], this);
        KGLOG_PROCESS_ERROR(nRetCode);

        nInitMissilePhases[nPhase] = true;
    }

    nRetCode = UpdateResult();
    KGLOG_PROCESS_ERROR(nRetCode);

#if 0
    m_Param.RTParam.vPosition = m_Param.TagParam.vInitialWorldPosition;
    m_Param.RTParam.vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_Param.RTParam.qRotation = m_Param.TagParam.qInitialWorldRotation;
    m_Param.RTParam.fScale = 1.0f;
#endif

    nResult = true;
Exit0:
    if (!nResult)
    {
        for (int nPhase = _countof(nInitMissilePhases) - 1; nPhase >= 0; --nPhase)
        {
            if (nInitMissilePhases[nPhase])
            {
                nRetCode = m_RepresentMissile.m_MissilePhases[nPhase].UnInit();
                KGLOG_CHECK_ERROR(nRetCode);
            }
        }

        m_pRLScene = NULL;

        memset(&m_Param, 0, sizeof(m_Param));
    }
    return nResult;
}

int KRLMissile::UnInit()
{
    int nRetCode = false;
    int nResult = false;

    for (size_t nIndex = 0; nIndex < _countof(m_RepresentMissile.m_MissilePhases); ++nIndex)
    {
        nRetCode = m_RepresentMissile.m_MissilePhases[nIndex].UnInit();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLMissile::Start()
{
    int nRetCode = false;
    int nResult = false;
    double fTime = 0.0;

    KG_ASSERT_EXIT(m_pRLScene);

    nRetCode = m_RepresentMissile.Start(g_pRL->m_fTime);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLMissile::UpdateResult()
{
    int nRetCode = false;
    int nResult = false;

    if (m_Param.Result.bHitTarget)
    {
        nRetCode = m_RepresentMissile.SetEndPhase(RL_MISSILE_PHASE_END_HIT);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = m_RepresentMissile.SetEndPhase(RL_MISSILE_PHASE_END_MISS);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLMissile::HitTarget()
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = ::HitTarget(&m_Param);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}


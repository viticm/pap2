#include "stdafx.h"
#include "./kcompositemissileprocessor.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "../krlmissile.h"
#include "../krldoodad.h"
#include "../krlscene.h"
#include "../krlcharacter.h"
#include "../../render/kmodel.h"
#include "../../../common/kmathtools.h"
#include "../../../SO3Represent.h"

KCompositeMissileProcessor::KCompositeMissileProcessor()
: m_pRLMissile(NULL)
{
    memset(&m_Param, 0, sizeof(m_Param));
}

KCompositeMissileProcessor::~KCompositeMissileProcessor()
{
}

int KCompositeMissileProcessor::OnInit()
{
    int nRetCode = false;
    int nResult = false;
    int nInitModelMissileProcessor = false;
    int nInitHomingMissileProcessor = false;
    int nInitYAxisMissileProcessor = false;
    int nInitTimelineMissileProcessor = false;

    if (m_Param.dwModelParamID != 0)
    {
        nRetCode = m_ModelMissileProcessor.Init(m_Param.dwModelParamID, this);
        KGLOG_PROCESS_ERROR(nRetCode);

        nInitModelMissileProcessor = true;
    }

    if (m_Param.dwHomingParamID != 0)
    {
        nRetCode = m_HomingMissileProcessor.Init(m_Param.dwHomingParamID, this);
        KGLOG_PROCESS_ERROR(nRetCode);

        nInitHomingMissileProcessor = true;
    }

    if (m_Param.dwYAxisParamID != 0)
    {
        nRetCode = m_YAxisMissileProcessor.Init(m_Param.dwYAxisParamID, this);
        KGLOG_PROCESS_ERROR(nRetCode);

        nInitYAxisMissileProcessor = true;
    }

    if (m_Param.dwTimelineParamID != 0)
    {
        nRetCode = m_TimelineMissileProcessor.Init(m_Param.dwTimelineParamID, this);
        KGLOG_PROCESS_ERROR(nRetCode);

        nInitTimelineMissileProcessor = true;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (nInitTimelineMissileProcessor)
        {
            nRetCode = m_TimelineMissileProcessor.UnInit(this);
            KGLOG_CHECK_ERROR(nRetCode);
        }
        if (nInitYAxisMissileProcessor)
        {
            nRetCode = m_YAxisMissileProcessor.UnInit(this);
            KGLOG_CHECK_ERROR(nRetCode);
        }
        if (nInitHomingMissileProcessor)
        {
            nRetCode = m_HomingMissileProcessor.UnInit(this);
            KGLOG_CHECK_ERROR(nRetCode);
        }
        if (nInitModelMissileProcessor)
        {
            nRetCode = m_ModelMissileProcessor.UnInit(this);
            KGLOG_CHECK_ERROR(nRetCode);
        }
    }
	return nResult;
}

int KCompositeMissileProcessor::OnUnInit()
{
    int nRetCode = false;
    int nResult = false;

    if (m_Param.dwTimelineParamID != 0)
    {
        nRetCode = m_TimelineMissileProcessor.UnInit(this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwYAxisParamID != 0)
    {
        nRetCode = m_YAxisMissileProcessor.UnInit(this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwHomingParamID != 0)
    {
        nRetCode = m_HomingMissileProcessor.UnInit(this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwModelParamID != 0)
    {
        nRetCode = m_ModelMissileProcessor.UnInit(this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
	return nResult;
}

int KCompositeMissileProcessor::OnUpdate(double fTime, double fTimeLast)
{
    int nRetCode = false;
    int nResult = false;

    if (m_Param.dwHomingParamID != 0)
    {
        nRetCode = m_HomingMissileProcessor.Update(fTime, fTimeLast, this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwYAxisParamID != 0)
    {
        nRetCode = m_YAxisMissileProcessor.Update(fTime, fTimeLast, this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwTimelineParamID != 0)
    {
        nRetCode = m_TimelineMissileProcessor.Update(fTime, fTimeLast, this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwModelParamID != 0)
    {
        nRetCode = m_ModelMissileProcessor.Update(fTime, fTimeLast, this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
	return nResult;
}

int KCompositeMissileProcessor::OnIsFinished() const
{
    if (m_Param.dwTimelineParamID != 0)
    {
        if (m_TimelineMissileProcessor.IsFinished())
        {
            return true;
        }
    }

    if (m_Param.dwModelParamID != 0)
    {
        if (!m_ModelMissileProcessor.IsFinished())
        {
            return false;
        }
    }

    if (m_Param.dwHomingParamID != 0)
    {
        if (!m_HomingMissileProcessor.IsFinished())
        {
            return false;
        }
    }

    if (m_Param.dwYAxisParamID != 0)
    {
        if (!m_YAxisMissileProcessor.IsFinished())
        {
            return false;
        }
    }

    return true;
}

int KCompositeMissileProcessor::OnStop(double fTime)
{
    int nRetCode = false;
    int nResult = false;

    if (m_Param.dwModelParamID != 0)
    {
        nRetCode = m_ModelMissileProcessor.Stop(this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KCompositeMissileProcessor::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
    return m_ModelMissileProcessor.OnTagPlayingSFX(pController, pModelSFX, pExtraInfo);
}

HRESULT KCompositeMissileProcessor::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
    return m_ModelMissileProcessor.OnPlayAnimationFinish(pController);
}

HRESULT KCompositeMissileProcessor::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
    return m_ModelMissileProcessor.OnMotionNotify(pController, dwMotionInfo, pMotionCallBackExtraInfo);
}

int KCompositeMissileProcessor::OnStart(double fTime)
{
    int nRetCode = false;
    int nResult = false;

    if (m_Param.dwModelParamID != 0)
    {
        nRetCode = m_ModelMissileProcessor.Start(fTime, this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwHomingParamID != 0)
    {
        nRetCode = m_HomingMissileProcessor.Start(fTime, this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwYAxisParamID != 0)
    {
        nRetCode = m_YAxisMissileProcessor.Start(fTime, this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_Param.dwTimelineParamID != 0)
    {
        nRetCode = m_TimelineMissileProcessor.Start(fTime, this);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
	return nResult;
}

int KCompositeMissileProcessor::SetMissile(KRLMissile* pMissile)
{
    int nResult = false;

	KG_ASSERT_EXIT(m_pRLMissile == NULL);
	KG_ASSERT_EXIT(pMissile != NULL);

	m_pRLMissile = pMissile;

    nResult = true;
Exit0:
	return nResult;
}

int KCompositeMissileProcessor::SetParam(KRLMissileProcessorParam const& Param)
{
    m_Param = Param;

    return true;
}



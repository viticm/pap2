#include "stdafx.h"
#include "./kmodelmissileprocessor.h"
#include "./kcompositemissileprocessor.h"
#include "../krlmissile.h"
#include "../krlcharacter.h"
#include "../krldoodad.h"
#include "../krlscene.h"
#include "../../render/kmodel.h"
#include "../../gameworld/ktabletransform.h"
#include "../../../common/kmathtools.h"
#include "../../../SO3Represent.h"
#include "SO3World/IKGSO3WorldClient.h"

static int AppendModelSlotToTarget(KTargetData const* pTarget, LPCTSTR szSocket, IKG3DModel* p3DModel, double fExpireTime)
{
    int nRetCode = false;
    int nResult = false;
	TARGET_TYPE eTargetType = ttInvalid;
    KAnimationModel* pRenderData = NULL;

	KGLOG_PROCESS_ERROR(pTarget);
    
	eTargetType = pTarget->m_eTargetType;

	switch (eTargetType)
	{
    case ttInvalid:
    case ttNoTarget:
        KG_PROCESS_ERROR(0);
        break;

	case ttCoordination:
		break;
	case ttNpc:
    case ttPlayer:
		{
			KRLCharacter* pRLCharacter = NULL;
            KSceneObject* pSceneObject = NULL;

            nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(const_cast<KTargetData*>(pTarget), &pSceneObject);
            KGLOG_PROCESS_ERROR(nRetCode);
            KG_PROCESS_ERROR(pSceneObject);
            
			pRLCharacter = reinterpret_cast<KRLCharacter*>(pSceneObject->m_pRepresentObject);
			KGLOG_PROCESS_ERROR(pRLCharacter);

            pRenderData = &pRLCharacter->m_RenderData;
		}
		break;
	case ttDoodad:
		{
			KRLDoodad* pRLDoodad = NULL;
            KSceneObject* pSceneObject = NULL;

            nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(const_cast<KTargetData*>(pTarget), &pSceneObject);
            KGLOG_PROCESS_ERROR(nRetCode);
            KGLOG_PROCESS_ERROR(pSceneObject);
            
            pRLDoodad = reinterpret_cast<KRLDoodad*>(pSceneObject->m_pRepresentObject);
			KGLOG_PROCESS_ERROR(pRLDoodad);

            pRenderData = &pRLDoodad->m_RenderData;
		}
		break;
	default:
		ASSERT(0);
		break;
	}

    if (pRenderData)
    {
        pRenderData->AppendSlot(szSocket, p3DModel, fExpireTime);
    }

    nResult = true;
Exit0:
    return nResult;
}

KModelMissileProcessor::KModelMissileProcessor()
{
    memset(&m_Param, 0, sizeof(m_Param));
	memset(m_a3DSFXModelItem, 0, sizeof(m_a3DSFXModelItem));
}

KModelMissileProcessor::~KModelMissileProcessor()
{
}

int KModelMissileProcessor::Init(DWORD dwMissileParamID, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    int nRetCode = false;
    int nResult = false;
    KRLMissileModelParam const* pParam = NULL;
    LPCTSTR szSFXFilePaths[MISSILE_PHASE_SFX_TYPE_COUNT];
    KRLMissileRTParam* pRTParam = NULL;

    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);
    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor->m_pRLMissile);

    pRTParam = &pCompositeMissileProcessor->m_pRLMissile->m_Param.RTParam;

    pParam = g_pRL->m_TableList.GetMissileModelParam(dwMissileParamID);
    KGLOG_PROCESS_ERROR(pParam);

    memcpy(&m_Param, pParam, sizeof(m_Param));

    pRTParam->fScale = m_Param.fModelScaleBegin;

    szSFXFilePaths[MISSILE_PHASE_SFX_TYPE_MODEL] = m_Param.szMeshFile;
    szSFXFilePaths[MISSILE_PHASE_SFX_TYPE_SFX] = m_Param.szSFXFile;

    for (int nModelType = 0; nModelType < MISSILE_PHASE_SFX_TYPE_COUNT; ++nModelType)
    {
        if (szSFXFilePaths[nModelType][0] != _T('\0'))
        {
            nRetCode = AppendModel(nModelType, szSFXFilePaths[nModelType], pCompositeMissileProcessor);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        memset(&m_Param, 0, sizeof(m_Param));
    }
    return nResult;
}

int KModelMissileProcessor::UnInit(KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    int nResult = false;
	HRESULT hr = E_FAIL;
    KRLScene* pRLScene = NULL;

	KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);
    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor->m_pRLMissile);

    pRLScene = pCompositeMissileProcessor->m_pRLMissile->m_pRLScene;
    KGLOG_PROCESS_ERROR(pRLScene);

	for (int nType = 0; nType != MISSILE_PHASE_SFX_TYPE_COUNT; ++nType)
	{
		for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
		{
			K3DSFXModelItem& Item = m_a3DSFXModelItem[nType][nIndex];
			if (Item.p3DModel)
			{
                hr = KModel::RemoveRenderEntity(pRLScene->m_p3DScene, Item.p3DModel, FALSE);
                KGLOG_COM_PROCESS_ERROR(hr);

				hr = KModel::UnloadModel(pRLScene, &Item.p3DModel, pCompositeMissileProcessor);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KModelMissileProcessor::Update(double fTime, double fTimeLast, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	int nRetCode = false;
    int nResult = false;
    KRLMissileRTParam* pRTParam = NULL;

    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);
    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor->m_pRLMissile);

    pRTParam = &pCompositeMissileProcessor->m_pRLMissile->m_Param.RTParam;

    for (int nSFXModel = 0; nSFXModel < GetSFXModelCount(); ++nSFXModel)
    {
        for (int nModelType = 0; nModelType != MISSILE_PHASE_SFX_TYPE_COUNT; ++nModelType)
        {
            nRetCode = SetSFXModelParam(nModelType, nSFXModel, pRTParam->fScale, pRTParam->vPosition, pRTParam->qRotation);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
    }
	return nResult;
}

int KModelMissileProcessor::IsFinished() const
{
    if (m_Param.nAnimationType == RL_ANIMATION_PLAY_TYPE_LOOP)
        return true;

    for (int nType = 0; nType != MISSILE_PHASE_SFX_TYPE_COUNT; ++nType)
    {
        for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
        {
            if (m_a3DSFXModelItem[nType][nIndex].p3DModel && !m_a3DSFXModelItem[nType][nIndex].nFinished)
            {
                return false;
            }
        }
    }

	return true;
}

int KModelMissileProcessor::Start(double fTime, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	int nRetCode = false;
    int nResult = false;

    if (m_Param.bBindTarget)
    {
        nRetCode = BindModels(pCompositeMissileProcessor);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = PlayModelsAnimation(pCompositeMissileProcessor);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
	return nResult;
}

int KModelMissileProcessor::Stop(KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;

	for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
	{
		K3DSFXModelItem& Item = m_a3DSFXModelItem[MISSILE_PHASE_SFX_TYPE_MODEL][nIndex];
		if (Item.p3DModel)
		{
            Item.nFinished = true;

			hr = KModel::SetAlpha(Item.p3DModel, 0.0f);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}

	for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
	{
		K3DSFXModelItem& Item = m_a3DSFXModelItem[MISSILE_PHASE_SFX_TYPE_SFX][nIndex];
		if (Item.p3DModel)
		{
            Item.nFinished = true;

			hr = KModel::SetAlpha(Item.p3DModel, 0.0f);
			KGLOG_COM_PROCESS_ERROR(hr);
        }
	}
	
    nResult = true;
Exit0:
	return nResult;
}

HRESULT KModelMissileProcessor::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
	return E_NOTIMPL;
}

HRESULT KModelMissileProcessor::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	IKG3DModel* p3DModel = NULL;
    
    p3DModel = pController->GetModel();
    KGLOG_PROCESS_ERROR(p3DModel);

	for (int nType = 0; nType != MISSILE_PHASE_SFX_TYPE_COUNT; ++nType)
	{
		for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
		{
			K3DSFXModelItem& Item = m_a3DSFXModelItem[nType][nIndex];
			if (Item.p3DModel == p3DModel)
			{
                Item.nFinished = true;
                break;
			}
		}
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModelMissileProcessor::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
    return S_OK;
}

int KModelMissileProcessor::PlayModelsAnimation(KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;

	for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
	{
		K3DSFXModelItem& Item = m_a3DSFXModelItem[MISSILE_PHASE_SFX_TYPE_MODEL][nIndex];
		if (Item.p3DModel)
		{
			hr = KModel::SetAlpha(Item.p3DModel, 1.0f);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}

	for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
	{
		K3DSFXModelItem& Item = m_a3DSFXModelItem[MISSILE_PHASE_SFX_TYPE_SFX][nIndex];
		if (Item.p3DModel)
		{
            KRLMonoAnimationParam MonoAnimationParam;

            MonoAnimationParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)m_Param.nAnimationType);
            MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
            MonoAnimationParam.fSpeed = m_Param.fAnimationSpeed;
            MonoAnimationParam.nOffsetTime = 0;
            MonoAnimationParam.dwTweenTime = 0;
            MonoAnimationParam.pUserdata = NULL;
            MonoAnimationParam.szFilePath = _T("");
            MonoAnimationParam.fStartTime = g_pRL->m_fTime;

            if (m_Param.nAnimationType == RL_ANIMATION_PLAY_TYPE_ONCE)
                MonoAnimationParam.pUserdata = pCompositeMissileProcessor;

            hr = KModel::PlayMonoAnimation(Item.p3DModel, MonoAnimationParam);
			KGLOG_COM_PROCESS_ERROR(hr);

			hr = KModel::SetAlpha(Item.p3DModel, 1.0f);
			KGLOG_COM_PROCESS_ERROR(hr);
        }
	}
	
    nResult = true;
Exit0:
	return nResult;
}

int KModelMissileProcessor::BindModels(KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;
    KRLMissileRTParam* pRTParam = NULL;
    KTargetData const* pTarget = NULL;
    LPCTSTR szSocket = _T("");
    double fExpireTime = 0.0;

    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);
    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor->m_pRLMissile);

    pRTParam = &pCompositeMissileProcessor->m_pRLMissile->m_Param.RTParam;
    pTarget = &pCompositeMissileProcessor->m_pRLMissile->m_Param.Result.Target;

    if (pCompositeMissileProcessor->m_Param.dwHomingParamID != 0)
        szSocket = pCompositeMissileProcessor->m_HomingMissileProcessor.m_Param.szTargetBoneName;

    if (pCompositeMissileProcessor->m_Param.dwTimelineParamID != 0)
        fExpireTime = pCompositeMissileProcessor->m_TimelineMissileProcessor.m_fExpireTime;

	for (int nType = 0; nType != MISSILE_PHASE_SFX_TYPE_COUNT; ++nType)
	{
		for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
		{
			K3DSFXModelItem& Item = m_a3DSFXModelItem[nType][nIndex];
			if (Item.p3DModel)
			{
                IKG3DModel* p3DModel = NULL;
                
                p3DModel = Item.p3DModel;
                Item.p3DModel = NULL;

                nRetCode = AppendModelSlotToTarget(pTarget, szSocket, p3DModel, fExpireTime);
                KGLOG_PROCESS_ERROR(nRetCode);
			}
		}
	}

    nResult = true;
Exit0:
    return nResult;
}

int KModelMissileProcessor::AppendModel(int nModelType, LPCTSTR szFilePath, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;
    IKG3DModel* p3DModel = NULL;
    KRLScene* pRLScene = NULL;
	int nIndex = -1;
    int nLoadMDL = false;
    int nAppendRenderEntity = false;

	KG_ASSERT_EXIT(nModelType >= 0 && nModelType < MISSILE_PHASE_SFX_TYPE_COUNT);

	KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);
    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor->m_pRLMissile);

    pRLScene = pCompositeMissileProcessor->m_pRLMissile->m_pRLScene;
    KGLOG_PROCESS_ERROR(pRLScene);

	nRetCode = szFilePath[0] != _T('\0');
	KGLOG_PROCESS_ERROR(nRetCode);

	nIndex = FindFreeModelItemIndex(nModelType);
	KGLOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MISSILE_PHASE_SFX_COUNT);

	hr = KModel::LoadModel(pRLScene, &p3DModel, pCompositeMissileProcessor, szFilePath);
	KGLOG_COM_PROCESS_ERROR(hr);

    nLoadMDL = true;

    hr = KModel::SetPosition(p3DModel, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::AppendRenderEntity(pRLScene->m_p3DScene, p3DModel, FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    nAppendRenderEntity = true;

	hr = KModel::SetScale(p3DModel, m_Param.fModelScaleBegin);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = KModel::SetAlpha(p3DModel, 0.0f);
	KGLOG_COM_PROCESS_ERROR(hr);

    m_a3DSFXModelItem[nModelType][nIndex].nFinished = false;

    m_a3DSFXModelItem[nModelType][nIndex].p3DModel = p3DModel;
    p3DModel = NULL;

	nResult = true;
Exit0:
    if (!nResult)
    {
        if (nAppendRenderEntity)
        {
            hr = KModel::RemoveRenderEntity(pRLScene->m_p3DScene, p3DModel, FALSE);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        if (nLoadMDL)
        {
            hr = KModel::UnloadModel(pRLScene, &p3DModel, pCompositeMissileProcessor);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        KGLogPrintf(KGLOG_ERR, "KModelMissileProcessor::AppendModel(%d, %s, %x.8x)\n", nModelType, szFilePath, pCompositeMissileProcessor);
    }
	return nResult;
}

int	KModelMissileProcessor::GetSFXModelCount(int nModelType) const
{
	return FindFreeModelItemIndex(nModelType);
}

int	KModelMissileProcessor::GetSFXModelCount() const
{
	int nRetCode = false;
	int aSFXModelCount[MISSILE_PHASE_SFX_TYPE_COUNT];
	int nSFXModelCount = 0;

	for (int nModelType = 0; nModelType != MISSILE_PHASE_SFX_TYPE_COUNT; ++nModelType)
	{
		aSFXModelCount[nModelType] = GetSFXModelCount(nModelType);
		nSFXModelCount += aSFXModelCount[nModelType];
	}

	return nSFXModelCount;
}

int	KModelMissileProcessor::FindFreeModelItemIndex(int nModelType) const
{
	ASSERT(nModelType >= 0 && nModelType < MISSILE_PHASE_SFX_TYPE_COUNT);
    
	for (int nIndex = 0; nIndex != MISSILE_PHASE_SFX_COUNT; ++nIndex)
    {
		if (m_a3DSFXModelItem[nModelType][nIndex].p3DModel == NULL)
        {
			return nIndex;
        }
    }

    return MISSILE_PHASE_SFX_COUNT;
}

HRESULT KModelMissileProcessor::SetLightningSFXModel(int nModelType, int nSFXModel, float fModelScale, D3DXVECTOR3 vPositionCaster, D3DXVECTOR3 vPositionTarget)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    hr = GetSFXModel(nModelType, nSFXModel, &p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (p3DModel)
    {
        hr = p3DModel->UpdateChannelEffectPos(vPositionCaster, vPositionTarget);
        KG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModelMissileProcessor::GetSFXModel(int nModelType, int nSFXModel, IKG3DModel** pp3DModel) const
{
	IKG3DModel* p3DModel = NULL;
	
	ASSERT(nModelType >= 0 && nModelType < MISSILE_PHASE_SFX_TYPE_COUNT);
	ASSERT(nSFXModel >= 0 && nSFXModel < MISSILE_PHASE_SFX_COUNT);

	ASSERT(pp3DModel);
	ASSERT(*pp3DModel == NULL);

	*pp3DModel = m_a3DSFXModelItem[nModelType][nSFXModel].p3DModel;

	return S_OK;
}

int KModelMissileProcessor::SetSFXModelParam(int nModelType, int nSFXModel, float fModelScale, D3DXVECTOR3 vWorldPosition, D3DXQUATERNION qWorldRotation)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    hr = GetSFXModel(nModelType, nSFXModel, &p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetScale(p3DModel, fModelScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::SetPosition(p3DModel, vWorldPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::SetRotation(p3DModel, qWorldRotation);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}


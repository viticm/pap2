#include "stdafx.h"
#include "./klightningmissileprocessor.h"
#include "../krlmissile.h"
#include "../../../common/kmathtools.h"

#if 0
KLightningMissileProcessor::KLightningMissileProcessor()
: m_fTotalModleScale(0.0f)
{
}

KLightningMissileProcessor::~KLightningMissileProcessor()
{
}

int KLightningMissileProcessor::Init()
{
	return true;
}

int KLightningMissileProcessor::UnInit()
{
	return true;
}

int KLightningMissileProcessor::Update(double fTime, double fTimeLast, KRLMissile* pRLMissile)
{
	int nRetCode = false;
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 vPosition(0.0f, 0.0f, 0.0f);
	D3DXQUATERNION qRotation;
	double fTimeDelta = 0.0;
	double fTimeTotal = 0.0;

	fTimeDelta = fTime - GetTimeStart();

	fTimeTotal = GetTimeTotal();

	if (fTimeDelta <= fTimeTotal)
	{
		int nSFXModelCount = 0;

		nRetCode = abs(fTimeTotal) > FLT_EPSILON;
		KGLOG_PROCESS_ERROR(nRetCode);

		nSFXModelCount = GetSFXModelCount();

		for (int nSFXModel = 0; nSFXModel < nSFXModelCount; ++nSFXModel)
		{
            float fModelScale = 0.0f;
			KLightningMissile& Missile = m_aFulminationMissile[nSFXModel];

            fModelScale = static_cast<float>(m_fTotalModleScale * (fTimeDelta / fTimeTotal) + m_pMissilePhaseModel->fModelScaleBegin);

			hr = GetTargetAndCasterPosition(Missile);
			KGLOG_COM_PROCESS_ERROR(hr);

            hr = SetLightningSFXModel(MISSILE_PHASE_SFX_TYPE_SFX, nSFXModel, fModelScale, Missile.vPositionCaster, Missile.vPositionTarget);
            KGLOG_COM_PROCESS_ERROR(hr);
		}
	}

	nRetCode = KTimelineMissileProcessor::OnUpdate(fTime, fTimeLast);
	KGLOG_PROCESS_ERROR(nRetCode);

	return true;
Exit0:
	return false;
}

int KLightningMissileProcessor::IsFinished() const
{
	return false;
}

int KLightningMissileProcessor::Start(double fTime, KRLMissile* pRLMissile)
{
	HRESULT hr = E_FAIL;
	int nRetCode = false;
	D3DXVECTOR3 vDistance(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vPositionTarget(0.0f, 0.0f, 0.0f);
	D3DXQUATERNION qRotationTarget;
	D3DXQUATERNION qRotationCaster;
    KMissilePhaseFulminationModel* pFulminationModle = NULL;

	ASSERT(m_pRLMissile);
	ASSERT(m_pMissilePhaseModel);

    pFulminationModle = (KMissilePhaseFulminationModel*)m_pMissilePhaseModel;

    // Track Parameter
    m_fTotalModleScale = m_pMissilePhaseModel->fModelScaleEnd - m_pMissilePhaseModel->fModelScaleBegin;

	SetTimeTotal(pFulminationModle->fPervasionTime);

	return true;
Exit0:
	return false;
}

HRESULT KLightningMissileProcessor::GetTargetAndCasterPosition(KLightningMissile& Missile) const
{
	HRESULT hr = E_FAIL;
	int nRetCode = false;
	KCaster const* pCaster = NULL;
	KTargetData const* pTarget = NULL;
	float fAngleTarget = 0.0f;
    float fAngleCaster = 0.0f;
    D3DXVECTOR3 vPosition;
    D3DXQUATERNION qRotation;

	KGLOG_PROCESS_ERROR(m_pRLMissile);

	pCaster = &m_pRLMissile->m_Param.Result.Caster;
	KGLOG_PROCESS_ERROR(pCaster);

	pTarget = &m_pRLMissile->m_Param.Result.Target;
	KGLOG_PROCESS_ERROR(pTarget);

    nRetCode = CalcCasterOrientation(Missile.vPositionCaster, fAngleCaster, *pCaster);
    KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = CalcTargetOrientation(Missile.vPositionTarget, fAngleTarget, *pCaster, const_cast<KTargetData*>(pTarget));
	KGLOG_PROCESS_ERROR(nRetCode);

	YawToQuatLH(qRotation, fAngleTarget);

	return S_OK;
Exit0:
	return hr;
}

int KLightningMissileProcessor::CalcCasterOrientation(D3DXVECTOR3& vPosition, float& fAngle, KCaster const& Caster) const
{
    int nResult = false;
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(m_pMissilePhaseModel);

    switch (Caster.eTargetType)
	{
	case ttPlayer:
    case ttNpc:
        {
			KRLCharacter* pRLCharacter = NULL;
			LPCTSTR szBoneName = NULL;
			D3DXVECTOR3 vOffset(0.0f, 0.0f, 0.0f);

            pRLCharacter = GetRLCharacter(Caster.dwCharacterID);
			KG_PROCESS_ERROR(pRLCharacter);

			szBoneName = m_pMissilePhaseModel->szBoneNameCaster;

            ASSERT(szBoneName);
			if (szBoneName[0] != _T('\0'))
			{
                hr = pRLCharacter->m_RenderData.GetBonePosition(vPosition, vOffset, szBoneName);
				KGLOG_COM_CHECK_ERROR(hr);

				if (FAILED(hr))
				{
					KGLogPrintf(KGLOG_ERR, "Cannot find bone [%s]\n", szBoneName);

                    pRLCharacter->GetPosition(vPosition);
				}
			}
			else
			{
				pRLCharacter->GetPosition(vPosition);
			}

			fAngle = pRLCharacter->GetDirectionLH();
		}
		break;
    case ttInvalid:
    case ttNoTarget:
    case ttCoordination:
    case ttDoodad:
    case ttItem:
    case ttItemPos:
        break;
	default:
		ASSERT(0);
		break;
	};

	nResult = true;
Exit0:
	return nResult;
}

BOOL KLightningMissileProcessor::CalcTargetOrientation(D3DXVECTOR3& vPosition, float& fAngle, KCaster const& Caster, KTargetData* pTarget) const
{
	HRESULT     hr          = E_FAIL;
    BOOL        nRetCode    = FALSE;    
	TARGET_TYPE eTargetType = ttInvalid;

	KGLOG_PROCESS_ERROR(m_pMissilePhaseModel);
	KGLOG_PROCESS_ERROR(pTarget);
    
	eTargetType = pTarget->m_eTargetType;

	switch (eTargetType)
	{
	case ttCoordination:
		{
			int nX = 0;
			int nY = 0;
			int nZ = 0;
            
			nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(pTarget, &nX, &nY, &nZ);
            KGLOG_PROCESS_ERROR(nRetCode);

			nRetCode = CalcCasterOrientation(vPosition, fAngle, Caster);
			KGLOG_PROCESS_ERROR(nRetCode);

			GameWorldCellPointToScenePoint(vPosition, nX, nY, nZ, TRUE);
		}
		break;
	case ttNpc:
    case ttPlayer:
		{
			KRLCharacter* pRLCharacter = NULL;
			LPCTSTR szBoneName = NULL;
            KSceneObject* pSceneObject = NULL;
			D3DXVECTOR3 vOffset(0.0f, 0.0f, 0.0f);

            nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(pTarget, &pSceneObject);
            KGLOG_PROCESS_ERROR(nRetCode && pSceneObject);
            
			pRLCharacter = reinterpret_cast<KRLCharacter*>(pSceneObject->m_pRepresentObject);
			KGLOG_PROCESS_ERROR(pRLCharacter);

			szBoneName = m_pMissilePhaseModel->szBoneNameCaster;
            ASSERT(szBoneName);
			if (szBoneName[0] != _T('\0'))
			{
                hr = pRLCharacter->m_RenderData.GetBonePosition(vPosition, vOffset, szBoneName);
				if (FAILED(hr))
					pRLCharacter->GetPosition(vPosition);
			}
			else
			{
				pRLCharacter->GetPosition(vPosition);
			}

			fAngle = pRLCharacter->GetDirectionLH();
		}
		break;
	case ttDoodad:
		{
			KRLDoodad* pRLDoodad = NULL;
            KSceneObject* pSceneObject = NULL;

            nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(pTarget, &pSceneObject);
            KGLOG_PROCESS_ERROR(nRetCode && pSceneObject);
            
            pRLDoodad = reinterpret_cast<KRLDoodad*>(pSceneObject->m_pRepresentObject);
			KGLOG_PROCESS_ERROR(pRLDoodad);

			pRLDoodad->GetPosition(vPosition);

			fAngle = pRLDoodad->GetDirectionLH();
		}
		break;
	default:
		ASSERT(0);
		break;
	};

	return TRUE;
Exit0:
	return FALSE;
}

#endif

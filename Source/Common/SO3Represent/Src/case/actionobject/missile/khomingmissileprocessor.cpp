#include "stdafx.h"
#include "./khomingmissileprocessor.h"
#include "./kcompositemissileprocessor.h"
#include "../krlmissile.h"
#include "../krlcharacter.h"
#include "../krldoodad.h"
#include "../krlscene.h"
#include "../../render/kmodel.h"
#include "../../../common/kmathtools.h"
#include "../../../SO3Represent.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "../../Source/Common/SO3World/Src/KSceneObject.h"

static int GetTargetPosition(KTargetData const* pTarget, LPCTSTR szBoneName, D3DXVECTOR3& vPosition)
{
    int nRetCode = false;
    int nResult = false;
	TARGET_TYPE eTargetType = ttInvalid;

	KGLOG_PROCESS_ERROR(pTarget);
    
	eTargetType = pTarget->m_eTargetType;

	switch (eTargetType)
	{
    case ttInvalid:
    case ttNoTarget:
        KG_PROCESS_ERROR(0);
        break;

	case ttCoordination:
		{
			int nX = 0;
			int nY = 0;
			int nZ = 0;
            
			nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(const_cast<KTargetData*>(pTarget), &nX, &nY, &nZ);
            KGLOG_PROCESS_ERROR(nRetCode);

			GameWorldCellPointToScenePoint(vPosition, nX, nY, nZ, TRUE);
		}
		break;
	case ttNpc:
    case ttPlayer:
		{
			KRLCharacter* pRLCharacter = NULL;
            KSceneObject* pSceneObject = NULL;
			D3DXVECTOR3 vOffset(0.0f, 0.0f, 0.0f);
            int nGetPosition = false;

            nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(const_cast<KTargetData*>(pTarget), &pSceneObject);
            KG_PROCESS_ERROR(nRetCode);
            KG_PROCESS_ERROR(pSceneObject);
            
			pRLCharacter = reinterpret_cast<KRLCharacter*>(pSceneObject->m_pRepresentObject);
			KG_PROCESS_ERROR(pRLCharacter);

			if (szBoneName && szBoneName[0] != _T('\0'))
			{
                if (SUCCEEDED(pRLCharacter->m_RenderData.GetBonePosition(vPosition, vOffset, szBoneName)))
                {
                    nGetPosition = true;
                }
			}
			
            if (!nGetPosition)
			{
				pRLCharacter->GetPosition(vPosition);
			}
		}
		break;
	case ttDoodad:
		{
			KRLDoodad* pRLDoodad = NULL;
            KSceneObject* pSceneObject = NULL;

            nRetCode = g_pRL->m_pSO3WorldClient->GetTarget(const_cast<KTargetData*>(pTarget), &pSceneObject);
            KG_PROCESS_ERROR(nRetCode);
            KG_PROCESS_ERROR(pSceneObject);
            
            pRLDoodad = reinterpret_cast<KRLDoodad*>(pSceneObject->m_pRepresentObject);
			KG_PROCESS_ERROR(pRLDoodad);

			pRLDoodad->GetPosition(vPosition);
		}
		break;
	default:
		ASSERT(0);
		break;
	}

    nResult = true;
Exit0:
    return nResult;
}

static int GetMissileRotation(D3DXVECTOR3 vMissileToTarget, D3DXQUATERNION& qRotation)
{
    int nResult = false;
    D3DXVECTOR3 vAxis;
    D3DXVECTOR3 vInitialDirection(-1.0f, 0.0f, 0.0f);// 模型默认朝向
    D3DXVECTOR3 vCurrentDirection;
    float fAngle = 0.0f;

    KG_PROCESS_ERROR(D3DXVec3Length(&vMissileToTarget) > FLT_EPSILON);

    D3DXVec3Normalize(&vMissileToTarget, &vMissileToTarget);

    D3DXVec3Cross(&vAxis, &vInitialDirection, &vMissileToTarget);
    fAngle = acosf(D3DXVec3Dot(&vInitialDirection, &vMissileToTarget));     // TODO: fAngle [0, pi]

    D3DXQuaternionRotationAxis(&qRotation, &vAxis, fAngle);

    nResult = true;
Exit0:
    return nResult;
}

static int GetCasterOrientation(D3DXVECTOR3& vPosition, D3DXQUATERNION& qRotation, KCaster const* pCaster, LPCTSTR szBoneName)
{
    int nResult = false;
	HRESULT hr = E_FAIL;

    KGLOG_PROCESS_ERROR(pCaster);

    switch (pCaster->eTargetType)
	{
	case ttPlayer:
    case ttNpc:
        {
			KRLCharacter* pRLCharacter = NULL;
			D3DXVECTOR3 vOffset(0.0f, 0.0f, 0.0f);
            float fDirectionLH = 0.0f;

            pRLCharacter = GetRLCharacter(pCaster->dwCharacterID);
			KG_PROCESS_ERROR(pRLCharacter);

			if (szBoneName && szBoneName[0] != _T('\0'))
			{
                hr = pRLCharacter->m_RenderData.GetBonePosition(vPosition, vOffset, szBoneName);
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

			fDirectionLH = pRLCharacter->GetDirectionLH();
        	YawToQuatLH(qRotation, YawLHToModelLH(fDirectionLH));
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

KHomingMissileProcessor::KHomingMissileProcessor()
: m_nFinished(false)
{
}

int KHomingMissileProcessor::Init(DWORD dwMissileParamID, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    int nRetCode = false;
    int nResult = false;
    KRLMissileHomingParam const* pParam = NULL;
    
    pParam = g_pRL->m_TableList.GetMissileHomingParam(dwMissileParamID);
    KGLOG_PROCESS_ERROR(pParam);

    memcpy(&m_Param, pParam, sizeof(m_Param));

    // TODO:
    {
        KRLMissile* pMissile = NULL;
        KRLMissileRTParam* pRTParam = NULL;
        D3DXMATRIX mRotation;

        KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);

        pMissile = pCompositeMissileProcessor->m_pRLMissile;
        KGLOG_PROCESS_ERROR(pMissile);

        pRTParam = &pMissile->m_Param.RTParam;

        nRetCode = GetCasterOrientation(pRTParam->vPosition, pRTParam->qRotation, &pMissile->m_Param.Result.Caster, m_Param.szCasterBoneName);
        KGLOG_PROCESS_ERROR(nRetCode);

        // 初始速度
        D3DXMatrixRotationQuaternion(&mRotation, &pRTParam->qRotation);

        pRTParam->vVelocity.x = 0.0f;
        pRTParam->vVelocity.y = 0.0f;
        pRTParam->vVelocity.z = -m_Param.fInitialVelocity;

        D3DXVec3TransformCoord(&pRTParam->vVelocity, &pRTParam->vVelocity, &mRotation);    
    }


    nResult = true;
Exit0:
    if (!nResult)
    {
        memset(&m_Param, 0, sizeof(m_Param));

        m_Param.fInitialVelocity = 0.0f;
        m_Param.fAcceleration = 0.1f;
        m_Param.fTargetRadius = 50.0f;
        m_Param.fConvergenceFactor = 0.01f;
    }
    return nResult;
}

int KHomingMissileProcessor::UnInit(KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    return true;
}

int KHomingMissileProcessor::Update(double fTime, double fTimeLast, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
	int nRetCode = false;
    int nResult = false;
    float fVelocity = 0.0f;
    D3DXVECTOR3 vTargetPosition;
    D3DXVECTOR3 vMissileToTarget;
    D3DXVECTOR3 vVelocity;
    KRLMissileRTParam* pRTParam = NULL;
    KTargetData const* pTarget = NULL;

    KG_PROCESS_SUCCESS(m_nFinished);

    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor);
    KGLOG_PROCESS_ERROR(pCompositeMissileProcessor->m_pRLMissile);

    pRTParam = &pCompositeMissileProcessor->m_pRLMissile->m_Param.RTParam;
    pTarget = &pCompositeMissileProcessor->m_pRLMissile->m_Param.Result.Target;

    if (pTarget->m_eTargetType != ttInvalid)
    {
        // 如果没有找到目标，半途消失
        nRetCode = GetTargetPosition(pTarget, m_Param.szTargetBoneName, vTargetPosition);
        KG_PROCESS_ERROR(nRetCode);

        // 找到目标

        vVelocity = vTargetPosition - pRTParam->vPosition;
    }
    else
    {
        // 伤害包没到的情况， 直飞

        static float const TARGET_DISTANCE = 10000.0f; // 100 m

        vVelocity = pRTParam->vVelocity * TARGET_DISTANCE;
        vTargetPosition = pRTParam->vPosition + vVelocity;
    }

    fVelocity = m_Param.fAcceleration * (float)(fTime - fTimeLast);

    if (fVelocity > 0.0f)
    {
        if (D3DXVec3Length(&vVelocity) > fVelocity)
        {
            D3DXVec3Normalize(&vVelocity, &vVelocity);
            vVelocity *= fVelocity;
        }

        pRTParam->vVelocity = vVelocity * m_Param.fConvergenceFactor + pRTParam->vVelocity * (1.0f - m_Param.fConvergenceFactor);

        // current world position
        pRTParam->vPosition += pRTParam->vVelocity;
    }

    vMissileToTarget = pRTParam->vPosition - vTargetPosition;

    if (D3DXVec3Length(&vMissileToTarget) < m_Param.fTargetRadius)
    {
        // hit target

        m_nFinished = true;

        nRetCode = pCompositeMissileProcessor->m_pRLMissile->HitTarget();
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        // current world rotation
        nRetCode = GetMissileRotation(vMissileToTarget, pRTParam->qRotation);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
	nResult = true;
Exit0:
    if (!nResult)
    {
        m_nFinished = true;
    }
	return nResult;
}

int KHomingMissileProcessor::IsFinished() const
{
	return m_nFinished;
}

int KHomingMissileProcessor::Start(double fTime, KCompositeMissileProcessor* pCompositeMissileProcessor)
{
    int nRetCode = false;
    int nResult = false;

    m_nFinished = false;

    nRetCode = Update(fTime, fTime, pCompositeMissileProcessor);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
	return nResult;
}


#include "stdafx.h"
#include "./krlscene.h"
#include "./krltarget.h"
#include "./krlremotecharacter.h"
#include "../render/kmodel.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"

KRLTarget::KRLTarget()
: m_pRLScene(NULL)
, m_nRLForceRelation(frtInvalid)
{
	memset(m_ap3DModel, 0, sizeof(m_ap3DModel));
	Reset();
}

KRLTarget::~KRLTarget()
{
	ASSERT(m_nRLForceRelation == RL_FORCE_RELATION_INVALID);

	for (size_t nIndex = 0; nIndex < RL_FORCE_RELATION_COUNT; ++nIndex)
		KGLOG_CHECK_ERROR(m_ap3DModel[nIndex] == NULL);
	KGLOG_CHECK_ERROR(m_pRLScene == NULL);
}

HRESULT KRLTarget::Reset()
{
	m_dwAttachedSceneObjectID = UINT_MAX;
    m_dwAttachedSceneObjectType = RL_OBJECT_UNKNOWN;

    m_vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    return S_OK;
}

int KRLTarget::Update(double /* fTime */, double /* fTimeLast */, DWORD /* dwGameLoop */, BOOL /* bFrame */)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;

	nRetCode = m_dwAttachedSceneObjectType == RL_OBJECT_UNKNOWN;
	KG_PROCESS_SUCCESS(nRetCode);

	nRetCode = m_ap3DModel[m_nRLForceRelation] == NULL;
	KG_PROCESS_SUCCESS(nRetCode);

    switch (m_dwAttachedSceneObjectType)
    {
    case RL_OBJECT_PLAYER:
    case RL_OBJECT_NPC:
        {
            KRLRemoteCharacter* pRLRemoteCharacter = NULL;

            pRLRemoteCharacter = m_pRLScene->m_CharacterMgr.Lookup(m_dwAttachedSceneObjectID);
            if (pRLRemoteCharacter)
            {
                pRLRemoteCharacter->m_RLCharacter.GetPosition(m_vPosition);
            }
            else
            {
                m_vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

                hr = Hide();
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }
        break;
    case RL_OBJECT_DOODAD:
    case RL_OBJECT_RIDES:
        break;
    default:
        ASSERT(0);
        break;
    }

	hr = KModel::SetPosition(m_ap3DModel[m_nRLForceRelation], m_vPosition);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
	nResult = true;
Exit0:
    if (!nResult)
    {
    }
	return nResult;
}

HRESULT KRLTarget::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	return E_FAIL;
}

HRESULT KRLTarget::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo* pMotionCallBackExtraInfo)
{
    return E_FAIL;
}

HRESULT KRLTarget::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
	return E_NOTIMPL;
}

HRESULT KRLTarget::Init(KRLScene* pRLScene)
{
	HRESULT hr = E_FAIL;

	ASSERT(pRLScene);
	ASSERT(m_pRLScene == NULL);

	m_pRLScene = pRLScene;

	hr = LoadFile();
	KGLOG_COM_CHECK_ERROR(hr);

	return S_OK;
}

HRESULT KRLTarget::Exit()
{
	HRESULT hr = E_FAIL;

	hr = Hide();
	KGLOG_COM_CHECK_ERROR(hr);

	hr = UnloadFile();
	KGLOG_COM_CHECK_ERROR(hr);

	m_pRLScene = NULL;

	return S_OK;
}

HRESULT KRLTarget::Show(int nForceRelationType)
{
	int nRetCode = false;
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
    KRLMonoAnimationParam MonoAnimationParam;
	RL_FORCE_RELATION nRLForceRelation = ForceRelationGWToRL(nForceRelationType);
    IKG3DModel* p3DModel = NULL;

	nRetCode = nRLForceRelation >= 0 && nRLForceRelation < RL_FORCE_RELATION_COUNT;
	KGLOG_PROCESS_ERROR(nRetCode);

	if (m_nRLForceRelation != nRLForceRelation)
	{
		if (m_ap3DModel[nRLForceRelation])
		{
			hr = Hide();
			KGLOG_COM_PROCESS_ERROR(hr);
		}

		m_nRLForceRelation = nRLForceRelation;

        p3DModel = m_ap3DModel[m_nRLForceRelation];
		KG_PROCESS_SUCCESS(p3DModel == NULL);

        hr = KModel::SetPosition(p3DModel, m_vPosition);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = KModel::AppendRenderEntity(m_pRLScene->m_p3DScene, p3DModel, FALSE);
		KGLOG_COM_PROCESS_ERROR(hr);

        MonoAnimationParam.dwPlayType = ENU_ANIMATIONPLAY_ONCE;
        MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
        MonoAnimationParam.fSpeed = 1.0f;
        MonoAnimationParam.nOffsetTime = 0;
        MonoAnimationParam.dwTweenTime = 0;
        MonoAnimationParam.pUserdata = NULL;
        MonoAnimationParam.szFilePath = _T("");
        MonoAnimationParam.fStartTime = g_pRL->m_fTime;

		hr = KModel::PlayMonoAnimation(p3DModel, MonoAnimationParam);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hr = AdjustScale();
	KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KRLTarget::Hide()
{
	int nRetCode = false;
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

	nRetCode = m_nRLForceRelation == RL_FORCE_RELATION_INVALID;
	KG_PROCESS_SUCCESS(nRetCode);

    p3DModel = m_ap3DModel[m_nRLForceRelation];
	KG_PROCESS_SUCCESS(p3DModel == NULL);

	hr = KModel::PauseAnimation(p3DModel, TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::RemoveRenderEntity(m_pRLScene->m_p3DScene, p3DModel, FALSE);
	KGLOG_COM_PROCESS_ERROR(hr);

	m_nRLForceRelation = RL_FORCE_RELATION_INVALID;

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KRLTarget::LoadFile()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

	for (int nIndex = 0; nIndex < RL_FORCE_RELATION_COUNT; ++nIndex)
	{
		KForceRelationModel const* pModel = NULL;
		LPCTSTR szFilePath = NULL;
		float fScale = 0.0f;

		pModel = g_pRL->m_TableList.GetForceRelationModel(nIndex);
		szFilePath = pModel->szSFXFile;
		fScale = pModel->fSFXScale;

		if (szFilePath[0] != _T('\0'))
		{
			hr = KModel::LoadModel(m_pRLScene, &p3DModel, NULL, szFilePath);
            KGLOG_COM_PROCESS_ERROR(hr);
		}

		if (abs(fScale) <= FLT_EPSILON)
			fScale = 1.0f;

		if (p3DModel)
		{
			D3DXVECTOR3 vTargetMin;
			D3DXVECTOR3 vTargetMax;

			hr = KModel::GetBBox(p3DModel, vTargetMin, vTargetMax);
			KGLOG_COM_CHECK_ERROR(hr);

			m_aModelDiagonal[nIndex] = GetXZPlanePointDistance(vTargetMin, vTargetMax) / fScale;
		}
		else
		{
			m_aModelDiagonal[nIndex] = 1.0f / fScale;
		}

        m_ap3DModel[nIndex] = p3DModel;
        p3DModel = NULL;
	}

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (p3DModel)
        {
            hr = KModel::UnloadMDL(m_pRLScene, &p3DModel, NULL, FALSE, FALSE);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
    return hrResult;
}

HRESULT KRLTarget::UnloadFile()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	for (size_t nIndex = 0; nIndex < RL_FORCE_RELATION_COUNT; ++nIndex)
	{
		if (m_ap3DModel[nIndex] == NULL)
			continue;

		hr = KModel::UnloadModel(m_pRLScene, &m_ap3DModel[nIndex], NULL);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLTarget::AttachSceneObject(DWORD dwObject, DWORD dwObjectType)
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(m_pRLScene);

	KG_PROCESS_ERROR(dwObject != UINT_MAX);

    switch (dwObjectType)
    {
    case RL_OBJECT_UNKNOWN:
        break;
    case RL_OBJECT_PLAYER:
        if (g_pRL->m_pSO3World->m_dwClientPlayerID != dwObject)
            KG_PROCESS_ERROR(m_pRLScene->m_CharacterMgr.Lookup(dwObject));
        break;
    case RL_OBJECT_NPC:
        KG_PROCESS_ERROR(m_pRLScene->m_CharacterMgr.Lookup(dwObject));
        break;
    case RL_OBJECT_DOODAD:
    case RL_OBJECT_RIDES:
        KG_ASSERT_EXIT(0);
        break;
    default:
        KG_ASSERT_EXIT(0);
        break;
    }

	m_dwAttachedSceneObjectID = dwObject;
	m_dwAttachedSceneObjectType = dwObjectType;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
	    KGLogPrintf(KGLOG_ERR, "KActionTarget::AttachSceneObject(%u, %u)\n", dwObject, dwObjectType);
    }
	return hrResult;
}

HRESULT KRLTarget::DetachSceneObject(DWORD dwObject, DWORD dwObjectType)
{
	Reset();

	return S_OK;
}

float KRLTarget::GetDefaultScale() const
{
    int nResult = false;
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 vObjectMin;
	D3DXVECTOR3 vObjectMax;
	IKG3DModel* p3DModel = NULL;
	float fScale = 1.0f;
	float fObjectDiagonal = 0.0f;

	p3DModel = m_ap3DModel[m_nRLForceRelation];
	KG_PROCESS_SUCCESS(p3DModel == NULL);

    switch (m_dwAttachedSceneObjectType)
    {
    case RL_OBJECT_PLAYER:
    case RL_OBJECT_NPC:
        {
            KRLRemoteCharacter* pRLRemoteCharacter = NULL;

            pRLRemoteCharacter = m_pRLScene->m_CharacterMgr.Lookup(m_dwAttachedSceneObjectID);
            KG_PROCESS_SUCCESS(pRLRemoteCharacter == NULL);

            hr = pRLRemoteCharacter->m_RLCharacter.GetBBox(vObjectMin, vObjectMax);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
        break;
    case RL_OBJECT_DOODAD:
    case RL_OBJECT_RIDES:
        KG_ASSERT_EXIT(0);
        break;
    default:
        KG_ASSERT_EXIT(0);
        break;
    }

	fObjectDiagonal = GetXZPlanePointDistance(vObjectMin, vObjectMax);
	fScale = fObjectDiagonal / m_aModelDiagonal[m_nRLForceRelation];

Exit1:
	nResult = true;
Exit0:
    if (!nResult)
    {
        fScale = 1.0f;
    }
	return fScale;
}

float KRLTarget::GetCharcterScale() const
{
	float fScale = 1.0f;
    KRLCharacter* pRLCharacter = NULL;

	pRLCharacter = GetRLCharacter(m_dwAttachedSceneObjectID);
	KGLOG_PROCESS_ERROR(pRLCharacter);

    fScale = pRLCharacter->m_EquipResource.fSelectionScale;

Exit0:
	return fScale;
}

HRESULT KRLTarget::AdjustScale()
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
	IKG3DModel* p3DModel = NULL;
	float fScale = 1.0f;

	p3DModel = m_ap3DModel[m_nRLForceRelation];
	KG_PROCESS_SUCCESS(p3DModel == NULL);

	switch (m_dwAttachedSceneObjectType)
	{
	case RL_OBJECT_PLAYER:
    case RL_OBJECT_NPC:
        fScale = GetCharcterScale();
		break;
	case RL_OBJECT_UNKNOWN:
        break;
	case RL_OBJECT_DOODAD:	/* fall through */
    case RL_OBJECT_RIDES:
		fScale = GetDefaultScale();
		break;
	};

	hr = KModel::SetScale(p3DModel, fScale);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

#include "stdafx.h"
#include "./krlcursor.h"
#include "./krlscene.h"
#include "./krlcamera.h"
#include "../render/kmodel.h"
#include "../../SO3Represent.h"

KRLCursor::KRLCursor()
: m_p3DModel(NULL)
, m_pRLScene(NULL)
, m_bVisible(TRUE)
{
}

KRLCursor::~KRLCursor()
{
	KGLOG_CHECK_ERROR(m_p3DModel == NULL);
	KGLOG_CHECK_ERROR(m_pRLScene == NULL);
}

int KRLCursor::Update(double /* fTime */, double /* fTimeLast */, DWORD /* dwGameLoop */, BOOL /* bFrame */)
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = UpdatePosition();
    KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLCursor::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	return S_OK;
}

HRESULT KRLCursor::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo* pMotionCallBackExtraInfo)
{
    return S_OK;
}

HRESULT KRLCursor::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
    return S_OK;
}

int KRLCursor::UpdatePosition()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    D3DXVECTOR2 vScreen;
    D3DXVECTOR3 vPosition;
    KRLCamera* pRLCamera = NULL;
    IKG3DScene* p3DScene = NULL;
	POINT ptCursor;

    KG_PROCESS_SUCCESS(m_p3DModel == NULL);

    KGLOG_PROCESS_ERROR(m_pRLScene);

    pRLCamera = m_pRLScene->m_pRLCamera;
    KGLOG_PROCESS_ERROR(pRLCamera);

    p3DScene = m_pRLScene->m_p3DScene;
    KGLOG_PROCESS_ERROR(p3DScene);

    nRetCode = pRLCamera->IsBeingDragged();
    KG_PROCESS_SUCCESS(nRetCode);

	nRetCode = ::GetCursorPos(&ptCursor);
	KGLOG_PROCESS_ERROR(nRetCode);
	
    nRetCode = ::ScreenToClient(g_pRL->m_hWnd, &ptCursor);
	KGLOG_PROCESS_ERROR(nRetCode);

    vScreen.x = (float)ptCursor.x;
    vScreen.y = (float)ptCursor.y;

	hr = p3DScene->GetSelectPosition(vScreen, vPosition);
	if (FAILED(hr))
    {
        vPosition.x = FLT_MAX;
        vPosition.y = FLT_MAX;
        vPosition.z = FLT_MAX;
    }

	hr = KModel::SetPosition(m_p3DModel, vPosition);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
	nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLCursor::Init(KRLScene* pRLScene)
{
	HRESULT hr = E_FAIL;

	ASSERT(pRLScene);
	ASSERT(m_pRLScene == NULL);
	ASSERT(m_p3DModel == NULL);

	m_pRLScene = pRLScene;

	return S_OK;
}

HRESULT KRLCursor::Exit()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if (m_p3DModel)
	{
		hr = Hide();
		KGLOG_COM_PROCESS_ERROR(hr);
	}
	m_pRLScene = NULL;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KRLCursor::Show(DWORD dwSkillID, DWORD dwSkillLevel, ROLE_TYPE nRoleType)
{
    int nRetCode = false;
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KSkillCasterModel const* pModel = NULL;
	LPCTSTR szFilePath = NULL;
	float fScale = 1.0f;

	KG_PROCESS_SUCCESS(m_bVisible);

    KGLOG_PROCESS_ERROR(m_pRLScene);

	m_bVisible = TRUE;

	if (m_p3DModel)
	{
		hr = Hide();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	pModel = g_pRL->m_TableList.GetPlayerSkillCasterModel(dwSkillID, dwSkillLevel, nRoleType);
	KGLOG_PROCESS_ERROR(pModel);

	szFilePath = pModel->szAOESelectionSFXFile;
	fScale = pModel->fAOESelectionSFXScale;
	
	if (szFilePath[0] != _T('\0'))
	{
		hr = KModel::LoadModel(m_pRLScene, &m_p3DModel, NULL, szFilePath);
		KGLOG_COM_PROCESS_ERROR(hr);

        hr = KModel::SetPosition(m_p3DModel, D3DXVECTOR3(0.0f, 0.0f, 0.0f));
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = KModel::AppendRenderEntity(m_pRLScene->m_p3DScene, m_p3DModel, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);

		hr = KModel::SetScale(m_p3DModel, fScale);
		KGLOG_COM_CHECK_ERROR(hr);
	}

    nRetCode = UpdatePosition();
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KRLCursor::Hide()
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	KG_PROCESS_SUCCESS(!m_bVisible);

	m_bVisible = FALSE;

	KG_PROCESS_SUCCESS(m_p3DModel == NULL);

    KGLOG_PROCESS_ERROR(m_pRLScene);

    hr = KModel::RemoveRenderEntity(m_pRLScene->m_p3DScene, m_p3DModel, FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = KModel::UnloadModel(m_pRLScene, &m_p3DModel, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

KRLScene* KRLCursor::GetScene()
{
	return m_pRLScene;
}

int GetRLCursorPosition(KRLScene* pRLScene, D3DXVECTOR3* pvPosition)
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hr = E_FAIL;

    IKG3DScene* p3DScene = NULL;
    D3DXVECTOR2 vScreen;
    D3DXVECTOR3 vPosition;
    POINT ptCursor;

    KGLOG_PROCESS_ERROR(pRLScene);
    KGLOG_PROCESS_ERROR(pvPosition);

    p3DScene = pRLScene->m_p3DScene;
    KGLOG_PROCESS_ERROR(p3DScene);

    nRetCode = ::GetCursorPos(&ptCursor);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::ScreenToClient(g_pRL->m_hWnd, &ptCursor);
    KGLOG_PROCESS_ERROR(nRetCode);

    vScreen.x = (float)ptCursor.x;
    vScreen.y = (float)ptCursor.y;

    hr = p3DScene->GetSelectPosition(vScreen, vPosition);
    if (FAILED(hr))
    {
        vPosition.x = FLT_MAX;
        vPosition.y = FLT_MAX;
        vPosition.z = FLT_MAX;
    }

    *pvPosition = vPosition;
    nResult = true;
Exit0:
    return nResult;
}

int ShowRLCursorEffect(KRLScene* pRLScene, KRLCursorEffectModel const* pcCursorEffectModel)
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    IKG3DModel* p3DModel = NULL;
    D3DXVECTOR3 vPosition;
    KRLMonoAnimationParam MonoAnimationParam;

    KGLOG_PROCESS_ERROR(pRLScene);
    KGLOG_PROCESS_ERROR(pcCursorEffectModel);
    KGLOG_PROCESS_ERROR(pcCursorEffectModel->szFilePath);

    p3DModel = pRLScene->m_ModelGCMgr.CreateAutoRemoveModel(pcCursorEffectModel->szFilePath, pRLScene);
    KGLOG_PROCESS_ERROR(p3DModel);

    nRetCode = GetRLCursorPosition(pRLScene, &vPosition);
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = KModel::SetPosition(p3DModel, vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::SetScale(p3DModel, pcCursorEffectModel->fScale);
    KGLOG_COM_CHECK_ERROR(hr);

    MonoAnimationParam.dwPlayType = pcCursorEffectModel->dwPlayType;
    MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
    MonoAnimationParam.fSpeed = pcCursorEffectModel->fSpeed;
    MonoAnimationParam.nOffsetTime = 0;
    MonoAnimationParam.dwTweenTime = 0;
    MonoAnimationParam.pUserdata = NULL;
    MonoAnimationParam.szFilePath = _T("");
    MonoAnimationParam.fStartTime = g_pRL->m_fTime;

    hr = KModel::PlayMonoAnimation(p3DModel, MonoAnimationParam);
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    return nResult;
}

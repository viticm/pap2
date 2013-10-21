#include "stdafx.h"
#include "./kmodelgcmgr.h"
#include "./kmodel.h"
#include "../actionobject/krlscene.h"
#include "../../SO3Represent.h"

KModelGCMgr::KModelGCMgr()
{
}

KModelGCMgr::~KModelGCMgr()
{
	ASSERT(m_a3DModelGC.empty());
	ASSERT(m_aAutoRemoveRenderEntity.empty());
	ASSERT(m_aAutoRemoveRenderEntityGC.empty());
}


IKG3DModel* KModelGCMgr::Create(LPCTSTR szFilePath)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
	IKG3DModel* p3DModel = NULL;
	IKG3DResourceManager* p3DModelManager = g_pRL->m_p3DModelManager;

    KGLOG_PROCESS_ERROR(p3DModelManager);
	KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

	hr = p3DModelManager->LoadResourceFromFile(szFilePath, 0, MLO_MULTI | MLO_MODEL_LOD, (IKG3DResourceBase**)&p3DModel);
    KG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    if (!nResult)
    {
        p3DModel = NULL;
		KFilePath::MissFile(szFilePath);
    }
	return p3DModel;
}

IKG3DModel* KModelGCMgr::CreateAutoRemoveModel(LPCTSTR szFilePath, KRLScene* pRLScene)
{
	int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;
	BOOL bAppendRenderEntity = FALSE;
	BOOL bRegisterEventHandler = FALSE;
    BOOL bAppendAutoRemoveRenderEntity = FALSE;
	IKG3DModel* p3DModel = NULL;
	IKG3DAnimationController* pController = NULL;
	KRenderEntity RenderEntity;

	p3DModel = Create(szFilePath);
	KGLOG_PROCESS_ERROR(p3DModel);

	RenderEntity.pRLScene = pRLScene;
	RenderEntity.p3DModel = p3DModel;
	RenderEntity.bSelectable = FALSE;
	
    hr = KModel::AppendRenderEntity(pRLScene->m_p3DScene, p3DModel, RenderEntity.bSelectable);
	KGLOG_COM_PROCESS_ERROR(hr);

	bAppendRenderEntity = TRUE;

	pController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
	KGLOG_PROCESS_ERROR(pController);

	hr = pController->RegistereEventHandler(this);
	KGLOG_COM_PROCESS_ERROR(hr);

	bRegisterEventHandler = TRUE;

	m_aAutoRemoveRenderEntity.push_back(RenderEntity);
    bAppendAutoRemoveRenderEntity = TRUE;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (bRegisterEventHandler)
        {
            pController->UnregisterEventHandler(this);
        }

        if (bAppendAutoRemoveRenderEntity)
        {
            m_aAutoRemoveRenderEntity.pop_back();
        }

        if (bAppendRenderEntity)
        {
            hr = KModel::RemoveRenderEntity(pRLScene->m_p3DScene, p3DModel, RenderEntity.bSelectable);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        SAFE_RELEASE(p3DModel);
    }
	return p3DModel;
}

int KModelGCMgr::Release(IKG3DModel* p3DModel)
{
    int nResult = false;

	KG_ASSERT_EXIT(p3DModel);

	m_a3DModelGC.push_back(p3DModel);

    nResult = true;
Exit0:
	return nResult;
}

int KModelGCMgr::Init()
{
	return true;
}

void KModelGCMgr::Exit()
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = PurgeAutoRemoveRenderEntity();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = PurgeAutoRemoveRenderEntityGC();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = Purge3DModelGC();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return;
}

int KModelGCMgr::Reset()
{
	int nRetCode = false;
    int nResult = false;

	Exit();

	nRetCode = Init();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
    if (!nResult)
    {
	    Exit();
    }
	return nResult;
}

int KModelGCMgr::Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    int nRetCode = false;
    int nResult = false;

    StartProfile(&g_pRL->m_Profile, KRLPROFILE_MODEL_GC);

    nRetCode = PurgeAutoRemoveRenderEntityGC();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = Purge3DModelGC();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    StopProfile(&g_pRL->m_Profile, KRLPROFILE_MODEL_GC);
    return nResult;
}

HRESULT KModelGCMgr::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    KG_PROCESS_SUCCESS(pController == NULL);

    p3DModel = pController->GetModel();
    KGLOG_PROCESS_ERROR(p3DModel);

    for (KRenderEntityList::iterator it = m_aAutoRemoveRenderEntity.begin(); it != m_aAutoRemoveRenderEntity.end(); ++it)
    {
        KRenderEntity RenderEntity = *it;

        if (RenderEntity.p3DModel == p3DModel)
        {
            m_aAutoRemoveRenderEntity.erase(it);

            hr = RenderEntity.p3DModel->SetAlpha(0.0f, FALSE);
            KGLOG_COM_PROCESS_ERROR(hr);

            pController->UnregisterEventHandler(this);

            m_aAutoRemoveRenderEntityGC.push_back(RenderEntity);

            break;
        }
    }

Exit1:
	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModelGCMgr::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModel, TagSFXCallBackExtraInfo* pCallBackExtraInfo)
{
	return S_OK;
}

HRESULT KModelGCMgr::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo* pMotionCallBackExtraInfo)
{
	return S_OK;
}

int KModelGCMgr::Purge3DModelGC()
{
	if (!m_a3DModelGC.empty())
	{
		IKG3DModePtrlList::iterator it = m_a3DModelGC.begin();
		IKG3DModePtrlList::iterator ie = m_a3DModelGC.end();
		for (; it != ie; ++it)
			SAFE_RELEASE(*it);
		m_a3DModelGC.clear();
	}

    return true;
}


int KModelGCMgr::PurgeAutoRemoveRenderEntity()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    IKG3DAnimationController* pController = NULL;

    if (!m_aAutoRemoveRenderEntity.empty())
    {
        KRenderEntityList::iterator it = m_aAutoRemoveRenderEntity.begin();
        KRenderEntityList::iterator ie = m_aAutoRemoveRenderEntity.end();
        for ( ; it != ie; ++it)
        {
            KRenderEntity RenderEntity = *it;

            pController = RenderEntity.p3DModel->GetAnimationController(ANICTL_PRIMARY);
            KGLOG_CHECK_ERROR(pController);

            if (pController)
                pController->UnregisterEventHandler(this);

            hr = KModel::RemoveRenderEntity(RenderEntity.pRLScene->m_p3DScene, RenderEntity.p3DModel, RenderEntity.bSelectable);
            KGLOG_COM_PROCESS_ERROR(hr);

            SAFE_RELEASE(RenderEntity.p3DModel);
        }

        m_aAutoRemoveRenderEntity.clear();
    }

    nResult = true;
Exit0:
    return nResult;
}

int KModelGCMgr::PurgeAutoRemoveRenderEntityGC()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    IKG3DAnimationController* pController = NULL;

    if (!m_aAutoRemoveRenderEntityGC.empty())
    {
        KRenderEntityList::iterator it = m_aAutoRemoveRenderEntityGC.begin();
        KRenderEntityList::iterator ie = m_aAutoRemoveRenderEntityGC.end();
        for ( ; it != ie; ++it)
        {
            KRenderEntity RenderEntity = *it;

            hr = KModel::RemoveRenderEntity(RenderEntity.pRLScene->m_p3DScene, RenderEntity.p3DModel, RenderEntity.bSelectable);
            KGLOG_COM_PROCESS_ERROR(hr);

            SAFE_RELEASE(RenderEntity.p3DModel);
        }

        m_aAutoRemoveRenderEntityGC.clear();
    }

    nResult = true;
Exit0:
    return nResult;
}

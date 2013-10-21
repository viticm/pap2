#include "StdAfx.h"
#include "KG3DMultiThreadLoader.h"
#include "KG3DAnimationTagContainer.h"


#include "KG3DModel.h"
#include "KG3DTexture.h"

KG3DMultiThreadLoader g_cMultiThreadLoader;

KG3DMultiThreadLoader::KG3DMultiThreadLoader(void)
{
	m_bTryPauseMultiThreadLoad = FALSE;
	m_bPauseMultiThreadLoad = FALSE;
}

KG3DMultiThreadLoader::~KG3DMultiThreadLoader(void)
{
	UnInit();
}

HRESULT KG3DMultiThreadLoader::Init()
{
    m_cMultiTreadLoadModels.Init();
	m_cMultiTreadLoadMaterial.Init();
	m_cMultiTreadLoadClip.Init();
	m_cMultiTreadLoadTag.Init();
	return S_OK;
}

HRESULT KG3DMultiThreadLoader::UnInit()
{
	m_cMultiTreadLoadModels.UnInit();
    m_cMultiTreadLoadMaterial.UnInit();
	m_cMultiTreadLoadClip.UnInit();
	m_cMultiTreadLoadTag.UnInit();
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
HRESULT KG3DMultiThreadLoader::TryMultiThreadLoad_Model(KG3DModel* pModel)
{
	HRESULT hr = E_FAIL;

	KGLOG_PROCESS_ERROR(pModel);
    if (pModel->m_bInMultiThreadLoadList)
    KGLOG_COM_PROCESS_ERROR(hr = E_MUTL_LOAD_ALREADY_IN_ARRAY);

	hr = m_cMultiTreadLoadModels.TryMultiThreadLoad(pModel,m_bTryPauseMultiThreadLoad);
Exit0:
	return hr;
}

HRESULT KG3DMultiThreadLoader::StopMultiThreadLoad_Model(KG3DModel* pModel)
{
	return m_cMultiTreadLoadModels.StopMultiThreadLoad(pModel);
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DMultiThreadLoader::TryMultiThreadLoad_Material(KG3DMaterial* pMaterial)
{
	return m_cMultiTreadLoadMaterial.TryMultiThreadLoad(pMaterial,m_bTryPauseMultiThreadLoad);
}

HRESULT KG3DMultiThreadLoader::StopMultiThreadLoad_Material(KG3DMaterial* pMaterial)
{
	return m_cMultiTreadLoadMaterial.StopMultiThreadLoad(pMaterial);
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DMultiThreadLoader::TryMultiThreadLoad_Clip(KG3DClip* pClip)
{
	return m_cMultiTreadLoadClip.TryMultiThreadLoad(pClip,m_bTryPauseMultiThreadLoad);
}

HRESULT KG3DMultiThreadLoader::StopMultiThreadLoad_Clip(KG3DClip* pClip)
{
	return m_cMultiTreadLoadClip.StopMultiThreadLoad(pClip);
}
//////////////////////////////////////////////////////////////////////////
HRESULT KG3DMultiThreadLoader::TryMultiThreadLoad_Tag(KG3DAnimationTagContainer* pTag)
{
	return m_cMultiTreadLoadTag.TryMultiThreadLoad(pTag,m_bTryPauseMultiThreadLoad);
}

HRESULT KG3DMultiThreadLoader::StopMultiThreadLoad_Tag(KG3DAnimationTagContainer* pTag)
{
	return m_cMultiTreadLoadTag.StopMultiThreadLoad(pTag);
}
//////////////////////////////////////////////////////////////////////////
HRESULT KG3DMultiThreadLoader::ProcessMultiThreadLoad()
{
	m_cMultiTreadLoadTag.ProcessMultiThreadLoad();
	m_cMultiTreadLoadClip.ProcessMultiThreadLoad();
	m_cMultiTreadLoadModels.ProcessMultiThreadLoad();
	m_cMultiTreadLoadMaterial.ProcessMultiThreadLoad();
	if(m_bTryPauseMultiThreadLoad)
	{
		m_cMultiTreadLoadTag.ProcessMultiThreadLoad();
		m_cMultiTreadLoadClip.ProcessMultiThreadLoad();
		m_cMultiTreadLoadModels.ProcessMultiThreadLoad();
		m_cMultiTreadLoadMaterial.ProcessMultiThreadLoad();
    }
	KG_PROCESS_SUCCESS(m_bTryPauseMultiThreadLoad);
	return S_OK;
Exit1:
	m_bPauseMultiThreadLoad = TRUE;
	m_bTryPauseMultiThreadLoad = TRUE;
	return S_OK;
}

HRESULT KG3DMultiThreadLoader::PauseMultiThreadLoad(BOOL bEnable)
{
	if(bEnable)
	{
		m_bTryPauseMultiThreadLoad = TRUE;
		while (!m_bPauseMultiThreadLoad)
		{
			Sleep(50);
		}
	}
	else
	{
		m_bTryPauseMultiThreadLoad = FALSE;
		m_bPauseMultiThreadLoad = FALSE;
	}

	return S_OK;
}

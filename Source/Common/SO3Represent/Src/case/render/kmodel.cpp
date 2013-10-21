#include "stdafx.h"
#include "./kmodel.h"
#include "../actionobject/krlscene.h"
#include "../actionobject/krlsceneobject.h"
#include "../../SO3Represent.h"
#include "SO3Represent/SO3GameWorldRepresenthandler.h"

#define MODEL_TRACE	0

namespace
{
#if MODEL_TRACE
	static int s_nMDLCount = 0;
	static int s_nModelCount = 0;
	static int s_nSFXSetCount = 0;
	static int s_nSelectableRenderEntityCount = 0;
	static int s_nUnselectableRenderEntityCount = 0;
    static int s_nUnselectableModelCount = 0;
#endif
}

int GetAnimationPlayTime(double fTime)
{
    int nOffsetTime = 0;

    if (fTime > 0.0)
        nOffsetTime = (int)(g_pRL->m_fTime - fTime);

    return nOffsetTime;
}

HRESULT KModel::LoadMDL(KRLScene* pRLScene, IKG3DModel** pp3DModel, 
                        KRLSceneObject* pRLSceneObject, LPCTSTR szMDLFilePath, 
                        BOOL bVisible, BOOL bSelectable)
{
#if MODEL_TRACE
	ATLTRACE("[%.4d]\tKModel::LoadMDL() %s\n", ++s_nMDLCount, szMDLFilePath);
#endif

	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;
	KSceneObject* pSceneObject = NULL;
    BOOL bAppendRenderEntity = FALSE;
    BOOL bRegistereEventHandler = FALSE;

	KGLOG_PROCESS_ERROR(pRLScene);
	KGLOG_PROCESS_ERROR(pp3DModel);
	KGLOG_PROCESS_ERROR(*pp3DModel == NULL);
    KGLOG_PROCESS_ERROR(szMDLFilePath && szMDLFilePath[0] != _T('\0'));

	p3DModel = pRLScene->m_ModelGCMgr.Create(szMDLFilePath);
	KG_PROCESS_ERROR(p3DModel);

    if (bVisible)
    {
        hr = AppendRenderEntity(pRLScene->m_p3DScene, p3DModel, bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);

        bAppendRenderEntity = TRUE;
    }

    hr = KModel::RegisterAnimationEventHandler(pRLScene, p3DModel, pRLSceneObject, bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    bRegistereEventHandler = TRUE;

    *pp3DModel = p3DModel;
    p3DModel = NULL;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bRegistereEventHandler)
        {
            hr = KModel::UnregisterAnimationEventHandler(pRLScene, p3DModel, pRLSceneObject, bSelectable);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        if (bAppendRenderEntity)
        {
            hr = RemoveRenderEntity(pRLScene->m_p3DScene, p3DModel, bSelectable);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        if (p3DModel)
        {
            pRLScene->m_ModelGCMgr.Release(p3DModel);
        }

        if (!g_pRL->m_FilePath.MissFile(szMDLFilePath))
        {
            KGLogPrintf(KGLOG_ERR, "RL KModel::LoadMDL(%.8x, %.8x, %.8x, %s)\n", pRLScene, pp3DModel, pRLSceneObject, szMDLFilePath );
        }
    }
	return hrResult;
}

HRESULT KModel::UnloadMDL(KRLScene* pRLScene, IKG3DModel** pp3DModel, KRLSceneObject* pRLSceneObject, BOOL bVisible, BOOL bSelectable)
{
#if MODEL_TRACE
	ATLTRACE("[%.4d]\tKModel::UnloadMDL(%.8x, %.8x, %d, %d)\n", s_nMDLCount--, *pp3DModel, pRLSceneObject, bVisible, bSelectable);
#endif

	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

	KGLOG_PROCESS_ERROR(pRLScene);
	KGLOG_PROCESS_ERROR(pp3DModel);

    p3DModel = *pp3DModel;
	KGLOG_PROCESS_ERROR(p3DModel);

    hr = KModel::UnregisterAnimationEventHandler(pRLScene, p3DModel, pRLSceneObject, bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (bVisible)
    {
        hr = RemoveRenderEntity(pRLScene->m_p3DScene, p3DModel, bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	p3DModel->RemoveCaption();
	pRLScene->m_ModelGCMgr.Release(p3DModel);
	
	*pp3DModel = NULL;

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModel::LoadModel(KRLScene* pRLScene, IKG3DModel** pp3DModel, IKG3DAnimationEventHandler* p3DAnimationEventHandler, LPCTSTR szModelFilePath)
{
#if MODEL_TRACE
	ATLTRACE("[%.4d]\tKModel::LoadModel(%.8x, %.8x, %.8x, %s)\n",
		++s_nUnselectableModelCount, pRLScene, pp3DModel, 
		p3DAnimationEventHandler, szModelFilePath ? szModelFilePath : "");
#endif

	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bRegisterePrimaryEventHandler = FALSE;
    BOOL bRegistereSecondaryEventHandler = FALSE;

	KGLOG_PROCESS_ERROR(pRLScene);
	KGLOG_PROCESS_ERROR(pp3DModel);
	KGLOG_PROCESS_ERROR(*pp3DModel == NULL);
    KGLOG_PROCESS_ERROR(szModelFilePath && szModelFilePath[0] != _T('\0'));

	*pp3DModel = pRLScene->m_ModelGCMgr.Create(szModelFilePath);
	KG_PROCESS_ERROR(*pp3DModel);

	if (p3DAnimationEventHandler)
	{
		IKG3DAnimationController* pController = NULL;

        pController = (*pp3DModel)->GetAnimationController(ANICTL_PRIMARY);
		if (pController)
		{
			hr = pController->RegistereEventHandler(p3DAnimationEventHandler);
			KGLOG_COM_PROCESS_ERROR(hr);

            bRegisterePrimaryEventHandler = TRUE;
		}

        pController = (*pp3DModel)->GetAnimationController(ANICTL_SECONDARY);
		if (pController)
		{
			hr = pController->RegistereEventHandler(p3DAnimationEventHandler);
            KGLOG_COM_PROCESS_ERROR(hr);

            bRegistereSecondaryEventHandler = TRUE;
        }
    }

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bRegistereSecondaryEventHandler)
        {
            IKG3DAnimationController* pController = (*pp3DModel)->GetAnimationController(ANICTL_SECONDARY);
            if (pController)
            {
                hr = pController->UnregisterEventHandler(p3DAnimationEventHandler);
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }

        if (bRegisterePrimaryEventHandler)
        {
            IKG3DAnimationController* pController = (*pp3DModel)->GetAnimationController(ANICTL_PRIMARY);
            if (pController)
            {
                hr = pController->UnregisterEventHandler(p3DAnimationEventHandler);
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }

        if (*pp3DModel)
        {
            pRLScene->m_ModelGCMgr.Release(*pp3DModel);
            *pp3DModel = NULL;
        }

        if (!g_pRL->m_FilePath.MissFile(szModelFilePath))
        {
            KGLogPrintf(KGLOG_ERR, "RL KModel::LoadUnselectableModel(%.8x, %.8x, %.8x, %s)\n", pRLScene, pp3DModel, p3DAnimationEventHandler, szModelFilePath);
        }
    }
	return hrResult;
}

HRESULT KModel::UnloadModel(KRLScene* pRLScene, IKG3DModel** pp3DModel, IKG3DAnimationEventHandler* p3DAnimationEventHandler)
{
#if MODEL_TRACE
	ATLTRACE("[%.4d]\tKModel::UnloadModel(%.8x, %.8x, %.8x)\n",
		s_nUnselectableModelCount--, pRLScene, pp3DModel, p3DAnimationEventHandler);
#endif

	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pRLScene);
	KGLOG_PROCESS_ERROR(pp3DModel);
	KGLOG_PROCESS_ERROR(*pp3DModel);

	if (p3DAnimationEventHandler)
	{
        IKG3DAnimationController* pController = NULL;

		pController = (*pp3DModel)->GetAnimationController(ANICTL_SECONDARY);
		if (pController)
		{
			hr = pController->UnregisterEventHandler(p3DAnimationEventHandler);
			KGLOG_COM_CHECK_ERROR(hr);
		}

        pController = (*pp3DModel)->GetAnimationController(ANICTL_PRIMARY);
        if (pController)
        {
            hr = pController->UnregisterEventHandler(p3DAnimationEventHandler);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

	hr = (*pp3DModel)->SetAlpha(0, FALSE);
    KGLOG_COM_CHECK_ERROR(hr);

	pRLScene->m_ModelGCMgr.Release(*pp3DModel);
	*pp3DModel = NULL;

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
    }
    return hrResult;
}

HRESULT KModel::RegisterAnimationEventHandler(KRLScene* pRLScene, IKG3DModel* p3DModel, KRLSceneObject* pRLSceneObject, BOOL bSelectable)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DAnimationController* pController = NULL;
	KSceneObject* pSceneObject = NULL;
	DWORD dwObject = 0;
	DWORD dwObjectType = 0;
    BOOL bRegisterePrimaryEventHandler = FALSE;
    BOOL bRegistereSecondaryEventHandler = FALSE;
    BOOL bRegisterModel = FALSE;

	KGLOG_PROCESS_ERROR(pRLScene);
	KGLOG_PROCESS_ERROR(p3DModel);

	KG_PROCESS_SUCCESS(pRLSceneObject == NULL);

    pController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
    if (pController)
    {
        hr = pController->RegistereEventHandler(pRLSceneObject);
        KGLOG_COM_PROCESS_ERROR(hr);

        bRegisterePrimaryEventHandler = TRUE;
    }

    pController = p3DModel->GetAnimationController(ANICTL_SECONDARY);
    if (pController)
    {
        hr = pController->RegistereEventHandler(pRLSceneObject);
        KGLOG_COM_PROCESS_ERROR(hr);

        bRegistereSecondaryEventHandler = TRUE;
    }

    if (bSelectable)
    {
        pSceneObject = pRLSceneObject->GetGameWorldSceneObject();
        KGLOG_PROCESS_ERROR(pSceneObject);

        dwObject = pRLSceneObject->GetObjectID();
        dwObjectType = pRLSceneObject->GetObjectType();

        hr = pRLScene->m_ModelMgr.RegisterModel(p3DModel, pSceneObject, dwObjectType, dwObject);
        KGLOG_COM_PROCESS_ERROR(hr);

        bRegisterModel = TRUE;
    }

Exit1:
	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bRegisterModel)
        {
            pRLScene->m_ModelMgr.UnregisterModel(p3DModel);
        }

        if (bRegistereSecondaryEventHandler)
        {
            pController = p3DModel->GetAnimationController(ANICTL_SECONDARY);
            if (pController)
            {
                hr = pController->UnregisterEventHandler(pRLSceneObject);
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }

        if (bRegisterePrimaryEventHandler)
        {
            pController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
            if (pController)
            {
                hr = pController->UnregisterEventHandler(pRLSceneObject);
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }
    }
	return hrResult;
}

HRESULT KModel::UnregisterAnimationEventHandler(KRLScene* pRLScene, IKG3DModel* p3DModel, KRLSceneObject* pRLSceneObject, BOOL bSelectable)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DAnimationController* pController = NULL;

	KGLOG_PROCESS_ERROR(pRLScene);
	KGLOG_PROCESS_ERROR(p3DModel);

    KG_PROCESS_SUCCESS(pRLSceneObject == NULL);

    if (bSelectable)
    {
        hr = pRLScene->m_ModelMgr.UnregisterModel(p3DModel);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    pController = p3DModel->GetAnimationController(ANICTL_SECONDARY);
    if (pController)
    {
        hr = pController->UnregisterEventHandler(pRLSceneObject);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    pController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
    if (pController)
    {
        hr = pController->UnregisterEventHandler(pRLSceneObject);
        KGLOG_COM_CHECK_ERROR(hr);
    }

Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModel::LoadMaterial(IKG3DModel* p3DModel, LPCTSTR szFilePath)
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);
	KGLOG_PROCESS_ERROR(szFilePath && szFilePath[0] != _T('\0'));

	hr = p3DModel->LoadMaterialFromFile(szFilePath, MLO_TEXTURE_MULTI);
	KG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (!g_pRL->m_FilePath.MissFile(szFilePath))
        {
            KGLogPrintf(KGLOG_ERR, "RL KModel::LoadMaterial(%.8x, %s)\n", p3DModel, szFilePath);
        }
    }
	return hrResult;
}

HRESULT KModel::AttachModel(IKG3DModel* p3DModelTo, IKG3DModel* p3DModel)
{
#if MODEL_TRACE
	ATLTRACE("KModel::AttachModel(%.8x, %.8x)\n", p3DModelTo, p3DModel);
#endif

    HRESULT hrResult = E_FAIL;
	int nIndex = -1;

	KGLOG_PROCESS_ERROR(p3DModelTo);
	KGLOG_PROCESS_ERROR(p3DModel);

	nIndex = p3DModelTo->AttachModel(p3DModel);
	KGLOG_PROCESS_ERROR(nIndex != -1);

	hrResult = S_OK;
Exit0:
	if(FAILED(hrResult))
	{
		KGLogPrintf(KGLOG_ERR, "KModel::AttachModel %d \n", nIndex);
	}
	return hrResult;
}

HRESULT KModel::DetachModel(IKG3DModel* p3DModelTo, IKG3DModel* p3DModel)
{
#if MODEL_TRACE
	ATLTRACE("KModel::DetachModel(%.8x, %.8x)\n", p3DModelTo, p3DModel);
#endif

    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModelTo);
	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModelTo->DetachModel(p3DModel);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetFaceMotionMode(IKG3DModel* p3DModel)
{
#if MODEL_TRACE
	ATLTRACE("KModel::SetFaceMotionMode(%.8x)\n", p3DModel);
#endif

    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModel->SetFaceMotionMode();

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::Bind(KRLScene* pRLScene, IKG3DModel* p3DModelTo, IKG3DModel* p3DModel, LPCTSTR szName)
{
#if MODEL_TRACE
	ATLTRACE("KModel::Bind(%.8x, %.8x, %.8x, %s)\n", pRLScene, p3DModelTo, p3DModel, szName ? szName : "");
#endif

	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pRLScene);
	KGLOG_PROCESS_ERROR(p3DModelTo);
	KGLOG_PROCESS_ERROR(p3DModel);

    if (szName && _tcslen(szName) > 2 && szName[0] == _T('S') && szName[1] == _T('_'))
    {
        hr = p3DModel->BindToSocket(p3DModelTo, szName);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        hr = p3DModel->BindToBone(p3DModelTo, szName);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
	    KGLogPrintf(KGLOG_ERR, "RL KModel::Bind(%.8x, %.8x, %s)\n", p3DModelTo, p3DModel, szName);
    }
	return hrResult;
}

HRESULT KModel::Unbind(KRLScene* pRLScene, IKG3DModel* p3DModelFrom, IKG3DModel* p3DModel)
{
#if MODEL_TRACE
	ATLTRACE("KModel::Unbind(%.8x, %.8x, %.8x)\n", pRLScene, p3DModelFrom, p3DModel);
#endif

	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(pRLScene);
	KGLOG_PROCESS_ERROR(p3DModelFrom);
	KGLOG_PROCESS_ERROR(p3DModel);

	hr = p3DModel->UnBindFromOther();
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModel::PlayMonoAnimation(IKG3DModel* p3DModel, KRLMonoAnimationParam Param)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    TweenTimeSpan Span;

	KGLOG_PROCESS_ERROR(p3DModel);

	Span.fTweenWeigth = 1.0f;

    if (Param.nOffsetTime > 0)
    {
        Span.dwTweenIn = 0;
        Span.dwTweenOut = 0;
    }
    else
    {
        Span.dwTweenIn = Param.dwTweenTime;
        Span.dwTweenOut = Param.dwTweenTime;
    }

    if (abs(Param.fSpeed) <= FLT_EPSILON)
        Param.fSpeed = 1.0f;

    if (!g_pRL->m_DebugOption.bDisableAnimation)
    {
        hr = p3DModel->PlayAnimation(Param.dwPlayType, Param.szFilePath, Param.fSpeed, 0, (void*)&Span, 
            Param.pUserdata, (enuAnimationControllerPriority)Param.nTrackType);
        KGLOG_COM_PROCESS_ERROR(hr);

        if (Param.nOffsetTime > 0)
        {
            hr = KModel::SeekMonoAnimation(p3DModel, Param);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

#ifdef RL_TRACE_OPTION
    if (g_pRL->m_CurrentTraceOption.bTrace3DAnimation)
    {
        printf("[RL] PlayMonoAnimation(%p, playtype:%d, %s, speed:%f, offsettime:%d, tweentime:%u, 0x%p, track:%d)\n", p3DModel, 
            Param.dwPlayType, Param.szFilePath, Param.fSpeed, Param.nOffsetTime, Param.dwTweenTime, Param.pUserdata, Param.nTrackType);
    }
#endif  // RL_TRACE_OPTION

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (!g_pRL->m_FilePath.MissFile(Param.szFilePath))
        {
            KGLogPrintf(KGLOG_ERR, "RL KModel::PlayMonoAnimation(%.8x, playtype:%d, %s, speed:%f, offsettime:%d, tweentime:%u, 0x%.8x, track:%d)\n", p3DModel, 
                Param.dwPlayType, Param.szFilePath, Param.fSpeed, Param.nOffsetTime, Param.dwTweenTime, Param.pUserdata, Param.nTrackType);
        }
    }
    return hrResult;
}

HRESULT KModel::SeekMonoAnimation(IKG3DModel* p3DModel, KRLMonoAnimationParam const& Param)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	IKG3DAnimationController* pAnimationController = NULL;

	KGLOG_PROCESS_ERROR(p3DModel);

	pAnimationController = p3DModel->GetAnimationController((enuAnimationControllerPriority)Param.nTrackType);
	KGLOG_PROCESS_ERROR(pAnimationController);
	
	pAnimationController->SeekAnimation(ANIMATION_SEEK_SET, Param.nOffsetTime);

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (!g_pRL->m_FilePath.MissFile(Param.szFilePath))
        {
            KGLogPrintf(KGLOG_ERR, "RL KModel::SeekMonoAnimation(%.8x, %u, %s, %d)\n", p3DModel, Param.nTrackType, Param.szFilePath, Param.nOffsetTime);
        }
    }
	return hrResult;
}

HRESULT KModel::SeekSplitAnimation(IKG3DModel* p3DModel, KRLSplitAnimationParam const& Param)
{
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);

    p3DModel->SeekSplitAnimation(
        (enuAnimationJointType)Param.dwExtraInfo, 
        Param.nTrackType, 
        ANIMATION_SEEK_SET, 
        (float)Param.nOffsetTime);
	
	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (!g_pRL->m_FilePath.MissFile(Param.szFilePath))
        {
            KGLogPrintf(KGLOG_ERR, "RL KModel::SeekSplitAnimation(%.8x, %u, %s, %d, %d)\n", 
                p3DModel, Param.nTrackType, Param.szFilePath, Param.dwExtraInfo, Param.nOffsetTime);
        }
    }
	return hrResult;
}

HRESULT KModel::PauseAnimation(IKG3DModel* p3DModel, BOOL bPause)
{
#if MODEL_TRACE
	ATLTRACE("KModel::PauseAnimation(%.8x, %d)\n", p3DModel, bPause);
#endif

    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);

    p3DModel->PauseAnimation(bPause);

#ifdef RL_TRACE_OPTION
    if (g_pRL->m_CurrentTraceOption.bTrace3DAnimation)
    {
        printf("[RL] PauseAnimation(%p, %d)\n", p3DModel, bPause);
    }
#endif  // RL_TRACE_OPTION

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModel::PlaySplitAnimation(IKG3DModel* p3DModel, KRLSplitAnimationParam Param)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    TweenTimeSpan Span;

	KGLOG_PROCESS_ERROR(p3DModel);
    KGLOG_PROCESS_ERROR(Param.szFilePath);

    Span.fTweenWeigth = 1.0f;

    if (Param.nOffsetTime > 0)
    {
        Span.dwTweenIn = 0;
        Span.dwTweenOut = 0;
    }
    else
    {
        Span.dwTweenIn = Param.dwTweenTime;
        Span.dwTweenOut = Param.dwTweenTime;
    }

    if (abs(Param.fSpeed) <= FLT_EPSILON)
        Param.fSpeed = 1.0f;

    ASSERT(Param.dwExtraInfo < ANI_JOINT_COUNT);

    if (!g_pRL->m_DebugOption.bDisableAnimation)
    {
        hr = p3DModel->PlaySplitAnimation(Param.szFilePath, Param.dwSplitType, Param.dwExtraInfo, 
            Param.dwPlayType, Param.fSpeed, 0, (void*)&Span, Param.pUserdata, Param.nTrackType);
        KG_COM_PROCESS_ERROR(hr);

        if (Param.nOffsetTime > 0)
        {
            hr = KModel::SeekSplitAnimation(p3DModel, Param);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

#ifdef RL_TRACE_OPTION
    if (g_pRL->m_CurrentTraceOption.bTrace3DAnimation)
    {
        printf("[RL] PlaySplitAnimation(%p, split:%d, extra:%u, playtype:%u, speed:%f, offsettime:%d, tweentime:%u, 0x%p, track:%u, %s)\n", 
                p3DModel, Param.dwSplitType, Param.dwExtraInfo, Param.dwPlayType, Param.fSpeed, 
                Param.nOffsetTime, Param.dwTweenTime, Param.pUserdata, Param.nTrackType, Param.szFilePath);
    }
#endif // RL_TRACE_OPTION

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (!g_pRL->m_FilePath.MissFile(Param.szFilePath))
        {
            KGLogPrintf(KGLOG_ERR, "RL KModel::PlaySplitAnimation(%.8x, %d, %u, %u, %f, %d, %u, 0x%.8x, %u, %s)\n", 
                p3DModel, Param.dwSplitType, Param.dwExtraInfo, Param.dwPlayType, Param.fSpeed, 
                Param.nOffsetTime, Param.dwTweenTime, Param.pUserdata, Param.nTrackType, Param.szFilePath);
        }
    }
	return hrResult;
}

HRESULT KModel::SetBrightness(IKG3DModel* p3DModel, float fScale)
{
	HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);
	KGLOG_PROCESS_ERROR(fScale >= 0.0f && fScale <= 1.0f);

	p3DModel->SetHighLightLevel(fScale * 2.0f, TRUE);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModel::SetAlpha(IKG3DModel* p3DModel, float fScale)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);
	KGLOG_PROCESS_ERROR(fScale >= 0.0f && fScale <= 1.0f);

	hr = p3DModel->SetAlpha(fScale, TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModel::SetScale(IKG3DModel* p3DModel, float fScale)
{
	return SetScale(p3DModel, D3DXVECTOR3(fScale, fScale, fScale));
}

HRESULT KModel::SetScale(IKG3DModel* p3DModel, D3DXVECTOR3 vScale)
{
    HRESULT hrResult = E_FAIL;

	KG_ASSERT_EXIT(p3DModel);
	
	p3DModel->SetScaling(&vScale);
	
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetOffsetDirection(IKG3DModel* p3DModel, D3DXVECTOR3 vOffset)
{
    HRESULT hrResult = E_FAIL;
    D3DXVECTOR3 vDir(1.0f, 0.0f, 0.0f);

    KG_ASSERT_EXIT(p3DModel);

    p3DModel->AjustWorldDirection(&vOffset, &vDir);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetPosition(IKG3DModel* p3DModel, D3DXVECTOR3 vPosition)
{
    HRESULT hrResult = E_FAIL;

	KG_ASSERT_EXIT(p3DModel);
	
	p3DModel->SetTranslation(&vPosition);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::GetPosition(IKG3DModel* p3DModel, D3DXVECTOR3& vPosition)
{
    HRESULT hrResult = E_FAIL;

    KG_ASSERT_EXIT(p3DModel);
	
	p3DModel->GetTranslation(&vPosition);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetRotation(IKG3DModel* p3DModel, D3DXQUATERNION qRotation)
{
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);
	
	p3DModel->SetRotation(&qRotation);
	
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::GetRotation(IKG3DModel* p3DModel, D3DXQUATERNION& qRotation)
{
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);
	
	p3DModel->GetRotation(&qRotation);
	
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::GetBBox(IKG3DModel* p3DModel, D3DXVECTOR3& vMin, D3DXVECTOR3& vMax)
{
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DModel);

	p3DModel->GetBBox(vMin, vMax);
	
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::AppendRenderEntity(IKG3DScene* p3DScene, IKG3DModel* p3DModel, BOOL bSelectable)
{
#if MODEL_TRACE
	{
		if (bSelectable)
			++s_nSelectableRenderEntityCount;
		else
			++s_nUnselectableRenderEntityCount;

		ATLTRACE("[%.4d %.4d] KModel::AppendRenderEntity(%.8x, %.8x, %s)\n",
			s_nSelectableRenderEntityCount, s_nUnselectableRenderEntityCount, 
			p3DScene, p3DModel, bSelectable ? "TRUE" : "FALSE");
	}
#endif

	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	KGLOG_PROCESS_ERROR(p3DScene);

	hr = p3DScene->AddRenderEntity(p3DModel, SCENEENTITY_MODEL, bSelectable, TRUE);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModel::RemoveRenderEntity(IKG3DScene* p3DScene, IKG3DModel* p3DModel, BOOL bSelectable)
{
#if MODEL_TRACE
	{
		ATLTRACE("[%.4d %.4d] KModel::RemoveRenderEntity(%.8x, %.8x, %s)\n",
			s_nSelectableRenderEntityCount, s_nUnselectableRenderEntityCount, 
			p3DScene, p3DModel, bSelectable ? "TRUE" : "FALSE");

		if (bSelectable)
			--s_nSelectableRenderEntityCount;
		else
			--s_nUnselectableRenderEntityCount;
	}
#endif

    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DScene);

    hr = p3DScene->RemoveRenderEntity(p3DModel, SCENEENTITY_MODEL, bSelectable);
	KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KModel::SetMonoAnimationSpeed(IKG3DModel* p3DModel, float fSpeed)
{
    HRESULT hrResult = E_FAIL;
    IKG3DAnimationController* pController = NULL;

    KGLOG_PROCESS_ERROR(p3DModel);

    ASSERT(fSpeed > 0.0f);

    pController = p3DModel->GetAnimationController(ANICTL_PRIMARY);
    KGLOG_PROCESS_ERROR(pController);

    pController->SetSpeed(fSpeed);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetSplitAnimationSpeed(IKG3DModel* p3DModel, DWORD dwPartIndex, float fSpeed)
{
    struct SplitInfo
    {
        DWORD dwPartIndex;
        DWORD dwControllerIndex;
    };
    
    struct SplitInfoTable
    {
        size_t uNumInfo;
        SplitInfo *pSplitInfo;
    };

    static SplitInfo s_TopInfo[] =
	{
        { ANI_JOINT_TOP, JOINT_TOPINFO_ANI_TOP_INDEX }
    };

    static SplitInfo s_BottomInfo[] =
	{
        { ANI_JOINT_BOTTOM, JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX },
        { ANI_JOINT_TOP, JOINT_TOPINFO_ANI_BOTTOM_INDEX }
    };
    
    static SplitInfoTable s_InfoTable[] = 
	{ 
        { _countof(s_TopInfo), s_TopInfo },
        { _countof(s_BottomInfo), s_BottomInfo }
	};

    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DAnimationController* pController = NULL;

    KGLOG_PROCESS_ERROR(p3DModel);

    for (size_t i = 0; i < s_InfoTable[dwPartIndex].uNumInfo; ++i)
    {
        pController = p3DModel->GetSplitAnimationContorller(
			s_InfoTable[dwPartIndex].pSplitInfo[i].dwPartIndex, 
			s_InfoTable[dwPartIndex].pSplitInfo[i].dwControllerIndex);

        if (pController == NULL)
            continue;

        pController->SetSpeed(fSpeed);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetCaptionVisible(IKG3DModel* p3DModel, DWORD dwIndex, BOOL bVisible)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    hr = p3DModel->SetCaptionVisible(dwIndex, bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetCaption(IKG3DModel* p3DModel, DWORD dwIndex, LPCSTR szContent)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    hr = p3DModel->SetCaption(dwIndex, szContent);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetCaptionColor(IKG3DModel* p3DModel, DWORD dwIndex, D3DCOLORVALUE const& color)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    hr = p3DModel->SetCaptionColor(dwIndex, color);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetCaptionFloorHeight(IKG3DModel* p3DModel, float fHeight)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    p3DModel->SetCaptionFloorHeight(fHeight);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetPercentage(IKG3DModel* p3DModel, float fPercentage)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    hr = p3DModel->SetPercentage(fPercentage);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetPercentageColor(IKG3DModel* p3DModel, D3DCOLORVALUE const& color)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    hr = p3DModel->SetPercentageStyle(color);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetPercentageVisible(IKG3DModel* p3DModel, BOOL bVisible)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    hr = p3DModel->SetPercentageVisible(bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetDetail(IKG3DModel* p3DModel, int nDefineTable, int nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    hr = p3DModel->SetDetail(nDefineTable, nIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KModel::SetBoneTransform(IKG3DModel* p3DModel, LPCSTR szBone, BoneTransformationInputData const& Data)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    hr = p3DModel->SetBoneTransform(szBone, Data);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KModel::SetBoneTransform(0x%.8x, %s)\n", p3DModel, szBone);
    }
    return hrResult;
}

HRESULT KModel::EnableBoneTransform(IKG3DModel* p3DModel, BOOL bEnable)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(p3DModel);

    p3DModel->EnableBoneTransform(bEnable);

    hrResult = S_OK;
Exit0:
    return hrResult;
}



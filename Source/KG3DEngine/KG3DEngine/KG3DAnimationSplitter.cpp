#include "StdAfx.h"
#include "KG3DAnimationSplitter.h"
#include "KG3DEngineManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DAnimationTagContainer.h"
#include "KG3DClipTable.h"
#include "KG3DClip.h"
#include "KG3DClipTools.h"
#include "KG3DModelST.h"

const TCHAR* KG3DAnimationSplitter::s_strConfigFile = "Data\\public\\animationJointInfo\\config.ini";
BOOL KG3DAnimationSplitter::m_bConfigLoaded = FALSE;
std::vector<string> KG3DAnimationSplitter::m_PresetPathName;

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DAnimationSplitter::KG3DAnimationSplitter(void)
{
	m_pModel = NULL;
	m_uCurrentPreset = 0xffffffff;
}

KG3DAnimationSplitter::~KG3DAnimationSplitter(void)
{
	m_pModel = NULL;
}

void KG3DAnimationSplitter::UnInit()
{
	m_Warper.UnInit();
}

void KG3DAnimationSplitter::SetModel(KG3DModel* pModel)
{
    KG_PROCESS_SUCCESS(m_pModel == pModel);

	m_pModel = pModel;
	if (m_pModel)
	{
		m_Warper.SetModel(m_pModel);
	}
Exit1:
    return;
}

BOOL IsAniLoaded(enuModelPlayAnimationType  AniType,
				 KG3DAnimationTagContainer* pTagInput,
				 KG3DClip*& pClipInput)
{
	switch (AniType)
	{
	case MPAT_NORMAL:
		{
			if(pClipInput && pClipInput->IsLoad())
				return TRUE;
		}
		break;
	case MPAT_TAGGED:
		{
			if(pTagInput && pTagInput->IsLoaded())
			{
				if(!pClipInput)
				{
					LPCSTR strTagAnimationFilePath = NULL;
					strTagAnimationFilePath = pTagInput->GetAnimationFilePath();

					HRESULT hr = g_cClipTable.LoadResourceFromFile(strTagAnimationFilePath, 0, 0, &pClipInput);
					KGLOG_COM_PROCESS_ERROR(hr);
				}
				
				if (pClipInput && pClipInput->IsLoad())
				{
					return TRUE;
				}
			}
		}
		break;
	}
Exit0:
	return FALSE;
}
HRESULT GetAniResource(LPCSTR strAnimationName,
					   enuModelPlayAnimationType&  AniType,
					   KG3DAnimationTagContainer*& pTagInput,
                       KG3DClip*& pClipInput,
                       BOOL IsFirstAni)
{
	HRESULT hr = E_FAIL;
    unsigned uClipLoadOption = 0;

	KGLOG_PROCESS_ERROR(strAnimationName);
	KG_PROCESS_ERROR(strAnimationName[0] != '\0');

    if (IsFirstAni)
    {
        uClipLoadOption = MLO_ANI_MULTI;
    }

	AniType = g_cGraphicsTool.GetAnimationTypeByFileName(strAnimationName);
	switch (AniType)
	{
	case MPAT_NORMAL:
		{
			hr = g_cClipTable.LoadResourceFromFile(strAnimationName, 0, uClipLoadOption, &pClipInput);
			KG_COM_PROCESS_ERROR(hr);
		}
		break;
	case MPAT_TAGGED:
		{
			LPCSTR strTagAnimationFilePath = NULL;
		
			pTagInput = g_tagConTable.LoadTag(strAnimationName); //g_cClipTable.GetTagTable()->LoadTag(strAnimationName);
			KGLOG_PROCESS_ERROR(pTagInput);

			if(pTagInput && pTagInput->IsLoaded())
			{
				strTagAnimationFilePath = pTagInput->GetAnimationFilePath();

				hr = g_cClipTable.LoadResourceFromFile(strTagAnimationFilePath, 0, uClipLoadOption, &pClipInput);
				KGLOG_COM_PROCESS_ERROR(hr);
			}
		}
		break;
	}
	hr = S_OK;
Exit0:
	return hr;

}

HRESULT KG3DAnimationSplitter::PlayAnimation(LPCSTR strAnimationName, 
											 unsigned int uPresetIndex,
											 DWORD dwSplitType,
											 DWORD dwPlayType,
											 FLOAT fSpeed,
											 int   nOffsetTime,
											 PVOID pReserved,
											 PVOID pUserdata,
											 enuAnimationControllerPriority Priority,
											 enuModelPlayAnimationType Type,
											 KG3DAnimationTagContainer* pTagContainerNew,
											 KG3DClip* pClip)
{
    HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	DWORD dwStartTime = timeGetTime();

	if (pReserved)
	{
		m_Warper.m_TweenSpan[dwSplitType] = *(TweenTimeSpan*)(pReserved);
	}
	else
	{
		m_Warper.m_TweenSpan[dwSplitType].dwTweenIn = 0;
		m_Warper.m_TweenSpan[dwSplitType].dwTweenOut = 0;
		m_Warper.m_TweenSpan[dwSplitType].fTweenWeigth = 1.0f;
	}
	switch (m_Warper.m_ControllerPriority[dwSplitType])
	{
	case ANICTL_PRIMARY:
		{
			switch (Priority)
			{
			case ANICTL_PRIMARY:
				//如果当前的优先级是高， 新的也是高， 那么替代掉高的
				hrRetCode = PlayAnimationChange(strAnimationName,
					uPresetIndex,
					dwSplitType,
					dwPlayType,
					fSpeed,
					nOffsetTime,
					pReserved,
					pUserdata,
					Type,pTagContainerNew,pClip);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);
				m_Warper.m_ControllerPriority[dwSplitType] = ANICTL_PRIMARY;
				break;
			case ANICTL_SECONDARY:
				//如果当前的优先级是低， 新的是高， 那么把当前的移到低轨， 并且在高轨播放新的
				hrRetCode = MoveCurrentToSecondaryAndPlay(strAnimationName,
					uPresetIndex,
					dwSplitType,
					dwPlayType,
					fSpeed,
					nOffsetTime,
					pReserved,
					pUserdata,
                    Type,pTagContainerNew,pClip);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);
				m_Warper.m_ControllerPriority[dwSplitType] = ANICTL_SECONDARY;
				break;
			}
		}
		break;
	case ANICTL_SECONDARY:
		{
			switch (Priority)
			{
			case ANICTL_PRIMARY:
				//如果当前的优先级是高， 新的是低， 那么在低轨播放
				hrRetCode= PlayAnimationSecondary(strAnimationName,
					dwSplitType,
					dwPlayType,
					fSpeed,
					nOffsetTime,
					pReserved,
					pUserdata,
                    Type,pTagContainerNew,pClip);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);
				m_Warper.m_ControllerPriority[dwSplitType] = ANICTL_SECONDARY;				
				break;
			case ANICTL_SECONDARY:
				//如果当前的优先级是低， 新的也是低， 那么在替换掉当前的低轨的
				hrRetCode = PlayAnimationChange(strAnimationName,
					uPresetIndex,
					dwSplitType,
					dwPlayType,
					fSpeed,
					nOffsetTime,
					pReserved,
					pUserdata,
                    Type,pTagContainerNew,pClip);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);
				m_Warper.m_ControllerPriority[dwSplitType] = ANICTL_SECONDARY;
				break;
			}
		}
		break;
	}
	
    hrResult = S_OK;
Exit0:
	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DAnimationSplitter::PlayAnimation cost %d",dwCost);
	}
	return hrResult;
}


void KG3DAnimationSplitter::OnSecondaryAnimationFinish(KG3DAnimationController *pActiveController)
{
	//处理Secondary播放完成之后的切回
	KG3DAnimationComposer *pComposer = NULL;
	KG3DAnimationController *pController = NULL;
	KG3DAnimationTagContainer *pTagContainerRemove = NULL;
	
	DWORD dwSplitType = pActiveController->GetMotionExtraInfo();
	static DWORD const s_dwContainerMap[] = { 1, //TYPE_NONE
		0, //TYPE_PART0
		1}; //TYPE_PART1
	pTagContainerRemove = m_Warper.m_vecTagContainers[s_dwContainerMap[dwSplitType]];
	if (pTagContainerRemove)
	{
		pActiveController->RemoveAnimationControllerUpdateNotifier(pTagContainerRemove);
	}

	switch (dwSplitType)
	{
	case IKG3DAnimationController::TYPE_PART0:
		{
			KG3DAnimationController ControllerSave;
			m_Warper.GetComposer(JOINT_TOPINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pComposer);
			ASSERT(pComposer);
			pController = pComposer->GetAnimationController(JOINT_TOPINFO_ANI_TOP_INDEX);
			ASSERT(pController);

			//保存当前的作为之后的pStart
			pController->Clone(ControllerSave);
			//复制二号到主控制器
			m_Warper.m_SecondaryControllers[ANI_JOINT_TOP].Clone(*pController);
			pController->m_Priority = ANICTL_PRIMARY;
			m_Warper.ChangeAnimation(JOINT_TOPINFO_COMPOSERINDEX, 
				JOINT_TOPINFO_ANI_TOP_INDEX, 
				pController->GetCurAnimation(), 
				&ControllerSave, 
				m_Warper.m_TweenSpan[ANI_JOINT_TOP].dwTweenOut);
			SAFE_RELEASE(pTagContainerRemove);
			if (m_Warper.m_SecondaryTagContainers[ANI_JOINT_TOP])
			{
				pController->AddAnimationControllerUpdateNotifier(m_Warper.m_SecondaryTagContainers[0]);
			}
			m_Warper.m_SecondaryControllers[ANI_JOINT_TOP].RemoveAnimationControllerUpdateNotifier(m_Warper.m_SecondaryTagContainers[0]);
            m_Warper.m_vecTagContainers[s_dwContainerMap[dwSplitType]] = m_Warper.m_SecondaryTagContainers[0];
			m_Warper.m_SecondaryTagContainers[ANI_JOINT_TOP] = NULL;
			m_Warper.m_SecondaryControllers[ANI_JOINT_TOP].Detach();
			m_Warper.m_ControllerPriority[ANI_JOINT_TOP] = ANICTL_PRIMARY;
		}
		break;
	case IKG3DAnimationController::TYPE_PART1:
		{
			KG3DAnimationController ControllerSaveBottom;
			KG3DAnimationController *pBottomController = NULL;
			//Set bottom animation controller information
			m_Warper.GetComposer(JOINT_BOTTOMINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pComposer);
			ASSERT(pComposer);

			pBottomController = pComposer->GetAnimationController(JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX);
			ASSERT(pBottomController);

			pBottomController->Clone(ControllerSaveBottom);
			m_Warper.m_SecondaryControllers[ANI_JOINT_BOTTOM].Clone(*pBottomController);
			pBottomController->m_Priority = ANICTL_PRIMARY;

			m_Warper.ChangeAnimation(JOINT_BOTTOMINFO_COMPOSERINDEX, 
				JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX, 
				pBottomController->GetCurAnimation(), 
				&ControllerSaveBottom, 
				m_Warper.m_TweenSpan[ANI_JOINT_BOTTOM].dwTweenOut);
			SAFE_RELEASE(pTagContainerRemove);
			m_Warper.m_vecTagContainers[ANI_JOINT_BOTTOM] = NULL;
			m_Warper.m_SecondaryControllers[ANI_JOINT_BOTTOM].Detach();
			m_Warper.m_ControllerPriority[ANI_JOINT_BOTTOM] = ANICTL_PRIMARY;
			
		}
		break;
	case IKG3DAnimationController::TYPE_NONE:
		break;
	default:
		ASSERT(0);
	}
}


HRESULT KG3DAnimationSplitter::PlayAnimationSecondary(LPCSTR strAnimationName, 
													  DWORD dwSplitType,
													  DWORD dwPlayType,
													  FLOAT fSpeed,
													  int  nOffsetTime,
													  PVOID pReserved,
													  PVOID pUserdata,
													  enuModelPlayAnimationType AniType,
													  KG3DAnimationTagContainer* pTagContainerNew,
													  KG3DClip* pClip)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
	KG3DAnimationController &Controller = m_Warper.m_SecondaryControllers[dwSplitType];
    KG3DModelST *pModelST = NULL;
    KG3DBip *pBip = NULL;
	DWORD dwStartTime = timeGetTime();
    BOOL bTagAddRefFlag = FALSE;

	KG_ASSERT_EXIT(strAnimationName);
    KG_ASSERT_EXIT(pClip);

	if (m_Warper.m_SecondaryTagContainers[dwSplitType])
	{
		Controller.RemoveAnimationControllerUpdateNotifier(m_Warper.m_SecondaryTagContainers[dwSplitType]);
		SAFE_RELEASE(m_Warper.m_SecondaryTagContainers[dwSplitType]);
    }

	if (AniType == MPAT_TAGGED)
	{
        KG_ASSERT_EXIT(pTagContainerNew);

        if (dwSplitType == ANI_JOINT_BOTTOM)
        {
            //在这里暂时对禁止掉下半身的标签的功能
            AniType = MPAT_NORMAL;
        }
        else
        {
            pTagContainerNew->AttachToModel(m_pModel);
            m_Warper.m_SecondaryTagContainers[dwSplitType] = pTagContainerNew;
            m_Warper.m_SecondaryTagContainers[dwSplitType]->AddRef();
            bTagAddRefFlag = TRUE;
        }
	}

    ASSERT(m_pModel->GetType() == MESHTYPE_MODELST);

    pModelST = dynamic_cast<KG3DModelST*>(m_pModel);
    KG_PROCESS_ERROR(pModelST);

    pBip = pModelST->GetBip();
    KG_PROCESS_ERROR(pBip);

    hrRetCode = GetClipTool().CheckClipByBip(pClip, pBip);
    KG_COM_PROCESS_ERROR(hrRetCode);

	Controller.SetUserdata(pUserdata);
	Controller.StartAnimation(dwPlayType, nOffsetTime, fSpeed);
	Controller.Attach(pClip);

	if (AniType == MPAT_TAGGED)
	{
        KG3DAnimationTagContainer *pTag = m_Warper.m_SecondaryTagContainers[dwSplitType];
        Controller.AddAnimationControllerUpdateNotifier(pTag);
	}

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bTagAddRefFlag)
        {
            KG_COM_RELEASE(m_Warper.m_SecondaryTagContainers[dwSplitType]);
            bTagAddRefFlag = FALSE;
        }
    }

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DAnimationSplitter::PlayAnimationSecondary cost %d",dwCost);
	}
	return hrResult;
}

HRESULT KG3DAnimationSplitter::MoveCurrentToSecondaryAndPlay(LPCSTR strAnimationName, 
															 unsigned int uPresetIndex,
															 DWORD dwSplitType,
															 DWORD dwPlayType,
															 FLOAT fSpeed,
															 int   nOffsetTime,
															 PVOID pReserved,
															 PVOID pUserdata,
															 enuModelPlayAnimationType AniType,
															 KG3DAnimationTagContainer* pTagContainerNew,
															 KG3DClip* pClip)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode  = E_FAIL;
    KG3DAnimationTagContainer *pTagContainerSrc = NULL;
    KG3DAnimationController *pController = NULL;
    KG3DAnimationComposer *pComposer = NULL;

	KG_ASSERT_EXIT(strAnimationName);
    KG_ASSERT_EXIT(pClip);
	
	static DWORD const s_dwControllerMap[] = { 1, 0 };

	hrRetCode = m_Warper.GetComposer(dwSplitType, (IEKG3DAnimationComposer**)&pComposer);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	pController = pComposer->GetAnimationController(s_dwControllerMap[dwSplitType]);
	KG_PROCESS_ERROR(pController);

	pController->m_Priority = ANICTL_SECONDARY;
	
	pTagContainerSrc = m_Warper.m_vecTagContainers[dwSplitType];
	if (pTagContainerSrc)
	{
		pController->RemoveAnimationControllerUpdateNotifier(pTagContainerSrc);
        KG3DAnimationTagContainer *pSecondaryTagContainer = m_Warper.m_SecondaryTagContainers[dwSplitType];
		if (pSecondaryTagContainer)
		{
			m_Warper.m_SecondaryControllers[dwSplitType].RemoveAnimationControllerUpdateNotifier(pSecondaryTagContainer);
		}
		SAFE_RELEASE(m_Warper.m_SecondaryTagContainers[dwSplitType]);
		m_Warper.m_SecondaryTagContainers[dwSplitType] = pTagContainerSrc;
		m_Warper.m_vecTagContainers[dwSplitType] = NULL;
		m_Warper.m_SecondaryControllers[dwSplitType].AddAnimationControllerUpdateNotifier(pTagContainerSrc);
	}
	pController->Clone(m_Warper.m_SecondaryControllers[dwSplitType]);
	
	hrRetCode = PlayAnimationChange(strAnimationName, 
		uPresetIndex, 
		dwSplitType, 
		dwPlayType, 
		fSpeed, 
		nOffsetTime, 
		pReserved, 
		pUserdata,
		AniType,pTagContainerNew,pClip);
	KG_COM_PROCESS_ERROR(hrRetCode);

	hrResult = S_OK;
Exit0:
	return hrResult;
}


HRESULT KG3DAnimationSplitter::PlayAnimationChange(LPCSTR strAnimationName, 
										  unsigned int uPresetIndex,
										  DWORD dwSplitType,
										  DWORD dwPlayType,
										  FLOAT fSpeed,
										  int nOffsetTime,
										  PVOID pReserved,
										  PVOID pUserdata,
										  enuModelPlayAnimationType AniType,
										  KG3DAnimationTagContainer* pTagContainerNew,
										  KG3DClip* pClip)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	KG3DAnimationComposer *pComposer = NULL;
	KG3DAnimationController *pController = NULL;
    KG3DAnimationTagContainer *pTagContainerRemove = NULL;
    KG3DModelST *pModelST = NULL;
    KG3DBip *pBip = NULL;
	DWORD dwStartTime = timeGetTime();
	BOOL bTagAddRefFlag = FALSE;
	
	KG_ASSERT_EXIT(strAnimationName);
	KG_ASSERT_EXIT(uPresetIndex < m_PresetPathName.size());
    KG_ASSERT_EXIT(pClip);

	if (uPresetIndex != m_uCurrentPreset)
	{
		hrRetCode = LoadPreset(uPresetIndex);
		KG_COM_PROCESS_ERROR(hrRetCode);
		m_uCurrentPreset = uPresetIndex;
	}
	m_Warper.Enable(TRUE);

	pTagContainerRemove = m_Warper.m_vecTagContainers[dwSplitType];
	if (pTagContainerRemove)
	{
		static DWORD const s_dwControllerMap[] = { 1, 0 };
		hrRetCode = m_Warper.GetComposer(dwSplitType, (IEKG3DAnimationComposer**)&pComposer);
		KG_COM_PROCESS_ERROR(hrRetCode);

		pController = pComposer->GetAnimationController(s_dwControllerMap[dwSplitType]);
		KG_PROCESS_ERROR(pController);

		pController->RemoveAnimationControllerUpdateNotifier(pTagContainerRemove);

		SAFE_RELEASE(pTagContainerRemove);
		m_Warper.m_vecTagContainers[dwSplitType] = NULL;
	}

	if (AniType == MPAT_TAGGED)
    {
        KG_ASSERT_EXIT(pTagContainerNew);

        if (dwSplitType == ANI_JOINT_BOTTOM)
        {
	        //在这里暂时禁止掉下半身的标签的功能
	        AniType = MPAT_NORMAL;
        }
        else
        {
	        pTagContainerNew->AttachToModel(m_pModel);
	        m_Warper.m_vecTagContainers[dwSplitType] = pTagContainerNew;
	        m_Warper.m_vecTagContainers[dwSplitType]->AddRef();
            bTagAddRefFlag = TRUE;
        }
	}

    ASSERT(m_pModel->GetType() == MESHTYPE_MODELST);

    pModelST = dynamic_cast<KG3DModelST*>(m_pModel);
    KG_PROCESS_ERROR(pModelST);

    pBip = pModelST->GetBip();
    KG_PROCESS_ERROR(pBip);

    hrRetCode = GetClipTool().CheckClipByBip(pClip, pBip);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	switch (dwSplitType)
	{
	case ANI_JOINT_TOP:
		{
			KG3DAnimationController ControllerSaveTop;
			m_Warper.GetComposer(JOINT_TOPINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pComposer);
			ASSERT(pComposer);

			pController = pComposer->GetAnimationController(JOINT_TOPINFO_ANI_TOP_INDEX);
			ASSERT(pController);

			pController->Clone(ControllerSaveTop);
			pController->SetUserdata(pUserdata);
			pController->StartAnimation(dwPlayType, nOffsetTime, fSpeed);

			if (AniType == MPAT_TAGGED)
				pController->AddAnimationControllerUpdateNotifier(pTagContainerNew);

			m_Warper.ChangeAnimation(JOINT_TOPINFO_COMPOSERINDEX, 
				JOINT_TOPINFO_ANI_TOP_INDEX, 
				pClip, 
				&ControllerSaveTop, 
				m_Warper.m_TweenSpan[dwSplitType].dwTweenIn);
		}
		break;
	case ANI_JOINT_BOTTOM:
		{
			KG3DAnimationController ControllerSaveBottom;
			m_Warper.GetComposer(JOINT_BOTTOMINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pComposer);
			ASSERT(pComposer);

			pController = pComposer->GetAnimationController(JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX);
			ASSERT(pController);

			pController->Clone(ControllerSaveBottom);
			pController->SetUserdata(pUserdata);
			pController->StartAnimation(dwPlayType, nOffsetTime, fSpeed);

			if (AniType == MPAT_TAGGED)
                pController->AddAnimationControllerUpdateNotifier(pTagContainerNew);

            m_Warper.ChangeAnimation(JOINT_BOTTOMINFO_COMPOSERINDEX, 
                JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX, 
                pClip, 
                &ControllerSaveBottom, 
                m_Warper.m_TweenSpan[dwSplitType].dwTweenIn);
		}
		break;
	default:
		ASSERT(0);
	}

	m_Warper.GetComposer(JOINT_TOPINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pComposer);
	ASSERT(pComposer);
	pController = pComposer->GetAnimationController(JOINT_TOPINFO_ANI_TOP_INDEX);
	ASSERT(pController);
	pController->SetMotionExtraInfo(IKG3DAnimationController::TYPE_PART0);

	m_Warper.GetComposer(JOINT_BOTTOMINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pComposer);
	ASSERT(pComposer);
	pController = pComposer->GetAnimationController(JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX);
	ASSERT(pController);
	pController->SetMotionExtraInfo(IKG3DAnimationController::TYPE_PART1);

	hrResult = S_OK;
Exit0:
	if (FAILED(hrResult))
	{
        if (bTagAddRefFlag)
        {
            KG_COM_RELEASE(m_Warper.m_vecTagContainers[dwSplitType]);
            bTagAddRefFlag = FALSE;
        }
	}

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize KG3DAnimationSplitter::PlayAnimationChange cost %d",dwCost);
	}
	return hrResult;
}

extern KG3DAnimationWarperTable g_cAnimationWarperTable;

HRESULT KG3DAnimationSplitter::LoadPreset(unsigned int uIndex)
{
	HRESULT hr = E_FAIL;
	TCHAR strFullPath[MAX_PATH];
	KG_PROCESS_ERROR(uIndex < m_PresetPathName.size());
	sprintf_s(strFullPath, 
		MAX_PATH,
		"%s",
		m_PresetPathName[uIndex].c_str());
	hr = g_cAnimationWarperTable.LoadWarper(&m_Warper,strFullPath);
	//hr = m_Warper.Load(strFullPath);
Exit0:
	return hr;
}


HRESULT KG3DAnimationSplitter::LoadConfig()
{
	HRESULT hResult = E_FAIL;
	TCHAR strConfigFilePath[MAX_PATH];
	TCHAR strSectionName[MAX_PATH];

	IIniFile *pConfig = NULL;
	int nNumSection = 0;
	KG_PROCESS_SUCCESS(m_bConfigLoaded);

	m_PresetPathName.clear();

	pConfig = g_OpenIniFile(s_strConfigFile);
	KG_PROCESS_ERROR(pConfig);

	pConfig->GetInteger("Header", "NumSection", 0, &nNumSection);
	
	m_PresetPathName.resize(nNumSection);

	for (int i = 0; i < nNumSection; i++)
	{
		sprintf_s(strSectionName,
			MAX_PATH,
			"Section%d",
			i);
		pConfig->GetString(strSectionName, 
			"File", 
			"", 
			strConfigFilePath, 
			MAX_PATH);
		m_PresetPathName[i] = strConfigFilePath;
	}
	m_bConfigLoaded = TRUE;

Exit1:
	hResult = S_OK;
Exit0:
	KG_COM_RELEASE(pConfig);
	if (FAILED(hResult))
	{
		KGLogPrintf(KGLOG_ERR, "KG3DAnimationSplitter load configuration file %s failed.", s_strConfigFile);
	}
	return hResult;
}

BOOL KG3DAnimationSplitter::GetEnable()
{
	return m_Warper.GetEnable();
}

HRESULT KG3DAnimationSplitter::FrameMove(D3DXMATRIX *pResult)
{
	HRESULT hRetCode = E_FAIL;

	if (m_Warper.GetEnable())
	{
		hRetCode = m_Warper.FrameMove(pResult);
	}
	return hRetCode;
}

KG3DAnimationWarper* KG3DAnimationSplitter::GetAnimationWarper()
{
	return &m_Warper;
}

void KG3DAnimationSplitter::EnableWarper(BOOL bEnable)
{
	m_Warper.Enable(bEnable);
}

void KG3DAnimationSplitter::SeekAnimation(DWORD dwSplitType, 
										  enuAnimationControllerPriority nPrority,
										  DWORD dwSeekType, 
										  int nOffset)
{
	switch(nPrority)
	{
	case ANICTL_PRIMARY:
		{
			KG3DAnimationComposer *pComposer = NULL;
			KG3DAnimationController *pController = NULL;
			switch (dwSplitType)
			{
			case ANI_JOINT_TOP:
				{
					m_Warper.GetComposer(JOINT_TOPINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pComposer);
					ASSERT(pComposer);
					KGLOG_PROCESS_ERROR(pComposer);

					pController = pComposer->GetAnimationController(JOINT_TOPINFO_ANI_TOP_INDEX);
					KGLOG_PROCESS_ERROR(pController);

					ASSERT(pController);
					pController->SeekAnimation(dwSeekType, nOffset);
				}
				break;
			case ANI_JOINT_BOTTOM:
				{
					m_Warper.GetComposer(JOINT_BOTTOMINFO_COMPOSERINDEX, (IEKG3DAnimationComposer **)&pComposer);
					ASSERT(pComposer);
					KGLOG_PROCESS_ERROR(pComposer);

					pController = pComposer->GetAnimationController(JOINT_BOTTOMINFO_ANI_BOTTOM_INDEX);
					KGLOG_PROCESS_ERROR(pController);

					ASSERT(pController);
					pController->SeekAnimation(dwSeekType, nOffset);

				}
				break;
			}
		}
		break;
	case ANICTL_SECONDARY:
		{
			m_Warper.m_SecondaryControllers[dwSplitType].SeekAnimation(dwSeekType, nOffset);
		}
		break;
	}
Exit0:
	;
}

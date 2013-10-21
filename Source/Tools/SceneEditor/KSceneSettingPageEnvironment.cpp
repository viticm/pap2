// KSceneSettingPageEnvironment.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageEnvironment.h"
#include "IEKG3DEnvironment.h"
//#include "KSceneInterfaceGetters.h"
#include "KSceneSceneHelpers.h"
#include "KSceneSettingDialogAddEnvEffect.h"

#include "IEKG3DCollocator.h"
#include "IEEditorTypes.h"
//#include "KGHelperClasses.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// KSceneSettingPageEnvironment dialog

IMPLEMENT_DYNAMIC(KSceneSettingPageEnvironment, CPropertyPage)




KSceneSettingPageEnvironment::KSceneSettingPageEnvironment()
	: KSceneSettingPageBase(KSceneSettingPageEnvironment::IDD)
	//, m_EffList(KGCommonListHelperEnv())
{
	//m_pEnvironment = NULL;
}

KSceneSettingPageEnvironment::~KSceneSettingPageEnvironment()
{
}

void KSceneSettingPageEnvironment::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SCENESETTING_ENV, m_EffList);
	DDX_Control(pDX, IDC_SS_CK_ENABLE_ENV, m_bnIsEnable);
	DDX_Control(pDX, IDC_SS_CK_ENABLE_CONTROL_BY_DW, m_cbControlByDW);
}

/*
HRESULT KSceneSettingPageEnvironment::SetSceneEditor( IEKG3DScene* pEdit )
{
	if (!pEdit)
	{
		return E_FAIL;
	}
	m_lpSceneEditor = pEdit;
	return S_OK;
}*/

BEGIN_MESSAGE_MAP(KSceneSettingPageEnvironment, CPropertyPage)
ON_LBN_SELCHANGE(IDC_LIST_SCENESETTING_ENV, &KSceneSettingPageEnvironment::OnLbnSelchangeListScenesettingEnv)
	ON_BN_CLICKED(IDC_SS_CK_ENABLE_ENV, &KSceneSettingPageEnvironment::OnBnClickedCkEnableScenesettingEnv)
	ON_BN_CLICKED(IDC_BN_ADD_SCENESETTING_ENV, &KSceneSettingPageEnvironment::OnBnClickedBnAddScenesettingEnv)
	ON_BN_CLICKED(IDC_BN_DEL_SCENESETTING_ENV2, &KSceneSettingPageEnvironment::OnBnClickedBnDelScenesettingEnv2)
	ON_BN_CLICKED(IDC_SS_CK_ENABLE_CONTROL_BY_DW, &KSceneSettingPageEnvironment::OnBnClickedCkSsEnableControlByDw)
	ON_BN_CLICKED(IDC_BUTTON1, &KSceneSettingPageEnvironment::OnBnClickedSaveAs)
	ON_BN_CLICKED(IDC_BUTTON2, &KSceneSettingPageEnvironment::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_BUTTON3, &KSceneSettingPageEnvironment::OnBnClickedSave)
END_MESSAGE_MAP()


// KSceneSettingPageEnvironment message handlers

BOOL KSceneSettingPageEnvironment::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	m_bnIsEnable.SetCheck(BST_UNCHECKED);
	m_cbControlByDW.SetCheck(BST_UNCHECKED);
	ReloadList();


#if defined(DEBUG) | defined(_DEBUG)
	{
		KG3DEngineCaps Caps;
		HRESULT hr = g_pEngineManager->GetEngineCaps(&Caps);
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(Caps.IsShadowTypeAvailable[EM_MODEL_SHADOW_TYPE_LOW] && _T("设备能力信息错误"));
		for (size_t i = 0; i < EM_MODEL_SHADOW_TYPE_COUNT; i++)
		{
			_ASSERTE(Caps.IsShadowTypeAvailable[i] && _T("设备能力信息错误"));
		}
	}
#endif

	return CPropertyPage::OnSetActive();
}

VOID KSceneSettingPageEnvironment::ReloadList()
{

	IEKG3DEnvironment* pEnv = KSCENEEDITOR_HELPERCLASSES::GetEngineInterface<KG3DTYPE_ENVIRONMENT>();
	//KG3DENVEFF_INFO EnvEffInfoArray[KG3DENVEFF_MAX];
	KG_PROCESS_ERROR(pEnv);
	
	{
		/*INT nElementCount = 0;
		KG_COM_PROCESS_ERROR(pEnv->GetTopEnvEffectCount(&nElementCount));*/
		//_ASSERTE(nElementCount >= 0);
		m_EffList.ResetContent();
		m_EnvEffDescArray.clear();
		CString TempString;
		INT nAddedEnvEff = 0;
		HRESULT hr = E_FAIL;

		//for(IEKG3DEnvEffect* pEnvEffect = pEnv->ResetIterator(); NULL != pEnvEffect; pEnvEffect = pEnv->NextIterator())
		IEKG3DEnvEffect* pEnvEffect = NULL;
		KGCH::IteratorHolder itHolder;
		for(hr = pEnv->GetFirstTopEnvEffect(itHolder, &pEnvEffect)
			; SUCCEEDED(hr)
			; hr = pEnv->GetNextTopEnvEffect(itHolder, &pEnvEffect))
		{
			KG3DENVEFF_HANDLE RootHandle = KG3DENVEFF_HANDLE(pEnvEffect->GetType(), 0);
			
			_ASSERTE(NULL != pEnvEffect && RootHandle.m_dwType != KG3DTYPE_ENVEFF_INVALID_EFFECT);
			INT hItem = AddEnvEffectToList(pEnvEffect, RootHandle, 0);
			if (hItem < 0)
			{
				_KG3D_DEBUG_REPORT("KSceneSettingPageEnvironment::ReloadList失败，很不正常，要检查\n");
				continue;
			}
			int nChildCount = 0;
			hr = pEnvEffect->GetSubEffectCount(&nChildCount);
			_KG3D_DEBUG_FAIL_REPORT(SUCCEEDED(hr));
			if (FAILED(hr) || nChildCount <= 0)
			{
				continue;	
			}
			_ASSERTE(nChildCount < KG3DENVEFF_MAX);
			for (int i = 0; i < nChildCount; i++ )
			{
				IEKG3DEnvEffect* pEnvSubEffect = NULL;
				KG3DENVEFF_HANDLE TempHandle;
				hr = pEnvEffect->GetSubEffect(i, &pEnvSubEffect, &TempHandle);
				if (SUCCEEDED(hr))
				{
					AddEnvEffectToList(pEnvSubEffect, TempHandle, hItem);
				}
			}

			
		}
		m_EffList.SetCurSel(0);
		SynFromInnerToInterface();
	}
	
Exit0:
	return;
	

}
INT KSceneSettingPageEnvironment::AddEnvEffectToList( IEKG3DEnvEffect* pEnvEffect, KG3DENVEFF_HANDLE hHandle, INT hItem )
{
	_ASSERTE(NULL != pEnvEffect);
#if defined(DEBUG) | defined(_DEBUG)
	{
		DWORD dwType = pEnvEffect->GetType();
		_ASSERTE(dwType == hHandle.m_dwType);
	}
#endif
	LPCTSTR tczTemp = KSH::GetRTTITypeName(hHandle.m_dwType);
	if (!tczTemp)
	{
		_KG3D_DEBUG_REPORT(":无法得到Effect名字\n");
		return -1;
	}
	CString TempString;
	TempString.Format(_T("%s\t%d"), tczTemp, hHandle.m_nIndex);
	int nAddedIndex = m_EffList.AddString((LPCTSTR)TempString);
	if (nAddedIndex >= 0)
	{
		size_t uOldSize = m_EnvEffDescArray.size();
		m_EnvEffDescArray.push_back(hHandle);
		size_t uNewSize = m_EnvEffDescArray.size();
		if (uNewSize > uOldSize)
		{
			m_EffList.SetItemData(nAddedIndex, _kg_to_bigger_size_cast<DWORD_PTR>(uNewSize - 1));
			return nAddedIndex;
		}	
	}
	return -1;
}

BOOL KSceneSettingPageEnvironment::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class
	//m_pEnvironment = NULL;
	return CPropertyPage::OnKillActive();
}

void KSceneSettingPageEnvironment::OnLbnSelchangeListScenesettingEnv()
{
	// TODO: Add your control notification handler code here
	SynFromInnerToInterface();
}

/*
VOID KSceneSettingPageEnvironment::UpdataInterface( BOOL bToInterface / *TRUE* /)
{
	if (bToInterface)
	{
		SynFromInnerToInterface();
		return;
	}
	SynFromInterfaceToInner();
	return;
}*/


HRESULT KSceneSettingPageEnvironment::SynFromInterfaceToInner(DWORD_PTR dwOption/* = 0*/)
{
	IEKG3DEnvEffect* pEnvEff = GetSelectedEffect();
	KG_PROCESS_ERROR(pEnvEff);
	KG_COM_PROCESS_ERROR(pEnvEff->Enable(m_bnIsEnable.GetCheck() == BST_CHECKED));
	KG_COM_PROCESS_ERROR(pEnvEff->EnableControlByDynamicWeather(m_cbControlByDW.GetCheck() == BST_CHECKED));
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KSceneSettingPageEnvironment::SynFromInnerToInterface(DWORD_PTR dwOption /*= 0*/)
{
	IEKG3DEnvEffect* pEnvEff = GetSelectedEffect();
	KG_PROCESS_ERROR(pEnvEff);
	{
		///同步Enable按钮
		BOOL bEnable = FALSE;
		BOOL bEnableControlByDW = FALSE;
		if (SUCCEEDED(pEnvEff->IsEnable(&bEnable)))
		{
			KSCENEEDITOR_HELPERCLASSES::SynACButtonWithABool(m_bnIsEnable, bEnable);
		}
		if (SUCCEEDED(pEnvEff->IsEnableControlByDynamicWeather(&bEnableControlByDW)))
		{
			KSCENEEDITOR_HELPERCLASSES::SynACButtonWithABool(m_cbControlByDW, bEnableControlByDW);
		}	
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
void KSceneSettingPageEnvironment::OnBnClickedCkEnableScenesettingEnv()
{
	// TODO: Add your control notification handler code here
	SynFromInterfaceToInner();
	SynFromInnerToInterface();
}
IEKG3DEnvEffect* KSceneSettingPageEnvironment::GetSelectedEffect()
{
	//IEKG3DEnvironment* pEnv = (IEKG3DEnvironment*)KSCENEEDITOR_HELPERCLASSES::GetEngineInterface(KG3DENVIRONMENT);
	IEKG3DEnvEffect* pEnvEff = NULL;
	int nSelIndex = m_EffList.GetCurSel();
	if (nSelIndex >= 0)
	{
		size_t uIndex = _kg_to_smaller_size_cast<size_t>(m_EffList.GetItemData(nSelIndex));
		_ASSERTE(uIndex >=0 && uIndex < m_EnvEffDescArray.size());
		return KSCENEEDITOR_HELPERCLASSES::GetEngineAbstractInterface<KG3DTYPE_ENV_EFFECT>(m_EnvEffDescArray[uIndex].m_dwType, m_EnvEffDescArray[uIndex].m_nIndex);
	}
//Exit0:
	return NULL;
}
void KSceneSettingPageEnvironment::OnBnClickedBnAddScenesettingEnv()
{
	// TODO: Add your control notification handler code here
	KSceneSettingDialogAddEnvEffect TempAddEnvEffect;
	DWORD dwType = 0;
	if (IDOK == TempAddEnvEffect.DoModal())
	{
		dwType = TempAddEnvEffect.GetResultType();
		IEKG3DEnvironment* pEnv = KSCENEEDITOR_HELPERCLASSES::GetEngineInterface<KG3DTYPE_ENVIRONMENT>();
		KG_PROCESS_ERROR(pEnv);
		KG_COM_PROCESS_ERROR(pEnv->NewEnvEffect(dwType, NULL));
		ReloadList();
	}
	return;
Exit0:
	if (dwType)
	{
		LPCTSTR tczName = KSCENEEDITOR_HELPERCLASSES::GetRTTITypeName(dwType);
		CString TempCString;
		TempCString.Format(_T("以下类型没有办法被添加：%s。如果是远山、天空盒和云的话，\
							  它们是从属于天气系统的，所以请先添加SkySystem, 然后到对应的面板添加。"), tczName);
		MessageBox((LPCTSTR)TempCString, _T("提示"), MB_OK | MB_ICONINFORMATION);
	}
	return;

}

void KSceneSettingPageEnvironment::OnBnClickedBnDelScenesettingEnv2()
{
	// TODO: Add your control notification handler code here
	IEKG3DEnvEffect* pEffect = GetSelectedEffect();
	DWORD dwType = 0;
	KG_PROCESS_ERROR(pEffect);
	{
		dwType = pEffect->GetType();
		IEKG3DEnvironment* pEnv = KSCENEEDITOR_HELPERCLASSES::GetEngineInterface<KG3DTYPE_ENVIRONMENT>();
		KG_PROCESS_ERROR(pEnv);
		KG_COM_PROCESS_ERROR(pEnv->DelEnvEffect(dwType, 0));
		ReloadList();
	}
	return;
Exit0:
	if (dwType)
	{
		LPCTSTR tczName = KSCENEEDITOR_HELPERCLASSES::GetRTTITypeName(dwType);
		CString TempCString;
		TempCString.Format(_T("以下类型没有办法被删除：%s 如果是远山、天空盒、和云的话，请使用相关的面板删除，或者删除整个SkySystem。"), tczName);
		MessageBox((LPCTSTR)TempCString, _T("提示"), MB_OK | MB_ICONINFORMATION);
	}
	return;
}

void KSceneSettingPageEnvironment::OnBnClickedCkSsEnableControlByDw()
{
	// TODO: Add your control notification handler code here
	SynFromInterfaceToInner();
	SynFromInnerToInterface();
}

void KSceneSettingPageEnvironment::OnBnClickedSaveAs()
{
	TCHAR filter[] = _T("Ini Files(*.ini)|*.ini||");

	TCHAR curDir[MAX_PATH * 2] = {_T('\0')};
	::GetCurrentDirectory(_countof(curDir), curDir);

	CFileDialog dlg(FALSE, _T("ini"), _T("*.ini"), OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT, filter);
	if (dlg.DoModal() == IDOK)
	{
		::SetCurrentDirectory(curDir);

		const CString filePath = dlg.GetPathName();
		IEKG3DEnvironment* pEnv = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
		if (NULL != pEnv)
		{
			HRESULT hr = pEnv->Save(filePath);
			if (FAILED(hr))
			{
				MessageBox(_T("保存没有成功，可能路径有问题"));
			}
		}
		else
		{
			MessageBox(_T("保存没有成功, KG3DEnvironment不存在，这是个异常情况"));
		}
	}
}
void KSceneSettingPageEnvironment::OnBnClickedLoad()
{
	if(IDYES != MessageBox(_T("重加载天气，旧的天气会被全部清除，是否继续"), NULL, MB_YESNO))
	{
		return;
	}

	TCHAR filter[] = _T("Ini Files(*.ini)|*.ini||");
	CFileDialog dlg(TRUE, _T("ini"), _T("*.ini"), OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_FILEMUSTEXIST, filter);

	TCHAR curDir[MAX_PATH * 2] = {_T('\0')};
	::GetCurrentDirectory(_countof(curDir), curDir);

	if(dlg.DoModal() == IDOK)
	{
		::SetCurrentDirectory(curDir);

		const CString filePath = dlg.GetPathName();
		IEKG3DEnvironment* pEnv = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
		if (NULL != pEnv)
		{
			HRESULT hr = pEnv->Reload(filePath);
			if (FAILED(hr))
			{
				MessageBox(_T("加载没有成功，可能路径或文件有问题"));
			}
			else
			{
				ReloadList();
			}
		}
		else
		{
			MessageBox(_T("加载没有成功, KG3DEnvironment不存在，这是个异常情况"));
		}
	}
}

void KSceneSettingPageEnvironment::OnBnClickedSave()
{
	IEKG3DEnvironment* pEnv = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
	if(NULL == pEnv)
	{
		MessageBox(_T("保存没有成功, KG3DEnvironment不存在，这是个异常情况"));
		return;
	}
	IEKG3DScene* pScene = KSH::GetEngineInterfaceEx<IEKG3DScene>();
	if (NULL == pScene)
	{
		MessageBox(_T("保存没有成功, Scene不存在，这是个异常情况"));
		return;
	}
	IEKG3DCollocator* pCollocator = KSH::GetEngineInterfaceEx<IEKG3DCollocator>();
	if (NULL == pCollocator)
	{
		MessageBox(_T("保存没有成功, 无法得到默认保存路径，这是个异常情况"));
		return;
	}

	TCHAR pathBuffer[MAX_PATH];
	HRESULT hr = pScene->GetName(pathBuffer, MAX_PATH);
	KG_COM_PROCESS_ERROR(hr);

	if(NULL == _tcsstr(pathBuffer, _T("map"))&& NULL == _tcsstr(pathBuffer, _T("Map"))&& NULL == _tcsstr(pathBuffer, _T("MAP")))
	{
		CString csTemp = _T("当前Scene的路径为：");
		csTemp += pathBuffer;
		csTemp += _T("\n看上去不是个map文件,是否继续？");
		if (IDYES != MessageBox(csTemp, _T("提示"), MB_YESNOCANCEL))
		{
			return;
		}
	}
	{
		pathBuffer[_countof(pathBuffer) - 1] = _T('\0');

		TCHAR pathEnv[MAX_PATH];
		hr = pCollocator->GetSavePath(EM_SAVE_OP_ENVIRONMENT, pathBuffer, NULL, pathEnv, _countof(pathEnv));
		KG_COM_PROCESS_ERROR(hr);

		pathEnv[_countof(pathEnv) - 1] = _T('\0');

		CString csTemp = _T("保存路径为：");
		csTemp += pathEnv;
		csTemp += _T("\n是否正确？");
		if(IDYES == MessageBox(csTemp, _T("提示"), MB_YESNOCANCEL))
		{
			hr = pEnv->Save(pathEnv);
			if (SUCCEEDED(hr))
			{
				MessageBox(_T("环境参数已被保存"));
			}
		}
	}

	return;
Exit0:
	MessageBox(_T("保存没有成功, 无法得到默认保存路径"));
	return;
}
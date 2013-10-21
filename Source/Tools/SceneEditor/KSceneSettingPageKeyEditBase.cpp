// KSceneSettingPageKeyEditBase.cpp : implementation file
//

#include "stdafx.h"

#if 0

#include "SceneEditor.h"
#include "KSceneSettingPageKeyEditBase.h"
//#include "KSceneInterfaceGetters.h"
//#include "KGHelperClasses.h"
#include "IEKG3DEnvironment.h"
#include "KSceneSceneHelpers.h"
#include "KSceneSettingPropertySheet.h"
#include "IEKG3DEnvEffDynamicWeather.h"
//#include "KGHelperClasses.h"
// KSceneSettingPageKeyEditBase dialog
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
///静态成员
int KSceneSettingPageKeyEditBase::m_pEnableInKeyEditStateIDs[] = {
	IDC_SS_BN_MOD_KEY,
	IDC_SS_BN_NO_MOD_KEY,
};
int KSceneSettingPageKeyEditBase::m_pDisableInKeyEditStateIDs[] = {
	IDC_SS_BN_APPLY,
	IDC_SS_BN_CANCEL,
	IDC_SS_TARGET_TREE,
	IDC_SS_LIST_BN_ADD,
	IDC_SS_LIST_BN_DEL,
	IDC_SS_LIST_BN_UP,
	IDC_SS_LIST_BN_DOWN,
	//IDC_SS_BN_SYNWITHFOGCOLOR,
	IDC_SS_BN_TEXPATH,
	IDC_SS_BN_UPDATEMESH,
	IDC_SS_BN_UPDATEBLEND,
	IDC_SS_BN_UPDATEALL,
};


IMPLEMENT_DYNAMIC(KSceneSettingPageKeyEditBase, CPropertyPage)

KSceneSettingPageKeyEditBase::KSceneSettingPageKeyEditBase(int nID, DWORD dwType)
	: CPropertyPage(/*KSceneSettingPageKeyEditBase::IDD*/nID)
	, m_dwType(dwType)
	, m_pCWndTargetTree(NULL)
	, m_nDWGCKeyIndex(0)
	, m_bHasModInnerState(false)
{
	
	ZeroMemory(m_CWndDisableInKeyEdit, sizeof(m_CWndDisableInKeyEdit));
	ZeroMemory(m_CWndEnableInKeyEdit, sizeof(m_CWndEnableInKeyEdit));
	ZeroMemory(m_cBufferForDescStruct, sizeof(m_cBufferForDescStruct));
}

KSceneSettingPageKeyEditBase::~KSceneSettingPageKeyEditBase()
{
}

void KSceneSettingPageKeyEditBase::CollectChildWindows()
{
	_KG3D_DEBUG_STATIC_CHECK_T(_countof(m_pEnableInKeyEditStateIDs) == _countof(m_CWndEnableInKeyEdit), Array_Size_Unmatch);
	_KG3D_DEBUG_STATIC_CHECK_T(_countof(m_pDisableInKeyEditStateIDs) == _countof(m_CWndDisableInKeyEdit), Array_Size_Unmatch);

	///先搜索好需要允许和禁止的按钮的CWnd，然后要禁止或者允许就不用GetDlgItem了
	CWnd* pTemp = NULL;
	int nWrited = 0;
	for (int i = 0; i < _countof(m_pEnableInKeyEditStateIDs); i++)
	{
		pTemp = GetDlgItem(m_pEnableInKeyEditStateIDs[i]);
		if (pTemp)
		{
			assert(nWrited < _countof(m_CWndEnableInKeyEdit));
			m_CWndEnableInKeyEdit[nWrited] = pTemp;
			nWrited++;
		}
	}
	nWrited = 0;
	for (int i = 0; i < _countof(m_pDisableInKeyEditStateIDs); i++)
	{
		pTemp = GetDlgItem(m_pDisableInKeyEditStateIDs[i]);
		if (pTemp)
		{
			assert(nWrited < _countof(m_CWndDisableInKeyEdit));
			m_CWndDisableInKeyEdit[nWrited] = pTemp;
			nWrited++;
		}
	}	

	m_pCWndTargetTree = GetDlgItem(IDC_SS_TARGET_TREE);
}

void KSceneSettingPageKeyEditBase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

void KSceneSettingPageKeyEditBase::OnBnClickedSsBnModKey()
{
	// TODO: Add your control notification handler code here
	CWnd* pParent = GetParent();
	KSceneSettingPropertySheet* pSheet = NULL;
	KG_PROCESS_ERROR(pParent && pParent->IsKindOf(RUNTIME_CLASS(KSceneSettingPropertySheet)));
	{
		pSheet = (KSceneSettingPropertySheet*)pParent;
		KSceneSettingPropertySheet::StructChangePage TempStruct = pSheet->GetData();
		INT nKeyIndex = TempStruct.m_nKeyIndex;
		KG_PROCESS_ERROR(nKeyIndex != 0); ///如果Data是0就表示没有修改对象

		KG_COM_PROCESS_ERROR(UpdateObject());
		{
			IEKG3DEnvEffDynamicWeather* pDW = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_DYNAMIC_WEATHER>();
			KG_PROCESS_ERROR(pDW);
			HRESULT hr = pDW->SKModifyKeyByState(m_dwType, 0, nKeyIndex);
			KG_COM_PROCESS_ERROR(hr);
		}
		
		OnEndKeyEditState();
		return;
	}
Exit0:
	if (pSheet)
	{
		//pSheet->ReleaseLock();
		pSheet->PostMessage(WM_DWGC_LOCK_TAB, FALSE, 0);
	}
	OnEndKeyEditState();
	return;
}

void KSceneSettingPageKeyEditBase::OnBnClickedSsBnNoModKey()
{
	// TODO: Add your control notification handler code here
	/*
	CWnd* pParent = GetParent();
		KSceneSettingPropertySheet* pSheet = NULL;
		KG_PROCESS_ERROR(pParent && pParent->IsKindOf(RUNTIME_CLASS(KSceneSettingPropertySheet)));
		pSheet = (KSceneSettingPropertySheet*)pParent;
		if (pSheet)
		{
			KSceneSettingPropertySheet::StructChangePage TempStruct;
			TempStruct.m_EnvEffectHandle.m_dwType = KG3DTYPE_ENVEFF_DYNAMIC_WEATHER_KEY_TRACK;
			pSheet->SendMessage(WM_DWGC_TO_KEY_TYPE_PAGE, (WPARAM)&TempStruct, FALSE);
		}*/
	
	OnEndKeyEditState();
	return;

}

void KSceneSettingPageKeyEditBase::OnBnClickedSsBnUpdate()
{
	UpdateObject();
}
void KSceneSettingPageKeyEditBase::OnBnClickedSsBnCancel()
{
	SynFromInnerToInterface();
}
void KSceneSettingPageKeyEditBase::OnBnClickedSsBnHelp()
{
	switch(m_dwType)
	{
	case KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX:
		{
			CString csTemp = _T("说明：天球从上到下由三个颜色定义，三个颜色的分布由它们对应的百分比决定。两个\
过渡百分比会在中天的两端产生渐变。上面的渐变从中天向天顶延伸，下面的从中天底部\
向中天顶部延伸。值的范围为所有正值。如果想雾处在地平线位置的话，注意天底色百分\
比应该等于天顶色百分比加中天色百分比。注意颜色之间的过渡尽量大，颜色的差值尽量小。\
大色差的话，会在过渡的地方产生一条颜色，这是错觉，把头向显示器挪就可以消除，但效果不好。\
渐变最大就可以消除这个问题");
			MessageBox(csTemp, _T("提示"), MB_OK | MB_ICONINFORMATION);
		}
		break;
/*
			case KG3DTYPE_ENVEFF_DYNAMIC_WEATHER_KEY_TRACK:
				{
					MessageBox(_T(""), _T("提示"), MB_OK | MB_ICONINFORMATION);
				}*/
		
	default:
		{
			MessageBox(_T("这个面板暂时没有提示"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		}
	    break;
	}
	
}
void KSceneSettingPageKeyEditBase::OnBnClickedSsCkEnableEnv()
{
	CWnd* pCWnd = GetDlgItem(IDC_SS_CK_ENABLE_ENV);
	INT nSel = -1;
	KG_PROCESS_ERROR(pCWnd);
	{
		nSel = GetSelectedEnvEffect();
		KG_PROCESS_ERROR(nSel >= 0);
		IEKG3DEnvEffect* pEnvEffect = KSH::GetEngineAbstractInterface<KG3DTYPE_ENV_EFFECT>(m_dwType, static_cast<DWORD_PTR>(nSel));
		KG_PROCESS_ERROR(pEnvEffect);
		BOOL bRet = FALSE;
		KSH::SynACheckWndWithABool(pCWnd, bRet, FALSE);
		HRESULT hr = pEnvEffect->Enable(bRet);
		KG_COM_PROCESS_ERROR(hr);
		///返回界面
		hr = pEnvEffect->IsEnable(&bRet);
		KG_COM_PROCESS_ERROR(hr);
		KSH::SynACheckWndWithABool(pCWnd, bRet, TRUE);		
	}
	return;
Exit0:
	_KG3D_DEBUG_REPORT("不能成功的Enable/Diable环境效果\n");
	return;
	
};
void KSceneSettingPageKeyEditBase::OnBnClickedSsCkEnableControlByDW()
{
	CWnd* pCWnd = GetDlgItem(IDC_SS_CK_ENABLE_CONTROL_BY_DW);
	INT nSel = -1;
	KG_PROCESS_ERROR(pCWnd);
	{
		nSel = GetSelectedEnvEffect();
		KG_PROCESS_ERROR(nSel >= 0);
		IEKG3DEnvEffect* pEnvEffect = KSH::GetEngineAbstractInterface<KG3DTYPE_ENV_EFFECT>(m_dwType, static_cast<DWORD_PTR>(nSel));
		KG_PROCESS_ERROR(pEnvEffect);
		BOOL bRet = FALSE;
		KSH::SynACheckWndWithABool(pCWnd, bRet, FALSE);
		HRESULT hr = pEnvEffect->EnableControlByDynamicWeather(bRet);
		KG_COM_PROCESS_ERROR(hr);
		///返回界面
		hr = pEnvEffect->IsEnableControlByDynamicWeather(&bRet);
		KG_COM_PROCESS_ERROR(hr);
		KSH::SynACheckWndWithABool(pCWnd, bRet, TRUE);		
	}
	return;
Exit0:
	_KG3D_DEBUG_REPORT("不能成功的Enable/Diable环境效果受动态天气控制\n");
	return;
};

HRESULT KSceneSettingPageKeyEditBase::SynFromInterfaceToInner( DWORD_PTR dwOption /*= 0*/ )
{
	//_ASSERTE(NULL && "错误的基类方法调用");
	CWnd* pCWndEnv = GetDlgItem(IDC_SS_CK_ENABLE_ENV);
	CWnd* pCWndCtrl = GetDlgItem(IDC_SS_CK_ENABLE_CONTROL_BY_DW);
	INT nSel = -1;
	//KG_PROCESS_ERROR(pCWndEnv && pCWndCtrl);
	{
		nSel = GetSelectedEnvEffect();
		KG_PROCESS_ERROR(nSel >= 0);
		IEKG3DEnvEffect* pEnvEffect = KSH::GetEngineAbstractInterface<KG3DTYPE_ENV_EFFECT>(m_dwType, static_cast<DWORD_PTR>(nSel));
		KG_PROCESS_ERROR(pEnvEffect);
		///取界面
		BOOL bRet = FALSE;
		if (pCWndEnv)
		{
			KSH::SynACheckWndWithABool(pCWndEnv, bRet, FALSE);
			pEnvEffect->Enable(bRet);
		}
		if (pCWndCtrl)
		{
			KSH::SynACheckWndWithABool(pCWndCtrl, bRet, FALSE);
			pEnvEffect->IsEnableControlByDynamicWeather(&bRet);
		}				
	}
	return S_OK;
Exit0:
	return S_OK;
}

HRESULT KSceneSettingPageKeyEditBase::SynFromInnerToInterface(DWORD_PTR dwOption /*= 0*/  )
{
	CWnd* pCWndEnv = GetDlgItem(IDC_SS_CK_ENABLE_ENV);
	CWnd* pCWndCtrl = GetDlgItem(IDC_SS_CK_ENABLE_CONTROL_BY_DW);
	INT nSel = -1;
	//KG_PROCESS_ERROR(pCWndEnv && pCWndCtrl);
	{
		nSel = GetSelectedEnvEffect();
		KG_PROCESS_ERROR(nSel >= 0);
		IEKG3DEnvEffect* pEnvEffect = KSH::GetEngineAbstractInterface<KG3DTYPE_ENV_EFFECT>(m_dwType, static_cast<DWORD_PTR>(nSel));
		KG_PROCESS_ERROR(pEnvEffect);
		BOOL bRet = FALSE;
		///返回界面
		HRESULT hr = pEnvEffect->IsEnable(&bRet);
		if (SUCCEEDED(hr) && pCWndEnv)
		{
			KSH::SynACheckWndWithABool(pCWndEnv, bRet, TRUE);
		}
		hr = pEnvEffect->IsEnableControlByDynamicWeather(&bRet);
		if (SUCCEEDED(hr) && pCWndCtrl)
		{
			KSH::SynACheckWndWithABool(pCWndCtrl, bRet, TRUE);
		}
		//KG_COM_PROCESS_ERROR(hr);

	}
	return S_OK;
Exit0:
	return S_OK;
}


HRESULT KSceneSettingPageKeyEditBase::UpdateObject()
{
	//HRESULT hr = S_OK;
	SynFromInterfaceToInner();
	SynFromInnerToInterface();//形成一个数据同步
	return S_OK;
}
BEGIN_MESSAGE_MAP(KSceneSettingPageKeyEditBase, CPropertyPage)
	ON_COMMAND(IDC_SS_BN_MOD_KEY, &KSceneSettingPageKeyEditBase::OnBnClickedSsBnModKey)
	ON_COMMAND(IDC_SS_BN_NO_MOD_KEY, &KSceneSettingPageKeyEditBase::OnBnClickedSsBnNoModKey)
	ON_COMMAND(IDC_SS_BN_APPLY, &KSceneSettingPageKeyEditBase::OnBnClickedSsBnUpdate)
	ON_COMMAND(IDC_SS_BN_CANCEL, &KSceneSettingPageKeyEditBase::OnBnClickedSsBnCancel)
	ON_COMMAND(IDC_SS_BN_HELP, &KSceneSettingPageKeyEditBase::OnBnClickedSsBnHelp)
	ON_COMMAND(IDC_SS_CK_ENABLE_ENV, &KSceneSettingPageKeyEditBase::OnBnClickedSsCkEnableEnv)
	ON_COMMAND(IDC_SS_CK_ENABLE_CONTROL_BY_DW, &KSceneSettingPageKeyEditBase::OnBnClickedSsCkEnableControlByDW)
END_MESSAGE_MAP()



// KSceneSettingPageKeyEditBase message handlers

BOOL KSceneSettingPageKeyEditBase::OnPageSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	this->CollectChildWindows();
	KSCENEEDITOR_HELPERCLASSES::LockChildWindows(this->GetSafeHwnd(), FALSE, NULL, 0);
	
	///得到是否在关键帧编辑状态
	BOOL bIsInKeyEditState = GetIsKeyEditStateFromParent();
	m_nDWGCKeyIndex = 0;
	///根据关键帧编辑状态，改某些按钮
	for (int i = 0; i < _countof(m_CWndEnableInKeyEdit); i++)
	{
		if (m_CWndEnableInKeyEdit[i])
		{
			m_CWndEnableInKeyEdit[i]->EnableWindow(bIsInKeyEditState);///如果Data是0就表示没有修改对象
		}
	}
	for (int i = 0; i < _countof(m_CWndDisableInKeyEdit); i++)
	{
		if (m_CWndDisableInKeyEdit[i])
		{
			m_CWndDisableInKeyEdit[i]->EnableWindow(!bIsInKeyEditState);
		}
	}
	if (m_pCWndTargetTree)
	{
		CTreeCtrl* pTree = (CTreeCtrl*)m_pCWndTargetTree;
		//UINT nCount = pTree->GetCount();
		HTREEITEM hItem = pTree->GetNextItem(TVI_ROOT, TVGN_LASTVISIBLE);
		if (hItem)
		{
			pTree->SelectItem(hItem);
		}
		m_pCWndTargetTree->EnableWindow(!bIsInKeyEditState);
	}
	///同步数据和界面
	
	if (bIsInKeyEditState)
	{
		///这个版本保存了在进入关键帧编辑之前的状态
		BOOL bRet = OnBeginKeyEditState();
		if (!bRet)
		{
			OnEndKeyEditState();
			goto Exit2;
		}
	}
	KG_COM_PROCESS_ERROR(SynFromInnerToInterface());	

//Exit1:
	return TRUE;
Exit0:///非关键帧状态下出错
	/*
	static int pEnableInKeyEditStateIDs[] = {
			IDC_SS_BN_MOD_KEY,
			IDC_SS_BN_NO_MOD_KEY,
		};
		KSCENEEDITOR_HELPERCLASSES::LockChildWindows(this->GetSafeHwnd(), TRUE, pEnableInKeyEditStateIDs, _countof(pEnableInKeyEditStateIDs));*/
	
	//CPropertyPage::OnSetActive();
	return FALSE;
Exit2:///关键桢编辑状态下出错
	for (int i = 0; i < _countof(m_CWndEnableInKeyEdit); i++)
	{
		if (m_CWndEnableInKeyEdit[i])
		{
			m_CWndEnableInKeyEdit[i]->EnableWindow(bIsInKeyEditState);///如果Data是0就表示没有修改对象
		}
	}
	return FALSE;

}


BOOL KSceneSettingPageKeyEditBase::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL KSceneSettingPageKeyEditBase::OnBeginKeyEditState()
{
	///得到最后一个同类的Effect；
	m_bHasModInnerState = false;
	m_nDWGCKeyIndex = 0;
	//IEKG3DEnvironment* pEnv = KSH::GetEngineInterface<KG3DTYPE_ENVIRONMENT>();
	IEKG3DEnvEffDynamicWeather* pDW = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_DYNAMIC_WEATHER>();
	KG_PROCESS_ERROR(pDW);
	{ 
		KG3DENVEFF_HANDLE* pEnvEffHandle = GetEnvEffHandleFromParent();
		INT nKeyIndex = GetEnvEffKeyIndexFromParent();
		KG_PROCESS_ERROR(pEnvEffHandle && nKeyIndex > 0);
		_ASSERTE(pEnvEffHandle->m_dwType == m_dwType);
		INT nTempNewIndex = 0;
		///记录下原来的状态
		HRESULT hr = pDW->SKCaptureStateToKey(pEnvEffHandle->m_dwType, pEnvEffHandle->m_nIndex, &nTempNewIndex);
		KG_COM_PROCESS_ERROR(hr);
		this->m_nDWGCKeyIndex = nTempNewIndex;
		hr = pDW->SKSynStateWithKey(pEnvEffHandle->m_dwType , pEnvEffHandle->m_nIndex, nKeyIndex);
		if(FAILED(hr))
		{
			pDW->SKDelKey(pEnvEffHandle->m_dwType, pEnvEffHandle->m_nIndex, nTempNewIndex);
			goto Exit0;
		}
		m_bHasModInnerState = true;
		
	}
	return TRUE;
Exit0:
	return FALSE; 
}

BOOL KSceneSettingPageKeyEditBase::OnEndKeyEditState()
{
	if (true == m_bHasModInnerState)///恢复状态
	{	
		IEKG3DEnvEffDynamicWeather* pDW = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_DYNAMIC_WEATHER>();
		if (pDW)
		{
			KG3DENVEFF_HANDLE* pEnvEffHandle = GetEnvEffHandleFromParent();
			INT nKeyIndex = GetEnvEffKeyIndexFromParent();
			_ASSERTE(pEnvEffHandle && m_nDWGCKeyIndex > 0);
			pDW->SKSynStateWithKey(pEnvEffHandle->m_dwType, pEnvEffHandle->m_nIndex, m_nDWGCKeyIndex);
			pDW->SKDelKey(pEnvEffHandle->m_dwType, pEnvEffHandle->m_nIndex, m_nDWGCKeyIndex);
			m_nDWGCKeyIndex = 0;
		}
		m_bHasModInnerState = false;
		CWnd* pParent = GetParent();
		KSceneSettingPropertySheet* pSheet = NULL;
		KG_PROCESS_ERROR(pParent && pParent->IsKindOf(RUNTIME_CLASS(KSceneSettingPropertySheet)));
		pSheet = (KSceneSettingPropertySheet*)pParent;
		if (pSheet)
		{
			//pSheet->ReleaseLock();
			KSceneSettingPropertySheet::StructChangePage TempStruct;
			TempStruct.m_EnvEffectHandle.m_dwType = KG3DTYPE_ENVEFF_DYNAMIC_WEATHER_KEY_TRACK;
			pSheet->SendMessage(WM_DWGC_TO_KEY_TYPE_PAGE, (WPARAM)&TempStruct, FALSE);
		}
	}
	
Exit0:
	return TRUE;
/*
Exit0:
	if(pSheet)
	{
		pSheet->PostMessage(WM_DWGC_LOCK_TAB, FALSE, 0);
	}
	
	return;*/

}
BOOL KSceneSettingPageKeyEditBase::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class
	OnEndKeyEditState();
	return CPropertyPage::OnKillActive();
}

KG3DENVEFF_HANDLE* KSceneSettingPageKeyEditBase::GetEnvEffHandleFromParent()
{
	CWnd* pCWnd = GetParent();
	_ASSERTE(pCWnd && pCWnd->IsKindOf(RUNTIME_CLASS(KSceneSettingPropertySheet)));
	KSceneSettingPropertySheet* pSheet = (KSceneSettingPropertySheet*)pCWnd;
	if(pSheet->IsInKeyEditState())
	{
		KSceneSettingPropertySheet::StructChangePage TempStruct;
		TempStruct = pSheet->GetData();
		static KG3DENVEFF_HANDLE TempHandle;
		TempHandle = TempStruct.m_EnvEffectHandle;
		return &TempHandle;
	}
	return NULL;
}

INT KSceneSettingPageKeyEditBase::GetEnvEffKeyIndexFromParent()
{
	CWnd* pCWnd = GetParent();
	_ASSERTE(pCWnd && pCWnd->IsKindOf(RUNTIME_CLASS(KSceneSettingPropertySheet)));
	KSceneSettingPropertySheet* pSheet = (KSceneSettingPropertySheet*)pCWnd;
	if(pSheet->IsInKeyEditState())
	{
		static KSceneSettingPropertySheet::StructChangePage TempStruct;
		TempStruct = pSheet->GetData();
		return TempStruct.m_nKeyIndex;
	}
	return -1;
}

BOOL KSceneSettingPageKeyEditBase::GetIsKeyEditStateFromParent()
{
	CWnd* pCWnd = GetParent();
	_ASSERTE(pCWnd && pCWnd->IsKindOf(RUNTIME_CLASS(KSceneSettingPropertySheet)));
	KSceneSettingPropertySheet* pSheet = (KSceneSettingPropertySheet*)pCWnd;
	return pSheet->IsInKeyEditState()? TRUE : FALSE;
}

#endif
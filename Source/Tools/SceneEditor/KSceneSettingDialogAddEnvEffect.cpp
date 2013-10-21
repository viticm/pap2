// KSceneSettingDialogAddEnvEffect.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingDialogAddEnvEffect.h"
//#include "KSceneInterfaceGetters.h"
#include "KSceneSceneHelpers.h"
#include "IEKG3DEnvironment.h"
//#include "KGHelperClasses.h"
// KSceneSettingDialogAddEnvEffect dialog
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNAMIC(KSceneSettingDialogAddEnvEffect, CDialog)

KSceneSettingDialogAddEnvEffect::KSceneSettingDialogAddEnvEffect(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSettingDialogAddEnvEffect::IDD, pParent)
	, m_dwRetType(0)
{

}

KSceneSettingDialogAddEnvEffect::~KSceneSettingDialogAddEnvEffect()
{
}

void KSceneSettingDialogAddEnvEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbEnvEffList);
}


BEGIN_MESSAGE_MAP(KSceneSettingDialogAddEnvEffect, CDialog)
END_MESSAGE_MAP()


// KSceneSettingDialogAddEnvEffect message handlers

BOOL KSceneSettingDialogAddEnvEffect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_dwRetType = 0;
	//DWORD* pTypeArray = NULL;
	IEKG3DEnvironment* pEnv = KSH::GetEngineInterface<KG3DTYPE_ENVIRONMENT>();
	KG_PROCESS_ERROR(pEnv);
	{
		/*INT_PTR nRetCount = 0;
		INT_PTR nRetWrited = 0;
		KG_COM_PROCESS_ERROR(pEnv->GetAllEnvEffectThatCanBeAddedCount(&nRetCount));
		KG_PROCESS_ERROR(nRetCount <= KG3DENVEFF_MAX);
		_ASSERTE(nRetCount >=0);
		
		for (int i = 0; i < nRetCount; i++)*/

		KG3DEnvEffectClassProperty* pProperty = pEnv->GetFirstClassProperty();
		while(NULL != pProperty)
		{	
			//DWORD dwType;
			//KG_COM_PROCESS_ERROR(pEnv->GetEnvEffectThatCanBeAdded(i, &dwType));

			DWORD dwType = pProperty->m_dwType;

			LPCTSTR tczName = KSH::GetRTTITypeName(dwType);
			if (tczName)
			{
				int nPos = m_cbEnvEffList.AddString(tczName);
				KG_PROCESS_ERROR(nPos >= 0);
				int nRet = m_cbEnvEffList.SetItemData(nPos, (DWORD_PTR)(dwType));
			}
			
			pProperty = pEnv->GetNextClassProperty(pProperty);
		}
	}
	this->BringWindowToTop();
Exit0:
	//SAFE_DELETE_ARRAY(pTypeArray);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

DWORD KSceneSettingDialogAddEnvEffect::GetResultType(void)
{
	return m_dwRetType;
}

void KSceneSettingDialogAddEnvEffect::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	int nSel = m_cbEnvEffList.GetCurSel();
	if (nSel >= 0)
	{
		DWORD_PTR dwPtrTemp = m_cbEnvEffList.GetItemData(nSel);
		m_dwRetType = _kg_to_smaller_size_cast<DWORD>(dwPtrTemp);
		//_kg_to_smaller_size_cast<DWORD_PTR>(m_dwRetType);
		//_kg_to_smaller_size_cast<double>(m_dwRetType);
	}
	
	CDialog::OnOK();
}

void KSceneSettingDialogAddEnvEffect::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	m_dwRetType = 0;
	CDialog::OnCancel();
}

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageFog.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-10-25 18:55:11
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageFog.h"
//#include "KSceneInterfaceGetters.h"
#include "KColordialog.h"
#include "IEKG3DScene.h"
#include "IEKG3DEnvironment.h"
#include "IEKG3DEnvEffFog.h"
#include "KSceneSceneHelpers.h"
#include "IEKG3DSkySystem.h"
#include "IEKG3DMeshSkyBox.h"
#include "IEKG3DMeshFarMountain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
#define MAX_FOG_FLOAT 100000000.0f

IMPLEMENT_DYNAMIC(KSceneSettingPageFog, CPropertyPage)
KSceneSettingPageFog::KSceneSettingPageFog()
	//: KSceneSettingPageKeyEditBase(KSceneSettingPageFog::IDD, KG3DTYPE_ENVEFF_FOG)
	: KSceneSettingPageBase(KSceneSettingPageFog::IDD)
{
	m_dwModificationOption = NULL;

#if defined(DEBUG) | defined(_DEBUG)
	//<LEAK_TEST>
	//char* pDbgLeakTest = new char[10];
#endif
}

KSceneSettingPageFog::~KSceneSettingPageFog()
{
}

void KSceneSettingPageFog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOG_ENABLE, m_ComboxFogFlag);
	DDX_Control(pDX, IDC_FOG_DTABLE, m_CanDoTableFog);
	DDX_Control(pDX, IDC_FOG_VERTEX, m_CanDoVertexFog);
	DDX_Control(pDX, IDC_FOG_DW, m_CanDoWFog);
	DDX_Control(pDX, IDC_FOG_UW, m_DeviceUsesWFog);
	DDX_Control(pDX, IDC_FOG_RANGE, m_RangeBasedFog);
	DDX_Control(pDX, IDC_FOG_UTABLE, m_UsingTableFog);

	DDX_Control(pDX, IDC_FOG_MODE, m_FogMode);

	DDX_Text(pDX, IDC_START, m_fFogStartValue);
	DDX_Text(pDX, IDC_END, m_fFogEndValue);
	DDX_Text(pDX, IDC_SITV, m_fFogDensity);
	DDX_Control(pDX, IDC_BUTTON1, m_bnColor);
}


BEGIN_MESSAGE_MAP(KSceneSettingPageFog, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_FOG_ENABLE, &KSceneSettingPageFog::OnCbnEditchangeFogOpen)
	ON_CBN_SELCHANGE(IDC_FOG_UW, &KSceneSettingPageFog::OnCbnEditchangeFogUw)
	ON_CBN_SELCHANGE(IDC_FOG_RANGE, &KSceneSettingPageFog::OnCbnEditchangeFogRange)
	ON_CBN_SELCHANGE(IDC_FOG_UTABLE, &KSceneSettingPageFog::OnCbnEditchangeFogUseTable)
	ON_CBN_SELCHANGE(IDC_FOG_MODE, &KSceneSettingPageFog::OnCbnEditchangeFogMode)
	ON_EN_CHANGE(IDC_START, &KSceneSettingPageFog::OnEnChangeStart)
	ON_EN_CHANGE(IDC_END, &KSceneSettingPageFog::OnEnChangeEnd)
	ON_EN_CHANGE(IDC_SITV, &KSceneSettingPageFog::OnEnChangeSitv)
	//ON_BN_CLICKED(IDC_SS_BN_SYNWITHFOGCOLOR, &KSceneSettingPageFog::OnBnClickCheckSynFogColor)
END_MESSAGE_MAP()

void KSceneSettingPageFog::OnBnClickedButton1()
{
     /*GetColorPickerDialog()->Init(&m_lpSceneEditor->m_Fog.m_FogColor, "Color Of Fog");*/
}

BOOL KSceneSettingPageFog::OnInitDialog()
{
    //KSceneSettingPageKeyEditBase::OnInitDialog();
	
	CPropertyPage::OnInitDialog();

    m_ComboxFogFlag.AddString("是");
    m_ComboxFogFlag.AddString("否");

    m_CanDoTableFog.AddString("是");
    m_CanDoTableFog.AddString("否");

    m_CanDoVertexFog.AddString("是");
    m_CanDoVertexFog.AddString("否");

    m_CanDoWFog.AddString("是");
    m_CanDoWFog.AddString("否");

    m_DeviceUsesWFog.AddString("是");
    m_DeviceUsesWFog.AddString("否");

    m_RangeBasedFog.AddString("是");
    m_RangeBasedFog.AddString("否");

    m_UsingTableFog.AddString("是");
    m_UsingTableFog.AddString("否");

    m_FogMode.AddString("D3DFOG_NONE");
    m_FogMode.AddString("D3DFOG_EXP");
    m_FogMode.AddString("D3DFOG_EXP2");
    m_FogMode.AddString("D3DFOG_LINEAR");
    m_FogMode.AddString("D3DFOG_FORCE_DWORD");

    //SynFromInnerToInterface();

    return TRUE;
}

BOOL KSceneSettingPageFog::OnSetActive()
{
	//KG_PROCESS_ERROR(KSceneSettingPageKeyEditBase::OnPageSetActive());	
	SynFromInnerToInterface();
	return CPropertyPage::OnSetActive();
}
void KSceneSettingPageFog::OnCbnEditchangeFogOpen()
{ 
	// TODO: Add your control notification handler code here
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_FOG_BASE;
}
void KSceneSettingPageFog::OnCbnEditchangeFogUw()
{
	// TODO: Add your control notification handler code here
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_TABLE_FOG;
}
void KSceneSettingPageFog::OnCbnEditchangeFogRange()
{
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_VERTEX_FOG;
}
void KSceneSettingPageFog::OnCbnEditchangeFogUseTable()
{
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_FOG_BASE;
}
void KSceneSettingPageFog::OnCbnEditchangeFogMode()
{
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_FOG_BASE;
}
void KSceneSettingPageFog::OnEnChangeStart()
{
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_MODE_PARAM;
}

void KSceneSettingPageFog::OnEnChangeEnd()
{
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_MODE_PARAM;
}
void KSceneSettingPageFog::OnEnChangeSitv()
{
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_MODE_PARAM;
}
HRESULT KSceneSettingPageFog::SynFromInterfaceToInner(DWORD_PTR dwOption/* = 0*/ )
{
	UpdateData(TRUE);
	
	HRESULT hr = E_FAIL;
	///同步界面数据到结构体
	KG3DENVEFFFOG_DESCSTRUCT tempStruct;
	CString szValue;
	IEKG3DEnvEffFog* lpFog = KSH::GetEngineInterfaceEx<IEKG3DEnvEffFog>();
	KG_PROCESS_ERROR(lpFog);
	
	
	SynBOOLwithContainedComboBox(tempStruct.m_bFogEnable, m_ComboxFogFlag);
	SynBOOLwithContainedComboBox(tempStruct.m_bDeviceUsesWFog, m_DeviceUsesWFog);
	SynBOOLwithContainedComboBox(tempStruct.m_bRangeBasedFog, m_RangeBasedFog);
	SynBOOLwithContainedComboBox(tempStruct.m_bUsingTableFog, m_UsingTableFog);

	m_FogMode.GetWindowText(szValue);
	if (szValue == "D3DFOG_LINEAR")
	{
		tempStruct.m_nFogMode = D3DFOG_LINEAR;
	}
	else if (szValue == "D3DFOG_NONE")
	{
		tempStruct.m_nFogMode = D3DFOG_NONE;
	}
	else if (szValue == "D3DFOG_EXP")
	{
		tempStruct.m_nFogMode = D3DFOG_EXP;
	}
	else if (szValue == "D3DFOG_EXP2")
	{
		tempStruct.m_nFogMode = D3DFOG_EXP2;
	}
	else if (szValue == "D3DFOG_FORCE_DWORD")
	{
		tempStruct.m_nFogMode = D3DFOG_FORCE_DWORD;
	}
	if (m_fFogDensity > 1.0)
	{
		m_fFogDensity = 1.0f;
	}
	if (m_fFogDensity < 0)
	{
		m_fFogDensity = 0.0f;
	}
	if (m_fFogStartValue > MAX_FOG_FLOAT)
	{
		m_fFogStartValue = MAX_FOG_FLOAT;
	}
	if (m_fFogEndValue > MAX_FOG_FLOAT)
	{
		m_fFogEndValue = MAX_FOG_FLOAT;
	}
	tempStruct.m_fFogDensity = m_fFogDensity;
	tempStruct.m_fFogStartValue = m_fFogStartValue;
	tempStruct.m_fFogEndValue  = m_fFogEndValue;
	
	tempStruct.m_FogColor = m_bnColor.GetBkColor();
	m_dwModificationOption |= KG3DENVEFFFOG_MDO_FOG_COLOR;

	////如果需要自动同步天空色，那么更改天空盒和远山的颜色
	//{
	//	D3DCOLOR FogColor = tempStruct.m_FogColor;
	//	SynColorElementsWithFogColor(FogColor);

	//}

	///选择刷新
	KG3DENVEFFFOG_DESCSTRUCT* pTStruct = &tempStruct;
	hr = lpFog->RequestModification(pTStruct, m_dwModificationOption);
	KG_COM_PROCESS_ERROR(hr);
	m_dwModificationOption = 0;///记得重新清零
	//KSceneSettingPageKeyEditBase::SynFromInterfaceToInner((DWORD_PTR)pTStruct);
	return S_OK;
Exit0:
	m_dwModificationOption = 0;///记得重新清零
	return E_FAIL;
}

HRESULT KSceneSettingPageFog::SynFromInnerToInterface(DWORD_PTR dwOption /*= 0*/ )
{
	IEKG3DEnvEffFog* lpFog = KSH::GetEngineInterfaceEx<IEKG3DEnvEffFog>();
	if (!lpFog)
	{
		return E_FAIL;
	}
	///同步内部数据到界面和结构体
	KG3DENVEFFFOG_DESCSTRUCT	tempStruct;
	KG_COM_PROCESS_ERROR(lpFog->GetStruct(&tempStruct));
	//KG_PROCESS_ERROR(NULL == pStruct);
	switch (tempStruct.m_nFogMode)
	{
	case D3DFOG_LINEAR :
		m_FogMode.SelectString(0, "D3DFOG_LINEAR");
		break;
	case D3DFOG_NONE :
		m_FogMode.SelectString(0, "D3DFOG_NONE");
		break;
	case D3DFOG_EXP :
		m_FogMode.SelectString(0, "D3DFOG_EXP");
		break;
	case D3DFOG_EXP2 :
		m_FogMode.SelectString(0, "D3DFOG_EXP2");
		break;	
	case D3DFOG_FORCE_DWORD :
		m_FogMode.SelectString(0, "D3DFOG_FORCE_DWORD");
		break;
	default:;
	}
	SynContainedComboBoxWithBOOL(m_ComboxFogFlag, tempStruct.m_bFogEnable);
	{
		KG3DENVEFFFOG_CAPS_STRUCT tempCapsStruct;
		lpFog->GetCaps(&tempCapsStruct);
		SynContainedComboBoxWithBOOL(m_CanDoTableFog, tempCapsStruct.m_bCanDoTableFog);
		SynContainedComboBoxWithBOOL(m_CanDoVertexFog, tempCapsStruct.m_bCanDoVertexFog);
		SynContainedComboBoxWithBOOL(m_CanDoWFog, tempCapsStruct.m_bCanDoWFog);
	}
	SynContainedComboBoxWithBOOL(m_DeviceUsesWFog, tempStruct.m_bDeviceUsesWFog);
	SynContainedComboBoxWithBOOL(m_RangeBasedFog, tempStruct.m_bRangeBasedFog);
	SynContainedComboBoxWithBOOL(m_UsingTableFog, tempStruct.m_bUsingTableFog);
	m_fFogDensity = tempStruct.m_fFogDensity;
	m_fFogStartValue = tempStruct.m_fFogStartValue;
	m_fFogEndValue = tempStruct.m_fFogEndValue;

	m_bnColor.SetBKColor(tempStruct.m_FogColor);

	UpdateData(FALSE);
	///选择是否额外复制结构体

	/*if (g_bSynFogColorWithSkyColor)
	{
		CWnd* p = this->GetDlgItem(IDC_SS_BN_SYNWITHFOGCOLOR);
		if (p)
		{
			((CButton*)p)->SetCheck(BST_CHECKED);
		}
	}*/

	return S_OK;
Exit0:
	return E_FAIL;
}

void KSceneSettingPageFog::SynContainedComboBoxWithBOOL( CComboBox& cbTarget, BOOL bData )
{
	if (bData)
		cbTarget.SetCurSel(1);
	else
		cbTarget.SetCurSel(0);

}

void KSceneSettingPageFog::SynBOOLwithContainedComboBox( BOOL& bData, CComboBox& cbTarget )
{
	if (cbTarget.GetCurSel() == 1)
		bData = TRUE;
	else
		bData = FALSE;
}

//void KSceneSettingPageFog::SynColorElementsWithFogColor( D3DCOLOR FogColor )
//{
//	CWnd* pCheck = this->GetDlgItem(IDC_SS_BN_SYNWITHFOGCOLOR);
//	if (! pCheck)
//		return;
//
//	if (pCheck->SendMessage(BM_GETCHECK, 0, 0) != BST_CHECKED)
//		return;
//
//	IEKG3DSkySystem* pSkySystem = KSH::GetEngineInterfaceEx<IEKG3DSkySystem>();
//	if (NULL == pSkySystem)
//		return;
//	
//	//改天空盒的颜色
//	{
//		IEKG3DMeshSkyBox* pSkyBox = NULL;
//		if (SUCCEEDED(pSkySystem->GetSkyBox(&pSkyBox)))
//		{
//			KG3DMESHSKYBOX_DESCSTRUCT TempDesc;
//			if (SUCCEEDED(pSkyBox->GetDescStruct(&TempDesc)))
//			{
//				TempDesc.d3dc_xrgb_BotmColor = FogColor;
//				TempDesc.m_dwModificationOption = KG3DMESHSKYBOX_MDO_ALL;
//				pSkyBox->RequestModification(&TempDesc);
//			}
//		}
//	}
//	//改远山的颜色
//	{
//		INT Size = 0;
//		HRESULT hr = pSkySystem->GetFarMountVecSize(&Size);
//		if (SUCCEEDED(hr) && Size > 0)
//		{
//			for (INT i = 0; i < Size; ++i)
//			{
//				IEKG3DMeshFarMountain* pFarMount = NULL;
//				if (SUCCEEDED(pSkySystem->GetFarMount(i, &pFarMount)))
//				{
//					KG3DMESHFARMOUNTAIN_DESCSTRUCT TempStruct;
//					if (SUCCEEDED(pFarMount->GetStruct(&TempStruct)))
//					{
//						TempStruct.m_d3dc_Color = FogColor;
//						TempStruct.m_dwModificationOption = KG3DMESHFARMOUNTAIN_MDO_TEX;
//						pFarMount->RequestModification(&TempStruct);
//					}
//				}
//			}
//
//		}
//	}
//}

//void KSceneSettingPageFog::OnBnClickCheckSynFogColor()
//{
//	CWnd* p = this->GetDlgItem(IDC_SS_BN_SYNWITHFOGCOLOR);
//	if (p && BST_CHECKED == ((CButton*)p)->GetCheck())
//	{
//		SynColorElementsWithFogColor(m_bnColor.GetBkColor());
//		g_bSynFogColorWithSkyColor = TRUE;
//	}
//	else
//		g_bSynFogColorWithSkyColor = FALSE;
//}

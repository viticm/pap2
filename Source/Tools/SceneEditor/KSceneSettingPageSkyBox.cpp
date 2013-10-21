
#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageSkyBox.h"
#include "IEKG3DMeshSkyBox.h"
//#include "KG3DSceneSceneEditor.h"
#include "KColorDialog.h"
//#include "SceneEditorCommon.h"
#include "IEKG3DSkySystem.h"
//#include "KSceneInterfaceGetters.h"
#include "KSceneSettingDialog.h"
#include "IEKG3DEnvironment.h"
#include "IEKG3DMeshSkyBox.h"
#include "IEKG3DEnvEffFog.h"
//#include "MathTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KSceneSettingPageSkyBox, CPropertyPage)

KSceneSettingPageSkyBox::KSceneSettingPageSkyBox()
	//: KSceneSettingPageKeyEditBase(KSceneSettingPageSkyBox::IDD, KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX)
	: KSceneSettingPageBase(KSceneSettingPageSkyBox::IDD)
	, m_czTopPercent(_T(""))
	, m_czTHBlendPercent(_T(""))
	, m_czMidPercent(_T(""))
	, m_czHBBlendPercent(_T(""))
	, m_czBotmPercent(_T(""))
	, m_szPath(_T(""))
	//, m_lpSkyBox(NULL)
	//, m_lpSkySystem(NULL)
{
    m_nFlag  = false;//这个标志决定Update方法是否应该被调用
}

KSceneSettingPageSkyBox::~KSceneSettingPageSkyBox()
{
}

void KSceneSettingPageSkyBox::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_TEXTURE_SKY, m_szPath);
	DDX_Control(pDX, IDC_BUTTON_SKYSPHERE_COLOR_TOP, m_bnTopColor);
	DDX_Control(pDX, IDC_BUTTON_SKYSPHERE_COLOR_MID, m_bnMidColor);
	//DDX_Control(pDX, IDC_BUTTON_SKYSPHERE_COLOR_BTOM, m_bnBotmColor);
	DDX_Text(pDX, IDC_EDIT_SKYSPHERE_TOP_PERCENT, m_czTopPercent);
	DDX_Text(pDX, IDC_EDIT_SKYSPHERE_TMBLEND_PERCENT, m_czTHBlendPercent);
	DDX_Text(pDX, IDC_EDIT_SKYSPHERE_MID_PERCENT, m_czMidPercent);
	DDX_Text(pDX, IDC_EDIT_SKYSPHERE_MBBLEND_PERCENT, m_czHBBlendPercent);
	DDX_Text(pDX, IDC_EDIT_SKYSPHERE_BOTM_PERCENT, m_czBotmPercent);
}


BEGIN_MESSAGE_MAP(KSceneSettingPageSkyBox, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_SS_BN_CANCEL, &KSceneSettingPageSkyBox::OnBnClickedSsBnCancel)
	ON_BN_CLICKED(IDC_SS_LIST_BN_ADD, &KSceneSettingPageSkyBox::OnBnClickedSsListBnAdd)
	ON_BN_CLICKED(IDC_SS_LIST_BN_DEL, &KSceneSettingPageSkyBox::OnBnClickedSsListBnDel)
	//ON_BN_DOUBLECLICKED(IDC_BUTTON_SKYSPHERE_COLOR_BTOM, &KSceneSettingPageSkyBox::OnBnClickedBottomColor)
END_MESSAGE_MAP()

void KSceneSettingPageSkyBox::OnBnClickedButton1()
{
    CFileDialog dlg(
        true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "DDS | *.dds"
    );
    if (dlg.DoModal() == IDOK)
    {
        m_szPath = dlg.GetFileName();
    }
    UpdateData(false);
}

BOOL KSceneSettingPageSkyBox::OnSetActive()
{
    m_nFlag  = true ;

	///<DWGC_KEY_EDIT>


	///</DWGC_KEY_EDIT>
		
	//KG_PROCESS_ERROR(KSceneSettingPageKeyEditBase::OnPageSetActive());
	SynFromInnerToInterface();	

	return CPropertyPage::OnSetActive();
}

HRESULT	KSceneSettingPageSkyBox::SynFromInterfaceToInner(DWORD_PTR dwOption/* = 0*/ )
{
	UpdateData();

	///收集界面信息到内部结构体
	m_DlgSkyColorStruct.d3dc_xrgb_TopColor = m_bnTopColor.GetBkColor();
	m_DlgSkyColorStruct.d3dc_xrgb_MidColor = m_bnMidColor.GetBkColor();
	//m_DlgSkyColorStruct.d3dc_xrgb_BotmColor = m_bnBotmColor.GetBkColor();

	//Get3DEngineInterface((void **)(&pManager));

	LPCTSTR2IntInRange((LPCTSTR)m_czTopPercent, &m_DlgSkyColorStruct.nTopPercent, 0, 1024 );
	LPCTSTR2IntInRange((LPCTSTR)m_czTHBlendPercent, &m_DlgSkyColorStruct.nTop_MidBlendPercent, 0, 1024 );
	LPCTSTR2IntInRange((LPCTSTR)m_czMidPercent, &m_DlgSkyColorStruct.nMidPercent, 0, 1024 );
	LPCTSTR2IntInRange((LPCTSTR)m_czHBBlendPercent, &m_DlgSkyColorStruct.nMid_BotmBlendPercent, 0, 1024 );
	LPCTSTR2IntInRange((LPCTSTR)m_czBotmPercent, &m_DlgSkyColorStruct.nBotmPercent, 0, 1024 );

	//m_DlgSkyColorStruct.fRadius = m_fRadius;

	///是否同步的Check的同步
	
	
	//KG_PROCESS_ERROR(m_lpSkySystem && m_lpSkyBox );
	IEKG3DSkySystem* pSkySystem =  KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
	IEKG3DMeshSkyBox* pSkyBox =  KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX>();
	KG_PROCESS_ERROR(pSkySystem && pSkyBox);

	/*
	int nCheckState = m_ckEnableSynWithFogColor.GetCheck();
		if ( BST_CHECKED == nCheckState )
		{
			///Init的时候是从SkyBox中读取这个，但现在要求颜色同步，所以用SceneEditor那个
			if ( SUCCEEDED(pSkySystem->EnableSynWithFogColor(TRUE)))
			{
				m_bnBotmColor.EnableWindow( FALSE );///如果允许同步，就禁止修改底部颜色
			}		
		}
		else
		{
			if ( SUCCEEDED(pSkySystem->EnableSynWithFogColor(FALSE)))
			{
				m_bnBotmColor.EnableWindow( TRUE );
			}		
		}*/
	

	///选择刷新
    KG3DMESHSKYBOX_DESCSTRUCT* pTStruct = &m_DlgSkyColorStruct;	
	pTStruct->m_dwModificationOption = KG3DMESHSKYBOX_MDO_ALL;
//	KSceneSettingPageKeyEditBase::SynFromInterfaceToInner((DWORD_PTR)pTStruct);
	return pSkyBox->RequestModification( pTStruct );
Exit0:
	return E_FAIL;
}
HRESULT	KSceneSettingPageSkyBox::SynFromInnerToInterface(DWORD_PTR dwOption /*= 0 */)
{
    BOOL bSynWithFogColor = NULL;
	///同步内部结构体到界面和Dlg结构体   
	IEKG3DSkySystem* pSkySystem =  KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
	IEKG3DMeshSkyBox* pSkyBox =  KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX>();
	KG_PROCESS_ERROR(pSkySystem && pSkyBox);
    pSkyBox->GetDescStruct(&m_DlgSkyColorStruct);
		
	m_bnTopColor.SetBKColor( m_DlgSkyColorStruct.d3dc_xrgb_TopColor );
	m_bnMidColor.SetBKColor( m_DlgSkyColorStruct.d3dc_xrgb_MidColor );
	//m_bnBotmColor.SetBKColor( m_DlgSkyColorStruct.d3dc_xrgb_BotmColor );

	m_czTopPercent.Format(_T("%d"), m_DlgSkyColorStruct.nTopPercent );
	m_czTHBlendPercent.Format(_T("%d"), m_DlgSkyColorStruct.nTop_MidBlendPercent );
	m_czMidPercent.Format(_T("%d"), m_DlgSkyColorStruct.nMidPercent );
	m_czHBBlendPercent.Format(_T("%d"), m_DlgSkyColorStruct.nMid_BotmBlendPercent );
	m_czBotmPercent.Format(_T("%d"), m_DlgSkyColorStruct.nBotmPercent );

	//m_fRadius = m_DlgSkyColorStruct.fRadius;

	///Check的部分

	//pSkySystem->GetSkyBox(&pMeshSkyBox);
	//pSkySystem->IsEnableSynWithFogColor(&bSynWithFogColor);
	/*
	if (TRUE == bSynWithFogColor)
		{
			m_ckEnableSynWithFogColor.SetCheck( BST_CHECKED );
			m_bnBotmColor.EnableWindow( FALSE );
		}
		else
		{
			m_ckEnableSynWithFogColor.SetCheck( BST_UNCHECKED );
			m_bnBotmColor.EnableWindow( TRUE );
		}*/
	
	UpdateData( FALSE );
	
	///选择是否多复制一份
	/*
	if ( NULL != pStruct )
		{
			_ASSERTE(nSizeOfStruct >= sizeof(KG3DMESHSKYBOX_DESCSTRUCT));
			*static_cast<KG3DMESHSKYBOX_DESCSTRUCT*>(pStruct) = m_DlgSkyColorStruct;
		}*/
	
	//KSceneSettingPageKeyEditBase::SynFromInnerToInterface((DWORD_PTR)&m_DlgSkyColorStruct);
	return S_OK;
Exit0:
	return E_FAIL;
}





void KSceneSettingPageSkyBox::OnBnClickedSsBnCancel()
{
	// TODO: Add your control notification handler code here
	SynFromInnerToInterface();
}

void KSceneSettingPageSkyBox::OnBnClickedSsListBnAdd()
{
	// TODO: Add your control notification handler code here
	IEKG3DSkySystem* pSkySystem = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
	if (pSkySystem)
	{
		if (SUCCEEDED(pSkySystem->AddSkyBox(NULL)))
		{
			//CheckFogColorSync();
			this->OnSetActive();
		}
	}
}
void KSceneSettingPageSkyBox::OnBnClickedSsListBnDel()
{
	// TODO: Add your control notification handler code here
	IEKG3DSkySystem* pSkySystem = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
	if (pSkySystem)
	{
		if (SUCCEEDED(pSkySystem->DelSkyBox()))
		{
			this->OnSetActive();
		}
	}
}

//VOID KSceneSettingPageSkyBox::CheckFogColorSync()
//{
//	IEKG3DMeshSkyBox* pSkyBox = KSH::GetEngineInterfaceEx<IEKG3DMeshSkyBox>();
//	if(! pSkyBox)
//		return;
//
//	IEKG3DEnvEffFog* pFog = KSH::GetEngineInterfaceEx<IEKG3DEnvEffFog>();
//	if(! pFog)
//		return;
//
//	D3DCOLOR FogColor;
//	pFog->GetFogColor(&FogColor);
//
//	KG3DMESHSKYBOX_DESCSTRUCT	Desc;
//	HRESULT hr = pSkyBox->GetDescStruct(&Desc);
//	if (FAILED(hr))
//		return;
//
//	Desc.d3dc_xrgb_BotmColor = FogColor;
//	Desc.m_dwModificationOption = KG3DMESHSKYBOX_MDO_ALL;
//
//	pSkyBox->RequestModification(&Desc);
//
//}

//void KSceneSettingPageSkyBox::OnBnClickedBottomColor()
//{
//	if (! m_bnBotmColor.IsColorPanelPoppingEnabled())
//	{
//		MessageBox(_T("当前处于同步雾色状态中，不能改这个颜色，可以通过改雾色来同步更改这个颜色\
//，或者取消同步雾色之后也能更改这个颜色，请在雾面板操作"));
//	}
//}

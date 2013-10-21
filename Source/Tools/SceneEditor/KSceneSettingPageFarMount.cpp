// KSceneSettingPageFarMount.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "IEKG3DSkySystem.h"
#include "KSceneSettingPageFarMount.h"
//#include "KG3DGraphicsTool.h"
//#include "KSceneInterfaceGetters.h"
#include "IEKG3DEnvironment.h"
#include "KSceneSceneHelpers.h"
#include "IEKG3DEnvEffFog.h"
// KSceneSettingPageFarMount dialog


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(KSceneSettingPageFarMount, CPropertyPage)

KSceneSettingPageFarMount::KSceneSettingPageFarMount()
	//: KSceneSettingPageKeyEditBase(KSceneSettingPageFarMount::IDD, KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN)
	//, m_lpSceneEditor( NULL )
	: KSceneSettingPageBase(KSceneSettingPageFarMount::IDD)
	, m_nFlag( FALSE )
	, m_fRadius(0)
	, m_fHeight(0)
	, m_fTexU(0.0f)
	, m_fZRotation(0.0f)
	, m_fBotmZPos(0)
	, m_fBotmBlendHeight(0)
	, m_fFogBlendFactor(0.0f)
    , m_fTexFactor(1.0f)
{
	ZeroMemory(&m_DlgStruct, sizeof(m_DlgStruct));
	//ZeroMemory(m_TexPathBuffer,sizeof(m_TexPathBuffer));
	//m_DlgStruct.m_tczTextureFullPath = m_TexPathBuffer;///绑定一个缓冲
}

KSceneSettingPageFarMount::~KSceneSettingPageFarMount()
{
}

void KSceneSettingPageFarMount::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SS_FARMOUNT_ED_TEXPATH, m_csTexPath );
	DDX_Text(pDX, IDC_SS_FARMOUNT_ED_RADIUS,  m_fRadius );
	DDX_Text(pDX, IDC_SS_FARMOUNT_ED_HEIGHT,  m_fHeight );
	DDX_Text(pDX, IDC_SS_FARMOUNT_ED_ROTATION, m_fZRotation );
	DDX_Text(pDX, IDC_SS_FARMOUNT_ED_TEXU, m_fTexU );
	DDX_Text(pDX, IDC_SS_FARMOUNT_ED_ZMIN, m_fBotmZPos );
	DDX_Text(pDX, IDC_SS_FARMOUNT_ED_BOTMBLEND, m_fBotmBlendHeight );
	DDX_Text(pDX, IDC_SS_FARMOUNT_ED_BLENDFACTOR, m_fFogBlendFactor );
    DDX_Text(pDX, IDC_SS_FARMOUNT_ED_TEXFACTOR, m_fTexFactor);
	//DDX_Control(pDX, IDC_SS_FARMOUNT_BN_SURFACECOLOR, m_bnColor);
	DDX_Control(pDX, IDC_SS_FARMOUNT_BLENDFOGCOLOR, m_bnFogColor);
	DDX_Control(pDX, IDC_SS_TARGET_TREE, m_Tree);
	DDX_Control(pDX, IDC_SS_FARMOUNT_ED_TEXPATH, m_edTexPath);
}


BEGIN_MESSAGE_MAP(KSceneSettingPageFarMount, CPropertyPage)
	ON_BN_CLICKED(IDC_SS_LIST_BN_ADD, &KSceneSettingPageFarMount::OnBnClickedSsFarmountBnAdd)
	ON_BN_CLICKED(IDC_SS_LIST_BN_DEL, &KSceneSettingPageFarMount::OnBnClickedSsFarmountBnDel)
	ON_BN_CLICKED(IDC_SS_LIST_BN_UP, &KSceneSettingPageFarMount::OnBnClickedSsFarmountBnUp)
	ON_BN_CLICKED(IDC_SS_LIST_BN_DOWN, &KSceneSettingPageFarMount::OnBnClickedSsFarmountBnDown)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SS_TARGET_TREE, &KSceneSettingPageFarMount::OnTvnSelchangedSsFarmountTree)
	ON_BN_CLICKED(IDC_SS_BN_TEXPATH, &KSceneSettingPageFarMount::OnBnClickedSsFarmountBnTexpath)
	ON_BN_CLICKED(IDC_SS_BN_UPDATEMESH, &KSceneSettingPageFarMount::OnBnClickedSsFarmountBnUpdatemesh)
	ON_BN_CLICKED(IDC_SS_BN_UPDATEBLEND, &KSceneSettingPageFarMount::OnBnClickedSsFarmountBnUpdateblend)
	ON_BN_CLICKED(IDC_CK_ENABLE_COLOR, &KSceneSettingPageFarMount::OnBnClickedCkEnableColor)
	ON_BN_CLICKED(IDC_SS_BN_UPDATEOTHER, &KSceneSettingPageFarMount::OnBnClickedSsBnUpdateother)
	//ON_BN_DOUBLECLICKED(IDC_SS_FARMOUNT_BN_SURFACECOLOR, &KSceneSettingPageFarMount::OnBnClickedSurfaceColor)
END_MESSAGE_MAP()

int KSceneSettingPageFarMount::Update()
{
	SynFromInterfaceToInner();
	SynFromInnerToInterface();
	return TRUE;
}

BOOL KSceneSettingPageFarMount::OnSetActive()
{	
	m_nFlag = TRUE;

	UpdateTree();

	return CPropertyPage::OnSetActive();
}

HRESULT KSceneSettingPageFarMount::SynFromInterfaceToInner(DWORD_PTR dwOption/* = 0*/ )///这个函数比较界面并自己处理ModificationOption
{
	IEKG3DMeshFarMountain* pFarMount = GetValidFarMountPointer();
	
	///从界面同步数据到内部结构体
	KG_PROCESS_ERROR (pFarMount );
	UpdateData( TRUE );

	///g_GetFullPath( m_DlgStruct.m_tczTextureFullPath, (LPCTSTR)m_csTexPath );
	///现在FullPath的问题由FarMount自己处理，传进去的是非FullPath
	if ( m_csTexPath.GetLength() < MAX_PATH && 0 != _tcscmp(m_DlgStruct.m_strTextureFullPath, (LPCTSTR)m_csTexPath) )
	{
		_tcscpy_s( m_DlgStruct.m_strTextureFullPath, MAX_PATH, (LPCTSTR)m_csTexPath );
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_TEXPATH;
	}
	if ( MT_FLOAT_UNEQUAL(m_DlgStruct.m_fRadius, m_fRadius) )
	{
		m_DlgStruct.m_fRadius = m_fRadius;
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_MESH;
	}
	if ( MT_FLOAT_UNEQUAL(m_DlgStruct.m_fHeight, m_fHeight) )
	{
		m_DlgStruct.m_fHeight = m_fHeight;
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_MESH;
	}
	if ( MT_FLOAT_UNEQUAL(m_DlgStruct.m_fZRotation, m_fZRotation) )
	{
		m_DlgStruct.m_fZRotation = m_fZRotation;
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_MESH;
	}
	if ( MT_FLOAT_UNEQUAL(m_DlgStruct.m_fTexCordUTiles,m_fTexU) )
	{
		m_DlgStruct.m_fTexCordUTiles = m_fTexU;
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_MESH;
	}
	if ( MT_FLOAT_UNEQUAL(m_DlgStruct.m_fBotmZPos, m_fBotmZPos) )
	{
		m_DlgStruct.m_fBotmZPos = m_fBotmZPos;
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_MESH;
	}
	if ( MT_FLOAT_UNEQUAL(m_DlgStruct.m_fBotmBlendHeight,m_fBotmBlendHeight) )
	{
		m_DlgStruct.m_fBotmBlendHeight = m_fBotmBlendHeight;
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_TEX;
	}
	if ( MT_FLOAT_UNEQUAL(m_DlgStruct.m_fFogBlendFactor, m_fFogBlendFactor) )
	{
		m_DlgStruct.m_fFogBlendFactor = m_fFogBlendFactor;
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_TEX;
	}
	
    if (MT_FLOAT_UNEQUAL(m_DlgStruct.m_fTexFactor, m_fTexFactor))
    {
        m_DlgStruct.m_fTexFactor = m_fTexFactor;
        m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_TEXFACTOR;
    }
	/*if (m_DlgStruct.m_d3dc_Color != m_bnColor.GetBkColor() )
	{
		m_DlgStruct.m_d3dc_Color = m_bnColor.GetBkColor();
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_TEX;
	}*/
	
	if ( m_DlgStruct.m_d3dc_FogColor != m_bnFogColor.GetBkColor() )
	{
		m_DlgStruct.m_d3dc_FogColor = m_bnFogColor.GetBkColor();
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_TEX;
	}
	bool bEnableTexColor = BST_CHECKED == IsDlgButtonChecked(IDC_CK_ENABLE_COLOR);
	//if (m_DlgStruct.m_bEnableTexColor != bEnableTexColor)
	{
		m_DlgStruct.m_bEnableTexColor = bEnableTexColor;
		m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_ENABLETEXCOLOR;
	}

	{
		CString csTemp;
		if (this->GetDlgItemText(IDC_ED_SPEED, csTemp))
		{
			float fSpeed = 0;
			if (KG_CUSTOM_HELPERS::TStringToValue(csTemp, fSpeed))
			{
				if (fSpeed != m_DlgStruct.m_fSpeed)
				{
					m_DlgStruct.m_fSpeed = fSpeed;
					m_DlgStruct.m_dwModificationOption |= KG3DMESHFARMOUNTAIN_MDO_SPEED;
				}
			}
		}
	}

	if (dwOption != 0)
	{
		m_DlgStruct.m_dwModificationOption = _kg_to_smaller_size_cast<DWORD>(dwOption);
	}
	//KSceneSettingPageKeyEditBase::SynFromInterfaceToInner((DWORD_PTR)&m_DlgStruct);
	///选择刷新
	return pFarMount->RequestModification(&m_DlgStruct);
Exit0:
	return E_FAIL;
}

HRESULT KSceneSettingPageFarMount::SynFromInnerToInterface(DWORD_PTR dwOption /*= 0 */)
{
	///同步内部数据到结构体和界面
	IEKG3DMeshFarMountain* pFarMount = GetValidFarMountPointer();
	KG_PROCESS_ERROR(pFarMount);
	KG_COM_PROCESS_ERROR(pFarMount->GetStruct(&m_DlgStruct));
	///g_GetFilePathFromFullPath( m_csTexPath.GetBuffer(MAX_PATH), m_DlgStruct.m_tczTextureFullPath );///这是CString的用法之一///这个函数记得检查返回
	///m_csTexPath.ReleaseBuffer();///现在FullPath的问题由FarMount自己处理，传进去的是非FullPath
	m_csTexPath = m_DlgStruct.m_strTextureFullPath;
	m_fRadius = m_DlgStruct.m_fRadius;
	m_fHeight = m_DlgStruct.m_fHeight;
	m_fZRotation = m_DlgStruct.m_fZRotation;
	m_fTexU = m_DlgStruct.m_fTexCordUTiles;
	m_fBotmZPos = m_DlgStruct.m_fBotmZPos;
	m_fBotmBlendHeight = m_DlgStruct.m_fBotmBlendHeight;
	m_fFogBlendFactor = m_DlgStruct.m_fFogBlendFactor;
    m_fTexFactor = m_DlgStruct.m_fTexFactor;

	/*{
		D3DCOLOR fogColor = D3DCOLOR_XRGB(128, 128, 128);
		IEKG3DEnvEffFog* pFog = KSH::GetEngineInterfaceEx<IEKG3DEnvEffFog>();
		if (NULL != pFog)
			pFog->GetFogColor(&fogColor);
		m_bnColor.SetBKColor(fogColor);
	}*/

	
	m_bnFogColor.SetBKColor( m_DlgStruct.m_d3dc_FogColor );

	KSCENEEDITOR_HELPERCLASSES::SynACheckWndWithABool(this->GetDlgItem(IDC_CK_ENABLE_COLOR)
		, m_DlgStruct.m_bEnableTexColor, TRUE);

	TCHAR tempString[128] = _T("");
	ZeroMemory(tempString, sizeof(tempString));
	KG_CUSTOM_HELPERS::TValueToString(m_DlgStruct.m_fSpeed, tempString);
	this->SetDlgItemText(IDC_ED_SPEED, tempString);

	UpdateData( FALSE );

	//KSceneSettingPageKeyEditBase::SynFromInnerToInterface((DWORD_PTR)&m_DlgStruct);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KSceneSettingPageFarMount::RestoreDefaultInterface()
{
	m_csTexPath = _T("");
	m_fRadius = 0;
	m_fHeight = 0;
	m_fZRotation = 0.0;
	m_fTexU = 0.0;
	m_fBotmZPos = 0;
	m_fBotmBlendHeight = 0;
	m_fFogBlendFactor = 0.0;
	//m_bnColor.SetBKColor( D3DCOLOR_XRGB(255, 255, 255) );
	m_bnFogColor.SetBKColor( D3DCOLOR_XRGB(255, 255, 255) );
	return S_OK;
}
HRESULT KSceneSettingPageFarMount::UpdateTree()
{
	HRESULT hr = E_FAIL;
	TCHAR tczNameBuffer[64];

	m_Tree.DeleteAllItems();

	IEKG3DSkySystem* pSkySystem = KSCENEEDITOR_HELPERCLASSES::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
	KG_PROCESS_ERROR(pSkySystem);
   
	int nVecFarMountSize = 0;
	hr = pSkySystem->GetFarMountVecSize( &nVecFarMountSize );
	KG_COM_PROCESS_ERROR( hr );
	for ( int i =0 ; i < nVecFarMountSize; i++ )
	{
		IEKG3DMeshFarMountain* pFarMount = NULL;
		if (SUCCEEDED(pSkySystem->GetFarMount(i, &pFarMount)))
		{
			_ASSERTE( NULL != pFarMount );
			if ( SUCCEEDED(pFarMount->GetName(tczNameBuffer, countof(tczNameBuffer))))
			{
				HTREEITEM hAdd = m_Tree.InsertItem(tczNameBuffer);// pFarMount->Name());
				if ( hAdd )
				{
					m_Tree.SetItemData( hAdd, (DWORD_PTR)pFarMount );
				}
			}			
		
		}//if (SUCCEEDED(pSkySystem->GetFarMount(i, &pFarMount)))	
	}
	HTREEITEM hHead = m_Tree.GetRootItem();
	if (hHead)
		m_Tree.SelectItem(hHead);
	UpdateData( FALSE );

//Exit1:
	return TRUE;
Exit0:
	return E_FAIL;
}
// KSceneSettingPageFarMount message handlers

void KSceneSettingPageFarMount::OnBnClickedSsFarmountBnAdd()
{
	// TODO: Add your control notification handler code here
	IEKG3DSkySystem* piSkySystem = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
	IEKG3DMeshFarMountain* pAdded = NULL;
	if(NULL == piSkySystem)
		return;
	HRESULT hr = piSkySystem->AddFarMount( (KG3DMESHFARMOUNTAIN_DESCSTRUCT*)NULL, &pAdded);
	if (SUCCEEDED(hr) )///用默认参数
	{
		UpdateTree();
		//CheckFogColorSyn(pAdded);
	}
	
	SynFromInnerToInterface();
}

void KSceneSettingPageFarMount::OnBnClickedSsFarmountBnDel()
{
	// TODO: Add your control notification handler code here
	int nIndex = GetSeletedFarMountIndex();
	if ( -1 != nIndex )
	{
		IEKG3DSkySystem* piSkyStem =  KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
		if (piSkyStem && SUCCEEDED(piSkyStem->DelFarMount( nIndex ) ) )
		{
			UpdateTree();
		}
	}	
}

void KSceneSettingPageFarMount::OnBnClickedSsFarmountBnUp()
{
	// TODO: Add your control notification handler code here
	int nIndex = GetSeletedFarMountIndex();
	if ( -1 != nIndex )
	{
		IEKG3DSkySystem* piSkyStem =  KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();

		if (piSkyStem && SUCCEEDED(piSkyStem->UpperFarMount( nIndex ) ) )
		{
			UpdateTree();
		}
	}	
}

void KSceneSettingPageFarMount::OnBnClickedSsFarmountBnDown()
{
	// TODO: Add your control notification handler code here
	int nIndex = GetSeletedFarMountIndex();
	if ( -1 != nIndex )
	{
        IEKG3DSkySystem* piSkyStem = KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
		if (piSkyStem && SUCCEEDED( piSkyStem->LowerFarMount( nIndex ) ) )
		{
			UpdateTree();
		}
	}	
}

void KSceneSettingPageFarMount::OnTvnSelchangedSsFarmountTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
	if (SUCCEEDED(SynFromInnerToInterface()))
	{
		NULL;
	}
	else		
	{
		RestoreDefaultInterface();
	}
}

void KSceneSettingPageFarMount::OnBnClickedSsFarmountBnTexpath()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK) 
	{
		TCHAR tempPath[MAX_PATH];
		*tempPath = '\0';
		BOOL bRet = g_GetFilePathFromFullPath( tempPath, (LPCTSTR)dlg.GetPathName() );///这个函数记得检查返回
		if (bRet)
		{
			m_edTexPath.SetWindowText( tempPath );
			
			SynFromInterfaceToInner(KG3DMESHFARMOUNTAIN_MDO_TEXPATH);
			SynFromInnerToInterface();
			return;
		}
		MessageBox(_T("设定贴图路径失败"), _T("提示"), MB_OK | MB_ICONINFORMATION);	
	}	
}

void KSceneSettingPageFarMount::OnBnClickedSsFarmountBnUpdatemesh()
{
	// TODO: Add your control notification handler code here
	
	SynFromInterfaceToInner(KG3DMESHFARMOUNTAIN_MDO_MESH);
	SynFromInnerToInterface();
}

void KSceneSettingPageFarMount::OnBnClickedSsFarmountBnUpdateblend()
{
	// TODO: Add your control notification handler code here
	
	SynFromInterfaceToInner(KG3DMESHFARMOUNTAIN_MDO_TEX);
	SynFromInnerToInterface();
}

/*
void KSceneSettingPageFarMount::OnBnClickedSsFarmountBnUpdateall()
{
	// TODO: Add your control notification handler code here
	
	Update();
}
*/

int KSceneSettingPageFarMount::GetSeletedFarMountIndex( IEKG3DMeshFarMountain* pFarMount /*= NULL */ )
{
	IEKG3DMeshFarMountain* pTempFarMount = pFarMount;
	if ( NULL == pTempFarMount )
	{
		HTREEITEM hSel = m_Tree.GetSelectedItem();
		if ( NULL != hSel )
		{
			pTempFarMount = (IEKG3DMeshFarMountain*)m_Tree.GetItemData( hSel ); 
		}
	}
	if ( NULL == pTempFarMount )
	{
		return -1;
	}
	int nIndex = -1;
    
	IEKG3DSkySystem* piSkyStem =  KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();

	piSkyStem->GetFarMountIndexByPointer(pTempFarMount, &nIndex);	
	return  nIndex;
}

IEKG3DMeshFarMountain* KSceneSettingPageFarMount::GetValidFarMountPointer( IEKG3DMeshFarMountain* pFarMount /*= NULL */ )
{
	IEKG3DMeshFarMountain* pTempFarMount = pFarMount;
	if ( NULL == pTempFarMount )
	{
		HTREEITEM hSel = m_Tree.GetSelectedItem();
		if ( NULL != hSel )
		{
			pTempFarMount = (IEKG3DMeshFarMountain*)m_Tree.GetItemData( hSel ); 
		}			
	}
	KGLOG_PROCESS_ERROR( NULL != pTempFarMount );
	BOOL bTemp = FALSE;
    
	IEKG3DSkySystem* piSkyStem =  KSH::GetEngineInterface<KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM>();
	KG_PROCESS_ERROR(piSkyStem);
	piSkyStem->IsFarMountPointerValid( pTempFarMount, &bTemp );
	if ( TRUE ==  bTemp )
	{
		_ASSERTE( pTempFarMount );
		return pTempFarMount;
	}
Exit0:
	return NULL;
}


void KSceneSettingPageFarMount::OnBnClickedCkEnableColor()
{
	// TODO: Add your control notification handler code here
	SynFromInterfaceToInner(KG3DMESHFARMOUNTAIN_MDO_ENABLETEXCOLOR);
	SynFromInnerToInterface();
}

/*
void KSceneSettingPageFarMount::OnEnChangeEdSpeed()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the KSceneSettingPageKeyEditBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	
}*/


void KSceneSettingPageFarMount::OnBnClickedSsBnUpdateother()
{
	// TODO: Add your control notification handler code here
	SynFromInterfaceToInner(KG3DMESHFARMOUNTAIN_MDO_SPEED);
	SynFromInnerToInterface();
}

//VOID KSceneSettingPageFarMount::CheckFogColorSyn(IEKG3DMeshFarMountain* pFarMount)
//{
//	if(NULL == pFarMount)
//		return;
//		
//	IEKG3DEnvEffFog* pFog = KSH::GetEngineInterfaceEx<IEKG3DEnvEffFog>();
//	if(! pFog)
//		return;
//
//	D3DCOLOR FogColor;
//	pFog->GetFogColor(&FogColor);
//	
//	KG3DMESHFARMOUNTAIN_DESCSTRUCT	Desc;
//	HRESULT hr = pFarMount->GetStruct(&Desc);
//	if (FAILED(hr))
//		return;
//
//	Desc.m_d3dc_Color = FogColor;
//	Desc.m_dwModificationOption = KG3DMESHFARMOUNTAIN_MDO_TEX;
//
//	pFarMount->RequestModification(&Desc);
//}

//void KSceneSettingPageFarMount::OnBnClickedSurfaceColor()
//{
//	if (! m_bnColor.IsColorPanelPoppingEnabled())
//	{
//		MessageBox(_T("当前处于同步雾色状态中，不能改这个颜色，可以通过改雾色来同步更改这个颜色\
//					  ，或者取消同步雾色之后也能更改这个颜色，请在雾面板操作"));
//	}
//}
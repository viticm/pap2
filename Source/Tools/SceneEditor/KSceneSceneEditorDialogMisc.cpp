// KSceneSceneEditorDialogMisc.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogMisc.h"
#include ".\kscenesceneeditordialogmisc.h"
//#include "KG3DSceneSceneEditor.h"
//#include "KG3DGraphiceEngine.h"
#include "KColorDialog.h"

#include "IEEditor.h"
#include "IEKG3DScene.h"
#include "IEKG3DEnvironment.h"
#include "IEKG3DEnvEffFog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


// KSceneSceneEditorDialogMisc dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogMisc, CDialog)
KSceneSceneEditorDialogMisc::KSceneSceneEditorDialogMisc(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogMisc::IDD, pParent)
	, m_nLogicalXStart(0)
	, m_nLogicalZStart(0)
	, m_nLogicalWidth(0)
	, m_nLogicalHeight(0)
{
	m_lpSceneSceneEditor = NULL;
	m_fLod = 0;
	m_fFogNear = 5000;
	m_fFogFar = 10000;
	m_fFogDes = 1.0F;
	m_dwFogModificationOption = 0;

	m_bCheckModel = FALSE;
	m_bCheckModelST = FALSE;
	m_bCheckSFX = FALSE;
	m_bCheckStree = FALSE;
	m_bCheckLogicObj = FALSE;
	m_bCheckObjSet = FALSE;
	m_bCheckUnSmallObj = FALSE;
	m_bCheckUnBigOBj = FALSE;
	m_dwLimit = 0;
	
}

KSceneSceneEditorDialogMisc::~KSceneSceneEditorDialogMisc()
{
}

void KSceneSceneEditorDialogMisc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOD, m_fLod);
	DDX_Text(pDX, IDC_EDIT_FOGNEAR, m_fFogNear);
	DDX_Text(pDX, IDC_EDIT_FOGFAR, m_fFogFar);
	DDX_Text(pDX, IDC_EDIT_FOGDES, m_fFogDes);
	DDX_Control(pDX, IDC_BUTTON_FOG_COLORPICKER, m_bnFogColorPicker);
	DDX_Text(pDX, IDC_EDIT_LOGICALX, m_nLogicalXStart);
	DDX_Text(pDX, IDC_EDIT_LOGICALZ, m_nLogicalZStart);
	DDX_Text(pDX, IDC_EDIT_LOGICALWIDTH, m_nLogicalWidth);
	DDX_Text(pDX, IDC_EDIT_LOGICALHEIGHT, m_nLogicalHeight);
	DDX_Check(pDX,IDC_CHECK_MODEL,m_bCheckModel);
	DDX_Check(pDX,IDC_CHECK_MODELST,m_bCheckModelST);
	DDX_Check(pDX,IDC_CHECK_STREE,m_bCheckStree);
	DDX_Check(pDX,IDC_CHECK_SFX,m_bCheckSFX);
	DDX_Check(pDX,IDC_CHECK_LOGICOBJ,m_bCheckLogicObj);
	DDX_Check(pDX,IDC_CHECK_GROUNP,m_bCheckObjSet);
	DDX_Check(pDX,IDC_CHECK_UNSMALLOBJ,m_bCheckUnSmallObj);
	DDX_Check(pDX,IDC_CHECK_UNBIGOBJ,m_bCheckUnBigOBj);
	DDX_Text(pDX,IDC_EDIT_LIMIT,m_dwLimit);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogMisc, CDialog)
	ON_BN_CLICKED(IDC_CHECK_MIPMAP, OnBnClickedCheckMipmap)
	ON_BN_CLICKED(IDC_CHECK_MAGMAP, OnBnClickedCheckMagmap)
	ON_BN_CLICKED(IDC_CHECK_MINMAP, OnBnClickedCheckMinmap)
	ON_EN_CHANGE(IDC_EDIT_LOD, OnEnChangeEditLod)
	ON_EN_CHANGE(IDC_EDIT_FOGNEAR, OnEnChangeEditFogNear)
	ON_EN_CHANGE(IDC_EDIT_FOGFAR, OnEnChangeEditFogFar)
	ON_EN_CHANGE(IDC_EDIT_FOGDES, OnEnChangeEditFogDes)
	ON_BN_CLICKED(IDC_BUTTON_FOGCOLOR, OnBnClickedButtonFogcolor)
	ON_BN_CLICKED(IDC_BN_FOG_UPDATE_FOG_PARAM, &KSceneSceneEditorDialogMisc::OnBnClickedBnFogUpdateFogParam)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_SHOWLOGICALSIZE, &KSceneSceneEditorDialogMisc::OnBnClickedCheckShowlogicalsize)
	ON_EN_CHANGE(IDC_EDIT_LOGICALX, &KSceneSceneEditorDialogMisc::OnEnChangeEditLogicalx)
	ON_EN_CHANGE(IDC_EDIT_LOGICALZ, &KSceneSceneEditorDialogMisc::OnEnChangeEditLogicalz)
	ON_EN_CHANGE(IDC_EDIT_LOGICALWIDTH, &KSceneSceneEditorDialogMisc::OnEnChangeEditLogicalwidth)
	ON_EN_CHANGE(IDC_EDIT_LOGICALHEIGHT, &KSceneSceneEditorDialogMisc::OnEnChangeEditLogicalheight)
	ON_BN_CLICKED(IDC_CHECK_MODEL, &KSceneSceneEditorDialogMisc::OnBnClickedCheckModel)
	ON_BN_CLICKED(IDC_CHECK_MODELST, &KSceneSceneEditorDialogMisc::OnBnClickedCheckModelst)
	ON_BN_CLICKED(IDC_CHECK_SFX, &KSceneSceneEditorDialogMisc::OnBnClickedCheckSfx)
	ON_BN_CLICKED(IDC_CHECK_STREE, &KSceneSceneEditorDialogMisc::OnBnClickedCheckStree)
	ON_BN_CLICKED(IDC_CHECK_LOGICOBJ, &KSceneSceneEditorDialogMisc::OnBnClickedCheckLogicobj)
	ON_BN_CLICKED(IDC_CHECK_GROUNP, &KSceneSceneEditorDialogMisc::OnBnClickedCheckGrounp)
	ON_BN_CLICKED(IDC_CHECK_UNSMALLOBJ, &KSceneSceneEditorDialogMisc::OnBnClickedCheckUnsmallobj)
	ON_BN_CLICKED(IDC_CHECK_UNBIGOBJ, &KSceneSceneEditorDialogMisc::OnBnClickedCheckUnbigobj)
	ON_EN_CHANGE(IDC_EDIT_LIMIT, &KSceneSceneEditorDialogMisc::OnEnChangeEditLimit)
	ON_BN_CLICKED(IDC_FOG_ENABLE, &KSceneSceneEditorDialogMisc::OnBnClickedFogEnable)
END_MESSAGE_MAP()


// KSceneSceneEditorDialogMisc message handlers

void KSceneSceneEditorDialogMisc::OnBnClickedCheckMipmap()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	if(IsDlgButtonChecked(IDC_CHECK_MIPMAP))
		m_lpSceneSceneEditor->SetSampleMip(D3DTEXF_LINEAR);//m_Sample_Mip = D3DTEXF_LINEAR;
	else
		m_lpSceneSceneEditor->SetSampleMip(D3DTEXF_NONE);//m_Sample_Mip = D3DTEXF_NONE;
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckMagmap()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	if(IsDlgButtonChecked(IDC_CHECK_MAGMAP))
		m_lpSceneSceneEditor->SetSampleMag(D3DTEXF_LINEAR);// m_Sample_Mag = D3DTEXF_LINEAR;
	else
		m_lpSceneSceneEditor->SetSampleMag(D3DTEXF_POINT);//m_Sample_Mag = D3DTEXF_POINT;
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckMinmap()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	if(IsDlgButtonChecked(IDC_CHECK_MINMAP))
		m_lpSceneSceneEditor->SetSampleMin(D3DTEXF_LINEAR);//m_Sample_Min = D3DTEXF_LINEAR;
	else
		m_lpSceneSceneEditor->SetSampleMin(D3DTEXF_POINT);//m_Sample_Min = D3DTEXF_POINT;
}

void KSceneSceneEditorDialogMisc::OnEnChangeEditLod()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	UpdateData(TRUE);

    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    //Get3DEngineInterface((void **)&piManager);
    ASSERT(piManager);

	KG3DEngineCaps Caps;
	HRESULT hr = piManager->GetEngineCaps(&Caps);
	_ASSERTE(SUCCEEDED(hr));

	if(Caps.D3DCaps.RasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS)
	{
		m_lpSceneSceneEditor->SetMipmapLod(m_fLod);//m_fMipmapLod = m_fLod;
	}
	else
	{
		GetDlgItem(IDC_EDIT_LOD)->EnableWindow(FALSE);
	}


}

#define GETSCENE() \
	IEKG3DScene* pScene = NULL;\
	m_lpSceneSceneEditor->GetScene(&pScene);\
	_ASSERTE(pScene); \
	if(!pScene) return



void KSceneSceneEditorDialogMisc::OnEnChangeEditFogNear()
{
	m_dwFogModificationOption |= KG3DENVEFFFOG_MDO_MODE_PARAM;
}
void KSceneSceneEditorDialogMisc::OnEnChangeEditFogFar()
{
	m_dwFogModificationOption |= KG3DENVEFFFOG_MDO_MODE_PARAM;
}
void KSceneSceneEditorDialogMisc::OnEnChangeEditFogDes()
{
	m_dwFogModificationOption |= KG3DENVEFFFOG_MDO_MODE_PARAM;
}

void KSceneSceneEditorDialogMisc::OnOK()
{

}
void KSceneSceneEditorDialogMisc::OnBnClickedButtonFogcolor()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;

	/*KColorPickerDialog* pDlg = GetColorPickerDialog();
	pDlg->Init(&m_lpSceneSceneEditor->m_Fog.m_FogColor,"Bloom Gate Color",NULL,TRUE);
	pDlg->ShowWindow(TRUE);*/
	GETSCENE();
	IEKG3DEnvEffFog* pTempFog = GetFog(pScene);
	if (NULL != pTempFog)
	{
		pTempFog->SetFogColor( m_bnFogColorPicker.GetBkColor());
	}		
	
}

BOOL KSceneSceneEditorDialogMisc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if ( m_lpSceneSceneEditor )
	{
        D3DCOLOR FogColor = 0;
        GETSCENE() FALSE;
		IEKG3DEnvEffFog* pTempFog = GetFog(pScene);
		if (NULL != pTempFog)
		{
			if(SUCCEEDED(pTempFog->GetFogColor(&FogColor)))
			{
				m_bnFogColorPicker.SetBKColor(FogColor);
			}
		}	

		UpdateSelectOption(FALSE);

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

IEKG3DEnvEffFog* KSceneSceneEditorDialogMisc::GetFog( IEKG3DScene* pScene )
{
	_ASSERTE(pScene);
	IEKG3DEnvironment* pEnvironment = NULL;
	IEKG3DEnvEffFog* pFog = NULL;
	if(!m_lpSceneSceneEditor)
		return NULL;
	if (SUCCEEDED(pScene->GetEnvironment(&pEnvironment)))
	{
		IEKG3DEnvEffect* piEffect = NULL;
		if (SUCCEEDED(pEnvironment->GetEnvEffect(KG3DTYPE_ENVEFF_FOG, 0, &piEffect)))
		{
			if (SUCCEEDED(piEffect->ToSolidInterface(reinterpret_cast<LPVOID*>(&pFog))))
			{
				_ASSERTE(pFog);
				return pFog;
			}		
		}
	}
	return NULL;
	
}
void KSceneSceneEditorDialogMisc::OnBnClickedBnFogUpdateFogParam()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_lpSceneSceneEditor)
		return;
	KG3DENVEFFFOG_DESCSTRUCT tempStruct;
	tempStruct.m_fFogDensity = m_fFogDes;
	tempStruct.m_fFogStartValue = m_fFogNear;
	tempStruct.m_fFogEndValue = m_fFogFar;
	GETSCENE();
	IEKG3DEnvEffFog* pTempFog = GetFog(pScene);
	if (NULL != pTempFog)
	{
		pTempFog->RequestModification(&tempStruct, m_dwFogModificationOption | KG3DENVEFFFOG_MDO_MODE_PARAM);
		m_dwFogModificationOption = 0;
	}		
}
void KSceneSceneEditorDialogMisc::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if (bShow)
	{
		if ( m_lpSceneSceneEditor )
		{
			UpdateFogUI();

			GETSCENE();
			pScene->GetLogicalSceneSize(&m_nLogicalXStart,&m_nLogicalZStart,&m_nLogicalWidth,&m_nLogicalHeight);
			
			UpdateData(FALSE);
		}
	}
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckShowlogicalsize()
{
	int nCheck = ((CButton*)GetDlgItem(IDC_CHECK_SHOWLOGICALSIZE))->GetCheck();
	if(m_lpSceneSceneEditor)
	{
		m_lpSceneSceneEditor->EnableShowLogicalSize(nCheck);
	}
}
void KSceneSceneEditorDialogMisc::OnEnChangeEditLogicalx()
{
	UpdataLogicalSize();
}

void KSceneSceneEditorDialogMisc::OnEnChangeEditLogicalz()
{
	UpdataLogicalSize();
}

void KSceneSceneEditorDialogMisc::OnEnChangeEditLogicalwidth()
{
	UpdataLogicalSize();
}

void KSceneSceneEditorDialogMisc::OnEnChangeEditLogicalheight()
{
	UpdataLogicalSize();
}

void KSceneSceneEditorDialogMisc::UpdataLogicalSize()
{
	if(m_lpSceneSceneEditor)
	{
		UpdateData(TRUE);

		GETSCENE();

		pScene->SetLogicalSceneSize(m_nLogicalXStart,m_nLogicalZStart,m_nLogicalWidth,m_nLogicalHeight);
	}
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckModel()
{
	// TODO: Add your control notification handler code here
	UpdateSelectOption();
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckModelst()
{
	// TODO: Add your control notification handler code here
	UpdateSelectOption();
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckSfx()
{
	// TODO: Add your control notification handler code here
	UpdateSelectOption();
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckStree()
{
	// TODO: Add your control notification handler code here
	UpdateSelectOption();
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckLogicobj()
{
	// TODO: Add your control notification handler code here
	UpdateSelectOption();
}
void KSceneSceneEditorDialogMisc::OnBnClickedCheckGrounp()
{
	// TODO: Add your control notification handler code here
	UpdateSelectOption();
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckUnsmallobj()
{
	// TODO: Add your control notification handler code here
	UpdateSelectOption();
}

void KSceneSceneEditorDialogMisc::OnBnClickedCheckUnbigobj()
{
	// TODO: Add your control notification handler code here
	UpdateSelectOption();
}

void KSceneSceneEditorDialogMisc::UpdateSelectOption(BOOL bSaveAndValidate)
{
	if(!m_lpSceneSceneEditor)
		return ; 
	DWORD dwOption = 0;
	if (bSaveAndValidate)
	{
		UpdateData();
		if(m_bCheckModel)
			dwOption |= EM_SELMASK_MODEL;
		if(m_bCheckModelST)
			dwOption |= EM_SELMASK_MODELSET;
		if(m_bCheckSFX)
			dwOption |= EM_SELMASK_SFX;
		if(m_bCheckStree)
			dwOption |= EM_SELMASK_SPEEDTREE;
		if (m_bCheckLogicObj)
			dwOption |= EM_SELMASK_LOGICOBJ;
		if (m_bCheckObjSet)
			dwOption |= EM_SELMASK_OBJSET;
		if (m_bCheckUnBigOBj)
			dwOption |= EM_SELMASK_NOBIGOBJ;
		if (m_bCheckUnSmallObj)
			dwOption |= EM_SELMASK_NOSMALLOBJ;
		m_lpSceneSceneEditor->SetSelectOption(dwOption);

	}
	else
	{
		DWORD dwLimit = m_lpSceneSceneEditor->GetLimitValue();
		m_dwLimit = dwLimit / 100 ;
		dwOption = m_lpSceneSceneEditor->GetSelectOption();
		m_bCheckModel     = !!(dwOption & EM_SELMASK_MODEL);
		m_bCheckModelST   = !!(dwOption & EM_SELMASK_MODELSET);
		m_bCheckSFX       = !!(dwOption & EM_SELMASK_SFX);
		m_bCheckStree     = !!(dwOption & EM_SELMASK_SPEEDTREE);
	    m_bCheckLogicObj  = !!(dwOption & EM_SELMASK_LOGICOBJ);
		m_bCheckObjSet    = !!(dwOption & EM_SELMASK_OBJSET);
		m_bCheckUnBigOBj  = !!(dwOption & EM_SELMASK_NOBIGOBJ);
		m_bCheckUnSmallObj= !!(dwOption & EM_SELMASK_NOSMALLOBJ);
		UpdateData(FALSE);
	}
}


void KSceneSceneEditorDialogMisc::OnEnChangeEditLimit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	if(!m_lpSceneSceneEditor)
		return ;
	DWORD dwLimit= m_dwLimit *100;
	m_lpSceneSceneEditor->SetLimitValue(dwLimit);

	
}

void KSceneSceneEditorDialogMisc::OnBnClickedFogEnable()
{
	CButton* p = static_cast<CButton*>(this->GetDlgItem(IDC_FOG_ENABLE));
	if (NULL == p)
		return;
	
	IEKG3DEnvEffFog* pFog = KSH::GetEngineInterfaceEx<IEKG3DEnvEffFog>();
	if(NULL == pFog)
		return;

	KG3DENVEFFFOG_DESCSTRUCT desc;
	HRESULT hr = pFog->GetStruct(&desc);
	if(FAILED(hr))
		return;

	desc.m_bFogEnable =  BST_CHECKED == p->GetCheck();

	pFog->RequestModification(&desc, KG3DENVEFFFOG_MDO_FOG_BASE);	

	UpdateFogUI();
}

void KSceneSceneEditorDialogMisc::UpdateFogUI()
{
	D3DCOLOR FogColor = 0;
	GETSCENE();
	IEKG3DEnvEffFog* pTempFog = GetFog(pScene);
	if (NULL != pTempFog)
	{
		KG3DENVEFFFOG_DESCSTRUCT tempStruct;
		if(SUCCEEDED(pTempFog->GetStruct(&tempStruct)))
		{
			m_bnFogColorPicker.SetBKColor(tempStruct.m_FogColor);
			m_fFogFar = tempStruct.m_fFogEndValue;
			m_fFogNear = tempStruct.m_fFogStartValue;
			m_fFogDes = tempStruct.m_fFogDensity;

			CButton* p = static_cast<CButton*>(this->GetDlgItem(IDC_FOG_ENABLE));
			if (NULL != p)
			{
				p->SetCheck(tempStruct.m_bFogEnable ? BST_CHECKED : BST_UNCHECKED);
			}
			UpdateData(FALSE);
		}
	}
}


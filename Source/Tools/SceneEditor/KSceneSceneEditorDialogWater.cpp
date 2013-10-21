// KSceneSceneEditorDialogWater.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogWater.h"
#include "kscenesceneeditordialogwater.h"
#include "IEEditor.h"
#include "IEKG3DTerrain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneSceneEditorDialogWater dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogWater, CDialog)
KSceneSceneEditorDialogWater::KSceneSceneEditorDialogWater(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogWater::IDD, pParent)
	, m_fHeight(0)
	, m_fAlpha(0)
{
	m_lpSceneSceneEditor = NULL;
	m_fBumpNear = 0;
	m_fBumpFar = 0;
}

KSceneSceneEditorDialogWater::~KSceneSceneEditorDialogWater()
{
}


void KSceneSceneEditorDialogWater::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BUMPNEAR, m_fBumpNear);
	DDX_Text(pDX, IDC_EDIT_BUMPFAR, m_fBumpFar);
	DDX_Text(pDX, IDC_EDIT1, m_fHeight);
	DDX_Text(pDX, IDC_EDIT2, m_fAlpha);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogWater, CDialog)
	ON_BN_CLICKED(IDC_CHECK_WATERUP,OnBnClickedCheckWaterUp)
	ON_BN_CLICKED(IDC_CHECK_WATERDOWN,OnBnClickedCheckWaterDown)
	ON_BN_CLICKED(IDC_CHECK_WATERFLAT,OnBnClickedCheckWaterFlat)
	ON_BN_CLICKED(IDC_CHECK_SHOWREFTEXT, OnBnClickedCheckShowreftext)
   	ON_EN_CHANGE(IDC_EDIT_BUMPNEAR, OnEnChangeEditBumpNear)
	ON_EN_CHANGE(IDC_EDIT_BUMPFAR, OnEnChangeEditBumpFar)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD,OnRefreshFx)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_SETDEFHEIGHT, &KSceneSceneEditorDialogWater::OnBnClickedSetdefheight)
	ON_BN_CLICKED(IDC_CHECK_WATERGET, &KSceneSceneEditorDialogWater::OnBnClickedCheckWaterget)
	ON_BN_CLICKED(IDC_CHECK_WATERSET, &KSceneSceneEditorDialogWater::OnBnClickedCheckWaterset)
	ON_BN_CLICKED(IDC_CHECK_WATERFOLLEW, &KSceneSceneEditorDialogWater::OnBnClickedCheckWaterfollew)
	ON_EN_CHANGE(IDC_EDIT1, &KSceneSceneEditorDialogWater::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &KSceneSceneEditorDialogWater::OnEnChangeEdit2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_SHOWWATER, &KSceneSceneEditorDialogWater::OnBnClickedCheckShowwater)
	ON_BN_CLICKED(IDC_CHECK_WATERRERACT, &KSceneSceneEditorDialogWater::OnBnClickedCheckWaterreract)
	//ON_BN_CLICKED(IDC_SETDEFHEIGHT2, &KSceneSceneEditorDialogWater::OnBnClickedSetdefheight2)
	ON_WM_SHOWWINDOW()

	ON_BN_CLICKED(IDC_CHECK_ALWAYRENDER, &KSceneSceneEditorDialogWater::OnBnClickedCheckAlwayrender)
	ON_BN_CLICKED(IDC_CHECK_WATEREDGE, &KSceneSceneEditorDialogWater::OnBnClickedCheckWateredge)
END_MESSAGE_MAP()


void KSceneSceneEditorDialogWater::OnBnClickedCheckShowreftext()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;

    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    //Get3DEngineInterface((void **)&piManager);
    ASSERT(piManager);
    KG3DEngineOption engineOption;
    piManager->GetEngineOption(&engineOption);   

    //m_lpSceneSceneEditor->m_lpTerrain->m_bWaterReflect = !m_lpSceneSceneEditor->m_lpTerrain->m_bWaterReflect;
	//pOption->bWaterReflection = m_lpSceneSceneEditor->m_lpTerrain->m_bWaterReflect;
	m_lpSceneSceneEditor->SetWaterReflect(!m_lpSceneSceneEditor->GetWaterReflect());

	engineOption.bWaterReflection = m_lpSceneSceneEditor->GetWaterReflect();
	piManager->SetEngineOption(&engineOption);
}

void KSceneSceneEditorDialogWater::OnOK()
{

}

void KSceneSceneEditorDialogWater::OnEnChangeEditBumpNear()
{
	UpdateData(TRUE);
}

void KSceneSceneEditorDialogWater::OnEnChangeEditBumpFar()
{
	UpdateData(TRUE);
}

void KSceneSceneEditorDialogWater::OnTimer(UINT nIDEvent)
{
	if(!m_lpSceneSceneEditor)
		return;
    IEKG3DTerrain *pTerrainEx = NULL;
   
	BOOL bUpdata = FALSE;

	float fHeight = 0;
	m_lpSceneSceneEditor->GetTerrainModifyHeight(fHeight);
	if (fHeight != m_fHeight)
	{
		m_fHeight = fHeight;
		bUpdata = TRUE;
	}
	
	if(bUpdata)
		UpdateData(FALSE);
}

BOOL KSceneSceneEditorDialogWater::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetTimer(0,1000,NULL);

	//FillList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogWater::OnBnClickedCheckWaterUp()
{
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);
	m_lpSceneSceneEditor->SetTBModifierState(WATER_UP);

	CheckDlgButton(IDC_CHECK_WATERUP,TRUE);
	CheckDlgButton(IDC_CHECK_WATERDOWN,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFLAT,FALSE);
	CheckDlgButton(IDC_CHECK_WATERGET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERSET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFOLLEW,FALSE);

}

void KSceneSceneEditorDialogWater::OnBnClickedCheckWaterDown()
{
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(WATER_DOWN);
   
	CheckDlgButton(IDC_CHECK_WATERUP,FALSE);
	CheckDlgButton(IDC_CHECK_WATERDOWN,TRUE);
	CheckDlgButton(IDC_CHECK_WATERFLAT,FALSE);
	CheckDlgButton(IDC_CHECK_WATERGET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERSET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFOLLEW,FALSE);
}

void KSceneSceneEditorDialogWater::OnBnClickedCheckWaterFlat()
{
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(WATER_FLAT);
	
	CheckDlgButton(IDC_CHECK_WATERUP,FALSE);
	CheckDlgButton(IDC_CHECK_WATERDOWN,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFLAT,TRUE);
	CheckDlgButton(IDC_CHECK_WATERGET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERSET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFOLLEW,FALSE);
}

void KSceneSceneEditorDialogWater::OnRefreshFx()
{
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->RefreshWaterEffect();
}

void KSceneSceneEditorDialogWater::OnBnClickedButtonLoad()
{
}


void KSceneSceneEditorDialogWater::OnBnClickedSetdefheight()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
  

	if(MessageBox("你确定恢复水面默认高度？所有操作将丢失", "危险操作",
			 MB_YESNO | MB_ICONQUESTION) == IDNO)
			 return;

	/*if(IDOK !=MessageBox("你确定恢复水面默认高度？所有操作将丢失", "危险操作", MB_YESNO))
		return;*/
	m_lpSceneSceneEditor->SetTerrainWaterDefaultHeight(m_fHeight);
}

void KSceneSceneEditorDialogWater::OnBnClickedCheckWaterget()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(WATER_GET);

	CheckDlgButton(IDC_CHECK_WATERUP,FALSE);
	CheckDlgButton(IDC_CHECK_WATERDOWN,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFLAT,FALSE);
	CheckDlgButton(IDC_CHECK_WATERGET,TRUE);
	CheckDlgButton(IDC_CHECK_WATERSET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFOLLEW,FALSE);
}

void KSceneSceneEditorDialogWater::OnBnClickedCheckWaterset()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(WATER_SET);
	
	CheckDlgButton(IDC_CHECK_WATERUP,FALSE);
	CheckDlgButton(IDC_CHECK_WATERDOWN,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFLAT,FALSE);
	CheckDlgButton(IDC_CHECK_WATERGET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERSET,TRUE);
	CheckDlgButton(IDC_CHECK_WATERFOLLEW,FALSE);
}

void KSceneSceneEditorDialogWater::OnBnClickedCheckWaterfollew()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(WATER_FOLOW);

    CheckDlgButton(IDC_CHECK_WATERUP,FALSE);
	CheckDlgButton(IDC_CHECK_WATERDOWN,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFLAT,FALSE);
	CheckDlgButton(IDC_CHECK_WATERGET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERSET,FALSE);
	CheckDlgButton(IDC_CHECK_WATERFOLLEW,TRUE);
}

void KSceneSceneEditorDialogWater::OnEnChangeEdit1()
{
	if(!m_lpSceneSceneEditor)
		return;
   
	UpdateData(TRUE);
	m_lpSceneSceneEditor->SetTerrainModifyHeight(m_fHeight);
}

void KSceneSceneEditorDialogWater::OnEnChangeEdit2()
{
}

void KSceneSceneEditorDialogWater::OnBnClickedCheckShowwater()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor || NULL == g_pEngineManager)
		return;
	BOOL bVal = ((CButton*)GetDlgItem(IDC_CHECK_SHOWWATER))->GetCheck();
	
	if ( g_pEngineManager->GetRenderOption( RENDER_WATER ) )
	{
		g_pEngineManager->SetRenderOption( RENDER_WATER, FALSE);
	}
	else
	{
		g_pEngineManager->SetRenderOption( RENDER_WATER, TRUE );
	}

}

void KSceneSceneEditorDialogWater::OnBnClickedCheckWaterreract()
{
	if(!m_lpSceneSceneEditor)
		return;

    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    //Get3DEngineInterface((void **)&piManager);
    ASSERT(piManager);
    KG3DEngineOption engineOption;
    piManager->GetEngineOption(&engineOption);

// 	m_lpSceneSceneEditor->m_lpTerrain->m_bWaterRefract = !m_lpSceneSceneEditor->m_lpTerrain->m_bWaterRefract;
// 	pOption->bWaterRefraction = m_lpSceneSceneEditor->m_lpTerrain->m_bWaterRefract;
	m_lpSceneSceneEditor->SetWaterRefract(!m_lpSceneSceneEditor->GetWaterRefract());
	engineOption.bWaterRefraction = m_lpSceneSceneEditor->GetWaterRefract();
	piManager->SetEngineOption(&engineOption);
}

void KSceneSceneEditorDialogWater::OnBnClickedSetdefheight2()
{
}

void KSceneSceneEditorDialogWater::OnShowWindow(BOOL bShow, UINT nStatus)
{
}

void KSceneSceneEditorDialogWater::OnBnClickedCheckAlwayrender()
{
	if(!m_lpSceneSceneEditor)
		return;
	BOOL bVal =  m_lpSceneSceneEditor->GetAlwayRenderWater();
	m_lpSceneSceneEditor->SetAlwayRenderWater(!bVal);
}

void KSceneSceneEditorDialogWater::OnBnClickedCheckWateredge()
{
	// TODO: Add your control notification handler code here
	BOOL bRenderEdge = ((CButton*)(GetDlgItem(IDC_CHECK_WATEREDGE)))->GetCheck();
	if(m_lpSceneSceneEditor)
		m_lpSceneSceneEditor->SetRenderWaterEdge(bRenderEdge);
}

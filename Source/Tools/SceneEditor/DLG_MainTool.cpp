// DLG_MainTool.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "DLG_MainTool.h"
//#include "MainFrm.h" //by dengzhihui

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLG_MainTool dialog

namespace MFCFramework
{
CDLG_MainTool::CDLG_MainTool(CWnd* pParent /*=NULL*/)
	: CDialog(CDLG_MainTool::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLG_MainTool)
	//}}AFX_DATA_INIT
	//m_lpMap = NULL;
	//if (pParent)
	//{
	//	MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) pParent;
	//	//m_lpMap = (KModelTerrain*) pMainFrame->m_lpSceneEdit->m_lpMapMesh;
	//}
}


void CDLG_MainTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLG_MainTool)
	DDX_Control(pDX, IDC_FRMFRAME1, m_MainTool);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLG_MainTool, CDialog)
	//{{AFX_MSG_MAP(CDLG_MainTool)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLG_MainTool message handlers

BEGIN_EVENTSINK_MAP(CDLG_MainTool, CDialog)
    //{{AFX_EVENTSINK_MAP(CDLG_MainTool)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 16 /* EAlphaFlat */, OnEAlphaFlatFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 12 /* EAdvancedSet */, OnEAdvancedSetFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 14 /* EAlphaUp */, OnEAlphaUpFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 21 /* EBrush0 */, OnEBrush0Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 23 /* EBrush2 */, OnEBrush2Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 22 /* EBrush1 */, OnEBrush1Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 15 /* EAlphaDown */, OnEAlphaDownFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 17 /* EAlphaNoise */, OnEAlphaNoiseFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 24 /* EBrush3 */, OnEBrush3Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 25 /* EBrush4 */, OnEBrush4Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 11 /* ECircleSelect */, OnECircleSelectFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 13 /* EGroundCave */, OnEGroundCaveFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 4 /* EGroundCliff */, OnEGroundCliffFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 19 /* EGroundDeepWater */, OnEGroundDeepWaterFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 28 /* EGroundDown */, OnEGroundDownFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 26 /* EGroundFlat */, OnEGroundFlatFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 18 /* EGroundThinWater */, OnEGroundThinWaterFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 27 /* EGroundUp */, OnEGroundUpFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 6 /* ELastMaterial1 */, OnELastMaterial1Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 8 /* ELastMaterial4 */, OnELastMaterial4Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 10 /* EMaterialDown */, OnEMaterialDownFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 9 /* EMaterialUp */, OnEMaterialUpFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 1 /* ESuck */, OnESuckFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 29 /* ETitleSetChange */, OnETitleSetChangeFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 2 /* EViewFrame */, OnEViewFrameFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 3 /* EViewTexture */, OnEViewTextureFrmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 5 /* ELastMaterial2 */, OnELastMaterial2Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 7 /* ELastMaterial3 */, OnELastMaterial3Frmframe1, VTS_NONE)
	ON_EVENT(CDLG_MainTool, IDC_FRMFRAME1, 20 /* EHeightMap */, OnEHeightMapFrmframe1, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CDLG_MainTool::OnEGroundUpFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//if (!m_lpMap->m_dwState)
		//m_lpMap->m_dwState = GROUND_UP;
	//else
	//	m_lpMap->m_dwState = 0;
}

void CDLG_MainTool::OnEGroundDownFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//if (!m_lpMap->m_dwState)
	//m_lpMap->m_dwState = GROUND_DOWN;
	//else
	//	m_lpMap->m_dwState = 0;
}

void CDLG_MainTool::OnEGroundFlatFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//if (!m_lpMap->m_dwState)
	//m_lpMap->m_dwState = GROUND_FLAT;
	//else
	//	m_lpMap->m_dwState = 0;
}


void CDLG_MainTool::OnEBrush0Frmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_BrushWidth = 0;
	
}

void CDLG_MainTool::OnEBrush1Frmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_BrushWidth = 1;
	
}

void CDLG_MainTool::OnEBrush2Frmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_BrushWidth = 2;
	
}

void CDLG_MainTool::OnEBrush3Frmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_BrushWidth = 3;
	
}

void CDLG_MainTool::OnEBrush4Frmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_BrushWidth = 4;
	
}

void CDLG_MainTool::OnEAlphaDownFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_dwState = MAP_ALPHA_DOWN;
}

void CDLG_MainTool::OnEAlphaUpFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_dwState = MAP_ALPHA_UP;
	
}

void CDLG_MainTool::OnEHeightMapFrmframe1() 
{
	// TODO: Add your control notification handler code here
	
}


void CDLG_MainTool::OnETitleSetChangeFrmframe1() 
{
	// TODO: Add your control notification handler code here
	
}


void CDLG_MainTool::OnEViewFrameFrmframe1() 
{
	// TODO: Add your control notification handler code here
	/*if (m_lpMap->m_FillMode !=D3DFILL_WIREFRAME)
		m_lpMap->m_FillMode = D3DFILL_WIREFRAME;
	else
		m_lpMap->m_FillMode = D3DFILL_SOLID;*/
}

void CDLG_MainTool::OnEAdvancedSetFrmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnEAlphaFlatFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_dwState = MAP_ALPHA_FLAT;
	
}

void CDLG_MainTool::OnEAlphaNoiseFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_dwState = MAP_ALPHA_NOISE;
}

void CDLG_MainTool::OnECircleSelectFrmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnEGroundCaveFrmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnEGroundCliffFrmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnEGroundDeepWaterFrmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnEGroundThinWaterFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//m_lpMap->m_dwState = GROUND_NOISE;
	
}

void CDLG_MainTool::OnELastMaterial1Frmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnELastMaterial2Frmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnELastMaterial3Frmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnELastMaterial4Frmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnEMaterialDownFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//DWORD MaterialUp = m_MainTool.GetPMaterialUp() - 1;
	//DWORD MaterialDn = m_MainTool.GetPMaterialDown()-1;
	//
	//m_lpMap->m_dwState = MAP_PAINT_GROUND;
}

void CDLG_MainTool::OnEMaterialUpFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//DWORD MaterialUp = m_MainTool.GetPMaterialUp() - 1;
	//DWORD MaterialDn = m_MainTool.GetPMaterialDown()-1;

	//m_lpMap->m_dwState = MAP_PAINT_GROUND;
}

void CDLG_MainTool::OnESuckFrmframe1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDLG_MainTool::OnEViewTextureFrmframe1() 
{
	// TODO: Add your control notification handler code here
	//if (m_lpMap->m_FillMode !=D3DFILL_WIREFRAME)
	//	m_lpMap->m_FillMode = D3DFILL_WIREFRAME;
	//else
	//	m_lpMap->m_FillMode = D3DFILL_SOLID;
	
}

BOOL CDLG_MainTool::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//m_MainTool.get
	/*m_MainTool.SetPTextureDir("GroundMap");
	m_MainTool.LoadPictures();*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
};
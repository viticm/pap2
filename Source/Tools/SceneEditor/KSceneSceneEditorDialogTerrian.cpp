// KSceneSceneEditorDialogTerrian.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorDialogTerrian.h"
//#include "SceneSkinEdit.h"
//#include "MeshTerran.h"
#include ".\kscenesceneeditordialogterrian.h"
#include "KColorDialog.h"
#include "KSceneSceneEditorObscaleDialog.h"
#include "SceneEditorView.h"
#include "SceneEditBase.h"
//#include "..\..\KG3DEngine\KG3DEngine\KG3DSceneSceneEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogTerrian dialog


KSceneSceneEditorDialogTerrian::KSceneSceneEditorDialogTerrian(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogTerrian::IDD, pParent), m_stSmoothSaveInfo(FALSE, FALSE, FALSE)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorDialogTerrian)
	m_bInInvertMode = FALSE;
	m_bInSmoothMode = FALSE;

	m_LastSize = m_Size = 20;
    m_LastSize2 = m_Size2 = 10;
	m_Alpha = 0;
	m_CStringBrushSize = _T("");
    m_CStringBrushSize2 = _T("");
	m_CStringBrushAlpha = _T("");
	m_fHeight = 0.0f;
	m_bGrass = FALSE;
	m_bGround = FALSE;
	m_bHeightStand = FALSE;
	m_bWater = FALSE;
	m_bBarrier = FALSE;
	m_bDeleteRegion = FALSE;
	m_bAddRegion = FALSE;
	//}}AFX_DATA_INIT

	m_lpSceneSceneEditor = NULL;
	m_dwNumGroundTexture = 0;
	m_lpObscaleDialog = NULL;
	m_lpSceneEditorView = NULL;
    m_bIsGetAbsoluteHeight = true;
    m_fOppositeHeightValue = 0.0f;
}


void KSceneSceneEditorDialogTerrian::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(KSceneSceneEditorDialogTerrian)
    DDX_Control(pDX, IDC_GROUNDGET, m_CheckHeightGet);
    DDX_Control(pDX, IDC_EDITHEIGHTVALUE, m_EditHeight);
    DDX_Control(pDX, IDC_SPIN1, m_Spin);
    DDX_Control(pDX, IDC_SLIDERALPHA, m_SliderCtrlAlpha);
    DDX_Control(pDX, IDC_SLIDERSZIE, m_SliderCtrlOuterSize);
    DDX_Control(pDX, IDC_SLIDERSZIE2, m_SliderCtrlInsideSize);
    DDX_Control(pDX, IDC_SLIDERSZIE3, m_SliderPower);
//    DDX_Control(pDX, IDC_BCOLOR, m_Color);
    DDX_Control(pDX, IDC_CGROUNDUP, m_CheckHeightUp);
    DDX_Control(pDX, IDC_CGROUNDSET, m_CheckHeightSet);
    DDX_Control(pDX, IDC_CGROUNDNOISE, m_CheckHeightNoise);
    DDX_Control(pDX, IDC_CGROUNDFLAT, m_CheckHeightFlat);
    DDX_Control(pDX, IDC_CGROUNDDOWN, m_CheckHeightDown);
    DDX_Control(pDX, IDC_CHEIGHT, m_CheckHeight);
//    DDX_Control(pDX, IDC_CCOLOR, m_CheckColor);
    DDX_Control(pDX, IDC_CCLIFFWATER, m_CheckCliffWater);
    DDX_Slider(pDX, IDC_SLIDERSZIE, m_Size);
    DDX_Slider(pDX, IDC_SLIDERSZIE2, m_Size2);
    DDX_Slider(pDX, IDC_SLIDERSZIE3, m_nPower);
    DDX_Slider(pDX, IDC_SLIDERALPHA, m_Alpha);
    DDX_Text(pDX, IDC_SBRUSHSIZE, m_CStringBrushSize);
    DDX_Text(pDX, IDC_SBRUSHSIZE2, m_CStringBrushSize2);  
    DDX_Text(pDX, IDC_SBRUSHSIZE3, m_CStringPower);  
    DDX_Text(pDX, IDC_SBRUSHALPHA, m_CStringBrushAlpha);
    
    DDX_Text(pDX, IDC_EDITHEIGHTVALUE, m_fHeight);
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_CHECK11, m_OppositeHeight);
    DDX_Control(pDX, IDC_CHECK2, m_AbsoluteHeight);
    DDX_Text(pDX, IDC_EDIT1, m_fOppositeHeightValue);
    DDV_MinMaxFloat(pDX,m_fOppositeHeightValue,0.0f,1000000.0f);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogTerrian, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorDialogTerrian)
	ON_BN_CLICKED(IDC_CGROUNDDOWN, OnCgrounddown)
	ON_BN_CLICKED(IDC_CGROUNDFLAT, OnCgroundflat)
	ON_BN_CLICKED(IDC_CGROUNDNOISE, OnCgroundnoise)
	ON_BN_CLICKED(IDC_CGROUNDSET, OnCgroundset)
	ON_BN_CLICKED(IDC_CGROUNDUP, OnCgroundup)
	ON_BN_CLICKED(IDC_CHEIGHT, OnCheight)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDITHEIGHTVALUE, OnChangeEditheightvalue)
	ON_WM_VSCROLL()

	ON_BN_CLICKED(IDC_GROUNDGET, OnGroundget)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_GRASS, OnCheckGrass)
	ON_BN_CLICKED(IDC_CHECK_GROUND, OnCheckGround)

	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO1,OnSetWireFrameMode)
	ON_BN_CLICKED(IDC_RADIO2,OnSetSolidMode)
	ON_BN_CLICKED(IDC_BUTTON_RELOADFX,OnRefreshFX)
	ON_BN_CLICKED(IDC_BUTTON_ADDREGION, &KSceneSceneEditorDialogTerrian::OnBnClickedButtonAddregion)
	ON_BN_CLICKED(IDC_BUTTON_DELETEREGION, &KSceneSceneEditorDialogTerrian::OnBnClickedButtonDeleteregion)
	ON_BN_CLICKED(IDC_CHECK_OPTIMIZE, &KSceneSceneEditorDialogTerrian::OnBnClickedCheckOptimize)
    ON_BN_CLICKED(IDC_CHECK_TREE_LOD, &KSceneSceneEditorDialogTerrian::OnBnClickedSpeedTreeLOD)

    ON_BN_CLICKED(IDC_CHECK1,OnOptimizeVideoMemoryLightMap)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERSZIE, &KSceneSceneEditorDialogTerrian::OnNMCustomdrawSliderszie)
    ON_BN_CLICKED(IDC_CHECK2, &KSceneSceneEditorDialogTerrian::OnSetAbsoluteHeight)
    ON_BN_CLICKED(IDC_CHECK11, &KSceneSceneEditorDialogTerrian::OnSetOppositeHeight)
    ON_EN_CHANGE(IDC_EDIT1, &KSceneSceneEditorDialogTerrian::OnSetOppositeHeightValue)
	ON_BN_CLICKED(IDC_CHECK_TERRAINLOD, &KSceneSceneEditorDialogTerrian::OnBnClickedCheckTerrainLOD)
	ON_BN_CLICKED(IDC_CHECK_SHOWEDGE, &KSceneSceneEditorDialogTerrian::OnBnClickedCheckShowedge)
	ON_BN_CLICKED(IDC_COMPUTE_NORMAL, &KSceneSceneEditorDialogTerrian::OnBnClickedComputeNormal)
	ON_BN_CLICKED(IDC_REFRESH_TERRAINTEXTURE, &KSceneSceneEditorDialogTerrian::OnBnClickedRefreshTerraintexture)
	ON_BN_CLICKED(IDC_BUTTON_REFRESHTERRAIN, &KSceneSceneEditorDialogTerrian::OnBnClickedButtonRefreshterrain)
	ON_BN_CLICKED(IDC_EXPORT_NORMAL, &KSceneSceneEditorDialogTerrian::OnBnClickedExportNormal)
    ON_BN_CLICKED(IDC_CHECK_PVS_CULLING, &KSceneSceneEditorDialogTerrian::OnBnClickedCheckPvsCulling)
	ON_BN_CLICKED(IDC_EXPORT_TERRAINTEXTURE, &KSceneSceneEditorDialogTerrian::OnBnClickedExportTerraintexture)
	ON_BN_CLICKED(IDC_RADIO_LOW, &KSceneSceneEditorDialogTerrian::OnBnClickedRadioLow)
	ON_BN_CLICKED(IDC_RADIO_MIDDLE, &KSceneSceneEditorDialogTerrian::OnBnClickedRadioMiddle)
	ON_BN_CLICKED(IDC_RADIO_HIGHT, &KSceneSceneEditorDialogTerrian::OnBnClickedRadioHight)
END_MESSAGE_MAP()


BOOL KSceneSceneEditorDialogTerrian::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetTimer(0,500,0);

	m_Spin.SetBuddy(&m_EditHeight);
	m_Spin.SetRange(-4096, 4096);

	m_SliderCtrlOuterSize.SetRange(1,100);
    m_SliderCtrlInsideSize.SetRange(1,100);
    m_SliderPower.SetRange(5,200);

    m_AbsoluteHeight.SetCheck(true);
    m_OppositeHeight.SetCheck(false);

    m_fOppositeHeightValue = 0.0f;
    m_nPower = 20;
    m_fPower = (float)m_nPower / 10;

    m_CStringBrushSize.Format("外圈大小 %d",m_Size);
    m_CStringBrushSize2.Format("内圈大小 %d",m_Size2);
    m_CStringPower.Format("Power系数 %f",m_fPower);

	((CButton*)GetDlgItem(IDC_CHECK_OPTIMIZE))->SetCheck(BST_CHECKED);

    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    ASSERT(piManager);
    KG3DEngineOption engineOption;
    piManager->GetEngineOption(&engineOption);
	((CButton*)GetDlgItem(IDC_CHECK_TREE_LOD))->SetCheck(engineOption.bUseSpeedTreeLod);
	((CButton*)GetDlgItem(IDC_CHECK_TERRAINLOD))->SetCheck(engineOption.bUseTerrainLOD);

	((CButton*)GetDlgItem(IDC_RADIO_LOW))->SetCheck((engineOption.nTerrainDetail >= 2));
	((CButton*)GetDlgItem(IDC_RADIO_MIDDLE))->SetCheck((engineOption.nTerrainDetail == 1));
	((CButton*)GetDlgItem(IDC_RADIO_HIGHT))->SetCheck((engineOption.nTerrainDetail == 0));
    UpdateData(FALSE);

    CheckDlgButton(IDC_CHECK_PVS_CULLING, TRUE);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSceneEditorDialogTerrian::OnOK()
{

}



void KSceneSceneEditorDialogTerrian::UnCheckAllHeights()
{
	m_CheckHeightDown .SetCheck(FALSE);
	m_CheckHeightFlat .SetCheck(FALSE);
	m_CheckHeightNoise.SetCheck(FALSE);
	m_CheckHeightSet  .SetCheck(FALSE);
	m_CheckHeightUp   .SetCheck(FALSE);
	m_CheckHeightGet  .SetCheck(FALSE);
	
}

void KSceneSceneEditorDialogTerrian::UnCheckAllTerrain()
{
	m_CheckCliffWater.SetCheck(FALSE);
	m_CheckHeight    .SetCheck(FALSE);
//	m_CheckColor     .SetCheck(FALSE);

}

void  KSceneSceneEditorDialogTerrian::UnCheckAllOperationType()
{
	m_bBarrier = FALSE;
	m_bGround = FALSE;
	m_bWater = FALSE;
	m_bHeightStand = FALSE;
	m_bGrass = FALSE;
	m_bAddRegion = FALSE;
	m_bDeleteRegion = FALSE;

	UpdateData(FALSE);
}

void KSceneSceneEditorDialogTerrian::OnCgrounddown() 
{
	if(!m_lpSceneSceneEditor)
		return;
    
	UnCheckAllHeights();
	m_CheckHeightDown.SetCheck(TRUE);
	m_CheckHeight.SetCheck(TRUE);
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;

    m_lpSceneSceneEditor->SetTBModifierState(GROUND_DOWN);
}

void KSceneSceneEditorDialogTerrian::OnCgroundflat() 
{
	if(m_bGround)
	{
		UnCheckAllHeights();
		UnCheckAllTerrain();
		m_CheckHeight.SetCheck(TRUE);
	}
	else if(m_bWater)
	{
		UnCheckAllHeights();
		UnCheckAllTerrain();
		m_CheckCliffWater.SetCheck(TRUE);
	}
	m_CheckHeightFlat .SetCheck(TRUE);

	if(!m_lpSceneEditorView)
	if(!m_lpSceneSceneEditor)
		return;

	m_lpSceneEditorView->m_dwState = SCENESTATE_TERRAIN;

 /*   IEKG3DTerrain *pEditorTerrainEx = NULL;
    m_lpSceneEditorView->m_DialogPanel.m_lpSceneEditor->GetTerrain(&pEditorTerrainEx);
    if (!pEditorTerrainEx)
        return;*/

	UnCheckAllHeights();
	m_CheckHeightFlat.SetCheck(TRUE);
	m_CheckHeight.SetCheck(TRUE);
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(GROUND_FLAT);
}

void KSceneSceneEditorDialogTerrian::OnCgroundnoise() 
{
	if(m_bGround)
	{
		UnCheckAllHeights();
		UnCheckAllTerrain();
		m_CheckHeight   .SetCheck(TRUE);
	}
	else if(m_bWater)
	{
		UnCheckAllHeights();
		UnCheckAllTerrain();
		m_CheckCliffWater.SetCheck(TRUE);
	}
	m_CheckHeightNoise .SetCheck(TRUE);

	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
}

void KSceneSceneEditorDialogTerrian::OnCgroundset() 
{
	if(!m_lpSceneSceneEditor)
		return;
	UnCheckAllHeights();
	m_CheckHeightSet.SetCheck(TRUE);
	m_CheckHeight.SetCheck(TRUE);
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(GROUND_SET);
}

void KSceneSceneEditorDialogTerrian::OnCgroundup() 
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
	UnCheckAllHeights();
	m_CheckHeightUp.SetCheck(TRUE);
	m_CheckHeight.SetCheck(TRUE);
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
    m_lpSceneSceneEditor->SetTBModifierState(GROUND_UP);
}

void KSceneSceneEditorDialogTerrian::OnCheight() 
{
	// TODO: Add your control notification handler code here
	/*if(!m_lpSceneSceneEditor)
		return;

	m_lpSceneSceneEditor->m_dwState = EDITSTATE_TERRANEDIT;

	m_lpSceneSceneEditor->m_lpMapMesh->m_dwState = GROUND_NOISE;*/
}

void KSceneSceneEditorDialogTerrian::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	if(!m_lpSceneSceneEditor)
		return;
	//extern BYTE g_KeyState[256];
	//g_KeyState[VK_LBUTTON] = 0;

	UpdateData(TRUE);

	m_Size = max(1,m_Size);
    m_Size2 = max(1,m_Size2);
    m_nPower = max(5,m_nPower);
    m_fPower = (float)m_nPower / 10;
    /*if(m_Size < m_Size2)
        m_Size = m_Size2;*/
    if(m_Size2 > m_Size)
        m_Size2 = m_Size;

	m_lpSceneSceneEditor->SetOuterHeightBrushSize(m_Size);
	m_lpSceneSceneEditor->SetInnerHeightBrushSize(m_Size2);
	m_lpSceneSceneEditor->SetDiShu(m_fPower);

	m_lpSceneSceneEditor->SetTerrainModifyAlpha((float)m_Alpha);

	if(m_Size != m_LastSize)
	{
		m_LastSize = m_Size;
		m_lpSceneSceneEditor->ReSizeHeightData((WORD)m_Size);// m_Brush.ReSizeHeightData((WORD)m_Size);
	}

	m_CStringBrushSize.Format("外圈大小 %d",m_Size);
	m_CStringBrushSize2.Format("内圈大小 %d",m_Size2);
	m_CStringBrushAlpha.Format("压感 %d",m_Alpha);
	m_CStringPower.Format("power系数 %f",m_fPower);

	UpdateData(FALSE);
}

void KSceneSceneEditorDialogTerrian::OnChangeEditheightvalue() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;

    UpdateData(TRUE);
	m_lpSceneSceneEditor->SetTerrainModifyHeight(m_fHeight);
	return ;
}

void KSceneSceneEditorDialogTerrian::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	//m_fHeight = (float)m_Spin.GetPos32();
	//UpdateData(FALSE);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

}
void KSceneSceneEditorDialogTerrian::OnRefreshFX()
{
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->ReloadTerrainRenderShader();
}

void KSceneSceneEditorDialogTerrian::OnGroundget() 
{
	if(!m_lpSceneSceneEditor)
		return;
	UnCheckAllHeights();
	m_CheckHeightGet.SetCheck(TRUE);
	m_CheckHeight.SetCheck(TRUE);
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(GROUND_GET);
}

void KSceneSceneEditorDialogTerrian::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(!m_lpSceneSceneEditor)
		return;

	if(m_Alpha!= m_lpSceneSceneEditor->GetTerrainModifyAlpha())
	{
		m_Alpha = (int) m_lpSceneSceneEditor->GetTerrainModifyAlpha();
		m_CStringBrushAlpha.Format("压感 %d",m_Alpha);
		UpdateData(FALSE);
	}

	DWORD dwState =m_lpSceneSceneEditor->GetTBModifierState();;
	if((dwState == GROUND_GET)
		||(dwState == GROUND_SET))
	{
		m_lpSceneSceneEditor->GetTerrainModifyHeight(m_fHeight);
		UpdateData(FALSE);
	}
	{
		m_Size = m_lpSceneSceneEditor->GetOuterHeightBrushSize();// m_Brush.wOuterHeightBrushSize;
		m_Size2 = m_lpSceneSceneEditor->GetInnerHeightBrushSize();// m_Brush.wInsideHeightBrushSize;
		UpdateData(FALSE);
	}
	CDialog::OnTimer(nIDEvent);
}

void KSceneSceneEditorDialogTerrian::OnCheckGrass() 
{
	if(!m_lpSceneEditorView)
		return;

	UnCheckAllOperationType();
	m_bGrass= TRUE;
	UpdateData(FALSE);
	UnCheckAllHeights();
	m_CheckHeightUp.SetCheck(1);

	m_lpSceneEditorView->m_dwState = SCENESTATE_TERRAIN;
}

void KSceneSceneEditorDialogTerrian::OnCheckGround() 
{
	//// TODO: Add your control notification handler code here
	if(!m_lpSceneEditorView)
		return;

	UnCheckAllOperationType();
	m_bGround = TRUE;
	UpdateData(FALSE);
	UnCheckAllHeights();
	m_CheckHeightUp.SetCheck(1);

	m_lpSceneEditorView->m_dwState = SCENESTATE_TERRAIN;
}



BOOL KSceneSceneEditorDialogTerrian::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	SAFE_DELETE(m_lpObscaleDialog);
	return CDialog::DestroyWindow();
}

void KSceneSceneEditorDialogTerrian::OnSetWireFrameMode()
{
 //   IEKG3DTerrain *pTerrainEx = NULL;
 //   m_lpSceneEditorView->m_DialogPanel.m_lpSceneEditor->GetTerrain(&pTerrainEx);
 //   if(!pTerrainEx)
 //       return;

	//pTerrainEx->SetEnableWireFrameMode(TRUE);//m_bIsSolidRenderMode = false;
		 m_lpSceneEditorView->m_DialogPanel.m_lpSceneEditor->SetEnableWireFrameMode(TRUE);
}

void KSceneSceneEditorDialogTerrian::OnSetSolidMode()
{
 //   IEKG3DTerrain *pTerrainEx = NULL;
 //   m_lpSceneEditorView->m_DialogPanel.m_lpSceneEditor->GetTerrain(&pTerrainEx);
 //   if(!pTerrainEx)
 //       return;

	//pTerrainEx->SetEnableWireFrameMode(FALSE);// m_bIsSolidRenderMode = true;
	 m_lpSceneEditorView->m_DialogPanel.m_lpSceneEditor->SetEnableWireFrameMode(FALSE);
}

void KSceneSceneEditorDialogTerrian::OnBnClickedButtonAddregion()
{
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(MAP_TERRAINBLOCK_ADD);
}

void KSceneSceneEditorDialogTerrian::OnBnClickedButtonDeleteregion()
{
	if(!m_lpSceneSceneEditor)
		return;
	m_lpSceneSceneEditor->SetEditState(SCENESTATE_TERRAIN);//m_dwEditState = SCENESTATE_TERRAIN;
	m_lpSceneSceneEditor->SetTBModifierState(MAP_TERRAINBLOCK_DELETE);
}



void KSceneSceneEditorDialogTerrian::OnBnClickedCheckOptimize()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return;
}

void KSceneSceneEditorDialogTerrian::OnBnClickedSpeedTreeLOD()
{
    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    ASSERT(piManager);
    KG3DEngineOption engineOption;
    piManager->GetEngineOption(&engineOption);
    engineOption.bUseSpeedTreeLod = ((CButton*)GetDlgItem(IDC_CHECK_TREE_LOD))->GetCheck();
    piManager->SetEngineOption(&engineOption);

}

void KSceneSceneEditorDialogTerrian::OnOptimizeVideoMemoryLightMap()
{
    //IEKG3DTerrain *pTerrainEx = NULL;
    //m_lpSceneSceneEditor->GetTerrain(&pTerrainEx);
    //if(!pTerrainEx)
    //    return;
    //BOOL bIsUseOldLightMap = FALSE;
    //pTerrainEx->GetIsUseOldLightMap(&bIsUseOldLightMap);
    //pTerrainEx->SetIsUseOldLightMap(!bIsUseOldLightMap);
}

void KSceneSceneEditorDialogTerrian::OnNMCustomdrawSliderszie(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}

void KSceneSceneEditorDialogTerrian::OnSetAbsoluteHeight()
{
    // TODO: Add your control notification handler code here

    m_AbsoluteHeight.SetCheck(true);
    m_OppositeHeight.SetCheck(false);
    m_bIsGetAbsoluteHeight = true;
    m_lpSceneSceneEditor->SetIsGetAbsoluteHeight(m_bIsGetAbsoluteHeight);
}

void KSceneSceneEditorDialogTerrian::OnSetOppositeHeight()
{
    // TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return ;
    m_AbsoluteHeight.SetCheck(false);
    m_OppositeHeight.SetCheck(true);
    m_bIsGetAbsoluteHeight = false;
	m_lpSceneSceneEditor->SetIsGetAbsoluteHeight(m_bIsGetAbsoluteHeight);
}

void KSceneSceneEditorDialogTerrian::OnSetOppositeHeightValue()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
	if (!m_lpSceneSceneEditor)
		return ;
    UpdateData(true);

    m_lpSceneSceneEditor->SetOppositeHeightValue(m_fOppositeHeightValue);
}

//(by dengzhihui 2006年8月30日 12:02:22
//将当前的操作置为反向
void KSceneSceneEditorDialogTerrian::OnInvertOperation(BOOL bEnabled)
{
	return;
	///C4702 UnReachableCode
	/*
	if (m_bInInvertMode == bEnabled)
			return;
	
		m_bInInvertMode = bEnabled;
	
		BOOL bHeightUpChecked = m_CheckHeightUp.GetCheck() == BST_CHECKED;
		BOOL bHeightDownChecked = m_CheckHeightDown.GetCheck() == BST_CHECKED;
		
		if (bHeightDownChecked || bHeightUpChecked)
		{
			if (bHeightUpChecked)
				OnCgrounddown();
			else
				OnCgroundup();
		}*/
	
}

//强行进入平滑地表状态
void KSceneSceneEditorDialogTerrian::OnSmoothOperation(BOOL bEnabled)
{
	return;
	///C4702 Unreachable Code
	/*
	if (m_bInSmoothMode == bEnabled)
			return;
		
		if (bEnabled)
		{
			m_stSmoothSaveInfo.bPreInvertMode = m_bInInvertMode;
			m_stSmoothSaveInfo.bPreHeightUpChecked = m_CheckHeightUp.GetCheck() == BST_CHECKED;
			m_stSmoothSaveInfo.bPreHeightDownChecked = m_CheckHeightDown.GetCheck() == BST_CHECKED;
			OnCgroundflat();
		}
		else
		{
			//恢复前面的环境
			if (m_stSmoothSaveInfo.bPreHeightUpChecked || m_stSmoothSaveInfo.bPreHeightDownChecked)
			{
				BOOL bInvert = FALSE;
				if (m_stSmoothSaveInfo.bPreInvertMode != m_bInInvertMode)
					bInvert = TRUE;
				
				if (m_stSmoothSaveInfo.bPreHeightUpChecked && !bInvert)
					OnCgroundup();
				else
					OnCgrounddown();
			}
		}
	
		m_bInSmoothMode = bEnabled;*/
	
}
//强行进入调整笔刷大小状态
void KSceneSceneEditorDialogTerrian::OnBrushWidthModify(BOOL bAdjustInternalCircle, int nXOff, int nYOff)
{
	return ;
}

void KSceneSceneEditorDialogTerrian::OnBrushForceModify(int nXOff, int nYOff)
{
	return ;
}
void KSceneSceneEditorDialogTerrian::OnUpdateUI(UINT nActionID)
{
	if(!m_lpSceneSceneEditor)
		return;

	m_Size = m_lpSceneSceneEditor->GetOuterHeightBrushSize();
	m_Size2 = m_lpSceneSceneEditor->GetInnerHeightBrushSize();
	m_Alpha = (int)m_lpSceneSceneEditor->GetTerrainModifyAlpha();
	//m_Alpha = (int) pTerrainEx->GetTerrainModifyAlpha();
	//m_Size = pTerrainEx->GetOuterHeightBrushSize();//m_Brush.wOuterHeightBrushSize;
	//m_Size2 = pTerrainEx->GetInnerHeightBrushSize();// .wInsideHeightBrushSize;
	m_CStringBrushSize.Format("外圈大小 %d",m_Size);
	m_CStringBrushSize2.Format("内圈大小 %d",m_Size2);
	m_CStringBrushAlpha.Format("压感 %d",m_Alpha);
	//m_CStringPower.Format("power系数 %f",m_fPower);
	UpdateData(FALSE);
}

void KSceneSceneEditorDialogTerrian::OnBnClickedCheckTerrainLOD()
{
	IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
	ASSERT(piManager);
	KG3DEngineOption engineOption;
	piManager->GetEngineOption(&engineOption);
	engineOption.bUseTerrainLOD = ((CButton*)GetDlgItem(IDC_CHECK_TERRAINLOD))->GetCheck();
	piManager->SetEngineOption(&engineOption);
}

void KSceneSceneEditorDialogTerrian::OnBnClickedCheckShowedge()
{
	// TODO: Add your control notification handler code here
	BOOL bRenderEdge = ((CButton*)(GetDlgItem(IDC_CHECK_SHOWEDGE)))->GetCheck();
	if(m_lpSceneSceneEditor)
		m_lpSceneSceneEditor->SetRenderBlockEdge(bRenderEdge);
}

void KSceneSceneEditorDialogTerrian::OnBnClickedComputeNormal()
{
	// TODO: Add your control notification handler code here
	if(m_lpSceneSceneEditor)
		m_lpSceneSceneEditor->ComputeTerrainNormal();
}

void KSceneSceneEditorDialogTerrian::OnBnClickedRefreshTerraintexture()
{
	if(m_lpSceneSceneEditor)
		m_lpSceneSceneEditor->CompositorTerrainTexture();
}

void KSceneSceneEditorDialogTerrian::OnBnClickedButtonRefreshterrain()
{
	if(m_lpSceneSceneEditor)
		m_lpSceneSceneEditor->RefreshTerrain();
}
void KSceneSceneEditorDialogTerrian::OnBnClickedExportNormal()
{
	// TODO: Add your control notification handler code here
	if(!m_lpSceneSceneEditor)
		return ;
	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szDir);

	TCHAR strFilePath[MAX_PATH];
	CFileDialog dlgSave(FALSE, 
		"tga", 
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"(*.tga)|*.tga||",
		this);

	if( dlgSave.DoModal() == IDOK)
	{
		sprintf_s(strFilePath,
			MAX_PATH,
			"%s",
			dlgSave.GetPathName());

		m_lpSceneSceneEditor->ExprotNormalMap(strFilePath);
	}

	::SetCurrentDirectory(szDir);
}

void KSceneSceneEditorDialogTerrian::OnBnClickedCheckPvsCulling()
{
    if (m_lpSceneSceneEditor)
        m_lpSceneSceneEditor->EnablePvsCulling(IsDlgButtonChecked(IDC_CHECK_PVS_CULLING));
}

void KSceneSceneEditorDialogTerrian::OnBnClickedExportTerraintexture()
{
	 KGDlgSnapTool DialogSnapTool;
	 DialogSnapTool.SetEditor(m_lpSceneSceneEditor);
	 DialogSnapTool.EnableExportTexture(TRUE);
	 DialogSnapTool.DoModal();
}

void KSceneSceneEditorDialogTerrian::OnBnClickedRadioLow()
{
	IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
	ASSERT(piManager);
	KG3DEngineOption engineOption;
	piManager->GetEngineOption(&engineOption);
	engineOption.nTerrainDetail = 2;
	piManager->SetEngineOption(&engineOption);
}
void KSceneSceneEditorDialogTerrian::OnBnClickedRadioMiddle()
{
	IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
	ASSERT(piManager);
	KG3DEngineOption engineOption;
	piManager->GetEngineOption(&engineOption);
	engineOption.nTerrainDetail = 1;
	piManager->SetEngineOption(&engineOption);
}

void KSceneSceneEditorDialogTerrian::OnBnClickedRadioHight()
{
	IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
	ASSERT(piManager);
	KG3DEngineOption engineOption;
	piManager->GetEngineOption(&engineOption);
	engineOption.nTerrainDetail = 0;
	piManager->SetEngineOption(&engineOption);
}

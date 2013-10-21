// KSceneObjectEditorPageModel.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectEditorPageModel.h"

//#include "KSceneObjectEditor.h"
#include "KObjectEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPageModel dialog

IMPLEMENT_DYNCREATE(KSceneObjectEditorPageModel, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(KSceneObjectEditorPageModel, CPropertyPage)
	//{{AFX_MSG_MAP(KSceneObjectEditorPageModel)
	ON_BN_CLICKED(IDC_BUTTON_ADDMODEL, OnButtonAddmodel)
	ON_BN_CLICKED(IDC_BUTTON_DELETEMODEL, OnButtonDeletemodel)
	ON_BN_CLICKED(IDC_CHECK_MOVE, OnCheckMove)
	ON_BN_CLICKED(IDC_CHECK_ROTATE, OnCheckRotate)
	ON_BN_CLICKED(IDC_CHECK_SCALE, OnCheckScale)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON_CELLINFO, OnButtonCellinfo)
	ON_BN_CLICKED(IDC_BUTTON_BUILDCELL, OnButtonBuildcell)
	ON_BN_CLICKED(IDC_CHECK12, OnShowHeightDown)
	ON_BN_CLICKED(IDC_CHECK11, OnShowHeightUp)
	ON_BN_CLICKED(IDC_CHECK1, OnShowBarrier)
	ON_BN_CLICKED(IDC_SHOWOBJECT, OnShowobject)
	ON_BN_CLICKED(IDC_CWALK, OnCwalk)
	ON_BN_CLICKED(IDC_CBULLET, OnCbullet)
	ON_BN_CLICKED(IDC_CJUMP, OnCjump)
	ON_BN_CLICKED(IDC_CPLACE, OnCplace)
	ON_BN_CLICKED(IDC_CBARRIERADD, OnCbarrieradd)
	ON_BN_CLICKED(IDC_CBARRIERREMOVE, OnCbarrierremove)
	ON_BN_CLICKED(IDC_CBARRIERSET, OnCbarrierset)
	ON_BN_CLICKED(IDC_CHECK_HEIGHTDOWN_DOWN, OnCheckHeightdownDown)
	ON_BN_CLICKED(IDC_CHECK_HEIGHTDOWN_SET, OnCheckHeightdownSet)
	ON_BN_CLICKED(IDC_CHECK_HEIGHTDOWN_UP, OnCheckHeightdownUp)
	ON_EN_CHANGE(IDC_EDIT_HEIGHTDOWN, OnChangeEditHeightdown)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SCRABBLE_SNAP2GRID, OnScrabbleSnap2grid)
	ON_BN_CLICKED(IDC_SCRABBLE_VERTICAL, OnScrabbleVertical)
	ON_BN_CLICKED(IDC_SCRABBLE_PITCH, OnScrabblePitch)
	ON_BN_CLICKED(IDC_SCRABBLE_MODIFYGROUND, OnScrabbleModifyground)
	ON_BN_CLICKED(IDC_SCRABBLE_MODIFYSELF, OnScrabbleModifyself)
	ON_BN_CLICKED(IDC_SCRABBLE_NOONBARRIER, OnScrabbleNoonbarrier)
	ON_BN_CLICKED(IDC_SCRABBLE_RUNTIME, OnScrabbleRuntime)
	ON_BN_CLICKED(IDC_SCRABBLE_WATER, OnScrabbleWater)
	ON_BN_CLICKED(IDC_BUTTON_SNAPX, OnButtonSnapx)
	ON_BN_CLICKED(IDC_BUTTON_SNAPZ, OnButtonSnapz)
	ON_BN_CLICKED(IDC_CHECK_SCALEX, OnCheckScalex)
	ON_BN_CLICKED(IDC_CHECK_SCALEZ, OnCheckScalez)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid


/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPageModel::KSceneObjectEditorPageModelFactory::UpdateRegistry -
// Adds or removes system registry entries for KSceneObjectEditorPageModel


/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPageModel::KSceneObjectEditorPageModel - Constructor

// TODO: Define string resource for page caption; replace '0' below with ID.

KSceneObjectEditorPageModel::KSceneObjectEditorPageModel() :
	CPropertyPage(IDD, 0)
{
	//{{AFX_DATA_INIT(KSceneObjectEditorPageModel)
	m_bShowBarrier = FALSE;
	m_bShowHeightUp = FALSE;
	m_bShowHeightDown = FALSE;
	m_bShowObject = FALSE;
	m_bBarrierWalk = FALSE;
	m_bBarrierJump = FALSE;
	m_bBarrierPlaceObject = FALSE;
	m_bBarrierPassType = FALSE;
	m_bBarrierGroundTexture = FALSE;
	m_bBarrierBullet = FALSE;
	m_bBarrierSet = FALSE;
	m_bBarrierAdd = FALSE;
	m_bBarrierRemove = FALSE;
	m_fHeightValue = 0.0f;
	m_bScrabble_Snap2Grid = FALSE;
	m_bScrabble_Water = FALSE;
	m_bScrabble_Vertical = FALSE;
	m_bScrabble_RunTime = FALSE;
	m_bScrabble_PitchOnly = FALSE;
	m_bScrabble_NoOnBarrier = FALSE;
	m_bScrabble_ModifySelf = FALSE;
	m_bScrabble_ModifyGround = FALSE;
	m_bScaleX = FALSE;
	m_bScaleZ = FALSE;
	//}}AFX_DATA_INIT
	m_lpSceneObjectEditorDoc = NULL;
	m_dwBarrier = 0;
}

void KSceneObjectEditorPageModel::GetCurBarrier()
{
	DWORD Barrier = (m_bBarrierWalk) | 
		(m_bBarrierJump<<1) | 
		(m_bBarrierBullet<<2) | 
		(m_bBarrierPlaceObject<<3);
	m_dwBarrier = Barrier;
}

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPageModel::DoDataExchange - Moves data between page and properties

void KSceneObjectEditorPageModel::DoDataExchange(CDataExchange* pDX)
{
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//{{AFX_DATA_MAP(KSceneObjectEditorPageModel)
/*	DDX_Control(pDX, IDC_CHECK_SCALE, m_Button_Scale);
	DDX_Control(pDX, IDC_CHECK_ROTATE, m_Button_Rotate);
	DDX_Control(pDX, IDC_CHECK_MOVE, m_Button_Move);
	DDX_Control(pDX, IDC_EDIT_HEIGHTDOWN, m_Edit_HeightDown);
	DDX_Control(pDX, IDC_CHECK_HEIGHTDOWN_DOWN, m_Button_HeightDown_Down);
	DDX_Control(pDX, IDC_CHECK_HEIGHTDOWN_SET, m_Button_HeightDown_Set);
	DDX_Control(pDX, IDC_CHECK_HEIGHTDOWN_UP, m_Button_HeightDown_Up);
	DDX_Control(pDX, IDC_STATIC_HEIGHTDOWN, m_Static_HeightDown);
	DDX_Control(pDX, IDC_STATIC_BARRIER, m_Static_Barrier);
	DDX_Control(pDX, IDC_CBARRIERSET, m_Button_Barrierset);
	DDX_Control(pDX, IDC_CBARRIERREMOVE, m_Button_BarrierRemove);
	DDX_Control(pDX, IDC_CBARRIERADD, m_Button_BarrierAdd);
	DDX_Control(pDX, IDC_CBULLET, m_Button_Bullet);
	DDX_Control(pDX, IDC_CJUMP, m_Button_Jump);
	DDX_Control(pDX, IDC_CPLACE, m_Button_Place);
	DDX_Control(pDX, IDC_CWALK, m_Button_Walk);
	DDX_Control(pDX, IDC_LIST1, m_ListBoxModelName);
	DDX_Check(pDX, IDC_CHECK1, m_bShowBarrier);
	DDX_Check(pDX, IDC_CHECK11, m_bShowHeightUp);
	DDX_Check(pDX, IDC_CHECK12, m_bShowHeightDown);
	DDX_Check(pDX, IDC_SHOWOBJECT, m_bShowObject);
	DDX_Check(pDX, IDC_CWALK, m_bBarrierWalk);
	DDX_Check(pDX, IDC_CJUMP, m_bBarrierJump);
	DDX_Check(pDX, IDC_CPLACE, m_bBarrierPlaceObject);
	DDX_Check(pDX, IDC_CPASSTYPE, m_bBarrierPassType);
	DDX_Check(pDX, IDC_CGROUNDTEXT, m_bBarrierGroundTexture);
	DDX_Check(pDX, IDC_CBULLET, m_bBarrierBullet);
	DDX_Check(pDX, IDC_CBARRIERSET, m_bBarrierSet);
	DDX_Check(pDX, IDC_CBARRIERADD, m_bBarrierAdd);
	DDX_Check(pDX, IDC_CBARRIERREMOVE, m_bBarrierRemove);
	DDX_Text(pDX, IDC_EDIT_HEIGHTDOWN, m_fHeightValue);
	DDX_Check(pDX, IDC_SCRABBLE_SNAP2GRID, m_bScrabble_Snap2Grid);
	DDX_Check(pDX, IDC_SCRABBLE_WATER, m_bScrabble_Water);
	DDX_Check(pDX, IDC_SCRABBLE_VERTICAL, m_bScrabble_Vertical);
	DDX_Check(pDX, IDC_SCRABBLE_RUNTIME, m_bScrabble_RunTime);
	DDX_Check(pDX, IDC_SCRABBLE_PITCH, m_bScrabble_PitchOnly);
	DDX_Check(pDX, IDC_SCRABBLE_NOONBARRIER, m_bScrabble_NoOnBarrier);
	DDX_Check(pDX, IDC_SCRABBLE_MODIFYSELF, m_bScrabble_ModifySelf);
	DDX_Check(pDX, IDC_SCRABBLE_MODIFYGROUND, m_bScrabble_ModifyGround);
	DDX_Check(pDX, IDC_CHECK_SCALEX, m_bScaleX);
	DDX_Check(pDX, IDC_CHECK_SCALEZ, m_bScaleZ);
    */
	//}}AFX_DATA_MAP
	
}


/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPageModel message handlers

void KSceneObjectEditorPageModel::OnButtonAddmodel() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	//char bigBuff[8000] = "";  // maximum common dialog buffer size
	//char szFilter[] ="模型文件|*.Mesh;*.STree|普通模型|*.Mesh|SpeedTree|*.STree|场景特效(*.SSfx)|*.SSfx|体积雾 (*.lvf)| *.lvf||";

	//CFileDialog dlg(true, NULL, NULL,
	//	OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT , szFilter);
	//// Modify OPENFILENAME members directly to point to bigBuff
	//dlg.m_ofn.lpstrFile = bigBuff;
	//dlg.m_ofn.nMaxFile = sizeof(bigBuff);

	//if (dlg.DoModal() == IDOK)
	//{
	//	POSITION ps = dlg.GetStartPosition(); 
	//	while(ps != NULL)
	//	{
	//		CString Vs = dlg.GetNextPathName(ps);
	//		TCHAR path_buffer[256];
	//		TCHAR drive[256];
	//		TCHAR dir[256];
	//		TCHAR fname[256];
	//		TCHAR ext[256];

	//		wsprintf(path_buffer,"%s",Vs);
	//		_splitpath( path_buffer, drive, dir, fname, ext );

	//		string str = Vs;

	//		m_ListBoxModelName.AddString(fname);

	//		LPMODEL pModel = NULL;
	//		DWORD   MeshID;
	//		KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	//		
	//		if(pScene)
	//		{
	//			g_cMeshTable.LoadFromFile(&MeshID,path_buffer);
	//			g_cMeshTable.GetXMesh(&pModel,MeshID);
	//			
	//			TCHAR MtlName[256];
	//			wsprintf(MtlName,"%s%s%s.Mtl",drive,dir,fname);
	//			if(FAILED(pModel->LoadMaterial(MtlName)))
	//			{
	//				CFileDialog dlg(true, "Mtl", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D材质文件(*.Mtl)|*.Mtl||");
	//				if(dlg.DoModal() == IDOK)
	//				{
	//					wsprintf(MtlName,"%s",dlg.GetPathName());
	//					pModel->LoadMaterial(MtlName);
	//				}
	//				/*CFileDialog dlgAni(true, "Ani", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D动作文件(*.Ani)|*.Ani||");
	//				if(dlgAni.DoModal() == IDOK)
	//				{
	//					wsprintf(str,"%s",dlg.GetPathName());
	//					m_lpSpeedTree->LoadTreeAni(str);
	//				}*/
	//			}

	//			pScene->m_lpObject->AddMesh(MeshID);
	//		}
	//	}
	//}

	//if(m_lpSceneObjectEditorDoc)
	//{
	//	m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	//}
}

void KSceneObjectEditorPageModel::OnButtonDeletemodel() 
{
	// TODO: Add your control notification handler code here
	/*int Index = m_ListBoxModelName.GetCurSel();

	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	
	if(pScene)
	{
		pScene->m_lpObject->DeleteMesh(Index);
		pScene->m_SelectedList.clear();
	}

	m_ListBoxModelName.DeleteString(Index);
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCheckMove() 
{
	// TODO: Add your control notification handler code here
	/*UnCheckOperatorState();
	m_Button_Move.SetCheck(TRUE);
	
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;

	if(pScene)
	{
		pScene->m_dwState = EDITSTATE_MOVE;
		pScene->m_bShowPart = !pScene->m_bShowPart;

	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCheckRotate() 
{
	// TODO: Add your control notification handler code here
	/*UnCheckOperatorState();
	m_Button_Rotate.SetCheck(TRUE);
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;

	if(pScene)
	{
		pScene->m_dwState = EDITSTATE_ROTATE;
		pScene->m_bShowPart = !pScene->m_bShowPart;

	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCheckScale() 
{
	// TODO: Add your control notification handler code here
	/*UnCheckOperatorState();
	m_Button_Scale.SetCheck(TRUE);
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;

	if(pScene)
	{
		pScene->m_dwState = EDITSTATE_SCALE;
		pScene->m_bShowPart = !pScene->m_bShowPart;
		
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	
	int Index = m_ListBoxModelName.GetCurSel();

	if(pScene)
	{
		pScene->m_SelectedList.clear();

		DWORD MeshID = pScene->m_lpObject->m_vecMeshKeeper[Index].m_dMeshID;
		LPMODEL pModel = NULL;
		g_cMeshTable.GetXMesh(&pModel,MeshID);

		KSceneEntity Entity(SCENEENTITY_OBJECT_PART,pScene->m_lpObject,(PVOID)Index);
		pScene->m_SelectedList.push_back(Entity);

	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnButtonCellinfo() 
{
	// TODO: Add your control notification handler code here
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_lpObject->CreateCellInformation();
	}
	m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);*/

}

void KSceneObjectEditorPageModel::OnButtonBuildcell() 
{
	// TODO: Add your control notification handler code here
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_lpObject->BuildCellInfoHeightDown();
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnShowHeightDown() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_bShowHeight_Down = m_bShowHeightDown;
	}
	EnableHeightDown(m_bShowHeightDown);
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnShowHeightUp() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_bShowHeight_Up = m_bShowHeightUp;
	}	
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnShowBarrier() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_bShowBarrier = m_bShowBarrier;
	}	

	EnableBarrierInfo(m_bShowBarrier);
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnShowobject() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_bShowObject = m_bShowObject;
	}	
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

BOOL KSceneObjectEditorPageModel::OnInitDialog() 
{
	//CPropertyPage::OnInitDialog();
	//
	//// TODO: Add extra initialization here
	//m_bShowObject = TRUE;
	//m_bShowHeightDown = TRUE;

	//m_fHeightValue = 10;

	//UpdateData(FALSE);

	//EnableBarrierInfo(m_bShowBarrier);
	//EnableHeightDown(m_bShowHeightDown);

	//SetTimer(0,1000,0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}

void KSceneObjectEditorPageModel::OnCwalk() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	GetCurBarrier();

	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_dwBarrier = m_dwBarrier;
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCbullet() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	GetCurBarrier();

	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_dwBarrier = m_dwBarrier;
	}	
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCjump() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	GetCurBarrier();

	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_dwBarrier = m_dwBarrier;
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCplace() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	GetCurBarrier();

	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_dwBarrier = m_dwBarrier;
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCbarrieradd() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	UnCheckOperatorState();

	m_bBarrierAdd    = TRUE;
	m_bBarrierRemove = FALSE;
	m_bBarrierSet    = FALSE;
	UpdateData(FALSE);

	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_dwState = EDITSTATE_OBJECTEDIT;
		pScene->m_dwSubState = BARRIER_OPERATOR_ADD;
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCbarrierremove() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	UnCheckOperatorState();

	m_bBarrierAdd    = FALSE;
	m_bBarrierRemove = TRUE;
	m_bBarrierSet    = FALSE;
	UpdateData(FALSE);
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_dwState = EDITSTATE_OBJECTEDIT;
		pScene->m_dwSubState = BARRIER_OPERATOR_REMOVE;
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCbarrierset() 
{
	// TODO: Add your control notification handler code here
	/*UpdateData(TRUE);
	UnCheckOperatorState();

	m_bBarrierAdd    = FALSE;
	m_bBarrierRemove = FALSE;
	m_bBarrierSet    = TRUE;
	UpdateData(FALSE);
	KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_dwState = EDITSTATE_OBJECTEDIT;
		pScene->m_dwSubState = BARRIER_OPERATOR_SET;
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void  KSceneObjectEditorPageModel::EnableBarrierInfo(BOOL Value)
{
	m_Static_Barrier.EnableWindow(Value);

	m_Button_Walk  .EnableWindow(Value);
	m_Button_Jump  .EnableWindow(Value);
	m_Button_Bullet.EnableWindow(Value);
	m_Button_Place .EnableWindow(Value);

	m_Button_BarrierAdd   .EnableWindow(Value);
	m_Button_BarrierRemove.EnableWindow(Value);
	m_Button_Barrierset   .EnableWindow(Value);

	/*if(Value)
	{
		m_bBarrierAdd    = TRUE;
		m_bBarrierRemove = FALSE;
		m_bBarrierSet    = FALSE;
		UpdateData(FALSE);

		KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
		if(pScene)
		{
			pScene->m_dwState = EDITSTATE_OBJECTEDIT;
			pScene->m_dwSubState = BARRIER_OPERATOR_ADD;
		}
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void  KSceneObjectEditorPageModel::EnableHeightDown(BOOL Value)
{
	m_Static_HeightDown.EnableWindow(Value);
	m_Edit_HeightDown.EnableWindow(Value);

	m_Button_HeightDown_Up  .EnableWindow(Value);
	m_Button_HeightDown_Down.EnableWindow(Value);
	m_Button_HeightDown_Set .EnableWindow(Value);
	
	/*if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void  KSceneObjectEditorPageModel::UnCheckOperatorState()
{
	m_Button_BarrierAdd   .SetCheck(FALSE);
	m_Button_BarrierRemove.SetCheck(FALSE);
	m_Button_Barrierset   .SetCheck(FALSE);
	
	m_Button_HeightDown_Up  .SetCheck(FALSE);
	m_Button_HeightDown_Down.SetCheck(FALSE);
	m_Button_HeightDown_Set .SetCheck(FALSE);

	m_Button_Move.SetCheck(FALSE);
	m_Button_Rotate.SetCheck(FALSE);
	m_Button_Scale.SetCheck(FALSE);

	/*if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}


void KSceneObjectEditorPageModel::OnCheckHeightdownDown() 
{
	// TODO: Add your control notification handler code here
	UnCheckOperatorState();
	m_Button_HeightDown_Down.SetCheck(TRUE);

	//KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	//if(pScene)
	//{
	//	pScene->m_dwState = EDITSTATE_OBJECTEDIT;
	//	pScene->m_dwSubState = HEIGHTDOWN_DOWN;
	//}
	//if(m_lpSceneObjectEditorDoc)
	//{
	//	m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	//}
}

void KSceneObjectEditorPageModel::OnCheckHeightdownSet() 
{
	// TODO: Add your control notification handler code here
	UnCheckOperatorState();
	m_Button_HeightDown_Set.SetCheck(TRUE);
	
	//KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	//if(pScene)
	//{
	//	pScene->m_dwState = EDITSTATE_OBJECTEDIT;
	//	pScene->m_dwSubState = HEIGHTDOWN_SET;
	//}
	//if(m_lpSceneObjectEditorDoc)
	//{
	//	m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	//}
}

void KSceneObjectEditorPageModel::OnCheckHeightdownUp() 
{
	// TODO: Add your control notification handler code here
	UnCheckOperatorState();
	m_Button_HeightDown_Up.SetCheck(TRUE);

	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_dwState = EDITSTATE_OBJECTEDIT;
		pScene->m_dwSubState = HEIGHTDOWN_UP;
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnChangeEditHeightdown() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		pScene->m_fHeightValue = m_fHeightValue;
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{

			if(pScene->m_lpObject->m_vecMeshKeeper.size()!=m_ListBoxModelName.GetCount())
			{
				FillListBox();
			}

			BOOL bNeenUpdate = FALSE;
			BOOL Scrabble = FALSE;

			Scrabble = (pScene->m_lpObject->m_dwScrabbleOption & SCRABBLE_SANP2GRID) && 1;
			if(Scrabble != m_bScrabble_Snap2Grid)
			{
				m_bScrabble_Snap2Grid = Scrabble;
				bNeenUpdate = TRUE;
			}
			Scrabble = (pScene->m_lpObject->m_dwScrabbleOption & SCRABBLE_VERTICAL) && 1;
			if(Scrabble != m_bScrabble_Vertical)
			{
				m_bScrabble_Vertical = Scrabble;
				bNeenUpdate = TRUE;
			}
			Scrabble = (pScene->m_lpObject->m_dwScrabbleOption & SCRABBLE_PITCHONLY) && 1;
			if(Scrabble != m_bScrabble_PitchOnly)
			{
				m_bScrabble_PitchOnly = Scrabble;
				bNeenUpdate = TRUE;
			}
			Scrabble = (pScene->m_lpObject->m_dwScrabbleOption & SCRABBLE_MODIFYTERRAIN) && 1;
			if(Scrabble != m_bScrabble_ModifyGround)
			{
				m_bScrabble_ModifyGround = Scrabble;
				bNeenUpdate = TRUE;
			}
			Scrabble = (pScene->m_lpObject->m_dwScrabbleOption & SCRABBLE_MODIFYSELF) && 1;
			if(Scrabble != m_bScrabble_ModifySelf)
			{
				m_bScrabble_ModifySelf = Scrabble;
				bNeenUpdate = TRUE;
			}
			Scrabble = (pScene->m_lpObject->m_dwScrabbleOption & SCRABBLE_NOONBARRIER) && 1;
			if(Scrabble != m_bScrabble_NoOnBarrier)
			{
				m_bScrabble_NoOnBarrier = Scrabble;
				bNeenUpdate = TRUE;
			}
			Scrabble = (pScene->m_lpObject->m_dwScrabbleOption & SCRABBLE_RUNTIME) && 1;
			if(Scrabble != m_bScrabble_RunTime)
			{
				m_bScrabble_RunTime = Scrabble;
				bNeenUpdate = TRUE;
			}
			Scrabble = (pScene->m_lpObject->m_dwScrabbleOption & SCRABBLE_WATER) && 1;
			if(Scrabble != m_bScrabble_Water)
			{
				m_bScrabble_Water = Scrabble;
				bNeenUpdate = TRUE;
			}

			if(bNeenUpdate)
			{
				UpdateData(FALSE);
			}
		}
	}*/

	CPropertyPage::OnTimer(nIDEvent);
}

void  KSceneObjectEditorPageModel::FillListBox()
{
	//KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	//if(pScene)
	//{
	//	if(pScene->m_lpObject)
	//	{
	//		m_ListBoxModelName.SetCurSel(-1);
	//		int K = m_ListBoxModelName.GetCount();
	//		for(int i=0;i<K;i++)
	//		{
	//			m_ListBoxModelName.DeleteString(i);
	//		}

	//		for(DWORD j=0;j<pScene->m_lpObject->m_vecMeshKeeper.size();j++)
	//		{
	//			LPMODEL pModel = NULL;
	//			if(SUCCEEDED(pScene->m_lpObject->GetMesh(&pModel,j+1)))
	//			{
	//				TCHAR path_buffer[256];
	//				TCHAR drive[256];
	//				TCHAR dir[256];
	//				TCHAR fname[256];
	//				TCHAR ext[256];

	//				wsprintf(path_buffer,"%s",pModel->m_szMeshFileName);
	//				_splitpath( path_buffer, drive, dir, fname, ext );

	//				m_ListBoxModelName.AddString(fname);
	//			}
	//			else
	//			{
	//				m_ListBoxModelName.AddString("???");
	//			}
	//		}		
	//		
	//	}
	//}
	//if(m_lpSceneObjectEditorDoc)
	//{
	//	m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	//}
}


void KSceneObjectEditorPageModel::OnScrabbleSnap2grid() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if(m_bScrabble_Snap2Grid)
				pScene->m_lpObject->m_dwScrabbleOption |= SCRABBLE_SANP2GRID;
			else
				pScene->m_lpObject->m_dwScrabbleOption &= ~SCRABBLE_SANP2GRID;
		}
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnScrabbleVertical() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if(m_bScrabble_Vertical)
				pScene->m_lpObject->m_dwScrabbleOption |= SCRABBLE_VERTICAL;
			else
				pScene->m_lpObject->m_dwScrabbleOption &= ~SCRABBLE_VERTICAL;
		}
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnScrabblePitch() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if(m_bScrabble_PitchOnly)
				pScene->m_lpObject->m_dwScrabbleOption |= SCRABBLE_PITCHONLY;
			else
				pScene->m_lpObject->m_dwScrabbleOption &= ~SCRABBLE_PITCHONLY;
		}
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnScrabbleModifyground() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if(m_bScrabble_ModifyGround)
				pScene->m_lpObject->m_dwScrabbleOption |= SCRABBLE_MODIFYTERRAIN;
			else
				pScene->m_lpObject->m_dwScrabbleOption &= ~SCRABBLE_MODIFYTERRAIN;
		}
	}	
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnScrabbleModifyself() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if(m_bScrabble_ModifySelf)
				pScene->m_lpObject->m_dwScrabbleOption |= SCRABBLE_MODIFYSELF;
			else
				pScene->m_lpObject->m_dwScrabbleOption &= ~SCRABBLE_MODIFYSELF;
		}
	}		
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnScrabbleNoonbarrier() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if(m_bScrabble_NoOnBarrier)
				pScene->m_lpObject->m_dwScrabbleOption |= SCRABBLE_NOONBARRIER;
			else
				pScene->m_lpObject->m_dwScrabbleOption &= ~SCRABBLE_NOONBARRIER;
		}
	}	
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnScrabbleRuntime() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if(m_bScrabble_RunTime)
				pScene->m_lpObject->m_dwScrabbleOption |= SCRABBLE_RUNTIME;
			else
				pScene->m_lpObject->m_dwScrabbleOption &= ~SCRABBLE_RUNTIME;
		}
	}	
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnScrabbleWater() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;
	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if(m_bScrabble_Water)
				pScene->m_lpObject->m_dwScrabbleOption |= SCRABBLE_WATER;
			else
				pScene->m_lpObject->m_dwScrabbleOption &= ~SCRABBLE_WATER;
		}
	}		
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnButtonSnapx() 
{
	// TODO: Add your control notification handler code here
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;

	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if((pScene->m_lpObject->m_nWidth<=0)||(pScene->m_lpObject->m_nHeight<=0))
				return;

			pScene->m_lpObject->ComputeBBox();

			float Grid_Width = pScene->m_lpObject->m_nWidth * TERRIANCELLWIDTH;
			float Obj_Width = pScene->m_lpObject->m_BBox.GetLength();

			float Grid_Center = Grid_Width * 0.5f + pScene->m_lpObject->m_nStart_X * TERRIANCELLWIDTH;
			float Obj_Center = pScene->m_lpObject->m_BBox.GetCenter().x;

			float S = Grid_Width / Obj_Width;
			float T = Grid_Center - Obj_Center;

			D3DXMATRIX  Scale;
			D3DXMatrixScaling(&Scale,S,1,1);


			for(DWORD i=0;i<pScene->m_lpObject->m_vecMeshKeeper.size();i++)
			{
				D3DXMATRIX  * pMat = &pScene->m_lpObject->m_vecMeshKeeper[i].m_Matrix;
				if(m_bScaleX)
				{
					D3DXMatrixMultiply(pMat,&Scale,pMat);
				}
				pMat->_41 += T;
			}

			pScene->m_lpObject->ComputeBBox();

		}
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnButtonSnapz() 
{
	// TODO: Add your control notification handler code here
	/*KSceneObjectEditor *pScene = m_lpSceneObjectEditorDoc->m_lpObjectEditorScene;

	if(pScene)
	{
		if(pScene->m_lpObject)
		{
			if((pScene->m_lpObject->m_nWidth<=0)||(pScene->m_lpObject->m_nHeight<=0))
				return;

			pScene->m_lpObject->ComputeBBox();

			float Grid_Width = pScene->m_lpObject->m_nHeight * TERRIANCELLWIDTH;
			float Obj_Width = pScene->m_lpObject->m_BBox.GetWidth();

			float Grid_Center = Grid_Width * 0.5f + pScene->m_lpObject->m_nStart_Z * TERRIANCELLWIDTH;
			float Obj_Center = pScene->m_lpObject->m_BBox.GetCenter().z;

			float S = Grid_Width / Obj_Width;
			float T = Grid_Center - Obj_Center;

			D3DXMATRIX  Scale;
			D3DXMatrixScaling(&Scale,1,1,S);


			for(DWORD i=0;i<pScene->m_lpObject->m_vecMeshKeeper.size();i++)
			{
				D3DXMATRIX  * pMat = &pScene->m_lpObject->m_vecMeshKeeper[i].m_Matrix;
				if(m_bScaleX)
				{
					D3DXMatrixMultiply(pMat,&Scale,pMat);
				}
				pMat->_43 += T;
			}

			pScene->m_lpObject->ComputeBBox();

		}
	}
	if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCheckScalex() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

void KSceneObjectEditorPageModel::OnCheckScalez() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	/*if(m_lpSceneObjectEditorDoc)
	{
		m_lpSceneObjectEditorDoc->SetModifiedFlag(TRUE);
	}*/
}

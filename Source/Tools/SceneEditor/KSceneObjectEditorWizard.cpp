// KSceneObjectEditorWizard.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectEditorWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorWizard dialog


KSceneObjectEditorWizard::KSceneObjectEditorWizard(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneObjectEditorWizard::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneObjectEditorWizard)
	m_bBrush = FALSE;
	m_bBuilding = FALSE;
	m_bHorRotate = FALSE;
	m_bPitchOnly = FALSE;
	m_ToCrid = FALSE;
	m_bGrass = FALSE;
	m_bItem = FALSE;
	m_bNPC = FALSE;
	m_bOranment = FALSE;
	m_bOranmentBrakable = FALSE;
	m_bPlayer = FALSE;
	m_bTree = FALSE;
	m_bWall = FALSE;
	//}}AFX_DATA_INIT
}


void KSceneObjectEditorWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneObjectEditorWizard)
	DDX_Check(pDX, IDC_BRUSH, m_bBrush);
	DDX_Check(pDX, IDC_BULIDING, m_bBuilding);
	DDX_Check(pDX, IDC_CHECK_HORROTATE, m_bHorRotate);
	DDX_Check(pDX, IDC_CHECK_PITCHONLY, m_bPitchOnly);
	DDX_Check(pDX, IDC_CHECK_TOCELL, m_ToCrid);
	DDX_Check(pDX, IDC_GRASS, m_bGrass);
	DDX_Check(pDX, IDC_ITEM, m_bItem);
	DDX_Check(pDX, IDC_NPC, m_bNPC);
	DDX_Check(pDX, IDC_ORANMENT, m_bOranment);
	DDX_Check(pDX, IDC_ORANMENTBREAKABLE, m_bOranmentBrakable);
	DDX_Check(pDX, IDC_PLAYER, m_bPlayer);
	DDX_Check(pDX, IDC_TREE, m_bTree);
	DDX_Check(pDX, IDC_WALL, m_bWall);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(KSceneObjectEditorWizard, CDialog)
	//{{AFX_MSG_MAP(KSceneObjectEditorWizard)
	ON_BN_CLICKED(IDC_BULIDING, OnBuliding)
	ON_BN_CLICKED(IDC_ORANMENT, OnOranment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorWizard message handlers

void KSceneObjectEditorWizard::UnCheckAllTypeCheck()
{
	m_bBuilding = FALSE;
	m_bOranment = FALSE;
	m_bOranmentBrakable = FALSE;

	UpdateData(FALSE);
}


void KSceneObjectEditorWizard::OnBuliding() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	UnCheckAllTypeCheck();
	m_bBuilding = TRUE;
	UpdateData(FALSE);
}

void KSceneObjectEditorWizard::OnOranment() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	UnCheckAllTypeCheck();
	m_bOranment = TRUE;
	UpdateData(FALSE);
}

BOOL KSceneObjectEditorWizard::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UnCheckAllTypeCheck();
	m_bBuilding = TRUE;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

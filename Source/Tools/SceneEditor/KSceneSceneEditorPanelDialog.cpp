// KSceneSceneEditorPanelDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneSceneEditorPanelDialog.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月27日

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(KSceneSceneEditorPanelDialog, CDialog)

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorPanelDialog dialog


KSceneSceneEditorPanelDialog::KSceneSceneEditorPanelDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorPanelDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(KSceneSceneEditorPanelDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

KSceneSceneEditorPanelDialog::~KSceneSceneEditorPanelDialog()
{
	//m_DialogTerrian.SetSceneEditor(NULL);
	//m_DialogOranment.SetSceneEditor(NULL);
	//m_DialogGrass.SetSceneEditor(NULL);
	//m_DialogWater.SetSceneEditor(NULL);
	//m_DialogMisc.SetSceneEditor(NULL);
	//m_DialogLogical.SetSceneEditor(NULL);
	//m_DialogPostRender.SetSceneEditor(NULL);
	//m_DialogCamera.SetSceneEditor(NULL);
	//m_DialogNPC.SetSceneEditor(NULL);	
	
	m_DialogLogical.DestroyWindow();
	DestroyWindow();
}

void KSceneSceneEditorPanelDialog::SetSceneEditor(IEKG3DSceneSceneEditor *pSceneEditor,KSceneEditorDocLogical* pLogical,KSceneEditorDoc* pDoc)
{
	m_lpSceneEditor = pSceneEditor;
	m_DialogTerrian.SetSceneEditor(pSceneEditor);
	m_DialogOranment.SetSceneEditor(pSceneEditor);
	m_DialogGrass.SetSceneEditor(pSceneEditor);
	m_DialogWater.SetSceneEditor(pSceneEditor);
	m_DialogMisc.SetSceneEditor(pSceneEditor);
	m_DialogLogical.SetSceneEditor(pSceneEditor);
	m_DialogLogical.SetSceneEditorDoc(pDoc);

	m_DialogPostRender.SetSceneEditor(pSceneEditor);
	m_DialogCamera.SetSceneEditor(pSceneEditor);
	m_DialogNPC.SetSceneEditor(pSceneEditor,pLogical);
    m_DialogMovie.SetSceneEditor(pSceneEditor);
	m_DialogDoodad.SetSceneEditor(pSceneEditor,pLogical);
	m_DialogWayPoint.SetSceneEditor(pSceneEditor);
	m_DialogWayPoint.SetDocLogical(pLogical);
	m_DialogTrafficPoint.SetSceneEditor(pSceneEditor);
	m_DialogTrafficPoint.SetDocLogical(pLogical);
	m_DialogGroup.SetSceneEditor(pSceneEditor);
	m_DialogGroup.SetDocLogical(pLogical);
	m_DialogNickName.SetSceneEditor(pSceneEditor);
	m_DialogNickName.SetDocLogical(pLogical);
	m_DialogOption.SetSceneEditor(pSceneEditor);
	m_DialogOption.SetDocLogical(pLogical);
	m_DialogOption.SetDoc(pDoc);

	m_DialogPoly.SetEditor(pDoc,pSceneEditor);
	m_DialogTCopy.SetSceneEditor(pSceneEditor);
    m_DialogPvs.SetSceneEditor(pSceneEditor);
	
}

void KSceneSceneEditorPanelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(KSceneSceneEditorPanelDialog)
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(KSceneSceneEditorPanelDialog, CDialog)
	//{{AFX_MSG_MAP(KSceneSceneEditorPanelDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorPanelDialog message handlers
void KSceneSceneEditorPanelDialog::OnSize(UINT nType, int cx, int cy)
{
	CRect rectClient;
	CRect rectOranment;

	GetClientRect(&rectClient);
	KG_PROCESS_ERROR(m_DialogOranment);

	m_DialogOranment.GetWindowRect(&rectOranment);
	ScreenToClient(&rectOranment);

	rectOranment.left   = rectClient.left;
	rectOranment.right  = rectClient.right;
	rectOranment.bottom = rectClient.bottom;

	m_DialogOranment.MoveWindow(&rectOranment);

	//(by dengzhihui 2006年7月28日 13:57:36
	rectClient.top += 24;
	for(size_t i=0;i<m_vecDialog.size();i++)
	{		
		ASSERT(m_vecDialog[i].pDialog);
		ASSERT_VALID(m_vecDialog[i].pDialog);
		if (m_vecDialog[i].pDialog)
		{		
			m_vecDialog[i].pDialog->MoveWindow(&rectClient);
		}
	}
	//)
	
Exit0:
	return CDialog::OnSize(nType, cx, cy);
}

BOOL KSceneSceneEditorPanelDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//m_DialogLogical.Create(m_DialogLogical.IDD,this);
	
	m_DialogTerrian .Create(m_DialogTerrian.IDD,this);
	m_DialogOranment.Create(m_DialogOranment.IDD,this);
	m_DialogDistrict.Create(m_DialogDistrict.IDD,this);
	m_DialogCamera  .Create(m_DialogCamera.IDD,this);
	m_DialogGrass.Create(m_DialogGrass.IDD,this);
	m_DialogWater.Create(m_DialogWater.IDD,this);
	m_DialogMisc.Create(m_DialogMisc.IDD,this);
	m_DialogLogical.Create(m_DialogLogical.IDD,this);
	m_DialogPostRender.Create(m_DialogPostRender.IDD,this);
	m_DialogNPC.Create(m_DialogNPC.IDD,this);
    m_DialogMovie.Create(m_DialogMovie.IDD, this);
	m_DialogDoodad.Create(m_DialogDoodad.IDD,this);
	m_DialogWayPoint.Create(m_DialogWayPoint.IDD,this);
	m_DialogTrafficPoint.Create(m_DialogTrafficPoint.IDD,this);
	m_DialogGroup.Create(m_DialogGroup.IDD,this);
	m_DialogNickName.Create(m_DialogNickName.IDD,this);
	m_DialogOption.Create(m_DialogOption.IDD,this);
	m_DialogPoly.Create(m_DialogPoly.IDD,this);
	m_DialogTCopy.Create(m_DialogTCopy.IDD,this);
	m_DialogLight.Create(m_DialogLight.IDD, this);
    m_DialogPvs.Create(m_DialogPvs.IDD, this);

	//MoveWindow(CRect(730, 50, 940, 950));
	//
	int nScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(nScreenWidth - 270, 10, 230, nScreenHeight - 50);
	//

	m_vecDialog.push_back(DialogInfo(&m_DialogTerrian,"地形面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogOranment,"地形装饰物面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogDistrict,"地区面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogCamera,"镜头面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogGrass,"草面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogWater,"水面面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogMisc,"杂项面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogLogical,"逻辑数据面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogPostRender,"屏幕特效面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogNPC,"NPC面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogDoodad,"Doodad面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogWayPoint,"路径点面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogTrafficPoint,"交通点面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogGroup,"DOODAD NPC 群组面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogNickName,"别名面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogOption,"控制选项面板"));
    m_vecDialog.push_back(DialogInfo(&m_DialogMovie, "Movie"));
	m_vecDialog.push_back(DialogInfo(&m_DialogPoly, "多边形范围编辑"));
	m_vecDialog.push_back(DialogInfo(&m_DialogTCopy, "地形复制粘贴面板"));
	m_vecDialog.push_back(DialogInfo(&m_DialogLight, "灯光面板"));
    m_vecDialog.push_back(DialogInfo(&m_DialogPvs, "Pvs 面板"));

	MoveAllWindow();
	AddToComboBox();

	ShowDialog(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void KSceneSceneEditorPanelDialog::UpdateUI()
{
    m_DialogPvs.UpdateUI();
}

void KSceneSceneEditorPanelDialog::ShowDialog(LPCTSTR strName)
{
    for (size_t i = 0; i < m_vecDialog.size(); ++i)
        if (stricmp(m_vecDialog[i].Title, strName) == 0)
            ShowDialog(static_cast<int>(i));
}

void KSceneSceneEditorPanelDialog::ShowDialog(int nIndex)
{
	static int dwIndex = -1;
	static DWORD dwState = SCENESTATE_SELECT;

	for(size_t i=0;i<m_vecDialog.size();i++)
	{
		if(m_vecDialog[i].Index == dwIndex) 
		{
			if (m_vecDialog[i].pDialog == &m_DialogTCopy)
				m_lpSceneEditor->GetEditState(&dwState);
		}
	}

	for(size_t i=0;i<m_vecDialog.size();i++)
	{
		if(m_vecDialog[i].Index==nIndex)
		{
			m_vecDialog[i].pDialog->ShowWindow(TRUE);
			//显示逻辑面板时，自动打开逻辑显示，非显示逻辑面板时，自动关闭
			//m_lpSceneEditor->m_bShowLogicalCell = m_vecDialog[i].pDialog == &m_DialogLogical;
			//m_lpSceneEditor->m_dwEditState = m_vecDialog[i].pDialog == &m_DialogLogical ? SCENESTATE_CELLLOGICAL : SCENESTATE_SELECT;
			m_lpSceneEditor->SetEditState(m_vecDialog[i].pDialog == &m_DialogLogical ? SCENESTATE_CELLLOGICAL : SCENESTATE_SELECT);
			if (m_vecDialog[i].pDialog == &m_DialogTCopy)
				m_lpSceneEditor->SetEditState(dwState);					
		}
		else 
		{
			m_vecDialog[i].pDialog->ShowWindow(FALSE);			
		}		
	}
	dwIndex = nIndex;

    m_ComboBox.SetCurSel(nIndex);
}
void KSceneSceneEditorPanelDialog::AddToComboBox()
{
	m_ComboBox.Clear();
	for(size_t i=0;i<m_vecDialog.size();i++)
	{
		m_vecDialog[i].Index = m_ComboBox.AddString(m_vecDialog[i].Title);
	}
}

void KSceneSceneEditorPanelDialog::MoveAllWindow()
{
	CRect rectFrame;
	GetClientRect(&rectFrame);

	for(size_t i=0;i<m_vecDialog.size();i++)
	{
		//m_vecDialog[i].pDialog->MoveWindow(CRect(0, 40, 250, 950));
		//
		CRect rectDlgNew;
		CRect rectDlgOld;
		m_vecDialog[i].pDialog->GetWindowRect(&rectDlgOld);
		rectDlgNew.left   = (rectFrame.Width() - rectDlgOld.Width()) / 2;
		rectDlgNew.right  = rectDlgNew.left + rectDlgOld.Width();
		rectDlgNew.top    = 24;
		rectDlgNew.bottom = rectFrame.bottom - 10;
		m_vecDialog[i].pDialog->MoveWindow(&rectDlgNew);

		//
	}
}

void KSceneSceneEditorPanelDialog::OnSelchangeCombo() 
{
	// TODO: Add your control notification handler code here
	ShowDialog(m_ComboBox.GetCurSel());
}

//(by dengzhihui 2006年7月14日 12:09:48
void KSceneSceneEditorPanelDialog::OnOK()
{

}
void KSceneSceneEditorPanelDialog::OnCancel()
{

}
//)

void KSceneSceneEditorPanelDialog::SetDocument(CDocument* pDoc, BOOL bSave)
{
	m_DialogLogical.SetDocument(pDoc, bSave);
}

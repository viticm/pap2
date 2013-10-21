// KSceneModelEditorFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneModelEditorFrameWnd.h"
#include "KSceneModelEditorPanelView.h"
#include "MeshEditorView.h"
#include "KSceneModelEditorAnimationPanel.h"
#include "MeshEditorDoc.h"
#include "KSceneModelEditorPanelView.h"
#include "KSceneModelEditorAnimationPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int s_AnimationPanelHeight = 150;
/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorFrameWnd

IMPLEMENT_DYNCREATE(KSceneModelEditorFrameWnd, CChildFrame)

KSceneModelEditorFrameWnd::KSceneModelEditorFrameWnd()
{
	m_bSplitterCreated = FALSE;
}

KSceneModelEditorFrameWnd::~KSceneModelEditorFrameWnd()
{
}

void KSceneModelEditorFrameWnd::ActivateFrame(int nCmdShow) 
{
	// TODO: Add your specialized code here and/or call the base class
	nCmdShow = SW_SHOWMAXIMIZED;
	MFCFramework::CChildFrame::ActivateFrame(nCmdShow);
}

BEGIN_MESSAGE_MAP(KSceneModelEditorFrameWnd, CChildFrame)
	//{{AFX_MSG_MAP(KSceneModelEditorFrameWnd)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER_MODEL_CHANGE, OnModelChange)
	//}}AFX_MSG_MAP
    ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorFrameWnd message handlers

LRESULT KSceneModelEditorFrameWnd::OnModelChange(WPARAM wParam, LPARAM lParam)
{
	int nCurSel = -1;
	HRESULT hr = S_OK;
	IEKG3DModel* pModel = NULL;
	KSceneModelEditorDoc* pDoc = static_cast<KSceneModelEditorDoc*>(GetActiveDocument());
	ASSERT(pDoc);
	hr = pDoc->GetCurrentModel(&pModel);
	KG_COM_PROCESS_ERROR(hr);
	KSceneModelEditorPanelView* pLeftPanel = GetLeftPanel();
	KG_PROCESS_ERROR(pLeftPanel);
	
	nCurSel = pLeftPanel->m_comboPartList.GetCurSel();
	pLeftPanel->UpdateModelPartList();
	KSceneModelEditorPropertyPageMesh* pMeshPanel = GetMeshPanel();
	if (pMeshPanel)
	{
		pMeshPanel->SetCurModel(NULL,NULL);
		pMeshPanel->SetCurModel(pModel,pModel);
	}

	KSceneModelEditorPropertyPageMaterial* pMaterialPanel = GetMaterialPanel();
	if (pMaterialPanel)
	{
		pMeshPanel->SetCurModel(NULL,NULL);
		pMaterialPanel->SetCurModel(pModel);
	}

    KSceneModelEditorPropertyPageAnimation* pAnimationPanel = GetAnimationPanel();
    if (pAnimationPanel)
    {
        pAnimationPanel->SetModel(NULL);
        pAnimationPanel->SetModel(pModel);
    }

    {
        pLeftPanel->SetCurrentPartIndex((LPTSTR)wParam, nCurSel);
	    pLeftPanel->OnCbnSelchangeComboModelpart();
    }

    KSceneModelEditorAnimationPanel* pDownPanel = GetAnimationSeekBar();
    if (pDownPanel)
    {
        pModel->GetAnimationTagContainer(&(pDownPanel->m_pContainer));
    }
	
Exit0:
	return 0;
}

BOOL KSceneModelEditorFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_wndSplitter.CreateStatic(this, 1, 2);

	m_wndSplitterLeft.CreateStatic(&m_wndSplitter,2,1,WS_CHILD|WS_VISIBLE,m_wndSplitter.IdFromRowCol(0,0));
	
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(KSceneModelEditorPanelView), CSize(218, 10), pContext);

	m_wndSplitterLeft.CreateView(0, 0, RUNTIME_CLASS(KSceneModelEditorView),CSize(550, 10), pContext);
	m_wndSplitterLeft.CreateView(1, 0, RUNTIME_CLASS(KSceneModelEditorAnimationPanel), CSize(180, 150), pContext);
	
	CRect rc;
	GetClientRect(&rc);
	m_wndSplitter.SetColumnInfo(1,180,180);
	m_wndSplitter.SetColumnInfo(0,600,30);

	m_wndSplitterLeft.SetRowInfo(0,600,600);
	m_wndSplitterLeft.SetRowInfo(1,600,250);
	m_wndSplitterLeft.SetColumnInfo(0,600,180);

	m_wndSplitter.RecalcLayout();
	m_wndSplitterLeft.RecalcLayout();

	m_bSplitterCreated = TRUE;
	return TRUE;
}

void KSceneModelEditorFrameWnd::OnSize(UINT nType, int cx, int cy) 
{
	MFCFramework::CChildFrame::OnSize(nType, cx, cy);
	
	if(m_bSplitterCreated)
	{
		CRect rc;
		GetClientRect(&rc);
		if(rc.left >= rc.right || rc.bottom <= rc.top)
			return;
		m_wndSplitter.SetColumnInfo(0,cx - 240,60);
		m_wndSplitter.SetColumnInfo(1,180,10);

		m_wndSplitterLeft.SetRowInfo(0, cy - s_AnimationPanelHeight,20);
		m_wndSplitterLeft.SetRowInfo(1, s_AnimationPanelHeight, 10);
		m_wndSplitterLeft.SetColumnInfo(0,cx - 240,60);

		m_wndSplitter.RecalcLayout();
		m_wndSplitterLeft.RecalcLayout();
	}
}

int KSceneModelEditorFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (MFCFramework::CChildFrame::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here
    return 0;
}


KSceneModelEditorPropertyPageMesh* KSceneModelEditorFrameWnd::GetMeshPanel()
{
	KSceneModelEditorPanelView* pPanel = static_cast<KSceneModelEditorPanelView*>(m_wndSplitter.GetPane(0, 1));
	KG_PROCESS_ERROR(pPanel);
	return pPanel->GetMeshPanel();
Exit0:
	return NULL;
}

KSceneModelEditorPropertyPageMaterial* KSceneModelEditorFrameWnd::GetMaterialPanel()
{
	KSceneModelEditorPanelView* pPanel = static_cast<KSceneModelEditorPanelView*>(m_wndSplitter.GetPane(0, 1));
	KG_PROCESS_ERROR(pPanel);
	return pPanel->GetMaterialPanel();
Exit0:
	return NULL;
}

KSceneModelEditorPropertyPageAnimation* KSceneModelEditorFrameWnd::GetAnimationPanel()
{
    KSceneModelEditorPanelView* pPanel = static_cast<KSceneModelEditorPanelView*>(m_wndSplitter.GetPane(0, 1));
    KG_PROCESS_ERROR(pPanel);
    return pPanel->GetAnimationPanel();
Exit0:
    return NULL;
}

KSceneModelEditorPanelView* KSceneModelEditorFrameWnd::GetLeftPanel()
{
	return static_cast<KSceneModelEditorPanelView*>(m_wndSplitter.GetPane(0, 1));
}

KSceneModelEditorAnimationPanel* KSceneModelEditorFrameWnd::GetAnimationSeekBar()
{
    return static_cast<KSceneModelEditorAnimationPanel*>(m_wndSplitterLeft.GetPane(1, 0));
}

KSceneModelEditorView* KSceneModelEditorFrameWnd::GetModelEditorView()
{
    return static_cast<KSceneModelEditorView*>(m_wndSplitterLeft.GetPane(0, 0));
}
// KSceneSceneEditorFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "SceneEditorView.h"
#include "KSceneSceneEditorFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorFrameWnd

IMPLEMENT_DYNCREATE(KSceneSceneEditorFrameWnd, CMDIChildWnd)

KSceneSceneEditorFrameWnd::KSceneSceneEditorFrameWnd()
{
	m_bSplitterCreated = FALSE;
}

KSceneSceneEditorFrameWnd::~KSceneSceneEditorFrameWnd()
{
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorFrameWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(KSceneSceneEditorFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorFrameWnd message handlers

int KSceneSceneEditorFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void KSceneSceneEditorFrameWnd::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(m_bSplitterCreated)
	{
		m_wndSplitter.SetColumnInfo(0,180,180);
		m_wndSplitter.SetColumnInfo(1,600,30);
		m_wndSplitter.RecalcLayout();
	}
}

BOOL KSceneSceneEditorFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

	m_wndSplitter.CreateStatic(this, 1, 2);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(KSceneEditorView), CSize(10, 10), pContext);
	
	m_bSplitterCreated = TRUE;

	CRect rc;
	GetClientRect(&rc);
	m_wndSplitter.SetColumnInfo(0,180,180);
	m_wndSplitter.SetColumnInfo(1,600,30);

	m_wndSplitter.RecalcLayout();

	return TRUE;
	//CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

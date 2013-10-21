// KSceneObjectEditorFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "sceneeditor.h"
#include "KSceneObjectEditorFrameWnd.h"

#include "KSceneObjectEditorView.h"
#include "KSceneObjectEditorPanelView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorFrameWnd

IMPLEMENT_DYNCREATE(KSceneObjectEditorFrameWnd, CChildFrame)

KSceneObjectEditorFrameWnd::KSceneObjectEditorFrameWnd()
{
	m_bSplitterCreated = FALSE;
}

KSceneObjectEditorFrameWnd::~KSceneObjectEditorFrameWnd()
{
}

void KSceneObjectEditorFrameWnd::ActivateFrame(int nCmdShow) 
{
	// TODO: Add your specialized code here and/or call the base class
	nCmdShow = SW_SHOWMAXIMIZED;
	MFCFramework::CChildFrame::ActivateFrame(nCmdShow);
}


BEGIN_MESSAGE_MAP(KSceneObjectEditorFrameWnd, MFCFramework::CChildFrame)
	//{{AFX_MSG_MAP(KSceneObjectEditorFrameWnd)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorFrameWnd message handlers

BOOL KSceneObjectEditorFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_wndSplitter.CreateStatic(this, 1, 2);

	m_wndSplitter.CreateView(
        0, 0, RUNTIME_CLASS(KSceneObjectEditorView), CSize(600, 10), pContext
    );
	m_wndSplitter.CreateView(
        0, 1, RUNTIME_CLASS(KSceneObjectEditorPanelView), CSize(60, 10), pContext
    );
	
	m_bSplitterCreated = TRUE;

	return TRUE;
	//MFCFramework::CChildFrame::OnCreateClient(lpcs, pContext);
}

void KSceneObjectEditorFrameWnd::OnSize(UINT nType, int cx, int cy) 
{
	MFCFramework::CChildFrame::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
    CRect rect;
    GetClientRect(&rect);
    int lwidth = rect.Width() - 130 ;

	if(m_bSplitterCreated)
	{
		m_wndSplitter.SetColumnInfo(0,lwidth,10);
		m_wndSplitter.RecalcLayout();
	}

}

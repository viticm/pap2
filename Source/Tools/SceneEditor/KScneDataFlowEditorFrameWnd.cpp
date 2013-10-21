// KScneDataFlowEditorFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KScneDataFlowEditorFrameWnd.h"
#include "KSceneDataFlowEditorPanelView.h"
#include "KSceneDataFlowEditorView.h"

// KScneDataFlowEditorFrameWnd

IMPLEMENT_DYNCREATE(KScneDataFlowEditorFrameWnd, CMDIChildWnd)

KScneDataFlowEditorFrameWnd::KScneDataFlowEditorFrameWnd()
{

}

KScneDataFlowEditorFrameWnd::~KScneDataFlowEditorFrameWnd()
{
}

BOOL KScneDataFlowEditorFrameWnd::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	//return m_wndSplitter.Create(this,
	//	2, 2,       // TODO: adjust the number of rows, columns
	//	CSize(10, 10),  // TODO: adjust the minimum pane size
	//	pContext);
	m_wndSplitter.CreateStatic(this, 1, 2);

	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(KSceneDataFlowEditorPanelView), CSize(800, 10), pContext);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(KSceneDataFlowEditorView),CSize(550, 10), pContext);


	CRect rc;
	GetClientRect(&rc);
	m_wndSplitter.SetColumnInfo(0,rc.Width() - 400,30);
	m_wndSplitter.SetColumnInfo(1,400,180);

	m_wndSplitter.RecalcLayout();

	//m_bSplitterCreated = TRUE;
	return TRUE;

}

BEGIN_MESSAGE_MAP(KScneDataFlowEditorFrameWnd, CMDIChildWnd)
	ON_WM_SIZE()

END_MESSAGE_MAP()


// KScneDataFlowEditorFrameWnd message handlers
void KScneDataFlowEditorFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType,cx,cy);

	/*CRect rc;
	GetClientRect(&rc);
	m_wndSplitter.SetColumnInfo(1,180,180);
	m_wndSplitter.SetColumnInfo(0,rc.Width() - 400,30);

	m_wndSplitter.RecalcLayout();*/

}
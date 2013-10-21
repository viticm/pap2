// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "RecordeOperator.h"
#include "RecordeOperatorDoc.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "IRecPlayer.h"
#include "KRecordUITools.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_GETMINMAXINFO()
	ON_WM_CHILDACTIVATE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	m_bFullScreen = FALSE;
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect   rect(50,50,1074,818);     
	::AdjustWindowRectEx(&rect,   GetStyle(),   FALSE,   GetExStyle());     
	MoveWindow(&rect);         
	return 0;
}

void CChildFrame::FullScreen(CRect recFullScreen)
{
	WINDOWPLACEMENT wpNew;
	if (!m_bFullScreen)
	{
		GetWindowPlacement (&m_wpPrev);
		m_wpPrev.length = sizeof m_wpPrev;
		m_FullScreenWindowRect = recFullScreen;
		wpNew = m_wpPrev;
		wpNew.showCmd =  SW_SHOWNORMAL;
		wpNew.rcNormalPosition = recFullScreen;
		m_bFullScreen=TRUE;
	}
	else
	{
		m_bFullScreen=FALSE;
		wpNew = m_wpPrev;
	}
	SetWindowPlacement(&wpNew);
}

// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers

void CChildFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_bFullScreen)
	{
		lpMMI->ptMaxSize.y = m_FullScreenWindowRect.Height();
		lpMMI->ptMaxTrackSize.y = lpMMI->ptMaxSize.y;
		lpMMI->ptMaxSize.x = m_FullScreenWindowRect.Width();
		lpMMI->ptMaxTrackSize.x = lpMMI->ptMaxSize.x;
	}

}
void CChildFrame::OnChildActivate()
{
	CRecordeOperatorDoc* pDoc = (CRecordeOperatorDoc*)GetActiveDocument();
	KG_PROCESS_ERROR(pDoc);
	IERecPlayer* pRecord = pDoc->GetRecord();
	KG_PROCESS_ERROR(pRecord);
	CMainFrame* pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd; 
	KG_PROCESS_ERROR(pMain);
	if (pRecord->GetPlayarState() == IERecPlayer::PLAY_STATE_PLAY)
		(pMain->m_wndProgressDialogBar).m_buttonPlay.SetWindowText("Pause");
	else
		(pMain->m_wndProgressDialogBar).m_buttonPlay.SetWindowText("Play");
	if (pRecord->IsFreeCamera())
		(pMain->m_wndProgressDialogBar).m_buttonFreeCamera.SetWindowText("RestoreCamera");
	else
		(pMain->m_wndProgressDialogBar).m_buttonFreeCamera.SetWindowText("FreeCamera");

	g_RecordUITools.FillListTree(pRecord, (pMain->m_wndMasterlistDialogBar).m_treeMasters);
Exit0:
	CMDIChildWnd::OnChildActivate();
}

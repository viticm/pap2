// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "RecordeOperator.h"
#include "MainFrm.h"
#include "RecordeOperatorDoc.h"
#include "ChildFrm.h"
#include "KGListCtrl.h"
#include "IEOther.h"
#include "KRecordUITools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IKG3DEngineManager* g_p3DEngine;
TCHAR g_szDefWorkDirectory[MAX_PATH];
BOOL g_bSlider;
IEKG3DGraphicsTool* g_p3DTools;
HMODULE g_hMod;
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CMainFrame::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CMainFrame::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_FREE, &CMainFrame::OnBnClickedButtonFree)
	ON_NOTIFY(TVN_ITEMEXPANDING,IDC_MASTER_TREE,OnExpandingTree)
	ON_NOTIFY(TVN_SELCHANGED,IDC_MASTER_TREE,OnSelChangedTree)
	ON_NOTIFY(NM_RELEASEDCAPTURE,IDC_SLIDER_PROGRESS,OnReleaseCapture)
	ON_COMMAND(ID_FULLSCREEN, OnViewFullScreen)
	ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewFullScreen)
	ON_COMMAND(ID_VIEW_MOVIEOUTOPTION,OnMovieOutOption)
	ON_WM_GETMINMAXINFO()
	ON_UPDATE_COMMAND_UI(ID_FULLSCREEN, OnUpdateViewFullScreen)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bFullScreen=FALSE;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	typedef HRESULT (*fnGet3DEngineInterface)(void**);
	fnGet3DEngineInterface pfnGet3DEngineInterface = NULL;
	g_hMod = NULL;
	HRESULT hr = E_FAIL;
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		//TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		//TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if(!m_wndProgressDialogBar.Create(this,IDD_PROGRESS_DIALOGBAR,
		CBRS_ALIGN_BOTTOM | CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY,IDD_PROGRESS_DIALOGBAR))
	{
		//TRACE0("Failed to create toolbar\n");
		return -1;
	}
	
	if(!m_wndMasterlistDialogBar.Create(this,IDD_MASTERLIST_DIALOGBAR,
		CBRS_ALIGN_RIGHT | CBRS_RIGHT | CBRS_TOOLTIPS | CBRS_FLYBY,IDD_MASTERLIST_DIALOGBAR))
	{
		//TRACE0("Failed to create toolbar\n");
		return -1;
	}


	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_wndMasterlistDialogBar.EnableDocking(CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndMasterlistDialogBar); 

	m_wndProgressDialogBar.EnableDocking(CBRS_ALIGN_BOTTOM);
	DockControlBar(&m_wndProgressDialogBar); 
	

	g_bSlider = TRUE;
	KGLOG_PARAM LogParam = {"logs", "Record", KGLOG_OPTION_FILE, 65536};
	int nRetCode = KGLogInit(LogParam, NULL);

	g_SetRootPath();
	g_SetFilePath("");
	g_GetRootPath(g_szDefWorkDirectory);

	

	//SetCurrentDirectory();

#ifdef _DEBUG
	g_hMod = ::LoadLibraryA("KG3DEngineD.dll");
#else
	g_hMod = ::LoadLibraryA("KG3DEngine.dll");
#endif

    if (g_hMod == NULL)
		return -1;
	pfnGet3DEngineInterface = (fnGet3DEngineInterface)GetProcAddress(g_hMod, "Get3DEngineInterface");
	if (pfnGet3DEngineInterface == NULL)
		return -1;

	hr = pfnGet3DEngineInterface((void**)&g_p3DEngine);
	if (hr == E_FAIL)
		return -1;
	ASSERT(g_p3DEngine);
	IEKG3DEngineManager* pEngine = static_cast<IEKG3DEngineManager*>(g_p3DEngine);
	ASSERT(pEngine);
	pEngine->GetGraphicsTool(&g_p3DTools);
	ASSERT(g_p3DTools);
	hr = g_p3DEngine->Init(0,KG3DENG_CLIENT | KG3DENG_RECORD, m_hWnd, m_hWnd);
	if (hr == E_FAIL)
		return -1;
	//pEngine->SetAutoReloadTexture(TRUE);

	RecordOption recordOption;

	recordOption.Size.cx = 1024;
	recordOption.Size.cy = 768;
	recordOption.nAutoScale = FALSE;
	recordOption.FiterType = D3DTEXF_LINEAR;
	recordOption.fInterval = 41;
	recordOption.ImageType = D3DXIFF_JPG;
	recordOption.bKeep = FALSE;

	g_p3DEngine->SetRecordOption(&recordOption);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::DestroyWindow()
{	
	return CMDIFrameWnd::DestroyWindow();   
}

void CMainFrame::OnDestroy()
{
	CMDIFrameWnd::OnDestroy();
	if(g_p3DEngine)
		g_p3DEngine->UnInit();
	FreeLibrary(g_hMod);
	g_hMod = NULL;
	KGLogUnInit(NULL);
	
}
// CMainFrame diagnostics




void CMainFrame::OnBnClickedButtonPlay()
{
	CRecordeOperatorDoc* pDoc = NULL;
	CChildFrame* pChild = NULL;
	pChild = (CChildFrame*)GetActiveFrame();
	if (!pChild)
		return;
	pDoc = (CRecordeOperatorDoc*)pChild->GetActiveDocument();
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	if (pRecord)
	{
		if (pRecord->RePlay() == S_OK)
		{
			m_wndProgressDialogBar.m_buttonPlay.SetWindowText("Pause");
		}
		else if (pRecord->PauseRePlay() == S_OK)
		{
			m_wndProgressDialogBar.m_buttonPlay.SetWindowText("Play");
		}
	}
	else
		MessageBox("还未绑定录像对象！","呀",MB_OK);
}

void CMainFrame::OnBnClickedButtonStop()
{
	CRecordeOperatorDoc* pDoc = NULL;
	CChildFrame* pChild = NULL;
	pChild = (CChildFrame*)GetActiveFrame();
	if (!pChild)
		return;
	pDoc = (CRecordeOperatorDoc*)pChild->GetActiveDocument();
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	if (pRecord)
	{
		if (pRecord->StopRePlay() == S_OK)
		{
			m_wndProgressDialogBar.m_buttonPlay.SetWindowText("Play");
		}
	}
	else
		MessageBox("还未绑定录像对象！","呀",MB_OK);
}

void CMainFrame::OnBnClickedButtonFree()
{
	CRecordeOperatorDoc* pDoc = NULL;
	CChildFrame* pChild = NULL;
	pChild = (CChildFrame*)GetActiveFrame();
	if (!pChild)
		return;
	pDoc = (CRecordeOperatorDoc*)pChild->GetActiveDocument();
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	if (pRecord)
	{
		if (pRecord->FreeCamera() == S_OK)
		{
			if (pRecord->IsFreeCamera())
				m_wndProgressDialogBar.m_buttonFreeCamera.SetWindowText("RestoreCamera");
			else
				m_wndProgressDialogBar.m_buttonFreeCamera.SetWindowText("FreeCamera");
		}
	}
	else
		MessageBox("还未绑定录像对象！","呀",MB_OK);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}


#endif //_DEBUG


// CMainFrame message handlers
void CMainFrame::OnViewFullScreen() 
{
	RECT rectDesktop;
	WINDOWPLACEMENT wpNew;

	if (!IsFullScreen())
	{
		// need to hide all status bars
		m_wndStatusBar.ShowWindow(SW_HIDE);
		m_wndToolBar.ShowWindow(SW_HIDE);
		m_wndProgressDialogBar.ShowWindow(SW_HIDE);
		m_wndMasterlistDialogBar.ShowWindow(SW_HIDE);
		// We'll need these to restore the original state.
		GetWindowPlacement (&m_wpPrev);

		m_wpPrev.length = sizeof m_wpPrev;

		//Adjust RECT to new size of window
		::GetWindowRect(::GetDesktopWindow(), &rectDesktop);
		::AdjustWindowRectEx(&rectDesktop, GetStyle(), TRUE, GetExStyle());

		// Remember this for OnGetMinMaxInfo()
		m_FullScreenWindowRect = rectDesktop;

		wpNew = m_wpPrev;
		wpNew.showCmd =  SW_SHOWNORMAL;
		wpNew.rcNormalPosition = rectDesktop;

		m_pwndFullScrnBar=new CToolBar;

		if (!m_pwndFullScrnBar->Create(this,CBRS_SIZE_DYNAMIC|CBRS_FLOATING) ||
			!m_pwndFullScrnBar->LoadToolBar(IDR_FULLSCREEN))
		{
		//	TRACE0("Failed to create toolbar\n");
			return;      // fail to create
		}

		//don't allow the toolbar to dock
		m_pwndFullScrnBar->EnableDocking(0);
		m_pwndFullScrnBar->SetWindowPos(0, 100,100, 0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);   
		m_pwndFullScrnBar->SetWindowText(_T("Full Screen"));
		FloatControlBar(m_pwndFullScrnBar, CPoint(100,100));
		m_bFullScreen=TRUE;
	}
	else
	{
		m_pwndFullScrnBar->DestroyWindow();
		delete m_pwndFullScrnBar;

		m_bFullScreen=FALSE;

		m_wndStatusBar.ShowWindow(SW_SHOWNORMAL);
		m_wndToolBar.ShowWindow(SW_SHOWNORMAL);
		m_wndProgressDialogBar.ShowWindow(SW_SHOWNORMAL);
		m_wndMasterlistDialogBar.ShowWindow(SW_SHOWNORMAL);
		wpNew = m_wpPrev;
	}
	//pView = (CRecordeOperatorDoc*)pChild->GetActiveDocument();
	//if (!pDoc)
	//	return;
	SetWindowPlacement(&wpNew);
	CChildFrame* pChild = NULL;
	pChild = (CChildFrame*)GetActiveFrame();
	if (pChild)
		pChild->FullScreen(m_FullScreenWindowRect);
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (IsFullScreen())
	{
		lpMMI->ptMaxSize.y = m_FullScreenWindowRect.Height();
		lpMMI->ptMaxTrackSize.y = lpMMI->ptMaxSize.y;
		lpMMI->ptMaxSize.x = m_FullScreenWindowRect.Width();
		lpMMI->ptMaxTrackSize.x = lpMMI->ptMaxSize.x;
	}

}

BOOL CMainFrame::IsFullScreen()
{
	return m_bFullScreen;
}

void CMainFrame::OnUpdateViewFullScreen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();

	if (IsFullScreen())
		pCmdUI->SetCheck();
	else
		pCmdUI->SetCheck(0);

}

void  CMainFrame::OnExpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* pTreeView = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hSelected = pTreeView->itemNew.hItem;
	if (pTreeView->action == TVE_EXPAND)
	{		
		int nDeep = 0;
		HTREEITEM hTempItem = m_wndMasterlistDialogBar.m_treeMasters.GetParentItem(hSelected);
		while (hTempItem)
		{
			nDeep++;
			hTempItem = m_wndMasterlistDialogBar.m_treeMasters.GetParentItem(hTempItem);
		}
		if (nDeep == 2)
		{
			CRecordeOperatorDoc* pDoc = NULL;
			CChildFrame* pChild = NULL;
			pChild = (CChildFrame*)GetActiveFrame();
			if (!pChild)
				return;
			pDoc = (CRecordeOperatorDoc*)pChild->GetActiveDocument();
			if (!pDoc)
				return;
			IERecPlayer* pRecord = pDoc->GetRecord();
			if (pRecord)
			{
				g_RecordUITools.FillEvent(pRecord, 
					hSelected, 
					m_wndMasterlistDialogBar.m_treeMasters);
			}
		}

	}
}

void CMainFrame::OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* pTreeView = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hSelected = pTreeView->itemNew.hItem;	
	int nDeep = 0;
	HTREEITEM hTempItem = m_wndMasterlistDialogBar.m_treeMasters.GetParentItem(hSelected);
	while (hTempItem)
	{
		nDeep++;
		hTempItem = m_wndMasterlistDialogBar.m_treeMasters.GetParentItem(hTempItem);
	}
	DWORD_PTR pPoint = m_wndMasterlistDialogBar.m_treeMasters.GetItemData(hSelected); 
	CRecordeOperatorDoc* pDoc = NULL;
	CChildFrame* pChild = NULL;
	pChild = (CChildFrame*)GetActiveFrame();
	if (!pChild)
		return;
	pDoc = (CRecordeOperatorDoc*)pChild->GetActiveDocument();
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	if (pRecord)
	{
		g_RecordUITools.FillListCtrl(pRecord, nDeep,pPoint,m_wndMasterlistDialogBar.m_listCtrl);
	}
}
void CMainFrame::OnReleaseCapture(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nRunTime = m_wndProgressDialogBar.m_sliderProgress.GetPos();
	g_bSlider = TRUE;
	CRecordeOperatorDoc* pDoc = NULL;
	CChildFrame* pChild = NULL;
	pChild = (CChildFrame*)GetActiveFrame();
	if (!pChild)
		return;
	pDoc = (CRecordeOperatorDoc*)pChild->GetActiveDocument();
	if (!pDoc)
		return;
	IERecPlayer* pRecord = pDoc->GetRecord();
	if (pRecord)
		pRecord->StepToEvent(nRunTime);

}

void CMainFrame::OnMovieOutOption()
{
	KMovieOptionDialog movieDialog;
	movieDialog.DoModal();
}





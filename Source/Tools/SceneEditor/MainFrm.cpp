// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SceneEditor.h"

#include "MainFrm.h"
#include "KEditorSettingDialog.h"
#include "kcolordialog.h"
//#include "kg3denginemanager.h"

//(by dengzhihui 2006年7月5日 12:05:57
//#include "afx.h"
#include "./CommonPro.Include/XTPDepreciated.h"
#include "ChildFrm.h"
#include "IKG3DSoundShell.h"
//)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace MFCFramework
{

	//static HWND g_hWndFullScreen = NULL;
	//static CFrameWnd* g_pBigMe;
	//static GraphicsStruct::KCamera  g_Camera;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND(ID_MAX_VIEW, OnMaxView)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDITORSETTING,OnEditorSetting)
	//}}AFX_MSG_MAP
    ON_COMMAND(ID_VIEW_DEBUG_CONSOLE, &CMainFrame::OnViewDebugConsole)
	//(by dengzhihui 2006年7月5日 18:53:37
	ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
	ON_XTP_CREATECONTROL()
	ON_WM_ACTIVATEAPP()
	//)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // mouse
	ID_SEPARATOR,           // monitor
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

void CMainFrame::OnActivateApp(BOOL bActive,DWORD dwThreadID)
{
	m_bActive = bActive;
}

CMainFrame::CMainFrame() : m_bFullScreen(FALSE), m_bStatusbarShow(TRUE)
{
	//m_pDlgMainTool = NULL;
	//m_lpSceneEdit = NULL;
	m_bActive = TRUE;
}

CMainFrame::~CMainFrame()
{
}

//(by dengzhihui 2006年7月5日 13:56:36
//#define TRACE ATLTRACE
int CMainFrame::InitUI()
{
	// Create Status bar.
	// Important: All control bars including the Status Bar
	// must be created before CommandBars....
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		ATLTRACE("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Initialize the command bars
	if (!InitCommandBars())
		return -1;

	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars == NULL)
	{
		ATLTRACE("Failed to create command bars object.\n");
		return -1;      // fail to create
	}

	// Add the menu bar
	CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(
		_T("菜单"), IDR_MAINFRAME);
	if(pMenuBar == NULL)
	{
		ATLTRACE("Failed to create menu bar.\n");
		return -1;      // fail to create
	}

	// Create ToolBar
	CXTPToolBar* pToolBar = (CXTPToolBar*)
		pCommandBars->Add(_T("标准"), xtpBarTop);
	if (!pToolBar || !pToolBar->LoadToolBar(IDR_MAINFRAME))
	{
		ATLTRACE("Failed to create toolbar\n");
		return -1;
	}

	// Set Theme
	CXTPPaintManager::SetTheme(xtpThemeNativeWinXP);//xtpThemeWhidbey);
	/*CXTPPaintManager* pTheme = new CXTPNativeXPTheme();
	XTP_COMMANDBARS_ICONSINFO* pIconsInfo = pTheme->GetIconsInfo();
	pIconsInfo->bUseDisabledIcons = TRUE;
	XTPPaintManager()->SetCustomTheme(pTheme);*/

	//if (!m_wndStatusBar.Create(this) ||
	//	!m_wndStatusBar.SetIndicators(indicators,
	//	sizeof(indicators)/sizeof(UINT)))
	//{
	//		TRACE0("Failed to create status bar\n");
	//	return -1;      // fail to create
	//}

	// Set "Always Show Full Menus" option to the FALSE
	XTP_COMMANDBARS_OPTIONS* pOptions = pCommandBars->GetCommandBarsOptions();
	pOptions->bAlwaysShowFullMenus = FALSE;

	static UINT uHideCmds[] =
	{
		ID_FILE_PRINT, ID_FILE_PRINT_PREVIEW, ID_WINDOW_CASCADE, ID_FILE_PRINT_SETUP, ID_AUTOSAVE, 
		ID_SAFEMODE, ID_SETWORKDIR, ID_WINDOW_ARRANGE, ID_REFRESHGROUND
	};
	// Hide array of commands
	pCommandBars->HideCommands(uHideCmds, _countof(uHideCmds));

	UINT nCommands[] = {ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_EDIT_CUT
		, ID_EDIT_COPY, ID_EDIT_PASTE, ID_EDIT_UNDO, ID_EDIT_REDO
		, ID_FILE_NEW_OBJECT, ID_FILE_NEW_SFX
		, ID_MAX_VIEW, ID_PLAYERMODE, ID_SELECT
		, ID_MOVE, ID_ROTATE, ID_SCALE, ID_DROP, ID_MESHEDIT, ID_COORD
	};
	
	if (XTPImageManager()->IsAlphaIconsSupported())
	{
		XTPImageManager()->SetIcons(IDR_ICONS_MAINFRM_TOOLBAR_32, nCommands, sizeof(nCommands)/sizeof(UINT), CSize(24, 24) );
		//XTPImageManager()->SetIcons(IDR_ICONS_MENU_32, nCommands, sizeof(nCommands)/sizeof(UINT), CSize(16, 16) );
	} else
	{
		XTPImageManager()->SetIcons(IDR_ICONS_MAINFRM_TOOLBAR_24, nCommands, sizeof(nCommands)/sizeof(UINT), CSize(24, 24) );
		//XTPImageManager()->SetIcons(IDR_ICONS_SM1, nCommands, sizeof(nCommands)/sizeof(UINT), CSize(16, 16) );
	}
	
	pCommandBars->GetCommandBarsOptions()->szLargeIcons = CSize(24, 24);
	//pCommandBars->GetCommandBarsOptions()->szIcons = CSize(16, 16);
	pCommandBars->GetCommandBarsOptions()->bLargeIcons = TRUE;

	//pToolBar->ShowTextBelowIcons();


    /* sfx tool bar
    */


    //pToolBarCombo->SetBarID(IDR_MAINFRAME + 108);
    //pToolBarCombo->GetControls()->Add(xtpControlLabel, 0)->SetCaption(_T("Coord:"));
    //CXTPControlComboBox* pControlCombo = 
    //    (CXTPControlComboBox*)pToolBarCombo->GetControls()->Add(xtpControlComboBox, ID_COMMAND_SFX_COORDINATE);
    //pControlCombo->SetDropDownListStyle(FALSE);
    //pControlCombo->AddString(_T("Local"));
    //pControlCombo->AddString(_T("World"));
    //pControlCombo->SetCurSel(0);
    //pToolBarCombo->GetControls()->Add(xtpControlButton, ID_COMMAND_SFX_LINK)->SetCaption(_T("Link"));
    //DockRightOf(pToolBarCombo, pToolBar);




	// Load the previous state for toolbars and menus.
	//LoadCommandBars(_T("CommandBars"));
	
	return 0;
}

//)

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	IEKG3DEngineManager *pManager = NULL;

	//(by dengzhihui 2006年7月5日 13:39:54
	if (InitUI() < 0)
		return -1;
	//)


	//(by dengzhihui Refactor 2006年11月3日
    //开始初始化3D引擎
    extern BOOL Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow);
    if (
        !Init3DEngine(
            0,
            //KG3DENG_TERRAIN_DYNAMIC_LOAD,
            KG3DENG_NONE,
            m_hWnd,m_hWnd
        )
    )
    {
        AfxMessageBox("Failed to Initalize Engine Manager\n");
        //KGLOG_OUTPUT_ERROR(hret);
        return -1;
    }
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if ( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

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

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	//g_pEngineManager->Render();  
	CMDIFrameWnd::OnTimer(nIDEvent);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return CMDIFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnFileSaveAs() 
{
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Map Files (*.Map)|*.Map|";
  
	CFileDialog dlg(true, NULL, NULL,
      OFN_HIDEREADONLY , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);	
}

void CMainFrame::OnFileSaveM() 
{
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Map Files (*.Map)|*.Map|";
  
	CFileDialog dlg(true, NULL, NULL,
      OFN_HIDEREADONLY , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	if (	dlg.DoModal() == IDOK)
	{
		
	}
		
}

void CMainFrame::OnMaxView()
{
    static WINDOWPLACEMENT wndPlace;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	KG_PROCESS_ERROR(MDIGetActive());

	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars == NULL)
	{
		return;
	}
	
	m_bFullScreen = !m_bFullScreen;

	//g_bFullScreen = m_bFullScreen;
    //Get3DEngineInterface((void **)(&pManager));
    pManager->SetFullScreen(m_bFullScreen);

	if (m_bFullScreen)
		SaveCommandBars(_T("BeforeFullScreenCommandBars"));

	//hide or show toolbars and menubars
	pCommandBars->OnSetPreviewMode(m_bFullScreen);
    
	if (m_bFullScreen)
	{		
		m_bStatusbarShow = m_wndStatusBar.IsVisible();
		m_wndStatusBar.ShowWindow(SW_HIDE); 

		CRect  rectDestop;
		CWnd* pDeskTop = GetDesktopWindow();
		pDeskTop->GetWindowRect(&rectDestop);
		//
		GetWindowPlacement(&m_wndPlace);
 
		//ModifyStyle(WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX, WS_OVERLAPPED, 0);
		ModifyStyle(WS_CAPTION|WS_THICKFRAME, WS_BORDER);
		ModifyStyleEx(0, WS_EX_WINDOWEDGE, SWP_FRAMECHANGED);
		int nbw = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXBORDER);
		int nbh = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYBORDER);
		/*rectDestop.left -= nbw;
		rectDestop.right += nbw + nbw;
		rectDestop.top -= nbh;
		rectDestop.bottom += nbh + nbh;*/

		WINDOWPLACEMENT wndFullScreen;
		wndFullScreen = m_wndPlace;
		wndFullScreen.showCmd = SW_SHOWNORMAL;
		wndFullScreen.rcNormalPosition = rectDestop;
		SetWindowPlacement(&wndFullScreen);
		
		RecalcLayout();

		CMDIChildWnd* pMDIChild = NULL;
		BOOL bMaximized = FALSE;
		pMDIChild = MDIGetActive(&bMaximized);
		pMDIChild->ShowWindow(SW_HIDE);
		pMDIChild->ShowWindow(SW_SHOWMINIMIZED);


		CDocument* pDoc = pMDIChild->GetActiveDocument();
		KSceneEditorDoc* pEditDoc = dynamic_cast<KSceneEditorDoc*>(pDoc);
		if(pEditDoc)
		{
			IEKG3DScene* pScene = pEditDoc->GetScene();
			if(pScene)
			{
				g_pEngineManager->SetMaxViewScene(pScene);
			}
		}

		
		CChildFrame* pS3Frame = dynamic_cast<CChildFrame*>(pMDIChild);
		if (pS3Frame)
			pS3Frame->SetFullScreenState(TRUE);
		pMDIChild->MDIMaximize();
	}
	else
	{
		ExitFullScreen();

		CMDIChildWnd* pMDIChild = NULL;
		BOOL bMaximized = FALSE;
		pMDIChild = MDIGetActive(&bMaximized);
		CChildFrame* pS3Frame = dynamic_cast<CChildFrame*>(pMDIChild);
		if (pS3Frame)
			pS3Frame->ShowWindow(SW_RESTORE);
	}
Exit0:
	return;
}
//(by dengzhihui 2006年7月6日 14:33:27
void CMainFrame::ExitFullScreen()
{
	SetWindowPlacement(&m_wndPlace);
	ModifyStyle(WS_OVERLAPPED, WS_OVERLAPPEDWINDOW, 0);
	ModifyStyleEx(WS_EX_WINDOWEDGE, 0, SWP_FRAMECHANGED);
	m_wndStatusBar.ShowWindow(m_bStatusbarShow);
	LoadCommandBars(_T("BeforeFullScreenCommandBars"));
	
	BOOL bMaximized = FALSE;
	CChildFrame* pS3Frame = dynamic_cast<CChildFrame*>(MDIGetActive(&bMaximized));
	if (pS3Frame)
	{
		pS3Frame->SetFullScreenState(FALSE);
	}
}
//)

void CMainFrame::OnFullscreen() 
{
}

void CMainFrame::OnSelect() 
{
	//m_lpSceneEdit->m_dwState = EDITSTATE_SELECT;
}

void CMainFrame::OnMove() 
{
	//m_lpSceneEdit->m_dwState = EDITSTATE_MOVE;
}

void CMainFrame::OnEditCut() 
{
	CMDIChildWnd* pChildWnd = MDIGetActive();
	if(!pChildWnd)
		return;
	CView* pView = pChildWnd->GetActiveView();
	if(!pView)
		return;
	pView->SendMessage(ID_EDIT_CUT,0,0);
}

void CMainFrame::OnEditPaste() 
{
	CMDIChildWnd* pChildWnd = MDIGetActive();
	if(!pChildWnd)
		return;
	CView* pView = pChildWnd->GetActiveView();
	if(!pView)
		return;
	pView->SendMessage(ID_EDIT_PASTE,0,0);
}

void CMainFrame::OnEditCopy() 
{
	CMDIChildWnd* pChildWnd = MDIGetActive();
	if(!pChildWnd)
		return;
	CView* pView = pChildWnd->GetActiveView();
	if(!pView)
		return;
	pView->SendMessage(ID_EDIT_COPY,0,0);
}

void CMainFrame::OnScale() 
{
	//m_lpSceneEdit->m_dwState = EDITSTATE_SCALE;
}

void CMainFrame::OnRotate() 
{
	//m_lpSceneEdit->m_dwState = EDITSTATE_ROTATE;
}

void CMainFrame::OnDrop() 
{
	//m_lpSceneEdit->Edit_DropSelectedObject();
}

void CMainFrame::OnHeightmap() 
{
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Texture Files|*.bmp;*.jpg;*.png;*.tga;*.DDS|";
  
	CFileDialog dlg(true, NULL, NULL,
      OFN_HIDEREADONLY , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	if (	dlg.DoModal() == IDOK)
	{	
		TCHAR str[256];
		wsprintf(str,"%s",dlg.GetFileName());

		//m_lpSceneEdit->m_lpMapMesh->CreateMapData(str);
		//m_lpSceneEdit->m_lpMapMesh->UpdateMesh();

	}
}

void CMainFrame::OnEditUndo() 
{
	//m_lpSceneEdit->Edit_Undo();
	CMDIChildWnd* pChildWnd = MDIGetActive();
	if(!pChildWnd)
		return;
	CView* pView = pChildWnd->GetActiveView();
	if(!pView)
		return;
	pView->SendMessage(ID_EDIT_UNDO,0,0);
}

void CMainFrame::OnClose() 
{
	KillTimer(0);

	//(by dengzhihui 2006年7月6日 11:48:11
	// Save the current state for toolbars and menus.
	if (m_bFullScreen)
	{
		ExitFullScreen();
	}
	SaveCommandBars(_T("CommandBars"));
	//)

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnDestroy() 
{
	//if(g_pEngineManager)
	//	g_pEngineManager->UnInit();
	CMDIFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	KGLogUnInit(NULL);
}

void CMainFrame::OnEditorSetting()
{
	KEditorSettingDialog Dialog;
	Dialog.DoModal();
}


BOOL MFCFramework::CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	return CXTPMDIFrameWnd::PreTranslateMessage(pMsg);
}


void CMainFrame::OnSafemode() 
{
	//g_bSafeMode = !g_bSafeMode;
}

void CMainFrame::OnUpdateSafemode(CCmdUI* pCmdUI) 
{
	//pCmdUI->SetCheck(g_bSafeMode);
}

};

void MFCFramework::CMainFrame::OnViewDebugConsole()
{
    // TODO: Add your command handler code here
    WinExec("DebugConsole.exe", SW_SHOW);
}

void MFCFramework::CMainFrame::OnCustomize()
{
	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars != NULL)
	{
		// Instanciate the customize dialog object.
		CXTPCustomizeSheet dlg(pCommandBars);

		// Add the commands page to the customize dialog.
		CXTPCustomizeCommandsPage* pCommands = dlg.GetCommandsPage();
		pCommands->AddCategories(IDR_SCENEETYPE);

		// Use the command bar manager to initialize the
		// customize dialog.
		pCommands->InsertAllCommandsCategory();
		pCommands->InsertBuiltInMenus(IDR_SCENEETYPE);
		pCommands->InsertNewMenuCategory();

		//增加快捷方式自定义页
		CXTPCustomizeKeyboardPage pageKeyboard(&dlg);
		dlg.AddPage(&pageKeyboard);
		pageKeyboard.AddCategories(IDR_SCENEETYPE);

		// Add the options page to the customize dialog.
		CXTPCustomizeOptionsPage pageOptions(&dlg);
		dlg.AddPage(&pageOptions);
		
		// Dispaly the dialog.
		dlg.DoModal();
	}
}

BOOL MFCFramework::CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle , CWnd* pParentWnd , CCreateContext* pContext)
{
	if (!CXTPMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
		return FALSE;

	XTPShortcutManager()->LoadShortcuts(this, _T("CommandBars"));

	return TRUE;
}

//(by dengzhihui 2006年7月6日 18:24:14
int MFCFramework::CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	CXTPToolBar* pToolBar = lpCreateControl->bToolBar? DYNAMIC_DOWNCAST(CXTPToolBar, lpCreateControl->pCommandBar): NULL;
	
	UINT uID = lpCreateControl->nID;

    if (lpCreateControl->bToolBar && lpCreateControl->nID == ID_COORD)
    {
        CXTPControlComboBox* pCombo = (CXTPControlComboBox*)CXTPControlComboBox::CreateObject();
        pCombo->AddString(_T("World"));
        pCombo->AddString(_T("Local"));
        pCombo->SetCurSel(0);
        pCombo->SetWidth(100);
        pCombo->SetDropDownListStyle(FALSE);
        pCombo->SetFlags(xtpFlagManualUpdate);
        lpCreateControl->pControl = pCombo;
        return TRUE;
    }

	if (lpCreateControl->bToolBar &&
		(
		lpCreateControl->nID == ID_FILE_NEW_SFX ||
		lpCreateControl->nID == ID_SCALE
		))
	{
		lpCreateControl->buttonStyle = xtpButtonIconAndCaption;
		return TRUE;
	}

	if (uID != ID_EDIT_UNDO && uID != ID_EDIT_REDO)
		return FALSE;
	
	if (pToolBar)
	{
		CXTPControlPopup* pButton = CXTPControlPopup::CreateControlPopup(xtpControlSplitButtonPopup);

		CXTPPopupToolBar* pCmdBar = CXTPPopupToolBar::CreatePopupToolBar(GetCommandBars());
		pCmdBar->EnableCustomization(FALSE);
		pCmdBar->SetBorders(CRect(2, 2, 2, 2));
		pCmdBar->DisableShadow();


		CXTPControlListBox* pControlListBox = (CXTPControlListBox*)pCmdBar->GetControls()->Add(new CXTPControlListBox(), uID);
		pControlListBox->SetWidth(140);
		pControlListBox->SetLinesMinMax(1, 120);
		pControlListBox->SetMultiplSel(TRUE);

		CXTPControlStatic* pControlListBoxInfo = (CXTPControlStatic*)pCmdBar->GetControls()->Add(new CXTPControlStatic(), uID);
		pControlListBoxInfo->SetWidth(140);


		pButton->SetCommandBar(pCmdBar);
		pCmdBar->InternalRelease();

		lpCreateControl->pControl = pButton;
		return TRUE;
	}

	return FALSE;
}
//)
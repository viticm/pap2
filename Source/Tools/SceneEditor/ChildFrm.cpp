// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "SceneEditor.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//(by dengzhihui 2006Äê7ÔÂ5ÈÕ 12:05:57
#include <XTToolkitPro.h> // Xtreme Toolkit Pro component library
//)

/////////////////////////////////////////////////////////////////////////////
// CChildFrame
namespace MFCFramework
{

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_WM_GETMINMAXINFO()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame() : m_bFullScreen(FALSE)
{
}

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	nCmdShow = SW_SHOWMAXIMIZED;
	//CRect   rect(100,100,1387,868);     
	//::AdjustWindowRectEx(&rect,GetStyle(),FALSE,GetExStyle());     
	//MoveWindow(&rect); 
	//nCmdShow = SW_NORMAL;
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

CChildFrame::~CChildFrame()
{
}

/*
BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if ( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}
*/



/////////////////////////////////////////////////////////////////////////////
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



/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
/*
BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}
*/

void CChildFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
	if (m_bFullScreen)
	{
		CRect  rectDestop;
		CWnd* pDeskTop = GetDesktopWindow();
		pDeskTop->GetWindowRect(&rectDestop);

		lpMMI->ptMaxSize.x		=
			lpMMI->ptMaxTrackSize.x	= rectDestop.Width() * 2;

		lpMMI->ptMaxSize.y		=
			lpMMI->ptMaxTrackSize.y	= rectDestop.Height() * 2;
	}
}

LRESULT CChildFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CWnd* pView = GetActiveView();
		ASSERT(pView);
		return pView->SendMessage(XTPWM_DOCKINGPANE_NOTIFY, wParam, lParam);
		/*CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			switch (pPane->GetID())
			{
			case IDR_PANE_PROPERTIES:
				{
					if (m_wndProperties.GetSafeHwnd() == 0)
					{
						m_wndProperties.Create(WS_CHILD|
							ES_AUTOVSCROLL|ES_MULTILINE,
							CRect(0, 0, 0, 0), this, 0);
					}
					pPane->Attach(&m_wndProperties);
					break;
				}
			}
		}
		return TRUE;
		*/
	}
	return FALSE;
}
void CChildFrame::SetFullScreenState(BOOL bFullScreen)
{
	m_bFullScreen = bFullScreen;
	
	CWnd* pView = GetActiveView();
	ASSERT(pView);
	//LockWindowUpdate();
	try
	{
		pView->SendMessage(XTPWM_DOCKINGPANE_NOTIFY, 1, bFullScreen);
	}
	catch(...){}
	//UnlockWindowUpdate();
}

};

int MFCFramework::CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void MFCFramework::CChildFrame::OnClose()
{
	CMDIChildWnd::OnClose();
}

class CTestCmdUIEx : public CCmdUI
{
public:
	CTestCmdUIEx();

public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetRadio(BOOL bOn);
	virtual void SetText(LPCTSTR);

	BOOL m_bEnabled;
};

CTestCmdUIEx::CTestCmdUIEx()
{
	m_bEnabled = TRUE;  // assume it is enabled
}

void CTestCmdUIEx::Enable(BOOL bOn)
{
	m_bEnabled = bOn;
	m_bEnableChanged = TRUE;
}

void CTestCmdUIEx::SetCheck(int)
{
	// do nothing -- just want to know about calls to Enable
}

void CTestCmdUIEx::SetRadio(BOOL)
{
	// do nothing -- just want to know about calls to Enable
}

void CTestCmdUIEx::SetText(LPCTSTR)
{
	// do nothing -- just want to know about calls to Enable
}
BOOL MFCFramework::CChildFrame::OnCommand(WPARAM wParam, LPARAM lParam)
// return TRUE if command invocation was attempted
{
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

	// default routing for command messages (through closure table)

	if (hWndCtrl == NULL)
	{
		// zero IDs for normal commands are not allowed
		if (nID == 0)
			return FALSE;

		// make sure command has not become disabled before routing
		CTestCmdUIEx state;
		state.m_nID = nID;
		OnCmdMsg(nID, CN_UPDATE_COMMAND_UI, &state, NULL);
		if (!state.m_bEnabled)
		{
			//TRACE(traceAppMsg, 0, "Warning: not executing disabled command %d\n", nID);
			return TRUE;
		}

		// menu or accelerator
		nCode = CN_COMMAND;
	}
	else
	{
		return CMDIChildWnd::OnCommand(wParam, lParam);
	}

#ifdef _DEBUG
	//if (nCode < 0 && nCode != (int)0x8000)
	//TRACE(traceAppMsg, 0, "Implementation Warning: control notification = $%X.\n",
	//nCode);
#endif

	return OnCmdMsg(nID, nCode, NULL, NULL);
}
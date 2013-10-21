////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorPanel.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:17:30
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorPanel.h"


////////////////////////////////////////////////////////////////////////////////
//(by dengzhihui 2006Äê7ÔÂ6ÈÕ 14:44:36
//#include "MainFrm.h"
//)
#include "SceneEditor.h"
#include "KSceneSFXEditorPanel.h"
#include "KSceneSFXEditorParticlePage.h"
#include "KSceneSFXEditorFrame.h"
#include "KSceneSFXEditorView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneSFXEditorPanel

IMPLEMENT_DYNCREATE(KSceneSFXEditorPanel, CDialog)

KSceneSFXEditorPanel::KSceneSFXEditorPanel()
	: CDialog(KSceneSFXEditorPanel::IDD)
{
	m_lpPropertySheet = NULL;
    m_pDependWnd = NULL;
    m_dwWndState = SFX_WND_STATE_FREE;
}

KSceneSFXEditorPanel::~KSceneSFXEditorPanel()
{
	SAFE_DELETE(m_lpPropertySheet);
}

int KSceneSFXEditorPanel::SetDependWnd(CView* pDependWnd)
{
    ASSERT(pDependWnd);
    m_pDependWnd = pDependWnd;
    return TRUE;
}

void KSceneSFXEditorPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KSceneSFXEditorPanel, CDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

void KSceneSFXEditorPanel::OnPaint()
{
    CDialog::OnPaint();
}

void KSceneSFXEditorPanel::OnSize(UINT nType, int cx, int cy)
{
    RECT rect;
    GetClientRect(&rect);
    rect.bottom += 10;
    rect.left += 2;
    if (m_lpPropertySheet && ::IsWindow(m_lpPropertySheet->m_hWnd))
        m_lpPropertySheet->MoveWindow(&rect);
    CDialog::OnSize(nType, cx, cy);
}

BOOL KSceneSFXEditorPanel::PreTranslateMessage(MSG* pMsg)
{
    //if (pMsg->message == WM_LBUTTONDBLCLK)
    //    OnLButtonDblClick();
    //if (pMsg->message == WM_MOUSEWHEEL)
    //{
    //    //m_lpPropertySheet->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
    //}
     //   m_lpPropertySheet->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
    return CDialog::PreTranslateMessage(pMsg);
}
void KSceneSFXEditorPanel::OnLButtonDblClick()
{
    if (m_dwWndState == SFX_WND_STATE_FREE)
        m_dwWndState =  SFX_WND_STATE_BIND;
    else
        m_dwWndState = SFX_WND_STATE_FREE;
    UpdateWndState();
}

DWORD KSceneSFXEditorPanel::UpdateWndState()
{
    KSceneSFXEditorView* pEditView = dynamic_cast<KSceneSFXEditorView*>(m_pDependWnd);
    KG_PROCESS_ERROR(pEditView);

    if (m_dwWndState == SFX_WND_STATE_FREE)
    {
        ModifyStyle(NULL, WS_CAPTION | WS_BORDER | WS_SIZEBOX, SWP_DRAWFRAME);
        RECT rectWnd;
        GetWindowRect(&rectWnd);
        SetParent(NULL);
        MoveWindow(&rectWnd);
    }
    else
    {
        ModifyStyle(WS_CAPTION | WS_BORDER | WS_SIZEBOX, NULL, SWP_DRAWFRAME);
        SetParent(m_pDependWnd);
        MoveWindow(&(pEditView->GetPaneRect(WND_EDIT_PANE)));
    }
    pEditView->m_paneSFXKeyFrame.UpdateWndState();
    pEditView->m_paneSFXMainWnd.UpdateWndState();
Exit0:
    return m_dwWndState;
}

#ifdef _DEBUG
void KSceneSFXEditorPanel::AssertValid() const
{
	CDialog::AssertValid();
}

void KSceneSFXEditorPanel::Dump(CDumpContext& dc) const
{
	CDialog::Dump(dc);
}
#endif //_DEBUG

BOOL KSceneSFXEditorPanel::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lpPropertySheet = new KSceneSFXEditorPropertySheet(this);
    RECT rect;
    GetClientRect(&rect);
	if (!m_lpPropertySheet->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, 123))//(/*this, WS_CHILD | WS_VISIBLE))*/
	//if (!m_lpPropertySheet->Create(this, WS_CHILD | WS_VISIBLE))
    {
		delete m_lpPropertySheet;
		m_lpPropertySheet = NULL;
		return TRUE;
	}

    //m_lpPropertySheet->GetTabControl()->SetExtendedStyle(
    //    m_lpPropertySheet->GetTabControl()->GetExtendedStyle() | TCS_EX_FLATSEPARATORS | TCS_RAGGEDRIGHT);

    //LONG dwStyle = GetWindowLong(m_lpPropertySheet->GetTabControl()->m_hWnd, GWL_STYLE);
    //SetWindowLong(
    //    m_lpPropertySheet->GetTabControl()->m_hWnd, GWL_STYLE, dwStyle | TCS_VERTICAL
    //);
    //m_lpPropertySheet->GetTabControl()->ModifyStyle(0, TCS_VERTICAL);
    //m_lpPropertySheet->GetTabControl()->ModifyStyleEx(0, WS_EX_CONTEXTHELP);

    // fit the property sheet into the place holder window, and show it
	//CRect rectPropSheet;
	//this->GetWindowRect(rectPropSheet);
    INT nHeight = 800;
    INT nWidth  = 480;
	m_lpPropertySheet->SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOACTIVATE);

	//m_lpPropertySheet->GetTabControl()->SetCurSel(0);

    KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)m_pDependWnd->GetDocument();
    m_lpPropertySheet->SetSceneSFXEditorDoc( pDoc );

    return TRUE;

}

// KGSFXModelPane.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXModelPane.h"
#include "KSceneSFXEditorView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(KGSFXModelViewSheet, KGPropertySheet)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()

int KGSFXModelViewSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (KGPropertySheet::OnCreate(lpCreateStruct) == -1)
        return -1;

    AddPage(&m_pageModelView, m_pageModelView.IDD);
    AddPage(&m_pageTex, m_pageTex.IDD);
    AddPage(&m_pageSockBone, m_pageSockBone.IDD);
    AddPage(&m_pageGlobPage, m_pageGlobPage.IDD);

    return 0;
}

IEKG3DSceneSFXEditor* KGSFXModelViewSheet::GetSfxScene()
{
    if (!GetParent())
        return NULL;

    return ((KGSFXModelPane*)GetParent())->GetSfxScene();
}

void KGSFXModelViewSheet::OnPaint()
{
    KGPropertySheet::OnPaint();
    //CClientDC dc(this);
    //RECT rect;
    //GetClientRect(&rect);
    //CPen pen(PS_SOLID, 4, RGB(100, 100, 100));//RGB(240, 200, 85));
    //dc.SelectObject(&pen);
    //dc.MoveTo(rect.right, rect.top);
    //dc.LineTo(rect.right, rect.bottom);
}

void KGSFXModelViewSheet::OnSize(UINT nType, int cx, int cy)
{
    KGPropertySheet::OnSize(nType, cx, cy);
    //RECT rect;
    //GetClientRect(&rect);
    //if (::IsWindow(m_pageModelView.m_hWnd))
    //{
    //    RECT rectPage;
    //    m_pageModelView.GetWindowRect(&rectPage);
    //    ScreenToClient(&rectPage);
    //    rectPage.bottom = rect.bottom - 5;
    //    m_pageModelView.MoveWindow(&rectPage);
    //}
    //if (::IsWindow(m_pageTex.m_hWnd))
    //{
    //    RECT rectPage;
    //    m_pageTex.GetWindowRect(&rectPage);
    //    ScreenToClient(&rectPage);
    //    rectPage.bottom = rect.bottom - 5;
    //    m_pageTex.MoveWindow(&rectPage);
    //}
}
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGSFXModelPane, CDialog)

KGSFXModelPane::KGSFXModelPane(CWnd* pParent /*=NULL*/)
	: CDialog(KGSFXModelPane::IDD, pParent)
{
    m_dwWndState = SFX_WND_STATE_FREE;
    m_pDependWnd = NULL;
}

KGSFXModelPane::~KGSFXModelPane()
{
}

int KGSFXModelPane::SetDependWnd(CView* pDependWnd)
{
    ASSERT(pDependWnd);
    m_pDependWnd = pDependWnd;
    return TRUE;
}

void KGSFXModelPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(KGSFXModelPane, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &KGSFXModelPane::OnBnClickedButton1)
    ON_WM_SIZE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

IEKG3DSceneSFXEditor* KGSFXModelPane::GetSfxScene() 
{ 
    if (!m_pDependWnd)
        return NULL;
    return ((KSceneSFXEditorView*)m_pDependWnd)->GetSfxScene(); 
}

void KGSFXModelPane::OnPaint()
{
    CDialog::OnPaint();
    //CClientDC dc(this);
    //RECT rect;
    //GetClientRect(&rect);
    //CPen pen(PS_SOLID, 4, RGB(100, 100, 100));//RGB(240, 200, 85));
    //dc.SelectObject(&pen);
    //dc.MoveTo(rect.right, rect.top);
    //dc.LineTo(rect.right, rect.bottom);
}

void KGSFXModelPane::OnSize(UINT nType, int cx, int cy)
{
    RECT rectClient;
    GetClientRect(&rectClient);
    rectClient.bottom += 5;
    rectClient.right -= 2;
    if (::IsWindow(m_sheet.m_hWnd))
        m_sheet.MoveWindow(&rectClient);
    CDialog::OnSize(nType, cx, cy);
}

BOOL KGSFXModelPane::OnInitDialog()
{
    int nResult = CDialog::OnInitDialog();
    RECT rectClient;
    GetClientRect(&rectClient);
    m_sheet.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rectClient, this, 123);//Create(this, WS_CHILD | WS_VISIBLE);
    //m_sheet.Create(this, WS_CHILD | WS_VISIBLE);
    m_sheet.MoveWindow(&rectClient);
    return nResult;
}

BOOL KGSFXModelPane::PreTranslateMessage(MSG* pMsg)
{
   /* if (pMsg->message == WM_LBUTTONDBLCLK)
        OnLButtonDblClick();*/
    return CDialog::PreTranslateMessage(pMsg);
}

void KGSFXModelPane::OnLButtonDblClick()
{
    if (m_dwWndState == SFX_WND_STATE_FREE)
        m_dwWndState =  SFX_WND_STATE_BIND;
    else
        m_dwWndState = SFX_WND_STATE_FREE;
    UpdateWndState();
}

int KGSFXModelPane::UpdateUI()
{
    if (::IsWindow(m_sheet.m_hWnd))
        m_sheet.UpdateUI();
    return TRUE;
}

DWORD KGSFXModelPane::UpdateWndState()
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
        MoveWindow(&(pEditView->GetPaneRect(WND_LEFT_PANE)));
    }
    pEditView->m_paneSFXKeyFrame.UpdateWndState();
    pEditView->m_paneSFXMainWnd.UpdateWndState();
Exit0:
    return m_dwWndState;
}

void KGSFXModelPane::OnBnClickedButton1()
{
    // TODO: Add your control notification handler code here
}

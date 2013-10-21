////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGPropertySheet.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-12 13:05:54
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGPropertySheet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGPropertySheet, CWnd)

KGPropertySheet::KGPropertySheet()
{
    m_nCurIndex = 0x7fffffff;
}

KGPropertySheet::~KGPropertySheet()
{
}

BEGIN_MESSAGE_MAP(KGPropertySheet, CWnd)
    ON_NOTIFY(TCN_SELCHANGE, TABCTRL_ID, &KGPropertySheet::OnTcnSelchangeTab)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


void KGPropertySheet::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
    int nIndex = m_tabCtrl.GetCurSel();
    if (m_nCurIndex == nIndex)
        return;
    m_vecPages[m_nCurIndex]->ShowWindow(SW_HIDE);
    m_vecPages[m_nCurIndex]->OnKillActive();
    m_nCurIndex = nIndex;
    m_vecPages[m_nCurIndex]->ShowWindow(SW_SHOW);
    m_vecPages[m_nCurIndex]->OnSetActive();
    *pResult = 0;
}


void KGPropertySheet::OnSize(UINT nType, int cx, int cy)
{
    RECT rectClient;
    GetClientRect(&rectClient);
    int width  = rectClient.right - rectClient.left;
    int height = rectClient.bottom - rectClient.top;

    if (::IsWindow(m_tabCtrl.m_hWnd))
        m_tabCtrl.MoveWindow(&rectClient);

    for (DWORD i = 0; i < m_vecPages.size(); i++)
    {
        RECT rectItem;
        m_tabCtrl.GetItemRect((int)i, &rectItem);
        m_tabCtrl.GetClientRect(&rectClient);
        rectClient.top     = rectItem.bottom + 2;
        rectClient.left   += 2;
        rectClient.right  -= 2;
        rectClient.bottom -= 5;
        m_vecPages[i]->MoveWindow(&rectClient);
    }
    return CWnd::OnSize(nType, cx, cy);
}

int  KGPropertySheet::AddPage(CPropertyPage* pPageWnd, DWORD dwTemplateID)
{
    ASSERT(::IsWindow(m_tabCtrl.m_hWnd));
    ASSERT(pPageWnd);

    CString strTitle = TEXT("");
    RECT rectPage;
    RECT rectItem;

    pPageWnd->Create(dwTemplateID, &m_tabCtrl);
    pPageWnd->SetParent(&m_tabCtrl);

    SetWindowLong(pPageWnd->m_hWnd, GWL_EXSTYLE, (LONG)0);
    LONG lStyle = GetWindowLong(pPageWnd->m_hWnd, GWL_STYLE);
    lStyle = WS_CHILD;
    SetWindowLong(pPageWnd->m_hWnd, GWL_STYLE, (LONG)lStyle);

    m_tabCtrl.GetItemRect((int)m_vecPages.size(), &rectItem);
    m_tabCtrl.GetClientRect(&rectPage);
    rectPage.top     = rectItem.bottom + 5;
    rectPage.left   += 2;
    rectPage.right  -= 2;
    rectPage.bottom -= 5;
    pPageWnd->MoveWindow(&rectPage);

    pPageWnd->GetWindowText(strTitle);
    m_tabCtrl.InsertItem((int)m_vecPages.size(), strTitle);

    m_vecPages.push_back(pPageWnd);

    if (m_nCurIndex == 0x7fffffff)
    {
        m_nCurIndex = 0;
        pPageWnd->ShowWindow(SW_SHOW);
    }

    return TRUE;
}

int KGPropertySheet::GetActiveIndex( ) const
{
    return m_nCurIndex;
}

CPropertyPage* KGPropertySheet::GetActivePage() const
{
    CPropertyPage* pActivePage = NULL;

    KG_PROCESS_ERROR(m_vecPages.size());
    pActivePage = m_vecPages[m_nCurIndex];
Exit0:
    return pActivePage;
}

BOOL KGPropertySheet::SetActivePage(int nPage)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(nPage < (int)m_vecPages.size());
    KG_PROCESS_ERROR(nPage > -1);
    KG_PROCESS_ERROR(nPage != m_nCurIndex);

    m_vecPages[m_nCurIndex]->ShowWindow(SW_HIDE);
    m_vecPages[m_nCurIndex]->OnKillActive();
    m_nCurIndex = nPage;
    m_vecPages[m_nCurIndex]->ShowWindow(SW_SHOW);
    m_vecPages[m_nCurIndex]->OnSetActive();
    m_tabCtrl.SetCurSel(m_nCurIndex);

    nResult = true;
Exit0:
    return nResult;
}

void KGPropertySheet::PreSubclassWindow()
{
    LONG lStyle = ::GetClassLong(m_hWnd, GCL_STYLE);
    lStyle &= ~CS_HREDRAW;
    lStyle &= ~CS_VREDRAW;
    ::SetClassLong(m_hWnd, GCL_STYLE, (LONG)lStyle);
    CWnd::PreSubclassWindow();
}

BOOL KGPropertySheet::PreCreateWindow(CREATESTRUCT& cs)
{
    return CWnd::PreCreateWindow(cs);
}

int KGPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    RECT rectClient;
    GetClientRect(&rectClient);
    m_tabCtrl.Create(
        WS_CHILD | WS_VISIBLE | TCS_TABS | TCS_FOCUSNEVER | TCS_FOCUSONBUTTONDOWN,
        rectClient,
        this,
        TABCTRL_ID
    );
    VERIFY(font.CreateFont(
        12,                        // nHeight
        0,                         // nWidth
        0,                         // nEscapement
        0,                         // nOrientation
        FW_NORMAL,                 // nWeight
        FALSE,                     // bItalic
        FALSE,                     // bUnderline
        0,                         // cStrikeOut
        ANSI_CHARSET,              // nCharSet
        OUT_DEFAULT_PRECIS,        // nOutPrecision
        CLIP_DEFAULT_PRECIS,       // nClipPrecision
        DEFAULT_QUALITY,           // nQuality
        DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
        TEXT("ËÎÌו"))              // lpszFacename
    );
    m_tabCtrl.SetFont(&font);
    m_tabCtrl.SetMinTabWidth(12);
    m_tabCtrl.SetPadding(CSize(5, 5));
    /*
    if (m_imageList.Create(24, 24, ILC_COLOR32, 0, 8))
    {
        m_imageList.Add(AfxGetApp()->LoadIcon(IDC_FC));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDC_GLEAN));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDC_TALK));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDC_OFFER));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDC_POS));

       
        m_tabCtrl.SetImageList(&m_imageList);
    }*/

    return 0;
}

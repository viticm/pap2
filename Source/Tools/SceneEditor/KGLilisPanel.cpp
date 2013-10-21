
#include "stdafx.h"
#include "KGLilisPanel.h"
#include <typeinfo.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// LilisButton define
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(KGLilisButton, CButton)

KGLilisButton::KGLilisButton()
{
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
        TEXT("宋体"))              // lpszFacename
        );
}

KGLilisButton::~KGLilisButton()
{
}


BEGIN_MESSAGE_MAP(KGLilisButton, CButton)
    ON_WM_PAINT()
    ON_WM_DRAWITEM()
END_MESSAGE_MAP()

void KGLilisButton::OnPaint()
{
    return CButton::OnPaint();
}

void KGLilisButton::DrawItem(LPDRAWITEMSTRUCT pStruct)
{
    HPEN penLT = CreatePen(PS_SOLID, 2, RGB(220, 220, 220));
    HPEN penRB = CreatePen(PS_SOLID, 2, RGB(70, 70, 70));
    HPEN penSave;

    RECT rect  = pStruct->rcItem;

    ::FillRect(pStruct->hDC, &rect, (HBRUSH)(COLOR_BTNFACE + 1));
    if (pStruct->itemState == 0x11)
    {
        RECT rectSl = rect;
        rectSl.left += 2;
        rectSl.right -= 2;
        rectSl.top += 2;
        rectSl.bottom -= 2;
        ::FillRect(pStruct->hDC, &rectSl, ::CreateSolidBrush(RGB(240, 200, 85)));
        penSave = (HPEN)::SelectObject(pStruct->hDC, penRB);
    }
    else
    {
        penSave = (HPEN)::SelectObject(pStruct->hDC, penLT);
    }

    ::MoveToEx(pStruct->hDC, rect.left, rect.bottom, NULL);
    ::LineTo(pStruct->hDC, rect.left, rect.top);
    ::LineTo(pStruct->hDC, rect.right, rect.top);
    
    if (pStruct->itemState == 0x11)
        ::SelectObject(pStruct->hDC, penLT);
    else
        ::SelectObject(pStruct->hDC, penRB);
    ::LineTo(pStruct->hDC, rect.right, rect.bottom);
    ::LineTo(pStruct->hDC, rect.left, rect.bottom);
    
    ::SetBkMode(pStruct->hDC, TRANSPARENT);
    ::SetTextColor(pStruct->hDC, RGB(0, 0, 0));
    
    HFONT fontSave = (HFONT)::SelectObject(pStruct->hDC, font.m_hObject);
    
    CString strTitle;
    GetWindowText(strTitle);
    ::DrawText(
        pStruct->hDC,
        strTitle.GetBuffer(),
        -1,
        &rect,
        DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE
        );

    ::SelectObject(pStruct->hDC, fontSave);
    ::SelectObject(pStruct->hDC, penSave);

    ::DeleteObject(penLT);
    ::DeleteObject(penRB);
}

//-----------------------------------------------------------------------------
// LilisButton define
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(KGLilisEdit, CEdit)

KGLilisEdit::KGLilisEdit()
{
}

KGLilisEdit::~KGLilisEdit()
{
}


BEGIN_MESSAGE_MAP(KGLilisEdit, CEdit)
    ON_WM_PAINT()
    ON_WM_DRAWITEM()
END_MESSAGE_MAP()

void KGLilisEdit::OnPaint()
{
    return CEdit::OnPaint();
}

void KGLilisEdit::DrawItem(LPDRAWITEMSTRUCT pStruct)
{
    HPEN penLT = CreatePen(PS_SOLID, 2, RGB(220, 220, 220));
    HPEN penRB = CreatePen(PS_SOLID, 2, RGB(70, 70, 70));
    RECT rect  = pStruct->rcItem;
    ::FillRect(pStruct->hDC, &rect, CreateSolidBrush(RGB(200, 200, 200)));
    if (pStruct->itemState == 0x11)
        ::SelectObject(pStruct->hDC, penRB);
    else
        ::SelectObject(pStruct->hDC, penLT);
    ::MoveToEx(pStruct->hDC, rect.left, rect.bottom, NULL);
    ::LineTo(pStruct->hDC, rect.left, rect.top);
    ::LineTo(pStruct->hDC, rect.right, rect.top);
    if (pStruct->itemState == 0x11)
        ::SelectObject(pStruct->hDC, penLT);
    else
        ::SelectObject(pStruct->hDC, penRB);
    ::LineTo(pStruct->hDC, rect.right, rect.bottom);
    ::LineTo(pStruct->hDC, rect.left, rect.bottom);

    CFont font;
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
        TEXT("宋体"))              // lpszFacename
        );
    ::SetBkMode(pStruct->hDC, TRANSPARENT);
    ::SetTextColor(pStruct->hDC, RGB(70, 70, 70));
    ::SelectObject(pStruct->hDC, font.m_hObject);
    CString strTitle;
    GetWindowText(strTitle);
    ::DrawText(
        pStruct->hDC,
        strTitle.GetBuffer(),
        -1,
        &rect,
        DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE
        );
}

//-----------------------------------------------------------------------------
// LilisSheet define
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(KGLilisSheet, CDialog)

KGLilisSheet::KGLilisSheet(UINT uTemplateID, CWnd* pParent /* = NULL */)
    : CDialog(uTemplateID, pParent)
{
}

KGLilisSheet::~KGLilisSheet()
{
}

BEGIN_MESSAGE_MAP(KGLilisSheet, CDialog)
    //ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL KGLilisSheet::PreTranslateMessage(MSG* pMsg)
{
    return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH KGLilisSheet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    return ::CreateSolidBrush(RGB(197, 197, 197));
}

//-----------------------------------------------------------------------------
// LilisPanel define
//-----------------------------------------------------------------------------


IMPLEMENT_DYNAMIC(KGLilisPanelTitle, CWnd)

KGLilisPanelTitle::KGLilisPanelTitle()
{
    VERIFY(font.CreateFont(
        15,                        // nHeight
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
        "Arial")                   // lpszFacename
        );

    dwState = TRUE;
    strName = TEXT("标题");
}

KGLilisPanelTitle::~KGLilisPanelTitle()
{
}


BEGIN_MESSAGE_MAP(KGLilisPanelTitle, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_NCPAINT()
END_MESSAGE_MAP()

BOOL KGLilisPanelTitle::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

void KGLilisPanelTitle::OnNcPaint()
{
}

void KGLilisPanelTitle::OnPaint()
{
    CPaintDC dc(this);

    RECT rect;

    GetClientRect(&rect);
    rect.right -= 1;
    rect.top += 1;
    rect.bottom -= 1;

    CPen pen1(PS_SOLID, 1, RGB(0, 0, 0));
    CPen pen2(PS_SOLID, 1, RGB(100, 100, 100));

    dc.FillRect(&rect, &CBrush(RGB(180, 180, 180)));
    dc.SelectObject(&pen2);
    dc.MoveTo(rect.right, rect.top);
    dc.LineTo(rect.left, rect.top);
    dc.LineTo(rect.left, rect.bottom);
    dc.SelectObject(&pen1);
    dc.LineTo(rect.right, rect.bottom);
    dc.LineTo(rect.right, rect.top);

    RECT rand;

    rand.left   = rect.left + 7;
    rand.top    = rect.top  + 7;
    rand.bottom = rect.bottom - 7;
    rand.right  = rand.left + (rand.bottom - rand.top);

    dc.SelectObject(&pen1);
    int half = (rand.right - rand.left) / 2;

    if (dwState)
    {
        dc.MoveTo(rand.left, rand.top + half);
        dc.LineTo(rand.right + 1, rand.top + half);
    }
    else
    {
        dc.MoveTo(rand.left + half, rand.top);
        dc.LineTo(rand.left + half, rand.bottom + 1);
        dc.MoveTo(rand.left, rand.top + half);
        dc.LineTo(rand.right + 1, rand.top + half);
    }

    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(0, 0, 0));

    dc.SelectObject(&font);
    dc.DrawText(
        strName,
        &rect,
        DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE
        );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(KGLilisPanel, CWnd)

KGLilisPanel::KGLilisPanel()
{
    m_vecChileWnd.clear();
    m_nPanelOffset = 0;
    m_nStrollFlag  = FALSE;
}

KGLilisPanel::~KGLilisPanel()
{
    for (DWORD i = 0; i < m_vecChileWnd.size(); i++)
        SAFE_DELETE(m_vecChileWnd[i].pTitleBar);
}


BEGIN_MESSAGE_MAP(KGLilisPanel, CWnd)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
    //ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


BOOL KGLilisPanel::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void KGLilisPanel::OnMouseMove(UINT nFlags, CPoint point)
{
    return CWnd::OnMouseMove(nFlags, point);
}

void KGLilisPanel::OnPaint()
{
    CWnd::OnPaint();
    CClientDC dc(this);
    for (size_t s = 0; s < m_vecChileWnd.size(); s++)
    {
        if (m_vecChileWnd[s].dwState)
        {
            CPen pen1(PS_SOLID, 1, RGB(100, 100, 100));
            RECT rectTitle;
            RECT rectPanle;
            m_vecChileWnd[s].pWndPane->GetWindowRect(&rectPanle);
            m_vecChileWnd[s].pTitleBar->GetWindowRect(&rectTitle);
            ScreenToClient(&rectPanle);
            ScreenToClient(&rectTitle);
            dc.SelectObject(&pen1);
            int   halfH = (rectTitle.bottom - rectTitle.top) / 2;
            int   halfV = (rectTitle.right - rectTitle.left) / 2 + 2;
            POINT lt;   // left top
            POINT rt;   // right top
            POINT rb;   // right buttom
            POINT lb;   // left buttom

            // left side
            lt.x = rectTitle.left - 5;
            lt.y = rectTitle.top + halfH;
            rt.x = rectTitle.left;
            rt.y = rectTitle.top + halfH;
            lb.x = lt.x;
            lb.y = rectPanle.bottom + 5;
            rb.x = rectTitle.left + halfV;
            rb.y = lb.y;
            dc.MoveTo(rt);
            dc.LineTo(lt);
            dc.LineTo(lb);
            dc.LineTo(rb);

            // right side
            lt.x = rectTitle.right;
            lt.y = rectTitle.top + halfH;
            rt.x = rectTitle.right + 5;
            rt.y = rectTitle.top + halfH;
            lb.x = rectTitle.right - halfV;
            lb.y = rectPanle.bottom + 5;
            rb.x = rectTitle.right + 5;
            rb.y = lb.y;
            dc.MoveTo(lt);
            dc.LineTo(rt);
            dc.LineTo(rb);
            dc.LineTo(lb);

        }
        else
        {
            CPen pen1(PS_SOLID, 1, RGB(100, 100, 100));
            RECT rectTitle;
            m_vecChileWnd[s].pTitleBar->GetWindowRect(&rectTitle);
            ScreenToClient(&rectTitle);
            dc.SelectObject(&pen1);
            POINT lt;   // left top
            POINT rt;   // right top
            POINT rb;   // right buttom
            POINT lb;   // left buttom

            // left side
            lt.x = rectTitle.left - 5;
            lt.y = rectTitle.top + 5;
            rt.x = rectTitle.left;
            rt.y = rectTitle.top + 5;
            lb.x = lt.x;
            lb.y = rectTitle.bottom - 5;
            rb.x = rectTitle.left;
            rb.y = lb.y;
            dc.MoveTo(rt);
            dc.LineTo(lt);
            dc.LineTo(lb);
            dc.LineTo(rb);

            // right sidt
            lt.x = rectTitle.right;
            lt.y = rectTitle.top + 5;
            rt.x = rectTitle.right + 5;
            rt.y = rectTitle.top + 5;
            lb.x = lt.x;
            lb.y = rectTitle.bottom - 5;
            rb.x = rectTitle.right + 5;
            rb.y = lb.y;
            dc.MoveTo(lt);
            dc.LineTo(rt);
            dc.LineTo(rb);
            dc.LineTo(lb);
        }
    }
    //RECT rectClient;
    //GetClientRect(&rectClient);
    //dc.MoveTo(rectClient.left,   rectClient.top);
    //dc.LineTo(rectClient.right,  rectClient.top);
    //dc.LineTo(rectClient.right,  rectClient.bottom);
    //dc.LineTo(rectClient.left,   rectClient.bottom);
    //dc.LineTo(rectClient.left,   rectClient.top);
}

BOOL KGLilisPanel::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_LBUTTONUP)
    {
        m_nStrollFlag = FALSE;
        ReleaseCapture();
    }
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        if (!OnClickTitle())
        {
            m_nStrollFlag = TRUE;
            SetCapture();
            GetCursorPos(&m_pointPrev);
        }
    }
    if (pMsg->message == WM_MOUSEMOVE)
    {
        //if (pMsg->hwnd == m_hWnd)
            OnMouseMove();
    }
	if (pMsg->message == WM_MOUSEWHEEL)
	{

	}



    return CWnd::PreTranslateMessage(pMsg);
}

int KGLilisPanel::OnClickTitle()
{
    POINT point;
    GetCursorPos(&point);
    size_t s = 0;
    for (s = 0; s < m_vecChileWnd.size(); s++)
    {
        RECT rect;
        m_vecChileWnd[s].pTitleBar->GetWindowRect(&rect);
        rect.left += 5;
        if (PtInRect(&rect, point))
        {
            m_vecChileWnd[s].dwState = !m_vecChileWnd[s].dwState;
            m_vecChileWnd[s].pTitleBar->dwState = m_vecChileWnd[s].dwState;
            m_vecChileWnd[s].pTitleBar->InvalidateRect(NULL);
            break;
        }
    }
    if (s < m_vecChileWnd.size())
        UpdateUI();
    else
        return FALSE;

    InvalidateRect(NULL);
    return TRUE;
}

int KGLilisPanel::OnMouseMove()
{
    SetClassLongPtr(this->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)m_hCursorScroll);

    if (!m_nStrollFlag)
        return TRUE;

    POINT pointCurr;
    GetCursorPos(&pointCurr);

    if (pointCurr.y == m_pointPrev.y)
        return TRUE;

    int nOffset = pointCurr.y - m_pointPrev.y;
    CWnd* pWnd = GetParent();
    if (!pWnd)
        return FALSE;

    RECT rectClient;
    GetWindowRect(&rectClient);
    pWnd->ScreenToClient(&rectClient);
    rectClient.top += nOffset;
    if (rectClient.top <= 0)
        MoveWindow(&rectClient);
    m_pointPrev = pointCurr;

    return TRUE;
}

int KGLilisPanel::UpdateUI()
{
    RECT rectClient;
    GetClientRect(&rectClient);
    rectClient.left  += 10;
    rectClient.right -= 10;
    for (size_t s = 0; s < m_vecChileWnd.size(); s++)
    {
        RECT rectTitle;
        RECT rectPanel;
        RECT rectTemp1;
        RECT rectTemp2;

        // title
        rectTitle.left  = rectClient.left;
        rectTitle.right = rectClient.right;
        if (!s)
        {
            rectTitle.top = rectClient.top + m_nPanelOffset;
        }
        else
        {
            if (m_vecChileWnd[s - 1].dwState)
                m_vecChileWnd[s - 1].pWndPane->GetWindowRect(&rectTemp2);
            else
                m_vecChileWnd[s - 1].pTitleBar->GetWindowRect(&rectTemp2);
            ScreenToClient(&rectTemp2);
            rectTitle.top = rectTemp2.bottom + 10;
        }
        rectTitle.bottom = rectTitle.top + 20;
        m_vecChileWnd[s].pTitleBar->MoveWindow(&rectTitle);

        // panle
        m_vecChileWnd[s].pWndPane->GetWindowRect(&rectTemp1);
        int left =
            ((rectClient.right - rectClient.left) -
            (rectTemp1.right - rectTemp1.left)) / 2;
        rectPanel.left   = left + 10;
        rectPanel.right  = rectPanel.left + (rectTemp1.right - rectTemp1.left);
        rectPanel.top    = rectTitle.bottom;
        rectPanel.bottom = rectPanel.top + (rectTemp1.bottom - rectTemp1.top);
        m_vecChileWnd[s].pWndPane->MoveWindow(&rectPanel);
        DWORD dwType = 0;
        if (m_vecChileWnd[s].dwState)
            dwType = SW_SHOW;
        else
            dwType = SW_HIDE;
        m_vecChileWnd[s].pWndPane->ShowWindow(dwType);
    }
    return TRUE;
}

int KGLilisPanel::AddPanel(CWnd* pWnd, CString strName, DWORD dwState)
{
    ASSERT(pWnd);
    if (pWnd)
    {
        _LILISPANE lilisPane;
        RECT rect = {0, 0, 0, 0};
        lilisPane.pTitleBar = new KGLilisPanelTitle();
        lilisPane.pTitleBar->Create(
            NULL,
            NULL,
            WS_VISIBLE | WS_CHILD,
            rect,
            this,
            (UINT)m_vecChileWnd.size()
        );
        lilisPane.pTitleBar->SetOwner(this);
        lilisPane.pTitleBar->SetParent(this);
        SetClassLongPtr(lilisPane.pTitleBar->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)m_hCursorNormal);
        ASSERT(lilisPane.pTitleBar);
        lilisPane.pTitleBar->strName = strName;
        lilisPane.pWndPane = pWnd;
        lilisPane.strName  = strName;
        lilisPane.dwState  = dwState;
      /*  int a = ::SetClassLong(
            lilisPane.pWndPane->GetSafeHwnd(),
            GCL_HBRBACKGROUND,
            (DWORD)CreateSolidBrush(RGB(200, 200, 200))
        );*/
        lilisPane.pWndPane->SetOwner(this);
        lilisPane.pWndPane->SetParent(this);
        m_vecChileWnd.push_back(lilisPane);
        UpdateUI();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

HBRUSH KGLilisPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBursh = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
    if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX)
    {
        static HBRUSH hFocus = ::CreateSolidBrush(RGB(240, 200, 85));
        static HBRUSH hNorml = ::CreateSolidBrush(RGB(210, 210, 233));
        COLORREF cFocus = RGB(240, 200, 85);
        COLORREF cNorml = RGB(210, 210, 233);

        if (::GetFocus() == pWnd->m_hWnd)
        {
            hBursh = hFocus;
            pDC->SetBkColor(cFocus);
        }
        else
        {
            hBursh = hNorml;
            pDC->SetBkColor(cNorml);
        }
    }
    return hBursh;
}

BOOL KGLilisPanel::PreCreateWindow(CREATESTRUCT& cs)
{
    m_hCursorScroll = ::LoadCursor(NULL, IDC_SIZEALL);
    m_hCursorNormal = ::LoadCursor(NULL, IDC_ARROW);

    cs.lpszClass = AfxRegisterWndClass(
        CS_SAVEBITS,
        0,
        //CreateSolidBrush(RGB(197, 197, 197)),
        (HBRUSH)(COLOR_BTNFACE+1),
        0
    );
    return CWnd::PreCreateWindow(cs);
}

//-----------------------------------------------------------------------------
// LilisVirtualWnd define
//-----------------------------------------------------------------------------

inline BOOL equelrect(RECT r1, RECT r2)
{
    if (
        (r1.left    == r2.left)  &&
        (r1.top     == r2.top)   &&
        (r1.right   == r2.right) &&
        (r1.bottom  == r2.bottom)
        )
        return TRUE;
    else
        return FALSE;
}

KGLiliVirtualWnd::KGLiliVirtualWnd(CWnd* pDependWnd)
{
    ASSERT(pDependWnd);
    m_pDependWindow = pDependWnd;
    m_pPanleWindws  = NULL;
    
    m_rectPerv.left   = INT_MAX;
    m_rectPerv.right  = INT_MIN;
    m_rectPerv.top    = INT_MAX;
    m_rectPerv.bottom = INT_MIN;

}

KGLiliVirtualWnd::KGLiliVirtualWnd()
{
    m_rectPerv.left   = INT_MAX;
    m_rectPerv.right  = INT_MIN;
    m_rectPerv.top    = INT_MAX;
    m_rectPerv.bottom = INT_MIN;
}

KGLiliVirtualWnd::~KGLiliVirtualWnd()
{

}

void KGLiliVirtualWnd::GetWindowRect(LPRECT lpRect) const
{
    lpRect->left       = m_rectPerv.left;
    lpRect->top        = m_rectPerv.top;
    lpRect->right      = m_rectPerv.right;
    lpRect->bottom     = m_rectPerv.bottom;
}

int KGLiliVirtualWnd::ShowWindow(int nCmdShow)
{
    for (size_t s = 0; s < m_vecElements.size(); s++)
        m_vecElements[s]->ShowWindow(nCmdShow);
    return TRUE;
}

int KGLiliVirtualWnd::IsGetFocus()
{
    for (size_t s = 0; s < m_vecElements.size(); s++)
    {
        if (::GetFocus() == m_vecElements[s]->m_hWnd)
            return TRUE;
    }
    return FALSE;
}

int KGLiliVirtualWnd::DrawWindow()
{
    //m_pDependWindow->RedrawWindow();
    for (size_t s = 0; s < m_vecElements.size(); s++)
        m_vecElements[s]->Invalidate();
    return TRUE;
}

int KGLiliVirtualWnd::AjustFrame()
{    
    size_t size = m_vecElements.size();

    m_vecEleRects.clear();

    for (size_t i = 0; i < size; i++)
    {
        RECT rectElement;
        RATE rectEOffset;

        m_vecElements[i]->GetWindowRect(&rectElement);
        m_pDependWindow->ScreenToClient(&rectElement);

        float height = (float)m_rectPerv.bottom - m_rectPerv.top;
        float width  = (float)m_rectPerv.right  - m_rectPerv.left;

        rectEOffset.left   = (rectElement.left - m_rectPerv.left) / width;
        rectEOffset.right  = (m_rectPerv.right - rectElement.right) / width;
        rectEOffset.top    = (rectElement.top - m_rectPerv.top) / height;
        rectEOffset.bottom = (m_rectPerv.bottom - rectElement.bottom) / height;

        m_vecEleRects.push_back(rectEOffset);
    }

    
    return true;
}

int KGLiliVirtualWnd::AddElement(CWnd* pElement)
{
    ASSERT(pElement);
    if (pElement && m_pDependWindow)
    {
        m_vecElements.push_back(pElement);

        RECT rectElement;
        //RATE rectEOffset;
        pElement->GetWindowRect(&rectElement);
        m_pDependWindow->ScreenToClient(&rectElement);

      /*  float height = (float)m_rectPerv.bottom - m_rectPerv.top;
        float width  = (float)m_rectPerv.right  - m_rectPerv.left;

        rectEOffset.left   = (rectElement.left - m_rectPerv.left) / width;
        rectEOffset.right  = (m_rectPerv.right - rectElement.right) / width;
        rectEOffset.top    = (rectElement.top - m_rectPerv.top) / height;
        rectEOffset.bottom = (m_rectPerv.bottom - rectElement.bottom) / height;*/

        m_rectPerv.bottom = max(m_rectPerv.bottom, rectElement.bottom);
        m_rectPerv.top    = min(m_rectPerv.top, rectElement.top);
        m_rectPerv.left   = min(m_rectPerv.left, rectElement.left);
        m_rectPerv.right  = max(m_rectPerv.right, rectElement.right);

        /*m_vecEleRects.push_back(rectEOffset);*/
        return TRUE;
    }
    else
        return FALSE;
}

int KGLiliVirtualWnd::MoveWindow(LPCRECT lpRect)
{
    if (equelrect(*lpRect, m_rectPerv) && !m_pDependWindow)
        return TRUE;


    /*int lOffset = lpRect->left - m_rectPerv.left;
    int tOffset = lpRect->top  - m_rectPerv.top;
    int rOffset = lpRect->right - m_rectPerv.right;
    int bOffset = lpRect->bottom - m_rectPerv.bottom;

    int hSize   =
    (lpRect->right - lpRect->left) - (m_rectPerv.right - m_rectPerv.left);
    int vSize   =
    (lpRect->bottom - lpRect->top) - (m_rectPerv.bottom - m_rectPerv.top);
    */

    for (size_t s = 0; s < m_vecElements.size(); s++)
    {
        RECT rectWnd;
        int witdh      = lpRect->right - lpRect->left;
        int height     = lpRect->bottom - lpRect->top;
        rectWnd.left   = (LONG)(lpRect->left + witdh * m_vecEleRects[s].left);
        rectWnd.right  = (LONG)(lpRect->right - witdh * m_vecEleRects[s].right);
        rectWnd.top    = (LONG)(lpRect->top + height * m_vecEleRects[s].top);
        rectWnd.bottom = (LONG)(lpRect->bottom - height * m_vecEleRects[s].bottom);
        m_vecElements[s]->MoveWindow(&rectWnd);
    }

    m_rectPerv.left     = lpRect->left;
    m_rectPerv.top      = lpRect->top;
    m_rectPerv.right    = lpRect->right;
    m_rectPerv.bottom   = lpRect->bottom;

    return TRUE;
}

//-----------------------------------------------------------------------------
// LilisSheet Define
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(KGLilisPanelDLG, CDialog)

KGLilisPanelDLG::KGLilisPanelDLG(UINT uTemplateID, CWnd* pParent /* = NULL */)
: CDialog(uTemplateID, pParent)
{
    m_nStrollFlag = FALSE;
    m_bEnable     = TRUE;
    m_bEnableSelpWeek = FALSE;
    m_nTopOffset = 2;
    m_nWeekRate  = 50;
}

KGLilisPanelDLG::~KGLilisPanelDLG()
{
    for (DWORD i = 0; i < m_vecChileWnd.size(); i++)
        SAFE_DELETE(m_vecChileWnd[i].pTitleBar);
}

int KGLilisPanelDLG::Clear()
{
    for (DWORD i = 0; i < m_vecChileWnd.size(); i++)
        SAFE_DELETE(m_vecChileWnd[i].pTitleBar);
    m_vecChileWnd.clear();

    return true;
}

BEGIN_MESSAGE_MAP(KGLilisPanelDLG, CDialog)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL KGLilisPanelDLG::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (pWnd == this && m_hCursorScroll)
    {
        ::SetCursor(m_hCursorScroll);
        return TRUE;
    }
    return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

int KGLilisPanelDLG::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (!m_bEnable)
        return CDialog::OnMouseWheel(nFlags, zDelta, pt);

    if (zDelta > 0)
        zDelta =  (short)m_nWeekRate;
    else
        zDelta = (short)(-m_nWeekRate);

    size_t size = m_vecChileWnd.size();
    CWnd* pWnd = GetParent();
    if (!pWnd || !size)
        return TRUE;

    RECT rectClient;
    RECT rectDepend;

    pWnd->GetClientRect(&rectDepend);
    GetWindowRect(&rectClient);
    pWnd->ScreenToClient(&rectClient);
    rectClient.top    += zDelta;
    rectClient.bottom = rectDepend.bottom - 5;
    
    RECT rectBT;
    m_vecChileWnd[size - 1].vWndPane.GetWindowRect(&rectBT);
    int n = rectBT.bottom + rectClient.top;
    
    //if (n + 50 < rectDepend.bottom)

    if (rectClient.top > 0)
        rectClient.top = 0;

    if (rectClient.top < rectClient.bottom - rectBT.bottom - 10)
        rectClient.top = rectClient.bottom - rectBT.bottom - 10;

    MoveWindow(&rectClient);


    return TRUE;
}

int KGLilisPanelDLG::UpdateUI()
{
    if (!m_bEnable)
        return false;

    RECT rectClient;
    GetClientRect(&rectClient);
    rectClient.left  += 10;
    rectClient.right -= 10;
    for (size_t s = 0; s < m_vecChileWnd.size(); s++)
    {
        RECT rectTitle;
        RECT rectPanel;
        RECT rectTemp1;
        RECT rectTemp2;

        // title
        rectTitle.left  = rectClient.left;
        rectTitle.right = rectClient.right;
        if (!s)
        {
            rectTitle.top = rectClient.top;
        }
        else
        {
            if (m_vecChileWnd[s - 1].dwState)
            {
                m_vecChileWnd[s - 1].vWndPane.GetWindowRect(&rectTemp2);
            }
            else
            {
                m_vecChileWnd[s - 1].pTitleBar->GetWindowRect(&rectTemp2);
                ScreenToClient(&rectTemp2);
            }
            rectTitle.top = rectTemp2.bottom + 10;
        }
        rectTitle.bottom = rectTitle.top + 20;
        m_vecChileWnd[s].pTitleBar->MoveWindow(&rectTitle);

        // panle
        m_vecChileWnd[s].vWndPane.GetWindowRect(&rectTemp1);
        int left =
            ((rectClient.right - rectClient.left) -
            (rectTemp1.right - rectTemp1.left)) / 2;
        rectPanel.left   = left + 10;
        rectPanel.right  = rectPanel.left + (rectTemp1.right - rectTemp1.left);
        rectPanel.top    = rectTitle.bottom + m_nTopOffset;
        rectPanel.bottom = rectPanel.top + (rectTemp1.bottom - rectTemp1.top);
        m_vecChileWnd[s].vWndPane.MoveWindow(&rectPanel);
        DWORD dwType = 0;
        if (m_vecChileWnd[s].dwState)
            dwType = SW_SHOW;
        else
            dwType = SW_HIDE;
        m_vecChileWnd[s].vWndPane.ShowWindow(dwType);
    }
    return TRUE;
}

int KGLilisPanelDLG::AddPanel(KGLiliVirtualWnd vWnd, CString strName, DWORD dwState)
{
    if (!m_bEnable)
        return false;

    _LILISPANE lilisPane;
    RECT rect = {0, 0, 0, 0};
    lilisPane.pTitleBar = new KGLilisPanelTitle();
    lilisPane.pTitleBar->Create(
        NULL,
        NULL,
        WS_VISIBLE | WS_CHILD,
        rect,
        this,
        (UINT)m_vecChileWnd.size()
        );
    lilisPane.pTitleBar->SetOwner(this);
    lilisPane.pTitleBar->SetParent(this);
    SetClassLongPtr(lilisPane.pTitleBar->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)m_hCursorNormal);
    ASSERT(lilisPane.pTitleBar);
    lilisPane.pTitleBar->strName = strName;
    lilisPane.vWndPane = vWnd;
    lilisPane.strName  = strName;
    lilisPane.dwState  = dwState;
    lilisPane.vWndPane.m_pPanleWindws = this;
    
    lilisPane.vWndPane.AjustFrame();

    m_vecChileWnd.push_back(lilisPane);

    UpdateUI();

    return TRUE;
}

int KGLilisPanelDLG::OnClickTitle()
{
    if (!m_bEnable)
        return false;

    POINT point;
    GetCursorPos(&point);
    size_t s = 0;
    for (s = 0; s < m_vecChileWnd.size(); s++)
    {
        RECT rect;
        m_vecChileWnd[s].pTitleBar->GetWindowRect(&rect);
        rect.left += 5;
        if (PtInRect(&rect, point))
        {
            m_vecChileWnd[s].dwState = !m_vecChileWnd[s].dwState;
            m_vecChileWnd[s].pTitleBar->dwState = m_vecChileWnd[s].dwState;
            m_vecChileWnd[s].pTitleBar->InvalidateRect(NULL);
            break;
        }
    }
    if (s < m_vecChileWnd.size())
        UpdateUI();
    else
        return FALSE;

    InvalidateRect(NULL);
    return TRUE;
}

void KGLilisPanelDLG::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


int KGLilisPanelDLG::OnMouseMove()
{
    if (!m_bEnable)
        return false;

    //SetClassLongPtr(this->m_hWnd, GCL_HCURSOR, (LONG)(LONG_PTR)m_hCursorScroll);

    size_t size = m_vecChileWnd.size();
    if (!size)
        return TRUE;

    if (!m_nStrollFlag)
        return TRUE;

    POINT pointCurr;
    GetCursorPos(&pointCurr);

    if (pointCurr.y == m_pointPrev.y)
        return TRUE;

    int nOffset = pointCurr.y - m_pointPrev.y;
    CWnd* pWnd = GetParent();
    if (!pWnd)
        return FALSE;

    RECT rectClient;
    RECT rectDepend;
    pWnd->GetClientRect(&rectDepend);
    GetWindowRect(&rectClient);
    pWnd->ScreenToClient(&rectClient);
    rectClient.top += nOffset;

    RECT rectBT;
    m_vecChileWnd[size - 1].vWndPane.GetWindowRect(&rectBT);
    int n = rectBT.bottom + rectClient.top;

    if (rectClient.top > 0)
        rectClient.top = 0;

    if (rectClient.top < rectClient.bottom - rectBT.bottom - 10)
        rectClient.top = rectClient.bottom - rectBT.bottom - 10;

    MoveWindow(&rectClient);

    //if (rectClient.top <= 0 && n + 50 >= rectDepend.bottom)
    //    MoveWindow(&rectClient);
    m_pointPrev = pointCurr;
    //ValidateRect(NULL);
    return TRUE;
}

BOOL KGLilisPanelDLG::PreTranslateMessage(MSG* pMsg)
{
    if (!m_bEnable)
        return CDialog::PreTranslateMessage(pMsg);

    int nResult = CDialog::PreTranslateMessage(pMsg);
    if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_hWnd)
    {
        m_nStrollFlag = FALSE;
        ReleaseCapture();
    }
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        if (!OnClickTitle())
        {
            if (pMsg->hwnd == this->m_hWnd)
            {
                m_nStrollFlag = TRUE;
                SetCapture();
                GetCursorPos(&m_pointPrev);
            }
        }
    }
    if (pMsg->message == WM_MOUSEMOVE)
    {
        if (pMsg->hwnd == m_hWnd)
            OnMouseMove();
    }

    if (m_bEnableSelpWeek)
    {
        if (pMsg->message == WM_MOUSEWHEEL)
            OnMouseWheel(0, HIWORD(pMsg->wParam), 0);
    }

    return nResult;
}

BOOL KGLilisPanelDLG::OnInitDialog()
{
    int nResult = CDialog::OnInitDialog();
    
    if (m_bEnable)
    {
        m_hCursorScroll = ::LoadCursor(NULL, IDC_SIZEALL);
        m_hCursorNormal = ::LoadCursor(NULL, IDC_ARROW);
    }
    return nResult;
}

void KGLilisPanelDLG::OnPaint()
{
    if (!m_bEnable)
        return CDialog::OnPaint();

    CWnd::OnPaint();
    CClientDC dc(this);
    for (size_t s = 0; s < m_vecChileWnd.size(); s++)
    {
        if (m_vecChileWnd[s].dwState)
        {
            CPen pen1(PS_SOLID, 1, RGB(100, 100, 100));
            RECT rectTitle;
            RECT rectPanle;
            m_vecChileWnd[s].vWndPane.GetWindowRect(&rectPanle);
            m_vecChileWnd[s].pTitleBar->GetWindowRect(&rectTitle);
            //ScreenToClient(&rectPanle);
            ScreenToClient(&rectTitle);
            dc.SelectObject(&pen1);
            int   halfH = (rectTitle.bottom - rectTitle.top) / 2;
            int   halfV = (rectTitle.right - rectTitle.left) / 2 + 2;
            POINT lt;   // left top
            POINT rt;   // right top
            POINT rb;   // right buttom
            POINT lb;   // left buttom

            // left side
            lt.x = rectTitle.left - 5;
            lt.y = rectTitle.top + halfH;
            rt.x = rectTitle.left;
            rt.y = rectTitle.top + halfH;
            lb.x = lt.x;
            lb.y = rectPanle.bottom + 5;
            rb.x = rectTitle.left + halfV;
            rb.y = lb.y;
            dc.MoveTo(rt);
            dc.LineTo(lt);
            dc.LineTo(lb);
            dc.LineTo(rb);

            // right side
            lt.x = rectTitle.right;
            lt.y = rectTitle.top + halfH;
            rt.x = rectTitle.right + 5;
            rt.y = rectTitle.top + halfH;
            lb.x = rectTitle.right - halfV;
            lb.y = rectPanle.bottom + 5;
            rb.x = rectTitle.right + 5;
            rb.y = lb.y;
            dc.MoveTo(lt);
            dc.LineTo(rt);
            dc.LineTo(rb);
            dc.LineTo(lb);

        }
        else
        {
            CPen pen1(PS_SOLID, 1, RGB(100, 100, 100));
            RECT rectTitle;
            m_vecChileWnd[s].pTitleBar->GetWindowRect(&rectTitle);
            ScreenToClient(&rectTitle);
            dc.SelectObject(&pen1);
            POINT lt;   // left top
            POINT rt;   // right top
            POINT rb;   // right buttom
            POINT lb;   // left buttom

            // left side
            lt.x = rectTitle.left - 5;
            lt.y = rectTitle.top + 5;
            rt.x = rectTitle.left;
            rt.y = rectTitle.top + 5;
            lb.x = lt.x;
            lb.y = rectTitle.bottom - 5;
            rb.x = rectTitle.left;
            rb.y = lb.y;
            dc.MoveTo(rt);
            dc.LineTo(lt);
            dc.LineTo(lb);
            dc.LineTo(rb);

            // right sidt
            lt.x = rectTitle.right;
            lt.y = rectTitle.top + 5;
            rt.x = rectTitle.right + 5;
            rt.y = rectTitle.top + 5;
            lb.x = lt.x;
            lb.y = rectTitle.bottom - 5;
            rb.x = rectTitle.right + 5;
            rb.y = lb.y;
            dc.MoveTo(lt);
            dc.LineTo(rt);
            dc.LineTo(rb);
            dc.LineTo(lb);
        }
    }
}

HBRUSH KGLilisPanelDLG::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (!m_bEnable)
        return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);


    HBRUSH hBursh = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    if (nCtlColor == CTLCOLOR_EDIT/* || nCtlColor == CTLCOLOR_LISTBOX*/)
    {
        static HBRUSH hFocus = ::CreateSolidBrush(RGB(240, 200, 85));
        static HBRUSH hNorml = ::CreateSolidBrush(RGB(210, 210, 233));
        COLORREF cFocus = RGB(240, 200, 85);
        COLORREF cNorml = RGB(210, 210, 233);

        if (::GetFocus() == pWnd->m_hWnd)
        {
            hBursh = hFocus;
            pDC->SetBkColor(cFocus);
        }
        else
        {
            hBursh = hNorml;
            pDC->SetBkColor(cNorml);
        }
    }
    return hBursh;

}
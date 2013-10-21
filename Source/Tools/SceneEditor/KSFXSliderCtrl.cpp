
#include "stdafx.h"
#include "KSFXSliderCtrl.h"
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KSFXSliderCtrl, CStatic)

BEGIN_MESSAGE_MAP(KSFXSliderCtrl, CStatic)
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

#define SFX_SLIDER_BUTTON   0

int KSFXSliderCtrl::Init()
{
    int nResult  = false;
    int nRetCode = false;

    VERIFY(m_font.CreateFont(
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
        "Arial"));                 // lpszFacename

    nResult = true;
//Exit0:
    return nResult;
}

int KSFXSliderCtrl::SetTic(int nTic)
{
    if (nTic < m_nMinPos || nTic > m_nMaxPos)
        return 0;

    vector<int>::iterator i = find(m_listTic.begin(), m_listTic.end(), nTic);
    if (i != m_listTic.end())
        return 0;

    m_listTic.push_back(nTic);
    sort(m_listTic.begin(), m_listTic.end());

    Invalidate();
    UpdateWindow();
    return 1;
}

int KSFXSliderCtrl::SetRange(int nMin, int nMax)
{
    if (nMax < nMin)
        return 0;

    m_nMinPos = nMin;
    m_nMaxPos = nMax;
    if (m_nCurPos < nMin)   m_nCurPos = nMin;
    if (m_nCurPos > nMax)  m_nCurPos = nMax;

    Invalidate();
    UpdateWindow();

    return 1;
}

int KSFXSliderCtrl::SetPos(int nPos)
{
    if (nPos >= m_nMinPos && nPos <= m_nMaxPos)
    {
        m_nCurPos = nPos;
        Invalidate();
        UpdateWindow();

        return 1;
    }
    else
        return 0;
}

int KSFXSliderCtrl::ClearTics()
{
    m_listTic.clear();
    return 1;
}

int KSFXSliderCtrl::GetCurPos()
{
    return m_nCurPos;
}

int KSFXSliderCtrl::GetRange(int &nMin, int &nMax)
{
    nMin = m_nMinPos;
    nMax = m_nMaxPos;
    return 1;
}

KSFXSliderCtrl::KSFXSliderCtrl(void)
{
    m_nMinPos = 0;
    m_nMaxPos = 0;
    m_nCurPos = 0;
    m_nPage = 5;

    m_fStep = 0;
    m_bShowTic = TRUE;
    m_SliderOpState = SOS_NONE;

    m_nStartPos = 0;
}

KSFXSliderCtrl::~KSFXSliderCtrl(void)
{
}

int KSFXSliderCtrl::GetSliderRect(RECT *pRc)
{
    *pRc = m_rcSlider;
    return 1;
}

void KSFXSliderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    if (!pDC)
        return;
    RECT rectClient;
    RECT rc;
    GetClientRect(&rectClient);
    rc = rectClient;

    CBitmap  memBitmap;
    CBitmap* pOldBmp = NULL;
    CDC      memDC;

    memDC.CreateCompatibleDC(pDC);
    memBitmap.CreateCompatibleBitmap(pDC, rectClient.right, rectClient.bottom);
    pOldBmp = memDC.SelectObject(&memBitmap);
    memDC.FillSolidRect(&rectClient, RGB(197, 197, 197));
    memDC.SelectObject(&m_font);

    int width   = rc.right  - rc.left;
    int height  = rc.bottom - rc.top;

    // channel
    m_rcChannel.left    = rc.left + 70;
    m_rcChannel.right   = rc.right - 70;
    m_rcChannel.top     = rc.top;
    m_rcChannel.bottom  = m_rcChannel.top + 20;
    RECT rectChannel  = m_rcChannel;
    rectChannel.left  = rectClient.left;
    rectChannel.right = rectClient.right;
    memDC.FillSolidRect(&rectChannel, RGB(207, 207, 207));//GetSysColor(COLOR_ACTIVEBORDER));


    // line
    CPen pen(PS_SOLID, 1, RGB(100, 100, 100));
    memDC.SelectObject(&pen);
    memDC.MoveTo(rectClient.left,  m_rcChannel.bottom);
    memDC.LineTo(rectClient.right, m_rcChannel.bottom);



    // rect
    memDC.MoveTo(rectClient.left,  rectClient.top);
    memDC.LineTo(rectClient.left,  rectClient.bottom - 1);
    memDC.LineTo(rectClient.right - 1, rectClient.bottom - 1);
    memDC.LineTo(rectClient.right - 1, rectClient.top);
    //DrawFrame(&memDC, rectClient, RGB(100, 100, 100), TRUE);

    //RECT rectLine;
    //rectLine.left   = rectClient.left;
    //rectLine.right  = rectClient.right;
    //rectLine.top    = m_rcChannel.bottom - 1;
    //rectLine.bottom = m_rcChannel.bottom;
    //memDC.DrawFrameControl(&rectLine, DFC_BUTTON, DFCS_FLAT);
    // slider
    int nCnt = m_nMaxPos - m_nMinPos;
    ASSERT(nCnt >= 0);
    if (nCnt)
        m_fStep = (float)(m_rcChannel.right - m_rcChannel.left) / nCnt;
    else
        m_fStep = 0;

    // point
    RECT rectPoint;
    rectPoint.left   = (LONG)(m_rcChannel.left + m_nCurPos * m_fStep - 3);
    rectPoint.right  = (LONG)(m_rcChannel.left + m_nCurPos * m_fStep + 4);
    rectPoint.top    = (LONG)(m_rcChannel.bottom);
    rectPoint.bottom = (LONG)(m_rcChannel.bottom + 25);
    memDC.FillRect(&rectPoint, &CBrush(RGB(142, 173, 195)));
    DrawFrame(&memDC, rectPoint, RGB(100, 100, 100), TRUE);

    vector<int>::iterator i = m_listTic.begin();
    while (i != m_listTic.end())
    {
        int n = *i;
        int m = (int)(m_rcChannel.left + n * m_fStep);
        RECT rect;
        rect.left = m - 3;
        rect.top  = m_rcChannel.bottom;
        rect.right = m + 3;
        rect.bottom = rect.top + 12;
        memDC.FillRect(&rect, &CBrush(RGB(2, 2, 255)));
        DrawFrame(&memDC, rect, RGB(100, 100, 100), TRUE);
        //pDC->MoveTo((int)(m_rcChannel.left + n * m_fStep), m_rcChannel.bottom);
        //pDC->LineTo((int)(m_rcChannel.left + n * m_fStep), m_rcChannel.bottom + 10);
        ++i;
    }

    i = m_listSelectTic.begin();
    while (i != m_listSelectTic.end())
    {
        /*rc = m_rcChannel;
        rc.left = (int)(m_rcChannel.left + (*i) * m_fStep - 2);
        rc.right = (int)(m_rcChannel.left + (*i) * m_fStep + 2);
        pDC->FillSolidRect(&rc, RGB(0,0,255));*/
        int n = *i;
        int m = (int)(m_rcChannel.left + n * m_fStep);
        RECT rect;
        rect.left = m - 3;
        rect.top  = m_rcChannel.bottom;
        rect.right = m + 3;
        rect.bottom = rect.top + 12;
        memDC.FillRect(&rect, &CBrush(RGB(255, 2, 2)));
        DrawFrame(&memDC, rect, RGB(100, 100, 100), TRUE);
        ++i;
    }

    i = m_listMovingTic.begin();
    while (i != m_listMovingTic.end())
    {
        //rc = m_rcChannel;
        //rc.left = (int)(m_rcChannel.left + (*i) * m_fStep - 2);
        //rc.right = (int)(m_rcChannel.left + (*i) * m_fStep + 2);
        int n = *i;
        int m = (int)(m_rcChannel.left + n * m_fStep);
        RECT rect;
        rect.left = m - 3;
        rect.top  = m_rcChannel.bottom;
        rect.right = m + 3;
        rect.bottom = rect.top + 12;
        if (m_SliderOpState == SOS_COPY_SELECTED_TIC)
            memDC.FillRect(&rect, &CBrush(RGB(255, 100, 100)));
        //pDC->FillSolidRect(&rect, RGB(128,128,128));
        else if (m_SliderOpState == SOS_MOVE_SELECTED_TIC)
            memDC.FillRect(&rect, &CBrush(RGB(100, 100, 255)));
        //pDC->Draw3dRect(&rect, RGB(128,128,128), RGB(128,128,128));

        ++i;
    }

    m_rcSlider.left   = (long)m_rcChannel.left + (long)(m_nCurPos * m_fStep - 70);
    m_rcSlider.right  = (long)m_rcChannel.left + (long)(m_nCurPos * m_fStep + 70);
    m_rcSlider.top    = (long)m_rcChannel.top;
    m_rcSlider.bottom = (long)m_rcChannel.bottom;

    rc = m_rcSlider;
    rc.left = m_rcChannel.left;
    rc.right = m_rcChannel.right;
    rc.top += 8;
    rc.bottom -= 8;
    //pDC->Draw3dRect(&rc, RGB(40,40,40), RGB(255,255,255));

    for (int i = 0; i <= nCnt; ++i)
    {
        int n = (int)(m_rcChannel.left + i * m_fStep);
        int lenght = 0;
        if (i % 10)
        {
            lenght = 8;
        }
        else
        {
            lenght = 18;
            RECT rectText;
            rectText.left = n - 100;
            rectText.top  = m_rcChannel.bottom + 10;
            rectText.right = n + 100;
            rectText.bottom = m_rcChannel.bottom + 27;
            memDC.SetBkMode(TRANSPARENT);
            TCHAR szInfot[128];
            sprintf(szInfot, "%d", i);
            memDC.DrawText(szInfot, &rectText, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }
        CPen pen(PS_SOLID, 1, RGB(100, 100, 100));
        memDC.SelectObject(&pen);
        memDC.MoveTo(n, m_rcChannel.bottom);
        memDC.LineTo(n, m_rcChannel.bottom + lenght);


    }


    // slider
    memDC.DrawFrameControl(&m_rcSlider, DFC_BUTTON, DFCS_BUTTONPUSH);
    memDC.SetBkMode(TRANSPARENT);
    TCHAR szInfot[128];
    sprintf(szInfot, "%d / %d", m_nCurPos, m_nMaxPos);
    memDC.DrawText(szInfot, &m_rcSlider, DT_SINGLELINE | DT_CENTER | DT_VCENTER);



    rc.top    = m_rcChannel.bottom;
    rc.bottom = m_rcChannel.bottom + 25;
    rc.left -= 10;
    rc.right += 10;
    if (m_SliderOpState == SOS_SELECT_TIC)
    {
        rc.left = (m_StartPos.x < m_EndPos.x) ? m_StartPos.x : m_EndPos.x;
        rc.right = (m_StartPos.x < m_EndPos.x) ? m_EndPos.x : m_StartPos.x;
        DrawFrame(&memDC, rc, RGB(80, 80, 80));
    }

    pDC->BitBlt(
        rectClient.left,
        rectClient.top,
        rectClient.right,
        rectClient.bottom,
        &memDC,
        0,
        0,
        SRCCOPY
    );

    memDC.SelectObject(pOldBmp);
    memDC.DeleteDC();
    memBitmap.DeleteObject();
}

void KSFXSliderCtrl::PreSubclassWindow()
{
    CStatic::PreSubclassWindow();
    ModifyStyle(0, SS_OWNERDRAW);
}

BOOL KSFXSliderCtrl::OnEraseBkgnd(CDC* pDC)
{
    //BOOL retValue= CStatic::OnEraseBkgnd(pDC);

    //CRect rc;
    //GetClientRect(&rc);
    //pDC->FillSolidRect(&rc, GetSysColor(COLOR_ACTIVEBORDER));

    return TRUE;//retValue;

}

BOOL KSFXSliderCtrl::IsTic(int nPos)
{
    vector<int>::iterator i = m_listTic.begin();
    while (i != m_listTic.end())
    {
        if (nPos == (*i))
            return TRUE;

        ++i;
    }

    return FALSE;
}

int KSFXSliderCtrl::DrawFrame(CDC* pDC, RECT rect, COLORREF color, int nFlag)
{
    DWORD dwPS = PS_DOT;
    if (nFlag)
        dwPS = PS_SOLID;
    CPen pen(dwPS, 1, color);
    pDC->SelectObject(&pen);
    pDC->MoveTo(rect.left, rect.top);
    pDC->LineTo(rect.right, rect.top);
    pDC->LineTo(rect.right, rect.bottom);
    pDC->LineTo(rect.left, rect.bottom);
    pDC->LineTo(rect.left, rect.top);
    return TRUE;
}

void KSFXSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    SetFocus();
    SetCapture();

    m_listMovingTic.clear();
    if (PtInRect(&m_rcSlider, point))
    {
        if (nFlags & MK_CONTROL)
        {
            m_SliderOpState = SOS_MOVE_SELECTED_TIC;
            m_nStartPos = m_nCurPos;
        }
        else if (nFlags & MK_SHIFT)
        {
            m_SliderOpState = SOS_COPY_SELECTED_TIC;
            m_nStartPos = m_nCurPos;
        }
        else
            m_SliderOpState = SOS_DRAG_SLIDER;
    }
    else if (point.y >= m_rcChannel.bottom - 50 && point.y <= m_rcChannel.bottom + 50)
    {
        m_SliderOpState = SOS_SELECT_TIC;
        m_StartPos = point;
        float left = (float)(m_rcChannel.left - 10);
        float right = (float)(m_rcChannel.right + 10);
        if (m_StartPos.x < left)    m_StartPos.x = (long)left;
        if (m_StartPos.x > right)   m_StartPos.x = (long)right;
        m_listSelectTic.clear();
    }
    else
        m_SliderOpState = SOS_NONE;

    CStatic::OnLButtonDown(nFlags, point);
}

void KSFXSliderCtrl::UpdateSelectedTic()
{
    m_listSelectTic.clear();
    vector<int>::iterator i = m_listTic.begin();
    while (i != m_listTic.end())
    {
        int x = m_rcChannel.left + (int)((*i) * m_fStep);
        int left = (m_StartPos.x > m_EndPos.x) ? m_EndPos.x : m_StartPos.x;
        int right = (m_StartPos.x > m_EndPos.x) ? m_StartPos.x : m_EndPos.x;
        if (x >= left && x <= right)
            m_listSelectTic.push_back(*i);
        ++i;
    }
}

void KSFXSliderCtrl::UpdateMovingTic(int nOffset)
{
    m_listMovingTic.clear();
    vector<int>::iterator i = m_listSelectTic.begin();
    while (i != m_listSelectTic.end())
    {
        m_listMovingTic.push_back((*i) + nOffset);
        assert((*i) + nOffset <= m_nMaxPos);
        assert((*i) + nOffset >= m_nMinPos);
        ++i;
    }
}

void KSFXSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    SetFocus();
    ReleaseCapture();

    switch (m_SliderOpState)
    {
    case SOS_NONE:
        {
            RECT rc = m_rcChannel;
            rc.top = m_rcSlider.top;
            rc.bottom = m_rcSlider.bottom;
            if (PtInRect(&rc, point))
            {
                if (point.x < m_rcSlider.left)
                    m_nCurPos -= m_nPage;
                else if (point.x > m_rcSlider.right)
                    m_nCurPos += m_nPage;
                else
                    return;

                if (m_nCurPos < m_nMinPos)
                    m_nCurPos = m_nMinPos;
                if (m_nCurPos > m_nMaxPos)
                    m_nCurPos = m_nMaxPos;

            }
        }
        break;
    case SOS_DRAG_SLIDER:
        {
            m_SliderOpState = SOS_NONE;
            m_listMovingTic.clear();
        }
        break;
    case SOS_COPY_SELECTED_TIC:
    case SOS_MOVE_SELECTED_TIC:
        {
            ASSERT(m_listMovingTic.size() == m_listSelectTic.size());
            if (m_listMovingTic.size())
            {
                SLIDER_OPERATE_TICS t;
                t.nCnt = (int)m_listMovingTic.size();
                t.pTicsList = &m_listSelectTic[0];
                if (m_SliderOpState == SOS_MOVE_SELECTED_TIC)
                    t.nOperate = SFX_SLIDER_TICS_MOVE;
                else
                    t.nOperate = SFX_SLIDER_TICS_COPY;
                t.nOffset = m_listMovingTic[0] - m_listSelectTic[0];
                WPARAM wParam = (WPARAM)&t;
                GetParent()->SendMessage(WM_SFX_SLIDER_OPERATE_TICS, wParam, 0);
            }

            m_listMovingTic.clear();
            m_listSelectTic.clear();
            m_SliderOpState = SOS_NONE;
        }
        break;
    case SOS_SELECT_TIC:
        {
            m_SliderOpState = SOS_NONE;
        }
        break;
    default:
        return;
    }

    Invalidate();
    UpdateWindow();

    WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_nCurPos);
    LPARAM lParam = (LPARAM)this;
    GetParent()->SendMessage(WM_VSCROLL, wParam, lParam);

    CStatic::OnLButtonUp(nFlags, point);
}

void KSFXSliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    //SetFocus();

    if (!m_fStep)
        return;

    int n = 0;
    BOOL bUpdate = FALSE;
    switch (m_SliderOpState)
    {
    case SOS_NONE:
        return;
    case SOS_DRAG_SLIDER:
    case SOS_COPY_SELECTED_TIC:
    case SOS_MOVE_SELECTED_TIC:
        {
            n = point.x - m_rcChannel.left;
            m_nCurPos = (int)((n + m_fStep * 0.5) / m_fStep);

            if (nFlags & MK_CONTROL)
                m_SliderOpState = SOS_MOVE_SELECTED_TIC;
            else if (nFlags & MK_SHIFT)
                m_SliderOpState = SOS_COPY_SELECTED_TIC;
            else
                m_SliderOpState = SOS_DRAG_SLIDER;

            switch (m_SliderOpState)
            {
            case SOS_DRAG_SLIDER:
                {
                    m_listMovingTic.clear();
                    if (m_nCurPos < m_nMinPos)
                        m_nCurPos = m_nMinPos;
                    if (m_nCurPos > m_nMaxPos)
                        m_nCurPos = m_nMaxPos;
                }
                break;
            case SOS_MOVE_SELECTED_TIC:
                {
                    if (m_listSelectTic.size())
                    {
                        int min = m_nMinPos - (*m_listSelectTic.begin());
                        int max = m_nMaxPos - m_listSelectTic.back();
                        int offset = m_nCurPos - m_nStartPos;
                        if (offset > max)   offset = max;
                        if (offset < min)   offset = min;
                        m_nCurPos = m_nStartPos + offset;
                        if (m_nCurPos > m_nMaxPos)
                        {
                            offset -= (m_nCurPos - m_nMaxPos);
                            m_nCurPos = m_nMaxPos;
                        }
                        if (m_nCurPos < m_nMinPos)
                        {
                            offset -= (m_nCurPos - m_nMinPos);
                            m_nCurPos = m_nMinPos;
                        }
                        UpdateMovingTic(offset);
                    }
                }
                break;
            case SOS_COPY_SELECTED_TIC:
                {
                    if (m_listSelectTic.size())
                    {
                        int min = m_nMinPos - (*m_listSelectTic.begin());
                        int max = m_nMaxPos - m_listSelectTic.back();
                        int offset = m_nCurPos - m_nStartPos;
                        if (offset > max)   offset = max;
                        if (offset < min)   offset = min;
                        m_nCurPos = m_nStartPos + offset;
                        if (m_nCurPos > m_nMaxPos)
                        {
                            offset -= (m_nCurPos - m_nMaxPos);
                            m_nCurPos = m_nMaxPos;
                        }
                        if (m_nCurPos < m_nMinPos)
                        {
                            offset -= (m_nCurPos - m_nMinPos);
                            m_nCurPos = m_nMinPos;
                        }
                        UpdateMovingTic(offset);
                    }
                }
                break;
            default:
                ASSERT(0);
            }

            WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_nCurPos);
            LPARAM lParam = (LPARAM)this;
            GetParent()->SendMessage(WM_VSCROLL, wParam, lParam);
        }

        break;
    case SOS_SELECT_TIC:
        {
            m_EndPos = point;
            float left = (float)(m_rcChannel.left - 10);
            float right = (float)(m_rcChannel.right + 10);
            if (m_EndPos.x < left)    m_EndPos.x = (long)left;
            if (m_EndPos.x > right)   m_EndPos.x = (long)right;
            UpdateSelectedTic();
        }
        break;
    default:
        return;
    }

    Invalidate();
    UpdateWindow();

    CStatic::OnMouseMove(nFlags, point);
}

void KSFXSliderCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default
    if (nChar == VK_DELETE)
    {
        if (m_listSelectTic.size())
        {
            SLIDER_OPERATE_TICS t;
            t.nCnt = (int)m_listSelectTic.size();
            t.pTicsList = &m_listSelectTic[0];
            t.nOperate = SFX_SLIDER_TICS_DEL;
            t.nOffset = 0;
            WPARAM wParam = (WPARAM)&t;
            GetParent()->SendMessage(WM_SFX_SLIDER_OPERATE_TICS, wParam, 0);
            m_listMovingTic.clear();
            m_listSelectTic.clear();

            Invalidate();
            UpdateWindow();
        }
    }

    CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}

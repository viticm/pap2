// KCurveCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KCurveCtrl.h"
#include "SceneEditorCommon.h"

#pragma warning(disable : 4996)

IMPLEMENT_DYNAMIC(KCurveCtrl, CWnd)

KCurveCtrl::KCurveCtrl()
    : m_State(Normal),
      m_SelIndex(-1),
      m_SelSpline(-1)
{
    m_pCellPen = new CPen(PS_DOT, 1, RGB(125, 125, 125));
    m_pBkBrush = new CBrush(GetSysColor(COLOR_BTNFACE));
}

KCurveCtrl::~KCurveCtrl()
{
    for (size_t i = 0; i < m_vpSplinePen.size(); ++i)
        delete m_vpSplinePen[i];
    delete m_pCellPen;
    delete m_pBkBrush;
}

BEGIN_MESSAGE_MAP(KCurveCtrl, CWnd)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void KCurveCtrl::SetSplineNum(int num)
{
    if (num < 0)
        return;
    
    while ((int)m_vpSplinePen.size() > num)
    {
        delete m_vpSplinePen.back();
        m_vpSplinePen.pop_back();
    }

    while ((int)m_vvPoints.size() > num)
        m_vvPoints.pop_back();

    while ((int)m_vShowFlag.size() > num)
        m_vShowFlag.pop_back();

    while ((int)m_vpSplinePen.size() < num)
        m_vpSplinePen.push_back(new CPen(PS_SOLID, 1, RGB(rand() % 255, rand() % 255, rand() % 255)));

    while ((int)m_vvPoints.size() < num)
    {
        vector<D3DXVECTOR2> vPoints;
        vPoints.push_back(D3DXVECTOR2(0.0f, 0.0f));
        vPoints.push_back(D3DXVECTOR2(1.0f, 1.0f));
        m_vvPoints.push_back(vPoints);
    }

    while ((int)m_vShowFlag.size() < num)
        m_vShowFlag.push_back(true);
}


namespace {

    struct _Compare
    {
        bool operator()(const D3DXVECTOR2& left, const D3DXVECTOR2& rigt)
        {
            if (left.x < rigt.x)
                return true;

            if (left.x == rigt.x)
                return left.y < rigt.y;

            return false;
        }
    };
}

void KCurveCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    POINT pt = {point.x, point.y};
    RECT rcClient;

    GetClientRect(&rcClient);

    rcClient.left += Client_Offset;
    rcClient.top += Client_Offset;
    rcClient.right -= Client_Offset;
    rcClient.bottom -= Client_Offset;

    if (m_State != Move_Point)
    {
        D3DXVECTOR2 ptc = Client2Canvas(pt);
        
        if (PtOnSpline(ptc.x, ptc.y, 10.f / (float)rcClient.right))
            m_State = On_Spline;
        else
            m_State = Normal;
    }
    else if (m_State == Move_Point)
    {
        ASSERT(m_SelSpline >= 0 && m_SelSpline < (int)m_vvPoints.size());

        vector<D3DXVECTOR2>& vPoints = m_vvPoints[m_SelSpline];

        if (m_SelIndex >= 0 && m_SelIndex < (int)vPoints.size())
        {
            D3DXVECTOR2 ptc = Client2Canvas(pt);

            if ((!(m_SelIndex > 0 && ptc.x == vPoints[m_SelIndex - 1].x)) &&
               (!(m_SelIndex + 1 < (int)vPoints.size() && ptc.x == vPoints[m_SelIndex + 1].x)))
            {
                if ((m_SelIndex == 0 && ptc.x != 0.0) || (m_SelIndex == (int)vPoints.size() - 1) && ptc.x != 1.f)
                    m_SelIndex = AddPoint(m_SelSpline, ptc);
                else
                {
                    vPoints[m_SelIndex] = ptc;
                
                    while (m_SelIndex + 1 < (int)vPoints.size() && vPoints[m_SelIndex].x > vPoints[m_SelIndex + 1].x)
                    {
                        swap(vPoints[m_SelIndex], vPoints[m_SelIndex + 1]);
                        ++m_SelIndex;
                    }

                    while (m_SelIndex > 0 && vPoints[m_SelIndex].x < vPoints[m_SelIndex - 1].x)
                    {
                        swap(vPoints[m_SelIndex], vPoints[m_SelIndex - 1]);
                        --m_SelIndex;
                    }
                }

                InvalidateRect(NULL);
            }
            else
            {
                if (ptc.x == vPoints.front().x)
                {
                    DelPoint(m_SelSpline, m_SelIndex);
                    m_SelIndex = 0;
                }

                if (ptc.x == vPoints.back().x)
                {
                    DelPoint(m_SelSpline, m_SelIndex);
                    m_SelIndex = (int)vPoints.size() - 1;
                }
            }

            if (GetParent())
                GetParent()->PostMessage(WM_CURVERCTRL_SPLINE_CHANGED, (WPARAM)m_SelSpline);

        }
    }
    else
    {

    }

    return CWnd::OnMouseMove(nFlags, point);
}

BOOL KCurveCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (pWnd == this && nHitTest == HTCLIENT) 
    {
        switch (m_State)
        {
        case Normal :
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            break;
        case On_Spline :
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            break;
        case Move_Point :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE));
            break;
        default :
            break;
        }
        return TRUE;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void KCurveCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    POINT pt = {point.x, point.y};

    switch (m_State)
    {
    case Move_Point :
        break;
    case On_Spline :
        {
            ASSERT(m_SelSpline >= 0 && m_SelSpline < (int)m_vvPoints.size());

            m_SelIndex = -1;

            for (size_t s = 0; s < m_vvPoints.size(); ++s)
            {
                vector<D3DXVECTOR2>& vPoints = m_vvPoints[m_SelSpline];

                for (size_t i = 0; i < vPoints.size(); ++i)
                {
                    POINT ptc = Canvas2Cleint(vPoints[i]);
                    RECT rc = {ptc.x - Point_Size - 5, ptc.y - Point_Size - 5, ptc.x + Point_Size + 5, ptc.y + Point_Size + 5};

                    if (PtInRect(&rc, pt))
                    {
                        m_SelIndex = static_cast<int>(i);
                        m_State = Move_Point;
                        SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE));
                        SetCapture();
                    }
                }
            }

            if (m_State == On_Spline)
            {
                m_SelIndex = AddPoint(m_SelSpline, pt);
                m_State = Move_Point;
                SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE));
                SetCapture();
            }

        }
        break;
    default :
        break;
    }

    return CWnd::OnLButtonDown(nFlags, point);
}

void KCurveCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    POINT pt = {point.x, point.y};
    RECT rcClient;

    GetClientRect(&rcClient);

    rcClient.left += Client_Offset;
    rcClient.top += Client_Offset;
    rcClient.right -= Client_Offset;
    rcClient.bottom -= Client_Offset;

    switch (m_State)
    {
    case Move_Point :
        {
            ASSERT(m_SelSpline >= 0 && m_SelSpline < (int)m_vvPoints.size());

            RECT rcBound = rcClient;

            rcBound.left -= 50;
            rcBound.top -= 50;
            rcBound.right += 50;
            rcBound.bottom += 50;

            if (!PtInRect(&rcBound, pt))
                DelPoint(m_SelSpline, m_SelIndex);

            ReleaseCapture();
        }
        break;
    case On_Spline :
        break;
    default :
        break;
    }

    m_SelIndex = -1;
    m_State = Normal;

    return CWnd::OnLButtonUp(nFlags, point);
}

void KCurveCtrl::GetSplinePoint(int index, double pt[], int n, bool clip /* = true */)
{
    if (n <= 0 || index < 0 || index >= (int)m_vvPoints.size())
        return;

    vector<D3DXVECTOR2>& vPoints = m_vvPoints[index];

    double step = 1.0 / (double)n;
    int i = 0;
    int size = (int)vPoints.size();
    
    double* x = new double[size];
    double* y = new double[size];
    double s[5];

    for (int i = 0; i < size; ++i)
    {
        x[i] = vPoints[i].x;
        y[i] = vPoints[i].y;
    }

    if (clip)
    {
        for (double curr = 0.0; curr <= 1.0 && i < n - 1; curr += step)
            pt[i++] = max(0.0, min(1.0, g_pGraphicsTool->CubicSplineIntl(x, y, size, -1, curr, s)));
    }
    else
    {
        for (double curr = 0.0; curr <= 1.0 && i < n - 1; curr += step)
            pt[i++] = g_pGraphicsTool->CubicSplineIntl(x, y, size, -1, curr, s);
    }

    pt[i++] = g_pGraphicsTool->CubicSplineIntl(x, y, size, -1, 1.0, s);

    delete[] x;
    delete[] y;
}

double KCurveCtrl::GetSplinePoint(int index, double x, bool clip /* = true */)
{
    if (index < 0 || index >= (int)m_vvPoints.size())
        return 0.0;

    vector<D3DXVECTOR2>& vPoints = m_vvPoints[index];

    int i = 0;
    int size = (int)vPoints.size();

    double* xs = new double[size];
    double* ys = new double[size];
    double s[5];

    for (int i = 0; i < size; ++i)
    {
        xs[i] = vPoints[i].x;
        ys[i] = vPoints[i].y;
    }

    g_pGraphicsTool->CubicSplineIntl(xs, ys, size, -1, x, s);

    delete[] xs;
    delete[] ys;

    if (clip)
    {
        if (s[4] > 1.0)
            return 1.0;
        if (s[4] < 0.0)
            return 0.0;
    }

    return s[4];
}

D3DXVECTOR2* KCurveCtrl::GetControlPoints(int index, int& num, bool& bNeedDel)
{
    if (index < 0 || index >= (int)m_vvPoints.size())
        return NULL;

    static D3DXVECTOR2  sPts[32];
    D3DXVECTOR2* pPts = sPts;
    
    bNeedDel = false;
    num = (int)m_vvPoints[index].size();

    if (num > 32)
    {
        pPts = new D3DXVECTOR2[num];
        bNeedDel = true ;
    }

    copy(m_vvPoints[index].begin(), m_vvPoints[index].end(), pPts);

    return pPts;

}

namespace {

    struct _CompareFind
    {
        bool operator() (const D3DXVECTOR2& pt)
        {
            return fabs(pt.x - ptCompare.x) < FLT_EPSILON;
        }

        _CompareFind(const D3DXVECTOR2& pt)
            : ptCompare(pt) 
        {
        }

        D3DXVECTOR2 ptCompare;
    };
}

int KCurveCtrl::AddPoint(int index, const POINT& pt)
{
    if (index < 0 || index >= (int)m_vvPoints.size())
        return -1;
    
    vector<D3DXVECTOR2>& vPoints = m_vvPoints[index];

    return AddPoint(index, Client2Canvas(pt));

}

int KCurveCtrl::AddPoint(int index, const D3DXVECTOR2& pt)
{
    if (index < 0 || index >= (int)m_vvPoints.size())
        return -1;

    vector<D3DXVECTOR2>& vPoints = m_vvPoints[index];

    vector<D3DXVECTOR2>::iterator i = find_if(vPoints.begin(), vPoints.end(), _CompareFind(pt));

    if (i == vPoints.end())
    {
        vPoints.push_back(pt);
        sort(vPoints.begin(), vPoints.end(), _Compare());
        i = find(vPoints.begin(), vPoints.end(), pt);
    }
    else
    {
        (*i) = pt;
    }

        if (IsWindow(GetSafeHwnd()))
            InvalidateRect(NULL);

        if (GetParent())
            GetParent()->PostMessage(WM_CURVERCTRL_SPLINE_CHANGED, (WPARAM)m_SelSpline);


    return (int)(i - vPoints.begin());
}

void KCurveCtrl::DelPoint(int indexSpline, int index)
{
    if (indexSpline < 0 || indexSpline >= (int)m_vvPoints.size())
        return;

    vector<D3DXVECTOR2>& vPoints = m_vvPoints[indexSpline];

    if (index > 0 && index < (int)vPoints.size() - 1)
    {
        vector<D3DXVECTOR2>::iterator idel = vPoints.begin();
        advance(idel, index);
        vPoints.erase(idel);

        if (IsWindow(GetSafeHwnd()))
            InvalidateRect(NULL);

        if (GetParent())
            GetParent()->PostMessage(WM_CURVERCTRL_SPLINE_CHANGED, (WPARAM)m_SelSpline);
    }
}

void KCurveCtrl::ResetPoint()
{
    for (size_t i = 0; i < m_vvPoints.size(); ++i)
    {
        m_vvPoints[i].clear();
        m_vvPoints[i].push_back(D3DXVECTOR2(0.0f, 0.0f));
        m_vvPoints[i].push_back(D3DXVECTOR2(1.0f, 1.0f));
    }

    if (IsWindow(GetSafeHwnd()))
        InvalidateRect(NULL);

    if (GetParent())
        GetParent()->PostMessage(WM_CURVERCTRL_SPLINE_CHANGED, (WPARAM)m_SelSpline);
    
}

void KCurveCtrl::ShowSpline(int index, bool show)
{
    if (index < 0 || index >= (int)m_vShowFlag.size())
        return;

    if (m_vShowFlag[index] != show)
    {
        m_vShowFlag[index] = show;
        InvalidateRect(NULL);
    }
}

BOOL KCurveCtrl::OnEraseBkgnd(CDC* pDC)
{
    return FALSE;
}

void KCurveCtrl::SetBkColor(COLORREF color)
{
    delete m_pBkBrush;
    m_pBkBrush = new CBrush(color);
}

void KCurveCtrl::SetCellColor(COLORREF color)
{
    delete m_pCellPen;
    m_pCellPen = new CPen(PS_DOT, 1, color);
}

void KCurveCtrl::SetSplineColor(int index, COLORREF color)
{
    if (index < 0 || index >= (int)m_vpSplinePen.size())
        return;

    delete m_vpSplinePen[index];
    m_vpSplinePen[index] = new CPen(PS_SOLID, 1, color);
}

void KCurveCtrl::OnPaint()
{
    const int MaxPtNum = 1024;

    CPaintDC dc(this);
    RECT rcClient;
    double pts[MaxPtNum];

    GetClientRect(&rcClient);
    dc.FillRect(&rcClient, m_pBkBrush);

    rcClient.left += Client_Offset;
    rcClient.top += Client_Offset;
    rcClient.right -= Client_Offset;
    rcClient.bottom -= Client_Offset;

    float w = static_cast<float>(rcClient.right - rcClient.left);
    float h = static_cast<float>(rcClient.bottom - rcClient.top);

    dc.SetBkMode(TRANSPARENT);
    dc.SelectObject(m_pCellPen);
    dc.SelectObject(m_pBkBrush);

    for (int i = 0; i < 5; ++i)
    {
        dc.MoveTo(rcClient.left, (int)(rcClient.top + h / 4 * i));
        dc.LineTo(rcClient.right, (int)(rcClient.top + h / 4 * i));
        dc.MoveTo((int)(rcClient.left + w / 4 * i), rcClient.top);
        dc.LineTo((int)(rcClient.left + w / 4 * i), rcClient.bottom);
    }

    int n = min(MaxPtNum, (int)(w + 0.5f));

    for (size_t s = 0; s < (int)m_vvPoints.size(); ++s)
    { 
        if (!m_vShowFlag[s])
            continue;

        vector<D3DXVECTOR2>& vPoints = m_vvPoints[s];

        dc.SelectObject(m_vpSplinePen[s]);

        for (size_t i = 0; i < (int)vPoints.size(); ++i)
        {
            POINT pt = Canvas2Cleint(vPoints[i]);
            RECT rc = {pt.x - Point_Size, pt.y - Point_Size, pt.x + Point_Size + 1, pt.y + Point_Size + 1};
            RECT rcBound = rcClient;

            rcBound.left -= 1;
            rcBound.right += 1;
            rcBound.top -= 1;
            rcBound.bottom += 1;
            
            if (PtInRect(&rcBound, pt))
                dc.Rectangle(&rc);
        }

        GetSplinePoint((int)s, pts, n);

        D3DXVECTOR2 pt = D3DXVECTOR2(0.f, (float)pts[0]);
        dc.MoveTo(Canvas2Cleint(pt));

        for (int i = 1; i < n; ++i)
        {
            pt = D3DXVECTOR2((float)i / (float)n, (float)pts[i]);
            dc.LineTo(Canvas2Cleint(pt));
        }
    }

}

POINT KCurveCtrl::Canvas2Cleint(const D3DXVECTOR2& pt)
{
    POINT ptRes;
    RECT  rcClient;
    
    GetClientRect(&rcClient);

    rcClient.left += Client_Offset;
    rcClient.top += Client_Offset;
    rcClient.right -= Client_Offset;
    rcClient.bottom -= Client_Offset;

    float w = static_cast<float>(rcClient.right - rcClient.left);
    float h = static_cast<float>(rcClient.bottom - rcClient.top);

    ptRes.x = static_cast<LONG>(rcClient.left + pt.x * w + 0.5f);
    ptRes.y = static_cast<LONG>(rcClient.bottom - pt.y * h + 0.5f);

    return ptRes;
}

D3DXVECTOR2 KCurveCtrl::Client2Canvas(const POINT& pt, bool clip)
{
    D3DXVECTOR2 ptRes;
    RECT        rcClient;
    
    GetClientRect(&rcClient);

    rcClient.left += Client_Offset;
    rcClient.top += Client_Offset;
    rcClient.right -= Client_Offset;
    rcClient.bottom -= Client_Offset;

    float w = static_cast<float>(rcClient.right - rcClient.left);
    float h = static_cast<float>(rcClient.bottom - rcClient.top);

    ptRes.x = (pt.x - rcClient.left) / w;
    ptRes.y = (rcClient.bottom - pt.y) / h;

    if (clip)
    {
        ptRes.x = max(0.f, min(ptRes.x, 1.f));
        ptRes.y = max(0.f, min(ptRes.y, 1.f));
    }

    return ptRes;
}

bool KCurveCtrl::PtOnSpline(double x, double y, double pre)
{
    for (size_t i = 0; i < m_vvPoints.size(); ++i)
    {
        if (!m_vShowFlag[i])
            continue;
       
        if (fabs(GetSplinePoint((int)i, x) - y) < pre)
        {
            m_SelSpline = (int)i;
            return true;
        }
    }

    m_SelSpline = -1;
    return false;
}

// KCurveCtrl message handlers



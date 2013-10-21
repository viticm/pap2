#pragma once

#define WM_CURVERCTRL_SPLINE_CHANGED WM_USER + 1225

class KCurveCtrl : public CWnd
{
	DECLARE_DYNAMIC(KCurveCtrl)

public :
    void GetSplinePoint(int index, double pt[], int n, bool clip = true);
    double GetSplinePoint(int index, double x, bool clip = true);

    template<typename iterator_type>
    static void GetSplinePoint(double pt[], int n, iterator_type* first, iterator_type* last, bool clip = true)
    {
        if (n <= 0)
            return;

        double step = 1.0 / (double)n;
        int i = 0;
        int size = static_cast<int>(distance(first, last));

        double* x = new double[size];
        double* y = new double[size];
        double s[5];

        for (iterator_type* p = first; p != last; ++p)
        {
            x[i] = p->x;
            y[i] = p->y;
        }

        if (clip)
        {
            for (double curr = 0.0; curr <= 1.0; curr += step)
                pt[i++] = max(0.0, min(1.0, CubicSplineIntl(x, y, size, -1, curr, s)));
        }
        else
        {
            for (double curr = 0.0; curr <= 1.0; curr += step)
                pt[i++] = CubicSplineIntl(x, y, size, -1, curr, s);
        }

        if (i < n)
            pt[i++] = CubicSplineIntl(x, y, size, -1, 1.0, s);

        delete[] x;
        delete[] y;
    }

    D3DXVECTOR2* GetControlPoints(int index, int& num, bool& bNeedDel);

    template<typename iterator_type>
    void AddPoint(int index, iterator_type* first, iterator_type* last)
    {
        for (iterator_type* p = first; p != last; ++p)
            AddPoint(index, *p);
    }

    int AddPoint(int index, const D3DXVECTOR2& pt);
    void DelPoint(int indexSpline, int index);
    void ResetPoint();
    void ShowSpline(int index, bool show);

    void SetSplineNum(int num);
    void SetBkColor(COLORREF color);
    void SetCellColor(COLORREF color);
    void SetSplineColor(int index, COLORREF color);


	KCurveCtrl();
	virtual ~KCurveCtrl();

protected :
    CPen*               m_pCellPen;
    vector<CPen*>       m_vpSplinePen;
    CBrush*             m_pBkBrush;
    vector<vector<D3DXVECTOR2>> m_vvPoints;
    vector<bool>        m_vShowFlag;
    int                 m_SelSpline;
    int                 m_SelIndex;
    DWORD               m_State;

protected :

    enum {Normal, On_Spline, Move_Point};
    enum {Point_Size = 2, Client_Offset = 5};

    POINT Canvas2Cleint(const D3DXVECTOR2& pt);
    D3DXVECTOR2 Client2Canvas(const POINT& pt, bool clip = true);
    bool  PtOnSpline(double x, double y, double pre);
    int AddPoint(int index, const POINT& pt);

	DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};



////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGLilisSlipPanel.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-4-21 23:03:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGLILISSLIPPANEL_H_
#define _INCLUDE_KGLILISSLIPPANEL_H_

#include "KGLilisPanel.h"

extern inline void _ScreenToClient(HWND hWnd, LPRECT Rect);

class KGLilisVirtualWnd
{
public :
    KGLilisVirtualWnd(HWND hDependWnd);
    virtual ~KGLilisVirtualWnd();

public :
    void GetWindowRect(LPRECT lpRect) const;
    int  ShowWindow(int       nCmdShow);
    int  MoveWindow(LPCRECT   lpRect);
    int  AddElement(HWND      hElement);
    int  DrawWindow();
    int  IsGetFocus();
    int  AjustFrame();

public :
    RECT              m_Rect;
    HWND              m_hDependWindow;
    std::vector<HWND> m_vecElements;

    struct RATE {
        float left;
        float top;
        float right;
        float bottom;
    };
    std::vector<RATE> m_vecEleRates;

};

template<class BaseType> 
class KGLilisSlipPanel : public BaseType
{
	enum PanelState
	{
		PS_SHRINK = 0,
		PS_EXPAND,
		PS_HIDE
	};

public :
    KGLilisSlipPanel(UINT uTemplateID, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE))
        : BaseType(uTemplateID, nIDCaption, dwSize) {

        m_nStrollFlag       = FALSE;
        m_nTopOffset        = 2;
        m_nWeekRate         = 50; 
        m_nOffset           = 0;
        m_vOffset.x         = 10;
        m_vOffset.y         = 10;
    };

    KGLilisSlipPanel(UINT uTemplateID, CWnd* pParent) : BaseType(uTemplateID, pParent) {

        m_nStrollFlag       = FALSE;
        m_nTopOffset        = 2;
        m_nWeekRate         = 50; 
        m_nOffset           = 0;
        m_vOffset.x         = 10;
        m_vOffset.y         = 10;
    };

    virtual ~KGLilisSlipPanel(){

        ::DeleteObject(m_hCursorNormal);
        ::DeleteObject(m_hCursorScroll);
        ::DeleteObject(m_hPanelBk);
        ::DeleteObject(m_hFocusBk);
        ::DeleteObject(m_hNormlBk);

        Clear();
    };

    BOOL PreTranslateMessage(MSG* pMsg) {

        int nResult = BaseType::PreTranslateMessage(pMsg);

        if (pMsg->message == WM_LBUTTONUP && pMsg->hwnd == m_hWnd) {

            m_nStrollFlag = FALSE;
            ReleaseCapture();
        }

        if (pMsg->message == WM_LBUTTONDOWN) {

            if (!OnClickTitle()) {

                if (pMsg->hwnd == this->m_hWnd) {

                    m_nStrollFlag = TRUE;
                    SetCapture();
                    GetCursorPos(&m_pointPrev);
                }
            }
        }

        if (pMsg->message == WM_MOUSEMOVE) {

            if (pMsg->hwnd == m_hWnd)
                OnMouseMove();
        }


        if (pMsg->message == WM_MOUSEWHEEL) {

            OnMouseWheel(0, HIWORD(pMsg->wParam), 0);
        }

        return nResult; 
    }

    BOOL OnInitDialog() {

        int nResult = BaseType::OnInitDialog();

        m_hCursorScroll = ::LoadCursor(NULL, IDC_SIZEALL);
        m_hCursorNormal = ::LoadCursor(NULL, IDC_ARROW);

        std::map<int, HWND> mapContainer;
        std::vector<HWND> vecChilds;

        HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);

        while (hChild)
        {
            LONG lStyle = ::GetWindowLong(hChild, GWL_STYLE);
            TCHAR szClassName[128];
            TCHAR szWndName[128] = {0};
            RECT  Rect;

            ::GetClassName(hChild, szClassName, 128);
            ::GetWindowText(hChild, szWndName, 128);
            ::GetWindowRect(hChild, &Rect);

            int scal = 0;
            if (
                (lStyle & BS_GROUPBOX) == BS_GROUPBOX && 
                !stricmp(szClassName, TEXT("Button")) && 
                *szWndName == '#'
                ) 
            {
                TCHAR* pStr = &szWndName[1];
                ::SetWindowText(hChild, pStr);
                scal = Rect.top;

                if (mapContainer.size() && mapContainer.find(scal) != mapContainer.end())
                    scal = mapContainer.rbegin()->first + 1;
                mapContainer[scal] = hChild;

            }
            else
                vecChilds.push_back(hChild);
            hChild = ::GetWindow(hChild, GW_HWNDNEXT);
        }

        // size_t NumGroups = vecContainer.size();

        //for (size_t i = 0; i < NumGroups; i++)
        for (std::map<int, HWND>::iterator i = mapContainer.begin(); i != mapContainer.end(); i++)
        {
            KGLilisVirtualWnd vWnd(this->m_hWnd);
            RECT  RectGroup;
            TCHAR szName[128];

            HWND hContainer = i->second;
            ::GetWindowRect(hContainer, &RectGroup);
            ::GetWindowText(hContainer, szName, 128);
            /* ::GetWindowRect(vecContainer[i], &RectGroup);
            ::GetWindowText(vecContainer[i], szName, 128);*/

            for (size_t c = 0; c < vecChilds.size(); c++)
            {
                RECT  RectGhild;
                ::GetWindowRect(vecChilds[c], &RectGhild);

                POINT center;

                center.x = (RectGhild.right + RectGhild.left) / 2;
                center.y = (RectGhild.bottom + RectGhild.top) / 2;

                if (::PtInRect(&RectGroup, center))
                {
                    vWnd.AddElement(vecChilds[c]);
                    vecChilds.erase(vecChilds.begin() + c);
                    c--;
                }
            }
            if ((::GetWindowLong(hContainer, GWL_STYLE) & WS_VISIBLE) == WS_VISIBLE)
                AddPanel(vWnd, szName, PS_EXPAND);
            else
                AddPanel(vWnd, szName, PS_SHRINK); 
            ::ShowWindow(hContainer, SW_HIDE);
            //::DestroyWindow(hContainer);
        }

        m_hPanelBk = ::CreateSolidBrush(RGB(198, 198, 198));
        m_hFocusBk = ::CreateSolidBrush(RGB(240, 200,  85));
        m_hNormlBk = ::CreateSolidBrush(RGB(210, 210, 233));

        return nResult;
    }

    int  Clear(){

        for (DWORD i = 0; i < m_vecChileWnd.size(); i++)
            SAFE_DELETE(m_vecChileWnd[i].pTitleBar);
        m_vecChileWnd.clear();
        return true;
    }

    int  AddPanel(KGLilisVirtualWnd vWnd, CString strName, PanelState dwState) {

        _LILISPANE lilisPane(this->m_hWnd);
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
        lilisPane.pTitleBar->SetParent(this);
        ASSERT(lilisPane.pTitleBar);
        lilisPane.pTitleBar->strName = strName;
        lilisPane.vWndPane = vWnd;
        lilisPane.strName  = strName;
        lilisPane.dwState  = dwState;
        lilisPane.vWndPane.m_hDependWindow = this->m_hWnd;

        lilisPane.vWndPane.AjustFrame();

        m_vecChileWnd.push_back(lilisPane);

        return UpdateUI();
    }

    int  UpdateUI() {

        RECT rectClient;

        GetClientRect(&rectClient);
        rectClient.left  += m_vOffset.x;
        rectClient.right -= m_vOffset.y;
       
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
                rectTitle.top = rectClient.top + m_nOffset;
            }
            else
            {
				SetRect(&rectTemp2, 0, 0, 0, 0);

				switch (m_vecChileWnd[s - 1].dwState)
				{
				case PS_EXPAND :
					m_vecChileWnd[s - 1].vWndPane.GetWindowRect(&rectTemp2);
					break;
				case PS_SHRINK :
					m_vecChileWnd[s - 1].pTitleBar->GetWindowRect(&rectTemp2);
					ScreenToClient(&rectTemp2);
					break;
				case PS_HIDE :
					rectTemp2.bottom = 0;
					break;
				default :
					ASSERT(FALSE);
					break;
				}

                rectTitle.top = rectTemp2.bottom + 10;
            }

			if (m_vecChileWnd[s].dwState == PS_HIDE)
			{
				m_vecChileWnd[s].pTitleBar->ShowWindow(SW_HIDE);
				m_vecChileWnd[s].vWndPane.ShowWindow(SW_HIDE);
				continue;
			}

			m_vecChileWnd[s].pTitleBar->ShowWindow(SW_SHOW);
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

			switch (m_vecChileWnd[s].dwState)
			{
			case PS_EXPAND :
				dwType = SW_SHOW;
				break;
			case PS_SHRINK :
				dwType = SW_HIDE;
				break;
			case PS_HIDE :
				dwType = SW_HIDE;
				break;
			default :
				ASSERT(FALSE);
				break;
			}

            m_vecChileWnd[s].vWndPane.ShowWindow(dwType);
        }
        return TRUE;
    }

    int  OnClickTitle() {

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
				if (m_vecChileWnd[s].dwState == PS_EXPAND)
					m_vecChileWnd[s].dwState = PS_SHRINK;
				else
					m_vecChileWnd[s].dwState = PS_EXPAND;
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

    int  OnMouseMove() {

        size_t size = m_vecChileWnd.size();
        if (!size)
            return TRUE;

        if (!m_nStrollFlag)
            return TRUE;

        POINT pointCurr;
        GetCursorPos(&pointCurr);

        if (pointCurr.y == m_pointPrev.y)
            return TRUE;

        int offset = pointCurr.y - m_pointPrev.y;        

        if (offset > 0) {

            int dy = m_nOffset + offset;

            if (dy > 0)
                offset = -m_nOffset;
        }
        else {

            RECT RectClient;
            RECT RectChild;
            
            m_vecChileWnd[size - 1].vWndPane.GetWindowRect(&RectChild);

            if (!m_vecChileWnd[size - 1].dwState == PS_EXPAND)
                RectChild.bottom = RectChild.top;

            GetClientRect(&RectClient);

            int dy = RectClient.bottom - RectChild.bottom - 10;

            if (dy > 0)
                return TRUE;

            if (offset < dy)
                offset = dy;
        }

        m_nOffset += offset;
        ScrollWindow(0, offset);
        
        for (size_t i = 0; i < size; i++) {
            OffsetRect(&(m_vecChileWnd[i].vWndPane.m_Rect), 0, offset);
        }

        m_pointPrev = pointCurr;

        return TRUE;
    }

    int  OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {

        if (zDelta > 0)
            zDelta =  static_cast<short>(m_nWeekRate);
        else
            zDelta = -static_cast<short>(m_nWeekRate);

        size_t size = m_vecChileWnd.size();
        CWnd* pWnd = GetParent();
        if (!pWnd || !size)
            return TRUE;

        int offset = zDelta;

        if (offset > 0) {

            int dy = m_nOffset + offset;

            if (dy > 0)
                offset = -m_nOffset;
        }
        else {

            RECT RectClient;
            RECT RectChild;

            m_vecChileWnd[size - 1].vWndPane.GetWindowRect(&RectChild);
            GetClientRect(&RectClient);

            int dy = RectClient.bottom - RectChild.bottom - 10;

            if (dy > 0)
                return TRUE;

            if (offset < dy)
                offset = dy;
        }

        m_nOffset += offset;
        ScrollWindow(0, offset);

        for (size_t i = 0; i < size; i++) {
            OffsetRect(&(m_vecChileWnd[i].vWndPane.m_Rect), 0, offset);
        }

       

        return TRUE;
    }

    int ReDraw(BOOL bErase, CDC& dc) {

        COLORREF color;
        if (bErase) 
            color = ::GetSysColor(COLOR_BTNFACE);
        else
            color = RGB(100, 100, 100);
           
            for (size_t s = 0; s < m_vecChileWnd.size(); s++) {

                if (m_vecChileWnd[s].dwState == PS_EXPAND) {

                    CPen pen1(PS_SOLID, 1, color);
                    RECT rectTitle;
                    RECT rectPanle;
                    m_vecChileWnd[s].vWndPane.GetWindowRect(&rectPanle);
                    m_vecChileWnd[s].pTitleBar->GetWindowRect(&rectTitle);
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
                else if (m_vecChileWnd[s].dwState == PS_SHRINK)
                {
                    CPen pen1(PS_SOLID, 1, color);
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
				else
				{

				}
            }

        return true;
    }

    void SetOffset(int x, int y) {
        m_vOffset.x = x;
        m_vOffset.y = y;
    }

    void RecalcPaneSize() {

        for (size_t i = 0; i < m_vecChileWnd.size(); ++i)
        {
            KGLilisVirtualWnd& vWnd = m_vecChileWnd[i].vWndPane;

            vWnd.m_Rect.left     = INT_MAX;
            vWnd.m_Rect.top      = INT_MAX;
            vWnd.m_Rect.right    = INT_MIN;
            vWnd.m_Rect.bottom   = INT_MIN;

            for (size_t e = 0; e < vWnd.m_vecElements.size(); ++e)
            {
                HWND hElement = vWnd.m_vecElements[e];

                RECT rectElement;

                ::GetWindowRect(hElement, &rectElement);
                _ScreenToClient(vWnd.m_hDependWindow, &rectElement);

                vWnd.m_Rect.bottom = max(vWnd.m_Rect.bottom, rectElement.bottom);
                vWnd.m_Rect.top    = min(vWnd.m_Rect.top,    rectElement.top);
                vWnd.m_Rect.left   = min(vWnd.m_Rect.left,   rectElement.left);
                vWnd.m_Rect.right  = max(vWnd.m_Rect.right,  rectElement.right);
            }

            vWnd.AjustFrame();
        }
    }

	void ShowPanel(size_t Index, bool bShow = true)
	{
		if (Index < 0 || Index >= m_vecChileWnd.size())
			return;

		if (bShow)
		{
			if (m_vecChileWnd[Index].dwState == PS_HIDE)
			{
				m_vecChileWnd[Index].dwState = PS_EXPAND;
				UpdateUI();
				InvalidateRect(NULL);
			}
		}
		else
		{
			if (m_vecChileWnd[Index].dwState != PS_HIDE)
			{
				m_vecChileWnd[Index].dwState = PS_HIDE;
				UpdateUI();
				InvalidateRect(NULL);
			}
		}
	}

public :
    struct _LILISPANE {

        KGLilisPanelTitle*  pTitleBar;
        KGLilisVirtualWnd   vWndPane; 
        CString             strName;
        PanelState			dwState; 

        _LILISPANE(HWND hDependWnd) : vWndPane(hDependWnd)
        {
            pTitleBar = NULL;
            strName   = TEXT("");
            dwState   = PS_EXPAND;
        }
    };
    std::vector<_LILISPANE>   m_vecChileWnd;
    int                  m_nStrollFlag;
    POINT                m_pointPrev;
    HCURSOR              m_hCursorScroll;
    HCURSOR              m_hCursorNormal;
    int                  m_nTopOffset;
    int                  m_nWeekRate;  
    HWND                 m_hFocusCtl;
    POINT                m_vOffset;

protected :
    int                  m_nOffset;        
    HBRUSH               m_hPanelBk;
    HBRUSH               m_hFocusBk;
    HBRUSH               m_hNormlBk;

public :

    PTM_WARNING_DISABLE
    virtual const AFX_MSGMAP* KGLilisSlipPanel<BaseType>::GetMessageMap() const 
    { 
        return GetThisMessageMap(); 
    } 
    static const AFX_MSGMAP* PASCAL KGLilisSlipPanel<BaseType>::GetThisMessageMap() 
    { 
        typedef KGLilisSlipPanel<BaseType> ThisClass;						
        typedef BaseType                   TheBaseClass;					 
        static const AFX_MSGMAP_ENTRY _messageEntries[] =  
        {
            ON_WM_CTLCOLOR()
            ON_WM_SETCURSOR()
            ON_WM_PAINT()
          
            {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } 
        }; 
        static const AFX_MSGMAP messageMap = 
        { 
            &TheBaseClass::GetThisMessageMap, &_messageEntries[0] 
        }; 
        return &messageMap; 
    }						
    PTM_WARNING_RESTORE


    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {

        HBRUSH hBursh = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

        if (nCtlColor == CTLCOLOR_EDIT)
        {
            if (::GetFocus() == pWnd->m_hWnd) {
                
                COLORREF color = RGB(240, 200, 85);
                hBursh = m_hFocusBk;
                pDC->SetBkColor(color);
            } 
            else  {

                COLORREF color = RGB(210, 210, 233);
                hBursh = m_hNormlBk;
                pDC->SetBkColor(color);
            }
        }

        //if (pWnd == this)
        //    hBursh = m_hPanelBk;

        return hBursh;
    }

    afx_msg BOOL OnEraseBkgnd(CDC* pDC) {

        return TRUE;
    }

    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
    
        if (pWnd == this && m_hCursorScroll && nHitTest == HTCLIENT) {

            ::SetCursor(m_hCursorScroll);
            return TRUE;
        }
        return BaseType::OnSetCursor(pWnd, nHitTest, message);
    }

    afx_msg void OnPaint() {
		{///±‹√‚warning C4239
			CPaintDC TempDC(this);
			ReDraw(false, TempDC);
		}
       //ReDraw(false, CPaintDC(this));
    }

    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_COMMAND) {

            if (HIWORD(wParam) == EN_SETFOCUS || HIWORD(wParam) == EN_KILLFOCUS)
                ::InvalidateRect((HWND)lParam, NULL, TRUE);
        }

        if (message == WM_SIZE) 
		{
			{
				CClientDC TempDC(this);
				ReDraw(true, TempDC);
			}
            UpdateUI();
			{
				CClientDC TempDC(this);

				ReDraw(false, TempDC);
			}
        }

        return BaseType::WindowProc(message, wParam, lParam);
    }
};

#endif //_INCLUDE_KGLILISSLIPPANEL_H_

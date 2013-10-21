
#ifndef _KGLILIS_H_
#define _KGLILIS_H_

#include "vector"


using namespace std;

//-----------------------------------------------------------------------------
// LilisButton define
//-----------------------------------------------------------------------------

class KGLilisButton : public CButton
{
     DECLARE_DYNAMIC(KGLilisButton)

public :
    KGLilisButton();
    ~KGLilisButton();

    CFont font;

public :
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
public:
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};

//-----------------------------------------------------------------------------
// LilisEdit define
//-----------------------------------------------------------------------------

class KGLilisEdit : public CEdit
{
    DECLARE_DYNAMIC(KGLilisEdit)

public :
    KGLilisEdit();
    ~KGLilisEdit();

public :
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


//-----------------------------------------------------------------------------
// LilisSheet Define
//-----------------------------------------------------------------------------

class KGLilisSheet : public CDialog
{
    DECLARE_DYNAMIC(KGLilisSheet)
public :
    KGLilisSheet(UINT uTemplateID, CWnd* pParent = NULL);
    ~KGLilisSheet();
    BOOL PreTranslateMessage(MSG* pMsg);
    

public :
    DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

//-----------------------------------------------------------------------------
// LilisPanelTitle define
//-----------------------------------------------------------------------------

class KGLilisPanelTitle : public CWnd
{
    DECLARE_DYNAMIC(KGLilisPanelTitle)

public :
    KGLilisPanelTitle();
    virtual ~KGLilisPanelTitle();

public :
    CFont   font;
    DWORD   dwState;
    CString strName;

public :
    DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnNcPaint();
};

//-----------------------------------------------------------------------------
// LilisPanel define
//-----------------------------------------------------------------------------

class KGLilisPanel : public CWnd
{
	DECLARE_DYNAMIC(KGLilisPanel)

public :
    BOOL PreTranslateMessage(MSG* pMsg);
    int  UpdateUI();
    int  AddPanel(CWnd* pWnd, CString strName, DWORD dwState);
    int  OnClickTitle();
    int  OnMouseMove();

public :
	KGLilisPanel();
	virtual ~KGLilisPanel();

public :
    struct _LILISPANE 
    {
        KGLilisPanelTitle*  pTitleBar;
        CWnd*               pWndPane;
        CString             strName;
        DWORD               dwState; 

        _LILISPANE()
        {
            pTitleBar = NULL;
            pWndPane  = NULL;
            strName   = TEXT("");
            dwState   = TRUE;
        }
    };
    vector<_LILISPANE>   m_vecChileWnd;
    int                  m_nPanelOffset;
    int                  m_nStrollFlag;

    POINT                m_pointPrev;

    //
    HCURSOR              m_hCursorScroll;
    HCURSOR              m_hCursorNormal;
  
protected :
	DECLARE_MESSAGE_MAP()
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

//-----------------------------------------------------------------------------
// LilisVirtualWnd define
//-----------------------------------------------------------------------------
class KGLiliVirtualWnd
{
public :
    KGLiliVirtualWnd(CWnd* pDependWnd);
    KGLiliVirtualWnd();
    ~KGLiliVirtualWnd();

public :
    void GetWindowRect(LPRECT lpRect) const;
    int  ShowWindow(int       nCmdShow);
    int  MoveWindow(LPCRECT   lpRect);
    int  AddElement(CWnd*     pElement);
    int  DrawWindow();
    int  IsGetFocus();
    int  AjustFrame();

public :
    RECT            m_rectPerv;
    CWnd*           m_pPanleWindws;
    CWnd*           m_pDependWindow;
    vector<CWnd*>   m_vecElements;
    struct RATE
    {
        float left;
        float top;
        float right;
        float bottom;
    };
    vector<RATE>    m_vecEleRects;

};

//-----------------------------------------------------------------------------
// KGLilisPanelDLG Define
//-----------------------------------------------------------------------------

class KGLilisPanelDLG : public CDialog
{
    DECLARE_DYNAMIC(KGLilisPanelDLG)

public :
    KGLilisPanelDLG(UINT uTemplateID, CWnd* pParent = NULL);
    ~KGLilisPanelDLG();
    virtual void DoDataExchange(CDataExchange* pDX);    
    BOOL PreTranslateMessage(MSG* pMsg);
    BOOL OnInitDialog();

public :
    int  AddPanel(KGLiliVirtualWnd vlWnd, CString strName, DWORD dwState);
    int  UpdateUI();
    int  OnClickTitle();
    int  OnMouseMove(); 
    int  OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    BOOL EnableSlip(BOOL bEnable) {m_bEnable = bEnable; return m_bEnable;}
    BOOL EnableSelfWeek(BOOL bEnable) {m_bEnableSelpWeek = bEnable; return m_bEnableSelpWeek;}
    int  Clear();

public :
    struct _LILISPANE 
    {
        KGLilisPanelTitle*  pTitleBar;
        KGLiliVirtualWnd    vWndPane; 
        CString             strName;
        DWORD               dwState; 

        _LILISPANE()
        {
            pTitleBar = NULL;
            strName   = TEXT("");
            dwState   = TRUE;
        }
    };
    vector<_LILISPANE>   m_vecChileWnd;
    int                  m_nStrollFlag;
    POINT                m_pointPrev;
    HCURSOR              m_hCursorScroll;
    HCURSOR              m_hCursorNormal;
    BOOL                 m_bEnable;
    int                  m_nTopOffset;
    int                  m_nWeekRate;
    BOOL                 m_bEnableSelpWeek;   

private :
    int                  m_nOffset;

public :
    DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

#endif
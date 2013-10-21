////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorView.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-8 16:08:24
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORVIEW_H_
#define _INCLUDE_KSCENESFXEDITORVIEW_H_

#include "KGSFXModelPane.h"
#include "KSceneSFXEditorPanel.h"
#include "KSceneSFXEditorKeyFrameDlg.h"
#include "afxwin.h"
#include "KGSFXSuedeBindToolBAR.h"
#include "KGSFXSocketBindDlg.h"
#include "InputProxy.h"

//-----------------------------------------------------------------------------
// Type Define
//-----------------------------------------------------------------------------
struct SFX_COLOR_PROJECT
{
    COLORREF colorBacks;
    COLORREF colorFocus;
};

enum 
{
    SFX_WND_STATE_BIND,
    SFX_WND_STATE_FREE
};

enum 
{
    WND_MAIN_PANE,
    WND_LEFT_PANE,
    WND_EDIT_PANE,
    WND_PLAY_PANE,
    WND_UNKONW
};

//-----------------------------------------------------------------------------
// KGSFXMainPaneWindow
//-----------------------------------------------------------------------------
class KGSFXMainPaneWindow : public CDialog
{
    DECLARE_DYNCREATE(KGSFXMainPaneWindow)

public :
    KGSFXMainPaneWindow(CWnd* pParent = NULL);
    ~KGSFXMainPaneWindow();
    enum { IDD = IDD_SFX_MAIN_VIEW_WND };

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    int   SetDependWnd(CView* pDependWnd);
    void  OnLButtonDownMessage();
    void  OnLButtonUpMessage();
    void  OnRButtonUpMessage();
    void  OnKeyDownMessage();
    void  OnMouseMoveMessage();
    
    DWORD UpdateWndState();
    int   Init();

public :
    CView*   m_pDependWnd;
    DWORD    m_dwWndState;
    HCURSOR  m_hCursor;
    DWORD    m_dwPrevTime;
	HACCEL	 m_hAccel;
    CInputProxy m_InputProxy;

    D3DXVECTOR3 m_vPervRayPoint;
    D3DXVECTOR3 m_vPervRayPointEx;

public :
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
    void OnOK(){};
    void OnCancel(){};
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSetFocus(CWnd*  pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
};

//-----------------------------------------------------------------------------
// KSceneSFXEditorView
//-----------------------------------------------------------------------------
class KSceneSFXEditorView : public CView
{	
protected:
	DECLARE_DYNCREATE(KSceneSFXEditorView)
	virtual void OnDraw(CDC* pDC);     
	int m_nOutputWindowID;

public :
    static KSceneSFXEditorView* GetActiveSFXView();
    IEKG3DSceneSFXEditor* GetSfxScene();

    
public :
    KSceneSFXEditorPanel       m_paneSFXEditerWnd;
    KGSFXModelPane             m_paneSFXModelView;
    KGSFXMainPaneWindow        m_paneSFXMainWnd;
    KSceneSFXEditorKeyFrameDlg m_paneSFXKeyFrame;
    //KGSFXSuedeBindToolBar      m_paneSFXSuedeTool;
    //KGSFXSocketBindDlg         m_popuSocketBindDlg;

    RECT  m_rectMainWnd;
    RECT  m_rectModelVW;

    SFX_COLOR_PROJECT          m_colorProject;
public:
	KSceneSFXEditorView(void);
	virtual ~KSceneSFXEditorView(void);

    RECT GetPaneRect(DWORD dwID);
    void  UpdateUI();

	DECLARE_MESSAGE_MAP()
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
    afx_msg void OnSceneSetting();
public:
    CWnd *m_pInvoker;
	afx_msg void OnMeteor();
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};




#endif //_INCLUDE_KSCENESFXEDITORVIEW_H_

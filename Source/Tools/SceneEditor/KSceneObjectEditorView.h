#if !defined(AFX_KSCENEOBJECTEDITORVIEW_H__A98AB4FA_CFA8_4591_AA53_53C2471D9969__INCLUDED_)
#define AFX_KSCENEOBJECTEDITORVIEW_H__A98AB4FA_CFA8_4591_AA53_53C2471D9969__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneObjectEditorView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorView view

class KSceneObjectEditorView : public CView
{
protected:
	int  m_nOutputWindowID;
	KSceneObjectEditorView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(KSceneObjectEditorView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneObjectEditorView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
protected:

	//{{AFX_MSG(KSceneObjectEditorView)
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

// Implementation
protected:
	virtual ~KSceneObjectEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(KSceneObjectEditorView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//GraphicsStruct::KCamera  m_Camera;

	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnInitialUpdate();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEOBJECTEDITORVIEW_H__A98AB4FA_CFA8_4591_AA53_53C2471D9969__INCLUDED_)

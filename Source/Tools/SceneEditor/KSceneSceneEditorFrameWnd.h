#if !defined(AFX_KSCENESCENEEDITORFRAMEWND_H__C4838C9E_C785_44BA_AE76_9E43FBF44B2C__INCLUDED_)
#define AFX_KSCENESCENEEDITORFRAMEWND_H__C4838C9E_C785_44BA_AE76_9E43FBF44B2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorFrameWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorFrameWnd frame

class KSceneSceneEditorFrameWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(KSceneSceneEditorFrameWnd)
protected:
	KSceneSceneEditorFrameWnd();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorFrameWnd)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~KSceneSceneEditorFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorFrameWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CSplitterWnd  m_wndSplitter;
	BOOL          m_bSplitterCreated;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORFRAMEWND_H__C4838C9E_C785_44BA_AE76_9E43FBF44B2C__INCLUDED_)

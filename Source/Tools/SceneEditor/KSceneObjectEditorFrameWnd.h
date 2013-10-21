#if !defined(AFX_KSCENEOBJECTEDITORFRAMEWND_H__D98DEBE4_22E0_4C75_A735_EE79E32D7B08__INCLUDED_)
#define AFX_KSCENEOBJECTEDITORFRAMEWND_H__D98DEBE4_22E0_4C75_A735_EE79E32D7B08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneObjectEditorFrameWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorFrameWnd frame
//(by dengzhihui 2006Äê7ÔÂ12ÈÕ 18:16:55
#include "ChildFrm.h"
//)

class KSceneObjectEditorFrameWnd : public MFCFramework::CChildFrame
{
	DECLARE_DYNCREATE(KSceneObjectEditorFrameWnd)
protected:
	KSceneObjectEditorFrameWnd();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneObjectEditorFrameWnd)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	void ActivateFrame(int nCmdShow);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~KSceneObjectEditorFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(KSceneObjectEditorFrameWnd)
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

#endif // !defined(AFX_KSCENEOBJECTEDITORFRAMEWND_H__D98DEBE4_22E0_4C75_A735_EE79E32D7B08__INCLUDED_)

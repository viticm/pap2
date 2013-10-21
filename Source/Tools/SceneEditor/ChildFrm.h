// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__0CEFBB61_F9AA_48AE_A718_4FA3E9C7D91A__INCLUDED_)
#define AFX_CHILDFRM_H__0CEFBB61_F9AA_48AE_A718_4FA3E9C7D91A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace MFCFramework
{
class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:

// Operations
public:

	void ActivateFrame(int nCmdShow);
	public:
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


	// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	
	LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	void SetFullScreenState(BOOL bFullScreen);
private:
	BOOL m_bFullScreen;
};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__0CEFBB61_F9AA_48AE_A718_4FA3E9C7D91A__INCLUDED_)

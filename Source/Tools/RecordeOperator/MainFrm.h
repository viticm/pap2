// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ProgressDialogBar.h"
#include "MasterListDialogBar.h"
#include "KMovieOptionDialog.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
protected:
	BOOL m_bFullScreen;
public:
	CToolBar * m_pwndFullScrnBar;
	BOOL IsFullScreen();
	WINDOWPLACEMENT m_wpPrev;
	CRect m_FullScreenWindowRect;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CProgressDialogBar  m_wndProgressDialogBar;
	MasterListDialogBar m_wndMasterlistDialogBar;
	CToolBar    m_wndToolBar;

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL DestroyWindow();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonFree();
	afx_msg void OnViewFullScreen();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnUpdateViewFullScreen(CCmdUI* pCmdUI);
	afx_msg void OnExpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleaseCapture(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnMovieOutOption();
	DECLARE_MESSAGE_MAP()
};



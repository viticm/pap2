// ChildFrm.h : interface of the CChildFrame class
//


#pragma once


class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
    
// Attributes
public:
	BOOL m_bFullScreen;
	WINDOWPLACEMENT m_wpPrev;
	CRect m_FullScreenWindowRect;
	
// Operations
public:
	void FullScreen(CRect recFullScreen);
// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI); 
	afx_msg void CChildFrame::OnChildActivate();
	DECLARE_MESSAGE_MAP()
};

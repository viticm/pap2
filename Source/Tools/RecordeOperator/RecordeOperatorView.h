// RecordeOperatorView.h : interface of the CRecordeOperatorView class
//


#pragma once
#define ID_MENUE_CAMERAOPTION 10
#define ID_MENUE_MOVIESTART 11
#define ID_MENUE_MOVIESTOP 12
#define ID_MENUE_FULLSCREEN 13
#define ID_MENUE_EXITFULLSCREEN 14
#define ID_MENUE_RECORDSTART 15
#define ID_MENUE_RECORDPAUSE 16
#define ID_MENUE_RECORDSTOP 17

class CRecordeOperatorView : public CView
{
protected: // create from serialization only
	CRecordeOperatorView();
	DECLARE_DYNCREATE(CRecordeOperatorView)

// Attributes
public:
	CRecordeOperatorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate(); // called first time after construct
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CRecordeOperatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int	 m_nOutputWindowID;
	DWORD m_dwLastTime;
	CPoint m_pointMouse;
// Generated message map functions
protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	afx_msg void OnCameraOption();
	afx_msg void OnMovieStart();
	afx_msg void OnMovieStop();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	
};

#ifndef _DEBUG  // debug version in RecordeOperatorView.cpp
inline CRecordeOperatorDoc* CRecordeOperatorView::GetDocument() const
   { return reinterpret_cast<CRecordeOperatorDoc*>(m_pDocument); }
#endif


#pragma once


// KSwordTestView 视图

class KSwordTestView : public CView
{
	DECLARE_DYNCREATE(KSwordTestView)

protected:
	KSwordTestView();           // 动态创建所使用的受保护的构造函数
	virtual ~KSwordTestView();

public:
	//GraphicsStruct::KCamera  m_Camera;

	virtual void OnDraw(CDC* pDC);      // 重写以绘制此视图
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	int	 m_nOutputWindowID;

	//{{AFX_VIRTUAL(KSwordTestView)
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(KSceneEditorView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

};



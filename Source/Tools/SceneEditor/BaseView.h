#pragma once
//用于实现VIEW的一些公共行为

// CBaseView view
#include "InputProxy.h"

interface IEKG3DSceneEditorBase;

class CBaseView : public CView
{
	DECLARE_DYNCREATE(CBaseView)

protected:
	CBaseView();           // protected constructor used by dynamic creation
	virtual ~CBaseView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreTranslateMessage(MSG* pMsg);

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnEditorModeUpdate(CCmdUI *pCmdUI);
	afx_msg void OnShowObjectSelectDlg();	
	
	CInputProxy m_InputProxy;
protected:
	IEKG3DSceneEditorBase* GetDocScene();

	afx_msg void OnPopMenuMove();
	afx_msg void OnPopMenuSelect();
	afx_msg void OnPopMenuRotate();
	afx_msg void OnPopMenuScale();
	afx_msg void OnModeSelectUpdate(CCmdUI *pCmdUI);
	afx_msg void OnModeScaleUpdate(CCmdUI *pCmdUI);
	afx_msg void OnModeMoveUpdate(CCmdUI *pCmdUI);
	afx_msg void OnModeRotateUpdate(CCmdUI *pCmdUI);

	afx_msg void OnZoomToObj();
//	afx_msg BOOL OnMouseWheel(UINT, short, CPoint);
};



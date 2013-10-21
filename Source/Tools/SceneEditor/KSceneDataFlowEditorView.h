#pragma once
#include "BaseView.h"

// KSceneDataFlowEditorView view

class KSceneDataFlowEditorView : public CBaseView
{
	DECLARE_DYNCREATE(KSceneDataFlowEditorView)

protected:
	int m_nOutputWindowID;
	KSceneDataFlowEditorView();           // protected constructor used by dynamic creation
	virtual ~KSceneDataFlowEditorView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnInitialUpdate();
	void OnSize(UINT nType, int cx, int cy);

};





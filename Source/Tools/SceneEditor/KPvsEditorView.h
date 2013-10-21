#pragma once

#include "InputProxy.h"
#include "BaseView.h"

class KPvsEditorDoc;
class KSceneSceneEditorDialogDirMgr;

class KPvsEditorView : public CBaseView
{
	DECLARE_DYNCREATE(KPvsEditorView)

public :
    void OnInitialUpdate();

protected :
	KPvsEditorView();           // protected constructor used by dynamic creation
	virtual ~KPvsEditorView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual void OnSize(UINT nType, int cx, int cy);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected :
    KPvsEditorDoc* m_pDoc;
    int m_nOutputWindowID;

    KSceneSceneEditorDialogDirMgr*   m_lpDialogDirMgr;
    CDialog*	m_pSceneSettingDialog;



protected :
	DECLARE_MESSAGE_MAP()
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnSetCoord();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnEditCopy();
    afx_msg void OnEditPaste();
	afx_msg void OnDestroy();
    afx_msg void OnSetPMode();
    afx_msg void OnActionUp();	
    afx_msg void OnActionDown();	
    afx_msg void OnActionLeft();	
    afx_msg void OnActionRight();	
    afx_msg void OnActionPageUp();	
    afx_msg void OnActionPageDown();	
	afx_msg void OnEngineCommand();
	afx_msg void OnDirMgr();
    afx_msg void OnScenesetting();
};



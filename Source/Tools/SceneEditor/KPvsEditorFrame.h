#pragma once


#include "ChildFrm.h"

class KGPvsViewSplitterWnd : public CSplitterWnd
{
    DECLARE_DYNCREATE(KGPvsViewSplitterWnd)
public :
    KGPvsViewSplitterWnd();
    virtual ~KGPvsViewSplitterWnd();

protected : 

    bool m_bTracked;

    DECLARE_MESSAGE_MAP()
    void OnSize(UINT nType, int cx, int cy);

};

class KPvsEditorFrame : public MFCFramework::CChildFrame
{
	DECLARE_DYNCREATE(KPvsEditorFrame)
protected :
	KPvsEditorFrame();           // protected constructor used by dynamic creation
	virtual ~KPvsEditorFrame();

    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    void OnSize(UINT nType, int cx, int cy);

protected :
    KGPvsViewSplitterWnd m_wndSplitter;

protected : 
	DECLARE_MESSAGE_MAP()
    afx_msg void OnDeleteSelectedObject();
    afx_msg void OnZoomInObject();
    afx_msg void OnZoomOutObject();
    afx_msg void OnAbout();
};



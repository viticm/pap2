#pragma once

#ifdef _WIN32_WCE
#error "CMDIChildWnd is not supported for Windows CE."
#endif 

// KScneDataFlowEditorFrameWnd frame with splitter

class KScneDataFlowEditorFrameWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(KScneDataFlowEditorFrameWnd)
protected:
	KScneDataFlowEditorFrameWnd();           // protected constructor used by dynamic creation
	virtual ~KScneDataFlowEditorFrameWnd();

	CSplitterWnd m_wndSplitter;

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};



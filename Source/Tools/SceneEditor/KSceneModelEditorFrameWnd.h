#if !defined(AFX_KSCENEMODELEDITORFRAMEWND_H__18101BC6_9F72_46E2_AA65_511600371E1D__INCLUDED_)
#define AFX_KSCENEMODELEDITORFRAMEWND_H__18101BC6_9F72_46E2_AA65_511600371E1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorFrameWnd.h : header file
//
#define WM_USER_MODEL_CHANGE WM_USER + 0x1

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorFrameWnd frame
//(by dengzhihui 2006Äê7ÔÂ12ÈÕ 18:16:55
#include "ChildFrm.h"
//)
#include "KSceneModelEditorPropertyPageMesh.h"
#include "KSceneModelEditorPropertyPageMaterial.h"
class KSceneModelEditorPanelView;
class KSceneModelEditorAnimationPanel;
class KSceneModelEditorView;
class KSceneModelEditorPropertyPageAnimation;
class KSceneModelEditorFrameWnd : public MFCFramework::CChildFrame
{
	DECLARE_DYNCREATE(KSceneModelEditorFrameWnd)
protected:
	KSceneModelEditorFrameWnd();           // protected constructor used by dynamic creation

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorFrameWnd)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	void ActivateFrame(int nCmdShow);
// Implementation
protected:
	virtual ~KSceneModelEditorFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorFrameWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CSplitterWnd  m_wndSplitter;
	CSplitterWnd  m_wndSplitterLeft;

	BOOL          m_bSplitterCreated;

public:
    LRESULT OnModelChange(WPARAM, LPARAM);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	KSceneModelEditorPropertyPageMesh* GetMeshPanel();
	KSceneModelEditorPropertyPageMaterial* GetMaterialPanel();
	KSceneModelEditorPanelView* GetLeftPanel();
    KSceneModelEditorAnimationPanel* GetAnimationSeekBar();
    KSceneModelEditorView* GetModelEditorView();
    KSceneModelEditorPropertyPageAnimation* GetAnimationPanel();
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORFRAMEWND_H__18101BC6_9F72_46E2_AA65_511600371E1D__INCLUDED_)

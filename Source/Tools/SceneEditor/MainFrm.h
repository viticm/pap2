// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__A7261292_D780_4E87_959B_A2FBA2548B85__INCLUDED_)
#define AFX_MAINFRM_H__A7261292_D780_4E87_959B_A2FBA2548B85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DLG_MainTool.h"
#include "kcolordialog.h"
//#include "SceneSkinEdit.h"


//(by dengzhihui 2006年7月5日 12:05:57
#include <XTToolkitPro.h> // Xtreme Toolkit Pro component library
//)

namespace MFCFramework
{

	class CMainFrame : public CXTPMDIFrameWnd
	{
		DECLARE_DYNAMIC(CMainFrame)
	public:
		CMainFrame();

		// Attributes
	public:

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMainFrame)
	public:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
		virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
		//}}AFX_VIRTUAL

		// Implementation
	public:
		// get control bar
		CToolBar* GetToolBar() 		{	return &m_wndToolBar;		}
		CStatusBar* GetStatusBar() 	{	return &m_wndStatusBar;		}


		//CDLG_MainTool* m_pDlgMainTool;
		//KSceneSceneEdit*  m_lpSceneEdit;
		//BOOL CreateFullScreenWindow();
		virtual ~CMainFrame();
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif

	protected:  // control bar embedded members
		CXTPStatusBar  m_wndStatusBar;
		CToolBar    m_wndToolBar;
		CReBar      m_wndReBar;
		CDialogBar      m_wndDlgBar;

		
		// Generated message map functions
	protected:
		//{{AFX_MSG(CMainFrame)
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnFileSaveAs();
		afx_msg void OnFileSaveM();
		afx_msg void OnFullscreen();
		afx_msg void OnMaxView();
		afx_msg void OnSelect();
		afx_msg void OnMove();
		afx_msg void OnEditCut();
		afx_msg void OnEditPaste();
		afx_msg void OnEditCopy();
		afx_msg void OnScale();
		afx_msg void OnRotate();
		afx_msg void OnDrop();
		afx_msg void OnHeightmap();
		afx_msg void OnEditUndo();
		afx_msg void OnClose();
		afx_msg void OnDestroy();
		afx_msg void OnEditorSetting();
		afx_msg void OnSafemode();
		afx_msg void OnUpdateSafemode(CCmdUI* pCmdUI);
		//}}AFX_MSG
		//(by dengzhihui 2006年7月5日 18:54:17
		afx_msg void OnCustomize();
		afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
		afx_msg void OnActivateApp(BOOL bActive,DWORD dwThreadID);
		//)
		DECLARE_MESSAGE_MAP()
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		//(by dengzhihui 2006年7月5日 18:58:44
		virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
		int InitUI();
		void ExitFullScreen();
		//)
	public:
		afx_msg void OnViewDebugConsole();

		BOOL m_bActive;
	private:
		BOOL m_bFullScreen; //by dengzhihui 2006年7月6日 11:46:40
		BOOL m_bStatusbarShow; //记录状态显示状态
		WINDOWPLACEMENT m_wndPlace;
	};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__A7261292_D780_4E87_959B_A2FBA2548B85__INCLUDED_)

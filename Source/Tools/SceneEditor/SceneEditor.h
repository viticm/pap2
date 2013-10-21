// SceneEditor.h : main header file for the SCENEEDITOR application
//

#if !defined(AFX_SCENEEDITOR_H__94EA1C0C_62C1_40ED_B3A0_1DA2E893B8E7__INCLUDED_)
#define AFX_SCENEEDITOR_H__94EA1C0C_62C1_40ED_B3A0_1DA2E893B8E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif



#include "resource.h"       // main symbols
//#include "SceneEditorCommon.h"
/////////////////////////////////////////////////////////////////////////////
// KSceneEditorApp:
// See SceneEditor.cpp for the implementation of this class
//
#define WM_SFXEDITORCLOSE_INVOKERCALLBACK (WM_USER + 100)
class KDlgPlayerCheckTool;
class KGPhysicsParamEditorDlg;
struct IKG3DSoundShell;
namespace MFCFramework
{

class KSceneEditorApp : public CWinApp
{
public:
	KSceneEditorApp();
	virtual int ExitInstance(); // return app exit code

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual void OnAppExit();
	//}}AFX_VIRTUAL

	KGPhysicsParamEditorDlg* m_lpPhysiscSetting;

// Implementation
	//{{AFX_MSG(KSceneEditorApp)

    // 
    void NewSFXEditor(LPCTSTR szModelName, LPCTSTR szBindName);
    void NewSFXEditorToModifyBindInfo(IKG3DModel* pModel, 
        IKG3DModel* pBind, 
        int nModifyIndex, 
        CWnd *pInvoker,
        const CString& strSFXStartPath,
        DWORD dwNowTime);

    afx_msg void OnNormalMapGen();
    afx_msg void OnSomeTest();
	afx_msg void OnAppAbout();
	afx_msg void OnFileNewDataFlow(void);
	afx_msg void OnFileNew(void);
    afx_msg void OnNewPvs();
	//afx_msg void OnFileNew2(void);
	afx_msg void OnFileOpen(void);
	afx_msg void OnFileSave(void);
	afx_msg void OnFileSaveAs(void);
	afx_msg void OnReset();
	afx_msg void OnMaterialTool();
	afx_msg void OnSpeedTreeConvert();
	afx_msg void OnPlayCheckTool();
	afx_msg void OnNpcCheckTool();
    afx_msg void OnShowD3D9DebugTool();
	afx_msg void OnModelOptimize();
	afx_msg void OnBsp();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedSwordtest();
    
	
	BOOL OnIdle(LONG lCount);
	afx_msg void OnFileNewSfx();
	void OnAmbientOnly();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	KDlgPlayerCheckTool* m_pdlgCheckTool;

public:
    CView* GetActiveView();
private:
	HMODULE m_hDummperModule;
    BOOL    m_bMemoryInitFlag;
    BOOL    m_bLogInitFlag;
    BOOL    m_bGameWorldOpenFlag;
};

extern KSceneEditorApp theApp;
};

CWnd* GetModelFrameWnd();
extern const TCHAR * g_ModelAnimationKeywords[2];
extern IKG3DSoundShell *g_pSoundShell;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEEDITOR_H__94EA1C0C_62C1_40ED_B3A0_1DA2E893B8E7__INCLUDED_)

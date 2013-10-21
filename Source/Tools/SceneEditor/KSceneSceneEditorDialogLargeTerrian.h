#if !defined(AFX_KSCENESCENEEDITORDIALOGLARGETERRIAN_H__A556D868_0655_4AAD_A0AB_0C6A78411A31__INCLUDED_)
#define AFX_KSCENESCENEEDITORDIALOGLARGETERRIAN_H__A556D868_0655_4AAD_A0AB_0C6A78411A31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorDialogLargeTerrian.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogLargeTerrian dialog

class KSceneSceneEditorDialogLargeTerrian : public CDialog
{
// Construction
public:
	KSceneSceneEditorDialogLargeTerrian(CWnd* pParent = NULL);   // standard constructor

	HWND GetOutPutWnd()
	{
		return m_Static_View.GetSafeHwnd();
	}
// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorDialogLargeTerrian)
	enum { IDD = IDD_SCENEEDITOR_LARGETERRIAN };
	CButton	m_Static_View;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorDialogLargeTerrian)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorDialogLargeTerrian)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORDIALOGLARGETERRIAN_H__A556D868_0655_4AAD_A0AB_0C6A78411A31__INCLUDED_)

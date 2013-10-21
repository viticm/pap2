#if !defined(AFX_KSCENESCENEEDITORDIALOGDISTRICT_H__B024429C_EE37_418A_9BC3_624749E428CE__INCLUDED_)
#define AFX_KSCENESCENEEDITORDIALOGDISTRICT_H__B024429C_EE37_418A_9BC3_624749E428CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorDialogDistrict.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogDistrict dialog

class KSceneSceneEditorDialogDistrict : public CDialog
{
// Construction
public:
	KSceneSceneEditorDialogDistrict(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorDialogDistrict)
	enum { IDD = IDD_SCENEEDITOR_DISTRICT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorDialogDistrict)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorDialogDistrict)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORDIALOGDISTRICT_H__B024429C_EE37_418A_9BC3_624749E428CE__INCLUDED_)

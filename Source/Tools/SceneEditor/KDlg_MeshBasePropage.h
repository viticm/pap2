#if !defined(AFX_KDLG_MESHBASEPROPAGE_H__3768E5A0_E627_44B5_8F79_81B2DBD0C5A3__INCLUDED_)
#define AFX_KDLG_MESHBASEPROPAGE_H__3768E5A0_E627_44B5_8F79_81B2DBD0C5A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_MeshBasePropage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_MeshBasePropage dialog

class KDlg_MeshBasePropage : public CPropertyPage
{
	DECLARE_DYNCREATE(KDlg_MeshBasePropage)

// Construction
public:
	KDlg_MeshBasePropage();
	~KDlg_MeshBasePropage();

// Dialog Data
	//{{AFX_DATA(KDlg_MeshBasePropage)
	enum { IDD = IDD_PROPAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KDlg_MeshBasePropage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(KDlg_MeshBasePropage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_MESHBASEPROPAGE_H__3768E5A0_E627_44B5_8F79_81B2DBD0C5A3__INCLUDED_)

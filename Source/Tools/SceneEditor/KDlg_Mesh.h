#if !defined(AFX_KDLG_MESH_H__BB31E3E2_EE16_4913_9C68_49F32AAD18D4__INCLUDED_)
#define AFX_KDLG_MESH_H__BB31E3E2_EE16_4913_9C68_49F32AAD18D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_Mesh.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KDlg_Mesh

class KDlg_Mesh : public CPropertySheet
{
	DECLARE_DYNAMIC(KDlg_Mesh)

// Construction
public:
	KDlg_Mesh(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	KDlg_Mesh(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_Mesh)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~KDlg_Mesh();

	// Generated message map functions
protected:
	//{{AFX_MSG(KDlg_Mesh)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_MESH_H__BB31E3E2_EE16_4913_9C68_49F32AAD18D4__INCLUDED_)

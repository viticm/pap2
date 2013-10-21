#if !defined(AFX_KSCENEOBJECTFRAME_H__93400D32_D1C8_4B4E_8C7B_C57D2C0C033C__INCLUDED_)
#define AFX_KSCENEOBJECTFRAME_H__93400D32_D1C8_4B4E_8C7B_C57D2C0C033C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneObjectFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectFrame form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class KSceneObjectFrame : public CFormView
{
protected:
	KSceneObjectFrame();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(KSceneObjectFrame)

// Form Data
public:
	//{{AFX_DATA(KSceneObjectFrame)
	enum { IDD = IDD_PROPAGE1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneObjectFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~KSceneObjectFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(KSceneObjectFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEOBJECTFRAME_H__93400D32_D1C8_4B4E_8C7B_C57D2C0C033C__INCLUDED_)

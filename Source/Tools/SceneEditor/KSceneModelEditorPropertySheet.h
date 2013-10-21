#if !defined(AFX_KSCENEMODELEDITORPROPERTYSHEET_H__1B02D9ED_9A75_4D35_8E90_5B651B5976F8__INCLUDED_)
#define AFX_KSCENEMODELEDITORPROPERTYSHEET_H__1B02D9ED_9A75_4D35_8E90_5B651B5976F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorPropertySheet.h : header file
//
#include "KSceneModelEditorPropertyPageMesh.h"
#include "KSceneModelEditorPropertyPageMaterial.h"
#include "KSceneModelEditorPropertyPageAnimation.h"
#include "KGModelEditorPaneMisc.h"


/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPropertySheet

class KSceneModelEditorPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(KSceneModelEditorPropertySheet)

// Construction
public:
	KSceneModelEditorPropertySheet( CWnd* pParentWnd = NULL);

// Attributes
public:
	KSceneModelEditorPropertyPageMesh           m_PropertyPageMesh;
	KSceneModelEditorPropertyPageMaterial       m_PropertyPageMaterial;
	KSceneModelEditorPropertyPageAnimation      m_PropertyPageAnimation;
    KGModelEditorPaneMisc                       m_PropertyPageMisc;

// Operations
public:
	KSceneModelEditorPropertySheet*  m_pPropertySheet;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~KSceneModelEditorPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(KSceneModelEditorPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORPROPERTYSHEET_H__1B02D9ED_9A75_4D35_8E90_5B651B5976F8__INCLUDED_)

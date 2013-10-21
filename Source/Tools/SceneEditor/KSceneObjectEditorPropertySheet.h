#if !defined(AFX_KSCENEOBJECTEDITORPROPERTYSHEET_H__025FE715_105E_414B_B420_F32A4415F8DF__INCLUDED_)
#define AFX_KSCENEOBJECTEDITORPROPERTYSHEET_H__025FE715_105E_414B_B420_F32A4415F8DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneObjectEditorPropertySheet.h : header file
//
#include "KSceneObjectEditorPageModel.h"
#include "KSceneObjectEditorPropertyPageBlock.h"
#include "KGObjectManagerSheet.h"
/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPropertySheet
class KSceneObjectEditorDoc;

class KSceneObjectEditorPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(KSceneObjectEditorPropertySheet)

// Construction
public:
	KSceneObjectEditorPropertySheet(CWnd* pParentWnd = NULL);

// Attributes
public:

// Operations
public:
    KSceneObjectEditorDoc*  m_lpSceneObjectEditorDoc;

	void SetSceneObjectEditorDoc(KSceneObjectEditorDoc* pScene)
	{
	//	m_lpSceneObjectEditorDoc = pScene;
	//	m_PropertyPageModel.SetSceneObjectEditorDoc(pScene);
    //  m_ObjectManagerSheet.SetSceneObjectEditorDoc(pScene);
	}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneObjectEditorPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~KSceneObjectEditorPropertySheet();
	
	//KSceneObjectEditorPageModel          m_PropertyPageModel;
	//KSceneObjectEditorPropertyPageBlock  m_PropertyPageBlock;
    //KGObjectManagerSheet                 m_ObjectManagerSheet;
    

	// Generated message map functions
protected:
	//{{AFX_MSG(KSceneObjectEditorPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEOBJECTEDITORPROPERTYSHEET_H__025FE715_105E_414B_B420_F32A4415F8DF__INCLUDED_)

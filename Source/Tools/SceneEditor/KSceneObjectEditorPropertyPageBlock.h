#if !defined(AFX_KSCENEOBJECTEDITORPROPERTYPAGEBLOCK_H__AB14F97D_1110_474F_A06C_A3F7563CFB67__INCLUDED_)
#define AFX_KSCENEOBJECTEDITORPROPERTYPAGEBLOCK_H__AB14F97D_1110_474F_A06C_A3F7563CFB67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneObjectEditorPropertyPageBlock.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneObjectEditorPropertyPageBlock dialog

class KSceneObjectEditorPropertyPageBlock : public CPropertyPage
{
	DECLARE_DYNCREATE(KSceneObjectEditorPropertyPageBlock)

// Construction
public:
	KSceneObjectEditorPropertyPageBlock();
	~KSceneObjectEditorPropertyPageBlock();

// Dialog Data
	//{{AFX_DATA(KSceneObjectEditorPropertyPageBlock)
	enum { IDD = IDD_OBJECTEDITOR_BLOCK };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KSceneObjectEditorPropertyPageBlock)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(KSceneObjectEditorPropertyPageBlock)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEOBJECTEDITORPROPERTYPAGEBLOCK_H__AB14F97D_1110_474F_A06C_A3F7563CFB67__INCLUDED_)

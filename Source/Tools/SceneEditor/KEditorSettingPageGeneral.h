#if !defined(AFX_KEDITORSETTINGPAGEGENERAL_H__EEACB0B0_F9C7_4D67_BE5C_91CE6DA26A1D__INCLUDED_)
#define AFX_KEDITORSETTINGPAGEGENERAL_H__EEACB0B0_F9C7_4D67_BE5C_91CE6DA26A1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KEditorSettingPageGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageGeneral dialog

class KEditorSettingPageGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(KEditorSettingPageGeneral)

// Construction
public:
	KEditorSettingPageGeneral();
	~KEditorSettingPageGeneral();

// Dialog Data
	//{{AFX_DATA(KEditorSettingPageGeneral)
	enum { IDD = IDD_EDITORSETTING_GENERAL };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KEditorSettingPageGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(KEditorSettingPageGeneral)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEDITORSETTINGPAGEGENERAL_H__EEACB0B0_F9C7_4D67_BE5C_91CE6DA26A1D__INCLUDED_)

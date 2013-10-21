#if !defined(AFX_KEDITORSETTINGPAGECOLOR_H__61D69BB1_C7B3_4B3B_A811_B551CE92C2CD__INCLUDED_)
#define AFX_KEDITORSETTINGPAGECOLOR_H__61D69BB1_C7B3_4B3B_A811_B551CE92C2CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KEditorSettingPageColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageColor dialog

class KEditorSettingPageColor : public CPropertyPage
{
	DECLARE_DYNCREATE(KEditorSettingPageColor)

// Construction
public:
	KEditorSettingPageColor();
	~KEditorSettingPageColor();

// Dialog Data
	//{{AFX_DATA(KEditorSettingPageColor)
	enum { IDD = IDD_EDITORSETTING_COLOR };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KEditorSettingPageColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(KEditorSettingPageColor)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEDITORSETTINGPAGECOLOR_H__61D69BB1_C7B3_4B3B_A811_B551CE92C2CD__INCLUDED_)

#if !defined(AFX_KEDITORSETTINGPAGEVIEW_H__B2133785_D15E_4735_A29F_26DCF87C8377__INCLUDED_)
#define AFX_KEDITORSETTINGPAGEVIEW_H__B2133785_D15E_4735_A29F_26DCF87C8377__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KEditorSettingPageView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageView dialog

class KEditorSettingPageView : public CPropertyPage 
{
	DECLARE_DYNCREATE(KEditorSettingPageView)

// Construction
public:
	KEditorSettingPageView();
	~KEditorSettingPageView();

// Dialog Data
	//{{AFX_DATA(KEditorSettingPageView)
	enum { IDD = IDD_EDITORSETTING_VIEW };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KEditorSettingPageView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(KEditorSettingPageView)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEDITORSETTINGPAGEVIEW_H__B2133785_D15E_4735_A29F_26DCF87C8377__INCLUDED_)

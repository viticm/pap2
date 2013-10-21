#if !defined(AFX_KEDITORSETTINGPAGEMAPTEST_H__D5860D08_0B40_4F45_B13B_B275F343CD62__INCLUDED_)
#define AFX_KEDITORSETTINGPAGEMAPTEST_H__D5860D08_0B40_4F45_B13B_B275F343CD62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KEditorSettingPageMapTest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageMapTest dialog

class KEditorSettingPageMapTest : public CPropertyPage
{
	DECLARE_DYNCREATE(KEditorSettingPageMapTest)

// Construction
public:
	KEditorSettingPageMapTest();
	~KEditorSettingPageMapTest();

// Dialog Data
	//{{AFX_DATA(KEditorSettingPageMapTest)
	enum { IDD = IDD_EDITORSETTING_MAPTEST };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KEditorSettingPageMapTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(KEditorSettingPageMapTest)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEDITORSETTINGPAGEMAPTEST_H__D5860D08_0B40_4F45_B13B_B275F343CD62__INCLUDED_)

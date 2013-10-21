#if !defined(AFX_KSCENESWORDTESTWIZARD_H__3B234E34_E43C_4D02_B43C_78AB4F371E60__INCLUDED_)
#define AFX_KSCENESWORDTESTWIZARD_H__3B234E34_E43C_4D02_B43C_78AB4F371E60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSwordTestWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSwordTestWizard dialog

class KSceneSwordTestWizard : public CDialog
{
// Construction
public:
	int m_mTestType;
	vector<CString>m_vecFileName;

	KSceneSwordTestWizard(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneSwordTestWizard)
	enum { IDD = IDD_TESTSCENEWIZARD };
	CListBox	m_ListBoxFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSwordTestWizard)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KSceneSwordTestWizard)
	afx_msg void OnRadioTest1();
	afx_msg void OnRadioTest2();
	afx_msg void OnRadioTest3();
	afx_msg void OnRadioTest4();
	afx_msg void OnButtonFileAdd();
	afx_msg void OnButtonFileRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESWORDTESTWIZARD_H__3B234E34_E43C_4D02_B43C_78AB4F371E60__INCLUDED_)

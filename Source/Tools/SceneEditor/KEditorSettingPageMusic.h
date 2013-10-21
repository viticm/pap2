#if !defined(AFX_KEDITORSETTINGPAGEMUSIC_H__74B6FBFE_5A84_4566_92C4_13C76E50A2F2__INCLUDED_)
#define AFX_KEDITORSETTINGPAGEMUSIC_H__74B6FBFE_5A84_4566_92C4_13C76E50A2F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KEditorSettingPageMusic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageMusic dialog

class KEditorSettingPageMusic : public CPropertyPage
{
	DECLARE_DYNCREATE(KEditorSettingPageMusic)

// Construction
public:
	KEditorSettingPageMusic();
	~KEditorSettingPageMusic();

// Dialog Data
	//{{AFX_DATA(KEditorSettingPageMusic)
	enum { IDD = IDD_EDITORSETTING_MUSIC };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KEditorSettingPageMusic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(KEditorSettingPageMusic)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEDITORSETTINGPAGEMUSIC_H__74B6FBFE_5A84_4566_92C4_13C76E50A2F2__INCLUDED_)

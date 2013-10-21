#if !defined(AFX_KEDITORSETTINGPAGEVIDEO_H__6987D421_944E_4470_B7BF_705966FB1189__INCLUDED_)
#define AFX_KEDITORSETTINGPAGEVIDEO_H__6987D421_944E_4470_B7BF_705966FB1189__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KEditorSettingPageVideo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingPageVideo dialog

class KEditorSettingPageVideo : public CPropertyPage
{
	DECLARE_DYNCREATE(KEditorSettingPageVideo)

// Construction
public:
	KEditorSettingPageVideo();
	~KEditorSettingPageVideo();

// Dialog Data
	//{{AFX_DATA(KEditorSettingPageVideo)
	enum { IDD = IDD_EDITORSETTING_VIDEO };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KEditorSettingPageVideo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(KEditorSettingPageVideo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEDITORSETTINGPAGEVIDEO_H__6987D421_944E_4470_B7BF_705966FB1189__INCLUDED_)

#if !defined(AFX_KEDITORSETTINGDIALOG_H__BE1045F6_F687_4B33_8347_522C8A776B0F__INCLUDED_)
#define AFX_KEDITORSETTINGDIALOG_H__BE1045F6_F687_4B33_8347_522C8A776B0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KEditorSettingDialog.h : header file
//

#include "KEditorSettingPageColor.h"
#include "KEditorSettingPageGeneral.h"
#include "KEditorSettingPageMapTest.h"
#include "KEditorSettingPageMusic.h"
#include "KEditorSettingPageVideo.h"
#include "KEditorSettingPageView.h"
#include "KGCameraSettingDlg.h"

/////////////////////////////////////////////////////////////////////////////
// KEditorSettingDialog dialog

class KEditorSettingDialog : public CDialog
{
// Construction
public:
	KEditorSettingDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KEditorSettingDialog)
	enum { IDD = IDD_EDITORSETTING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KEditorSettingDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertySheet            m_PropertySheet;
	KEditorSettingPageGeneral m_PropertyPageGeneral;
	KEditorSettingPageView    m_PropertyPageView;
	KEditorSettingPageColor   m_PropertyPageColor;
	KEditorSettingPageMapTest m_PropertyPageMapTest;
	KEditorSettingPageVideo   m_PropertyPageVideo;
	KEditorSettingPageMusic   m_PropertyPageMusic;
	KGCameraSettingDlg		  m_cameraSetting;

	// Generated message map functions
	//{{AFX_MSG(KEditorSettingDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEDITORSETTINGDIALOG_H__BE1045F6_F687_4B33_8347_522C8A776B0F__INCLUDED_)

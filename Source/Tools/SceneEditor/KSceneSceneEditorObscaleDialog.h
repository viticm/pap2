#if !defined(AFX_KSCENESCENEEDITOROBSCALEDIALOG_H__07BBEE68_B41B_46C8_A002_DC4A12EBA256__INCLUDED_)
#define AFX_KSCENESCENEEDITOROBSCALEDIALOG_H__07BBEE68_B41B_46C8_A002_DC4A12EBA256__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorObscaleDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorObscaleDialog dialog
class KSceneSceneEdit;

class KSceneSceneEditorObscaleDialog : public CDialog
{
// Construction
public:
	KSceneSceneEditorObscaleDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorObscaleDialog)
	enum { IDD = IDD_SCENEEDITOR_OBSCALE };
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorObscaleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorObscaleDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	KSceneSceneEdit*  m_lpSceneSceneEditor;
public:
	
	void SetSceneEditor(KSceneSceneEdit* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}
	void FillList();

	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck11();
	afx_msg void OnBnClickedCheck12();
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITOROBSCALEDIALOG_H__07BBEE68_B41B_46C8_A002_DC4A12EBA256__INCLUDED_)

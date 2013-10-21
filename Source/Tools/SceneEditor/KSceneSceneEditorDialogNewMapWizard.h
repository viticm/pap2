#if !defined(AFX_KSCENESCENEEDITORDIALOGNEWMAPWIZARD_H__499C82BF_F9E4_4A40_9505_B8505DA38A47__INCLUDED_)
#define AFX_KSCENESCENEEDITORDIALOGNEWMAPWIZARD_H__499C82BF_F9E4_4A40_9505_B8505DA38A47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorDialogNewMapWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogNewMapWizard dialog

class KSceneSceneEditorDialogNewMapWizard : public CDialog
{
// Construction
public:
	KSceneSceneEditorDialogNewMapWizard(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorDialogNewMapWizard)
	enum { IDD = IDD_SCENEEDITOR_NEWMAP };
	CListCtrl	m_ListCtrlTexture;
	CComboBox	m_ComboWidth;
	CComboBox	m_ComboHeight;
	CEdit		m_EditMapID;
	CString	m_CStringMapSize;
	CString	m_CStringMapDescribe;
	CString	m_CString_DefaultTexture;
	float	m_fGroundHeight;
	CString	m_CString_IniFile;
	//}}AFX_DATA

	DWORD m_dwWidth;
	DWORD m_dwHeight;

protected:
	//KSceneSceneEdit*  m_lpSceneSceneEditor;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorDialogNewMapWizard)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillGroundTextureList();
	void FillGroundSetCombo();

	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorDialogNewMapWizard)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboMapHeight();
	afx_msg void OnSelchangeComboMapWidth();
	afx_msg void OnClickListTexture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditGroundHeight();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//void SetSceneEditor(KSceneSceneEdit* pSceneEdit);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonBrownini();
	afx_msg void OnBnClickedButtonNewini();
	afx_msg void OnLvnItemchangedListTexture(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORDIALOGNEWMAPWIZARD_H__499C82BF_F9E4_4A40_9505_B8505DA38A47__INCLUDED_)

#if !defined(AFX_KSCENESCENEEDITORNPCPROPERTIESDIALOG_H__363402FD_9110_4DD5_BB7C_A998A7A84EFD__INCLUDED_)
#define AFX_KSCENESCENEEDITORNPCPROPERTIESDIALOG_H__363402FD_9110_4DD5_BB7C_A998A7A84EFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorNpcPropertiesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorNpcPropertiesDialog dialog

class KSceneSceneEditorNpcPropertiesDialog : public CDialog
{
// Construction
public:
	/*void SetSceneEditor(KSceneSceneEdit* pEdit)
	{
		m_lpSceneEditor = pEdit;
	}*/

	KSceneSceneEditorNpcPropertiesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorNpcPropertiesDialog)
	enum { IDD = IDD_NPCPRO };
	CEdit	m_Edit_ScriptFile;
	CStatic	m_Static_Tip;
	CListBox	m_List_Script;
	CEdit	m_Edit_Name;
	CEdit	m_Edit_Level;
	CEdit	m_Edit_Kind;
	CComboBox	m_ComboBox_TemplateID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorNpcPropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillComBo();

	//KRepNPC*  GetFirstSelectedNPC();

	/*KSceneSceneEdit* m_lpSceneEditor;*/
	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorNpcPropertiesDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonSetfile();
	afx_msg void OnEnChangeEditName();
	afx_msg void OnEnChangeEditKind();
	afx_msg void OnEnChangeEditLevel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORNPCPROPERTIESDIALOG_H__363402FD_9110_4DD5_BB7C_A998A7A84EFD__INCLUDED_)

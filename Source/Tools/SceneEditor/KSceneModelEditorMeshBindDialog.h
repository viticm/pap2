#if !defined(AFX_KSCENEMODELEDITORMESHBINDDIALOG_H__0087668A_E931_4F6B_BED3_6EB8BFF9D39D__INCLUDED_)
#define AFX_KSCENEMODELEDITORMESHBINDDIALOG_H__0087668A_E931_4F6B_BED3_6EB8BFF9D39D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorMeshBindDialog.h : header file
//
//#include "kg3dscenemodeleditor.h"
interface IEKG3DModel;
/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorMeshBindDialog dialog

class KSceneModelEditorMeshBindDialog : public CDialog
{
// Construction
public:
	KSceneModelEditorMeshBindDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneModelEditorMeshBindDialog)
	enum { IDD = IDD_MODELEDITOR_MESHBIND };
	CListBox	m_ListBoxModel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorMeshBindDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL               m_bSelectChild;//是根据插槽选择绑定的物体，还是根据物体选择父物体
	IEKG3DModel*         m_pModel;
    LPCTSTR            m_pszSocketName;
	IEKG3DSceneModelEditor*   m_pEditor;
	//KModel*            m_lpModel;
	//KModel::LPSOCKET   m_lpSocket;
	//KSceneModelEditor* m_lpModelEditor;

	void FillEntityListBox();
	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorMeshBindDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetSocket(IEKG3DSceneModelEditor* pEditor, IEKG3DModel* pModel, LPCTSTR pszSocketName);
	void SetChildModel(IEKG3DSceneModelEditor* pEditor, IEKG3DModel* pModel);
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORMESHBINDDIALOG_H__0087668A_E931_4F6B_BED3_6EB8BFF9D39D__INCLUDED_)

#include "afxcmn.h"
#include "afxwin.h"
#if !defined(AFX_KSCENESCENEEDITORDIALOGOBJECTMGR_H__28F5448B_5B1D_4A25_9EF2_E4178E8B6650__INCLUDED_)
#define AFX_KSCENESCENEEDITORDIALOGOBJECTMGR_H__28F5448B_5B1D_4A25_9EF2_E4178E8B6650__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorDialogObjectMgr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogObjectMgr dialog
interface IEKG3DSceneSceneEditor;



class KSceneSceneEditorDialogObjectMgr : public CDialog
{
// Construction
public:
	KSceneSceneEditorDialogObjectMgr(CWnd* pParent = NULL);   // standard constructor
	void SetSceneEditor(IEKG3DSceneSceneEditor* lpSceneEditor)
	{
		m_lpSceneEditor = lpSceneEditor;
	}
// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorDialogObjectMgr)
	enum { IDD = IDD_SCENEEDITOR_OBJECTMGR };
	CTreeCtrl	m_TreeGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorDialogObjectMgr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nOutputWindowID;
	void FillTreeGroup();
	//void FillListSingleMesh();

	HRESULT FillTree();

	HRESULT FillTreeWithDir(LPSTR Dir,LPSTR pFileType,HTREEITEM hStart);


	IEKG3DSceneSceneEditor* m_lpSceneEditor;

	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorDialogObjectMgr)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void GetItemFilePath(HTREEITEM hItem, TCHAR strFilePath[]);
	afx_msg void OnBnClickedButtonAutocheck();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CTreeCtrl m_Tree;
	afx_msg void OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_outputWnd;
	HTREEITEM m_hLastItem;

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMClickTree2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton_ReplaceSelectedWithObjectInhandle();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORDIALOGOBJECTMGR_H__28F5448B_5B1D_4A25_9EF2_E4178E8B6650__INCLUDED_)

#if !defined(AFX_KSCENEEDITBASESELECTDIALOG_H__71DAFB04_DD4C_47F6_B440_E4B92EC7A356__INCLUDED_)
#define AFX_KSCENEEDITBASESELECTDIALOG_H__71DAFB04_DD4C_47F6_B440_E4B92EC7A356__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneEditBaseSelectDialog.h : header file
//
//#include ".\kg3dsceneeditorbase.h"
#include "afxcmn.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// KSceneEditBaseSelectDialog dialog
interface IEKG3DSceneEditorBase;
interface IEKG3DSceneLayer;

class KSceneEditBaseSelectDialog : public CDialog
{
// Construction
public:
	KSceneEditBaseSelectDialog(CWnd* pParent = NULL);   // standard constructor
	
	void SetEditor(IEKG3DSceneEditorBase* pEditor)
	{
		m_lpSceneEditor = pEditor;
	}
// Dialog Data
	//{{AFX_DATA(KSceneEditBaseSelectDialog)
	enum { IDD = IDD_SCENCESELECTDIALOG };
	CListBox	m_ListBoxEntity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneEditBaseSelectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

	void FillEntityListBox();
	void FillLayerTree();

protected:
	void FillLayerEntity(IEKG3DSceneLayer *piLayer);

	void FillOneLayer(IEKG3DSceneLayer *piLayer, HTREEITEM hRoot);

	IEKG3DSceneEditorBase* m_lpSceneEditor;
    struct KG3DSceneEntityInfo 
    {
        SCENEENTITYTYPE Type;
        PVOID           pvPointer;
    };
	vector<KG3DSceneEntityInfo>m_vecEntityInListBox;

	// Generated message map functions
	//{{AFX_MSG(KSceneEditBaseSelectDialog)
	afx_msg void OnSelchangeList1();
	afx_msg void OnBnClickedButtonSelectNone();
	afx_msg void OnBnClickedButtonSelectInvert();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
public:
	// Í¼²ãµÄÊ÷×´ÁÐ±í
	CTreeCtrl m_TreeLayer;
public:
	afx_msg void OnTvnSelchangedTreeLayer(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButtonAddlayer();
public:
	afx_msg void OnBnClickedButtonDeletelayer();
public:
	afx_msg void OnBnClickedButtonAddentity();
public:
	afx_msg void OnBnClickedButtonFreeon();	
	afx_msg void OnBnClickedButtonHide();
public:
	afx_msg void OnLbnSelchangeListLayerentity();
public:
	CListBox m_ListBoxLayer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEEDITBASESELECTDIALOG_H__71DAFB04_DD4C_47F6_B440_E4B92EC7A356__INCLUDED_)

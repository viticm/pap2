#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// KSceneEditorLayerDialog dialog
interface IEKG3DSceneEditorBase;
interface IEKG3DSceneLayer;

class KSceneEditorLayerDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorLayerDialog)

public:
	KSceneEditorLayerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorLayerDialog();

// Dialog Data
	enum { IDD = IDD_SCENEEDITORLAYER };

	void SetEditor(IEKG3DSceneEditorBase* pEditor)
	{
		m_lpEditor = pEditor;
	}
protected:
	void FillLayerTree();
	void FillOneLayer(IEKG3DSceneLayer *piLayer, HTREEITEM hRoot);

	IEKG3DSceneEditorBase* m_lpEditor;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_TreeLayer;
public:
	afx_msg void OnBnClickedButtonNewlayer();
public:
	afx_msg void OnBnClickedButtonDeletelayer();
public:
	afx_msg void OnEnChangeEditName();
public:
	afx_msg void OnBnClickedButtonRename();
public:
	afx_msg void OnBnClickedCheckHide();
public:
	afx_msg void OnBnClickedCheckFreeze();
public:
	afx_msg void OnBnClickedButtonSelect();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	afx_msg void OnBnClickedButtonInputselected();
public:
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CEdit m_LayerName;
};

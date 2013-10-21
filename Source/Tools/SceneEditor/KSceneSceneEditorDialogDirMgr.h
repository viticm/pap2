#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class IEKG3DRepresentObject;

// KSceneSceneEditorDialogDirMgr dialog
interface IEKG3DSceneSceneEditor;



class KSceneSceneEditorDialogDirMgr : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogDirMgr)

public:
	KSceneSceneEditorDialogDirMgr(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogDirMgr();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_DIRMGR };

protected:
	int m_nOutputWindowID;
	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	IEKG3DRepresentObjectSet* m_lpObjectSet;
	IEKG3DRepresentObject* m_lpObject;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// //Â·¾¶
	CTreeCtrl m_Tree;
	CImageList m_imgList;
public:
	CStatic m_Static_Preview;
public:
	void RemoveItemInListFromTree(HTREEITEM hItem);

	int GetItemFullText(HTREEITEM hItem,LPSTR pName);

	HRESULT SetSceneEditor(IEKG3DSceneSceneEditor* pEditor)
	{
		m_lpSceneEditor = pEditor;
		return S_OK;
	}
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	void FillDirListBox();
	void FillSetModelListBox();
	void FillTree();
	HRESULT FillTreeWithDir(LPSTR Dir,HTREEITEM hStart);
	int GetItemImage(LPSTR extName);
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonRefresh();
public:
	afx_msg void OnTvnBegindragTree1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CListBox m_ListBox_Dir;
public:
	CListBox m_ListBox_Model;
public:
	afx_msg void OnBnClickedButtonAdddir();
public:
	afx_msg void OnBnClickedButtonRemovedir();
public:
	afx_msg void OnBnClickedButtonNewse();
public:
	afx_msg void OnBnClickedButtonAddtoset();
public:
	CStatic m_Static_SetName;
public:
	afx_msg void OnBnClickedButtonSaveset();
public:
	afx_msg void OnBnClickedButtonLoadset();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	afx_msg void OnBnClickedButtonRemovefromset();
public:
	afx_msg void OnLbnSelchangeListModels();
public:
	afx_msg void OnBnClickedButtonRemoveformtree();
    afx_msg void OnTvnItemExpanding(NMHDR *pNMHDR, LRESULT *pResult);
};



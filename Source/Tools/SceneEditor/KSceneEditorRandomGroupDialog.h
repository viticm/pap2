#pragma once
#include "afxcmn.h"


// KSceneEditorRandomGroupDialog dialog
class IEKG3DSceneSceneEditor;
class KSceneEditorDocLogical;

class KSceneEditorRandomGroupDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorRandomGroupDialog)

public:
	KSceneEditorRandomGroupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorRandomGroupDialog();
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_pDocLogical;
	DWORD m_dwSelectedGroupID;
	int   m_nCurrentSelectedNpcTempleteID;

	HRESULT SetSceneEditor(IEKG3DSceneSceneEditor* pEditor,KSceneEditorDocLogical* pDocLogical)
	{
		m_lpSceneSceneEditor = pEditor;
		m_pDocLogical = pDocLogical;
		return S_OK;
	}

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_RANDOMGROUP };
	void FillNpcTree();
	void FillRandomGroupTree();
	void UpdataGroup(DWORD dwGroupID);

	map<CString,HTREEITEM>m_mapType;

	map<DWORD,HTREEITEM>m_mapGroupItem;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTreeGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
	CTreeCtrl m_TreeGroup;
	CTreeCtrl m_TreeNpc;
	CString m_szGroupName;
	CString m_szCurrentSelectedGroup;
	afx_msg void OnBnClickedButtonAddnpc();
	afx_msg void OnBnClickedButtonRemovenpc();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonAddgroup();
	afx_msg void OnEnChangeEditGroupname();
	afx_msg void OnBnClickedButtonDeletegroup();
	afx_msg void OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeGroup(NMHDR *pNMHDR, LRESULT *pResult);
};

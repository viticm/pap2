#pragma once


// KSceneModelEditorLoadAni dialog
class KSceneModelEditorView;
class KSceneModelEditorLoadAni : public CDialog
{
	DECLARE_DYNAMIC(KSceneModelEditorLoadAni)

public:
	KSceneModelEditorLoadAni(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneModelEditorLoadAni();

// Dialog Data
	enum { IDD = IDD_LOAD_ANI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
	CListBox m_listAni;
	CTreeCtrl	m_Tree;
	CString m_strCurDir;
	CString m_strExt;
	CString m_strPathName;
	IKG3DModel *m_pModel;
public:
	void SetCurDir(const TCHAR* strDirectory,const TCHAR* strExt)
	{
		m_strCurDir = strDirectory;
		m_strExt = strExt;
	}
	CString GetPathName()
	{
		return m_strPathName;
	}
	void CreateTree();
	void CreateTypeNode(IIniFile* pIniFile,TCHAR* pTitle,HTREEITEM ParentNode);
	void FillTree();
	void ClearAniList();
	void SetModel(IKG3DModel *pModel)
	{
		m_pModel = pModel;
	}
	KSceneModelEditorView *m_pParentView;
	HTREEITEM FindTreeItembyString(CString strText, HTREEITEM hStartItem = NULL);
	

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnLbnSelchangeListAniNotype();
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	map<int, HTREEITEM> m_ID2Item;
	afx_msg void OnTvnKeydownTree1(NMHDR *pNMHDR, LRESULT *pResult);
};

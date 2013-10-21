#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "KGTreeCtrl.h"

// KSceneSceneEditorDialogDoodad dialog
interface IEKG3DSceneSceneEditor;
class KSceneEditorDocLogical;

class KSceneSceneEditorDialogDoodad : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogDoodad)

public:
	KSceneSceneEditorDialogDoodad(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogDoodad();
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit,KSceneEditorDocLogical* pDocLogical)
	{
		m_lpSceneSceneEditor = pSceneEdit;
		m_lpDocLogical = pDocLogical;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_DOODAD };

protected:
	int m_nOutputWindowID;
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_lpDocLogical;
	map<CString,HTREEITEM>m_mapType;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	HTREEITEM m_FindNode;
	DWORD m_dwDoodadTempleteID;
	DWORD m_dwDefaultReliveID;
	CString m_szDoodadName;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	KGTreeCtrl m_Tree;
public:
	afx_msg void OnBnClickedButtonNewnpc();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	CStatic m_Static_PreView;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonLookup();
	CString m_Substr;
	afx_msg void OnEnChangeEditLookup();
	afx_msg void OnBnClickedButtonRelive();
	afx_msg void OnBnClickedButtonCheckRelive();
	CButton m_Button_Relive;
	CButton m_Button_CheckRelive;

	HRESULT CreateDoodadInHandle();

	afx_msg void OnBnClickedDoodadReplaceOne();
	afx_msg void OnBnClickedDoodadReplaceAll();
	bool ReplaceSelectedDoodad(OUT vector<DWORD> &templateids);
};

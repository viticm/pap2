#pragma once
#include "afxwin.h"
#include "KGTreeCtrl.h"
interface IEKG3DSceneSceneEditor;//by dengzhihui Refactor 2006年11月27日
// KSceneSceneEditorDialogNPC dialog
class KSceneEditorDocLogical;

class KSceneSceneEditorDialogNPC : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogNPC)

	//CTreeCtrl
	KGTreeCtrl	m_Tree;

public:
	KSceneSceneEditorDialogNPC(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogNPC();
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit,KSceneEditorDocLogical* pLogical)
	{
		m_lpSceneSceneEditor = pSceneEdit;
		m_lpDocLogical = pLogical;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_NPC };

protected:
	DWORD m_dwDefaultReliveID;
	DWORD m_dwDefaultRandomID;
	DWORD m_dwDefaultAISet;
	DWORD m_dwNpcTempleteID;
	CString m_szNpcName;

	KSceneEditorDocLogical* m_lpDocLogical;

	map<CString,HTREEITEM>m_mapType;
	int m_nOutputWindowID;
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	HTREEITEM m_FindNode ; // 用于记录查找出来的节点 by huangjinshou
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	HRESULT CreateNpcInHandle();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonEditmovespline();
	afx_msg void OnBnClickedButtonEditnpc();
	afx_msg void OnBnClickedButtonNewnpc();
	afx_msg void OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	CStatic m_Static_PreView;
public:
	virtual BOOL OnInitDialog();
	//char m_Substring;
	afx_msg void OnBnClickedButtonLookup();
	CString m_SubStr;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButtonRelive();
	afx_msg void OnBnClickedButtonRandom();
	CButton m_Button_ReliveGroup;
	CButton m_Button_RandomGroup;
	afx_msg void OnBnClickedButtonAiset();
	bool ReplaceSelectedNpc(OUT vector<DWORD> &templateids);
	afx_msg void OnBnClickedNpcReplaceOne();
	afx_msg void OnBnClickedNpcReplaceAll();
};

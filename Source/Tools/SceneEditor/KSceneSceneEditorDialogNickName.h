#pragma once


#include "afxcmn.h"
#include "afxwin.h"
#include "IETools.h"

interface IEKG3DSceneSceneEditor;
class KSceneEditorDocLogical;
class KSceneEditorAIDialog;

class KSceneSceneEditorDialogNickName : 
	public CDialog,
	public IEKG3DSimpleSink
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogNickName)

public:
	KSceneSceneEditorDialogNickName(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogNickName();
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;

	}
	void SetDocLogical(KSceneEditorDocLogical* pLogical)
	{
		m_lpDocLogical = pLogical;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_NICKNAME };

	virtual void SinkFunc(INT nSinkID){}
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);

protected:
	KSceneEditorAIDialog* m_lpAIDialog;

	void FillAICombo();
	void FillCombo();
	void FillTree();

	HTREEITEM m_hNpcRoot;
	HTREEITEM m_hDoodRoot;
	map<int,HTREEITEM>m_mapNPCType;
	map<int,HTREEITEM>m_mapDoodadType;

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_lpDocLogical;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRefresh();
public:
	afx_msg void OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CTreeCtrl m_Tree;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnEnChangeEditNickname();
public:
	CEdit m_Edit_NickName;
public:
	CButton m_Button_Brow;
public:
	CButton m_Button_ClearScript;
public:
	CEdit m_Edit_Script;
public:
	afx_msg void OnBnClickedButtonBrownscript();
public:
	afx_msg void OnBnClickedButtonClearscript();
public:
	afx_msg void OnEnChangeEditScript();
public:
	CComboBox m_ComBo_Way;
public:
	afx_msg void OnCbnSelchangeComboWaypointse();
public:
	afx_msg void OnBnClickedButtonWayclear2();
public:
	afx_msg void OnCbnSelchangeComboAitype();
public:
	CComboBox m_Combo_AIType;
	BOOL m_bShowAllName;
	afx_msg void OnBnClickedCheckShowallname();
	afx_msg void OnBnClickedButtonRevivegroup();
	CButton m_Button_ReliveGroup;
	CButton m_Button_Random;
	afx_msg void OnBnClickedButtonRandomgroup();
	afx_msg void OnBnClickedButton1();
};

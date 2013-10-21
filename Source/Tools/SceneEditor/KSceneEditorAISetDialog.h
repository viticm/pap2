#pragma once
#include "afxwin.h"
#include "KSceneEditorDocLogical.h"

// KSceneEditorAISetDialog dialog
class IEKG3DSceneSceneEditor;

class KSceneEditorAISetDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorAISetDialog)

public:

	KSceneEditorAISetDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorAISetDialog();

	IEKG3DSceneSceneEditor* m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_pDocLogical;
	DWORD                   m_dwSelectedID;
	DWORD                   m_dwCurrentNpcTempleteID;
	KSceneEditorDocLogical::NPCInfo* m_pCurrentNpc;

	list<KSceneEditorDocLogical::NPCInfo*>m_listNpcToModify;


	HRESULT SetSceneEditor(IEKG3DSceneSceneEditor* pEditor,KSceneEditorDocLogical* pDocLogical,DWORD dwNpcTempleteID,KSceneEditorDocLogical::NPCInfo* pNpc)
	{
		m_lpSceneSceneEditor = pEditor;
		m_pDocLogical = pDocLogical;
		m_dwCurrentNpcTempleteID = dwNpcTempleteID;
		m_pCurrentNpc = pNpc;
		return S_OK;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_AISET };

	void ApplyModiyToNpcList(KSceneEditorDocLogical::AIParameterSet&  OldAIParameterSet,
	KSceneEditorDocLogical::AIParameterSet&  NewAIParameterSet,
	KSceneEditorDocLogical::AIParameterSet&  TemplateAIParameterSet,
	BOOL bModifyToAll = FALSE);

	KSceneEditorDocLogical::AIParameterSet  m_OldAIParameterSet;
	KSceneEditorDocLogical::AIParameterSet  m_NewAIParameterSet;
	KSceneEditorDocLogical::AIParameterSet  m_TemplateAIParameterSet;

	void ApplyModiyToNpcList(BOOL bModifyToAll = FALSE);
protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void FillList();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDelete();
	CListBox m_List;
	CString m_szName;
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButtonClearselection();
	afx_msg void OnEnChangeEditName();
	afx_msg void OnBnClickedButtonNewtemplete();
};

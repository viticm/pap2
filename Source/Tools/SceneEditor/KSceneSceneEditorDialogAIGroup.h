#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "IETools.h"
#include "IEEditor.h"
// KSceneSceneEditorDialogAIGroup dialog
class KG3DSceneSceneEditor;
class KSceneEditorDocLogical;

class KSceneSceneEditorDialogAIGroup : 
	public CDialog,
	public IEKG3DSimpleSink
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogAIGroup)

public:

	virtual void SinkFunc(INT nSinkID){}
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);
	KSceneSceneEditorDialogAIGroup(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogAIGroup();
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}
	void SetDocLogical(KSceneEditorDocLogical* pLogical)
	{
		m_lpDocLogical = pLogical;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_AIGROUP };

protected:
	void FillCombo();
	void FillTree();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorDocLogical* m_lpDocLogical;

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
public:
	afx_msg void OnBnClickedButtonRefresh();
public:
	afx_msg void OnBnClickedButtonAdd();
public:
	afx_msg void OnBnClickedButtonRemove();
public:
	afx_msg void OnBnClickedButtonFit();
public:
	afx_msg void OnBnClickedCheckKing();
public:
	afx_msg void OnBnClickedButtonAddset();
public:
	afx_msg void OnBnClickedButtonRemoveset();
public:
	CEdit m_Edit_GroupName;
public:
	afx_msg void OnEnChangeEditGroupname();
public:
	CButton m_Check_King;
public:
	CComboBox m_Combo_Way;
public:
	CButton m_Button_ArrayBrown;
public:
	CButton m_Button_ArrayClear;
public:
	CEdit m_Edit_ArrayFile;
public:
	afx_msg void OnTvnSelchangedTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnCbnSelchangeComboWaypointse();
public:
	afx_msg void OnNMDblclkTreeNpc(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButtonBrown();
public:
	afx_msg void OnBnClickedButtonClear();
public:
	afx_msg void OnEnChangeEditMatrixfile();
};

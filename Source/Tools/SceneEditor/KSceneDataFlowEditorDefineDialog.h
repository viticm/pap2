#pragma once
#include "afxcmn.h"
#include "afxwin.h"
class IEKG3DSceneDataFlowEditor;

// KSceneDataFlowEditorDefineDialog dialog

class KSceneDataFlowEditorDefineDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneDataFlowEditorDefineDialog)

public:
	KSceneDataFlowEditorDefineDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneDataFlowEditorDefineDialog();

// Dialog Data
	enum { IDD = IDD_DATAFLOWEDITOR_DEFINE };

	HRESULT SetEditor(IEKG3DSceneDataFlowEditor* pEditor)
	{
		m_lpEditor = pEditor;
		return S_OK;
	}

	UINT m_uCurrentSelectedDefine;
	int  m_nCurrentSelectedVariable;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	IEKG3DSceneDataFlowEditor* m_lpEditor;

	HRESULT FillListDefine();
	HRESULT FillListVariable();
	HRESULT FillComboVariable();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedListDefine(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonAdddefine();
	afx_msg void OnBnClickedButtonDeletedefine();
	afx_msg void OnEnChangeEditDefinename();
	afx_msg void OnEnChangeEditNumvariable();
	afx_msg void OnLbnSelchangeListVariable();
	afx_msg void OnEnChangeEditVariablename();
	afx_msg void OnCbnSelchangeComboVariabletype();
	CListCtrl m_ListDefine;
	CString m_szDefineName;
	DWORD m_dwNumVariable;
	CListBox m_ListVariable;
	CString m_szVariableName;
	CComboBox m_ComboVariableType;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSetnumvariable();
};

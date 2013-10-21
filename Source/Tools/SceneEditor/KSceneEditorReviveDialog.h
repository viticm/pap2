#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// KSceneEditorReviveDialog dialog
class KSceneEditorDocLogical;

class KSceneEditorReviveDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorReviveDialog)

	BOOL m_bNpcReliveGroup;
	KSceneEditorDocLogical* m_pDocLogical;

public:
	HRESULT SetDocLogical(KSceneEditorDocLogical* pLogical,BOOL bNpc)
	{
		m_pDocLogical = pLogical;
		m_bNpcReliveGroup = bNpc;
		return S_OK;
	}
	void SetCurSelectedID(DWORD dwSelectedID);
	KSceneEditorReviveDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorReviveDialog();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_REVIVEGROUP };

	DWORD m_dwCurrentSelectedID;
protected:
	HRESULT FillList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	HRESULT ModifyGroup();

	DECLARE_MESSAGE_MAP()
public:
	CString m_szGroupName;
	int m_nMin;
	int m_nMax;
	CButton m_Random;
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDelete();
	virtual BOOL OnInitDialog();
	CListCtrl m_List;
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditGroupname();
	afx_msg void OnEnChangeEditMin();
	afx_msg void OnEnChangeEditMax();
	CString m_szSelectedTip;
	afx_msg void OnBnClickedButtonClearselect();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCheckIsRandom();
};

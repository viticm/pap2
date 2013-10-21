#pragma once
#include "afxwin.h"

class KSceneEditorDoc;
class IEKG3DSceneSceneEditor;

// KSceneSceneEditorDialogPoly dialog
class KSceneSceneEditorDialogPoly : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogPoly)

public:
	KSceneSceneEditorDialogPoly(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogPoly();

	KSceneEditorDoc* m_lpDoc;
	IEKG3DSceneSceneEditor* m_lpEditor;

	HRESULT SetEditor(KSceneEditorDoc* pDoc,IEKG3DSceneSceneEditor* pEditor)
	{
		m_lpDoc = pDoc;
		m_lpEditor = pEditor;
		return S_OK;
	}
// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_POLY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void FillList();

	DECLARE_MESSAGE_MAP()
protected:
	float m_fHeight;
	int   m_nVergeNum;
	float m_fDegree;
	float m_fRadii;
private:
	CListBox m_List;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedNewpoly();
	afx_msg void OnBnClickedEnd();
	afx_msg void OnEnChangeEditHeight();
	afx_msg void OnBnClickedShowpoly();
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnBnClickedBuildPoly();
	afx_msg void OnEnChangeEditVergenum();
	afx_msg void OnEnChangeEditDegree();
	afx_msg void OnEnChangeEditRadii();
};



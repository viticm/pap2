#pragma once

#include "KSceneModelEditorPropertySheet.h"
#include "KG3DModelSetEdit.h"


class KSceneModelEditorPanelView : public CFormView
{
protected:
	KSceneModelEditorPanelView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(KSceneModelEditorPanelView)

// Form Data
public:
	//{{AFX_DATA(KSceneModelEditorPanelView)
	enum { IDD = IDD_MODELEDITORMAINPANEL };
	CString	m_scModelName;
	//}}AFX_DATA

// Attributes
public:
	KSceneModelEditorPropertySheet*  m_pPropertySheet;
	KSceneModelEditorPropertyPageMesh* GetMeshPanel();
	KSceneModelEditorPropertyPageMaterial* GetMaterialPanel();
    KSceneModelEditorPropertyPageAnimation* GetAnimationPanel();
// Operations 
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorPanelView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~KSceneModelEditorPanelView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorPanelView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnCbnSelchangeComboModelpart();
	void UpdateModelPartList();
	int GetCurrentPartIndex();
    int SetCurrentPartIndex(LPTSTR pszPartName, int nIndex);
	CComboBox m_comboPartList;
	KG3DModelSetEdit m_ModelSetEdit;
};



#pragma once
#include "afxwin.h"


// KSceneEditorVisibleSetDialog dialog
interface IEKG3DSceneSceneEditor;
class IEKG3DRepresentObjectVisibleEntitySet;

class KSceneEditorVisibleSetDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorVisibleSetDialog)

	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	IEKG3DRepresentObjectVisibleEntitySet* m_lpVisibleSet;
public:
	void SetVisibleEntitySet(IEKG3DSceneSceneEditor* pEditor,IEKG3DRepresentObjectVisibleEntitySet* pSet)
	{
		m_lpSceneEditor = pEditor;
		m_lpVisibleSet = pSet;
	}

	KSceneEditorVisibleSetDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorVisibleSetDialog();

// Dialog Data
	enum { IDD = IDD_SCENEEDITORVISIBLESET };

protected:
	void FillDepthCombo();
	void FillLayerCombo(CComboBox* pCombo,DWORD dwCurID);

	void GetVisibleEntitySetInfo();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonModelname();
public:
	CButton m_ButtonName;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	afx_msg void OnCbnSelchangeComboInlayer();
public:
	CComboBox m_Combo_InLayer;
public:
	CComboBox m_Combo_OutLayer;
public:
	afx_msg void OnCbnSelchangeComboOutlayer();
public:
	CComboBox m_Combo_Depth;
public:
	afx_msg void OnCbnSelchangeCombo2();
};



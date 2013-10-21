#pragma once

#include "IEEditor.h"
#include "IEKG3DTerrain.h"
#include "KStaticBitmapSectionSelector.h"
// KSceneEditorDialogAutoGrass dialog

class KSceneEditorDialogAutoGrass : public CDialog
{
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	IEKG3DSceneOutDoorSpaceMgr* m_lpOutDoorSpaceMgr;
public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit , IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
	{
		m_lpSceneSceneEditor = pSceneEdit;
		m_lpOutDoorSpaceMgr = pOutDoorMgr;
	}
	DECLARE_DYNAMIC(KSceneEditorDialogAutoGrass)

public:
	KSceneEditorDialogAutoGrass(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorDialogAutoGrass();

// Dialog Data
	enum { IDD = IDD_AUTO_GRASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	int m_nWidhtCount;
	int m_nHeightCount;

	float	m_fWidthMin;
	float	m_fWidthMax;
	float	m_fHeightMin;
	float	m_fHeightMax; 
//	int m_nIndexMin;
//	int m_nIndexMax;
	BYTE m_byDetialTexIndex;
	BYTE* m_pbyGrassTexIndex;
	BYTE* m_pbyPatternIndex;
	void SetDetialTexData(BYTE byIndex,BYTE* byGrassTexIndex,BYTE* byPatternIndex);
	void ClearSelected();
	//////////////////////////////
	CListBox  m_ListBoxPattern;
	CComboBox m_ComboxBoxTexture;
	CStatic   m_StaticPreview;
	int m_nLastSel;
	KStaticBitmapSectionSelector m_Selector;
	int m_nGrassDensity;
	TCHAR m_GrassTexturePath[MAX_PATH];
	void LoadTextureSettings();
	void SaveTextureSettings();
	//afx_msg void OnStnClickedStaticPreview();
	
	void UpdateTextureInfo(BOOL bFrameToData = TRUE);
	void SetComboBoxTexture();
public:
	afx_msg void OnBnClickedBunRefreshtexture();
	afx_msg void OnBnClickedBunRefresh();
	afx_msg void OnCbnSelchangeComboTexture();
	afx_msg void OnStnClickedPreview();
	afx_msg void OnLbnSelchangeListPattern();
	afx_msg void OnEnChangeEditDensity();
	afx_msg void OnBnClickedDelGrass();
	afx_msg void OnBnClickedBulidGrass();
	afx_msg void OnBnClickedButtonDelgrass();
	afx_msg void OnEnKillfocusEditWidthmin();
	afx_msg void OnEnKillfocusEditWidthmax();
	afx_msg void OnEnKillfocusEditHeightmin();
	afx_msg void OnEnKillfocusEditHeightmax();
	afx_msg void OnBnClickedButtonAddgrass();
	afx_msg void OnEnChangeEditSplith();
	afx_msg void OnEnChangeEditSplitv();
};

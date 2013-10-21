#pragma once
#include "IEEditor.h"
#include "IEKG3DTerrain.h"
#include "KStaticBitmapSectionSelector.h"
#include "KGLilisPanel.h"
// KSceneSceneEditorDialogGrass dialog

interface IEKG3DSceneSceneEditor;

class KSceneSceneEditorDialogGrass : public KGLilisPanelDLG
{

	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
		return;
	}

	DECLARE_DYNAMIC(KSceneSceneEditorDialogGrass)

public:

	KSceneSceneEditorDialogGrass(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogGrass();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_GRASS };  
	float	m_fWidthMin;
	float	m_fWidthMax;
	float	m_fHeightMin;
	float	m_fHeightMax;  
	float	m_fNumPerM2;//每平方米多少科
	int m_nAlpha;
	
    CListBox  m_ListBoxGrassPattern;
    CComboBox m_ComboxBoxGrassTexture;
    CStatic   m_StaticPreview;
	
	DWORD m_Radio;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedButtonRandommap();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditWidthmax();
	afx_msg void OnEnChangeEditWidthmin();
	afx_msg void OnEnChangeEditHeightmax();
	afx_msg void OnEnChangeEditHeightmin();
	afx_msg void OnBnClickedCheckGrassadd();
	afx_msg void OnBnClickedCheckGrassremove();
	afx_msg void OnEnChangeEditNumber();
	afx_msg void OnBnClickedButtonOpengrasstexture();
	afx_msg void OnBnClickedButtonAutoLight();
	int m_nWidhtCount;
	int m_nHeightCount;
	afx_msg void OnEnChangeEditHsplit();
	afx_msg void OnEnChangeEditVsplit();
	afx_msg void OnEnChangeEditTexturemin();
	afx_msg void OnEnChangeEditTexturemax();
	int m_nIndexMin;
	int m_nIndexMax;
	afx_msg void OnBnClickedButtonFitground();
	void CheckButtonEnable();
private:
	afx_msg void OnBnClickedButtonRefresh();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ChangeDefault(LPCTSTR WidthMin ,LPCTSTR WidthMax,LPCTSTR HeightMin,LPCTSTR HeightMax,LPCTSTR NumPerM2);
	void UpdateTextureInfo(BOOL bFrameToData = TRUE);
	void OnBnClickedButtonRefreshTexture();
	void OnCbnSelchangeComboGrasstexture();
	void OnLbnSelChangeListPattern();
	void LoadTextureSettings();
	void SaveTextureSettings();
	int m_nLastSel;
	KStaticBitmapSectionSelector m_Selector;
	afx_msg void OnStnClickedStaticPreview();
public:
    afx_msg void OnBnClickedStatic4();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadioGrass();
	afx_msg void OnBnClickedRadioStone();

	afx_msg void OnBnClickedRadioFrutex();
	afx_msg void OnBnClickedCheckAlphablend();
	afx_msg void OnBnClickedButtonDelAll();
};

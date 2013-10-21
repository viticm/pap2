#pragma once
#include "afxwin.h"
#include "KTgaButton.h"
#include "KStaticBitmapSectionSelector.h"
interface IEKG3DSceneSceneEditor;
interface IEKG3DSceneOutDoorSpaceMgr;

// KSceneEditorDetialMapDialog dialog

class KSceneEditorDetialMapDialog : public CDialog
{
	DECLARE_DYNAMIC(KSceneEditorDetialMapDialog)

public:
	void SetPotinerToTgaButton();
	HRESULT FillMtl();
	IEKG3DSceneOutDoorSpaceMgr* m_lpSceneOutDoorSpaceMgr;
	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	HRESULT SetEditor(IEKG3DSceneSceneEditor* pEditor,IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
	{
		m_lpSceneEditor = pEditor;
		m_lpSceneOutDoorSpaceMgr = pOutDoorSpaceMgr;
		return S_OK;
	}

	KSceneEditorDetialMapDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneEditorDetialMapDialog();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_DETAILMTL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	std::string m_strCurTexName;
	float m_fScaleU;
	float m_fScaleV;
	BYTE m_byCurTexIndex;
	BOOL m_bAssociateGrass;//ÊÇ·ñ¹ØÁª²Ý
	BYTE m_byGrassTexIndex[8];
	BYTE m_byPatternIndex[8];
	CListCtrl m_ListFilterMap;
public:
	KTgaButton  m_firstLayer;
	KTgaButton	m_BrushMap[10];
	CStatic   m_StaticPreview;
	KStaticBitmapSectionSelector m_Selector;
	CListBox m_ListBox_Text;
	vector<BYTE> m_vecTexIndex;
	BYTE m_byTexCoordIndex;
	void UpdateTexCoord();
	afx_msg void OnLbnSelchangeListText();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonAddtexture();
	afx_msg void OnBnClickedButtonDeltexture();
	void LoadInfo();
	afx_msg void OnBnClickedRadioXz();
	afx_msg void OnBnClickedRadioXy();
	afx_msg void OnBnClickedRadioYz();
	afx_msg void OnBnClickedButtonLayer0();
	afx_msg void OnEnChangeEditScaleu();
	afx_msg void OnEnChangeEditScalev();
	afx_msg void OnBnClickedChangetex();
	afx_msg void OnBnClickedCheckAutograss();
	afx_msg void OnBnClickedButtonFilter();

	afx_msg void OnBnClickedButtonExport();
	float m_fEnv;
	float m_fSpecular;
	float m_fEmissive;
	afx_msg void OnEnChangeEditEnv();
	afx_msg void OnEnChangeEditSpecular();
	afx_msg void OnEnChangeEditEmissive();
};

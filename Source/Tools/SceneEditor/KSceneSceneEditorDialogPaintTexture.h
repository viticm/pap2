#pragma once
#include <assert.h>
#include <afx.h>
#include "afxwin.h"
#include "resource.h"

interface IEKG3DSceneSceneEditor;

// KSceneSceneEditorDialogPaintTexture dialog
class IEKG3DTerrain;

#define GROUND_TEXTURE_LAYERS 4

class KSceneSceneEditorDialogPaintTexture : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogPaintTexture)

	virtual BOOL OnInitDialog();

public:
	KSceneSceneEditorDialogPaintTexture(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogPaintTexture();

// Dialog Data
	enum { IDD = IDD_KSCENESCENEEDITORDIALOGPAINTTEXTURE}; //IDD_DIALOG22 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSetOperatorLayter1();
	afx_msg void OnSetOperatorLayter2();
	afx_msg void OnSetOperatorLayter3();
	afx_msg void OnSetOperatorLayter4();

	afx_msg void OnSetVisibleLayter1();
	afx_msg void OnSetVisibleLayter2();
	afx_msg void OnSetVisibleLayter3();
	afx_msg void OnSetVisibleLayter4();

	afx_msg void OnSetIsSelectAllRegion();
	afx_msg void OnSetIsDrawRegionEdge();

	afx_msg void OnSetSelectBrushMap();

public:

	void SetDataToTerrainEx();

	inline void SetTerrain(IEKG3DTerrain *lpTerrainEx)
	{
		m_lpCurrentTerrainEx = lpTerrainEx;
		assert(m_lpCurrentTerrainEx);

		//return S_OK;
	}

	inline void SetSceneEditor(IEKG3DSceneSceneEditor *lpSceneEditor)
	{
		m_lpSceneEditor = lpSceneEditor;
		assert(m_lpSceneEditor);

		//return S_OK;
	}


private:

	IEKG3DTerrain       		*m_lpCurrentTerrainEx;
	IEKG3DSceneSceneEditor		*m_lpSceneEditor;

	BOOL						m_bLayerIsVisible[GROUND_TEXTURE_LAYERS];
	bool						m_bIsSelectAllRegion;
	int							m_nOperatorLayer;
	CButton						m_bnVLayter1,m_bnVLayter2,m_bnVLayter3,m_bnVLayter4;
	int							m_nOLayter1;
	CButton						m_IsSelectAllRegion;
	CButton						m_IsDrawRegionEdge;
	bool						m_bIsDrawRegionEdge;
	CSliderCtrl					m_Slider;		
	CString						m_szStatic;
	int							m_nRotationAngle;
	afx_msg void OnNMThemeChangedSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

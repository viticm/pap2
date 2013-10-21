#if !defined(AFX_KSCENESCENEEDITORDIALOGTERRIAN_H__5AC8B175_A952_4A65_860C_941DF12265E1__INCLUDED_)
#define AFX_KSCENESCENEEDITORDIALOGTERRIAN_H__5AC8B175_A952_4A65_860C_941DF12265E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneSceneEditorDialogTerrian.h : header file
//
#include "KColorButton.h"
#include "afxwin.h"

interface IEKG3DSceneSceneEditor;//by dengzhihui Refactor 2006年11月27日
class KSceneSceneEditorObscaleDialog;
class KSceneEditorView;

/////////////////////////////////////////////////////////////////////////////
// KSceneSceneEditorDialogTerrian dialog

class KSceneSceneEditorDialogTerrian : public CDialog
{
// Construction
public:
	KSceneSceneEditorDialogTerrian(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KSceneSceneEditorDialogTerrian)
	enum { IDD = IDD_SCENEEDITOR_TERRIAN };
	CButton	m_CheckHeightGet;
	CEdit	m_EditHeight;
	CSpinButtonCtrl	m_Spin;
	CSliderCtrl	m_SliderCtrlAlpha;
	CSliderCtrl	m_SliderCtrlOuterSize;
    CSliderCtrl	m_SliderCtrlInsideSize;
    CSliderCtrl m_SliderPower;
	KColorButton	m_Color;
	CButton	m_CheckHeightUp;
	CButton	m_CheckHeightSet;
	CButton	m_CheckHeightNoise;
	CButton	m_CheckHeightFlat;
	CButton	m_CheckHeightDown;
	CButton	m_CheckHeight;
	CButton	m_CheckColor;
	CButton	m_CheckCliffWater;
    CButton m_OppositeHeight;
    CButton m_AbsoluteHeight;
	int		m_Size,m_Size2,m_LastSize,m_LastSize2;
	int		m_Alpha;
	CString	m_CStringBrushSize,m_CStringBrushSize2;
	CString	m_CStringBrushAlpha;
    CString	m_CStringPower;
	float	m_fHeight;
    float   m_fOppositeHeightValue;
    float   m_fPower;
    int     m_nPower;
	BOOL	m_bGrass;
	BOOL	m_bGround;
	BOOL	m_bHeightStand;
	BOOL	m_bWater;
	BOOL	m_bBarrier;
	BOOL	m_bDeleteRegion;
	BOOL	m_bAddRegion;
    BOOL    m_bIsGetAbsoluteHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneSceneEditorDialogTerrian)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	KSceneSceneEditorObscaleDialog* m_lpObscaleDialog;
	//CImageList  m_imgList;

	void  UnCheckAllOperationType();
	void  UnCheckAllTerrain();
	void  UnCheckAllHeights();
	// Generated message map functions
	//{{AFX_MSG(KSceneSceneEditorDialogTerrian)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefreshFX();
	afx_msg void OnCgrounddown();
	afx_msg void OnCgroundflat();
	afx_msg void OnCgroundnoise();
	afx_msg void OnCgroundset();
	afx_msg void OnCgroundup();
	afx_msg void OnCheight();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEditheightvalue();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnGroundget();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCheckGrass();
	afx_msg void OnCheckGround();

	afx_msg void OnSetWireFrameMode();
	afx_msg void OnSetSolidMode();

    afx_msg void OnOptimizeVideoMemoryLightMap();

    void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
	KSceneEditorView *m_lpSceneEditorView;
	DWORD             m_dwNumGroundTexture;
public:
	void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit)
	{
		m_lpSceneSceneEditor = pSceneEdit;
	}

	virtual BOOL DestroyWindow();
	void		SetSceneEditorView(KSceneEditorView *lpSceneEditorView)
	{
		m_lpSceneEditorView = lpSceneEditorView;
	}
public:
	afx_msg void OnBnClickedButtonAddregion();
	afx_msg void OnBnClickedButtonDeleteregion();
	afx_msg void OnBnClickedCheckOptimize();
    afx_msg void OnBnClickedSpeedTreeLOD();
    afx_msg void OnNMCustomdrawSliderszie(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSetAbsoluteHeight();
    afx_msg void OnSetOppositeHeight();
    afx_msg void OnSetOppositeHeightValue();
//(by dengzhihui 2006年8月30日 12:01:39
public:
	void OnInvertOperation(BOOL bEnabled);
	void OnSmoothOperation(BOOL bEnabled);
	void OnBrushWidthModify(BOOL bAdjustInternalCircle, int nXOff, int nYOff);
	void OnBrushForceModify(int nXOff, int nYOff);
	void OnUpdateUI(UINT nActionID);
private:
	BOOL m_bInInvertMode, m_bInSmoothMode;
	typedef struct tagSmoothSave
	{
		BOOL bPreInvertMode;
		BOOL bPreHeightUpChecked;
		BOOL bPreHeightDownChecked;
		tagSmoothSave(BOOL argbPreInvertMode, BOOL argbPreHeightUpChecked, BOOL argbHeightDownChecked)
		{
			bPreInvertMode = argbPreInvertMode;
			bPreHeightUpChecked = argbPreHeightUpChecked;
			bPreHeightDownChecked = argbHeightDownChecked;
		}
	}SmoothSave;
	SmoothSave m_stSmoothSaveInfo;

//)
public:
	afx_msg void OnBnClickedCheckTerrainLOD();
	afx_msg void OnBnClickedCheckShowedge();
	afx_msg void OnBnClickedComputeNormal();
	afx_msg void OnBnClickedRefreshTerraintexture();
	afx_msg void OnBnClickedButtonRefreshterrain();
	afx_msg void OnBnClickedExportNormal();
    afx_msg void OnBnClickedCheckPvsCulling();
	afx_msg void OnBnClickedExportTerraintexture();
	afx_msg void OnBnClickedRadioLow();
	afx_msg void OnBnClickedRadioMiddle();
	afx_msg void OnBnClickedRadioHight();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENESCENEEDITORDIALOGTERRIAN_H__5AC8B175_A952_4A65_860C_941DF12265E1__INCLUDED_)

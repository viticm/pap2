#pragma once
//#include "KG3DPostRenderEffectBase.h"
#include "KColorButton.h"
#include "KEnterEdit.h"
#include "KGLilisSlipPanel.h"
#include "afxwin.h"
#include "KCurveCtrl.h"
class KG3DSceneSceneEditor;

#define EFFECT_INI_FILE TEXT("\\Data\\public\\shader\\PostRenderFinalPass.ini")
#define EFFECT_SFX_FILE TEXT("\\Data\\public\\shader\\PostRenderFinalPass.fx")


class KSceneSceneEditorDialogPostRender : public KGLilisSlipPanel<CDialog>
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogPostRender)

public:
	KSceneSceneEditorDialogPostRender(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogPostRender();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_POSTRENDER };

	void SetSceneEditor(IEKG3DSceneSceneEditor* pEditor)
	{
		m_lpSceneEditor = pEditor;
	}
protected:
	IEKG3DSceneSceneEditor* m_lpSceneEditor;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    KGLilisButton m_ButtonReload;
    KGLilisButton m_ButtonOpenSX;

    KEnterEdit  m_EditBloom_0;
    KEnterEdit  m_EditBloom_1;
    KEnterEdit  m_EditBloom_2;
    KEnterEdit  m_EditBloom_3;
    KEnterEdit  m_EditBloom_4;
    KEnterEdit  m_EditBloom_5;

    CSpinButtonCtrl m_SpinB0;
    CSpinButtonCtrl m_SpinB1;
    CSpinButtonCtrl m_SpinB2;
    CSpinButtonCtrl m_SpinB3;
    CSpinButtonCtrl m_SpinB4;
    CSpinButtonCtrl m_SpinB5;

    CComboBox   m_ComboBloomSamNum;

    KEnterEdit  m_EditDetailPower;
    KEnterEdit  m_EditBlurPower;
    KEnterEdit  m_EditBlurDistance;

    CSpinButtonCtrl m_SpinD0;
    CSpinButtonCtrl m_SpinD1;
    CSpinButtonCtrl m_SpinD2;

    KEnterEdit  m_EditHSI_0;
    KEnterEdit  m_EditHSI_1;
    KEnterEdit  m_EditHSI_2;
    KEnterEdit  m_EditHSI_3;
    KEnterEdit  m_EditHSI_4;
    KEnterEdit  m_EditHSI_5;

    CSpinButtonCtrl m_SpinH0;
    CSpinButtonCtrl m_SpinH1;
    CSpinButtonCtrl m_SpinH2;
    CSpinButtonCtrl m_SpinH3;
    CSpinButtonCtrl m_SpinH4;
    CSpinButtonCtrl m_SpinH5;

    KEnterEdit  m_EditWave_0;
    CSpinButtonCtrl m_SpinS5;

    KEnterEdit  m_EditGodRaySampleNum;
    KEnterEdit  m_EditGodRayDensity;
    KEnterEdit  m_EditGodRayWeight;
    KEnterEdit  m_EditGodRayDecay;
    KEnterEdit  m_EditGodRayExposure;
    KEnterEdit  m_EditGodRayAngle;

    CSpinButtonCtrl m_SpinGodRaySampleNum;
    CSpinButtonCtrl m_SpinGodRayDensity;
    CSpinButtonCtrl m_SpinGodRayWeight;
    CSpinButtonCtrl m_SpinGodRayDecay;
    CSpinButtonCtrl m_SpinGodRayExposure;
    CSpinButtonCtrl m_SpinGodRayAngle;

    KEnterEdit  m_EditSSAOLevel;
    KEnterEdit  m_EditSSAOSampRadius;
    KEnterEdit  m_EditSSAODark;
    KEnterEdit  m_EditSSAOBlurWeight;

    CSpinButtonCtrl m_SpinSSAOLevel;
    CSpinButtonCtrl m_SpinSSAOSampRadius;
    CSpinButtonCtrl m_SpinSSAODark;
    CSpinButtonCtrl m_SpinSSAOBlurWeight;

    CButton     m_checkPostRender;
    CButton     m_checkBloom;
    CButton     m_checkWave;
    CButton     m_checkHSI;
    CButton     m_checkMotionBlur;
	CButton     m_chk_AreaShadow;

    KCurveCtrl  m_CurveCtrl;
    CComboBox   m_ComboStype;

    CSliderCtrl m_SliderKC;
    CSliderCtrl m_SliderKM;
    CSliderCtrl m_SliderKY;

	KEnterEdit  m_EditShadow_Softness;
	KEnterEdit  m_EditShadow_Jitter;

	CSpinButtonCtrl m_SpinSoftness;
	CSpinButtonCtrl m_SpinJitter;

	CButton     m_CheckHDR;
	KEnterEdit  m_EditHDRBrightGate;
	KEnterEdit  m_EditHDRRectify;
	KEnterEdit  m_EditHDRGrayLevel;
	KEnterEdit  m_EditHDRBloomScale;
	KEnterEdit  m_EditHDRStarScale;
	CSpinButtonCtrl m_SpinHDRBrightGate;
	CSpinButtonCtrl m_SpinHDRRectify;
	CSpinButtonCtrl m_SpinHDRGrayLevel;
	CSpinButtonCtrl m_SpinHDRBloomScale;
	CSpinButtonCtrl m_SpinHDRStarScale;
	CComboBox       m_ComboHDRStarType;
	CButton         m_CheckHDREnableBlueShift;

 
    DWORD m_dwSettingType;

    HRESULT UpdateDataToInterface();

    BOOL OnInitDialog();

    int  EnableBloomUI(int nEnable);
    int  EnableWaveUI(int nEnable);
    int  EnableHSIUI(int nEnable);
    int  EnableGodRayUI(int nEnable);
    int  EnableSSAOUI(int nEnable);
    int  EnableCurveCMKY(int nEnable);
    int  EnableShadowUI(int nEnable);
    int  EnableDetailUI(int nEnable);
	int  EnableHDRUI(int nEnable);

    void UpdateSelCurveCMYKStype(int index);

    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCheckPostrender();
	afx_msg void OnRefReshFx();
    afx_msg void OnBnClickedCheckShockWave();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnCbnSelchangeComboMisc();
    afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnBnClickedButtonOpenSFX();
public:
    afx_msg void OnBnClickedCheckBloom();
public:
    afx_msg void OnBnClickedCheckHsi();
    afx_msg void OnBnClickedCheckHsi2();
	afx_msg void OnBnClickedCheckDetail();
	CButton m_checkDetail;
    afx_msg void OnBnClickedCheckShadow();
    CButton m_bnShadow;
    afx_msg void OnBnClickedButtonOpensfx();
    afx_msg void OnBnClickedCheckGodray();
    CButton m_checkGodRay;
	afx_msg void OnBnClickedCheckAreashadow();
    CButton m_checkSSAO;
    afx_msg void OnBnClickedCheckSSAO();
    afx_msg void OnBnClickedCheckR();
    afx_msg void OnBnClickedCheckG();
    afx_msg void OnBnClickedCheckB();
    afx_msg void OnBnClickedCheckCurvecmyk();
    CButton m_checkCurveCMYK;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnCbnSelchangeComboStype();
    afx_msg void OnBnClickedButtonCcCopy();
    afx_msg void OnBnClickedButtonCcDel();
    afx_msg void OnBnClickedButtonCcReset();
    afx_msg void OnCbnEditupdateComboStype();
    afx_msg void OnBnClickedButtonCcRename();
    afx_msg void OnBnClickedButtonCcResetM();
    afx_msg void OnBnClickedButtonCcResetK();
    afx_msg void OnBnClickedButtonCcResetY();
    afx_msg void OnCbnSelchangeBloomSamnum();
	afx_msg void OnBnClickedCheckHDR();
	afx_msg void OnBnClickedCheckEnableBlueShift();
	afx_msg void OnCbnSelchangeComboHDRStarType();
};

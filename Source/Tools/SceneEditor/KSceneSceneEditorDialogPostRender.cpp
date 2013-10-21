
#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogPostRender.h"
#include ".\kscenesceneeditordialogpostrender.h"

//#include "KG3DPostRenderEffectBloom.h"
#include "KColorDialog.h"
#include "KG3DInterface.h"
#include "IEEditor.h"
#include "IEKG3DSceneOutputWnd.h"
#include "InputBox.h"
#include "IEKG3DEnvironment.h"


IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogPostRender, KGLilisSlipPanel<CDialog>)
KSceneSceneEditorDialogPostRender::KSceneSceneEditorDialogPostRender(CWnd* pParent /*=NULL*/)
	: KGLilisSlipPanel<CDialog>(KSceneSceneEditorDialogPostRender::IDD, pParent)
{
    m_dwSettingType = 0;
	m_lpSceneEditor = NULL;
}

KSceneSceneEditorDialogPostRender::~KSceneSceneEditorDialogPostRender()
{
}

void KSceneSceneEditorDialogPostRender::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_PARAM_B0,   m_EditBloom_0);
    DDX_Control(pDX, IDC_PARAM_B1,   m_EditBloom_1);
    DDX_Control(pDX, IDC_PARAM_B2,   m_EditBloom_2);
    DDX_Control(pDX, IDC_PARAM_B3,   m_EditBloom_3);
    DDX_Control(pDX, IDC_PARAM_B4,   m_EditBloom_4);
    DDX_Control(pDX, IDC_PARAM_B5,   m_EditBloom_5);

    DDX_Control(pDX, IDC_SPIN_B0,    m_SpinB0);
    DDX_Control(pDX, IDC_SPIN_B1,    m_SpinB1);
    DDX_Control(pDX, IDC_SPIN_B2,    m_SpinB2);
    DDX_Control(pDX, IDC_SPIN_B3,    m_SpinB3);
    DDX_Control(pDX, IDC_SPIN_B4,    m_SpinB4);
    DDX_Control(pDX, IDC_SPIN_B5,    m_SpinB5);

    DDX_Control(pDX, IDC_DETAIL_POWER,   m_EditDetailPower);
    DDX_Control(pDX, IDC_BLUR_POWER,   m_EditBlurPower);
    DDX_Control(pDX, IDC_BLUR_DISTANCE,   m_EditBlurDistance);
    
    DDX_Control(pDX, IDC_SPIN_DETAIL_POWER,  m_SpinD0);
    DDX_Control(pDX, IDC_SPIN_BLUR_POWER,    m_SpinD1);
    DDX_Control(pDX, IDC_SPIN_BLUR_DISTANCE, m_SpinD2);


    DDX_Control(pDX, IDC_BLOOM_SAMNUM, m_ComboBloomSamNum);

    DDX_Control(pDX, IDC_PARAM_H0,   m_EditHSI_0);
    DDX_Control(pDX, IDC_PARAM_H1,   m_EditHSI_1);
    DDX_Control(pDX, IDC_PARAM_H2,   m_EditHSI_2);
    DDX_Control(pDX, IDC_PARAM_H3,   m_EditHSI_3);
    DDX_Control(pDX, IDC_PARAM_H4,   m_EditHSI_4);
    DDX_Control(pDX, IDC_PARAM_H5,   m_EditHSI_5);

    DDX_Control(pDX, IDC_SPIN_H0,    m_SpinH0);
    DDX_Control(pDX, IDC_SPIN_H1,    m_SpinH1);
    DDX_Control(pDX, IDC_SPIN_H2,    m_SpinH2);
    DDX_Control(pDX, IDC_SPIN_H3,    m_SpinH3);
    DDX_Control(pDX, IDC_SPIN_H4,    m_SpinH4);
    DDX_Control(pDX, IDC_SPIN_H5,    m_SpinH5);

    DDX_Control(pDX, IDC_PARAM_S0,   m_EditWave_0);
    DDX_Control(pDX, IDC_SPIN_S0,    m_SpinS5);

    DDX_Control(pDX, IDC_SAMPLE_NUM,   m_EditGodRaySampleNum);
    DDX_Control(pDX, IDC_DENSITY,      m_EditGodRayDensity);
    DDX_Control(pDX, IDC_WEIGHT,       m_EditGodRayWeight);
    DDX_Control(pDX, IDC_DECAY,        m_EditGodRayDecay);
    DDX_Control(pDX, IDC_EXPOSURE,     m_EditGodRayExposure);
    DDX_Control(pDX, IDC_RAYANGLE,     m_EditGodRayAngle);

    DDX_Control(pDX, IDC_SPIN_SAMPLE_NUM,   m_SpinGodRaySampleNum);
    DDX_Control(pDX, IDC_SPIN_DENSITY,      m_SpinGodRayDensity);
    DDX_Control(pDX, IDC_SPIN_WEIGHT,       m_SpinGodRayWeight);
    DDX_Control(pDX, IDC_SPIN_DECAY,        m_SpinGodRayDecay);
    DDX_Control(pDX, IDC_SPIN_EXPOSURE,     m_SpinGodRayExposure);
    DDX_Control(pDX, IDC_SPIN_RAYANGLE,     m_SpinGodRayAngle);

    DDX_Control(pDX, IDC_SSAO_LEVEL,        m_EditSSAOLevel);
    DDX_Control(pDX, IDC_SSAO_RADIUS,       m_EditSSAOSampRadius);
    DDX_Control(pDX, IDC_SSAO_DARK,         m_EditSSAODark);
    DDX_Control(pDX, IDC_SSAO_BLUR,         m_EditSSAOBlurWeight);

    DDX_Control(pDX, IDC_SPIN_SSAO_LEVEL,       m_SpinSSAOLevel);
    DDX_Control(pDX, IDC_SPIN_SSAO_RADIUS,      m_SpinSSAOSampRadius);
    DDX_Control(pDX, IDC_SPIN_SSAO_DARK,        m_SpinSSAODark);
    DDX_Control(pDX, IDC_SPIN_SSAO_BLUR,        m_SpinSSAOBlurWeight);


    DDX_Control(pDX, IDC_BUTTON_REFRESH, m_ButtonReload);
    DDX_Control(pDX, IDC_BUTTON_OPENSFX, m_ButtonOpenSX);

    DDX_Control(pDX, IDC_CHECK_POSTRENDER, m_checkPostRender);
    DDX_Control(pDX, IDC_CHECK_BLOOM, m_checkBloom);
    DDX_Control(pDX, IDC_CHECK_SHOCK_WAVE, m_checkWave);
    DDX_Control(pDX, IDC_CHECK_HSI, m_checkHSI);
    DDX_Control(pDX, IDC_CHECK_HSI2, m_checkMotionBlur);

    DDX_Control(pDX, IDC_CHECK_DETAIL, m_checkDetail);
    DDX_Control(pDX, IDC_CHECK_SHADOW, m_bnShadow);

    DDX_Control(pDX, IDC_CHECK_GODRAY, m_checkGodRay);
    DDX_Control(pDX, IDC_CHECK_SSAO, m_checkSSAO);
    DDX_Control(pDX, IDC_CHECK_CurveCMYK, m_checkCurveCMYK);

	DDX_Control(pDX, IDC_CHECK_AREASHADOW, m_chk_AreaShadow);

    DDX_Control(pDX, IDC_CURVE, m_CurveCtrl);
    DDX_Control(pDX, IDC_COMBO_STYPE, m_ComboStype);

    DDX_Control(pDX, IDC_SLIDER_KC, m_SliderKC);
    DDX_Control(pDX, IDC_SLIDER_KM, m_SliderKM);
    DDX_Control(pDX, IDC_SLIDER_KY, m_SliderKY);


	DDX_Control(pDX, IDC_SOFTNESS, m_EditShadow_Softness);
	DDX_Control(pDX, IDC_JITTER, m_EditShadow_Jitter);
	DDX_Control(pDX, IDC_SPIN_SOFTNESS, m_SpinSoftness);
	DDX_Control(pDX, IDC_SPIN_JITTER, m_SpinJitter);

	DDX_Control(pDX, IDC_HDR_BRIGHTGATE, m_EditHDRBrightGate);
	DDX_Control(pDX, IDC_HDR_RECTIFY, m_EditHDRRectify);
	DDX_Control(pDX, IDC_HDR_GRAYLEVEL, m_EditHDRGrayLevel);
	DDX_Control(pDX, IDC_HDR_BLOOMSCALE, m_EditHDRBloomScale);
	DDX_Control(pDX, IDC_HDR_STARSCALE, m_EditHDRStarScale);

	DDX_Control(pDX, IDC_SPIN_HDR_BRIGHTGATE, m_SpinHDRBrightGate);
	DDX_Control(pDX, IDC_SPIN_HDR_RECTIFY, m_SpinHDRRectify);
	DDX_Control(pDX, IDC_SPIN_HDR_GRAYLEVEL, m_SpinHDRGrayLevel);
	DDX_Control(pDX, IDC_SPIN_HDR_BLOOMSCALE, m_SpinHDRBloomScale);
	DDX_Control(pDX, IDC_SPIN_HDR_STARSCALE, m_SpinHDRStarScale);

	DDX_Control(pDX, IDC_HDR_STARTYPE, m_ComboHDRStarType);
	DDX_Control(pDX, IDC_HDR_ENABLEBLUESHIFT, m_CheckHDREnableBlueShift);
	DDX_Control(pDX, IDC_CHECK_HDR, m_CheckHDR);
}


BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogPostRender, KGLilisSlipPanel<CDialog>)
	ON_BN_CLICKED(IDC_CHECK_POSTRENDER, OnBnClickedCheckPostrender)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnRefReshFx)
    ON_BN_CLICKED(IDC_CHECK_SHOCK_WAVE, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckShockWave)
	ON_WM_SHOWWINDOW()
    ON_WM_SIZE()
    ON_CBN_SELCHANGE(IDC_COMBO_MISC, &KSceneSceneEditorDialogPostRender::OnCbnSelchangeComboMisc)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_B0, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_B1, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_B2, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_B3, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_B4, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_B5, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_S0, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_H0, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_H1, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_H2, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_H3, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_H4, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_H5, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)

    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DETAIL_POWER, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BLUR_POWER, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BLUR_DISTANCE, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)

    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAMPLE_NUM, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DENSITY, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WEIGHT, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DECAY, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EXPOSURE, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RAYANGLE, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)

    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SSAO_LEVEL, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SSAO_RADIUS, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SSAO_DARK, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SSAO_BLUR, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SOFTNESS, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_JITTER, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HDR_BRIGHTGATE, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HDR_RECTIFY, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HDR_GRAYLEVEL, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HDR_BLOOMSCALE, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HDR_STARSCALE, &KSceneSceneEditorDialogPostRender::OnDeltaposSpin)

    ON_BN_CLICKED(IDC_BUTTON_REFRESH2, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonOpenSFX)
    ON_BN_CLICKED(IDC_CHECK_BLOOM, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckBloom)
    ON_BN_CLICKED(IDC_CHECK_HSI, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckHsi)
    ON_BN_CLICKED(IDC_CHECK_HSI2, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckHsi2)
	ON_BN_CLICKED(IDC_CHECK_DETAIL, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckDetail)
    ON_BN_CLICKED(IDC_CHECK_SHADOW, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckShadow)
    ON_BN_CLICKED(IDC_BUTTON_OPENSFX, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonOpensfx)
    ON_BN_CLICKED(IDC_CHECK_GODRAY, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckGodray)
	ON_BN_CLICKED(IDC_CHECK_AREASHADOW, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckAreashadow)
    ON_BN_CLICKED(IDC_CHECK_SSAO, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckSSAO)
    ON_BN_CLICKED(IDC_CHECK_R, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckR)
    ON_BN_CLICKED(IDC_CHECK_G, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckG)
    ON_BN_CLICKED(IDC_CHECK_B, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckB)
    ON_BN_CLICKED(IDC_CHECK_CurveCMYK, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckCurvecmyk)
    ON_BN_CLICKED(IDC_BUTTON1, &KSceneSceneEditorDialogPostRender::OnBnClickedButton1)
    
    ON_WM_HSCROLL()
    ON_CBN_SELCHANGE(IDC_COMBO_STYPE, &KSceneSceneEditorDialogPostRender::OnCbnSelchangeComboStype)
    ON_BN_CLICKED(IDC_BUTTON_CC_COPY, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcCopy)
    ON_BN_CLICKED(IDC_BUTTON_CC_DEL, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcDel)
    ON_BN_CLICKED(IDC_BUTTON_CC_RESET, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcReset)
    ON_CBN_EDITUPDATE(IDC_COMBO_STYPE, &KSceneSceneEditorDialogPostRender::OnCbnEditupdateComboStype)
    ON_BN_CLICKED(IDC_BUTTON_CC_RENAME, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcRename)
    ON_BN_CLICKED(IDC_BUTTON_CC_RESET_M, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcResetM)
    ON_BN_CLICKED(IDC_BUTTON_CC_RESET_K, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcResetK)
    ON_BN_CLICKED(IDC_BUTTON_CC_RESET_Y, &KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcResetY)
    ON_CBN_SELCHANGE(IDC_BLOOM_SAMNUM, &KSceneSceneEditorDialogPostRender::OnCbnSelchangeBloomSamnum)

	ON_BN_CLICKED(IDC_CHECK_HDR, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckHDR)
	ON_BN_CLICKED(IDC_HDR_ENABLEBLUESHIFT, &KSceneSceneEditorDialogPostRender::OnBnClickedCheckEnableBlueShift)
	ON_CBN_SELCHANGE(IDC_HDR_STARTYPE, &KSceneSceneEditorDialogPostRender::OnCbnSelchangeComboHDRStarType)
END_MESSAGE_MAP()

void KSceneSceneEditorDialogPostRender::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);
        
    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    if (nSBCode == SB_ENDSCROLL || nSBCode == SB_THUMBTRACK)
    {
        if (static_cast<CWnd*>(pScrollBar) == static_cast<CWnd*>(&m_SliderKC))
        {
            float k = static_cast<float>(m_SliderKC.GetPos()) / 200.f * 2.f - 1.f;
            pWnd->SetCMYK_KC(k);
            
        }

        if (static_cast<CWnd*>(pScrollBar) == static_cast<CWnd*>(&m_SliderKM))
        {
            float k = static_cast<float>(m_SliderKM.GetPos()) / 200.f * 2.f - 1.f;
            pWnd->SetCMYK_KM(k);
        }

        if (static_cast<CWnd*>(pScrollBar) == static_cast<CWnd*>(&m_SliderKY))
        {
            float k = static_cast<float>(m_SliderKY.GetPos()) / 200.f * 2.f - 1.f;
            pWnd->SetCMYK_KY(k);
        }
    }

Exit0 :
    return KGLilisSlipPanel<CDialog>::OnHScroll(nSBCode, nPos, pScrollBar);
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckPostrender()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
	
    if (!m_lpSceneEditor)
		return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

	RECT r;
    pWnd->GetRect(&r);
	DWORD dwWidth = r.right - r.left;
	DWORD dwHeight =  r.bottom - r.top;

    BOOL bEnable = m_checkPostRender.GetCheck();
	pWnd->ResetDevice(bEnable, dwWidth, dwHeight);

    m_checkBloom.EnableWindow(bEnable);
    m_checkWave.EnableWindow(bEnable);
    m_checkHSI.EnableWindow(bEnable);
    m_checkMotionBlur.EnableWindow(bEnable);
    m_checkGodRay.EnableWindow(bEnable);
    m_checkSSAO.EnableWindow(bEnable);
    m_checkCurveCMYK.EnableWindow(bEnable);
    m_bnShadow.EnableWindow(bEnable);
    m_checkDetail.EnableWindow(bEnable);
	m_CheckHDR.EnableWindow(bEnable);



    BOOL bEnableBloom = m_checkBloom.GetCheck();
    BOOL bEnableWave  = m_checkWave.GetCheck();
    BOOL bEnableHSI   = m_checkHSI.GetCheck();
    BOOL bEnableBlur  = m_checkMotionBlur.GetCheck();
    BOOL bEnableGodRay   = m_checkGodRay.GetCheck();
    BOOL bEnableSSAO  = m_checkSSAO.GetCheck();
    BOOL bEnableCurveCMKY = m_checkCurveCMYK.GetCheck();
    BOOL bEnableShadow = m_bnShadow.GetCheck();
    BOOL bEnableDetial    = m_checkDetail.GetCheck();
	BOOL bEnableHDR    = m_CheckHDR.GetCheck();

    EnableBloomUI(bEnableBloom);
    EnableWaveUI(bEnableWave);
    EnableHSIUI(bEnableHSI);
    EnableGodRayUI(bEnableGodRay);
    EnableSSAOUI(bEnableSSAO);
    EnableCurveCMKY(bEnableCurveCMKY);
    EnableShadowUI(bEnableShadow);
    EnableDetailUI(bEnableDetial);
	EnableHDRUI(bEnableHDR);

    pWnd->EnablePostEffect(POST_EFFECT_BLOOM, bEnableBloom);
    pWnd->EnablePostEffect(POST_EFFECT_SOCKWARE, bEnableWave);
    pWnd->EnablePostEffect(POST_EFFECT_HSI, bEnableHSI);
    pWnd->EnablePostEffect(POST_EFFECT_CURVE_CMYK, bEnableCurveCMKY);
    pWnd->EnablePostEffect(POST_EFFECT_MOTIONBLUE, bEnableBlur);
    pWnd->EnablePostEffect(POST_EFFECT_GODRAY, bEnableGodRay);
    pWnd->EnablePostEffect(POST_EFFECT_SSAO, bEnableSSAO);
    pWnd->EnablePostEffect(POST_EFFECT_SHADOW, bEnableShadow);
    pWnd->EnablePostEffect(POST_EFFECT_DETAIL, bEnableDetial);
	pWnd->EnablePostEffect(POST_EFFECT_HDR, bEnableHDR);

}

void KSceneSceneEditorDialogPostRender::OnSize(UINT nType, int cx, int cy)
{
    SetOffset(10, max(0, cx - 185));
    UpdateUI();

    return KGLilisSlipPanel<CDialog>::OnSize(nType, cx, cy);
}

void KSceneSceneEditorDialogPostRender::OnOK()
{

}

void KSceneSceneEditorDialogPostRender::OnRefReshFx()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    pWnd->ReloadFX();
}


LRESULT KSceneSceneEditorDialogPostRender::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    {
        IEKG3DEnvironment *pEnv = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
        //_ASSERTE(pEnv);
		if(pEnv)
		{
			DWORD dwSettingType = pEnv->GetSettingType();
			if (dwSettingType != m_dwSettingType)
			{
				m_dwSettingType = dwSettingType;
				UpdateDataToInterface();
			}
		}
    }
    switch (message)
    {
    case WM_CURVERCTRL_SPLINE_CHANGED :
        {   
            IEKG3DSceneOutputWnd* pWnd = NULL;

            if (!m_lpSceneEditor)
                break;

            m_lpSceneEditor->GetCurOutputWnd(&pWnd);

            if (!pWnd)
                break;
            

            switch ((int)wParam)
            {
            case 0 :
                {
                    D3DXVECTOR2* pts = NULL;
                    bool bNeedDel = false;
                    int  num = 0;

                    pts = m_CurveCtrl.GetControlPoints(0, num, bNeedDel);

                    if (!pts)
                        break;

                    pWnd->SetActiveCurveRedPoints(pts, num);

                    if (bNeedDel)
                        delete[] pts;
                }
                /*
                m_CurveCtrl.GetSplinePoint(0, pts, 256);
                pWnd->SetCurveRed(pts);
                */
                break;
            case 1 :
                {
                    D3DXVECTOR2* pts = NULL;
                    bool bNeedDel = false;
                    int  num = 0;

                    pts = m_CurveCtrl.GetControlPoints(1, num, bNeedDel);

                    if (!pts)
                        break;

                    pWnd->SetActiveCurveGreedPoints(pts, num);

                    if (bNeedDel)
                        delete[] pts;
                }
                /*
                m_CurveCtrl.GetSplinePoint(1, pts, 256);
                pWnd->SetCurveGreed(pts);
                */
                break;
            case 2 :
                {
                    D3DXVECTOR2* pts = NULL;
                    bool bNeedDel = false;
                    int  num = 0;

                    pts = m_CurveCtrl.GetControlPoints(2, num, bNeedDel);

                    if (!pts)
                        break;

                    pWnd->SetActiveCurveBluePoints(pts, num);

                    if (bNeedDel)
                        delete[] pts;
                }
                /*
                m_CurveCtrl.GetSplinePoint(2, pts, 256);
                pWnd->SetCurveBlue(pts);
                */
                break;
            default :
                break;
            }
        }
        break;
    case WM_EDIT_RECEIVE_ENTER :
        {
            IEKG3DSceneOutputWnd* pWnd = NULL;

            if (!m_lpSceneEditor)
                break;

            m_lpSceneEditor->GetCurOutputWnd(&pWnd);

            if (!pWnd)
                break;

            CString* strText = (CString*)lParam;
            float fValue = 0.f;
            sscanf(strText->GetBuffer(), "%f", &fValue);
            strText->ReleaseBuffer();

            switch (wParam)
            {
            case IDC_PARAM_B0 :
                pWnd->SetBloomGate(fValue);
                break;
            case IDC_PARAM_B1 :
                pWnd->SetBloomExposure1(fValue);
                break;
            case IDC_PARAM_B2 :
                pWnd->SetBloomExposure2(fValue);
                break;
            case IDC_PARAM_B3 :
                pWnd->SetBloomGaussCoefficient(fValue);
                break;
            case IDC_PARAM_B4 :
                pWnd->SetfBloomSampleNum((int)fValue);
                break;
            case IDC_PARAM_B5 :
                pWnd->SetBloomDelaye(fValue);
                break;
            case IDC_PARAM_H0 :
                pWnd->SetHSIHueScale(fValue);
                break;
            case IDC_PARAM_H1 :
                pWnd->SetHSIHueBias(fValue);
                break;
            case IDC_PARAM_H2 :
                pWnd->SetHSIIntensityScale(fValue);
                break;
            case IDC_PARAM_H3 :
                pWnd->SetHSIIntensityBias(fValue);
                break;
            case IDC_PARAM_H4 :
                pWnd->SetHSIIntensityScale(fValue);
                break;
            case IDC_PARAM_H5 :
                pWnd->SetHSISaturationBias(fValue);
                break;
            case IDC_PARAM_S0 :
                pWnd->SetShockWavePower(fValue);
                break;
            case IDC_SAMPLE_NUM :
                if ((int)fValue < 0 || (int)fValue > 5)
                    break;
                pWnd->SetGodRaySampleNum((int)fValue);
                break;
            case IDC_DENSITY :
                pWnd->SetGodRayDensity(fValue);
                break;
            case IDC_WEIGHT :
                pWnd->SetGodRayWeight(fValue);
                break;
            case IDC_DECAY :
                pWnd->SetGodRayDecay(fValue);
                break;
            case IDC_EXPOSURE :
                pWnd->SetGodRayExposure(fValue);
                break;
            case IDC_RAYANGLE :
                if (fValue < 0.f || fValue > D3DX_PI * 2.f)
                    break;
                pWnd->SetGodRayAngle(fValue);
                break;
            case IDC_SSAO_LEVEL :
                if ((int)fValue < 0 || (int)fValue > 2)
                    break;
                pWnd->SetSSAOLevel((int)fValue);
                break;
            case IDC_SSAO_RADIUS :
                pWnd->SetSSAOSampRadius(fValue);
                break;
            case IDC_SSAO_DARK :
                pWnd->SetSSAODarkFact(fValue);
                break;
            case IDC_SSAO_BLUR :
                pWnd->SetSSAOBlurWeight(fValue);
                break;
            case IDC_DETAIL_POWER :
                if (fValue < 0 || fValue > 2.f)
                    break;
                pWnd->SetDetailPower(fValue);
                break;
            case IDC_BLUR_POWER :
                fValue = fValue * -1.5f + 1.f;
                if (fValue > 1.f || fValue < -0.5f)
                    break;
                pWnd->SetBlurPower(fValue);
                break;
            case IDC_BLUR_DISTANCE :
                if (fValue < 0.f)
                    break;
                pWnd->SetBlurDistance(fValue * 100.f);
                break;
			case IDC_SOFTNESS :
				if (fValue < 0.f || fValue > 10.0f)
					break;
				pWnd->SetShadowSoftness(fValue);
				break;
			case IDC_JITTER :
				if (fValue < 0.f || fValue > 10.0f)
					break;
				pWnd->SetShadowJitter(fValue);
				break;
			case IDC_HDR_BRIGHTGATE :
				if (fValue < 0.f || fValue > 10.0f)
					break;
				pWnd->SetHDRBrightGate(fValue);
				break;
			case IDC_HDR_RECTIFY :
				if (fValue < 0.f || fValue > 10.0f)
					break;
				pWnd->SetHDRRectify(fValue);
				break;
			case IDC_HDR_GRAYLEVEL :
				if (fValue < 0.f || fValue > 10.0f)
					break;
				pWnd->SetHDRGrayLevel(fValue);
				break;
			case IDC_HDR_BLOOMSCALE :
				if (fValue < 0.f || fValue > 10.0f)
					break;
				pWnd->SetHDRBloomScale(fValue);
				break;
			case IDC_HDR_STARSCALE :
				if (fValue < 0.f || fValue > 10.0f)
					break;
				pWnd->SetHDRStarScale(fValue);
				break;
            default :
                break;
            }
        }
        break;
    default:
        break;
    }

    return KGLilisSlipPanel<CDialog>::WindowProc(message, wParam, lParam);
}

BOOL KSceneSceneEditorDialogPostRender::OnInitDialog()
{
    int nReslut = KGLilisSlipPanel<CDialog>::OnInitDialog();

    {
        IEKG3DEnvironment *pEnv = KSH::GetEngineInterfaceEx<IEKG3DEnvironment>();
        _ASSERTE(pEnv);
		if(pEnv)
			m_dwSettingType = pEnv->GetSettingType();
    }

    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    //Get3DEngineInterface((void **)&piManager);
    ASSERT(piManager);
    KG3DEngineOption engineOption;
    PostRenderParams& Params = *piManager->LockPostRenderParams(TRUE);

	//KG3DEngineEditorOption engineEditorOption;
    piManager->GetEngineOption(&engineOption);
	//piManager->GetEngineEditorOption(&engineEditorOption);

    m_CurveCtrl.SetSplineNum(3);
    m_CurveCtrl.SetSplineColor(0, RGB(128, 0, 0));
    m_CurveCtrl.SetSplineColor(1, RGB(0, 128, 128));
    m_CurveCtrl.SetSplineColor(2, RGB(0, 0, 128));

    CheckDlgButton(IDC_CHECK_R, TRUE);
    CheckDlgButton(IDC_CHECK_G, TRUE);
    CheckDlgButton(IDC_CHECK_B, TRUE);

    m_SliderKC.SetRange(0, 200);
    m_SliderKM.SetRange(0, 200);
    m_SliderKY.SetRange(0, 200);

    for (int i = 0; i < 8; ++i)
    {
        char text[32];
        sprintf(text, "L%d (%d blur pixel)", i, (i + 1) * 2);
        m_ComboBloomSamNum.AddString(text);
    }


    SetOffset(10, 75);

    if (m_lpSceneEditor)
    {
        IEKG3DSceneOutputWnd* pWnd = NULL;
        m_lpSceneEditor->GetCurOutputWnd(&pWnd);

        if (pWnd)
        {
            m_checkPostRender.SetCheck(engineOption.bPostEffectEnable);
            if (engineOption.bPostEffectEnable)
            {
                m_checkBloom.EnableWindow(true);
                m_checkWave.EnableWindow(true);
                m_checkHSI.EnableWindow(true);
                m_checkMotionBlur.EnableWindow(true);
                m_checkGodRay.EnableWindow(true);
                m_checkSSAO.EnableWindow(true);
                m_checkCurveCMYK.EnableWindow(true);
                m_checkDetail.EnableWindow(true);
                m_bnShadow.EnableWindow(true);
				m_CheckHDR.EnableWindow(true);

                EnableBloomUI(engineOption.bBloomEnable);
                EnableWaveUI(engineOption.bShockWaveEnable);
                EnableHSIUI(engineOption.bHSIEnable);
                EnableCurveCMKY(engineOption.bCurveCMYK);
                EnableGodRayUI(engineOption.bGodRay);
                EnableSSAOUI(engineOption.bSSAO);
                EnableCurveCMKY(engineOption.bCurveCMYK);
                EnableDetailUI(engineOption.bDetail);
                EnableShadowUI(engineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW);
				EnableHDRUI(engineOption.bHDR);
            }
            else
            {
                m_checkBloom.EnableWindow(false);
                m_checkWave.EnableWindow(false);
                m_checkHSI.EnableWindow(false);
                m_checkMotionBlur.EnableWindow(false);
                m_checkGodRay.EnableWindow(false);
                m_checkSSAO.EnableWindow(false);
                m_checkCurveCMYK.EnableWindow(false);
                m_bnShadow.EnableWindow(false);
                m_checkDetail.EnableWindow(false);
				m_CheckHDR.EnableWindow(false);

                EnableBloomUI(false);
                EnableWaveUI(false);
                EnableHSIUI(false);
                EnableGodRayUI(false);
                EnableSSAOUI(false);
                EnableCurveCMKY(false);
                EnableShadowUI(false);
                EnableDetailUI(false);
				EnableHDRUI(false);
            }

            m_SliderKC.SetPos(static_cast<int>((Params.fKC + 1.f) * 100.f));
            m_SliderKM.SetPos(static_cast<int>((Params.fKM + 1.f) * 100.f));
            m_SliderKY.SetPos(static_cast<int>((Params.fKY + 1.f) * 100.f));
            
            m_CurveCtrl.ResetPoint();
            m_ComboStype.ResetContent();

            for (int i = 0; i < pWnd->GetStypeCount(); ++i)
            {
                char szName[128];
                pWnd->GetStypeName(i, szName);
                m_ComboStype.AddString(szName);
            }

            ASSERT(pWnd->GetStypeCount() > 0);

            int nActive = pWnd->GetActiveStype();
            
            if (nActive == -1)
                nActive = 0;

            m_ComboStype.SetCurSel(nActive);

            for (int c = 0; c < 3; ++c)
            {
                for (int i = 0; i < pWnd->GetSplinePointCount(nActive, c); ++i)
                {
                    const D3DXVECTOR2& pt = pWnd->GetSplinePoint(nActive, c, i);
                    m_CurveCtrl.AddPoint(c, pt);
                }
            }



            m_checkBloom.SetCheck(engineOption.bBloomEnable);
            m_checkDetail.SetCheck(engineOption.bDetail);
            //m_bnShadow.SetCheck(engineOption.bEnableShadowMap);
            m_checkGodRay.SetCheck(engineOption.bGodRay);
            m_checkWave.SetCheck(engineOption.bShockWaveEnable);
            m_checkHSI.SetCheck(engineOption.bHSIEnable);
            m_checkCurveCMYK.SetCheck(engineOption.bCurveCMYK);
            m_checkMotionBlur.SetCheck(engineOption.bMotionBlur);
            m_checkGodRay.SetCheck(engineOption.bGodRay);
            m_checkSSAO.SetCheck(engineOption.bSSAO);
            m_checkCurveCMYK.SetCheck(engineOption.bCurveCMYK);
			m_bnShadow.SetCheck(engineOption.ModelShadowType > EM_MODEL_SHADOW_TYPE_LOW ? BST_CHECKED : BST_UNCHECKED);
			m_CheckHDR.SetCheck(engineOption.bHDR);

            CString strText;

            m_ComboBloomSamNum.SetCurSel(Params.nBloomSampleNum / 2 - 1);
            strText.Format("%.2f", Params.fBloomGate);
            m_EditBloom_0.SetWindowText(strText);
            strText.Format("%.2f", Params.fBloomExposure1);
            m_EditBloom_1.SetWindowText(strText);
            strText.Format("%.2f", Params.fBloomExposure2);
            m_EditBloom_2.SetWindowText(strText);
            strText.Format("%.2f", Params.fBloomGaussCoefficient);
            m_EditBloom_3.SetWindowText(strText);
            strText.Format("%d", Params.nBloomSampleNum);
            m_EditBloom_4.SetWindowText(strText);
            strText.Format("%.2f", Params.fBloomDelay);
            m_EditBloom_5.SetWindowText(strText);

            strText.Format("%.2f", Params.fDetailPower);
            m_EditDetailPower.SetWindowText(strText);
            strText.Format("%.2f", (Params.fBlurPower - 1.f) / -1.5f);
            m_EditBlurPower.SetWindowText(strText);
            strText.Format("%.2f", Params.fBlurDistance / 100.f);
            m_EditBlurDistance.SetWindowText(strText);

            strText.Format("%.2f", Params.fHueScale);
            m_EditHSI_0.SetWindowText(strText);
            strText.Format("%.2f", Params.fHueBias);
            m_EditHSI_1.SetWindowText(strText);
            strText.Format("%.2f", Params.fSaturationScale);
            m_EditHSI_2.SetWindowText(strText);
            strText.Format("%.2f", Params.fSaturationBias);
            m_EditHSI_3.SetWindowText(strText);
            strText.Format("%.2f", Params.fIntensityScale);
            m_EditHSI_4.SetWindowText(strText);
            strText.Format("%.2f", Params.fIntensityBias);
            m_EditHSI_5.SetWindowText(strText);

            strText.Format("%.2f", Params.fShockWavePower);
            m_EditWave_0.SetWindowText(strText);
            strText.Format("%d", Params.nGodRaySampleNum);
            m_EditGodRaySampleNum.SetWindowText(strText);
            strText.Format("%.2f", Params.fGodRayDensity);
            m_EditGodRayDensity.SetWindowText(strText);
            strText.Format("%.2f", Params.fGodRayWeight);
            m_EditGodRayWeight.SetWindowText(strText);
            strText.Format("%.2f", Params.fGodRayDecay);
            m_EditGodRayDecay.SetWindowText(strText);
            strText.Format("%.2f", Params.fGodRayExposure);
            m_EditGodRayExposure.SetWindowText(strText);
            strText.Format("%.2f", Params.fLimitAngle);
            m_EditGodRayAngle.SetWindowText(strText);

            strText.Format("%d", Params.nSSAOLevel);
            m_EditSSAOLevel.SetWindowText(strText);
            strText.Format("%.2f", Params.fSSAOSampleRadius);
            m_EditSSAOSampRadius.SetWindowText(strText);
            strText.Format("%.2f", Params.fSSAODarkFact);
            m_EditSSAODark.SetWindowText(strText);
            strText.Format("%.2f", Params.fBlurWeight);
            m_EditSSAOBlurWeight.SetWindowText(strText);

			strText.Format("%.2f", Params.fSoftness);
			m_EditShadow_Softness.SetWindowText(strText);
			strText.Format("%.2f", Params.fJitter);
			m_EditShadow_Jitter.SetWindowText(strText);

			strText.Format("%.2f", Params.fHDRBrightGate);
			m_EditHDRBrightGate.SetWindowText(strText);
			strText.Format("%.2f", Params.fHDRRectify);
			m_EditHDRRectify.SetWindowText(strText);
			strText.Format("%.2f", Params.fHDRGrayLevel);
			m_EditHDRGrayLevel.SetWindowText(strText);
			strText.Format("%.2f", Params.fHDRBloomScale);
			m_EditHDRBloomScale.SetWindowText(strText);
			strText.Format("%.2f", Params.fHDRStarScale);
			m_EditHDRStarScale.SetWindowText(strText);
			m_CheckHDREnableBlueShift.SetCheck(Params.bHDREnableBlueShift);
			m_ComboHDRStarType.ResetContent();
			for (int i = 0; i < pWnd->GetHDRStarTypeCount(); ++i)
			{
				char szName[128];
				pWnd->GetHDRStarName(i, szName);
				m_ComboHDRStarType.AddString(szName);
			}

			ASSERT(pWnd->GetStypeCount() > 0);
			int nCurStarType = Params.nHDRStarType;
			if (nCurStarType <= -1)
				nCurStarType = 0;
			if (nCurStarType >= pWnd->GetHDRStarTypeCount())
				nCurStarType = 4;
			m_ComboHDRStarType.SetCurSel(nCurStarType);

        }
    }

    piManager->UnlockPostRenderParams();
    return nReslut;
}

void KSceneSceneEditorDialogPostRender::UpdateSelCurveCMYKStype(int index)
{
    KG3DEngineOption engineOption;
   // KG3DEngineEditorOption engineEditorOption;

    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    ASSERT(piManager);

    PostRenderParams& Params = *piManager->LockPostRenderParams(TRUE);
    
    piManager->GetEngineOption(&engineOption);
    //piManager->GetEngineEditorOption(&engineEditorOption);

    if (pWnd->SetActiveStype(index, true))
    {
        m_CurveCtrl.ResetPoint();

        for (int c = 0; c < 3; ++c)
        {
            for (int i = 0; i < pWnd->GetSplinePointCount(index, c); ++i)
            {
                const D3DXVECTOR2& pt = pWnd->GetSplinePoint(index, c, i);
                m_CurveCtrl.AddPoint(c, pt);
            }
        }
    }

    m_SliderKC.SetPos(static_cast<int>((Params.fKC + 1.f) * 100.f));
    m_SliderKM.SetPos(static_cast<int>((Params.fKM + 1.f) * 100.f));
    m_SliderKY.SetPos(static_cast<int>((Params.fKY + 1.f) * 100.f));

    piManager->UnlockPostRenderParams();
   
Exit0 :
    return;   
}

int KSceneSceneEditorDialogPostRender::EnableGodRayUI(int nEnable)
{
    m_EditGodRaySampleNum.EnableWindow(nEnable);
    m_EditGodRayDensity.EnableWindow(nEnable);
    m_EditGodRayWeight.EnableWindow(nEnable);
    m_EditGodRayDecay.EnableWindow(nEnable);
    m_EditGodRayExposure.EnableWindow(nEnable);
    m_EditGodRayAngle.EnableWindow(nEnable);
    return TRUE;
}

int KSceneSceneEditorDialogPostRender::EnableSSAOUI(int nEnable)
{

    return TRUE;
}

int KSceneSceneEditorDialogPostRender::EnableCurveCMKY(int nEnable)
{
    ::EnableWindow(GetDlgItem(IDC_COMBO_STYPE)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_BUTTON_CC_RENAME)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_BUTTON_CC_COPY)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_BUTTON_CC_DEL)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_BUTTON_CC_RESET)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_CHECK_R)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_CHECK_G)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_CHECK_B)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_BUTTON_CC_RESET_M)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_BUTTON_CC_RESET_Y)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_BUTTON_CC_RESET_K)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_SLIDER_KC)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_SLIDER_KM)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_SLIDER_KY)->GetSafeHwnd(), nEnable);
    ::EnableWindow(GetDlgItem(IDC_CURVE)->GetSafeHwnd(), nEnable);

    return TRUE;
}

int KSceneSceneEditorDialogPostRender::EnableShadowUI(int nEnable)
{
    m_chk_AreaShadow.EnableWindow(nEnable);
	m_EditShadow_Softness.EnableWindow(nEnable);
	m_EditShadow_Jitter.EnableWindow(nEnable);
    return TRUE;
}

int KSceneSceneEditorDialogPostRender::EnableDetailUI(int nEnable)
{
    m_EditDetailPower.EnableWindow(nEnable);
    m_EditBlurPower.EnableWindow(nEnable);
    m_EditBlurDistance.EnableWindow(nEnable);
	return TRUE;
}

int  KSceneSceneEditorDialogPostRender::EnableHDRUI(int nEnable)
{
	m_EditHDRBrightGate.EnableWindow(nEnable);
	m_EditHDRRectify.EnableWindow(nEnable);
	m_EditHDRGrayLevel.EnableWindow(nEnable);
	m_EditHDRBloomScale.EnableWindow(nEnable);
	m_EditHDRStarScale.EnableWindow(nEnable);
	m_ComboHDRStarType.EnableWindow(nEnable);
	m_CheckHDREnableBlueShift.EnableWindow(nEnable);
	return TRUE;
}


int KSceneSceneEditorDialogPostRender::EnableBloomUI(int nEnable)
{
    m_EditBloom_0.EnableWindow(nEnable);
    m_EditBloom_1.EnableWindow(nEnable);
    m_EditBloom_2.EnableWindow(nEnable);
    m_EditBloom_3.EnableWindow(nEnable);
    m_EditBloom_4.EnableWindow(nEnable);
    m_EditBloom_5.EnableWindow(nEnable);
    m_ComboBloomSamNum.EnableWindow(nEnable);
    return TRUE;
}

int KSceneSceneEditorDialogPostRender::EnableWaveUI(int nEnable)
{
    m_EditWave_0.EnableWindow(nEnable);
    return TRUE;
}

int KSceneSceneEditorDialogPostRender::EnableHSIUI(int nEnable)
{
    m_EditHSI_0.EnableWindow(nEnable);
    m_EditHSI_1.EnableWindow(nEnable);
    m_EditHSI_2.EnableWindow(nEnable);
    m_EditHSI_3.EnableWindow(nEnable);
    m_EditHSI_4.EnableWindow(nEnable);
    m_EditHSI_5.EnableWindow(nEnable);
    return TRUE;
}


void KSceneSceneEditorDialogPostRender::OnBnClickedCheckShockWave()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    BOOL bEnableWave  = m_checkWave.GetCheck();
    EnableWaveUI(bEnableWave);
    pWnd->EnablePostEffect(POST_EFFECT_SOCKWARE, bEnableWave);

}

void KSceneSceneEditorDialogPostRender::OnShowWindow(BOOL bShow, UINT nStatus)
{
	KGLilisSlipPanel<CDialog>::OnShowWindow(bShow, nStatus);
}

void KSceneSceneEditorDialogPostRender::OnCbnSelchangeComboMisc()
{
   
}

void KSceneSceneEditorDialogPostRender::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;


    if (!m_checkPostRender.GetCheck())
        return;

    CString strText;
    float   fValue;
    int     nValue;

    switch (pNMHDR->idFrom)
    {
    case IDC_SPIN_B0 :
        if (!m_checkBloom.GetCheck())
            return;
        m_EditBloom_0.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditBloom_0.SetWindowText(strText);
        pWnd->SetBloomGate(fValue);
        break;
    case IDC_SPIN_B1 :
        if (!m_checkBloom.GetCheck())
            return;
        m_EditBloom_1.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditBloom_1.SetWindowText(strText);
        pWnd->SetBloomExposure1(fValue);
        break;
    case IDC_SPIN_B2 :
        if (!m_checkBloom.GetCheck())
            return;
        m_EditBloom_2.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditBloom_2.SetWindowText(strText);
        pWnd->SetBloomExposure2(fValue);
        break;
    case IDC_SPIN_B3 :
        if (!m_checkBloom.GetCheck())
            return;
        m_EditBloom_3.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditBloom_3.SetWindowText(strText);
        pWnd->SetBloomGaussCoefficient(fValue);
        break;
    case IDC_SPIN_B4 :
        if (!m_checkBloom.GetCheck())
            return;
        m_EditBloom_4.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditBloom_4.SetWindowText(strText);
        pWnd->SetfBloomSampleNum((int)fValue);
        break;
    case IDC_SPIN_B5 :
        if (!m_checkBloom.GetCheck())
            return;
        m_EditBloom_5.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditBloom_5.SetWindowText(strText);
        pWnd->SetBloomDelaye(fValue);
        break;
    case IDC_SPIN_DETAIL_POWER :
        if (!m_checkDetail.GetCheck())
            return;
        m_EditDetailPower.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        if (fValue < 0.f || fValue > 2.f)
            break;
        strText.Format("%f", fValue);
        m_EditDetailPower.SetWindowText(strText);
        pWnd->SetDetailPower(fValue);
        break;
    case IDC_SPIN_BLUR_POWER :
        if (!m_checkDetail.GetCheck())
            return;
        m_EditBlurPower.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);

        fValue = fValue * -1.5f + 1.f;

        if (fValue > 1.f || fValue < -0.5f)
            break;

        m_EditBlurPower.SetWindowText(strText);
        pWnd->SetBlurPower(fValue);
        break;
    case IDC_SPIN_BLUR_DISTANCE :
        if (!m_checkDetail.GetCheck())
            return;
        m_EditBlurDistance.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 10.f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditBlurDistance.SetWindowText(strText);
        pWnd->SetBlurDistance(fValue * 100.f);
        break;
    case IDC_SPIN_H0 :
        if (!m_checkHSI.GetCheck())
            return;
        m_EditHSI_0.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditHSI_0.SetWindowText(strText);
        pWnd->SetHSIHueScale(fValue);
        break;
    case IDC_SPIN_H1 :
        if (!m_checkHSI.GetCheck())
            return;
        m_EditHSI_1.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditHSI_1.SetWindowText(strText);
        pWnd->SetHSIHueBias(fValue);
        break;
    case IDC_SPIN_H2 :
        if (!m_checkHSI.GetCheck())
            return;
        m_EditHSI_2.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditHSI_2.SetWindowText(strText);
        pWnd->SetHSIIntensityScale(fValue);
        break;
    case IDC_SPIN_H3 :
        if (!m_checkHSI.GetCheck())
            return;
        m_EditHSI_3.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditHSI_3.SetWindowText(strText);
        pWnd->SetHSISaturationBias(fValue);
        break;
    case IDC_SPIN_H4 :
        if (!m_checkHSI.GetCheck())
            return;
        m_EditHSI_4.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditHSI_4.SetWindowText(strText);
        pWnd->SetHSIIntensityScale(fValue);
        break;
    case IDC_SPIN_H5 :
        if (!m_checkHSI.GetCheck())
            return;
        m_EditHSI_5.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditHSI_5.SetWindowText(strText);
        pWnd->SetHSIIntensityBias(fValue);
        break;
    case IDC_SPIN_S0 :
        if (!m_checkWave.GetCheck())
            return;
        m_EditWave_0.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditWave_0.SetWindowText(strText);
        pWnd->SetShockWavePower(fValue);
        break;
    case IDC_SPIN_SAMPLE_NUM :
        if (!m_checkGodRay.GetCheck())
            return;
        m_EditGodRaySampleNum.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%d", &nValue);
        nValue -= 1 * pNMUpDown->iDelta;
        if (nValue < 0 || nValue > 5)
            break;
        strText.Format("%d", nValue);
        m_EditGodRaySampleNum.SetWindowText(strText);
        pWnd->SetGodRaySampleNum(nValue);
        break;
    case IDC_SPIN_DENSITY :
        if (!m_checkGodRay.GetCheck())
            return;
        m_EditGodRayDensity.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditGodRayDensity.SetWindowText(strText);
        pWnd->SetGodRayDensity(fValue);
        break;
    case IDC_SPIN_WEIGHT :
        if (!m_checkGodRay.GetCheck())
            return;
        m_EditGodRayWeight.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditGodRayWeight.SetWindowText(strText);
        pWnd->SetGodRayWeight(fValue);
        break;
    case IDC_SPIN_DECAY :
        if (!m_checkGodRay.GetCheck())
            return;
        m_EditGodRayDecay.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditGodRayDecay.SetWindowText(strText);
        pWnd->SetGodRayDecay(fValue);
        break;
    case IDC_SPIN_EXPOSURE :
        if (!m_checkGodRay.GetCheck())
            return;
        m_EditGodRayExposure.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditGodRayExposure.SetWindowText(strText);
        pWnd->SetGodRayExposure(fValue);
        break;
    case IDC_SPIN_RAYANGLE :
        if (!m_checkGodRay.GetCheck())
            return;
        m_EditGodRayAngle.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        if (fValue < 0.f || fValue > D3DX_PI * 2.f)
            break;
        strText.Format("%f", fValue);
        m_EditGodRayAngle.SetWindowText(strText);
        pWnd->SetGodRayAngle(fValue);
        break;
    case IDC_SPIN_SSAO_LEVEL :
        if (!m_checkSSAO.GetCheck())
            return;
        m_EditSSAOLevel.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%d", &nValue);
        nValue -= 1 * pNMUpDown->iDelta;
        if (nValue < 0 || nValue > 2)
            break;
        strText.Format("%d", nValue);
        m_EditSSAOLevel.SetWindowText(strText);
        pWnd->SetSSAOLevel(nValue);
        break;
    case IDC_SPIN_SSAO_RADIUS :
        if (!m_checkSSAO.GetCheck())
            return;
        m_EditSSAOSampRadius.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditSSAOSampRadius.SetWindowText(strText);
        pWnd->SetSSAOSampRadius(fValue);
        break;
    case IDC_SPIN_SSAO_DARK :
        if (!m_checkSSAO.GetCheck())
            return;
        m_EditSSAODark.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditSSAODark.SetWindowText(strText);
        pWnd->SetSSAODarkFact(fValue);
        break;
    case IDC_SPIN_SSAO_BLUR :
        if (!m_checkSSAO.GetCheck())
            return;
        m_EditSSAOBlurWeight.GetWindowText(strText);
        sscanf(strText.GetBuffer(), "%f", &fValue);
        fValue -= 0.01f * pNMUpDown->iDelta;
        strText.Format("%f", fValue);
        m_EditSSAOBlurWeight.SetWindowText(strText);
        pWnd->SetSSAOBlurWeight(fValue);
        break;
	case IDC_SPIN_SOFTNESS :
		if (!m_bnShadow.GetCheck())
			return;
		m_EditShadow_Softness.GetWindowText(strText);
		sscanf(strText.GetBuffer(), "%f", &fValue);
		fValue -= 0.01f * pNMUpDown->iDelta;
		strText.Format("%f", fValue);
		m_EditShadow_Softness.SetWindowText(strText);
		pWnd->SetShadowSoftness(fValue);
		break;
	case IDC_SPIN_JITTER :
		if (!m_bnShadow.GetCheck())
			return;
		m_EditShadow_Jitter.GetWindowText(strText);
		sscanf(strText.GetBuffer(), "%f", &fValue);
		fValue -= 0.01f * pNMUpDown->iDelta;
		strText.Format("%f", fValue);
		m_EditShadow_Jitter.SetWindowText(strText);
		pWnd->SetShadowJitter(fValue);
		break;
	case IDC_SPIN_HDR_BRIGHTGATE :
		if (!m_CheckHDR.GetCheck())
			return;
		m_EditHDRBrightGate.GetWindowText(strText);
		sscanf(strText.GetBuffer(), "%f", &fValue);
		fValue -= 0.01f * pNMUpDown->iDelta;
		strText.Format("%f", fValue);
		m_EditHDRBrightGate.SetWindowText(strText);
		pWnd->SetHDRBrightGate(fValue);
		break;
	case IDC_SPIN_HDR_RECTIFY :
		if (!m_CheckHDR.GetCheck())
			return;
		m_EditHDRRectify.GetWindowText(strText);
		sscanf(strText.GetBuffer(), "%f", &fValue);
		fValue -= 0.01f * pNMUpDown->iDelta;
		strText.Format("%f", fValue);
		m_EditHDRRectify.SetWindowText(strText);
		pWnd->SetHDRRectify(fValue);
		break;
	case IDC_SPIN_HDR_GRAYLEVEL :
		if (!m_CheckHDR.GetCheck())
			return;
		m_EditHDRGrayLevel.GetWindowText(strText);
		sscanf(strText.GetBuffer(), "%f", &fValue);
		fValue -= 0.01f * pNMUpDown->iDelta;
		strText.Format("%f", fValue);
		m_EditHDRGrayLevel.SetWindowText(strText);
		pWnd->SetHDRGrayLevel(fValue);
		break;
	case IDC_SPIN_HDR_BLOOMSCALE :
		if (!m_CheckHDR.GetCheck())
			return;
		m_EditHDRBloomScale.GetWindowText(strText);
		sscanf(strText.GetBuffer(), "%f", &fValue);
		fValue -= 0.01f * pNMUpDown->iDelta;
		strText.Format("%f", fValue);
		m_EditHDRBloomScale.SetWindowText(strText);
		pWnd->SetHDRBloomScale(fValue);
		break;
	case IDC_SPIN_HDR_STARSCALE :
		if (!m_CheckHDR.GetCheck())
			return;
		m_EditHDRStarScale.GetWindowText(strText);
		sscanf(strText.GetBuffer(), "%f", &fValue);
		fValue -= 0.01f * pNMUpDown->iDelta;
		strText.Format("%f", fValue);
		m_EditHDRStarScale.SetWindowText(strText);
		pWnd->SetHDRStarScale(fValue);
		break;
    default :
        break;
    }
    

    *pResult = 0;
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonOpenSFX()
{
    TCHAR szFullPath[MAX_PATH];
    g_GetFullPath(szFullPath, EFFECT_SFX_FILE);

    ShellExecute(
        this->GetSafeHwnd(),
        TEXT("open"),
        TEXT("notepad"),
        szFullPath,
        NULL,
        SW_SHOWNORMAL
        );
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckBloom()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    BOOL bEnableBloom = m_checkBloom.GetCheck();
    EnableBloomUI(bEnableBloom);
    pWnd->EnablePostEffect(POST_EFFECT_BLOOM, bEnableBloom);
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckHsi()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    BOOL bEnableHSI   = m_checkHSI.GetCheck();
    EnableHSIUI(bEnableHSI);
    pWnd->EnablePostEffect(POST_EFFECT_HSI, bEnableHSI);
}



void KSceneSceneEditorDialogPostRender::OnBnClickedCheckHsi2()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    BOOL bEnableBlur   = m_checkMotionBlur.GetCheck();
    
    pWnd->EnablePostEffect(POST_EFFECT_MOTIONBLUE, bEnableBlur);
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckDetail()
{
	IEKG3DSceneOutputWnd* pWnd = NULL;

	if (!m_lpSceneEditor)
		return;

	m_lpSceneEditor->GetCurOutputWnd(&pWnd);

	if (!pWnd)
		return;

	BOOL bEnableDetail   = m_checkDetail.GetCheck();
    EnableDetailUI(bEnableDetail);
	pWnd->EnablePostEffect(POST_EFFECT_DETAIL, bEnableDetail);
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckShadow()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    BOOL bEnableShadow   = m_bnShadow.GetCheck();
	EnableShadowUI(bEnableShadow);
    pWnd->EnablePostEffect(POST_EFFECT_SHADOW, bEnableShadow);
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckHDR()
{
	IEKG3DSceneOutputWnd* pWnd = NULL;

	if (!m_lpSceneEditor)
		return;

	m_lpSceneEditor->GetCurOutputWnd(&pWnd);

	if (!pWnd)
		return;

	BOOL bEnableHDR   = m_CheckHDR.GetCheck();
	EnableHDRUI(bEnableHDR);
	pWnd->EnablePostEffect(POST_EFFECT_HDR, bEnableHDR);
}
void KSceneSceneEditorDialogPostRender::OnBnClickedCheckEnableBlueShift()
{
	IEKG3DSceneOutputWnd* pWnd = NULL;

	if (!m_lpSceneEditor)
		return;

	m_lpSceneEditor->GetCurOutputWnd(&pWnd);
	_ASSERTE(pWnd);
	if (!pWnd)
		return;

	if (!m_CheckHDR.GetCheck())
		return;

	pWnd->SetHDREnableBlueShift(!!m_CheckHDREnableBlueShift.GetCheck());
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonOpensfx()
{
    // TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckGodray()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    BOOL bGodRay   = m_checkGodRay.GetCheck();
    EnableGodRayUI(bGodRay);
    pWnd->EnablePostEffect(POST_EFFECT_GODRAY, bGodRay);
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckSSAO()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    BOOL bSSAO   = m_checkSSAO.GetCheck();
    EnableSSAOUI(bSSAO);
    pWnd->EnablePostEffect(POST_EFFECT_SSAO, bSSAO);
}


void KSceneSceneEditorDialogPostRender::OnBnClickedCheckAreashadow()
{
	IEKG3DSceneOutputWnd* pWnd = NULL;

	if (!m_lpSceneEditor)
		return;

	m_lpSceneEditor->GetCurOutputWnd(&pWnd);
	_ASSERTE(pWnd);
	if (!pWnd)
		return;

	if (!m_bnShadow.GetCheck())
		return;

	pWnd->SetEnabledAreaShadow(!!m_chk_AreaShadow.GetCheck());
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckR()
{
    m_CurveCtrl.ShowSpline(0, !!IsDlgButtonChecked(IDC_CHECK_R));
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckG()
{
    m_CurveCtrl.ShowSpline(1, !!IsDlgButtonChecked(IDC_CHECK_G));
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckB()
{
    m_CurveCtrl.ShowSpline(2, !!IsDlgButtonChecked(IDC_CHECK_B));
}

void KSceneSceneEditorDialogPostRender::OnBnClickedCheckCurvecmyk()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    if (!m_lpSceneEditor)
        return;

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);

    if (!pWnd)
        return;

    BOOL bCurveCMYK  = m_checkCurveCMYK.GetCheck();
    EnableCurveCMKY(bCurveCMYK);
    pWnd->EnablePostEffect(POST_EFFECT_CURVE_CMYK, bCurveCMYK);
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButton1()
{
    // TODO: Add your control notification handler code here
}

void KSceneSceneEditorDialogPostRender::OnCbnSelchangeComboStype()
{
    int sel = m_ComboStype.GetCurSel();

    if (sel == CB_ERR)
        return;

    UpdateSelCurveCMYKStype(sel);
}

void KSceneSceneEditorDialogPostRender::OnCbnSelchangeComboHDRStarType()
{	
	IEKG3DSceneOutputWnd* pWnd = NULL;

	if (!m_lpSceneEditor)
		return;

	m_lpSceneEditor->GetCurOutputWnd(&pWnd);

	if (!pWnd)
		return;

	int sel = m_ComboHDRStarType.GetCurSel();
	if (sel == CB_ERR)
		return;
	pWnd->SetHDRStarType(sel);
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcCopy()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    int nActive = pWnd->CloneActiveStype();

    ASSERT(nActive != -1);

    char szName[128];
    pWnd->GetStypeName(nActive, szName);
    m_ComboStype.AddString(szName);

    m_ComboStype.SetCurSel(nActive);
    UpdateSelCurveCMYKStype(nActive);

Exit0 :
    return;
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcDel()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    int nActiveOld = pWnd->GetActiveStype();    
    int nActive = pWnd->DelActiveStype();

    if (nActive != -1)
    {
        if (nActiveOld != -1)
            m_ComboStype.DeleteString(nActiveOld);
        m_ComboStype.SetCurSel(nActive);
        UpdateSelCurveCMYKStype(nActive);
    }

Exit0 :
    return;
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcReset()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    DWORD dwResetBit = 
        (IsDlgButtonChecked(IDC_CHECK_R) ? CHANNEL_RED : 0)   | 
        (IsDlgButtonChecked(IDC_CHECK_G) ? CHANNEL_GREEN : 0) | 
        (IsDlgButtonChecked(IDC_CHECK_B) ? CHANNEL_BLUE : 0);

    int nActive = pWnd->ResetActiveStype(dwResetBit);
    
    ASSERT(nActive != -1);

    UpdateSelCurveCMYKStype(nActive);

Exit0 :
    return;
}

void KSceneSceneEditorDialogPostRender::OnCbnEditupdateComboStype()
{

//    IEKG3DSceneOutputWnd* pWnd = NULL;
//    KG_PROCESS_ERROR(m_lpSceneEditor);
//
//    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
//    KG_PROCESS_ERROR(pWnd);
//
//    char szName[128];
//    m_ComboStype.GetWindowText(szName, sizeof(szName));
//    pWnd->SetStypeName(pWnd->GetActiveStype(), szName);
//    m_ComboStype.DeleteString(pWnd->GetActiveStype());
//    m_ComboStype.InsertString(pWnd->GetActiveStype(), szName);
////    m_ComboStype.SetCurSel(pWnd->GetActiveStype());
//
//Exit0 :
//    return;
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcRename()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    KG_PROCESS_ERROR(pWnd->GetActiveStype() != -1);
    
    char szName[128];
    pWnd->GetStypeName(pWnd->GetActiveStype(), szName);

    if (InputBox("Please Enter The Name : ", this, szName) == IDOK)
    {       
        pWnd->SetStypeName(pWnd->GetActiveStype(), szName);
        m_ComboStype.DeleteString(pWnd->GetActiveStype());
        m_ComboStype.InsertString(pWnd->GetActiveStype(), szName);
        m_ComboStype.SetCurSel(pWnd->GetActiveStype());
    }

Exit0 :
    return;
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcResetM()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    m_SliderKC.SetPos(100);
    pWnd->SetCMYK_KC(0);

Exit0 :
    return;
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcResetK()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    m_SliderKM.SetPos(100);
    pWnd->SetCMYK_KM(0);

Exit0 :
    return;
}

void KSceneSceneEditorDialogPostRender::OnBnClickedButtonCcResetY()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;
    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    m_SliderKY.SetPos(100);
    pWnd->SetCMYK_KY(0);

Exit0 :
    return;

}

void KSceneSceneEditorDialogPostRender::OnCbnSelchangeBloomSamnum()
{
    IEKG3DSceneOutputWnd* pWnd = NULL;

    int sel = m_ComboBloomSamNum.GetCurSel();
    KG_PROCESS_ERROR(sel != CB_ERR);

    KG_PROCESS_ERROR(m_lpSceneEditor);

    m_lpSceneEditor->GetCurOutputWnd(&pWnd);
    KG_PROCESS_ERROR(pWnd);

    pWnd->SetfBloomSampleNum((sel + 1) * 2);


Exit0 :
    return;
}

HRESULT KSceneSceneEditorDialogPostRender::UpdateDataToInterface()
{
    HRESULT hResult = E_FAIL;
    IEKG3DEngineManager *piManager = g_GetEngineIEInterface();
    PostRenderParams& Params = *piManager->LockPostRenderParams(TRUE);

    if (m_lpSceneEditor)
    {
        IEKG3DSceneOutputWnd* pWnd = NULL;
        m_lpSceneEditor->GetCurOutputWnd(&pWnd);

        if (pWnd)
        {
            m_SliderKC.SetPos(static_cast<int>((Params.fKC + 1.f) * 100.f));
            m_SliderKM.SetPos(static_cast<int>((Params.fKM + 1.f) * 100.f));
            m_SliderKY.SetPos(static_cast<int>((Params.fKY + 1.f) * 100.f));

            CString strText;

            m_ComboBloomSamNum.SetCurSel(Params.nBloomSampleNum / 2 - 1);
            strText.Format("%.2f", Params.fBloomGate);
            m_EditBloom_0.SetWindowText(strText);
            strText.Format("%.2f", Params.fBloomExposure1);
            m_EditBloom_1.SetWindowText(strText);
            strText.Format("%.2f", Params.fBloomExposure2);
            m_EditBloom_2.SetWindowText(strText);
            strText.Format("%.2f", Params.fBloomGaussCoefficient);
            m_EditBloom_3.SetWindowText(strText);
            strText.Format("%d", Params.nBloomSampleNum);
            m_EditBloom_4.SetWindowText(strText);
            strText.Format("%.2f", Params.fBloomDelay);
            m_EditBloom_5.SetWindowText(strText);

            strText.Format("%.2f", Params.fDetailPower);
            m_EditDetailPower.SetWindowText(strText);
            strText.Format("%.2f", (Params.fBlurPower - 1.f) / -1.5f);
            m_EditBlurPower.SetWindowText(strText);
            strText.Format("%.2f", Params.fBlurDistance / 100.f);
            m_EditBlurDistance.SetWindowText(strText);

            strText.Format("%.2f", Params.fHueScale);
            m_EditHSI_0.SetWindowText(strText);
            strText.Format("%.2f", Params.fHueBias);
            m_EditHSI_1.SetWindowText(strText);
            strText.Format("%.2f", Params.fSaturationScale);
            m_EditHSI_2.SetWindowText(strText);
            strText.Format("%.2f", Params.fSaturationBias);
            m_EditHSI_3.SetWindowText(strText);
            strText.Format("%.2f", Params.fIntensityScale);
            m_EditHSI_4.SetWindowText(strText);
            strText.Format("%.2f", Params.fIntensityBias);
            m_EditHSI_5.SetWindowText(strText);

            strText.Format("%.2f", Params.fShockWavePower);
            m_EditWave_0.SetWindowText(strText);
            strText.Format("%d", Params.nGodRaySampleNum);
            m_EditGodRaySampleNum.SetWindowText(strText);
            strText.Format("%.2f", Params.fGodRayDensity);
            m_EditGodRayDensity.SetWindowText(strText);
            strText.Format("%.2f", Params.fGodRayWeight);
            m_EditGodRayWeight.SetWindowText(strText);
            strText.Format("%.2f", Params.fGodRayDecay);
            m_EditGodRayDecay.SetWindowText(strText);
            strText.Format("%.2f", Params.fGodRayExposure);
            m_EditGodRayExposure.SetWindowText(strText);
            strText.Format("%.2f", Params.fLimitAngle);
            m_EditGodRayAngle.SetWindowText(strText);

            strText.Format("%d", Params.nSSAOLevel);
            m_EditSSAOLevel.SetWindowText(strText);
            strText.Format("%.2f", Params.fSSAOSampleRadius);
            m_EditSSAOSampRadius.SetWindowText(strText);
            strText.Format("%.2f", Params.fSSAODarkFact);
            m_EditSSAODark.SetWindowText(strText);
            strText.Format("%.2f", Params.fBlurWeight);
            m_EditSSAOBlurWeight.SetWindowText(strText);

            strText.Format("%.2f", Params.fSoftness);
            m_EditShadow_Softness.SetWindowText(strText);
            strText.Format("%.2f", Params.fJitter);
            m_EditShadow_Jitter.SetWindowText(strText);

            strText.Format("%.2f", Params.fHDRBrightGate);
            m_EditHDRBrightGate.SetWindowText(strText);
            strText.Format("%.2f", Params.fHDRRectify);
            m_EditHDRRectify.SetWindowText(strText);
            strText.Format("%.2f", Params.fHDRGrayLevel);
            m_EditHDRGrayLevel.SetWindowText(strText);
            strText.Format("%.2f", Params.fHDRBloomScale);
            m_EditHDRBloomScale.SetWindowText(strText);
            strText.Format("%.2f", Params.fHDRStarScale);
            m_EditHDRStarScale.SetWindowText(strText);
            m_CheckHDREnableBlueShift.SetCheck(Params.bHDREnableBlueShift);
            m_ComboHDRStarType.ResetContent();
            for (int i = 0; i < pWnd->GetHDRStarTypeCount(); ++i)
            {
                char szName[128];
                pWnd->GetHDRStarName(i, szName);
                m_ComboHDRStarType.AddString(szName);
            }

            ASSERT(pWnd->GetStypeCount() > 0);
            int nCurStarType = Params.nHDRStarType;
            if (nCurStarType <= -1)
                nCurStarType = 0;
            if (nCurStarType >= pWnd->GetHDRStarTypeCount())
                nCurStarType = 4;
            m_ComboHDRStarType.SetCurSel(nCurStarType);

        }
    }

    piManager->UnlockPostRenderParams();

    hResult = S_OK;
// Exit0:
    return hResult;
}
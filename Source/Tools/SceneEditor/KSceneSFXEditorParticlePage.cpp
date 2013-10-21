////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorParticlePage.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:17:24
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorParticlePage.h"


////////////////////////////////////////////////////////////////////////////////
//#include "MainFrm.h" //by dengzhihui 2006年7月6日 14:45:13
#include "SceneEditor.h"
#include "KSceneSFXEditorParticlePage.h"
#include "IEKG3DSceneSFXEditor.h"
//#include "KG3DSceneEditorBase.h"
#include "IEKG3DSFX.h"
#include "KSceneSFXEditorTimeView.h"
#include "KSceneSFXEditorFrame.h"
#include "KColorDialog.h"
#include "KSceneSFXEditorLauncherSelectDlg.h"
#include "KSceneHelperFuncs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


#define LAUNCHER_SHAPE_RECT_INDEX       0
#define LAUNCHER_SHAPE_CIRCLE_INDEX     1
#define LAUNCHER_SHAPE_SPHERE_INDEX     2
#define LAUNCHER_SHAPE_CYLINDER_INDEX   3

#define LAUNCHER_BILLBOARD_POINT_INDEX      0
#define LAUNCHER_BILLBOARD_ORIENTSELF_INDEX 1
#define LAUNCHER_LEAVES_INDEX               2
#define LAUNCHER_BILLBOARD_WORLD_LOCK_Y           3
#define LAUNCHER_BILLBOARD_WORLD_LOCK_Z           4
#define LAUNCHER_BILLBOARD_LOCAL_LOCK_Y           5
#define LAUNCHER_BILLBOARD_LOCAL_LOCK_Z           6

#define PARTICLE_UPDATE_FOLLOW_SFX_INDEX  0
#define PARTICLE_UPDATE_SELF_INDEX        1

#define FORCE_FIELD_G_INDEX             0
#define FORCE_FIELD_WIND_INDEX          1
#define FORCE_FIELD_WHIRLWIND_INDEX     2

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(KSceneSFXEditorParticlePageAUT, CPropertyPage)

KSceneSFXEditorParticlePageAUT::KSceneSFXEditorParticlePageAUT()
    : CPropertyPage(KSceneSFXEditorParticlePageAUT::IDD)
{
}

KSceneSFXEditorParticlePageAUT::~KSceneSFXEditorParticlePageAUT()
{
}

BEGIN_MESSAGE_MAP(KSceneSFXEditorParticlePageAUT, CPropertyPage)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL KSceneSFXEditorParticlePageAUT::PreTranslateMessage(MSG* pMsg)
{
   /* if (pMsg->message == WM_MOUSEWHEEL)
        m_particlePage.PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);*/
    return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL KSceneSFXEditorParticlePageAUT::OnInitDialog()
{
    int nResult = CPropertyPage::OnInitDialog();

    m_particlePage.Create(IDD_SFXEDITOR_PARTICLE, this);
    m_particlePage.ShowWindow(SW_SHOW);
    m_particlePage.UpdateUI();

    return nResult;
}

BOOL KSceneSFXEditorParticlePageAUT::OnSetActive()
{
    m_particlePage.OnSetActive();
    return CPropertyPage::OnSetActive();
}

int KSceneSFXEditorParticlePageAUT::UpdateUI()
{
    m_particlePage.UpdateUI();
    return TRUE;
}



void KSceneSFXEditorParticlePageAUT::OnSize(UINT nType, int cx, int cy)
{
    CPropertyPage::OnSize(nType, cx, cy);

    RECT rectClient;
    GetClientRect(&rectClient);

    if (::IsWindow(m_particlePage.m_hWnd))
        m_particlePage.MoveWindow(&rectClient);
}



BOOL CLauCheckCtl::PreTranslateMessage(MSG* pMsg)
{
    BOOL r = CButton::PreTranslateMessage(pMsg);

    if (pMsg->message == WM_LBUTTONUP)
    {
        GET_SFX_EDITOR_RET(r);
        int nSelect = pPage->m_comboForceField.GetCurSel();
        if (nSelect == CB_ERR)
            return r;
        
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);

        IEKG3DSFXParticleForceField* pForce    = NULL;
    
        if (!pSFX)
            return r;

        pSFX->GetForceField(nSelect, &pForce);

        if (!pForce)
            return r;

        if (GetCheck())
        {
            pForce->DelLauncherRef(pLauncher);
            pLauncher->DelEffForce(pForce);
        }
        else
        {
            pForce->AddLauncherRef(pLauncher);
            pLauncher->AddEffForce(pForce);
        }
    }
    return r;
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KSceneSFXEditorParticlePage, KGLilisPanelDLG)

KSceneSFXEditorParticlePage::KSceneSFXEditorParticlePage(CWnd* pParent /* = NULL */)
	: KGLilisPanelDLG(KSceneSFXEditorParticlePage::IDD, pParent)
{
    m_ColorL = D3DXCOLOR( 0, 0, 0, 0 );
    m_ColorR = D3DXCOLOR( 0, 0, 0, 0 );
}

KSceneSFXEditorParticlePage::~KSceneSFXEditorParticlePage()
{
    DWORD dwBnNum = (DWORD)m_vecCheckLaus.size();
    for (DWORD i = 0; i < dwBnNum; i++)
    {
        m_vecCheckLaus[i]->DestroyWindow();
        delete m_vecCheckLaus[i];
    }
    m_vecCheckLaus.clear();
}

void KSceneSFXEditorParticlePage::DoDataExchange(CDataExchange* pDX)
{
    KGLilisPanelDLG::DoDataExchange(pDX);

    // Particle kind control
    DDX_Control(pDX, IDC_ADD_PARTICLE_KIND, m_buttonAddParticleKind);
    DDX_Control(pDX, IDC_DEL_PARTICLE_KIND, m_buttonDelParticleKind);
    DDX_Control(pDX, IDC_EDIT_ROTATION_FACTOR,m_editRotationFactor);
    DDX_Control(pDX, IDC_COMBO_PARTICLE_KIND, m_comboParticleKind);
    DDX_Control(pDX, IDC_COMBO_TEXTURE_INDEX, m_comboTextureIndex);
    DDX_Control(pDX, IDC_STATIC_TYPE, m_sParticleType);
    DDX_Control(pDX, IDC_COMBO_TYPE, m_comboParticleType);
    DDX_Control(pDX, IDC_RADIO_METORE_KEEP, m_redioMeteorDelay);
    DDX_Control(pDX, IDC_RADIO_COLOR_L, m_radioColorL);
    DDX_Control(pDX, IDC_RADIO_COLOR_R, m_radioColorR);
    DDX_Control(pDX, IDC_RADIO_WIDTH, m_radioWidth);
    DDX_Control(pDX, IDC_RADIO_HEIGHT, m_radioHeight);
    DDX_Control(pDX, IDC_RADIO_ROTATION_MIN, m_radioRotationMin);
    DDX_Control(pDX, IDC_RADIO_ROTATION_MAX, m_radioRotationMax);
    DDX_Control(pDX, IDC_RATATION_MIN, m_editRotationMin);
    DDX_Control(pDX, IDC_RATATION_MAX, m_editRotationMax);
    DDX_Control(pDX, IDC_RADIO_TEXTURE_FRAME_INDEX, m_radioTextureFrameIndex);
    DDX_Control(pDX, IDC_COLOR_PIC_L, m_staticColorL);
    DDX_Control(pDX, IDC_COLOR_PIC_R, m_staticColorR);
    DDX_Control(pDX, IDC_EDIT_KEY_WIDTH, m_editWidth);
    DDX_Control(pDX, IDC_EDIT_KEY_HEIGHT, m_editHeight);
    //DDX_Control(pDX, IDC_EDIT_KEY_TEXTURE_FRAME_INDEX, m_editTextureFrameIndex);
    DDX_Control(pDX, IDC_RAND_DELAY_TEX, m_editRandDelayTex);
    DDX_Control(pDX, IDC_RAND_DELAY_COL, m_editRandDelayCol);
    DDX_Control(pDX, IDC_RAND_DELAY_ALP, m_editRandDelayAlp);
    DDX_Control(pDX, IDC_RAND_DELAY_ROT, m_editRandDelayRot);
    DDX_Control(pDX, IDC_T_FRAME_KEY, m_comboxTexFrame);
    DDX_Control(pDX, IDC_RAND_RT, m_stRandRotation);
    DDX_Control(pDX, IDC_TEXT_INDEX, m_stTextIndex);

    // Launcher control
    DDX_Control(pDX, IDC_COMBO_LAUNCHER_PARTICLE_KIND, m_comboLauncherParticleKind);
    DDX_Control(pDX, IDC_COMBO_LAUNCHER_SHAPE, m_comboLauncherShape);
    DDX_Control(pDX, IDC_COMBO_PARTICLE_TYPE, m_comboLauncherType);
    DDX_Control(pDX, IDC_COMBO_LAUNCHER, m_comboLauncher);
    DDX_Control(pDX, IDC_COMBO_PARTICLE_UPDATE_MODE, m_comboParticleUpdateMode);

    DDX_Control(pDX, IDC_EDIT_LAUNCHER_PARAM1, m_editLauncherParam1);
    DDX_Control(pDX, IDC_EDIT_LAUNCHER_PARAM2, m_editLauncherParam2);

    DDX_Control(pDX, IDC_BUTTON_ADD_LAUNCHER, m_buttonAddLauncher);
    DDX_Control(pDX, IDC_BUTTON_DEL_LAUNCHER, m_buttonDelLauncher);
    DDX_Control(pDX, IDC_EDIT_LAUNCHER_NAME, m_editLauncherName);

    DDX_Control(pDX, IDC_RADIO_PARTICLE_NUM, m_radioParticleNum);
    DDX_Control(pDX, IDC_RADIO_MIN_SPEED, m_radioMinSpeed);
    DDX_Control(pDX, IDC_RADIO_MAX_SPEED, m_radioMaxSpeed);
    DDX_Control(pDX, IDC_RADIO_MIN_LIFE, m_radioMinLife);
    DDX_Control(pDX, IDC_RADIO_MAX_LIFE, m_radioMaxLife);
    DDX_Control(pDX, IDC_RADIO_SCATTER_MIN, m_radioMinScatter);
    DDX_Control(pDX, IDC_RADIO_SCATTER_MAX, m_radioMaxScatter);

    DDX_Control(pDX, IDC_EDIT_KEY_PARTICLE_NUM, m_editParticleNum);
    DDX_Control(pDX, IDC_EDIT_KEY_MIN_SPEED, m_editMinSpeed);
    DDX_Control(pDX, IDC_EDIT_KEY_MAX_SPEED, m_editMaxSpeed);
    DDX_Control(pDX, IDC_EDIT_KEY_MIN_LIFE, m_editMinLife);
    DDX_Control(pDX, IDC_EDIT_KEY_MAX_LIFE, m_editMaxLife);
    DDX_Control(pDX, IDC_EDIT_KEY_SCATTER_MIN, m_editMinScatter);
    DDX_Control(pDX, IDC_EDIT_KEY_SCATTER_MAX, m_editMaxScatter);

    DDX_Control(pDX, IDC_COMBO_BLEND_MODE, m_comboBlendMode);
    DDX_Control(pDX, IDC_COMBO_MOTION_TYPE, m_comboMotionType);
    DDX_Control(pDX, IDC_STATIC_LNAME, m_staticLable1);
    DDX_Control(pDX, IDC_STATIC_PKIND, m_staticLable2);
    DDX_Control(pDX, IDC_STATIC_SHAPE, m_staticLable3);
    DDX_Control(pDX, IDC_STATIC_KINDS, m_staticLable4);
    DDX_Control(pDX, IDC_STATIC_MODES, m_staticLable5);
    DDX_Control(pDX, IDC_P1, m_staticLable6);
    DDX_Control(pDX, IDC_P2, m_staticLable7);
    DDX_Control(pDX, IDC_STATIC_MUSS, m_staticLable8);
    DDX_Control(pDX, IDC_STATIC_DELAY, m_staticLable9);
    DDX_Control(pDX, IDC_STATIC_MUSS2, m_staticLable10);
    DDX_Control(pDX, IDC_STATIC_DELAY_TEX, m_staticLableDelayTex);
    DDX_Control(pDX, IDC_STATIC_DELAY_COL, m_staticLableDelayCol);
    DDX_Control(pDX, IDC_STATIC_DELAY_ALP, m_staticLableDelayAlp);
    DDX_Control(pDX, IDC_STATIC_DELAY_ROT, m_staticLableDelayRot);

    DDX_Control(pDX, IDC_STATIC_T, m_staticLableT);
    DDX_Control(pDX, IDC_STATIC_FORCE_PARAM, m_staticLableL);
    DDX_Control(pDX, IDC_STATIC_FORCE_PARAM_EX, m_staticLableEx);
    DDX_Control(pDX, IDC_STATIC_RD, m_staticLableRD);
    DDX_Control(pDX, IDC_STATIC_X, m_staticLableX);
    DDX_Control(pDX, IDC_STATIC_Y, m_staticLableY);
    DDX_Control(pDX, IDC_STATIC_Z, m_staticLableZ);

    // Force field control
    DDX_Control(pDX, IDC_COMBO_FORCE_FIELD_LIST, m_comboForceField);

    DDX_Control(pDX, IDC_BUTTON_ADD_FORCE_FIELD, m_buttonAddForceField);
    DDX_Control(pDX, IDC_BUTTON_DEL_FORCE_FIELD, m_buttonDelForceField);

    DDX_Control(pDX, IDC_COMBO_FOURCE_TYPE, m_comboForceFieldType);

    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_FACTOR1, m_editForceFieldFactor1);
    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_FACTOR_EX, m_editForceFieldFactorEx);
    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_RD, m_editForceFieldFactorRD);
    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_X, m_editForceFieldX);
    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_Y, m_editForceFieldY);
    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_Z, m_editForceFieldZ);

    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_FACTOR2, m_editForceFieldFactor2);
    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_X2, m_editForceFieldX2);
    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_Y2, m_editForceFieldY2);
    DDX_Control(pDX, IDC_EDIT_FORCE_FIELD_Z2, m_editForceFieldZ2);

    DDX_Control(pDX, IDC_EFFICE_LAU,         m_staticEffLau);
    DDX_Control(pDX, IDC_SLIDER, m_sliderSmooth);
    DDX_Control(pDX, IDC_STATIC_SMOOTH, m_staticMooth);
    DDX_Control(pDX, IDC_STATIC_SMOOTH_LOW, m_staticMooth1);
    DDX_Control(pDX, IDC_STATIC_SMOOTH_HIGH, m_staticMooth2);
}


BEGIN_MESSAGE_MAP(KSceneSFXEditorParticlePage, KGLilisPanelDLG)
    ON_BN_CLICKED(IDC_ADD_PARTICLE_KIND, OnBnClickedButtonAddParticleKind)
    ON_BN_CLICKED(IDC_DEL_PARTICLE_KIND, OnBnClickedDelParticleKind)
    ON_BN_CLICKED(IDC_RADIO_COLOR_L, OnBnClickedRadioColorL)
    ON_BN_CLICKED(IDC_RADIO_COLOR_R, OnBnClickedRadioColorR)
    ON_BN_CLICKED(IDC_RADIO_WIDTH, OnBnClickedRadioWidth)
    ON_BN_CLICKED(IDC_RADIO_HEIGHT, OnBnClickedRadioHeight)
	ON_BN_CLICKED(IDC_RADIO_ROTATION_MIN, OnBnClickedRadioRotationMin)
	ON_BN_CLICKED(IDC_RADIO_ROTATION_MAX, OnBnClickedRadioRotationMax)
    ON_BN_CLICKED(IDC_RADIO_TEXTURE_FRAME_INDEX, OnBnClickedRadioTextureFrameIndex)
    ON_BN_CLICKED(IDC_RADIO_METORE_KEEP, OnCbnMeteorDelay)

    ON_STN_CLICKED(IDC_COLOR_PIC_L, OnStnClickedColorPicL)
    ON_STN_CLICKED(IDC_COLOR_PIC_R, OnStnClickedColorPicR)
    ON_CBN_SELCHANGE(IDC_COMBO_TEXTURE_INDEX, OnCbnSelchangeComboTextureIndex)
    ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)

    ON_BN_CLICKED(IDC_BUTTON_ADD_LAUNCHER, OnBnClickedAddLauncher)
    ON_BN_CLICKED(IDC_BUTTON_DEL_LAUNCHER, OnBnClickedDelLauncher)

    ON_CBN_SELCHANGE(IDC_COMBO_PARTICLE_KIND, OnCbnSelchangeComboParticleKind)
    ON_CBN_SELCHANGE(IDC_COMBO_LAUNCHER, OnCbnSelchangeComboLauncher)
    ON_CBN_SELCHANGE(IDC_COMBO_LAUNCHER_PARTICLE_KIND, OnCbnSelchangeComboLauncherParticleKind)
    ON_CBN_SELCHANGE(IDC_COMBO_LAUNCHER_SHAPE, OnCbnSelchangeComboLauncherShape)
    ON_CBN_SELCHANGE(IDC_COMBO_PARTICLE_TYPE, OnCbnSelchangeComboParticleType)
    ON_CBN_SELCHANGE(IDC_COMBO_PARTICLE_UPDATE_MODE, OnCbnSelchangeComboParticleUpdateMode)
    ON_CBN_SELCHANGE(IDC_COMBO_BLEND_MODE, OnCbnSelchangeComboBlendMode)
    ON_CBN_SELCHANGE(IDC_COMBO_MOTION_TYPE, OnCbnSelchangeComboMotionType)

    ON_BN_CLICKED(IDC_RADIO_PARTICLE_NUM, OnBnClickedRadioParticleNum)
    ON_BN_CLICKED(IDC_RADIO_MIN_SPEED, OnBnClickedRadioMinSpeed)
    ON_BN_CLICKED(IDC_RADIO_MAX_SPEED, OnBnClickedRadioMaxSpeed)
    ON_BN_CLICKED(IDC_RADIO_MIN_LIFE, OnBnClickedRadioMinLife)
    ON_BN_CLICKED(IDC_RADIO_MAX_LIFE, OnBnClickedRadioMaxLife)
    ON_BN_CLICKED(IDC_RADIO_SCATTER_MIN, OnBnClickedRadioScatterMin)
    ON_BN_CLICKED(IDC_RADIO_SCATTER_MAX, OnBnClickedRadioScatterMax)

    ON_BN_CLICKED(IDC_BUTTON_ADD_FORCE_FIELD, OnBnClickedButtonAddForceField)
    ON_BN_CLICKED(IDC_BUTTON_DEL_FORCE_FIELD, OnBnClickedButtonDelForceField)
    ON_CBN_SELCHANGE(IDC_COMBO_FORCE_FIELD_LIST, OnCbnSelchangeComboForceFieldList)
    ON_CBN_SELCHANGE(IDC_COMBO_FOURCE_TYPE, OnCbnSelchangeComboForceFieldType)
    ON_EN_SETFOCUS(IDC_EDIT_KEY_WIDTH, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyWidth)
    ON_EN_SETFOCUS(IDC_EDIT_KEY_HEIGHT, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyHeight)
	ON_EN_SETFOCUS(IDC_RATATION_MIN, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyRotationMin)
	ON_EN_SETFOCUS(IDC_RATATION_MAX, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyRotationMax)
    //ON_EN_SETFOCUS(IDC_EDIT_KEY_TEXTURE_FRAME_INDEX, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyTextureFrameIndex)
	ON_CBN_SETFOCUS(IDC_T_FRAME_KEY, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyTextureFrameIndex)
	ON_EN_SETFOCUS(IDC_EDIT_KEY_PARTICLE_NUM, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyParticleNum)
    ON_EN_SETFOCUS(IDC_EDIT_KEY_MIN_SPEED, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyMinSpeed)
    ON_EN_SETFOCUS(IDC_EDIT_KEY_MAX_SPEED, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyMaxSpeed)
    ON_EN_SETFOCUS(IDC_EDIT_KEY_MIN_LIFE, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyMinLife)
    ON_EN_SETFOCUS(IDC_EDIT_KEY_MAX_LIFE, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyMaxLife)
    ON_EN_SETFOCUS(IDC_EDIT_KEY_SCATTER_MIN, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyScatterMin)
    ON_EN_SETFOCUS(IDC_EDIT_KEY_SCATTER_MAX, &KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyScatterMax)
    ON_CBN_DROPDOWN(IDC_COMBO_LAUNCHER, &KSceneSFXEditorParticlePage::OnCbnDropdownComboLauncher)
    ON_CBN_DROPDOWN(IDC_COMBO_PARTICLE_KIND, &KSceneSFXEditorParticlePage::OnCbnDropdownComboParticleKind)
    ON_CBN_DROPDOWN(IDC_COMBO_FORCE_FIELD_LIST, &KSceneSFXEditorParticlePage::OnCbnDropdownComboForceFieldList)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_X, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPower)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_Y, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPower)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_Z, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPower)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_FACTOR1, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPower)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_FACTOR_EX, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPower)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_X2, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPos)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_Y2, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPos)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_Z2, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPos)
    ON_EN_SETFOCUS(IDC_EDIT_FORCE_FIELD_FACTOR2, &KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldFactor2)
	ON_CBN_SELCHANGE(IDC_T_FRAME_KEY, &KSceneSFXEditorParticlePage::OnCbnSelchangeTFrameKey)
    ON_WM_HSCROLL()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH KSceneSFXEditorParticlePage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBursh = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX)
    {
        static HBRUSH hFocus = ::CreateSolidBrush(RGB(240, 200, 85));
        static HBRUSH hNorml = ::CreateSolidBrush(RGB(210, 210, 233));
        COLORREF cFocus = RGB(240, 200, 85);
        COLORREF cNorml = RGB(210, 210, 233);

        if (::GetFocus() == pWnd->m_hWnd)
        {
            hBursh = hFocus;
            pDC->SetBkColor(cFocus);
        }
        else
        {
            hBursh = hNorml;
            pDC->SetBkColor(cNorml);
        }
    }


    if (pWnd == &m_staticColorL || pWnd == &m_staticColorR)
    {
        GET_SFX_EDITOR_RET(hBursh);
        GET_LIFE_TIME_LINE_RET(hBursh);
        int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
        UpdateColor(pLifeTime, nFrame);
    }

    return hBursh;
}


BOOL KSceneSFXEditorParticlePage::OnInitDialog()
{
    KGLilisPanelDLG::OnInitDialog();

    // TODO:  Add extra initialization here
    m_comboBlendMode.InsertString(SFX_BM_SRCALPHA_INVSRCALPHA, "Alpha");
    m_comboBlendMode.InsertString(SFX_BM_SRCALPHA_ONE, "Addition");
    m_comboBlendMode.InsertString(SFX_BM_SRCALPHA_INVSRCCOLOR, "Subtriction");
    m_comboBlendMode.InsertString(SFX_BM_SRCALPHA_SCREEN, "Screen");

    m_comboMotionType.AddString("世界");
    m_comboMotionType.AddString("特效");
    m_comboMotionType.AddString("自身");

    m_comboParticleType.AddString("公告版");
    m_comboParticleType.AddString("拖  尾");

    m_comboLauncherShape.InsertString(LAUNCHER_SHAPE_RECT_INDEX, "矩形");
    m_comboLauncherShape.InsertString(LAUNCHER_SHAPE_CIRCLE_INDEX, "圆形");
    m_comboLauncherShape.InsertString(LAUNCHER_SHAPE_SPHERE_INDEX, "球形");
    m_comboLauncherShape.InsertString(LAUNCHER_SHAPE_CYLINDER_INDEX, "圆柱");

    m_comboLauncherType.InsertString(LAUNCHER_BILLBOARD_POINT_INDEX, "完全面向眼睛的公告板");
    m_comboLauncherType.InsertString(LAUNCHER_BILLBOARD_ORIENTSELF_INDEX, "基于速度方向的公告板");
    m_comboLauncherType.InsertString(LAUNCHER_LEAVES_INDEX, "树叶");
    m_comboLauncherType.InsertString(LAUNCHER_BILLBOARD_WORLD_LOCK_Y, "锁世界Y轴公告板");
    m_comboLauncherType.InsertString(LAUNCHER_BILLBOARD_WORLD_LOCK_Z, "锁世界Z轴公告板");
    m_comboLauncherType.InsertString(LAUNCHER_BILLBOARD_LOCAL_LOCK_Y, "锁自身Y轴公告板");
    m_comboLauncherType.InsertString(LAUNCHER_BILLBOARD_LOCAL_LOCK_Z, "锁自身Z轴公告板");

    m_comboParticleUpdateMode.InsertString(PARTICLE_UPDATE_FOLLOW_SFX_INDEX, "粒子整体更新");
    m_comboParticleUpdateMode.InsertString(PARTICLE_UPDATE_SELF_INDEX, "粒子自己更新");

    //m_comboForceFieldType.InsertString(FORCE_FIELD_G_INDEX, "重力");
    //m_comboForceFieldType.InsertString(FORCE_FIELD_WIND_INDEX, "风力");
    //m_comboForceFieldType.InsertString(FORCE_FIELD_WHIRLWIND_INDEX, "旋风");

    m_sliderSmooth.SetRange(0, 1000, TRUE);

    m_comboForceFieldType.AddString("重力");
    m_comboForceFieldType.AddString("风力");
    m_comboForceFieldType.AddString("聚合");
    m_comboForceFieldType.AddString("旋风");
    m_comboForceFieldType.AddString("布尔");


    // Oh Yesh :)
    RECT rectClient;
    GetClientRect(&rectClient);

    // kind
    RECT rect_k = {rectClient.left, rectClient.top, rectClient.right, 200};
    KGLiliVirtualWnd vWnd_1(this);
    vWnd_1.AddElement(&m_buttonAddParticleKind);
    vWnd_1.AddElement(&m_buttonDelParticleKind);
    //vWnd_1.AddElement(&m_editRotationFactor);
    vWnd_1.AddElement(&m_sParticleType);
    vWnd_1.AddElement(&m_comboParticleType);
    vWnd_1.AddElement(&m_redioMeteorDelay);
    vWnd_1.AddElement(&m_staticMooth);
    vWnd_1.AddElement(&m_staticMooth1);
    vWnd_1.AddElement(&m_staticMooth2);
    vWnd_1.AddElement(&m_sliderSmooth);
    vWnd_1.AddElement(&m_radioColorL);
    vWnd_1.AddElement(&m_radioColorR);
    vWnd_1.AddElement(&m_radioWidth);
    vWnd_1.AddElement(&m_radioHeight);
    vWnd_1.AddElement(&m_radioRotationMin);
    vWnd_1.AddElement(&m_radioRotationMax);
    vWnd_1.AddElement(&m_radioTextureFrameIndex);
    vWnd_1.AddElement(&m_staticColorL);
    vWnd_1.AddElement(&m_staticColorR);
    vWnd_1.AddElement(&m_comboTextureIndex);
    vWnd_1.AddElement(&m_editWidth);
    vWnd_1.AddElement(&m_editHeight);
    vWnd_1.AddElement(&m_editRotationMin);
    vWnd_1.AddElement(&m_editRotationMax);
    vWnd_1.AddElement(&m_comboxTexFrame);
    vWnd_1.AddElement(&m_comboParticleKind);
    vWnd_1.AddElement(&m_stTextIndex);
    AddPanel(vWnd_1, "粒子类型", TRUE);

    RECT rect_l = {rectClient.left, 200, rectClient.right, 615};
    KGLiliVirtualWnd vWnd_2(this);
    vWnd_2.AddElement(&m_comboLauncherParticleKind);
    vWnd_2.AddElement(&m_comboLauncherShape);
    vWnd_2.AddElement(&m_comboLauncherType);
    vWnd_2.AddElement(&m_comboLauncher);
    vWnd_2.AddElement(&m_comboParticleUpdateMode);
    vWnd_2.AddElement(&m_editLauncherParam1);
    vWnd_2.AddElement(&m_editLauncherParam2);
    vWnd_2.AddElement(&m_buttonAddLauncher);
    vWnd_2.AddElement(&m_buttonDelLauncher);
    vWnd_2.AddElement(&m_editLauncherName);
    vWnd_2.AddElement(&m_radioParticleNum);
    vWnd_2.AddElement(&m_radioMinSpeed);
    vWnd_2.AddElement(&m_radioMaxSpeed);
    vWnd_2.AddElement(&m_radioMinLife);
    vWnd_2.AddElement(&m_radioMaxLife);
    vWnd_2.AddElement(&m_radioMinScatter);
    vWnd_2.AddElement(&m_radioMaxScatter);
    vWnd_2.AddElement(&m_editParticleNum);
    vWnd_2.AddElement(&m_editMinSpeed);
    vWnd_2.AddElement(&m_editMaxSpeed);
    vWnd_2.AddElement(&m_editMinLife);
    vWnd_2.AddElement(&m_editMaxLife);
    vWnd_2.AddElement(&m_editMinScatter);
    vWnd_2.AddElement(&m_editMaxScatter);
    vWnd_2.AddElement(&m_comboBlendMode);
    vWnd_2.AddElement(&m_comboMotionType);
    vWnd_2.AddElement(&m_editRandDelayTex);
    vWnd_2.AddElement(&m_editRandDelayAlp);
    vWnd_2.AddElement(&m_editRandDelayCol);
    vWnd_2.AddElement(&m_editRandDelayRot);
    vWnd_2.AddElement(&m_staticLable1);
    vWnd_2.AddElement(&m_staticLable2);
    vWnd_2.AddElement(&m_staticLable3);
    vWnd_2.AddElement(&m_staticLable4);
    vWnd_2.AddElement(&m_staticLable5);
    vWnd_2.AddElement(&m_staticLable6);
    vWnd_2.AddElement(&m_staticLable7);
    vWnd_2.AddElement(&m_staticLable8);
    vWnd_2.AddElement(&m_staticLable9);
    vWnd_2.AddElement(&m_staticLable10);
    vWnd_2.AddElement(&m_staticLableDelayTex);
    vWnd_2.AddElement(&m_staticLableDelayCol);
    vWnd_2.AddElement(&m_staticLableDelayAlp);
    vWnd_2.AddElement(&m_staticLableDelayRot);
    vWnd_2.AddElement(&m_editForceFieldFactorRD);
    vWnd_2.AddElement(&m_staticLableRD);
    AddPanel(vWnd_2, "发射器", TRUE);

    RECT rect_p = {rectClient.left, 615, rectClient.right, 1000};
    KGLiliVirtualWnd vWnd_3(this);
    vWnd_3.AddElement(&m_comboForceField);
    vWnd_3.AddElement(&m_buttonDelForceField);
    vWnd_3.AddElement(&m_buttonAddForceField);
    vWnd_3.AddElement(&m_comboForceFieldType);
    vWnd_3.AddElement(&m_editForceFieldFactor1);
    vWnd_3.AddElement(&m_editForceFieldFactorEx);
    vWnd_3.AddElement(&m_editForceFieldX);
    vWnd_3.AddElement(&m_editForceFieldY);
    vWnd_3.AddElement(&m_editForceFieldZ);
    vWnd_3.AddElement(&m_editForceFieldFactor2);
    vWnd_3.AddElement(&m_editForceFieldX2);
    vWnd_3.AddElement(&m_editForceFieldY2);
    vWnd_3.AddElement(&m_editForceFieldZ2);
    vWnd_3.AddElement(&m_staticLableT);
    vWnd_3.AddElement(&m_staticLableL);
    vWnd_3.AddElement(&m_staticLableEx);
    vWnd_3.AddElement(&m_staticLableX);
    vWnd_3.AddElement(&m_staticLableY);
    vWnd_3.AddElement(&m_staticLableZ);
    vWnd_3.AddElement(&m_staticEffLau);

    AddPanel(vWnd_3, "力场", TRUE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneSFXEditorParticlePage::UpdateLauncherListUI()
{
    GET_SFX_EDITOR();

    int nSelectIndex = m_comboLauncher.GetCurSel();
    m_comboLauncher.ResetContent();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    int nCount =pSFX->GetParticleLauncherNum();
	DWORD dwLauncherCount = 0;
    pScene->GetLauncherEntityDataCount(&dwLauncherCount);
	for (DWORD i = dwLauncherCount; i < (DWORD)nCount; i++)
	{
        pScene->AddLauncherEntityData();
	}
    SFX_ENTITY_DATA *pLauncherEntityData = NULL;
	for (DWORD i = 0; i < (DWORD)nCount; ++i)
	{
        pScene->GetLauncherEntityData(i, &pLauncherEntityData);
		if (pLauncherEntityData->szEntityName[0] == '\0')
		{
            pScene->SetLauncherEntityDataNameToDefault(pLauncherEntityData);
		}
        CString str;
		str.Format(_T("%s"), pLauncherEntityData->szEntityName);
		m_comboLauncher.InsertString(-1, str);
	}

    m_comboLauncher.SetCurSel( pSFX->GetParticleLauncherNum() - 1 );
}

void KSceneSFXEditorParticlePage::UpdateForceFieldListUI()
{
    GET_SFX_EDITOR();


    m_comboForceField.ResetContent();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    int nCount = pSFX->GetForceFieldNum();
    while (nCount)
    {
        CString str;
        str.Format(_T("力场%d"), nCount);
        m_comboForceField.InsertString(0, str);
        nCount--;
    }

    m_comboForceField.SetCurSel(pSFX->GetForceFieldNum() - 1);


    DWORD dwBnNum = (DWORD)m_vecCheckLaus.size();
    for (DWORD i = 0; i < dwBnNum; i++)
    {
        m_vecCheckLaus[i]->DestroyWindow();
        delete m_vecCheckLaus[i];
    }
    m_vecCheckLaus.clear();

    
    DWORD dwLauNum = pSFX->GetParticleLauncherNum();
    RECT  rectContain;

    GetDlgItem(IDC_EFFICE_LAU)->GetClientRect(&rectContain);


    int nSelectIndex = m_comboForceField.GetCurSel();

    for (DWORD i = 0; i < dwLauNum; i++)
    {
        IEKG3DSFXParticleLauncher* pLauncher = NULL;
        pSFX->GetParticleLauncher(i, &pLauncher);
        
        ASSERT(pLauncher);
     
        SFX_ENTITY_DATA* data = NULL;
        pScene->GetLauncherEntityData(i, &data);

        CString strName = data->szEntityName;
        CLauCheckCtl* Bn = new CLauCheckCtl();
        
        RECT rect = {
            rectContain.left + 10 + i % 2 * 100, 
            rectContain.top + i / 2 * 20 + 15, 
            rectContain.left + 10 + i % 2 * 100 + 75, 
            rectContain.top + i / 2 * 20 + 30
        };

        Bn->pPage     = this;
        Bn->pLauncher = pLauncher;
        Bn->Create(strName, BS_AUTOCHECKBOX , rect, GetDlgItem(IDC_EFFICE_LAU), i + 100);
        Bn->SetFont(GetFont());
        Bn->ShowWindow(SW_SHOW);
        if (nSelectIndex == CB_ERR)
        {
            Bn->SetCheck(FALSE);
            Bn->EnableWindow(FALSE);
        }
        else
        {
            IEKG3DSFXParticleForceField* pForce = NULL;
            pSFX->GetForceField(nSelectIndex, &pForce);
            ASSERT(pForce);

            if (pLauncher->HasEffForce(pForce))
                Bn->SetCheck(TRUE);
            else
                Bn->SetCheck(FALSE);

            Bn->EnableWindow(TRUE);
        }
        
        m_vecCheckLaus.push_back(Bn);
    }

    GetDlgItem(IDC_EFFICE_LAU)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
}

void KSceneSFXEditorParticlePage::UpdateParticleKindListUI(CComboBox *pComboBox)
{
    GET_SFX_EDITOR();

    int nSelectIndex = pComboBox->GetCurSel();
    pComboBox->ResetContent();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    int nCount = pSFX->GetParticleLifeTimeLineNum();
    while (nCount)
    {
        CString str;
        str.Format( _T("种类%d"), nCount );
        pComboBox->InsertString( 0, str );
        nCount--;
    }

    pComboBox->SetCurSel( nSelectIndex );
}

void KSceneSFXEditorParticlePage::UpdateTextureListUI(CComboBox *pComboBox)
{
    GET_SFX_EDITOR();

    int nSelect = pComboBox->GetCurSel();
    pComboBox->ResetContent();
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    int nCount = pSFX->GetTextureNum();
    while (nCount)
    {
        CString str;
        str.Format(_T("纹理%d"), nCount);
        pComboBox->InsertString(0, str);
        nCount--;
    }

    pComboBox->SetCurSel(nSelect);
}

void KSceneSFXEditorParticlePage::UpdateUI()
{
    if (!IsWindow(m_hWnd))
        return;
    

    UpdateParticleKindListUI(&m_comboParticleKind);
    if (m_comboParticleKind.GetCount())
        m_comboParticleKind.SetCurSel(m_comboParticleKind.GetCount() - 1);
    UpdateLauncherListUI();
    UpdateParticleKindListUI(&m_comboLauncherParticleKind);
    UpdateForceFieldListUI();

    UpdateParticleKindUI();
    UpdateLauncherUI();
    UpdateForceFieldUI();
}

void KSceneSFXEditorParticlePage::UpdateParticleType(IEKG3DSFXParticleLifeTimeLine *pLifeTime)
{
    m_comboParticleType.SetCurSel(pLifeTime->GetParticleType());
    m_redioMeteorDelay.SetCheck(pLifeTime->GetParticleFlag() & 1);
    m_sliderSmooth.SetPos((int)pLifeTime->GetSmooth());

    if (pLifeTime->GetParticleType() == PARTICLE_TYPE_METEOR)
    {
        m_redioMeteorDelay.EnableWindow(TRUE);
        m_staticMooth.EnableWindow(TRUE);
        m_staticMooth1.EnableWindow(TRUE);
        m_staticMooth2.EnableWindow(TRUE);
        m_sliderSmooth.EnableWindow(TRUE);
    }
    else
    {
        m_redioMeteorDelay.EnableWindow(FALSE);
        m_staticMooth.EnableWindow(FALSE);
        m_staticMooth1.EnableWindow(FALSE);
        m_staticMooth2.EnableWindow(FALSE);
        m_sliderSmooth.EnableWindow(FALSE);
    }
}

void KSceneSFXEditorParticlePage::UpdateTextureIndex(IEKG3DSFXParticleLifeTimeLine *pLifeTime)
{
    m_comboTextureIndex.SetCurSel(pLifeTime->GetTextureIndex());
}

void KSceneSFXEditorParticlePage::UpdateRotation(IEKG3DSFXParticleLifeTimeLine *pLifeTime)
{
    CString str;
    str.Format("%.3f", pLifeTime->GetRotationFactor());
    m_editRotationFactor.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateWidth(IEKG3DSFXParticleLifeTimeLine *pLifeTime, int nFrame)
{
    CString str;
    float fWidth = 0;
    TKG3DTimeLine<float>* pLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetWidthTimeLine());
    pLine->GetData(&fWidth, (float)nFrame);
    str.Format("%.3f", fWidth);
    m_editWidth.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateHeight(IEKG3DSFXParticleLifeTimeLine *pLifeTime, int nFrame)
{
    CString str;
    float fHeight = 0;
    TKG3DTimeLine<float>* pLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetHeightTimeLine());
    pLine->GetData(&fHeight, (float)nFrame);
    str.Format("%.3f", fHeight);
    m_editHeight.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateRotationMin(IEKG3DSFXParticleLifeTimeLine* pLineTime, int nFrame)
{
	CString str;
	float fRotation = 0;
    TKG3DTimeLine<float>* pLine = static_cast<TKG3DTimeLine<float>*>(pLineTime->GetRotationTimeLineMin());
    pLine->GetData(&fRotation, (float)nFrame);
	str.Format("%.3f", D3DXToDegree(fRotation));
	m_editRotationMin.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateRotationMax(IEKG3DSFXParticleLifeTimeLine* pLineTime, int nFrame)
{
    CString str;
    float fRotation = 0;
    TKG3DTimeLine<float>* pLine = static_cast<TKG3DTimeLine<float>*>(pLineTime->GetRotationTimeLineMax());
    pLine->GetData(&fRotation, (float)nFrame);
    str.Format("%.3f", D3DXToDegree(fRotation));
    m_editRotationMax.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateColor(IEKG3DSFXParticleLifeTimeLine *pLifeTime, int nFrame)
{
    D3DXCOLOR color;
    TKG3DTimeLine<D3DXCOLOR>* pLineL = static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLineL());
    pLineL->GetData(&color, (float)nFrame);
    DWORD dwColor = (DWORD)color;

    CRect rect;
    m_staticColorL.GetClientRect (rect);
    CClientDC dcL(&m_staticColorL);
    CBrush Brush(RGB(D3DCOLOR_GETRED(dwColor), D3DCOLOR_GETGREEN(dwColor), D3DCOLOR_GETBLUE(dwColor)));
    CBrush* pOldBrush = dcL.SelectObject(&Brush);
    dcL.Rectangle(&rect);
    dcL.SelectObject(pOldBrush);

    TKG3DTimeLine<D3DXCOLOR>* pLineR = static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLineR());
    pLineR->GetData(&color, (float)nFrame);
    dwColor = (DWORD)color;

    m_staticColorR.GetClientRect (rect);
    CClientDC dcR(&m_staticColorR);
    CBrush BrushR(RGB(D3DCOLOR_GETRED(dwColor), D3DCOLOR_GETGREEN(dwColor), D3DCOLOR_GETBLUE(dwColor)));
    CBrush* pOldBrushR = dcR.SelectObject(&BrushR);
    dcR.Rectangle(&rect);
    dcR.SelectObject(pOldBrushR);

}

void KSceneSFXEditorParticlePage::UpdateTextureFrameIndex(IEKG3DSFXParticleLifeTimeLine *pLifeTime, int nFrame)
{
    CString str;
    int nTextureIndex = 0;
    TKG3DTimeLine<int>* pLine = static_cast<TKG3DTimeLine<int>*>(pLifeTime->GetTextureFrameTimeLine());
    pLine->GetData(&nTextureIndex, (float)nFrame);
    str.Format("%d", nTextureIndex);
    //m_editTextureFrameIndex.SetWindowText(str);

	// combox
	if (m_comboxTexFrame.FindString(0, str.GetBuffer()) == LB_ERR)
		m_comboxTexFrame.SelectString(0, TEXT("随机"));
	else
		m_comboxTexFrame.SelectString(0, str);
}

void KSceneSFXEditorParticlePage::UpdateParticleKindUI()
{
    GET_SFX_EDITOR();

    UpdateTextureListUI(&m_comboTextureIndex);

    int nSelectTimeLine = m_comboParticleKind.GetCurSel();
    if (nSelectTimeLine == CB_ERR)
    {
        m_comboParticleType.EnableWindow(FALSE);
        m_editRotationFactor.EnableWindow(FALSE);
        m_editRotationFactor.EnableWindow(FALSE);
        m_comboTextureIndex.EnableWindow(FALSE);
        m_staticColorL.EnableWindow(FALSE);
        m_staticColorR.EnableWindow(FALSE);
        m_editWidth.EnableWindow(FALSE);
        m_editHeight.EnableWindow(FALSE);
        //m_editTextureFrameIndex.EnableWindow(FALSE);
        m_radioWidth.EnableWindow(FALSE);
        m_radioHeight.EnableWindow(FALSE);
		m_radioRotationMin.EnableWindow(FALSE);
		m_radioRotationMax.EnableWindow(FALSE);
        m_radioTextureFrameIndex.EnableWindow(FALSE);
        m_radioColorL.EnableWindow(FALSE);
        m_radioColorR.EnableWindow(FALSE);
        m_comboxTexFrame.EnableWindow(FALSE);
        m_editRotationMin.EnableWindow(FALSE);
        m_editRotationMax.EnableWindow(FALSE);
        m_redioMeteorDelay.EnableWindow(FALSE);
        m_sliderSmooth.EnableWindow(FALSE);
        m_staticMooth.EnableWindow(FALSE);
        m_staticMooth1.EnableWindow(FALSE);
        m_staticMooth2.EnableWindow(FALSE);
        return;
    }
    else
    {
        m_comboParticleType.EnableWindow();
        m_editRotationFactor.EnableWindow();
        m_comboTextureIndex.EnableWindow();
        m_staticColorL.EnableWindow();
        m_staticColorR.EnableWindow();
        m_editWidth.EnableWindow();
        m_editHeight.EnableWindow();
        //m_editTextureFrameIndex.EnableWindow();
        m_radioWidth.EnableWindow();
        m_radioHeight.EnableWindow();
		m_radioRotationMin.EnableWindow();
		m_radioRotationMax.EnableWindow();
        m_radioTextureFrameIndex.EnableWindow();
        m_radioColorL.EnableWindow();
        m_radioColorR.EnableWindow();
        m_comboxTexFrame.EnableWindow();
        m_editRotationMin.EnableWindow();
        m_editRotationMax.EnableWindow();
        m_redioMeteorDelay.EnableWindow();
        m_sliderSmooth.EnableWindow();
        m_staticMooth.EnableWindow();
        m_staticMooth1.EnableWindow();
        m_staticMooth2.EnableWindow();
    }

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;
    pSFX->GetParticleLifeTimeLine(nSelectTimeLine, &pLifeTime);
    ASSERT(pLifeTime);

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

    UpdateParticleType(pLifeTime);
    UpdateTextureIndex(pLifeTime);
    UpdateRotation(pLifeTime);
    UpdateWidth(pLifeTime, nFrame);
    UpdateHeight(pLifeTime, nFrame);
	UpdateRotationMin(pLifeTime, nFrame);
	UpdateRotationMax(pLifeTime, nFrame);
    UpdateColor(pLifeTime, nFrame);
    UpdateTextureFrameIndex(pLifeTime, nFrame);
}

void KSceneSFXEditorParticlePage::UpdateLauncherName(IEKG3DSceneSFXEditor *pScene, int nSelect)
{
    SFX_ENTITY_DATA *pLauncherEntityData = NULL;
    pScene->GetLauncherEntityData(nSelect, &pLauncherEntityData);
    m_editLauncherName.SetWindowText(pLauncherEntityData->szEntityName);
}

void KSceneSFXEditorParticlePage::UpdateLaunchParticleKind(
    IEKG3DSceneSFXEditor *pScene,
    IEKG3DSFXParticleLauncher *pLauncher
)
{
    int i = 0;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    DWORD dwLifeTimeNum = pSFX->GetParticleLifeTimeLineNum();
    for (i = 0; i < (int)dwLifeTimeNum; i++)
    {
        IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;
        pSFX->GetParticleLifeTimeLine( i, &pLifeTime );

        if( pLifeTime == pLauncher->GetParticleLifeTime() )
        {
            break;
        }
    }

    UpdateParticleKindListUI(&m_comboLauncherParticleKind);
    if (i == (int)dwLifeTimeNum)
        m_comboLauncherParticleKind.SetCurSel(-1);
    else
        m_comboLauncherParticleKind.SetCurSel( i );
}

void KSceneSFXEditorParticlePage::UpdateLauncherShape(IEKG3DSFXParticleLauncher *pLauncher)
{
    if (pLauncher->GetLauncherShape() == SFX_LS_RECTANGLE)
        m_comboLauncherShape.SetCurSel(LAUNCHER_SHAPE_RECT_INDEX);
    else if (pLauncher->GetLauncherShape() == SFX_LS_CIRCLE)
        m_comboLauncherShape.SetCurSel(LAUNCHER_SHAPE_CIRCLE_INDEX);
    else if (pLauncher->GetLauncherShape() == SFX_LS_SPHERE)
        m_comboLauncherShape.SetCurSel(LAUNCHER_SHAPE_SPHERE_INDEX);
    else if (pLauncher->GetLauncherShape() == SFX_LS_CYLINDER)
        m_comboLauncherShape.SetCurSel(LAUNCHER_SHAPE_CYLINDER_INDEX);
}

void KSceneSFXEditorParticlePage::UpdateLauncherType(IEKG3DSFXParticleLauncher *pLauncher)
{
    if (pLauncher->GetParticleType() == SFX_PT_BILLBOARD_POINT)
        m_comboLauncherType.SetCurSel(LAUNCHER_BILLBOARD_POINT_INDEX);
    else if (pLauncher->GetParticleType() == SFX_PT_BILLBOARD_ORIENTSELF)
        m_comboLauncherType.SetCurSel(LAUNCHER_BILLBOARD_ORIENTSELF_INDEX);
    else if (pLauncher->GetParticleType() == SFX_PT_BILLBOARD_LOCK_WORLD_Y)
        m_comboLauncherType.SetCurSel(LAUNCHER_BILLBOARD_WORLD_LOCK_Y);
    else if (pLauncher->GetParticleType() == SFX_PT_BILLBOARD_LOCK_WORLD_Z)
        m_comboLauncherType.SetCurSel(LAUNCHER_BILLBOARD_WORLD_LOCK_Z);
    else if (pLauncher->GetParticleType() == SFX_PT_LEAVES)
        m_comboLauncherType.SetCurSel(LAUNCHER_LEAVES_INDEX);
    else if (pLauncher->GetParticleType() == SFX_PT_BILLBOARD_LOCK_LOCAL_Y)
        m_comboLauncherType.SetCurSel(LAUNCHER_BILLBOARD_LOCAL_LOCK_Y);
    else if (pLauncher->GetParticleType() == SFX_PT_BILLBOARD_LOCK_LOCAL_Z)
        m_comboLauncherType.SetCurSel(LAUNCHER_BILLBOARD_LOCAL_LOCK_Z);
}

void KSceneSFXEditorParticlePage::UpdateUpdateMode(IEKG3DSFXParticleLauncher *pLauncher)
{
    if (pLauncher->GetParticleUpdateMode() == SFX_PUM_FOLLOW_SFX)
        m_comboParticleUpdateMode.SetCurSel(PARTICLE_UPDATE_FOLLOW_SFX_INDEX);
    else if (pLauncher->GetParticleUpdateMode() == SFX_PUM_SELF)
        m_comboParticleUpdateMode.SetCurSel(PARTICLE_UPDATE_SELF_INDEX);
}

void KSceneSFXEditorParticlePage::UpdateLauncherParams(IEKG3DSFXParticleLauncher *pLauncher)
{
    CString str;
    str.Format("%.3f", pLauncher->GetShapeParam1());
    m_editLauncherParam1.SetWindowText(str);
    str.Format("%.3f", pLauncher->GetShapeParam2());
    m_editLauncherParam2.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateLaunchNum(IEKG3DSFXParticleLauncher *pLauncher, int nFrame)
{
    CString str;
    float fParticleNum = 0;
	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleNumLine());
    pLine->GetData( &fParticleNum, (float)nFrame );
    str.Format( "%.3f", fParticleNum );
    m_editParticleNum.SetWindowText( str );
}

void KSceneSFXEditorParticlePage::UpdateSpeedMin(IEKG3DSFXParticleLauncher *pLauncher, int nFrame)
{
    CString str;
    float fMinSpeed = 0;

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMinLine());

    pLine->GetData(&fMinSpeed, (float)nFrame);
    str.Format("%.3f", fMinSpeed);
    m_editMinSpeed.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateSpeedMax(IEKG3DSFXParticleLauncher *pLauncher, int nFrame)
{
    CString str;
    float fMaxSpeed = 0;

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMaxLine());

    pLine->GetData(&fMaxSpeed, (float)nFrame);
    str.Format("%.3f", fMaxSpeed);
    m_editMaxSpeed.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateLifeMin(IEKG3DSFXParticleLauncher *pLauncher, int nFrame)
{
    CString str;
    float fMinLife = 0;

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMinLine());

    pLine->GetData(&fMinLife, (float)nFrame);
    str.Format("%.3f", fMinLife);
    m_editMinLife.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateLifeMax(IEKG3DSFXParticleLauncher *pLauncher, int nFrame)
{
    CString str;
    float fMaxLife = 0;

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMaxLine());

    pLine->GetData(&fMaxLife, (float)nFrame);
    str.Format("%.3f", fMaxLife);
    m_editMaxLife.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateScatterMin(IEKG3DSFXParticleLauncher *pLauncher, int nFrame)
{
    CString str;
    float fScatter = 0;

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMin());

    pLine->GetData(&fScatter, (float)nFrame);
    str.Format("%.3f", (fScatter / D3DX_PI * 180));
    m_editMinScatter.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateScatterMax(IEKG3DSFXParticleLauncher *pLauncher, int nFrame)
{
    CString str;
    float fScatter = 0;

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMax());

    pLine->GetData(&fScatter, (float)nFrame);
    str.Format("%.3f", (fScatter / D3DX_PI * 180));
    m_editMaxScatter.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateMotionType(IEKG3DSFXParticleLauncher *pLauncher)
{
    m_comboMotionType.SetCurSel((int)pLauncher->GetMotionType());
}

void KSceneSFXEditorParticlePage::UpdateBlendMode(IEKG3DSFXParticleLauncher *pLauncher)
{
    m_comboBlendMode.SetCurSel((int)pLauncher->GetBlendMode());
}

void KSceneSFXEditorParticlePage::UpdateRandDelay(IEKG3DSFXParticleLauncher *pLauncher)
{
    CString str;
    str.Format("%u", pLauncher->GetRandTexDelay());
    m_editRandDelayTex.SetWindowText(str);
    str.Format("%u", pLauncher->GetRandColDelay());
    m_editRandDelayCol.SetWindowText(str);
    str.Format("%u", pLauncher->GetRandAlpDelay());
    m_editRandDelayAlp.SetWindowText(str);
    str.Format("%u", pLauncher->GetRandRotDelay());
    m_editRandDelayRot.SetWindowText(str);

}

void KSceneSFXEditorParticlePage::UpdateLauncherUI()
{
    GET_SFX_EDITOR();

    int nSelect = m_comboLauncher.GetCurSel();
    if (nSelect == CB_ERR)
    {
        m_editLauncherName.EnableWindow(FALSE);
        m_comboLauncherType.EnableWindow(FALSE);
        m_comboLauncherParticleKind.EnableWindow(FALSE);
        m_comboLauncherShape.EnableWindow(FALSE);
        m_comboParticleUpdateMode.EnableWindow(FALSE);
        m_editLauncherParam1.EnableWindow(FALSE);
        m_editLauncherParam2.EnableWindow(FALSE);
        m_radioParticleNum.EnableWindow(FALSE);
        m_radioMaxSpeed.EnableWindow(FALSE);
        m_radioMinSpeed.EnableWindow(FALSE);
        m_radioMaxLife.EnableWindow(FALSE);
        m_radioMinLife.EnableWindow(FALSE);
        m_radioMaxScatter.EnableWindow(FALSE);
        m_radioMinScatter.EnableWindow(FALSE);
        m_editParticleNum.EnableWindow(FALSE);
        m_editMaxSpeed.EnableWindow(FALSE);
        m_editMinSpeed.EnableWindow(FALSE);
        m_editMaxLife.EnableWindow(FALSE);
        m_editMinLife.EnableWindow(FALSE);
        m_editMaxScatter.EnableWindow(FALSE);
        m_editMinScatter.EnableWindow(FALSE);
        m_comboBlendMode.EnableWindow(FALSE);
        m_comboMotionType.EnableWindow(FALSE);
        m_editForceFieldFactorRD.EnableWindow(FALSE);
        m_editRandDelayTex.EnableWindow(FALSE);
        m_editRandDelayCol.EnableWindow(FALSE);
        m_editRandDelayAlp.EnableWindow(FALSE);
        m_editRandDelayRot.EnableWindow(FALSE);
        return;
    }
    else
    {
        m_editLauncherName.EnableWindow();
        m_comboLauncherType.EnableWindow();
        m_comboLauncherParticleKind.EnableWindow();
        m_comboLauncherShape.EnableWindow();
        m_comboParticleUpdateMode.EnableWindow();
        m_editLauncherParam1.EnableWindow();
        m_editLauncherParam2.EnableWindow();
        m_radioParticleNum.EnableWindow();
        m_radioMaxSpeed.EnableWindow();
        m_radioMinSpeed.EnableWindow();
        m_radioMaxLife.EnableWindow();
        m_radioMinLife.EnableWindow();
        m_radioMaxScatter.EnableWindow();
        m_radioMinScatter.EnableWindow();
        m_editParticleNum.EnableWindow();
        m_editMaxSpeed.EnableWindow();
        m_editMinSpeed.EnableWindow();
        m_editMaxLife.EnableWindow();
        m_editMinLife.EnableWindow();
        m_editMaxScatter.EnableWindow();
        m_editMinScatter.EnableWindow();
        m_comboBlendMode.EnableWindow();
        m_comboMotionType.EnableWindow();
        m_editForceFieldFactorRD.EnableWindow();
        m_editRandDelayTex.EnableWindow();
        m_editRandDelayCol.EnableWindow();
        m_editRandDelayAlp.EnableWindow();
        m_editRandDelayRot.EnableWindow();
    }

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXParticleLauncher *pLauncher = NULL;
    pSFX->GetParticleLauncher( nSelect, &pLauncher );
    ASSERT(pLauncher);

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

    UpdateLauncherName(pScene, nSelect);
    UpdateLaunchParticleKind(pScene, pLauncher);
    UpdateLauncherParams(pLauncher);
    UpdateLauncherShape(pLauncher);
    UpdateLauncherType(pLauncher);
    UpdateUpdateMode(pLauncher);
    UpdateLaunchNum(pLauncher, nFrame);
    UpdateSpeedMin(pLauncher, nFrame);
    UpdateSpeedMax(pLauncher, nFrame);
    UpdateLifeMin(pLauncher, nFrame);
    UpdateLifeMax(pLauncher, nFrame);
    UpdateScatterMin(pLauncher, nFrame);
    UpdateScatterMax(pLauncher, nFrame);
    UpdateBlendMode(pLauncher);
    UpdateMotionType(pLauncher);
    UpdateRandDelay(pLauncher);
    UpdateForceRandDelay(pLauncher);
}

void KSceneSFXEditorParticlePage::UpdateForceFieldType(IEKG3DSFXParticleForceField *pForceField)
{
    m_comboForceFieldType.SetCurSel(pForceField->GetType());
    
}

void KSceneSFXEditorParticlePage::UpdateForceRandDelay(IEKG3DSFXParticleLauncher *pLauncher)
{
    CString str;
    str.Format("%u", pLauncher->GetRandForceRotDelay());
    m_editForceFieldFactorRD.SetWindowText(str);

}

void KSceneSFXEditorParticlePage::UpdateForcePower(IEKG3DSFXParticleForceField *pForceField, int nFrame)
{
    TKG3DTimeLine<FLOAT5>* pLine = static_cast<TKG3DTimeLine<FLOAT5>*>(pForceField->GetForceTimeLine());
    FLOAT5 vPower;
    CString str;

    pLine->GetData(&vPower, (float)nFrame);

    str.Format("%.3f", vPower.x);
    m_editForceFieldX.SetWindowText(str);

    str.Format("%.3f", vPower.y);
    m_editForceFieldY.SetWindowText(str);

    str.Format("%.3f", vPower.z);
    m_editForceFieldZ.SetWindowText(str);

    str.Format("%.3f", vPower.u);
    m_editForceFieldFactor1.SetWindowText(str);

    str.Format("%.3f", vPower.v);
    m_editForceFieldFactorEx.SetWindowText(str);

}


void KSceneSFXEditorParticlePage::UpdateForcePos(IEKG3DSFXParticleForceField *pForceField, int nFrame)
{
    TKG3DTimeLine<D3DXVECTOR3>* pLine = static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pForceField->GetForcePosTimeLine());
    D3DXVECTOR3 vPos;
    CString str;

    pLine->GetData(&vPos, (float)nFrame);

    str.Format("%.3f", vPos.x);
    m_editForceFieldX2.SetWindowText(str);

    str.Format("%.3f", vPos.y);
    m_editForceFieldY2.SetWindowText(str);

    str.Format("%.3f", vPos.z);
    m_editForceFieldZ2.SetWindowText(str);
}

void KSceneSFXEditorParticlePage::UpdateForceFactor2(IEKG3DSFXParticleForceField *pForceField, int nFrame)
{
    TKG3DTimeLine<FLOAT>* pLine = static_cast<TKG3DTimeLine<FLOAT>*>(pForceField->GetForceGTimeLine());
    FLOAT vFactor2;
    CString str;

    pLine->GetData(&vFactor2, (float)nFrame);

    str.Format("%.3f", vFactor2);
    m_editForceFieldFactor2.SetWindowText(str);
}
//
//void KSceneSFXEditorParticlePage::UpdateForceX(IEKG3DSFXParticleForceField *pForceField, int nFrame)
//{
//    D3DXVECTOR3 Force;
//
//	TKG3DTimeLine<D3DXVECTOR3>* pLine =
//		static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pForceField->GetForceTimeLine());
//
//    pLine->GetData(&Force, (float)nFrame);
//
//    CString str;
//    str.Format("%.3f", Force.x);
//    m_editForceFieldX.SetWindowText(str);
//}
//
//void KSceneSFXEditorParticlePage::UpdateForceY(IEKG3DSFXParticleForceField *pForceField, int nFrame)
//{
//    D3DXVECTOR3 Force;
//
//	TKG3DTimeLine<D3DXVECTOR3>* pLine =
//		static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pForceField->GetForceTimeLine());
//
//    pLine->GetData(&Force, (float)nFrame);
//
//    CString str;
//    str.Format("%.3f", Force.y);
//    m_editForceFieldY.SetWindowText(str);
//}
//
//void KSceneSFXEditorParticlePage::UpdateForceZ(IEKG3DSFXParticleForceField *pForceField, int nFrame)
//{
//    D3DXVECTOR3 Force;
//
//	TKG3DTimeLine<D3DXVECTOR3>* pLine =
//		static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pForceField->GetForceTimeLine());
//
//    pLine->GetData(&Force, (float)nFrame);
//
//    CString str;
//    str.Format("%.3f", Force.z);
//    m_editForceFieldZ.SetWindowText(str);
//}

void KSceneSFXEditorParticlePage::UpdateForceFieldUI()
{
    GET_SFX_EDITOR();

    //UpdateForceFieldListUI();

    int nSelectForceField = m_comboForceField.GetCurSel();
    if (nSelectForceField == CB_ERR)
    {
        m_comboForceFieldType.EnableWindow(FALSE);
        m_editForceFieldFactor1.EnableWindow(FALSE);
        m_editForceFieldFactorEx.EnableWindow(FALSE);
        m_editForceFieldX.EnableWindow(FALSE);
        m_editForceFieldY.EnableWindow(FALSE);
        m_editForceFieldZ.EnableWindow(FALSE);
        m_editForceFieldFactor2.EnableWindow(FALSE);
        m_editForceFieldX2.EnableWindow(FALSE);
        m_editForceFieldY2.EnableWindow(FALSE);
        m_editForceFieldZ2.EnableWindow(FALSE);
        return;
    }

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXParticleForceField *pForceField = NULL;
    pSFX->GetForceField(nSelectForceField, &pForceField);
    ASSERT(pForceField);
    SFX_FORCE_FIELD_TYPE dwType = pForceField->GetType();

    m_comboForceFieldType.EnableWindow();
    m_editForceFieldX.EnableWindow(dwType != SFX_FFT_BROWNIAN);
    m_editForceFieldY.EnableWindow(dwType != SFX_FFT_BROWNIAN);
    m_editForceFieldZ.EnableWindow(dwType != SFX_FFT_BROWNIAN);

    m_editForceFieldFactor1.EnableWindow(dwType == SFX_FFT_WIND || dwType == SFX_FFT_ROTATION || dwType == SFX_FFT_BROWNIAN);
    m_editForceFieldFactorEx.EnableWindow(dwType == SFX_FFT_WIND || dwType == SFX_FFT_ROTATION);

    m_editForceFieldFactor2.EnableWindow(dwType == SFX_FFT_ROTATION);
    m_editForceFieldX2.EnableWindow(dwType == SFX_FFT_ROTATION);
    m_editForceFieldY2.EnableWindow(dwType == SFX_FFT_ROTATION);
    m_editForceFieldZ2.EnableWindow(dwType == SFX_FFT_ROTATION);

    if (dwType == SFX_FFT_BROWNIAN)
        m_staticLableL.SetWindowText("随机速度大小");
    else
        m_staticLableL.SetWindowText("旋转量最小值");

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateForceFieldType(pForceField);
    UpdateForcePower(pForceField, nFrame);
    UpdateForcePos(pForceField, nFrame);
    UpdateForceFactor2(pForceField, nFrame);
}

void KSceneSFXEditorParticlePage::OnSetForceFieldPower()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    FLOAT5 vPower;
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_X, str);
    vPower.x = (float)::atof(str);

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_Y, str);
    vPower.y = (float)::atof(str);

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_Z, str);
    vPower.z = (float)::atof(str);

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_FACTOR1, str);
    vPower.u = (float)::atof(str);

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_FACTOR_EX, str);
    vPower.v = (float)::atof(str);

    TKG3DTimeLine<FLOAT5>* pLine = static_cast<TKG3DTimeLine<FLOAT5>*>(pForceField->GetForceTimeLine());
    pLine->InsertKeyFrame(nFrame, vPower);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();

}

void KSceneSFXEditorParticlePage::OnSetForceFieldRandDelay()
{
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CString str;
    GetDlgItemText(IDC_EDIT_FORCE_FIELD_RD, str);

    pLauncher->SetRandForceRotDelay((DWORD)::atoi(str));
}

void KSceneSFXEditorParticlePage::OnSetForceFieldPos()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    D3DXVECTOR3 vPos;
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_X2, str);
    vPos.x = (float)::atof(str);

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_Y2, str);
    vPos.y = (float)::atof(str);

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_Z2, str);
    vPos.z = (float)::atof(str);

    TKG3DTimeLine<D3DXVECTOR3>* pLine = static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pForceField->GetForcePosTimeLine());
    pLine->InsertKeyFrame(nFrame, vPos);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetForceFieldFactor2()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    FLOAT factor2;
    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;

    GetDlgItemText(IDC_EDIT_FORCE_FIELD_FACTOR2, str);
    factor2 = (float)::atof(str);

    TKG3DTimeLine<FLOAT>* pLine = static_cast<TKG3DTimeLine<FLOAT>*>(pForceField->GetForceGTimeLine());
    pLine->InsertKeyFrame(nFrame, factor2);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}
/*
void KSceneSFXEditorParticlePage::OnSetForceFieldX()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_FORCE_FIELD_X, str);
    float fValue = (float)::atof(str);
    D3DXVECTOR3 vec;

	TKG3DTimeLine<D3DXVECTOR3>* pLine =
		static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pForceField->GetForceTimeLine());

    pLine->GetData(&vec, (float)nFrame);
    vec.x = fValue;
    pLine->InsertKeyFrame(nFrame, vec);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetForceFieldY()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_FORCE_FIELD_Y, str);
    float fValue = (float)::atof(str);
    D3DXVECTOR3 vec;

	TKG3DTimeLine<D3DXVECTOR3>* pLine =
		static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pForceField->GetForceTimeLine());

    pLine->GetData(&vec, (float)nFrame);
    vec.y = fValue;
    pLine->InsertKeyFrame(nFrame, vec);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetForceFieldZ()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_FORCE_FIELD_Z, str);
    float fValue = (float)::atof(str);
    D3DXVECTOR3 vec;

	TKG3DTimeLine<D3DXVECTOR3>* pLine =
		static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pForceField->GetForceTimeLine());

    pLine->GetData(&vec, (float)nFrame);
    vec.z = fValue;
    pLine->InsertKeyFrame(nFrame, vec);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}
*/
void KSceneSFXEditorParticlePage::OnBnClickedButtonAddParticleKind()
{
    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSFXParticleLifeTimeLine *pLifeTimeLine = NULL;
    pSFX->AddParticleLifeTimeLine( &pLifeTimeLine );
    UpdateParticleKindListUI(&m_comboParticleKind);
    m_comboParticleKind.SetCurSel(pSFX->GetParticleLifeTimeLineNum() - 1);
    UpdateParticleKindListUI(&m_comboLauncherParticleKind);
    UpdateParticleKindUI();
}

void KSceneSFXEditorParticlePage::OnBnClickedDelParticleKind()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nItem = m_comboParticleKind.GetCurSel();
    if (nItem == CB_ERR)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    if (SUCCEEDED(pSFX->RemoveParticleLifeTimeLine(nItem)))
    {
        UpdateParticleKindListUI(&m_comboParticleKind);
        UpdateParticleKindListUI(&m_comboLauncherParticleKind);
        m_comboParticleKind.SetCurSel(pSFX->GetParticleLifeTimeLineNum() - 1);

        UpdateParticleKindUI();
        UpdateLauncherUI();
    }
}

void KSceneSFXEditorParticlePage::UpdateLauncherSelected()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    VOID *pvEntityPointer = NULL;

    GET_SFX_EDITOR();

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

	GETSELECT(piSceneEditorBase);

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	pScene->GetSelectionToolInterface(&pSelTool);
	if(NULL == pSelTool)
		return;

	//if (pSelector->GetSelectedEntitySize() == 1)
	if(1 == pSelTool->GetSelectionCount())
    {
		IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
		if(NULL == pProxy)
			return;

		//for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
		for(IEKG3DSceneEntity* piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
        {
			/*IEKG3DSceneEntity* piEntity = NULL;
			pSelector->GetSelectedEntity(i, &pEntity);*/

            IEKG3DSFXParticleLauncher *pLauncher = NULL;
            DWORD dwIndex = pSFX->GetParticleLauncherNum();
            for (DWORD i = 0; i < dwIndex; ++i)
            {
                pSFX->GetParticleLauncher(i, &pLauncher);
                ASSERT(pLauncher);

                hRetCode = piEntity->GetPointer(&pvEntityPointer);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

                if ((VOID *)pLauncher == pvEntityPointer)
                {
                    hRetCode = piEntity->GetType(&EntityType);
                    KGLOG_COM_PROCESS_ERROR(hRetCode);
                    KG_ASSERT_EXIT(EntityType == SCENEENTITY_SFX_LAUNCHER);

                    m_comboLauncher.SetCurSel(i);
                    UpdateLauncherUI();
                    return;
                }
            }

            //++it;
        }
    }
    else
    {
        m_comboLauncher.SetCurSel(-1);
        UpdateLauncherUI();
        return;
    }
Exit0:
    return;
}

void KSceneSFXEditorParticlePage::OnBnClickedDelLauncher()
{
    GET_SFX_EDITOR();

    int nItem = m_comboLauncher.GetCurSel();
    if (nItem == CB_ERR)
        return;

    IEKG3DSFX* sfx = GetSfx();

    if (!sfx)
        return;

    if (SUCCEEDED(pScene->RemoveLauncher(nItem)))
    {
        UpdateLauncherListUI();
        m_comboLauncher.SetCurSel(sfx->GetParticleLauncherNum() - 1);
        UpdateLauncherUI();
        UpdateForceFieldListUI();
    }
    pFrame->GetKeysFrame()->SetTimeLine("", NULL, SFX_TLDT_NONE);
}

void KSceneSFXEditorParticlePage::OnBnClickedAddLauncher()
{
    GET_SFX_EDITOR();
    pScene->AddLauncher(0, 0, 0);
    
    IEKG3DSFX* sfx = GetSfx();
    
    if (!sfx)
        return;

    IEKG3DSFXParticleLauncher* laucher = NULL;
    sfx->GetParticleLauncher(sfx->GetParticleLauncherNum() - 1, &laucher);
    
    if (!laucher)
        return;

    IEKG3DSFXParticleLifeTimeLine* timeLine = NULL;
    sfx->GetParticleLifeTimeLine(0, &timeLine);

    if (timeLine)
        laucher->SetParticleLifeTimeLine(timeLine);

    float life = 50.f;
    float num = 1.f;
    float speed = 10.f;
    laucher->GetParticleLifeMinLine()->Insert(0, &life);
    laucher->GetParticleLifeMaxLine()->Insert(0, &life);
    laucher->GetParticleNumLine()->Insert(0, &num);
    laucher->GetParticleSpeedMaxLine()->Insert(0, &speed);
    laucher->GetParticleSpeedMinLine()->Insert(0, &speed);

    UpdateLauncherListUI();
    UpdateLauncherUI();
    UpdateForceFieldListUI();
}

void KSceneSFXEditorParticlePage::OnStnClickedColorPicL()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    CheckRadioButton(IDC_RADIO_COLOR_L, IDC_RADIO_SCATTER_MAX, IDC_RADIO_COLOR_L);
    OnBnClickedRadioColorL();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    TKG3DTimeLine<D3DXCOLOR>* pLine = static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLineL());
    pLine->GetData(&m_ColorL, (float)nFrame);

    //EnableWindow(FALSE);
	GetColorPickerDialog()->Init( (D3DCOLORVALUE*)&m_ColorL, "粒子颜色", this, TRUE );
}

void KSceneSFXEditorParticlePage::OnStnClickedColorPicR()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    CheckRadioButton(IDC_RADIO_COLOR_L, IDC_RADIO_SCATTER_MAX, IDC_RADIO_COLOR_R);
    OnBnClickedRadioColorR();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    TKG3DTimeLine<D3DXCOLOR>* pLine = static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLineR());
    pLine->GetData(&m_ColorR, (float)nFrame);

    //EnableWindow(FALSE);
    GetColorPickerDialog()->Init( (D3DCOLORVALUE*)&m_ColorR, "粒子颜色", this, TRUE );
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioColorL()
{
    // TODO: Add your control notification handler code here
    CheckRadioButton(IDC_RADIO_COLOR_L, IDC_RADIO_SCATTER_MAX, IDC_RADIO_COLOR_L);
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    pFrame->GetKeysFrame()->SetTimeLine("粒子颜色", pLifeTime->GetColorTimeLineL(),SFX_TLDT_D3DXCOLOR);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioColorR()
{
    // TODO: Add your control notification handler code here
    CheckRadioButton(IDC_RADIO_COLOR_L, IDC_RADIO_SCATTER_MAX, IDC_RADIO_COLOR_R);
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    pFrame->GetKeysFrame()->SetTimeLine("粒子颜色", pLifeTime->GetColorTimeLineR(),SFX_TLDT_D3DXCOLOR);
}


void KSceneSFXEditorParticlePage::OnBnClickedRadioWidth()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    pFrame->GetKeysFrame()->SetTimeLine("粒子宽度", pLifeTime->GetWidthTimeLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioHeight()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    pFrame->GetKeysFrame()->SetTimeLine("粒子高度", pLifeTime->GetHeightTimeLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioRotationMin()
{
	// TODO: Add your control notification handler code here
	GET_SFX_EDITOR();
	GET_LIFE_TIME_LINE();

	pFrame->GetKeysFrame()->SetTimeLine("偏转角", pLifeTime->GetRotationTimeLineMin(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioRotationMax()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    pFrame->GetKeysFrame()->SetTimeLine("偏转角", pLifeTime->GetRotationTimeLineMax(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboParticleKind()
{
    // TODO: Add your control notification handler code here
    UpdateParticleKindUI();
}

void KSceneSFXEditorParticlePage::OnSetWidth()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_WIDTH, str);
    float fValue = (float)::atof(str);
    if (fValue < 0)
        fValue = -fValue;
    static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetWidthTimeLine())->InsertKeyFrame(nFrame, fValue);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetHeight()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_HEIGHT, str);
    float fValue = (float)::atof(str);
    if (fValue < 0)
        fValue = -fValue;
    static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetHeightTimeLine())->InsertKeyFrame(nFrame, fValue);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

// add by zhaoxinyu
void KSceneSFXEditorParticlePage::OnSetRotationMin()
{
	int nResult  = false;
	int nRetCode = false;

	CString strFrame = TEXT("");
	float	fValue = 0.0f;
	int	    nFrame = 0;

	GET_SFX_EDITOR();
	GET_LIFE_TIME_LINE();

	KG_PROCESS_ERROR(pLifeTime);
	KG_PROCESS_ERROR(pFrame);

	nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

	nRetCode = GetDlgItemText(IDC_RATATION_MIN, strFrame);
	KG_PROCESS_ERROR(nRetCode);

	fValue = static_cast<float>(atof(strFrame.GetBuffer()));

	static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetRotationTimeLineMin())->InsertKeyFrame(nFrame, D3DXToRadian(fValue));
	pFrame->GetKeysFrame()->UpdateSliderCtrl();

	nResult = true;
Exit0:
	return;
}

void KSceneSFXEditorParticlePage::OnSetRotationMax()
{
    int nResult  = false;
    int nRetCode = false;

    CString strFrame = TEXT("");
    float	fValue = 0.0f;
    int	    nFrame = 0;

    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    KG_PROCESS_ERROR(pLifeTime);
    KG_PROCESS_ERROR(pFrame);

    nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();

    nRetCode = GetDlgItemText(IDC_RATATION_MAX, strFrame);
    KG_PROCESS_ERROR(nRetCode);

    fValue = static_cast<float>(atof(strFrame.GetBuffer()));

    static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetRotationTimeLineMax())->InsertKeyFrame(nFrame, D3DXToRadian(fValue));
    pFrame->GetKeysFrame()->UpdateSliderCtrl();

    nResult = true;
Exit0:
    return;
}


LRESULT KSceneSFXEditorParticlePage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    switch (message)
    {
    case WM_COLORPOINTER_CHANGE:
        {
            GET_SFX_EDITOR_RET(-1);
            GET_LIFE_TIME_LINE_RET(-1);

            if (pFrame->GetKeysFrame()->GetTimeLine() == pLifeTime->GetColorTimeLineL())
            {
                EnableWindow();
                GET_SFX_EDITOR_RET(-1);
                GET_LIFE_TIME_LINE_RET(-1);
                int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
                static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLineL())->InsertKeyFrame( nFrame, m_ColorL );
                pFrame->GetKeysFrame()->UpdateSliderCtrl();
                UpdateColor(pLifeTime, nFrame);
            }

            if (pFrame->GetKeysFrame()->GetTimeLine() == pLifeTime->GetColorTimeLineR())
            {
                EnableWindow();
                int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
                static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLineR())->InsertKeyFrame( nFrame, m_ColorR );
                pFrame->GetKeysFrame()->UpdateSliderCtrl();
                UpdateColor(pLifeTime, nFrame);
            }
        }
        break;


   /* case WM_COLORPOINTER_CHANGE :
        {
            DWORD dwColor = (DWORD)wParam;
            CRect rect;
            m_staticColor.GetClientRect (rect);
            CClientDC dc(&m_staticColor);
            CBrush Brush(RGB(GETRED(dwColor), D3DCOLOR_GETGREEN(dwColor), D3DCOLOR_GETBLUE(dwColor)));
            CBrush* pOldBrush = dc.SelectObject(&Brush);
            dc.Rectangle(&rect);
            dc.SelectObject(pOldBrush);

        }
        break;*/


    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_ROTATION_FACTOR:
                OnSetRotationFactor();
                break;
            case IDC_EDIT_KEY_WIDTH:
                OnSetWidth();
                break;
            case IDC_EDIT_KEY_HEIGHT:
                OnSetHeight();
                break;
			case IDC_RATATION_MIN :
				OnSetRotationMin();
				break;
            case IDC_RATATION_MAX :
                OnSetRotationMax();
                break;
            /*case IDC_EDIT_KEY_TEXTURE_FRAME_INDEX:
                OnSetTextureFrameIndex();
                break;*/
			case IDC_RAND_DELAY_TEX :
				OnSetTexRandDelayTex();
				break;
            case IDC_RAND_DELAY_COL :
                OnSetTexRandDelayCol();
                break;
            case IDC_RAND_DELAY_ALP :
                OnSetTexRandDelayAlp();
                break;
            case IDC_RAND_DELAY_ROT :
                OnSetTexRandDelayRot();
                break;
            case IDC_EDIT_LAUNCHER_PARAM1:
                OnSetLauncherParam1();
                break;
            case IDC_EDIT_LAUNCHER_PARAM2:
                OnSetLauncherParam2();
                break;
            case IDC_EDIT_KEY_PARTICLE_NUM:
                OnSetParticleNum();
                break;
            case IDC_EDIT_KEY_MAX_LIFE:
                OnSetMaxLife();
                break;
            case IDC_EDIT_KEY_MIN_LIFE:
                OnSetMinLife();
               break;
            case IDC_EDIT_KEY_MAX_SPEED:
                OnSetMaxSpeed();
                break;
            case IDC_EDIT_KEY_MIN_SPEED:
                OnSetMinSpeed();
                break;
            case IDC_EDIT_KEY_SCATTER_MIN:
                OnSetMinScatter();
                break;
            case IDC_EDIT_KEY_SCATTER_MAX:
                OnSetMaxScatter();
                break;
            case IDC_EDIT_FORCE_FIELD_RD :
                OnSetForceFieldRandDelay();
                break;
            case IDC_EDIT_FORCE_FIELD_FACTOR1 :
            case IDC_EDIT_FORCE_FIELD_FACTOR_EX :
            case IDC_EDIT_FORCE_FIELD_X :
            case IDC_EDIT_FORCE_FIELD_Z :
            case IDC_EDIT_FORCE_FIELD_Y :
                OnSetForceFieldPower();
				break;
            case IDC_EDIT_FORCE_FIELD_FACTOR2 :
                OnSetForceFieldFactor2();
                break;
            case IDC_EDIT_FORCE_FIELD_X2 :
            case IDC_EDIT_FORCE_FIELD_Z2 :
            case IDC_EDIT_FORCE_FIELD_Y2 :
                OnSetForceFieldPos();
                break;
			case IDC_EDIT_LAUNCHER_NAME:
				OnSetLauncherName();
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
    return KGLilisPanelDLG::WindowProc(message, wParam, lParam);
}

void KSceneSFXEditorParticlePage::OnSetLauncherName()
{
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nSelect = m_comboLauncher.GetCurSel();
    if (nSelect == CB_ERR)
        return;

    SFX_ENTITY_DATA *pEntityData = NULL;
    HRESULT hRetCode = E_FAIL;
    hRetCode = pScene->GetLauncherEntityData(nSelect, &pEntityData);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	m_editLauncherName.GetWindowText(
        pEntityData->szEntityName,
        sizeof(pEntityData->szEntityName) / sizeof(pEntityData->szEntityName[0])
    );
	m_comboLauncher.DeleteString(nSelect);
	m_comboLauncher.InsertString(nSelect, pEntityData->szEntityName);
	m_comboLauncher.SetCurSel(nSelect);

    pLauncher->SetName(pEntityData->szEntityName);


    UpdateForceFieldListUI();

Exit0:
    return;
}

void KSceneSFXEditorParticlePage::OnSetRotationFactor()
{
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    CString str;
    GetDlgItemText(IDC_EDIT_ROTATION_FACTOR, str);
    float fValue = (float)::atof(str);
    pLifeTime->SetRotationFactor( fValue );
}

void KSceneSFXEditorParticlePage::OnSetLauncherParam1()
{
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CString str;
    GetDlgItemText(IDC_EDIT_LAUNCHER_PARAM1, str);
    float fValue = (float)::atof(str);
    if (fValue < 0)
        fValue = -fValue;
    pLauncher->SetShapeParam1(fValue);
}

void KSceneSFXEditorParticlePage::OnSetLauncherParam2()
{
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CString str;
    GetDlgItemText(IDC_EDIT_LAUNCHER_PARAM2, str);
    float fValue =(float) ::atof(str);
    if (fValue < 0)
        fValue = -fValue;
    pLauncher->SetShapeParam2(fValue);
}

void KSceneSFXEditorParticlePage::OnSetTexRandDelayTex()
{
	int nDelay = GetDlgItemInt(m_editRandDelayTex.GetDlgCtrlID());
	if (nDelay < 0)
		return;
	GET_SFX_EDITOR();
	GET_LAUNCHER();
	pLauncher->SetRandTexDelay(nDelay);
}

void KSceneSFXEditorParticlePage::OnSetTexRandDelayCol()
{
    int nDelay = GetDlgItemInt(m_editRandDelayCol.GetDlgCtrlID());
    if (nDelay < 0)
        return;
    GET_SFX_EDITOR();
    GET_LAUNCHER();
    pLauncher->SetRandColDelay(nDelay);
}

void KSceneSFXEditorParticlePage::OnSetTexRandDelayAlp()
{
    int nDelay = GetDlgItemInt(m_editRandDelayAlp.GetDlgCtrlID());
    if (nDelay < 0)
        return;
    GET_SFX_EDITOR();
    GET_LAUNCHER();
    pLauncher->SetRandAlpDelay(nDelay);
}

void KSceneSFXEditorParticlePage::OnSetTexRandDelayRot()
{
    int nDelay = GetDlgItemInt(m_editRandDelayRot.GetDlgCtrlID());
    if (nDelay < 0)
        return;
    GET_SFX_EDITOR();
    GET_LAUNCHER();
    pLauncher->SetRandRotDelay(nDelay);
}

void KSceneSFXEditorParticlePage::OnSetTextureFrameIndex()
{
    //// TODO: Add your control notification handler code here
    //GET_SFX_EDITOR();
    //GET_LIFE_TIME_LINE();

    //int nFrame = pFrame->m_dlgKeyFrame.m_Slider.GetCurPos();
    //UINT uTextureIndex = GetDlgItemInt(IDC_EDIT_KEY_TEXTURE_FRAME_INDEX);
    //UINT uCutNum;
    //HRESULT hr = pScene->m_lpSFX->GetTextureCutNum(
    //    pLifeTime->m_uTextureIndex,
    //    &uCutNum
    //);

    //if (FAILED(hr))
    //{
    //    ASSERT(0);
    //}
    //else
    //{
    //    if (uTextureIndex < uCutNum * uCutNum)
    //    {
    //        pLifeTime->m_nTextureFrameTimeLine.InsertKeyFrame( nFrame, uTextureIndex );
    //        pFrame->m_dlgKeyFrame.UpdateSliderCtrl();
    //    }
    //    else
    //    {
    //        UpdateTextureFrameIndex(pLifeTime, nFrame);
    //        MessageBox("错误的纹理坐标索引值！", "大牌提示您：");
    //    }
    //}
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboLauncher()
{
    HRESULT hRetCode = E_FAIL;
    VOID *pvEntityPointer = NULL;

    // TODO: Add your control notification handler code here
    UpdateLauncherUI();

    GET_SFX_EDITOR();
    GET_LAUNCHER();

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

	GETSELECT(piSceneEditorBase);
	//pSelector->SelectNone(0);

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	HRESULT hr = pScene->GetSelectionToolInterface(&pSelTool);
	_ASSERTE(SUCCEEDED(hr));

	pSelTool->ClearSelection();

	pSelector->CopySelectable2SelectedSet(pLauncher);

//Exit0:
    return;
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboLauncherParticleKind()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER_PARTICLE_KIND();
    GET_LAUNCHER();

    pLauncher->SetParticleLifeTimeLine(pLifeTime);
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboLauncherShape()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

	CString strParam1;
	CString strParam2;

    int nSelectShape = m_comboLauncherShape.GetCurSel();
    SFX_LAUNCHER_SHAPE eShape;
	switch (nSelectShape)
	{
	case LAUNCHER_SHAPE_CYLINDER_INDEX :
		eShape = SFX_LS_CYLINDER;
		strParam1 = TEXT("半径：");
		strParam2 = TEXT("高度：");
		break;
	case LAUNCHER_SHAPE_CIRCLE_INDEX   :
		eShape = SFX_LS_CIRCLE;
		strParam1 = TEXT("外径：");
		strParam2 = TEXT("内径：");
		break;
	case LAUNCHER_SHAPE_SPHERE_INDEX   :
		eShape = SFX_LS_SPHERE;
		strParam1 = TEXT("半径：");
		strParam2 = TEXT("----：");
		break;
	case LAUNCHER_SHAPE_RECT_INDEX     :
		eShape = SFX_LS_RECTANGLE;
		strParam1 = TEXT("矩宽：");
		strParam2 = TEXT("矩高：");
		break;
	default :
		return;
	}

	GetDlgItem(IDC_P1)->SetWindowText(strParam1);
	GetDlgItem(IDC_P2)->SetWindowText(strParam2);

    //if (nSelectShape == LAUNCHER_SHAPE_CYLINDER_INDEX)
    //    eShape = SFX_LS_CYLINDER;
    //else if (nSelectShape == LAUNCHER_SHAPE_CIRCLE_INDEX)
    //    eShape = SFX_LS_CIRCLE;
    //else if (nSelectShape == LAUNCHER_SHAPE_SPHERE_INDEX)
    //    eShape = SFX_LS_SPHERE;
    //else if (nSelectShape == LAUNCHER_SHAPE_RECT_INDEX)
    //    eShape = SFX_LS_RECTANGLE;
    //else
    //    return;

    pLauncher->SetParticleShape(eShape);
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboParticleType()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nSelectType = m_comboLauncherType.GetCurSel();
    SFX_PARTICLE_TYPE eType;
    if (nSelectType == LAUNCHER_BILLBOARD_POINT_INDEX)
        eType = SFX_PT_BILLBOARD_POINT;
    else if (nSelectType == LAUNCHER_BILLBOARD_ORIENTSELF_INDEX)
        eType = SFX_PT_BILLBOARD_ORIENTSELF;
    else if (nSelectType == LAUNCHER_BILLBOARD_WORLD_LOCK_Y)
        eType = SFX_PT_BILLBOARD_LOCK_WORLD_Y;
    else if (nSelectType == LAUNCHER_BILLBOARD_WORLD_LOCK_Z)
        eType = SFX_PT_BILLBOARD_LOCK_WORLD_Z;
    else if (nSelectType == LAUNCHER_BILLBOARD_LOCAL_LOCK_Y)
        eType = SFX_PT_BILLBOARD_LOCK_LOCAL_Y;
    else if (nSelectType == LAUNCHER_BILLBOARD_LOCAL_LOCK_Z)
        eType = SFX_PT_BILLBOARD_LOCK_LOCAL_Z;
    else if (nSelectType == LAUNCHER_LEAVES_INDEX)
        eType = SFX_PT_LEAVES;
    else
        return;

    pLauncher->SetParticleType(eType);
}

void KSceneSFXEditorParticlePage::OnCbnMeteorDelay()
{
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    if (m_redioMeteorDelay.GetCheck())
        pLifeTime->SetParticleFlag(1);
    else
        pLifeTime->SetParticleFlag(0);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioParticleNum()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    pFrame->GetKeysFrame()->SetTimeLine("发射数量", pLauncher->GetParticleNumLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioMinSpeed()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    pFrame->GetKeysFrame()->SetTimeLine("最小速度", pLauncher->GetParticleSpeedMinLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioMaxSpeed()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    pFrame->GetKeysFrame()->SetTimeLine("最大速度", pLauncher->GetParticleSpeedMaxLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioMinLife()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    pFrame->GetKeysFrame()->SetTimeLine("最小生命", pLauncher->GetParticleLifeMinLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioMaxLife()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    pFrame->GetKeysFrame()->SetTimeLine("最大生命", pLauncher->GetParticleLifeMaxLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioScatterMin()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    pFrame->GetKeysFrame()->SetTimeLine("最小喷射发散", pLauncher->GetParticleScatterMin(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnSetParticleNum()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_PARTICLE_NUM, str);
    float fValue = (float)::atof(str);

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleNumLine());

    pLine->InsertKeyFrame(nFrame, fValue);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetMinSpeed()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_MIN_SPEED, str);
    float fValue =(float) ::atof(str);

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMinLine());

    pLine->InsertKeyFrame(nFrame, fValue);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetMaxSpeed()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UINT uMaxSpeed = GetDlgItemInt(IDC_EDIT_KEY_MAX_SPEED);
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_MAX_SPEED, str);
    float fValue =(float) ::atof(str);

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMaxLine());

    pLine->InsertKeyFrame(nFrame, fValue);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetMinLife()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_MIN_LIFE, str);
    float fValue =(float) ::atof(str);
    if (fValue < 0)
    {
        MessageBox("不能是负数", "大牌提示");
        return;
    }

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMinLine());

    pLine->InsertKeyFrame(nFrame, fValue);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetMaxLife()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_MAX_LIFE, str);
    float fValue = (float)::atof(str);
    if (fValue < 0)
    {
        MessageBox("不能是负数", "大牌提示");
        return;
    }

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMaxLine());

	pLine->InsertKeyFrame(nFrame, fValue);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetMinScatter()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_SCATTER_MIN, str);
    float fValue = (float)::atof(str);
    if (fValue < 0)
    {
        MessageBox("不能是负数", "大牌提示");
        return;
    }
    float fAngle = (float)fValue / 180 * D3DX_PI;

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMin());

    pLine->InsertKeyFrame(nFrame, fAngle);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnSetMaxScatter()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    CString str;
    GetDlgItemText(IDC_EDIT_KEY_SCATTER_MAX, str);
    float fValue = (float)::atof(str);
    if (fValue < 0)
    {
        MessageBox("不能是负数", "大牌提示");
        return;
    }
    float fAngle = (float)fValue / 180 * D3DX_PI;

	TKG3DTimeLine<float>* pLine =
		static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMax());

    pLine->InsertKeyFrame(nFrame, fAngle);
    pFrame->GetKeysFrame()->UpdateSliderCtrl();
}

void KSceneSFXEditorParticlePage::OnBnClickedButtonAddForceField()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    pSFX->AddForceField(SFX_FFT_G, NULL);
    UpdateForceFieldListUI();
    UpdateForceFieldUI();
}

void KSceneSFXEditorParticlePage::OnBnClickedButtonDelForceField()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nSelect = m_comboForceField.GetCurSel();
    if (nSelect != CB_ERR)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        pSFX->RemoveForceField(nSelect);
        m_comboForceField.SetCurSel(-1);
        UpdateForceFieldListUI();
        UpdateForceFieldUI();
    }
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboForceFieldList()
{
    // TODO: Add your control notification handler code here
    UpdateForceFieldUI();
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboParticleUpdateMode()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();


    int nSelectUpdateMode = m_comboParticleUpdateMode.GetCurSel();
    SFX_PARTICLE_UPDATE_MODE eUpdateMode;
    if (nSelectUpdateMode == PARTICLE_UPDATE_SELF_INDEX)
        eUpdateMode = SFX_PUM_SELF;
    else if (nSelectUpdateMode == PARTICLE_UPDATE_FOLLOW_SFX_INDEX)
        eUpdateMode = SFX_PUM_FOLLOW_SFX;
    else
        return;

    pLauncher->SetParticleUpdateMode(eUpdateMode);
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboType()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nSelect = m_comboParticleType.GetCurSel();
    if (nSelect != CB_ERR)
    {
        GET_LIFE_TIME_LINE();

        pLifeTime->SetParticleType(_kg_to_smaller_size_cast<WORD>(nSelect)) ;
        UpdateParticleType(pLifeTime);
    }
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboTextureIndex()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nSelect = m_comboTextureIndex.GetCurSel();
    if (nSelect != CB_ERR)
    {
        GET_LIFE_TIME_LINE();

        pLifeTime->SetTextureIndex(nSelect) ;
    }
}

BOOL KSceneSFXEditorParticlePage::OnSetActive()
{
    // TODO: Add your specialized code here and/or call the base class
    UpdateUI();

    return TRUE;// KGLilisPanelDLG::OnSetActive();
}

void KSceneSFXEditorParticlePage::OnCbnSelchangeComboBlendMode()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;

    GET_SFX_EDITOR();

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

    int nSelect = m_comboBlendMode.GetCurSel();
    if (nSelect != CB_ERR)
    {
		GETSELECT(piSceneEditorBase);

		IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
		if(NULL == pProxy)
			return;

		//for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
		for(IEKG3DSceneEntity* piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
        {
   //         IEKG3DSceneEntity *piEntity = NULL;//&(*i);
			//pSelector->GetSelectedEntity(i, &piEntity);

            hRetCode = piEntity->GetType(&EntityType);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            if (EntityType == SCENEENTITY_SFX_LAUNCHER)
            {
                IEKG3DSFXParticleLauncher *pLauncher = NULL;
                hRetCode = piEntity->GetPointer((VOID **)&pLauncher);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
                pLauncher->SetBlendMode((SFX_BLEND_MODE)nSelect);
            }

            //++i;
        }
    }
Exit0:
    return;
}


void KSceneSFXEditorParticlePage::OnCbnSelchangeComboMotionType()
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;

    GET_SFX_EDITOR();

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

    int nSelect = m_comboMotionType.GetCurSel();
    if (nSelect != CB_ERR)
    {
        GETSELECT(piSceneEditorBase);

		IEKG3DSceneSelectionToolAccessProxy* pProxy = KSF::GetSelectionProxy(pScene);
		if(NULL == pProxy)
			return;

		//for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
		for(IEKG3DSceneEntity* piEntity = pProxy->Reset(); NULL != piEntity; piEntity = pProxy->Next())
        {
            //IEKG3DSceneEntity *piEntity = NULL;
            //pSelector->GetSelectedEntity(i, &piEntity);

            hRetCode = piEntity->GetType(&EntityType);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            if (EntityType == SCENEENTITY_SFX_LAUNCHER)
            {
                IEKG3DSFXParticleLauncher *pLauncher = NULL;
                hRetCode = piEntity->GetPointer((VOID **)&pLauncher);
                KGLOG_COM_PROCESS_ERROR(hRetCode);
                pLauncher->SetMotionType((DWORD)nSelect);
            }

            //++i;
        }
    }
Exit0:
    return;
}


void KSceneSFXEditorParticlePage::OnCbnSelchangeComboForceFieldType()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();

    int nSel = m_comboForceFieldType.GetCurSel();
    if (nSel != CB_ERR)
    {
        
        GET_FORCE_FIELD(;);
        pForceField->SetType((SFX_FORCE_FIELD_TYPE)nSel);

        UpdateForceFieldUI();
    }
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioScatterMax()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    pFrame->GetKeysFrame()->SetTimeLine("最大喷射发散", pLauncher->GetParticleScatterMax(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnBnClickedRadioTextureFrameIndex()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    pFrame->GetKeysFrame()->SetTimeLine("纹理索引", pLifeTime->GetTextureFrameTimeLine(), SFX_TLDT_INT);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyWidth()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_WIDTH);
    OnBnClickedRadioWidth();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateWidth(pLifeTime, nFrame);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyHeight()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_HEIGHT);
    OnBnClickedRadioHeight();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateHeight(pLifeTime, nFrame);
}


void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyRotationMin()
{
	// TODO: Add your control notification handler code here
	GET_SFX_EDITOR();
	GET_LIFE_TIME_LINE();

	CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_ROTATION_MIN);
	OnBnClickedRadioRotationMin();

	int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
	UpdateRotationMin(pLifeTime, nFrame);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyRotationMax()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_ROTATION_MAX);
    OnBnClickedRadioRotationMax();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateRotationMax(pLifeTime, nFrame);
}


void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyTextureFrameIndex()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_TEXTURE_FRAME_INDEX);
    OnBnClickedRadioTextureFrameIndex();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateTextureFrameIndex(pLifeTime, nFrame);

	// combox
	CString strIndex;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
	UINT uCutNum = 0;
	pSFX->GetTextureCutNum(
		pLifeTime->GetTextureIndex(),
		&uCutNum
	);
	m_comboxTexFrame.ResetContent();
	for (UINT i = 0; i < uCutNum * uCutNum; i++)
	{
		strIndex.Format(TEXT("%u"), i);
		m_comboxTexFrame.AddString(strIndex);
	}
	m_comboxTexFrame.AddString(TEXT("随机"));
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyParticleNum()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_PARTICLE_NUM);
    OnBnClickedRadioParticleNum();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateLaunchNum(pLauncher, nFrame);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyMinSpeed()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_MIN_SPEED);
    OnBnClickedRadioMinSpeed();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateSpeedMin(pLauncher, nFrame);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyMaxSpeed()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_MAX_SPEED);
    OnBnClickedRadioMaxSpeed();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateSpeedMax(pLauncher, nFrame);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyMinLife()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_MIN_LIFE);
    OnBnClickedRadioMinLife();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateLifeMin(pLauncher, nFrame);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyMaxLife()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_MAX_LIFE);
    OnBnClickedRadioMaxLife();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateLifeMax(pLauncher, nFrame);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyScatterMin()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_SCATTER_MIN);
    OnBnClickedRadioScatterMin();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateScatterMin(pLauncher, nFrame);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditKeyScatterMax()
{
    // TODO: Add your control notification handler code here
    GET_SFX_EDITOR();
    GET_LAUNCHER();

    CheckRadioButton(IDC_RADIO_COLOR, IDC_RADIO_SCATTER_MAX, IDC_RADIO_SCATTER_MAX);
    OnBnClickedRadioScatterMax();

    int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
    UpdateScatterMax(pLauncher, nFrame);
}

void KSceneSFXEditorParticlePage::OnCbnDropdownComboLauncher()
{
    // TODO: Add your control notification handler code here
    UpdateLauncherListUI();
}

void KSceneSFXEditorParticlePage::OnCbnDropdownComboParticleKind()
{
    // TODO: Add your control notification handler code here
    UpdateParticleKindListUI(&m_comboParticleKind);
}

void KSceneSFXEditorParticlePage::OnCbnDropdownComboForceFieldList()
{
    // TODO: Add your control notification handler code here
    //UpdateForceFieldListUI();
}


void KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPower()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    pFrame->GetKeysFrame()->SetTimeLine("力量", pForceField->GetForceTimeLine(), SFX_TLDT_VECTOR5);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldPos()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    pFrame->GetKeysFrame()->SetTimeLine("位置", pForceField->GetForcePosTimeLine(), SFX_TLDT_VECTOR3);
}

void KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldFactor2()
{
    GET_SFX_EDITOR();
    GET_FORCE_FIELD(;);

    pFrame->GetKeysFrame()->SetTimeLine("旋转", pForceField->GetForceGTimeLine(), SFX_TLDT_FLOAT);
}

void KSceneSFXEditorParticlePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    GET_SFX_EDITOR();
    GET_LIFE_TIME_LINE();

    int nHPos = 0;

    if (pScrollBar == (CWnd*)&m_sliderSmooth)
    {
        nHPos = m_sliderSmooth.GetPos();
        pLifeTime->SetSmooth(nHPos);
    }
}

//void KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldY()
//{
//    // TODO: Add your control notification handler code here
//    OnSetForceTimeLine();
//}
//
//void KSceneSFXEditorParticlePage::OnEnSetfocusEditForceFieldZ()
//{
//    // TODO: Add your control notification handler code here
//    OnSetForceTimeLine();
//}

void KSceneSFXEditorParticlePage::OnCbnSelchangeTFrameKey()
{
	GET_SFX_EDITOR();
	GET_LIFE_TIME_LINE();

	// combox
	int  nIndex = m_comboxTexFrame.GetCurSel();
	char szTexIndex[8];
	m_comboxTexFrame.GetLBText(nIndex, szTexIndex);

	int nFrame = pFrame->GetKeysFrame()->m_Slider.GetCurPos();
	UINT uTextureIndex = 0;
	if (strcmp(TEXT("随机"), szTexIndex))
		uTextureIndex = atoi(szTexIndex);//GetDlgItemInt(IDC_EDIT_KEY_TEXTURE_FRAME_INDEX);
	else
		uTextureIndex = INT_MAX;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    UINT uCutNum;
	HRESULT hr = pSFX->GetTextureCutNum(
		pLifeTime->GetTextureIndex(),
		&uCutNum
		);

	if (FAILED(hr))
	{
		ASSERT(0);
	}
	else
	{
		if (uTextureIndex < uCutNum * uCutNum || uTextureIndex == INT_MAX)
		{
			static_cast<TKG3DTimeLine<int>*>(pLifeTime->GetTextureFrameTimeLine())->InsertKeyFrame( nFrame, uTextureIndex );
			pFrame->GetKeysFrame()->UpdateSliderCtrl();
		}
		else
		{
			UpdateTextureFrameIndex(pLifeTime, nFrame);
			MessageBox("错误的纹理坐标索引值！", "大牌提示您：");
		}
	}
}


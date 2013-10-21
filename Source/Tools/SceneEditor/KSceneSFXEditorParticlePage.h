////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorParticlePage.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:12:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORPARTICLEPAGE_H_
#define _INCLUDE_KSCENESFXEDITORPARTICLEPAGE_H_

////////////////////////////////////////////////////////////////////////////////
#include "Resource.h"
#include "KSceneSFXEditorDoc.h"
#include "KEnterEdit.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KGLilisPanel.h"
#include "afxwin.h"
#include "afxcmn.h"





// KSceneSFXEditorPartcalePage dialog

/*
IDC_RADIO_COLOR;
IDC_RADIO_WIDTH;
IDC_RADIO_HEIGHT;
IDC_RADIO_TEXTURE_FRAME_INDEX;

IDC_RADIO_PARTICLE_NUM;
IDC_RADIO_MIN_SPEED;
IDC_RADIO_MAX_SPEED;
IDC_RADIO_MIN_LIFE;
IDC_RADIO_MAX_LIFE;
IDC_RADIO_SCATTER_MIN;
IDC_RADIO_SCATTER_MAX;
*/
interface IEKG3DSFXParticleLifeTimeLine;
interface IEKG3DSFXParticleForceField;
interface IEKG3DSceneSFXEditor;
interface IEKG3DSFXParticleLauncher;

class KSceneSFXEditorParticlePage;

class CLauCheckCtl : public CButton
{
public :
    IEKG3DSFXParticleLauncher*   pLauncher;
    KSceneSFXEditorParticlePage* pPage;
    BOOL PreTranslateMessage(MSG* pMsg);
   
    
};

class KSceneSFXEditorParticlePage : public KGLilisPanelDLG
{
	DECLARE_DYNAMIC(KSceneSFXEditorParticlePage)

public:
	KSceneSFXEditorParticlePage(CWnd* pParent = NULL);
	virtual ~KSceneSFXEditorParticlePage();

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_PARTICLE };

    // Particle kind edit control.
    CComboBox   m_comboParticleKind;

    KGLilisButton m_buttonAddParticleKind;
    KGLilisButton m_buttonDelParticleKind;

    KEnterEdit  m_editRotationFactor;

    CButton m_radioColorL;
    CButton m_radioColorR;
    CButton m_radioWidth;
    CButton m_radioHeight;
	CButton m_radioRotationMin;
	CButton m_radioRotationMax;
    CButton m_radioTextureFrameIndex;

    CStatic m_stRandRotation;
    CStatic m_stTextIndex;

    CStatic m_sParticleType;
    CComboBox m_comboParticleType;

    CButton m_redioMeteorDelay;



    CStatic     m_staticColorL;
    CStatic     m_staticColorR;
    CComboBox   m_comboTextureIndex;
    KEnterEdit  m_editWidth;
    KEnterEdit  m_editHeight;
	KEnterEdit  m_editRotationMin;
	KEnterEdit  m_editRotationMax;
    KEnterEdit  m_editTextureFrameIndex;
	CComboBox	m_comboxTexFrame;
	KEnterEdit  m_editRandDelayTex;
    KEnterEdit  m_editRandDelayCol;
    KEnterEdit  m_editRandDelayAlp;
    KEnterEdit  m_editRandDelayRot;

    D3DXCOLOR   m_ColorL;
    D3DXCOLOR   m_ColorR;

    // Launcher control
    CComboBox   m_comboLauncherParticleKind;
    CComboBox   m_comboLauncherShape;
    CComboBox   m_comboLauncherType;
    CComboBox   m_comboLauncher;
    CComboBox   m_comboParticleUpdateMode;

    KEnterEdit  m_editLauncherParam1;
    KEnterEdit  m_editLauncherParam2;

    KGLilisButton     m_buttonAddLauncher;
    KGLilisButton     m_buttonDelLauncher;
 
	KEnterEdit	m_editLauncherName;

    CButton     m_radioParticleNum;
    CButton     m_radioMinSpeed;
    CButton     m_radioMaxSpeed;
    CButton     m_radioMinLife;
    CButton     m_radioMaxLife;
    CButton     m_radioMinScatter;
    CButton     m_radioMaxScatter;

    KEnterEdit      m_editParticleNum;
    KEnterEdit      m_editMinSpeed;
    KEnterEdit      m_editMaxSpeed;
    KEnterEdit      m_editMinLife;
    KEnterEdit      m_editMaxLife;
    KEnterEdit      m_editMinScatter;
    KEnterEdit      m_editMaxScatter;
    
    CComboBox   m_comboBlendMode;
    CComboBox   m_comboMotionType;


    CStatic     m_staticLable1;
    CStatic     m_staticLable2;
    CStatic     m_staticLable3;
    CStatic     m_staticLable4;
    CStatic     m_staticLable5;
    CStatic     m_staticLable6;
    CStatic     m_staticLable7;
    CStatic     m_staticLable8;
    CStatic     m_staticLable9;
    CStatic     m_staticLable10;
    CStatic     m_staticLableDelayTex;
    CStatic     m_staticLableDelayCol;
    CStatic     m_staticLableDelayAlp;
    CStatic     m_staticLableDelayRot;

    CStatic     m_staticLableT;
    CStatic     m_staticLableL;
    CStatic     m_staticLableEx;
    CStatic     m_staticLableRD;
    CStatic     m_staticLableX;
    CStatic     m_staticLableY;
    CStatic     m_staticLableZ;

    CStatic     m_staticEffLau;

    // Force field control
    CComboBox   m_comboForceField;

    KGLilisButton     m_buttonAddForceField;
    KGLilisButton     m_buttonDelForceField;

    CComboBox   m_comboForceFieldType;

    KEnterEdit      m_editForceFieldFactor1;
    KEnterEdit      m_editForceFieldFactorEx;
    KEnterEdit      m_editForceFieldFactorRD;
    KEnterEdit      m_editForceFieldX;
    KEnterEdit      m_editForceFieldY;
    KEnterEdit      m_editForceFieldZ;

    KEnterEdit      m_editForceFieldFactor2;
    KEnterEdit      m_editForceFieldX2;
    KEnterEdit      m_editForceFieldY2;
    KEnterEdit      m_editForceFieldZ2;

    CSliderCtrl     m_sliderSmooth;
    CStatic         m_staticMooth;
    CStatic         m_staticMooth1;
    CStatic         m_staticMooth2;

    vector<CButton*> m_vecCheckLaus;


    void UpdateUI();

    void UpdateParticleKindUI();
    void UpdateLauncherUI();
    void UpdateLauncherSelected();
    void UpdateForceFieldUI();

    void UpdateTextureListUI(CComboBox *pComboBox);
    void UpdateLauncherListUI();
    void UpdateParticleKindListUI(CComboBox *pComboBox);
    void UpdateForceFieldListUI();

    // 更新控件里显示的数据
    // particle kind
    void UpdateParticleType(IEKG3DSFXParticleLifeTimeLine *pLifeTime);
    void UpdateTextureIndex(IEKG3DSFXParticleLifeTimeLine *pLifeTime);

    void UpdateRotation(IEKG3DSFXParticleLifeTimeLine *pLifeTime);
    void UpdateWidth(IEKG3DSFXParticleLifeTimeLine *pLifeTime, int nFrame);
    void UpdateHeight(IEKG3DSFXParticleLifeTimeLine *pLifeTime, int nFrame);
	void UpdateRotationMin(IEKG3DSFXParticleLifeTimeLine* pLineTime, int nFrame);
	void UpdateRotationMax(IEKG3DSFXParticleLifeTimeLine* pLineTime, int nFrame);
    void UpdateColor(IEKG3DSFXParticleLifeTimeLine *pLifeTime, int nFrame);
    void UpdateTextureFrameIndex(IEKG3DSFXParticleLifeTimeLine *pLifeTime, int nFrame);

    // launcher
    void UpdateLauncherName(IEKG3DSceneSFXEditor *pScene, int nSelect);
    void UpdateLaunchParticleKind(IEKG3DSceneSFXEditor *pScene, IEKG3DSFXParticleLauncher *pLauncher);
    void UpdateLauncherShape(IEKG3DSFXParticleLauncher *pLauncher);
    void UpdateLauncherType(IEKG3DSFXParticleLauncher *pLauncher);
    void UpdateUpdateMode(IEKG3DSFXParticleLauncher *pLauncher);
    void UpdateLauncherParams(IEKG3DSFXParticleLauncher *pLauncher);
    void UpdateLaunchNum(IEKG3DSFXParticleLauncher *pLauncher, int nFrame);
    void UpdateSpeedMin(IEKG3DSFXParticleLauncher *pLauncher, int nFrame);
    void UpdateSpeedMax(IEKG3DSFXParticleLauncher *pLauncher, int nFrame);
    void UpdateLifeMin(IEKG3DSFXParticleLauncher *pLauncher, int nFrame);
    void UpdateLifeMax(IEKG3DSFXParticleLauncher *pLauncher, int nFrame);
    void UpdateScatterMin(IEKG3DSFXParticleLauncher *pLauncher, int nFrame);
    void UpdateScatterMax(IEKG3DSFXParticleLauncher *pLauncher, int nFrame);
    void UpdateBlendMode(IEKG3DSFXParticleLauncher *pLauncher);
    void UpdateMotionType(IEKG3DSFXParticleLauncher* pLaucher);
    void UpdateRandDelay(IEKG3DSFXParticleLauncher* pLaucher);
    void UpdateForceRandDelay(IEKG3DSFXParticleLauncher *pLaucher);

    // force field
    void UpdateForceFieldType(IEKG3DSFXParticleForceField *pForceField);
    void UpdateForcePower(IEKG3DSFXParticleForceField *pForceField, int nFrame);
    void UpdateForcePos(IEKG3DSFXParticleForceField *pForceField, int nFrame);
    void UpdateForceFactor2(IEKG3DSFXParticleForceField *pForceField, int nFrame);
    //void UpdateForceX(IEKG3DSFXParticleForceField *pForceField, int nFrame);
    //void UpdateForceY(IEKG3DSFXParticleForceField *pForceField, int nFrame);
    //void UpdateForceZ(IEKG3DSFXParticleForceField *pForceField, int nFrame);
    void UpdateForceFactor2(IEKG3DSFXParticleForceField *pForceField);
    void UpdateForceX2(IEKG3DSFXParticleForceField *pForceField, int nFrame);
    void UpdateForceY2(IEKG3DSFXParticleForceField *pForceField, int nFrame);
    void UpdateForceZ2(IEKG3DSFXParticleForceField *pForceField, int nFrame);


    // 编辑框数据输入后回车响应
    void OnSetFrameNum();
    void OnSetFrameInterval();

    void OnSetWidth();
    void OnSetHeight();
	void OnSetRotationMin();
	void OnSetRotationMax();
    void OnSetTextureFrameIndex();
    void OnSetRotationFactor();

    void OnSetLauncherParam1();
    void OnSetLauncherParam2();

    void OnSetMinSpeed();
    void OnSetMaxSpeed();
    void OnSetMinLife();
    void OnSetMaxLife();
    void OnSetMinScatter();
    void OnSetMaxScatter();
    void OnSetParticleNum();

    //void OnSetForceFieldFactor1();
    void OnSetForceFieldPower();
    void OnSetForceFieldPos();
    void OnSetForceFieldFactor2();
    void OnSetForceFieldRandDelay();
    //void OnSetForceFieldY();
    //void OnSetForceFieldZ();
    //void OnSetForceTimeLine();

	void OnSetLauncherName();
	void OnSetTexRandDelayTex();
   	void OnSetTexRandDelayCol();
    void OnSetTexRandDelayAlp();
	void OnSetTexRandDelayRot();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

    // Particle Kind
    afx_msg void OnBnClickedButtonAddParticleKind();
    afx_msg void OnBnClickedDelParticleKind();

    afx_msg void OnBnClickedRadioColorL();
    afx_msg void OnBnClickedRadioColorR();
    afx_msg void OnBnClickedRadioWidth();
    afx_msg void OnBnClickedRadioHeight();
	afx_msg void OnBnClickedRadioRotationMin();
	afx_msg void OnBnClickedRadioRotationMax();
    afx_msg void OnBnClickedRadioTextureFrameIndex();

    afx_msg void OnCbnSelchangeComboParticleType();
    afx_msg void OnCbnSelchangeComboTextureIndex();
    afx_msg void OnStnClickedColorPicL();
    afx_msg void OnStnClickedColorPicR();

    afx_msg void OnCbnMeteorDelay();

    // Launcher
    afx_msg void OnBnClickedAddLauncher();
    afx_msg void OnBnClickedDelLauncher();
    afx_msg void OnCbnSelchangeComboParticleKind();
    afx_msg void OnBnClickedRadioParticleKind();
    afx_msg void OnBnClickedRadioLauncher();

    afx_msg void OnCbnSelchangeComboLauncher();
    afx_msg void OnCbnSelchangeComboLauncherParticleKind();
    afx_msg void OnCbnSelchangeComboLauncherShape();
    afx_msg void OnCbnSelchangeComboType();
    afx_msg void OnCbnSelchangeComboParticleUpdateMode();
    afx_msg void OnCbnSelchangeComboBlendMode();
    afx_msg void OnCbnSelchangeComboMotionType();

    afx_msg void OnBnClickedRadioParticleNum();
    afx_msg void OnBnClickedRadioMinSpeed();
    afx_msg void OnBnClickedRadioMaxSpeed();
    afx_msg void OnBnClickedRadioMinLife();
    afx_msg void OnBnClickedRadioMaxLife();
    afx_msg void OnBnClickedRadioScatterMin();
    afx_msg void OnBnClickedRadioScatterMax();

    // Force Field
    afx_msg void OnBnClickedButtonAddForceField();
    afx_msg void OnBnClickedButtonDelForceField();
    afx_msg void OnCbnSelchangeComboForceFieldList();
    afx_msg void OnCbnSelchangeComboForceFieldType();

protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    virtual BOOL OnSetActive();
    void OnOK(){};
    void OnCancel(){};
public:
public:
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnEnSetfocusEditKeyWidth();
public:
    afx_msg void OnEnSetfocusEditKeyHeight();
public :
	afx_msg void OnEnSetfocusEditKeyRotationMin();
	afx_msg void OnEnSetfocusEditKeyRotationMax();
public:
    afx_msg void OnEnSetfocusEditKeyTextureFrameIndex();
public:
    afx_msg void OnEnSetfocusEditKeyParticleNum();
public:
    afx_msg void OnEnSetfocusEditKeyMinSpeed();
public:
    afx_msg void OnEnSetfocusEditKeyMaxSpeed();
public:
    afx_msg void OnEnSetfocusEditKeyMinLife();
public:
    afx_msg void OnEnSetfocusEditKeyMaxLife();
public:
    afx_msg void OnEnSetfocusEditKeyScatterMin();
public:
    afx_msg void OnEnSetfocusEditKeyScatterMax();
public:
    afx_msg void OnCbnDropdownComboLauncher();
public:
    afx_msg void OnCbnDropdownComboParticleKind();
public:
    afx_msg void OnCbnDropdownComboForceFieldList();
public:
    afx_msg void OnEnSetfocusEditForceFieldPower();
    afx_msg void OnEnSetfocusEditForceFieldPos();
    afx_msg void OnEnSetfocusEditForceFieldFactor2();
//public:
//    afx_msg void OnEnSetfocusEditForceFieldY();
//public:
//    afx_msg void OnEnSetfocusEditForceFieldZ();
public:
	afx_msg void OnCbnSelchangeTFrameKey();

};

#define GET_LIFE_TIME_LINE_RET(n)\
int nSelectTimeLine = m_comboParticleKind.GetCurSel();\
if (nSelectTimeLine == CB_ERR)\
    return n;\
IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;\
{/* to prevent redefinition error */\
    IEKG3DSFX *pSFX = NULL;\
    pScene->GetSFX(&pSFX);\
    if (!pSFX)\
        return n;\
    pSFX->GetParticleLifeTimeLine( nSelectTimeLine, &pLifeTime );\
}\
if (!pLifeTime)\
    return n

#define GET_LIFE_TIME_LINE()\
int nSelectTimeLine = m_comboParticleKind.GetCurSel();\
if (nSelectTimeLine == CB_ERR)\
    return;\
IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;\
{/* to prevent redefinition error */\
    IEKG3DSFX *pSFX = NULL;\
    pScene->GetSFX(&pSFX);\
    if (!pSFX)\
        return ;\
    pSFX->GetParticleLifeTimeLine( nSelectTimeLine, &pLifeTime );\
}\
if (!pLifeTime)\
    return

#define GET_LAUNCHER_PARTICLE_KIND()\
int nSelectTimeLine = m_comboLauncherParticleKind.GetCurSel();\
if (nSelectTimeLine == CB_ERR)\
    return;\
IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;\
{/* to prevent redefinition error */\
    IEKG3DSFX *pSFX = NULL;\
    pScene->GetSFX(&pSFX);\
    if (!pSFX)\
        return ;\
    pSFX->GetParticleLifeTimeLine( nSelectTimeLine, &pLifeTime );\
}\
if (!pLifeTime)\
    return


#define GET_LAUNCHER()\
int nSelectLauncher = m_comboLauncher.GetCurSel();\
if (nSelectLauncher == CB_ERR)\
    return;\
IEKG3DSFXParticleLauncher *pLauncher = NULL;\
{/* to prevent redefinition error */\
    IEKG3DSFX *pSFX = NULL;\
    pScene->GetSFX(&pSFX);\
    if (!pSFX)\
        return ;\
    pSFX->GetParticleLauncher(\
        nSelectLauncher,\
        &pLauncher\
        );\
}\
if (!pLauncher)\
    return

#define GET_FORCE_FIELD(n)\
IEKG3DSFXParticleForceField *pForceField = NULL;\
int nSelect = m_comboForceField.GetCurSel();\
if (nSelect == CB_ERR)\
    return n;\
{/* to prevent redefinition error */\
    IEKG3DSFX *pSFX = NULL;\
    pScene->GetSFX(&pSFX);\
    if (!pSFX)\
        return n;\
    pSFX->GetForceField(nSelect, &pForceField);\
}\
if(!pForceField)\
    return n


//////////////////////////////////////////////////////////////////////////
class KSceneSFXEditorParticlePageAUT : public CPropertyPage
{
    DECLARE_DYNAMIC(KSceneSFXEditorParticlePageAUT)

public :
    KSceneSFXEditorParticlePageAUT();
    ~KSceneSFXEditorParticlePageAUT();
    enum { IDD = IDD_PAGE };
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();
    BOOL PreTranslateMessage(MSG* pMsg);

public :
    int UpdateUI();

public :
    KSceneSFXEditorParticlePage m_particlePage;

public :
    DECLARE_MESSAGE_MAP()
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

};

#endif //_INCLUDE_KSCENESFXEDITORPARTICLEPAGE_H_

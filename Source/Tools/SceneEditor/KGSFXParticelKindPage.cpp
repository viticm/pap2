////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXParticelKindPage.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-8-25 15:35:52
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSFXParticelKindPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGSFXParticelKindPage, KGLilisSheet)
KGSFXParticelKindPage::KGSFXParticelKindPage(CWnd* pParent /*=NULL*/)
	: KGLilisSheet(KGSFXParticelKindPage::IDD, pParent)
{
    m_pDependPage = NULL;
}

KGSFXParticelKindPage::~KGSFXParticelKindPage()
{
}

int KGSFXParticelKindPage::SetDependPage(KSceneSFXEditorParticlePage* pDependPage)
{
    ASSERT(pDependPage);
    m_pDependPage = pDependPage;
    return TRUE;
}

void KGSFXParticelKindPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_PARTICLE_KIND,        m_comboParticleKind);
    DDX_Control(pDX, IDC_ADD_PARTICLE_KIND,          m_buttonAddParticleKind);
    DDX_Control(pDX, IDC_DEL_PARTICLE_KIND,          m_buttonDelParticleKind);
    //DDX_Control(pDX, IDC_EDIT_ROTATION_FACTOR,       m_editRotationFactor);
    DDX_Control(pDX, IDC_COMBO_TEXTURE_INDEX,        m_comboTextureIndex);
    DDX_Control(pDX, IDC_RADIO_COLOR,                m_radioColor);
    DDX_Control(pDX, IDC_COLOR_PIC,                  m_staticColor);
    DDX_Control(pDX, IDC_RADIO_WIDTH,                m_radioWidth);
    DDX_Control(pDX, IDC_EDIT_KEY_WIDTH,             m_editWidth);
    DDX_Control(pDX, IDC_RADIO_HEIGHT,               m_radioHeight);
    DDX_Control(pDX, IDC_EDIT_KEY_HEIGHT,            m_editHeight);
    DDX_Control(pDX, IDC_RADIO_TEXTURE_FRAME_INDEX,  m_radioTextureFrameIndex);
    DDX_Control(pDX, IDC_T_FRAME_KEY,                m_comboxTexFrame);
    DDX_Control(pDX, IDC_RADIO_ROTATION,             m_radioRotation);
    DDX_Control(pDX, IDC_RATATION,                   m_editRotation);
}


BEGIN_MESSAGE_MAP(KGSFXParticelKindPage, KGLilisSheet)
    ON_BN_CLICKED(IDC_ADD_PARTICLE_KIND, &KGSFXParticelKindPage::OnBnClickedAddParticleKind)
    ON_BN_CLICKED(IDC_DEL_PARTICLE_KIND, &KGSFXParticelKindPage::OnBnClickedDelParticleKind)
    ON_BN_CLICKED(IDC_RADIO_COLOR,       &KGSFXParticelKindPage::OnBnClickedRadioColor)
    ON_BN_CLICKED(IDC_RADIO_WIDTH,       &KGSFXParticelKindPage::OnBnClickedRadioWidth)
    ON_BN_CLICKED(IDC_RADIO_HEIGHT,      &KGSFXParticelKindPage::OnBnClickedRadioHeight)
    ON_BN_CLICKED(IDC_RADIO_ROTATION,    &KGSFXParticelKindPage::OnBnClickedRadioRotation)
    ON_BN_CLICKED(IDC_RADIO_TEXTURE_FRAME_INDEX, &KGSFXParticelKindPage::OnBnClickedRadioTextureFrameIndex)
END_MESSAGE_MAP()


void KGSFXParticelKindPage::OnBnClickedAddParticleKind()
{
    if (m_pDependPage)
        m_pDependPage->OnBnClickedButtonAddParticleKind();
}

void KGSFXParticelKindPage::OnBnClickedDelParticleKind()
{
    if (m_pDependPage)
        m_pDependPage->OnBnClickedDelParticleKind();
}

void KGSFXParticelKindPage::OnBnClickedRadioColor()
{
    ASSERT(false);
    if (m_pDependPage)
        m_pDependPage->OnBnClickedRadioColorL();
}

void KGSFXParticelKindPage::OnBnClickedRadioWidth()
{
    if (m_pDependPage)
        m_pDependPage->OnBnClickedRadioWidth();
}

void KGSFXParticelKindPage::OnBnClickedRadioHeight()
{
    if (m_pDependPage)
        m_pDependPage->OnBnClickedRadioHeight();
}

void KGSFXParticelKindPage::OnBnClickedRadioRotation()
{
    ASSERT(false);
    if (m_pDependPage)
        m_pDependPage->OnBnClickedRadioRotationMin();
}

void KGSFXParticelKindPage::OnBnClickedRadioTextureFrameIndex()
{
    if (m_pDependPage)
        m_pDependPage->OnBnClickedRadioTextureFrameIndex();
}

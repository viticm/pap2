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

#ifndef _KGSFXPARTICEKINDPAGE_H_
#define _KGSFXPARTICEKINDPAGE_H_

#include "KSceneSFXEditorParticlePage.h"

class KGSFXParticelKindPage : public KGLilisSheet
{
	DECLARE_DYNAMIC(KGSFXParticelKindPage)

public :
	KGSFXParticelKindPage(CWnd* pParent = NULL);   
	virtual ~KGSFXParticelKindPage();
	enum { IDD = IDD_PANEL_SHEET_SFX_PARTICL_KIND };

public :
    int  SetDependPage(KSceneSFXEditorParticlePage* pDependPage);

public :
    KSceneSFXEditorParticlePage*  m_pDependPage;

    CComboBox     m_comboParticleKind;
    KGLilisButton m_buttonAddParticleKind;
    KGLilisButton m_buttonDelParticleKind;

    KEnterEdit    m_editRotationFactor;

    CButton       m_radioColor;
    CButton       m_radioWidth;
    CButton       m_radioHeight;
    CButton       m_radioRotation;
    CButton       m_radioTextureFrameIndex;

    CStatic       m_staticColor;
    CComboBox     m_comboTextureIndex;
    KEnterEdit    m_editWidth;
    KEnterEdit    m_editHeight;
    KEnterEdit    m_editRotation;
    KEnterEdit    m_editTextureFrameIndex;
    CComboBox	  m_comboxTexFrame;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedAddParticleKind();
    afx_msg void OnBnClickedDelParticleKind();
    afx_msg void OnBnClickedRadioColor();
    afx_msg void OnBnClickedRadioWidth();
    afx_msg void OnBnClickedRadioHeight();
    afx_msg void OnBnClickedRadioRotation();
    afx_msg void OnBnClickedRadioTextureFrameIndex();
    //afx_msg void OnCbnSelchangeComboTextureIndex();
    //afx_msg void OnStnClickedColorPic();
};

#endif // _KGSFXPARTICEKINDPAGE_H_
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditChannel.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-10-25 15:10:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITCHANNEL_H_
#define _INCLUDE_KSCENESFXEDITCHANNEL_H_

#include "KGResourseMiniViewWnd.h"
#include "KEnterEdit.h"
#include "KGLilisPanel.h"

interface IEKG3DModel;

class KSceneSFXEditChannel : public CDialog
{
	DECLARE_DYNAMIC(KSceneSFXEditChannel)

public:
	KSceneSFXEditChannel(CWnd* pParent = NULL);   
	virtual ~KSceneSFXEditChannel();

	enum { IDD = IDD_SFXEDITOR_CHANNEL };

    KGResourseMiniViewWnd   m_wndMinView;

    KGLilisButton m_buttonAdd;
    KGLilisButton m_buttonDel;

    KEnterEdit    m_editPieceCount;
    KEnterEdit    m_editPieceWidth;
    KEnterEdit    m_editPieceLength;
    KEnterEdit    m_editUSpeed;
    KEnterEdit    m_editVSpeed;
    KEnterEdit    m_editVCount;
    KEnterEdit    m_editTexture;

    CComboBox     m_comboLModel;
    CComboBox     m_comboLTargt;
    CComboBox     m_comboRModel;
    CComboBox     m_comboRTargt;
    CComboBox     m_comboBlend;

    IEKG3DModel*    m_pLModel;
    IEKG3DModel*    m_pRModel;

    string        m_strLTar;
    string        m_strRTar;

    void SetPieceCount();
    void SetPieceWidth();
    void SetPieceLength();
    void SetUSpeed();
    void SetVSpeed();
    void SetVCount();

    void UpdateUI();
    BOOL OnSetActive();
    BOOL OnInitDialog();

    void UpdateChanel();
    void Check();
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedButtonAdd();
    afx_msg void OnBnClickedButtonDel();
    afx_msg void OnBnClickedSetTexture();
    afx_msg void OnCbnSelchangedLModel();
    afx_msg void OnCbnSelchangedLTargt();
    afx_msg void OnCbnSelchangedRModel();
    afx_msg void OnCbnSelchangedRTargt();
    afx_msg void OnCbnSelchangedBlend();
    afx_msg void OnCbnSetfocusLModel();
    afx_msg void OnCbnSetfocusLTargt();
    afx_msg void OnCbnSetfocusRModel();
    afx_msg void OnCbnSetfocusRTargt();

};

#define GETSFX(pScene) \
    IEKG3DSFX* pSFX = NULL;\
    pScene->GetSFX(&pSFX);\
    _ASSERTE(pSFX);\
    if (!pSFX) return

#define GETCHANNEL(pSFX) \
    IEKG3DSFXChannel* pChannel = NULL;\
    pSFX->GetChannel(&pChannel);\

#endif //_INCLUDE_KSCENESFXEDITCHANNEL_H_

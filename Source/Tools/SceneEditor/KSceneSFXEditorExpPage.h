#pragma once

#include "KGLilisSlipPanel.h"
#include "KEnterEdit.h"
#include "KGLilisPanel.h"
#include "IEKG3DSfx.h"
#include "KColorDialog.h"

// KSceneSFXEditorExpPage dialog

class KSceneSFXEditorExpPage : public KGLilisSlipPanel<CPropertyPage>
{
	DECLARE_DYNAMIC(KSceneSFXEditorExpPage)

public:
	KSceneSFXEditorExpPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSFXEditorExpPage();

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_EXP };

    BOOL OnInitDialog();
    BOOL OnSetActive();
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    IKG3DSfxBoxProjection* GetSelBoxProjection();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    KEnterEdit m_EditTimeScal;
    KEnterEdit m_EditSwingX;
    KEnterEdit m_EditFrequX;
    KEnterEdit m_EditSwingY;
    KEnterEdit m_EditFrequY;
    KEnterEdit m_EditSwingZ;
    KEnterEdit m_EditFrequZ;
    KEnterEdit m_EditScanX;
    KEnterEdit m_EditScanY;
    KEnterEdit m_EditScanZ;

    KEnterEdit m_EditUV_X;
    KEnterEdit m_EditUV_Y;
    KEnterEdit m_EditUV_Z;
    KEnterEdit m_EditUV_W;

    KEnterEdit m_EditClipU;
    KEnterEdit m_EditClipV;
    KEnterEdit m_EditFrameSpeed;

    KGLilisButton m_Button0;
    KGLilisButton m_Button1;
    KGLilisButton m_Button2;

    KColorPickerButton m_btColor;
    D3DXCOLOR m_Color;

    void UpdateUI();

    afx_msg void OnEnSetfocusEditTime();
    afx_msg void OnEnSetfocusEditSwing();
    afx_msg void OnEnSetfocusEditFrequ();
    afx_msg void OnEnSetfocusModelScanl();
    afx_msg void OnBnClickedCheckShowScanlMdl();
    afx_msg void OnBnClickedButtonAddVp();
    afx_msg void OnCbnSelchangeComboVp();
    afx_msg void OnCbnSetfocusComboVp();
    afx_msg void OnBnClickedButtonDelVp();
    afx_msg void OnCbnSelchangeComboVpType();
    afx_msg void OnBnClickedColorButtonVp();
    afx_msg void OnEnSetfocusUVTime();
    afx_msg void OnCbnSelchangeComboBlendType();
    afx_msg void OnBnClickedButtonBrowseTexture();
};

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorBillboardPage.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-12-1 16:07:43
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORBILLBOARDPAGE_H_
#define _INCLUDE_KSCENESFXEDITORBILLBOARDPAGE_H_

////////////////////////////////////////////////////////////////////////////////
#include "KSceneSFXEditorDoc.h"
#include "KEnterEdit.h"
#include "KGLilisPanel.h"

// KSceneSFXEditorBillboardPage dialog
interface IEKG3DSceneSFXEditor;
interface IEKG3DSFXBillboard;

class KSceneSFXEditorBillboardPage : public CDialog
{
	DECLARE_DYNAMIC(KSceneSFXEditorBillboardPage)

public:
	KSceneSFXEditorBillboardPage(CWnd* pWnd = NULL);
	virtual ~KSceneSFXEditorBillboardPage();

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_BILLBOARD };

    void UpdateUI();

	void UpdateBillboardList();
    void UpdateBillboardSelected();
    void UpdateBillboardUI();

    void UpdateSpecifiedUI();

public:

    /////////////////////////////////////////////
    CComboBox   m_comboBillboard;
	KEnterEdit	m_editBillboardName;

    KGLilisButton     m_buttonAddBillboard;
    KGLilisButton     m_buttonDelBillboard;

    CComboBox   m_comboBlendMode;
    CComboBox   m_comboType;

    CButton     m_radioWidth;
    KEnterEdit  m_editWidth;

    CButton     m_radioHeight;
    KEnterEdit  m_editHeight;

    CButton     m_radioColor;
    CStatic     m_picColor;

    D3DXCOLOR   m_Color;

    CButton     m_radioTextureFrameIndex;
    KEnterEdit  m_editTextureFrameIndex;

    CButton     m_radioTexture2FrameIndex;
    KEnterEdit  m_editTexture2FrameIndex;

    KGLilisButton     m_checkRotationAnimation;
    CButton     m_radioRotationAnimation;
    KEnterEdit  m_editRotationAnimation;

    KGLilisButton     m_checkSpecialAdjustAxis;

    CButton     m_radioUVOffset;
    KEnterEdit  m_editOffsetU;
    KEnterEdit  m_editOffsetV;

    KEnterEdit  m_editTextureName;
    KGLilisButton     m_buttonBrowseTextureName;

    KEnterEdit  m_editTexture2Name;
    KGLilisButton     m_buttonBrowseTexture2Name;

    KEnterEdit  m_editTextureCut;
    KEnterEdit  m_editTexture2Cut;

private:
    void OnSetBillboardWidth();
    void OnSetBillboardHeight();

    void OnSetBillboardTextureFrameIndex();
    void OnSetBillboardTexture2FrameIndex();

	void OnSetBillboardName();

    void OnSetUOffset();
    void OnSetVOffset();
    
    void OnSetRotation();

    void OnSetTexture();
    void OnSetTexture2();

    void OnSetTextureCutNum();
    void OnSetTexture2CutNum();

    bool BrowseTexture(CString &strTextureName);

    // Update Data
    void UpdateBillboardName(IEKG3DSceneSFXEditor *pScene, int nSelect);
    void UpdateBillboardAjustAxis(IEKG3DSFXBillboard *pBillboard);
    void UpdateBillboardColor(IEKG3DSFXBillboard *pBillboard, int nFrame);
    void UpdateBillboardWidth(IEKG3DSFXBillboard *pBillboard, int nFrame);
    void UpdateBillboardHeight(IEKG3DSFXBillboard *pBillboard, int nFrame);
    void UpdateTextureFrameIndex1(IEKG3DSFXBillboard *pBillboard, int nFrame);
    void UpdateTextureFrameIndex2(IEKG3DSFXBillboard *pBillboard, int nFrame);
    void UpdateRotation(IEKG3DSFXBillboard *pBillboard, int nFrame);
    void UpdateUV(IEKG3DSFXBillboard *pBillboard, int nFrame);
    void UpdateTextureName1(IEKG3DSFXBillboard *pBillboard);
    void UpdateTextureName2(IEKG3DSFXBillboard *pBillboard);
    void UpdateTextureCut1(IEKG3DSFXBillboard *pBillboard);
    void UpdateTextureCut2(IEKG3DSFXBillboard *pBillboard);
    void UpdateBlendMode(IEKG3DSFXBillboard *pBillboard);
    void UpdateType(IEKG3DSFXBillboard *pBillboard);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnOK(){};
    void OnCancel(){};
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAddBillboard();
    afx_msg void OnBnClickedButtonDelBillboard();
    afx_msg void OnCbnSelchangeComboBillboardList();
    afx_msg void OnCbnDropdownComboBillboardList();

    afx_msg void OnBnClickedRadioBillboardWidth();
    afx_msg void OnBnClickedRadioBillboardHeight();
    afx_msg void OnBnClickedRadioBillboardColor();
    afx_msg void OnStnClickedBillboardColorPic();
    afx_msg void OnBnClickedRadioBillboardTextureIndex();
    afx_msg void OnBnClickedRadioBillboardTexture2Index();
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedButtonBillboardBrowseTextureName();
    afx_msg void OnBnClickedButtonBillboardBrowseTexture2Name();

    afx_msg void OnBnClickedCheckRotationAnimation();
    afx_msg void OnBnClickedCheckAdjustAxis();
    afx_msg void OnBnClickedRadioUvOffset();

    afx_msg void OnBnClickedRadioRotationAnimation();
    
    virtual BOOL OnSetActive();
public:
    afx_msg void OnEnSetfocusEditBillboardWidth();
public:
    afx_msg void OnEnSetfocusEditBillboardHeight();
public:
    afx_msg void OnEnSetfocusEditBillboardTextureIndex();
public:
    afx_msg void OnEnSetfocusEditBillboardTexture2Index();
public:
    afx_msg void OnEnSetfocusEditRotationAnimation();
public:
    afx_msg void OnEnSetfocusEditUOffset();
public:
    afx_msg void OnEnSetfocusEditVOffset();
public:
    virtual BOOL OnInitDialog();
public:
    afx_msg void OnCbnSelchangeComboBlendMode();
    afx_msg void OnCheckRenderTarget();
    afx_msg void OnCbnSelchangeComboType();
};

#define GET_BILLBOARD()\
    int nSelect = m_comboBillboard.GetCurSel();\
    if (nSelect == CB_ERR)\
        return;\
    IEKG3DSFX *pSFX = NULL;\
    pScene->GetSFX(&pSFX);\
    if (!pSFX)\
        return;\
    IEKG3DSFXBillboard *pBillboard = NULL;\
    pSFX->GetBillboard(nSelect, &pBillboard );\
    if (!pBillboard)\
        return

#define GET_BILLBOARD_RET(n)\
    int nSelect = m_comboBillboard.GetCurSel();\
    if (nSelect == CB_ERR)\
        return n;\
    IEKG3DSFX *pSFX = NULL;\
    pScene->GetSFX(&pSFX);\
    if (!pSFX)\
        return n;\
    IEKG3DSFXBillboard *pBillboard = NULL;\
    pSFX->GetBillboard(nSelect, &pBillboard );\
    if (!pBillboard)\
        return n

#endif //_INCLUDE_KSCENESFXEDITORBILLBOARDPAGE_H_

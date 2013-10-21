// ***************************************************************
//
//  Description: 
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-06-06
//	-------------------------------------------------------------
//	History: 
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
#pragma once
#include "KEnterEdit.h"
#include "KGLilisPanel.h"
#include "afxwin.h"
#include "KColorDialog.h"

class IEKG3DSFXMagicParpet;
// KSceneSFXEditorMagicParpet dialog

class KSceneSFXEditorMagicParpet : public CDialog
{
	DECLARE_DYNAMIC(KSceneSFXEditorMagicParpet)

public:
	KSceneSFXEditorMagicParpet(CWnd* pWnd = NULL);
	virtual ~KSceneSFXEditorMagicParpet();

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_MAGICPARPET };

    KGLilisButton m_buttonAddParpet;
    KGLilisButton m_buttonDelParpet;

   // KEnterEdit  m_editGridSize;
    KEnterEdit  m_editGridNum;
    KEnterEdit  m_editClipHeight;
    KEnterEdit  m_editHeightOffset;
	KEnterEdit  m_editLife;
    KEnterEdit  m_editTextureName;
    KGLilisButton m_buttonBrowseTexture;
	
	CEdit m_editFPTextureName;
	KEnterEdit m_EditFootWidth;
	CButton m_ButtonAddFootprint;
	CButton m_Button_DelFootprint;
	CButton m_Button_Browse;
	CComboBox m_ComboFootprintType;
	/*KEnterEdit m_EditOffsetX;
	KEnterEdit m_EditOffsetY;*/
    bool m_bInit;

    void UpdateMagicParpetProperty();
	void UpdateFootprintProperty();
    void OnSetGridNum();
    void OnSetGridSize();
    void OnSetClipHeight();
    void OnSetHeightOffset();

    int  UpdateUI();

    BOOL OnSetActive();
    void OnOK(){};
    void OnCancel(){};
	
	void OnSetFootWidth();
	void OnSetLife();

    IEKG3DSFXMagicParpet* GetSelAoe();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAddParpet();
public:
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedButtonDelParpet();
public:
    afx_msg void OnBnClickedButtonBrowseTexture();
public:
	afx_msg void OnBnClickedAddFootprint();
	afx_msg void OnBnClickedDelFootprint();
	afx_msg void OnBnClickedBrowseTexture();
	
    CComboBox m_comboType;
    CComboBox m_comboFace;
    CComboBox m_comboAoes;
    afx_msg void OnCbnSelchangeCombo5();
    afx_msg void OnCbnSelchangeCombo1();
    afx_msg void OnCbnSelchangeCombo6();
    KColorPickerButton m_stColor;
    afx_msg void OnStnClickedTlColor();
    afx_msg void OnBnClickedTlColor();
    afx_msg void OnEnSetfocusEditGridNum();
    afx_msg void OnCbnSelchangeComboFootprinttype();
    KEnterEdit m_EditU;
    KEnterEdit m_EditV;
    KEnterEdit m_EditSpeed;
};

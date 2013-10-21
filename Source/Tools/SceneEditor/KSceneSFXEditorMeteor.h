// ***************************************************************
//
//  Description: Do you see my past ?
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-05-25
//	-------------------------------------------------------------
//	History: 
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
#pragma once

// KSceneSFXEditorMeteor dialog
#include "Resource.h"
#include "KEnterEdit.h"
#include "KGLilisPanel.h"

class KG3DSFXMeteor;

class KSceneSFXEditorMeteor : public CDialog
{
	DECLARE_DYNAMIC(KSceneSFXEditorMeteor)

public:
	KSceneSFXEditorMeteor(CWnd* pWnd = NULL);
	virtual ~KSceneSFXEditorMeteor();

    KEnterEdit  m_editWidth;
    KEnterEdit  m_editLife;
    KEnterEdit  m_editTrackIdx;
    KEnterEdit  m_editTextureIdx;
    KEnterEdit  m_editEmitInterval;
    KEnterEdit  m_editDutTime;

    CButton     m_checkRandomOffset;
    KEnterEdit  m_editOffsetX;
    KEnterEdit  m_editOffsetY;
    KEnterEdit  m_editOffsetZ;
    KEnterEdit  m_editRallying;
    KEnterEdit  m_editInterporateLength;

    CButton m_checkAddBlend;

    CListCtrl   m_listMeteor;

    KGLilisButton m_buttonAddMeteor;
    KGLilisButton m_buttonDeleteMeteor;

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_METEOR };

    void UpdateUI();

    void UpdateProperties();
    void UpdateMeteorList();

    void OnSetLife();
    void OnSetWidth();
    void OnSetEmitInterval();
    void OnSetTrackIdx();
    void OnSetTextureIdx();
    void OnSetOffsetX();
    void OnSetOffsetY();
    void OnSetOffsetZ();
    void OnSetRallying();
    void OnSetInterporateLength();
    void OnOK(){};
    void OnCancel(){};
    int  DelMeteor(KG3DSFXMeteor* pMeteor);
	
	void OnMeteorOpenClost();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedButtonAddMeteor();
public:
    afx_msg void OnBnClickedButtonDelMeteor();
public:
    virtual BOOL OnInitDialog();
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnNMClickListMeteor(NMHDR *pNMHDR, LRESULT *pResult);
public:
    virtual BOOL OnSetActive();
public:
    afx_msg void OnBnClickedCheckAddBlend();
public:
    afx_msg void OnBnClickedCheckRandomOffset();
public:
    afx_msg void OnBnClickedInterpolate();
	afx_msg void OnLvnItemchangedListMeteor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioSampling();
	afx_msg void OnBnClickedRadioSamplingStop();
	
	
};

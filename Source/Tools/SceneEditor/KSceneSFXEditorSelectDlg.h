// ***************************************************************
//
//  Description: 你的选择就是我的选择
//  -------------------------------------------------------------
//	Author: Zeb
//  -------------------------------------------------------------
//	Create Date: 2006-03-08
//	-------------------------------------------------------------
//	History: 
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
//
// ***************************************************************
#pragma once

// KSceneSFXEditorSelectDlg dialog

class KSceneSFXEditorSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(KSceneSFXEditorSelectDlg)

public:
	KSceneSFXEditorSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSFXEditorSelectDlg();

// Dialog Data
	enum { IDD = IDD_SFXEDITOR_SELECT };

    CListCtrl   m_listSelect;

    void UpdateUI();
    void UpdateList();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
public:
    afx_msg void OnLvnItemchangedSfxeditorElement(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnLvnEndlabeleditSfxeditorElement(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnLvnKeydownSfxeditorElement(NMHDR *pNMHDR, LRESULT *pResult);
};

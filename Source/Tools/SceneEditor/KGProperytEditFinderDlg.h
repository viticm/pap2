#pragma once

#include "KGPropertyEditTabDlg.h"

class KGProperytEditFinderDlg : public CDialog
{
	DECLARE_DYNAMIC(KGProperytEditFinderDlg)

public:
	KGProperytEditFinderDlg(CWnd* pParent, LPKGLISTITEM pItem);    // standard constructor
	virtual ~KGProperytEditFinderDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG26 };

public :
	KGPropertyListCtrl*			m_pListFrame;
	LPKGLISTITEM			    m_pListItem;
	CComboBox                   m_comboFindString;
	
	int							m_nBeginFindFLag;

public :
	virtual BOOL OnInitDialog();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnCbnSelchangeCombo();
public:
	afx_msg void OnCbnEditchangeCombo();
};

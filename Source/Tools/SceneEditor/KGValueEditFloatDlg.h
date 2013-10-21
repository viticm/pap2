#pragma once

enum SHOW_TYPE
{
	FT_PERCENTAGE,
	FT_FLOAT
};

#include "KGPropertyEditTabDlg.h"

class KGValueEditFloatDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditFloatDlg)

public:
	KGValueEditFloatDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		SHOW_TYPE eType = FT_FLOAT, CWnd* pParent = NULL
	);  
	virtual ~KGValueEditFloatDlg();
	enum { IDD = IDD_VALUE_EDIT_FLOAT };

public :
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	CWnd*			m_pParant;	
	LPKGLISTITEM	m_pListItem;
	CSpinButtonCtrl m_spinButton;
	SHOW_TYPE		m_eShowType;
	int				m_nPecentageValue;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeValue();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);

};

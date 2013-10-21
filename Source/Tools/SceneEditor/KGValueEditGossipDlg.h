#pragma once


#include "KGPropertyEditTabDlg.h"
#include "afxcmn.h"
#include "afxwin.h"

class KGValueEditGossipDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditGossipDlg)

public:
	KGValueEditGossipDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEditGossipDlg();

// Dialog Data
	enum { IDD = IDD_VALUE_EDIT_GOSSIP };
	
public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CWnd*			m_pParant;

	CString			m_szGossipText[4];
	CTabCtrl		m_tabCtrl;
	CString			m_szText;
	CComboBox		m_comboType;
	char			m_szTypeTrue[256];
	char			m_szTypeFalse[256];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
};

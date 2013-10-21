#pragma once

#include "KGPropertyEditTabDlg.h"
class KGValueEidtPrequestIDsDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEidtPrequestIDsDlg)

public:
	KGValueEidtPrequestIDsDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEidtPrequestIDsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG22 };

public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CWnd*			m_pParant;
	CString			m_szPreName1;
	CString			m_szPreName2;
	CString			m_szPreName3;
	CString			m_szPreName4;
	CString			m_szPreID1;
	CString			m_szPreID2;
	CString			m_szPreID3;
	CString			m_szPreID4;
	CButton			m_BnAnd;
	CButton			m_BnOr;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedOr();
public:
	afx_msg void OnBnClickedAnd();
};

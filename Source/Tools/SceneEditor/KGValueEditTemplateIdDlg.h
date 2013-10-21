#pragma once


#include "KGPropertyEditTabDlg.h"

class KGValueEditTemplateIdDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditTemplateIdDlg)

public :
	KGValueEditTemplateIdDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);   
	virtual ~KGValueEditTemplateIdDlg();
	enum { IDD = IDD_DIALOG29 };

public :
	virtual BOOL OnInitDialog();

public :
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CComboBox	    m_comboxIDs;
	
protected :
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

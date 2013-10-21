////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditBoolDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:08:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGVALUEEDITINTBOOLDLG_H_
#define _KGVALUEEDITINTBOOLDLG_H_

#include "KGPropertyEditTabDlg.h"

class KGValueEditBoolDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditBoolDlg)

public:
	KGValueEditBoolDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);   
	virtual ~KGValueEditBoolDlg();
	enum { IDD = IDD_VALUE_EDIT_BOOL };

public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CWnd*			m_pParant;
	CComboBox       m_comboxValue;
	int             m_nCurRow;
	CString			m_pszState[3];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
};

#endif // _KGVALUEEDITINTDLG_H_
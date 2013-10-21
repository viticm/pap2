////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditStringDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:08:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGVALUEEDITINTSTRINGDLG_H_
#define _KGVALUEEDITINTSTRINGDLG_H_

#include "KGPropertyEditTabDlg.h"

class KGValueEditStringDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditStringDlg)

public:
	KGValueEditStringDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);   
	virtual ~KGValueEditStringDlg();
	enum { IDD = IDD_VALUE_EDIT_STRING };

public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CWnd*			m_pParant;
	CString         m_stringValue;
	CString			m_stringPreTitle;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeValue();
};

#endif // _KGVALUEEDITINTDLG_H_
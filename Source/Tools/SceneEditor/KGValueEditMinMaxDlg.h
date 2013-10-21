////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditMinMaxDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:08:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGVALUEEDITINTMINMAXDLG_H_
#define _KGVALUEEDITINTMINMAXDLG_H_

#include "KGPropertyEditTabDlg.h"

class KGValueEditMinMaxDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditMinMaxDlg)
public:
	KGValueEditMinMaxDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);   
	virtual ~KGValueEditMinMaxDlg();
	enum { IDD = IDD_VALUE_EDIT_MIN_MAX };

public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CWnd*			m_pParant;
	CSpinButtonCtrl m_spinButtonMin;
	CSpinButtonCtrl m_spinButtonMax;
	int             m_nValueMin;
	int				m_nValueMax;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
};

#endif // _KGVALUEEDITINTDLG_H_
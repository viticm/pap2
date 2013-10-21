////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:08:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
 
#ifndef _KGVALUEEDITINTDLG_H_
#define _KGVALUEEDITINTDLG_H_

#include "KGPropertyEditTabDlg.h"

class KGValueEditIntDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditIntDlg)
public :
	KGValueEditIntDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEditIntDlg();
	enum { IDD = IDD_VALUE_EDIT_INT };

public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	CWnd*			m_pParant;	
	int             m_nValue;
	CSpinButtonCtrl m_spinButton;
	LPKGLISTITEM    m_pListItem;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
};

#endif // _KGVALUEEDITINTDLG_H_
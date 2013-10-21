////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditForceDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-11 16:45:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGVALUEEDITFORCEDLG_H_
#define _INCLUDE_KGVALUEEDITFORCEDLG_H_

#include "KGPropertyEditTabDlg.h"

class KGValueEditForceDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditForceDlg)

public:
	KGValueEditForceDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);   
	virtual ~KGValueEditForceDlg();

	enum { IDD = IDD_DLG_FORCE };
	virtual BOOL OnInitDialog();

public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CComboBox	    m_comboxForce;
	CWnd*			m_pParant;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

#endif //_INCLUDE_KGVALUEEDITFORCEDLG_H_

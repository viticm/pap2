////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditQuestTypeDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-1-5 17:06:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGVALUEEDITQUESTTYPE_H_
#define _KGVALUEEDITQUESTTYPE_H_

#include "KGPropertyEditTabDlg.h"

class KGValueEditQuestTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditQuestTypeDlg)

public:
	KGValueEditQuestTypeDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);   
	virtual ~KGValueEditQuestTypeDlg();
	enum { IDD = IDD_QUEST_TYPE };

	virtual BOOL OnInitDialog();

public:
	afx_msg void OnCbnSelect();
	afx_msg void OnBnOK();

public:
	ITabFile*		m_pTabFile;
	IIniFile*		m_pIniFile;
	LPKGLISTITEM	m_pListItem;
	CWnd*			m_pParant;
	CComboBox       m_comboxType;
	int             m_nIconId;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	DECLARE_MESSAGE_MAP()
};

#endif // _KGVALUEEDITQUESTTYPE_H_
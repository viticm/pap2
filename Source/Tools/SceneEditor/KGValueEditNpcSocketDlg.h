////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditNpcSocketDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-12 11:50:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGVALUEEDITNPCSOCKETDLG_H_
#define _INCLUDE_KGVALUEEDITNPCSOCKETDLG_H_

#include "KGValueEditNpcAniSoundDlg.h"

class KGValueEditNpcSocketDlg;

#define  CTL_BROWSE		100
#define  CTL_NUMBER     150

class KGListCtrl_NpcSocket : public KGListCtrl
{
public :
	KGListCtrl_NpcSocket();
	virtual ~KGListCtrl_NpcSocket();
	int SetMainDlg(KGValueEditNpcSocketDlg* pMainDlg);

public :
	KGClassButton		m_bnBrowse;
	KGValueEditNpcSocketDlg* m_pMianDlg;

	int m_nCurItem;

public :
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBrowseClick();
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};

class KGValueEditNpcSocketDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditNpcSocketDlg)

public :
	KGValueEditNpcSocketDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEditNpcSocketDlg();
	enum { IDD = IDD_DIALOG_NPC_SOCKET };

	BOOL OnInitDialog();

public :
	ITabFile*			m_pTabFile;
	IIniFile*			m_pIniFile;
	LPKGLISTITEM		m_pListItem;
	CWnd*				m_pParant;

	KGListCtrl_NpcSocket    m_listCtrl;

	map<CString, Property_Pate_Data>	m_mapProperty;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()

public :
	afx_msg void OnBnClickedOk();
};


#endif //_INCLUDE_KGVALUEEDITNPCSOCKETDLG_H_

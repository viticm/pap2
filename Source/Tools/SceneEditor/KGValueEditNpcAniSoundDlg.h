////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditNpcAniSoundDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-8 10:34:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGVALUEEDITNPCANISOUNDDLG_H_
#define _INCLUDE_KGVALUEEDITNPCANISOUNDDLG_H_

#include "KG3DObjectEditor.h"

#define  CTL_BROWSE		100
#define  CTL_NUMBER     150
#define  PAGE_SIZE		9
#define  ITEM_COUNT		4
#define  SUB_COUNT		4

class KGValueEditNpcAniSoundDlg;

class KGListCtrl_NpcAniSound : public KGListCtrl
{
public :
	KGListCtrl_NpcAniSound();
	virtual ~KGListCtrl_NpcAniSound();
	int SetMainDlg(KGValueEditNpcAniSoundDlg* pMainDlg);

public :
	KGClassButton		m_bnBrowse;
	CSpinButtonCtrl		m_spinCtrl;
	KGValueEditNpcAniSoundDlg* m_pMianDlg;

	int m_nCurPage;
	int m_nCurItem;
	int m_nCurSub;
	
public :
	DECLARE_MESSAGE_MAP()
	afx_msg	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBrowseClick();
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};

struct Property_Pate_Data
{
	CString	 m_stringPate;
	CString  m_stringData;

	Property_Pate_Data()
	{
		m_stringPate = _T("");
		m_stringData = _T("");
	}
};

class KGValueEditNpcAniSoundDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditNpcAniSoundDlg)

public :
	KGValueEditNpcAniSoundDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEditNpcAniSoundDlg();
	enum { IDD = IDD_VALUEEDIT_NPC_ANI_SOUNT };

	BOOL OnInitDialog();

public :
	CTabCtrl			m_tabCtrlPage;
	KGListCtrl_NpcAniSound		m_listCtrl;

	ITabFile*			m_pTabFile;
	IIniFile*			m_pIniFile;
	LPKGLISTITEM		m_pListItem;
	CWnd*				m_pParant;

	Property_Pate_Data	m_pateData[PAGE_SIZE][ITEM_COUNT][SUB_COUNT];

protected :
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


#endif //_INCLUDE_KGVALUEEDITNPCANISOUNDDLG_H_

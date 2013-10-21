////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditNpcNumDataDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-13 20:18:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGVALUEEDITNPCNUMDATADLG_H_
#define _INCLUDE_KGVALUEEDITNPCNUMDATADLG_H_

#include "KGValueEditNpcAniSoundDlg.h"

#define  INT_DATA_SIZE	18

class KGValueEditNpcNumDataDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditNpcNumDataDlg)

public :
	KGValueEditNpcNumDataDlg(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEditNpcNumDataDlg();
	enum { IDD = IDD_VALUE_NPC_NUM };

	BOOL OnInitDialog();

public :
	ITabFile*			m_pTabFile;
	IIniFile*			m_pIniFile;
	LPKGLISTITEM		m_pListItem;
	CWnd*				m_pParant;

	int					m_nLevel;
	int					m_nIntensity;
	int					m_nMaxLife;
	int					m_nLifeReplenish;
	int					m_nLifeReplenishPercent;
	int					m_nMaxMana;
	int					m_nManaReplenish;
	int					m_nManaReplenishPercent;
	int					m_nMaxRage;
	int					m_nHit;
	int					m_nDodge;
	int					m_nCriticalStrike;
	int					m_nPhysicsShieldBase;
	int					m_nSolarMagicDefence;
	int					m_nNeutralMagicDefence;
	int					m_nLunarMagicDefence;
	int					m_nPoisonMagicDefence;

	/*vector<Property_Pate_Data> m_vectorPropertyData;*/

protected :
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};


#endif //_INCLUDE_KGVALUEEDITNPCNUMDATADLG_H_

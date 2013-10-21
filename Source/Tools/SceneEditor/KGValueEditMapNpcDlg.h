////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditMapNpcDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:08:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGVALUEEDITINTMAPNCPDLG_H_
#define _KGVALUEEDITINTMAPNPCDLG_H_

#include "KGPropertyEditTabDlg.h"

class KGValueEditMapNpcDlg : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditMapNpcDlg)

public:
	KGValueEditMapNpcDlg(
		KGPropertyListCtrl* pList, int nItem, ITabFile* pFile, 
		int nCurRow, CWnd* pParent = NULL
	);   
	virtual ~KGValueEditMapNpcDlg();
	enum { IDD = IDD_VALUE_EDIT_MAP_NPC };

public:
	int             m_nCurRow;
	ITabFile*       m_pTabFile;
	KGPropertyListCtrl*     m_pList;
	int             m_nItem;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
};

#endif // _KGVALUEEDITINTDLG_H_
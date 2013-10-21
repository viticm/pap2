////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCreateQuestDlg.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-12-14 10:08:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGCREATEQUESTDLG_H_
#define _KGCREATEQUESTDLG_H_

#include "KGPropertyEditTabDlg.h"

class KGCreateQuestDlg : public CDialog
{
	DECLARE_DYNAMIC(KGCreateQuestDlg)

public:
	KGCreateQuestDlg(
		CTreeCtrl* pTree, HTREEITEM hItem, ITabFile* pFile, 
		CString szMap, CString szLink, CString szQuest,
		QUEST_TYPE eType, CWnd* pParent = NULL
	);   
	virtual ~KGCreateQuestDlg();
	enum { IDD = IDD_CREATE_QUEST };

public:
	CTreeCtrl*		m_pTree;
	ITabFile*       m_pTabFile;
	HTREEITEM       m_hItem;
	QUEST_TYPE	    m_eType;
	CString			m_szName;

	CString			m_szMapName;
	CString			m_szQuestLinkName;
	CString			m_szQuestName;

	int             m_nOkFlag;
	
	CString         m_szQuestType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
};

#endif // _KGVALUEEDITINTDLG_H_
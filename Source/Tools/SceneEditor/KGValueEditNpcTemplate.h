////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditNpcTemplate.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-16 11:58:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGVALUEEDITNPCTEMPLATE_H_
#define _INCLUDE_KGVALUEEDITNPCTEMPLATE_H_

#include "KGObjectEditerTreeCtrl.h"
#include "KGPropertyEditTabDlg.h"
#include "KG3DObjectEditor.h"

class KGValueEditNpcTemplate : public CDialog
{
	DECLARE_DYNAMIC(KGValueEditNpcTemplate)

public :
	KGValueEditNpcTemplate(
		ITabFile* pTabFile, IIniFile* pIniFile, LPKGLISTITEM pListItem, 
		CWnd* pParent = NULL
	);
	virtual ~KGValueEditNpcTemplate();
	enum { IDD = IDD_DIALOG_NPC_TEMPLATE };

	BOOL OnInitDialog();

public :
	KGObjectEditerTreeCtrl	m_treeCtrl;
	KGPropertyListCtrl		m_listCtrl;
	CImageList          m_imageListTree;

	ITabFile*			m_pTabFile;
	IIniFile*			m_pIniFile;
	LPKGLISTITEM		m_pListItem;

	CWnd*				m_pParant;

	KGSubEditer			m_subEditer;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
};

#endif //_INCLUDE_KGVALUEEDITNPCTEMPLATE_H_

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGObjectEditerTreeCtrl.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-3-24 12:53:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGOBJECTEDITERTREECTRL_H_
#define _KGOBJECTEDITERTREECTRL_H_

#include "KGPropertyEditUtility.h"
#include "KGTreeCtrl.h"

class KGSubEditer;

class KGObjectEditerTreeCtrl : public KGTreeCtrl
{
public :
	KGSubEditer* m_pSubEditer;

public :
	virtual int BeginStick(HTREEITEM hSour, HTREEITEM hDest, MOVE_TYPE eCopyType);
	virtual int AfterCopyItem(
		HTREEITEM hCopyItem, HTREEITEM hNewItem, HTREEITEM hParentItem, HTREEITEM hAfterItem 
	);
	virtual int BeginDelItem(HTREEITEM hTreeItem);
	int Update(KGSubEditer* pSubEditer);
	HTREEITEM GetInsertNode(HTREEITEM hStartItem, CString szClassPath);
	int RefrushClassInfo(HTREEITEM hTreeItem);
	int DelRowInTabFile(HTREEITEM hTreeItem);
	HTREEITEM GetItemByPrimaryKey(HTREEITEM hStartItem, DWORD_PTR nPrimaryKey);

	static int RefrushClassInfoEx(CTreeCtrl* pTree, HTREEITEM hTreeItem);
public :
	KGObjectEditerTreeCtrl();
	~KGObjectEditerTreeCtrl();

protected :
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();	
	DECLARE_MESSAGE_MAP()
};

#endif // _KGOBJECTEDITERTREECTRL_H_
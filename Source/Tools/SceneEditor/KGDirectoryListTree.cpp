#include "StdAfx.h"
#include "KGDirectoryListTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


void KGDirectoryListTree::Helper_UpdateUI(CWnd* pWnd, const Directory& Child, HTREEITEM hParent)
{
	CTreeCtrl* pTree = static_cast<CTreeCtrl*>(pWnd);
	HTREEITEM hThis = TVI_ROOT;
	int nIndex = 0;

	hThis = pTree->InsertItem(Child.DirName, hParent);
	pTree->SetItemData(hThis, NULL);

	for (std::list<CString>::const_iterator i = Child.FileName.begin(),  j = Child.FilePath.begin();
		i != Child.FileName.end();
		i++, j++)
	{
		HTREEITEM hChild = pTree->InsertItem((*i), hThis);
		pTree->SetItemData(hChild, (DWORD_PTR)(LPCTSTR(*j)));
	}

	for (std::list<Directory*>::const_iterator i = Child.Children.begin();
		i != Child.Children.end();
		i++)
	{
		Helper_UpdateUI(pWnd, **i, hThis);
	}
}

void KGDirectoryListTree::UpdateUI()
{
	for (std::list<CWnd*>::iterator i = m_Wnds.begin();
		i != m_Wnds.end();
		i++)
	{
		CTreeCtrl* pWnd = static_cast<CTreeCtrl*>(*i);
		pWnd->DeleteAllItems();

		Helper_UpdateUI(pWnd, m_Root, TVI_ROOT);

	}
}
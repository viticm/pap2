#pragma once
#include "kgdirectorylisttree.h"

class KGNPCItemListTree : public KGDirectoryListTree
{
public:
	virtual void UpdateUI();
	virtual void FindFiles(CFileFind& FindFile, 
		Directory& Dir, 
		BOOL bIncludeSubDir);
protected:
	virtual void Helper_UpdateUI(CWnd* pWnd, const Directory& Child, HTREEITEM hParent);
};

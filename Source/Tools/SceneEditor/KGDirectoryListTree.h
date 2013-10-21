#pragma once

#include "directorylist.h"

class KGDirectoryListTree : public CDirectoryList
{
public:
	virtual void UpdateUI();
protected:
	virtual void Helper_UpdateUI(CWnd* pWnd, const Directory& Child, HTREEITEM hParent);
};

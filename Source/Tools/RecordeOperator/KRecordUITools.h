#pragma once
#include "KGMemDateList.h"

struct IERecPlayer;
class KRecordUITools
{
public:
	void FillEvent(IERecPlayer *pPlayer, 
		HTREEITEM& hItem, 
		CTreeCtrl& ListTree);
	
	void FillListTree(IERecPlayer *pPlayer, 
		CTreeCtrl& ListTree);

	void FillListCtrl(IERecPlayer *pPlayer, 
		int nDeeps,
		DWORD_PTR pPoint,
		KGMemDateList& listctrlObj);
private:
	HRESULT GetDesByID(string& strDes,
		FunctionType Ftype);
};

extern KRecordUITools g_RecordUITools;
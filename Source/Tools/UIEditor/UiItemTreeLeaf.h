////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemTreeLeaf.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 16:02:06
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMTREELEAF_H_
#define _INCLUDE_UIITEMTREELEAF_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemTreeLeaf: public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_TREE_LEAFE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiItemTreeLeaf();
	~KUiItemTreeLeaf();
private:
	KUiWndWindow *m_pItemCommon;
    KUiWndWindow *m_pItemHandleCommon;
	KUiWndWindow *m_pItemTreeLeafCommon;
};

#endif //_INCLUDE_UIITEMTREELEAF_H_

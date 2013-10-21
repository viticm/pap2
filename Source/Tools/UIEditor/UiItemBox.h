////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemBox.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-3-1 15:16:46
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMBOX_H_
#define _INCLUDE_UIITEMBOX_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemBox : public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_ANIAMTE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
	virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiItemBox();
	~KUiItemBox();
private:
	KUiWndWindow *m_pItemCommon;
	KUiWndWindow *m_pItemBoxCommon;
};


#endif //_INCLUDE_UIITEMBOX_H_

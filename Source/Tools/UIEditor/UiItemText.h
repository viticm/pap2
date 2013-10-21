////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemText.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 16:21:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMTEXT_H_
#define _INCLUDE_UIITEMTEXT_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemText : public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_TEXT; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiItemText();
	~KUiItemText();
private:
	KUiWndWindow *m_pItemCommon;
	KUiWndWindow *m_pItemTextCommon;
};

#endif //_INCLUDE_UIITEMTEXT_H_

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemImage.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 9:16:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMIMAGE_H_
#define _INCLUDE_UIITEMIMAGE_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemImage : public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_IMAGE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiItemImage();
	~KUiItemImage();
private:
	KUiWndWindow *m_pItemCommon;
	KUiWndWindow *m_pItemImageCommon;
};

#endif //_INCLUDE_UIITEMIMAGE_H_

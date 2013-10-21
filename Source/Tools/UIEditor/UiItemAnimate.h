////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemAnimate.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 13:47:52
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMANIMATE_H_
#define _INCLUDE_UIITEMANIMATE_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemAnimate : public KUiWndWindow
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
	KUiItemAnimate();
	~KUiItemAnimate();
private:
	KUiWndWindow *m_pItemCommon;
	KUiWndWindow *m_pItemAnimateCommon;
};


#endif //_INCLUDE_UIITEMANIMATE_H_

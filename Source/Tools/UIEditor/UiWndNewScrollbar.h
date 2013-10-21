////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiWndNewScrollbar.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-11 11:43:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIWNDNEWSCROLLBAR_H_
#define _INCLUDE_UIWNDNEWSCROLLBAR_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

 class KUiWndNewScrollBar: public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_SCROLLBAR; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiWndNewScrollBar();
	~KUiWndNewScrollBar();
private:
	KUiWndWindow *m_pWndNewScrollbarCommon;
	KUiWndWindow *m_pWndWindowCommon;
};


#endif //_INCLUDE_UIWNDNEWSCROLLBAR_H_

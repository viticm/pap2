////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemHandle.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-30 16:31:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMHANDLE_H_
#define _INCLUDE_UIITEMHANDLE_H_

#include "UiWndWindow.h"
#include "UiItemCommon.h"
#include "UiItemHandleCommon.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemHandle : public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_HANDLE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiItemHandle();
	~KUiItemHandle();
private:
	KUiWndWindow *m_pItemCommon;
	KUiWndWindow *m_pItemHandleCommon;
};



#endif //_INCLUDE_UIITEMHANDLE_H_

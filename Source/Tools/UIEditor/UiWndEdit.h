#pragma once
#include "UiWndWindow.h"
#include "UiWndEditCommon.h"
#include "UiWndWindowCommon.h"

class KUiWndEdit: public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_EDIT; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiWndEdit();
	~KUiWndEdit();

private:
	KUiWndWindow *m_pWndEditCommon;
	KUiWndWindow *m_pWndWindowCommon;

	
};

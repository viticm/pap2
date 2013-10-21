#pragma once

#include "UiWndWindow.h"
#include "UiWndWindowCommon.h"
#include "UiWndImageCommon.h"
#include "UiWndButtonCommon.h"

class KUiWndButton : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_BUTTON; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
    KUiWndButton();
    ~KUiWndButton();

private:
	KUiWndWindow *m_pWndImageCommon;
	KUiWndWindow *m_pWndWindowCommon;
    KUiWndWindow *m_pWndButtonCommon;
    
};

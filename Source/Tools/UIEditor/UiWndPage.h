# pragma once
#include "UiWndData.h"
#include "UiWndWindow.h"
#include "UiWndWindowCommon.h"
#include "UiWndImageCommon.h"

class KUiWndPage: public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_PAGE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
    KUiWndPage();
    ~KUiWndPage();
private:
	KUiWndWindow *m_pWndWindowCommon;
    KUiWndWindow *m_pWndImageCommon;
};
#ifndef K_UI_WND_WEB_PAGE_H
#define K_UI_WND_WEB_PAGE_H

#include "UiWndWindow.h"

class KUiWndWebPage : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_WEBPAGE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);

protected:
	KUiWndWebPage();
	~KUiWndWebPage();

private:
	KUiWndWindow* m_pWndWindowCommon;
	KUiWndWindow* m_pWndWebPageCommon;
};

#endif // K_UI_WND_WEB_PAGE_H

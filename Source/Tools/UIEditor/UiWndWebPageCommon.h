#ifndef K_UI_WND_WEB_PAGE_COMMON_H
#define K_UI_WND_WEB_PAGE_COMMON_H

#include "UiWndWindow.h"

class KUiWndWebPageCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_WEBPAGE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);

protected:
	KUiWndWebPageCommon();
	~KUiWndWebPageCommon();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void OnUpdateEditText(int nId);
	void OnButtonClick(int nId);
};


#endif //K_UI_WND_WEB_PAGE_COMMON_H

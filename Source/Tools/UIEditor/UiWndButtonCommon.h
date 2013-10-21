#pragma once
#include "UiWndWindow.h"

class KUiWndButtonCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_BUTTON; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);
protected:
    KUiWndButtonCommon();
    ~KUiWndButtonCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void    OnUpdateEditText(int nId);
    void    OnButtonClick(int nId);
    int     SelAnimate(int *pnSelResult);
};

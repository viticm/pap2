# pragma once
#include "UiWndData.h"
#include "UiWndWindow.h"

class KUiWndShowAnimateCommon: public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_SHOWANIMATE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);
protected:
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    KUiWndShowAnimateCommon();
    ~KUiWndShowAnimateCommon();
private:
    void	OnUpdateEditText(int nId);
    void    OnButtonClick(int nId);
};
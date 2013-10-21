#pragma once
#include "UiWndWindow.h"

class KUiWndFrameScript : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_FRAME; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
protected:
	KUiWndFrameScript();
	~KUiWndFrameScript();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
    char*   EquirotalStrStr(const char *pString, const char *pStrSearch);
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);
    int     OnSelLua();
    void    OnEditLua();
    void    OnCreateLua();
};
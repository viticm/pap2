#pragma once
#include "UiWndData.h"

class IIniFile;

class KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_WINDOW; }
    virtual char*           SectionName(){ return m_szName; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo) { };
    void                    ShowErrorInformation();

	HWND GetHWND();

protected:
	KUiWndWindow();
	~KUiWndWindow();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	HWND m_hWnd;
	char m_szName[128];
};

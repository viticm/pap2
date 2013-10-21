# pragma once
#include "UiWndData.h"
#include "UiWndWindow.h"
#include "UiWndWindowCommon.h"

class KUiWndPageSetCommon: public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_PAGESET; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);
protected:
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    KUiWndPageSetCommon();
    ~KUiWndPageSetCommon();
private:
    void	OnCheckBoxClick(int nBtnId);
    void    OnListNotify(LPNMHDR pNotify);
    void    OnChangeWndowName(int nWndIndex, int nKeyType);
    void    UpDataListView();
    void    OnDelPage();
    void    OnMovePage(bool bUp);
private:
    HWND m_hListWnd;
    int m_nCurrentPageNum;
};
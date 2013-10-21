# pragma once
#include "UiWndData.h"
#include "UiWndWindow.h"

class KUiWndFrameTabControl : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_FRAME; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);
protected:
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    KUiWndFrameTabControl();
    ~KUiWndFrameTabControl();
private:
    void	OnButtonClick(int nBtnId);
    void    OnListNotify(LPNMHDR pNotify);
    void    OnSelWnd();
    void    OnDel();
    void    SaveData();
    void    OnMove(bool bUp);
    BOOL    IsItemExist(LPCTSTR pszItem);
private:
    HWND m_hListWnd;
    int m_nCurrentItem;
};
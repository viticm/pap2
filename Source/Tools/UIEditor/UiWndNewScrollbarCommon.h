////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiWndNewScrollbarCommon.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-11 11:43:43
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIWNDNEWSCROLLBARCOMMON_H_
#define _INCLUDE_UIWNDNEWSCROLLBARCOMMON_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiWndNewScrollBarCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_SCROLLBAR; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
protected:
	KUiWndNewScrollBarCommon();
	~KUiWndNewScrollBarCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);
    void    OnSelectWndName();
};


#endif //_INCLUDE_UIWNDNEWSCROLLBARCOMMON_H_

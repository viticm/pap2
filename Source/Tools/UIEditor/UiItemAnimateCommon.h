////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemAnimateCommon.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 13:22:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMANIMATECOMMON_H_
#define _INCLUDE_UIITEMANIMATECOMMON_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemAnimateCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_ANIAMTE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void			Show(int bShow, int nSubWnd = -1);
protected:
	KUiItemAnimateCommon();
	~KUiItemAnimateCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);

    int		OnChangeImage();
    char*   EquirotalStrStr(const char *pString, const char *pStrSearch);
};

#endif //_INCLUDE_UIITEMANIMATECOMMON_H_

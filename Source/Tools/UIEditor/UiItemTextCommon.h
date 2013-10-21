////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemTextCommon.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 14:14:24
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMTEXTCOMMON_H_
#define _INCLUDE_UIITEMTEXTCOMMON_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemTextCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_TEXT; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void			Show(int bShow, int nSubWnd = -1);
protected:
	KUiItemTextCommon();
	~KUiItemTextCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);
};

#endif //_INCLUDE_UIITEMTEXTCOMMON_H_

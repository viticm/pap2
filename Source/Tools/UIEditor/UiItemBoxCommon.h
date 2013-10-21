////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemBoxCommon.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-3-1 15:35:34
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMBOXCOMMON_H_
#define _INCLUDE_UIITEMBOXCOMMON_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemBoxCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_ANIAMTE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void			Show(int bShow, int nSubWnd = -1);
protected:
	KUiItemBoxCommon();
	~KUiItemBoxCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);
};

#endif //_INCLUDE_UIITEMBOXCOMMON_H_

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemSceneCommon.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-4-4 11:59:12
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMSCENECOMMON_H_
#define _INCLUDE_UIITEMSCENECOMMON_H_

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemSceneCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_ANIAMTE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void			Show(int bShow, int nSubWnd = -1);
protected:
	KUiItemSceneCommon();
	~KUiItemSceneCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);
};

#endif //_INCLUDE_UIITEMSCENECOMMON_H_

////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemTreeLeafCommon.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-5 15:27:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMTREELEAFCOMMON_H_
#define _INCLUDE_UIITEMTREELEAFCOMMON_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemTreeLeafCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_TREE_LEAFE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void			Show(int bShow, int nSubWnd = -1);
protected:
	KUiItemTreeLeafCommon();
	~KUiItemTreeLeafCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);

    int		OnChangeImage();
    char*   EquirotalStrStr(const char *pString, const char *pStrSearch);

public:
	static char ms_szLastImgPath[MAX_PATH];
};

#endif //_INCLUDE_UIITEMTREELEAFCOMMON_H_

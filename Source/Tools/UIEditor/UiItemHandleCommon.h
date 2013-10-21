////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemHandleCommon.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 9:40:50
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMHANDLECOMMON_H_
#define _INCLUDE_UIITEMHANDLECOMMON_H_

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

enum ITEM_HANDLE_STYLE
{
    ITEM_HANDLE_STYLE_BEGINE = 0,
    ITEM_HANDLE_CUSTOM = ITEM_HANDLE_STYLE_BEGINE,
    ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS,
    ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX,
    ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM,
    ITEM_HANDLE_CHAT_ITEM_HANDLE,
    ITEM_HANDLE_TREE_ITEM_HANDLE,
	ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE,
    ITEM_HANDEL_STYLE_END,
};


class KUiItemHandleCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_HANDLE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void			Show(int bShow, int nSubWnd = -1);
protected:
	KUiItemHandleCommon();
	~KUiItemHandleCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);
    void    OnUpdataComboBox(int nId);
};

#endif //_INCLUDE_UIITEMHANDLECOMMON_H_

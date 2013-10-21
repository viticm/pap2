////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemCommon.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 9:44:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMCOMMON_H_
#define _INCLUDE_UIITEMCOMMON_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////


enum ITEM_POSITION
{
    POSITION_BEGINE = 0,
    POSITION_BY_CUSTOM = POSITION_BEGINE,
    POSITION_LEFT_BOTTOM,     
    POSITION_LEFT_TOP,  
    POSITION_LEFT_CENTER,
    POSITION_TOP_LEFT,        
    POSITION_TOP_RIGHT,
    POSITION_TOP_CENTER,
    POSITION_RIGHT_TOP,       
    POSITION_RIGHT_BOTTOM,
    POSITION_RIGHT_CENTER,
    POSITION_BOTTOM_LEFT,     
    POSITION_BOTTOM_RIGHT,   
    POSITION_BOTTOM_CENTER,
    POSITION_END,
};

class KUiItemCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_NULL; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void			Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiItemCommon();
	~KUiItemCommon();
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void	OnButtonClick(int nBtnId);
	void	OnUpdateEditText(int nId);
    void    OnUpdataComboBox(int nId);

	char * EquirotalStrStr(const char *pString, const char *pStrSearch);
	int OnChangeArea();
};

#endif //_INCLUDE_UIITEMCOMMON_H_

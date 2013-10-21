////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemScene.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2007-4-4 12:46:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_UIITEMSCENE_H_
#define _INCLUDE_UIITEMSCENE_H_

////////////////////////////////////////////////////////////////////////////////

#include "UiWndWindow.h"

////////////////////////////////////////////////////////////////////////////////

class KUiItemScene : public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_ITEM_TEXT; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
	virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiItemScene();
	~KUiItemScene();
private:
	KUiWndWindow *m_pItemCommon;
	KUiWndWindow *m_pItemSceneCommon;
};

#endif //_INCLUDE_UIITEMSCENE_H_

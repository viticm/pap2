#pragma once
#include "UiWndWindowCommon.h"
#include "UiWndFrameCommon.h"
#include "UiWndShowAnimateCommon.h"
#include "UiWndFrameTabControl.h"
#include "UiWndFrameDragDrop.h"
#include "UiWndFrameScript.h"

class KUiWndFrame: public KUiWndWindow
{
public:
	static  KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_FRAME; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
	virtual void		    Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);
protected:
	KUiWndFrame();
	~KUiWndFrame();
private:
	KUiWndWindow *m_pWndWindowCommon;
	KUiWndWindow *m_pWndFrameCommon;
    KUiWndWindow *m_pWndShowAnimateOrDragDrop;   
    KUiWndWindow *m_pWndFrameTabControl;
    KUiWndWindow *m_pWndFrameScript;
};

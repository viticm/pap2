#ifndef K_UI_WND_SCENE_H
#define K_UI_WND_SCENE_H

#include "UiWndWindow.h"

class KUiWndScene : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create();
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_SCENE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);
    virtual void            FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo);

protected:
	KUiWndScene();
	~KUiWndScene();

private:
	KUiWndWindow* m_pWndWindowCommon;
	KUiWndWindow* m_pWndSceneCommon;
};

#endif // K_UI_WND_SCENE_H

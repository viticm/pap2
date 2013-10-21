#pragma once
#include "UiWndWindow.h"

class KUiWndWindowCommon;
class KUiWndImageCommon : public KUiWndWindow
{
public:
	static KUiWndWindow*	Create(KUiWndWindow* pUiWndWindowCommon);
	virtual int             Init(HWND hWnd, RECT* pRc, const char *pszName);
	virtual void			Release();
	virtual UI_WND_TYPE		Type() { return UI_WND_IMAGE; }
	virtual void			UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni);
    virtual void            Show(int bShow, int nSubWnd = -1);

protected:
	KUiWndImageCommon(KUiWndWindow* pUiWndWindowCommon);
	~KUiWndImageCommon();

	int		OnChangeImage();
	void	OnSetImageSizeToWndSize(const char* pszImage, int nFrame);
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	static	char		ms_szLastImgPath[MAX_PATH];
public:
    static int GetImageSize(LPCTSTR pszImageName, int nFrame, SIZE *pSize); 
private:
    char* EquirotalStrStr(const char *pString, const char *pStrSearch);
private:
	KUiWndWindow* m_pUiWndWindowCommon;
};

#ifndef _KMESSAGEWINDOW_H_
#define _KMESSAGEWINDOW_H_

class KMessageWindow
{
protected:
	KMessageWindow(void);
	~KMessageWindow(void);
public:
 	static HWND Show(const TCHAR szMessageText[]);
	static void Active();
	static void Close();

private:
	static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static ATOM MyRegisterClass(HINSTANCE hInstance);

private:
	static HWND ms_hWnd;	
	static TCHAR ms_szDisplayText[128];
	static int ms_nTextLength;
};

#endif //_KMESSAGEWINDOW_H_
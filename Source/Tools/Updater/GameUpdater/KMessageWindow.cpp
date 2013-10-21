#include "StdAfx.h"
#include "KGPublic.h"
#include "KMessageWindow.h"
#include "KGameupdaterText.h"

#define MSG_WND_TIMER_ID 0x01

HWND KMessageWindow::ms_hWnd = NULL;
TCHAR KMessageWindow::ms_szDisplayText[128];
int KMessageWindow::ms_nTextLength = 0;

KMessageWindow::KMessageWindow(void)
{
}

KMessageWindow::~KMessageWindow(void)
{	
}

HWND KMessageWindow::Show(const TCHAR szMessageText[])
{
	int Win_W = GetSystemMetrics(SM_CXSCREEN);
	int Win_H = GetSystemMetrics(SM_CYSCREEN);	
	int x = (Win_W - 300) /2;
	int y = (Win_H - 80) /2;
	KGameupdaterText Text;

	KG_PROCESS_ERROR(szMessageText);

	ms_nTextLength = (int)_tcslen(szMessageText);
	_tcsncpy(ms_szDisplayText, szMessageText, sizeof(ms_szDisplayText) / sizeof(ms_szDisplayText[0]));
	ms_szDisplayText[sizeof(ms_szDisplayText) / sizeof(ms_szDisplayText[0]) - 1] = _T('\0');

	if (!ms_hWnd)
	{
		MyRegisterClass(NULL);
		ms_hWnd = CreateWindowEx(
			WS_EX_TOPMOST,
			_T("Message"), 
			Text.GetText(IDS_WARNNING_DIALOG_TITLE).c_str(), 
			WS_POPUP | WS_VISIBLE | WS_BORDER,
			x, y, 
			300, 80, 
			NULL, NULL, 
			NULL, 
			NULL);
		KG_PROCESS_ERROR(ms_hWnd);
	}

	::SetTimer(ms_hWnd, MSG_WND_TIMER_ID, 500, NULL);

	ShowWindow(ms_hWnd, true);
	UpdateWindow(ms_hWnd);

Exit0:	
	return ms_hWnd;
}

void KMessageWindow::Active()
{
	MSG msg;
	while (PeekMessage(&msg, ms_hWnd, 0, 0, true))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); 			
	}
}

void KMessageWindow::Close()
{
	if (ms_hWnd)
	{
		KillTimer(ms_hWnd, MSG_WND_TIMER_ID);
		ShowWindow(ms_hWnd, false);
		ms_szDisplayText[0] = _T('\0');
		ms_nTextLength = 0;
	}
}

LRESULT CALLBACK KMessageWindow::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int x = 0;
	int y = 0;
	RECT rc;
	static int nTimerCount = 1;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rc);
		x = rc.right / 2;
		y = (rc.bottom - 14) / 2;

		SetTextAlign(hdc, VTA_CENTER);
		SetBkMode(hdc, TRANSPARENT); 
		TextOut(hdc, x, y, ms_szDisplayText, _tcslen(ms_szDisplayText));

		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		{
			int nModRet = nTimerCount % 6;
			if (nModRet > 0)
			{
				_tcscat(ms_szDisplayText, _T("."));
			}
			else
			{
				ms_szDisplayText[ms_nTextLength] = _T('\0');
				nTimerCount = 0;
			}
			++nTimerCount;
			InvalidateRect(hWnd, NULL, true);
		}		
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

ATOM KMessageWindow::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	memset(&wcex, 0 ,sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= &KMessageWindow::WndProcStatic;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hCursor		= LoadCursor(NULL, IDC_WAIT);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszClassName	= _T("Message");

	return RegisterClassEx(&wcex);
}
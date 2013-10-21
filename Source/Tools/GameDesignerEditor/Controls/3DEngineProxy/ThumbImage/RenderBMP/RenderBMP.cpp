// RenderBMP.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RenderBMP.h"
#include "KGImage.h"
#include "Windows.h"

#define MAX_LOADSTRING 100
#define WM_USER_RERENDERBMP		WM_USER + 107

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#define WINDOW_SIZE		300
#define BUTTON_SIZE		21

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//HDC hdc;
HDC hdcmem;
HBITMAP hbm[20];
BITMAP bm[20];

HBITMAP hLock1;
HBITMAP hLock2;
HBITMAP hClose;
BITMAP bmLock1;
BITMAP bmLock2;
BITMAP bmClose;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HANDLE hMutex = CreateMutex(NULL, FALSE, "_MUTEX_RENDER_BMP_");
	DWORD dwError = GetLastError(); 
	if (dwError == ERROR_ALREADY_EXISTS)
	{
		return 0;
	}

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RENDERBMP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		ReleaseMutex(hMutex);
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RENDERBMP));
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ReleaseMutex(hMutex);
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RENDERBMP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_MENU);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

	int cx = GetSystemMetrics(SM_CXSCREEN);   
	int cy = GetSystemMetrics(SM_CYSCREEN);

   POINT pt;
   GetCursorPos(&pt);	

   hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, szWindowClass, szTitle, WS_VISIBLE | WS_POPUP,
	   pt.x, pt.y, WINDOW_SIZE, WINDOW_SIZE, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   	char szFileName[MAX_PATH];
	for (int i = 0; i < 20; i++)
	{
		sprintf(szFileName, "c:\\Animation%03d.bmp", i + 1);
		hbm[i] = (HBITMAP)LoadImage(hInstance, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		GetObject(hbm[i], sizeof(BITMAP), (LPVOID)&bm[i]);
	}

	hLock1 = (HBITMAP)LoadImage(hInstance, "钉子1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hLock2 = (HBITMAP)LoadImage(hInstance, "钉子2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hClose = (HBITMAP)LoadImage(hInstance, "关闭按钮.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	GetObject(hLock1, sizeof(BITMAP), (LPVOID)&bmLock1);
	GetObject(hLock2, sizeof(BITMAP), (LPVOID)&bmLock2);
	GetObject(hClose, sizeof(BITMAP), (LPVOID)&bmClose);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static bool bLock = FALSE;
	static int i = 0;
	static RECT LockRect;
	static POINT LockPoint;
	static bool bFollowMouse = FALSE;

	static bool bOverLock = false;
	static bool bOverClose = false;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		if (bOverClose)
		{
			PostQuitMessage(0);
		}
		if (bOverLock)
		{
			bLock = !bLock;
		}
		if (!bOverClose && !bOverLock)
		{
			GetWindowRect(hWnd, &LockRect);
			GetCursorPos(&LockPoint);
			bFollowMouse = true;
		}
		break;
	case WM_LBUTTONUP:
		if (bFollowMouse)
		{
			bFollowMouse = false;
		}
		break;
	case WM_MOUSEMOVE:
		if (bFollowMouse)
		{
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam); 

			xPos = LockRect.left + (xPos + LockRect.left - LockPoint.x);
			yPos = LockRect.top + (yPos + LockRect.top - LockPoint.y);
			
			MoveWindow(hWnd, xPos, yPos, WINDOW_SIZE, WINDOW_SIZE, true);
			GetWindowRect(hWnd, &LockRect);
			GetCursorPos(&LockPoint);
		}
		else
		{
			int xPos = GET_X_LPARAM(lParam); 
			int yPos = GET_Y_LPARAM(lParam);

			if (xPos > WINDOW_SIZE - BUTTON_SIZE - 3 && xPos < WINDOW_SIZE - 3 && yPos > 2 && yPos < BUTTON_SIZE + 2)
			{
				bOverClose = true;
			}
			else 
			{
				bOverClose  =false;
			}

			if (xPos > WINDOW_SIZE - BUTTON_SIZE * 2 && xPos < WINDOW_SIZE - BUTTON_SIZE  - 3 && yPos > 2 && yPos < BUTTON_SIZE + 2)
			{
				bOverLock = true;
			}
			else
			{
				bOverLock = false;
			}
		}
		break;
	case WM_KEYUP:
		if (wParam == 192)//192: scan code of '`'
		{
			bLock = !bLock;
		}
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		break;
	case WM_CREATE:
		hdc = GetDC(hWnd);
		hdcmem = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);
		SetWindowText(hWnd, "__RENDER_BMP__");
		SetTimer(hWnd, 1, 100, NULL);
		break;
	case WM_TIMER:
		if (!bLock)
		{
			POINT CursorPos;
			GetCursorPos(&CursorPos);
			RECT Rect;
			GetWindowRect(hWnd, &Rect);

			if (CursorPos.x < Rect.left - 10 ||
				CursorPos.x > Rect.right + 10 ||
				CursorPos.y < Rect.top - 10 ||
				CursorPos.y > Rect.bottom + 10)
				PostQuitMessage(0);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:

		if (i == 20)
		{
			i = 0;
		}


		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		i++;
		SelectObject(hdcmem, hbm[i]);
		BitBlt(hdc, 0, 0, bm[i].bmWidth, bm[i].bmHeight, hdcmem, 0, 0, SRCCOPY);

		{
			HBRUSH	hBlueBrush	= CreateSolidBrush(RGB(188,233,251));
			HPEN	hBluePen	= CreatePen(PS_SOLID, 1, RGB(188,233,251));

			HGDIOBJ oldBrush = SelectObject(hdc, hBlueBrush);
			HGDIOBJ oldPen	 = SelectObject(hdc, hBluePen);

			Rectangle(hdc, WINDOW_SIZE - 48, 0, WINDOW_SIZE, 24);

			if (bOverClose)
			{
				SelectObject(hdcmem, hClose);
				BitBlt(hdc, WINDOW_SIZE - BUTTON_SIZE - 2, 2, bmClose.bmWidth, bmClose.bmHeight, hdcmem, 0, 0, SRCCOPY);
			}
			else
			{
				SelectObject(hdcmem, hClose);
				BitBlt(hdc, WINDOW_SIZE - BUTTON_SIZE - 3, 1, bmClose.bmWidth, bmClose.bmHeight, hdcmem, 0, 0, SRCCOPY);
			}
		}
		if (bLock)
		{

			if (bOverLock)
			{
				SelectObject(hdcmem, hLock1);
				BitBlt(hdc, WINDOW_SIZE - BUTTON_SIZE * 2 - 2, 2, bmLock1.bmWidth, bmLock1.bmHeight, hdcmem, 0, 0, SRCCOPY);
			}
			else
			{
				SelectObject(hdcmem, hLock1);
				BitBlt(hdc, WINDOW_SIZE - BUTTON_SIZE * 2 - 3, 1, bmLock1.bmWidth, bmLock1.bmHeight, hdcmem, 0, 0, SRCCOPY);
			}
		}
		else
		{
			if (bOverLock)
			{
				SelectObject(hdcmem, hLock2);
				BitBlt(hdc, WINDOW_SIZE - BUTTON_SIZE * 2 - 2, 2, bmLock2.bmWidth, bmLock2.bmHeight, hdcmem, 0, 0, SRCCOPY);
			}
			else
			{
				SelectObject(hdcmem, hLock2);
				BitBlt(hdc, WINDOW_SIZE - BUTTON_SIZE * 2 - 3, 1, bmLock2.bmWidth, bmLock2.bmHeight, hdcmem, 0, 0, SRCCOPY);
			}
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_USER_RERENDERBMP:
		char szFileName[MAX_PATH];
		for (int n = 0; n < 20; n++)
		{
			sprintf(szFileName, "c:\\Animation%03d.bmp", n + 1);
			hbm[n] = (HBITMAP)LoadImage(hInst, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			GetObject(hbm[n], sizeof(BITMAP), (LPVOID)&bm[n]);
		}

		i = 0;

		if (!bLock)
		{
			POINT pt;
			GetCursorPos(&pt);	
			MoveWindow(hWnd, pt.x, pt.y, WINDOW_SIZE, WINDOW_SIZE, TRUE);
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


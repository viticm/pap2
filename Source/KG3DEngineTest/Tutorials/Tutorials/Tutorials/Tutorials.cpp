////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : Tutorials.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tutorials.h"
#include "KG3DInterface.h"
#include "direct.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

HWND g_hWnd;

IKG3DEngineManager* g_pEngineManager = NULL;
IKG3DModel*         g_pModel = NULL;

CONST char* TEST_MESH = "\\data\\public\\scenerunner\\scenerunner.Mesh";
CONST char* TEST_ANI  = "\\data\\public\\scenerunner\\scenerunner.Ani";

HRESULT InitEngine()
{
    typedef HRESULT (*pfnGet3DEngineInterface)(void** pEngineManager);
    pfnGet3DEngineInterface Get3DEngineInterface = NULL;

    char szMeshPath[MAX_PATH];
    char szAniPath[MAX_PATH];
    char szCurrPath[MAX_PATH];

    _getcwd(szCurrPath, sizeof(szCurrPath));
    sprintf(szMeshPath, "%s%s", szCurrPath, TEST_MESH);
    sprintf(szAniPath,  "%s%s", szCurrPath, TEST_ANI);

    HMODULE hEngine = LoadLibrary("KG3DEngineD.dll");

    Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(
        hEngine, "Get3DEngineInterface"
        );
    
    if (!Get3DEngineInterface)
        return E_FAIL;

    if (FAILED(Get3DEngineInterface((void**)&g_pEngineManager)))
        return E_FAIL;
    

    if (FAILED(g_pEngineManager->Init(0, 0x0004, g_hWnd, g_hWnd)))
        return E_FAIL;

    IKG3DScene* pScene = NULL;
    if (FAILED(g_pEngineManager->NewOneScene(0, &pScene)))
        return E_FAIL;

    int nWndId;

    if (FAILED(pScene->AddOutputWindow("Turorals 1", g_hWnd, 2, 0, &nWndId)))
        return E_FAIL;

    pScene->GetCurrentCamera()->SetPosition(D3DXVECTOR3(0.f, 100.f, -500.f));


    if (SUCCEEDED(g_pEngineManager->Get3DModelTable()->NewOneResourse(0, (IKG3DResourceBase**)&g_pModel)))
    {
        if (SUCCEEDED(g_pModel->LoadFromFile(szMeshPath, 0, 0)))
        {
            g_pModel->PlayAnimation(0, szAniPath, 1.f, 0.f, 0);
            g_pModel->SetTranslation(&D3DXVECTOR3(0,0,10));
            g_pModel->FrameMove();
            pScene->AddRenderEntity(g_pModel, 0, 0);
        }
    }

    return S_OK;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TUTORIALS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TUTORIALS));


    InitEngine();



	// Main message loop:
	while (1)
	{
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
        }
        
        if (msg.message == WM_QUIT)
            break;

        g_pEngineManager->FrameMove();
        g_pEngineManager->Render();
	}

    g_pEngineManager->UnInit();
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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TUTORIALS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TUTORIALS);
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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

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

	switch (message)
	{
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
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
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

// MergeTga.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "MergeTga.h"
#include "KWndNameList.h"
#include "KTgaBlockLoader.h"
#include "KTgaManager.h"
#include "KPickBox.h"
#include "KShow.h"
#include "KAnimateManager.h"
#include "KSaveManager.h"
#include "KWndAnimateList.h"
#include "KWndFrameList.h"
#include "KIconManager.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szWindowText[MAX_PATH];
unsigned long g_ulWindwoTextHash = 0;

int g_nSelReplacedBlockFlag = false;
int g_nReplaceBlockIndex = -1;
HWND g_hWnd = NULL;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    SetWidthAndHeight(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//user function
int                 InitUserData(HWND hWnd);
int                 UnInitUserData();
int                 OnNew(HWND hWnd);
int                 OnOpen(HWND hWnd);
int                 OnSave(HWND hWnd);
int                 OnSaveAs(HWND hWnd);
int                 OnImportTga(HWND hWnd);
int                 OnImportUITex(HWND hWnd);
int					OnImportText(HWND hWnd);
int                 OnAutoMakeup(HWND hWnd);
int                 OnUnMakeAll(HWND hWnd);
int                 OnExportAllTga(HWND hWnd);
int                 OnExportText(HWND hWnd);
int					OnExportNewText(HWND hWnd);
int                 OnClearTheMakeup(HWND hWnd);
int					OnBatchMakeUp(HWND hWnd);
int					OnMakeTestArea(HWND hWnd);
int					OnMakeWinAlpha(HWND hWnd);

int                 OnSetDesiredWidthAndHeight(HWND hWnd);

int                 OnLButtonDown(HWND hWnd);
int                 OnRButtonDown(HWND hWnd);
int                 OnCtrlRButtonDown(HWND hWnd);

int                 ExportTga(HWND hWnd, int nTgaBlockIndex);
int					ExportCur(HWND hWnd, int nTgaBlockIndex);
int                 ReplaceTgaBlock(HWND hWnd, int nTgaBlockIndex);
int                 OnReplaceTgaBlock(HWND hWnd);

int                 SetScrollPos(HWND hWnd);
int                 OnHScrollBar(HWND hWnd, WPARAM wParam);
int                 OnVScrollBar(HWND hWnd, WPARAM wParam);

int					PaintWindowEx(HWND hWnd);

int					UpdateWindowText(HWND hWnd);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

    //加载gdiplus
    Gdiplus::GdiplusStartupInput gdiplusStartupInput; 
	ULONG_PTR           gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    //加载commoncontrol
    InitCommonControls(); 

    //加载各项管理器
    //GetCurrentDirectory(sizeof(szLastOpenOrSaveFileDirectory), szLastOpenOrSaveFileDirectory);


	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MERGETGA, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MERGETGA);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		PaintWindowEx(g_hWnd);
		Sleep(10);
	}

	UnInitUserData();

    //卸载gdiplus
    Gdiplus::GdiplusShutdown(gdiplusToken);

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
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MERGETGA);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_MERGETGA;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   g_hWnd = hWnd;


   if (!InitUserData(hWnd))
   {
	   return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
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
	//PAINTSTRUCT ps;
	//HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
        case IDM_NEW:
            OnNew(hWnd);
            break;
        case IDM_OPEN:
            OnOpen(hWnd);
            break;
        case IDM_SAVE:
            OnSave(hWnd);
            break;
        case IDM_SAVE_AS:
            OnSaveAs(hWnd);
            break;
        case IDM_IMPORT_TGA:
            OnImportTga(hWnd);
            break;
        case IDM_IMPORT_UITEX:
            OnImportUITex(hWnd);
            break;
		case IDM_IMPORT_TEXT:
			OnImportText(hWnd);
			break;
        case IDM_EXPORT_ALL_TGA:
            OnExportAllTga(hWnd);
            break;
        case IDM_EXPORT_TEXT:
            OnExportText(hWnd);
            break;
		case IDM_EXPORT_NEW_TEXT:
			OnExportNewText(hWnd);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
        case IDM_UNMAKE_ALL:
            OnUnMakeAll(hWnd);
            break;
        case IDM_CLEAR_THE_MAKE_UP:
            OnClearTheMakeup(hWnd);
            break;
        case IDM_AUTO_MAKE_UP:
            OnAutoMakeup(hWnd);
            break;
		case ID_BATCH:
			OnBatchMakeUp(hWnd);
			break;
		case IDM_ATEA_TEST:
			OnMakeTestArea(hWnd);
			break;
		case IDM_WIN_ALPHA:
			OnMakeWinAlpha(hWnd);
			break;
        case IDM_SET_WIDTH_AND_HEIGHT:
            DialogBox(hInst, (LPCTSTR)IDD_SET_WIDTH_AND_HEIGHT, hWnd, (DLGPROC)SetWidthAndHeight);
            break;
        case IDM_SHOW_NAME_LIST: 
            OnShowOrHideListNameWindow(hWnd);
            break;
        case IDM_SHOW_ANIMATE_LIST:
            OnShowOrHideAnimateListWindow(hWnd);
            break;
		case IDM_SHOW_FRAME_LIST:
			OnShowOrHideFrameListWindow(hWnd);
			break;
        case IDM_SAVE_HIDE_CODE_WHEN_SAVE:
            OnSaveOrUnSaveHideCode(hWnd);
            break;
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
    case WM_SETFOCUS:
    case WM_ERASEBKGND :
    
		//hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		//EndPaint(hWnd, &ps);
//        KShow::GetSelf().SetNeedReDraw();
//		PaintWindow(hWnd);
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
    case WM_MOUSEMOVE:
        KPickBox::GetSelf().CalculatePickBlockPos(hWnd);
//        PaintWindow(hWnd);
        break;
    case WM_LBUTTONDOWN:
        if (g_nSelReplacedBlockFlag)
            OnReplaceTgaBlock(hWnd);
        else
            OnLButtonDown(hWnd);
        break;
    case WM_RBUTTONDOWN:
        if (::GetKeyState(VK_CONTROL) & 0x8000)
            OnCtrlRButtonDown(hWnd);
        else
            OnRButtonDown(hWnd);
        break;
    case WM_HSCROLL:
        OnHScrollBar(hWnd, wParam);
        break;
    case WM_VSCROLL:
        OnVScrollBar(hWnd, wParam);
        break;
    case WM_MOUSEWHEEL:
        if ((int)((short)HIWORD(wParam)) < 0)
            OnVScrollBar(hWnd, MAKEWPARAM(SB_LINEDOWN, 0));
        else
            OnVScrollBar(hWnd, MAKEWPARAM(SB_LINEUP, 0));  
//		PaintWindow(hWnd);
        break;
    case WM_CREATE:
        break;
    case WM_SIZE:
        SetScrollPos(hWnd);
//		PaintWindow(hWnd);
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
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK SetWidthAndHeight(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
        {
            TCHAR szOriginal[16];
            HWND hwndCombo = ::GetDlgItem(hDlg, IDC_COMB_WIDTH);
            int nDesiredWidth = KShow::GetSelf().GetDesiredWidth();
            ::SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0); 
            for (int i = 0; i < 8; ++i)
            {
                _sntprintf(szOriginal, sizeof(szOriginal) / sizeof(TCHAR), _T("%d"), 8<<i);                
                ::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)szOriginal); 
                if (nDesiredWidth == (8 <<i))
                    ::SendMessage(hwndCombo, CB_SETCURSEL, i, 0); 
            }

            hwndCombo = ::GetDlgItem(hDlg, IDC_COMB_HEIGHT);
            int nDesiredHeight = KShow::GetSelf().GetDesiredHeight();
            ::SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0); 
            for (int i = 0; i < 8; ++i)
            {
                _sntprintf(szOriginal, sizeof(szOriginal) / sizeof(TCHAR), _T("%d"), 8<<i);                
                ::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)szOriginal); 
                if (nDesiredHeight == (8 << i))
                    ::SendMessage(hwndCombo, CB_SETCURSEL, i, 0); 
            }
        }
		return TRUE;

	case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            if (!OnSetDesiredWidthAndHeight(hDlg))
                break;
        case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
            break;
        }
		break;
	}
	return FALSE;
}

int InitUserData(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(hInst);

	szWindowText[0] = _T('\0');

	nRetCode = KShow::Init(hWnd);
	KG_PROCESS_ERROR(nRetCode);

    nRetCode = KWndNameList::Init(hInst, hWnd);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KWndAnimateList::Init(hInst, hWnd);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = OnShowOrHideAnimateListWindow(hWnd);
    KG_PROCESS_ERROR(nRetCode);

	nRetCode = KWndFrameList::Init(hInst, hWnd);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = OnShowOrHideFrameListWindow(hWnd);
	KG_PROCESS_ERROR(nRetCode);

    nRetCode = KTgaBlockLoader::Init();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KTgaManager::Init();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KAnimateManager::Init();
    KG_PROCESS_ERROR(nRetCode);

	nRetCode = KIconManager::Init();
	KG_PROCESS_ERROR(nRetCode);

    nRetCode = KPickBox::Init();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KSaveManager::Init();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = OnNew(hWnd);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = OnSaveOrUnSaveHideCode(hWnd);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        UnInitUserData();
    return nResult;
}

int UnInitUserData()
{
    KSaveManager::Exit();
    KPickBox::Exit();
    KAnimateManager::Exit();
    KTgaManager::Exit();
    KTgaBlockLoader::Exit();
	KIconManager::Exit();
    KWndNameList::Exit();
    KWndAnimateList::Exit();
	KShow::Exit();
    return true;
}

int OnNew(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;

	_tcsncpy(szWindowText, _T("新建UITex"), MAX_PATH);

    KSaveManager::GetSelf().Clear();
    KAnimateManager::GetSelf().Clear();
    KPickBox::GetSelf().Clear();
    KShow::GetSelf().Clear();
    nRetCode = KTgaManager::GetSelf().Clear();
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int OnOpen(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    LPTSTR pszClip = NULL;
    LPTSTR pszTemp = NULL;
	OPENFILENAME	ofn;
	TCHAR	szFile[MAX_PATH];
    szFile[0] = _T('\0');

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = _T("请选择UITex文件");
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("UITex文件\0*.UITex\0\0");
    if (KSaveManager::GetSelf().IsLastOpenDirExist())
        ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir();
    nRetCode = GetOpenFileName(&ofn);
    KG_PROCESS_ERROR(nRetCode);

    OnNew(hWnd);

	pszClip = _tcsrchr(szFile, _T('\\'));
    if (!pszClip)
        pszClip = szFile;
    pszTemp = _tcsrchr(pszClip, _T('/')); 
    if (pszTemp)
        pszClip = pszTemp;
	*pszClip = _T('\0');
    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
	*pszClip = _T('\\');

	nRetCode = KTgaBlockLoader::GetSelf().LoadUITex(szFile, LOAD_AS_REPLACE, READ_TEXT, NOT_FIND_ICON_FILE, 0, 0, hWnd);
    if (!nRetCode)
    {
        nRetCode = KTgaBlockLoader::GetSelf().LoadUITex(szFile, LOAD_AS_REPLACE, NOT_READ_TEXT, NOT_FIND_ICON_FILE, 0, 0, hWnd);
        ::MessageBox(hWnd, _T("Txt文件不符合，加载完毕后请按保存，之后在重新操作!"), _T("错误:"), MB_OK | MB_ICONINFORMATION);
        KG_PROCESS_ERROR(nRetCode);
    }    

    KTgaManager::GetSelf().UpDataShow();

	_tcsncpy(szWindowText, szFile, MAX_PATH);

    nResult = true;
Exit0:
    return nResult;
}

int OnImportUITex(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    LPTSTR pszClip = NULL;
    LPTSTR pszTemp = NULL; 
	OPENFILENAME	ofn;
	TCHAR	szFile[MAX_PATH];
    szFile[0] = _T('\0');

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = _T("请选择UITex文件");
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("UITex文件\0*.UITex\0\0");
    if (KSaveManager::GetSelf().IsLastOpenDirExist())
        ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir();
    nRetCode = GetOpenFileName(&ofn);
    KG_PROCESS_ERROR(nRetCode);

	pszClip = _tcsrchr(szFile, _T('\\'));
    if (!pszClip)
        pszClip = szFile;
    pszTemp = _tcsrchr(pszClip, _T('/'));
    if (pszTemp)
        pszClip = pszTemp;

	*pszClip = _T('\0');
    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
	*pszClip = _T('\\');

    nRetCode = KTgaBlockLoader::GetSelf().LoadUITex(
        szFile, LOAD_AS_APPEND, NOT_READ_TEXT, NOT_FIND_ICON_FILE, 0, 0, hWnd
        );
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int OnImportText(HWND hWnd)
{
	int nResult = false;
	int nRetCode = false;
    int nReadOk = false;
    int nCount = 0;
    int i = 0;
	LPTSTR pszClip = NULL;
	LPTSTR pszTemp = NULL;
    LPCTSTR pcszCurrent = NULL;
	OPENFILENAME	ofn;
	BROWSEINFO bi;
	ITEMIDLIST *pItemList = NULL;
    TCHAR   szIcon[MAX_PATH];
	TCHAR	szFile[MAX_PATH];
    TCHAR   szSelf[MAX_PATH];
    TCHAR   szAllPath[256][MAX_PATH];
    FILE    *fpSelf = NULL;
    szIcon[0] = _T('\0');
    szFile[0] = _T('\0');
    szSelf[0] = _T('\0');

    if (KSaveManager::GetSelf().IsLastCurrentPath())
    {
        pcszCurrent = KSaveManager::GetSelf().GetLastCurrentPath();
        _tcsncpy(szSelf, pcszCurrent, sizeof(szSelf) / sizeof(szSelf[0]));
    }
    else
    {
        GetCurrentDirectory(sizeof(szSelf), szSelf);
        _tcsncat(szSelf, "\\path.txt", sizeof(szSelf) / sizeof(szSelf[0]));
        KSaveManager::GetSelf().SetLastCurrentPath(szSelf);
    }
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = _T("请选择txt文件");
	ofn.nMaxFile = sizeof(szFile); 
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("txt文件\0*.txt\0\0");
	if (KSaveManager::GetSelf().IsLastOpenDirExist())
		ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir();
	nRetCode = GetOpenFileName(&ofn);
	KG_PROCESS_ERROR(nRetCode);

	pszClip = _tcsrchr(szFile, _T('\\'));
	if (!pszClip)
		pszClip = szFile;
	pszTemp = _tcsrchr(pszClip, _T('/'));
	if (pszTemp)
		pszClip = pszTemp;

	*pszClip = _T('\0');
	KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
	*pszClip = _T('\\');

	nRetCode = KTgaBlockLoader::GetSelf().ReadIconText(szFile);
    _tcsncpy(szIcon, szFile, sizeof(szIcon) / sizeof(szIcon[0]));
	KG_PROCESS_ERROR(nRetCode);

    fpSelf = fopen(szSelf, "r");
    if (fpSelf == NULL)
    {
        fpSelf = fopen(szSelf, "a+");
        fclose(fpSelf);
        fpSelf = NULL;
        fpSelf = fopen(szSelf, "r");
    }
    nCount = 0;
    for (i = 0; i < 256; ++i)
        szAllPath[i][0] = _T('\0');
    while (1 == fscanf(fpSelf, "%s", szAllPath[nCount]))
        nCount++;
    fclose(fpSelf); 
    fpSelf = NULL;
   
    if (nCount)
    {
        for (i = 0; i < nCount; ++i)
        {
            if (szAllPath[i][_tcslen(szAllPath[i]) - 1] == _T('\\') || 
            szAllPath[i][_tcslen(szAllPath[i]) - 1] == _T('/')
            )
            szAllPath[i][_tcslen(szAllPath[i]) - 1] = _T('\0');

            KSaveManager::GetSelf().SetLastUITexOpenOrSaveDir(szAllPath[i]);
            KWndFrameList::GetSelf().Show();
            nRetCode = KIconManager::GetSelf().SetHwnd(hWnd);
            KG_PROCESS_ERROR(nRetCode);
            nRetCode = KTgaBlockLoader::GetSelf().ReadTextFromIcon(szAllPath[i], hWnd);
            if (nRetCode)
            {         
                nReadOk = true;
                break;
            }
            nRetCode = KTgaBlockLoader::GetSelf().ReadIconText(szIcon);
        }
		_tcsncpy(szWindowText, szAllPath[0], MAX_PATH);
    }
    if (!nReadOk)
    {
        szAllPath[0][0] = _T('\0');
        memset(&bi, 0, sizeof(BROWSEINFO));
        bi.hwndOwner = hWnd;
        bi.lpszTitle = _T("请指定存放UITex文件的目录");
        bi.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS
            |BIF_VALIDATE | BIF_RETURNFSANCESTORS;
        pItemList = SHBrowseForFolder(&bi);
        KG_PROCESS_ERROR(pItemList);
        nRetCode = SHGetPathFromIDList(pItemList, szAllPath[0]);
        KG_PROCESS_ERROR(nRetCode);

        fpSelf = fopen(szSelf, "a");
        fprintf(fpSelf, "%s\n", szAllPath[0]);
        fclose(fpSelf);
        fpSelf = NULL;

        if (szAllPath[i][_tcslen(szAllPath[i]) - 1] == _T('\\') || 
            szAllPath[i][_tcslen(szAllPath[i]) - 1] == _T('/')
            )
            szAllPath[i][_tcslen(szAllPath[i]) - 1] = _T('\0');
        KSaveManager::GetSelf().SetLastUITexOpenOrSaveDir(szAllPath[0]);
        KWndFrameList::GetSelf().Show();
        nRetCode = KIconManager::GetSelf().SetHwnd(hWnd);
        KG_PROCESS_ERROR(nRetCode);
        nRetCode = KTgaBlockLoader::GetSelf().ReadTextFromIcon(szAllPath[0], hWnd);
        KG_PROCESS_ERROR(nRetCode);
		_tcsncpy(szWindowText, szAllPath[0], MAX_PATH);
    }
	nResult = true; 
Exit0:
    if (fpSelf)
    {
        fclose(fpSelf);
        fpSelf = NULL;
    }
	return nResult;
}

int OnShowOrHideListNameWindow(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    HMENU hMenu = NULL;

    KG_PROCESS_ERROR(hWnd);
    hMenu = GetMenu(hWnd);        
    KG_PROCESS_ERROR(hMenu);

    if (GetMenuState(hMenu, IDM_SHOW_NAME_LIST, MF_BYCOMMAND) == MF_CHECKED) 
    {
        CheckMenuItem(hMenu, IDM_SHOW_NAME_LIST, MF_UNCHECKED);
        KWndNameList::GetSelf().Hide();
    }
    else
    {
        CheckMenuItem(hMenu, IDM_SHOW_NAME_LIST, MF_CHECKED);
        KWndNameList::GetSelf().Show();
    }

    nResult = true;
Exit0:
    return nResult;
    
}

int OnShowOrHideAnimateListWindow(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    HMENU hMenu = NULL;

    KG_PROCESS_ERROR(hWnd);
    hMenu = GetMenu(hWnd);        
    KG_PROCESS_ERROR(hMenu);

    if (GetMenuState(hMenu, IDM_SHOW_ANIMATE_LIST, MF_BYCOMMAND) == MF_CHECKED) 
    {
        CheckMenuItem(hMenu, IDM_SHOW_ANIMATE_LIST, MF_UNCHECKED);
        KWndAnimateList::GetSelf().Hide();
    }
    else
    {
        CheckMenuItem(hMenu, IDM_SHOW_ANIMATE_LIST, MF_CHECKED);
        KWndAnimateList::GetSelf().Show();
    }

    nResult = true;
Exit0:
    return nResult;
    
}

int OnShowOrHideFrameListWindow(HWND hWnd)
{
	int nResult = false; 
	int nRetCode = false;
	HMENU hMenu = NULL;

	KG_PROCESS_ERROR(hWnd);
	hMenu = GetMenu(hWnd);        
	KG_PROCESS_ERROR(hMenu);

	if (GetMenuState(hMenu, IDM_SHOW_FRAME_LIST, MF_BYCOMMAND) == MF_CHECKED) 
	{
		CheckMenuItem(hMenu, IDM_SHOW_FRAME_LIST, MF_UNCHECKED);
		KWndFrameList::GetSelf().Hide();
	}
	else
	{
		CheckMenuItem(hMenu, IDM_SHOW_FRAME_LIST, MF_CHECKED);
		KWndFrameList::GetSelf().Show();
	}

	nResult = true;
Exit0:
	return nResult;
}

int OnSaveOrUnSaveHideCode(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    HMENU hMenu = NULL;

    KG_PROCESS_ERROR(hWnd);
    hMenu = GetMenu(hWnd);        
    KG_PROCESS_ERROR(hMenu);

    if (GetMenuState(hMenu, IDM_SAVE_HIDE_CODE_WHEN_SAVE, MF_BYCOMMAND) == MF_CHECKED) 
    {
        CheckMenuItem(hMenu, IDM_SAVE_HIDE_CODE_WHEN_SAVE, MF_UNCHECKED);
        KSaveManager::GetSelf().ClearSaveHideCode();
    }
    else
    {
        CheckMenuItem(hMenu, IDM_SAVE_HIDE_CODE_WHEN_SAVE, MF_CHECKED);
        KSaveManager::GetSelf().SetSaveHideCode();
    }

    nResult = true;
Exit0:
    return nResult;
    
}

int OnImportTga(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    LPTSTR pszClip = NULL;
    LPTSTR pszTemp = NULL;
	OPENFILENAME	ofn;
    TCHAR szOpenFile[MAX_PATH];
	TCHAR szFile[4096];
    szFile[0] = _T('\0');

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = _T("请选择Tga文件");
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("Tga文件\0*.Tga\0\0");
    if (KSaveManager::GetSelf().IsLastOpenDirExist())
        ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir();
    nRetCode = GetOpenFileName(&ofn);
    KG_PROCESS_ERROR(nRetCode);

    if (szFile[_tcslen(szFile) + 1] == _T('\0'))
    {
        pszClip = _tcsrchr(szFile, _T('\\'));
        if (!pszClip)
            pszClip = szFile;
        pszTemp = _tcsrchr(pszClip, _T('/'));
        if (pszTemp)
            pszClip = pszTemp;
	    *pszClip = _T('\0');        
    }

    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
    pszClip = szFile + _tcslen(szFile) + 1;
    while (*pszClip != _T('\0'))
    {
        _sntprintf(
            szOpenFile, sizeof(szOpenFile) / sizeof(TCHAR), _T("%s%s%s"), 
            szFile, _T("\\"), pszClip
        );
        KTgaBlockLoader::GetSelf().LoadTga(szOpenFile);
        pszClip += (_tcslen(pszClip) + 1);
    }

    nResult = true;
Exit0:
    return nResult;
}


int PaintWindow(HWND hWnd)
{
/*
    int nResult = false;
    HDC hDc = NULL;
    Gdiplus::Graphics *pGraphics = NULL;

    KG_PROCESS_ERROR(hWnd);

    hDc = GetDC(hWnd);
    KG_PROCESS_ERROR(hDc);

    ValidateRect(hWnd, NULL);

    pGraphics = new Gdiplus::Graphics(hDc);
    KG_PROCESS_ERROR(pGraphics);
    //绘制

    KShow::GetSelf().PaintWindow(hWnd, pGraphics);
    KPickBox::GetSelf().PaintWindow(hWnd, pGraphics);

    nResult = true;
Exit0:
    KG_DELETE(pGraphics);
    return nResult;
*/
	return true;
}

int PaintWindowEx(HWND hWnd)
{
	int nResult = false;

	KG_PROCESS_ERROR(hWnd);

	KShow::GetSelf().PaintWindow(hWnd);

	UpdateWindowText(hWnd);

	nResult = true;
Exit0:
	return nResult;
}

unsigned long hashpjw(char *arKey)  
{  
	unsigned long h = 0, g;  

	while (*arKey) {  
		h = (h << 4) + *arKey++;  
		if ((g = (h & 0xF0000000))) 
		{  
			h = h ^ (g >> 24);  
			h = h ^ g;  
		}  
	}  
	return h;  
}

int	UpdateWindowText(HWND hWnd)
{
	TCHAR szText[MAX_PATH * 2];
	LPCTSTR pcszTip = KPickBox::GetSelf().GetTip();
	if (*pcszTip)
		_sntprintf(szText, _countof(szText), "%s——%s", szWindowText, pcszTip);
	else
		_tcscpy(szText, szWindowText);
	unsigned long ulHash = hashpjw(szText);
	if (g_ulWindwoTextHash != ulHash)
	{
		g_ulWindwoTextHash = ulHash;
		::SetWindowText(hWnd, szText);
	}
	
	return TRUE;
}

int OnLButtonDown(HWND hWnd)
{
    int nResult = false;
    int nPickIndex = -1;
    KTgaBlock *pBlock = NULL;

    KG_PROCESS_ERROR(hWnd);

    nPickIndex = KPickBox::GetSelf().GetPickBlockIndex();
    pBlock = KTgaManager::GetSelf().Get(nPickIndex);
    if (pBlock)
    {
        KPickBox::GetSelf().CalculatePickBlockPos(hWnd);
        KPickBox::GetSelf().Drop();
    }
    else
    {
        POINT msPos;
        ::GetCursorPos(&msPos);
        ::ScreenToClient(hWnd, &msPos);
        nPickIndex = KTgaManager::GetSelf().GetPosedBlockIndex(
            msPos.x + KShow::GetSelf().GetShowStartX(), 
            msPos.y + KShow::GetSelf().GetShowStartY()
        );
        KPickBox::GetSelf().Pick(nPickIndex);
    }

    KShow::GetSelf().SetNeedReDraw();
//    PaintWindow(hWnd);

    nResult = true;
Exit0:
    return nResult;
}

int OnRButtonDown(HWND hWnd)
{
    int nResult = false;
    int nPickIndex = -1;
    KTgaBlock *pBlock = NULL;
    KG_PROCESS_ERROR(hWnd);

    nPickIndex = KPickBox::GetSelf().GetPickBlockIndex();
    pBlock = KTgaManager::GetSelf().Get(nPickIndex);
    if (pBlock)
    {
        KPickBox::GetSelf().Clear();
    }
    else
    {
        POINT msPos;
        ::GetCursorPos(&msPos);
        ::ScreenToClient(hWnd, &msPos);
        nPickIndex = KTgaManager::GetSelf().GetPosedBlockIndex(
            msPos.x + KShow::GetSelf().GetShowStartX(), 
            msPos.y + KShow::GetSelf().GetShowStartY()
        );
        pBlock = KTgaManager::GetSelf().Get(nPickIndex);
        KG_PROCESS_ERROR(pBlock);
        pBlock->ClearPosHasBeenSeted();

        KTgaManager::GetSelf().UpDataListView();
        KTgaManager::GetSelf().UpDataShow();
    }

//    PaintWindow(hWnd);

    nResult = true;
Exit0:
    return nResult;
}

int OnCtrlRButtonDown(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    int nPickIndex = -1;
    int nId = -1;
    KTgaBlock *pBlock = NULL;
    POINT msPos;
    HMENU hMenu = NULL;
    KG_PROCESS_ERROR(hWnd);

    nPickIndex = KPickBox::GetSelf().GetPickBlockIndex();
    pBlock = KTgaManager::GetSelf().Get(nPickIndex);
    if (pBlock)
    {
        KPickBox::GetSelf().Clear();
//        PaintWindow(hWnd);
        KG_PROCESS_SUCCESS(true);
    }
    
    ::GetCursorPos(&msPos);
    ::ScreenToClient(hWnd, &msPos);
    nPickIndex = KTgaManager::GetSelf().GetPosedBlockIndex(
        msPos.x + KShow::GetSelf().GetShowStartX(), 
        msPos.y + KShow::GetSelf().GetShowStartY()
    );
    KG_PROCESS_ERROR(nPickIndex >= 0);

    hMenu = ::LoadMenu((HINSTANCE)hInst, MAKEINTRESOURCE(IDR_LIST_RCLICK_MENU));
    KG_PROCESS_ERROR(hMenu);
    ::GetCursorPos(&msPos);
    nId = ::TrackPopupMenu(
        ::GetSubMenu(hMenu, 0),
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		msPos.x, msPos.y, 0, hWnd, NULL
    );
    ::DestroyMenu(hMenu);

    switch(nId)
    {
    case IDM_LIST_RCLICK_EXPORT_TGA:
        nRetCode = ExportTga(hWnd, nPickIndex);
        KG_PROCESS_ERROR(nRetCode);
        break;
	case IDM_LIST_RCLICK_EXPORT_CUR:
		nRetCode = ExportCur(hWnd, nPickIndex);
		KG_PROCESS_ERROR(nRetCode);
		break;
    case IDM_LIST_RCLICK_REPLACE:
        nRetCode = ReplaceTgaBlock(hWnd, nPickIndex);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case IDM_LIST_RCLICK_DELETE:
        nRetCode = KTgaManager::GetSelf().Remove(nPickIndex);
        KG_PROCESS_ERROR(nRetCode);
        break;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int ReplaceTgaBlock(HWND hWnd, int nTgaBlockIndex)
{
    int nResult = false;

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(nTgaBlockIndex >= 0);

    ::SetCapture(hWnd);
    g_nSelReplacedBlockFlag = true;
    g_nReplaceBlockIndex = nTgaBlockIndex;

    nResult = true;
Exit0:
    return nResult;
}

int OnReplaceTgaBlock(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    int nPickedIndex = false;
    POINT msPos;

    if (::GetCapture() == hWnd)
        ::ReleaseCapture();

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(g_nReplaceBlockIndex >= 0);
    
    ::GetCursorPos(&msPos);
    ::ScreenToClient(hWnd, &msPos);
    nPickedIndex = KTgaManager::GetSelf().GetPosedBlockIndex(
        msPos.x + KShow::GetSelf().GetShowStartX(), 
        msPos.y + KShow::GetSelf().GetShowStartY()
    );
    KG_PROCESS_ERROR(nPickedIndex >= 0);

    KG_PROCESS_ERROR(nPickedIndex != g_nReplaceBlockIndex);
    nRetCode = KTgaManager::GetSelf().Replace(nPickedIndex, g_nReplaceBlockIndex);
    KG_PROCESS_ERROR(nRetCode);

//    PaintWindow(hWnd);

    nResult = true;
Exit0:
    g_nSelReplacedBlockFlag = false;
    g_nReplaceBlockIndex = -1;
    return nResult;
}

int ExportTga(HWND hWnd, int nTgaBlockIndex)
{
    int nResult = false;
    int nRetCode = false;
	BROWSEINFO bi;
    ITEMIDLIST *pItemList = NULL;
    TCHAR szPath[MAX_PATH];

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(nTgaBlockIndex >= 0);

	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = hWnd;
    bi.lpszTitle = _T("请指定目录(警告:会覆盖目录下的同名文件)");
	bi.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS
        |BIF_VALIDATE | BIF_RETURNFSANCESTORS;
    pItemList = SHBrowseForFolder(&bi);
    KG_PROCESS_ERROR(pItemList);
	nRetCode = SHGetPathFromIDList(pItemList, szPath);
    KG_PROCESS_ERROR(nRetCode);

    if (
        szPath[_tcslen(szPath) - 1] == _T('\\') || 
        szPath[_tcslen(szPath) - 1] == _T('/')
    )
    szPath[_tcslen(szPath) - 1] = _T('\0');

    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szPath);

    nRetCode = KTgaManager::GetSelf().ExportTga(szPath, nTgaBlockIndex);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int ExportCur(HWND hWnd, int nTgaBlockIndex)
{
	int nResult = false;
	int nRetCode = false;
	BROWSEINFO bi;
	ITEMIDLIST *pItemList = NULL;
	TCHAR szPath[MAX_PATH];

	KG_PROCESS_ERROR(hWnd);
	KG_PROCESS_ERROR(nTgaBlockIndex >= 0);

	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = hWnd;
	bi.lpszTitle = _T("请指定目录(警告:会覆盖目录下的同名文件)");
	bi.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS
		|BIF_VALIDATE | BIF_RETURNFSANCESTORS;
	pItemList = SHBrowseForFolder(&bi);
	KG_PROCESS_ERROR(pItemList);
	nRetCode = SHGetPathFromIDList(pItemList, szPath);
	KG_PROCESS_ERROR(nRetCode);

	if (
		szPath[_tcslen(szPath) - 1] == _T('\\') || 
		szPath[_tcslen(szPath) - 1] == _T('/')
		)
		szPath[_tcslen(szPath) - 1] = _T('\0');

	KSaveManager::GetSelf().SetLastOpenOrSaveDir(szPath);

	nRetCode = KTgaManager::GetSelf().ExportCur(szPath, nTgaBlockIndex);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int OnSave(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_ERROR(hWnd);

    if (KSaveManager::GetSelf().IsSaveFileNameEmpty())
    {
        nRetCode = OnSaveAs(hWnd);
        KG_PROCESS_ERROR_OR_SUCCESS(nRetCode);
    }

    nRetCode = KSaveManager::GetSelf().Save();
    if (!nRetCode)
        ::MessageBox(hWnd, _T("存盘失败!"), _T("警告："), MB_OK | MB_ICONINFORMATION);
    KG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
int OnSaveAs(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];
    TCHAR chRemp = _T('\0');
    szFile[0] = _T('\0');
    LPTSTR pszSplit = NULL; 
    LPTSTR pszTemp = NULL;
    KG_PROCESS_ERROR(hWnd);

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
    ofn.lpstrTitle = _T("请选择UITex文件:");
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("UITex文件\0*.UITex\0\0");
    if (KSaveManager::GetSelf().IsLastOpenDirExist())
        ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir(); 
	nRetCode = GetSaveFileName(&ofn);
    KG_PROCESS_ERROR(nRetCode);

    pszSplit = _tcsrchr(szFile, _T('\\'));
    if (!pszSplit)
        pszSplit = szFile;
    pszTemp = _tcsrchr(pszSplit, _T('/'));
    if (pszTemp)
        pszSplit = pszTemp;

    chRemp = *pszSplit;
    *pszSplit = _T('\0');
    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
    *pszSplit = chRemp;

	pszSplit = _tcsrchr(pszSplit, _T('.'));
    if (!pszSplit)
        _tcsncat(szFile, ".UITex", sizeof(szFile) / sizeof(TCHAR));

    nRetCode = KSaveManager::GetSelf().SetCurrentEditFile(szFile);
    KG_PROCESS_ERROR(nRetCode);

	_tcsncpy(szWindowText, szFile, MAX_PATH);

    nRetCode = OnSave(hWnd);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return true;
}

int SetScrollPos(HWND hWnd)
{
    int nResult = false;
    SCROLLINFO si;
    RECT rcClient;    
    int nCurrentScrollPosX = KShow::GetSelf().GetShowStartX();
    nCurrentScrollPosX = max(nCurrentScrollPosX, 0);
    int nCurrentScrollPosY = KShow::GetSelf().GetShowStartY();
    nCurrentScrollPosY = max(nCurrentScrollPosY, 0);

    KG_PROCESS_ERROR(hWnd);
    ::GetClientRect(hWnd, &rcClient);
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMax =  max(SHOW_BUFFER_WIDTH, rcClient.right); //SHOW_BUFFER_WIDTH;
    si.nMin = 0;
    si.nPage = rcClient.right;
    si.nPos = min(nCurrentScrollPosX, si.nMax - rcClient.right);
	//si.nPos = max(si.nPos, 0);
    ::SetScrollInfo(hWnd, SB_HORZ, &si, true); 

    KShow::GetSelf().SetShowStartX(si.nPos);

    si.nMax = max(SHOW_BUFFER_HEIGHT, rcClient.bottom);//SHOW_BUFFER_HEIGHT;//
    si.nPage = rcClient.bottom;
    si.nPos = min(nCurrentScrollPosY, si.nMax - rcClient.bottom);
	//si.nPos = max(si.nPos, 0);
    SetScrollInfo(hWnd, SB_VERT, &si, true); 

    KShow::GetSelf().SetShowStartY(si.nPos);

    KPickBox::GetSelf().CalculatePickBlockPos(hWnd);
    KShow::GetSelf().SetNeedReDraw();
//    PaintWindow(hWnd);

    nResult = true;
Exit0:
    return nResult;
}

int OnHScrollBar(HWND hWnd, WPARAM wParam)
{
    int nResult = false;
    int nCurrentScrollPosX = KShow::GetSelf().GetShowStartX();

    KG_PROCESS_ERROR(hWnd);

    switch(LOWORD(wParam))
    {
    case SB_PAGEUP: 
        nCurrentScrollPosX -= 50; 
        break; 
    case SB_PAGEDOWN: 
        nCurrentScrollPosX += 50; 
        break; 
    case SB_LINEUP: 
        nCurrentScrollPosX -= 10; 
        break; 
    case SB_LINEDOWN: 
        nCurrentScrollPosX += 10; 
        break; 
    case SB_THUMBPOSITION: 
        nCurrentScrollPosX = HIWORD(wParam); 
        break; 
    default: 
        break;
    }

    KShow::GetSelf().SetShowStartX(nCurrentScrollPosX);

    SetScrollPos(hWnd);

    nResult = true;
Exit0:
    return nResult;  

}

int OnVScrollBar(HWND hWnd, WPARAM wParam)
{
    int nResult = false;
    int nCurrentScrollPosY = KShow::GetSelf().GetShowStartY();
    KG_PROCESS_ERROR(hWnd);

    switch(LOWORD(wParam))
    {
    case SB_PAGEUP: 
        nCurrentScrollPosY -= 50; 
        break; 
    case SB_PAGEDOWN: 
        nCurrentScrollPosY += 50; 
        break; 
    case SB_LINEUP: 
        nCurrentScrollPosY -= 5; 
        break; 
    case SB_LINEDOWN: 
        nCurrentScrollPosY += 5; 
        break; 
    case SB_THUMBPOSITION: 
        nCurrentScrollPosY = HIWORD(wParam); 
        break; 
    default: 
        break;
    }

    KShow::GetSelf().SetShowStartY(nCurrentScrollPosY);
    SetScrollPos(hWnd);

    nResult = true;
Exit0:
    return nResult;  
}

int OnAutoMakeup(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_ERROR(hWnd);

    nRetCode = KTgaManager::GetSelf().AutoMakeup();
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KTgaManager::GetSelf().UpDataListView();
    KTgaManager::GetSelf().UpDataShow();
    if (!nResult)
        ::MessageBox(hWnd, _T("对不起,自动拼图不能在指定的大小内完成拼图."), _T("警告:"), MB_OK | MB_ICONINFORMATION);
    return nResult;

}

int OnSetDesiredWidthAndHeight(HWND hWnd)
{
    int nResult = false;
    LRESULT lRet = CB_ERR;
    HWND hwndCombo = NULL;
    int nWidth = 0;
    int nHeight = 0;
    KG_PROCESS_ERROR(hWnd);

    hwndCombo = ::GetDlgItem(hWnd, IDC_COMB_WIDTH);
    KG_PROCESS_ERROR(hwndCombo);
    lRet = ::SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
    KG_PROCESS_ERROR(lRet != CB_ERR);
    nWidth = 8 << ((int)lRet);

    hwndCombo = ::GetDlgItem(hWnd, IDC_COMB_HEIGHT);
    KG_PROCESS_ERROR(hwndCombo);
    lRet = ::SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
    KG_PROCESS_ERROR(lRet != CB_ERR);
    nHeight = 8 << ((int)lRet);

    if (nWidth > nHeight)
        KG_PROCESS_ERROR(nWidth <= nHeight * 2);
    else
        KG_PROCESS_ERROR(nHeight <= nWidth * 2);

    KShow::GetSelf().SetDesiredWidth(nWidth);
    KShow::GetSelf().SetDesiredHeight(nHeight);

    nResult = true;
Exit0:
    if (!nResult)
        ::MessageBox(hWnd, _T("请正确设置,长宽不能超过对方的两倍."), _T("错误:"), MB_OK | MB_ICONINFORMATION);
    return nResult;
}

int OnUnMakeAll(HWND hWnd)
{
    int nResult = false;

    KG_PROCESS_ERROR(hWnd);

    nResult = KTgaManager::GetSelf().UnMakeAll();
    KG_PROCESS_ERROR(nResult);

    nResult = true;
Exit0:
    return nResult;
}

int OnExportAllTga(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
	BROWSEINFO bi;
    ITEMIDLIST *pItemList = NULL;
    TCHAR szPath[MAX_PATH];
    KG_PROCESS_ERROR(hWnd);

	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = hWnd;
    bi.lpszTitle = _T("请指定目录(警告:会覆盖目录下的同名文件)");
	bi.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS
        |BIF_VALIDATE | BIF_RETURNFSANCESTORS;
    pItemList = SHBrowseForFolder(&bi);
    KG_PROCESS_ERROR(pItemList);
	nRetCode = SHGetPathFromIDList(pItemList, szPath);
    KG_PROCESS_ERROR(nRetCode);

    if (
        szPath[_tcslen(szPath) - 1] == _T('\\') || 
        szPath[_tcslen(szPath) - 1] == _T('/')
    )
    szPath[_tcslen(szPath) - 1] = _T('\0');

    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szPath);

    nRetCode = KTgaManager::GetSelf().ExportAllTga(szPath);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int OnExportText(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];
    TCHAR chRemp = _T('\0');
    szFile[0] = _T('\0');
    LPTSTR pszSplit = NULL; 
    LPTSTR pszTemp = NULL;
    KG_PROCESS_ERROR(hWnd);

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
    ofn.lpstrTitle = _T("请选择Text文件:");
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("Text文件\0*.txt\0\0");
    if (KSaveManager::GetSelf().IsLastOpenDirExist())
        ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir(); 
	nRetCode = GetSaveFileName(&ofn);
    KG_PROCESS_ERROR(nRetCode);

    pszSplit = _tcsrchr(szFile, _T('\\'));
    if (!pszSplit)
        pszSplit = szFile;
    pszTemp = _tcsrchr(pszSplit, _T('/'));
    if (pszTemp)
        pszSplit = pszTemp;

    chRemp = *pszSplit;
    *pszSplit = _T('\0');
    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
    *pszSplit = chRemp;

	pszSplit = _tcsrchr(pszSplit, _T('.'));
    if (!pszSplit)
        _tcsncat(szFile, ".txt", sizeof(szFile) / sizeof(TCHAR));

    nRetCode = KSaveManager::GetSelf().ExportText(szFile);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return true;
}

int OnExportNewText(HWND hWnd)
{
	int nResult = false;
	int nRetCode = false;
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];
	TCHAR chRemp = _T('\0');
	szFile[0] = _T('\0');
	LPTSTR pszSplit = NULL; 
	LPTSTR pszTemp = NULL;
	KG_PROCESS_ERROR(hWnd);

	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = _T("请选择新的Text文件:");
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("Text文件\0*.txt\0\0");
	if (KSaveManager::GetSelf().IsLastOpenDirExist())
		ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir();
	nRetCode = GetSaveFileName(&ofn);
	KG_PROCESS_ERROR(nRetCode);

	pszSplit = _tcsrchr(szFile, _T('\\'));
	if (!pszSplit)
		pszSplit = szFile;
	pszTemp = _tcsrchr(pszSplit, _T('/'));
	if (pszTemp)
		pszSplit = pszTemp;

	chRemp = *pszSplit;
	*pszSplit = _T('\0');
	KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
	*pszSplit = chRemp;

	pszSplit = _tcsrchr(pszSplit, _T('.'));
	if (!pszSplit)
		_tcsncat(szFile, ".txt", sizeof(szFile) / sizeof(TCHAR));

	nRetCode = KSaveManager::GetSelf().ExportText1(szFile);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int OnClearTheMakeup(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_ERROR(hWnd);

    nRetCode = KTgaManager::GetSelf().RemoveThePosedBlock();
    KG_PROCESS_ERROR(nRetCode);

//    PaintWindow(hWnd);

    nResult = true;
Exit0:
    return nResult;
}

int OnBatchMakeUp(HWND hWnd)
{
	int nResult = false;
	int nRetCode = false;
	BROWSEINFO bi;
	ITEMIDLIST *pItemList = NULL;
	TCHAR szPath[MAX_PATH];
	TCHAR szLastPath[MAX_PATH];
	TCHAR szLastFile[MAX_PATH];
	int nIndex = 0;

	_tcsncpy(szLastPath, KSaveManager::GetSelf().GetLastCurrentPath(), MAX_PATH);
	KSaveManager::GetSelf().GetCurrentEditFile(szLastFile, MAX_PATH);

	KG_PROCESS_ERROR(hWnd);

	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = hWnd;
	bi.lpszTitle = _T("请指定目录(警告:会覆盖目录下的同名文件)");
	bi.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS
		|BIF_VALIDATE | BIF_RETURNFSANCESTORS;
	pItemList = SHBrowseForFolder(&bi);
	KG_PROCESS_ERROR(pItemList);
	nRetCode = SHGetPathFromIDList(pItemList, szPath);
	KG_PROCESS_ERROR(nRetCode);

	if (
		szPath[_tcslen(szPath) - 1] == _T('\\') || 
		szPath[_tcslen(szPath) - 1] == _T('/')
		)
		szPath[_tcslen(szPath) - 1] = _T('\0');

	KSaveManager::GetSelf().SetLastOpenOrSaveDir(szPath);

	OnUnMakeAll(hWnd);
	while(true)
	{
		TCHAR szName[MAX_PATH];
		szName[0] = _T('\0');
		TCHAR szFile[MAX_PATH];
		LPTSTR pcszClip = NULL;
		LPTSTR pcszTemp = NULL;
		KTgaBlock *pBlock = KTgaManager::GetSelf().Get(0);
		if (!pBlock)
			break;
		pBlock->SetTop(0);
		pBlock->SetLeft(0);
		nRetCode = KTgaManager::GetSelf().TryDropBlock(0);
		KG_PROCESS_ERROR(nRetCode);

		pBlock->GetName(szName, sizeof(szName) / sizeof(TCHAR));
		pcszClip = _tcsrchr(szName, _T('\\'));
		if (!pcszClip)
			pcszClip = szName;
		pcszTemp = _tcsrchr(pcszClip, _T('/'));
		if (pcszTemp)
			pcszClip = pcszTemp;
		pcszTemp = _tcsrchr(pcszClip, _T('.'));
		if (pcszTemp)
			*pcszTemp = _T('\0');
		if(pcszClip[0] == _T('\0'))
		{
			_sntprintf(szFile, MAX_PATH, _T("%s\\%d%s"), szPath, nIndex, _T(".UITex"));
			nIndex++;
		}
		else
		{
			_sntprintf(szFile, MAX_PATH, _T("%s%s%s"), szPath, pcszClip, _T(".UITex"));
		}
		nRetCode = KSaveManager::GetSelf().SetCurrentEditFile(szFile);
		KG_PROCESS_ERROR(nRetCode);
		nRetCode = OnSave(hWnd);
		KG_PROCESS_ERROR(nRetCode);

		KTgaManager::GetSelf().RemoveThePosedBlock();

		KTgaManager::GetSelf().UpDataListView();
		KTgaManager::GetSelf().UpDataShow();

		_tcsncpy(szWindowText, szFile, MAX_PATH);
	}

	nResult = true;
Exit0:
	KSaveManager::GetSelf().SetLastOpenOrSaveDir(szLastPath);
	KSaveManager::GetSelf().SetCurrentEditFile(szLastFile);
	_tcsncpy(szWindowText, szLastFile, MAX_PATH);
	return nResult;
}

int	OnMakeTestArea(HWND hWnd)
{
	int nResult = false;
	int nRetCode = false;
	LPTSTR pszClip = NULL;
	LPTSTR pszTemp = NULL;
	OPENFILENAME	ofn;
	TCHAR szOpenFile[MAX_PATH];
	TCHAR szFile[4096];
	szFile[0] = _T('\0');

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = _T("请选择单通道BMP文件");
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("Bmp文件\0*.Bmp\0\0");
	if (KSaveManager::GetSelf().IsLastOpenDirExist())
		ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir();
	nRetCode = GetOpenFileName(&ofn);
	KG_PROCESS_ERROR(nRetCode);

	if (szFile[_tcslen(szFile) + 1] == _T('\0'))
	{
		pszClip = _tcsrchr(szFile, _T('\\'));
		if (!pszClip)
			pszClip = szFile;
		pszTemp = _tcsrchr(pszClip, _T('/'));
		if (pszTemp)
			pszClip = pszTemp;
		*pszClip = _T('\0');        
	}

	KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
	pszClip = szFile + _tcslen(szFile) + 1;
	while (*pszClip != _T('\0'))
	{
		_sntprintf(
			szOpenFile, sizeof(szOpenFile) / sizeof(TCHAR), _T("%s%s%s"), 
			szFile, _T("\\"), pszClip
			);
		{
			Gdiplus::Bitmap *pBitMap = NULL;
#ifdef _UNICODE
			pBitMap = Gdiplus::Bitmap::FromFile(szOpenFile);
#else
			WCHAR wszOpenFile[MAX_PATH];
			wszOpenFile[0] = L'\0';
			INT nChar = 0;
			nChar = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szOpenFile, -1, wszOpenFile, _countof(wszOpenFile));
			if (nChar >= 0)
			{
				wszOpenFile[nChar] = L'\0';
				pBitMap = Gdiplus::Bitmap::FromFile(wszOpenFile);
			}
#endif
			if (pBitMap && pBitMap->GetPixelFormat() == PixelFormat1bppIndexed)
			{
				UINT uW = pBitMap->GetWidth();
				UINT uH = pBitMap->GetHeight();
				UINT uSize = (uW * uH + 7) / 8;
				BYTE *pbyData = new BYTE[uSize];
				if (pbyData)
				{
					ZeroMemory(pbyData, uSize * sizeof(BYTE));
					for (UINT y = 0; y < uH; ++y)
					{
						for (UINT x = 0; x < uW; ++x)
						{
							Gdiplus::Color pixColor;
							pBitMap->GetPixel(x, y, &pixColor);

							if (pixColor.GetR() || pixColor.GetG() || pixColor.GetB())
							{
								const INT nIndex = x + y * uW;
								BYTE *pbyPos = pbyData + nIndex / 8;
								*pbyPos |= (1 << (nIndex % 8));
							}
						}
					}

					LPTSTR pszLastPoint = _tcsrchr(szOpenFile, _T('.'));
					if (pszLastPoint)
					{
						_tcscpy(pszLastPoint, _T(".area"));
						FILE *fpFile = fopen(szOpenFile ,"wb");
						if(fpFile)
						{
							fwrite(&uW, sizeof(uW), 1, fpFile);
							fwrite(&uH, sizeof(uH), 1, fpFile);
							fwrite(pbyData, uSize, 1, fpFile);
							fclose(fpFile);
							fpFile = NULL;
						}
					}

					KG_DELETE_ARRAY(pbyData);
				}
			}
			KG_DELETE(pBitMap);
		}

		pszClip += (_tcslen(pszClip) + 1);
	}

	nResult = true;
Exit0:
	return nResult;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}


int	OnMakeWinAlpha(HWND hWnd)
{
	int nResult = false;
	int nRetCode = false;
	LPTSTR pszClip = NULL;
	LPTSTR pszTemp = NULL;
	OPENFILENAME	ofn;
	TCHAR szOpenFile[MAX_PATH];
	TCHAR szFile[4096];
	szFile[0] = _T('\0');

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = _T("请选择32位Tga文件");
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("Tga文件\0*.Tga\0\0");
	if (KSaveManager::GetSelf().IsLastOpenDirExist())
		ofn.lpstrInitialDir = KSaveManager::GetSelf().GetLastOpenOrSaveDir();
	nRetCode = GetOpenFileName(&ofn);
	KG_PROCESS_ERROR(nRetCode);

	if (szFile[_tcslen(szFile) + 1] == _T('\0'))
	{
		pszClip = _tcsrchr(szFile, _T('\\'));
		if (!pszClip)
			pszClip = szFile;
		pszTemp = _tcsrchr(pszClip, _T('/'));
		if (pszTemp)
			pszClip = pszTemp;
		*pszClip = _T('\0');        
	}

	KSaveManager::GetSelf().SetLastOpenOrSaveDir(szFile);
	pszClip = szFile + _tcslen(szFile) + 1;
	while (*pszClip != _T('\0'))
	{
		_sntprintf(
			szOpenFile, sizeof(szOpenFile) / sizeof(TCHAR), _T("%s%s%s"), 
			szFile, _T("\\"), pszClip
			);
		{
			KTga32 tga;
			if(tga.CreateFromFile(szOpenFile, TRUE))
			{
				tga.ReadAllDataToSelfMemory();
				tga.Premultiplied();
				TCHAR * pszAdd = _tcsrchr(szOpenFile, _T('.'));
				if (pszAdd)
					strcpy(pszAdd, ".bmp");
				else
					strcat(szOpenFile, ".bmp");
				tga.SaveDataBufferToBmpFile(szOpenFile);
			}
		}

		pszClip += (_tcslen(pszClip) + 1);
	}

	nResult = true;
Exit0:
	return nResult;
}


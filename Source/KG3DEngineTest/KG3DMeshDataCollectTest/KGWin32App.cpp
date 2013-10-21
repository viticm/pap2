#include "stdafx.h"
#include "./KGWin32App.h"


KGWin32App* KGWin32App::ms_pSelf = NULL;


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return KGWin32App::ms_pSelf->MsgProc(hWnd, uMsg, wParam, lParam);
}

KGWin32App::KGWin32App()
{
    m_szClass[0] = '\0';
    m_szTitle[0] = '\0';

    ms_pSelf = this;
    m_nIcon = 0;
    m_hWnd = NULL;
    m_hInstance = NULL;
    m_dwHoverTime = 400;
    m_bEnabled = TRUE;
    m_bLoop = FALSE;
}

KGWin32App::~KGWin32App()
{
}

int KGWin32App::Init(HINSTANCE hInstance, LPCSTR szAppName, int nIcon)
{
    int nRetCode = false;
    int nResult = false;
    BOOL bInitClass = FALSE;
    BOOL bInitWindow = FALSE;
    BOOL bInit = FALSE;

    m_nIcon = nIcon;
    m_hInstance = hInstance;

    strcpy(m_szTitle, szAppName);
    strcpy(m_szClass, szAppName);
    strcat(m_szClass, "_Class");

    m_Events[EVENT_TIMER] = CreateWaitableTimer(NULL, FALSE, NULL);
    m_Events[EVENT_RENDER] = CreateEvent(NULL, FALSE, FALSE, NULL);

    nRetCode = InitClass();
    KGLOG_PROCESS_ERROR(nRetCode);
    bInitClass = TRUE;

    nRetCode = InitWindow();
    KGLOG_PROCESS_ERROR(nRetCode);
    bInitWindow = TRUE;

    nRetCode = OnInit();
    KGLOG_PROCESS_ERROR(nRetCode);
    bInit = TRUE;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (bInit)
        {
            OnExit();
        }

        if (bInitWindow)
        {
            nRetCode = DestroyWindow(m_hWnd);
            KGLOG_CHECK_ERROR(nRetCode);
            m_hWnd = NULL;
        }

        if (bInitClass)
        {
            ExitClass();
        }

        for (int nIndex = 0; nIndex < EVENT_TOTAL; ++nIndex)
        {
            if (m_Events[nIndex])
            {
                CloseHandle(m_Events[nIndex]);
                m_Events[nIndex] = NULL;
            }
        }

        m_hInstance = NULL;
        m_szTitle[0] = '\0';
    }
    return nResult;
}

void KGWin32App::Exit()
{
    int nRetCode = false;

    OnExit();

    if (m_hWnd)
    {
        nRetCode = DestroyWindow(m_hWnd);
        KGLOG_CHECK_ERROR(nRetCode);
        m_hWnd = NULL;
    }

    ExitClass();

    for (int nIndex = 0; nIndex < EVENT_TOTAL; ++nIndex)
    {
        if (m_Events[nIndex])
        {
            CloseHandle(m_Events[nIndex]);
            m_Events[nIndex] = NULL;
        }
    }

    m_hInstance = NULL;
    m_szTitle[0] = '\0';
}

int KGWin32App::InitClass()
{
    WNDCLASS wc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(m_hInstance, MAKEINTATOM(m_nIcon));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_szClass;

    return RegisterClassA(&wc);
}

void KGWin32App::ExitClass()
{
    if (m_szClass[0] != '\0' && m_hInstance)
    {
        UnregisterClassA(m_szClass, m_hInstance);
        m_szClass[0] = '\0';
    }
}

int KGWin32App::InitWindow()
{
    int nRetCode = false;
    int nResult = false;
    IIniFile* pFile = NULL;
    int nX = 0;
    int nY = 0;
    int nWidth  = 0;
    int nHeight = 0;
    int nFullScreen = 0;

    g_SetRootPath();
    g_SetFilePath("");

    pFile = g_OpenIniFile("config.ini");
    KGLOG_PROCESS_ERROR(pFile);
    pFile->GetInteger("Main", "X", 0, &nX);
    pFile->GetInteger("Main", "Y", 0, &nY);
    pFile->GetInteger("Main", "CanvasWidth",  1024, &nWidth);
    pFile->GetInteger("Main", "CanvasHeight", 768,  &nHeight);
    pFile->GetInteger("Main", "ExclusiveMode",   0,    &nFullScreen);

    if (nFullScreen)
    {
        nX = CW_USEDEFAULT;
        nY = CW_USEDEFAULT;
        nWidth = CW_USEDEFAULT;
        nHeight = CW_USEDEFAULT;
    }

    m_hWnd = CreateWindowExA(
        0,
        m_szClass, 
        m_szTitle,
        WS_OVERLAPPEDWINDOW,
        nX, nY, nWidth, nHeight,
        NULL, 
        NULL, 
        NULL, 
        NULL);
    KGLOG_PROCESS_ERROR(m_hWnd);

    ShowWindow(m_hWnd, TRUE);

    nRetCode = UpdateWindow(m_hWnd);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(pFile);

    if (!nResult)
    {
        if (m_hWnd)
        {
            nRetCode = DestroyWindow(m_hWnd);
            KGLOG_CHECK_ERROR(nRetCode);

            m_hWnd = NULL;
        }
    }
    return nResult;
}

void KGWin32App::SetTimerFrame(int nFrame)
{
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -10000000LL / 16;

    if (nFrame <= 0)
        nFrame = 1;
    if (nFrame > 1000)
        nFrame = 1000;

    LONG lPeriod = 1000 / nFrame;

    SetWaitableTimer(m_Events[EVENT_TIMER], &liDueTime, lPeriod, NULL, NULL, 0);
}

void KGWin32App::SetMouseHoverTime(DWORD dwHoverTime)
{
    m_dwHoverTime = dwHoverTime;
}

void KGWin32App::Enable(BOOL bEnable)
{
    if (ms_pSelf)
    {
        ms_pSelf->m_bEnabled = bEnable;
    }
}

const char * KGWin32App::GetTitle()
{
	return m_szTitle;
}

int KGWin32App::GetIcon()
{
	return m_nIcon;
}


void KGWin32App::Run()
{
    m_bLoop = TRUE;

    SetTimerFrame(1000);

    while (m_bLoop) 
    {
        DWORD dwResult = MsgWaitForMultipleObjects(EVENT_TOTAL, m_Events, FALSE, INFINITE, QS_ALLEVENTS);
        switch (dwResult - WAIT_OBJECT_0) 
        {
        case EVENT_TIMER:
            if (m_bEnabled)
                m_bLoop = OnLoop();
            break;
        case EVENT_RENDER:
            OnTest();
            break;
        case EVENT_TOTAL: 
            {
                MSG msg;
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
                {
                    if (msg.message == WM_QUIT) 
                    {
                        OnMsgProc(WM_QUIT, 0, 0);
                        m_bLoop = FALSE;
                    }
                    else
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
                break;
            }
        }
    }

    Exit();
}

int KGWin32App::OnInit()
{
    return 0;
}

void KGWin32App::OnExit()
{
}

int KGWin32App::OnLoop()
{
    return TRUE;
}

void KGWin32App::OnTest()
{
}

LRESULT	KGWin32App::OnMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

LRESULT KGWin32App::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        if (OnMsgProc(uMsg, wParam, lParam))
            return 0;
        break;
    case WM_QUIT:
        OnMsgProc(uMsg, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SETCURSOR:
        if (m_bEnabled)
            OnMsgProc(uMsg, wParam, lParam);
        return TRUE;
    case WM_IME_CHAR:
        if (m_bEnabled)
            return OnMsgProc(uMsg, wParam, lParam);
    case WM_INPUTLANGCHANGEREQUEST:
    case WM_IME_NOTIFY:
        if (m_bEnabled)
        {
            if (OnMsgProc(uMsg, wParam, lParam))
            {
                return 0;
            }
        }
        break;
    case WM_NCPAINT:
    case WM_NCCALCSIZE:
        if (m_bEnabled)
        {
            if (OnMsgProc(uMsg, wParam, lParam))
            {
                return 0;
            }
        }
        break;
    case WM_ACTIVATEAPP:
        OnMsgProc(uMsg, wParam, lParam);
        SetTimerFrame(wParam ? 1000 : 250);
        break;
    case WM_SYSCOMMAND:
        if (wParam == SC_KEYMENU)
            return 0;
    case WM_COPYDATA:
        if (m_bEnabled)
            OnMsgProc(uMsg, wParam, lParam);
        break;
    case WM_NCACTIVATE:
    case WM_NCHITTEST:
        if (m_bEnabled)
            OnMsgProc(uMsg, wParam, lParam);
        break;
	case WM_MOVE:
	case WM_MOVING:
    case WM_SIZE:
	case WM_SIZING:
    case WM_WINDOWPOSCHANGED:
    case WM_WINDOWPOSCHANGING:
        OnMsgProc(uMsg, wParam, lParam);
        break;
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
        OnMsgProc(uMsg, wParam, lParam);
        break;
    case WM_ERASEBKGND:
        if (m_bLoop)
            return 1;
        break;
    case WM_PAINT:
        {
            RECT rcClient;

            GetClientRect(hWnd, &rcClient);

            if (rcClient.right > rcClient.left && rcClient.bottom > rcClient.top)
            {
                OnTest();
            }
        }
        break;
    default:
        if (m_bEnabled)
        {
            if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
            {
                OnMsgProc(uMsg, wParam, lParam);
            }
            else if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST || uMsg == WM_MOUSEHOVER)
            {
                if (uMsg == WM_MOUSEMOVE || uMsg == WM_MOUSEHOVER)
                {
                    TRACKMOUSEEVENT tme;
                    tme.cbSize = sizeof(tme);
                    tme.hwndTrack = hWnd;
                    tme.dwFlags = TME_HOVER;
                    tme.dwHoverTime = m_dwHoverTime;
                    TrackMouseEvent(&tme);
                }
                OnMsgProc(uMsg, wParam, lParam);
            }
        }
        break;

    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND KGWin32App::GetHWND()
{
    return ms_pSelf ? ms_pSelf->m_hWnd : NULL;
}

HINSTANCE KGWin32App::GetHINSTANCE()
{
    return ms_pSelf ? ms_pSelf->m_hInstance : NULL;
}


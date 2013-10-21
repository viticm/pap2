////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_RecPlayer.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-8-11 15:47:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_RecPlayer.h"
#include "IEKG3DRecPlayer.h"
#include "IEOther.h"
#include "IEKG3DShader.h"
#include "KG_Benchmark.h"
#include "IEKG3DCollocator.h"

static const char gs_szAppName[] = "JX3 Benchmark";
const unsigned TIMER_EVENT_ID = 133;
static bool  s_bShowResult = false;


std::string& trim(std::string &s) 
{
	if (s.empty()) 
	{
		return s;
	}

	s.erase(0,s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}


////////////////////////////////////////////////////////////////////////////////
KG_RecPlayer* KG_RecPlayer::ms_pSelf = NULL;
KG_RecPlayer::KG_RecPlayer(void)
: m_dwState(STATE_NOT_INIT)
, m_hWnd(NULL)
//, m_hFont(NULL)
//, m_hBrush(NULL)
{
    m_nFullScreen = false;
    m_hEngineDLL = NULL;
    m_piEngineManager = NULL;
    m_piRecPlayer = NULL;

  
    ms_pSelf = this;

	m_hBg = NULL;
	m_hProgress = NULL;
	//m_hSprite = NULL;
	m_nBgWidth = 600;
	m_nBgHeight = 333;
	m_nProgressWidth = 338;
	m_nProgressHeight = 3;
	//m_nSpriteWidth = 28;
	//m_nSpriteHeight = 28;
	m_nProgressX = 35;
	m_nProgressY = 297;
	m_fCurrentPercentage = 0.0f;
	m_hInstance = NULL;
	//m_nRestorX = 0;
	//m_nRestorY = 0;
	m_bldFunc.BlendOp = AC_SRC_OVER;
	m_bldFunc.BlendFlags = 0;
	m_bldFunc.SourceConstantAlpha = 255;
	m_bldFunc.AlphaFormat = AC_SRC_ALPHA;


	//m_dwMinFPS = m_dwMaxFPS = m_dwAverageFPS = 999;
	//m_strDisplayCrad = "Display Crad";
	//m_strCPU = "CPU";
	//m_bUseGpu = false;
	m_nFontSize = 25;
	m_nFontFPSSize = 50;
	m_nFontPosX = 0;
	m_nFontPosY = 0;
}

KG_RecPlayer::~KG_RecPlayer(void)
{
}

////////////////////////////////////////////////////////////////////////////////
int KG_RecPlayer::Init(unsigned uX,  unsigned uY,  unsigned uWidth,  unsigned uHeight, int nFullScreen)
{
    int nResult  = false;
    int nRetCode = false;
    int nWorkThreadCreateFlag = false;
    int nInitEngineFlag = false;
	HINSTANCE hInstance;

    if (nFullScreen)
    {
        uX = 0;
        uY = 0;
    }
    m_WndInitRect.left = uX;
    m_WndInitRect.top  = uY;
    m_WndInitRect.right  = m_WndInitRect.left + uWidth;
    m_WndInitRect.bottom = m_WndInitRect.top  + uHeight;
    m_nFullScreen = nFullScreen;


	hInstance = (HINSTANCE)::GetModuleHandle(NULL);
	KGLOG_PROCESS_ERROR(hInstance);
	
	nRetCode = _RegisterClass(hInstance);
	KGLOG_PROCESS_ERROR(nRetCode);
	m_hInstance = hInstance;
	m_hWnd = ::CreateWindow( 
		gs_szAppName,  gs_szAppName,  WS_CAPTION | WS_SYSMENU,
		m_WndInitRect.left, m_WndInitRect.top, 
		(m_WndInitRect.right - m_WndInitRect.left),
		(m_WndInitRect.bottom - m_WndInitRect.top),
		NULL, NULL, hInstance, NULL 
		);
	KGLOG_PROCESS_ERROR(m_hWnd);

	if (m_nFullScreen)
	{
		nRetCode = ::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPED | WS_SYSMENU);
		KGLOG_PROCESS_ERROR(nRetCode);

		_ASSERTE(NULL != m_hWnd);
		nRetCode = ::SetWindowPos(
			m_hWnd, NULL, 
			m_WndInitRect.left, m_WndInitRect.top, 
			(m_WndInitRect.right - m_WndInitRect.left),
			(m_WndInitRect.bottom - m_WndInitRect.top),
			SWP_FRAMECHANGED
			);
		KGLOG_PROCESS_ERROR(nRetCode);


		// set font
	/*	LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));
		logFont.lfCharSet = DEFAULT_CHARSET;
		logFont.lfHeight = 20;

		_ASSERTE(NULL == m_hFont);
		m_hFont = ::CreateFontIndirect(&logFont);
		KGLOG_PROCESS_ERROR(m_hFont);

		_ASSERTE(NULL == m_hBrush);
		m_hBrush = ::CreateSolidBrush(RGB(0,0,0));
		KGLOG_PROCESS_ERROR(m_hBrush);*/

	}
	LoadSource(uWidth,uHeight);
	::ShowWindow(m_hWnd, SW_SHOW);
	
    //nRetCode = m_WorkThread.Create(&_WorkThreadFunction, this);
    //KGLOG_PROCESS_ERROR(nRetCode);
    //nWorkThreadCreateFlag = true;

    //while (m_Statue == STATE_NOT_INIT)
    //{
    //    Sleep(100);
    //}
    KGLOG_PROCESS_ERROR(m_hWnd);

    nRetCode = _InitEngine();
    KGLOG_PROCESS_ERROR(nRetCode);
    nInitEngineFlag = true;

	m_dwState = STATE_READY;
	KGLOG_PROCESS_ERROR(m_dwState == STATE_READY);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (nInitEngineFlag)
        {
            _UnInitEngine();
            nInitEngineFlag = false;
        }
        m_dwState = STATE_NOT_INIT;
        if (nWorkThreadCreateFlag)
        {
            m_WorkThread.Destroy();
            nWorkThreadCreateFlag = false;
        }
    }
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RecPlayer::UnInit()
{
    m_dwState = STATE_WAIT_FOR_EXIT;

    _UnInitEngine();

    m_dwState = STATE_NOT_INIT;
    //m_WorkThread.Destroy();

	/*if(NULL != m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	if (NULL != m_hBrush)
	{
		::DeleteObject(m_hBrush);
		m_hBrush = NULL;
	}

	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}*/
	UnloadSource();
    return true;
}
////////////////////////////////////////////////////////////////////////////////
int KG_RecPlayer::Play(
    const char cszRecFileName[], IKG_RecPlayerNotify *piNotify
)
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hRetCode = E_FAIL;
    int nRePlayFlag = false;
    int nPlayerBeginFlag = false;
    char szTitle[MAX_PATH];
	s_bShowResult = false;
	//int nSetTimerFlag = false;
	MSG msg;

	KGLOG_COM_PROCESS_ERROR(GetNewIERecPlayer());
    KGLOG_PROCESS_ERROR(cszRecFileName);
    KGLOG_PROCESS_ERROR(piNotify);

    KGLOG_PROCESS_ERROR(m_hWnd);
	//_ASSERTE(STATE_READY == m_dwState);
	if (m_dwState != STATE_NOT_INIT)
		m_dwState = STATE_READY;
    KGLOG_PROCESS_ERROR(STATE_READY == m_dwState);

    snprintf(szTitle, sizeof(szTitle) - 1, "%s (%s)", gs_szAppName, cszRecFileName);
    szTitle[sizeof(szTitle) - 1] = '\0';
    ::SetWindowText(m_hWnd, szTitle);

    nRetCode = piNotify->OnPlayBegin(m_piEngineManager, cszRecFileName);
    //KGLOG_PROCESS_ERROR(nRetCode);
    nPlayerBeginFlag = true;

    m_dwState = STATE_LOADING;
//	_OnPaint();
	_ASSERTE(NULL != m_piRecPlayer);
    hRetCode = m_piRecPlayer->LoadRecordFileEx(cszRecFileName, this);
	if (FAILED(hRetCode))
	{
		MessageBox(NULL, _T("rec file load failed!"), _T("Error"), MB_OK);
	}
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = m_piRecPlayer->RePlay();
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    nRePlayFlag = true;

    KG_PROCESS_ERROR(m_dwState == STATE_LOADING);// maybe change by another thread
    //m_Statue = STATE_PLAYING;



	//nRetCode = (int)::SetTimer(m_hWnd, TIMER_EVENT_ID, 500, NULL);
	//KGLOG_PROCESS_ERROR(nRetCode);
	//nSetTimerFlag = true;

	//m_Statue = STATE_READY;
	while (m_dwState != STATE_EXIT && m_dwState != STATE_NOT_INIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			if (msg.message == WM_KEYDOWN && msg.wParam==VK_ESCAPE)
			{ 
				if(m_dwState < STATE_SHOW_RESULT)
					m_dwState = STATE_SHOW_RESULT;
				else if(s_bShowResult)					
					break;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		} 
		else
		{
			switch(m_dwState)
			{
			case STATE_LOADING:
			case STATE_PLAYING:
				{
					hRetCode = m_piRecPlayer->FrameMoveRePlay();
					hRetCode = m_piEngineManager->FrameMove();
                    hRetCode = m_piEngineManager->Render();

					piNotify->OnFrameMove(m_piEngineManager);
					
					if (!(m_piRecPlayer->IsLoading()))
					{
						m_dwState = STATE_PLAYING;
					}
					if (m_piRecPlayer->IsPlayFinished())
					{
						m_dwState = STATE_SHOW_RESULT;
					}
					break;
				}
			case STATE_SHOW_RESULT:
				{
					if(!s_bShowResult)
						PaintResult();
					KG_PROCESS_ERROR(g_bNormalMode);
					break;
				}
			}
			
		}
	}
    nResult = true;
Exit0:
    //if (m_Statue == STATE_PLAYING) // maybe change by another thread
    //    m_Statue = STATE_READY;

	/*if (nSetTimerFlag)
	{
		::KillTimer(NULL, TIMER_EVENT_ID);
		nSetTimerFlag = false;
	}*/
	m_dwState = STATE_EXIT;

    if (nRePlayFlag)
    {
        ::InvalidateRect(m_hWnd, NULL, TRUE);
        m_piRecPlayer->StopRePlay();
        nRePlayFlag = false;
    }
    if (nPlayerBeginFlag)
    {
        piNotify->OnPlayEnd(m_piEngineManager);
        ::SetWindowText(m_hWnd, gs_szAppName);
        nPlayerBeginFlag = false;
    }

    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG_RecPlayer::GetEngineOption(KG3DEngineOption* pEngineOption)
{
    return m_piEngineManager->GetEngineOption(pEngineOption);
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG_RecPlayer::SetEngineOption(const KG3DEngineOption* pEngineOption)
{
    return m_piEngineManager->SetEngineOption(pEngineOption);
}
////////////////////////////////////////////////////////////////////////////////
int KG_RecPlayer::SetResolution(unsigned uWidth, unsigned uHeight)
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hRetCode = E_FAIL;
    IRecPlayer* pieRecPlayer = /*dynamic_cast<IRecPlayer *>*/(m_piRecPlayer);

    KGLOG_PROCESS_ERROR(pieRecPlayer);

    nRetCode = ::MoveWindow(m_hWnd, 0, 0, uWidth, uHeight, TRUE);
    KGLOG_PROCESS_ERROR(nRetCode);

    hRetCode = pieRecPlayer->OnResizeWindow(NULL);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
void KG_RecPlayer::_WorkThreadFunction(void *pvParam)
{
    KG_RecPlayer *pThis = (KG_RecPlayer *)pvParam;

    ASSERT(pThis);

    pThis->_ThreadFunction();
}
////////////////////////////////////////////////////////////////////////////////
int KG_RecPlayer::_ThreadFunction()
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int KG_RecPlayer::_RegisterClass(HINSTANCE hInstance)
{
    int nResult = false;
    WNDCLASSEX wcex;
    ATOM ret;

    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize         = sizeof(wcex);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= _WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_MAIN));
    wcex.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName	= NULL;
    wcex.lpszClassName	= gs_szAppName;

    ret = RegisterClassEx(&wcex);
    KGLOG_PROCESS_ERROR(ret != 0);

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_RecPlayer::_InitEngine()
{
    HRESULT hr = E_FAIL;
    int nResult = false;
//    int nRetCode = true;
    HRESULT hRetCode = E_FAIL;
    typedef HRESULT (*FN_GET_3DENGINE_INTERFACE)(void**);
    FN_GET_3DENGINE_INTERFACE pfnGet3DEngineInterface = NULL;
    int nEngineManagerInitFlag = false;
    KG3DEngineOption engineOption;
    
#ifdef _DEBUG
        m_hEngineDLL = ::LoadLibraryA("KG3DEngineD.dll");
#else
        m_hEngineDLL = ::LoadLibraryA("KG3DEngine.dll");
#endif
    KGLOG_PROCESS_ERROR(m_hEngineDLL);

    pfnGet3DEngineInterface = (FN_GET_3DENGINE_INTERFACE)GetProcAddress(
        m_hEngineDLL, "Get3DEngineInterface"
    );
    KGLOG_PROCESS_ERROR(pfnGet3DEngineInterface);

    hRetCode = pfnGet3DEngineInterface((void **)&m_piEngineManager);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    ASSERT(m_piEngineManager);

    hRetCode = m_piEngineManager->Init(0, KG3DENG_CLIENT | KG3DENG_RECORD, m_hWnd, m_hWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = m_piEngineManager->GetEngineOption(&engineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    //强制打开刀光插值，CUDA
    engineOption.bEnableBladesSmoot = true;
    engineOption.bEnableCuda = true;//需要检查是否能够打开
    hRetCode = m_piEngineManager->SetEngineOption(&engineOption);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    {
        IEKG3DEngineManagerFlags flags;
        hRetCode = (static_cast<IEKG3DEngineManager*>(m_piEngineManager))->GetFlags(&flags);
        _ASSERTE(SUCCEEDED(hRetCode));

        flags.bFrameMoveFocuseSceneOnly = true;
        flags.bRenderFocuseSceneOnly = true;
        hRetCode = (static_cast<IEKG3DEngineManager*>(m_piEngineManager))->SetFlags(&flags);
        _ASSERTE(SUCCEEDED(hRetCode));
    }

    {
        IEKG3DCollocator* pCollocator = NULL;
        hr = (static_cast<IEKG3DEngineManager*>(m_piEngineManager))->GetIEInterface(0,KG3DTYPE_COLLOCATOR,0,0, (LPVOID*)&pCollocator);
        _ASSERTE(SUCCEEDED(hr));

        /*IEKG3DGlobalOptions opt = pCollocator->GetGlobalOptions();
        opt.bCreateForceFieldInModel = FALSE;
        _ASSERTE(SUCCEEDED(pCollocator->SetGlobalOptions(opt)));*/
    }

    nEngineManagerInitFlag = true;


	
	if(! g_benchMarkOption.bDisablePreloadShader)
	{
		//_ASSERTE(STATE_READY == m_dwState);
		IEKG3DEngineManager* pIEEngineManager = dynamic_cast<IEKG3DEngineManager*>(m_piEngineManager);
		_ASSERTE(NULL != pIEEngineManager);
		
		IEKG3DDefaultShaderGroup* pShaders = NULL;
		if(SUCCEEDED(pIEEngineManager->GetIEInterface(0, KG3DTYPE_DEFAULT_SHADER_GROUP, 0, 0, (LPVOID*)&pShaders)))
		{
			_ASSERTE(NULL != m_hWnd);
			pShaders->LoadAllShaders(this);
		}

	}

	nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(m_piRecPlayer);

        if (nEngineManagerInitFlag)
        {
            m_piEngineManager->UnInit();
            nEngineManagerInitFlag = false;
        }
        m_piEngineManager = NULL;
        
        if (m_hEngineDLL)
        {
            ::FreeLibrary(m_hEngineDLL);
            m_hEngineDLL = NULL;
        }
    }
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_RecPlayer::_UnInitEngine()
{
    int nResult = false;
    //int nRetCode = true;

    KG_DELETE(m_piRecPlayer);

    m_piEngineManager->UnInit();
    m_piEngineManager = NULL;

    if (m_hEngineDLL)
    {
        ::FreeLibrary(m_hEngineDLL);
        m_hEngineDLL = NULL;
    }

    nResult = true;
//Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK KG_RecPlayer::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ms_pSelf->_MsgProc(hWnd, uMsg, wParam, lParam);
}
////////////////////////////////////////////////////////////////////////////////
LRESULT KG_RecPlayer::_MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int    s_nCursorShowFlag = true;
    static DWORD  s_dwCursorLastMoveTime = 0;
    static LPARAM s_lCursorLastMosePos = 0;

    //if (hWnd == m_hWnd && message != KG3D_MULTITRHEAD_MSG && message != WM_TIMER)
    //{
    //    char szMsg[128];
    //    static int i = 0;
    //    sprintf(
    //        szMsg, 
    //        "Receive Message %d: hWnd=%X, msg=0x%X, wParam=0x%X, lParam=0x%X\n", 
    //        ++i, hWnd, message, wParam, lParam
    //    );
    //    ::OutputDebugString(szMsg);
    //}

    switch (message)
    {
    case WM_TIMER:
        if ((m_dwState == STATE_LOADING) || (m_dwState == STATE_PLAYING))
        {
            if (s_nCursorShowFlag && (::GetTickCount() - s_dwCursorLastMoveTime) >= 1000)
            {
                ::ShowCursor(FALSE);
                s_nCursorShowFlag = false;
            }
        }
        else if (!s_nCursorShowFlag)
        {
            ::ShowCursor(TRUE);
            s_nCursorShowFlag = true;
        }

       // _OnTimer();
        return 0; //return zero if it processes this message

    case WM_NCMOUSEMOVE:
    case WM_MOUSEMOVE:
        if (lParam != s_lCursorLastMosePos)
        {
            if (!s_nCursorShowFlag)
            {
                ::ShowCursor(TRUE);
                s_nCursorShowFlag = true;
            }
            s_lCursorLastMosePos   = lParam;
            s_dwCursorLastMoveTime = ::GetTickCount();
        }

        return 0;

    case WM_CLOSE:
        ::InvalidateRect(hWnd, NULL, TRUE);
        m_dwState = STATE_WAIT_FOR_EXIT; // notify render thread stop
        return 0;//return zero if it processes this message

    case WM_PAINT:
      _OnPaint();
      break; // returns zero if it processes this message
    
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0; // If an application processes this message, it should return zero.
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}


HRESULT KG_RecPlayer::GetNewIERecPlayer()
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	KG_DELETE(m_piRecPlayer);
	KGLOG_PROCESS_ERROR(m_dwState != STATE_NOT_INIT);
	{
		IRecPlayer* pPlayer = m_piEngineManager->GetNewRecPlayer();
		KGLOG_PROCESS_ERROR(NULL != pPlayer);

		IERecPlayer* pIEPlayer = dynamic_cast<IERecPlayer*>(pPlayer);
		_ASSERTE(NULL != pIEPlayer && _T("必须能转到IE接口"));
		KGLOG_PROCESS_ERROR(NULL != pIEPlayer);

		m_piRecPlayer = pIEPlayer;
		pIEPlayer = NULL;

		IERecPlayerOption opt;
		opt.bDisablePreload = g_benchMarkOption.bDisablePreload;
		m_piRecPlayer->SetOption(opt);
	}   
	KGLOG_PROCESS_ERROR(m_piRecPlayer);

	hRetCode = m_piRecPlayer->SetReplayWindow(m_hWnd);
	KGLOG_COM_PROCESS_ERROR(hRetCode);
	hResult = S_OK;
Exit0:
	return hResult;
}
//VOID KG_RecPlayer::_PrepareDrawState( DrawStateElem& drawElem )
//{
//	_ASSERTE(NULL == drawElem.hWnd);
//	drawElem.hWnd = m_hWnd;
//
//	_ASSERTE(NULL == drawElem.hDC);
//	drawElem.hDC = ::GetDC(drawElem.hWnd);
//	
//	::SetTextColor(drawElem.hDC, RGB(255, 255, 255));
//	::SetBkColor(drawElem.hDC, RGB(0, 0, 0));
//	int nRet = ::GetClientRect(drawElem.hWnd, &drawElem.clientRect);
//	_ASSERTE(nRet);
//
//	_ASSERTE(NULL == drawElem.hOldFont);
//	if(NULL != m_hFont)
//	{
//		drawElem.hOldFont = (HFONT)::SelectObject(drawElem.hDC, (HGLOBAL)m_hFont);
//	}
//	
//	return;
//}
int KG_RecPlayer::_ShowState()
{
    int nResult  = false;
    int nRetCode = false;
    HDC hDC = NULL;
    RECT clientRect;
   
    HFONT hOldFont = NULL;

    KGLOG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_SUCCESS(
        (m_dwState != STATE_LOADING) && (m_dwState != STATE_WAIT_FOR_EXIT)
    );

    hDC = ::GetDC(m_hWnd);
    KGLOG_PROCESS_ERROR(hDC);

	/*if(NULL != m_hFont)
	{
		hOldFont = (HFONT)::SelectObject(hDC, (HGDIOBJ)m_hFont);
		KGLOG_PROCESS_ERROR(m_hFont);
	}*/

    // draw text
    nRetCode = ::GetClientRect(m_hWnd, &clientRect);
    KGLOG_PROCESS_ERROR(nRetCode);

    ::SetTextColor(hDC, RGB(255, 255, 255));
    ::SetBkColor(hDC, RGB(0, 0, 0));
   
    if (m_dwState == STATE_LOADING)
    {
        _ShowNotice();
    }
    else
    {
        nRetCode = _ShowStatue_WaitForExit(hDC, clientRect.right, clientRect.bottom);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    if (NULL != hOldFont)
    {
        ::SelectObject(hDC, (HGDIOBJ)hOldFont);
        hOldFont = NULL;
    }
    if (hDC)
    {
        ::ReleaseDC (m_hWnd, hDC);
        hDC = NULL;
    }
    return nResult;
}
//////////////////////////////////////////////////////////////////////////////

VOID  KG_RecPlayer::_ShowNotice()
{
	return ;
	_ASSERTE(STATE_LOADING == m_dwState);

	//DrawStateElem drawElem;
	//_PrepareDrawState(drawElem);
	
   // INT nTmp = false;

	//_ASSERTE(NULL != drawElem.hDC);
    _ASSERTE(m_dwState == STATE_LOADING);

 //   // show notify message
	//{
	//	TCHAR NOTIFY_MESSAGE[] =
	//		_T(" JX3BenchMark will run a program, and collect some information.\r\n")
	//		_T(" This program does not collect any of your personal information.");
	//	size_t uMessageLen = _tcslen(NOTIFY_MESSAGE);

	//	RECT MessageRect;
	//	nTmp = ::SetRectEmpty(&MessageRect);
	//	KGLOG_PROCESS_ERROR(nTmp);

	//	//计算Rect的大小；
	//	nTmp = ::DrawText(drawElem.hDC, NOTIFY_MESSAGE, (int)uMessageLen, &MessageRect, DT_CALCRECT);
	//	KGLOG_PROCESS_ERROR(nTmp);

	//	// Adjust the text so it is vertical centered
	//	INT nLeft = drawElem.clientRect.left + (RECT_GetWidth(drawElem.clientRect) - RECT_GetWidth(MessageRect)) / 2;
	//	INT nTop = drawElem.clientRect.top + RECT_GetHeight(drawElem.clientRect) / 3 - RECT_GetHeight(MessageRect) / 2;
	//	::OffsetRect(&MessageRect,nLeft,nTop);
	//	nTmp = ::DrawText(drawElem.hDC, NOTIFY_MESSAGE, (int)uMessageLen, &MessageRect, 0);
	//	KGLOG_PROCESS_ERROR(nTmp);
	//}
  
//Exit0:
    return;
}
////////////////////////////////////////////////////////////////////////////////
int  KG_RecPlayer::_ShowStatue_WaitForExit(
    HDC hDC, unsigned uClientWidth, unsigned uClientHeight
)
{
    int nResult  = false;
    int nRetCode = false;
    const char* STEP_MESSAGE_ARRAY[] =
    {
        "正在退出，请稍候    ",
        "正在退出，请稍候 .  ",
        "正在退出，请稍候 .. ",
        "正在退出，请稍候 ..."
    };
    const unsigned STEP_MESSAGE_ARRAY_COUNT = 
        sizeof(STEP_MESSAGE_ARRAY) / sizeof(STEP_MESSAGE_ARRAY[0]);
    const char *pcszMessage = NULL;
    unsigned    uMessageLen = 0;
    
    ASSERT(hDC);
    ASSERT(m_dwState == STATE_WAIT_FOR_EXIT);


    // show step message
    nRetCode = ::SetTextAlign(hDC, TA_BASELINE | TA_CENTER | TA_NOUPDATECP);
    KGLOG_PROCESS_ERROR(nRetCode != GDI_ERROR);

    pcszMessage = STEP_MESSAGE_ARRAY[(::GetTickCount() / 1000) % STEP_MESSAGE_ARRAY_COUNT];
    uMessageLen = (unsigned)strlen(pcszMessage);
    nRetCode = ::TextOut(hDC,  uClientWidth / 2,  uClientHeight / 2, pcszMessage, uMessageLen); 
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
VOID KG_RecPlayer::_ShowLoadPercent(LPCTSTR strLoadContent, FLOAT fPercent)
{
	_ASSERTE((STATE_LOADING == m_dwState || STATE_READY == m_dwState) && NULL != strLoadContent);
	m_fCurrentPercentage = fPercent;
	this->PaintLoading(TRUE);
	return;
}
void __stdcall KG_RecPlayer::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	if(KM_LOADING_PERCENT == Message.m_uMessage)
	{
		KG3DMessageLoadPercent* pLoadPercent = reinterpret_cast<KG3DMessageLoadPercent*>(Message.m_dwWParam);

		if (m_dwState == STATE_READY)
		{
			_ShowLoadPercent(_T("Loading Shader"), pLoadPercent->fShaderLoadPercent * 0.1f);
		}
		else if (m_dwState == STATE_LOADING)
		{
			_ASSERTE(pLoadPercent->fPreLoadPercent - 1 < FLT_EPSILON);
			_ASSERTE(pLoadPercent->fObjectLoadingPercent - 1 < FLT_EPSILON);
			_ASSERTE(pLoadPercent->fTerrainLoadingPercent - 1 < FLT_EPSILON);
			_ASSERTE(pLoadPercent->fRestLoadingPercent - 1 < FLT_EPSILON);

			const IERecPlayerInfo& info = m_piRecPlayer->GetInfo();
			_ASSERTE(info.nSceneLoaded <= info.nSceneNeedToLoad);

			FLOAT fModelLoadPercent = 1;
			if (0 < info.nModelNeedToLoad)
			{
				fModelLoadPercent = (FLOAT)info.nModelLoaded / (FLOAT)info.nModelNeedToLoad;
				fModelLoadPercent = min(1, fModelLoadPercent);
			}

			//3个比例加上权重比较好计算
			FLOAT fSceneLoadPercent = (pLoadPercent->fPreLoadPercent * 400 + pLoadPercent->fObjectLoadingPercent * 250 + pLoadPercent->fTerrainLoadingPercent * 250 + pLoadPercent->fRestLoadingPercent * 100) /1000.f;


			_ASSERTE(fSceneLoadPercent - 1 < FLT_EPSILON);


			//已经加载的场景加上当前场景加载进度等于总进度
			fSceneLoadPercent = ((FLOAT)info.nSceneLoaded + fSceneLoadPercent)/ (FLOAT)info.nSceneNeedToLoad;
			fSceneLoadPercent = min(fSceneLoadPercent, 1);

			FLOAT fFinalPercent = 0.1f + (80 * fSceneLoadPercent + 20 * fModelLoadPercent) / 100;

			_ShowLoadPercent(_T("Loading Scene"), fFinalPercent);
		}
		
	}
}

BOOL KG_RecPlayer::LoadSource(int nWidth,int nHeight)
{
	BOOL bResult = FALSE;
	BITMAP bmp;
	char szBg[MAX_PATH] = "DebugFiles\\BenchMark\\Loading\\background.bmp";
	char szProgress[MAX_PATH] = "DebugFiles\\BenchMark\\Loading\\progress.bmp";
	//char szSprite[MAX_PATH] = "DebugFiles\\BenchMark\\Loading\\sprite.bmp";
	float fXPercent = 0.0f;
	float fYPercent = 0.0f;
	IIniFile* pIni = g_OpenIniFile("DebugFiles\\BenchMark\\Loading\\Loading.ini");
	if (pIni)
	{
		char szPath[MAX_PATH];
		pIni->GetString("Loading", "background", "", szPath, _countof(szPath));
		if (szPath[0])
		{
			strncpy(szBg, szPath, _countof(szBg));
			szBg[_countof(szBg) - 1] = '\0';
		}
		pIni->GetString("Loading", "progress", "", szPath, _countof(szPath));
		if (szPath[0])
		{
			strncpy(szProgress, szPath, _countof(szProgress));
			szProgress[_countof(szProgress) - 1] = '\0';
		}
		/*pIni->GetString("Loading", "sprite", "", szPath, _countof(szPath));
		if (szPath[0])
		{
			strncpy(szSprite, szPath, _countof(szSprite));
			szSprite[_countof(szSprite) - 1] = '\0';
		}*/
		pIni->GetString("Loading", "strFont", "Arial Unicode MS", szPath, _countof(szPath));
		m_strfont = szPath;
		pIni->GetString("Loading", "strFontFPS", "Arial Unicode MS", szPath, _countof(szPath));
		m_strfontFPS = szPath;
		pIni->GetInteger("Loading", "nFontSize", 20, &m_nFontSize);
		pIni->GetInteger("Loading", "nFontFPSSize", 60, &m_nFontFPSSize);

		pIni->GetFloat("Loading", "ProgressXPercent",0.1f,&fXPercent);
		pIni->GetFloat("Loading", "ProgressYPercent",0.8f,&fYPercent);
		m_nProgressX = (int)(nWidth *  fXPercent);
		m_nProgressY = (int)(nHeight * fYPercent);

		pIni->GetFloat("Loading", "fFontXPercent",0.06f,&fXPercent);
		pIni->GetFloat("Loading", "fFontYPercent",0.1f,&fYPercent);
		m_nFontPosX = (int)(nWidth *  fXPercent);
		m_nFontPosY = (int)(nHeight * fYPercent);
		SAFE_RELEASE(pIni);
	}

	m_hBg = (HBITMAP)LoadImageA(m_hInstance, szBg, IMAGE_BITMAP, nWidth, nHeight, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	KGLOG_PROCESS_ERROR(m_hBg);
	GetObject(m_hBg, sizeof(BITMAP), &bmp);
	m_nBgWidth = bmp.bmWidth;
	m_nBgHeight = bmp.bmHeight;

	m_hProgress = (HBITMAP)LoadImageA(m_hInstance, szProgress, IMAGE_BITMAP,nWidth -m_nProgressX * 2, 10, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	KGLOG_PROCESS_ERROR(m_hProgress);
	GetObject(m_hProgress, sizeof(BITMAP), &bmp);
	m_nProgressWidth = bmp.bmWidth;
	m_nProgressHeight = bmp.bmHeight;
	
	/*m_hSprite = (HBITMAP)LoadImageA(m_hInstance, szSprite, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	KGLOG_PROCESS_ERROR(m_hSprite);
	GetObject(m_hSprite, sizeof(BITMAP), &bmp);*/
	//m_nSpriteWidth = bmp.bmWidth;
	//m_nSpriteHeight = bmp.bmHeight;

	bResult = TRUE;
Exit0:
	SAFE_RELEASE(pIni);
	return bResult;

}
void KG_RecPlayer::UnloadSource()
{
	/*if (m_hSprite)
	{
		DeleteObject(m_hSprite);
		m_hSprite = NULL;
	}*/

	if (m_hProgress)
	{
		DeleteObject(m_hProgress);
		m_hProgress = NULL;
	}

	if (m_hBg)
	{
		DeleteObject(m_hBg);
		m_hBg = NULL;
	}
}
void KG_RecPlayer::_OnPaint()
{
	if(m_dwState == STATE_SHOW_RESULT)
	{
		PaintResult();
	}
	else
	{
		PaintLoading(TRUE);
	}
}
void KG_RecPlayer::PaintLoading(BOOL bProgress)
{
	HDC hDc = NULL;
	HDC hdcSrc = NULL;
	HGDIOBJ hOldObj = NULL;
	int nProgressLen = (int)(m_nProgressWidth * m_fCurrentPercentage);
	KG_PROCESS_ERROR(m_hWnd);

	hDc = GetDC(m_hWnd);
	KG_PROCESS_ERROR(hDc);
	
	hdcSrc = CreateCompatibleDC(NULL);
	KG_PROCESS_ERROR(hdcSrc);

	hOldObj = SelectObject(hdcSrc, m_hBg);

	BitBlt(hDc, 0, 0, m_nBgWidth, m_nBgHeight, hdcSrc, 0, 0, SRCCOPY);

	if (bProgress)
	{
		SelectObject(hdcSrc, m_hProgress);
		BitBlt(hDc, m_nProgressX, m_nProgressY, nProgressLen, m_nProgressHeight, hdcSrc, 0, 0, SRCCOPY);

		//m_nRestorX = m_nProgressX + nProgressLen - m_nSpriteWidth / 2;
		//m_nRestorY = m_nProgressY + (m_nProgressHeight - m_nSpriteHeight) / 2;

		/*SelectObject(hdcSrc, m_hSprite);
		AlphaBlend(hDc, m_nRestorX, m_nRestorY, m_nSpriteWidth, m_nSpriteHeight, hdcSrc, 0, 0, m_nSpriteWidth, m_nSpriteHeight, m_bldFunc);*/

		
	}
	SelectObject(hdcSrc, hOldObj);
Exit0:
	if (hdcSrc)
		DeleteDC(hdcSrc);
	if (hDc)
		ReleaseDC(m_hWnd, hDc);
	
}

HRESULT KG_RecPlayer::PaintResult()
{
	BOOL nRetCode = FALSE;
	HDC hDc = NULL;
	HDC hdcSrc = NULL;
	HGDIOBJ hOldObj = NULL;
	int nWidth = RECT_GetWidth(m_WndInitRect);
	int nHeight= RECT_GetHeight(m_WndInitRect);
	char szBg[MAX_PATH] = "DebugFiles\\BenchMark\\Loading\\Result.bmp";
	
	hDc = ::GetDC(m_hWnd);
	KGLOG_PROCESS_ERROR(hDc);

	HBITMAP hBitmap = (HBITMAP)LoadImageA(m_hInstance, szBg, IMAGE_BITMAP,nWidth ,nHeight
		, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	KGLOG_PROCESS_ERROR(hBitmap);
	

	hdcSrc = CreateCompatibleDC(NULL);
	KG_PROCESS_ERROR(hdcSrc);

	hOldObj = SelectObject(hdcSrc, hBitmap);

	BitBlt(hDc, 0, 0, nWidth, nHeight, hdcSrc, 0, 0, SRCCOPY);


	/*if(NULL != m_hFont)
	{
	hOldFont = (HFONT)::SelectObject(hDC, (HGDIOBJ)m_hFont);
	KGLOG_PROCESS_ERROR(m_hFont);
	}*/
	
	// draw text
	{
		HFONT hOldFont = NULL;
		HFONT hFont = NULL;
		HFONT hFontFPS = NULL;
		LOGFONT logFont;
		memset(&logFont, 0, sizeof(LOGFONT));
		logFont.lfCharSet = DEFAULT_CHARSET;
		logFont.lfHeight = m_nFontSize;
		//logFont.lfWidth  = 10;
		//logFont.lfWeight = 200;
		sprintf_s(logFont.lfFaceName,sizeof(logFont.lfFaceName),"%s",m_strfont.c_str());  
		
		hFont = ::CreateFontIndirect(&logFont);
		KGLOG_PROCESS_ERROR(hFont);		
		
		logFont.lfHeight = m_nFontFPSSize;
		sprintf_s(logFont.lfFaceName,sizeof(logFont.lfFaceName),"%s",m_strfontFPS.c_str());
		hFontFPS = ::CreateFontIndirect(&logFont);
		KGLOG_PROCESS_ERROR(hFontFPS);	

		hOldFont=(HFONT)SelectObject(hDc,hFontFPS);
		
		::SetTextColor(hDc, RGB(198, 198, 198));
		::SetBkColor(hDc, RGB(0, 0, 0));
		::SetBkMode(hDc,TRANSPARENT);
		int nX = m_nFontPosX;
		int nY = m_nFontPosY;
		int nYStep = m_nFontSize + 10;
		TCHAR szInfo[MAX_PATH];

		IERecPlayerInfo infoResult;
		infoResult = m_piRecPlayer->GetInfo();
		sprintf_s(szInfo,sizeof(szInfo),"%d " ,infoResult.nAvgFPS);
		TextOut(hDc,nX + 180,nY,szInfo,(int)strlen(szInfo));
		
		SelectObject(hDc,hFont);
		
		nY += m_nFontFPSSize - m_nFontSize ;
		sprintf_s(szInfo,sizeof(szInfo),"Average FPS:");
		TextOut(hDc,nX,nY,szInfo,(int)strlen(szInfo));

		nY += m_nFontFPSSize;
		sprintf_s(szInfo,sizeof(szInfo),"Min : %d fps       Max : %d fps",infoResult.nMinFps,infoResult.nMaxFps);
		TextOut(hDc,nX,nY,szInfo,(int)strlen(szInfo));
		nY += nYStep;
		
		trim(infoResult.m_strCPU);
		TextOut(hDc,nX,nY,(infoResult.m_strCPU).c_str(),(int)(infoResult.m_strCPU).length());
		nY += nYStep;
		trim(infoResult.m_strDisplayCrad);
		TextOut(hDc,nX,nY,(infoResult.m_strDisplayCrad).c_str(),(int)(infoResult.m_strDisplayCrad).length());
		nY += nYStep;
		if(infoResult.m_bUseGpu)
			sprintf_s(szInfo,sizeof(szInfo),"Hardware PhyX Accelerate Enable:  Yes");
		else
			sprintf_s(szInfo,sizeof(szInfo),"Hardware PhyX Accelerate Enable:  No");
		TextOut(hDc,nX,nY,szInfo,(int)strlen(szInfo));
		 
		::SetTextColor(hDc, RGB(255, 100, 100));
		sprintf_s(szInfo,sizeof(szInfo),"Press 'Esc' to end the BenchMark");
		nX = (int) (nWidth * 0.35f);
		nY = (int) (nHeight * 0.95f);
		TextOut(hDc, nX,nY ,szInfo,(int)strlen(szInfo));
	
		hOldFont=(HFONT)SelectObject(hDc,hOldFont);

		if(hFont)
			DeleteObject(hFont);
		if(hFontFPS)
			DeleteObject(hFontFPS);
	}
	
	
	SelectObject(hdcSrc, hOldObj);
Exit0:
	if(hBitmap)
		DeleteObject(hBitmap);
	if (hdcSrc)
		DeleteDC(hdcSrc);
	if (hDc)
		ReleaseDC(m_hWnd, hDc);
	s_bShowResult = true;
	return S_OK;
}

const IERecPlayerInfo& KG_RecPlayer::GetInfo() const
{
	return (m_piRecPlayer->GetInfo());
}
//VOID KG_RecPlayer::_LoadBenchMarkOption()
//{
//	_ASSERTE(NULL != m_piRecPlayer);
//
//	TCHAR strBenchMarkOpPath[] = _T("DebugFiles/BenchMark/BenchMark.ini");
//
//	IIniFile* pIni = g_OpenIniFile(strBenchMarkOpPath);
//	KGLOG_PROCESS_ERROR(NULL != pIni);
//
//	{
//		IERecPlayerOption opt;
//		KGLOG_PROCESS_ERROR(0 < pIni->GetInteger("Global", "DisablePreload", 0, &opt.bDisablePreload));
//
//		m_piRecPlayer->SetOption(opt);
//	}
//
//Exit0:
//	SAFE_RELEASE(pIni);
//	return;
//}
//

#include "stdafx.h"
#include "KLoadingWindow.h"

KLoadingWindow::KLoadingWindow()
{
	m_hWnd = NULL;
	m_fCurrentPercentage = 0.0f;
	m_fPercentage = 0.0f;
	m_fNextPercentage = 0.0f;
	m_dwTimeLast = 0;
	m_dwStartTime = 0;
	m_dwTime = 0;
	m_bLoading = FALSE;
	m_bFinished = FALSE;
	m_hInstance = NULL;
	m_szClass[0] = '\0';
	m_szTitle[0] = '\0';
	m_nIcon = 0;
	m_hBg = NULL;
	m_hProgress = NULL;
	m_hSprite = NULL;
	m_nBgWidth = 600;
	m_nBgHeight = 333;
	m_nProgressWidth = 338;
	m_nProgressHeight = 3;
	m_nSpriteWidth = 28;
	m_nSpriteHeight = 28;
	m_nProgressX = 35;
	m_nProgressY = 297;
	m_bldFunc.BlendOp = AC_SRC_OVER;
	m_bldFunc.BlendFlags = 0;
	m_bldFunc.SourceConstantAlpha = 255;
	m_bldFunc.AlphaFormat = AC_SRC_ALPHA;
	m_nRestorX = 0;
	m_nRestorY = 0;
	m_bCanceled = FALSE;
}

KLoadingWindow::~KLoadingWindow()
{
	EndLoading();
}

BOOL KLoadingWindow::Init(HINSTANCE hInstance, LPCSTR szAppName, int nIcon)
{
	m_nIcon = nIcon;
	m_hInstance = hInstance;

	strcpy(m_szTitle, szAppName);
	strcpy(m_szClass, szAppName);
	strcat(m_szClass, "_LoadingClass");

	return TRUE;
}

void KLoadingWindow::Exit()
{
	EndLoading();
}

void KLoadingWindow::StartLoading(DWORD dwTime)
{
	EndLoading();
	m_bLoading = TRUE;
	m_dwTimeLast = KG_GetTickCount();
	m_dwStartTime = m_dwTimeLast;
	if (dwTime == 0)
		dwTime = 500;
	m_dwTime = dwTime;
	m_fPercentage = 0.0f;
	m_fNextPercentage = 1.0f;
	m_bCanceled = FALSE;
	m_Thread.Create(ThreadFunction, this);
}

void KLoadingWindow::SetPercentage(float fPercentage, float fNextPercentage)
{
	m_fPercentage = fPercentage;
	if (fPercentage >= 1.0f)
		m_dwTime = (DWORD)((float)(KG_GetTickCount() - m_dwStartTime) / fPercentage);
	
	m_fNextPercentage = fNextPercentage;
}

BOOL KLoadingWindow::IsFinished()
{
	return m_bFinished;
}

void KLoadingWindow::EndLoading()
{
	if (m_bLoading)
	{
        if (m_hWnd)
        {
		    ::ShowWindow(m_hWnd, SW_HIDE);
        }

		m_bLoading = FALSE;
		m_Thread.Destroy();
	}
}

void KLoadingWindow::HideWindow()
{
    if (m_hWnd)
    {
        ::ShowWindow(m_hWnd, SW_HIDE);
    }
}

BOOL KLoadingWindow::IsCanceled()
{
	return m_bCanceled;
}

LRESULT CALLBACK KLoadingWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KLoadingWindow* pWindow = (KLoadingWindow*)(LONG_PTR)GetWindowLongPtrA(hWnd, GWL_USERDATA);
	if (pWindow)
	{
		switch (message)
		{
		case WM_CREATE:
		case WM_PAINT:
			pWindow->Paint(TRUE);
			break;
		case WM_CLOSE:
		case WM_QUIT:
			pWindow->SetCanceled(TRUE);
			break;
		}
	}

	return DefWindowProcA(hWnd, message, wParam, lParam);
}

void KLoadingWindow::SetCanceled(BOOL bCanceled)
{
	m_bCanceled = bCanceled;
}

void KLoadingWindow::ThreadFunction(void* lpParam)
{
	KLoadingWindow* pLoadingWindow = (KLoadingWindow *)lpParam;
	if (pLoadingWindow)
    {
		pLoadingWindow->ThreadFunction();
    }
}

BOOL KLoadingWindow::LoadSource()
{
	BOOL bResult = FALSE;
	BITMAP bmp;
	char szBg[MAX_PATH] = "UI\\Loading\\background.bmp";
	char szProgress[MAX_PATH] = "UI\\Loading\\progress.bmp";
	char szSprite[MAX_PATH] = "UI\\Loading\\sprite.bmp";

	IIniFile* pIni = g_OpenIniFile("UI\\Loading\\Loading.ini");
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
		pIni->GetString("Loading", "sprite", "", szPath, _countof(szPath));
		if (szPath[0])
		{
			strncpy(szSprite, szPath, _countof(szSprite));
			szSprite[_countof(szSprite) - 1] = '\0';
		}
		pIni->GetInteger("Loading", "ProgressX", m_nProgressX, &m_nProgressX);
		pIni->GetInteger("Loading", "ProgressY", m_nProgressY, &m_nProgressY);
		SAFE_RELEASE(pIni);
	}

	m_hBg = (HBITMAP)LoadImageA(m_hInstance, szBg, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	KGLOG_PROCESS_ERROR(m_hBg);
	GetObject(m_hBg, sizeof(BITMAP), &bmp);
	m_nBgWidth = bmp.bmWidth;
	m_nBgHeight = bmp.bmHeight;

	m_hProgress = (HBITMAP)LoadImageA(m_hInstance, szProgress, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	KGLOG_PROCESS_ERROR(m_hProgress);
	GetObject(m_hProgress, sizeof(BITMAP), &bmp);
	m_nProgressWidth = bmp.bmWidth;
	m_nProgressHeight = bmp.bmHeight;

	m_hSprite = (HBITMAP)LoadImageA(m_hInstance, szSprite, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	KGLOG_PROCESS_ERROR(m_hSprite);
	GetObject(m_hSprite, sizeof(BITMAP), &bmp);
	m_nSpriteWidth = bmp.bmWidth;
	m_nSpriteHeight = bmp.bmHeight;

	bResult = TRUE;
Exit0:
	SAFE_RELEASE(pIni);
	return bResult;

}

void KLoadingWindow::UnloadSource()
{
	if (m_hSprite)
	{
		DeleteObject(m_hSprite);
		m_hSprite = NULL;
	}

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

void KLoadingWindow::ThreadFunction()
{
	BOOL bResult = FALSE;
	BOOL bInitClass = FALSE;

	WNDCLASS wc;
	MSG msg;
	int nX = 0;
	int nY = 0;

	bResult = LoadSource();
	KGLOG_PROCESS_ERROR(bResult);

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

	bResult = RegisterClassA(&wc);
	KGLOG_PROCESS_ERROR(bResult);

	bInitClass = TRUE;

	nX = (::GetSystemMetrics(SM_CXSCREEN) - m_nBgWidth) / 2;
	nY = (::GetSystemMetrics(SM_CYSCREEN) - m_nBgHeight) / 2;

	m_hWnd = CreateWindowExA(
		0,
		m_szClass, 
		m_szTitle,
		WS_VISIBLE | WS_POPUP, 
		nX, nY, m_nBgWidth, m_nBgHeight,
		NULL, 
		NULL, 
		NULL, 
		NULL);
	KGLOG_PROCESS_ERROR(m_hWnd);

	::SetWindowLongPtrA(m_hWnd, GWL_USERDATA, (__int3264)(LONG_PTR)this);

	bResult = UpdateWindow(m_hWnd);
	KGLOG_PROCESS_ERROR(bResult);

	while (m_bLoading) 
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Calculate();
		Sleep(50);
	}

Exit0:
	if (bInitClass)
	{
		UnregisterClassA(m_szClass, m_hInstance);
		m_szClass[0] = '\0';
	}

	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	UnloadSource();
	m_bFinished = TRUE;
}

void KLoadingWindow::Calculate()
{
	DWORD dwTimeNow = KG_GetTickCount();
	DWORD dwDeltaTime = dwTimeNow - m_dwTimeLast;
	m_dwTimeLast = dwTimeNow;

	float fDelta = (float)dwDeltaTime / (float)m_dwTime;

	float fSet = m_fCurrentPercentage;

	if (m_fCurrentPercentage < m_fPercentage)
	{
		if (fDelta < 0.003f)
			fDelta = 0.003f;
		fSet += fDelta * 3.0f;
	}
	else
	{
		fSet += fDelta;
	}

	if (fSet >= 1.0f)
		m_bFinished = TRUE;

	if (fSet > m_fNextPercentage)
		fSet = m_fNextPercentage;
	
	if (m_fCurrentPercentage != fSet)
	{
		m_fCurrentPercentage = fSet;
		Paint(FALSE);
	}
}

void KLoadingWindow::Paint(BOOL bRepaint)
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

	if (bRepaint)
	{
		BitBlt(hDc, 0, 0, m_nBgWidth, m_nBgHeight, hdcSrc, 0, 0, SRCCOPY);

		SelectObject(hdcSrc, m_hProgress);
		BitBlt(hDc, m_nProgressX, m_nProgressY, nProgressLen, m_nProgressHeight, hdcSrc, 0, 0, SRCCOPY);
	}
	else
	{
		BitBlt(hDc, m_nRestorX, m_nRestorY, m_nBgWidth, m_nBgHeight, hdcSrc, m_nRestorX, m_nRestorY, SRCCOPY);

		SelectObject(hdcSrc, m_hProgress);
		BitBlt(hDc, m_nRestorX, m_nProgressY, nProgressLen - m_nRestorX + m_nProgressX, m_nProgressHeight, hdcSrc, m_nRestorX - m_nProgressX, 0, SRCCOPY);
	}

	m_nRestorX = m_nProgressX + nProgressLen - m_nSpriteWidth / 2;
	m_nRestorY = m_nProgressY + (m_nProgressHeight - m_nSpriteHeight) / 2;

	SelectObject(hdcSrc, m_hSprite);
	AlphaBlend(hDc, m_nRestorX, m_nRestorY, m_nSpriteWidth, m_nSpriteHeight, hdcSrc, 0, 0, m_nSpriteWidth, m_nSpriteHeight, m_bldFunc);

	SelectObject(hdcSrc, hOldObj);
	
Exit0:
	if (hdcSrc)
		DeleteDC(hdcSrc);
	if (hDc)
		ReleaseDC(m_hWnd, hDc);
}

HWND KLoadingWindow::GetHWND() const
{
    return m_hWnd;
}

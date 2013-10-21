#include "stdafx.h"
#include "./kinput.h"
#include "./khotkey.h"
#include "../elem/wndmessage.h"
#include "../elem/wndedit.h"
#include "../config/kdebugconfig.h"
#include "../script/ksystemscripttable.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"
#include "IKG3DSoundShell.h"

namespace UI
{


	KInput::KInput()
        : m_bActivated(true)
	{
	}


	KInput::~KInput()
	{
	}


	bool KInput::Init()
	{
		return true;
	}

	
	void KInput::Exit()
	{
	}

	LRESULT KInput::HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRet = FALSE;

		if (g_pUI->m_HotkeyMgr.CaptureAllHotkey(uMsg, wParam, lParam))
			return lRet;

		switch (uMsg)
		{
		case WM_MOVE:
			g_pUI->m_WndStation.SetClientPosition((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			lRet = TRUE;
			break;
		case WM_SIZE:
			g_pUI->m_WndStation.SetClientSize(LOWORD(lParam), HIWORD(lParam));
			lRet = TRUE;
			break;
		case WM_SIZING:
			lRet = OnSizing(uMsg, wParam, lParam);
			break;
		case WM_NCCALCSIZE:
			return OnNCSizing(uMsg, wParam, lParam);
			break;
		case WM_MOUSEMOVE:
            if (m_bActivated)
            {
				if (g_pUI->m_WndStation.IsExclusiveMode())
				{
					POINT ptCursor;

					GetCursorPos(&ptCursor);

					g_pUI->m_p3DUI->SetD3D9DeviceCursorPosition(ptCursor.x, ptCursor.y, 0);
				}

				lRet = g_pUI->m_WndStation.HandleInput(uMsg, wParam, lParam);
            }
            break;
        case WM_SETCURSOR:
            if (m_bActivated)
            {
                if (g_pUI->m_WndStation.IsExclusiveMode())
                    g_pUI->m_p3DUI->ShowD3D9DeviceCursor(g_pUI->m_Pointer.IsCursorVisible());

                lRet = g_pUI->m_WndStation.HandleInput(uMsg, wParam, lParam);
            }
            break;
		case WM_NCPAINT:
			{
				HWND hWnd = g_pUI->m_hWnd;

                if (g_pUI->m_WndStation.IsProcessNCSizing() && g_pUI->m_WndStation.IsFullScreen() && 
					g_pUI->m_WndStation.IsPanauision() && !g_pUI->m_WndStation.IsExclusiveMode())
				{
					HDC hDC = ::GetWindowDC(hWnd);
					if (hDC)
					{
						HBRUSH hBrush = ::CreateSolidBrush(RGB(0,0,0));
						if (hBrush)
						{
							RECT rectWindow;
							RECT rectClient;
							RECT rectDraw;
							::GetWindowRect(hWnd, &rectWindow);
							::GetClientRect(hWnd, &rectClient);
							rectDraw.left = 0;
							rectDraw.right = rectWindow.right - rectWindow.left;
							rectDraw.top = 0;
							rectDraw.bottom = ((rectWindow.bottom - rectWindow.top) - (rectClient.bottom - rectClient.top)) / 2;
							::FillRect(hDC, &rectDraw, hBrush);
							rectDraw.top = (rectWindow.bottom - rectWindow.top) - ((rectWindow.bottom - rectWindow.top) - (rectClient.bottom - rectClient.top)) / 2;
							rectDraw.bottom = rectWindow.bottom - rectWindow.top;
							::FillRect(hDC, &rectDraw, hBrush);
							::DeleteObject(hBrush);
							hBrush = NULL;
						}
						::ReleaseDC(hWnd, hDC);
					}
				}
				return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
			break;
		case WM_NCACTIVATE:
			lRet = OnNCActivate(uMsg, wParam, lParam);
			break;
        case WM_NCHITTEST:
            lRet = DefWindowProc(g_pUI->m_hWnd, uMsg, wParam, lParam);
            if (lRet == HTCAPTION)
                g_pUI->m_HotkeyMgr.FlipAllDownHotkey();
            break;
		case WM_ACTIVATEAPP:
            m_bActivated = wParam != 0;
			lRet = OnActivateApp(uMsg, wParam, lParam);			
			break;
		case WM_CLOSE:
			if (g_pUI->GetInitReason() != RETURN_CLEAR_UI)
			{
				g_pUI->m_Script.FireEvent("WM_CLOSE", 0);
				lRet = TRUE;
			}
			break;
		case WM_QUIT:
			g_pUI->m_Script.FireEvent("WM_QUIT", 0);
			break;
		case WM_INPUTLANGCHANGEREQUEST:
		case WM_IME_NOTIFY:
			break;
        case WM_KG3DSOUND_MSG:
            OnKG3DSoundMsg(wParam, lParam);
            break;
		case WM_WINDOWPOSCHANGED:
			lRet = OnWindowPosChanged(uMsg, wParam, lParam);
			break;
		default:
			if (m_bActivated)
				lRet = g_pUI->m_WndStation.HandleInput(uMsg, wParam, lParam);
			break;
		}

		return lRet;
	}

    void KInput::OnKG3DSoundMsg(WPARAM wParam, LPARAM lParam)
    {
        switch (wParam)
        {
        case KG3DSOUND_MUSIC_FINISHED:
            KMusicMgr::BgMusicFinishCallback();
            break;
        default:
            break;
        }
    }

	LRESULT KInput::OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRet = FALSE;
		int nRetCode = false;
		RECT rcClient;
		RECT rcWindow;
		RECT& rcSizing = *reinterpret_cast<PRECT>(lParam);
		HWND hWnd = NULL;
		LONG lDeltaX = 0;
		LONG lDeltaY = 0;
		LONG lWidth = 0;
		LONG lHeight = 0;
		float fYXRatio = 3.0f / 4.0f;
		KWndStation &wndStation = g_pUI->m_WndStation;

        if (::GetAsyncKeyState(VK_SHIFT) & 0x8000)
        {
            ::GetClientRect(g_pUI->m_hWnd, &rcClient);
            ::GetWindowRect(g_pUI->m_hWnd, &rcWindow);

            lDeltaX = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
            lDeltaY = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);

            lWidth = rcSizing.right - rcSizing.left - lDeltaX;
            lHeight = rcSizing.bottom - rcSizing.top - lDeltaY;

            if (wndStation.GetClientWidth() > FLT_EPSILON)
                fYXRatio = wndStation.GetClientHeight() / wndStation.GetClientWidth();

            //限制最小值
            if (lWidth < 100 || lHeight < 100 * fYXRatio)
            {
                rcSizing.left = rcWindow.left;
                rcSizing.top = rcWindow.top;
                rcSizing.right = rcWindow.right;
                rcSizing.bottom = rcWindow.bottom;
                goto Exit1;
            }

            switch (wParam)
            {
            case WMSZ_BOTTOM:
            case WMSZ_BOTTOMRIGHT:
            case WMSZ_TOPRIGHT:
                lWidth =  static_cast<LONG>(lHeight / fYXRatio);
                rcSizing.right = rcSizing.left + lWidth + lDeltaX;
                break;
            case WMSZ_LEFT:
            case WMSZ_RIGHT:
                lHeight =  static_cast<LONG>(lWidth * fYXRatio);
                rcSizing.bottom = rcSizing.top + lHeight + lDeltaY;
                break;
            case WMSZ_TOP:
            case WMSZ_TOPLEFT:
            case WMSZ_BOTTOMLEFT:
                lWidth =  static_cast<LONG>(lHeight / fYXRatio);
                rcSizing.left = rcSizing.right - lWidth - lDeltaX;
                break;
            }
        }
	
Exit1:
		lRet = TRUE;
		return lRet;
	}

	LRESULT KInput::OnNCSizing(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (g_pUI->m_WndStation.IsProcessNCSizing() && g_pUI->m_WndStation.IsFullScreen() && 
			g_pUI->m_WndStation.IsPanauision() && !g_pUI->m_WndStation.IsExclusiveMode())
		{
			RECT *pRc = (RECT *)(lParam);
			int nDeltaH = ((pRc->bottom - pRc->top) * 16 - (pRc->right - pRc->left) * 9) / 32;
			if (nDeltaH > 0)
			{
				pRc->top += nDeltaH;
				pRc->bottom -= nDeltaH;
			}
			return ::DefWindowProc(g_pUI->m_hWnd, uMsg, wParam, lParam);;
		}
		return FALSE;
	}

	LRESULT KInput::OnActivateApp(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		g_pUI->m_HotkeyMgr.FlipAllDownHotkey();
        g_pUI->m_WndStation.Activate(m_bActivated);

		return TRUE;
	}

	LRESULT KInput::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WINDOWPOS * pPos = (WINDOWPOS *)(lParam);
		if (pPos)
		{
			g_pUI->m_WndStation.SetWindowPosition(pPos->x, pPos->y);
			if (g_pUI->m_WndStation.IsFullScreen() && !g_pUI->m_WndStation.IsExclusiveMode())
			{
				int nX = 0;
				int nY = 0;
				MONITORINFO Info;
				Info.cbSize = sizeof(MONITORINFO);
				HMONITOR hM = ::MonitorFromWindow(g_pUI->m_hWnd, MONITOR_DEFAULTTONEAREST);
				if (hM && ::GetMonitorInfo(hM, &Info))
				{
					nX = Info.rcMonitor.left;
					nY = Info.rcMonitor.top;
				}

				if (pPos->x != nX || pPos->y != nY)
				{
					g_pUI->m_WndStation.SetWindowPosition(nX, nY);
					g_pUI->m_WndStation.SetProcessNCSizing(TRUE);
					::SetWindowPos(g_pUI->m_hWnd, NULL, nX, nY, Info.rcMonitor.right - Info.rcMonitor.left, Info.rcMonitor.bottom - Info.rcMonitor.top, 
						SWP_NOREDRAW | SWP_NOZORDER | SWP_FRAMECHANGED);
					g_pUI->m_WndStation.SetProcessNCSizing(FALSE);
					return 0;
				}
			}
		}
		return DefWindowProc(g_pUI->m_hWnd, uMsg, wParam, lParam);
	}

	LRESULT KInput::OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return FALSE;
	}

    bool KInput::IsActivated() const
    {
        return m_bActivated;
    }

}

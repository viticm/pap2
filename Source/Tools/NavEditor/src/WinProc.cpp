#define _WIN32_WINNT 0x0500 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>
#include <zmouse.h>
#include <WinUser.h>
#include "KApplication.h"
#define _WIN32_WINNT 0x0500 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>
#include <zmouse.h>
#include <WinUser.h>
#include "KApplication.h"
#include "KCamera.h"
#include "KMainPlayer.h"
#include "KScene.h"
#include "WinProc.h"
#include "MathTools.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    LRESULT                 nResult    = 0;

	switch (uMessage)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

    case WM_SIZE:
        g_App.ResetDevice();
        break;

    case WM_MOUSEMOVE:
        g_App.OnMouseMove(wParam, lParam);
        break;

    case WM_MOUSEWHEEL:
        g_Camera.OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        break;

    case WM_KEYUP:
        if (wParam == VK_ADD)
        {
            g_Player.SpeedUp();
        }

        if (wParam == VK_SUBTRACT)
        {
            g_Player.SpeedDown();
        }

        if (wParam == VK_F3)
        {
            g_Scene.m_bDrawSolid = g_Scene.m_bDrawSolid ? false : true;
        }

        if (wParam == VK_F4)
        {
            g_Scene.m_bDrawWireframe = g_Scene.m_bDrawWireframe ? false : true;
        }

        if (wParam == VK_F5)
        {
            g_Scene.m_bDrawUpFaceOnly = g_Scene.m_bDrawUpFaceOnly ? false : true;
        }

        if (wParam == VK_F6)
        {
            g_Scene.m_bPR = g_Scene.m_bPR ? false : true;
            g_Scene.PR();
        }

        break;

	default:
		nResult = DefWindowProc(hWnd, uMessage, wParam, lParam);
	}

	return nResult;
}


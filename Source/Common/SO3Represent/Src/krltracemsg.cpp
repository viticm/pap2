#include "stdafx.h"
#include "./krltracemsg.h"
#include "./SO3Represent.h"
#include "./common/kmathtools.h"
#include "SO3World/KSO3World.h"

int TraceMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        if ((lParam & 0x40000000) == 0) 
            printf("[RL] %f %d msg WM_KEYDOWN 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_KEYUP:
        printf("[RL] %f %d msg WM_KEYUP 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_CHAR:
        printf("[RL] %f %d msg WM_CHAR 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_DEADCHAR:
        printf("[RL] %f %d msg WM_DEADCHAR 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_SYSKEYDOWN:
        printf("[RL] %f %d msg WM_SYSKEYDOWN 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_SYSKEYUP:
        printf("[RL] %f %d msg WM_SYSKEYUP 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_SYSCHAR:
        printf("[RL] %f %d msg WM_SYSCHAR 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_SYSDEADCHAR:
        printf("[RL] %f %d msg WM_SYSDEADCHAR 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;

    case WM_MOUSEMOVE:
        break;

    case WM_LBUTTONDOWN:
        printf("[RL] %f %d msg WM_LBUTTONDOWN 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_LBUTTONUP:
        printf("[RL] %f %d msg WM_LBUTTONUP 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_LBUTTONDBLCLK:
        printf("[RL] %f %d msg WM_LBUTTONDBLCLK 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_RBUTTONDOWN:
        printf("[RL] %f %d msg WM_RBUTTONDOWN 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_RBUTTONUP:
        printf("[RL] %f %d msg WM_RBUTTONUP 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_RBUTTONDBLCLK:
        printf("[RL] %f %d msg WM_RBUTTONDBLCLK 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_MBUTTONDOWN:
        printf("[RL] %f %d msg WM_MBUTTONDOWN 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_MBUTTONUP:
        printf("[RL] %f %d msg WM_MBUTTONUP 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    case WM_MBUTTONDBLCLK:
        printf("[RL] %f %d msg WM_MBUTTONDBLCLK 0x%p, 0x%p\n", GetPerformanceTickCount(), g_pRL->m_pSO3World->m_nGameLoop, wParam, lParam);
        break;
    }

    return true;
}

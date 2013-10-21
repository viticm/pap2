/*****************************************************************************************
//	界面窗口体系结构--输入窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-22
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include <string>
#include "../../Public/KWin32.h"
#include "WndEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


extern HINSTANCE    hInst;  // in S3Client.cpp
		//
        //m_hEdit = CreateWindowEx(
        //    0, "EDIT", NULL,
        //    ES_LEFT | WS_CHILD,
        //    //0, 0,
        //    //100, 20,
        //    m_nAbsoluteLeft, m_nAbsoluteTop,
        //    m_Width, m_Height,
        //    ::g_GetMainHWnd(),
        //    NULL, hInst, NULL
        //);
        //
        //ShowWindow(m_hEdit, SW_SHOW);

HWND Win32Edit_Create(HWND hGameMainWnd, unsigned int uFlag, int nLimitLen)
{
    DWORD dwStyle = WS_POPUP | ES_LEFT | ES_AUTOHSCROLL;

    if (uFlag & WNDEDIT_ES_MASK_CHARACTER)
    {
        dwStyle |= ES_PASSWORD;
    }

    if (uFlag & WNDEDIT_ES_MULTI_LINE)
    {
        dwStyle |= ES_MULTILINE | ES_AUTOVSCROLL;
    }


    HWND hwndEdit = ::CreateWindowEx(
        0, "EDIT", NULL,
        dwStyle,
        0, 0,
        0, 0,
        hGameMainWnd,
        NULL, hInst, NULL
    );

    if (hwndEdit)
    {
        ::SendMessage(hwndEdit, EM_LIMITTEXT, nLimitLen - 1, 0);
    }

    return hwndEdit;
}
void Win32Edit_UpdateSizePos(HWND hWnd, int nLeft, int nTop, int nWidth, int nHeight)
{
    if (
        (!hWnd) ||
        (!::IsWindow(hWnd))
    )
        return;

    RECT rtMainWndRect;
    ::GetClientRect(g_GetMainHWnd(), &rtMainWndRect);
    POINT ptLeftTop;
    ptLeftTop.x = rtMainWndRect.left + nLeft;
    ptLeftTop.y = rtMainWndRect.top  + nTop; //540; //572;
    ::ClientToScreen(g_GetMainHWnd(), &ptLeftTop);


    ::SetWindowPos(hWnd, HWND_TOP, ptLeftTop.x, ptLeftTop.y, nWidth, nHeight, 0);
}

void Win32Edit_Destroy(HWND hWnd)
{
    if (
        (!hWnd) ||
        (!::IsWindow(hWnd))
    )
        return;


    ::DestroyWindow(hWnd);
}

void Win32Edit_SetFocus(HWND hWnd, bool bFocus)
{
    if (
        (!hWnd) ||
        (!::IsWindow(hWnd))
    )
        return;

    if (bFocus)
    {
        ::SetFocus(hWnd);
    }
    else
    {
        ::SetFocus(g_GetMainHWnd());
    }
}

void Win32Edit_SetTextColor(HWND hWnd, unsigned int uColor)
{

}

bool Win32Edit_InsertString(HWND hWnd, const char* pString, int nLen)
{
    if (
        (!hWnd) ||
        (!::IsWindow(hWnd))
    )
        return 0;

    ::SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)(const char*)pString);

    return true;
}

void Win32Edit_SetText(HWND hWnd, const char* pszString)
{
    if (
        (!hWnd) ||
        (!::IsWindow(hWnd))
    )
        return;

    ::SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)"");
    ::SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)pszString);
}

int  Win32Edit_GetText(HWND hWnd, char* pBuffer, int nSize, bool bExcludeSpace)
{
    if (
        (!hWnd) ||
        (!::IsWindow(hWnd))
    )
        return 0;

    if (!pBuffer)
        return 0;

    int nMsgLength = ::SendMessage(hWnd, WM_GETTEXT, nSize, (LPARAM)pBuffer);

    if (bExcludeSpace)
    {
        if (nMsgLength == 0)
            return nMsgLength;

        while (nMsgLength > 0)
        {
            if (!isspace(pBuffer[nMsgLength - 1]))
                break;

            nMsgLength--;
        }

        pBuffer[nMsgLength] = '\0';

        char *pszPos = pBuffer;

        while (*pszPos)
        {
            if (!isspace(*pszPos))
                break;

            pszPos++;
        }

        if (pszPos != pBuffer)
        {
            char *pszDst = pBuffer;
            while (true)
            {
                *pszDst = *pszPos;
                if (!*pszPos)
                    break;

                pszPos++;
                pszDst++;
            }
            nMsgLength = pszDst - pBuffer;
        }
    }

    return nMsgLength;
}

int  Win32Edit_ShowWindow(HWND hWnd, BOOL bShow)
{
    if (
        (!hWnd) ||
        (!::IsWindow(hWnd))
    )
        return bShow;

    if (bShow)
    {
        ShowWindow(hWnd, SW_SHOW);
    }
    else
    {
        ShowWindow(hWnd, SW_HIDE);
    }

    return bShow;
}

#include "StdAfx.h"
#include "KOutputDebug.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

void g_OutputDebugInfoFmt(const char* szFormat, ...)
{
    char szLogBuffer[BUFFER_SIZE];
    va_list list;
    va_start(list, szFormat);
    vsprintf_s(szLogBuffer, szFormat, list);
    g_OutputDebugInfo(szLogBuffer, 0);
    va_end(list);
}

void g_OutputDebugInfo(const char* szInfo, size_t size)
{
    ASSERT(szInfo);
    HWND hWnd = ::FindWindow("#32770", "¥Û≈∆ Inside");
    if (hWnd)
    {
        HANDLE hMapFile;
        LPSTR pBuf;

        hMapFile = OpenFileMapping(
            FILE_MAP_WRITE,
            FALSE,
            "DebugConsole");

        if (hMapFile == NULL) 
            return;

        pBuf = (LPSTR)MapViewOfFile(hMapFile,
            FILE_MAP_WRITE,
            0,                    
            0,                    
            BUFFER_SIZE);                   

        if (pBuf == NULL) 
        {
            CloseHandle(hMapFile);
            return;
        }

        _snprintf_s(pBuf, BUFFER_SIZE, BUFFER_SIZE, "%s\r\n", szInfo);
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        ::PostMessage(hWnd, WM_EDIT_RECEIVE_STRING, 0, 0);
    }
}
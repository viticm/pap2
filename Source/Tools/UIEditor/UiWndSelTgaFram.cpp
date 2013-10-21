#include "Stdafx.h"
#include "UiWndSelTgaFram.h"
#include <GdiPlus.h>
#include <WinGDI.h>
#include "resource.h"
#include "UiEditor.h"
#include "ClientManage.h"
#include "Ui.h"
#include "KTga.h"
#include "UiTex.h"

namespace SELTGAFRAME
{
    using namespace Gdiplus;

    HWND               g_hPopUp = NULL;

    Graphics          *pGraphics   = NULL;
    Bitmap            *pBitMap    = NULL;
    Pen               *pPen       = NULL;
    RECT              *pRectShow = NULL;
    int                nRectNum = 0;
    int                nCurrentFram = -1;

    LRESULT CALLBACK WinPopUpProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        int                 nRetCode = FALSE;
        PAINTSTRUCT         ps;
        HDC                 hdc;
        RECT               RectShow;
        POINT              msPos;
        RECT               RectSource;
        int                i = 0;
	    switch (message)
        {
	    case WM_CLOSE: 
		    DestroyWindow(g_hPopUp);
		    break;
	    case WM_CREATE:
		    break;
	    case WM_KEYDOWN:
		    switch (wParam)
            {
		    case VK_ESCAPE:
                nCurrentFram = -1;
			    PostMessage(g_hPopUp, WM_CLOSE, 0, 0);			
                break;
            default:
                break;
		    }
		    break;
	    case WM_DESTROY:
            PostQuitMessage(0);
		    break;
        case WM_MOUSEMOVE:
            hdc = GetDC(hWnd);
            ValidateRect(hWnd, NULL);
            pGraphics = new Graphics(hdc);
            KG_PROCESS_ERROR(pGraphics);

            GetCursorPos(&msPos);
            RectShow.bottom = 0;
            RectShow.left = 0;
            RectShow.right = 0;
            RectShow.top = 0;

            for (i = 0; i < nRectNum; i++) 
            {
                RectSource.top = pRectShow[i].top;
                RectSource.left = pRectShow[i].left;
                RectSource.bottom = pRectShow[i].top + pRectShow[i].bottom;
                RectSource.right = pRectShow[i].left + pRectShow[i].right;
                if(PtInRect(&RectSource, msPos)) 
                {
                    RectShow.top = pRectShow[i].top;
                    RectShow.bottom = pRectShow[i].bottom;
                    RectShow.left = pRectShow[i].left;
                    RectShow.right = pRectShow[i].right;
                    nCurrentFram = i;
                    break;
                }
            }
            pGraphics->DrawImage(pBitMap, 0, 0);        
            pGraphics->DrawRectangle(pPen, RectShow.left, RectShow.top, RectShow.right, RectShow.bottom);
            
            ReleaseDC(hWnd, hdc);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            pGraphics = new Graphics(hdc);
            KG_PROCESS_ERROR(pGraphics);
            pGraphics->DrawImage(pBitMap, 0, 0);
            EndPaint(hWnd, &ps);
            break;
	    case WM_LBUTTONDOWN:
            GetCursorPos(&msPos);
            for (i = 0; i < nRectNum; i++) 
            {
                RectSource.top = pRectShow[i].top;
                RectSource.left = pRectShow[i].left;
                RectSource.bottom = pRectShow[i].top + pRectShow[i].bottom;
                RectSource.right = pRectShow[i].left + pRectShow[i].right;
                if(PtInRect(&RectSource, msPos)) 
                {
                    
                    nCurrentFram = i;
                    break;
                }
                else
                {
                    nCurrentFram = -1;
                }
            }
	        PostMessage(g_hPopUp, WM_CLOSE, 0, 0);	
		    break;			
	    }
    Exit0:
        if (pGraphics != NULL) 
        {
            delete pGraphics;
            pGraphics = NULL;
        }
	    return DefWindowProc(hWnd, message, wParam, lParam); 	
    }

    int InitPopUpWindow(HINSTANCE hInstance, int nCmdShow, HWND g_hWnd, int nWidth, int nHeight)
    {
        int nResult  = -1;
        int nRetCode = false;
	    WNDCLASSEX wc; 

        wc.cbSize        = sizeof(WNDCLASSEX);
	    wc.style         = NULL;
	    wc.lpfnWndProc   = (WNDPROC)WinPopUpProc;
	    wc.cbClsExtra    = 0;
	    wc.cbWndExtra    = 0;
	    wc.hInstance     = hInstance;
	    wc.hIcon         = NULL;
	    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); 
	    wc.lpszMenuName  = NULL;
	    wc.lpszClassName = TEXT("SelTgaFram");
        wc.hIconSm       = NULL;

	    nRetCode = (int)RegisterClassEx(&wc);
        KG_PROCESS_ERROR(nRetCode);

	    g_hPopUp = CreateWindowEx(
            WS_EX_APPWINDOW | WS_EX_TOPMOST,
		    TEXT("SelTgaFram"),
            TEXT("SelTgaFram"),
		    WS_POPUPWINDOW,
            0,0, 1024, 768,        //nWidth, nHeight,
		    g_hWnd, NULL, hInstance, NULL
        );


        KG_PROCESS_ERROR(g_hPopUp);
	    ShowWindow(g_hPopUp, nCmdShow);
	    UpdateWindow(g_hPopUp);

        nResult = true;
    Exit0:
	    return nResult;	
    }

    int OnSelTgaFram(const char *szUiTexFileName, HWND hWnd, int *pFrameWidth, int *pFrameHeight)
    {
        int nResult  = false;
        int nRetCode = false;

        MSG                 msg;
	    GdiplusStartupInput gdiplusStartupInput;
	    ULONG_PTR           gdiplusToken;

        KTga32             TgaShow;
        BITMAPINFO         bmpInfo;
        FILE              *fp = NULL;
        char               szTgaFileName[MAX_PATH];
        int                i      = 0;
        int                nFrame = 0;
        int                nWidth = 0;
        int                nHeight = 0;
        char               *szClip;

        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        UITEXFILEHEADER headinfo;
        UITEXFRAMEDATASTRUCTURE FrameInfo;
        size_t dwRead = 0;
        fp =fopen(szUiTexFileName, "rb");
        KG_PROCESS_ERROR(fp);
        dwRead = fread(&headinfo, sizeof(UITEXFILEHEADER), 1, fp);
        KG_PROCESS_ERROR(dwRead == 1);
        strncpy(szTgaFileName, headinfo.szTgaFileName, sizeof(szTgaFileName));
        nWidth = headinfo.nWidth;
        nHeight = headinfo.nHeight;
        nRectNum = headinfo.nFrameCount;

        pRectShow = new RECT[nRectNum];
        KG_PROCESS_ERROR(pRectShow);

        for (i = 0; i < nRectNum; i++) 
        {
            dwRead = fread(&FrameInfo, sizeof(UITEXFRAMEDATASTRUCTURE), 1, fp);
            KG_PROCESS_ERROR(dwRead == 1);
            pRectShow[i].left = FrameInfo.nLeft;
            pRectShow[i].top  = FrameInfo.nTop;
            pRectShow[i].right = FrameInfo.nWidth - 1;
            pRectShow[i].bottom = FrameInfo.nHeight - 1;   
        }
        fclose(fp);
        fp = NULL;

        szClip = (char *)strrchr(szUiTexFileName, '\\');
        KG_PROCESS_ERROR(szClip);
        strcpy(szClip + 1, szTgaFileName);
        strcpy(szTgaFileName, szUiTexFileName);  
        nRetCode = TgaShow.CreateFromFile(szTgaFileName, TRUE);
        KG_PROCESS_ERROR(nRetCode);
        nRetCode = TgaShow.ReadAllDataToSelfMemery();
        KG_PROCESS_ERROR(nRetCode);


        ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
        bmpInfo.bmiHeader.biBitCount = 32;
        bmpInfo.bmiHeader.biClrImportant = 0;
        bmpInfo.bmiHeader.biClrUsed = 0;
        bmpInfo.bmiHeader.biCompression = 0;
        bmpInfo.bmiHeader.biHeight = 0 - TgaShow.GetHeight();
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biSizeImage = TgaShow.GetHeight() * TgaShow.GetWidth() *sizeof(DWORD);
        bmpInfo.bmiHeader.biWidth = TgaShow.GetWidth();
        bmpInfo.bmiHeader.biXPelsPerMeter = 2834;
        bmpInfo.bmiHeader.biYPelsPerMeter = 2834;
        bmpInfo.bmiColors[0].rgbBlue = 255;
        bmpInfo.bmiColors[0].rgbGreen = 255;
        bmpInfo.bmiColors[0].rgbRed = 255;
        bmpInfo.bmiColors[0].rgbReserved = 0;

        pPen = new Pen(Color(255, 0, 0), 3);
        pBitMap = Bitmap::FromBITMAPINFO(&bmpInfo, TgaShow.GetDataBufferPointer());
        KG_PROCESS_ERROR(pBitMap);

        nRetCode = InitPopUpWindow((HINSTANCE)g_hPluginInstance, SW_SHOW, hWnd, nWidth, nHeight);
        KG_PROCESS_ERROR(nRetCode);

	    while (GetMessage(&msg, NULL, 0, 0))
        {
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
	    }
        	
	    nResult = nCurrentFram;
        if (nCurrentFram >= 0 && nCurrentFram < nRectNum) 
        {
            *pFrameHeight = pRectShow[nCurrentFram].bottom;
            *pFrameWidth  = pRectShow[nCurrentFram].right;
        }
        else
        {
            *pFrameHeight = 0;
            *pFrameWidth  = 0;
        }
    Exit0:    

        if (fp != NULL) 
        {
            fclose(fp);
            fp = NULL;
        }
        if (pBitMap != NULL) 
        {
            delete pBitMap;
            pBitMap = NULL;
        }
        if (pPen != NULL) 
        {
            delete pPen;
            pPen = NULL;
        }
        if (pRectShow != NULL) 
        {
            delete[] pRectShow;
            pRectShow = NULL;
        }

        TgaShow.Release();
        UnregisterClass(TEXT("SelTgaFram"), (HINSTANCE)g_hPluginInstance);
        GdiplusShutdown(gdiplusToken);
        return nResult;

    }

}



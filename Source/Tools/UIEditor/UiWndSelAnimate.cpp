#include "Stdafx.h"
#include "UiWndSelAnimate.h"
#include "resource.h"
#include "UiEditor.h"
#include "ClientManage.h"
#include "Ui.h"
#include "KTga.h"
#include "UiTex.h"

using namespace Gdiplus;

namespace SELANIMATE
{

    HWND               g_hPopUp_Sel_Animate = NULL;

    static int         g_nHasRunYet_Sel_Animate = 0;
    Graphics          *g_pGraphics_Sel_Animate   = NULL;
    Bitmap            *g_pBitMap_Sel_Animate    = NULL;
    Pen               *g_pPen_Sel_Animate       = NULL;
    RECT              *g_pRectShow_Sel_Animate = NULL;
    int                g_nRectNum_Sel_Animate = 0;
    int                g_nCurrentFram_Sel_Animate = -1;

    struct ANIMATEDATA
    {
        int     nFrame;
        int     nFrameNow;
        int     *pnFrameData;
    };

    ANIMATEDATA *g_pAnimate_Sel_Animate = NULL;
    int g_nAnimateNum_Sel_Animate = 0;



    LRESULT CALLBACK g_WinPopUpProc_Sel_Animate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        int                 nRetCode = FALSE;
        PAINTSTRUCT         ps;
        HDC                 hdc;
        RECT               RectShow;
        POINT              msPos;
        RECT               RectSource;
        int                i = 0;
        int                j = 0;
        int                 nNeedToRestoryGroup = -1;
	    switch (message)
        {
	    case WM_CLOSE: 
		    DestroyWindow(g_hPopUp_Sel_Animate);
		    break;
	    case WM_CREATE:
		    break;

        case WM_TIMER:
            hdc = GetDC(hWnd);
            ValidateRect(hWnd, NULL);
            g_pGraphics_Sel_Animate = new Graphics(hdc);
            KG_PROCESS_ERROR(g_pGraphics_Sel_Animate);

            GetCursorPos(&msPos);
            ::GetWindowRect(hWnd, &RectShow);
            msPos.x -= RectShow.left;
            msPos.y -= RectShow.top;

            RectShow.bottom = 0;
            RectShow.left = 0;
            RectShow.right = 0;
            RectShow.top = 0;

            KG_PROCESS_ERROR(g_pAnimate_Sel_Animate);

            for (i = 0; i < g_nAnimateNum_Sel_Animate; i++) 
            {
                int nShowFrame = g_pAnimate_Sel_Animate[i].pnFrameData[0];

				if (nShowFrame < 0 || nShowFrame >= g_nRectNum_Sel_Animate)
				{
					RectSource.top = 0;
					RectSource.left = 0;
					RectSource.bottom = 0;
					RectSource.right = 0;
				}
				else
				{
					RectSource.top = g_pRectShow_Sel_Animate[nShowFrame].top;
					RectSource.left = g_pRectShow_Sel_Animate[nShowFrame].left;
					RectSource.bottom = RectSource.top + g_pRectShow_Sel_Animate[nShowFrame].bottom;
					RectSource.right = RectSource.left + g_pRectShow_Sel_Animate[nShowFrame].right;
				}

                if(PtInRect(&RectSource, msPos)) 
                {
					if (nShowFrame < 0 || nShowFrame >= g_nRectNum_Sel_Animate)
					{
						RectShow.top = 0;
						RectShow.left = 0;
						RectShow.bottom = 0;
						RectShow.right = 0;
					}
					else
					{
						RectShow.top = g_pRectShow_Sel_Animate[nShowFrame].top;
						RectShow.left = g_pRectShow_Sel_Animate[nShowFrame].left;
						RectShow.bottom = g_pRectShow_Sel_Animate[nShowFrame].bottom;;
						RectShow.right = g_pRectShow_Sel_Animate[nShowFrame].right;
					}

                    if(g_nCurrentFram_Sel_Animate != i)
                    {
                        nNeedToRestoryGroup = g_nCurrentFram_Sel_Animate;
                        g_nCurrentFram_Sel_Animate = i;
                    }                   
                    break;
                }
            }  

            for (i = 0; i < g_nAnimateNum_Sel_Animate; i++) 
            {
                KG_PROCESS_ERROR(g_pAnimate_Sel_Animate[i].pnFrameData);
                if(i != nNeedToRestoryGroup)
                {
                    if(g_pAnimate_Sel_Animate[i].nFrame <= 1)
                        continue;
                }
                j = g_pAnimate_Sel_Animate[i].nFrameNow;
                int nShowFrame = g_pAnimate_Sel_Animate[i].pnFrameData[0];
                int nTempFrame = g_pAnimate_Sel_Animate[i].pnFrameData[j];
                RectF rcDrawRect;
				if (nShowFrame < 0 || nShowFrame >= g_nRectNum_Sel_Animate)
				{
					rcDrawRect.X = 0;
					rcDrawRect.Y = 0;
					rcDrawRect.Width = 0;
					rcDrawRect.Height = 0;
				}
				else
				{
					rcDrawRect.X = (REAL)g_pRectShow_Sel_Animate[nShowFrame].left;
					rcDrawRect.Y = (REAL)g_pRectShow_Sel_Animate[nShowFrame].top;
					rcDrawRect.Width = (REAL)g_pRectShow_Sel_Animate[nShowFrame].right;
					rcDrawRect.Height = (REAL)g_pRectShow_Sel_Animate[nShowFrame].bottom;
				}
                if(i == nNeedToRestoryGroup)
                {
                    g_pPen_Sel_Animate->SetColor(Color(0, 0, 0));
					if (nShowFrame < 0 || nShowFrame >= g_nRectNum_Sel_Animate)
					{
						g_pGraphics_Sel_Animate->DrawRectangle(
						g_pPen_Sel_Animate, 
						0, 
						0, 
						0, 
						0
						);
					}
					else
					{
						g_pGraphics_Sel_Animate->DrawRectangle(
							g_pPen_Sel_Animate, 
							g_pRectShow_Sel_Animate[nShowFrame].left, 
							g_pRectShow_Sel_Animate[nShowFrame].top, 
							g_pRectShow_Sel_Animate[nShowFrame].right, 
							g_pRectShow_Sel_Animate[nShowFrame].bottom
						);
					}
                    g_pPen_Sel_Animate->SetColor(Color(255, 0, 0));
                }
				if (nTempFrame < 0 || nTempFrame >= g_nRectNum_Sel_Animate)
				{
					g_pGraphics_Sel_Animate->DrawImage(g_pBitMap_Sel_Animate, rcDrawRect,
						(REAL)0, (REAL)0, 
						(REAL)0, (REAL)0,
						UnitPixel);
				}
				else
				{
					g_pGraphics_Sel_Animate->DrawImage(g_pBitMap_Sel_Animate, rcDrawRect,
						(REAL)g_pRectShow_Sel_Animate[nTempFrame].left, (REAL)g_pRectShow_Sel_Animate[nTempFrame].top, 
						(REAL)g_pRectShow_Sel_Animate[nTempFrame].right, (REAL)g_pRectShow_Sel_Animate[nTempFrame].bottom,
						UnitPixel);
				}
                g_pAnimate_Sel_Animate[i].nFrameNow++;
                if (g_pAnimate_Sel_Animate[i].nFrameNow >= g_pAnimate_Sel_Animate[i].nFrame) 
                {
                    g_pAnimate_Sel_Animate[i].nFrameNow = 0;
                }
                
            }
            g_pGraphics_Sel_Animate->DrawRectangle(g_pPen_Sel_Animate, 
                RectShow.left, RectShow.top, RectShow.right, RectShow.bottom
            );

            break;
	    case WM_KEYDOWN:
		    switch (wParam)
            {
		    case VK_ESCAPE:
                g_nCurrentFram_Sel_Animate = -1;
			    PostMessage(g_hPopUp_Sel_Animate, WM_CLOSE, 0, 0);			
                break;
            default:
                break;
		    }
		    break;
	    case WM_DESTROY:
            PostQuitMessage(0);
		    break;
        case WM_MOUSEMOVE:
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            g_pGraphics_Sel_Animate = new Graphics(hdc);
            KG_PROCESS_ERROR(g_pGraphics_Sel_Animate);

            KG_PROCESS_ERROR(g_pAnimate_Sel_Animate);

            for (i = 0; i < g_nAnimateNum_Sel_Animate; i++) 
            {
                KG_PROCESS_ERROR(g_pAnimate_Sel_Animate[i].pnFrameData);
                j = g_pAnimate_Sel_Animate[i].nFrameNow;
                int nShowFrame = g_pAnimate_Sel_Animate[i].pnFrameData[0];
                int nTempFrame = g_pAnimate_Sel_Animate[i].pnFrameData[j];
                RectF rcDrawRect;
				if (nShowFrame < 0 || nShowFrame >= g_nRectNum_Sel_Animate)
				{
					rcDrawRect.X = 0;
					rcDrawRect.Y = 0;
					rcDrawRect.Width = 0;
					rcDrawRect.Height = 0;
				}
				else
				{
					rcDrawRect.X = (REAL)g_pRectShow_Sel_Animate[nShowFrame].left;
					rcDrawRect.Y = (REAL)g_pRectShow_Sel_Animate[nShowFrame].top;
					rcDrawRect.Width = (REAL)g_pRectShow_Sel_Animate[nShowFrame].right;
					rcDrawRect.Height = (REAL)g_pRectShow_Sel_Animate[nShowFrame].bottom;
				}
				if (nTempFrame < 0 || nTempFrame >= g_nRectNum_Sel_Animate)
				{
					g_pGraphics_Sel_Animate->DrawImage(g_pBitMap_Sel_Animate, rcDrawRect,
						(REAL)0, (REAL)0, 
						(REAL)0, (REAL)0,
						UnitPixel);
				}
				else
				{
					g_pGraphics_Sel_Animate->DrawImage(g_pBitMap_Sel_Animate, rcDrawRect,
						(REAL)g_pRectShow_Sel_Animate[nTempFrame].left, (REAL)g_pRectShow_Sel_Animate[nTempFrame].top, 
						(REAL)g_pRectShow_Sel_Animate[nTempFrame].right, (REAL)g_pRectShow_Sel_Animate[nTempFrame].bottom,
						UnitPixel);
				}
                g_pAnimate_Sel_Animate[i].nFrameNow++;
                if (g_pAnimate_Sel_Animate[i].nFrameNow >= g_pAnimate_Sel_Animate[i].nFrame) 
                {
                    g_pAnimate_Sel_Animate[i].nFrameNow = 0;
                }
                
            }

            EndPaint(hWnd, &ps);
            break;
	    case WM_LBUTTONDOWN:

            GetCursorPos(&msPos);
            ::GetWindowRect(hWnd, &RectShow);
            msPos.x -= RectShow.left;
            msPos.y -= RectShow.top;

            KG_PROCESS_ERROR(g_pAnimate_Sel_Animate);

            for (i = 0; i < g_nAnimateNum_Sel_Animate; i++) 
            {
                int nShowFrame = g_pAnimate_Sel_Animate[i].pnFrameData[0];
				if (nShowFrame < 0 || nShowFrame >= g_nRectNum_Sel_Animate)
				{
					RectSource.top = 0;
					RectSource.left = 0;
					RectSource.bottom = 0;
					RectSource.right = 0;
				}
				else
				{
					RectSource.top = (LONG)(g_pRectShow_Sel_Animate[nShowFrame].top);
					RectSource.left = (LONG)(g_pRectShow_Sel_Animate[nShowFrame].left);
					RectSource.bottom = (LONG)(RectSource.top + g_pRectShow_Sel_Animate[nShowFrame].bottom);
					RectSource.right = (LONG)(RectSource.left + g_pRectShow_Sel_Animate[nShowFrame].right);
				}
                if(PtInRect(&RectSource, msPos)) 
                {
                    g_nCurrentFram_Sel_Animate = i;
                    break;
                }
                else
                {
                    g_nCurrentFram_Sel_Animate = -1;
                }
            } 
	        PostMessage(g_hPopUp_Sel_Animate, WM_CLOSE, 0, 0);	
		    break;			
	        }
    Exit0:
        if (g_pGraphics_Sel_Animate != NULL) 
        {
            delete g_pGraphics_Sel_Animate;
            g_pGraphics_Sel_Animate = NULL;
        }
	    return DefWindowProc(hWnd, message, wParam, lParam); 	
    }

    int g_InitPopUpWindow_Sel_Animate(HINSTANCE hInstance, int nCmdShow, HWND g_hWnd, int nWidth, int nHeight)
    {
        int nResult  = false;
        int nRetCode = false;
	    WNDCLASSEX wc; 

        wc.cbSize        = sizeof(WNDCLASSEX);
	    wc.style         = NULL;
	    wc.lpfnWndProc   = (WNDPROC)g_WinPopUpProc_Sel_Animate;
	    wc.cbClsExtra    = 0;
	    wc.cbWndExtra    = 0;
	    wc.hInstance     = hInstance;
	    wc.hIcon         = NULL;
	    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); 
	    wc.lpszMenuName  = NULL;
	    wc.lpszClassName = TEXT("SelAnimateFram");
        wc.hIconSm       = NULL;

	    nRetCode = (int)RegisterClassEx(&wc);
        KG_PROCESS_ERROR(nRetCode);

	    g_hPopUp_Sel_Animate = CreateWindowEx(
            WS_EX_APPWINDOW | WS_EX_TOPMOST,
		    TEXT("SelAnimateFram"),
            TEXT("SelAnimateFram"),
		    WS_POPUPWINDOW,
            0,0, 1024, 768,
		    g_hWnd, NULL, hInstance, NULL
        );


        KG_PROCESS_ERROR(g_hPopUp_Sel_Animate);
	    ShowWindow(g_hPopUp_Sel_Animate, nCmdShow);
	    UpdateWindow(g_hPopUp_Sel_Animate);

        nResult = true;
    Exit0:
	    return nResult;	
    }

    int OnSelAnimate(const char *szUiTexFileName, HWND hWnd)
    {
       if (g_nHasRunYet_Sel_Animate)
       {
           return -1;
       };
        g_nHasRunYet_Sel_Animate = TRUE;

        int nResult  = -1;
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
        char               *szClip = NULL;
        UINT_PTR           timer = 0;


        int nFrameCount = 0;
        int nFrameTemp  = 0;
        int j = 0;

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
        g_nRectNum_Sel_Animate = headinfo.nFrameCount;
        g_nAnimateNum_Sel_Animate =headinfo.nAnimateCount;

        g_pRectShow_Sel_Animate = new RECT[g_nRectNum_Sel_Animate];
        KG_PROCESS_ERROR(g_pRectShow_Sel_Animate);

        for (i = 0; i < g_nRectNum_Sel_Animate; i++) 
        {
            dwRead = fread(&FrameInfo, sizeof(UITEXFRAMEDATASTRUCTURE), 1, fp);
            KG_PROCESS_ERROR(dwRead == 1);
            g_pRectShow_Sel_Animate[i].left = FrameInfo.nLeft;
            g_pRectShow_Sel_Animate[i].top  = FrameInfo.nTop;
            g_pRectShow_Sel_Animate[i].right = FrameInfo.nWidth - 1;
            g_pRectShow_Sel_Animate[i].bottom = FrameInfo.nHeight - 1;   
        }

        g_pAnimate_Sel_Animate = new ANIMATEDATA[g_nAnimateNum_Sel_Animate];
        KG_PROCESS_ERROR(g_pAnimate_Sel_Animate);

        for (i = 0; i < g_nAnimateNum_Sel_Animate; i++) 
        {
            dwRead = fread(&nFrameCount, sizeof(int), 1, fp);
            KG_PROCESS_ERROR(dwRead == 1);

            g_pAnimate_Sel_Animate[i].nFrame = nFrameCount;
            g_pAnimate_Sel_Animate[i].nFrameNow = 0;
            g_pAnimate_Sel_Animate[i].pnFrameData = new int[nFrameCount];
            KG_PROCESS_ERROR(g_pAnimate_Sel_Animate[i].pnFrameData);

            for (j = 0; j < nFrameCount; j++) 
            {
                dwRead = fread(&nFrameTemp, sizeof(int), 1, fp);
                KG_PROCESS_ERROR(dwRead == 1);
                g_pAnimate_Sel_Animate[i].pnFrameData[j] = nFrameTemp;
            }

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

        g_pPen_Sel_Animate = new Pen(Color(255, 0, 0), 3);
        g_pBitMap_Sel_Animate = Bitmap::FromBITMAPINFO(&bmpInfo, TgaShow.GetDataBufferPointer());
        KG_PROCESS_ERROR(g_pBitMap_Sel_Animate);

        nRetCode = g_InitPopUpWindow_Sel_Animate((HINSTANCE)g_hPluginInstance, SW_SHOW, hWnd, nWidth, nHeight);
        KG_PROCESS_ERROR(nRetCode);


        timer = SetTimer(g_hPopUp_Sel_Animate, 1, 200, NULL);
        KG_PROCESS_ERROR(timer);

	    while (GetMessage(&msg, NULL, 0, 0))
        {
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
	    }
        	

	    nResult = g_nCurrentFram_Sel_Animate;
        g_nCurrentFram_Sel_Animate = -1;
    Exit0:    

        if (timer)
        {
            KillTimer(g_hPopUp_Sel_Animate, timer);
            timer = 0;
        }

        if (g_pAnimate_Sel_Animate) 
        {
            for (i = 0; i < g_nAnimateNum_Sel_Animate; i++) 
            {
                if (g_pAnimate_Sel_Animate[i].pnFrameData) 
                {
                    delete[] g_pAnimate_Sel_Animate[i].pnFrameData;
                    g_pAnimate_Sel_Animate[i].pnFrameData = NULL;
                }
            }

            delete[] g_pAnimate_Sel_Animate;
            g_pAnimate_Sel_Animate = NULL;
        }

        if (fp != NULL) 
        {
            fclose(fp);
            fp = NULL;
        }
        if (g_pBitMap_Sel_Animate != NULL) 
        {
            delete g_pBitMap_Sel_Animate;
            g_pBitMap_Sel_Animate = NULL;
        }
        if (g_pPen_Sel_Animate != NULL) 
        {
            delete g_pPen_Sel_Animate;
            g_pPen_Sel_Animate = NULL;
        }
        if (g_pRectShow_Sel_Animate != NULL) 
        {
            delete[] g_pRectShow_Sel_Animate;
            g_pRectShow_Sel_Animate = NULL;
        }

        TgaShow.Release();
        UnregisterClass(TEXT("SelAnimateFram"), (HINSTANCE)g_hPluginInstance);
        GdiplusShutdown(gdiplusToken);
        
        g_nHasRunYet_Sel_Animate = FALSE;

        return nResult;

    }

}


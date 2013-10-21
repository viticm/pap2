#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <Windows.h>
#include <gdiplus.h>
#include <GdiplusInit.h>
#include <WindowsX.h>
#include <commdlg.h>
#include <atlconv.h>
#include <math.h>
#include "resource.h"
#include "KOrderEditor.h"
#include "Engine.h"
#include "KGPublic.h"
using namespace Gdiplus;
using namespace std;


int WINAPI  WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow
)
{
    BOOL                bRetCode        = false;
    KOrderEditor*       pApp            = KOrderEditor::GetInstance();
	GdiplusStartupInput gdiPlusStartupInput;
	ULONG_PTR           gdiPlusToken;

    GdiplusStartup(&gdiPlusToken, &gdiPlusStartupInput, NULL);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(szCmdLine);

    bRetCode = pApp->Init(hInstance, nCmdShow);
    KG_PROCESS_ERROR(bRetCode);

    pApp->Run();

    pApp->UnInit();

Exit0:
	GdiplusShutdown(gdiPlusToken);
    return 0;
}



KOrderEditor::KOrderEditor()
{
    m_hInstance         = NULL;
    m_hWnd              = NULL;
}

BOOL KOrderEditor::Init(HINSTANCE hInstance, int nCmdShow)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;


    m_hInstance = hInstance;

    m_nMaxWidth  = GetSystemMetrics(SM_CXSCREEN); //此函数返回屏幕宽度
	m_nMaxHeight = GetSystemMetrics(SM_CYSCREEN); //此函数返回屏幕高度

    KG_PROCESS_ERROR(m_nMaxWidth >= WIN_MIN_W);
    KG_PROCESS_ERROR(m_nMaxHeight >= WIN_MIN_H);
    
    m_nGridStartX = GRID_WIDTH;
    m_nGridStartY = TITLE_HEIGHT;

    m_nGridXCount = (m_nMaxWidth  - GRID_WIDTH - COMMENT_WIDTH) / GRID_WIDTH;
    m_nGridYCount = (m_nMaxHeight - GRID_HEIGHT - PROMPT_HEIGHT) / GRID_HEIGHT;

    m_nOrgX = m_nGridStartX + GRID_WIDTH * (m_nGridXCount / 2); 
    m_nOrgY = m_nGridStartY + GRID_HEIGHT * (m_nGridYCount / 2);

    m_nMouseX = 0;
    m_nMouseY = 0;

    m_szFileName[0] = '\0';

    m_bViewMode = false;

    bRetCode = InitWindow(nCmdShow);
    KG_PROCESS_ERROR(bRetCode);


    bResult = true;
Exit0:
    return bResult;
}

void KOrderEditor::UnInit()
{
}

KOrderEditor* KOrderEditor::GetInstance()
{
    static KOrderEditor s_App;
    return &s_App;
}

void KOrderEditor::Run()
{
    HACCEL  hAccTable   = NULL;
    MSG     msg;

    hAccTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

    while (GetMessage(&msg, NULL, 0, 0))
    {
		if (!TranslateAccelerator(m_hWnd, hAccTable, &msg)) 
        {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
    }
}

BOOL KOrderEditor::InitWindow(int nCmdShow)
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
	WNDCLASSEX  wcex;
    ATOM        WinClassID          = 0;
    HWND        hWnd                = NULL;
    TCHAR       szTitle[32]         = __TEXT("Npc队列编辑器");		// 标题栏文本
    TCHAR       szWindowClass[32]   = __TEXT("KOrderEditor");	    // 主窗口类名

	wcex.cbSize         = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInstance;
	wcex.hIcon			= NULL; // LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_KLUAEDITOR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= CreateSolidBrush (RGB(0,0,0)); 
	wcex.lpszMenuName	= NULL; // MAKEINTRESOURCE(IDC_KLUAEDITOR);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL; // LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	WinClassID = RegisterClassEx(&wcex);
    KG_PROCESS_ERROR(WinClassID);   

    hWnd = CreateWindow(
        szWindowClass, szTitle, 
        WS_POPUP | WS_MAXIMIZE,
        0, 0, 
        m_nMaxWidth, m_nMaxHeight, 
        NULL, NULL, 
        m_hInstance, NULL
    );
    KG_PROCESS_ERROR(hWnd);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   m_hWnd = hWnd;

    bResult = true;
Exit0:
    return bResult;
}

LRESULT CALLBACK KOrderEditor::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    BOOL                    bRetCode   = false;
	PAINTSTRUCT             ps;
	HDC                     hdc        = NULL;
    static KOrderEditor*    pThis      = KOrderEditor::GetInstance();

	switch (uMessage)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
        return pThis->OnCommond(uMessage, wParam, lParam);
        break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
        if (hdc)
        {
            pThis->OnPaint(hdc);
		    EndPaint(hWnd, &ps);
        }
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
    case WM_LBUTTONUP:
        pThis->OnLMouseUp(wParam, lParam);
        break;
    case WM_RBUTTONUP:
        pThis->OnRMouseUp(wParam, lParam);
        break;
    case WM_ERASEBKGND:
        break;
    case WM_MOUSEMOVE:
        pThis->OnMouseMove(wParam, lParam);
        break;
	default:
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}

	return 0;
}

LRESULT KOrderEditor::OnCommond(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    int                 nRetCode   = false;
	int                 nMenuId    = LOWORD(wParam);
    int                 nMenuEvent = HIWORD(wParam);

    switch (nMenuId)
    {
    case IDA_OPEN:
        {
        OPENFILENAME ofn;
        SetFileDlg(ofn);
        ofn.Flags  = OFN_READONLY;        
        nRetCode = GetOpenFileName(&ofn);
        KG_PROCESS_ERROR(nRetCode);
        Open();
        }
        break;
    case IDA_SAVE:
        if (m_szFileName[0] == '\0')
        {
            OPENFILENAME ofn;
            SetFileDlg(ofn);
            ofn.Flags = OFN_OVERWRITEPROMPT;        
            nRetCode  = GetSaveFileName(&ofn);
            KG_PROCESS_ERROR(nRetCode);
            InvalidateRect(m_hWnd, NULL, false);
        }
        Save();
        break;
    case IDA_SAVE_AS:
        {
        OPENFILENAME ofn;
        SetFileDlg(ofn);
        ofn.Flags = OFN_OVERWRITEPROMPT;        
        nRetCode  = GetSaveFileName(&ofn);
        KG_PROCESS_ERROR(nRetCode);
        Save();
        InvalidateRect(m_hWnd, NULL, false);
        }
        break;
    case IDA_SWITCH:
        m_bViewMode = !m_bViewMode;
        InvalidateRect(m_hWnd, NULL, false);
        break;
    case IDA_DELETE_ALL:
        if (m_bViewMode)
        {
            m_ViewList.clear();
        }
        else
        {
            m_OrderList.clear();
        }
        InvalidateRect(m_hWnd, NULL, false);
        break;
    default:
        break;
    }

Exit0:
    return 0;
}

LRESULT KOrderEditor::OnPaint(HDC hdc)
{
    HDC         hMemDC      = NULL;
    HBITMAP     hBitmap     = NULL;

    hMemDC = CreateCompatibleDC(hdc);
    KG_PROCESS_ERROR(hMemDC);

    hBitmap = CreateCompatibleBitmap(hdc, m_nMaxWidth, m_nMaxHeight);
    KG_PROCESS_ERROR(hBitmap);

    SelectObject(hMemDC, hBitmap);

    DrawItems(hMemDC);

    BitBlt(
        hdc, 0, 0, m_nMaxWidth, m_nMaxHeight, 
        hMemDC, 0, 0, SRCCOPY
    );

Exit0:

    if (hBitmap)
    {
        DeleteObject(hBitmap);
        hBitmap = NULL;
    }

    if (hMemDC)
    {
        DeleteDC(hMemDC);
        hMemDC = NULL;
    }
    return 0;
}

void KOrderEditor::DrawItems(HDC hdc)
{
	Graphics    graphics(hdc);    

    // -------------- 画Title ---------------------------------------------------------------
    {
    LPWSTR      pszFileName = L"未命名";
    FontFamily  fontFamily(L"Arial"); // 
    Font        font(&fontFamily, 14, FontStyleBold, UnitPixel);
    WCHAR       szTitle[MAX_PATH];
    RectF       rectf(250.0f, 20.0f, 1000.0f, 40.0f);
    SolidBrush  solidBrush(Color(255, 200, 200, 0));
    USES_CONVERSION;

    if (m_szFileName[0])
    {
        pszFileName = A2W(m_szFileName);
    }
    _snwprintf(szTitle, sizeof(szTitle) / sizeof(szTitle[0]), L"NPC Order Editor(%s) : %s", m_bViewMode ? L"View" : L"Edit", pszFileName);
    graphics.DrawString(szTitle, -1, &font, rectf, NULL, &solidBrush);
    }

    // ------------- 画网格 ----------------------------------------------------------------
    {
    Pen pen(Color(255, 0, 0, 100));

    // 画横线
    for (int i = 0; i < m_nGridYCount + 1; ++i)
    {
        PointF Src((REAL)m_nGridStartX, (REAL)m_nGridStartY + GRID_HEIGHT * i);
        PointF Dst((REAL)m_nGridStartX + GRID_WIDTH * m_nGridXCount, (REAL)m_nGridStartY + GRID_HEIGHT * i);        
        graphics.DrawLine(&pen, Src, Dst);
    }

    // 画竖线
    for (int i = 0; i < m_nGridXCount + 1; ++i)
    {
        PointF Src((REAL)m_nGridStartX + GRID_WIDTH * i, (REAL)m_nGridStartY);
        PointF Dst((REAL)m_nGridStartX + GRID_WIDTH * i, (REAL)m_nGridStartY + GRID_HEIGHT * m_nGridYCount);        
        graphics.DrawLine(&pen, Src, Dst);
    }
    }

    // ------------ 画坐标,原点 ------------------------------------------------------------
    {
        Pen OrgPen(Color(255, 0, 128, 0), 2);
        // 原点
        graphics.DrawEllipse(&OrgPen, m_nOrgX - 3, m_nOrgY - 3, 7, 7);
        // X,Y 轴线
        Pen    XYPen(Color(255, 0, 0, 200));
        XYPen.SetEndCap(LineCapArrowAnchor);
        PointF XSrc((REAL)m_nGridStartX, (REAL)m_nOrgY);
        PointF XDst((REAL)m_nGridStartX + GRID_WIDTH * m_nGridXCount, (REAL)m_nOrgY);
        graphics.DrawLine(&XYPen, XSrc, XDst);
        PointF YSrc((REAL)m_nOrgX, (REAL)(m_nGridStartY + GRID_HEIGHT * m_nGridYCount));
        PointF YDst((REAL)m_nOrgX, (REAL)m_nGridStartY);
        graphics.DrawLine(&XYPen, YSrc, YDst);
    }

    // ------------- 画View信息 ------------------------------------------------------------
    {
        FontFamily  fontFamily(L"Arial"); // 
        Font        font(&fontFamily, 13, FontStyleRegular, UnitPixel);
        SolidBrush  solidBrush(Color(255, 128, 128, 128));
        for (KORDER_LIST::iterator it = m_ViewList.begin(); it != m_ViewList.end(); ++it)
        {
            REAL fXPos = m_nOrgX + it->fRadius * cos(it->fAngel) - 4;
            REAL fYPos = m_nOrgY - it->fRadius * sin(it->fAngel) - 5;

            WCHAR   szChild[2] = L"*";
            PointF  point(fXPos, fYPos);

            graphics.DrawString(szChild, -1, &font, point, NULL, &solidBrush);
        }
    }

    // ------------- 画Order信息 ------------------------------------------------------------
    {
        int         nIndex = 1;
        FontFamily  fontFamily(L"Arial"); // 
        Font        font(&fontFamily, 13, FontStyleRegular, UnitPixel);
        SolidBrush  solidBrush(Color(255, 255, 0, 0));
        for (KORDER_LIST::iterator it = m_OrderList.begin(); it != m_OrderList.end(); ++it)
        {
            REAL fXPos = m_nOrgX + it->fRadius * cos(it->fAngel);
            REAL fYPos = m_nOrgY - it->fRadius * sin(it->fAngel) - 7;

            WCHAR szChild[32];
            int nCount = _snwprintf(szChild, sizeof(szChild) / sizeof(szChild[0]), L"%d", nIndex);
            if (nCount != 1 && nCount != 2)
                break;

            if (nCount == 1)
                fXPos -= 4;
            else if (nCount == 2)
                fXPos -= 8;

            PointF      point(fXPos, fYPos);

            graphics.DrawString(szChild, -1, &font, point, NULL, &solidBrush);
            nIndex++;
        }
    }

    // ---------------- 画Comment -------------------------------------------------------------
    {
        FontFamily  fontFamily(L"宋体"); // 
        Font        font(&fontFamily, 12, FontStyleRegular, UnitPixel);
        SolidBrush  solidBrush(Color(255, 150, 150, 0));
        REAL        PosX     = (REAL)m_nMaxWidth - COMMENT_WIDTH + 20;
        REAL        PosY     = (REAL)100;
        REAL        CommentW = (REAL)150;
        REAL        CommentH = (REAL)600;
        RectF       rect(PosX, PosY, CommentW, CommentH);
        WCHAR       szComment[] = L"注释:\n\n"
                                  L"    1. \"狼王\"在原点,面朝正北方向.\n\n"
                                  L"    2. 编辑器里面的一个格子相当于游戏逻辑里面的一个CELL.\n\n"
                                  L"                 \n"
                                  L"        ,︵∧ ∧ \n"
                                  L"     ～(灬ミ^ō^ミ\n";

        graphics.DrawString(szComment, -1, &font, rect, NULL, &solidBrush);
    }

    // ----------------- 画帮助信息 -----------------------------------------------------------
    {
        FontFamily  fontFamily(L"Arial"); // 
        Font        font(&fontFamily, 12, FontStyleRegular, UnitPixel);
        SolidBrush  PromptBrush(Color(255, 150, 150, 0));
        REAL        PosX    = (REAL)100;
        REAL        PosY    = (REAL)m_nMaxHeight - PROMPT_HEIGHT + 50;
        REAL        PromptW = (REAL)600;
        REAL        PromptH = (REAL)140;
        RectF       rect(PosX + 10, PosY + 10, PromptW - 20, PromptH - 20);
        WCHAR       szPrompt[] = 
        L"\n"
        L"\t1. Ctrl + O 打开文件.\n"
        L"\t2. Ctrl + S 保存文件.\n"
        L"\t3. Alt  + S 文件另存为.\n"
        L"\t4. Esc 切换 View / Edit 模式.\n"
        L"\t5. Delete 删除所有节点.\n"
        L"\t6. 点击鼠标左键放置一个\"狼崽子\".\n"
        L"\t7. 点击鼠标右键删除该序号后面的所有\"狼崽子\"";

        graphics.DrawString(szPrompt, -1, &font, rect, NULL, &PromptBrush);    

        // 画提示框
	    Pen pen(Color(255, 0, 200, 0));
	    pen.SetDashStyle(DashStyleDashDotDot);

	    PointF RectPoints[6] = {
            PointF(PosX + 100, PosY), PointF(PosX, PosY), PointF(PosX, PosY + PromptH), 
            PointF(PosX + PromptW, PosY + PromptH), PointF(PosX + PromptW, PosY), PointF(PosX + 280, PosY)
        };

	    for(int i = 0; i < 5; i++)
        {
		    graphics.DrawLine(&pen, RectPoints[i], RectPoints[i + 1]);
        }

        WCHAR       szMouse[MAX_PATH];
        int         nOrgGridX = (m_nOrgX - m_nGridStartX) / GRID_WIDTH;
        int         nOrgGridY = (m_nOrgY - m_nGridStartY) / GRID_HEIGHT;
        PointF      point(RectPoints[0].X + 6, RectPoints[0].Y - 8);
        SolidBrush  TitleBrush(Color(255, 180, 0, 0));

        _snwprintf(
            szMouse, sizeof(szMouse) / sizeof(szMouse[0]), 
            L"当前鼠标位置 : (%d,  %d)", 
            m_nMouseX - m_nOrgX,  m_nOrgY - m_nMouseY
        );
        
        graphics.DrawString(szMouse, -1, &font, point, NULL, &TitleBrush);
    }
}

LRESULT KOrderEditor::OnLMouseUp(WPARAM wParam, LPARAM lParam)
{
    int     nRetCode    = false;
    int     nMouseX     = GET_X_LPARAM(lParam); 
    int     nMouseY     = GET_Y_LPARAM(lParam); 
    float   fRadius     = 0.0f;
    float   fAngel      = 0.0f;

    KG_PROCESS_ERROR(nMouseX >= m_nGridStartX && nMouseX < GRID_WIDTH * m_nGridXCount);
    KG_PROCESS_ERROR(nMouseY >= m_nGridStartY && nMouseY < GRID_HEIGHT * m_nGridYCount);

    fRadius = (float)sqrt((double)(nMouseX - m_nOrgX) * (nMouseX - m_nOrgX) + (nMouseY - m_nOrgY) * (nMouseY - m_nOrgY));
    if (fRadius >= 1.0f)
    {
        fAngel = atan2(-(float)(nMouseY - m_nOrgY), (float)(nMouseX - m_nOrgX));
    }
    else
    {
        fRadius = 0.0f;
        fAngel  = 0.0f;
    }

    KORDER_NODE OrderNode;

    OrderNode.fRadius = fRadius;
    OrderNode.fAngel  = fAngel;

    // 查找是否已经在队列中
    if (m_bViewMode)
    {
        nRetCode = (int)m_ViewList.size();
        KG_PROCESS_ERROR(nRetCode < 100);
        m_ViewList.push_back(OrderNode);
    }
    else
    {
        nRetCode = (int)m_OrderList.size();
        KG_PROCESS_ERROR(nRetCode < 100);
        m_OrderList.push_back(OrderNode);
    }

    // 更新界面
    InvalidateRect(m_hWnd, NULL, false);

Exit0:
    return 0;
}

LRESULT KOrderEditor::OnRMouseUp(WPARAM wParam, LPARAM lParam)
{
    KORDER_LIST::iterator it;
    int nMouseX = GET_X_LPARAM(lParam); 
    int nMouseY = GET_Y_LPARAM(lParam); 

    KG_PROCESS_ERROR(nMouseX >= m_nGridStartX && nMouseX < GRID_WIDTH * m_nGridXCount);
    KG_PROCESS_ERROR(nMouseY >= m_nGridStartY && nMouseY < GRID_HEIGHT * m_nGridYCount);

    // 查找是否已经在队列中
    if (m_bViewMode)
    {
        for (it = m_ViewList.begin(); it != m_ViewList.end(); ++it)
        {
            double fPosX     = m_nOrgX + it->fRadius * cos(it->fAngel);
            double fPosY     = m_nOrgY - it->fRadius * sin(it->fAngel);
            double fDistance = sqrt((fPosX - nMouseX) * (fPosX - nMouseX) + (fPosY - nMouseY) * (fPosY - nMouseY));
            if (fDistance < 6.0f)
            {
                m_ViewList.erase(it);
                break;
            }
        }
    }
    else
    {
        for (it = m_OrderList.begin(); it != m_OrderList.end(); ++it)
        {
            double fPosX     = m_nOrgX + it->fRadius * cos(it->fAngel);
            double fPosY     = m_nOrgY - it->fRadius * sin(it->fAngel);
            double fDistance = sqrt((fPosX - nMouseX) * (fPosX - nMouseX) + (fPosY - nMouseY) * (fPosY - nMouseY));
            if (fDistance < 10.0f)
            {
                m_OrderList.erase(it, m_OrderList.end());
                break;
            }
        }
    }

    // 更新界面
    InvalidateRect(m_hWnd, NULL, false);

Exit0:
    return 0;
}


LRESULT KOrderEditor::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    int nMouseX       = GET_X_LPARAM(lParam); 
    int nMouseY       = GET_Y_LPARAM(lParam); 

    KG_PROCESS_ERROR(nMouseX >= m_nGridStartX && nMouseX < GRID_WIDTH * m_nGridXCount);
    KG_PROCESS_ERROR(nMouseY >= m_nGridStartY && nMouseY < GRID_HEIGHT * m_nGridYCount);

    m_nMouseX = nMouseX;
    m_nMouseY = nMouseY;

    InvalidateRect(m_hWnd, NULL, false);

Exit0:
    return 0;
}

BOOL KOrderEditor::Open()
{
    BOOL      bResult   = false;
    int       nRetCode  = false;
    ITabFile* piTabFile = NULL;
    int       nMaxIndex = 0;

    piTabFile = g_OpenTabFile(m_szFileName);
    KG_PROCESS_ERROR(piTabFile);

    nMaxIndex = piTabFile->GetHeight() - 1;
    KG_PROCESS_ERROR(nMaxIndex >= 0);

    m_OrderList.clear();

    for (int i = 0; i < nMaxIndex; ++i)
    {
        int     nIndex  = 0;
        float   fRadius = 0.0f;
        float   fAngel  = 0.0f;
        float   fPosX   = 0;
        float   fPosY   = 0;
        KORDER_NODE OrderNode;

        nRetCode = piTabFile->GetInteger(2 + i, "Index", 0, &nIndex);
        KG_PROCESS_ERROR(nRetCode == 1);
        nRetCode = piTabFile->GetFloat(2 + i, "Radius", 0, &fRadius);
        KG_PROCESS_ERROR(nRetCode == 1);
        nRetCode = piTabFile->GetFloat(2 + i, "Angel", 0, &fAngel);  
        KG_PROCESS_ERROR(nRetCode == 1);

        KG_PROCESS_ERROR(nIndex == i + 1);

        fPosX = m_nOrgX + fRadius * cos(fAngel);
        fPosY = m_nOrgY - fRadius * sin(fAngel);

        KG_PROCESS_ERROR(fPosX >= m_nGridStartX && fPosX < GRID_WIDTH * m_nGridXCount);
        KG_PROCESS_ERROR(fPosY >= m_nGridStartY && fPosY < GRID_HEIGHT * m_nGridYCount);

        OrderNode.fRadius = fRadius;
        OrderNode.fAngel  = fAngel;

        m_OrderList.push_back(OrderNode);
    }

    InvalidateRect(m_hWnd, NULL, false);

    bResult = true;
Exit0:
    if (!bResult)
    {
        TCHAR szMsg[MAX_PATH];
        _sntprintf(szMsg, MAX_PATH, _T("载入文件失败: \"%s\"."), m_szFileName);
        MessageBox(m_hWnd, szMsg, "error", MB_OK);
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;   
}

BOOL KOrderEditor::Save()
{
    BOOL      bResult   = false;
    int       nRetCode  = false;
    ITabFile* piTabFile = NULL;
    int       nIndex    = 1;

    piTabFile = g_OpenTabFile(m_szFileName, false, true);
    KG_PROCESS_ERROR(piTabFile);

    piTabFile->Clear();

    piTabFile->WriteString(1, 1, "Index");
    piTabFile->WriteString(1, 2, "Radius");
    piTabFile->WriteString(1, 3, "Angel");

    for (KORDER_LIST::iterator it = m_OrderList.begin(); it != m_OrderList.end(); ++it)
    {
        piTabFile->WriteInteger(1 + nIndex, 1, nIndex);
        piTabFile->WriteFloat(1 + nIndex, 2, it->fRadius);
        piTabFile->WriteFloat(1 + nIndex, 3, it->fAngel);  
        nIndex++;
    }

    nRetCode = piTabFile->Save(m_szFileName);
    KG_PROCESS_ERROR(nRetCode);


    bResult = true;
Exit0:
    if (!bResult)
    {
        TCHAR szMsg[MAX_PATH];
        _sntprintf(szMsg, MAX_PATH, _T("无法打开文件写: \"%s\"."), m_szFileName);
        MessageBox(m_hWnd, szMsg, "error", MB_OK);
    }
    KG_COM_RELEASE(piTabFile);
    return bResult;
}

void KOrderEditor::SetFileDlg(OPENFILENAME& ofn)
{
    static TCHAR szFilter[] = _T("Tab File (*.tab)\0*.tab\0\0");
    static TCHAR szDir[MAX_PATH];

    GetCurrentDirectory(MAX_PATH, szDir);

    ofn.lStructSize       = sizeof(OPENFILENAME);    
    ofn.hwndOwner         = m_hWnd;
    ofn.hInstance         = NULL;
    ofn.lpstrFilter       = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = m_szFileName;
    ofn.nMaxFile          = MAX_PATH;
    ofn.lpstrFileTitle    = m_szFileName;
    ofn.nMaxFileTitle     = MAX_PATH;
    ofn.lpstrInitialDir   = szDir;
    ofn.lpstrTitle        = NULL;
    ofn.Flags             = 0;                 // Set in Open and Close functions
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = TEXT("TAB");
    ofn.lCustData         = 0L;
    ofn.lpfnHook          = NULL;
    ofn.lpTemplateName    = NULL;
}

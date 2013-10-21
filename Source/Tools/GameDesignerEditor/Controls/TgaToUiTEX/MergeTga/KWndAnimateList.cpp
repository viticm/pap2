////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KWndAnimateList.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-27 16:14:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MergeTga.h"
#include "resource.h"
#include "KWndAnimateList.h"
#include "UiTex.h"
#include "KAnimateManager.h"
#include "KTgaManager.h"
#include "KShow.h"
#include "KSaveManager.h"


////////////////////////////////////////////////////////////////////////////////


KWndAnimateList *KWndAnimateList::ms_pself = NULL;

KWndAnimateList::KWndAnimateList()
{
    m_hWndParent = NULL;
    m_hWnd = NULL;
    m_hWndAnimateList = NULL; 
    m_hWndFrameList = NULL;
    m_hWndDrawWindow = NULL;
    m_hInstance = NULL;
    m_nCurrentSelGroup = -1;
    m_nCurrentSelFrame = -1;  
    m_puTimer = NULL;
    m_nDrawType = DRAW_NOTHING;
    m_nCurrentDrawFrame = -1;
    m_nLButtonDownDoWhat = DO_NOTING;
}

KWndAnimateList::~KWndAnimateList()
{
}

int KWndAnimateList::Init(HINSTANCE hInst, HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_ERROR(!ms_pself);
    ms_pself = new KWndAnimateList;
    KG_PROCESS_ERROR(ms_pself);
    nRetCode = ms_pself->CreateDialogWnd(hInst, hWnd);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = ms_pself->SetTimer();
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        KWndAnimateList::Exit();
    return nResult;

}

int KWndAnimateList::Exit()
{
    if (ms_pself)
    {
        ms_pself->ClearTimer();
        ms_pself->DestroyDialogWnd();
    }
    KG_DELETE(ms_pself);
    return true;
}

KWndAnimateList &KWndAnimateList::GetSelf()
{
    ASSERT(ms_pself);
    return *ms_pself;
}

LRESULT CALLBACK KWndAnimateList::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRet = 0;
    KWndAnimateList *pWndNameList = (KWndAnimateList *)GetWindowLong(hWnd, GWL_USERDATA);
    KG_PROCESS_ERROR(pWndNameList);

    switch(message) 
    {
    case WM_NOTIFY:
        {
			LPNMHDR pNotify = (LPNMHDR)lParam;
			if (pNotify->hwndFrom == pWndNameList->GetAnimateListHWND())
				pWndNameList->OnAnimateListNotify(pNotify);
            if (pNotify->hwndFrom == pWndNameList->GetFrameListHWND())
                pWndNameList->OnFrameListNotify(pNotify);
        }
    	break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
            OnShowOrHideAnimateListWindow(pWndNameList->GetParentHWND());
        break;
    case WM_TIMER:
        pWndNameList->OnTimer();
        break;
    case WM_LBUTTONDOWN:
        pWndNameList->OnLButtonDown();
        break;
    default:
        break;
    }

Exit0:
    return lRet;
}

LRESULT CALLBACK KWndAnimateList::NewAnimateGroupProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
        ::SetDlgItemText(hDlg, IDC_NEW_ANIMATE_GROUP_NAME, _T(""));
		return TRUE;
	case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            if (!OnNewAnimateGroupNameSeted(hDlg))
                break;
        case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
            break;
        }
		break;
	}
	return FALSE;
}

int KWndAnimateList::CreateDialogWnd(HINSTANCE hInst, HWND hWnd)
{
    int nResult = false;

    ASSERT(!m_hWnd);
	m_hWnd = ::CreateDialog(hInst, MAKEINTRESOURCE(IDD_ANIMATE_LIST), hWnd, (DLGPROC)WndProc);

    KG_PROCESS_ERROR(m_hWnd);
    ::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
    ::ShowWindow(m_hWnd, SW_SHOW);
    
    {
	    m_hWndAnimateList = ::GetDlgItem(m_hWnd, IDC_ANIMATE_LIST);
        KG_PROCESS_ERROR(m_hWndAnimateList);

	    RECT	rc;
	    ::GetClientRect(m_hWndAnimateList, &rc);
	    LVCOLUMN	lvc;
	    TCHAR		szHeaderText[128];
	    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	    lvc.cx = rc.right - rc.left;
	    lvc.pszText = szHeaderText;
	    lvc.cchTextMax = sizeof(szHeaderText);
        _tcsncpy(szHeaderText, "动画组", sizeof(szHeaderText) / sizeof(TCHAR));
	    ListView_InsertColumn(m_hWndAnimateList, 0, &lvc);
    }

    {
	    m_hWndFrameList = ::GetDlgItem(m_hWnd, IDC_ANIMATE_FRAME_LIST);
        KG_PROCESS_ERROR(m_hWndFrameList);

	    RECT	rc;
	    ::GetClientRect(m_hWndFrameList, &rc);
	    LVCOLUMN	lvc;
	    TCHAR		szHeaderText[128];
	    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	    lvc.cx = rc.right - rc.left;
	    lvc.pszText = szHeaderText;
	    lvc.cchTextMax = sizeof(szHeaderText);
        _tcsncpy(szHeaderText, "动画帧", sizeof(szHeaderText) / sizeof(TCHAR));
	    ListView_InsertColumn(m_hWndFrameList, 0, &lvc);
    }

    m_hWndDrawWindow = ::GetDlgItem(m_hWnd, IDC_ANIMATE_SHOW_RECT);
    KG_PROCESS_ERROR(m_hWndDrawWindow);

    m_hWndParent = hWnd;
    m_hInstance = hInst;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_hWndFrameList)
        {
		    ::DestroyWindow(m_hWndFrameList);
		    m_hWndFrameList = NULL;
        }
        if (m_hWndAnimateList)
        {
		    ::DestroyWindow(m_hWndAnimateList);
		    m_hWndAnimateList = NULL;
        }
        if (m_hWnd)
	    {
		    ::DestroyWindow(m_hWnd);
		    m_hWnd = NULL;
	    } 
        m_hInstance = NULL;
        m_hWndParent = NULL;
        m_hWndDrawWindow = NULL;
    }    
    return nResult;
}

int KWndAnimateList::DestroyDialogWnd()
{
    if (m_hWndFrameList)
    {
		::DestroyWindow(m_hWndFrameList);
		m_hWndFrameList = NULL;
    }
    if (m_hWndAnimateList)
    {
        ::DestroyWindow(m_hWndAnimateList);
        m_hWndAnimateList = NULL;
    }
    if (m_hWnd)
    {
	    ::DestroyWindow(m_hWnd);
	    m_hWnd = NULL;
    }
    m_hInstance = NULL;  
    m_hWndParent = NULL;
    m_hWndDrawWindow = NULL;
    return true;
}

HWND KWndAnimateList::GetHWND()
{
    return m_hWnd;
}

HWND KWndAnimateList::GetAnimateListHWND()
{
    return m_hWndAnimateList;
}

HWND KWndAnimateList::GetFrameListHWND()
{
    return m_hWndFrameList;
}

HWND KWndAnimateList::GetParentHWND()
{
    return m_hWndParent;
}

int KWndAnimateList::OnAnimateListNotify(LPNMHDR pNotify)
{
    int nResult = false;
    KG_PROCESS_ERROR(pNotify);
    
    switch(pNotify->code) 
    {
	case LVN_ITEMACTIVATE:
	case NM_CLICK:
		{

			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
            OnSelGroup(pActive->iItem);
		}
    	break;
    case NM_RCLICK:
		{
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
            OnSelGroup(pActive->iItem);
            OnRButtonClickGroup(pActive->iItem);
		}
        break;
    case LVN_COLUMNCLICK:
        break;
    default:
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::OnFrameListNotify(LPNMHDR pNotify)
{
    int nResult = false;
    KG_PROCESS_ERROR(pNotify);
    
    switch(pNotify->code) 
    {
	case LVN_ITEMACTIVATE:
	case NM_CLICK:
		{
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
            OnSelFrame(pActive->iItem);
		}
    	break;
    case NM_RCLICK:
		{
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
            OnSelFrame(pActive->iItem);
            OnRButtonClickFrame(pActive->iItem);
		}
        break;
    case LVN_COLUMNCLICK:
        break;
    default:
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::OnSelGroup(int nGroup)
{
    KAnimateBlock *pBlock = KAnimateManager::GetSelf().Get(nGroup);
    if (pBlock)
    {
        pBlock->UpdataListView();
        m_nDrawType = DRAW_ANIMATE;
    }
    else
    {
        m_nDrawType = DRAW_NOTHING;
        ClearDraw();
    }
    m_nCurrentSelGroup = nGroup;
    m_nCurrentDrawFrame = 0;

    return true;
}

int KWndAnimateList::OnSelFrame(int nFrame)
{
    m_nCurrentSelFrame = nFrame;
    if (m_nCurrentSelFrame >= 0)
    {
        m_nDrawType = DRAW_FRAME;
        DrawFrame(m_nCurrentSelGroup, m_nCurrentSelFrame);
    }
    else
    {
        m_nDrawType = DRAW_NOTHING;
        ClearDraw();
    }

    return true;
}

int KWndAnimateList::Show()
{
    ::ShowWindow(m_hWnd, SW_SHOW);
    return true;
}

int KWndAnimateList::Hide()
{
    ::ShowWindow(m_hWnd, SW_HIDE);
    return true;
}

int KWndAnimateList::SetTimer()
{
    int nResult = false;
    KG_PROCESS_ERROR(m_hWnd);
    ASSERT(!m_puTimer);
    m_puTimer = ::SetTimer(m_hWnd, 1, 200, NULL);
    KG_PROCESS_ERROR(m_puTimer);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (m_hWnd && m_puTimer)
        {
            ::KillTimer(m_hWnd, m_puTimer);
            m_puTimer = NULL;
        }
    }
    return nResult;
}

int KWndAnimateList::ClearTimer()
{
    int nResult = false;

    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_puTimer);

    ::KillTimer(m_hWnd, m_puTimer);
    m_puTimer = NULL;

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::AppendAnimateGroup(LPCTSTR pcszGroupName)
{
    int nResult = false;
	LV_ITEM			lvi;

    KG_PROCESS_ERROR(pcszGroupName);
    KG_PROCESS_ERROR(m_hWndAnimateList);

	memset(&lvi, 0, sizeof(LV_ITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText		= _T("");
	lvi.cchTextMax  = 128;
	lvi.iItem       = ListView_GetItemCount(m_hWndAnimateList);
    ListView_InsertItem(m_hWndAnimateList, &lvi);
    ListView_SetItemText(m_hWndAnimateList, lvi.iItem, 0, (LPTSTR)pcszGroupName);  

    nResult = true;
Exit0:
    return true;
}

int KWndAnimateList::AppendAnimateFrame(LPCTSTR pcszFrameName)
{
    int nResult = false;
	LV_ITEM			lvi;

    KG_PROCESS_ERROR(pcszFrameName);
    KG_PROCESS_ERROR(m_hWndFrameList);

	memset(&lvi, 0, sizeof(LV_ITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText		= _T("");
	lvi.cchTextMax  = 128;
	lvi.iItem       = ListView_GetItemCount(m_hWndFrameList);
    ListView_InsertItem(m_hWndFrameList, &lvi);
    ListView_SetItemText(m_hWndFrameList, lvi.iItem, 0, (LPTSTR)pcszFrameName);  

Exit0:
    return true;
}

int KWndAnimateList::ClearAnimateGroup()
{
    ListView_DeleteAllItems(m_hWndAnimateList);
    return true;
}

int KWndAnimateList::ClearAnimateFrame()
{
    ListView_DeleteAllItems(m_hWndFrameList);
    return true;
}



int KWndAnimateList::OnTimer()
{
    int nResult = false;
    int nRetCode = false;
    KAnimateBlock *pAnimate = NULL;

    KG_PROCESS_ERROR(m_nDrawType == DRAW_ANIMATE);

    nRetCode = DrawFrame(m_nCurrentSelGroup, m_nCurrentDrawFrame);
    KG_PROCESS_ERROR(nRetCode);
    
    pAnimate = KAnimateManager::GetSelf().Get(m_nCurrentSelGroup);
    ASSERT(pAnimate);
    ++m_nCurrentDrawFrame;
    if (m_nCurrentDrawFrame >= pAnimate->GetFrameCount())
        m_nCurrentDrawFrame = 0;

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::DrawFrame(int nGroup, int nFrame)
{
    int nResult = false;
    int nRetCode = false;
    RECT rcDrawWindow;
    HDC hDc = NULL;
    Gdiplus::Graphics *pGraphics = NULL;
    Gdiplus::Bitmap *pBitmap = NULL;
    KAnimateBlock *pAnimate = NULL;
    KTgaBlock *pBlock = NULL;
    LPCTSTR pcszFrameName = NULL;
    BITMAPINFO bmpInfo;

    KG_PROCESS_ERROR(nGroup >= 0);
    KG_PROCESS_ERROR(nFrame >= 0);

    KG_PROCESS_ERROR(m_hWndDrawWindow);
    ::GetClientRect(m_hWndDrawWindow, &rcDrawWindow);
    hDc = GetDC(m_hWndDrawWindow);
    KG_PROCESS_ERROR(hDc);
    ::ValidateRect(m_hWndDrawWindow, NULL);
    pGraphics = new Gdiplus::Graphics(hDc);
    KG_PROCESS_ERROR(pGraphics);
	pGraphics->Clear(Gdiplus::Color(128, 128, 128));

    pAnimate = KAnimateManager::GetSelf().Get(nGroup);
    KG_PROCESS_ERROR(pAnimate);
    pcszFrameName = pAnimate->Get(nFrame);
    KG_PROCESS_ERROR(pcszFrameName);
    pBlock = KTgaManager::GetSelf().Get(pcszFrameName);
    KG_PROCESS_ERROR(pBlock);

    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biClrImportant = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biCompression = 0;
    bmpInfo.bmiHeader.biHeight = 0 - pBlock->GetHeight();   
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biSizeImage = pBlock->GetHeight() * pBlock->GetWidth()* sizeof(DWORD);
    bmpInfo.bmiHeader.biWidth = pBlock->GetWidth();
    bmpInfo.bmiHeader.biXPelsPerMeter = 2834;
    bmpInfo.bmiHeader.biYPelsPerMeter = 2834;
    bmpInfo.bmiColors[0].rgbBlue = 255;
    bmpInfo.bmiColors[0].rgbGreen = 255;
    bmpInfo.bmiColors[0].rgbRed = 255;
    bmpInfo.bmiColors[0].rgbReserved = 0;

    pBitmap = Gdiplus::Bitmap::FromBITMAPINFO(&bmpInfo, pBlock->GetBufferPtr());
    KG_PROCESS_ERROR(pBitmap);

    
    pGraphics->DrawImage(
        pBitmap, 
        (rcDrawWindow.right - pBlock->GetWidth()) / 2, 
        (rcDrawWindow.bottom - pBlock->GetHeight()) / 2
    );

    nResult = true;
Exit0:
    KG_DELETE(pBitmap);
    KG_DELETE(pGraphics);
    return nResult;
}

int KWndAnimateList::ClearDraw()
{
    int nResult = false;
    int nRetCode = false;
    RECT rcDrawWindow;
    HDC hDc = NULL;
    Gdiplus::Graphics *pGraphics = NULL;

    KG_PROCESS_ERROR(m_hWndDrawWindow);
    ::GetClientRect(m_hWndDrawWindow, &rcDrawWindow);
    hDc = GetDC(m_hWndDrawWindow);
    KG_PROCESS_ERROR(hDc);
    ::ValidateRect(m_hWndDrawWindow, NULL);
    pGraphics = new Gdiplus::Graphics(hDc);
    KG_PROCESS_ERROR(pGraphics);

    pGraphics->Clear(Gdiplus::Color(128, 128, 128));

    nResult = true;
Exit0:
    KG_DELETE(pGraphics);
    return nResult;

}

int KWndAnimateList::OnRButtonClickGroup(int nIndex)
{
    int nResult = false;
    int nRetCode = false;
    HMENU hMenu = NULL;
    POINT msPos;
    int nId = -1;
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hWndAnimateList);
    KG_PROCESS_ERROR(m_hInstance);
    
    KG_PROCESS_ERROR(nIndex >= 0);

    hMenu = ::LoadMenu((HINSTANCE)m_hInstance, MAKEINTRESOURCE(IDR_ANIMATE_GROUP_RCLICK_MENU));
    KG_PROCESS_ERROR(hMenu);

    ::GetCursorPos(&msPos);
    nId = ::TrackPopupMenu(
        ::GetSubMenu(hMenu, 0),
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		msPos.x, msPos.y, 0, m_hWnd, NULL
    );
    ::DestroyMenu(hMenu);

    switch(nId)
    {
    case IDM_ANIMATE_GROUP_DELETE:
        if (m_nCurrentSelGroup == nIndex)
        {
            m_nCurrentSelGroup = -1;
            m_nCurrentSelFrame = -1;
            m_nDrawType = DRAW_NOTHING;
            ClearDraw();
        }
        nRetCode = KAnimateManager::GetSelf().Remove(nIndex);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case IDM_ANIMATE_GROUP_ADD_FRAME:
        ::SetFocus(m_hWndParent);
        ::SetCapture(m_hWnd);
        m_nLButtonDownDoWhat = DO_APPENDFRAME;
        break;
    case IDM_ANIMATE_GROUP_ADD_GROUP:
        nRetCode = OnNewAnimateGroup();
        KG_PROCESS_ERROR(nRetCode);
        break;
    case IDM_ANIMATE_GROUP_EXPORT_TGA:
        nRetCode = OnExportTgaGroup();
        KG_PROCESS_ERROR(nRetCode);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::OnRButtonClickFrame(int nIndex)
{
    int nResult = false;
    int nRetCode = false;
    HMENU hMenu = NULL;
    POINT msPos;
    int nId = -1;
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hWndAnimateList);
    KG_PROCESS_ERROR(m_hInstance);
    
    KG_PROCESS_ERROR(nIndex >= 0);

    hMenu = ::LoadMenu((HINSTANCE)m_hInstance, MAKEINTRESOURCE(IDR_ANIMATE_FRAME_RCLICK_MENU));
    KG_PROCESS_ERROR(hMenu);

    ::GetCursorPos(&msPos);
    nId = ::TrackPopupMenu(
        ::GetSubMenu(hMenu, 0),
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY,
		msPos.x, msPos.y, 0, m_hWnd, NULL
    );
    ::DestroyMenu(hMenu);

    switch(nId)
    {
    case IDM_ANIMATE_FRAME_INSERT:
        ::SetFocus(m_hWndParent);
        ::SetCapture(m_hWnd);
        m_nLButtonDownDoWhat = DO_INSERTFRAME;
        break;
    case IDM_ANIMATE_FRAME_ADD:
        ::SetFocus(m_hWndParent);
        ::SetCapture(m_hWnd);
        m_nLButtonDownDoWhat = DO_APPENDFRAME;
        break;
    case IDM_ANIMATE_FRAME_DELETE:
        {
            KAnimateBlock *pAnimate = KAnimateManager::GetSelf().Get(m_nCurrentSelGroup);
            if (pAnimate)
            {
                nRetCode = pAnimate->RemoveFrame(m_nCurrentSelFrame);
                KG_PROCESS_ERROR(nRetCode);
                pAnimate->UpdataListView();
                ClearDraw();
            }
        }
        break;
    case IDM_ANIMATE_FRAME_MOVE_UP:
        {
            KAnimateBlock *pAnimate = KAnimateManager::GetSelf().Get(m_nCurrentSelGroup);
            if (pAnimate)
            {
                nRetCode = pAnimate->MoveUp(m_nCurrentSelFrame);
                KG_PROCESS_ERROR(nRetCode);
                pAnimate->UpdataListView();
                ClearDraw();
            }
        }
        break;
    case IDM_ANIMATE_FRAME_MOVE_DOWN:
        {
            KAnimateBlock *pAnimate = KAnimateManager::GetSelf().Get(m_nCurrentSelGroup);
            if (pAnimate)
            {
                nRetCode = pAnimate->MoveDown(m_nCurrentSelFrame);
                KG_PROCESS_ERROR(nRetCode);
                pAnimate->UpdataListView();
                ClearDraw();
            }
        }
        break;
    case IDM_ANIMATE_FRAME_EXPORT_TGA:
        nRetCode = OnExportTgaFrame();
        KG_PROCESS_ERROR(nRetCode);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::OnLButtonDown()
{
    int nResult = false;
    int nRetCode = false;
    int nPickedIndex = -1;
    POINT msPos;
    KTgaBlock *pBlock = NULL;
    KAnimateBlock *pAnimate = NULL;
    TCHAR szName[MAX_PATH];

    if (::GetCapture() == m_hWnd)
        ::ReleaseCapture();

    KG_PROCESS_ERROR (m_nLButtonDownDoWhat != DO_NOTING);

    ::GetCursorPos(&msPos);
    ::ScreenToClient(m_hWndParent, &msPos);
    nPickedIndex = KTgaManager::GetSelf().GetPosedBlockIndex(
        msPos.x + KShow::GetSelf().GetShowStartX(), 
        msPos.y + KShow::GetSelf().GetShowStartY()
    );
    KG_PROCESS_ERROR(nPickedIndex >= 0);
    pBlock = KTgaManager::GetSelf().Get(nPickedIndex);
    KG_PROCESS_ERROR(pBlock);
    pBlock->GetName(szName, sizeof(szName) / sizeof(TCHAR));
    nRetCode = KAnimateManager::GetSelf().IsFrameAleadyInAnyAnimateGroup(szName);
    if (nRetCode)
    {
        ::MessageBox(m_hWnd, _T("对不起,不能添加,其他组用到了该帧"), _T("错误:"), MB_OK | MB_ICONINFORMATION);
        KG_PROCESS_ERROR(false);
    }

    pAnimate = KAnimateManager::GetSelf().Get(m_nCurrentSelGroup);
    KG_PROCESS_ERROR(pAnimate);

    switch(m_nLButtonDownDoWhat)
    {
    case DO_INSERTFRAME:
        nRetCode = pAnimate->InsertFrame(m_nCurrentSelFrame, szName);
        KG_PROCESS_ERROR(nRetCode);
        pAnimate->UpdataListView();
        break;
    case DO_APPENDFRAME:
        nRetCode = pAnimate->AppendFrame(szName);
        KG_PROCESS_ERROR(nRetCode);
        pAnimate->UpdataListView();
        break;
    }

Exit0:
    m_nLButtonDownDoWhat = DO_NOTING;
    return nResult;
}


int KWndAnimateList::OnExportTgaFrame()
{
    int nResult = false;
    int nRetCode = false;
    int nBlockIndex = -1;
	BROWSEINFO bi;
    ITEMIDLIST *pItemList = NULL;
    KAnimateBlock *pAnimate = NULL;
    TCHAR szPath[MAX_PATH];

    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_nCurrentSelFrame >= 0);
    KG_PROCESS_ERROR(m_nCurrentSelGroup >= 0);

	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
    bi.lpszTitle = _T("请指定目录(警告:会覆盖目录下的同名文件)");
	bi.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS
        |BIF_VALIDATE | BIF_RETURNFSANCESTORS;
    pItemList = SHBrowseForFolder(&bi);
    KG_PROCESS_ERROR(pItemList);
	nRetCode = SHGetPathFromIDList(pItemList, szPath);
    KG_PROCESS_ERROR(nRetCode);

    if (
        szPath[_tcslen(szPath) - 1] == _T('\\') || 
        szPath[_tcslen(szPath) - 1] == _T('/')
    )
    szPath[_tcslen(szPath) - 1] = _T('\0');

    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szPath);

    pAnimate = KAnimateManager::GetSelf().Get(m_nCurrentSelGroup);
    KG_PROCESS_ERROR(pAnimate);
    nBlockIndex = KTgaManager::GetSelf().GetIndex(pAnimate->Get(m_nCurrentSelFrame));
    KG_PROCESS_ERROR(nBlockIndex >= 0);

    nRetCode = KTgaManager::GetSelf().ExportTga(szPath, nBlockIndex);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::OnExportTgaGroup()
{
    int nResult = false;
    int nRetCode = false;
    int nBlockIndex = -1;
	BROWSEINFO bi;
    ITEMIDLIST *pItemList = NULL;
    KAnimateBlock *pAnimate = NULL;
    TCHAR szPath[MAX_PATH];

    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_nCurrentSelGroup >= 0);

	memset(&bi, 0, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
    bi.lpszTitle = _T("请指定目录(警告:会覆盖目录下的同名文件)");
	bi.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS
        |BIF_VALIDATE | BIF_RETURNFSANCESTORS;
    pItemList = SHBrowseForFolder(&bi);
    KG_PROCESS_ERROR(pItemList);
	nRetCode = SHGetPathFromIDList(pItemList, szPath);
    KG_PROCESS_ERROR(nRetCode);

    if (
        szPath[_tcslen(szPath) - 1] == _T('\\') || 
        szPath[_tcslen(szPath) - 1] == _T('/')
    )
    szPath[_tcslen(szPath) - 1] = _T('\0');

    KSaveManager::GetSelf().SetLastOpenOrSaveDir(szPath);

    pAnimate = KAnimateManager::GetSelf().Get(m_nCurrentSelGroup);
    KG_PROCESS_ERROR(pAnimate);
    for (int i = 0; i < pAnimate->GetFrameCount(); ++i)
    {
        nBlockIndex = KTgaManager::GetSelf().GetIndex(pAnimate->Get(i));
        KG_PROCESS_ERROR(nBlockIndex >= 0);
        nRetCode = KTgaManager::GetSelf().ExportTga(szPath, nBlockIndex);
        KG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::OnNewAnimateGroup()
{
    int nResult = false;

    KG_PROCESS_ERROR(m_hInstance);
    KG_PROCESS_ERROR(m_hWnd);

    DialogBox(m_hInstance, (LPCTSTR)IDD_NEW_ANIMATE_GROUP, m_hWnd, (DLGPROC)NewAnimateGroupProc);

    nResult = true;
Exit0:
    return nResult;
}

int KWndAnimateList::OnNewAnimateGroupNameSeted(HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    TCHAR szName[MAX_PATH];
    KAnimateBlock *pAnimate;

    KG_PROCESS_ERROR(hWnd);

    ::GetDlgItemText(hWnd, IDC_NEW_ANIMATE_GROUP_NAME, szName, sizeof(szName) / sizeof(TCHAR));
    szName[sizeof(szName) / sizeof(TCHAR) - 1] = _T('\0');

    KG_PROCESS_ERROR(szName[0] != _T('\0'));

    pAnimate = KAnimateManager::GetSelf().Get(szName);   
    KG_PROCESS_ERROR(!pAnimate);

    pAnimate = KAnimateBlock::Create(szName, true, -1);
    KG_PROCESS_ERROR(pAnimate);

    nRetCode = KAnimateManager::GetSelf().Append(pAnimate);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        ::MessageBox(hWnd, _T("对不起,不能添加组(可能是该动画已经存在)，请输入正确的名称."), _T("错误:"), MB_OK | MB_ICONINFORMATION);
    return nResult;
}




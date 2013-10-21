////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KWndFrameList.cpp
//  Version     : 1.0
//  Creator     : Lin Jiaqi
//  Create Date : 2005-12-20 10:46:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//#include "MergeTga.h"
#include "KWndFrameList.h"
#include "resource.h"
#include "KPickBox.h"
#include "KTgaManager.h"
#include "KSaveManager.h"
#include "KShow.h"
#include "KIconManager.h"
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////////////

KWndFrameList *KWndFrameList::ms_pself = NULL;

KWndFrameList::KWndFrameList()
{
	m_hWndParent = NULL;
	m_hWnd = NULL;
	m_hListWnd = NULL; 
	m_hInstance = NULL;
	m_nCurrentSortType = SORT_TYPE_BEGINE;
	m_nGetSelBlockFlag = false;
	m_nThePelaceBlockIndex = -1;
}

KWndFrameList::~KWndFrameList()
{
}

int KWndFrameList::Init(HINSTANCE hInst, HWND hWnd)
{
	int nResult = false;
	int nRetCode = false;
	KG_PROCESS_ERROR(!ms_pself);
	ms_pself = new KWndFrameList;
	KG_PROCESS_ERROR(ms_pself);
	nRetCode = ms_pself->CreateDialogWnd(hInst, hWnd);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (!nResult)
		KWndFrameList::Exit();
	return nResult;

}

int KWndFrameList::Exit()
{
	if (ms_pself)
		ms_pself->DestroyDialogWnd();
	KG_DELETE(ms_pself);
	return true;
}

KWndFrameList &KWndFrameList::GetSelf()
{
	ASSERT(ms_pself);
	return *ms_pself;
}

LRESULT CALLBACK KWndFrameList::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	KWndFrameList *pWndNameList = (KWndFrameList *)GetWindowLong(hWnd, GWL_USERDATA);

	switch(message) 
	{
	case WM_COMMAND:
        KG_PROCESS_ERROR(pWndNameList);
// 		if (LOWORD(wParam) == IDCANCEL)
// 			OnShowOrHideFrameListWindow(pWndNameList->GetParentHWND());
		break;
    case WM_DRAWITEM:
        KG_PROCESS_ERROR(pWndNameList);
        pWndNameList->Draw((LPDRAWITEMSTRUCT)lParam);
        break;
    case WM_MEASUREITEM:
        if (lParam)
        {
            LPMEASUREITEMSTRUCT lpmis; 
            lpmis = (LPMEASUREITEMSTRUCT)lParam; 
            lpmis->itemHeight = TGA_HEIGHT;
            lRet = TRUE;
        }
        break;
	default:
		break;
	}

Exit0:
	return lRet;
}

int KWndFrameList::CreateDialogWnd(HINSTANCE hInst, HWND hWnd)
{
	int nResult = false;

	ASSERT(!m_hWnd);
	m_hWnd = ::CreateDialog(hInst, MAKEINTRESOURCE(IDD_FRAME_LIST), hWnd, (DLGPROC)WndProc);

	KG_PROCESS_ERROR(m_hWnd);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
	::ShowWindow(m_hWnd, SW_SHOW);

	{
        m_hListWnd = ::GetDlgItem(m_hWnd, IDC_FRAME_LIST);
		KG_PROCESS_ERROR(m_hListWnd);
		//设置文件列表框的属性与分栏信息
		ListView_SetExtendedListViewStyle(m_hListWnd,
			ListView_GetExtendedListViewStyle(m_hListWnd) |	LVS_EX_FULLROWSELECT);

		RECT	rc;
		::GetClientRect(m_hListWnd, &rc);
		LVCOLUMN	lvc;
		TCHAR		szHeaderText[128];
		lvc.mask = LVCF_TEXT | LVCF_WIDTH;
		lvc.cx = rc.right - rc.left;
		lvc.pszText = szHeaderText;
		lvc.cchTextMax = 1024;
		_tcsncpy(szHeaderText, "NEW_TGA_LIST", sizeof(szHeaderText) / sizeof(TCHAR));

		lvc.mask = LVCF_TEXT | LVCF_WIDTH;
		lvc.cx = (rc.right - rc.left) / 15;
		lvc.pszText = szHeaderText;
		lvc.cchTextMax = 128;
		strcpy(szHeaderText, "ID");
		ListView_InsertColumn(m_hListWnd, 0, &lvc);

        lvc.cx = (rc.right - rc.left)  / 15;
        lvc.mask |= LVCF_SUBITEM;
        lvc.iSubItem = 1;
        strcpy(szHeaderText, "Picture");
        ListView_InsertColumn(m_hListWnd, 1, &lvc);

		lvc.cx = (rc.right - rc.left)  / 5;
		lvc.mask |= LVCF_SUBITEM;
		lvc.iSubItem = 1;
		strcpy(szHeaderText, "UITexName");
		ListView_InsertColumn(m_hListWnd, 2, &lvc);

		lvc.cx = (rc.right - rc.left)  / 15;
		lvc.mask |= LVCF_SUBITEM;
		lvc.iSubItem = 1;
		strcpy(szHeaderText, "Frame");
		ListView_InsertColumn(m_hListWnd, 3, &lvc);

		lvc.cx = (rc.right - rc.left)  / 5;
		lvc.mask |= LVCF_SUBITEM;
		lvc.iSubItem = 1;
		strcpy(szHeaderText, "FrameName");
		ListView_InsertColumn(m_hListWnd, 4, &lvc);

		lvc.cx = (rc.right - rc.left)  / 15;
		lvc.mask |= LVCF_SUBITEM;
		lvc.iSubItem = 1;
		strcpy(szHeaderText, "Width");
		ListView_InsertColumn(m_hListWnd, 5, &lvc); 

		lvc.cx = (rc.right - rc.left)  / 15;
		lvc.mask |= LVCF_SUBITEM;
		lvc.iSubItem = 1;
		strcpy(szHeaderText, "Height");
		ListView_InsertColumn(m_hListWnd, 6, &lvc); 

		lvc.cx = (rc.right - rc.left)  / 8;
		lvc.mask |= LVCF_SUBITEM;
		lvc.iSubItem = 1;
		strcpy(szHeaderText, "Remark");
		ListView_InsertColumn(m_hListWnd, 7, &lvc);
	}

	m_hWndParent = hWnd;
	m_hInstance = hInst;

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (m_hListWnd)
		{
			::DestroyWindow(m_hListWnd);
			m_hListWnd = NULL;
		}
		if (m_hWnd)
		{
			::DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		} 
		m_hInstance = NULL;
		m_hWndParent = NULL;
	}    
	return nResult;
}

int KWndFrameList::DestroyDialogWnd()
{
	if (m_hListWnd)
	{
		::DestroyWindow(m_hListWnd);
		m_hListWnd = NULL;
	}
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
	m_hInstance = NULL;  
	m_hWndParent = NULL;
	return true;
}

HWND KWndFrameList::GetHWND()
{
	return m_hWnd;
}

HWND KWndFrameList::GetListHWND()
{
	return m_hListWnd;
}

HWND KWndFrameList::GetParentHWND()
{
	return m_hWndParent;
}

int KWndFrameList::Show()
{
	::ShowWindow(m_hWnd, SW_SHOW);
	return true;
}

int KWndFrameList::Hide()
{
	::ShowWindow(m_hWnd, SW_HIDE);
	return true;
}

int KWndFrameList::Clear()
{
	ListView_DeleteAllItems(m_hListWnd);
	return true;
}

int KWndFrameList::AppendFrameGroup(
		LPCTSTR pcszFileName, LPCTSTR pcszFrameName, 
        int nId, int nFrame, int nWidth, int nHeight, 
		LPCTSTR pcszRemark, BYTE *pbyBuffer, int nBufferSize
)
{
	int nResult = false;
	LV_ITEM			lvi;
	LPCTSTR pszTemp = NULL;
	TCHAR szText[32];
    szText[0] = _T('\0');

	KG_PROCESS_ERROR(pcszFileName);
    KG_PROCESS_ERROR(pcszFrameName);
    KG_PROCESS_ERROR(pbyBuffer);
	KG_PROCESS_ERROR(m_hListWnd);

	memset(&lvi, 0, sizeof(LV_ITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText		= _T("");
	lvi.cchTextMax  = 128;
	lvi.iItem       = ListView_GetItemCount(m_hListWnd);
	ListView_InsertItem(m_hListWnd, &lvi);

    _sntprintf(szText, sizeof(szText) / sizeof(szText[0]), "%d", nId);
	ListView_SetItemText(m_hListWnd, lvi.iItem, 0, (LPTSTR)szText);        //ID号
	ListView_SetItemText(m_hListWnd, lvi.iItem, 2, (LPTSTR)pcszFileName);  //文件名
    _sntprintf(szText, sizeof(szText) / sizeof(szText[0]), "%d", nFrame);
	ListView_SetItemText(m_hListWnd, lvi.iItem, 3, (LPTSTR)szText);        //文件内帧号
    ListView_SetItemText(m_hListWnd, lvi.iItem, 4, (LPTSTR)pcszFrameName);  //图片名
    _sntprintf(szText, sizeof(szText) / sizeof(szText[0]), "%d", nWidth);
	ListView_SetItemText(m_hListWnd, lvi.iItem, 5, (LPTSTR)szText);        //图片宽
    _sntprintf(szText, sizeof(szText) / sizeof(szText[0]), "%d", nHeight);
	ListView_SetItemText(m_hListWnd, lvi.iItem, 6, (LPTSTR)szText);        //图片高
//	ListView_SetItemText(m_hListWnd, lvi.iItem, 7, (LPTSTR)pRemark);        //图片说明

	nResult = true;
Exit0:
	return nResult;
}

int KWndFrameList::ClearFrameGroup()
{
	ListView_DeleteAllItems(m_hListWnd);
	return true;
}

//////////////////////////////////////////////////////////////////////////
int KWndFrameList::Draw(LPDRAWITEMSTRUCT lpDrawItem)
{
    RECT rc;
    RECT rcTga;
    HDC hDc = NULL;
    TCHAR szId[128] = "";
    szId[0] = _T('\0');
    TCHAR szUitex[128] = "";
    szUitex[0] = _T('\0');
    TCHAR szFrame[128] = "";
    szFrame[0] = _T('\0');
    TCHAR szName[128] = "";
    szName[0] = _T('\0');
    TCHAR szWeith[128] = "";
    szWeith[0] = _T('\0');
    TCHAR szHeight[128] = "";
    szHeight[0] = _T('\0');
    TCHAR szInfo[128] = "";
    szInfo[0] = _T('\0');

    RECT rcDrawWindow;
    Gdiplus::Graphics *pGraphics = NULL;
    Gdiplus::Bitmap *pBitmap = NULL;
    KTgaBlock *pBlock = NULL;
    LPCTSTR pcszFrameName = NULL;
    BITMAPINFO bmpInfo; 

    KG_PROCESS_ERROR(lpDrawItem);
    KG_PROCESS_ERROR(lpDrawItem->CtlType == ODT_LISTVIEW);
    KG_PROCESS_ERROR(lpDrawItem->hwndItem == m_hListWnd);
    hDc = lpDrawItem->hDC;

    ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 0, szId, sizeof(szId) / sizeof(szId[0]));
    ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 2, szUitex, sizeof(szUitex) / sizeof(szUitex[0]));
    ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 3, szFrame, sizeof(szFrame) / sizeof(szFrame[0]));
    ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 4, szName, sizeof(szName) / sizeof(szName[0]));
    ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 5, szWeith, sizeof(szWeith) / sizeof(szWeith[0]));
    ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 6, szHeight, sizeof(szHeight) / sizeof(szHeight[0]));
    ListView_GetItemText(m_hListWnd, lpDrawItem->itemID, 7, szInfo, sizeof(szInfo) / sizeof(szInfo[0]));

    ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 0, LVIR_BOUNDS, &rc);
    rc.top += TGA_HEIGHT / 3 - 3;
    DrawText(hDc, szId, sizeof(szId) / sizeof(szId[0]) , &rc, NULL);
    ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 1, LVIR_BOUNDS, &rcTga);
    ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 2, LVIR_BOUNDS, &rc);
    rc.top += TGA_HEIGHT / 3 - 3;
    DrawText(hDc, szUitex, sizeof(szUitex) / sizeof(szUitex[0]), &rc, NULL);
    ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 3, LVIR_BOUNDS, &rc);
    rc.top += TGA_HEIGHT / 3 - 3;
    DrawText(hDc, szFrame, sizeof(szFrame) / sizeof(szFrame[0]), &rc, NULL);
    ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 4, LVIR_BOUNDS, &rc);
    rc.top += TGA_HEIGHT / 3 - 3;
    DrawText(hDc, szName, sizeof(szName) / sizeof(szName[0]), &rc, NULL);
    ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 5, LVIR_BOUNDS, &rc);
    rc.top += TGA_HEIGHT / 3 - 3;
    DrawText(hDc, szWeith, sizeof(szWeith) / sizeof(szWeith[0]), &rc, NULL);
    ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 6, LVIR_BOUNDS, &rc);
    rc.top += TGA_HEIGHT / 3 - 3;
    DrawText(hDc, szHeight, sizeof(szHeight) / sizeof(szHeight[0]), &rc, NULL);
    ListView_GetSubItemRect(m_hListWnd, lpDrawItem->itemID, 7, LVIR_BOUNDS, &rc);
    rc.top += TGA_HEIGHT / 3 - 3;
    DrawText(hDc, szInfo, sizeof(szInfo) / sizeof(szInfo[0]), &rc, NULL);

    pBlock = KIconManager::GetSelf().Get(lpDrawItem->itemID)->GetTgaBlock();
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
    KG_PROCESS_ERROR(m_hListWnd);
    ::GetClientRect(m_hListWnd, &rcDrawWindow); 
    hDc = GetDC(m_hListWnd);
    KG_PROCESS_ERROR(hDc);
    ::ValidateRect(m_hListWnd, NULL);
    pGraphics = new Gdiplus::Graphics(hDc);
    KG_PROCESS_ERROR(pGraphics);
    pGraphics->DrawImage(pBitmap, rcTga.left, rcTga.top);

Exit0:
    KG_DELETE(pBitmap);
    KG_DELETE(pGraphics);
    return true;
}

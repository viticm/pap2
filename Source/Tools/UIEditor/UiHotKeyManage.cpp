#include "Stdafx.h"
#include "UiHotKeyManage.h"
#include "resource.h"
#include "khotkeytable.h"



static KUiHotKeyManage g_UiHotKeyManager;

KUiHotKeyManage::KUiHotKeyManage()
{
    m_hWnd      = NULL;
    m_hListWnd  = NULL;
    m_pIni      = NULL;
    m_bNowGetKey = false;

}

KUiHotKeyManage::~KUiHotKeyManage()
{

}

int KUiHotKeyManage::Init(HWND hWnd)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);
    m_hWnd = hWnd;

    RECT rcWindow;
    ::GetWindowRect(m_hWnd, &rcWindow);
    ::SetWindowPos(m_hWnd, HWND_TOP, 400, 200,
		rcWindow.right -rcWindow.left, rcWindow.bottom - rcWindow.top,
		SWP_SHOWWINDOW);

	m_hListWnd = ::GetDlgItem(hWnd, IDC_WND_HOTKEY_LIST);
    KG_PROCESS_ERROR(m_hListWnd);
    
	RECT	rc;
	::GetClientRect(m_hListWnd, &rc);
	LVCOLUMN	lvc;
	char		szHeaderText[128];
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = (rc.right - rc.left) / 3;
	lvc.pszText = szHeaderText;
	lvc.cchTextMax = 128;
    strcpy(szHeaderText, "ÑÕÉ«Ãû:");
	ListView_InsertColumn(m_hListWnd, 0, &lvc);

Exit0:
    return nResult;
}

void KUiHotKeyManage::OnListNotify(LPNMHDR pNotify, LPARAM lParam)
{
	switch(pNotify->code)
	{
	case LVN_ITEMACTIVATE:
	case NM_CLICK:
		{
            m_bNowGetKey = true;
        }
        break;
    case LVN_KEYDOWN:
        if(m_bNowGetKey)
        {
            NMLVKEYDOWN *pnkd = (LPNMLVKEYDOWN)lParam;

 	        int nModifier = 0;
	        if (::GetKeyState(VK_CONTROL) & 0x8000)
		        nModifier |= HOTKEYF_CONTROL;
	        if (::GetKeyState(VK_SHIFT) & 0x8000)
		        nModifier |= HOTKEYF_SHIFT;
	        if (::GetKeyState(VK_MENU) & 0x8000)
		        nModifier |= HOTKEYF_ALT;

	        DWORD const dwKey = MAKELONG(pnkd->wVKey, nModifier);
            
            UI::KHotKeyTable HotKeyTable;
            char szBuffer[128];
            strcpy(szBuffer, HotKeyTable.TranslateHotkeyToText((DWORD)dwKey));
            LV_ITEM		lvi;
		    memset(&lvi, 0, sizeof(LV_ITEM));
		    lvi.mask = LVIF_TEXT;
		    lvi.pszText		= (char*)szBuffer;
		    lvi.cchTextMax  = 128;
		    lvi.iItem       = ListView_GetItemCount(m_hListWnd);
		    lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);
        }
        break;
    }
}

BOOL CALLBACK g_HotKeyDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int nResult = FALSE;

    switch(message) 
    {
    case WM_INITDIALOG:
        g_UiHotKeyManager.Init(hWnd);
        break;
    case WM_CLOSE:
        EndDialog(hWnd, FALSE);
        nResult = TRUE;
        break;
    case WM_NOTIFY:
        {
		    LPNMHDR pNotify = (LPNMHDR)lParam;
		    if (pNotify->hwndFrom == g_UiHotKeyManager.GetListHandle())
			    g_UiHotKeyManager.OnListNotify(pNotify, lParam);
        }
        break;
    case WM_COMMAND:
	    {
		    int nNotify = HIWORD(wParam);
		    switch(nNotify)
		    {
		    case EN_CHANGE:
			    break;
		    case 0:
			    break;
		    }
	    }
        break;
    case WM_KEYDOWN:
        if(g_UiHotKeyManager.GetGetKeyState())
        {
            UI::KHotKeyTable HotKeyTable;
            char szBuffer[128];
            strcpy(szBuffer, HotKeyTable.TranslateHotkeyToText(wParam));
            LV_ITEM		lvi;
		    memset(&lvi, 0, sizeof(LV_ITEM));
		    lvi.mask = LVIF_TEXT;
		    lvi.pszText		= (char*)szBuffer;
		    lvi.cchTextMax  = 128;
		    lvi.iItem       = ListView_GetItemCount(g_UiHotKeyManager.GetListHandle());
		    lvi.iItem		= ListView_InsertItem(g_UiHotKeyManager.GetListHandle(), &lvi);
        }
        break;
    default:
        break;
    }

	return nResult;
}

void KUiHotKeyManage::OnHotKeyManage(HWND hWnd)
{
    ::DialogBox((HINSTANCE)g_hPluginInstance, 
        MAKEINTRESOURCE(IDD_WndHotKeyManage), 
        hWnd, (DLGPROC)g_HotKeyDlgProc);
}

int HotKeyManage(HWND hWnd)
{
     g_UiHotKeyManager.OnHotKeyManage(hWnd);
     return true;
}
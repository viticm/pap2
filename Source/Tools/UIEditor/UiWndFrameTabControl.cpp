#include "Stdafx.h"
#include "UiWndFrameTabControl.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"

KUiWndFrameTabControl::KUiWndFrameTabControl()
{   

}
KUiWndFrameTabControl::~KUiWndFrameTabControl()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndFrameTabControl::Create()
{
	KUiWndFrameTabControl* pWnd = new KUiWndFrameTabControl;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndFrameTabControl::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);
    strcpy(m_szName, pszName);

	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndFrameTabControl), hWnd, (DLGPROC)WndProc);
	KG_PROCESS_ERROR(m_hWnd);

    ::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);


	m_hListWnd = ::GetDlgItem(m_hWnd, IDC_WND_FRAME_FOCUS_LIST);
	//设置文件列表框的属性与分栏信息
	ListView_SetExtendedListViewStyle(m_hListWnd,
		ListView_GetExtendedListViewStyle(m_hListWnd) |	LVS_EX_FULLROWSELECT);
	RECT	rc;
	::GetClientRect(m_hListWnd, &rc);
	LVCOLUMN	lvc;
	char		szHeaderText[128];
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = (rc.right - rc.left);
	lvc.pszText = szHeaderText;
	lvc.cchTextMax = 128;
	strcpy(szHeaderText, "焦点切换顺序（从上到下）：");
	ListView_InsertColumn(m_hListWnd, 0, &lvc);

    nResult = true;
Exit0:

	return nResult;
}

LRESULT CALLBACK KUiWndFrameTabControl::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	KUiWndFrameTabControl*	pView = (KUiWndFrameTabControl*)GetWindowLong(hWnd, GWL_USERDATA);
	if (!pView)
		return 0;
    if (message == WM_NOTIFY) 
    {
       if (pView)
        {
			LPNMHDR pNotify = (LPNMHDR)lParam;
			if (pNotify->hwndFrom == pView->m_hListWnd)
				pView->OnListNotify(pNotify);
        }

    }
	if (message == WM_COMMAND)
	{
		int nNotify = HIWORD(wParam);
		switch(nNotify)
		{
		case EN_CHANGE:
			break;
		case 0:
            pView->OnButtonClick(LOWORD(wParam));
			break;
		}
	}
	return 0;
}

void KUiWndFrameTabControl::OnListNotify(LPNMHDR pNotify)
{
	switch(pNotify->code)
	{
	case LVN_ITEMACTIVATE:
	case NM_CLICK:
		{
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			if (pActive->iItem >= 0 && pActive->iItem < ListView_GetItemCount(m_hListWnd))
                m_nCurrentItem = pActive->iItem;
		}
		break;
	}
}

void KUiWndFrameTabControl::OnSelWnd()
{
    typedef std::vector<_tstring>	ListItemArray; 
    ListItemArray aList;
    char  szWndType[128] = "";  
    char  szWndName[128] = "";
    HMENU hMenu = NULL;
    int   i = 0;
    POINT MousePosition;
    int   nId = 0;
    int   nSel = 0;
    int   nMax = 0;
    IIniFile *pIni = g_Ui.GetIni();
    KG_PROCESS_ERROR(pIni);

    while (g_Ui.GetNextUnit(szWndName)) 
    {
        pIni->GetString(szWndName, "._WndType", "", szWndType, sizeof(szWndType));

        if (!IsItemExist(szWndName) && _tcsicmp(szWndType, g_GetWndTypeKey(UI_WND_FRAME)) != 0) 
        {
            aList.push_back(szWndName);
        }
    }

    nId =91000;
	hMenu = CreatePopupMenu();
    KG_PROCESS_ERROR(hMenu);   
    nMax = aList.size();
    for (i = 0; i < nMax; i++) 
    {
        strcpy(szWndName, aList[i].c_str());
        AppendMenu(hMenu, MF_ENABLED | MF_STRING, nId, szWndName);
        nId++;
    }
    AppendMenu(hMenu, MF_ENABLED | MF_STRING, nId, "全部添加");
    nId++;

    GetCursorPos(&MousePosition);
    nSel = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY, 
        MousePosition.x, MousePosition.y, 0, m_hWnd, NULL);
	DestroyMenu(hMenu);

    KG_PROCESS_ERROR(nSel >= 91000 && nSel < nId);
    if (nSel == nId - 1) 
    {
        nMax = aList.size();
        for (i = 0; i < nMax; i++) 
        {
            strcpy(szWndName, aList[i].c_str());
            LV_ITEM			lvi;
            memset(&lvi, 0, sizeof(LV_ITEM));
            lvi.mask = LVIF_TEXT;
            lvi.pszText		= (char*)szWndName;
            lvi.cchTextMax  = strlen(szWndName);
            lvi.iItem       = ListView_GetItemCount(m_hListWnd);
            lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);
        }
    }
    else
    {
        strcpy(szWndName, aList[nSel - 91000].c_str());
        LV_ITEM			lvi;
        memset(&lvi, 0, sizeof(LV_ITEM));
        lvi.mask = LVIF_TEXT;
        lvi.pszText		= (char*)szWndName;
        lvi.cchTextMax  = strlen(szWndName);
        lvi.iItem       = ListView_GetItemCount(m_hListWnd);
        lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);
    }

    SaveData();

Exit0:
    aList.clear();
    return;
}

BOOL KUiWndFrameTabControl::IsItemExist(LPCTSTR pszItem)
{
    int nResult = FALSE;
    KG_PROCESS_ERROR(m_hListWnd);
    KG_PROCESS_ERROR(pszItem && pszItem[0]);

    int i = 0;
    int nMaxItem = ListView_GetItemCount(m_hListWnd); 
    char szItemName[128] = "";
    for (i = 0; i < nMaxItem; i++) 
    {
        ListView_GetItemText(m_hListWnd, i, 0, szItemName, sizeof(szItemName));
        if(_tcsicmp(pszItem, szItemName) == 0)
        {
            nResult = TRUE;
            break;
        }
    }

Exit0:
    return nResult;
}

void KUiWndFrameTabControl::OnDel()
{
    KG_PROCESS_ERROR(m_hListWnd);
    KG_PROCESS_ERROR(m_nCurrentItem >= 0 && m_nCurrentItem < ListView_GetItemCount(m_hListWnd));
    ListView_DeleteItem(m_hListWnd, m_nCurrentItem);
    m_nCurrentItem = m_nCurrentItem >= ListView_GetItemCount(m_hListWnd) ? m_nCurrentItem - 1 : m_nCurrentItem;
    ListView_SetItemState(m_hListWnd,m_nCurrentItem,UINT(LVIS_SELECTED | LVIS_FOCUSED ),LVIS_SELECTED | LVIS_FOCUSED);
    SaveData();

Exit0:
    return;
}

void KUiWndFrameTabControl::OnButtonClick(int nBtnId)
{
	switch(nBtnId)
	{
        case IDC_WND_FRAME_FOCUS_ADD_LIST:
            OnSelWnd();
            break;
        case IDC_WND_FRAME_FOCUS_DEL_LIST:
            OnDel();
            break;
        case IDC_WND_FRAME_FOCUS_MOVE_UP:
            OnMove(true);
            break;
        case IDC_WND_FRAME_FOCUS_MOVE_DOWN:
            OnMove(false);
            break;
        case IDC_WND_FRAME_TAB_CHANGE_FOCUS:
            {
		        int nValue = (::IsDlgButtonChecked(m_hWnd, IDC_WND_FRAME_TAB_CHANGE_FOCUS) == BST_CHECKED);
                g_Ui.GetIni()->WriteInteger(m_szName, "EnableTabChangeFocus", nValue);
		        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
                ShowErrorInformation();
            }
		break;
        case IDC_WND_FRAME_TAB_CHANGE_FOCUS_MOUSE_FOLLOW:
            {
		        int nValue = (::IsDlgButtonChecked(m_hWnd, IDC_WND_FRAME_TAB_CHANGE_FOCUS_MOUSE_FOLLOW) == BST_CHECKED);
                g_Ui.GetIni()->WriteInteger(m_szName, "MouseFollowFocus", nValue);
		        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
                ShowErrorInformation();
            }
            break;
        default:
            break;
	}

}

void KUiWndFrameTabControl::OnMove(bool bUp)
{
    KG_PROCESS_ERROR(m_hListWnd);

    if(bUp)
    {
        KG_PROCESS_ERROR(m_nCurrentItem > 0 && m_nCurrentItem < ListView_GetItemCount(m_hListWnd));
        char szItem[128] = "";
        char szPreItem[128] = "";
        ListView_GetItemText(m_hListWnd, m_nCurrentItem, 0, szItem, sizeof(szItem));
        ListView_GetItemText(m_hListWnd, m_nCurrentItem - 1, 0, szPreItem, sizeof(szItem));
        
        ListView_SetItemText(m_hListWnd, m_nCurrentItem, 0, szPreItem);
        ListView_SetItemText(m_hListWnd, m_nCurrentItem - 1, 0, szItem);
        m_nCurrentItem--;
        ListView_SetItemState(m_hListWnd,m_nCurrentItem,UINT(LVIS_SELECTED | LVIS_FOCUSED ),LVIS_SELECTED | LVIS_FOCUSED);
        SaveData();
    }
    else
    {
        KG_PROCESS_ERROR(m_nCurrentItem >= 0 && m_nCurrentItem < ListView_GetItemCount(m_hListWnd) - 1);
        char szItem[128] = "";
        char szNextItem[128] = "";
        ListView_GetItemText(m_hListWnd, m_nCurrentItem, 0, szItem, sizeof(szItem));
        ListView_GetItemText(m_hListWnd, m_nCurrentItem + 1, 0, szNextItem, sizeof(szNextItem));
        
        ListView_SetItemText(m_hListWnd, m_nCurrentItem, 0, szNextItem);
        ListView_SetItemText(m_hListWnd, m_nCurrentItem + 1, 0, szItem);
        m_nCurrentItem++;
        ListView_SetItemState(m_hListWnd,m_nCurrentItem,UINT(LVIS_SELECTED | LVIS_FOCUSED ),LVIS_SELECTED | LVIS_FOCUSED);
        SaveData();
    }

Exit0:
    return;
}

void KUiWndFrameTabControl::SaveData()
{
    KG_PROCESS_ERROR(m_hListWnd);
    KG_PROCESS_ERROR(m_szName[0]);
    IIniFile *pIni = g_Ui.GetIni();
    KG_PROCESS_ERROR(pIni);

    int nMaxNum = 0;
    int i = 0;
    char szKey[128] = "";
    char szKeyValue[128] = "";
    pIni->GetInteger(m_szName, "ItemCount", 0, &nMaxNum);
    for (i = 0; i < nMaxNum; i++) 
    {
        sprintf(szKey, "Item_%d", i);
        pIni->EraseKey(m_szName, szKey);
    }

    nMaxNum = ListView_GetItemCount(m_hListWnd);
    pIni->WriteInteger(m_szName, "ItemCount", nMaxNum);
    for (i = 0; i < nMaxNum; i++) 
    {
        sprintf(szKey, "Item_%d", i);
        ListView_GetItemText(m_hListWnd, i, 0, szKeyValue, sizeof(szKeyValue));
        pIni->WriteString(m_szName, szKey, szKeyValue);
    }

	g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
    ShowErrorInformation();

Exit0:
    return;
}

void KUiWndFrameTabControl::Release()
{
	delete this;
}

void KUiWndFrameTabControl::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(m_hWnd && pIni && pszName);
    strcpy(m_szName, pszName);

    int nValue = 0;
    char szKey[128] = "";
    char szValue[128] = "";
    int i = 0;
    pIni->GetInteger(pszName, "EnableTabChangeFocus", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_FRAME_TAB_CHANGE_FOCUS, nValue ? BST_CHECKED : 0);
    pIni->GetInteger(pszName, "MouseFollowFocus", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_FRAME_TAB_CHANGE_FOCUS_MOUSE_FOLLOW, nValue ? BST_CHECKED : 0);

    KG_PROCESS_ERROR(m_hListWnd);
    ListView_DeleteAllItems(m_hListWnd);
    pIni->GetInteger(pszName, "ItemCount", 0, &nValue);
    for (i = 0; i < nValue; i++) 
    {
        sprintf(szKey, "Item_%d", i);
        pIni->GetString(pszName, szKey, "", szValue, sizeof(szValue));

	    LV_ITEM			lvi;
	    memset(&lvi, 0, sizeof(LV_ITEM));
	    lvi.mask = LVIF_TEXT;
	    lvi.pszText		= (char*)szValue;
	    lvi.cchTextMax  = strlen(szValue);
	    lvi.iItem       = ListView_GetItemCount(m_hListWnd);
	    lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);
    }

Exit0:
    return;
}

void KUiWndFrameTabControl::Show(int bShow, int nSubWnd)
{
	KG_PROCESS_ERROR(m_hWnd);
	::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);

Exit0:
    return;
}

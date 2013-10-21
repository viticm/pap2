#include "Stdafx.h"
#include "UiWndPageSet.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndPageSetCommon::KUiWndPageSetCommon()
{   

}
KUiWndPageSetCommon::~KUiWndPageSetCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndPageSetCommon::Create()
{
	KUiWndPageSetCommon* pWnd = new KUiWndPageSetCommon;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndPageSetCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);
    strcpy(m_szName, pszName);

	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndPageSetCommon), hWnd, (DLGPROC)WndProc);
	KG_PROCESS_ERROR(m_hWnd);

    ::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);


	m_hListWnd = ::GetDlgItem(m_hWnd, IDC_WND_PAGE_SET_PAGE_TREE);
	//设置文件列表框的属性与分栏信息
	ListView_SetExtendedListViewStyle(m_hListWnd,
		ListView_GetExtendedListViewStyle(m_hListWnd) |	LVS_EX_FULLROWSELECT);
	RECT	rc;
	::GetClientRect(m_hListWnd, &rc);
	LVCOLUMN	lvc;
	char		szHeaderText[32];
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = (rc.right - rc.left) / 3;
	lvc.pszText = szHeaderText;
	lvc.cchTextMax = 32;
	strcpy(szHeaderText, "叶数");
	ListView_InsertColumn(m_hListWnd, 0, &lvc);

	lvc.mask |= LVCF_SUBITEM;
	lvc.iSubItem = 1;
	strcpy(szHeaderText, "页面名");
	ListView_InsertColumn(m_hListWnd, 1, &lvc);

	lvc.iSubItem = 2;
	strcpy(szHeaderText, "按钮名");
	ListView_InsertColumn(m_hListWnd, 2, &lvc);

    nResult = true;
Exit0:

	return nResult;
}

LRESULT CALLBACK KUiWndPageSetCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	KUiWndPageSetCommon*	pView = (KUiWndPageSetCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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
            pView->OnCheckBoxClick(LOWORD(wParam));
			break;
		}
	}
	return 0;
}

void KUiWndPageSetCommon::OnListNotify(LPNMHDR pNotify)
{
	switch(pNotify->code)
	{
	case LVN_ITEMACTIVATE:
	case NM_CLICK:
		{
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			if (pActive->iItem >= 0)
			{
                m_nCurrentPageNum = pActive->iItem;
				if (pActive->iSubItem == 1)
                {
                    OnChangeWndowName(pActive->iItem, 0);//page_n
                }
				if (pActive->iSubItem == 2)
                {
					OnChangeWndowName(pActive->iItem, 1);//nutton_n
                }
			}
		}
		break;
	}
}

void KUiWndPageSetCommon::OnChangeWndowName(int nWndIndex, int nKeyType)
{

    POINT MousePosition;
    int   nId;
    int   i;
    HMENU hMenu;
    char szWndName[128];
    szWndName[0] = 0;
    char szKeyName[128];
    szKeyName[0] = 0;
    char szMatchName[128];
    szMatchName[0] = 0;

    IIniFile *pKIniFile = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pKIniFile);

    if (nKeyType == 0) 
    {
        sprintf(szKeyName, "Page_%d", nWndIndex); 
        sprintf(szMatchName, "WndPage");
    }
    else if(nKeyType == 1)
    {
        sprintf(szKeyName, "CheckBox_%d", nWndIndex);  
        sprintf(szMatchName, "WndCheckBox");
    }

    nId =91000;
	hMenu = CreatePopupMenu();
    KG_PROCESS_ERROR(hMenu);
	while(g_Ui.GetNextUnit(szWndName))
	{
        char szWndType[128];
        pKIniFile->GetString(szWndName, "._WndType", "", szWndType, sizeof(szWndType));
        if (!strcmp(szWndType, szMatchName)) 
        {
            AppendMenu(hMenu, MF_ENABLED | MF_STRING, nId, szWndName);
        }
        else
        {
            AppendMenu(hMenu, MF_DISABLED | MF_GRAYED | MF_STRING, nId, szWndName);
        }
        nId++;
	}
    GetCursorPos(&MousePosition);
    int nSel = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NONOTIFY, 
        MousePosition.x, MousePosition.y, 0, m_hWnd, NULL);
	DestroyMenu(hMenu);
    szWndName[0] = 0;
    for (i = 91000; i <= nSel; i++) 
    {
        g_Ui.GetNextUnit(szWndName);
    }

    KG_PROCESS_ERROR(szWndName[0]);
    pKIniFile->WriteString(m_szName, szKeyName, szWndName);
    g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
    ShowErrorInformation();
    UpDataListView();

Exit0:
    return;
}

void KUiWndPageSetCommon::OnDelPage()
{
    IIniFile *pKIniFile = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pKIniFile);

    int nValue = 0;
    int i = 0;
    char szName[128];
    char szKeyValue[128];
    szKeyValue[0] = 0;

    pKIniFile->GetInteger(m_szName, "PageCount", 0, &nValue);
    if (nValue <= 0) 
    {
        nValue = 0;
        return;
    }
    else
    {
        nValue--;
    }

    szName[0] = 0;
    sprintf(szName, "Page_%d", m_nCurrentPageNum);
    pKIniFile->EraseKey(m_szName, szName);
    szName[0] = 0;
    sprintf(szName, "CheckBox_%d", m_nCurrentPageNum);
    pKIniFile->EraseKey(m_szName, szName);

    for (i = m_nCurrentPageNum; i < nValue; i++) 
    {
        szName[0] = 0;
        sprintf(szName, "Page_%d", i + 1);
        pKIniFile->GetString(m_szName, szName, "", szKeyValue, sizeof(szKeyValue));
        pKIniFile->EraseKey(m_szName, szName);
        szName[0] = 0;
        sprintf(szName, "Page_%d", i);
        pKIniFile->WriteString(m_szName, szName, szKeyValue);

        szName[0] = 0;
        sprintf(szName, "CheckBox_%d", i + 1);
        pKIniFile->GetString(m_szName, szName, "", szKeyValue, sizeof(szKeyValue));
        pKIniFile->EraseKey(m_szName, szName);
        szName[0] = 0;
        sprintf(szName, "CheckBox_%d", i);
        pKIniFile->WriteString(m_szName, szName, szKeyValue);

    }

    pKIniFile->WriteInteger(m_szName, "PageCount", nValue);
    g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
    ShowErrorInformation();
    UpDataListView();


Exit0:
    return;
}

void KUiWndPageSetCommon::OnCheckBoxClick(int nBtnId)
{
    int nValue = 0;
    char szName[128];

	switch(nBtnId)
	{
        case IDC_WND_PAGE_SET_ADD:
            g_Ui.GetIni()->GetInteger(m_szName, "PageCount", 0, &nValue);
            szName[0] = 0;
            sprintf(szName, "Page_%d", nValue);
            g_Ui.GetIni()->WriteString(m_szName, szName, "");
            szName[0] = 0;
            sprintf(szName, "CheckBox_%d", nValue);
            g_Ui.GetIni()->WriteString(m_szName, szName, "");
            nValue++;
            g_Ui.GetIni(true)->WriteInteger(m_szName, "PageCount", nValue);
            g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
            ShowErrorInformation();
            UpDataListView();
            break;
        case IDC_WND_PAGE_SET_DELETE:
            OnDelPage();
            break;
        case IDC_WND_PAGE_SET_MOVE_UP:
            OnMovePage(true);
            break;
        case IDC_WND_PAGE_SET_MOVE_DOWN:
            OnMovePage(false);
            break;
        default:
            break;
	}

}

void KUiWndPageSetCommon::OnMovePage(bool bUp)
{
    IIniFile *pKIniFile = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pKIniFile);
    char szKeyName[128];
    szKeyName[0] = 0;
    char szKeyTempValue[128];
    szKeyTempValue[0] = 0;
    char szKeyValue[128];
    szKeyValue[0] = 0;
    int nPageCount;
    pKIniFile->GetInteger(m_szName, "PageCount", 0, &nPageCount);
    KG_PROCESS_ERROR(nPageCount > 0);
    switch(bUp) 
    {
    case true:
        if (m_nCurrentPageNum < 1 || m_nCurrentPageNum >= nPageCount) 
            break;
        szKeyName[0] = 0;
        sprintf(szKeyName, "Page_%d", m_nCurrentPageNum - 1);
        pKIniFile->GetString(m_szName, szKeyName, "", szKeyValue, sizeof(szKeyValue));
        szKeyName[0] = 0;
        sprintf(szKeyName, "Page_%d", m_nCurrentPageNum);
        pKIniFile->GetString(m_szName, szKeyName, "", szKeyTempValue, 128);
        pKIniFile->WriteString(m_szName, szKeyName, szKeyValue);
        szKeyName[0] = 0;
        sprintf(szKeyName, "Page_%d", m_nCurrentPageNum - 1);
        pKIniFile->WriteString(m_szName, szKeyName, szKeyTempValue);

        szKeyName[0] = 0;
        sprintf(szKeyName, "CheckBox_%d", m_nCurrentPageNum - 1);
        pKIniFile->GetString(m_szName, szKeyName, "", szKeyValue, sizeof(szKeyValue));
        szKeyName[0] = 0;
        sprintf(szKeyName, "CheckBox_%d", m_nCurrentPageNum);
        pKIniFile->GetString(m_szName, szKeyName, "", szKeyTempValue, 128);
        pKIniFile->WriteString(m_szName, szKeyName, szKeyValue);
        szKeyName[0] = 0;
        sprintf(szKeyName, "CheckBox_%d", m_nCurrentPageNum - 1);
        pKIniFile->WriteString(m_szName, szKeyName, szKeyTempValue);
        m_nCurrentPageNum--;
    	break;
    case false:
        if (m_nCurrentPageNum < 0 || m_nCurrentPageNum >= nPageCount - 1) 
            break;
        szKeyName[0] = 0;
        sprintf(szKeyName, "Page_%d", m_nCurrentPageNum + 1);
        pKIniFile->GetString(m_szName, szKeyName, "", szKeyValue, sizeof(szKeyValue));
        szKeyName[0] = 0;
        sprintf(szKeyName, "Page_%d", m_nCurrentPageNum);
        pKIniFile->GetString(m_szName, szKeyName, "", szKeyTempValue, 128);
        pKIniFile->WriteString(m_szName, szKeyName, szKeyValue);
        szKeyName[0] = 0;
        sprintf(szKeyName, "Page_%d", m_nCurrentPageNum + 1);
        pKIniFile->WriteString(m_szName, szKeyName, szKeyTempValue);

        szKeyName[0] = 0;
        sprintf(szKeyName, "CheckBox_%d", m_nCurrentPageNum + 1);
        pKIniFile->GetString(m_szName, szKeyName, "", szKeyValue, sizeof(szKeyValue));
        szKeyName[0] = 0;
        sprintf(szKeyName, "CheckBox_%d", m_nCurrentPageNum);
        pKIniFile->GetString(m_szName, szKeyName, "", szKeyTempValue, 128);
        pKIniFile->WriteString(m_szName, szKeyName, szKeyValue);
        szKeyName[0] = 0;
        sprintf(szKeyName, "CheckBox_%d", m_nCurrentPageNum + 1);
        pKIniFile->WriteString(m_szName, szKeyName, szKeyTempValue);
        m_nCurrentPageNum++;
    	break;
    }
    g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
    ShowErrorInformation();
    UpDataListView();

Exit0:
    return;
}

void KUiWndPageSetCommon::UpDataListView()
{
    IIniFile *pKIniFile = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pKIniFile);

    int i;
    int nPageCount = 0;
	ListView_DeleteAllItems(m_hListWnd);
    pKIniFile->GetInteger(m_szName, "PageCount", 0, &nPageCount);
    for(i = 0; i < nPageCount; i++)
    {
        char  szPageName[128],szCheckBoxName[128];
        char  szPageKeyName[128],szCheckBoxKeyName[128];
        char  szTypeName[128];
        szPageName[0]      = 0;
        szCheckBoxName[0]    = 0;
        szPageKeyName[0]   = 0;
        szCheckBoxKeyName[0] = 0;
        szTypeName[0]      = 0;
        sprintf(szPageKeyName, "Page_%d", i);
        sprintf(szCheckBoxKeyName, "CheckBox_%d", i);
        sprintf(szTypeName, "第%d页:", i);        
        pKIniFile->GetString(m_szName, szPageKeyName, "", szPageName, sizeof(szPageName));
        pKIniFile->GetString(m_szName, szCheckBoxKeyName, "", szCheckBoxName, sizeof(szCheckBoxName));

		LV_ITEM			lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.mask = LVIF_TEXT;
		lvi.pszText		= (char*)szTypeName;
		lvi.cchTextMax  = strlen(szTypeName);
		lvi.iItem       = ListView_GetItemCount(m_hListWnd);
		lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);
		ListView_SetItemText(m_hListWnd, lvi.iItem, 1, szPageName);
		ListView_SetItemText(m_hListWnd, lvi.iItem, 2, szCheckBoxName);        
    }
    ListView_SetItemState(m_hListWnd,m_nCurrentPageNum,UINT(LVIS_SELECTED | LVIS_FOCUSED ),LVIS_SELECTED | LVIS_FOCUSED);
    ListView_EnsureVisible(m_hListWnd, m_nCurrentPageNum, FALSE);

Exit0:
    return;
}

void KUiWndPageSetCommon::Release()
{
	delete this;
}



void KUiWndPageSetCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(m_hWnd && pIni && pszName);
    strcpy(m_szName, pszName);

    char szKeyValue[256];
    szKeyValue[0]  = 0;
    int nKeyValue  = 0;
    int nKeyValue1 = 0;
    IIniFile *pKIniFile = g_Ui.GetIni(true);
    //UpData Self Private Property.
    int i;
    int nPageCount = 0;
	ListView_DeleteAllItems(m_hListWnd);
    pKIniFile->GetInteger(pszName, "PageCount", 0, &nPageCount);
    for(i = 0; i < nPageCount; i++)
    {
        char  szPageName[128],szCheckBoxName[128];
        char  szPageKeyName[128],szCheckBoxKeyName[128];
        char  szTypeName[128];
        szPageName[0]      = 0;
        szCheckBoxName[0]    = 0;
        szPageKeyName[0]   = 0;
        szCheckBoxKeyName[0] = 0;
        szTypeName[0]      = 0;
        sprintf(szPageKeyName, "Page_%d", i);
        sprintf(szCheckBoxKeyName, "CheckBox_%d", i);
        sprintf(szTypeName, "第%d页:", i);        
        pKIniFile->GetString(pszName, szPageKeyName, "", szPageName, sizeof(szPageName));
        pKIniFile->GetString(pszName, szCheckBoxKeyName, "", szCheckBoxName, sizeof(szCheckBoxName));

		LV_ITEM			lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.mask = LVIF_TEXT;
		lvi.pszText		= (char*)szTypeName;
		lvi.cchTextMax  = strlen(szTypeName);
		lvi.iItem       = ListView_GetItemCount(m_hListWnd);
		lvi.iItem		= ListView_InsertItem(m_hListWnd, &lvi);
		ListView_SetItemText(m_hListWnd, lvi.iItem, 1, szPageName);
		ListView_SetItemText(m_hListWnd, lvi.iItem, 2, szCheckBoxName);        
    }

Exit0:
    return;
}

void KUiWndPageSetCommon::Show(int bShow, int nSubWnd)
{
	KG_PROCESS_ERROR(m_hWnd);
	::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);

Exit0:
    return;
}

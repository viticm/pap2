////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KWndNameList.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-20 10:46:59
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MergeTga.h"
#include "KWndNameList.h"
#include "resource.h"
#include "KPickBox.h"
#include "KTgaManager.h"
#include "KSaveManager.h"
#include "KShow.h"


////////////////////////////////////////////////////////////////////////////////

KWndNameList *KWndNameList::ms_pself = NULL;

KWndNameList::KWndNameList()
{
    m_hWndParent = NULL;
    m_hWnd = NULL;
    m_hListWnd = NULL; 
    m_hInstance = NULL;
    m_nCurrentSortType = SORT_TYPE_BEGINE;
    m_nGetSelBlockFlag = false;
    m_nThePelaceBlockIndex = -1;
}

KWndNameList::~KWndNameList()
{
}

int KWndNameList::Init(HINSTANCE hInst, HWND hWnd)
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_ERROR(!ms_pself);
    ms_pself = new KWndNameList;
    KG_PROCESS_ERROR(ms_pself);
    nRetCode = ms_pself->CreateDialogWnd(hInst, hWnd);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        KWndNameList::Exit();
    return nResult;

}

int KWndNameList::Exit()
{
    if (ms_pself)
        ms_pself->DestroyDialogWnd();
    KG_DELETE(ms_pself);
    return true;
}

KWndNameList &KWndNameList::GetSelf()
{
    ASSERT(ms_pself);
    return *ms_pself;
}

LRESULT CALLBACK KWndNameList::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRet = 0;
    KWndNameList *pWndNameList = (KWndNameList *)GetWindowLong(hWnd, GWL_USERDATA);
    KG_PROCESS_ERROR(pWndNameList);

    switch(message) 
    {
    case WM_NOTIFY:
        {
			LPNMHDR pNotify = (LPNMHDR)lParam;
			if (pNotify->hwndFrom == pWndNameList->GetListHWND())
				pWndNameList->OnListNotify(pNotify);
        }
    	break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
            OnShowOrHideListNameWindow(pWndNameList->GetParentHWND());
        break;
    case WM_LBUTTONDOWN:
        pWndNameList->OnLButtonDown();
        break;
	case WM_MOUSEMOVE:
		PaintWindow(pWndNameList->GetParentHWND());
		break;
    default:
        break;
    }

Exit0:
    return lRet;
}

int KWndNameList::CreateDialogWnd(HINSTANCE hInst, HWND hWnd)
{
    int nResult = false;

    ASSERT(!m_hWnd);
	m_hWnd = ::CreateDialog(hInst, MAKEINTRESOURCE(IDD_UNMAKE_LIST), hWnd, (DLGPROC)WndProc);

    KG_PROCESS_ERROR(m_hWnd);
    ::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
    ::ShowWindow(m_hWnd, SW_SHOW);
    
    {
	    m_hListWnd = ::GetDlgItem(m_hWnd, IDC_NAME_LIST);
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
        _tcsncpy(szHeaderText, "还没有合并的TGA", sizeof(szHeaderText) / sizeof(TCHAR));
	    ListView_InsertColumn(m_hListWnd, 0, &lvc);
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

int KWndNameList::DestroyDialogWnd()
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

HWND KWndNameList::GetHWND()
{
    return m_hWnd;
}

HWND KWndNameList::GetListHWND()
{
    return m_hListWnd;
}

HWND KWndNameList::GetParentHWND()
{
    return m_hWndParent;
}

int KWndNameList::OnListNotify(LPNMHDR pNotify)
{
    int nResult = false;
    KG_PROCESS_ERROR(pNotify);
    
    switch(pNotify->code) 
    {
	case LVN_ITEMACTIVATE:
	case NM_CLICK:
		{
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
            int nIndex = KTgaManager::GetSelf().GetIndexByUnPosedIndex(pActive->iItem);
            KPickBox::GetSelf().Pick(nIndex);
		}
    	break;
    case NM_RCLICK:
		{
			KPickBox::GetSelf().Clear();
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
            OnRButtonClick(pActive->iItem);
		}
        break;
    case LVN_COLUMNCLICK:
        ++m_nCurrentSortType;
        if (m_nCurrentSortType > SORT_BY_SIZE)
            m_nCurrentSortType = SORT_BY_NAME;
        KPickBox::GetSelf().Clear();
        KTgaManager::GetSelf().Sort(m_nCurrentSortType);
        SetColumn();
        break;
    default:
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KWndNameList::SetColumn()
{
    int nResult = false;
    LPCTSTR pcszSortTypeName = NULL;

    KG_PROCESS_ERROR(m_hListWnd);

	RECT	rc;
	::GetClientRect(m_hListWnd, &rc);
	LVCOLUMN	lvc;
	TCHAR		szHeaderText[128];
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = rc.right - rc.left;
	lvc.pszText = szHeaderText;
	lvc.cchTextMax = 1024;
    
    pcszSortTypeName = g_GetSortTypeName(m_nCurrentSortType);
    if (!pcszSortTypeName)
        pcszSortTypeName = _T("");
    _sntprintf(
        szHeaderText, sizeof(szHeaderText) / sizeof(TCHAR), _T("%s%s"), _T("还没有合并的TGA"), pcszSortTypeName
    );
    ListView_SetColumn(m_hListWnd, 0, &lvc);

    nResult = true;
Exit0:
    return nResult;
}

int KWndNameList::Show()
{
    ::ShowWindow(m_hWnd, SW_SHOW);
    return true;
}

int KWndNameList::Hide()
{
    ::ShowWindow(m_hWnd, SW_HIDE);
    return true;
}

int KWndNameList::Append(LPCTSTR pcszName)
{
	LV_ITEM			lvi;
	memset(&lvi, 0, sizeof(LV_ITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText		= _T("");
	lvi.cchTextMax  = 1024;
	lvi.iItem       = ListView_GetItemCount(m_hListWnd);
	ListView_InsertItem(m_hListWnd, &lvi);
	ListView_SetItemText(m_hListWnd, lvi.iItem, 0, (LPTSTR)pcszName);  

    return true;
}

int KWndNameList::Remove(LPCTSTR pcszName)
{
    int nCout = ListView_GetItemCount(m_hListWnd);
    TCHAR szName[MAX_PATH];
    for (int i = 0; i < nCout; ++i) 
    {
        ListView_GetItemText(m_hListWnd, i, 0, szName, sizeof(szName) / sizeof(TCHAR));
        if (_tcsicmp(pcszName, szName) == 0)
        {
            ListView_DeleteItem(m_hListWnd, i);
            break;
        }
    }
    
    return true;
}

int KWndNameList::Clear()
{
    ListView_DeleteAllItems(m_hListWnd);
    return true;
}

int KWndNameList::OnRButtonClick(int nIndex)
{
    int nResult = false;
    int nRetCode = false;
    int nBlockIndex = -1;
    HMENU hMenu = NULL;
    POINT msPos;
    int nId = -1;
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hListWnd);
    KG_PROCESS_ERROR(m_hInstance);
    
    nBlockIndex = KTgaManager::GetSelf().GetIndexByUnPosedIndex(nIndex);
    KG_PROCESS_ERROR(nBlockIndex != -1);

    hMenu = ::LoadMenu((HINSTANCE)m_hInstance, MAKEINTRESOURCE(IDR_LIST_RCLICK_MENU));
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
    case IDM_LIST_RCLICK_EXPORT_TGA:
        nRetCode = OnExportTga(nBlockIndex);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case IDM_LIST_RCLICK_REPLACE:
        nRetCode = OnReplaceTgaBlock(nBlockIndex);
        KG_PROCESS_ERROR(nRetCode);
        break;
    case IDM_LIST_RCLICK_DELETE:
        nRetCode = KTgaManager::GetSelf().Remove(nBlockIndex);
        KG_PROCESS_ERROR(nRetCode);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KWndNameList::OnExportTga(int nBlockIndex)
{
    int nResult = false;
    int nRetCode = false;
	BROWSEINFO bi;
    ITEMIDLIST *pItemList = NULL;
    TCHAR szPath[MAX_PATH];

    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(nBlockIndex >= 0);

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

    nRetCode = KTgaManager::GetSelf().ExportTga(szPath, nBlockIndex);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KWndNameList::OnReplaceTgaBlock(int nBlockIndex)
{
    int nResult = false;

    KG_PROCESS_ERROR(nBlockIndex >= 0);
    KG_PROCESS_ERROR(m_hWnd);
    KG_PROCESS_ERROR(m_hWndParent);

    ::SetFocus(m_hWndParent);
    ::SetCapture(m_hWnd);

    m_nGetSelBlockFlag = true;
    m_nThePelaceBlockIndex = nBlockIndex;

    nResult = true;
Exit0:
    return nResult;
}

int KWndNameList::OnLButtonDown()
{
    int nResult = false;
    int nRetCode = false;
    int nPickedIndex = false;
    POINT msPos;

    if (::GetCapture() == m_hWnd)
        ::ReleaseCapture();

    KG_PROCESS_ERROR(m_nGetSelBlockFlag);
    KG_PROCESS_ERROR(m_hWndParent);
    KG_PROCESS_ERROR(m_nThePelaceBlockIndex >= 0);

    ::GetCursorPos(&msPos);
    ::ScreenToClient(m_hWndParent, &msPos);
    nPickedIndex = KTgaManager::GetSelf().GetPosedBlockIndex(
        msPos.x + KShow::GetSelf().GetShowStartX(), 
        msPos.y + KShow::GetSelf().GetShowStartY()
    );
    KG_PROCESS_ERROR(nPickedIndex >= 0);

    KG_PROCESS_ERROR(nPickedIndex != m_nThePelaceBlockIndex);
    nRetCode = KTgaManager::GetSelf().Replace(nPickedIndex, m_nThePelaceBlockIndex);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    m_nGetSelBlockFlag = false;
    m_nThePelaceBlockIndex = -1;
    return nResult;
}
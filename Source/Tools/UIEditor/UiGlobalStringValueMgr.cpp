////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiGlobalStringValueMgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-14 13:40:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "./UiGlobalStringValueMgr.h"
#include "resource.h"


////////////////////////////////////////////////////////////////////////////////

KUiGlobalStrMgr * KUiGlobalStrMgr::ms_pSelf = NULL;

KUiGlobalStrMgr::KUiGlobalStrMgr()
{
	m_hWnd = NULL;
	m_hWndList = NULL;
	m_szSelKey[0] = '\0';
	m_szSelValue[0] = '\0';
	m_nTabChange = false;
	m_szStringFileName[0] = '\0';
}

KUiGlobalStrMgr::~KUiGlobalStrMgr()
{
}

int KUiGlobalStrMgr::Init()
{
	int nResult = false;
	int nRetCode = false;

	ASSERT(!ms_pSelf);

	ms_pSelf = new KUiGlobalStrMgr;
	KG_PROCESS_ERROR(ms_pSelf);

	ms_pSelf->Load();

	nResult = true;
Exit0:
	if (!nResult)
		Exit();
	return nResult;
}

int KUiGlobalStrMgr::Exit()
{
	SAFE_DELETE(ms_pSelf);
	return true;
}

KUiGlobalStrMgr &KUiGlobalStrMgr::GetSelf()
{
	ASSERT(ms_pSelf);
	return *ms_pSelf;
}

BOOL CALLBACK KUiGlobalStrMgr::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int nResult = FALSE;

	switch(message) 
	{
	case WM_INITDIALOG:
		KUiGlobalStrMgr::GetSelf().InitShow(hWnd);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		nResult = TRUE;
		break;
	case WM_NOTIFY:
		KUiGlobalStrMgr::GetSelf().OnListNotify((LPNMHDR)lParam);
		break;
	case WM_COMMAND:
		{
			int nNotify = HIWORD(wParam);
			switch(nNotify)
			{
			case EN_CHANGE:
				KUiGlobalStrMgr::GetSelf().OnUpdataEditText(LOWORD(wParam));
				break;
			case 0:
				KUiGlobalStrMgr::GetSelf().OnButtonClick(LOWORD(wParam));
				break;
			}
		}
		break;
	default:
		break;
	}
	return nResult;
}

int KUiGlobalStrMgr::Load()
{
	int nResult = false;
	int nValueCount = 0;
	ITabFile *pTab = NULL;
	//::GetCurrentDirectory(MAX_PATH, m_szStringFileName);
	g_GetRootPath(m_szStringFileName);
	strcat(m_szStringFileName, "\\UI\\Scheme\\Case\\String.txt");
	KGLOBALSTRINFO StrInfo;
	TCHAR szCotent[1024];
	szCotent[0] = _T('\0');
	
	pTab = g_OpenTabFile(m_szStringFileName);
	KG_PROCESS_ERROR(pTab);

	nValueCount = pTab->GetHeight();
	for (int i = 2; i <= nValueCount; ++i)
	{
		pTab->GetString(i, "ID", "", StrInfo.szID, sizeof(StrInfo.szID));
		pTab->GetInteger(i, "Length", 0, &(StrInfo.nSize));
		pTab->GetString(i, "String", "", szCotent, sizeof(szCotent));
		StrInfo.strContent = szCotent;
		m_aGlobalStr.push_back(StrInfo);
	}

	nResult = true;
Exit0:
	KG_COM_RELEASE(pTab);
	return nResult;
}

int KUiGlobalStrMgr::InitShow(HWND hWnd)
{
	int nResult = false;
	RECT rcWindow;
	LVCOLUMN	lvc;
	char		szHeaderText[128];
	int nValueCount = 0;
	char szkey[128];
	szkey[0] = '\0';
	char szValue[1024];
	szValue[0] = '\0';
	char szLength[32];
	szLength[0] = '\0';

	KG_PROCESS_ERROR(hWnd);
	m_hWnd = hWnd;

	::GetWindowRect(m_hWnd, &rcWindow);
	::SetWindowPos(m_hWnd, HWND_TOP, 200, 200,
		rcWindow.right -rcWindow.left, rcWindow.bottom - rcWindow.top,
		SWP_SHOWWINDOW);

	m_hWndList = ::GetDlgItem(hWnd, IDC_STR_MGR_LIST);
	KG_PROCESS_ERROR(m_hWndList);
	ListView_SetExtendedListViewStyle(m_hWndList,
		ListView_GetExtendedListViewStyle(m_hWndList) |	LVS_EX_FULLROWSELECT);

	::GetClientRect(m_hWndList, &rcWindow);

	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = (rcWindow.right - rcWindow.left) / 5;
	lvc.pszText = szHeaderText;
	lvc.cchTextMax = 128;
	strcpy(szHeaderText, "ID:");
	ListView_InsertColumn(m_hWndList, 0, &lvc);

	lvc.cx = (rcWindow.right - rcWindow.left)  / 10;
	lvc.mask |= LVCF_SUBITEM;
	lvc.iSubItem = 1;
	strcpy(szHeaderText, "Length");
	ListView_InsertColumn(m_hWndList, 1, &lvc);

	lvc.cx = (rcWindow.right - rcWindow.left)  * 2;
	lvc.mask |= LVCF_SUBITEM;
	lvc.iSubItem = 1;
	strcpy(szHeaderText, "String");
	ListView_InsertColumn(m_hWndList, 2, &lvc); 

	UpDataShowFormStrArray();

	nResult = true;
Exit0:
	return nResult;
}

int KUiGlobalStrMgr::UpDataShowFormStrArray()
{
	int nResult = false;
	char szLength[32];
	szLength[0] = '\0';

	KG_PROCESS_ERROR(m_hWndList);

	ListView_DeleteAllItems(m_hWndList);

	{
		KGlobalStrArray::iterator it = m_aGlobalStr.begin();
		KGlobalStrArray::iterator ie = m_aGlobalStr.end();
		for (; it != ie; ++it)
		{
			LV_ITEM			lvi;
			memset(&lvi, 0, sizeof(LV_ITEM));
			lvi.mask = LVIF_TEXT;
			lvi.pszText		= (*it).szID;
			lvi.cchTextMax  = 128;
			lvi.iItem       = ListView_GetItemCount(m_hWndList);
			lvi.iItem		= ListView_InsertItem(m_hWndList, &lvi);

			sprintf(szLength, "%d", (*it).nSize);
			ListView_SetItemText(m_hWndList, lvi.iItem, 1, szLength);
			ListView_SetItemText(m_hWndList, lvi.iItem, 2, (LPTSTR)((*it).strContent.c_str()));
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KUiGlobalStrMgr::UpDataStrArrayFormShow()
{
	int nResult = false;
	int nCount = 0;
	KGLOBALSTRINFO StrInfo;
	TCHAR szLength[32];
	szLength[0] = _T('\0');
	TCHAR szValue[1024];
	szValue[0] = _T('\0');

	KG_PROCESS_ERROR(m_hWndList);

	m_aGlobalStr.clear();
	nCount = ListView_GetItemCount(m_hWndList);
	for (int i = 0; i < nCount; ++i)
	{
		ListView_GetItemText(m_hWndList, i, 0, StrInfo.szID, sizeof(StrInfo.szID));
		ListView_GetItemText(m_hWndList, i, 1, szLength, sizeof(szLength));
		StrInfo.nSize = (int)_ttoi(szLength);
		ListView_GetItemText(m_hWndList, i , 2, szValue, sizeof(szValue));
		StrInfo.strContent = szValue;

		m_aGlobalStr.push_back(StrInfo);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KUiGlobalStrMgr::OnListNotify(LPNMHDR pNotify)
{
	int nResult = false;

	KG_PROCESS_ERROR(pNotify);
	KG_PROCESS_SUCCESS(pNotify->hwndFrom != m_hWndList);

	switch(pNotify->code)
	{
	case LVN_ITEMACTIVATE:
	case NM_CLICK:
	case NM_RCLICK:
		{
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			if (pActive->iItem >= 0)
			{
				char szBuffer[1024];
				m_nCurrentEditItem = pActive->iItem;
				ListView_GetItemText(m_hWndList, m_nCurrentEditItem, 0, szBuffer, sizeof(szBuffer));
				::SetDlgItemText(m_hWnd, IDC_STR_MGR_ID, szBuffer);
				ListView_GetItemText(m_hWndList, m_nCurrentEditItem, 1, szBuffer, sizeof(szBuffer));
				::SetDlgItemText(m_hWnd, IDC_STR_MGR_LEN, szBuffer);
				ListView_GetItemText(m_hWndList, m_nCurrentEditItem, 2, szBuffer, sizeof(szBuffer));
				::SetDlgItemText(m_hWnd, IDC_STR_MGR_VALUE, szBuffer);
			}
		}
		break;
	case NM_DBLCLK:
		{
			LPNMITEMACTIVATE pActive = (LPNMITEMACTIVATE)pNotify;
			if (pActive->iItem >= 0)
				m_nCurrentEditItem = pActive->iItem;
		}
		OnSel();
		break;
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KUiGlobalStrMgr::OnUpdataEditText(int nId)
{
	int nResult = false;

	nResult = true;
	return nResult;
}

int KUiGlobalStrMgr::OnButtonClick(int nId)
{

	switch(nId)
	{
	case IDC_STR_MGR_DEL:
		 OnDel();
		 break;
	case IDC_STR_MGR_ADD:
		OnAdd();
		break;
	case IDC_STR_MGR_CHANGE:
		OnChange();
		break;
	case IDC_STR_MGR_CANCEL:
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		break;
	case IDC_STR_MGR_SEL:
		OnSel();
		break;
	}

	return true;
}


int KUiGlobalStrMgr::OnDel()
{
	int nResult = false;
	KG_PROCESS_ERROR(m_hWndList);
	KG_PROCESS_ERROR(m_nCurrentEditItem >= 0 && m_nCurrentEditItem < ListView_GetItemCount(m_hWndList));
	ListView_DeleteColumn(m_hWndList, m_nCurrentEditItem);
	m_nTabChange = true;
	nResult = true;
Exit0:
	return nResult;
}

int KUiGlobalStrMgr::OnAdd()
{
	int nResult = false;
	int nRetCode = false;
	char szKey[128];
	szKey[0] = '\0';
	char szValue[1024];
	szValue[0] = '\0';
	int nLenght = 0;

	KG_PROCESS_ERROR(m_hWndList);

	::GetDlgItemText(m_hWnd, IDC_STR_MGR_ID, szKey, sizeof(szKey));
	::GetDlgItemText(m_hWnd, IDC_STR_MGR_VALUE, szValue, sizeof(szValue));
	nLenght = ::GetDlgItemInt(m_hWnd, IDC_STR_MGR_LEN, NULL, false);
	
	nRetCode = Add(szKey, nLenght, szValue);
	KG_PROCESS_ERROR(nRetCode);
	m_nTabChange = true;
	nResult = true;
Exit0:
	return nResult;
}

int KUiGlobalStrMgr::Add(const char *pcszKey, int nLength, const char *pcszValue)
{
	int nResult = false;
	char szLength[16];
	szLength[0] = '\0';
	LV_ITEM			lvi;

	KG_PROCESS_ERROR(pcszKey);
	KG_PROCESS_ERROR(pcszValue);
	if (!pcszKey[0])
	{
		::MessageBox(m_hWnd, "ID不能为空！", "不能添加：", MB_OK | MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}
	if (IskeyExist(pcszKey))
	{
		::MessageBox(m_hWnd, "该ID已经存在！", "不能添加：", MB_OK | MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}
	if (!IsValueLegality(pcszValue))
	{
		::MessageBox(m_hWnd, "该String中存在非法字符，比如Tab或者Enter！", "不能添加：", MB_OK | MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}

	sprintf(szLength, "%d", nLength);

	memset(&lvi, 0, sizeof(LV_ITEM));
	lvi.mask = LVIF_TEXT;
	lvi.pszText		= (char*)pcszKey;
	lvi.cchTextMax  = 128;
	lvi.iItem       = ListView_GetItemCount(m_hWndList);
	lvi.iItem		= ListView_InsertItem(m_hWndList, &lvi);
	ListView_SetItemText(m_hWndList, lvi.iItem, 1, szLength);
	ListView_SetItemText(m_hWndList, lvi.iItem, 2, (char *)pcszValue);	

	nResult = true;
Exit0:
	return nResult;
}

int KUiGlobalStrMgr::OnChange()
{
	int nResult = false;
	int nRetCode = false;
	int nCount = 0;
	char szKey[128];
	szKey[0] = '\0';
	char szValue[1024];
	szValue[0] = '\0';
	char szLength[16];
	szLength[0] = '\0';
	char szOrigalKey[128];
	szOrigalKey[0] = '\0';

	KG_PROCESS_ERROR(m_hWndList);
	nCount = ListView_GetItemCount(m_hWndList);
	KG_PROCESS_ERROR(m_nCurrentEditItem >= 0 && m_nCurrentEditItem < nCount);

	::GetDlgItemText(m_hWnd, IDC_STR_MGR_ID, szKey, sizeof(szKey));
	::GetDlgItemText(m_hWnd, IDC_STR_MGR_VALUE, szValue, sizeof(szValue));
	::GetDlgItemText(m_hWnd, IDC_STR_MGR_LEN, szLength, sizeof(szLength));

	ListView_GetItemText(m_hWndList, m_nCurrentEditItem, 0, szOrigalKey, sizeof(szOrigalKey));
	if (!szKey[0])
	{
		::MessageBox(m_hWnd, "ID不能为空！", "不能更改：", MB_OK | MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}
	if (_stricmp(szKey, szOrigalKey) != 0  && IskeyExist(szKey))
	{
		::MessageBox(m_hWnd, "该ID已经被其他值！", "不能更改：", MB_OK | MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}
	if (!IsValueLegality(szValue))
	{
		::MessageBox(m_hWnd, "该String中存在非法字符，比如Tab或者Enter！", "不能更改：", MB_OK | MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}
	if (!szLength[0])
	{
		::MessageBox(m_hWnd, "请指定字符串的长度！", "不能更改：", MB_OK | MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}

	ListView_SetItemText(m_hWndList, m_nCurrentEditItem, 0, szKey);
	ListView_SetItemText(m_hWndList, m_nCurrentEditItem, 1, szLength);
	ListView_SetItemText(m_hWndList, m_nCurrentEditItem, 2, szValue);
	m_nTabChange = true;

	nResult = true;
Exit0:
	return true;
}

int KUiGlobalStrMgr::Save()
{
	int nResult = false;
	FILE * fp = NULL;
	fp = fopen(m_szStringFileName, "w");
	KG_PROCESS_ERROR(fp);

	fprintf(fp, "%s\t%s\t%s\n", "ID", "Length", "String");

	{
		KGlobalStrArray::iterator it = m_aGlobalStr.begin();
		KGlobalStrArray::iterator ie = m_aGlobalStr.end();
		for (; it != ie; ++it)
		{
			fprintf(fp, "%s\t%d\t%s\n", (*it).szID, (*it).nSize, (*it).strContent.c_str());
		}
	}

	fflush(fp);

	nResult = true;
Exit0:
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
	return nResult;
}

int KUiGlobalStrMgr::OnSel()
{
	if (m_nTabChange && m_hWndList)
	{
		UpDataStrArrayFormShow();
		Save();
	}

	ListView_GetItemText(m_hWndList, m_nCurrentEditItem, 0, m_szSelKey, sizeof(m_szSelKey));
	PostMessage(m_hWnd, WM_CLOSE, 0, 0);

	g_pfnUIBehavior(PCB_UI_RELOAD_STRING_SCHEME, (WPARAM)0, (LPARAM)0);

	return true;
}

const char *KUiGlobalStrMgr::GetString(const char *pcszKey)
{
	if (!pcszKey)
		return NULL;

	KGlobalStrArray::iterator it = m_aGlobalStr.begin();
	KGlobalStrArray::iterator ie = m_aGlobalStr.end();
	for (; it != ie; ++it)
	{
		if (strcmp(pcszKey, (*it).szID) == 0)
		{
			return (*it).strContent.c_str();
		}
	}

	return NULL;
}

int KUiGlobalStrMgr::IskeyExist(const char *pcszkey)
{
	int nResult = false;
	int ncount = 0;
	char szValueKey[128];
	szValueKey[0] = '\0';
	KG_PROCESS_ERROR(pcszkey);
	KG_PROCESS_ERROR(m_hWndList);

	ncount = ListView_GetItemCount(m_hWndList);
	for (int i = 0; i < ncount; ++i)
	{
		ListView_GetItemText(m_hWndList, i, 0, szValueKey, sizeof(szValueKey));
		if (_stricmp(szValueKey, pcszkey) == 0)
			KG_PROCESS_SUCCESS(true);
	}
	KG_PROCESS_ERROR(false);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KUiGlobalStrMgr::IsValueLegality(const char *pcszKey)
{
	int nResult = false;

	while (*pcszKey)
	{
		KG_PROCESS_ERROR((*pcszKey) != '\t' && (*pcszKey) != '\n');
		++pcszKey;
	}

	nResult = true;
Exit0:
	return nResult;
}

const char * KUiGlobalStrMgr::SelString(HWND hWndParent)
{
	m_szSelKey[0] = '\0';
	::DialogBox(
		(HINSTANCE)g_hPluginInstance, 
		MAKEINTRESOURCE(IDD_StrMgr), 
		hWndParent, (DLGPROC)DlgProc
	);
	return m_szSelKey;
}

int KUiGlobalStrMgr::StringManage(HWND hParent)
{
	m_szSelKey[0] = '\0';
	::DialogBox(
		(HINSTANCE)g_hPluginInstance, 
		MAKEINTRESOURCE(IDD_StrMgr), 
		hParent, (DLGPROC)DlgProc
		);
	return true;
}


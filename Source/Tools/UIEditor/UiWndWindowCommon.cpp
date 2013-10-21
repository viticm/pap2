#include "Stdafx.h"
#include "UiWndWindowCommon.h"
#include "resource.h"
#include "UiEditor.h"
#include "ClientManage.h"
#include "Ui.h"
#include "UiGlobalStringValueMgr.h"
#include "UiItemImageCommon.h"

KUiWndWindowCommon::KUiWndWindowCommon()
{
     // __FUNCDNAME__
	m_szName[0] = 0;
}

KUiWndWindowCommon::~KUiWndWindowCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndWindowCommon::Create()
{
	KUiWndWindowCommon* pWnd = new KUiWndWindowCommon;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndWindowCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL)
		return FALSE;

	strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndWindow), hWnd, (DLGPROC)WndProc);
	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
			pRc->right - pRc->left, pRc->bottom - pRc->top,
			SWP_SHOWWINDOW);
		::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

		static const char *pcszTipType[3] = 
		{
			"显示在窗口旁边",
			"跟随鼠标移动",
			"显示在固定位置",
		};
		HWND hwndCombo = ::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE);
		::SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0); 
		for (int i = 0; i < 3; ++i)
		{
			::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)(pcszTipType[i])); 
		}

        // up down button
        HWND hSpin = ::GetDlgItem(m_hWnd, IDC_WND_X_CHANGE);
        if (hSpin) 
        {
            SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
        }

        hSpin = ::GetDlgItem(m_hWnd, IDC_WND_Y_CHANGE);
        if (hSpin) 
        {
            SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
        }

        hSpin = ::GetDlgItem(m_hWnd, IDC_WND_CX_CHANGE);
        if (hSpin) 
        {
            SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
        }

        hSpin = ::GetDlgItem(m_hWnd, IDC_WND_CY_CHANGE);
        if (hSpin) 
        {
            SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
        }
	}
	else
	{
		return FALSE;
	}	
	return TRUE;
}

void KUiWndWindowCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndWindowCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiWndWindowCommon*	pView = (KUiWndWindowCommon*)GetWindowLong(hWnd, GWL_USERDATA);
	if (!pView)
		return 0;
	if (message == WM_COMMAND)
	{
		int nNotify = HIWORD(wParam);
		switch(nNotify)
		{
		case EN_CHANGE:
			pView->OnUpdateEditText(LOWORD(wParam));
			break;
		case 0:
			pView->OnButtonClick(LOWORD(wParam));
			break;
		case CBN_SELCHANGE:
			pView->OnUpdataComboBox(LOWORD(wParam));
			break;
		}
	}
	return 0;
}

void KUiWndWindowCommon::OnButtonClick(int nBtnId)
{
	int	x, y;
	bool bUpdateShower = false;
	switch(nBtnId)
	{
	case IDC_WND_MOVEABLE:
		{
			int nChecked = ::IsDlgButtonChecked(m_hWnd, IDC_WND_MOVEABLE) == BST_CHECKED;
			g_Ui.GetIni()->WriteInteger(m_szName, "Moveable", nChecked);
			bUpdateShower = true;
		}
		break;
	case IDC_WND_FOLLOW_MOVE_X:
		{
			x = ::IsDlgButtonChecked(m_hWnd, IDC_WND_FOLLOW_MOVE_X) == BST_CHECKED;
            y = ::IsDlgButtonChecked(m_hWnd, IDC_WND_FOLLOW_MOVE_Y) == BST_CHECKED;
		}
		break;
	case IDC_WND_FOLLOW_MOVE_Y:
		{
			x = ::IsDlgButtonChecked(m_hWnd, IDC_WND_FOLLOW_MOVE_X) == BST_CHECKED;
            y = ::IsDlgButtonChecked(m_hWnd, IDC_WND_FOLLOW_MOVE_Y) == BST_CHECKED;
		}
		break;
	case IDC_WND_FOLLOW_SIZE_X:
		{
			x = ::IsDlgButtonChecked(m_hWnd, IDC_WND_FOLLOW_SIZE_X) == BST_CHECKED;
            y = ::IsDlgButtonChecked(m_hWnd, IDC_WND_FOLLOW_SIZE_Y) == BST_CHECKED;
		}
		break;
	case IDC_WND_DISABLE_BRING_TO_TOP:
		{
			int nChecked = ::IsDlgButtonChecked(m_hWnd, IDC_WND_DISABLE_BRING_TO_TOP) == BST_CHECKED;
			g_Ui.GetIni()->WriteInteger(m_szName, "DisableBringToTop", nChecked);
			bUpdateShower = true;
		}
		break;
	case IDC_WND_FOLLOW_SIZE_Y:
		{
			x = ::IsDlgButtonChecked(m_hWnd, IDC_WND_FOLLOW_SIZE_X) == BST_CHECKED;
            y = ::IsDlgButtonChecked(m_hWnd, IDC_WND_FOLLOW_SIZE_Y) == BST_CHECKED;
		}
		break;
	case IDC_WND_DUMMY_WND:
		{
			int nChecked = ::IsDlgButtonChecked(m_hWnd, IDC_WND_DUMMY_WND) == BST_CHECKED;
			g_Ui.GetIni()->WriteInteger(m_szName, "DummyWnd", nChecked);
			bUpdateShower = true;
		}
		break;
    case IDC_WND_MOUSE_PENETRABLE:
		{
			int nChecked = ::IsDlgButtonChecked(m_hWnd, IDC_WND_MOUSE_PENETRABLE) == BST_CHECKED;
			g_Ui.GetIni()->WriteInteger(m_szName, "MousePenetrable", nChecked);
			bUpdateShower = true;
		}
		break;
	case IDC_WND_BREATHE_WHEN_HIDE:
		{
			int nChecked = ::IsDlgButtonChecked(m_hWnd, IDC_WND_BREATHE_WHEN_HIDE) == BST_CHECKED;
			g_Ui.GetIni()->WriteInteger(m_szName, "BreatheWhenHide", nChecked);
			bUpdateShower = true;
		}
		break;
	case IDC_IMAGE_GET_POS_FORM_SHOWER:
    case IDC_WND_COM_GET_POS:
        g_ClientManager.EnableUpDataData(false);
        if(g_ClientManager.GetPositonFromClient(m_szName, &x, &y))
        {
			::SetDlgItemInt(m_hWnd, IDC_WND_POS_LEFT, x, true);
			::SetDlgItemInt(m_hWnd, IDC_WND_POS_TOP, y, true);
        }
        g_ClientManager.EnableUpDataData(true);
		break;
    case IDC_WND_COM_GET_SIZE:
        g_ClientManager.EnableUpDataData(false);
        if(g_ClientManager.GetSizeFromClient(m_szName, &x, &y))
        {
			::SetDlgItemInt(m_hWnd, IDC_WND_SIZE_WIDTH, x, true);
			::SetDlgItemInt(m_hWnd, IDC_WND_SIZE_HEIGHT, y, true);
        }
        g_ClientManager.EnableUpDataData(true);
        break;
	case IDC_WND_TIP_SEL:
		{
			const char *pszTip = KUiGlobalStrMgr::GetSelf().SelString(m_hWnd);
			if (pszTip && pszTip[0])
				::SetDlgItemText(m_hWnd, IDC_ITEM_TIP, pszTip);
		}
		break;
	case IDC_WINDOW_TIP_RICHTEXT:
		if (::IsDlgButtonChecked(m_hWnd, IDC_WINDOW_TIP_RICHTEXT))
			g_Ui.GetIni()->WriteInteger(m_szName, "TipRichText", 1);
		else
			g_Ui.GetIni()->WriteInteger(m_szName, "TipRichText", 0);
		break;
	case IDC_WINDOW_TIP_SHOW_DIR:
		if (::IsDlgButtonChecked(m_hWnd, IDC_WINDOW_TIP_SHOW_DIR))
			g_Ui.GetIni()->WriteInteger(m_szName, "OrgTip", 1);
		else
			g_Ui.GetIni()->WriteInteger(m_szName, "OrgTip", 0);
		break;
	case IDC_WND_MULTIFRAME:
		if (::IsDlgButtonChecked(m_hWnd, IDC_WND_MULTIFRAME))
			g_Ui.GetIni()->WriteInteger(m_szName, "MultiFrame", 1);
		else
			g_Ui.GetIni()->EraseKey(m_szName, "MultiFrame");
		break;
	case IDC_WINDOW_AREA_SEL:
		{
			OnChangeArea();
		}
		break;
	case IDC_WINDOW_AREA_CLEAR:
		{
			::SetDlgItemText(m_hWnd, IDC_WINDOW_AREA, "");
			g_Ui.GetIni(true)->EraseKey(m_szName, "AreaFile");
		}
		break;
	}	
	if (bUpdateShower)
	{
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

char * KUiWndWindowCommon::EquirotalStrStr(const char *pString, const char *pStrSearch)
{
	const char *pszResult = NULL;
	KG_PROCESS_ERROR(pString);
	KG_PROCESS_ERROR(pStrSearch);

	char *pStringCopy = new char[strlen(pString) + 10];
	KG_PROCESS_ERROR(pStringCopy);
	char *pStrSearchCopy = new char[strlen(pStrSearch) + 10];
	KG_PROCESS_ERROR(pStrSearchCopy);
	strcpy(pStringCopy, pString);
	strcpy(pStrSearchCopy, pStrSearch);

	char *pChange = pStringCopy;
	while (*pChange != '\0') 
	{
		if(*pChange >= 'a' && *pChange <= 'z') 
			*pChange += 'A' - 'a';
		if (*pChange == '\\')
			*pChange = '/';
		pChange++;
	}

	pChange = pStrSearchCopy;
	while (*pChange != '\0') 
	{
		if(*pChange >= 'a' && *pChange <= 'z') 
			*pChange += 'A' - 'a';
		if (*pChange == '\\')
			*pChange = '/';
		pChange++;
	}
	char *pStrSearchResult = strstr(pStringCopy, pStrSearchCopy);
	KG_PROCESS_ERROR(pStrSearchResult);

	pszResult = pString + (pStrSearchResult - pStringCopy); 
Exit0:
	if (pStringCopy) 
	{
		delete[] pStringCopy;
		pStringCopy = NULL;
	}
	if (pStrSearchCopy) 
	{
		delete[] pStrSearchCopy;
		pStrSearchCopy = NULL;
	}
	return (char *)pszResult;
}

int KUiWndWindowCommon::OnChangeArea()
{
	OPENFILENAME	ofn;
	char			szFile[MAX_PATH] = "", szRoot[MAX_PATH];
	szFile[0] = '\0';
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR |
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
	ofn.lpstrTitle = "请选择界面图文件";
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile = szFile;
	ofn.lpstrFilter = _T("Area文件\0*.area\0\0");
	if (KUiItemImageCommon::ms_szLastImgPath[0])
		ofn.lpstrInitialDir = KUiItemImageCommon::ms_szLastImgPath;
	if (!GetOpenFileName(&ofn))
		return false;

	char* pSplit = strrchr(szFile, '\\');
	*pSplit = 0;
	strcpy(KUiItemImageCommon::ms_szLastImgPath, szFile);
	*pSplit = '\\';
	pSplit = strrchr(szFile, '.');
	g_GetRootPath(szRoot);

	if (EquirotalStrStr(szFile, szRoot) != szFile)
	{
		char szMsg[512];
		sprintf(szMsg, "文件:%s必须在游戏根目录%s范围内!", szFile, szRoot);
		MessageBox(m_hWnd, szMsg, "提示", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	int nLen = strlen(szRoot) + 1;
	::SetDlgItemText(m_hWnd, IDC_WINDOW_AREA, szFile + nLen);
	g_Ui.GetIni(true)->WriteString(m_szName, "AreaFile", szFile + nLen);

	return true;
}


void KUiWndWindowCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	int nRetCode = false;
	char szValue[512];
	if (!m_hWnd)
		return;
	strcpy(m_szName, pszName);
	::SetDlgItemInt(m_hWnd, IDC_WND_POS_LEFT, data.Left, true);
	::SetDlgItemInt(m_hWnd, IDC_WND_POS_TOP, data.Top, true);
	::SetDlgItemInt(m_hWnd, IDC_WND_SIZE_WIDTH, data.Width, false);
	::SetDlgItemInt(m_hWnd, IDC_WND_SIZE_HEIGHT, data.Height, false);
	int	nValue;
    int x, y;
	pIni->GetInteger(pszName, "Moveable",  0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_MOVEABLE, nValue ? BST_CHECKED : 0);
	pIni->GetInteger(pszName, "DummyWnd",  0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_AUTOSIZE, nValue ? BST_CHECKED : 0);
	::CheckDlgButton(m_hWnd, IDC_WND_DUMMY_WND, nValue ? BST_CHECKED : 0);
	pIni->GetInteger(pszName, "MousePenetrable",  0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_MOUSE_PENETRABLE, nValue ? BST_CHECKED : 0);

	pIni->GetInteger(pszName, "BreatheWhenHide",  0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_BREATHE_WHEN_HIDE, nValue ? BST_CHECKED : 0);
	

    x = 0;
    y = 0;
	pIni->GetInteger2(pszName, "FollowMove", &x, &y);
	::CheckDlgButton(m_hWnd, IDC_WND_FOLLOW_MOVE_X, x? BST_CHECKED : 0);
	::CheckDlgButton(m_hWnd, IDC_WND_FOLLOW_MOVE_Y, y? BST_CHECKED : 0);

    x = 0;
    y = 0;
	pIni->GetInteger2(pszName, "FollowSize", &x, &y);
	::CheckDlgButton(m_hWnd, IDC_WND_FOLLOW_SIZE_X, x? BST_CHECKED : 0);
	::CheckDlgButton(m_hWnd, IDC_WND_FOLLOW_SIZE_Y, y? BST_CHECKED : 0);

	TCHAR szTreePath[256] = { 0 };
    if(g_Ui.GetUnitTreePath(szTreePath, sizeof(szTreePath), m_szName))
        ::SetDlgItemText(m_hWnd, IDC_EDIT_WND_TREE_PATH, szTreePath);

	nRetCode = pIni->GetString(pszName, "$Tip", "", szValue, sizeof(szValue));
	if (nRetCode && szValue[0])
	{
		::SetDlgItemText(m_hWnd, IDC_ITEM_TIP, szValue);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_WINDOW_TIP_RICHTEXT), TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE), TRUE);
		pIni->GetInteger(pszName, "ShowTipType", 0, &nValue);
		HWND hwndCombo = ::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE);
		::SendMessage(hwndCombo, CB_SETCURSEL, nValue, 0);	
		pIni->GetInteger(pszName, "TipRichText", 0, &nValue);
		if (nValue)
			::CheckDlgButton(m_hWnd, IDC_WINDOW_TIP_RICHTEXT, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_WINDOW_TIP_RICHTEXT, BST_UNCHECKED);
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_WINDOW_TIP_RICHTEXT), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE), FALSE);
	}

	pIni->GetInteger(pszName, "MultiFrame", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_WND_MULTIFRAME, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_WND_MULTIFRAME, BST_UNCHECKED);

	pIni->GetInteger(pszName, "DisableBringToTop", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_WND_DISABLE_BRING_TO_TOP, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_WND_DISABLE_BRING_TO_TOP, BST_UNCHECKED);

	pIni->GetInteger(pszName, "OrgTip", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_WINDOW_TIP_SHOW_DIR, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_WINDOW_TIP_SHOW_DIR, BST_UNCHECKED);

	pIni->GetString(pszName, "AreaFile", "", szValue, sizeof(szValue));
	::SetDlgItemText(m_hWnd, IDC_WINDOW_AREA, szValue);

}       

void KUiWndWindowCommon::OnUpdataComboBox(int nId)
{
	int nValue = 0;
	int nUpdateShower = true;
	LRESULT lRet = CB_ERR;
	switch(nId)
	{
	case IDC_ITEM_SHOW_TIP_TYPE:
		{
			HWND hwndCombo = ::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE);
			KG_PROCESS_ERROR(hwndCombo);
			lRet = ::SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
			KG_PROCESS_ERROR(lRet != CB_ERR);
			g_Ui.GetIni()->WriteInteger(m_szName, "ShowTipType", (int)lRet);
		}
		break;
	default:
		nUpdateShower = false;
		break;
	}

	if (nUpdateShower)
	{
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
		ShowErrorInformation();
	}

Exit0:
	return;
}

void KUiWndWindowCommon::OnUpdateEditText(int nId)
{
	if (!g_Ui.GetIni())
		return;

    const char *pszKey = NULL;
    int nValue = 0;
	switch(nId)
	{
	case IDC_WND_POS_LEFT:
        nValue = ::GetDlgItemInt(m_hWnd, nId, NULL, true);
        pszKey = "Left";
		break;
	case IDC_WND_POS_TOP:
        nValue = ::GetDlgItemInt(m_hWnd, nId, NULL, true);
        pszKey = "Top";
		break;
	case IDC_WND_SIZE_WIDTH:
        nValue = ::GetDlgItemInt(m_hWnd, nId, NULL, true);
        pszKey = "Width";
		break;
	case IDC_WND_SIZE_HEIGHT:
        nValue = ::GetDlgItemInt(m_hWnd, nId, NULL, true);
        pszKey = "Height";
		break;
	case IDC_WINDOW_AREA:
		{
			char szValue[MAX_PATH];
			szValue[0] = '\0';
			::GetDlgItemText(m_hWnd, IDC_WINDOW_AREA, szValue, sizeof(szValue));
			if (szValue[0])
			{
				g_Ui.GetIni()->WriteString(m_szName, "AreaFile", szValue);
			}
			else
			{
				g_Ui.GetIni()->EraseKey(m_szName, "AreaFile");
			}
			g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
			ShowErrorInformation();
		}
		break;
	case IDC_ITEM_TIP:
		{
			char szBuffer[512];
			szBuffer[0] = '\0';
			::GetDlgItemText(m_hWnd, IDC_ITEM_TIP, szBuffer, sizeof(szBuffer));
			if (szBuffer[0])
			{
				g_Ui.GetIni()->WriteString(m_szName, "$Tip", szBuffer);
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE), TRUE);
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_WINDOW_TIP_RICHTEXT), TRUE);
				OnUpdataComboBox(IDC_ITEM_SHOW_TIP_TYPE);
			}
			else
			{
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE), FALSE);
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_WINDOW_TIP_RICHTEXT), FALSE);
				g_Ui.GetIni()->EraseKey(m_szName, "$Tip");
				g_Ui.GetIni()->EraseKey(m_szName, "ShowTipType");
				g_Ui.GetIni()->EraseKey(m_szName, "TipRichText");
			}
		}
		break;
	}

    if (pszKey) 
    {
        g_Ui.GetIni()->WriteInteger(m_szName, pszKey, nValue);
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
    }

}



void KUiWndWindowCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	}
}

void KUiWndWindowCommon::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 1;
    strcpy(pPageInfo->m_szPage[0], "窗口属性");
Exit0:
    return;
}

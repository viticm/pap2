////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 9:44:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemCommon.h"
#include "KGPublic.h"      
#include "UiEditor.h"
#include "resource.h"
#include "ClientManage.h"
#include "UiItemHandleCommon.h"
#include "UiGlobalStringValueMgr.h"
#include "UiItemImageCommon.h"

////////////////////////////////////////////////////////////////////////////////


KUiItemCommon::KUiItemCommon()
{
    m_szName[0] = '\0';
}

KUiItemCommon::~KUiItemCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemCommon::Create()
{
	KUiItemCommon* pWnd = new KUiItemCommon;
	return pWnd;
}

int KUiItemCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult = false;
    HWND hwndCombo = NULL;
    HWND hSpin = NULL;

    KG_PROCESS_ERROR(hWnd);
    KG_PROCESS_ERROR(pRc);
    KG_PROCESS_ERROR(pszName);

    strncpy(m_szName, pszName, sizeof(m_szName));
    m_hWnd = ::CreateDialog(
        (HINSTANCE)g_hPluginInstance, 
        MAKEINTRESOURCE(IDD_ItemNullCommon), 
        hWnd, (DLGPROC)WndProc
    );
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

    //初始化位置选项
    static const char *pcszPosType[POSITION_END] = 
    {
        "指定坐标",
        "左下", "左上", "左中",
        "上左", "上右", "上中",
        "右上", "右下", "右中",
        "下左", "下右", "下中",
    };
    hwndCombo = ::GetDlgItem(m_hWnd, IDC_ITEM_POS_TYPE);
    KG_PROCESS_ERROR(hwndCombo);
    ::SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0); 
    for (int i = POSITION_BEGINE; i < POSITION_END; ++i)
    {
        ::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)(pcszPosType[i])); 
    }

	static const char *pcszTipType[3] = 
	{
		"显示在控件旁边",
		"跟随鼠标移动",
		"显示在固定位置",
	};
	hwndCombo = ::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE);
	KG_PROCESS_ERROR(hwndCombo);
	::SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0); 
	for (int i = 0; i < 3; ++i)
	{
		::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)(pcszTipType[i])); 
	}

    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_POS_LEFT_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));

    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_POS_TOP_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));

    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_WIDTH_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));

    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_HEIGHT_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
    

    nResult = true;
Exit0:
    if (!nResult)
        Release();
    return nResult;
}

void KUiItemCommon::Release()
{
    delete this;
}

LRESULT CALLBACK KUiItemCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemCommon*	pView = (KUiItemCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiItemCommon::OnButtonClick(int nBtnId)
{
    int nUpdateShower = true;
    int nRetCode = false;
    int nValue = 0;
    int x, y;
    switch(nBtnId)
    {
    case IDC_ITEM_AUTO_SIZE:
        {
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_AUTO_SIZE);
            if (nRetCode)
            {
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_WIDTH), FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_HEIGHT), FALSE);
                g_Ui.GetIni()->EraseKey(m_szName, "Width");
                g_Ui.GetIni()->EraseKey(m_szName, "Height");
            }
            else
            {
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_WIDTH), TRUE);
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_HEIGHT), TRUE);
                nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_WIDTH, NULL, false);
                g_Ui.GetIni()->WriteInteger(m_szName, "Width", nValue);
                nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_HEIGHT, NULL, false);
                g_Ui.GetIni()->WriteInteger(m_szName, "Height", nValue);
            }
        }
        break;
    case IDC_ITEM_ACCEPT_MESSAGE:
        {
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_ACCEPT_MESSAGE);
            if (nRetCode)
            {
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_EVENT_NAME), TRUE); 
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONDOWN), TRUE); 
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONDOWN), TRUE); 
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONUP), TRUE);  
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONUP), TRUE);   
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONCLICK), TRUE);     
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONCLICK), TRUE);     
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONDBCLICK), TRUE);   
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONDBCLICK), TRUE);   
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSEENTER), TRUE);       
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSELEAVE), TRUE);       
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSEMOVE), TRUE);        
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSEWHEEL), TRUE);       
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_KEYDOWN), TRUE);  
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_KEYUP), TRUE); 

				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONDOWN), TRUE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONUP), TRUE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONCLICK), TRUE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONDBCLICK), TRUE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONDRAG), TRUE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONDRAG), TRUE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONDRAG), TRUE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSEHOVER), TRUE); 


                nValue = 0;
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONDOWN);
                if (nRetCode)
                    nValue = 1;
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONDOWN);
                if (nRetCode)
                    nValue += 2;
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONUP);
                if (nRetCode)
                    nValue += (2 << 1);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONUP);
                if (nRetCode)
                    nValue += (2 << 2);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONCLICK);
                if (nRetCode)
                    nValue += (2 << 3);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONCLICK);
                if (nRetCode)
                    nValue += (2 << 4);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONDBCLICK);
                if (nRetCode)
                    nValue += (2 << 5);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONDBCLICK);
                if (nRetCode)
                    nValue += (2 << 6);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSEENTER);
                if (nRetCode)
                    nValue += (2 << 7);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSELEAVE);
                if (nRetCode)
                    nValue += (2 << 8);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSEMOVE);
                if (nRetCode)
                    nValue += (2 << 9);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSEWHEEL);
                if (nRetCode)
                    nValue += (2 << 10);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_KEYDOWN);
                if (nRetCode)
                    nValue += (2 << 11);
                nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_KEYUP);
                if (nRetCode)
                    nValue += (2 << 12);

				nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONDOWN);
				if (nRetCode)
					nValue += (2 << 13);
				nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONUP);
				if (nRetCode)
					nValue += (2 << 14);
				nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONCLICK);
				if (nRetCode)
					nValue += (2 << 15);
				nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONDBCLICK);
				if (nRetCode)
					nValue += (2 << 16);
				nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSEHOVER);
				if (nRetCode)
					nValue += (2 << 17);
				nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONDRAG);
				if (nRetCode)
					nValue += (2 << 18);
				nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONDRAG);
				if (nRetCode)
					nValue += (2 << 19);
				nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONDRAG);
				if (nRetCode)
					nValue += (2 << 20);

                if (nValue)
                    g_Ui.GetIni()->WriteInteger(m_szName, "EventID", nValue);
                else
                    g_Ui.GetIni()->EraseKey(m_szName, "EventID");
            }
            else
            {
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_EVENT_NAME), FALSE); 
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONDOWN), FALSE); 
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONDOWN), FALSE); 
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONUP), FALSE);  
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONUP), FALSE);   
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONCLICK), FALSE);     
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONCLICK), FALSE);     
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONDBCLICK), FALSE);   
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONDBCLICK), FALSE);   
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSEENTER), FALSE);       
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSELEAVE), FALSE);       
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSEMOVE), FALSE);        
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSEWHEEL), FALSE);       
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_KEYDOWN), FALSE);  
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_KEYUP), FALSE); 

				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONDOWN), FALSE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONUP), FALSE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONCLICK), FALSE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONDBCLICK), FALSE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_LBUTTONDRAG), FALSE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_RBUTTONDRAG), FALSE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MBUTTONDRAG), FALSE); 
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_EVENT_MOUSEHOVER), FALSE); 

                g_Ui.GetIni()->EraseKey(m_szName, "EventID");
            }
        }
        break;
    case IDC_EVENT_LBUTTONDOWN: 
    case IDC_EVENT_RBUTTONDOWN: 
    case IDC_EVENT_LBUTTONUP:   
    case IDC_EVENT_RBUTTONUP:   
    case IDC_EVENT_LBUTTONCLICK:     
    case IDC_EVENT_RBUTTONCLICK:     
    case IDC_EVENT_LBUTTONDBCLICK:   
    case IDC_EVENT_RBUTTONDBCLICK:   
    case IDC_EVENT_MOUSEENTER:       
    case IDC_EVENT_MOUSELEAVE:       
    case IDC_EVENT_MOUSEMOVE:        
    case IDC_EVENT_MOUSEWHEEL:       
    case IDC_EVENT_KEYDOWN:  
    case IDC_EVENT_KEYUP: 

	case IDC_EVENT_MBUTTONDOWN:
	case IDC_EVENT_MBUTTONUP:
	case IDC_EVENT_MBUTTONCLICK:
	case IDC_EVENT_MBUTTONDBCLICK:
	case IDC_EVENT_LBUTTONDRAG:
	case IDC_EVENT_RBUTTONDRAG:
	case IDC_EVENT_MBUTTONDRAG:
	case IDC_EVENT_MOUSEHOVER:
        {
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_ACCEPT_MESSAGE);
            if (!nRetCode)
                break;
            nValue = 0;
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONDOWN);
            if (nRetCode)
                nValue = 1;
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONDOWN);
            if (nRetCode)
                nValue += 2;
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONUP);
            if (nRetCode)
                nValue += (2 << 1);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONUP);
            if (nRetCode)
                nValue += (2 << 2);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONCLICK);
            if (nRetCode)
                nValue += (2 << 3);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONCLICK);
            if (nRetCode)
                nValue += (2 << 4);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONDBCLICK);
            if (nRetCode)
                nValue += (2 << 5);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONDBCLICK);
            if (nRetCode)
                nValue += (2 << 6);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSEENTER);
            if (nRetCode)
                nValue += (2 << 7);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSELEAVE);
            if (nRetCode)
                nValue += (2 << 8);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSEMOVE);
            if (nRetCode)
                nValue += (2 << 9);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSEWHEEL);
            if (nRetCode)
                nValue += (2 << 10);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_KEYDOWN);
            if (nRetCode)
                nValue += (2 << 11);
            nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_KEYUP);
            if (nRetCode)
                nValue += (2 << 12);

			nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONDOWN);
			if (nRetCode)
				nValue += (2 << 13);
			nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONUP);
			if (nRetCode)
				nValue += (2 << 14);
			nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONCLICK);
			if (nRetCode)
				nValue += (2 << 15);
			nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONDBCLICK);
			if (nRetCode)
				nValue += (2 << 16);
			nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MOUSEHOVER);
			if (nRetCode)
				nValue += (2 << 17);
			nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_LBUTTONDRAG);
			if (nRetCode)
				nValue += (2 << 18);
			nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_RBUTTONDRAG);
			if (nRetCode)
				nValue += (2 << 19);
			nRetCode = ::IsDlgButtonChecked(m_hWnd, IDC_EVENT_MBUTTONDRAG);
			if (nRetCode)
				nValue += (2 << 20);

            if (nValue)
                g_Ui.GetIni()->WriteInteger(m_szName, "EventID", nValue);
            else
                g_Ui.GetIni()->EraseKey(m_szName, "EventID");
        }
        break;
    case IDC_ITEM_NULL_COM_GET_POS:
        g_ClientManager.EnableUpDataData(false);
        if(g_ClientManager.GetPositonFromClient(m_szName, &x, &y))
        {
			::SetDlgItemInt(m_hWnd, IDC_ITEM_POS_LEFT, x, true);
			::SetDlgItemInt(m_hWnd, IDC_ITEM_POS_TOP, y, true);
        }
        g_ClientManager.EnableUpDataData(true);
        nUpdateShower = false;
        break;
    case IDC_ITEM_NULL_COM_GET_SIZE:
        g_ClientManager.EnableUpDataData(false);
        if(g_ClientManager.GetSizeFromClient(m_szName, &x, &y))
        {
			::SetDlgItemInt(m_hWnd, IDC_ITEM_WIDTH, x, true);
			::SetDlgItemInt(m_hWnd, IDC_ITEM_HEIGHT, y, true);
            ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_WIDTH), TRUE);
            ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_HEIGHT), TRUE);
            ::CheckDlgButton(m_hWnd, IDC_ITEM_AUTO_SIZE, BST_UNCHECKED);
        }
        g_ClientManager.EnableUpDataData(true);
        nUpdateShower = false;
        break;
	case IDC_ITEM_TIP_SEL:
		{
			const char *pszTip = KUiGlobalStrMgr::GetSelf().SelString(m_hWnd);
			if (pszTip && pszTip[0])
				::SetDlgItemText(m_hWnd, IDC_ITEM_TIP, pszTip);
		}
		break;
	case IDC_ITEM_AREA_SEL:
		{
			OnChangeArea();
		}
		break;
	case IDC_ITEM_AREA_CLEAR:
		{
			::SetDlgItemText(m_hWnd, IDC_ITEM_AREA, "");
			g_Ui.GetIni(true)->EraseKey(m_szName, "AreaFile");
		}
		break;
	case IDC_ITEM_DISABLE_SCALE:
		if (::IsDlgButtonChecked(m_hWnd, IDC_ITEM_DISABLE_SCALE))
			g_Ui.GetIni()->WriteInteger(m_szName, "DisableScale", 1);
		else
			g_Ui.GetIni()->EraseKey(m_szName, "DisableScale");
		break;
	case IDC_ITEM_NULL_SHOW_DIR:
		if (::IsDlgButtonChecked(m_hWnd, IDC_ITEM_NULL_SHOW_DIR))
			g_Ui.GetIni()->WriteInteger(m_szName, "OrgTip", 1);
		else
			g_Ui.GetIni()->EraseKey(m_szName, "OrgTip");
		break;
	case IDC_ITEM_LOCKSHOWHIDE:
		if (::IsDlgButtonChecked(m_hWnd, IDC_ITEM_LOCKSHOWHIDE))
			g_Ui.GetIni()->WriteInteger(m_szName, "LockShowAndHide", 1);
		else
			g_Ui.GetIni()->EraseKey(m_szName, "LockShowAndHide");
		break;
	case IDC_ITEM_NULL_TIP_RICHTEXT:
		if (::IsDlgButtonChecked(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT))
			g_Ui.GetIni()->WriteInteger(m_szName, "TipRichText", 1);
		else
			g_Ui.GetIni()->WriteInteger(m_szName, "TipRichText", 0);
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

}

char * KUiItemCommon::EquirotalStrStr(const char *pString, const char *pStrSearch)
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

int KUiItemCommon::OnChangeArea()
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
	::SetDlgItemText(m_hWnd, IDC_ITEM_AREA, szFile + nLen);
	g_Ui.GetIni(true)->WriteString(m_szName, "AreaFile", szFile + nLen);

	return true;
}

void KUiItemCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    int nUpdateShower = true;
    switch(nId)
    {
    case IDC_ITEM_POS_LEFT:
        {
            nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_POS_LEFT, NULL, true);
            g_Ui.GetIni()->WriteInteger(m_szName, "Left", nValue);
        }
        break;
    case IDC_ITEM_POS_TOP:
        {
            nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_POS_TOP, NULL, true);
            g_Ui.GetIni()->WriteInteger(m_szName, "Top", nValue);
        }
        break;
    case IDC_ITEM_WIDTH:
        {
            nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_WIDTH, NULL, true);
            g_Ui.GetIni()->WriteInteger(m_szName, "Width", nValue);
        }
        break;
    case IDC_ITEM_HEIGHT:
        {
            nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_HEIGHT, NULL, true);
            g_Ui.GetIni()->WriteInteger(m_szName, "Height", nValue);
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
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT), TRUE);
				OnUpdataComboBox(IDC_ITEM_SHOW_TIP_TYPE);
				::CheckDlgButton(m_hWnd, IDC_ITEM_ACCEPT_MESSAGE, BST_CHECKED);
				::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEENTER, BST_CHECKED);
				OnButtonClick(IDC_ITEM_ACCEPT_MESSAGE);
				g_Ui.GetIni()->GetInteger(m_szName, "TipRichText", 0, &nValue);
				if (nValue)
					::CheckDlgButton(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT, BST_CHECKED);
				else
					::CheckDlgButton(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT, BST_UNCHECKED);
				OnButtonClick(IDC_ITEM_NULL_TIP_RICHTEXT);
			}
			else
			{
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT), FALSE);
				::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE), FALSE);
				g_Ui.GetIni()->EraseKey(m_szName, "$Tip");
				g_Ui.GetIni()->EraseKey(m_szName, "ShowTipType");
			}
		}
		break;
	case IDC_ITEM_AREA:
		{
			char szValue[MAX_PATH];
			szValue[0] = '\0';
			::GetDlgItemText(m_hWnd, IDC_ITEM_AREA, szValue, sizeof(szValue));
			if (szValue[0])
			{
				g_Ui.GetIni()->WriteString(m_szName, "AreaFile", szValue);
			}
			else
			{
				g_Ui.GetIni()->EraseKey(m_szName, "AreaFile");
			}
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
}

void KUiItemCommon::OnUpdataComboBox(int nId)
{
    int nValue = 0;
    int nUpdateShower = true;
    LRESULT lRet = CB_ERR;
    switch(nId)
    {
    case IDC_ITEM_POS_TYPE:
        {
            HWND hwndCombo = ::GetDlgItem(m_hWnd, IDC_ITEM_POS_TYPE);
            KG_PROCESS_ERROR(hwndCombo);
            lRet = ::SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
            KG_PROCESS_ERROR(lRet != CB_ERR);
            g_Ui.GetIni()->WriteInteger(m_szName, "PosType", (int)lRet);
            switch(lRet)
            {
            case POSITION_BY_CUSTOM:
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_LEFT), TRUE);
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_TOP), TRUE);
                nValue = GetDlgItemInt(m_hWnd, IDC_ITEM_POS_LEFT, NULL, true);
                g_Ui.GetIni()->WriteInteger(m_szName, "Left", nValue);
                nValue = GetDlgItemInt(m_hWnd, IDC_ITEM_POS_TOP, NULL, true);
                g_Ui.GetIni()->WriteInteger(m_szName, "Top", nValue);
                break;
            case POSITION_LEFT_BOTTOM:     
            case POSITION_LEFT_TOP:  
            case POSITION_LEFT_CENTER:
            case POSITION_TOP_LEFT:        
            case POSITION_TOP_RIGHT:
            case POSITION_TOP_CENTER:
            case POSITION_RIGHT_TOP:       
            case POSITION_RIGHT_BOTTOM:
            case POSITION_RIGHT_CENTER:
            case POSITION_BOTTOM_LEFT:     
            case POSITION_BOTTOM_RIGHT:   
            case POSITION_BOTTOM_CENTER:
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_LEFT), FALSE);
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_TOP), FALSE);
                g_Ui.GetIni()->EraseKey(m_szName, "Left");
                g_Ui.GetIni()->EraseKey(m_szName, "Top");
                break;
            }
        }
        break;
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

void KUiItemCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
    int nRetCode = false;
    int nValue = 0;
    char szValue[512];
    szValue[0] = '\0';
    HWND hwndCombo = NULL;

    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pIni);

    strncpy(m_szName, pszName, sizeof(m_szName));

    pIni->GetString(pszName, "._Parent", "", szValue, sizeof(szValue));
    pIni->GetInteger(szValue, "HandleType", ITEM_HANDLE_CUSTOM, &nValue);
    if (nValue != ITEM_HANDLE_CUSTOM)
    {
        ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_TYPE), FALSE);
        pIni->WriteInteger(pszName, "PosType", POSITION_RIGHT_BOTTOM);
    }

    pIni->GetInteger(pszName, "PosType", 0, &nValue);
    hwndCombo = ::GetDlgItem(m_hWnd, IDC_ITEM_POS_TYPE);
    KG_PROCESS_ERROR(hwndCombo);
    ::SendMessage(hwndCombo, CB_SETCURSEL, nValue, 0);
    switch(nValue)
    {
    case POSITION_BY_CUSTOM:
        ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_LEFT), TRUE);
        ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_TOP), TRUE);
        pIni->GetInteger(pszName, "Left", 0, &nValue);
        ::SetDlgItemInt(m_hWnd, IDC_ITEM_POS_LEFT, nValue, true);
        pIni->GetInteger(pszName, "Top", 0, &nValue);
        ::SetDlgItemInt(m_hWnd, IDC_ITEM_POS_TOP, nValue, true);        
        break;
    case POSITION_LEFT_BOTTOM:     
    case POSITION_LEFT_TOP:  
    case POSITION_LEFT_CENTER:
    case POSITION_TOP_LEFT:        
    case POSITION_TOP_RIGHT:
    case POSITION_TOP_CENTER:
    case POSITION_RIGHT_TOP:       
    case POSITION_RIGHT_BOTTOM:
    case POSITION_RIGHT_CENTER:
    case POSITION_BOTTOM_LEFT:     
    case POSITION_BOTTOM_RIGHT:   
    case POSITION_BOTTOM_CENTER:
        ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_LEFT), FALSE);
        ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_POS_TOP), FALSE);
        break;
    }
    OnUpdataComboBox(IDC_ITEM_POS_TYPE);
    

    nRetCode = pIni->GetInteger(pszName, "Width", 0, &nValue);
    if (nRetCode)
    {
        ::CheckDlgButton(m_hWnd, IDC_ITEM_AUTO_SIZE, BST_UNCHECKED);
        ::SetDlgItemInt(m_hWnd, IDC_ITEM_WIDTH, nValue, false);
    }
    else
        ::CheckDlgButton(m_hWnd, IDC_ITEM_AUTO_SIZE, BST_CHECKED);

    nRetCode = pIni->GetInteger(pszName, "Height", 0, &nValue);
    if (nRetCode)
    {
        ::CheckDlgButton(m_hWnd, IDC_ITEM_AUTO_SIZE, BST_UNCHECKED);
        ::SetDlgItemInt(m_hWnd, IDC_ITEM_HEIGHT, nValue, false);
    }
    else
        ::CheckDlgButton(m_hWnd, IDC_ITEM_AUTO_SIZE, BST_CHECKED);
    OnButtonClick(IDC_ITEM_AUTO_SIZE);

	nRetCode = pIni->GetInteger(pszName, "EventID", 0, &nValue);
    if (nRetCode && nValue)
    {
        DWORD dwKey = DWORD(nValue);

        if (dwKey & 0x00000001)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONDOWN, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONDOWN, BST_UNCHECKED);
        if (dwKey & 0x00000002)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONDOWN, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONDOWN, BST_UNCHECKED);
        if (dwKey & 0x00000004)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONUP, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONUP, BST_UNCHECKED);
        if (dwKey & 0x00000008)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONUP, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONUP, BST_UNCHECKED);
        if (dwKey & 0x00000010)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONCLICK, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONCLICK, BST_UNCHECKED);
        if (dwKey & 0x00000020)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONCLICK, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONCLICK, BST_UNCHECKED);
        if (dwKey & 0x00000040)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONDBCLICK, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONDBCLICK, BST_UNCHECKED);
        if (dwKey & 0x00000080)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONDBCLICK, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONDBCLICK, BST_UNCHECKED); 
        if (dwKey & 0x00000100)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEENTER, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEENTER, BST_UNCHECKED);
        if (dwKey & 0x00000200)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSELEAVE, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSELEAVE, BST_UNCHECKED);
        if (dwKey & 0x00000400)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEMOVE, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEMOVE, BST_UNCHECKED);
        if (dwKey & 0x00000800)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEWHEEL, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEWHEEL, BST_UNCHECKED);
        if (dwKey & 0x00001000)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_KEYDOWN, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_KEYDOWN, BST_UNCHECKED);
        if (dwKey & 0x00002000)
            ::CheckDlgButton(m_hWnd, IDC_EVENT_KEYUP, BST_CHECKED);
        else
            ::CheckDlgButton(m_hWnd, IDC_EVENT_KEYUP, BST_UNCHECKED);

		if (dwKey & 0x00004000)
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONDOWN, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONDOWN, BST_UNCHECKED);
		if (dwKey & 0x00008000)
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONUP, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONUP, BST_UNCHECKED);
		if (dwKey & 0x00010000)
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONCLICK, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONCLICK, BST_UNCHECKED);
		if (dwKey & 0x00020000)
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONDBCLICK, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONDBCLICK, BST_UNCHECKED);
		if (dwKey & 0x00040000)
			::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEHOVER, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_EVENT_MOUSEHOVER, BST_UNCHECKED);
		if (dwKey & 0x00080000)
			::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONDRAG, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_EVENT_LBUTTONDRAG, BST_UNCHECKED);
		if (dwKey & 0x00100000)
			::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONDRAG, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_EVENT_RBUTTONDRAG, BST_UNCHECKED);
		if (dwKey & 0x00200000)
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONDRAG, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_EVENT_MBUTTONDRAG, BST_UNCHECKED);

        ::CheckDlgButton(m_hWnd, IDC_ITEM_ACCEPT_MESSAGE, BST_CHECKED);
        
    }
    else
        ::CheckDlgButton(m_hWnd, IDC_ITEM_ACCEPT_MESSAGE, BST_UNCHECKED);
    OnButtonClick(IDC_ITEM_ACCEPT_MESSAGE);  

	nRetCode = pIni->GetString(pszName, "$Tip", "", szValue, sizeof(szValue));
	if (nRetCode && szValue[0])
	{
		::SetDlgItemText(m_hWnd, IDC_ITEM_TIP, szValue);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE), TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT), TRUE);
		pIni->GetInteger(pszName, "ShowTipType", 0, &nValue);
		hwndCombo = ::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE);
		KG_PROCESS_ERROR(hwndCombo);
		::SendMessage(hwndCombo, CB_SETCURSEL, nValue, 0);		
		pIni->GetInteger(pszName, "TipRichText", 0, &nValue);
		if (nValue)
			::CheckDlgButton(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT, BST_CHECKED);
		else
			::CheckDlgButton(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT, BST_UNCHECKED);
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_SHOW_TIP_TYPE), FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_NULL_TIP_RICHTEXT), FALSE);
	}

	pIni->GetInteger(pszName, "OrgTip", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_ITEM_NULL_SHOW_DIR, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_NULL_SHOW_DIR, BST_UNCHECKED);

	pIni->GetInteger(pszName, "LockShowAndHide", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_ITEM_LOCKSHOWHIDE, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_LOCKSHOWHIDE, BST_UNCHECKED);
	
	pIni->GetInteger(pszName, "DisableScale", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_ITEM_DISABLE_SCALE, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_DISABLE_SCALE, BST_UNCHECKED);

	pIni->GetString(pszName, "AreaFile", "", szValue, sizeof(szValue));
	::SetDlgItemText(m_hWnd, IDC_ITEM_AREA, szValue);

Exit0:
    return;
}

void KUiItemCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}

void KUiItemCommon::FillPageInfomation(KUIWNDPAGEINFOMATION *pPageInfo)
{
    KG_PROCESS_ERROR(pPageInfo);
    pPageInfo->m_PageCount = 1;
    strcpy(pPageInfo->m_szPage[0], "组件属性");
Exit0:
    return;
}
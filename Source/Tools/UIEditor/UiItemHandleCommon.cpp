////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemHandleCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 9:40:57
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemHandleCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "ClientManage.h"

////////////////////////////////////////////////////////////////////////////////


KUiItemHandleCommon::KUiItemHandleCommon()
{
    m_szName[0] = '\0';
}

KUiItemHandleCommon::~KUiItemHandleCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemHandleCommon::Create()
{
	KUiItemHandleCommon* pWnd = new KUiItemHandleCommon;
	return pWnd;
}

int KUiItemHandleCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
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
        MAKEINTRESOURCE(IDD_ItemHandleCommon), 
        hWnd, (DLGPROC)WndProc
    );
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

    static const char *pcszHandleType[ITEM_HANDEL_STYLE_END] = 
    {
        "组件自己定位",
        "图文,行高固定",
        "图文,行高在最大最小间",
        "图文,行高为该行组件最高",
        "聊天框容器",
        "树容器",
		"图文,制表符进行左右对其",
    };
    hwndCombo = ::GetDlgItem(m_hWnd, IDC_HANDLE_TYPE);
    KG_PROCESS_ERROR(hwndCombo);
    ::SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0); 
    for (int i = ITEM_HANDLE_STYLE_BEGINE; i < ITEM_HANDEL_STYLE_END; ++i)
    {
        ::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)(pcszHandleType[i])); 
    }

    static const char *pcszPosType[POSITION_END] = 
    {
        "指定坐标",
        "左下", "左上", "左中",
        "上左", "上右", "上中",
        "右上", "右下", "右中",
        "下左", "下右", "下中",
    };
    hwndCombo = ::GetDlgItem(m_hWnd, IDC_HANDLE_FIRST_ITEM_POS_TYPE);
    KG_PROCESS_ERROR(hwndCombo);
    ::SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0); 
    for (int i = POSITION_BEGINE; i < POSITION_END; ++i)
    {
        ::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)(pcszPosType[i])); 
    }

    hSpin = ::GetDlgItem(m_hWnd, IDC_HANDLE_MIN_ROW_HEIGHT_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));

    hSpin = ::GetDlgItem(m_hWnd, IDC_HANDLE_MAX_ROW_HEIGHT_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));

    nResult = true;
Exit0:
    if (!nResult)
        Release();
    return nResult;
}

void KUiItemHandleCommon::Release()
{
    delete this;
}

LRESULT CALLBACK KUiItemHandleCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemHandleCommon*	pView = (KUiItemHandleCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiItemHandleCommon::OnButtonClick(int nBtnId)
{
    int nUpdateShower = false;
    int nRetCode = false;
    int nValue = 0;

	switch(nBtnId)
	{
	case IDC_PIX_SCROLL:
		{
			nValue = ::IsDlgButtonChecked(m_hWnd, IDC_PIX_SCROLL);
			g_Ui.GetIni()->WriteInteger(m_szName, "PixelScroll", nValue);
			g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
			ShowErrorInformation();
		}
		break;
	case IDC_CONTROL_SHOW:
		{
			nValue = ::IsDlgButtonChecked(m_hWnd, IDC_CONTROL_SHOW);
			g_Ui.GetIni()->WriteInteger(m_szName, "ControlShow", nValue);
			g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
			ShowErrorInformation();
		}
		break;
	}
}

void KUiItemHandleCommon::OnUpdataComboBox(int nId)
{
    int nValue = 0;
    int nUpdateShower = true;
    LRESULT lRet = CB_ERR;
    switch(nId)
    {
    case IDC_HANDLE_TYPE:
        {
            BOOL bEnableMinRowHeight = FALSE;
            BOOL bEnableMaxRowHeight = FALSE;
            BOOL bEnableRowSpacing = FALSE;
            BOOL bEnableSetFirstPosType = FALSE;
            HWND hwndCombo = ::GetDlgItem(m_hWnd, IDC_HANDLE_TYPE);
            KG_PROCESS_ERROR(hwndCombo);
            lRet = ::SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
            KG_PROCESS_ERROR(lRet != CB_ERR);
            g_Ui.GetIni()->WriteInteger(m_szName, "HandleType", (int)lRet);
            switch(lRet)
            {
            case ITEM_HANDLE_CUSTOM:
                bEnableSetFirstPosType = TRUE;
                break;
            case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
                bEnableMinRowHeight = TRUE;
                bEnableRowSpacing = TRUE;
                break;
            case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
                bEnableMinRowHeight = TRUE;
                bEnableMaxRowHeight = TRUE;
                bEnableRowSpacing = TRUE;
                break;
            case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
                bEnableRowSpacing = TRUE;
                break;
            case ITEM_HANDLE_CHAT_ITEM_HANDLE:
                bEnableRowSpacing = TRUE;
                break;
            case ITEM_HANDLE_TREE_ITEM_HANDLE:
                bEnableRowSpacing = TRUE;
                break;
			case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
				bEnableRowSpacing = TRUE;
				break;
            }

            ::EnableWindow(::GetDlgItem(m_hWnd, IDC_HANDLE_MIN_ROW_HEIGHT), bEnableMinRowHeight);
            ::EnableWindow(::GetDlgItem(m_hWnd, IDC_HANDLE_MAX_ROW_HEIGHT), bEnableMaxRowHeight);
            ::EnableWindow(::GetDlgItem(m_hWnd, IDC_HANDLE_ROW_SPACING), bEnableRowSpacing);
            ::EnableWindow(::GetDlgItem(m_hWnd, IDC_HANDLE_FIRST_ITEM_POS_TYPE), bEnableSetFirstPosType);
            if (bEnableMinRowHeight)
            {
                nValue = ::GetDlgItemInt(m_hWnd, IDC_HANDLE_MIN_ROW_HEIGHT, NULL, FALSE);
                g_Ui.GetIni()->WriteInteger(m_szName, "MinRowHeight", nValue);
            }
            else
                g_Ui.GetIni()->EraseKey(m_szName, "MinRowHeight");

            if (bEnableMaxRowHeight)
            {
                nValue = ::GetDlgItemInt(m_hWnd, IDC_HANDLE_MAX_ROW_HEIGHT, NULL, FALSE);
                g_Ui.GetIni()->WriteInteger(m_szName, "MaxRowHeight", nValue);
            }
            else
                g_Ui.GetIni()->EraseKey(m_szName, "MaxRowHeight");

            if (bEnableRowSpacing)
            {
                nValue = ::GetDlgItemInt(m_hWnd, IDC_HANDLE_ROW_SPACING, NULL, FALSE);
                g_Ui.GetIni()->WriteInteger(m_szName, "RowSpacing", nValue);
            }
            else
                g_Ui.GetIni()->EraseKey(m_szName, "RowSpacing");

            if (bEnableSetFirstPosType)
                OnUpdataComboBox(IDC_HANDLE_FIRST_ITEM_POS_TYPE);
            else
                g_Ui.GetIni()->EraseKey(m_szName, "FirstItemPosType");

        }
        break;
    case IDC_HANDLE_FIRST_ITEM_POS_TYPE:
        {
            HWND hwndCombo = ::GetDlgItem(m_hWnd, IDC_HANDLE_FIRST_ITEM_POS_TYPE);
            KG_PROCESS_ERROR(hwndCombo);
            lRet = ::SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
            KG_PROCESS_ERROR(lRet != CB_ERR);
            g_Ui.GetIni()->WriteInteger(m_szName, "FirstItemPosType", (int)lRet);
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

void KUiItemHandleCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    int nUpdateShower = true;
    switch(nId)
    {
    case IDC_HANDLE_MIN_ROW_HEIGHT:
        {
            nValue = ::GetDlgItemInt(m_hWnd, IDC_HANDLE_MIN_ROW_HEIGHT, NULL, true);
            g_Ui.GetIni()->WriteInteger(m_szName, "MinRowHeight", nValue);
        }
        break;
    case IDC_HANDLE_MAX_ROW_HEIGHT:
        {
            nValue = ::GetDlgItemInt(m_hWnd, IDC_HANDLE_MAX_ROW_HEIGHT, NULL, true);
            g_Ui.GetIni()->WriteInteger(m_szName, "MaxRowHeight", nValue);
        }
        break;
    case IDC_HANDLE_ROW_SPACING:
        {
            nValue = ::GetDlgItemInt(m_hWnd, IDC_HANDLE_ROW_SPACING, NULL, true);
            g_Ui.GetIni()->WriteInteger(m_szName, "RowSpacing", nValue);
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

void KUiItemHandleCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
    int nRetCode = false;
    int nValue = 0;
    HWND hwndCombo = NULL;

    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pIni);

    strncpy(m_szName, pszName, sizeof(m_szName));

    nRetCode = pIni->GetInteger(pszName, "MinRowHeight", 0, &nValue);
    if (nRetCode)
        ::SetDlgItemInt(m_hWnd, IDC_HANDLE_MIN_ROW_HEIGHT, nValue, false);
    nRetCode = pIni->GetInteger(pszName, "MaxRowHeight", 0, &nValue);
    if (nRetCode)
        ::SetDlgItemInt(m_hWnd, IDC_HANDLE_MAX_ROW_HEIGHT, nRetCode, false);
    nRetCode = pIni->GetInteger(pszName, "RowSpacing", 0, &nValue);
    if (nRetCode)
        ::SetDlgItemInt(m_hWnd, IDC_HANDLE_ROW_SPACING, nValue, false);

    pIni->GetInteger(pszName, "HandleType", 0, &nValue);
    hwndCombo = ::GetDlgItem(m_hWnd, IDC_HANDLE_TYPE);
    KG_PROCESS_ERROR(hwndCombo);
    ::SendMessage(hwndCombo, CB_SETCURSEL, nValue, 0);

    pIni->GetInteger(pszName, "FirstItemPosType", 0, &nValue);
    hwndCombo = ::GetDlgItem(m_hWnd, IDC_HANDLE_FIRST_ITEM_POS_TYPE);
    KG_PROCESS_ERROR(hwndCombo);
    ::SendMessage(hwndCombo, CB_SETCURSEL, nValue, 0);

	pIni->GetInteger(pszName, "PixelScroll", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_PIX_SCROLL, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_PIX_SCROLL, BST_UNCHECKED);

	pIni->GetInteger(pszName, "ControlShow", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_CONTROL_SHOW, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_CONTROL_SHOW, BST_UNCHECKED);

    OnUpdataComboBox(IDC_HANDLE_TYPE);

Exit0:
    return;
}

void KUiItemHandleCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiItemTextCommon.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-1-4 14:14:26
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include <Windows.h>
#include "UiItemTextCommon.h"
#include "UiItemHandleCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "UiItemCommon.h"
#include "ClientManage.h"
#include "UiFontManage.h"
#include "UiGlobalStringValueMgr.h"

////////////////////////////////////////////////////////////////////////////////


KUiItemTextCommon::KUiItemTextCommon()
{
    m_szName[0] = '\0';
}

KUiItemTextCommon::~KUiItemTextCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow *KUiItemTextCommon::Create()
{
	KUiItemTextCommon* pWnd = new KUiItemTextCommon;
	return pWnd;
}

int KUiItemTextCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
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
        MAKEINTRESOURCE(IDD_ItemTextCommon), 
        hWnd, (DLGPROC)WndProc
    );
    KG_PROCESS_ERROR(m_hWnd);

	::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

    hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ROWSPACING_CHANGE);
    if (hSpin) 
        ::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));

	hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_FONTSPACING_CHANGE);
	if (hSpin) 
		::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));

	hSpin = ::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ALPHA_CHANGE);
	if (hSpin) 
		::SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(255, 0));


    nResult = true;
Exit0:
    if (!nResult)
        Release();
    return nResult;
}

void KUiItemTextCommon::Release()
{
    delete this;
}

LRESULT CALLBACK KUiItemTextCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiItemTextCommon*	pView = (KUiItemTextCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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
		}
	}
	return 0;
}

void KUiItemTextCommon::OnButtonClick(int nBtnId)
{
    int nUpdateShower = true;
    int nRetCode = false;
    int nValue = 0;
    switch(nBtnId)
    {
    case IDC_ITEM_TEXT_MULTILINE:
        {
            nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TEXT_MULTILINE);
            g_Ui.GetIni()->WriteInteger(m_szName, "MultiLine", nValue);
            if (nValue)
            {
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ROWSPACING), TRUE); 
                nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TEXT_ROWSPACING, NULL, FALSE);
                g_Ui.GetIni()->WriteInteger(m_szName, "RowSpacing", nValue);
            }
            else
            {
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ROWSPACING), FALSE);
                g_Ui.GetIni()->EraseKey(m_szName, "RowSpacing");
            }
        }
        break;
	case IDC_ITEM_TEXT_CEWNTER_EACHLINE:
		{
			nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TEXT_CEWNTER_EACHLINE);
			g_Ui.GetIni()->WriteInteger(m_szName, "CenterEachRow", nValue);
		}
		break;
	case IDC_ITEM_TEXT_AUTO_ETC:
		{
			nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TEXT_AUTO_ETC);
			g_Ui.GetIni()->WriteInteger(m_szName, "AutoEtc", nValue);
		}
		break;
	case IDC_ITEM_TEXT_ML_AUTO_ADJ:
		{
			nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TEXT_ML_AUTO_ADJ);
			g_Ui.GetIni()->WriteInteger(m_szName, "MlAutoAdj", nValue);
		}
		break;
	case IDC_ITEM_TEXT_SHOW_ALL:
		{
			nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TEXT_SHOW_ALL);
			g_Ui.GetIni()->WriteInteger(m_szName, "ShowAll", nValue);
		}
		break;
	case IDC_ITEM_TEXT_SHOW_DIR:
		{
			nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TEXT_SHOW_DIR);
			g_Ui.GetIni()->WriteInteger(m_szName, "OrgText", nValue);
		}
		break;
	case IDC_ITEM_TEXT_RICH_TEXT:
		{
			nValue = ::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TEXT_RICH_TEXT);
			nValue = nValue != 0 ? 0 : 1;
			g_Ui.GetIni()->WriteInteger(m_szName, "RichText", nValue);

			char szParent[128];
			int nValue1 = 0;
			int nValue2 = 0;

			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_MULTILINE), TRUE);  
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ROWSPACING), TRUE); 
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_LEFT), TRUE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_CENTER), TRUE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_RIGHT), TRUE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_TOP), TRUE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_CENTER), TRUE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_BOTTOM), TRUE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_CEWNTER_EACHLINE), TRUE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_AUTO_ETC), TRUE);

			if (::IsDlgButtonChecked(m_hWnd, IDC_ITEM_TEXT_MULTILINE))
				g_Ui.GetIni()->WriteInteger(m_szName, "MultiLine", 1);
			nValue2 = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TEXT_ROWSPACING, NULL, true);
			g_Ui.GetIni()->WriteInteger(m_szName, "RowSpacing", nValue2);

			g_Ui.GetIni()->GetString(m_szName, "._Parent", "", szParent, sizeof(szParent));
			g_Ui.GetIni()->GetInteger(szParent, "HandleType", 0, &nValue1);
			switch(nValue1)
			{
			case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
			case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
			case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
			case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
				if (nValue)
				{
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_MULTILINE), FALSE);  
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ROWSPACING), FALSE); 
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_LEFT), FALSE);
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_CENTER), FALSE);
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_RIGHT), FALSE);
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_TOP), FALSE);
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_CENTER), FALSE);
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_BOTTOM), FALSE);
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_CEWNTER_EACHLINE), FALSE);
					::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_AUTO_ETC), FALSE);
					
					g_Ui.GetIni()->EraseKey(m_szName, "MultiLine");
					g_Ui.GetIni()->EraseKey(m_szName, "RowSpacing");
					g_Ui.GetIni()->EraseKey(m_szName, "AutoEtc");
				}
				break;
			case ITEM_HANDLE_CUSTOM:
			case ITEM_HANDLE_CHAT_ITEM_HANDLE:
			case ITEM_HANDLE_TREE_ITEM_HANDLE:
				break;
			}
		}
		break;
    case IDC_ITEM_TEXT_HALIGN_LEFT:
        g_Ui.GetIni()->WriteInteger(m_szName, "HAlign", 0);
        break;
    case IDC_ITEM_TEXT_HALIGN_CENTER:
        g_Ui.GetIni()->WriteInteger(m_szName, "HAlign", 1);
        break;
    case IDC_ITEM_TEXT_HALIGN_RIGHT:
        g_Ui.GetIni()->WriteInteger(m_szName, "HAlign", 2);
        break;
    case IDC_ITEM_TEXT_VALIGN_TOP:
        g_Ui.GetIni()->WriteInteger(m_szName, "VAlign", 0);
        break;
    case IDC_ITEM_TEXT_VALIGN_CENTER:
        g_Ui.GetIni()->WriteInteger(m_szName, "VAlign", 1);
        break;
    case IDC_ITEM_TEXT_VALIGN_BOTTOM:
        g_Ui.GetIni()->WriteInteger(m_szName, "VAlign", 2);
        break;
    case IDC_ITEM_TEXT_FONT_SEL:
        {
            int nFont = 0;
            char szValue[32];
            if (UIFONTMANAGE::SelFont(m_hWnd, &nFont)) 
            {
                UIFONTMANAGE::GetNameById(nFont, szValue);
                ::SetDlgItemText(m_hWnd, IDC_ITEM_TEXT_FONT, szValue);
                g_Ui.GetIni()->WriteInteger(m_szName, "FontScheme", nFont);
            }
        }
        break;
	case IDC_ITEM_TEXT_SEL:
		{
			const char *pszTip = KUiGlobalStrMgr::GetSelf().SelString(m_hWnd);
			if (pszTip && pszTip[0])
				::SetDlgItemText(m_hWnd, IDC_ITEM_TEXT_TEXT, pszTip);
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

void KUiItemTextCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    int nUpdateShower = true;
    switch(nId)
    {
    case IDC_ITEM_TEXT_ROWSPACING:
        {
            nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TEXT_ROWSPACING, NULL, true);
            g_Ui.GetIni()->WriteInteger(m_szName, "RowSpacing", nValue);
        }
        break;
	case IDC_ITEM_TEXT_ALPHA:
		{
			nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TEXT_ALPHA, NULL, true);
			g_Ui.GetIni()->WriteInteger(m_szName, "Alpha", nValue);
		}
		break;
	case IDC_ITEM_TEXT_FONTSPACING:
		{
			nValue = ::GetDlgItemInt(m_hWnd, IDC_ITEM_TEXT_FONTSPACING, NULL, true);
			g_Ui.GetIni()->WriteInteger(m_szName, "FontSpacing", nValue);
		}
		break;
    case IDC_ITEM_TEXT_TEXT:
	    {
		    char	szString[1024];
		    ::GetDlgItemText(m_hWnd, IDC_ITEM_TEXT_TEXT, szString, sizeof(szString));
		    g_Ui.GetIni()->WriteString(m_szName, "$Text", szString);
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

void KUiItemTextCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
    int nRetCode = false;
    int nValue = 0;
	int nValue1 = 0;
    char szParent[128];
    char szText[1024];

    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(pIni);

    strncpy(m_szName, pszName, sizeof(m_szName));

    pIni->GetInteger(m_szName, "RowSpacing", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_ITEM_TEXT_ROWSPACING, nValue, FALSE);

	pIni->GetInteger(m_szName, "FontSpacing", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_ITEM_TEXT_FONTSPACING, nValue, FALSE);

	pIni->GetInteger(m_szName, "Alpha", 255, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_ITEM_TEXT_ALPHA, nValue, FALSE);

    pIni->GetInteger(m_szName, "MultiLine", 0, &nValue);
    if (nValue)
    {
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_MULTILINE, BST_CHECKED);
        ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ROWSPACING), TRUE); 
    }
    else
    {
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_MULTILINE, BST_UNCHECKED);
        ::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ROWSPACING), FALSE);
    }

    pIni->GetInteger(m_szName, "HAlign", 0, &nValue);
    switch(nValue)
    {
    case 1:
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_HALIGN_CENTER, BST_CHECKED);
        break;
    case 2:
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_HALIGN_RIGHT, BST_CHECKED);
        break;
    default:
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_HALIGN_LEFT, BST_CHECKED);
        break;
    }

    pIni->GetInteger(m_szName, "VAlign", 0, &nValue);
    switch(nValue)
    {
    case 1:
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_VALIGN_CENTER, BST_CHECKED);
        break;
    case 2:
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_VALIGN_BOTTOM, BST_CHECKED);
        break;
    default:
        ::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_VALIGN_TOP, BST_CHECKED);
        break;
    }

    szParent[0] = '\0';
    pIni->GetInteger(pszName, "FontScheme", 0, &nValue);
    UIFONTMANAGE::GetNameById(nValue, szParent);
    ::SetDlgItemText(m_hWnd, IDC_ITEM_TEXT_FONT, szParent);

    pIni->GetString(pszName, "$Text", "", szText, sizeof(szText));
    ::SetDlgItemText(m_hWnd, IDC_ITEM_TEXT_TEXT, szText);

	pIni->GetInteger(pszName, "CenterEachRow", 0, &nValue);
	if (nValue)
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_CEWNTER_EACHLINE, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_CEWNTER_EACHLINE, BST_UNCHECKED);

	pIni->GetInteger(pszName, "RichText", 1, &nValue1);
	if (nValue1)
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_RICH_TEXT, BST_UNCHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_RICH_TEXT, BST_CHECKED);

	pIni->GetInteger(pszName, "AutoEtc", 0, &nValue1);
	if (nValue1)
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_AUTO_ETC, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_AUTO_ETC, BST_UNCHECKED);

	pIni->GetInteger(pszName, "MlAutoAdj", 0, &nValue1);
	if (nValue1)
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_ML_AUTO_ADJ, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_ML_AUTO_ADJ, BST_UNCHECKED);

	pIni->GetInteger(pszName, "ShowAll", 1, &nValue1);
	if (nValue1)
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_SHOW_ALL, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_SHOW_ALL, BST_UNCHECKED);

	pIni->GetInteger(pszName, "OrgText", 0, &nValue1);
	if (nValue1)
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_SHOW_DIR, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_ITEM_TEXT_SHOW_DIR, BST_UNCHECKED);

    pIni->GetString(m_szName, "._Parent", "", szParent, sizeof(szParent));
    pIni->GetInteger(szParent, "HandleType", 0, &nValue);
    switch(nValue)
    {
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS:
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX:
    case ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM:
	case ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE:
		if (nValue1)
		{
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_MULTILINE), FALSE);  
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_ROWSPACING), FALSE); 
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_LEFT), FALSE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_CENTER), FALSE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_HALIGN_RIGHT), FALSE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_TOP), FALSE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_CENTER), FALSE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_VALIGN_BOTTOM), FALSE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_CEWNTER_EACHLINE), FALSE);
			::EnableWindow(::GetDlgItem(m_hWnd, IDC_ITEM_TEXT_AUTO_ETC), FALSE);
			pIni->EraseKey(m_szName, "MultiLine");
			pIni->EraseKey(m_szName, "RowSpacing");
			g_Ui.GetIni()->EraseKey(m_szName, "AutoEtc");
		}
        break;
    case ITEM_HANDLE_CUSTOM:
    case ITEM_HANDLE_CHAT_ITEM_HANDLE:
    case ITEM_HANDLE_TREE_ITEM_HANDLE:
        break;
    }

Exit0:
    return;
}

void KUiItemTextCommon::Show(int bShow, int nSubWnd /* = -1 */)
{
	if (m_hWnd)
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
}
#include "Stdafx.h"
#include "UiWndEditCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiColorManage.h"
#include "UiFontManage.h"

KUiWndEditCommon::KUiWndEditCommon()
{
    
    m_szName[0] = 0;
}

KUiWndEditCommon::~KUiWndEditCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndEditCommon::Create()
{
	KUiWndEditCommon* pWnd = new KUiWndEditCommon;
	if (pWnd == NULL)
		return NULL;


	return pWnd;
}

int KUiWndEditCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL) {
		return FALSE;
	}
    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndEditCommon), hWnd, (DLGPROC)WndProc);
	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
			pRc->right - pRc->left, pRc->bottom - pRc->top,
			SWP_SHOWWINDOW);
		::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);

        HWND hSpin = ::GetDlgItem(m_hWnd, IDC_WND_EDIT_COMMON_BK_ALPHA_SPIN);
        if (hSpin) 
        {
            SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(255, 0));
        }

		hSpin = ::GetDlgItem(m_hWnd, IDC_WND_EDIT_COMMON_SELECT_COLOR_ALPHA_SPIN);
		if (hSpin) 
		{
			SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(255, 0));
		}

		hSpin = ::GetDlgItem(m_hWnd, IDC_WND_EDIT_COMMON_ROW_SPACING_SPIN);
		if (hSpin) 
		{
			SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
		}

		hSpin = ::GetDlgItem(m_hWnd, IDC_WND_EDIT_COMMON_FONT_SPACING_SPIN);
		if (hSpin) 
		{
			SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
		}

		hSpin = ::GetDlgItem(m_hWnd, IDC_WND_EDIT_COMON_MAX_LENGTH_SPIN);
		if (hSpin) 
		{
			SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
		}
	}
	else
	{
		return FALSE;
	}

	return true;
}


void KUiWndEditCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndEditCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//KUiWndWindow::WndProc(hWnd, message, wParam, lParam);
	KUiWndEditCommon*	pView = (KUiWndEditCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiWndEditCommon::OnButtonClick(int nBtnId)
{
	int	nValue = 0;
	const char* pszKey = NULL;
	switch(nBtnId)
	{
	case IDC_WND_EDIT_COMMON_TEXT_TYPE_0:
		pszKey = "Type";
		nValue = 0;
		break;
	case IDC_WND_EDIT_COMMON_TEXT_TYPE_1:
		pszKey = "Type";
		nValue = 1;
		break;
	case IDC_WND_EDIT_COMMON_TEXT_TYPE_2:
		pszKey = "Type";
		nValue = 2;
		break;
	case IDC_WND_EDIT_COMMON_PASSWORD:
		pszKey = "Password";
		nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_EDIT_COMMON_PASSWORD) == BST_CHECKED;
		break;	
	case IDC_WND_EDIT_COMMON_MULTILINE:
		pszKey = "MultiLine";
		nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_EDIT_COMMON_MULTILINE) == BST_CHECKED;
		break;
    case IDC_WND_EDIT_COMMON_FONT_SEL:
        {
            int nFont = 0;
            char szValue[32] = "";
            if (UIFONTMANAGE::SelFont(m_hWnd, &nFont)) 
            {

                UIFONTMANAGE::GetNameById(nFont, szValue);
                ::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_FONT, szValue);
                pszKey = "FontScheme";
                nValue = nFont;
            }
        }
        break;
	case IDC_WND_EDIT_COMMON_SELFONT_SEL:
		{
			int nFont = 0;
			char szValue[32] = "";
			if (UIFONTMANAGE::SelFont(m_hWnd, &nFont)) 
			{

				UIFONTMANAGE::GetNameById(nFont, szValue);
				::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_SELFONT, szValue);
				pszKey = "SelFontScheme";
				nValue = nFont;
			}
		}
		break;
	case IDC_WND_EDIT_COMMON_CARETFONT_SEL:
		{
			int nFont = 0;
			char szValue[32] = "";
			if (UIFONTMANAGE::SelFont(m_hWnd, &nFont)) 
			{

				UIFONTMANAGE::GetNameById(nFont, szValue);
				::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_CARETFONT, szValue);
				pszKey = "CaretFontScheme";
				nValue = nFont;
			}
		}
		break;
    case IDC_WND_EDIT_COMMON_FOCUS_COLOR_SEL:
        {
            char szValue[32] = "";
            if (UICOLORMANAGE::SelColor(m_hWnd, szValue)) 
            {
                ::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_FOCUS_COLOR, szValue);
                g_Ui.GetIni(true)->WriteString(m_szName, "FocusBgColor", szValue);
                g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
                ShowErrorInformation();
            }
        }
        break;
    case IDC_WND_EDIT_COMMON_SELECT_COLOR_SEL:
        {
            char szValue[32] = "";
            if (UICOLORMANAGE::SelColor(m_hWnd, szValue)) 
            {
                ::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_SELECT_COLOR, szValue);
                g_Ui.GetIni(true)->WriteString(m_szName, "SelectBgColor", szValue);
                g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
                ShowErrorInformation();
            }
        }
        break;
	}
	if (pszKey)
	{
		g_Ui.GetIni(true)->WriteInteger(m_szName, pszKey, nValue);
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

void KUiWndEditCommon::OnUpdateEditText(int nId)
{
	int	nValue = 0;
	const char* pszKey = NULL;

	switch (nId)
	{
	case IDC_WND_EDIT_COMON_MAX_LENGTH:
		{
			nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMON_MAX_LENGTH, NULL, false);
			if (nValue < 0)
				nValue = 0;
			pszKey = "MaxLen";
		}
		break;
	case IDC_WND_EDIT_COMMON_BK_ALPHA:
		{
			nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMMON_BK_ALPHA, NULL, false);
			if (nValue < 0)
				nValue = 0;
			if (nValue > 255)
				nValue = 255;
			pszKey = "FocusBgColorAlpha";
		}
		break;
	case IDC_WND_EDIT_COMMON_SELECT_COLOR_ALPHA:
		{
			nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMMON_SELECT_COLOR_ALPHA, NULL, false);
			if (nValue < 0)
				nValue = 0;
			if (nValue > 255)
				nValue = 255;
			pszKey = "SelectBgColorAlpha";
		}
		break;
	case IDC_WND_EDIT_COMMON_ROW_SPACING:
		{
			nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMMON_ROW_SPACING, NULL, false);
			if (nValue < 0)
				nValue = 0;
			pszKey = "RowSpacing";
		}
		break;
	case IDC_WND_EDIT_COMMON_FONT_SPACING:
		{
			nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMMON_FONT_SPACING, NULL, false);
			if (nValue < 0)
				nValue = 0;
			pszKey = "FontSpacing";
		}
		break;

	}
	if (pszKey)
	{
		g_Ui.GetIni(true)->WriteInteger(m_szName, pszKey, nValue);
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
		ShowErrorInformation();
	}

}

void KUiWndEditCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd || !pIni || !pszName)
		return;
    strcpy(m_szName, pszName);
	char	buffer[512];
	int		nValue;


    pIni->GetInteger(pszName, "MaxLen", 32, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMON_MAX_LENGTH, nValue, true);

	pIni->GetInteger(pszName, "Type", 0, &nValue);
	if (nValue == 1)
		::CheckDlgButton(m_hWnd, IDC_WND_EDIT_COMMON_TEXT_TYPE_1, BST_CHECKED);
	else if (nValue == 2)
		::CheckDlgButton(m_hWnd, IDC_WND_EDIT_COMMON_TEXT_TYPE_2, BST_CHECKED);
	else
		::CheckDlgButton(m_hWnd, IDC_WND_EDIT_COMMON_TEXT_TYPE_0, BST_CHECKED);


	pIni->GetString(pszName, "SelectBgColor", "", buffer, sizeof(buffer));
	::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_SELECT_COLOR, buffer);
	pIni->GetInteger(pszName, "SelectBgColorAlpha", 255, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMMON_SELECT_COLOR_ALPHA, nValue, false);

	pIni->GetString(pszName, "FocusBgColor", "", buffer, sizeof(buffer));
	::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_FOCUS_COLOR, buffer);
    pIni->GetInteger(pszName, "FocusBgColorAlpha", 255, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMMON_BK_ALPHA, nValue, false);

	pIni->GetInteger(pszName, "MultiLine", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_EDIT_COMMON_MULTILINE, nValue ? BST_CHECKED : 0);
	pIni->GetInteger(pszName, "Password", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_EDIT_COMMON_PASSWORD, nValue ? BST_CHECKED : 0);

	pIni->GetInteger(pszName, "RowSpacing", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMMON_ROW_SPACING, nValue, false);

	pIni->GetInteger(pszName, "FontSpacing", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_EDIT_COMMON_FONT_SPACING, nValue, false);

    pIni->GetInteger(pszName, "FontScheme", 0, &nValue);
    UIFONTMANAGE::GetNameById(nValue, buffer);
    ::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_FONT, buffer);

	pIni->GetInteger(pszName, "SelFontScheme", 0, &nValue);
	UIFONTMANAGE::GetNameById(nValue, buffer);
	::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_SELFONT, buffer);

	pIni->GetInteger(pszName, "CaretFontScheme", 0, &nValue);
	UIFONTMANAGE::GetNameById(nValue, buffer);
	::SetDlgItemText(m_hWnd, IDC_WND_EDIT_COMMON_CARETFONT, buffer);

}
void KUiWndEditCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	} 
}


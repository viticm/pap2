#include "Stdafx.h"
#include "UiWndButtonCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiWndSelTgaFram.h"
#include "UiWndSelAnimate.h"
#include "UiFontManage.h"


KUiWndButtonCommon::KUiWndButtonCommon()
{
    
	m_szName[0] = 0;
}

KUiWndButtonCommon::~KUiWndButtonCommon()
{
    
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndButtonCommon::Create()
{
	KUiWndButtonCommon* pWnd = new KUiWndButtonCommon;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndButtonCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL) {
		return FALSE;
	}
    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog(
        (HINSTANCE)g_hPluginInstance, 
        MAKEINTRESOURCE(IDD_WndButtonCommon), 
        hWnd, (DLGPROC)WndProc
    );
	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
			pRc->right - pRc->left, pRc->bottom - pRc->top,
			SWP_SHOWWINDOW);
		::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);
	}
	else
	{
		return FALSE;
	}	
	return TRUE;
}

void KUiWndButtonCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndButtonCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	KUiWndButtonCommon* pView = (KUiWndButtonCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiWndButtonCommon::OnUpdateEditText(int nId)
{
    LPCTSTR pszKeyName = NULL;
    int nValue = 0;
    IIniFile *pIni = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pIni);

    switch(nId) 
    {
    case IDC_WND_BTN_COMMON_TIP:
        {
            char szBuffer[128];
            ::GetDlgItemText(m_hWnd, IDC_WND_BTN_COMMON_TIP, szBuffer, sizeof(szBuffer));
            pIni->WriteString(m_szName, "$Tip", szBuffer); 
            g_ClientManager.UpDataWndInClient(pIni);
            ShowErrorInformation();
        }
    	break;
    case IDC_WND_BTN_COMMON_ANIMATE_NORMAL:
        pszKeyName = "NormalGroup";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_NORMAL, NULL, false);
        break;
    case IDC_WND_BTN_COMMON_ANIMATE_OVER:
        pszKeyName = "MouseOverGroup";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_OVER, NULL, false);
        break;
    case IDC_WND_BTN_COMMON_ANIMATE_MOUSE_DOWN:
        pszKeyName = "MouseDownGroup";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_MOUSE_DOWN, NULL, false);
        break;
    case IDC_WND_BTN_COMMON_ANIMATE_DISABLE:
        pszKeyName = "DisableGroup";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_DISABLE, NULL, false);
        break;
    default:
        break;
    }

    KG_PROCESS_ERROR(pszKeyName);

    pIni->WriteInteger(m_szName, pszKeyName, nValue);
	g_ClientManager.UpDataWndInClient(pIni);
    ShowErrorInformation();        

Exit0:
    return;
}

void KUiWndButtonCommon::OnButtonClick(int nId)
{
    LPCTSTR pszKeyName = NULL;
    int nValue = 0;
    IIniFile *pIni = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pIni);
    switch(nId) 
    {
    case IDC_WND_BTN_COMMON_ANIMATE_NORMAL_SPIN:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "NormalGroup";
            ::SetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_NORMAL, nValue, false);
        }
    	break;
    case IDC_WND_BTN_COMMON_ANIMATE_OVER_SPIN:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "MouseOverGroup";
            ::SetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_OVER, nValue, false);
        }
        break;
    case IDC_WND_BTN_COMMON_ANIMATE_MOUSE_DOWN_SPIN:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "MouseDownGroup";
            ::SetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_MOUSE_DOWN, nValue, false);
        }
        break;
    case IDC_WND_BTN_COMMON_ANIMATE_DISABLE_SPIN:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "DisableGroup";
            ::SetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_DISABLE, nValue, false);
        }
        break;
    case IDC_WND_BTN_DISABLE_WHEN_CREATE:
        pszKeyName = "ButtonDisable";
        nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_BTN_DISABLE_WHEN_CREATE) == BST_CHECKED;
        break;
	case IDC_WND_BUTTON_TEXT_NORMAL_SEL:
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_BUTTON_TEXT_NORMAL, szValue);
			pszKeyName = "NormalFont";
		}
		break;
	case IDC_WND_BUTTON_TEXT_MOUSEOVER_SEL:
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_BUTTON_TEXT_MOUSEOVER, szValue);
			pszKeyName = "MouseOverFont";
		}
		break;
	case IDC_WND_BUTTON_TEXT_DOWN_SEL:	
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_BUTTON_TEXT_DOWN, szValue);
			pszKeyName = "MouseDownFont";
		}
		break;
	case IDC_WND_BUTTON_TEXT_DISABLE_SEL:
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_BUTTON_TEXT_DISABLE, szValue);
			pszKeyName = "DisableFont";
		}
		break;
    default:
        break;
    }

    KG_PROCESS_ERROR(pszKeyName);

    pIni->WriteInteger(m_szName, pszKeyName, nValue);
	g_ClientManager.UpDataWndInClient(pIni);
    ShowErrorInformation();

Exit0:
    return;
}

int KUiWndButtonCommon::SelAnimate(int *pnSelResult)
{
    int nResult     = false;
    int nRetCode    = false;
    char szUiTexFileName[MAX_PATH];
    char szRoot[MAX_PATH];
    int nSelAinmate = 0;
    IIniFile *pIni = NULL;

    pIni = g_Ui.GetIni();
    KG_PROCESS_ERROR(pIni);

    g_GetRootPath(szRoot);
    KG_PROCESS_ERROR(szRoot[0] != '\0');

    szUiTexFileName[0] = '\0';
    pIni->GetString(m_szName, "Image", "", szUiTexFileName, sizeof(szUiTexFileName));
    KG_PROCESS_ERROR(szUiTexFileName[0] != '\0');

    strcat(szRoot, "\\");
    strcat(szRoot, szUiTexFileName);
    strcpy(szUiTexFileName, szRoot);

    nSelAinmate = SELANIMATE::OnSelAnimate(szUiTexFileName, m_hWnd);    
    KG_PROCESS_ERROR(nSelAinmate != -1);

    *pnSelResult = nSelAinmate;

    nResult = true;
Exit0:
    return nResult;
}



void KUiWndButtonCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd || !pIni || !pszName)
		return;

    strcpy(m_szName, pszName);
	int		nValue;

    pIni->GetInteger(pszName, "NormalGroup", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_NORMAL, nValue, false);
    pIni->GetInteger(pszName, "MouseOverGroup", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_OVER, nValue, false);
    pIni->GetInteger(pszName, "MouseDownGroup", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_MOUSE_DOWN, nValue, false);
    pIni->GetInteger(pszName, "DisableGroup", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_BTN_COMMON_ANIMATE_DISABLE, nValue, false);


	pIni->GetInteger(pszName, "ButtonDisable", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_BTN_DISABLE_WHEN_CREATE, nValue ? BST_CHECKED : 0);

	if (pIni->GetInteger(pszName, "NormalFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_BUTTON_TEXT_NORMAL, szName);
	}

	if (pIni->GetInteger(pszName, "MouseOverFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_BUTTON_TEXT_MOUSEOVER, szName);
	}

	if (pIni->GetInteger(pszName, "MouseDownFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_BUTTON_TEXT_DOWN, szName);
	}

	if (pIni->GetInteger(pszName, "DisableFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_BUTTON_TEXT_DISABLE, szName);
	}
}
void KUiWndButtonCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	}
}



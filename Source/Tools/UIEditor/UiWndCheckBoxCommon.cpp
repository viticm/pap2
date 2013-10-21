#include "Stdafx.h"
#include "UiWndCheckBoxCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"
#include "UiWndSelTgaFram.h"
#include "UiWndSelAnimate.h"
#include "UiFontManage.h"


KUiWndCheckBoxCommon::KUiWndCheckBoxCommon()
{    
	m_szName[0] = 0;
}

KUiWndCheckBoxCommon::~KUiWndCheckBoxCommon()
{   
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndCheckBoxCommon::Create()
{
	KUiWndCheckBoxCommon* pWnd = new KUiWndCheckBoxCommon;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndCheckBoxCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL) {
		return FALSE;
	}
    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog(
        (HINSTANCE)g_hPluginInstance, 
        MAKEINTRESOURCE(IDD_WndCheckBox), 
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

void KUiWndCheckBoxCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndCheckBoxCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	KUiWndCheckBoxCommon* pView = (KUiWndCheckBoxCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiWndCheckBoxCommon::OnUpdateEditText(int nId)
{
    LPCTSTR pszKeyName = NULL;
    int nValue = 0;
    IIniFile *pIni = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pIni);

    switch(nId) 
    {
    case IDC_WND_CKECKBOX_TIP:
        {
            char szBuffer[128];
            ::GetDlgItemText(m_hWnd, IDC_WND_CKECKBOX_TIP, szBuffer, sizeof(szBuffer));
            pIni->WriteString(m_szName, "$Tip", szBuffer); 
            g_ClientManager.UpDataWndInClient(pIni);
            ShowErrorInformation();
        }
    	break;
    case IDC_WND_CKECKBOX_NORMAL:
        pszKeyName = "UnCheckAndEnable";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_NORMAL, NULL, false);
        break;
    case IDC_WND_CKECKBOX_SEL:
        pszKeyName = "CheckAndEnable";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL, NULL, false);
        break;
    case IDC_WND_CKECKBOX_UNSEL_AND_DISABLE:
        pszKeyName = "UnCheckAndDisable";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_AND_DISABLE, NULL, false);
        break;
    case IDC_WND_CKECKBOX_SEL_AND_DISABLE:
        pszKeyName = "CheckAndDisable";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_AND_DISABLE, NULL, false);
        break;
    case IDC_WND_CKECKBOX_UNSEL_MSOVER:
        pszKeyName = "UnCheckedAndEnableWhenMouseOver";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_MSOVER, NULL, false);
        break;
    case IDC_WND_CKECKBOX_SEL_MSOVER:
        pszKeyName = "CheckedAndEnableWhenMouseOver";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_MSOVER, NULL, false);
        break;
    case IDC_WND_CKECKBOX_SEL_AND_DIABLE_MSOVER:
        pszKeyName = "CheckedAndDisableWhenMouseOver";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_AND_DIABLE_MSOVER, NULL, false);
        break;
    case IDC_WND_CKECKBOX_UNSEL_AND_DIABLE_MSOVER:
        pszKeyName = "UnCheckedAndDisableWhenMouseOver";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_AND_DIABLE_MSOVER, NULL, false);
        break;
    case IDC_WND_CKECKBOX_CHECKING:
        pszKeyName = "Checking";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_CHECKING, NULL, false);
        break;
    case IDC_WND_CKECKBOX_UNCHECKING:
        pszKeyName = "UnChecking";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNCHECKING, NULL, false);
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

void KUiWndCheckBoxCommon::OnButtonClick(int nId)
{
    LPCTSTR pszKeyName = NULL;
    int nValue = 0;
    IIniFile *pIni = g_Ui.GetIni(true);
    KG_PROCESS_ERROR(pIni);
    switch(nId) 
    {
    case IDC_WND_CKECKBOX_NORMAL_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "UnCheckAndEnable";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_NORMAL, nValue, false);
        }
    	break;
    case IDC_WND_CKECKBOX_SEL_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "CheckAndEnable";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL, nValue, false);
        }
        break;
    case IDC_WND_CKECKBOX_UNSEL_AND_DISABLE_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "UnCheckAndDisable";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_AND_DISABLE, nValue, false);
        }
        break;
    case IDC_WND_CKECKBOX_SEL_AND_DISABLE_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "CheckAndDisable";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_AND_DISABLE, nValue, false);
        }
        break;
    case IDC_WND_CKECKBOX_UNSEL_MSOVER_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "UnCheckedAndEnableWhenMouseOver";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_MSOVER, nValue, false);
        }
        break;
    case IDC_WND_CKECKBOX_SEL_MSOVER_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "CheckedAndEnableWhenMouseOver";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_MSOVER, nValue, false);
        }
        break;
    case IDC_WND_CKECKBOX_SEL_AND_DIABLE_MSOVER_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "CheckedAndDisableWhenMouseOver";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_AND_DIABLE_MSOVER, nValue, false);
        }
        break;
    case IDC_WND_CKECKBOX_UNSEL_AND_DIABLE_MSOVER_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "UnCheckedAndDisableWhenMouseOver";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_AND_DIABLE_MSOVER, nValue, false);
        }
        break;
    case IDC_WND_CKECKBOX_CHECKING_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "Checking";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_CHECKING, nValue, false);
        }
        break;
    case IDC_WND_CKECKBOX_UNCHECKING_SEL:
        if(SelAnimate(&nValue))
        {
            pszKeyName = "UnChecking";
            ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNCHECKING, nValue, false);
        }
        break;
    case IDC_WND_CHECKBOX_DISABLE:
        pszKeyName = "DisableCheck";
        nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_CHECKBOX_DISABLE) == BST_CHECKED;
        break;
    case IDC_WND_CKECKBOX_CHECK_DEFAULT:
        pszKeyName = "CheckedWhenCreate";
        nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_CKECKBOX_CHECK_DEFAULT) == BST_CHECKED;
        break;
	case IDC_WND_CKECKBOX_RADIO:
		pszKeyName = "RadioButton";
		nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_CKECKBOX_RADIO) == BST_CHECKED;
		break;
	case IDC_WND_CHECKBOX_UNCHECK_FONT_SEL:
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_UNCHECK_FONT, szValue);
			pszKeyName = "UncheckFont";
		}
		break;
	case IDC_WND_CHECKBOX_CHECK_FONT_SEL:
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_CHECK_FONT, szValue);
			pszKeyName = "CheckFont";
		}
		break;
	case IDC_WND_CHECKBOX_OVER_FONT_SEL:
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_OVER_FONT, szValue);
			pszKeyName = "MouseOverFont";
		}
		break;
	case IDC_WND_CHECKBOX_DOWN_FONT_SEL:
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_DOWN_FONT, szValue);
			pszKeyName = "MouseDownFont";
		}
		break;
	case IDC_WND_CHECKBOX_DISABLE_FONT_SEL:
		if (UIFONTMANAGE::SelFont(m_hWnd, &nValue)) 
		{
			char szValue[32];
			UIFONTMANAGE::GetNameById(nValue, szValue);
			::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_DISABLE_FONT, szValue);
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

int KUiWndCheckBoxCommon::SelAnimate(int *pnSelResult)
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



void KUiWndCheckBoxCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd || !pIni || !pszName)
		return;

    strcpy(m_szName, pszName);
	int		nValue;

    pIni->GetInteger(pszName, "UnCheckAndEnable", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_NORMAL, nValue, false);
    pIni->GetInteger(pszName, "CheckAndEnable", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL, nValue, false);
    pIni->GetInteger(pszName, "UnCheckAndDisable", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_AND_DISABLE, nValue, false);
    pIni->GetInteger(pszName, "CheckAndDisable", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_AND_DISABLE, nValue, false);

    pIni->GetInteger(pszName, "UnCheckedAndEnableWhenMouseOver", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_MSOVER, nValue, false);
    pIni->GetInteger(pszName, "CheckedAndEnableWhenMouseOver", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_MSOVER, nValue, false);
    pIni->GetInteger(pszName, "CheckedAndDisableWhenMouseOver", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_SEL_AND_DIABLE_MSOVER, nValue, false);
    pIni->GetInteger(pszName, "UnCheckedAndDisableWhenMouseOver", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNSEL_AND_DIABLE_MSOVER, nValue, false);
  
    pIni->GetInteger(pszName, "Checking", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_CHECKING, nValue, false);
    pIni->GetInteger(pszName, "UnChecking", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_CKECKBOX_UNCHECKING, nValue, false);

	pIni->GetInteger(pszName, "DisableCheck", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_CHECKBOX_DISABLE, nValue ? BST_CHECKED : 0);
 	pIni->GetInteger(pszName, "CheckedWhenCreate", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_CKECKBOX_CHECK_DEFAULT, nValue ? BST_CHECKED : 0);
	pIni->GetInteger(pszName, "RadioButton", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_CKECKBOX_RADIO, nValue ? BST_CHECKED : 0);

	if (pIni->GetInteger(pszName, "CheckFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_CHECK_FONT, szName);
	}

	if (pIni->GetInteger(pszName, "UncheckFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_UNCHECK_FONT, szName);
	}

	if (pIni->GetInteger(pszName, "MouseOverFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_OVER_FONT, szName);
	}

	if (pIni->GetInteger(pszName, "MouseDownFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_DOWN_FONT, szName);
	}

	if (pIni->GetInteger(pszName, "DisableFont", -1, &nValue))
	{
		char szName[64];
		UIFONTMANAGE::GetNameById(nValue, szName);
		::SetDlgItemText(m_hWnd, IDC_WND_CHECKBOX_DISABLE_FONT, szName);
	}
}
void KUiWndCheckBoxCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	}
}



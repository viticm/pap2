#include "Stdafx.h"
#include "UiWndShowAnimateCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndShowAnimateCommon::KUiWndShowAnimateCommon()
{
}


KUiWndShowAnimateCommon::~KUiWndShowAnimateCommon()
{   
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndShowAnimateCommon::Create()
{
	KUiWndShowAnimateCommon* pWnd = new KUiWndShowAnimateCommon;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndShowAnimateCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
    int nResult  = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(hWnd);

    strcpy(m_szName, pszName);

	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndShowAnimate), hWnd, (DLGPROC)WndProc);
	KG_PROCESS_ERROR(m_hWnd);
    ::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
		pRc->right - pRc->left, pRc->bottom - pRc->top,
		SWP_SHOWWINDOW);
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);

    HWND hSpin = ::GetDlgItem(m_hWnd, IDC_WND_SHOW_ANIMATE_START_POS_X_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
    }

    hSpin = ::GetDlgItem(m_hWnd, IDC_WND_SHOW_ANIMATE_START_POS_Y_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
    }

    hSpin = ::GetDlgItem(m_hWnd, IDC_WND_SHOW_ANIMATE_END_POS_X_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
    }

    hSpin = ::GetDlgItem(m_hWnd, IDC_WND_SHOW_ANIMATE_END_POS_Y_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
    }
    hSpin = ::GetDlgItem(m_hWnd, IDC_WND_SHOW_ANIMATE_TIMESPACE_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
    }
    hSpin = ::GetDlgItem(m_hWnd, IDC_WND_SHOW_ANIMATE_SPEED_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(1000, -1000));
    }

    nResult = true;
Exit0:
	return nResult;
}

LRESULT CALLBACK KUiWndShowAnimateCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	KUiWndShowAnimateCommon*	pView = (KUiWndShowAnimateCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiWndShowAnimateCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    LPCTSTR pszKey = NULL;
    switch(nId) 
    {
    case IDC_WND_SHOW_ANIMATE_START_POS_X:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_START_POS_X, NULL, true);
        pszKey = "AnimateStartPosX";
    	break;
    case IDC_WND_SHOW_ANIMATE_START_POS_Y:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_START_POS_Y, NULL, true);
        pszKey = "AnimateStartPosY";
    	break;
    case IDC_WND_SHOW_ANIMATE_END_POS_X:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_END_POS_X, NULL, true);
        pszKey = "AnimateEndPosX";
    	break;
    case IDC_WND_SHOW_ANIMATE_END_POS_Y:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_END_POS_Y, NULL, true);
        pszKey = "AnimateEndPosY";
    	break;
    case IDC_WND_SHOW_ANIMATE_TIMESPACE:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_TIMESPACE, NULL, false);
        pszKey = "AnimateTimeSpace";
    	break;
    case IDC_WND_SHOW_ANIMATE_SPEED:
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_SPEED, NULL, false);
        pszKey = "AnimateMoveSpeed";
    	break;
    default:
        break;
    }
    if (pszKey) 
    {
        g_Ui.GetIni()->WriteInteger(m_szName, pszKey, nValue);
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
    }

}

void KUiWndShowAnimateCommon::OnButtonClick(int nId)
{
    if (nId == IDC_SHOWANIMATE_SHOW) 
    {
        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
    }
    else if (nId == IDC_WND_SHOW_ANIMATE_WHEN_CREATE) 
    {
        int nValue = ::IsDlgButtonChecked(m_hWnd, IDC_WND_SHOW_ANIMATE_WHEN_CREATE) == BST_CHECKED;
        g_Ui.GetIni()->WriteInteger(m_szName, "ShowAnimateWhenCreate", nValue); 

        g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
    }

}

void KUiWndShowAnimateCommon::Release()
{
	delete this;
}



void KUiWndShowAnimateCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	KG_PROCESS_ERROR(m_hWnd && pIni && pszName);
    strcpy(m_szName, pszName);

    int nValue;

    pIni->EraseKey(pszName, "FollowMouse");
    pIni->EraseKey(pszName, "FollowMouseOffsetX");
    pIni->EraseKey(pszName, "FollowMouseOffsetY");
    
	pIni->GetInteger(pszName, "AnimateStartPosX", 0,&nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_START_POS_X, nValue, true);
    pIni->GetInteger(pszName, "AnimateStartPosY", 0,&nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_START_POS_Y, nValue, true);
	pIni->GetInteger(pszName, "AnimateEndPosX", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_END_POS_X, nValue, true);
    pIni->GetInteger(pszName, "AnimateEndPosY", 0, &nValue);
	::SetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_END_POS_Y, nValue, true);
    pIni->GetInteger(pszName, "AnimateTimeSpace", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_TIMESPACE, nValue, true);
    pIni->GetInteger(pszName, "AnimateMoveSpeed", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_SHOW_ANIMATE_SPEED, nValue, true);

	pIni->GetInteger(pszName, "ShowAnimateWhenCreate", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_SHOW_ANIMATE_WHEN_CREATE, nValue ? BST_CHECKED : 0);


Exit0:
    return;
}

void KUiWndShowAnimateCommon::Show(int bShow, int nSubWnd)
{
	KG_PROCESS_ERROR(m_hWnd);

	::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);

Exit0:
    return;
}

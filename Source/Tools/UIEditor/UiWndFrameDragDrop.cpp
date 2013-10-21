#include "Stdafx.h"
#include "UiWndFrameDragDrop.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"

KUiWndFrameDragDrop::KUiWndFrameDragDrop()
{
    
    m_szName[0] = 0;
}

KUiWndFrameDragDrop::~KUiWndFrameDragDrop()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndFrameDragDrop::Create()
{
	KUiWndFrameDragDrop* pWnd = new KUiWndFrameDragDrop;
	if (pWnd == NULL)
		return NULL;


	return pWnd;
}

int KUiWndFrameDragDrop::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL) {
		return FALSE;
	}
    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndFrameDragDrop), hWnd, (DLGPROC)WndProc);
	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
			pRc->right - pRc->left, pRc->bottom - pRc->top,
			SWP_SHOWWINDOW);
		::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)(KUiWndWindow*)this);

        HWND hSpin = ::GetDlgItem(m_hWnd, IDC_FRAME_DRAGDROP_OFFSET_X_SPIN);
        if (hSpin) 
        {
            SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
        }
        hSpin = ::GetDlgItem(m_hWnd, IDC_FRAME_DRAGDROP_OFFSET_Y_SPIN);
        if (hSpin) 
        {
            SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, -10000));
        }

	}
	else
	{
		return FALSE;
	}

	return true;
}


void KUiWndFrameDragDrop::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndFrameDragDrop::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiWndFrameDragDrop*	pView = (KUiWndFrameDragDrop*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiWndFrameDragDrop::OnButtonClick(int nBtnId)
{
	int	nValue = 0;
	const char* pszKey = NULL;
	switch(nBtnId)
	{
	case IDC_FRAME_DRAGDROP_FOLLOWMOUSE:
		pszKey = "FollowMouse";
		nValue = ::IsDlgButtonChecked(m_hWnd, IDC_FRAME_DRAGDROP_FOLLOWMOUSE) == BST_CHECKED;
		break;	
	}
	if (pszKey)
	{
		g_Ui.GetIni(true)->WriteInteger(m_szName, pszKey, nValue);
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

void KUiWndFrameDragDrop::OnUpdateEditText(int nId)
{
	int	nValue = 0;
	const char* pszKey = NULL;
	switch(nId)
	{
	case IDC_FRAME_DRAGDROP_OFFSET_X:
		pszKey = "FollowMouseOffsetX";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_FRAME_DRAGDROP_OFFSET_X, NULL, true);
		break;	
	case IDC_FRAME_DRAGDROP_OFFSET_Y:
		pszKey = "FollowMouseOffsetY";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_FRAME_DRAGDROP_OFFSET_Y, NULL, true);
		break;	
	}
	if (pszKey)
	{
		g_Ui.GetIni(true)->WriteInteger(m_szName, pszKey, nValue);
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

void KUiWndFrameDragDrop::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd || !pIni || !pszName)
		return;
    strcpy(m_szName, pszName);
	int		nValue;

    pIni->EraseKey(pszName, "AnimateStartPosX");
    pIni->EraseKey(pszName, "AnimateStartPosY");
    pIni->EraseKey(pszName, "AnimateEndPosX");
    pIni->EraseKey(pszName, "AnimateEndPosY");
    pIni->EraseKey(pszName, "AnimateMoveSpeed");
    pIni->EraseKey(pszName, "AnimateTimeSpace");
    pIni->EraseKey(pszName, "ShowAnimateWhenCreate");  

	pIni->GetInteger(pszName, "FollowMouse", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_FRAME_DRAGDROP_FOLLOWMOUSE, nValue ? BST_CHECKED : 0);
    pIni->GetInteger(pszName, "FollowMouseOffsetX", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_FRAME_DRAGDROP_OFFSET_X, nValue, true);
    pIni->GetInteger(pszName, "FollowMouseOffsetY", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_FRAME_DRAGDROP_OFFSET_Y, nValue, true);

}
void KUiWndFrameDragDrop::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	} 
}


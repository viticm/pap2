#include "Stdafx.h"
#include "UiWndFrameCommon.h"
#include "UiEditor.h"
#include "resource.h"
#include "Ui.h"
#include "ClientManage.h"


KUiWndFrameCommon::KUiWndFrameCommon()
{
    m_szName[0]     = 0;
}

KUiWndFrameCommon::~KUiWndFrameCommon()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndFrameCommon::Create()
{
	KUiWndFrameCommon* pWnd = new KUiWndFrameCommon;
	if (pWnd == NULL)
		return NULL;


	return pWnd;
}

int KUiWndFrameCommon::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL) {
		return FALSE;
	}
    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndFrameCommon), hWnd, (DLGPROC)WndProc);
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

    HWND hSpin = ::GetDlgItem(m_hWnd, IDC_WND_FRAME_DRAG_AREA_TOP_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
    }

    hSpin = ::GetDlgItem(m_hWnd, IDC_WND_FRAME_DRAG_AREA_LEFT_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
    }

    hSpin = ::GetDlgItem(m_hWnd, IDC_WND_FRAME_DRAG_AREA_WIDTH_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
    }

    hSpin = ::GetDlgItem(m_hWnd, IDC_WND_FRAME_DRAG_AREA_HIGHT_SPIN);
    if (hSpin) 
    {
        SendMessage(hSpin, UDM_SETRANGE, 0, MAKELPARAM(10000, 0));
    }

	return true;
}


void KUiWndFrameCommon::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndFrameCommon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiWndFrameCommon*	pView = (KUiWndFrameCommon*)GetWindowLong(hWnd, GWL_USERDATA);
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

void KUiWndFrameCommon::OnButtonClick(int nBtnId)
{
	int	nValue = 0;
	const char* pszKey = NULL;
	switch(nBtnId)
	{
	case IDC_WND_FRAME_ENABLEMOVE:
		pszKey = "IsCustomDragable";
		nValue = (::IsDlgButtonChecked(m_hWnd, IDC_WND_FRAME_ENABLEMOVE) == BST_CHECKED);
		break;
	case IDC_WND_FRAME_SHOW_WHEN_UI_HIDE:
		pszKey = "ShowWhenHideUI";
		nValue = (::IsDlgButtonChecked(m_hWnd, IDC_WND_FRAME_SHOW_WHEN_UI_HIDE) == BST_CHECKED);
		break;
    }
	if (pszKey)
	{
		g_Ui.GetIni(true)->WriteInteger(m_szName, pszKey, nValue);
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}
}

void KUiWndFrameCommon::OnUpdateEditText(int nId)
{
    int nValue = 0;
    int nPos = 0;
	LPCTSTR pszKey = NULL;
    switch(nId) 
    {
	case IDC_WND_FRAME_DRAG_AREA_LEFT:
		pszKey = "DragAreaLeft";
		nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FRAME_DRAG_AREA_LEFT, NULL, false);
		break;
    case IDC_WND_FRAME_DRAG_AREA_TOP:
        pszKey = "DragAreaTop";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FRAME_DRAG_AREA_TOP, NULL, false);
    	break;
    case IDC_WND_FRAME_DRAG_AREA_HEIGHT:
        pszKey = "DragAreaHeight";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FRAME_DRAG_AREA_HEIGHT, NULL, false);
    	break;
    case IDC_WND_FRAME_DRAG_AREA_WIDTH:
        pszKey = "DragAreaWidth";
        nValue = ::GetDlgItemInt(m_hWnd, IDC_WND_FRAME_DRAG_AREA_WIDTH, NULL, false);
    	break;
    default:
        break;
    }
	if (pszKey)
	{
		g_Ui.GetIni(true)->WriteInteger(m_szName, pszKey, nValue);
		g_ClientManager.UpDataWndInClient(g_Ui.GetIni(true));
        ShowErrorInformation();
	}

}


void KUiWndFrameCommon::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd || !pIni || !pszName )
		return;
    strcpy(m_szName, pszName);
	int		nValue;
    int     nX;
    int     nY;
    char    szKey[128] = "";
    char    szValue[128] = "";
    int     i = 0;

	pIni->GetInteger(pszName, "IsCustomDragable", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_FRAME_ENABLEMOVE, nValue ? BST_CHECKED : 0);

    pIni->GetInteger(pszName, "DragAreaLeft", 0, &nX);
    ::SetDlgItemInt(m_hWnd, IDC_WND_FRAME_DRAG_AREA_LEFT, nX, false);
    pIni->GetInteger(pszName, "DragAreaTop", 0, &nY);
    ::SetDlgItemInt(m_hWnd, IDC_WND_FRAME_DRAG_AREA_TOP, nY, false);
    pIni->GetInteger(pszName, "DragAreaWidth", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_FRAME_DRAG_AREA_WIDTH, nValue, false);
    pIni->GetInteger(pszName, "DragAreaHeight", 0, &nValue);
    ::SetDlgItemInt(m_hWnd, IDC_WND_FRAME_DRAG_AREA_HEIGHT, nValue, false);

	pIni->GetInteger(pszName, "ShowWhenHideUI", 0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_FRAME_SHOW_WHEN_UI_HIDE, nValue ? BST_CHECKED : 0);

}
void KUiWndFrameCommon::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	} 
}


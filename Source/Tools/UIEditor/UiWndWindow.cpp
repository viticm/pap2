#include "Stdafx.h"
#include "UiWndWindow.h"
#include "resource.h"
#include "UiEditor.h"
#include "ClientManage.h"
#include "Ui.h"

KUiWndWindow::KUiWndWindow()
{
	m_szName[0]      = 0;
    m_hWnd           = NULL;
}

KUiWndWindow::~KUiWndWindow()
{
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

KUiWndWindow* KUiWndWindow::Create()
{
	KUiWndWindow* pWnd = new KUiWndWindow;
	if (pWnd == NULL)
		return NULL;
	return pWnd;
}

int KUiWndWindow::Init(HWND hWnd, RECT* pRc, const char *pszName)
{
	if (hWnd == NULL) {
		return FALSE;
	}
    strcpy(m_szName, pszName);
	m_hWnd = ::CreateDialog((HINSTANCE)g_hPluginInstance, MAKEINTRESOURCE(IDD_WndWindow), hWnd, (DLGPROC)WndProc);
	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd, HWND_TOP, pRc->left, pRc->top,
			pRc->right - pRc->left, pRc->bottom - pRc->top,
			SWP_SHOWWINDOW);
		::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
	}
	else
	{
		return FALSE;
	}	
	return TRUE;
}

void KUiWndWindow::Release()
{
	delete this;
}

LRESULT CALLBACK KUiWndWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KUiWndWindow*	pView = (KUiWndWindow*)GetWindowLong(hWnd, GWL_USERDATA);
	if (!pView)
		return 0;
	if (message == WM_COMMAND)
	{
		int nNotify = HIWORD(wParam);
		switch(nNotify)
		{
		case EN_CHANGE:
			break;
		case 0:
			break;
		}
	}
	return 0;
}

void KUiWndWindow::UpdateData(const char* pszName, KUiWndWindowData& data, IIniFile* pIni)
{
	if (!m_hWnd)
		return;
	strcpy(m_szName, pszName);
	::SetDlgItemInt(m_hWnd, IDC_WND_POS_LEFT, data.Left, true);
	::SetDlgItemInt(m_hWnd, IDC_WND_POS_TOP, data.Top, true);
	::SetDlgItemInt(m_hWnd, IDC_WND_SIZE_WIDTH, data.Width, false);
	::SetDlgItemInt(m_hWnd, IDC_WND_SIZE_HEIGHT, data.Height, false);
	int	nValue;
	pIni->GetInteger(pszName, "Moveable",  0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_MOVEABLE, nValue ? BST_CHECKED : 0);
	pIni->GetInteger(pszName, "DummyWnd",  0, &nValue);
	::CheckDlgButton(m_hWnd, IDC_WND_AUTOSIZE, nValue ? BST_CHECKED : 0);
}

void KUiWndWindow::Show(int bShow, int nSubWnd)
{
	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	}
}


HWND KUiWndWindow::GetHWND()
{
	return m_hWnd;
}

void KUiWndWindow::ShowErrorInformation()
{
    HWND hParent = ::GetParent(m_hWnd);
    if (hParent) 
    {
        if (KUiEditor::ms_ErrorInfomation.bHasError) 
        {
            ::SetDlgItemText(hParent, IDC_WND_ERROR, KUiEditor::ms_ErrorInfomation.szErrorMsg);
        }
        else
        {
            ::SetDlgItemText(hParent, IDC_WND_ERROR, "Ok! No Error!");
        }
    }
}
